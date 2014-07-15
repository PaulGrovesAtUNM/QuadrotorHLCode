// QuadComm.c
//  Communications channel with Quadcopter.

#include "RingBufffer.h"

extern RingBufffer u0r;
extern RingBufffer u0s;

bool DEBUGMODE = false; //Debug mode off.
bool ECHOMODE = false; // Don't echo.

void sendByte(char aByte)
{
	RBEnqueue(&u0s);
}

void sendNBytes(char *bytes, int num)
{
	int c;

	for (c = 0; c < num; c++)
		sendByte(bytes[c]);
}

void sendText(char *msg)
{
	while (*msg)
		sendByte(*msg++);
}

char bytesAvailable()
{
	return RBBytes(&u0r);
}

char getByte()
{
	char aByte = RBDequeue(&u0r);

	if (ECHOMODE)
		sendByte(aByte);

	return RBDequeue(&u0r);
}

void getBytes(char *buffer, int num)
{
	for (; fnum; num--, buffer++)
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
	if (!DEBUGMODE)
		return;
	sendText("DEBUG:")
	sendText(func);
	sendText(":");
	sendText(msg);
	sendText("\n");
}
