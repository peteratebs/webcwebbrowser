/*
* KBD.C                                                     Version 3.7.1
*
* Keyboard Handler for ARM
*
* Copyright (c) 2001-2009 Micro Digital Associates, Inc.
*               All Rights Reserved.
*
* This is where a keyboard driver should be implemented if the target
* has a keyboard. Currently this module has only the store_key() lsr,
* which is assumed to get the ASCII key value from the serial port.
*
* Currently, the ASCII byte is passed to opcon. If this is developed
* into a full keyboard driver, this is how the key code information
* should be passed, for consistency with other versions of smx:
*
* (Proposed) Format of key code passed to opcon:
*
*                byte 3     byte 2     byte 1     byte 0
*             | -------- | xxxxxxxx | xxxxxxxx | xxxxxxxx |
*
*      byte 3: unused
*      byte 2: shift state (alt, ctrl, etc. Models bioskey() etc.)
*      byte 1: high byte of extended scan code
*      byte 0: low byte (or only byte) of scan code
*
*
* Author: David Moore
*
***********************************************************************/

#include "smx.h"
#include "core.h"

#if (DEVICE_CONSOLE_IN)

#ifdef __cplusplus
 extern "C" {
#endif

/* variables */

PXCB_PTR kbd_pipe, op_pipe;

#ifdef __cplusplus
}
#endif


/* keyboard initialization */

void _cdecl kbd_init(void)
{
#if 0  //DM: TODO: Enable when we supply an interrupt driven UART driver and take out polling code in opcon_main()
   MCB_PTR  pbuf;

   op_pipe = (PXCB_PTR)create_cx(CB_PIPE);
   kbd_pipe = op_pipe;
   pbuf = create_nmsg(0, 100);
   put_msg(pbuf, 100, op_pipe);
#endif
}


/* keyboard exit */

void _cdecl kbd_exit(void)
{
}


/*
* store_key(key_dword)
*
* Put the ASCII byte into op_pipe, for opcon or another task to process it.
* See x86 xkbd.c for a guide if this is implemented to pass a full key code
* to opcon.
*/

#if 0  //DM: TODO: Enable when we supply an interrupt driven UART driver and take out polling code in opcon_main()
void store_key(dword key_dword)
{
   pput_char((byte)key_dword, op_pipe, NO_WAIT);
}
#endif


/*
* get_kbd_char_polled()
*
* Get key from UART. Use only when interrupt-driven UART driver not present.
*/

