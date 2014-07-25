#ifndef LOADFRAME_H
#define LOADFRAME_H

typedef struct FRAME 
{
	char prefix;	
	char command;
	char data[5];
	char suffix;
} FRAME;

char loadFrame(void);

#endif
