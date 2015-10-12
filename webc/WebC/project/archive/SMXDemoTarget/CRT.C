/*
* CRT.C                                                     Version 3.7.1
*
* Text display routines. Implemented to send characters to a terminal
* via serial port or to PEG console window (when implemented).
*
* Copyright (c) 1998-2009 Micro Digital Associates, Inc.
*               All Rights Reserved.
*
* Author: A. C. Verbeck
*
*  Screen write routines for TASKS and LSR's -- do not call from isr's
*  since these call unlockx() and smx calls are not permitted from isr's.
*  --> For isr's, use _isr() versions.
*
*  USER:
*    1. Use functions such as _itoa to convert numbers to strings.
*       Then use wr_string(), ws(), or oputs() to output strings
*
*    2. 1,1 is top left corner of screen, on a terminal.
*
***********************************************************************/

#include "smx.h"
#include "xcrt.h"
#include "core.h"
#include <stdio.h>

#if defined(BOARD_LUMINARY_LM3S)
/* Output from Luminary boards is via USB virtual COM port and colors
   are hard to see in HyperTerminal. */
#define USE_COLOR 0
#else
#define USE_COLOR 1  /* 1 for color; 0 for b/w */
#endif

#if (DEVICE_CONSOLE_OUT_PORT >= 0)
#define USES_UART 1
#else
#define USES_UART 0
#endif


#ifdef __cplusplus
 extern "C" {
#endif

char buf[DISPLAY_COLUMNS_MAX+1];  /* Display work buffer */
char crt_is_init = FALSE;         /* Display functions do nothing if FALSE. See note 1 below. */

#if USES_UART
static char ANSI_buf[10];
static void ANSI_ClrEndOfLine(void);
static void ANSI_ClrScreen(void);
static void ANSI_CmdSeq(void);
static void ANSI_GModeReset(void);
static void ANSI_GotoXY(int x, int y);
static void ANSI_SetGMode(int gm);
#endif

#ifdef __cplusplus
 }
#endif


#if USES_UART

/*
 *    Name:
 *       pc / ps
 *
 * Description:
 *    Local assembly interface output functions:
 *    pc -- put character
 *    ps -- put string
 *
 * Parameters:
 *    pc: char  -- character to print
 *    ps: char* -- string to print
 *
 * Returns:
 *    (None)
 */

#if defined(PROCESSOR_LH7A400) || defined(PROCESSOR_LH7A404)
extern INT_32 uartdev;   /* instantiated in bsp.c */
void pc(const char c)
{
    uart_write_ring(uartdev, (void *)&c, 1);
}
#define ps(s)  uart_write_ring(uartdev, (UNS_8 *)(s), strlen(s));
#endif /* PROCESSOR_LH7A400 || PROCESSOR_LH7A404 */

#if defined(PROCESSOR_STR71x)
/* STR71x library's UART_ByteSend() can't be used because it takes a
   pointer to char not char, and this can't be solved with just a typecast
   for a character literal (e.g. 'K'). The problem is that a character
   literal is part of the instruction, not necessarily in a data variable,
   so you can't just take the address. Here is the code of that function
   with minor edits to change the data type:
*/
void pc(const char c)
{
   if (CONSOLE_UART->CR & (0x0001<<UART_FIFOEnableBit)) // if FIFO ENABLED
      while((CONSOLE_UART->SR & UART_TxFull)) {} // while the UART_TxFIFO contain 16 characters.
   else                  // if FIFO DISABLED
      while (!(CONSOLE_UART->SR & UART_TxEmpty)){} // while the transmit shift register not empty
   CONSOLE_UART->TxBUFR = c;
}
#define ps(s)  UART_DataSend(CONSOLE_UART, (u8 *)(s), strlen(s));  /* Don't use UART_StringSend() since it does CRLF. We use ps() for ANSI sequences too. */
#endif /* PROCESSOR_STR71x */

