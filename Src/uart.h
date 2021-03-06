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

#ifndef __UART_H
#define __UART_H

void UART0Initialize(unsigned int baud);
void UART1Initialize(unsigned int baud);

void UART0WriteChar(unsigned char);
void UART1WriteChar(unsigned char);
//extern unsigned char UART0ReadChar(void);
extern unsigned char UART1ReadChar(void);
void __putchar(int);
extern void UART0_send(char *, unsigned char);
//extern void UART1_send(unsigned char *, unsigned char);
//extern void mdv_output(unsigned int);
void uart0ISR(void);
void uart1ISR(void);


void emptyUART0(void);
void UART0Debug(char *msg, int length);
void uart0Prime(void);


extern unsigned char send_buffer[16];
extern unsigned short crc16(void *, unsigned short);
extern unsigned short crc_update (unsigned short, unsigned char);
extern unsigned char chksum_trigger;
extern unsigned char UART_CalibDoneFlag;
extern unsigned char trigger_transmission;
extern unsigned char transmission_running;

#define RBREAD 0
#define RBWRITE 1
#define RBFREE  2
#define RINGBUFFERSIZE	384
#define MATLABFIFOSIZE 256


#define RX_IDLE 0
#define RX_ACTSYNC1 1
#define RX_ACTSYNC2 2
#define RX_ACTDATA 3
#define RX_ACTCHKSUM 4

#define GPSCONF_TIMEOUT 200

#endif //__UART_H

