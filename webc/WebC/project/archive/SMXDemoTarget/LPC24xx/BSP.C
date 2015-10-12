/*
* BSP.C                                                      Version 3.7.1
*
* Board Support Package API Module for NXP (Philips) LPC24xx processors.
* All are similar so just use conditionals here for differences.
* Currently supported:  Embedded Artists LPC2468 OEM Board,
*                       Embedded Artists LPC2478 OEM Board.
*                       IAR LPC2478 KickStart / Olimex Board.
*
* See XSMX\XBSP.H.
*
* USER: We recommend that you delete conditional sections for other
*       processors to make this module simpler and easier to read.
*
* Copyright (c) 2001-2009 Micro Digital Associates, Inc.
*               All Rights Reserved.
*
* Author:  Wan Xingsheng
*
*-------------------------------------------------------------------------
*
* Notes:
*
* 1. IRQ Numbering Convention: IRQ's are numbered according to:
*
*    LPC2468: Table 91 Connection of interrupt sources to the Vectored
*             Interrupt Controller in the section 7.5 Vectored Interrupt
*             Controller (VIC) / Interrupt sources of LPC2468 User Manual.
*    LPC2478: Table 106 Connection of interrupt sources to the Vectored
*             Interrupt Controller in the section 7.4 Vectored Interrupt
*             Controller (VIC) / Interrupt sources of LPC2478 User Manual.
*
*    This corresponds to the bit positions in the Interrupt Enable register
*    (and similar registers) for each processor.
*
* 2. IRQ Priorities: 0 to 15 (0 highest)
*
**************************************************************************/

#include "smx.h"
#include "bsp.h"
#include "core.h"
#include <time.h>

#ifdef __cplusplus
 extern "C" {
#endif

/* Global Variables */

#if SMX_EVB && defined(SMXAWARE)
/*
   These are used by smxAware to convert timestamps to actual times.
   IMPORTANT: hz and max_count must match config of timer used for tick.
*/

dword evb_clk_counts_up = 1;             /* 1 if clock counts up; 0 if it counts down */
dword evb_clk_hz = PCLK;                 /* frequency of timer/counter used for timestamps (input clock frequency divided by prescalers) */
dword evb_clk_max_count = PCLK / BSP_TICKS_PER_SEC;  /* number of counts per rollover */
dword evb_clk_method = EVB_CLK_HI_PREC;  /* event clocking method (defined in xevb.h) */
#endif

/* Local Variables */
STATIC PROC_FLAGS_TYPE saved_int_state;  /* stores interrupt flag state before bspMaskIRQs() */
STATIC dword           saved_int_mask;   /* stores the previous contents of the main interrupt mask register, which shows which interrupts are enabled */

/* External Variables */

/* External Functions */
#if defined(SMXUSB) || defined(SMXUSBD)
extern void LPC24XXUSBInitInterrupt(void);
#endif


/* IRQ Table */
/*
   This is the single place where all irq information should be configured.
   Having it all here helps avoid double-assignment of priorities and
   vector numbers. If you need the ability to change these dynamically
   and re-set them in the hardware, call bsppWriteIRQTableEntry() to change
   the entry and then bspConfigureIRQ() to write it to the hardware.
*/
/* The LPC24xx Vectored Interrupt Controller (VIC) takes 32 interrupt
   request inputs and has 16 programmable interrupt priority levels. 
   When more than one interrupt is assigned the same priority and occur
   simultaneously, the one connected to the lowest numbered VIC channel
   will be serviced first.
*/
IRQ_REC irq_table[NUM_IRQS] =
{
   /* pri      IRQ  Summary */
   /* ---      ---  ------- */
   { 99 },  /*   0  Watchdog Interrupt (WDINT) */
   { 99 },  /*   1  Reserved for Software Interrupts only */
   { 99 },  /*   2  Embedded ICE, DbgCommRx */
   { 99 },  /*   3  Embedded ICE, DbgCommTx */
   {  0 },  /*   4  Timer 0 (Match 0-1 Capture 0-1) */ /* used for smx tick */
   {  2 },  /*   5  Timer 1 (Match 0-2 Capture 0-1) */
   {  3 },  /*   6  UART 0 (RLS, THRE, RDA, CTI) */
   {  3 },  /*   7  UART 1 (RLS, THRE, RDA, CTI, MSI) */
   { 99 },  /*   8  PWM 0 & 1 (Match 0-6 Capture 0-1) */
   { 99 },  /*   9  I2C 0 (SI) */
   { 99 },  /*  10  SPI, SSP 0 */
   { 99 },  /*  11  SSP 1 */
   { 99 },  /*  12  PLL lock (PLOCK) */
   { 99 },  /*  13  RTC (RTCCIF, RTCALF) */
   { 99 },  /*  14  External interrupt 0 (EINT0) */
   { 99 },  /*  15  External interrupt 1 (EINT1) */
   { 99 },  /*  16  External interrupt 2 (EINT2) */
   { 99 },  /*  17  External interrupt 3 (EINT3) */
   { 99 },  /*  18  A/D Converter 0 end of conversion */
   { 99 },  /*  19  I2C 1 (SI) */
   { 99 },  /*  20  Brown Out detect */
   {  4 },  /*  21  Ethernet */
   {  1 },  /*  22  USB Low and High priority */
   { 99 },  /*  23  CAN 0, CAN 1 (Tx, Rx) */
   {  1 },  /*  24  SD/MMC */
   { 99 },  /*  25  DMA channel 0, DMA channel 1 */
   {  2 },  /*  26  Timer 2 (Match 0-3 Capture 0-3) */
   {  2 },  /*  27  Timer 3 (Match 0-3 Capture 0-3) */
   {  3 },  /*  28  UART 2 (RLS, THRE, RDA, CTI) */
   {  3 },  /*  29  UART 3 (RLS, THRE, RDA, CTI) */
   { 99 },  /*  30  I2C 2 (SI) */
   { 99 }   /*  31  I2S (Rx, Tx) */

   /* 99 means an unused row (any value > MAX_IRQ_PRI would work) */
};

#define UNUSED_IRQ_REC 99  /* use same value in irq_table[] above */

#ifdef __cplusplus
 }
