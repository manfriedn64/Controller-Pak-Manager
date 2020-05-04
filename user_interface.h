#ifndef _USER_INTERFACE_
#define _USER_INTERFACE_

#include "font.h"

#define UI_FILL_COLOR_GENERAL	(Rgb){192, 192, 192}
#define UI_FILL_COLOR_ELEMENT	(Rgb){255, 255, 255}
#define UI_FILL_COLOR_TITLE		(Rgb){  0,   0, 128}
#define UI_BORDER_CLEAR1		(Rgb){255, 255, 255}
#define UI_BORDER_CLEAR2		(Rgb){192, 192, 192}
#define UI_BORDER_DARK1			(Rgb){  0  , 0,   0}
#define UI_BORDER_DARK2			(Rgb){128, 128, 128}
#define UI_TEXT_COLOR_GENERAL	FONT_BLACK
#define UI_TEXT_COLOR_TITLE		FONT WHITE
#define UI_TEXT_COLOR_ELEMENT	FONT_BLACK

void my2D_drawDialogBox(short x, short y, short width, short height, char *text);
void my2D_drawButton(short x, short y, short width, short height, short isSelected);
void my2D_drawButtonIcon(short x, short y, short isSelected, Coordinates* coordinates);
void my2D_drawButtonText(short x, short y, short width, short height, short isSelected, char *text);
void my2D_drawSelectList(short start_x, short start_y, short end_x, short end_y);



#endif /* _USER_INTERFACE_ */