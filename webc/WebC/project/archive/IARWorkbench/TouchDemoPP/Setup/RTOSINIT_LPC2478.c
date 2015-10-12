/*********************************************************************
*  
*   IAR PowerPac
*
*   (c) Copyright IAR Systems 2008.  All rights reserved.
*
**********************************************************************
----------------------------------------------------------------------
File    : RTOSInit_LPC2478.c   (for NXP LPC2478 CPU)
Purpose : Initializes and handles the hardware for the OS as far
          as required by the OS.
          Feel free to modify this file acc. to your
          target system.
Literature:  [1]: NXP ERRATA-SHEET LPC2468 Ver. 1.3
--------  END-OF-HEADER  ---------------------------------------------
*/

#include "RTOS.H"
#if DEBUG
  #include "JLINKDCC.h"
#endif

/*********************************************************************
*
*       Configuration
*
*********************************************************************/

/*********************************************************************
*
*       Clock frequency settings
*/
#ifndef   OS_XTAL_FREQUENCY
  #define OS_XTAL_FREQUENCY     12000000
#endif

#ifndef   OS_USE_INTERNAL_RC
  #define OS_USE_INTERNAL_RC  0
#endif

#ifndef   OS_FSYS                      /* CPU main clock freqeuncy     */
  #define OS_FSYS (57600000uL)         /* may depend on PLL            */
#endif

#ifndef   OS_PCLK_TIMER                /* Peripheral clock for timer   */
  #define OS_PCLK_TIMER (OS_FSYS/4)    /* defaults to CPU clock / 4    */
#endif

#ifndef   OS_PCLK_UART                 /* Peripheral clock for UART    */
  #define OS_PCLK_UART (OS_FSYS/4)     /* defaults to CPU clock / 4    */
#endif

#ifndef   OS_INIT_PLL                  /* PLL may be initialized       */
  #define OS_INIT_PLL         (1)      /* during __low_level_init      */
#endif

#if (OS_USE_INTERNAL_RC)
  #ifndef   OS_PLL_MULTIPLIER_VAL        /* PLL multiplier may be        */
    #define OS_PLL_MULTIPLIER_VAL  (35)  /* defined by project settings  */
  #endif
#else
  #ifndef   OS_PLL_MULTIPLIER_VAL        /* PLL multiplier may be        */
    #define OS_PLL_MULTIPLIER_VAL  (11)  /* defined by project settings  */
  #endif
#endif

#ifndef   OS_INIT_MAM                  /* Memory accelerator           */
  #define OS_INIT_MAM            (1)   /* initialized per default      */
#endif

/****** OS internal timing settings ******************************/

#define _OS_TIMER_INTERVAL (OS_PCLK_TIMER / 1000)

#if OS_INIT_MAM                     /* Define timer value for MAM   */
  #if   (OS_FSYS < 2000000)
    #define _OS_MAMTIM_VALUE    (2)
  #elif (OS_FSYS < 4000000)
    #define _OS_MAMTIM_VALUE    (3)
  #else
    #define _OS_MAMTIM_VALUE    (5)
  #endif
#endif

/*********************************************************************
*
*       UART settings for OSView
*       If you do not want (or can not due to hardware limitations)
*       to dedicate a UART to OSView, please define it to be -1
*       Currently the standard code enables UART 1 per default
*       and supports UART0 to UART1
*/

#ifndef   OS_UART
  #define OS_UART     (0)
#endif
#ifndef   OS_BAUDRATE
  #define OS_BAUDRATE (38400L)
#endif

/****** Define behavior of undefined interrupt handling *************/

#ifndef   OS_IGNORE_UNDEFINED_INTERRUPT
  #define OS_IGNORE_UNDEFINED_INTERRUPT 0
#endif

/****** End of configuration settings *******************************/

/****** Configuration check  *******************************/
#if OS_FSYS  > 60000000
#error "Undefined behavior of the CPU, see errata sheet for NXP LPC2468"
#endif


#define OS_UART_USED ((OS_UART == 0) || (OS_UART == 1))

/*********************************************************************
*
*       Local defines (sfrs used in RTOSInit.c)
*
**********************************************************************
*/

/****** UART sfdr definition ****************************************/
/****** UART0 *******************************************************/