#endif


/*------ bspInitClocks(void)
*
* Documented in XBSP.H.
*
* Differences from Spec: none
*
----------------------------------------------------------------------*/

BOOLEAN bspInitClocks(void)
{
   /* NOTE: Must agree with BSP_xxx_HZ/_MHZ settings in bsp.h and LPC24xx.h. */
    
   if (PLLSTAT & (1 << 25))
   {
      /* PLL Enabled, But PLL is Disconnected */
      PLLCON  = 1;
      PLLFEED = 0xAA;
      PLLFEED = 0x55;
   }

   /* PLL is Disabled, PLL is Disconnected */
   PLLCON = 0;
   PLLFEED = 0xAA;
   PLLFEED = 0x55;
   
   /* Enable main oscillator */
   SCS = 0x20;

   /* Wait for main oscillator stable */
   while (!(SCS & 0x40)) {}

   /* Set PLL source clock, select main oscillator */
   CLKSRCSEL = 0x01;

   /* PLL output clock = (MOSC * M) / N */
   /*         N_SEL (bit 23:16) | N_SEL (bit 14:0) */
   PLLCFG = ((PLL_N - 1) << 16) | (PLL_M - 1);

   /* Set PLL Feed register */
   PLLFEED = 0xAA;
   PLLFEED = 0x55;

   /* Set PLL Enable */
   PLLCON  = 0x1;

   /* Set PLL Feed register */
   PLLFEED = 0xAA;
   PLLFEED = 0x55;
   
   /* Set CPU clock divider */
   CCLKCFG = CCLK_DIV - 1;

   /* Set USB clock divider */
   USBCLKCFG = USBCLK_DIV - 1;

   /* Wait until PLL Locked */
   while (((PLLSTAT & (1 << 26)) == 0)) {}

   /* Wait until M and N are correct */
   while ((PLLSTAT & 0xFF7FFF) != (((PLL_N - 1) << 16) | (PLL_M - 1))) {}

   /* Set Peripheral Clock. Every tow bit for a Peripheral. We set PCLK = CCLK / 2. (See LPC24xx.h)
      00: PCLK = CCLK / 4;
      01: illegal value.
      10: PCLK = CCLK / 2;
      11: PCLK = CCLK / 8;
   */
   PCLKSEL0 = 0xAAAAAAAA; 
   PCLKSEL1 = 0x22AAA8AA; /* Bit 31:30, 27:26, 9:8 are unused. */

   /* Switch to PLL Clock and Enable it. */
   PLLCON  = 0x3;

   /* Set PLL Feed register */
   PLLFEED = 0xAA;
   PLLFEED = 0x55;

   /* Check connect bit status */
   while (((PLLSTAT & (1 << 25)) == 0)) {}

   return(TRUE);
}