#if defined(PROCESSOR_STR75X)
void pc(const char c)
{
   while (UART_GetFlagStatus(CONSOLE_UART, UART_FLAG_TxFIFOFull)!= RESET) {}
   UART_SendData(CONSOLE_UART, c);
}
#endif /* PROCESSOR_STR75X */

/* The ones below here define just pc() and then ps() uses it. */

#if defined(PROCESSOR_AT91LIB)  /* AT91 library supports old Atmel processors */
void pc(const char c)
{
   while (!(CONSOLE_UART.usart_base->US_CSR & US_TXRDY)) {}  /* wait till ready for next char */
   at91_usart_write(&CONSOLE_UART, (u_int)(c));
}
#endif /* PROCESSOR_AT91LIB */

#if defined(PROCESSOR_AT91SAM7A1) || defined(PROCESSOR_AT91SAM7A2)
#define pc(c) Uart0PutChar((unsigned char)c);
#endif /* PROCESSOR_AT91SAM7A1 || PROCESSOR_AT91SAM7A2 */


#if defined(PROCESSOR_AT91RM9200) || defined(PROCESSOR_AT91SAM7A3) || \
    defined(PROCESSOR_AT91SAM7S) || defined(PROCESSOR_AT91SAM7SE) || \
    defined(PROCESSOR_AT91SAM7X) || defined(PROCESSOR_AT91SAM9)
#define pc(c) UartDbgPutChar((unsigned char)c);
#endif /* PROCESSOR_AT91RM9200 || ... || PROCESSOR_AT91SAM9 */

#if defined(PROCESSOR_COREMP7)
#define pc(c) Uart1PutChar(c);
#endif /* PROCESSOR_COREMP7 */

#if defined(PROCESSOR_EP93XX)
#define pc(c) Uart1PutChar(c);
#endif /* PROCESSOR_EP93XX */

#if defined(PROCESSOR_IMX31)
#define pc(c) Uart1PutChar(c);
#endif /* PROCESSOR_IMX31 */

#if defined(PROCESSOR_LH75401)
#define pc(c) LH75401UARTTxPolling((UNS_8)(c));
#endif /* PROCESSOR_LH75401 */

#if defined(PROCESSOR_LH79520)
#define pc(c) Uart1PutChar((unsigned char)c);
#endif /* PROCESSOR_LH79520 */

#if defined(PROCESSOR_LH79524)
#define pc(c) Uart0PutChar((unsigned char)c);
#endif /* PROCESSOR_LH79524 */

#if defined(PROCESSOR_LM3S)
#define pc(c) UARTCharPut(DEVICE_CONSOLE_OUT_PORT_BASE, c);
#endif /* PROCESSOR_LM3S */

#if defined(PROCESSOR_LPC2XXX)
#if (DEVICE_CONSOLE_OUT_PORT == 0)
#define pc(c) uart0SendByte((unsigned char)c);
#elif (DEVICE_CONSOLE_OUT_PORT == 1)
#define pc(c) uart1SendByte((unsigned char)c);
#endif
#endif /* PROCESSOR_LPC2XXX */

#if defined(PROCESSOR_LPC23XX)
#define pc(c) Uart0PutChar((unsigned char)c);
#endif /* PROCESSOR_LPC23XX */

#if defined(BOARD_EA_LPC2468)
#define pc(c) Uart1PutChar((unsigned char)c);
#endif /* BOARD_EA_LPC2468 */

#if defined(BOARD_EA_LPC2478)
#define pc(c) Uart2PutChar((unsigned char)c);
#endif /* BOARD_EA_LPC2478 */

#if defined (BOARD_IAR_LPC2478KS)
#define pc(c) Uart0PutChar((unsigned char)c);
#endif /* BOARD_IAR_LPC2478KS */

#if defined(PROCESSOR_LPC288X)
#define pc(c) Uart0PutChar((unsigned char)c);
#endif /* PROCESSOR_LPC288X */