#define _UART0_BASE_ADDR   0xE000C000
#define _UART1_BASE_ADDR   0xE0010000

#define _UART_RBR_OFFS     0x00
#define _UART_THR_OFFS     0x00
#define _UART_IER_OFFS     0x04
#define _UART_IIR_OFFS     0x08
#define _UART_FCR_OFFS     0x08
#define _UART_LCR_OFFS     0x0C
#define _UART_LSR_OFFS     0x14
#define _UART_SCR_OFFS     0x1C
#define _UART_DLL_OFFS     0x00
#define _UART_DLM_OFFS     0x04

/****** Assign UART sfrs used for OSView communication ***********/

#if OS_UART_USED
  #if   (OS_UART == 0)
    #define _OS_UART_BASE_ADDR     (_UART0_BASE_ADDR)
    #define _OS_UART_INT_INDEX     (6)
    #define _UART_PCONP_BIT        (3)
    #define _OS_UART_PINSEL_RX     _PINSEL0
    #define _OS_UART_PINSEL_TX     _PINSEL0
    #define _PINSEL_UART_MODE_RX_POS (6)
    #define _PINSEL_UART_MODE_TX_POS (4)
    #define _PINSEL_UART_MODE_RX_VAL (1)
    #define _PINSEL_UART_MODE_TX_VAL (1)

  #elif (OS_UART == 1)
    #define _OS_UART_BASE_ADDR     (_UART1_BASE_ADDR)
    #define _OS_UART_INT_INDEX     (7)
    #define _UART_PCONP_BIT        (4)
    #define _OS_UART_PINSEL_RX     _PINSEL1
    #define _OS_UART_PINSEL_TX     _PINSEL0
    #define _PINSEL_UART_MODE_RX_POS (0)
    #define _PINSEL_UART_MODE_TX_POS (30)
    #define _PINSEL_UART_MODE_RX_VAL (1)
    #define _PINSEL_UART_MODE_TX_VAL (1)
#endif

  #define _OS_UART_RBR   *(volatile OS_U8*)(_OS_UART_BASE_ADDR + _UART_RBR_OFFS)
  #define _OS_UART_THR   *(volatile OS_U8*)(_OS_UART_BASE_ADDR + _UART_THR_OFFS)
  #define _OS_UART_IER   *(volatile OS_U8*)(_OS_UART_BASE_ADDR + _UART_IER_OFFS)
  #define _OS_UART_IIR   *(volatile OS_U8*)(_OS_UART_BASE_ADDR + _UART_IIR_OFFS)
  #define _OS_UART_FCR   *(volatile OS_U8*)(_OS_UART_BASE_ADDR + _UART_FCR_OFFS)
  #define _OS_UART_LCR   *(volatile OS_U8*)(_OS_UART_BASE_ADDR + _UART_LCR_OFFS)
  #define _OS_UART_LSR   *(volatile OS_U8*)(_OS_UART_BASE_ADDR + _UART_LSR_OFFS)
  #define _OS_UART_SCR   *(volatile OS_U8*)(_OS_UART_BASE_ADDR + _UART_SCR_OFFS)
  #define _OS_UART_DLL   *(volatile OS_U8*)(_OS_UART_BASE_ADDR + _UART_DLL_OFFS)
  #define _OS_UART_DLM   *(volatile OS_U8*)(_OS_UART_BASE_ADDR + _UART_DLM_OFFS)

  #define _RX_FULL_INT_ENABLE_BIT  0
  #define _TX_EMPTY_INT_ENABLE_BIT 1
  #define _RX_ERROR_INT_ENABLE_BIT 2
  #define _INT_PENDING_BIT         0

  #define _UART_INT_MASK         0x0E
  #define _UART_ERROR_INT_STATUS 0x06
  #define _UART_RX_INT_STATUS    0x04
  #define _UART_TX_INT_STATUS    0x02
#endif    /* OS_UART_USED  */

/****** Port, port mode settings ************************************/

#define _PINSEL0      *(volatile OS_U32*)0xE002C000
#define _PINSEL1      *(volatile OS_U32*)0xE002C004

#define _SCS      *(volatile OS_U32*)0xE01FC1A0

/****** Power, reset clock control unit register ********************/