/*------ bspInitConsoleIn(void)
*
* Documented in XBSP.H.
*
* Differences from Spec: none
*
----------------------------------------------------------------------*/

BOOLEAN bspInitConsoleIn(void)
{
   /* UART is initialized in bspInitPeripherals. */

  #if (DEVICE_CONSOLE_IN) && !(defined(PEG) && defined(WIN32))
   kbd_init();
  #endif

   return(TRUE);
}


/*------ bspInitConsoleOut(void)
*
* Documented in XBSP.H.
*
* Differences from Spec: none
*
----------------------------------------------------------------------*/

BOOLEAN bspInitConsoleOut(void)
{
   /* UART is initialized in bspInitPeripherals. */

  #if (DEVICE_CONSOLE_OUT)
   crt_init();        /* ANSI command to reset terminal */
   clr_screen_isr();  /* ANSI command to clear screen. isr version used because we're not
                         yet in tasking environment (and normal version calls unlockx()) */
  #endif

   return(TRUE);
}


/*------ bspInitInterruptCtrl(void)
*
* Documented in XBSP.H.
*
* Differences from Spec: none
*
----------------------------------------------------------------------*/

BOOLEAN bspInitInterruptCtrl(void)
{
   dword* pdwVectAddr;
   dword* pdwVectPriority;
   int    nNum;

   /* Clear IRQ protection */
   VICProtection = 0;

   /* Disable all IRQs */
   VICIntEnClr = 0xFFFFFFFF;

   /* Clear the IRQ Service Routine function Address */
   VICVectAddr = 0;
   
   /* Set all interrupts are not FIQ */
   VICIntSelect = 0;
   
   /* Set all interrupts are not Software IRQ */
   VICSoftIntClr = 0xFFFFFFFF;

   /* Initialize Vector Interrupts */
   pdwVectAddr     = (dword*)&VICVectAddr0;
   pdwVectPriority = (dword*)&VICVectPriority0;
   for (nNum = MIN_IRQ_NUM; nNum <= MAX_IRQ_NUM; nNum++)
   {
       pdwVectAddr[nNum]     = 0;
       pdwVectPriority[nNum] = 0;
   }

   return(TRUE);
}


/*------ bspInitPeripherals(void)
*
* Documented in XBSP.H.
*
* Differences from Spec: none
*
----------------------------------------------------------------------*/

BOOLEAN bspInitPeripherals(void)
{
   /* Call peripheral init routines. Add others, as desired. Note that
      products such as smxNet initialize the hardware they use.
   */

   InitLEDs();

   /* The first timer is used for tick and is initialized by bspInitTick(). */

   /* Initialize other timers, UARTs, etc here */
#if defined(BOARD_EA_LPC2468)
   Uart1Init(CONSOLE_UART_BAUD);
#endif
#if defined(BOARD_EA_LPC2478)
   Uart2Init(CONSOLE_UART_BAUD);
#endif
#if defined(BOARD_IAR_LPC2478KS)
   Uart0Init(CONSOLE_UART_BAUD);
#endif

  #if defined(SMXUSB) || defined(SMXUSBD)
   /* Install USB dispatcher. Host and Device share same IRQ. */
   LPC24XXUSBInitInterrupt();
  #endif

   return(TRUE);
}


/*------ bspInitTick(void)
*
* Documented in XBSP.H.
*
* Differences from Spec: none
*
* Notes:
* 1. What we call the IRQ number is the same as their peripheral ID.
*
----------------------------------------------------------------------*/

BOOLEAN bspInitTick(void)
{

   /* Note: If anything is changed here, ensure the evb_ globals are set
            correctly at the top of this file.
   */

   /* LPC24xx has four 32-bit Timer/Counters which are identical except
      for the peripheral base address. The Counters is incremented every
      PR(Prescale Register)+1 cycles of PCLK. The Counter can be reset 
      when MR0-2 is matched.
      
      We use Timer/Counters 0 as smx tick timer.
   */

   /* Disable Timer 0 and reset Timer 0 Counter */
   T0TCR = 0x2;

   /* Set Prescale Register as 0. Timer Counter increases every rising PCLK edge. */
   T0PR  = 0;

   /* Enable Interrupt and Reset when matches on Timer 0 MR0 */
   T0MCR = 0x3;

   /* Set the match compare value for match register 0. */
   T0MR0 = PCLK / BSP_TICKS_PER_SEC;

   /* Set Timer Mode: Every rising PCLK edge */
   T0CTCR = 0;

   /* enable interrupt and Set Timer Routine function tick */
   bspSetIRQVect(TICK_IRQ, tick);
   bspConfigureIRQ(TICK_IRQ);
   bspUnmaskIRQ(TICK_IRQ);

   /* Enable Timer */
   T0TCR  = 0x1;   

   return(TRUE);
}


