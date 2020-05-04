#include <nusys.h>
#include <PR/gs2dex.h>
#include "main.h"
#include "2dlibrary.h"
#include "controller_pak.h"

#define SCREEN_SIZE_LOW		0
#define SCREEN_SIZE_HIGH	1
#define GFX_LIST_NUM		1
#define GFX_LIST_SIZE		16384

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

Gfx	gfxListBuf[GFX_LIST_NUM][GFX_LIST_SIZE];
u32	gfxListCnt = 0;
Gfx*	gfxListPtr;
Gfx*	gfxListStartPtr;

extern u32 mainNo;

/*----------------------------------------------------------------------*/
/*	Clear the frame buffer. 											*/
/*	IN:		None														*/
/*	RET:	None											   			*/
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
/*	Change the display list buffer. 									*/
/*	IN:	None															*/
/*	RET:	Pointer to the display list buffer.		   					*/
/*----------------------------------------------------------------------*/
void gfxListBufferChange()
{
	gfxListStartPtr = &gfxListBuf[gfxListCnt][0];
	gfxListPtr = gfxListStartPtr;
	return;
}


void RCPInit() {
	/* Set the display list buffer. */
	gfxListBufferChange();
	gSPDisplayList(gfxListPtr++, rdpInit_dl);
    gDPSetDepthSource(gfxListPtr++, G_ZS_PRIM);
	/* Clear the screen. */
	gfxClearCfb();

	my2D_clear();
	
	gSPDisplayList(gfxListPtr++, rdpInit_dl);
	gSPDisplayList(gfxListPtr++, spriteInit_dl);
	gDPSetRenderMode(gfxListPtr++, G_RM_SPRITE, G_RM_SPRITE2);
	gDPSetCycleType(gfxListPtr++, G_CYC_COPY);
	gDPSetTextureFilter(gfxListPtr++, G_TF_POINT);
	

}

void RCPEnd() {
	gDPFullSync(gfxListPtr++);
	gSPEndDisplayList(gfxListPtr++);
	
	if (show_console) {
		nuGfxTaskStart(gfxListStartPtr, gfxListPtr - gfxListStartPtr, NU_GFX_UCODE_S2DEX,NU_SC_NOSWAPBUFFER);
		nuDebConDisp(NU_SC_NOSWAPBUFFER);
		nuDebTaskPerfBar1(1, 420, NU_SC_SWAPBUFFER);
	}
	else
		nuGfxTaskStart(gfxListStartPtr, gfxListPtr - gfxListStartPtr, NU_GFX_UCODE_S2DEX,NU_SC_SWAPBUFFER);
	
	
	frame_number++;
}


