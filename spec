/*======================================================================*/
/*		NuSYS hello program					*/
/*		main.c							*/
/*									*/
/*		Copyright (C) 1997, NINTENDO Co,Ltd.			*/
/* 97/09/23	Created by Kensaku Ohki(SLANP)				*/
/*======================================================================*/  
#include "nusys.h"

beginseg
	name	"code"
	flags	BOOT OBJECT
	entry 	nuBoot
	address 0x80000400
	/*stack	nuMainStack + 0x8000
	address NU_SPEC_BOOT_ADDR*/
	stack	NU_SPEC_BOOT_STACK
	include "codesegment.o"
	include "$(ROOT)/usr/lib/PR/rspboot.o"
	include "$(ROOT)/usr/lib/PR/aspMain.o"
	include "$(ROOT)/usr/lib/PR/n_aspMain.o"
	include "$(ROOT)/usr/lib/PR/gspF3DEX2.fifo.o"
	include "$(ROOT)/usr/lib/PR/gspL3DEX2.fifo.o"
	include "$(ROOT)/usr/lib/PR/gspF3DEX2.Rej.fifo.o"
        include "$(ROOT)/usr/lib/PR/gspF3DEX2.NoN.fifo.o"
        include "$(ROOT)/usr/lib/PR/gspF3DLX2.Rej.fifo.o"
	include "$(ROOT)/usr/lib/PR/gspS2DEX2.fifo.o"
	include "$(ROOT)/usr/lib/PR/aspMain.o"
	include "$(ROOT)/usr/lib/PR/n_aspMain.o"
endseg


beginseg
	name    "tahoma_normal"
	flags   RAW
	include "assets/fonts/tahoma_normal.551"
endseg
beginseg
	name    "tahoma_bold"
	flags   RAW
	include "assets/fonts/tahoma_bold.551"
endseg

beginseg
	name    "controllers"
	flags   RAW
	include "assets/sprites/controllers.551"
endseg


beginwave
	name	"controllerpak"
	include	"code"
	include "tahoma_normal"
	include "tahoma_bold"
	include "controllers"
endwave
