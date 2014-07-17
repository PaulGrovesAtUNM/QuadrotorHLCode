/*

AscTec SDK 3.0

Copyright (c) 2011, Ascending Technologies GmbH
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.
 * Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE FOR ANY
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH
DAMAGE.

 */

#include "main.h"
#include "sdk.h"
#include "LL_HL_comm.h"
#include "gpsmath.h"
#include "sdk_telemetry.h"
#include "uart.h"
#include "system.h"
#include "lpc_aci_eeprom.h"

#include "RingBuffer.h"
#include "quadCommands.h"
#include "quadComm.h"
#include "uart.h"

struct WO_SDK_STRUCT WO_SDK;
struct WO_CTRL_INPUT WO_CTRL_Input;
struct RO_RC_DATA RO_RC_Data;
struct RO_ALL_DATA RO_ALL_Data;
struct WO_DIRECT_MOTOR_CONTROL WO_Direct_Motor_Control;
struct WO_DIRECT_INDIVIDUAL_MOTOR_CONTROL WO_Direct_Individual_Motor_Control;

//emergency mode variables
unsigned char emergencyMode;
unsigned char emergencyModeUpdate=0;

int SDK_EXAMPLE_turn_motors_on(void);
int SDK_EXAMPLE_turn_motors_off(void);

extern char newvals;

/******** SDK in general ************
 *
 * You can find further information about the AscTec SDK in our AscTec Wiki: http://wiki.asctec.de
 *
 * Scroll down for information about how to change the Eclipse settings to use the SDK with the AscTec Simulink Toolkit.
 *
 *
 * SDK_mainloop(void) is triggered @ 1kHz.
 *
 * RO_(Read Only) data is updated before entering this function
 * and can be read to obtain information for supervision or control
 *
 * WO_(Write Only) data is written to the LL processor after
 * execution of this function.
 *
 * WO_ and RO_ strfucts are defined in sdk.h
 *
 * The struct RO_ALL_Data (defined in sdk.h)
 * is used to read all sensor data, results of the data fusion
 * and R/C inputs transmitted from the LL-processor. This struct is
 * automatically updated at 1 kHz.
 *
 */

/******* How to flash the high level processor ********
 *
 * The easiest way to get your code on the high level processor is to use the JTAG-adapter.
 *
 * It needs three steps to get your code on the high level processor.
 * 1. Build your code ("Build Project")
 * 2. Connect your JTAG adapter and start the JTAG connection (Run "OpenOCD Asctec-JTAG")
 * 3. Flash the processor (Run "Asctec JTAG Debug")
 *
 * In the menu "Run -> External Tools -> External Tools Configuration..." you
 * will find "OpenOCD Asctec-JTAG". If the JTAG connection was activated
 * correctly, the console will show only the following line:
 * "Info:    openocd.c:92 main(): Open On-Chip Debugger (2007-08-10 22:30 CEST)"
 *
 * Do not launch more than ONE JTAG-connection at the same time!
 *
 * In the menu "Run -> Debug Configurations..." you will find "Asctec JTAG Debug"
 * If the code was successfully flashed on the processor, the program will switch
 * to the Debug window.
 *
 * If you want to flash the high level processor using a serial interface (AscTec USB adapter)
 * and bootloader software like "Flash Magic", you can find the main.hex in your workspace folder.
 *
 */

/********* Debugging and testing your code ************
 *
 * After flashing the HL, your code can be debugged online via JTAG. The ARM7 supports ONE hardware breakpoint.
 * You can monitor the CPU-load by looking at RO_ALL_Data.HL_cpu_load. As long as this value is below 1 your
 * code in SDK_mainloop() is executed at 1 kHz. Example: 0.021 means the HL code uses 21% of the available capacity.
 *
 * To activate the SDK controls, the serial interface switch on your R/C (channel 5) needs to be in ON position.
 * If you use our standard RC Futaba FF7, the black marked switch on the right hand side needs to be pulled towards
 * the pilot to enable the serial interface.
 *
 * To test if the flashing of the HLP worked and how to enable the serial interface, you can run the example "turn
 * motors on and off every 2 seconds" in the SDK_mainloop.
 *
 */

/********** Serial communication **********
 *
 * If your project needs communication via HL serial 0 port, the easiest way is to use the AscTec Communication Interface
 * (information and tutorials can be found in the AscTec Wiki) -
 * or you can directly program the serial port (you find an example of how to do so in main.c, line 284).
 *
 */


/********** Emergency Modes ************
 *
 * Now you can set the Emergency Modes directly from the HLP. For more information about the EMs, please have a
 * look at the AscTec Wiki. Please set an emergency mode according to the flight path of your flight mission
 * with SDK_SetEmergencyMode(). If non was set, Direct Landing is activated.
 */

extern char updated;
extern char DEBUG_ENABLED;
int loops;
int firstime = 0;
int motors[4] = {0,0,0,0};

void SDK_mainloop(void)
{
	int i;
	char dbgMsg[50];
	char hdr; //Header character.
	// If <=200 it's a direct motor command.
	//  Else it's a command command.

	if (firstime == 0)
	{
		LED(0,ON);
		firstime = 1;
	}

	loops++;
	if (loops > 2000)
	{
		loops = 0;
		sprintf(dbgMsg,"Bytes: %i Motors: %i,%i,%i,%i\n\r", bytesAvailable(), 
			motors[0], motors[1], motors[2], motors[3]);
		sendText(dbgMsg);
	}

	if ( bytesAvailable() < 4 )
		return; //Nothing to dFo.

	hdr = getByte();
	if ( hdr <= 200 )
	{ //Direct motor commands sent.

		WO_SDK.ctrl_mode=0x00;	//0x00: direct individual motor control: individual commands for motors 0..3
								//0x01: direct motor control using standard output mapping: commands are interpreted as pitch, roll, yaw and thrust inputs; no attitude controller active
								//0x02: attitude and throttle control: commands are input for standard attitude controller
								//0x03: GPS waypoint control

		WO_SDK.ctrl_enabled=1;  //0: disable control by HL processor
								//1: enable control by HL processor

		WO_SDK.disable_motor_onoff_by_stick = 0;

		motors[0] = hdr;
		WO_Direct_Individual_Motor_Control.motor[0] = hdr;
		for (i = 1; i < 4; i++ )
		{
			motors[i] = getByte();
			WO_Direct_Individual_Motor_Control.motor[i] = motors[i];
		}
		WO_Direct_Individual_Motor_Control.motor[4] = 0;
		WO_Direct_Individual_Motor_Control.motor[5] = 0;
	} else {
		switch ((int)hdr)
		{
			case VERSION:
				skipBytes(3); // Remove the 3 filler chars.
				sendText( QUAD_VERSION );
			break;
			case DEBUGMODE:
				DEBUG_ENABLED = ( getByte() != 0);
				skipBytes(2);
			break;			
		}

	}
}	


/*
 *
 * Sets emergency mode on LowLevel processor. Select one of the EM_ defines as mode option. See EM_ defines for details
 */

void SDK_SetEmergencyMode(unsigned char mode) {

	return;
	if ((mode != EM_SAVE_EXTENDED_WAITING_TIME) && (mode != EM_SAVE) && (mode
			!= EM_RETURN_AT_MISSION_SUMMIT) && (mode
			!= EM_RETURN_AT_PREDEFINED_HEIGHT))
		return;
	emergencyMode = mode;
	emergencyModeUpdate = 1;
}

