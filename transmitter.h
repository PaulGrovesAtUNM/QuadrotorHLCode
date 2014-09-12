#ifndef TRANSMITTER_H
#define TRANSMITTER_H
#include <stdint.h> // uint32_t
typedef struct QUADFRAME
{
	unsigned char StartOfFrame;
	unsigned char cmd;
	uint32_t data[3];
	unsigned char count;
	unsigned char EndOfFrame;
} QUADFRAME;

void transmitterInit(void);
void transmitter(void); // Sends the current frame.
void setFrame(QUADFRAME *data);  //Swaps the current frame and the next frame

void initFrame(QUADFRAME *frame, unsigned char command, unsigned char count, uint32_t  *data);
#endif