#if defined(PROCESSOR_LPC3180)
#if (DEVICE_CONSOLE_OUT_PORT == 1)
#define pc(c) Uart1PutChar((unsigned char)c);
#elif (DEVICE_CONSOLE_OUT_PORT == 2)
#define pc(c) Uart2PutChar((unsigned char)c);
#endif
#endif /* PROCESSOR_LPC3180 */

#if defined(PROCESSOR_LPC32X0)
#if (DEVICE_CONSOLE_OUT_PORT == 5)
#define pc(c) Uart5PutChar((unsigned char)c);
#endif
#endif /* PROCESSOR_LPC32X0 */

#if defined(PROCESSOR_M9328MX1_MXL)
#define pc(c)  MX1UART1TxPolling((U8)(c));
#endif /* PROCESSOR_M9328MX1_MXL */

#if defined(PROCESSOR_MAC71XX)
#define pc(c) Uart0SendByte((unsigned char)c);
#endif /* PROCESSOR_MAC71XX */

#if defined(PROCESSOR_S3C2410)
#define pc(c) Uart0PutChar((unsigned char)c);
#endif /* PROCESSOR_S3C2410 */

#if defined(PROCESSOR_STM32)
void pc(const char c)
{
   while(USART_GetFlagStatus(DEVICE_CONSOLE_OUT_PORT_BASE, USART_FLAG_TXE) == RESET) {}
   USART_SendData(DEVICE_CONSOLE_OUT_PORT_BASE, c);
}
#endif /* PROCESSOR_STM32 */

#if defined(PROCESSOR_STR91X)
#define pc(c) Uart0PutChar((unsigned char)c);
#endif /* PROCESSOR_STR91X */

#if defined(PROCESSOR_TMS470)
#define pc(c) uart0SendByte((unsigned char)c);
#endif /* PROCESSOR_TMS470 */

#if defined(PROCESSOR_AT91) || defined(PROCESSOR_COREMP7) || defined(PROCESSOR_EP93XX) || \
    defined(PROCESSOR_IMX31) || \
    defined(PROCESSOR_LH75401) || defined(PROCESSOR_LH79520) || defined(PROCESSOR_LH79524) || \
    defined(PROCESSOR_LM3S) || \
    defined(PROCESSOR_LPC2XXX) || defined(PROCESSOR_LPC23XX) || defined(PROCESSOR_LPC24XX) || \
    defined(PROCESSOR_LPC288X) || defined(PROCESSOR_LPC3XXX) || defined(PROCESSOR_LPC32X0) || \
    defined(PROCESSOR_M9328MX1_MXL) || defined(PROCESSOR_MAC71XX) || \
    defined(PROCESSOR_S3C2410) || \
    defined(PROCESSOR_STM32) || \
    defined(PROCESSOR_STR75X) || defined(PROCESSOR_STR91X) || \
    defined(PROCESSOR_TMS470)
    
void ps(const char *s)
{
   int i;
   int len = strlen(s);

   for (i = 0; i < len; i++)
      pc(s[i]);
}
#endif /* PROCESSOR_AT91 || _M9328MX1_MXL || ... */
#endif /* USES_UART */


/*--------------------------------------------------------------------------*/
/* The functions below here should not need to be ported.                   */
/*--------------------------------------------------------------------------*/

/*
 *  SCREEN FUNCTIONS
 */

/*
 *  crt_init() does any necessary initialization and then sets crt_is_init.
 *  If the display routines in this file are output via serial port, we
 *  assume the UART has already been initialized before this is called.
 */
void _cdecl crt_init(void)
{
#if (DEVICE_CONSOLE_OUT)

  #if (USES_UART)
   ANSI_GModeReset();
   ANSI_ClrScreen();
   crt_is_init = TRUE;
  #endif

  #if defined(PEG) && SMXPEG_CONSOLE_WINDOW
   //TODO:
  #endif

#endif
}

/*
 *  clr_line() can be used to clear a line (row) of the screen to a particular
 *  background color.  See xcrt.h for a list of background colors.
 */
