#include <iostream>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
// PFG

using namespace std;
                                         // (or libViconDataStreamSDK_CPP.dll for Windows)
#include "setMotorSpeeds.h"


int main(int argc, char **argv)
{
	// Control Variable Setup
	double Omegas[4] = {0,0,0,0};

	// State Variable Setup
	
	// Opening XBee Comm Port
	printf("Configuring the XBee's port...");
	system("stty -F /dev/ttyUSB0 57600 raw");
	printf("Waiting for configuration to take effect...\n\r");
	sleep(0.005);	// sleeps for 2 seconds to let the command take effect
	
	printf("Opening the XBee's port...\n\r");
	FILE *fp;
	fp = fopen("/dev/ttyUSB0","rw+");
	
	if (!fp) {
		printf("Could not open XBee port!\n");
		exit(-1);
	}
	setvbuf(fp, NULL, _IONBF,0); // No buffering
	
	Omegas[1] = 150;

	/*** SEND INPUT ***/
	cout << "Setting motor speeds..." << endl;
	setMotorSpeeds(fp,Omegas);
	cin.ignore(1024, '\n');
	cout << "Press enter to continue..." << endl;
	cin.get();
	Omegas[0] = 0;
	Omegas[1] = 0;
	Omegas[2] = 0;
	Omegas[3] = 0;
	setMotorSpeeds(fp, Omegas);
	setMotorSpeeds(fp, Omegas); // used three times in case of character loss
	setMotorSpeeds(fp, Omegas);
	fclose(fp);
  return 0;
}
