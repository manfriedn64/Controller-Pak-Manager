#include  <nusys.h>
#include "user_interface.h"
#include "2dlibrary.h"
#include "font.h"

extern FontConfig font_config;

void my2D_drawSelectList(short start_x, short start_y, short end_x, short end_y) {
	my2D_drawRectangle(start_x, start_y, start_x, end_y, 132, 132, 132, 1);
	my2D_drawRectangle(start_x, start_y, end_x, start_y, 132, 132, 132, 1);
	my2D_drawRectangle(start_x+1, start_y+1, start_x+1, end_y-1, 0, 0, 0, 1);
	my2D_drawRectangle(start_x+1, start_y+1, end_x-2, start_y+1, 0, 0, 0, 1);
	my2D_drawRectangle(end_x-1, start_y+1, end_x-1, end_y-1, 192, 192, 192, 1);
	my2D_drawRectangle(start_x+1, end_y-1, end_x-1, end_y-1, 192, 192, 192, 1);
	my2D_drawRectangle(end_x, start_y, end_x, end_y, 255, 255, 255, 1);
	my2D_drawRectangle(start_x, end_y, end_x, end_y, 255, 255, 255, 1);
	my2D_drawRectangle(start_x+2, start_y+2, end_x-2, end_y-2, 255, 255, 255, 1);
}

void my2D_drawDialogBox(short x, short y, short width, short height, char *text) {
	short start_x, start_y, end_x, end_y;
	FontConfig save_config = font_config;
	if (x == MY2D_CENTER) 
		start_x = my2dlibrary.width / 2 - width / 2;
	else 
		start_x = x;
	end_x = start_x + width;
	if (y == MY2D_CENTER) 
		start_y = my2dlibrary.height / 2 - height / 2;
	else
		start_y = y;
	end_y = start_y + height;
	my2D_drawRectangle(start_x+1, start_y+1, end_x-1, end_y-1, 192, 192, 192, 1); // fill
	my2D_drawRectangle(start_x, start_y, start_x, end_y, 255, 255, 255, 1); // left
	my2D_drawRectangle(start_x, start_y, end_x, start_y, 255, 255, 255, 1); // top
	my2D_drawRectangle(end_x-1, start_y+1, end_x-1, end_y-1, 132, 132, 132, 1); // right
	my2D_drawRectangle(start_x+1, end_y-1, end_x-1, end_y-1, 132, 132, 132, 1); // bottom
	my2D_drawRectangle(end_x, start_y, end_x, end_y, 0, 0, 0, 1); // right 
	my2D_drawRectangle(start_x, end_y, end_x, end_y, 0, 0, 0, 1); // bottom
	my2D_drawRectangle(start_x+2, start_y+2, end_x-3, start_y+17, 0, 0, 127, 1); // title
	font_config.weight = FONT_BOLD;
	font_config.halign = FONT_CENTER;
	font_config.color  = FONT_WHITE;
	font_config.writePosition(start_x, start_y+4, end_x, text);
	font_config = save_config;
}

void my2D_drawButtonIcon(short x, short y, short isSelected, Coordinates* coordinates) {
	my2D_drawButton(x, y, coordinates->width + 4, coordinates->height + 4, isSelected);
	my2D_drawBackGroundCoordinates(coordinates, x+2, y+2, 0);
}

void my2D_drawButtonText(short x, short y, short width, short height, short isSelected, char *text) {
	short start_x, start_y, end_x, end_y, halign;
	if (x == MY2D_CENTER) 
		start_x = my2dlibrary.width / 2 - width / 2;
	else 
		start_x = x;
	end_x = start_x + width;
	if (y == MY2D_CENTER) 
		start_y = my2dlibrary.height / 2 - height / 2;
	else
		start_y = y;
	end_y = start_y + height;
	my2D_drawButton(x, y, width, height, isSelected);
	halign = font_config.halign;
	font_config.halign = FONT_CENTER;
	font_config.writePosition(start_x+3, start_y+3, end_x-3, text);
	font_config.halign = halign;
}

void my2D_drawButton(short x, short y, short width, short height, short isSelected) {
	short start_x, start_y, end_x, end_y;
	if (x == MY2D_CENTER) 
		start_x = my2dlibrary.width / 2 - width / 2;
	else 
		start_x = x;
	end_x = start_x + width;
	if (y == MY2D_CENTER) 
		start_y = my2dlibrary.height / 2 - height / 2;
	else
		start_y = y;
	end_y = start_y + height;
	if (!isSelected) {
		my2D_drawRectangle(start_x, start_y, start_x, end_y, 255, 255, 255, 1);
		my2D_drawRectangle(start_x, start_y, end_x, start_y, 255, 255, 255, 1);
		my2D_drawRectangle(end_x-1, start_y+1, end_x-1, end_y-1, 132, 132, 132, 1);
		my2D_drawRectangle(start_x+1, end_y-1, end_x-1, end_y-1, 132, 132, 132, 1);
		my2D_drawRectangle(end_x, start_y, end_x, end_y, 0, 0, 0, 1);
		my2D_drawRectangle(start_x, end_y, end_x, end_y, 0, 0, 0, 1);
	} else {
		my2D_drawRectangle(start_x, start_y, start_x, end_y, 132, 132, 132, 1);
		my2D_drawRectangle(start_x, start_y, end_x, start_y, 132, 132, 132, 1);
		
		my2D_drawRectangle(start_x+1, start_y+1, start_x+1, end_y-1, 0, 0, 0, 1);
		my2D_drawRectangle(start_x+1, start_y+1, end_x-1, start_y+1, 0, 0, 0, 1);
		
		my2D_drawRectangle(start_x+2, start_y+2, end_x-1, end_y-1, 192, 192, 192, 1);
		my2D_drawRectangle(end_x, start_y, end_x, end_y, 255, 255, 255, 1);
		my2D_drawRectangle(start_x, end_y, end_x, end_y, 255, 255, 255, 1);		
	}
}
