// QuadComm.c
//  Communications channel with Quadcopter.
#include "quadComm.h"
#include "quadCommands.h"
#include "RingBuffer.h"
#include "uart.h"


extern RING_BUFFER u0r;
extern RING_BUFFER u0s;
#define FALSE 0;
#define TRUE 1;

int DEBUG_ENABLED = FALSE; //Debug mode off.
int ECHOMODE = FALSE; // Don't echo.

void showRBuffer(void)
{
	showBuffer(&u0r);
}

void loadVersion(signed short tdata[])
{
	int i;	
	char version[12] = QUAD_VERSION;

	for(i = 0; i < 6; i++) {
		tdata[i] = (unsigned short)version[2*i] << 8;
		tdata[i] = tdata[i] | (unsigned short)version[2*i + 1]; 
	}
}

void sendByte(char aByte)
{
	UART0WriteChar(aByte);
	RBEnqueue(&u0s, aByte);
}

void sendNBytes(char *bytes, int num)
{
	int c;

	for (c = 0; c < num; c++)
		UART0WriteChar(bytes[c]);
}

void sendText(char *msg)
{
	while (*msg)
		RBEnqueue(&u0s,*msg++);
//	uart0Prime();
}

char bytesAvailable(void)
{
	return RBCount(&u0r); //Groups of 4...
}

char getByte(void)
{
	char aByte;

	if ( !RBCount(&u0r) )
		return 0;

 	aByte = RBDequeue(&u0r);

	if (ECHOMODE)
		sendByte(aByte);

	return aByte;
}

void getBytes(char *buffer, int num)
{
	for (; num; num--, buffer++)
		*buffer = getByte();
}

// Pushes a byte back onto the Ring Buffer.
void pushBack(char byte)
{
	RBPushBack(&u0r, byte);
}

// skipBytes
//	Discard num bytes in the buffer.
int skipBytes(int num)
{
	RBDiscard(&u0r, num);
	return RBCount(&u0r);
}

// peekByte
//  Returns the next byte in the buffer w/o consuming it.
char peekByte()
{
	return RBPeek(&u0r);
}

// Searches the buffer, starting at startIndex, for aByte.
//  Returns the index in the buffer (from 0) the byte first occurs at.
//  Returns -1 if there is no occurrence.
int findInBuffer(int startIndex, int distance, char aByte)
{
	return RBfindInBuffer(&u0r, startIndex, distance, aByte); 
}

void debugMsg(char *func, char *msg)
{
	if (!DEBUG_ENABLED)
		return;
	sendText("DEBUG:");
	sendText(func);
	sendText(":");
	sendText(msg);
	sendText("\n");
}