/*
*  bspGetIntVect(), bspSetIntVect(), bspSetTrapVect(), and bspIRQToInt()
*  are not implemented for this processor. They are irrelevant since there
*  is no interrupt vector table. The only vectors are IRQ's.
*  Software interrupts are independent of IRQ's; see discussion of
*  the SWI instruction in the ARM documentation.
*/


/*------ bspClearIRQ(irq_num)
*
* Documented in XBSP.H.
*
* Differences from Spec: none
*
----------------------------------------------------------------------*/

BOOLEAN bspClearIRQ(int irq_num)
{
   BOOLEAN retcode = TRUE;

   switch (irq_num)
   {
      /* Add IRQ's here, as needed. */

      default:
         retcode = FALSE;
         break;
   }

   return(retcode);
}


/*------ bspConfigureIRQ(irq_num)
*
* Documented in XBSP.H.
*
* Differences from Spec: none
*
----------------------------------------------------------------------*/

BOOLEAN bspConfigureIRQ(int irq_num)
{
   /* This function does nothing. Priority is established in bspSetIRQVect() */

   if (irq_num < MIN_IRQ_NUM || irq_num > MAX_IRQ_NUM)
      return(FALSE);

   /* Check priority level of this entry in irq_table[]. */
   if ((uint)(irq_table[irq_num].pri > MAX_IRQ_PRI))
      return(FALSE);

      

   return(TRUE);
}


/*------ bspEndOfIRQ(irq_num)
*
* Documented in XBSP.H.
*
* Differences from Spec: none
*
----------------------------------------------------------------------*/

#if defined(__IAR_SYSTEMS_ICC__)
#pragma diag_suppress=Pe550  /* avoid warning about dummy variable set but not used */
#endif

BOOLEAN bspEndOfIRQ(int irq_num)
{
   BOOLEAN retcode = TRUE;

   switch (irq_num)
   {
      /* Add other IRQ's here, as needed. */
      case TICK_IRQ:
         T0IR = 0xFF; /* Clear the interrupt status */
         break;
      default:
         retcode = FALSE;
         break;
   }
   VICVectAddr = 0;
   return(retcode);
}

#if defined(__IAR_SYSTEMS_ICC__)
#pragma diag_default=Pe550  /* restore warning */
#endif

/*------ bspGetIRQVect(irq_num, extra_info)
*
* Documented in XBSP.H.
*
* Differences from Spec: none
*
----------------------------------------------------------------------*/

ISR_PTR bspGetIRQVect(int irq_num, dword * extra_info)
{
   dword* pdwVectAddr;

   (void)extra_info;

   if (irq_num < MIN_IRQ_NUM || irq_num > MAX_IRQ_NUM)
      return(0);

   pdwVectAddr     = (dword*)&VICVectAddr0;

   /* return addresses of the Interrupt Service routines */
   return((ISR_PTR)pdwVectAddr[irq_num]);
}


/*------ bspSetIRQVect(irq_num, isr_ptr)
*
* Documented in XBSP.H.
*
* Differences from Spec: none
*
----------------------------------------------------------------------*/

BOOLEAN bspSetIRQVect(int irq_num, ISR_PTR isr_ptr)
{
   dword* pdwVectAddr;
   dword* pdwVectPriority;

   if (irq_num < MIN_IRQ_NUM || irq_num > MAX_IRQ_NUM)
      return(FALSE);

   /* Check priority level of this entry in irq_table[]. */
   if ((uint)(irq_table[irq_num].pri > MAX_IRQ_PRI))
      return(FALSE);

   /* Hook the vector and set priority. */
   bspMaskIRQ(irq_num);
   
   pdwVectAddr     = (dword*)&VICVectAddr0;
   pdwVectPriority = (dword*)&VICVectPriority0;
   
   /* Instructions specify setting the source and priority */
   pdwVectAddr[irq_num]     = (dword)isr_ptr;
   pdwVectPriority[irq_num] = irq_table[irq_num].pri;
   
   return(TRUE);
}


/*------ bspMaskIRQ(irq_num)
*
* Documented in XBSP.H.
*
* Differences from Spec: none
*
----------------------------------------------------------------------*/