#define _PLLCON       *(volatile OS_U32*)0xE01FC080
#define _PLLCFG       *(volatile OS_U32*)0xE01FC084
#define _PLLSTAT      *(volatile OS_U32*)0xE01FC088
#define _PLLFEED      *(volatile OS_U32*)0xE01FC08C
#define _PCON         *(volatile OS_U32*)0xE01FC0C0
#define _PCONP        *(volatile OS_U32*)0xE01FC0C4
#define _CPUCLKCFG    *(volatile OS_U32*)0xE01FC104
#define _USBCLKCFG    *(volatile OS_U32*)0xE01FC108
#define _CLKSRCSEL    *(volatile OS_U32*)0xE01FC10C
#define _PCLKSEL0     *(volatile OS_U32*)0xE01FC1A8
#define _PCLKSEL1     *(volatile OS_U32*)0xE01FC1AC

/****** PLL and memeory timing **************************************/

#define _MAMCR       *(volatile OS_U32*)(0xE01FC000)
#define _MAMTIM      *(volatile OS_U32*)(0xE01FC004)

/****** Timer sfr definition ****************************************/

#define _TIM0_BASE_ADDR   0xE0004000
#define _TIM1_BASE_ADDR   0xE0008000

#define _TIM_IR_OFFS      0x00
#define _TIM_TCR_OFFS     0x04
#define _TIM_TC_OFFS      0x08
#define _TIM_PR_OFFS      0x0C
#define _TIM_MCR_OFFS     0x14
#define _TIM_MR0_OFFS     0x18

/****** Assign timer sfrs used for OS timer **********************/
/****** initially, we use timer 0        ****************************/

#define _OS_TIM_BASE_ADDR       _TIM0_BASE_ADDR
#define _OS_TIM_IR_BIT          0  // Used to reset timer interrupt (Timer IR register), Match channel 0
#define _TIMER_PCONP_BIT        1

#define _OS_TIMER_INT_INDEX     4 // Timer 0 interrupt

#define _OS_TIM_IR       *(volatile OS_U32*)(_OS_TIM_BASE_ADDR + _TIM_IR_OFFS)
#define _OS_TIM_TCR      *(volatile OS_U32*)(_OS_TIM_BASE_ADDR + _TIM_TCR_OFFS)
#define _OS_TIM_TC       *(volatile OS_U32*)(_OS_TIM_BASE_ADDR + _TIM_TC_OFFS)
#define _OS_TIM_PR       *(volatile OS_U32*)(_OS_TIM_BASE_ADDR + _TIM_PR_OFFS)
#define _OS_TIM_MCR      *(volatile OS_U32*)(_OS_TIM_BASE_ADDR + _TIM_MCR_OFFS)
#define _OS_TIM_MR0      *(volatile OS_U32*)(_OS_TIM_BASE_ADDR + _TIM_MR0_OFFS)

/****** Vectored interrupt controller (VIC) *************************/

#define _VIC_BASE_ADDR  0xFFFFF000

#define _VIC_IRQSTATUS       *(volatile OS_U32*)(_VIC_BASE_ADDR + 0x000)
#define _VIC_FIQSTATUS       *(volatile OS_U32*)(_VIC_BASE_ADDR + 0x004)
#define _VIC_RAWINTERRUPT    *(volatile OS_U32*)(_VIC_BASE_ADDR + 0x008)
#define _VIC_INTSELECT       *(volatile OS_U32*)(_VIC_BASE_ADDR + 0x00C)
#define _VIC_INTENABLE       *(volatile OS_U32*)(_VIC_BASE_ADDR + 0x010)
#define _VIC_INTENABLECLEAR  *(volatile OS_U32*)(_VIC_BASE_ADDR + 0x014)
#define _VIC_SOFTINT         *(volatile OS_U32*)(_VIC_BASE_ADDR + 0x018)
#define _VIC_SOFTINTCLEAR    *(volatile OS_U32*)(_VIC_BASE_ADDR + 0x01C)

#define _VIC_PRIO_0          *(volatile OS_U32*)(_VIC_BASE_ADDR + 0x200)
#define _VIC_VECTORADDR      *(volatile OS_U32*)(_VIC_BASE_ADDR + 0xF00)

