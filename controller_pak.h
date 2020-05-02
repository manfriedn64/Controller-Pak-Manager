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


void drawTitle();
void controllerPakInit();
void readControllerPaks();

#endif /* _CONTROLLERPAK_H_*/