BOOLEAN bspMaskIRQ(int irq_num)
{
   //PROC_FLAGS_TYPE prev_int_state;
   
   if (irq_num < MIN_IRQ_NUM || irq_num > MAX_IRQ_NUM)
      return(FALSE);
   
   //Not necessary to disable/enable interrupts because this is done
   //with a single instruction (write, rather than read-modify-write)
   //so it can't be interrupted. This is the advantage of having
   //separate registers for disable and enable.
   //
   //prev_int_state = bspSaveIntStateAndDisable();  /* DISABLE ints */
   
   /* Set Enable/Disable bit */
   VICIntEnClr  = 1 << irq_num;
   
   //bspRestoreIntState(prev_int_state);            /* ENABLE ints (if previously enabled) */

   return(TRUE);
}


/*------ bspUnmaskIRQ(irq_num)
*
* Documented in XBSP.H.
*
* Differences from Spec: none
*
----------------------------------------------------------------------*/

BOOLEAN bspUnmaskIRQ(int irq_num)
{
   //PROC_FLAGS_TYPE prev_int_state;
   
   if (irq_num < MIN_IRQ_NUM || irq_num > MAX_IRQ_NUM)
      return(FALSE);
   
   /* Check priority level of this entry in irq_table[]. */
   if ((uint)(irq_table[irq_num].pri > MAX_IRQ_PRI))
      return(FALSE);

   //Not necessary to disable/enable interrupts because this is done
   //with a single instruction (write, rather than read-modify-write)
   //so it can't be interrupted. This is the advantage of having
   //separate registers for disable and enable.
   //
   //prev_int_state = bspSaveIntStateAndDisable();  /* DISABLE ints */

   VICIntEnable = 1 << irq_num;

      //bspRestoreIntState(prev_int_state);            /* ENABLE ints (if previously enabled) */
   return(TRUE);
}


/*------ bspMaskIRQs(void)
*
* Documented in XBSP.H.
*
* Differences from Spec: none
*
----------------------------------------------------------------------*/

BOOLEAN bspMaskIRQs(void)
{
   /* DISABLE interrupts after saving current interrupt state */
   saved_int_state = bspSaveIntStateAndDisable();

   /* Save current interrupt mask priority level. See note above. */
   saved_int_mask = VICIntEnable;
   
   /* Disable all interrupt */
   VICIntEnClr  = 0xFFFFFFFF;

   /* Interrupts are still DISABLED. bspUnmaskIRQs() will restore
      the previous interrupt state.
   */
   return(TRUE);
}


/*------ bspUnmaskIRQs(void)
*
* Documented in XBSP.H.
*
* Differences from Spec: none
*
----------------------------------------------------------------------*/

BOOLEAN bspUnmaskIRQs(void)
{
   /* DISABLE interrupts. The original interrupt state was saved in
      bspMaskIRQs() to be restored below.
   */
   DISABLE();

   /* Restore all interrupts that were previously unmasked. */
   VICIntEnable = saved_int_mask;

   /* ENABLE interrupts if they were enabled before the call to bspSaveIntStateAndDisable()
      in bspMaskIRQs().
   */
   bspRestoreIntState(saved_int_state);

   return(TRUE);
}


/*------ bspRestoreIntState(prev_state)
*
* Documented in XBSP.H.
*
* Differences from Spec: none
*
----------------------------------------------------------------------*/

#if defined(__GNUC__)
/* Need to compile the whole file for ARM mode for GNU. */
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma type_attribute=__arm
#else
#error Add ARM pragma here for your compiler.
#endif

void bspRestoreIntState(PROC_FLAGS_TYPE prev_state)
{
  #if defined(__CC_ARM) || defined(__MWERKS__)
   asm {msr  CPSR_c, prev_state} /* Restore current processor status register */
  #elif defined(__GNUC__)
   /* prev_state is in r0 since 1st par is passed in r0 */
   asm("msr  CPSR_c, r0\n\t");   /* Restore current processor status register */
  #elif defined(__ICCARM__)
   /* prev_state is in r0 since 1st par is passed in r0 */
   asm("msr  CPSR_c, r0");       /* Restore current processor status register */
  #endif
}


/*------ bspSaveIntStateAndDisable(void)
*
* Documented in XBSP.H.
*
* Differences from Spec: none
*
----------------------------------------------------------------------*/

