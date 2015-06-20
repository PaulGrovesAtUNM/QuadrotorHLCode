// UARTData.c

// #define DEBUGGING

#ifdef DEBUGGING
#include <stdio.h>
#endif

#include "RingBuffer.h"
#include "quadComm.h"

#ifndef DEBUGGING
#include "irq.h"
#endif

#include "uart.h";


void UART0Debug(char *msg, int length);

void RBInit(RING_BUFFER *aBuffer)
{
	aBuffer->EOB = 0;
	aBuffer->SOB = -1; //we increment first. 
	aBuffer->bytes = 0;
}

void showBuffer(RING_BUFFER *aBuffer)
{
	int i;
	char msg[25];

	return;
	for (i = 0; i < aBuffer->bytes; i++)
	{
		sprintf(msg, "Byte %i:%i\n\r", i, aBuffer->buffer[ aBuffer->SOB + i + 1]);
		sendText(msg);
	}
}
 
void RBEnqueue(RING_BUFFER *aBuffer, char x)
{
	if ( aBuffer->bytes >= BUFF_LEN )
	{
		//UART0Debug("1Buffer FULL!\n",13);
		return;
	}
	aBuffer->buffer[ aBuffer->EOB ] = x;
	aBuffer->EOB++;
	aBuffer->bytes++;
	if (aBuffer->EOB >= BUFF_LEN)
		aBuffer->EOB = 0;
}

char RBDequeue(RING_BUFFER *aBuffer)
{
	char aByte;
	
	if ( aBuffer->bytes == 0 )
	{
		//UART0Debug("RBDequeue: Buffer Empty!\n",25);
		return 0; //Empty buffer!
	}
	

	aBuffer->bytes--;
	aBuffer->SOB++;
	if ( aBuffer->SOB >= BUFF_LEN)
		aBuffer->SOB = 0;

	aByte = aBuffer->buffer[ aBuffer->SOB ];
	
	
	return aByte;
}

void RBDiscard(RING_BUFFER *aBuffer, char numToDiscard)
{
	int i;
	
	if ( aBuffer->bytes <= numToDiscard )
	{
		aBuffer->bytes = 0;
		aBuffer->EOB = 0;
		aBuffer->SOB = -1;		
	}
	else
		for ( i = 0; i < numToDiscard; i++)
			RBDequeue(aBuffer);
}

void RBPushBack(RING_BUFFER *aBuffer, char aByte)
{
	aBuffer->buffer[ aBuffer->SOB ] = aByte;
	aBuffer->SOB--;
	if ( aBuffer->SOB < 0 )
		aBuffer->SOB = BUFF_LEN - 1;
	aBuffer->bytes++;
}

char RBPeek(RING_BUFFER *aBuffer)
{
	int idxNByte;
	char aByte;
	
	if ( aBuffer->bytes == 0 )
		return 0;
	idxNByte = aBuffer->SOB + 1;
	
	if ( idxNByte >= BUFF_LEN )
		idxNByte = 0;
	aByte = aBuffer->buffer[ idxNByte ];
	return aByte;
}

// Searches the Ring Buffer for a specific character within searchLen characters of start.
//  Returns index if found, -1 if not.
int RBfindInBuffer(RING_BUFFER *aBuffer, int startIndex, int searchLen, unsigned char aByte)
{
	int x, idx;
	int hit = -1;

	idx = aBuffer->SOB + startIndex + 1;

	if ( idx >= BUFF_LEN )
		idx = idx - BUFF_LEN;
	
	for (x = 0; x < searchLen && hit == -1; x++)
	{			
		if ( aBuffer->buffer[idx] == aByte )
			hit = x;
		idx++;	
		if ( idx >= BUFF_LEN )
			idx = 0;
	}
	return hit; //Not found...	
}

int RBCount(RING_BUFFER *aBuffer)
{
	return aBuffer->bytes;
}
