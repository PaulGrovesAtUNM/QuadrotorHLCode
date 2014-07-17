// QuadComm.c
//  Communications channel with Quadcopter.
#include "quadComm.h"
#include "RingBuffer.h"



extern RING_BUFFER u0r;
extern RING_BUFFER u0s;
#define FALSE 0;
#define TRUE 1;

int DEBUG_ENABLED = FALSE; //Debug mode off.
int ECHOMODE = FALSE; // Don't echo.

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
	uart0Prime();
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

void skipBytes(int num)
{
	int i;

	for (i = 0; i < num; i++)
		RBDequeue(&u0r);
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
