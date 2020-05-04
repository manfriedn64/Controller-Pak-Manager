#
# 	NuSystem highreso Program 
#
#	Copyright (C) 1997, NINTENDO Co,Ltd.
#

include $(ROOT)/usr/include/make/PRdefs

#
# NUSYS LIBRARY
#
#
N64KITDIR    = c:\nintendo\n64kit
NUSYSINCDIR  = $(N64KITDIR)/nusys/include
NUSYSLIBDIR  = $(N64KITDIR)/nusys/lib
NUSTDINCDIR  = $(N64KITDIR)/nustd/include
NUSTDLIBDIR  = $(N64KITDIR)/nustd/lib

NUAUDIOLIB = -lnualstl_n_d -ln_gmus_d -ln_gaudio_sc

APP 		=	controller_pak.out

OPTIMIZER 	=	-g

TARGETS 	=	controller_pak.n64

TEXHFILES 	=

HFILES 		= 	main.h

CODEFILES   	= 	main.c graphics.c controller_pak.c 2dlibrary.c font.c font_tahoma.c user_interface.c

CODEOBJECTS	=	$(CODEFILES:.c=.o) $(NUSYSLIBDIR)/nusys.o

DATAFILES   	=

DATAOBJECTS	=	$(DATAFILES:.c=.o)

CODESEGMENT	=	codesegment.o

OBJECTS		=	$(CODESEGMENT) $(DATAOBJECTS)
MAKEROM = mild


LCDEFS = 
LCINCS =	-I. -I$(NUSYSINCDIR) -I$(NUSTDINCDIR) -I$(ROOT)/usr/include/PR
LDFLAGS = $(MKDEPOPT) -L$(LIB) -L$(NUSYSLIBDIR) -L$(NUSTDLIBDIR) $(NUAUDIOLIB) -lnustd_d -lnusys_d -lgultra_rom -L$(GCCDIR)/mipse/lib -lkmc

LCOPTS =	-G 0
		
LDIRT  =	$(APP)

default:	$(TARGETS)

include $(COMMONRULES)

$(CODESEGMENT):	$(CODEOBJECTS)
		$(LD) -o $(CODESEGMENT) -r $(CODEOBJECTS) $(LDFLAGS)

$(TARGETS) $(APP):	spec $(OBJECTS)
		$(MAKEROM)  spec -I$(NUSYSINCDIR) -r $(TARGETS) -e $(APP) 
		MAKEMASK $(TARGETS)
