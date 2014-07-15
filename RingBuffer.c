// UARTData.c

#include <stdio.h>
#include "RingBuffer.h"

void RBInit(RING_BUFFER *aBuffer)
{
	aBuffer->EOB = 0;
	aBuffer->SOB = -1; //we increment first. 
	aBuffer->bytes = 0;
}

void RBEnqueue(RING_BUFFER *aBuffer, char x)
{
	if ( aBuffer->bytes >= BUFF_LEN )
	{	
		printf("Buffer FULL!\n");
		return; // Buffer full!
	}
	aBuffer->buffer[ aBuffer->EOB ] = x;
	aBuffer->EOB++;
	aBuffer->bytes++;
	if (aBuffer->EOB >= BUFF_LEN)
		aBuffer->EOB = 0;
}

char RBDequeue(RING_BUFFER *aBuffer)
{
	if ( aBuffer->bytes == 0 )
	{
		printf("Buffer Empty!\n");
		return 1; //Empty buffer!
	}

	aBuffer->bytes--;
	aBuffer->SOB++;
	if ( aBuffer->SOB >= BUFF_LEN)
		aBuffer->SOB = 0;

	return aBuffer->buffer[ aBuffer->SOB ];
}

int RBCount(RING_BUFFER *aBuffer)
{
	return aBuffer->bytes;
}
