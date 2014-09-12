#include <string.h> // memcpy
#include <stdlib.h> // abs

#include "LPC214x.h"
#include "hardware.h" //LED
#include "quadComm.h"
#include "quadCommands.h"
#include "transmitter.h"


QUADFRAME frames[2];
int currentFrame = 2;
int nextFrame = 2;
int nextByte = 0;

void initFrame(QUADFRAME *frame, unsigned char command, unsigned char count, uint32_t *data)
{
	frame->StartOfFrame = SOF;
	frame->EndOfFrame = EOFm;
	frame->cmd = command;
	frame->count = count;
	memcpy( frame->data, data, 12);
}

void transmitterInit(void)
{
	unsigned char data[12] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12};

	initFrame(frames, 1, 13, data); 
	currentFrame = 0;
	nextFrame = 1;
	nextByte = 0;
}

// This should only be called from the timer interrupt
void transmitter(void) // Sends the current frame.
{
	unsigned char *nb;
	int i;

	if ( ( U0LSR & 0x20 ) == 0 ) // Return if transmitter is not empty.
		return;

	if ( currentFrame == 2 ) // We don't have a current frame. 
	{
		if ( nextFrame == 2 ) //Do we have a next frame?
			return; //No frame ready
		currentFrame = nextFrame;
		nextFrame = 2; // Next frame empty
	}

	LED(0,1); //led 0 on.

	// Dump 16 characters into our buffer.
	nb = (unsigned char *)&frames[currentFrame];
	for (i = 0; i < 16; i++)
		U0THR = *nb++;
	
	LED(0, 0); //led 0 off.
	currentFrame = 2; //We don't have a ready frame.
}
				
void setFrame(QUADFRAME *data)  //Swaps the current frame and the next frame
{
	int temp = nextFrame;

	nextFrame = 2; // Make sure we don't try to send out this frame while we're working on it.

	temp = abs(1 - currentFrame); //1 - 0 = 1, 1 - 1 = 0, 1 - 2 = 1, valid for all states.

	memcpy( frames + temp, data, 16);
	
	nextFrame = temp;
}	