#define _VIC_VECT_0          *(volatile OS_U32*)(_VIC_BASE_ADDR + 0x100)
#define _VIC_VECT_BASE_ADDR  (_VIC_BASE_ADDR + 0x100)
#define _VIC_PRIO_BASE_ADDR  (_VIC_BASE_ADDR + 0x200)

#ifndef   _NUM_INT_VECTORS
  #define _NUM_INT_VECTORS     32
#endif

#define _NUM_INT_PRIORITIES   (16)
#define _INT_PRIORITY_MASK    (_NUM_INT_PRIORITIES - 1)

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

/*********************************************************************
*
*       Local functions
*
**********************************************************************
*/

/*********************************************************************
*
*       _InitPLL
*
* Function description
*   Initialize PLL
*   WARNING: Improper settings may lock CPU
*   Please examine whether init sequence fits your harware configuration
*   We assume a CPU running with internal oscillator.
*/
static void _InitPLL(void) {
  if (_PLLSTAT & (1 << 25)) {
    _PLLCON  |= (1 << 1);
    _PLLFEED  = 0xAA;
    _PLLFEED  = 0x55;
  }
  _PLLCON     = 0;                         // Set PLL disabled and disconnected
  _PLLFEED    = 0xAA;
  _PLLFEED    = 0x55;
#if (OS_USE_INTERNAL_RC)
  _CLKSRCSEL  = 0;                         // Select internal RC oscillator 4 MHz as clock source
#else
  _SCS        |= (1 << 4)                  // The frequency range of the main oscillator is 1 MHz to 20 MHz.
              |  (1 << 5)                  // Main oscillator enable.
                ;
  _CLKSRCSEL  = 1;                         // Select internal RC oscillator 4 MHz as clock source
#endif

  _PLLCFG     = OS_PLL_MULTIPLIER_VAL;     // MSEL, default = 35 with 4MHz clock input
  _PLLFEED    = 0xAA;
  _PLLFEED    = 0x55;
  _PLLCON     = 1;                         // Enable PLL, Disconnect
  _PLLFEED    = 0xAA;
  _PLLFEED    = 0x55;
  do {
  } while ((_PLLSTAT & (1 << 26)) == 0);   // Check PLOCK bit	
  _CPUCLKCFG  = 5;                         // Set CPU clock divider
  _USBCLKCFG  = 5;                         // Set USB clock divider
  _PCLKSEL0   = 0;
  _PCLKSEL1   = 0;
  _PLLCON     = (1 << 0) | (1 << 1);       // Enable PLL and select as clock source
  _PLLFEED    =  0xAA;
  _PLLFEED    =  0x55;
}

/*********************************************************************
*
*       _Init_VIC()
*
* Function description
*   Clears and disables all interrut vectors in VIC.
*/
static void _Init_VIC(void) {
  int Index;

  _VIC_INTENABLECLEAR = 0xFFFFFFFF;            // Disable all source interrupts
  _VIC_VECTORADDR     = 0;                     // Write to vector, reset VIC
  for (Index = 0; Index < _NUM_INT_VECTORS; Index++) {
    OS_ARM_InstallISRHandler(Index, NULL);      // Set vector to 0;
  }
  _VIC_SOFTINTCLEAR   = 0xFFFFFFFF;            // Disable all soft interrupts
  _VIC_INTSELECT      = 0;                     // Set all channels to generate IRQ
}

/*********************************************************************
*
*       OS_ISR_Tick() interrupt Handler
*/
static void _OS_ISR_Tick(void) {
  _OS_TIM_IR = (1 << _OS_TIM_IR_BIT);  // Clear OS timer interrupt flag
  OS_HandleTick();                     // Call OS tick handler
#if DEBUG
  DCC_Process();
#endif
}

/*********************************************************************
*
*       Global functions
*
**********************************************************************
*/

/*********************************************************************
*
*       __low_level_init()
*
* Function description
*   Called from startup code.
*   Used to initialize PLL and memory accelerator as early as possible
*/
OS_INTERWORK int __low_level_init(void);       /* Avoid "No prototype" warning */
OS_INTERWORK int __low_level_init(void) {
  #if OS_INIT_PLL
    _InitPLL();
  #endif
  #if OS_INIT_MAM                // Speedup CPU
    _MAMCR  = 0x00;              // Disable Memory accelerator module
    _MAMTIM = _OS_MAMTIM_VALUE;  // Depends on OS_FSYS
    _MAMCR  = 0x01;              // Enable Memory accelerator module partially , refer to [1], MAM.1 operation
  #endif
  return 1;                      // Always initialize segments !
};

