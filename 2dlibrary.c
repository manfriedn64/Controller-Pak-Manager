#include  <nusys.h>
#include "2dlibrary.h"

extern Gfx*	gfxListPtr;
extern Gfx*	gfxListPtr;
extern My2DLibrary my2dlibrary;

void my2D_clear() {
	my2dlibrary.currentTextureInTMEM  = NULL;
	my2d_moveObjectsClear();
	my2D_backgroundListClear();
	my2D_spriteListClear();
}

void my2D_init(short width, short height, u32* texturePointer) {
	my2dlibrary.currentTextureInTMEM	= NULL;
	my2dlibrary.width					= width;
	my2dlibrary.height					= height;
	my2dlibrary.ratio					= 640 / width;
	my2dlibrary.cycleType				= NULL;
	my2dlibrary.rgb						= (Rgb){NULL, NULL, NULL};
	my2dlibrary.texturePointer			= texturePointer;
	return;
}

u32* my2D_loadSegmentToRam(u32 segmentStart, u32 segmentEnd) {
	my2dlibrary.texturePointer -= segmentEnd - segmentStart;
	nuPiReadRom(segmentStart, my2dlibrary.texturePointer, segmentEnd - segmentStart);
	return my2dlibrary.texturePointer;
}

void my2D_loadTexture(Texture* texture, u32 segmentStart, u32 segmentEnd, short width, short height) {
	texture->romStart			= segmentStart;
	texture->romEnd				= segmentEnd;
	texture->pointer32			= my2D_loadSegmentToRam(segmentStart, segmentEnd);
	texture->pointer64			= (u64*)texture->pointer32;
	texture->width				= width;
	texture->height				= height;
	texture->malloc				= False;
}

/*
	void my2D_setCycleType(u32 newCycleType)
	
	change the RDP Cycle Type only if needed
	newCycleType = cycle type as defined in n64 sdk
*/
void my2D_setCycleType(u32 newCycleType) {
	if (my2dlibrary.cycleType != newCycleType) {
		gDPPipeSync(gfxListPtr++);
		gDPSetCycleType(gfxListPtr++, newCycleType);
	}
}

/*
	void my2D_setColorFill(Rgb rgb)
	
	change the RDP fill color only if needed
	rgb = red, green, blue values
	
*/
void my2D_setColorFill(Rgb rgb) {
	if (rgb.red != my2dlibrary.rgb.red || rgb.green != my2dlibrary.rgb.green || rgb.blue != my2dlibrary.rgb.blue) {
		gDPPipeSync(gfxListPtr++);
		gDPSetFillColor(gfxListPtr++, (GPACK_RGBA5551(rgb.red, rgb.green, rgb.blue, 1) << 16 | GPACK_RGBA5551(rgb.red, rgb.green, rgb.blue, 1)));
		my2dlibrary.rgb.red = rgb.red;
		my2dlibrary.rgb.green = rgb.green;
		my2dlibrary.rgb.blue = rgb.blue;
	}
}

void my2d_moveObjectsClear() {
	my2d_moveOjectsCount = -1;
}

My2DMoveObject* my2D_getNextMoveObjects() {
	if (my2d_moveOjectsCount < MY2D_MOVE_OBJECTS_LIMIT) {
		my2d_moveOjectsPointer = &my2d_moveOjectsList[my2d_moveOjectsCount++];
		return my2d_moveOjectsPointer;
	}
	return NULL;
}

Bool my2D_doMoveOject(My2DMoveObject* moveobject) {
	if (moveobject->destY == moveobject->posY && moveobject->destX == moveobject->posX)
		return True;
	else {
		if (moveobject->destX != moveobject->posX)
			moveobject->posX += moveobject->moveX;
		if (moveobject->destY != moveobject->posY)
			moveobject->posY += moveobject->moveY;
		return False;
	}
}


/*
	void my2D_drawRectangle(short start_x, short start_y, short end_x, short end_y, short red, short green, short blue, short isFilled)
	
	draw rectangle of the desired shape and color. This rectangle can be filled with color or not.
	start_x		= the x-coordinate of upper-left corner of rectangle
	start_y		= the y-coordinate of upper-left corner of rectangle
	end_x		= the x-coordinate of lower-right corner of rectangle
	end_y		= the y-coordinate of lower-right corner of rectangle
	red			= RGB red value
	green		= RGB green value
	blue		= RGB blue value
	isFilled	= is this rectangle filled with color or not ?
*/
void my2D_drawRectangle(short start_x, short start_y, short end_x, short end_y, short red, short green, short blue, short isFilled) {
	my2D_setCycleType(G_CYC_FILL);
	my2D_setColorFill((Rgb){red, green, blue});
	if (isFilled) {
		gDPFillRectangle(gfxListPtr++, start_x, start_y, end_x, end_y);
		/*gDPSetCycleType(gfxListPtr++, G_CYC_1CYCLE);
		gDPPipeSync(gfxListPtr++);*/
	} else {
		gDPFillRectangle(gfxListPtr++, start_x, start_y, end_x, start_y);
		gDPFillRectangle(gfxListPtr++, start_x, start_y, start_x, end_y);
		gDPFillRectangle(gfxListPtr++, start_x, end_y, end_x, end_y);
		gDPFillRectangle(gfxListPtr++, end_x, start_y, end_x, end_y);
	}
}

