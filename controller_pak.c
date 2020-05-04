#include  <nusys.h>
#include "main.h"
#include "controller_pak.h"
#include "2dlibrary.h"
#include "font.h"
#include "font_tahoma.h"
#include "user_interface.h"

extern objListCnt;
extern FontConfig font_config;
extern Gfx*	gfxListPtr;
extern Gfx*	gfxListStartPtr;

Texture	controllers_texture;

NUContData			contData[NU_CONT_MAXCONTROLLERS];
NUContPakFile		pakFile[NU_CONT_MAXCONTROLLERS];
u8 					contPattern;
int 				debug[4];
s32					file_num[NU_CONT_MAXCONTROLLERS], file_num_max[NU_CONT_MAXCONTROLLERS], free_blocks[NU_CONT_MAXCONTROLLERS], fetch_file;
OSPfsState			pfs_stat[NU_CONT_MAXCONTROLLERS][16];
s32					pfs_stat_ret[NU_CONT_MAXCONTROLLERS][16];
char				tmp_char[100];
char*				char_ptr;
SelectPakFile		selected_pak_file;
ControllerManager	controller_manager;
MenuManager			menu[4];
int					menu_focus;

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
	font_init();
	font_tahomaInit();
	my2D_loadTexture(&controllers_texture, (u32)_controllersSegmentRomStart, (u32)_controllersSegmentRomEnd, 256, 128);
	/* Initialize the controller */
	contPattern = nuContInit();
	nuContGBPakMgrInit();
	nuVrsMgrInit();
	readControllerPaks();
	controller_manager.last_button = -1;
	controller_manager.last_time = 0;
	menu_focus = -1;
	menu[0].visible	= 0;
	menu[1].visible	= 0;
	menu[2].visible	= 0;
	menu[3].visible	= 0;
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
	int			controller_sprite_position_x, controller_sprite_min_x, controller_sprite_max_x, i;
	Coordinates tmp_coordinates;
	int fetch_controller;
	int controller_pak_plugged[4];
	int controller_pak_count = 0;
	
	//my2D_drawDialogBox(8, 8, my2dlibrary.width-9, my2dlibrary.height-9);
	my2D_drawDialogBox(MY2D_CENTER, MY2D_CENTER, my2dlibrary.width-16, my2dlibrary.height-16, "Controller Pak Manager UI for Nintendo 64");
	
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
			controller_pak_plugged[fetch_controller] = 0;
			tmp_coordinates = (Coordinates){192, 64, &controllers_texture, 64, 64};
			my2D_drawBackGroundCoordinates(&tmp_coordinates,  controller_sprite_position_x , 40, 0);
		}		
		font_config.weight = FONT_REGULAR;
		font_config.color = FONT_BLACK;
		font_config.halign = FONT_LEFT;
		// check if there is a controller plugged
		if (pakFile[fetch_controller].type == NU_CONT_PAK_TYPE_PAK) {
			controller_pak_count++;
			controller_pak_plugged[fetch_controller] = 1;
			// draw file list
			my2D_drawSelectList(controller_sprite_min_x+5, 120, controller_sprite_max_x-5, 334);
			if(pakFile[fetch_controller].error) {
				font_write(30+fetch_controller*200, 150, 600, "corrupted");
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
					convertPakfilenameToChar(pfs_stat[fetch_controller][fetch_file].game_name, tmp_char);
					if (pfs_stat[fetch_controller][fetch_file].ext_name[0]) {
						strAppendChar(tmp_char, '.');
						strAppendChar(tmp_char, convertPakcharToChar(pfs_stat[fetch_controller][fetch_file].ext_name[0]));
					}
					font_config.halign = FONT_LEFT;
					font_write(controller_sprite_min_x+10, 125+fetch_file*13, controller_sprite_max_x - 10, tmp_char);
					sprintf(tmp_char, "%d", (u16)((pfs_stat[fetch_controller][fetch_file].file_size+255) >> 8));
					font_config.halign = FONT_RIGHT;
					font_write(controller_sprite_max_x - 50, 125+fetch_file*13, controller_sprite_max_x - 10, tmp_char);
				}
				else {
					font_config.halign = FONT_CENTER;
					font_write(controller_sprite_min_x+10, 125+fetch_file*13, controller_sprite_max_x - 10, "- no save -");
				}
			}
			//tmp_char = fontSet("free: %d");
			sprintf(tmp_char, "free: %d", (u16)free_blocks[fetch_controller]);
			font_config.halign = FONT_RIGHT;
			font_config.color = FONT_BLACK;
			font_write(controller_sprite_min_x+10, 345, controller_sprite_max_x - 10, tmp_char);
		}
		else {
			font_config.halign = FONT_CENTER;
			font_write(controller_sprite_min_x+10, 125, controller_sprite_max_x - 10, "- no controller pak -");
		}
	}
	font_config.halign = FONT_LEFT;
	font_write(20+10, 400, 620, "Explanations:\nMaximum of 16 files per memory card.\n1 block = 256 bytes\nUse D-Pad to navigate.");
	font_write(350, 413, 620, "Press A to validate selection.\nPress B to cancel.\nPress Z to repair the pak");
	
	convertPakfilenameToChar(pfs_stat[selected_pak_file.pak_number][selected_pak_file.file_number].game_name, tmp_char);
	if (pfs_stat[selected_pak_file.pak_number][selected_pak_file.file_number].ext_name[0]) {
		strAppendChar(tmp_char, '.');
		strAppendChar(tmp_char, convertPakcharToChar(pfs_stat[selected_pak_file.pak_number][selected_pak_file.file_number].ext_name[0]));
	}
	if (menu[0].visible) {
		my2D_drawDialogBox(MY2D_CENTER, MY2D_CENTER, 300, 200, tmp_char);
		my2D_drawButtonText(MY2D_CENTER, 160, 100, 20, 0 == menu[0].selected, "Delete file");
		my2D_drawButtonText(MY2D_CENTER, 190, 100, 20, 1 == menu[0].selected, "Copy file");
		my2D_drawButtonText(MY2D_CENTER, 220, 100, 20, 2 == menu[0].selected, "Cancel");
		
		tmp_coordinates = (Coordinates){64*selected_pak_file.pak_number, 0, &controllers_texture, 64, 64};
		my2D_drawBackGroundCoordinates(&tmp_coordinates, 288, 240, 0);
	}
	
	if (menu[1].visible) {
		my2D_drawDialogBox(MY2D_CENTER, MY2D_CENTER, 400, 100, tmp_char);
		font_write(140, 220, 510, "Are you sure you want to delete this file ?");
		my2D_drawButtonText(210, 240, 100, 20, 0 == menu[1].selected, "delete");
		my2D_drawButtonText(330, 240, 100, 20, 1 == menu[1].selected, "NO");
	}
	if (menu[2].visible) {
		my2D_drawDialogBox(MY2D_CENTER, MY2D_CENTER, 400, 140, tmp_char);
		font_write(140, 200, 510, "Where do you want to copy this file to ?");
		tmp_coordinates = (Coordinates){0, 0, &controllers_texture, 64, 64};
		controller_sprite_position_x = (my2dlibrary.width / 2) -  (controller_pak_count * 40);
		
		for (i = 0; i < NU_CONT_MAXCONTROLLERS; i++) {
			if (controller_pak_plugged[i] == 1) {
				if (i != selected_pak_file.pak_number)
					tmp_coordinates = (Coordinates){64*i, 0, &controllers_texture, 64, 64};
				else
					tmp_coordinates = (Coordinates){192, 64, &controllers_texture, 64, 64};
				my2D_drawButtonIcon(controller_sprite_position_x, 220, i == menu[2].selected, &tmp_coordinates);
				controller_sprite_position_x += 80;
			}
		}
	}
	if (menu[3].visible) {
		my2D_drawDialogBox(MY2D_CENTER, MY2D_CENTER, 300, 100, tmp_char);
		if (pakFile[selected_pak_file.pak_number].error)
			font_write(180, 240, 460, "Ouch, an error happened.");
		else
			font_write(180, 240, 460, "File successfully deleted.\n Press A, B or Start to reload.");
	}
	
	nuContDataGetExAll(contData);
	if (controller_pak_count) { // move selection only if there is at least one controller pak plugged
		switch (menu_focus) {
			case -1: // no dialog box open
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
				if (contData[0].trigger & A_BUTTON && !pfs_stat_ret[selected_pak_file.pak_number][selected_pak_file.file_number]) {
					menu_focus = 0;
					menu[0].visible = 1;
					menu[0].selected = 0;
					menu[0].file.pak_number = selected_pak_file.pak_number;
					menu[0].file.file_number = selected_pak_file.file_number;
				}
				break;
			case 0:  // choose action menu
				if (contData[0].trigger & A_BUTTON)
					switch(menu[0].selected) {
						case 0: 
							menu[1].visible = 1; 
							menu[1].selected = 1; 
							menu_focus = 1;
							break;
						case 1: 
							menu_focus = 2;
							menu[2].visible = 1; 
							menu[2].selected = selected_pak_file.pak_number; 
							break;
						case 2: 
							menu[0].visible = 0; 
							menu_focus = -1;
							break;
					}
				if (contData[0].trigger & B_BUTTON) {
					menu_focus = -1;
					menu[0].visible = 0;
				}
				if (controllerTiming(U_JPAD))
					menu[0].selected--;
				if (controllerTiming(D_JPAD))
					menu[0].selected++;
				if (menu[0].selected < 0)
					menu[0].selected = 2;
				if (menu[0].selected > 2)
					menu[0].selected = 0;
				break;
			case 1: // delete file menu
				if (contData[0].trigger & A_BUTTON)
					switch(menu[1].selected) {
						case 0: // do the deletion !
							menu[0].visible = 0;
							menu[1].visible = 0;
							menu[3].visible = 1;
							menu_focus = 3;
							nuContPakCodeSet(
								(u8*)&pfs_stat[selected_pak_file.pak_number][selected_pak_file.file_number].company_code, 
								(u8*)&pfs_stat[selected_pak_file.pak_number][selected_pak_file.file_number].game_code
							);
							nuContPakFileDelete(
								&pakFile[selected_pak_file.pak_number], 
								pfs_stat[selected_pak_file.pak_number][selected_pak_file.file_number].game_name, 
								pfs_stat[selected_pak_file.pak_number][selected_pak_file.file_number].ext_name
							);
							break;
						case 1:  // cancel deletion
							menu[1].visible = 0; 
							menu_focus = 0;
							break;
					}		
				if (contData[0].trigger & B_BUTTON)
					menu[1].visible = 0;
				if (controllerTiming(L_JPAD))
					menu[1].selected--;
				if (controllerTiming(R_JPAD))
					menu[1].selected++;
				if (menu[1].selected < 0)
					menu[1].selected = 1;
				if (menu[1].selected > 1)
					menu[1].selected = 0;
				break;
			case 2: // copy file menu
				if (contData[0].trigger & B_BUTTON) {
					menu[2].visible = 0;
					menu_focus = 0;
				}
				if (contData[0].trigger & A_BUTTON) {
					if (menu[2].selected == selected_pak_file.pak_number) { // can not copy to itself
						menu[2].visible = 0;
						menu_focus = 0;
					} else { // do copy the file
					}
				}
				if (controllerTiming(L_JPAD)) 
					do {
						menu[2].selected--;
						if (menu[2].selected < 0)
							menu[2].selected = NU_CONT_MAXCONTROLLERS - 1;
					} while(!controller_pak_plugged[menu[2].selected]);
				if (controllerTiming(R_JPAD))
					do {
						menu[2].selected++;
						if (menu[2].selected >= NU_CONT_MAXCONTROLLERS)
							menu[2].selected = 0;
					} while(!controller_pak_plugged[menu[2].selected]);
				break;
			case 3: // dialog delete confirmation
				if (contData[0].trigger & A_BUTTON || contData[0].trigger & B_BUTTON || contData[0].trigger & START_BUTTON) {
					menu[3].visible = 0;
					menu_focus = -1;
					readControllerPaks();
				}
				break;
		}
	}
	// show debug console only if both Z+L+R triggers are pressed
	if ((contData[0].trigger & R_TRIG && contData[0].button & Z_TRIG && contData[0].button & L_TRIG) ||
		(contData[0].trigger & L_TRIG && contData[0].button & Z_TRIG && contData[0].button & R_TRIG) ||
		(contData[0].trigger & Z_TRIG && contData[0].button & R_TRIG && contData[0].button & L_TRIG))
		show_console ^= 1;
	
	if (show_console) {
		nuDebConClear(0);	
		nuDebConTextAttr(0, NU_DEB_CON_ATTR_BLINK);
		nuDebConTextColor(0, NU_DEB_CON_TEXT_RED);
		nuDebConTextPos(0, 17,21);
		nuDebConPuts(0, "Running ");
		nuDebConTextAttr(0, NU_DEB_CON_ATTR_NORMAL);
		nuDebConTextColor(0, NU_DEB_CON_TEXT_YELLOW);

		sprintf(conbuf, "Pak id  : %d  ", selected_pak_file.pak_number);
		nuDebConTextPos(0,1,22);
		nuDebConCPuts(0, conbuf);
		sprintf(conbuf, "File id : %d  ", selected_pak_file.file_number);
		nuDebConTextPos(0,1,23);
		nuDebConCPuts(0, conbuf);
		sprintf(conbuf, "FPS     : %d  ", (int)(1 / ((float)(OS_CYCLES_TO_USEC(osGetTime()-time_lastframe)/1000)/1000)));
		time_lastframe = osGetTime();
		nuDebConTextPos(0,1,24);
		nuDebConCPuts(0, conbuf);
		
		sprintf(conbuf, "File size (KB): %d   ", pfs_stat[selected_pak_file.pak_number][selected_pak_file.file_number].file_size);
		nuDebConTextPos(0,15,22);
		nuDebConCPuts(0, conbuf);
		sprintf(conbuf, "BG Count      : %d   ", my2dlibrary.background_count);
		nuDebConTextPos(0,15,23);
		nuDebConCPuts(0, conbuf);
		sprintf(conbuf, "Display list  : %d   ", (gfxListPtr - gfxListStartPtr) / sizeof(Gfx));
		nuDebConTextPos(0,15,24);
		nuDebConCPuts(0, conbuf);
		
		
	}
}
