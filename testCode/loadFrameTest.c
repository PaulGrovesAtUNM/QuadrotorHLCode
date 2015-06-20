#include <stdio.h>
#include "RingBuffer.h"
#include "loadFrame.h"
#include "quadCommands.h"

RING_BUFFER u0r;

int main()
{

	int frame[17] = {SOF, SOF, 0x03, 0x03, 0x03, 0x03, 0x03, EOFm, EOFm,
					SOF, 0x03, 0x03, 0x03, 0x03, 0x03, 0x03, EOFm };

	int i;

	RBInit(&u0r);
	for (i = 0; i < 17; i++)
		RBEnqueue(&u0r, frame[i]);

	printf("Result: %i\n\r", loadFrame() );
	printf("Done.\n\r");
}

		