/*
	void my2D_drawSprite(int x, int y, Coordinates* coordinates)
	
	draw a sprite and the requested position
	coordinates	= definition of the sprite
	x			= the x-coordinate of upper-left corner of the sprite
	y			= the y-coordinate of upper-left corner of the sprite
*/
void my2D_drawSprite(Coordinates* coordinates, int x, int y) {
	uObjSprite* sprite;
	my2D_setCycleType(G_CYC_COPY);
	// load texture to TMEM if not the current one
	if (my2dlibrary.currentTextureInTMEM != coordinates->texture->pointer32) {
		gDPLoadTextureBlock(
			gfxListPtr++, 
			coordinates->texture->pointer32, 
			G_IM_FMT_RGBA, G_IM_SIZ_16b, 
			coordinates->texture->width, coordinates->texture->height, 0, 
			G_TX_WRAP, G_TX_WRAP, 
			G_TX_NOMASK, G_TX_NOMASK, 
			G_TX_NOLOD, G_TX_NOLOD
		);
		my2dlibrary.currentTextureInTMEM = coordinates->texture->pointer32;
	}
	// configure the sprite and draw it
	sprite = (uObjSprite*)my2D_getNextSprite();
	sprite->s.objX = x << 2; // x screen coordinate
	sprite->s.objY = y << 2; // y screen coordinate
	sprite->s.imageW = coordinates->width << 5; // width
	sprite->s.imageH = coordinates->height << 5; // height
	sprite->s.imageAdrs = GS_PIX2TMEM (coordinates->x + coordinates->y * 64, G_IM_SIZ_16b);
	sprite->s.imageStride = GS_PIX2TMEM (coordinates->texture->width, G_IM_SIZ_16b);
	gSPObjRectangle(gfxListPtr++, sprite);
	return;
}

/*
	void my2D_drawBackGroundCoordinates(Coordinates* coordinates, short x, short y, short line)
	
	draw a part of a background at the request x and y position.
	coordinates	= definition of the background part
	x			= x position in screen
	y			= y position in screen
	line		= multiplies y coordinates of texture. 
	
*/
void my2D_drawBackGroundCoordinates(Coordinates* coordinates, short x, short y, short line) {
	uObjBg* background = (uObjBg*)my2D_getNextBackground();
	if (background) {
	my2D_setCycleType(G_CYC_COPY);
	if (x == MY2D_CENTER)
		x = my2dlibrary.width / 2 - coordinates->width / 2;
	if (y == MY2D_CENTER)
		y = my2dlibrary.height / 2 - coordinates->height / 2;
	background->b.imagePtr 	= coordinates->texture->pointer64;
	background->b.frameX 		= x << 2;
	background->b.frameY 		= y << 2;
	background->b.imageX 		= coordinates->x << 5;
	background->b.imageY 		= (coordinates->y + line * coordinates->height) << 5;
	background->b.imageW 		= coordinates->texture->width << 2;
	background->b.imageH 		= coordinates->texture->height << 2;
	background->b.frameW 		= coordinates->width << 2;
	background->b.frameH 		= coordinates->height << 2;
	guS2DInitBg(background);
	gSPBgRectCopy(gfxListPtr++, background);	
	}
	return;
}

/*
	void my2D_drawFullBackGround(Texture* texture, short x, short y)
	
	Draw and entire "background sprite" at the requested position in screen
	texture	= definition of the background to draw
	x		= x position in screen
	y		= y position in screen
*/
void my2D_drawFullBackGround(Texture* texture, short x, short y) {
	if (texture != NULL) {
		Coordinates coordinates = (Coordinates){0, 0, texture, texture->width, texture->height};
		my2D_drawBackGroundCoordinates(&coordinates, x, y, 0);
	}
	return;
}


/* 
	The following functions handles the uObjBg objects array and pointers
 */
void my2D_backgroundListClear() {
	my2dlibrary.background_pointer	= &my2dlibrary.background_list[0];
	my2dlibrary.background_count		= 0;		
}
uObjBg* my2D_getNextBackground() {
	if (my2dlibrary.background_count < MY2D_OBJ_LIST_SIZE) {
		my2dlibrary.background_pointer = &my2dlibrary.background_list[my2dlibrary.background_count++];
		my2dlibrary.background_pointer->b.imageLoad 	= G_BGLT_LOADTILE;
		my2dlibrary.background_pointer->b.imageFmt 		= G_IM_FMT_RGBA;
		my2dlibrary.background_pointer->b.imageSiz 		= G_IM_SIZ_16b;
		my2dlibrary.background_pointer->b.imagePal 		= 0;
		my2dlibrary.background_pointer->b.imageFlip 	= 0;
		return my2dlibrary.background_pointer;
	}
	return 0;
}


/* 
	The following functions handles the uObjSprite objects array and pointers
 */
void my2D_spriteListClear() {
	my2dlibrary.sprite_pointer = &my2dlibrary.sprite_list[0];
	my2dlibrary.sprite_count = 0;
	return;
}
 
uObjSprite* my2D_getNextSprite() {
	if (my2dlibrary.sprite_count < MY2D_SPRITE_LIST_SIZE) {
		my2dlibrary.sprite_pointer = &my2dlibrary.sprite_list[my2dlibrary.sprite_count++];
		my2dlibrary.sprite_pointer->s.paddingX 		= 0;
		my2dlibrary.sprite_pointer->s.paddingY 		= 0;
		my2dlibrary.sprite_pointer->s.imageFmt 		= G_IM_FMT_RGBA;
		my2dlibrary.sprite_pointer->s.imageSiz 		= G_IM_SIZ_16b;
		my2dlibrary.sprite_pointer->s.imagePal 		= 0;
		my2dlibrary.sprite_pointer->s.imageFlags	= 0;
		my2dlibrary.sprite_pointer->s.scaleH		= 1 << 10;
		my2dlibrary.sprite_pointer->s.scaleW		= 1 << 10;
		return my2dlibrary.sprite_pointer;
	}
	else
		return 0;
}