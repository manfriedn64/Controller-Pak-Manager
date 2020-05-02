#include  <nusys.h>
#include "main.h"
#include "controller_pak.h"
#include "2dlibrary.h"
#include "font_tahoma.h"

extern objListCnt;
extern FontTahoma font_config;
Texture	controllers_texture;

NUContData			contData[NU_CONT_MAXCONTROLLERS];
NUContPakFile		pakFile[NU_CONT_MAXCONTROLLERS];
u8 					contPattern;
int 				debug[4];
s32					file_num[NU_CONT_MAXCONTROLLERS], file_num_max[NU_CONT_MAXCONTROLLERS], free_blocks[NU_CONT_MAXCONTROLLERS], fetch_file;
OSPfsState			pfs_stat[NU_CONT_MAXCONTROLLERS][16];
s32					pfs_stat_ret[NU_CONT_MAXCONTROLLERS][16];
char*				tmp_char;
SelectPakFile		selected_pak_file;
ControllerManager	controller_manager;

void readControllerPaks() {
	int i;
	s32 ret;
	u8 status;
	
	nuContQueryRead();
	// if at least one controller is plugged
	if (nuContNum > 0) {
		nuContDataReadStart();
		nuContDataReadWait();
		for (i = 0; i < NU_CONT_MAXCONTROLLERS; i++) 
		{
			debug[i] = 1;
			file_num_max[i] = 1;
			file_num[i] = 1;
			// if controller #i is plugged
			if (nuContStatus[i].status & CONT_CARD_ON) {
				debug[i] = 2;
				nuContDataGetEx(&contData[i], i);
				nuContPakOpen(&pakFile[i], i);
				// check if the pak is a memory card
				if (pakFile[i].type == NU_CONT_PAK_TYPE_PAK) {
					// how many files can and do we have here ?
					nuContPakFileNum(&pakFile[i], &file_num_max[i], &file_num[i]);
					if(pakFile[i].error) {
						continue;
					}
					// check each file contained in the controller pak
					for (fetch_file = 0; fetch_file < file_num_max[i]; fetch_file++) 
					{
						pakFile[i].file_no = fetch_file;
						pfs_stat_ret[i][fetch_file] = nuContPakFileState(&pakFile[i], pfs_stat[i]+fetch_file);
						if (!pfs_stat_ret[i][fetch_file]) {
							if (selected_pak_file.pak_number == -1)
								selected_pak_file.pak_number = i;
							if (selected_pak_file.file_number == -1)
								selected_pak_file.file_number = fetch_file;
						}
					}
					// get how many blocks remains free
					free_blocks[i] =  nuContPakGetFree(&pakFile[i]);
					if(pakFile[i].error) {
						continue;
					}
					free_blocks[i] >>= 8;
					debug[i] = 3;
				}
				else {
					// check if the pak is a rumble one
					ret = nuContRmbCheck(i);
					if (!ret) {
						pakFile[i].type = NU_CONT_PAK_TYPE_RUMBLE;
						debug[i] = 4;
					}
					else if (ret == PFS_ERR_DEVICE) {
						// come on, what is it, a transfer pak ?
						ret = nuContGBPakOpen(&pakFile[i], i);
						if (!ret) {
							pakFile[i].type = NU_CONT_PAK_TYPE_GBPAK;
							debug[i] = 5;
						}
						else // ok, I give up, no idea what this is, probably a failed device or bio sensor ?
							debug[i] = 7;
					}
					
				}
			}
		}
	}
}

void controllerPakInit() {
	selected_pak_file.pak_number = -1;
	selected_pak_file.file_number = -1;
	my2D_init(640, 480, (u32*)(0x80400000 - 640*480*2*2));
	initTahoma();
	my2D_loadTexture(&controllers_texture, (u32)_controllersSegmentRomStart, (u32)_controllersSegmentRomEnd, 256, 128);
	/* Initialize the controller */
	contPattern = nuContInit();
	nuContGBPakMgrInit();
	nuVrsMgrInit();
	readControllerPaks();
	controller_manager.last_button = -1;
	controller_manager.last_time = 0;
}

Bool controllerTiming(int button) {
	if (contData[0].button & button)
		if (controller_manager.last_button != button || (controller_manager.last_button == button && OS_CYCLES_TO_USEC(osGetTime() - controller_manager.last_time) / 50000 > 1)) {
			controller_manager.last_button = button;
			controller_manager.last_time = osGetTime();
			return True;
		}
	return False;
}

