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
#include "ssp.h"
#include "sdk.h"

#include "UARTData.h"

extern char updated;

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

//bool UART0_TX_ENABLED = 0;  //Not sending

RingBuffer u0s, u0r; //Send / Receive ring buffers.

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
  f iir = (U0IIR >> 1) & 0x7;
  if ( iir & 0x2 )
  {
      //case 2://RX_DATA_AVAILABLE:
      //case RX_CHAR_TIMEOUT: //Occurs if characters in buffer and no more have arrived.
        // Read out all characters until the buffer is empty.

          updated = 1;
          LED(0,OFF);
        }
	if ( iir & 0x1 ) //NEEDED:  HOW MANY CHARACTERS TO FILL THE TX BUFFER?
//      case 1://TX_BUFFER_EMPTY, fill buffer.
        while (RBbytes(&ub0s) && (U0LSR & 0x20))
          U0THR = RBDequeue(&ub0s);
  //    break;
  //    default:
      //case 3:
        // RLS interrupt
  //      break;
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
  U0FCR = FIFO_ENABLE | TL2; // Enable FIFO's, interrupt every 8 characters.
  RBInit(&ub0s);
  RBInit(&ub0r); //Send / Receive uart buffers.
  U0TER = 0x80; //Enable transmitter.
}

void UART0Debug(char *msg, int length)
{

  if ( U0LSR & 0x40 ) // Transmitter Empty, so we need to write directly to transmitter.
  {
    length--;
    U0THR = *msg;
    msg++;
  }

  while (length > 0)
  {
    RBEnqueue(&ub0s, *msg);
    msg++;
    length--;
  }
}

//Write to UART0
void UART0WriteChar(unsigned char ch)
{
  if ( U0LSR & 0x40 ) // Transmitter Empty, so we need to write directly to transmitter.
    U0THR = ch;
  else
    RBEnqueue(&ub0s, ch);
}