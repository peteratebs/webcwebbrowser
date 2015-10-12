/*
* ARMDEFS.H                                                 Version 3.7.1
*
* ARM main definitions file. Contains:
*
*    1. Includes of the appropriate processor .h files for I/O register
*       definitions and function prototypes.
*
*    2. Processor peripheral configuration settings.
*
* Copyright (c) 2001-2009 Micro Digital Associates, Inc.
*               All Rights Reserved.
*
* Author: David Moore
*
***************************************************************************/

#ifndef SMX_ARMDEFS_H
#define SMX_ARMDEFS_H


/*--- Derived Defines ----------------------------------------------------*/
/*
   This first section defines additional preprocessor symbols based
   on those passed in on the command line or via files. These are
   defined first so they can be used by the sections that follow,
   and by the header files that are included below.
*/

/* Atmel AT91 processors */
/*
   Atmel AT91X40 processors are very much the same (they all share 1
   datasheet for the bulk of the info). Define PROCESSOR_AT91X40 to
   avoid needless repetition.
*/
#if defined(PROCESSOR_AT91M40800) || defined(PROCESSOR_AT91M40807) || \
    defined(PROCESSOR_AT91R40807) || defined(PROCESSOR_AT91R40008) || \
    defined(PROCESSOR_AT91F40816) || defined(PROCESSOR_AT91FR4081)
#define PROCESSOR_AT91
#define PROCESSOR_AT91X40
#define PROCESSOR_AT91LIB
#endif

#if defined(PROCESSOR_AT91M43300)  || defined(PROCESSOR_AT91M63200)  || \
    defined(PROCESSOR_AT91M42800A) || defined(PROCESSOR_AT91M55800A) || \
    defined(PROCESSOR_AT91RM3400)
#define PROCESSOR_AT91
#define PROCESSOR_AT91LIB
#endif

/* Newer processors are not supported by AT91 library. */
#if defined(PROCESSOR_AT91SAM7) || defined(PROCESSOR_AT91SAM9) || \
    defined(PROCESSOR_AT91RM9200)
#define PROCESSOR_AT91
#endif


/*--- Master Includes ----------------------------------------------------*/

#if defined(BOARD_ARM_EVALUATOR_7T)
#include "uhal.h"                      /* Master .h for board and processor */
#endif

/* Note: Don't change order of Atmel includes! lib_.h must precede reg_.h
         since reg_.h defines macros with the same names as fields defined
         in headers included by lib_.h. Otherwise, those field names will
         be changed by the macros, which is wrong.
*/
#if defined(BOARD_ATMEL_AT91EB40A)
#include "parts\r40008\lib_r40008.h"   /* Master .h for AT91 library for this processor */
#include "parts\r40008\reg_r40008.h"   /* Processor registers */
#include "targets\eb40a\eb40a.h"       /* Master .h for board and processor (they should have made this include the other 2) */
#endif
#if defined(BOARD_ATMEL_AT91EB55)
#include "parts\m55800\lib_m55800.h"   /* Master .h for AT91 library for this processor */
#include "parts\m55800\reg_m55800.h"   /* Processor registers */
#include "targets\eb55\eb55.h"         /* Master .h for board and processor (they should have made this include the other 2) */
#endif

#if defined(PROCESSOR_AT91RM)
#include "at91rm.h"
#endif

#if defined(PROCESSOR_AT91SAM7)
#include "at91sam7.h"
#endif

#if defined(PROCESSOR_AT91SAM9)
#include "at91sam9.h"
#endif

#if defined(PROCESSOR_COREMP7)
#include "coremp7.h"
#endif

#if defined(PROCESSOR_EP93XX)
#include "ep93xx.h"
#endif

#if defined(PROCESSOR_IMX31)
#include "imx31.h"
#endif

#if defined(PROCESSOR_LH75401)
//#include "lh754xx_timer_driver.h"    /* conflicts with smxNet defs */
#include "lh754xx_vic_driver.h"
#include "lh75401uart.h"
#endif

#if defined(PROCESSOR_LH79520)
#include "lh79520bd.h"
#endif

#if defined(PROCESSOR_LH79524)
#include "lh79524bd.h"
#endif

#if defined(BOARD_LOGICPD_LH7A400)
#include "abl_types.h"
#include "abl_arm922t_cp15_driver.h"
#include "lh7a400_csc.h"
#include "lh7a400_gpio.h"
#include "lh7a400_int_driver.h"
#include "lh7a400_timer_driver.h"
#include "lh7a400_uart_driver.h"
#include "sdk7a400_cpld_driver.h"
#endif

#if defined(BOARD_LOGICPD_LH7A404)
#include "abl_types.h"
#include "abl_arm922t_cp15_driver.h"
#include "lh7a404_gpio.h"
#include "lh7a404_timer_driver.h"
#include "lh7a404_uart_driver.h"
#include "lh7a404_vic_driver.h"
#include "sdk7a404_cpld_driver.h"
#endif

#if defined(PROCESSOR_LM3S)
#include "hw_memmap.h"
#include "hw_types.h"
#include "sysctl.h"
#include "gpio.h"
#include "uart.h"
#endif

#if defined(PROCESSOR_LPC2XXX)
#include "lpc2xxx.h"
#endif

#if defined(PROCESSOR_LPC23XX)
#include "lpc23xx.h"
#endif

#if defined(PROCESSOR_LPC24XX)
#include "lpc24xx.h"
#endif

#if defined(PROCESSOR_LPC288X)
#include "lpc288x.h"
#endif

#if defined(PROCESSOR_LPC3XXX)
#include "lpc3xxx.h"
#endif

#if defined(PROCESSOR_LPC32X0)
#include "lpc32x0.h"
#endif

#if defined(PROCESSOR_M9328MX1_MXL)
#include "mx1_def.h"
#include "mx1uart1.h"
#endif

#if defined(PROCESSOR_MAC71XX)
#include "iomac71xx.h"
#endif

#if defined(PROCESSOR_S3C2XXX)
#include "s3c2xxx.h"
#endif

#if defined(PROCESSOR_STM32F10X)
#include "stm32f10x_lib.h"
#endif

#if defined(PROCESSOR_STR71x)
#include "71x_lib.h"
#endif

#if defined(PROCESSOR_STR75X)
#include "75x_lib.h"
#endif

#if defined(PROCESSOR_STR91X)
#include "str91x.h"
#endif

#if defined(PROCESSOR_TMS470)
#include "tms470.h"
#endif


/*--- Other Defines ------------------------------------------------------*/

#if !defined(ARMM)
/* IRQ Dispatch in Hardware or Software */
/*
   Some ARM's support hardware vectoring of interrupts. For these, our
   ENTER_ISR and EXIT_ISR macros are used. Otherwise, smx_irq_handler
   is hooked to the IRQ slot and it calls the IRQ dispatcher routine.
*/
#if defined(PROCESSOR_AT91) || \
    defined(PROCESSOR_LH75401) || defined(PROCESSOR_LH79520) || defined(PROCESSOR_LH79524) || \
    defined(PROCESSOR_LPC2XXX) || defined(PROCESSOR_LPC23XX) || defined(PROCESSOR_LPC24XX) || \
    defined(PROCESSOR_STR91X)
#define IRQ_DISPATCH_IN_HDW 1
#else /* PROCESSOR_LH7A404, and others */
#define IRQ_DISPATCH_IN_HDW 0
#endif
#endif /* ARMM */

#endif /* SMX_ARMDEFS_H */