#if defined(__MWERKS__)
#pragma warn_uninitializedvar off  /* avoid warning about prev_state not being initialized before used, in next function */
#endif
#if defined(__IAR_SYSTEMS_ICC__)
#pragma optimize=no_inline   /* Needed for ret in r0. Applies only to next function. */
#pragma diag_suppress=Pe940  /* Avoid warning about no return value. */
#endif

#if defined(__GNUC__)
/* Need to compile the whole file for ARM mode for GNU. */
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma type_attribute=__arm
#else
#error Add ARM pragma here for your compiler.
#endif

PROC_FLAGS_TYPE bspSaveIntStateAndDisable(void)
{
  #if defined(__CC_ARM) || defined(__MWERKS__)
   PROC_FLAGS_TYPE prev_state;
   PROC_FLAGS_TYPE temp;

   asm
   {
      mrs  prev_state, CPSR         /* Save current processor status register */
      mrs  temp, CPSR               /* Get current processor status register */
     #if defined(__CC_ARM)
      orr  temp, temp, 0x80         /* Set the I-bit to disable IRQ's */
     #else
      orr  temp, temp, #0x80        /* Set the I-bit to disable IRQ's */
     #endif
      msr  CPSR_c, temp             /* Write back */
   }
   return (prev_state);

  #elif defined(__GNUC__)
   asm("mrs  r0, CPSR\n\t"          /* Save current processor status register */
       "mrs  r1, CPSR\n\t"          /* Get current processor status register */
       "orr  r1, r1, #0x80\n\t"     /* Set the I-bit to disable IRQ's */
       "msr  CPSR_c, r1\n\t");      /* Write back */
   /* return value is in r0 (surprising the compiler doesn't complain about no return statement) */

  #elif defined(__ICCARM__)
   asm("mrs  r0, CPSR");            /* Save current processor status register */
   asm("mrs  r1, CPSR");            /* Get current processor status register */
   asm("orr  r1, r1, #0x80");       /* Set the I-bit to disable IRQ's */
   asm("msr  CPSR_c, r1");          /* Write back */
   /* return value is in r0 */
  #endif
}

#if defined(__IAR_SYSTEMS_ICC__)
#pragma diag_default=Pe940  /* restore warning */
#endif

// This causes pragma above to be ignored. Reported to Metrowerks.
//#if defined(__MWERKS__)
//#pragma warn_uninitializedvar reset  /* reenable warning */
//#endif


/*------ bspAllocDMABuffer(num_bytes)
*
* Documented in XBSP.H.
*
* Differences from Spec: none
*
----------------------------------------------------------------------*/

void * bspAllocDMABuffer(uint num_bytes)
{
   return(nmallocx(num_bytes));
}


/*------ bspFreeDMABuffer(buf)
*
* Documented in XBSP.H.
*
* Differences from Spec: none
*
----------------------------------------------------------------------*/

BOOLEAN bspFreeDMABuffer(void *buf)
{
   return(nfreex(buf));
}


/*------ bspGetHeapSpace(size, flag, heap)
*
* Documented in XBSP.H.
*
* Differences from Spec: None
*
----------------------------------------------------------------------*/

#if defined(__BORLANDC__) || defined(__PARADIGM__)
#pragma warn -aus   /* turn off warning "identifier is assigned a value that is never used" */
#endif

BOOLEAN bspGetHeapSpace(dword size, uint flag, uint heap)
{
   dword near *scanptr;

   /* avoid warnings about these parameters being unused */
   size = size;
   flag = flag;
   heap = heap;

   /* Heap start and end should have been statically set, so heap start
      should not be 0. Heap end set to 0 means to scan for the end of RAM
      starting at heap start.
   */

   if (cf.nheap_min == 0L)
      return(FALSE);

   if (cf.nheap_max == 0L)
   {
      /* Do RAM scan. Set end of heap at end of RAM. */

      /* Start scan pointer at first scan incr boundary. Memory is going
         to end at an even boundary, such as 1MB, so by scanning on the
         boundary, we won't have to back up very far (probably only 1 byte)
         once we go past the end of memory.
      */
      scanptr = (dword near *)((byte near *)cf.nheap_min + RAM_SCAN_INCR);
      scanptr = (dword near *)((dword)scanptr & (dword)(-RAM_SCAN_INCR));

      /* scan up until we go past end of memory */
      for (; ; scanptr = (dword near *)((byte near *)scanptr + RAM_SCAN_INCR))
      {
         *scanptr = RAM_SCAN_PATTERN;
         if (*scanptr != RAM_SCAN_PATTERN)
            break;  /* now pointing past end of memory */
      }

      /* now scan down until first memory location hit */
      for (; ; scanptr--)
      {
         *scanptr = RAM_SCAN_PATTERN;
         if (*scanptr == RAM_SCAN_PATTERN)
            break;  /* now pointing at last dword in RAM */
      }

      /* back up a few bytes for NHCB */
      cf.nheap_max = (byte near *)scanptr + 4 - sizeof(struct NHCB);
   }
   return(TRUE);
}

