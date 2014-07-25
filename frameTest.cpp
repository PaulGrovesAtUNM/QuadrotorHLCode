#include <stdlib.h>
#include <stdio.h>

int main()
{
	FILE *fp;
	fp = fopen("/dev/ttyUSB0","rw+");
	char buff[50];
	int cr = 0;
	int x;

	unsigned char frame[8] = {234, 202, 0, 0, 0, 0, 0, 213};
	
	if (!fp) {
		printf("Could not open XBee port!\n");
		exit(-1);
	}
	setvbuf(fp, NULL, _IONBF,0); // No buffering

	printf("Version:");
	frame[1] = 202;
	
	fwrite(frame, 1, 8, fp);
	for (x = 0; x < 100; x++)
	{
		cr = fread(buff, 10, 1, fp);
		buff[cr] = 0;
		printf("%s", buff);
	}
	fclose(fp);
}
