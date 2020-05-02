#ifndef _2DLIBRARY_
#define _2DLIBRARY_

#define MY2D_MOVE_OBJECTS_LIMIT	10
#define MY2D_CENTER				-9999

typedef enum
{ False = 0, True = 1 }
Bool;

typedef struct {
	int 	red;
	int		green;
	int		blue;
} Rgb;

/* defining a texture */

typedef struct  {
	u32*	pointer32;
	u64*	pointer64;
	u32		romStart;
	u32		romEnd;
	u32		romSize;
	int		width;
	int		height;
	Bool	malloc;
} Texture; 


typedef struct {
	Texture*	texture;
	int			positionY;
	int			scrollX;
	float		speed;
} Background;


typedef struct {
	int x;
	int y;
	Texture* texture;
	int width;
	int height;
} Coordinates;

typedef struct {
	Coordinates		coordinates;
	int				posX;
	int 			posY;
	int				destX;
	int 			destY;
	int				moveX;
	int				moveY;
} My2DMoveObject;

typedef struct {
	short 	width;
	short	height;
	short	ratio;
	u32		cycleType;
	u32*	currentTextureInTMEM;
	Rgb		rgb;
	u32*	texturePointer;
} My2DLibrary;


void my2D_init(short width, short height, u32* texturePointer);
void my2d_moveObjectsListClear();
My2DMoveObject* getNextMoveObjects();
Bool my2D_doMoveOject(My2DMoveObject* moveobject);
void my2D_drawSprite(Coordinates* coordinates, int x, int y);
void my2D_drawBackGroundCoordinates(Coordinates* coordinates, short x, short y, short line);
void my2D_drawFullBackGround(Texture* texture, short x, short y);
void my2D_drawRectangle(short start_x, short start_y, short end_x, short end_y, short red, short green, short blue, short isFilled);
void my2D_drawDialogBox(short start_x, short start_y, short width, short height);
void my2D_drawSelectList(short start_x, short start_y, short end_x, short end_y);

void my2D_setColorFill(Rgb rgb);
void my2D_setCycleType(u32 newCycleType);

u32* my2D_loadSegmentToRam(u32 segmentStart, u32 segmentEnd);
void my2D_loadTexture(Texture* texture, u32 segmentStart, u32 segmentEnd, short width, short height);

My2DLibrary	my2dlibrary;

My2DMoveObject		my2d_moveOjectsList[MY2D_MOVE_OBJECTS_LIMIT];
My2DMoveObject*		my2d_moveOjectsPointer;
short				my2d_moveOjectsCount;

#endif /* _2DLIBRARY_ */