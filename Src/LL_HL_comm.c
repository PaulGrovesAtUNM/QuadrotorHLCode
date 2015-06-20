/*

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
#include "LL_HL_comm.h"
#include "system.h"
//#include "gpsmath.h"
#include "sdk.h"
//#include "declination.h"
//#include "jetiTelemetry.h"
//#include "sdk_telemetry.h"
#include "buildInfoSetup.h"
#include <string.h>

unsigned short SSP_ack = 0;
extern char SPIWRData[128];
extern char data_sent_to_HL;
extern char data_sent_to_LL;
extern unsigned int SPIWR_num_bytes;

struct LL_ATTITUDE_DATA LL_1khz_attitude_data;
struct LL_CONTROL_INPUT LL_1khz_control_input;

unsigned char wpCtrlWpCmd = 0;
unsigned char wpCtrlWpCmdUpdated = 0;

unsigned char wpCtrlAckTrigger = 0;

unsigned short wpCtrlNavStatus = 0;
unsigned short wpCtrlDistToWp = 0;

struct WAYPOINT wpToLL;

volatile unsigned char transmitBuildInfoTrigger = 0;

void SSP_data_distribution_HL(void)
{
    unsigned char i;
    unsigned char current_page = LL_1khz_attitude_data.system_flags & 0x03;
    static unsigned char oldKey = 0;

 //   if (LL_1khz_attitude_data.system_flags & SF_GPS_NEW)
 //       gpsDataOkTrigger = 0;



}

int HL2LL_write_cycle(void) //write data to low-level processor
{
    static char pageselect = 0;
    unsigned char i;
    static unsigned char jetiValuePartialSyncPending = 0;
    static unsigned char * transmitPtr;
    static unsigned short transmitCnt = 0;

    if (!data_sent_to_LL)
        return (0);

    //update 1kHz data
    LL_1khz_control_input.system_flags = 0 | pageselect;
    //SSP_ack=0;	//reset ack

   // if (gpsDataOkTrigger)
    //    LL_1khz_control_input.system_flags |= SF_GPS_NEW;

    if (WO_SDK.ctrl_enabled)
        LL_1khz_control_input.system_flags |= SF_HL_CONTROL_ENABLED
                                              | SF_NEW_SDK;
    else
        LL_1khz_control_input.system_flags &= ~(SF_HL_CONTROL_ENABLED
                                                | SF_NEW_SDK);

    if (WO_SDK.disable_motor_onoff_by_stick)
        LL_1khz_control_input.system_flags
        |= SF_SDK_DISABLE_MOTORONOFF_BY_STICK;
    else
        LL_1khz_control_input.system_flags
        &= ~SF_SDK_DISABLE_MOTORONOFF_BY_STICK;

    if (WO_SDK.ctrl_mode == 0x00) //direct individual motor control
    {
        LL_1khz_control_input.system_flags
        |= SF_DIRECT_MOTOR_CONTROL_INDIVIDUAL;
        for (i = 0; i < 8; i++)
        {
            LL_1khz_control_input.direct_motor_control[i]
                = WO_Direct_Individual_Motor_Control.motor[i];
            LL_1khz_control_input.ctrl_flags=WO_Direct_Individual_Motor_Control.motorReverseMask<<8;
        }
    }

    if (pageselect == 0)
    {
        //write data
        LL_write_ctrl_data(pageselect);
        //set pageselect to other page for next cycle
        pageselect = 1;
    }
    else   //pageselect=1
    {
        //write data
        LL_write_ctrl_data(pageselect);
        //set pageselect to other page for next cycle
        pageselect = 0;
    }
    return (1);
}

void LL_write_ctrl_data(char page)
{
    unsigned int i;
    unsigned char *dataptr;
    static volatile short spi_chksum;

    dataptr = (unsigned char *) &LL_1khz_control_input;

    //initialize syncbytes
    SPIWRData[0] = '>';
    SPIWRData[1] = '*';

    spi_chksum = 0xAAAA;

    if (!page)
    {
        for (i = 2; i < 40; i++)
        {
            SPIWRData[i] = *dataptr++;
            spi_chksum += SPIWRData[i];
        }
    }
    else
    {
        for (i = 2; i < 22; i++)
        {
            SPIWRData[i] = *dataptr++;
            spi_chksum += SPIWRData[i];
        }
        dataptr += 18;
        for (i = 22; i < 40; i++)
        {
            SPIWRData[i] = *dataptr++;
            spi_chksum += SPIWRData[i];
        }
    }

    SPIWRData[40] = spi_chksum; //chksum LSB
    SPIWRData[41] = (spi_chksum >> 8); //chksum MSB

    SPIWR_num_bytes = 42;
    data_sent_to_LL = 0;
}

inline void SSP_rx_handler_HL(unsigned char SPI_rxdata) //rx_handler @ high-level processor
{
    static volatile unsigned char SPI_syncstate = 0;
    static volatile unsigned char SPI_rxcount = 0;
    static volatile unsigned char *SPI_rxptr;
    static volatile unsigned char incoming_page;

    //receive handler
    if (SPI_syncstate == 0)
    {
        if (SPI_rxdata == '>')
            SPI_syncstate++;
        else
            SPI_syncstate = 0;
    }
    else if (SPI_syncstate == 1)
    {
        if (SPI_rxdata == '*')
        {
            SPI_syncstate++;
            SPI_rxptr = (unsigned char *) &LL_1khz_attitude_data;
            SPI_rxcount = 40;
        }
        else
            SPI_syncstate = 0;
    }
    else if (SPI_syncstate == 2)
    {
        if (SPI_rxcount == 26) //14 bytes transmitted => select 500Hz page
        {
            incoming_page = LL_1khz_attitude_data.system_flags & 0x03; //system flags were already received
            if (incoming_page == 1)
                SPI_rxptr += 26;
            else if (incoming_page == 2)
                SPI_rxptr += 52;
        }
        SPI_rxcount--;
        *SPI_rxptr = SPI_rxdata;
        SPI_rxptr++;
        if (SPI_rxcount == 0)
        {
            SPI_syncstate++;
        }
    }
    else if (SPI_syncstate == 3)
    {
        if (SPI_rxdata == '<') //last byte ok => data should be valid
        {
            SSP_data_distribution_HL(); //only distribute data to other structs, if it was received correctly
            //ack data receiption
        }
        SPI_syncstate = 0;
    }
    else
        SPI_syncstate = 0;
}

