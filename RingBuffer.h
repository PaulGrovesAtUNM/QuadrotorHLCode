#ifndef RINGBUFFER_H
#define RINGBUFFER_H

// RingBuffer.h
//  Ring buffer for UARTs.

#define BUFF_LEN 50

typedef struct
{
	unsigned char EOB; //Index to End of Buffer.
	unsigned char SOB; //Index to the Start of the Buffer
	unsigned char bytes; //Bytes available in the buffer.
	unsigned char buffer[ BUFF_LEN ];
} RING_BUFFER;

void RBInit(RING_BUFFER *aBuffer);
void RBEnqueue(RING_BUFFER *aBuffer, char x);
char RBDequeue(RING_BUFFER *aBuffer);
int RBCount(RING_BUFFER *aBuffer);

#endif
