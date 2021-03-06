#ifndef _CONTROLLERPAK_H_
#define _CONTROLLERPAK_H_

typedef struct {
	short	pak_number;
	short	file_number;
} SelectPakFile;

typedef struct {
	int		last_button;
	u64		last_time;
} ControllerManager;

typedef struct {
	u8				visible;
	int				selected;
	SelectPakFile 	file;
} MenuManager;



void drawTitle();
void controllerPakInit();
void readControllerPaks();


#endif /* _CONTROLLERPAK_H_*/