void _cdecl clr_line(int row, int B_color)
{
#if (DEVICE_CONSOLE_OUT)

  #if (USES_UART)
   int   was_unlocked;
  #endif

   if (!crt_is_init) return;

  #if (USES_UART)
   was_unlocked = !IS_LOCKED(self);
   LOCK(self);
   /* goto beginning of line and clear to end */
   ANSI_GotoXY(row,0);
   ANSI_SetGMode(B_color+40);
   ANSI_ClrEndOfLine();
   if (was_unlocked) unlockx(self);
  #endif

  #if defined(PEG) && SMXPEG_CONSOLE_WINDOW
   //TODO:
  #endif

#else
   /* Avoid compiler warning about unused pars. */
   (void) row;
   (void) B_color;

#endif
}

/*
 *  clr_screen() clears the operation screen to the background color
 *  passed to it and sets a foreground color. See xcrt.h for a list
 *  of colors.
 */
void _cdecl clr_screen(int F_color, int B_color)
{
#if (DEVICE_CONSOLE_OUT)

 #if (USES_UART)
   int   was_unlocked;
 #endif

   if (!crt_is_init) return;

 #if (USES_UART)
  #if (USE_COLOR == 0)
   F_color = WHITE;
   B_color = BLACK;
  #endif
   was_unlocked = !IS_LOCKED(self);
   LOCK(self);
   ANSI_GotoXY(0,0);
   ANSI_SetGMode(F_color+30);
   ANSI_SetGMode(B_color+40);
   ANSI_ClrScreen();
   if (was_unlocked) unlockx(self);
 #endif

 #if defined(PEG) && SMXPEG_CONSOLE_WINDOW
   //TODO:
 #endif

#else
   /* Avoid compiler warning about unused pars. */
   (void) F_color;
   (void) B_color;

#endif
}


/* clr_screen_isr() clears the operation screen to the background color
*  passed to it and sets a foreground color. It can be called from isr's
*  and from go_smx() which runs before we're in the tasking environment
*  since it does not call any ssr's (e.g. unlockx()).
*/
void _cdecl clr_screen_isr(void)
{
#if (DEVICE_CONSOLE_OUT)

  #if (USES_UART)
   if (!crt_is_init) return;
   ANSI_GotoXY(0,0);
   ANSI_SetGMode(LIGHTGRAY+30);
   ANSI_SetGMode(BLACK+40);
   ANSI_ClrScreen();
  #endif

  #if defined(PEG) && SMXPEG_CONSOLE_WINDOW
   //TODO:
  #endif

#endif
}


/*
 *  clr_screen_block() clears a block of the operation screen to a
 *  particular background color (see xcrt.h for a list of these).  Pass
 *  this function the leftmost and rightmost columns of the screen to be
 *  cleared and then the topmost and bottommost rows to be cleared,
 *  followed by the block color.
 *
 *  NOT SUPPORTED
 */
void _cdecl clr_screen_block(int l_col, int r_col, int t_row, int b_row, int B_color)
{
   (void)l_col; (void)r_col; (void)t_row; (void)b_row; (void)B_color;
#if (DEVICE_CONSOLE_OUT)
#endif
}

/*
 *  cursor_on / cursor_off
 *
 *  NOT SUPPORTED
 */
void _cdecl cursor_off(void)
{
#if (DEVICE_CONSOLE_OUT)
#endif
}

void _cdecl cursor_on(void)
{
#if (DEVICE_CONSOLE_OUT)
#endif
}

/*
 *  scroll_up() moves every char + attribute up one line. The first line
 *  is discarded. The last line is cleared.
 *
 *  NOT SUPPORTED
 */
void _cdecl scroll_up(void)
{
#if (DEVICE_CONSOLE_OUT)
#endif
}


/*
 *  oputs is a task-reentrant equivalent of puts()
 */
