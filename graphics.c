#include <nusys.h>
#include <PR/gs2dex.h>
#include "main.h"
#include "2dlibrary.h"
#include "controller_pak.h"

#define SCREEN_SIZE_LOW		0
#define SCREEN_SIZE_HIGH	1
#define GFX_LIST_NUM		1
#define GFX_LIST_SIZE		16384
#define OBJ_LIST_SIZE		1000
#define SPRITE_LIST_SIZE	100

Gfx rdpInit_dl[] = {
	gsDPPipeSync(),
	gsDPPipelineMode(G_PM_1PRIMITIVE),
	gsDPSetTextureLOD(G_TL_TILE),
	gsDPSetTextureLUT(G_TT_NONE),
	gsDPSetTextureDetail(G_TD_CLAMP),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetTextureFilter(G_TF_BILERP),
	gsDPSetTextureConvert(G_TC_FILT),
	gsDPSetCombineMode(G_CC_SHADE, G_CC_SHADE),
	gsDPSetCombineKey(G_CK_NONE),
	gsDPSetAlphaCompare(G_AC_NONE),
	gsDPSetColorDither(G_CD_DISABLE),
	gsDPSetScissor(G_SC_NON_INTERLACE, 0, 0,640, 480),
	gsSPEndDisplayList(),
};

Gfx spriteInit_dl[] = {
	gsDPPipeSync(),
	gsDPSetTexturePersp(G_TP_NONE),
	gsDPSetTextureLOD(G_TL_TILE),
	gsDPSetTextureLUT(G_TT_NONE),
	gsDPSetTextureConvert(G_TC_FILT),
	gsDPSetAlphaCompare(G_AC_THRESHOLD),
	gsDPSetBlendColor(0, 0, 0, 0x01),
	gsDPSetCombineMode(G_CC_DECALRGBA, G_CC_DECALRGBA),
	gsSPEndDisplayList(),
};

uObjBg	objList[OBJ_LIST_SIZE];
uObjBg* objListPrt;
int objListCnt = 0;

uObjSprite	spriteList[SPRITE_LIST_SIZE];
uObjSprite*	spriteListPtr;
int spriteListCnt = 0;

Gfx	gfxListBuf[GFX_LIST_NUM][GFX_LIST_SIZE];
u32	gfxListCnt = 0;
Gfx*	gfxListPtr;
Gfx*	gfxListStartPtr;

extern u32 mainNo;

/*----------------------------------------------------------------------*/
/*	Clear the frame buffer. 								*/
/*	IN:	**glist_ptr										  */
/*			SCREEN_SIZE_LOW	 0							*/
/*			SCREEN_SIZE_HIGH 1					  		*/
/*	RET:	None										   	*/
/*----------------------------------------------------------------------*/
void gfxClearCfb()
{
	gSPSegment(gfxListPtr++, 0, 0x0);
	
	
	gDPSetColorImage(gfxListPtr++, G_IM_FMT_RGBA, G_IM_SIZ_16b, my2dlibrary.width, OS_K0_TO_PHYSICAL(nuGfxCfb_ptr)); 	
	
	my2D_drawRectangle(0, 0, my2dlibrary.width-1, my2dlibrary.height-1, 192, 192, 192, 1);
	
	//drawRectangle(8, 8, my2dlibrary.width-9, my2dlibrary.height-9, 0, 0, 255, 0);
	
	
	//gDPPipeSync(gfxListPtr++);
}

/*----------------------------------------------------------------------*/
/*	Change the display list buffer. 							*/
/*	IN:	None												*/
/*	RET:	Pointer to the display list buffer.		   			*/
/*----------------------------------------------------------------------*/
void gfxListBufferChange(void)
{
	gfxListStartPtr = &gfxListBuf[gfxListCnt][0];
	gfxListPtr = gfxListStartPtr;
	return;
}

void objListClear(void)
{
	objListPrt = &objList[0];
	objListCnt = 0;
	return;
}

uObjBg* getNextObj()
{
	if (objListCnt < OBJ_LIST_SIZE) {
		objListPrt = &objList[objListCnt++];
		//objListPrt->b.imageLoad 	= G_BGLT_LOADBLOCK;
		objListPrt->b.imageLoad 	= G_BGLT_LOADTILE;
		objListPrt->b.imageFmt 		= G_IM_FMT_RGBA;
		objListPrt->b.imageSiz 		= G_IM_SIZ_16b;
		objListPrt->b.imagePal 		= 0;
		objListPrt->b.imageFlip 	= 0;
		return objListPrt;
	}
	return 0;
}

void spriteListClear(void)
{
	spriteListPtr = &spriteList[0];
	spriteListCnt = 0;
	return;
}

