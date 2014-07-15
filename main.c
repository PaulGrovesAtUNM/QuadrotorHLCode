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

/**********************************************************
                  Header files
 **********************************************************/
#include "LPC214x.h"
#include "main.h"
#include "system.h"
#include "uart.h"
#include "mymath.h"
#include "hardware.h"
#include "irq.h"
#include "i2c.h"
#include "i2c1.h"
#include "gpsmath.h"
#include "adc.h"
#include "uart.h"
#include "ssp.h"
#include "LL_HL_comm.h"
#include "sdk.h"
#include "buzzer.h"
#include "ublox.h"
#include "declination.h"
#include "lpc_aci_eeprom.h"

/* *********************************************************
               Function declarations
  ********************************************************* */

void Initialize(void);
void feed(void);
void beeper(unsigned char);

/**********************************************************
                  Global Variables
 **********************************************************/
struct HL_STATUS HL_Status;
struct IMU_CALCDATA IMU_CalcData, IMU_CalcData_tmp;

volatile unsigned int int_cnt=0, cnt=0, mainloop_cnt=0;
volatile unsigned char mainloop_trigger=0;
volatile unsigned int trigger_cnt=0;
volatile char SYSTEM_initialized=0;

unsigned char DataOutputsPerSecond=10;
unsigned short mainloop_overflows=0;

extern char updated;
char newvals = 0;

void timer0ISR(void) __irq
{
  T0IR = 0x01;      //Clear the timer 0 interrupt

  IENABLE;

  // Performance Counter
  trigger_cnt++;
  if (trigger_cnt == ControllerCyclesPerSecond)
  {
  	trigger_cnt=0;
  	HL_Status.up_time++;
  	HL_Status.cpu_load=mainloop_cnt;

  	mainloop_cnt=0;
  }

  if ( mainloop_trigger < 10 ) 
    mainloop_trigger++;

  IDISABLE;
  VICVectAddr = 0;		// Acknowledge Interrupt
}

/**********************************************************
                       MAIN
**********************************************************/
int	main (void) {

  static int vbat1; //battery_voltage (lowpass-filtered)
  unsigned int TimerT1, TimerT2;

  init();
  buzzer(OFF);
  LL_write_init();

  //initialize AscTec Firefly LED fin on I2C1 (not necessary on AscTec Hummingbird or Pelican)
  I2C1Init();
  I2C1_setRGBLed(255,0,0);

  ADC0triggerSampling(1<<VOLTAGE_1); //activate ADC sampling

  generateBuildInfo();

  HL_Status.up_time=0;

  LED(1,ON);

  //update parameters stored by ACI:
  //...

  while(1)
  {
      if(mainloop_trigger)
      {
      	TimerT1 =  T0TC;

        //battery monitoring
        ADC0getSamplingResults(0xFF,adcChannelValues);
        vbat1=(vbat1*14+(adcChannelValues[VOLTAGE_1]*9872/579))/15;	//voltage in mV

		    HL_Status.battery_voltage_1=vbat1;
        mainloop_cnt++;
		    if(!(mainloop_cnt % 10)) 
          buzzer_handler(HL_Status.battery_voltage_1);

        if(mainloop_trigger) 
          mainloop_trigger--;
        mainloop();

        // CPU Usage calculation
        TimerT2 = T0TC;
        if (mainloop_trigger)
        {
        	HL_Status.cpu_load = 1000;
        	mainloop_overflows++;
        }
        else if (TimerT2 < TimerT1)
        	HL_Status.cpu_load = (T0MR0 - TimerT1 + TimerT2)*1000/T0MR0; // load = "timer cycles" / "timer cycles per controller cycle" * 1000
        else
        	HL_Status.cpu_load = (TimerT2 - TimerT1)*1000/T0MR0; // load = "timer cycles" / "timer cycles per controller cycle" * 1000
      }
  }
  return 0;
}


void mainloop(void) //mainloop is triggered at 1 kHz
{

	//run SDK mainloop. Please put all your data handling / controller code in sdk.c
	SDK_mainloop();

  //write data to transmit buffer for immediate transfer to LL processor
  HL2LL_write_cycle();
}