char get_kbd_char_polled(void)
{
   char key;

   while (1)
   {
     #if defined(PROCESSOR_AT91LIB)  /* AT91 library supports old AT91 processors */
      if (CONSOLE_UART.usart_base->US_CSR & US_RXRDY)  /* if char available */
      {
         at91_usart_read(&CONSOLE_UART, (u_int *) &key);
         break;
      }
     #elif defined(PROCESSOR_AT91SAM7A1) || defined(PROCESSOR_AT91SAM7A2)
      if (CSP_USART_GET_SR((CSP_USART_T *)USART0_BASE_ADDRESS) & RXRDY) /* if char available */
      {
         key = Uart0GetChar();
         break;
      }
      #elif defined(PROCESSOR_AT91RM9200) || defined(PROCESSOR_AT91SAM7A3) || \
            defined(PROCESSOR_AT91SAM7S) || defined(PROCESSOR_AT91SAM7SE) || \
            defined(PROCESSOR_AT91SAM7X) || defined(PROCESSOR_AT91SAM9)
      if ((AT91C_BASE_DBGU->DBGU_CSR & AT91C_US_RXRDY)) /* if char available */
      {
         key = UartDbgGetChar();
         break;
      }
     #elif defined(PROCESSOR_COREMP7)
      if (COREMP7_BASE_UART1->UART_STATUS & UART_SR_RXFULL) /* if char available */
      {
         key = Uart1GetChar();
         break;
      }
     #elif defined(PROCESSOR_EP93XX)
      if (UART1Flag & 0x40) /* if char available */
      {
         key = Uart1GetChar();
         break;
      }
     #elif defined(PROCESSOR_IMX31)
      if (!(UTS_1 & 0x20)) /* if char available */
      {
         key = Uart1GetChar();
         break;
      }
     #elif defined(PROCESSOR_LH75401)
      if (LH75401UARTRxReady()) /* if char available */
      {
         key = LH75401UARTRx();
         break;
      }
     #elif defined(PROCESSOR_LH79520)
      if (0 == (UART1_FR & 0x10)) /* if char available */
      {
         key = Uart1GetChar();
         break;
      }
     #elif defined(PROCESSOR_LH79524)
      if (0 == (UART0_UARTFR & 0x10)) /* if char available */
      {
         key = Uart0GetChar();
         break;
      }
     #elif defined(PROCESSOR_LH7A400) || defined(PROCESSOR_LH7A404)
      extern INT_32 uartdev;   /* defined in bsp.c */
      if (uart_read_ring(uartdev, &key, 1) == 1)
      {
         break;
      }
     #elif defined(PROCESSOR_LM3S)
      if (UARTCharsAvail(DEVICE_CONSOLE_IN_PORT_BASE))
      {
         key = (u8) UARTCharGet(DEVICE_CONSOLE_IN_PORT_BASE);
         break;
      }
     #elif defined(PROCESSOR_LPC2XXX)
      #if (DEVICE_CONSOLE_IN_PORT == 0)
      if (U0LSR & ULSR_RDR) /* if char available */
      {
         key = uart0GetByte();
         break;
      }
      #elif (DEVICE_CONSOLE_IN_PORT == 1)
      if (U1LSR & ULSR_RDR) /* if char available */
      {
         key = uart1GetByte();
         break;
      }
      #endif
     #elif defined(PROCESSOR_LPC23XX)
      if (U0LSR & 0x01) /* if char available */
      {
         key = Uart0GetChar();
         break;
      }
     #elif defined(PROCESSOR_LPC24XX)
      #if (DEVICE_CONSOLE_IN_PORT == 0)
      if (U0LSR & 0x01) /* if char available */
      {
         key = Uart0GetChar();
         break;
      }
      #elif (DEVICE_CONSOLE_IN_PORT == 1)
      if (U1LSR & 0x01) /* if char available */
      {
         key = Uart1GetChar();
         break;
      }
      #elif (DEVICE_CONSOLE_IN_PORT == 2)
      if (U2LSR & 0x01) /* if char available */
      {
         key = Uart2GetChar();
         break;
      }
      #endif
     #elif defined(PROCESSOR_LPC288X)
      if (_LSR & 0x01) /* if char available */
      {
         key = Uart0GetChar();
         break;
      }
     #elif defined(PROCESSOR_LPC3180)
      #if (DEVICE_CONSOLE_IN_PORT == 1)
      if (Uart1GetChar(&key, 1) == 1)
      {
        break;
      }
      #elif (DEVICE_CONSOLE_IN_PORT == 2)
      if (Uart2GetChar(&key, 1) == 1)
      {
        break;
      }
      #endif
     #elif defined(PROCESSOR_LPC32X0)
      #if (DEVICE_CONSOLE_IN_PORT == 5)
      if (U5LSR & 0x01) /* if char available */
      {
         key = Uart5GetChar();
         break;
      }
      #endif
     #elif defined(PROCESSOR_M9328MX1_MXL)
      if (MX1UART1RxReady())  /* if char available */
      {
         key = MX1UART1Rx();
         break;
      }
     #elif defined(PROCESSOR_MAC71XX)
      if (ESCISR1_A & 0x20) /* if char available */
      {
         key = Uart0GetByte();
         break;
      }
     #elif defined(PROCESSOR_S3C2410)
      if (UFSTAT0 & 0xF) /* if char available */
      {
         key = Uart0GetChar();
         break;
      }
     #elif defined(PROCESSOR_STM32)
      if (USART_GetFlagStatus(DEVICE_CONSOLE_IN_PORT_BASE, USART_FLAG_RXNE) != RESET)
      {
         key = (u8) USART_ReceiveData(DEVICE_CONSOLE_IN_PORT_BASE);
         break;
      }
     #elif defined(PROCESSOR_STR71x)
      if (UART_FlagStatus(CONSOLE_UART) & 0x0001) /* if char available */
      {
         UART_ByteReceive(CONSOLE_UART, (u8 *)&key, 0);
         break;
      }
     #elif defined(PROCESSOR_STR75X)
      if(UART_GetFlagStatus(CONSOLE_UART, UART_FLAG_RxFIFOEmpty) == RESET)
      {
        key = UART_ReceiveData(CONSOLE_UART);
        break;
      }
     #elif defined(PROCESSOR_STR91X)
      if (UART_GetFlagStatus(UART0, UART_FLAG_RxFIFOEmpty) != SET) /* if char available */
      {
         key = Uart0GetChar();
         break;
      }
     #elif defined(PROCESSOR_TMS470)
      if (SCI1CTL1 & RXRDY) /* if char available */
      {
         key = uart0GetByte();
         break;
      }
     #endif

      SMX_DELAY_MSEC(100);             /* 10 chars/sec */
   }

   return key;
}

#endif // DEVICE_CONSOLE_IN

