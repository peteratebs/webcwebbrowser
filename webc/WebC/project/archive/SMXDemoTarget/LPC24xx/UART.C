/*
* UART.C                                                  Version 3.7.1
*
* Routines to send/recieve a char through UART of NXP (Philips) LPC24xx.
*
* Copyright (c) 2007-2009 Micro Digital Associates, Inc.
*               All Rights Reserved.
*
* Author:  Wan Xingsheng
*
***********************************************************************/

#include "smx.h"
#include "core.h"

/* Initializes UART0. */
void Uart0Init(dword baud)
{
    dword dwLatch;

    /* Enable TxD0 and RxD0 Pins */
    PINSEL0 |= 0x00000050;
    
    /* Fractional divider not used. */
    U0FDR   = 0;

    /* 8 bits, no Parity, 1 Stop bit, Enable access to Divisor Latches. */
    U0LCR   = 0x83;

    /* Set Baud Rate */
    dwLatch = (PCLK + (8 * baud)) / (16 * baud);
    U0DLL   = dwLatch & 0xFF;          /* Low divisor */
    U0DLM   = (dwLatch >> 8) & 0xFF;   /* High divisor */
    
    /* 8 bits, no Parity, 1 Stop bit, Disable access to Divisor Latches. */
    U0LCR   = 0x03;
}

/* Send a char */
void Uart0PutChar(unsigned char chr)
{
    /* Waiting untill Transmitter Holding Register Empty. */
    while (!(U0LSR & 0x20)) {}
   
    /* Write data to Transmit Data Register */
    U0THR = chr;
}

/* Receive a char */
unsigned char Uart0GetChar()
{
    /* Waiting untill RXD0 Receiver Data Ready. */
    //while (!(U0LSR & 0x01)) {}

    /* Return char in  */
    return (U0RBR & 0xFF);
}

/* Initializes UART1. */
void Uart1Init(dword baud)
{
    dword dwLatch;

    /* Enable TxD1 and RxD1 Pins */
    PINSEL7 |= 0x0000000F;
    
    /* Fractional divider not used. */
    U1FDR   = 0;

    /* 8 bits, no Parity, 1 Stop bit, Enable access to Divisor Latches. */
    U1LCR   = 0x83;

    /* Set Baud Rate */
    dwLatch = (PCLK + (8 * baud)) / (16 * baud);
    U1DLL   = dwLatch & 0xFF;          /* Low divisor */
    U1DLM   = (dwLatch >> 8) & 0xFF;   /* High divisor */
    
    /* 8 bits, no Parity, 1 Stop bit, Disable access to Divisor Latches. */
    U1LCR   = 0x03;
}

/* Send a char */
void Uart1PutChar(unsigned char chr)
{
    /* Waiting untill Transmitter Holding Register Empty. */
    while (!(U1LSR & 0x20)) {}
   
    /* Write data to Transmit Data Register */
    U1THR = chr;
}

/* Receive a char */
unsigned char Uart1GetChar()
{
    /* Waiting untill RXD1 Receiver Data Ready. */
    //while (!(U1LSR & 0x01)) {}

    /* Return char in  */
    return (U1RBR & 0xFF);
}

/* Initializes UART2. */
void Uart2Init(dword baud)
{
    dword dwLatch;

    /* Enable TxD2 and RxD2 Pins */
    PINSEL0 |= 0x00500000;
    
    /* Enable the power for UART 2 */
    PCONP |= 0x01000000;
    
    /* Fractional divider not used. */
    U2FDR   = 0;

    /* 8 bits, no Parity, 1 Stop bit, Enable access to Divisor Latches. */
    U2LCR   = 0x83;

    /* Set Baud Rate */
    dwLatch = (PCLK + (8 * baud)) / (16 * baud);
    U2DLL   = dwLatch & 0xFF;          /* Low divisor */
    U2DLM   = (dwLatch >> 8) & 0xFF;   /* High divisor */
    
    /* 8 bits, no Parity, 1 Stop bit, Disable access to Divisor Latches. */
    U2LCR   = 0x03;
}

/* Send a char */
void Uart2PutChar(unsigned char chr)
{
    /* Waiting untill Transmitter Holding Register Empty. */
    while (!(U2LSR & 0x20)) {}
   
    /* Write data to Transmit Data Register */
    U2THR = chr;
}

/* Receive a char */
unsigned char Uart2GetChar()
{
    /* Waiting untill RXD2 Receiver Data Ready. */
    //while (!(U2LSR & 0x01)) {}

    /* Return char in  */
    return (U2RBR & 0xFF);
}

