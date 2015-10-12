/*
* UART.H                                                   Version 3.7.1
*
* Defines for UART of NXP (Philips) LPC24xx.
*
* Copyright (c) 2007-2009 Micro Digital Associates, Inc.
*               All Rights Reserved.
*
* Author:  Wan Xingsheng
*
***********************************************************************/

#ifndef SMX_UART_H
#define SMX_UART_H

#ifdef __cplusplus
extern "C" {
#endif

void Uart0Init (dword baud);
void Uart0PutChar(unsigned char chr);
unsigned char Uart0GetChar();

void Uart1Init (dword baud);
void Uart1PutChar(unsigned char chr);
unsigned char Uart1GetChar();

void Uart2Init (dword baud);
void Uart2PutChar(unsigned char chr);
unsigned char Uart2GetChar();

#ifdef __cplusplus
}
#endif

#endif /* SMX_UART_H */
