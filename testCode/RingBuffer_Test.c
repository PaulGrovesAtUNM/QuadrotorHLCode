//UARTData Test

#include <stdio.h>
#include <stdlib.h>

#include "RingBuffer.h"

RING_BUFFER u0;

int main()
{
	printf("Starting...\n");
	RBInit(&u0);

	printf("Buffer initilized...\n");

	printf("Enqueuing some data...\n");
	RBEnqueue(&u0,'a');
	RBEnqueue(&u0,'b');

	printf("Size: %i\n", RBCount(&u0));
	printf("Dequeueing...\n");
	int sz = RBCount(&u0);

	for (int i = 0; i < sz; i++)
		printf("%i\n", RBDequeue(&u0));

	printf("\n\n");
	printf("Size: %i\n", RBCount(&u0));
}