char convertPakcharToChar(int i) {
	int converted = 0;
	if (i >= 0x10 && i <= 0x19) { // 0-9
		converted = i + 0x20;
	}
	else if (i >= 0x1a && i < 0x34) { // A-Z
		converted = i + 0x27;
	}
	else switch(i) {  //   !# '*+,-./:=?@
		case 0x00: converted =  0; break; //  nice, the real ending character !
		case 0x0f: converted = 32; break; //  space
		case 0x34: converted = 33; break; //  !
		case 0x35: converted = 34; break; //  "
		case 0x36: converted = 35; break; //  #
		case 0x37: converted = 39; break; //  '
		case 0x38: converted = 42; break; //  *
		case 0x39: converted = 43; break; //  +
		case 0x3a: converted = 44; break; //  ,
		case 0x3b: converted = 45; break; //  -
		case 0x3c: converted = 46; break; //  .
		case 0x3d: converted = 47; break; //  /
		case 0x3e: converted = 58; break; //  :
		case 0x3f: converted = 61; break; //  =
		case 0x40: converted = 63; break; //  ?
		case 0x41: converted = 64; break; //  @
		default:   converted = 37; break;   //  % // unknown char
	}
	return (char)converted;
}

void convertPakfilenameToChar(char* filename, char* converted) {
	int i;
	int text_lenght = strlen(filename);
	if (text_lenght <= sizeof(converted))
		return;
	for (i = 0; i < text_lenght; i++) 
		converted[i] = convertPakcharToChar(filename[i]);
	converted[text_lenght] = 0;
}

void strAppendChar(char* string, int c) {
	int str_length = strlen(string);
	string[str_length] = c;
	string[str_length+1] = 0;
}

