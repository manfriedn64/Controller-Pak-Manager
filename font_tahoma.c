#include  <nusys.h>
#include "main.h"
#include "font_tahoma.h"
#include "2dlibrary.h"

char font_buffer[100];
u32* tahoma_bold;
u32* tahoma_regular;
Texture tahoma_bold_texture;
Texture tahoma_regular_texture;
FontTahoma	font_config;

char * fontSet(char* text) {
	memset(&font_buffer[0], 0, sizeof(font_buffer));
	strcpy(font_buffer, text);
}

void initTahoma() {
	tahoma_bold_texture.romStart	= (u32)_tahoma_boldSegmentRomStart;
	tahoma_bold_texture.romEnd		= (u32)_tahoma_boldSegmentRomEnd;
	tahoma_bold_texture.pointer32	= my2D_loadSegmentToRam(tahoma_bold_texture.romStart, tahoma_bold_texture.romEnd);
	tahoma_bold_texture.pointer64	= (u64*)tahoma_bold_texture.pointer32;
	tahoma_bold_texture.width		= 560;
	tahoma_bold_texture.height		= 24;
	tahoma_bold_texture.malloc		= False;
	
	tahoma_regular_texture.romStart		= (u32)_tahoma_normalSegmentRomStart;
	tahoma_regular_texture.romEnd		= (u32)_tahoma_normalSegmentRomEnd;
	tahoma_regular_texture.pointer32	= my2D_loadSegmentToRam(tahoma_regular_texture.romStart, tahoma_regular_texture.romEnd);
	tahoma_regular_texture.pointer64	= (u64*)tahoma_regular_texture.pointer32;
	tahoma_regular_texture.width		= 488;
	tahoma_regular_texture.height		= 24;
	tahoma_regular_texture.malloc		= False;
}

