#include  <nusys.h>
#include "font.h"
#include "font_tahoma.h"
#include "2dlibrary.h"

char font_buffer[100];
FontConfig	font_config;

char * fontSet(char* text) {
	memset(&font_buffer[0], 0, sizeof(font_buffer));
	strcpy(font_buffer, text);
}

void font_init() {
	font_config.weight			= FONT_REGULAR;
	font_config.color			= FONT_WHITE;
	font_config.halign			= FONT_LEFT;
	font_config.writeNext		= font_writeNext;
	font_config.writeNextLine	= font_writeNextLine;
	font_config.writePosition	= font_write;
}

int font_getNmbrLines(char* text) {
	int i;
	int text_lenght = strlen(text);
	int nbr_lines = 0;
	for (i = 0; i < text_lenght; i++) 
		if (text[i] == 5 || text[i] == '\n')
			nbr_lines++;
		else if (text[i] == 3 || text[i] == '\r' || text[i] == 0)
			break;
	return nbr_lines;
}

int font_getLineWidth(char* text, int line) {
	Coordinates coordinates;
	int i;
	int nbr_lines = 0;
	int width = 0;
	int text_lenght = strlen(text);
	for (i = 0; i < text_lenght; i++) {
		if (text[i] == 5 || text[i] == '\n')
			nbr_lines++;
		if (nbr_lines == line) {
			coordinates = font_config.getChar(text[i]);
			if (coordinates.width > 0)
				width += 1+coordinates.width;
			else
				if (text[i] == 5 || text[i] == '\n')
					continue;
				else if (text[i] == 3 || text[i] == '\r' || text[i] == 0)
					break;
				else
					width += 5;
		}
	}
	return width;
}

void font_writeNext(char* text) {
	font_write(font_config.latestX, font_config.latestY, font_config.latestMaxX, text);
}

void font_writeNextLine(char* text) {
	font_write(font_config.nextStartX, font_config.nextStartY, font_config.latestMaxX, text);
}

void font_write(int start_x, int start_y, int max_x, char* text) {
	Coordinates coordinates;
	int i, nbr_lines, x;
	int y = start_y;
	int current_line = 0;
	int text_lenght = strlen(text);
	nbr_lines = font_getNmbrLines(text);
	if (font_config.halign == FONT_RIGHT)
		x = max_x - font_getLineWidth(text, current_line);
	else if (font_config.halign == FONT_CENTER)
		x = (start_x + max_x) / 2 - font_getLineWidth(text, current_line) / 2;
	else
		x = start_x;
	for (i = 0; i < text_lenght; i++) {
		
		coordinates = font_config.getChar(text[i]);
		if (coordinates.width > 0) {
			if (x + coordinates.width > max_x) {
				x = start_x;
				y += coordinates.height + 1;
				text[i] = '\n';
			}
			my2D_drawBackGroundCoordinates(&coordinates, x, y, font_config.color);
			x += 1 + coordinates.width;
		}
		else 
			if (text[i] == 5 || text[i] == '\n') { // ascii = end of line
				current_line++;
				if (font_config.halign == FONT_RIGHT)
					x = max_x - font_getLineWidth(text, current_line);
				else if (font_config.halign == FONT_CENTER)
					x = (start_x + max_x) / 2 - font_getLineWidth(text, current_line) / 2;
				else
					x = start_x;
				y += coordinates.height +1;				
			}
			else if (text[i] == 3 || text[i] == '\r' || text[i] == 0) // ascii = end of text
				break;
			else 
				x += 5; // unknown char, add blank space
	}
	font_config.latestX = x;
	font_config.latestY = y;
	font_config.nextStartX = start_x;
	if (text[i] == '\n')
		font_config.nextStartY = y;
	else
		font_config.nextStartY = y + 1 + coordinates.height;
	font_config.latestMaxX = max_x;
}