/*********************************************************************
*
*       OS_InitHW()
*
*       Initialize the hardware (timer) required for the OS to run.
*       May be modified, if an other timer should be used
*       Also enables memory accelerator module
*/
void OS_InitHW(void) {
  OS_DI();
  _Init_VIC();                   // Initialize VIC, clear and disable all interrupts
  // Initialize timer for OS
  _PCONP      |=  (1 << _TIMER_PCONP_BIT);   // Enable Clock for OS timer
  _OS_TIM_TCR &= ~(1 << 0);                  // Disable timer
  _OS_TIM_IR   =  (1 << _OS_TIM_IR_BIT);     // Clear OS timer interrupt flag
  _OS_TIM_PR   = 0;                          // Count on every pclk
  _OS_TIM_MR0  = _OS_TIMER_INTERVAL - 1;     // Initialize match register 1
  _OS_TIM_MCR  =  (1 << 0)                   // Interrupt on match channel 0
                | (1 << 1);                  // Counter reset on match channel 0
  _OS_TIM_TC   =  0;                         // Clear counter
  _OS_TIM_TCR |=  (1 << 0);                  // Enable timer
  // Setup interrupt controller
  OS_ARM_InstallISRHandler(_OS_TIMER_INT_INDEX, &_OS_ISR_Tick); // Timer/counter interrupt vector.
  OS_ARM_EnableISR(_OS_TIMER_INT_INDEX);                        // Enable timer/counter 0 interrupt.
  OS_ARM_ISRSetPrio(_OS_TIMER_INT_INDEX, 15);                   // lowest priority
  // optionally initialize UART for OSView
  OS_COM_Init();
  OS_RestoreI();
}

/*********************************************************************
*
*       Idle loop  (OS_Idle)
*
*       Please note:
*       This is basically the "core" of the idle loop.
*       This core loop can be changed, but:
*       The idle loop does not have a stack of its own, therefore no
*       functionality should be implemented that relies on the stack
*       to be preserved. However, a simple program loop can be programmed
*       (like toggeling an output or incrementing a counter)
*/
void OS_Idle(void) {     // Idle loop: No task is ready to exec
  while (1) {
  }
}

/*********************************************************************
*
*       Get time [cycles]
*
*       This routine is required for task-info via OSView or high
*       resolution time maesurement functions.
*       It returns the system time in timer clock cycles.
*/
OS_U32 OS_GetTime_Cycles(void) {
  OS_U32 time;
  OS_U32 count;

  count = _OS_TIM_TC;                               // Read current timer value
  time  = OS_GetTime32();                           // Read current OS time
  if ((_OS_TIM_IR & (1 << _OS_TIM_IR_BIT)) != 0) {  // Timer Interrupt pending ?
    time++;                                         // Adjust result, read timer again
    count = _OS_TIM_TC;                             // Read again
  }
  return (_OS_TIMER_INTERVAL) * time + count;
}

/*********************************************************************
*
*       OS_ConvertCycles2us
*
*       Convert Cycles into micro seconds.
*
*       If your clock frequency is not a multiple of 1 MHz,
*       you may have to modify this routine in order to get proper
*       diagonstics.
*
*       This routine is required for profiling or high resolution time
*       measurement only. It does not affect operation of the OS.
*/
OS_U32 OS_ConvertCycles2us(OS_U32 Cycles) {
  return Cycles/(OS_PCLK_TIMER/1000000);
}

/*********************************************************************
*
*       Communication for OSView via UART (optional)
*
**********************************************************************
*/

#if OS_UART_USED

