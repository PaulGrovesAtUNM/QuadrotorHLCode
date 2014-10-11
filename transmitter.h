#ifndef TRANSMITTER_H
#define TRANSMITTER_H
#include <stdint.h> // uint32_t
typedef struct QUADFRAME
{
	unsigned char StartOfFrame;
	unsigned char count;
	unsigned char data[12];
	unsigned char cmd;
	unsigned char EndOfFrame;
} QUADFRAME;

void transmitterInit(void);
void transmitter(void); // Sends the current frame.
void setFrame(QUADFRAME *data);  //Swaps the current frame and the next frame

void initFrame(QUADFRAME *frame, unsigned char command, unsigned char count, signed int  *data);
#endif


