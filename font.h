#ifndef _FONT_
#define _FONT_

#include "2dlibrary.h"

#define FONT_REGULAR	0
#define FONT_BOLD		1

#define FONT_WHITE		0
#define FONT_BLACK		1

#define FONT_LEFT		0
#define FONT_RIGHT		1
#define FONT_CENTER		2 

typedef struct {
	short		weight;
	short		color;
	short		halign;
	short		latestX;
	short		latestY;
	short		latestMaxX;
	short		nextStartX;
	short		nextStartY;
	void		(*writeNext)(char *text);
	void		(*writeNextLine)(char *text);
	void		(*writePosition)(int, int, int, char *text);
	Coordinates	(*getChar)(char);
} FontConfig;

char * fontSet(char* text);
void font_init();
int  font_getLineWidth(char* text, int line);
int  font_getNmbrLines(char* text);
void font_writeNext(char* text);
void font_writeNextLine(char* text);
void font_write(int start_x, int start_y, int max_x, char* text);



#endif /* _FONT_ */