/*********************************************************************
*
*       OS_COM_ISR_Usart() embOS USART interrupt handler
*       handles both, Rx and Tx interrupt
*/
static void _OS_COM_ISR(void) {
  int UartStatus;
  volatile OS_U32 Dummy;

  UartStatus = _OS_UART_IIR;                                          // Examine interrupt identification register
  if ((UartStatus & _UART_INT_MASK) == _UART_ERROR_INT_STATUS) {      // Error pending ?
    Dummy = _OS_UART_RBR;                                             // Discard data
    Dummy = _OS_UART_LSR;                                             // reset error
  } else if ((UartStatus & _UART_INT_MASK) == _UART_RX_INT_STATUS) {  // Data received?
    OS_OnRx(_OS_UART_RBR);                                            // Process actual byte
  } else if ((UartStatus & _UART_INT_MASK) == _UART_TX_INT_STATUS) {  // Tx interrupt pending?
    if (OS_OnTx() != 0) {                                             // No more character sent?
      _OS_UART_IER &= ~(1 << _TX_EMPTY_INT_ENABLE_BIT);               // Disable Tx interrupts
    }
  }
}

/*********************************************************************
*
*       OS_COM_Send1()
*       Never call this function directly from your application
*/
void OS_COM_Send1(OS_U8 c) {
  _OS_UART_THR  = c;                                 // Send character
  _OS_UART_IER |= (1 << _TX_EMPTY_INT_ENABLE_BIT);   // Enable Tx interrupts
}

/*********************************************************************
*
*       OS_COM_Init()
*       Initialize UART for OSView
*/
#define _BAUDDIVIDE ((OS_PCLK_UART+OS_BAUDRATE*8L)/(OS_BAUDRATE*16L))

void OS_COM_Init(void) {
  OS_DI();
  // Setup Port-Mode to alternate function
  _OS_UART_PINSEL_RX &= ~(0x03 << _PINSEL_UART_MODE_RX_POS);
  _OS_UART_PINSEL_TX &= ~(0x03 << _PINSEL_UART_MODE_TX_POS);
  _OS_UART_PINSEL_RX |=  (_PINSEL_UART_MODE_RX_VAL << _PINSEL_UART_MODE_RX_POS);
  _OS_UART_PINSEL_TX |=  (_PINSEL_UART_MODE_TX_VAL << _PINSEL_UART_MODE_TX_POS);

  _PCONP       |= (1 << _UART_PCONP_BIT);  // Enable UART unit
  _OS_UART_IER  = 0x00;                    // Initially disable all interrupts
  _OS_UART_LCR  = 0x80;                    // Set DLAB to initialize Baudrate generator
  _OS_UART_DLL  = (_BAUDDIVIDE & 0xFF);
  _OS_UART_DLM  = ((_BAUDDIVIDE >> 8) & 0xFF);
  _OS_UART_LCR &= ~0x80;                   // reset DLAB to lock baudrate generator access
  _OS_UART_LCR  = 0x03                     // 8 data bits
                 |(0 << 2)                 // 1 stop bit
                 |(0 << 3)                 // NO parity
                 |(0 << 4)                 // Parity setting (bit 5:4) does not care
                 |(0 << 6)                 // Disable Break transmission
                 |(0 << 7);                // Clear DLAB
  _OS_UART_FCR  = (1 << 0)                 // Enable FIFO (according to documentation)
                | (1 << 1)                 // Reset RxFIFO
                | (1 << 2)                 // Reset TxFIFO
                | (0 << 6)                 // Set trigger-level for RxFIFO
                ;
  /*  Install OS UART interrupt handler */
  OS_ARM_InstallISRHandler(_OS_UART_INT_INDEX, &_OS_COM_ISR);  // OS UART interrupt vector.
  OS_ARM_EnableISR(_OS_UART_INT_INDEX);                        // Enable UART interrupt vector.
  OS_ARM_ISRSetPrio(_OS_UART_INT_INDEX, 15);                   // lowest priority
  _OS_UART_IER |= (1 << _RX_FULL_INT_ENABLE_BIT);              // Enable Rx interrupts
  OS_RestoreI();
}

#else  /* selected UART not supported, using dummies */

void OS_COM_Init(void) {}
void OS_COM_Send1(OS_U8 c) {
  OS_USEPARA(c);           // avoid compiler warning
  OS_COM_ClearTxActive();  // let the OS know that Tx is not busy
}

#endif /*  OS_UART_USED  */

/****** Final check of configuration ********************************/
#ifndef OS_UART_USED
  #error "OS_UART_USED has to be defined"
