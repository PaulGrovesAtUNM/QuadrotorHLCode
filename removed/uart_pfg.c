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

#include "LPC214x.h"
#include "system.h"
#include "main.h"
#include "uart.h"
#include "irq.h"
#include "hardware.h"
#include "gpsmath.h"
#include "ssp.h"
#include "sdk.h"
#include "ublox.h"
//#include "asctecCommIntfOnboard.h"

//#include "ringbuffer.h"

unsigned char send_buffer[16];
unsigned char chksum_trigger;
unsigned char UART_CalibDoneFlag;
unsigned char trigger_transmission;
unsigned char transmission_running;

char debugChar = 'a';

char transmission1_running;
extern unsigned char m1, m2, m3, m4;
extern char updated;



// GPS Uart
void uart1ISR(void) __irq
{
  unsigned char t;
  IENABLE;
  unsigned iir = U1IIR;
  // Handle UART interrupt
  switch ((iir >> 1) & 0x7)
    {
      case 1: //TX
		  // THRE interrupt
//		    if ( !ringBufferEmpty(rb1Tx)  )
//		    {
 //         t = ringBufferGetByte(rb1Tx);
//		      transmission1_running=1;
//		      UART1WriteChar(t);
	//	    }
		//    else
		  //  {
		  //    transmission1_running=0;
		   // }
      break;
      case 2:
    	// RX interrupt
	     uBloxReceiveHandler(U1RBR);
	    break;
      case 3:
        // RLS interrupt
        break;
      case 6:
        // CTI interrupt
        break;
   }
  IDISABLE;
  VICVectAddr = 0;		/* Acknowledge Interrupt */
}

// IIR
#define RX_DATA_AVAILABLE 0x40
#define RX_CHAR_TIMEOUT   0xB0
#define TX_BUFFER_EMPTY   0x20

// LSR bits
// Bit 5 -- THRE = 1 means Empty 
#define LSR0_THRE  (U0LSR & 0x20)
#define UART0_TX_EMPTY (LSR0_THRE)
 // Bit 0 -- RDR = Receive Data Ready: 1 means data ready
#define LSR0_RDR   (U0LSR & 0x01)
#define UART0_RX_BYTES_AVAILABLE (LSR0_RDR)
#define UART0_RX_EMPTY (!LSR0_RDR)

// FCR Masks
#define FIFO_ENABLE 0x01
#define RX_FIFO_RESET 0x02
#define TX_FIFO_RESET 0X04

// Trigger Level Masks for RX interrupt for U0FCR (or U1FCR)
#define TL0 0x00  // Interrupt every character
#define TL1 0x40  // Interrupt every 4 characters
#define TL2 0x80  // Interrupt every 8 characters
#define TL3 0xB0  // Interrupt every 14 characters

// IER Masks
#define RBR_INT_ENABLE 0x01
#define THRE_INT_ENABLE 0x02 
#define IER_RESERVED_MASK 0x0F  //Bits 7 to 4 are reserved

bool UART0_TX_ENABLED = 0;  //Not sending
char txBuffer[50];
int txBuffLen;


// user uart
void uart0ISR(void) __irq
{
  unsigned char UART_rxdata;
  unsigned char t;
  int i;
  unsigned int iir;

  // Read IIR to clear interrupt and find out the cause
  IENABLE;
  
  LED(1,ON);
  LED(0,ON);
  // Handle UART interrupt
  iir = U0IIR;
  switch ((iir >> 1) & 0x7)
  {
      case 2://RX_DATA_AVAILABLE:
      //case RX_CHAR_TIMEOUT: //Occurs if characters in buffer and no more have arrived.
        // Read out all characters until the buffer is empty.
        //while ( UART0_RX_BYTES_AVAILABLE )
          //ringBufferPutByte(rb0Rx, U0RBR);
          // Expect: B1B2B3B4 for motor 1, motor 2, motor 3, motor 4.
          LED(1,OFF);
          m1 = U0RBR;
          while ( UART0_RX_EMPTY );
          m2 = U0RBR;
          while ( UART0_RX_EMPTY );
          m3 = U0RBR;
          while ( UART0_RX_EMPTY );
          m4 = U0RBR;
          updated = 1;
          LED(0,OFF);

		  break;
      case 1://TX_BUFFER_EMPTY:
      break;
      default:
      //case 3:
        // RLS interrupt
        break;
      //case 6:
        // CTI interrupt
      //break;
  }
  IDISABLE;
  VICVectAddr = 0;		// Acknowledge Interrupt
 }