int _cdecl oputs(const char *in_string)
{
#if (DEVICE_CONSOLE_OUT)

  #if (USES_UART)
   int   was_unlocked;
  #endif

   if (!crt_is_init) return(FALSE);

  #if (USES_UART)
   was_unlocked = !IS_LOCKED(self);
   LOCK(self);
   ps(in_string);
   if (was_unlocked) unlockx(self);
  #endif

  #if defined(PEG) && SMXPEG_CONSOLE_WINDOW
   //TODO:
  #endif

   return(TRUE);

#else
   /* Avoid compiler warning about unused pars. */
   (void) in_string;
   return(TRUE);

#endif
}


/*
 *  wr_char() writes a character to the operation screen at the loca-
 *  tion specified by col and row.  F_color is the color of the character
 *  itself and B_color is the color of the space around the character.
 *  blinking should be BLINK if the text is to blink, and !BLINK if not.
 *  If monochrome, the attribute is converted such that the intensity and
 *  blinking bits are preserved and normal text is selected (rather than
 *  inverse text or underlined text).
 */
void _cdecl wr_char(int col, int row, int F_color, int B_color, int blinking, char ch)
{
#if (DEVICE_CONSOLE_OUT)

 #if (USES_UART)
   int   was_unlocked;
 #endif

   if (!crt_is_init) return;

 #if (USES_UART)
  #if (USE_COLOR == 0)
   F_color = WHITE;
   B_color = BLACK;
  #endif
   was_unlocked = !IS_LOCKED(self);
   LOCK(self);
   ANSI_GotoXY(row,col);
   ANSI_SetGMode(F_color+30);
   ANSI_SetGMode(B_color+40);
   if (blinking)
      ANSI_SetGMode(BLINK);
   pc(ch);
   if (was_unlocked) unlockx(self);
 #endif

 #if defined(PEG) && SMXPEG_CONSOLE_WINDOW
   //TODO:
 #endif

#else
   /* Avoid compiler warning about unused pars. */
   (void) col;
   (void) row;
   (void) F_color;
   (void) B_color;
   (void) blinking;
   (void) ch;

#endif
}


/*
 *  wc() is a shortened version of wr_char(). It does not change character
 *  attributes.
 */
void _cdecl wc(int col, int row, char ch)
{
#if (DEVICE_CONSOLE_OUT)

  #if (USES_UART)
   int   was_unlocked;
  #endif

   if (!crt_is_init) return;

  #if (USES_UART)
   was_unlocked = !IS_LOCKED(self);
   LOCK(self);
   ANSI_GotoXY(row,col);
   pc(ch);
   if (was_unlocked) unlockx(self);
  #endif

  #if defined(PEG) && SMXPEG_CONSOLE_WINDOW
   //TODO:
  #endif

#else
   /* Avoid compiler warning about unused pars. */
   (void) col;
   (void) row;
   (void) ch;

#endif
}


/*
 *  wr_string() writes a string to the operation screen at the location
 *  specified by col and row.  F_color is the color of the string
 *  and B_color is the color of the background. blinking should be BLINK
 *  if the string is to blink, and !BLINK if not. The last parameter is
 *  a pointer to the null-terminated string. wr_string goes to the next
 *  line if the current line is filled.
 *
 *  wr_string() was modified to support writing to graphics screen
 *  as well as text screen.  If graphics has not been initialized,
 *  writes messages to text screen.  Necessary in order to see any
 *  smx error messages that occur during initialization (these are
 *  usually insufficient resource errors -- for example, failure to
 *  create task due to lack of tcb's.
 */
void _cdecl wr_string(int col, int row, int F_color, int B_color,
                                         int blinking, const char *in_string)
{
#if (DEVICE_CONSOLE_OUT)

 #if (USES_UART)
   int   was_unlocked;
 #endif

   if (!crt_is_init) return;

 #if (USES_UART)
  #if (USE_COLOR == 0)
   F_color = WHITE;
   B_color = BLACK;
  #endif
   was_unlocked = !IS_LOCKED(self);
   LOCK(self);
   ANSI_GotoXY(row,col);
   ANSI_SetGMode(F_color+30);
   ANSI_SetGMode(B_color+40);
   if (blinking)
      ANSI_SetGMode(BLINK);
   ps(in_string);
   ANSI_ClrEndOfLine();
   if (was_unlocked) unlockx(self);
 #endif

 #if defined(PEG) && SMXPEG_CONSOLE_WINDOW
   //TODO:
 #endif

#else
   /* Avoid compiler warning about unused pars. */
   (void) col;
   (void) row;
   (void) F_color;
   (void) B_color;
   (void) blinking;
   (void) in_string;

#endif
}


