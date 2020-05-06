#include <nusys.h>
#include "main.h"
#include "controller_pak.h"

u16* HighFrameBuf[2] = {
	(u16*)CFB_HIGH_ADDR0,
	(u16*)CFB_HIGH_ADDR1
};

static void callbackPrenmi();
static void callbackControllerPak();

extern u64 time_lastframe;
extern int frame_number;
extern float random;

extern char conbuf[40];
extern char mem_heap[32 * 1024]; 

extern u32 memory_size;
extern u8 show_console;


/*----------------------------------------------------------------------*/
/*	Game startup. 											*/
/*	IN:	Nothing in particular. 							  	*/
/*	RET:	None											  */
/*----------------------------------------------------------------------*/
void mainproc(void* arg)
{
	// expansion pak detection. Needed to play sounds as I was running out of RAM for this feature
	memory_size = osGetMemSize();
	controllerPakInit();
	
	// variable initialization used to count frames per second
	time_lastframe = 0;
	frame_number = 0;
	show_console = 0;
	
	/* Initialize graphics */
	nuGfxInit();
	
	
	 /* Set VI */
	 osCreateViManager(OS_PRIORITY_VIMGR);
	 if (osTvType == OS_TV_PAL ) {
		osViSetMode(&osViModeTable[OS_VI_PAL_HAN1]);
		osViSetYScale(0.833);
		nuPreNMIFuncSet((NUScPreNMIFunc)callbackPrenmi);
	 }
	 else if (osTvType == OS_TV_MPAL )
		osViSetMode(&osViModeTable[OS_VI_MPAL_HAN1]); 
	else
		osViSetMode(&osViModeTable[OS_VI_NTSC_HAN1]); 
	osViSetSpecialFeatures(OS_VI_GAMMA_OFF);
	
	/* Since osViBlack becomes FALSE when the VI mode is changed, */
	/* set the screen display to OFF again. 					*/
	nuGfxDisplayOff();

	/* Set the frame buffer address */
	nuGfxSetCfb(HighFrameBuf, 2);
	
	/* Set the Z buffer address 
	We won't use it, but I prefer to set it somewhere it won't hurt if for any reason it comes to be used inadvertently
	*/
	nuGfxSetZBuffer((u16*)(ZBUFFER_ADDR));
	
	if (InitHeap(mem_heap, sizeof(mem_heap)) == -1)
        return;

	/* Game main */
	//controllerPakInit();
	
	osSetTime(0);
	/*
	while (OS_CYCLES_TO_USEC(osGetTime())/1000000 < 10) {
	}*/
	
	while(1){
		nuGfxTaskAllEndWait();
		nuGfxFuncSet(callbackControllerPak);
		nuGfxDisplayOn();
	}
}

/*----------------------------------------------------------------------*/
/*	setupHigh - Set high resolution. 			  				*/
/*	IN:	Nothing in particular. 								*/
/*	RET:	None												*/
/*----------------------------------------------------------------------*/


void callbackPrenmi()
{
	nuGfxDisplayOff();
	osViSetYScale(1);
}

void callbackControllerPak(u32 taskNum) {
	/* Do not process if the tasks are not finished.  Any reference to "nuGfxTaskSpool" MUST be inside a nuGfxFuncSet callback function. Otherwise hardware crashes */ 
	if(nuGfxTaskSpool) return;
	RCPInit();
	
	drawTitle();
	
	RCPEnd();
}
