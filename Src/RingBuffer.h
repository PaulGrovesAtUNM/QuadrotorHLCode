#ifndef RINGBUFFER_H
#define RINGBUFFER_H

// RingBuffer.h
//  Ring buffer for UARTs.

#define BUFF_LEN 150

typedef volatile struct
{
    int EOB; //Index to End of Buffer.
    int SOB; //Index to the Start of the Buffer
    unsigned char bytes; //Bytes available in the buffer.
    unsigned char buffer[ BUFF_LEN ];
} RING_BUFFER;

void RBInit(RING_BUFFER *aBuffer);
void RBEnqueue(RING_BUFFER *aBuffer, char x);
char RBDequeue(RING_BUFFER *aBuffer);
char RBPeek(RING_BUFFER *aBuffer);
int RBCount(RING_BUFFER *aBuffer);
void RBPushBack(RING_BUFFER *aBuffer, char aByte);
int RBfindInBuffer(RING_BUFFER *aBuffer, int startIndex, int searchLen, char aByte);
void RBDiscard(RING_BUFFER *aBuffer, char numToDiscard);
void showBuffer(RING_BUFFER *aBuffer);
#endif