uObjSprite* getNextSprite()
{
	// typedef struct {
	  // s16 objX;        /* x-coordinate of upper-left corner of OBJ (s10.2) */
	  // u16 scaleW;      /* Width-direction scaling (u5.10) */
	  // u16 imageW;      /* Texture width (length in s direction, u10.5)   */
	  // u16 paddingX;    /* Unused (always 0)  */
	  // s16 objY;        /* y-coordinate of upper-left corner of OBJ (s10.2)  */
	  // u16 scaleH;      /* Height-direction scaling (u5.10) */
	  // u16 imageH;      /* Texture height (length in t direction, u10.5)  */
	  // u16 paddingY;    /* Unused (always 0) */
	  // u16 imageStride; /* Texel wrapping width (In units of 64-bit words)  */
	  // u16 imageAdrs;   /* Texture starting position in TMEM (In units of 64-bit words)  */  
	  // u8  imageFmt;    /* Texel format
						  // G_IM_FMT_RGBA (RGBA format)
						  // G_IM_FMT_YUV (YUV format)
						  // G_IM_FMT_CI (CI format)
						  // G_IM_FMT_IA (IA format)
						  // G_IM_FMT_I (I format)*/
	  // u8  imageSiz;    /* Texel size
						  // G_IM_SIZ_4b (4 bits/texel)
						  // G_IM_SIZ_8b (8 bits/texel)
						  // G_IM_SIZ_16b (16 bits/texel)
						  // G_IM_SIZ_32b (32 bits/texel) */
	  // u16 imagePal;    /* Position of palette for 4-bit color
						  // index texture  (4-bit precision, 0~7)   */
	  // u8  imageFlags;  /* Display flag
						  // (*) Multiple flags can be specified as the bit sum of the flags: 
						  // 0 (Normal display (no inversion))
						  // G_OBJ_FLAG_FLIPS (s-direction (x) inversion)
						  // G_OBJ_FLAG_FLIPT  (t-direction (y) inversion)  */
	// } uObjSprite_t;    /* 24 bytes */
	if (spriteListCnt < SPRITE_LIST_SIZE) {
		spriteListPtr = &spriteList[spriteListCnt++];
		spriteListPtr->s.paddingX 		= 0;
		spriteListPtr->s.paddingY 		= 0;
		spriteListPtr->s.imageFmt 		= G_IM_FMT_RGBA;
		spriteListPtr->s.imageSiz 		= G_IM_SIZ_16b;
		spriteListPtr->s.imagePal 		= 0;
		spriteListPtr->s.imageFlags		= 0;
		spriteListPtr->s.scaleH			= 1 << 10;
		spriteListPtr->s.scaleW			= 1 << 10;
	}
	else
		return 0;
}

void RCPInit() {
	my2dlibrary.currentTextureInTMEM  = NULL;
	/* Set the display list buffer. */
	gfxListBufferChange();
	gSPDisplayList(gfxListPtr++, rdpInit_dl);
    gDPSetDepthSource(gfxListPtr++, G_ZS_PRIM);
	/* Clear the screen. */
	gfxClearCfb();

	
	/* before drawing any sprite, first clear / init the list of uObjBg that we will be using */
	objListClear();
	spriteListClear();
	gSPDisplayList(gfxListPtr++, rdpInit_dl);
	gSPDisplayList(gfxListPtr++, spriteInit_dl);
	gDPSetRenderMode(gfxListPtr++, G_RM_SPRITE, G_RM_SPRITE2);
	gDPSetCycleType(gfxListPtr++, G_CYC_COPY);
	gDPSetTextureFilter(gfxListPtr++, G_TF_POINT);
	

}

void RCPEnd() {
	//nuDebConClear(0);	
	
	/*
	nuDebConTextPos(0,1,16);
	sprintf(conbuf, "objListCnt    : %d   ", objListCnt);
	nuDebConCPuts(0, conbuf);
	nuDebConTextPos(0,1,17);
	sprintf(conbuf, "spriteListCnt : %d   ", spriteListCnt);
	nuDebConCPuts(0, conbuf);
	nuDebConTextPos(0,1,18);
	sprintf(conbuf, "gfx size      : %d   ", (int)((gfxListPtr - gfxListStartPtr) / sizeof(Gfx)));
	nuDebConCPuts(0, conbuf);
	*/
	
	
	gDPFullSync(gfxListPtr++);
	
	/* end top-level display list */
	
	gSPEndDisplayList(gfxListPtr++);
	
	nuGfxTaskStart(gfxListStartPtr, gfxListPtr - gfxListStartPtr, NU_GFX_UCODE_S2DEX,NU_SC_NOSWAPBUFFER);
	
	
	//gfxListBufferChange();
	
	
	/* debug bar + blink message to check it's running */
	
	nuDebConDisp(NU_SC_NOSWAPBUFFER);
	
	nuDebTaskPerfBar1(1, 420, NU_SC_SWAPBUFFER);
	
	//readController();
	//drawDebug();
	
	frame_number++;
}