#if defined(__BORLANDC__) || defined(__PARADIGM__)
#pragma warn +aus   /* turn warning back on */
#endif


/*------ bspSetStime()
*
* Documented in XBSP.H.
*
* Differences from Spec: None
*
----------------------------------------------------------------------*/

BOOLEAN bspSetStime(void)
{
#if !CC_TIME_FUNCS
   stimex = 0;  /* Just set to 0 for compilers that don't have time functions. */

#else
   struct tm time;
   static const char * months[12] = {"Jan","Feb","Mar","Apr","May","Jun",
                                     "Jul","Aug","Sep","Oct","Nov","Dec"};
   int    month;

   /* USER: Replace this case with appropriate code to set the time
      structure from your hardware clock or by prompting user, etc.

      Below is just an example that sets the time to either a fixed time
      or to the date and time of compilation of this module. Neither
      are appropriate for a real system, but are fine for demos.
   */

  #if 1
   // Use fixed time and date; using time and date string can confuse the
   // user since it will appear to be almost correct the first time they run.
   static char datestr[15] = "Jan 01 2001";
   static char timestr[10] = "00:00:00";
  #else
   static char datestr[15];
   static char timestr[10];
   strcpy(datestr,__DATE__);  // Get date of compilation in MMM DD YYYY (e.g. Jun 10 1988)
   strcpy(timestr,__TIME__);  // Get time of compilation in hh:mm:ss format (e.g. 15:32:10)
  #endif

   //_strupr(datestr);
   for (month = 0; month < 12; ++month)    // Search for matching month string.
      if (memcmp(months[month],datestr,3) == 0)
         break;

   memset(&time, 0, sizeof(struct tm));    // clear time structure
   time.tm_mon  = month;                   // Month  (0 - 11)
   time.tm_mday = atoi(datestr+4);         // Day of month (1 - 31)
   time.tm_year = atoi(datestr+7) - 1900;  // Year since 1900
   time.tm_hour = atoi(timestr);           // Hour   (0 - 23)
   time.tm_min  = atoi(timestr+3);         // Minute (0 - 59)
   time.tm_sec  = atoi(timestr+6);         // Second (0 - 59)

   /* The time structure has been set. Now initialize stimex from it. */

   stimex = mktime(&time);
   if (stimex == (dword) -1)
   {
      stimex = 0;
      WR_STRING(0,8,LIGHTRED,BLACK,!BLINK,"time/date conversion failed in init_stime()");
      return(FALSE);
   }
#endif /* CC_TIME_FUNCS */

   return(TRUE);
}


/*------ bspDelayUsec(num)
*
* Documented in XBSP.H.
*
* Differences from Spec: None
*
* Notes:
* 1. This is not an accurate delay but will delay AT LEAST as long
*    as specified. See the discussion in XBSP.H.
*
----------------------------------------------------------------------*/

