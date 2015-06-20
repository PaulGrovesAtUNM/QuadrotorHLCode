#include "setMotorSpeeds.h"

// Pre-defined constant globals: RADS_TO_RPM

/*** SET MOTOR SPEEDS ***/
// This function is used for sending direct motor control comands to the quadrotor.
void setMotorSpeeds(FILE *fp, double speeds[4])
{
	int i;
	double rpms[4];
	unsigned char swap; // Motor 1 = 1st, 2 = 3rd, 3 = 2nd, 4th = 4
	unsigned char frame[8] = {234, 201, 0, 0, 0, 0, 0, 213};
	

	for (i = 0; i < 4; i++)
	{

		// Convert from radians per second to RPM
		// 60 Sec / Min, 2PI Radians per Revolution
		//rpms[i] = (speeds[i] * 60) / (2.0 * 3.1415926); 
		rpms[i] = speeds[i]*RADS_TO_RPM;
		
		// Now convert from RPM's to Direct Motor Commands.
		if ( rpms[i] < 1075 )
			frame[i+2] = 0;
		// From sdk.h in ASCTEC 3.0 SDK: RPM = (25+(motor*175)/200)*43
		else 
			frame[i+2] = (int)((((double)rpms[i]/43.0)-25.0)*(200.0/175.0)); //8600 rpm is max

		if ( frame[i+2] > 150)
		{
			//printf("Waring! Motor %i Direct Motor Control commanded to be greater than 200! [D:%i, S:%f, R:%f]\n", i + 1, dmc[i],speeds[i], rpms[i] );
			frame[i+2] = 150;
		}
		
	}

	swap = frame[3];
	frame[3] = frame[4];
	frame[4] = swap; //Swap values to match our model.
	fwrite(frame, 1, 8, fp);

}
