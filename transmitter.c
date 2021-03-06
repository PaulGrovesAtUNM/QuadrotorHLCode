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

void initFrame(QUADFRAME *frame, unsigned char command, unsigned char count, signed short *data)
{
	int i;
	frame->StartOfFrame = SOF;
	frame->EndOfFrame = EOFm;
	frame->cmd = command;
	frame->count = count;
	
	//memcpy( frame->data, data, 12);
	// ensures the data goes into Network Short format
	frame->data[0] =  (unsigned char)((data[0] & 0xFF00 ) >> 8 ); 
	frame->data[1] =  (unsigned char)((data[0] & 0x00FF ));
	frame->data[2] =  (unsigned char)((data[1] & 0xFF00 ) >> 8 );
	frame->data[3] =  (unsigned char)((data[1] & 0x00FF ));
 	frame->data[4] =  (unsigned char)((data[2] & 0xFF00 ) >> 8 ); 
	frame->data[5] =  (unsigned char)((data[2] & 0x00FF ));
	frame->data[6] =  (unsigned char)((data[3] & 0xFF00 ) >> 8 );
	frame->data[7] =  (unsigned char)((data[3] & 0x00FF ));
 	frame->data[8] =  (unsigned char)((data[4] & 0xFF00 ) >> 8 ); 
	frame->data[9] =  (unsigned char)((data[4] & 0x00FF ));
	frame->data[10] = (unsigned char)((data[5] & 0xFF00 ) >> 8 );
	frame->data[11] = (unsigned char)((data[5] & 0x00FF ));
}

void transmitterInit(void)
{

}

// This should only be called from the timer interrupt
void transmitter(void) // Sends the current frame.
{
	unsigned char *nb;
	int i;

	if ( ( U0LSR & 0x20 ) == 0) // Return if transmitter is not empty.
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