int getNmbrLines(char* text) {
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

int getLineWidth(char* text, int line) {
	Coordinates coordinates;
	int i;
	int nbr_lines = 0;
	int width = 0;
	int text_lenght = strlen(text);
	for (i = 0; i < text_lenght; i++) {
		if (text[i] == 5 || text[i] == '\n')
			nbr_lines++;
		if (nbr_lines == line) {
			coordinates = getCharCoordinates(text[i]);
			if (coordinates.width > 0)
				width += 1+coordinates.width;
			else
				if (text[i] == 5 || text[i] == '\n')
					break;
				else if (text[i] == 3 || text[i] == '\r' || text[i] == 0)
					break;
				else
					width += 5;
		}
	}
	return width;
}

void writeTahoma(int start_x, int start_y, int max_x, char* text) {
	Coordinates coordinates;
	int i, nbr_lines, x;
	int y = start_y;
	int current_line = 0;
	int text_lenght = strlen(text);
	nbr_lines = getNmbrLines(text);
	if (font_config.halign == FONT_RIGHT)
		x = max_x - getLineWidth(text, current_line);
	else if (font_config.halign == FONT_CENTER)
		x = (start_x + max_x) / 2 - getLineWidth(text, current_line) / 2;
	else
		x = start_x;
	for (i = 0; i < text_lenght; i++) {
		
		coordinates = getCharCoordinates(text[i]);
		if (coordinates.width > 0) {
			if (x + coordinates.width > max_x) {
				x = start_x;
				y += coordinates.height + 1;
			}
			my2D_drawBackGroundCoordinates(&coordinates, x, y, font_config.color);
			x += 1 + coordinates.width;
		}
		else 
			if (text[i] == 5 || text[i] == '\n') { // ascii = end of line
				current_line++;
				if (font_config.halign == FONT_RIGHT)
					x = max_x - getLineWidth(text, current_line);
				else if (font_config.halign == FONT_CENTER)
					x = (start_x + max_x) / 2 - getLineWidth(text, current_line) / 2;
				else
					x = start_x;
				y += coordinates.height +1;				
			}
			else if (text[i] == 3 || text[i] == '\r' || text[i] == 0) // ascii = end of text
				break;
			else 
				x += 5; // unknown char, add blank space
	}
}

Coordinates getCharCoordinates(char character) {
	Texture* font_texture;
	if (font_config.weight == FONT_BOLD) {
		font_texture = &tahoma_bold_texture;
		switch (character) {
			case 'A': return (Coordinates){  0, 0, font_texture, 8, 12}; break;
			case 'B': return (Coordinates){  8, 0, font_texture, 8, 12}; break;
			case 'C': return (Coordinates){ 15, 0, font_texture, 7, 12}; break;
			case 'D': return (Coordinates){ 22, 0, font_texture, 8, 12}; break;
			case 'E': return (Coordinates){ 30, 0, font_texture, 6, 12}; break;
			case 'F': return (Coordinates){ 36, 0, font_texture, 5, 12}; break;
			case 'G': return (Coordinates){ 41, 0, font_texture, 7, 12}; break;
			case 'H': return (Coordinates){ 48, 0, font_texture, 8, 12}; break;
			case 'I': return (Coordinates){ 56, 0, font_texture, 4, 12}; break;
			case 'J': return (Coordinates){ 60, 0, font_texture, 5, 12}; break;
			case 'K': return (Coordinates){ 65, 0, font_texture, 7, 12}; break;
			case 'L': return (Coordinates){ 72, 0, font_texture, 6, 12}; break;
			case 'M': return (Coordinates){ 78, 0, font_texture, 10, 12}; break;
			case 'N': return (Coordinates){ 88, 0, font_texture, 7, 12}; break;
			case 'O': return (Coordinates){ 95, 0, font_texture, 8, 12}; break;
			case 'P': return (Coordinates){103, 0, font_texture, 7, 12}; break;
			case 'Q': return (Coordinates){110, 0, font_texture, 8, 12}; break;
			case 'R': return (Coordinates){118, 0, font_texture, 8, 12}; break;
			case 'S': return (Coordinates){126, 0, font_texture, 7, 12}; break;
			case 'T': return (Coordinates){133, 0, font_texture, 6, 12}; break;
			case 'U': return (Coordinates){139, 0, font_texture, 7, 12}; break;
			case 'V': return (Coordinates){146, 0, font_texture, 7, 12}; break;
			case 'W': return (Coordinates){153, 0, font_texture, 11, 12}; break;
			case 'X': return (Coordinates){164, 0, font_texture, 7, 12}; break;
			case 'Y': return (Coordinates){171, 0, font_texture, 6, 12}; break;
			case 'Z': return (Coordinates){177, 0, font_texture, 6, 12}; break;
			case 'a': return (Coordinates){183, 0, font_texture, 6, 12}; break;
			case 'b': return (Coordinates){189, 0, font_texture, 7, 12}; break;
			case 'c': return (Coordinates){196, 0, font_texture, 5, 12}; break;
			case 'd': return (Coordinates){201, 0, font_texture, 7, 12}; break;
			case 'e': return (Coordinates){208, 0, font_texture, 6, 12}; break;
			case 'f': return (Coordinates){214, 0, font_texture, 5, 12}; break;
			case 'g': return (Coordinates){219, 0, font_texture, 7, 12}; break;
			case 'h': return (Coordinates){226, 0, font_texture, 7, 12}; break;
			case 'i': return (Coordinates){233, 0, font_texture, 2, 12}; break;
			case 'j': return (Coordinates){235, 0, font_texture, 3, 12}; break;
			case 'k': return (Coordinates){238, 0, font_texture, 6, 12}; break;
			case 'l': return (Coordinates){244, 0, font_texture, 2, 12}; break;
			case 'm': return (Coordinates){246, 0, font_texture, 10, 12}; break;
			case 'n': return (Coordinates){256, 0, font_texture, 7, 12}; break;
			case 'o': return (Coordinates){263, 0, font_texture, 7, 12}; break;
			case 'p': return (Coordinates){270, 0, font_texture, 7, 12}; break;
			case 'q': return (Coordinates){277, 0, font_texture, 7, 12}; break;
			case 'r': return (Coordinates){284, 0, font_texture, 4, 12}; break;
			case 's': return (Coordinates){288, 0, font_texture, 5, 12}; break;
			case 't': return (Coordinates){293, 0, font_texture, 5, 12}; break;
			case 'u': return (Coordinates){298, 0, font_texture, 7, 12}; break;
			case 'v': return (Coordinates){305, 0, font_texture, 6, 12}; break;
			case 'w': return (Coordinates){311, 0, font_texture, 10, 12}; break;
			case 'x': return (Coordinates){321, 0, font_texture, 6, 12}; break;
			case 'y': return (Coordinates){327, 0, font_texture, 6, 12}; break;
			case 'z': return (Coordinates){333, 0, font_texture, 5, 12}; break;
			case '0': return (Coordinates){338, 0, font_texture, 7, 12}; break;
			case '1': return (Coordinates){345, 0, font_texture, 6, 12}; break;
			case '2': return (Coordinates){351, 0, font_texture, 6, 12}; break;
			case '3': return (Coordinates){358, 0, font_texture, 7, 12}; break;
			case '4': return (Coordinates){365, 0, font_texture, 7, 12}; break;
			case '5': return (Coordinates){372, 0, font_texture, 7, 12}; break;
			case '6': return (Coordinates){379, 0, font_texture, 7, 12}; break;
			case '7': return (Coordinates){386, 0, font_texture, 7, 12}; break;
			case '8': return (Coordinates){393, 0, font_texture, 7, 12}; break;
			case '9': return (Coordinates){400, 0, font_texture, 7, 12}; break;
			case '=': return (Coordinates){407, 0, font_texture, 7, 12}; break;
			case '/': return (Coordinates){414, 0, font_texture, 5, 12}; break;
			case '*': return (Coordinates){419, 0, font_texture, 6, 12}; break;
			case '-': return (Coordinates){425, 0, font_texture, 4, 12}; break;
			case '+': return (Coordinates){429, 0, font_texture, 7, 12}; break;
			case ',': return (Coordinates){436, 0, font_texture, 3, 12}; break;
			case ';': return (Coordinates){439, 0, font_texture, 3, 12}; break;
			case ':': return (Coordinates){442, 0, font_texture, 3, 12}; break;
			case '!': return (Coordinates){445, 0, font_texture, 3, 12}; break;
			case '?': return (Coordinates){448, 0, font_texture, 6, 12}; break;
			case '.': return (Coordinates){454, 0, font_texture, 3, 12}; break;
			case '%': return (Coordinates){457, 0, font_texture, 12, 12}; break;
			case '$': return (Coordinates){469, 0, font_texture, 7, 12}; break;
			case '^': return (Coordinates){476, 0, font_texture, 8, 12}; break;
			case 178: return (Coordinates){484, 0, font_texture, 5, 12}; break;
			case '&': return (Coordinates){489, 0, font_texture, 8, 12}; break;
			case '"': return (Coordinates){497, 0, font_texture, 5, 12}; break;
			case '#': return (Coordinates){502, 0, font_texture, 8, 12}; break;
			case 39:  return (Coordinates){510, 0, font_texture, 3, 12}; break; // apostrophe
			case 40:  return (Coordinates){513, 0, font_texture, 4, 12}; break; // (
			case 41:  return (Coordinates){517, 0, font_texture, 4, 12}; break; // )
			case 91:  return (Coordinates){521, 0, font_texture, 4, 12}; break; // [
			case 93:  return (Coordinates){525, 0, font_texture, 4, 12}; break; // ]
			case 123: return (Coordinates){529, 0, font_texture, 6, 12}; break; // {
			case 125: return (Coordinates){535, 0, font_texture, 6, 12}; break; // }
			case 64:  return (Coordinates){541, 0, font_texture, 9, 12}; break; // @
			case 95:  return (Coordinates){550, 0, font_texture, 8, 12}; break; // _
		}
	}
	else {
		font_texture = &tahoma_regular_texture;
		switch (character) {
			case 'A': return (Coordinates){  0, 0, font_texture, 7, 12}; break;
			case 'B': return (Coordinates){  7, 0, font_texture, 6, 12}; break;
			case 'C': return (Coordinates){ 13, 0, font_texture, 6, 12}; break;
			case 'D': return (Coordinates){ 19, 0, font_texture, 7, 12}; break;
			case 'E': return (Coordinates){ 26, 0, font_texture, 6, 12}; break;
			case 'F': return (Coordinates){ 32, 0, font_texture, 5, 12}; break;
			case 'G': return (Coordinates){ 37, 0, font_texture, 7, 12}; break;
			case 'H': return (Coordinates){ 44, 0, font_texture, 7, 12}; break;
			case 'I': return (Coordinates){ 51, 0, font_texture, 3, 12}; break;
			case 'J': return (Coordinates){ 54, 0, font_texture, 4, 12}; break;
			case 'K': return (Coordinates){ 58, 0, font_texture, 6, 12}; break;
			case 'L': return (Coordinates){ 64, 0, font_texture, 5, 12}; break;
			case 'M': return (Coordinates){ 69, 0, font_texture, 8, 12}; break;
			case 'N': return (Coordinates){ 77, 0, font_texture, 7, 12}; break;
			case 'O': return (Coordinates){ 84, 0, font_texture, 8, 12}; break;
			case 'P': return (Coordinates){ 92, 0, font_texture, 6, 12}; break;
			case 'Q': return (Coordinates){ 98, 0, font_texture, 8, 12}; break;
			case 'R': return (Coordinates){106, 0, font_texture, 6, 12}; break;
			case 'S': return (Coordinates){112, 0, font_texture, 6, 12}; break;
			case 'T': return (Coordinates){118, 0, font_texture, 7, 12}; break;
			case 'U': return (Coordinates){125, 0, font_texture, 7, 12}; break;
			case 'V': return (Coordinates){132, 0, font_texture, 7, 12}; break;
			case 'W': return (Coordinates){139, 0, font_texture, 11, 12}; break;
			case 'X': return (Coordinates){150, 0, font_texture, 6, 12}; break;
			case 'Y': return (Coordinates){156, 0, font_texture, 7, 12}; break;
			case 'Z': return (Coordinates){163, 0, font_texture, 6, 12}; break;
			case 'a': return (Coordinates){169, 0, font_texture, 5, 12}; break;
			case 'b': return (Coordinates){174, 0, font_texture, 6, 12}; break;
			case 'c': return (Coordinates){180, 0, font_texture, 5, 12}; break;
			case 'd': return (Coordinates){185, 0, font_texture, 6, 12}; break;
			case 'e': return (Coordinates){191, 0, font_texture, 6, 12}; break;
			case 'f': return (Coordinates){197, 0, font_texture, 4, 12}; break;
			case 'g': return (Coordinates){201, 0, font_texture, 6, 12}; break;
			case 'h': return (Coordinates){207, 0, font_texture, 6, 12}; break;
			case 'i': return (Coordinates){213, 0, font_texture, 1, 12}; break;
			case 'j': return (Coordinates){214, 0, font_texture, 3, 12}; break;
			case 'k': return (Coordinates){217, 0, font_texture, 5, 12}; break;
			case 'l': return (Coordinates){222, 0, font_texture, 1, 12}; break;
			case 'm': return (Coordinates){223, 0, font_texture, 9, 12}; break;
			case 'n': return (Coordinates){232, 0, font_texture, 6, 12}; break;
			case 'o': return (Coordinates){238, 0, font_texture, 6, 12}; break;
			case 'p': return (Coordinates){244, 0, font_texture, 6, 12}; break;
			case 'q': return (Coordinates){250, 0, font_texture, 6, 12}; break;
			case 'r': return (Coordinates){256, 0, font_texture, 3, 12}; break;
			case 's': return (Coordinates){259, 0, font_texture, 4, 12}; break;
			case 't': return (Coordinates){263, 0, font_texture, 4, 12}; break;
			case 'u': return (Coordinates){267, 0, font_texture, 6, 12}; break;
			case 'v': return (Coordinates){273, 0, font_texture, 5, 12}; break;
			case 'w': return (Coordinates){278, 0, font_texture, 9, 12}; break;
			case 'x': return (Coordinates){287, 0, font_texture, 5, 12}; break;
			case 'y': return (Coordinates){292, 0, font_texture, 5, 12}; break;
			case 'z': return (Coordinates){297, 0, font_texture, 4, 12}; break;
			case '0': return (Coordinates){301, 0, font_texture, 6, 12}; break;
			case '1': return (Coordinates){307, 0, font_texture, 5, 12}; break;
			case '2': return (Coordinates){312, 0, font_texture, 6, 12}; break;
			case '3': return (Coordinates){318, 0, font_texture, 6, 12}; break;
			case '4': return (Coordinates){324, 0, font_texture, 6, 12}; break;
			case '5': return (Coordinates){330, 0, font_texture, 6, 12}; break;
			case '6': return (Coordinates){336, 0, font_texture, 6, 12}; break;
			case '7': return (Coordinates){342, 0, font_texture, 6, 12}; break;
			case '8': return (Coordinates){348, 0, font_texture, 6, 12}; break;
			case '9': return (Coordinates){354, 0, font_texture, 6, 12}; break;
			case '=': return (Coordinates){360, 0, font_texture, 6, 12}; break;
			case '/': return (Coordinates){366, 0, font_texture, 4, 12}; break;
			case '*': return (Coordinates){370, 0, font_texture, 5, 12}; break;
			case '-': return (Coordinates){375, 0, font_texture, 3, 12}; break;
			case '+': return (Coordinates){378, 0, font_texture, 7, 12}; break;
			case ',': return (Coordinates){385, 0, font_texture, 3, 12}; break;
			case ';': return (Coordinates){388, 0, font_texture, 3, 12}; break;
			case ':': return (Coordinates){391, 0, font_texture, 2, 12}; break;
			case '!': return (Coordinates){393, 0, font_texture, 2, 12}; break;
			case '?': return (Coordinates){395, 0, font_texture, 5, 12}; break;
			case '.': return (Coordinates){400, 0, font_texture, 3, 12}; break;
			case '%': return (Coordinates){403, 0, font_texture, 11, 12}; break;
			case '$': return (Coordinates){414, 0, font_texture, 5, 12}; break;
			case '^': return (Coordinates){419, 0, font_texture, 7, 12}; break;
			case 178: return (Coordinates){426, 0, font_texture, 4, 12}; break;
			case '&': return (Coordinates){430, 0, font_texture, 7, 12}; break;
			case '"': return (Coordinates){437, 0, font_texture, 3, 12}; break;
			case '#': return (Coordinates){440, 0, font_texture, 7, 12}; break;
			case 39:  return (Coordinates){447, 0, font_texture, 2, 12}; break; // apostrophe
			case 40:  return (Coordinates){449, 0, font_texture, 3, 12}; break; // (
			case 41:  return (Coordinates){452, 0, font_texture, 3, 12}; break; // )
			case 91:  return (Coordinates){455, 0, font_texture, 3, 12}; break; // [
			case 93:  return (Coordinates){458, 0, font_texture, 3, 12}; break; // ]
			case 123: return (Coordinates){461, 0, font_texture, 5, 12}; break; // {
			case 125: return (Coordinates){466, 0, font_texture, 5, 12}; break; // }
			case 64:  return (Coordinates){471, 0, font_texture, 10, 12}; break; // @
			case 95:  return (Coordinates){481, 0, font_texture, 7, 12}; break; // _
		}
	}
	return (Coordinates){0, 0, 0, 0, 12};
}