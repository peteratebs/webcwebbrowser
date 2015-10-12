/*
* BSP.H                                                      Version 3.7.1
*
* Board Support Package API Module for NXP (Philips) LPC24xx processors.
* All are similar so just use conditionals here for differences.
* Currently supported:  Embedded Artists LPC2468 OEM Board,
*                       Embedded Artists LPC2478 OEM Board.
*                       IAR LPC2478 KickStart / Olimex Board.
*
* Contains BSP-specific defines, types, prototypes, and configuration
* settings.
*
* See XSMX\XBSP.H.
*
* Copyright (c) 2002-2009 Micro Digital Associates, Inc.
*               All Rights Reserved.
*
* Author:  Wan Xingsheng
*
**************************************************************************/

#ifndef SMX_BSP_H
#define SMX_BSP_H

#include "armdefs.h"  /* for processor defines */

#ifdef __cplusplus
 extern "C" {
#endif

/* Configuration */

#define BSP_CPU_MHZ   ((PLLCLK + 500000) / 1000000)

#define BSP_TICKS_PER_SEC 100

#define CONSOLE_UART_BAUD 115200

/* Console Configuration. See documentation in XSMX\xbsp.h. */
#if defined(BOARD_EA_LPC2468)
#define DEVICE_CONSOLE_IN            1
#define DEVICE_CONSOLE_IN_PORT       1
#define DEVICE_CONSOLE_IN_USES_INT   0
#define DEVICE_CONSOLE_OUT           1
#define DEVICE_CONSOLE_OUT_PORT      1
#define DEVICE_CONSOLE_OUT_USES_INT  0
#elif defined(BOARD_EA_LPC2478)
#define DEVICE_CONSOLE_IN            1
#define DEVICE_CONSOLE_IN_PORT       2
#define DEVICE_CONSOLE_IN_USES_INT   0
#define DEVICE_CONSOLE_OUT           1
#define DEVICE_CONSOLE_OUT_PORT      2
#define DEVICE_CONSOLE_OUT_USES_INT  0
#elif defined(BOARD_IAR_LPC2478KS)
#define DEVICE_CONSOLE_IN            1
#define DEVICE_CONSOLE_IN_PORT       0
#define DEVICE_CONSOLE_IN_USES_INT   0
#define DEVICE_CONSOLE_OUT           1
#define DEVICE_CONSOLE_OUT_PORT      0
#define DEVICE_CONSOLE_OUT_USES_INT  0
#endif

#define DEBUGGER_IRQ_RX -1       /* -1 for unused (i.e. JTAG) */
#define DEBUGGER_IRQ_TX -1       /* -1 for unused (i.e. JTAG) */
/*
   NOTE: Keep these -1 and use a JTAG unit instead. We were unsuccessful to
         get SMX working with Angel and we have discontinued work on it.
         We currently have no plans to support any software debug monitor.

   Set to match the IRQs used by the debug monitor so the Protosystem
   can unmask them. This allows the debugger's Stop button to work, and
   may be needed for normal operation of some debuggers. Note that the
   debug monitor is independent of the application and could be set to
   any value, so that is why this can't be automatic.
*/

#define MIN_RAM        0         /* Minimize RAM use if 1. See documentation in XSMX\xbsp.h. */


/* Defines */

#define MIN_IRQ_NUM    0         /* there are 32 irq's; we number 0 to 31 */
#define MAX_IRQ_NUM    31

#define MIN_INT_NUM              /* not used */
#define MAX_INT_NUM              /* not used */
#define NUM_IRQS       (MAX_IRQ_NUM-MIN_IRQ_NUM+1)
#define NUM_INTS       (MAX_INT_NUM-MIN_INT_NUM+1)  /* not used */
#define MIN_IRQ_PRI    0
#define MAX_IRQ_PRI    15

#define IRQ_VAL_TIMER0    4
#define IRQ_VAL_TIMER1    5
#define IRQ_VAL_TIMER2    26
#define IRQ_VAL_TIMER3    27

#define TICK_IRQ  IRQ_VAL_TIMER0

#if (DEVICE_CONSOLE_IN || DEVICE_CONSOLE_OUT)
#if (DEVICE_CONSOLE_IN_PORT != DEVICE_CONSOLE_OUT_PORT) && (DEVICE_CONSOLE_OUT_PORT != -1)
#error Set both console ports to be the same or see note here in bsp.h.
/* Or change bspInitPeripherals() to init more than 1 UART. */
#endif
#endif


/* Typedefs */

typedef struct
{
   byte  pri;     /* interrupt priority (0) */
   /* no need for vector number since easy to calculate from irq */
} IRQ_REC;


/* Macros */


/* Platform-Specific Functions (bspp) */

BOOLEAN bsppWriteIRQTableEntry(int irq_num, int pri);


/* Local Functions (bspl) */

#if defined(__ICCARM__)
void bsplDelayLoop(dword num);  /* in bspiar.s */
#endif


#ifdef __cplusplus
 }
#endif

#endif /* SMX_BSP_H */
