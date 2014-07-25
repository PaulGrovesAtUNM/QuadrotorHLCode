#include <stdio.h>
#include "RingBuffer.h"
#include "loadFrame.h"

RING_BUFFER u0r;

int main()
{

	int frame[8] = {0x01, 0x04, 0x03, 0x03, 0x03, 0x03, 0x03, 0x02};

	int i;

	RBInit(&u0r);

	for (i = 0; i < 8; i++)
		RBEnqueue(&u0r, frame[i]);

	printf("Result: %i\n\r", loadFrame() );
	printf("Done.\n\r");
}

		