void UART0Initialize(unsigned int baud)
{
  unsigned int divisor = peripheralClockFrequency() / (16 * baud);

// U0IER
//  Bit0: RBR enable
//  Bit1: THRE enable

  //UART0
  U0LCR = 0x83; /* 8 bit, 1 stop bit, no parity, enable DLAB */
  U0DLL = divisor & 0xFF;
  U0DLM = (divisor >> 8) & 0xFF;
  U0LCR &= ~0x80; /* Disable DLAB */
  U0FCR = FIFO_ENABLE | TL1; // Enable FIFO's, interrupt every 4 characters.
  U0TER = 0x80; //Enable transmitter.
}

// Uart1 seems to get used for GPS.
void UART1Initialize(unsigned int baud)
{
  unsigned int divisor = peripheralClockFrequency() / (16 * baud);
//UART1
  U1LCR = 0x83; /* 8 bit, 1 stop bit, no parity, enable DLAB */
  U1DLL = divisor & 0xFF;
  U1DLM = (divisor >> 8) & 0xFF;
  U1LCR &= ~0x80; /* Disable DLAB */
  U1FCR = 1;
}

void UART0Debug(char *msg, int length)
{
  while (length > 0)
  {
    UART0WriteChar(msg[0]);
    msg++;
    length--;
  }
}

//Write to UART0
void UART0WriteChar(unsigned char ch)
{
  while ((U0LSR & 0x20) == 0);
  U0THR = ch;
}

//Write to UART1
void UART1WriteChar(unsigned char ch)
{
  while ((U1LSR & 0x20) == 0);
  U1THR = ch;
}

unsigned char UART0ReadChar(void)
{
  while ((U0LSR & 0x01) == 0);
  return U0RBR;
}

unsigned char UART1ReadChar(void)
{
  while ((U1LSR & 0x01) == 0);
  return U1RBR;
}

void __putchar(int ch)
{
  if (ch == '\n')
    UART0WriteChar('\r');
  UART0WriteChar(ch);
}

void UART0_send(char *buffer, unsigned char length)
{
  unsigned char cnt=0;
  while (!(U0LSR & 0x20)); //wait until U0THR and U0TSR are both empty
  while(length--)
  {
    U0THR = buffer[cnt++];
    if(cnt>15)
    {
      while (!(U0LSR & 0x20)); //wait until U0THR is empty
    }
  }
}

void UART1_send(unsigned char *buffer, unsigned char length)
{
  unsigned char cnt=0;
  while(length--)
  {
    while (!(U1LSR & 0x20)); //wait until U1THR is empty
    U1THR = buffer[cnt++];
  }
}

/*
void UART1_send_ringbuffer(void)
{
  unsigned char t;
  if(!transmission1_running)
  {
    if ( !ringBufferEmpty(rb1Tx) )
    {
      t = ringBufferGetByte(rb1Tx);
      transmission1_running=1;
      UART1WriteChar(t);
    }
  }
}
*/
//example CRC16 function
unsigned short crc_update (unsigned short crc, unsigned char data)
     {
         data ^= (crc & 0xff);
         data ^= data << 4;

         return ((((unsigned short )data << 8) | ((crc>>8)&0xff)) ^ (unsigned char )(data >> 4)
                 ^ ((unsigned short )data << 3));
     }

 unsigned short crc16(void* data, unsigned short cnt)
 {
   unsigned short crc=0xff;
   unsigned char * ptr=(unsigned char *) data;
   int i;

   for (i=0;i<cnt;i++)
     {
       crc=crc_update(crc,*ptr);
       ptr++;
     }
   return crc;
 }