void bspDelayUsec(dword num)
{
   if (num == 0)
      return;

#if defined(__CC_ARM) || defined(__MWERKS__)
   asm {ldr  r0, =num}
   asm {ldr  r0, [r0]}
   asm {mov  r1, #(BSP_CPU_MHZ+3)/4}  /* div 4, rounded up since best case (book) is 4 clocks per loop */
   asm {mul  r0, r1, r0}
loop_top:
   asm {subs r0, r0, #1}      /* book says 1 clock */
   asm {bne  loop_top}        /* book says 3 clocks */

#elif defined(__GNUC__)
   asm volatile(              /* volatile is needed when using parameters %0 etc. or function will be empty */
       "ldr  r1, =(" stringify_expanded(BSP_CPU_MHZ) "+3)/4\n\t"  /* div 4, rounded up since best case (book) is 4 clocks per loop */
       "mov  r0, %0\n\t"      /* move num par into r0 */
       "mul  r0, r1, r0\n"
    "loop_top:\n\t"
       "subs r0, r0, #1\n\t"  /* book says 1 clock */
       "bne  loop_top\n\t"    /* book says 3 clocks */
       :"+r"(num)             /* set input parameter num as "%0" */
   );

#elif defined(__ICCARM__)
   /* Implemented in bspiar.s since IAR inline assembler is weak. */
   bsplDelayLoop(num * ((BSP_CPU_MHZ+3)/40));

#else
#error Implement bspDelayUsec() for your compiler.
#endif
}


#if SMX_EVB
/*------ bspEVBGetTimestamp(void)
*
* Documented in XBSP.H.
*
* Differences from Spec: None
*
* Notes:
* 1. Returns the counter of the timer used to generate the tick for smx.
*
----------------------------------------------------------------------*/
dword bspEVBGetTimestamp(void)
{
   /* Return the counter of Timer 0. */
   return T0TC;
}
#endif


#if SMX_EVB
/*------ bspEVBInit(void)
*
* Documented in XBSP.H.
*
* Differences from Spec: None
*
----------------------------------------------------------------------*/
BOOLEAN bspEVBInit(void)
{
   /* Nothing to do since evb_ globals are statically defined above. */
   return TRUE;
}
#endif


/*------ bspExit(retcode)
*
* Documented in XBSP.H.
*
* Differences from Spec: None
*
----------------------------------------------------------------------*/

void bspExit(int retcode)
{
   (void)retcode;
   bspReboot();  /* No environment to exit to. Could also infinite loop. */
}


/*------ bspReboot(void)
*
* Documented in XBSP.H.
*
* Differences from Spec: None
*
----------------------------------------------------------------------*/

void bspReboot(void)
{
   bspMaskIRQs();  /* mask all interrupts */

   //TODO: Implement reboot.
   while (1)
   {
      DEBUGTRAP();
   }
}


/*------ bspRestart(void)
*
* Documented in XBSP.H.
*
* Differences from Spec: None
*
----------------------------------------------------------------------*/

void bspRestart(void)
{
   bspMaskIRQs();  /* mask all interrupts */

   //TODO: Implement. Branch to reset vector?
   while (1)
   {
      DEBUGTRAP();
   }
}


/*------ bspDemoInit(void)
*
* Documented in XBSP.H.
*
* Differences from Spec: none
*
----------------------------------------------------------------------*/

void bspDemoInit(void)
{
#if defined(SMX)
#else
#endif
}


/*------ bspDemoExit(void)
*
* Documented in XBSP.H.
*
* Differences from Spec: none
*
----------------------------------------------------------------------*/

void bspDemoExit(void)
{
#if defined(SMX)
#else
#endif
}


/****************** Platform-Specific BSP functions *******************/
/*
   These functions provide services that are specific to this
   particular target, or they have argument lists that vary for
   each target, so they are not part of the standard API in xbsp.h.
   These are prefixed with "bspp" to distinguish them from the
   standard BSP API functions.

   This is where the user should add any other needed functions.
*/

/*------ bsppWriteIRQTableEntry(irq_num, pri)
*
* Changes an entry dynamically in the irq_table. Generally, the
* irq_table should be initialized statically and left alone, but
* this function is provided in case there is a need to change it
* while running. After calling this, call bspConfigureIRQ() to
* make the change in the hardware.
*
* This function is primarily provided for assembly access, since
* it may not be possible to access a C structure in assembly.
* Even for C, it is good style to call this function rather than
* modifying the structure directly.
*
* USER: Create additional functions if you wish to read/write
* individual fields of an irq_table[] record. We have not created
* additional functions because most users will not need/want them.
* Most will set the hardware once using the values in irq_table,
* above.
*
* Parameters
*    irq_num       indexes into irq_table[]
*    pri           interrupt priority (0)
*
* Returns
*    TRUE          ok
*    FALSE         fail
*
----------------------------------------------------------------------*/

BOOLEAN bsppWriteIRQTableEntry(int irq_num, int pri)
{
   /* bspConfigureIRQ() checks the fields; here we just need to ensure
      irq_num is in range so we don't write outside irq_table[].
   */
   if (irq_num < MIN_IRQ_NUM || irq_num > MAX_IRQ_NUM)
      return(FALSE);

   irq_table[irq_num].pri = pri;

   return(TRUE);
}


/********* Local functions used by the BSP API routines above *********/
/*
   These functions are intended for use by this module, but since some
   may be useful to the application, we don't declare them static.
   These are prefixed with "bspl" to distinguish them from the
   standard BSP API functions. The set of functions defined here will be
   unique to each BSP. In order to gain access, add the prototypes for
   these functions to core.h or directly to the .c module that uses them.
*/

/* none */