#endif

/*********************************************************************
*
*       OS interrupt handler and ISR specific functions
*
**********************************************************************
*/

/*********************************************************************
*
*       _OS_ISR_Undefined
*
*       Is called when an uninstalled interrupt was detected
*       As interrupt pending condition of peripherals has to be reset,
*       program will not continue when interrupt is ignored.
*/
static void _OS_ISR_Undefined(void) {
  #if (OS_IGNORE_UNDEFINED_INTERRUPT == 0)
    volatile OS_U32 Dummy;

    Dummy = 1;
    /* You may set a breakpoint here to detect undefined interrupts */
    while (Dummy > 0) {
    }
  #endif
}

/*********************************************************************
*
*       OS_irq_handler
*
*       Detect reason for IRQ and call correspondig service routine.
*       OS_irq_handler is called from OS_IRQ_SERVICE function
*       found in RTOSVect.asm
*/
OS_INTERWORK void OS_irq_handler(void) {
  OS_ISR_HANDLER* pISR;
  pISR = (OS_ISR_HANDLER*) _VIC_VECTORADDR;   // Get current interrupt handler
  OS_EnterNestableInterrupt();                // Now interrupts may be reenabled. If nesting should be allowed
  if (pISR != NULL) {
    pISR();                                   // Call installed interrupt service routine
  } else {
    _OS_ISR_Undefined();
  }
  OS_DI();                                    // Disable interrupts and unlock
  _VIC_VECTORADDR = 0;                        // Clear current interrupt pending condition, reset VIC
  OS_LeaveNestableInterrupt();                // Replace by OS_LeaveInterrupt(), when nesting was disabled
}

/*********************************************************************
*
*       OS_ARM_InstallISRHandler
*/
OS_ISR_HANDLER* OS_ARM_InstallISRHandler (int ISRIndex, OS_ISR_HANDLER* pISRHandler) {
  OS_ISR_HANDLER*  pOldHandler;
  OS_ISR_HANDLER** papISR;

#if DEBUG
  if ((unsigned)ISRIndex >= _NUM_INT_VECTORS) {
    OS_Error(OS_ERR_ISR_INDEX);
    return NULL;
  }
#endif
  OS_DI();
  papISR = (OS_ISR_HANDLER**) &_VIC_VECT_0;
  pOldHandler          = *(papISR + ISRIndex);
  *(papISR + ISRIndex) = pISRHandler;
  OS_RestoreI();
  return pOldHandler;
}

/*********************************************************************
*
*       OS_ARM_EnableISR
*/
void OS_ARM_EnableISR(int ISRIndex) {
#if DEBUG
  if ((unsigned)ISRIndex >= _NUM_INT_VECTORS) {
    OS_Error(OS_ERR_ISR_INDEX);
    return;
  }
#endif
  OS_DI();
  _VIC_INTENABLE = (1UL << ISRIndex);
  OS_RestoreI();
}

/*********************************************************************
*
*       OS_ARM_DisableISR
*/
void OS_ARM_DisableISR(int ISRIndex) {
#if DEBUG
  if ((unsigned)ISRIndex >= _NUM_INT_VECTORS) {
    OS_Error(OS_ERR_ISR_INDEX);
    return;
  }
#endif
  OS_DI();
  _VIC_INTENABLECLEAR = (1UL << ISRIndex);
  OS_RestoreI();
}

/*********************************************************************
*
*       OS_ARM_ISRSetPrio
*/
int OS_ARM_ISRSetPrio(int ISRIndex, int Prio) {
  OS_U32* pPrio;
  int     OldPrio;
#if DEBUG
  if ((unsigned)ISRIndex >= _NUM_INT_VECTORS) {
    OS_Error(OS_ERR_ISR_INDEX);
    return 0;
  }
#endif
  OS_DI();
  pPrio = (OS_U32*) _VIC_PRIO_BASE_ADDR;
  OldPrio = pPrio[ISRIndex];
  pPrio[ISRIndex] = (OldPrio & ~_INT_PRIORITY_MASK) | (Prio & _INT_PRIORITY_MASK);
  OS_RestoreI();
  return OldPrio & _INT_PRIORITY_MASK;
}

/*****  EOF  ********************************************************/

