#ifndef LOADFRAME_H
#define LOADFRAME_H

typedef struct FRAME 
{
	unsigned char prefix;	
	unsigned char command;
	unsigned char data[5];
	unsigned char suffix;
} FRAME;

char loadFrame(void);

#endif