/*
 *  ws() is a shortened version of wr_string().
 */
void _cdecl ws(int col, int row, const char *in_string)
{
#if (DEVICE_CONSOLE_OUT)

  #if (USES_UART)
   int   was_unlocked;
  #endif

   if (!crt_is_init) return;

  #if (USES_UART)
   was_unlocked = !IS_LOCKED(self);
   LOCK(self);
   ANSI_GotoXY(row,col);
   ps(in_string);
   if (was_unlocked) unlockx(self);
  #endif

  #if defined(PEG) && SMXPEG_CONSOLE_WINDOW
   //TODO:
  #endif

#else
   /* Avoid compiler warning about unused pars. */
   (void) col;
   (void) row;
   (void) in_string;

#endif
}


/*
 *  ws_isr() writes a string to the screen and can be called from isr's
 *  since it does not call any ssr's (e.g. unlockx()). Useful for displaying
 *  markers for debugging purposes.
 */
void _cdecl ws_isr(int col, int row, const char * in_string)
{
#if (DEVICE_CONSOLE_OUT)

  #if (USES_UART)
   if (!crt_is_init) return;

   ANSI_GotoXY(row,col);
   ps(in_string);
  #endif

  #if defined(PEG) && SMXPEG_CONSOLE_WINDOW
   /* Don't implement for PEG since called from isr. */
  #endif

#else
   /* Avoid compiler warning about unused pars. */
   (void) col;
   (void) row;
   (void) in_string;

#endif
}


/******* WR_COUNTER
* Writes a 6-digit counter to the screen. Used by various demo tasks.
* (When the counter exceeds 6 digits, counter display moves to the right.)
* Note that sprintf is not used because it can cause problems when SS != DS
* and because it requires so much stack (500+ bytes!).
***********************************************************************/

void _cdecl wr_counter(dword ctr, int col, int row, int Fcolor, int Bcolor, int radix)
{
#if (DEVICE_CONSOLE_OUT)

   char  buffer[11];
   uint  i, j;

   ENTER_CLIB();
   _ultoa(ctr, buffer, radix);
   EXIT_CLIB();

   for(i = 0; buffer[i] && i < 6; i++) {}  /* find end of string */
   for(; i < 6; i++)                       /* right align */
   {
      for(j = i+1; j > 0; j--)
         buffer[j] = buffer[j-1];
      buffer[j] = '0';
   }

   wr_string(col,row,Fcolor,Bcolor,!BLINK,buffer);

#else
   /* Avoid compiler warning about unused pars. */
   (void) ctr;
   (void) col;
   (void) row;
   (void) Fcolor;
   (void) Bcolor;
   (void) radix;

#endif /* DEVICE_CONSOLE_OUT */
}


#if (USES_UART)

/*
 * Name:
 *    ANSI Terminal Functions
 *
 * Description:
 *    These send ANSI codes to the terminal to control cursor position,
 *    color, etc.
 *
 * Parameters:
 *    (None)
 *
 * Returns:
 *    (None)
 *
 * Notes:
 *    We do not check crt_is_init because it is checked in all functions
 *    (above) that call these.
 */

static void ANSI_CmdSeq(void)
{
#if (DEVICE_CONSOLE_OUT)

   ps("\x1b["); /* '0x1b', '[' */

#endif
}

static void ANSI_ClrEndOfLine(void)
{
#if (DEVICE_CONSOLE_OUT)

   ANSI_CmdSeq();
   pc('K');

#endif
}

