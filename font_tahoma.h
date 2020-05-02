#ifndef _FONT_TAHOMA_
#define _FONT_TAHOMA_

#include "2dlibrary.h"

#define FONT_REGULAR	0
#define FONT_BOLD		1

#define FONT_WHITE		0
#define FONT_BLACK		1

#define FONT_LEFT		0
#define FONT_RIGHT		1
#define FONT_CENTER		2 

typedef struct {
	short	weight;
	short	color;
	short	halign;
} FontTahoma;

int getLineWidth(char* text, int line);
int getNmbrLines(char* text);
Coordinates getCharCoordinates(char character);

char * fontSet(char* text);
void initTahoma();
void writeTahoma(int start_x, int start_y, int max_x, char* text);

#endif /* _FONT_TAHOMA_ */