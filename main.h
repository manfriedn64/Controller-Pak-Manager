/*======================================================================*/
/*	File : localdef.h				                                    */
/*  included by almost all .c files, it defines our global variables    */
/*======================================================================*/  
#include <nualstl_n.h>

#ifndef _MAIN_H_
#define _MAIN_H_

#define	ZBUFFER_ADDR	(0x80400000-640*480*2*3)
#define CFB_HIGH_ADDR0	(0x80400000-640*480*2*2)
#define CFB_HIGH_ADDR1	(0x80400000-640*480*2*1)

/*
 *  Macro for extern declaration of WAVE data segment
 */
#define EXTERN_SEGMENT(name)    \
extern  char    _##name##SegmentStart[], _##name##SegmentEnd[], \
                _##name##SegmentRomStart[], _##name##SegmentRomEnd[]
				
#define EXTERN_SEGMENT_U8(name)    \
extern  u8      _##name##SegmentStart[], _##name##SegmentEnd[], \
                _##name##SegmentRomStart[], _##name##SegmentRomEnd[]

/* this is to know where the segments are in ROM (check initGame() or initExpansionPak() for example) */
EXTERN_SEGMENT(code);
EXTERN_SEGMENT(tahoma_bold);
EXTERN_SEGMENT(tahoma_normal);
EXTERN_SEGMENT(controllers);

/* */
u64 time_lastframe;
/* Counter for how many frames were drawn */
int frame_number;
/* Buffer for console display */
char conbuf[40];
/* storing the amount of RAM available. Shared because calling osGetMemSize() clears memory in the expansion pak, so it needs to be done early in the code */
u32 memory_size;

u8 contPattern;

#endif /* _MAIN_H_*/