static void ANSI_ClrScreen(void)
{
#if (DEVICE_CONSOLE_OUT)

   ANSI_CmdSeq();
   ps("2J");

#endif
}

static void ANSI_GotoXY(int x, int y)
{
#if (DEVICE_CONSOLE_OUT)

   int i;

   ANSI_CmdSeq();
   /* output x,';',y,'H' (example "0;0H") */
  #if 1
   /* don't use sprintf() since it requires a lot of stack */
   _itoa(x, ANSI_buf, 10);
   i = strlen(ANSI_buf);
   ANSI_buf[i] = ';';
   _itoa(y, ANSI_buf+i+1, 10);
   i = strlen(ANSI_buf);
   ANSI_buf[i] = 'H';
   ANSI_buf[i+1] = 0;
  #else
   sprintf(ANSI_buf,"%d;%dH",x,y);
  #endif
   ps(ANSI_buf);

#else
   /* Avoid compiler warning about unused pars. */
   (void) x;
   (void) y;

#endif
}

static void ANSI_SetGMode(int gm)
{
#if (DEVICE_CONSOLE_OUT)

   int i;

   ANSI_CmdSeq();
   /* output gm,'m' (example "37m") */
  #if 1
   /* don't use sprintf() since it requires a lot of stack */
   _itoa(gm, ANSI_buf, 10);
   i = strlen(ANSI_buf);
   ANSI_buf[i] = 'm';
   ANSI_buf[i+1] = 0;
  #else
   sprintf(ANSI_buf,"%dm",gm);
  #endif
   ps(ANSI_buf);

#else
   /* Avoid compiler warning about unused pars. */
   (void) gm;

#endif
}

static void ANSI_GModeReset(void)
{
#if (DEVICE_CONSOLE_OUT)

   ANSI_CmdSeq();
   ps("0m");

#endif
}

#endif /* USES_UART */


#if (DEVICE_CONSOLE_OUT) || defined(SMX_PROBE)

/*
 *  DISPLAY BUFFER FUNCTIONS
 *  These functions are used in combination with sputs() to display
 *  information on sp_screen.
 */
void _cdecl clrb(int col)  /* clear buf[] from col to end */
{
   int    i;

   for(i = col; i < DISPLAY_COLUMNS_MAX; i++)
      buf[i] = ' ';

   buf[i] = 0;  /* NUL term. */
}


void _cdecl clrbn(const char *p) /* clear buf[] from next NUL to end */
{
   int    i;

   i = (int)(p - buf);

   for(; i < DISPLAY_COLUMNS_MAX && buf[i]; i++) {}

   for(; i < DISPLAY_COLUMNS_MAX; i++)
      buf[i] = ' ';

   buf[i] = 0;  /* NUL term. */
}


/* Fill buf from col with string and return last col position. Also
   clear preceding null characters, if any. Last col position has null. */

int _cdecl fillb(int col, const char *string)
{
   int         i;
   const char *p;

   for(i = 0; i < col; i++)  /* clear preceding null's */
      if(!buf[i]) buf[i] = ' ';

   p = string;               /* load string into buf */
   for(i = col; i < DISPLAY_COLUMNS_MAX && *p; i++)
      buf[i] = *p++;
   buf[i] = 0;
   if(i == DISPLAY_COLUMNS_MAX && *p)
      for(i = DISPLAY_COLUMNS_MAX-3; i < DISPLAY_COLUMNS_MAX; i++)
         buf[i] = '>';
   return(i);
}


int _cdecl null_after(int col)
{
   int    i;

   for(i = col; buf[i] && i < 160; i++) {}
   return(i);
}

#endif /* DEVICE_CONSOLE_OUT || SMX_PROBE */


/* Notes:

   1. crt_is_init prevents attempts to write to the terminal before the
      UART is initialized. ainit() calls bspInitPeripherals() which
      initializes the UART, before calling bspInitConsoleOut().
*/