void drawTitle() {
	int			controller_sprite_position_x, controller_sprite_min_x, controller_sprite_max_x;
	Coordinates tmp_coordinates;
	int fetch_controller;
	short is_there_a_controller_pak = 0;
	
	my2D_drawDialogBox(8, 8, my2dlibrary.width-9, my2dlibrary.height-9);
	font_config.weight = FONT_BOLD;
	font_config.color = FONT_WHITE;
	font_config.halign = FONT_CENTER;
	writeTahoma(14, 12, 626, "Controller Pak Manager UI for Nintendo 64");
	
	for (fetch_controller = 0; fetch_controller < NU_CONT_MAXCONTROLLERS; fetch_controller++) {
		controller_sprite_min_x = 10 + ((my2dlibrary.width-20) / 4) * fetch_controller;
		controller_sprite_max_x = 10 + ((my2dlibrary.width-20) / 4) * (fetch_controller+1);
		// draw controller and its pak
		controller_sprite_position_x = (controller_sprite_min_x + controller_sprite_max_x) / 2 - 32; // center controller sprite within its area
		if (nuContStatus[fetch_controller].type & CONT_TYPE_NORMAL) {
			tmp_coordinates = (Coordinates){64*fetch_controller, 0, &controllers_texture, 64, 64};
			if (pakFile[fetch_controller].type == NU_CONT_PAK_TYPE_NONE) 
				my2D_drawBackGroundCoordinates(&tmp_coordinates,  controller_sprite_position_x , 40, 0);
			else {
				controller_sprite_position_x = (controller_sprite_min_x + controller_sprite_max_x) / 2 - 64; // center controller + pak sprites within their area
				my2D_drawBackGroundCoordinates(&tmp_coordinates,  controller_sprite_position_x , 40, 0);
				switch (pakFile[fetch_controller].type) {
					case NU_CONT_PAK_TYPE_PAK: tmp_coordinates = (Coordinates){0, 64, &controllers_texture, 64, 64}; break;
					case NU_CONT_PAK_TYPE_RUMBLE: tmp_coordinates = (Coordinates){64, 64, &controllers_texture, 64, 64}; break;
					case NU_CONT_PAK_TYPE_GBPAK: tmp_coordinates = (Coordinates){128, 64, &controllers_texture, 64, 64}; break;
					case 4: tmp_coordinates = (Coordinates){192, 64, &controllers_texture, 64, 64}; break;
				}
				my2D_drawBackGroundCoordinates(&tmp_coordinates, controller_sprite_position_x + 64, 40, 0);
			}
		}
		else {
			tmp_coordinates = (Coordinates){192, 64, &controllers_texture, 64, 64};
			my2D_drawBackGroundCoordinates(&tmp_coordinates,  controller_sprite_position_x , 40, 0);
		}		
		font_config.weight = FONT_REGULAR;
		font_config.color = FONT_BLACK;
		font_config.halign = FONT_LEFT;
		// check if there is a controller plugged
		if (pakFile[fetch_controller].type == NU_CONT_PAK_TYPE_PAK) {
			is_there_a_controller_pak++;
			// draw file list
			my2D_drawSelectList(controller_sprite_min_x+5, 120, controller_sprite_max_x-5, 334);
			if(pakFile[fetch_controller].error) {
				writeTahoma(30+fetch_controller*200, 150, 600, "corrupted");
				continue;
			}
			for (fetch_file = 0; fetch_file < file_num_max[fetch_controller]; fetch_file++) {
				if (selected_pak_file.pak_number == fetch_controller && selected_pak_file.file_number == fetch_file) {
					font_config.color = FONT_WHITE;
					my2D_drawRectangle(controller_sprite_min_x+7, 124+fetch_file*13, controller_sprite_max_x-7, 136+fetch_file*13, 0, 0, 127, 1);
				}
				else 
					font_config.color = FONT_BLACK;
				if (!pfs_stat_ret[fetch_controller][fetch_file]) {
					convertPakfilenameToChar(pfs_stat[fetch_controller][fetch_file].game_name, conbuf);
					if (pfs_stat[fetch_controller][fetch_file].ext_name[0]) {
						strAppendChar(conbuf, '.');
						strAppendChar(conbuf, convertPakcharToChar(pfs_stat[fetch_controller][fetch_file].ext_name[0]));
					}
					font_config.halign = FONT_LEFT;
					writeTahoma(controller_sprite_min_x+10, 125+fetch_file*13, controller_sprite_max_x - 10, conbuf);
					sprintf(conbuf, "%d", (u16)((pfs_stat[fetch_controller][fetch_file].file_size+255) >> 8));
					font_config.halign = FONT_RIGHT;
					writeTahoma(controller_sprite_max_x - 50, 125+fetch_file*13, controller_sprite_max_x - 10, conbuf);
				}
				else {
					font_config.halign = FONT_CENTER;
					writeTahoma(controller_sprite_min_x+10, 125+fetch_file*13, controller_sprite_max_x - 10, "- no save -");
				}
			}
			tmp_char = fontSet("free: %d");
			sprintf(tmp_char, tmp_char, (u16)free_blocks[fetch_controller]);
			font_config.halign = FONT_RIGHT;
			font_config.color = FONT_BLACK;
			writeTahoma(controller_sprite_min_x+10, 345, controller_sprite_max_x - 10, tmp_char);
		}
		else {
			font_config.halign = FONT_CENTER;
			writeTahoma(controller_sprite_min_x+10, 125, controller_sprite_max_x - 10, "- no controller pak -");
		}
	}
	
	nuContDataGetExAll(contData);
	if (is_there_a_controller_pak) { // move selection only if there is at least one controller pak plugged
		if (controllerTiming(R_JPAD)) {
			do {
				selected_pak_file.pak_number++;
				if (selected_pak_file.pak_number > NU_CONT_MAXCONTROLLERS-1)
					selected_pak_file.pak_number = 0;
			} while (pakFile[selected_pak_file.pak_number].type != NU_CONT_PAK_TYPE_PAK);
			if (selected_pak_file.file_number > file_num_max[selected_pak_file.pak_number] - 1)
				selected_pak_file.file_number = file_num_max[selected_pak_file.pak_number]-1;
		}
		if (controllerTiming(L_JPAD)) {
			do {
				selected_pak_file.pak_number--;
				if (selected_pak_file.pak_number < 0)
					selected_pak_file.pak_number = NU_CONT_MAXCONTROLLERS-1;
			} while (pakFile[selected_pak_file.pak_number].type != NU_CONT_PAK_TYPE_PAK);		
			if (selected_pak_file.file_number > file_num_max[selected_pak_file.pak_number] - 1)
				selected_pak_file.file_number = file_num_max[selected_pak_file.pak_number]-1;
		}
		if (controllerTiming(U_JPAD)) {
			selected_pak_file.file_number--;
			if (selected_pak_file.file_number < 0)
				selected_pak_file.file_number = file_num_max[selected_pak_file.pak_number] - 1;
		}
		if (controllerTiming(D_JPAD)) {
			selected_pak_file.file_number++;
			if (selected_pak_file.file_number > file_num_max[selected_pak_file.pak_number] - 1)
				selected_pak_file.file_number = 0;
		}
	}
	/*
	if (contData[0].trigger & R_JPAD) {
		do {
			selected_pak_file.file_number++;
			if (selected_pak_file.pak_number > )
				selected_pak_file.pak_number = NU_CONT_MAXCONTROLLERS-1;
		} while (!pfs_stat_ret[fetch_controller][fetch_file]);		
	}
	if (contData[0].trigger & L_JPAD)*/
	

	
	nuDebConClear(0);	
	nuDebConTextAttr(0, NU_DEB_CON_ATTR_BLINK);
	nuDebConTextColor(0, NU_DEB_CON_TEXT_RED);
	nuDebConTextPos(0, 17,21);
	nuDebConPuts(0, "Running ");
	nuDebConTextAttr(0, NU_DEB_CON_ATTR_NORMAL);
	nuDebConTextColor(0, NU_DEB_CON_TEXT_BLACK);
	
	sprintf(conbuf, " %d   ", selected_pak_file.pak_number);
	nuDebConTextPos(0,1,22);
	nuDebConCPuts(0, conbuf);
	sprintf(conbuf, " %d   ", selected_pak_file.file_number);
	nuDebConTextPos(0,1,23);
	nuDebConCPuts(0, conbuf);
	sprintf(conbuf, " %d   ", U_JPAD);
	nuDebConTextPos(0,1,24);
	nuDebConCPuts(0, conbuf);
	sprintf(conbuf, " fps = %d   ", (int)(1 / ((float)(OS_CYCLES_TO_USEC(osGetTime()-time_lastframe)/1000)/1000)));
	time_lastframe = osGetTime();
	nuDebConTextPos(0,1,25);
	nuDebConCPuts(0, conbuf);
}