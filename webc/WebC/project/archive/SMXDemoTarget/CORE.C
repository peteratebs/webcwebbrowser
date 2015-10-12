/*
* CORE.C                                                    Version 3.7.1
*
* !!! THIS MODULE IS NOT OPTIONAL !!!
*
* PROTOSYSTEM CORE MODULE. Includes smx configuration table,
* initialization code, idle task, timeout task, and exit task code.
* All routines must be present, but their content can be changed
* as necessary.
*
* "USER" marks places in this module and others that you may wish to
* change for your system.
*
* Copyright (c) 1989-2009 Micro Digital Associates, Inc.
*               All Rights Reserved.
*
* Author:  Ralph Moore
*
***********************************************************************/

#include    "smx.h"
#include    "core.h"
#include    "initmods.h"
#include    <stdio.h>
#ifdef WIN32
#include    "smxsim.h"
#endif

#define  DIVR(x,y) ((2*((x)%(y)) < (y)) ? (x)/(y) : ((x)/(y))+1)
#define  DIV8(x)   (((x)+7)/8)
#define  ALIGN8(x) ((((x)+7)/8)*8)

#if !MIN_RAM
TCB_PTR near exitx;
#endif

#ifdef __cplusplus
 extern "C" {
#endif


/* function prototypes */

#if defined(PEG) && defined(WIN32)
void        WinMesgLoop(void);
#endif

#if PROFILE_FREQ
void              profile_task_main(void);
void              profile_lsr(void);
TCB_PTR           profile_task;
TMCB_PTR          profile_timer; /* used to start the profile task periodically */
dword             percent_idle;  /* % of time in idle task */
dword             percent_ovh;   /* % of time in smx */
dword             percent_work;  /* % of time doing useful work */
volatile uint  near  ictr;       /* idle tick counter */
volatile uint  near  octr;       /* overhead tick counter */
volatile uint  near  wctr;       /* (useful) work tick counter */
#endif // PROFILE_FREQ


/* variables */

struct   DAR   near  adar;                    /* app dar structure */
VOID_PTR adar_ptr = (VOID_PTR)&adar;          /* handle for BUILD_HT */
char display[DISPLAY_ROWS_MAX][DISPLAY_COLUMNS_MAX+1];
SCB_PTR        near  in_clib;                 /* in C library call */

#if defined(DEBUGVER) && !MIN_RAM
/* error buffer, event buffer, lsr queue, and ready queue pointers.
   NOTE: array sizes must be specified for code debuggers.
*/
#if EB_SIZE
struct EREC     (far *e) [EB_SIZE];            /* eb array ptr */
#endif
#if EVB_SIZE
       EVBREC   (far *ev)[EVB_SIZE];           /* evb array ptr */
#endif
struct LQ_CELL (near *l) [LQ_SIZE];            /* lq array ptr */
struct XCB     (near *r) [SMX_MAX_PRIORITY+1]; /* rq array ptr */
#endif /* DEBUGVER && !MIN_RAM */


/* IRQ Handling. These are used by irq.s and irq.inc. */

dword r14_irq_sav;         /* Saves r14_irq across the mode/stack switch; */
                           /* it is the address of the point of interrupt */
dword SPSR_irq_sav;        /* Saves SPSR_irq across the mode/stack switch; */
                           /* it holds the flags (CPSR) at the point of interrupt */


/* Configuration Table */
/*
   Reserve space for the stack pool, heap, and adar using arrays. Then,
   the appropriate fields of the cf table are set using bstack, etc.
   Alternatively, you can set the cf fields below directly to addresses
   to use for these areas.

   Defining the byte arrays as 8 byte unions is a portable way to force
   the compiler to align the arrays on 8 byte boundries. It doesn't work
   for all compilers, so for those, it is necessary to use a section pragma
   and then the linker command file can set the alignment. The padding
   values below can be used to add a little safety padding around the
   stack pool. These can be set to 0 or any desired value.
*/

#if defined(__MWERKS__)  /* CodeWarrior */
#pragma define_section smx_data ".smx_data" ".smx_bss" abs32 RW
#pragma section smx_data begin   /* the following arrays --> .smx_bss section so we can control alignment in linker command file */
#endif

#define PRE_PADDING     0
#define POST_PADDING    0

#if defined(__CC_ARM)
__align(8) byte bstack[STACK_SPACE + PRE_PADDING + POST_PADDING + 8];
__align(8) byte bnheap[HEAP_SPACE + 8];    /* add 8 bytes of extra padding */
__align(8) byte badar [ADAR_SPACE + 8];

#elif defined(__GNUC__)
byte bstack[STACK_SPACE + PRE_PADDING + POST_PADDING + 8] __attribute__ ((aligned(8)));
byte bnheap[HEAP_SPACE + 8] __attribute__ ((aligned(8)));    /* add 8 bytes of extra padding */
byte badar [ADAR_SPACE + 8] __attribute__ ((aligned(8)));

#elif defined(__IAR_SYSTEMS_ICC__)  /* IAR */
#pragma data_alignment=8
__no_init byte bstack[STACK_SPACE + PRE_PADDING + POST_PADDING + 8];
#pragma data_alignment=8
__no_init byte bnheap[HEAP_SPACE + 8];    /* add 8 bytes of extra padding */
#pragma data_alignment=8
__no_init byte badar [ADAR_SPACE + 8];

#else
#if defined(WIN32)
union eightBytes {double b8;};    /* force 8 byte alignment; double = 8 bytes */
#else
union eightBytes {long long b8;}; /* force 8 byte alignment; long long = 8 bytes */
#endif
union eightBytes bstack[DIV8(STACK_SPACE + PRE_PADDING + POST_PADDING + 8)];
union eightBytes bnheap[DIV8(HEAP_SPACE + 8)];    /* add 8 bytes of extra padding */
union eightBytes badar [DIV8(ADAR_SPACE + 8)];
#endif

#if defined(__MWERKS__)  /* CodeWarrior */
#pragma section smx_data end
#endif


struct CONF near cf = {                         /* see conf.h */
         /* max_priority */   SMX_MAX_PRIORITY,
         /* lq_size      */   LQ_SIZE,
         /* spare        */   0,
         /* sec          */   SEC,
         /* timeout_freq */   TIMEOUT_FREQ,
         /* num_blocks   */   NUM_BLOCKS,
         /* num_buckets  */   NUM_BUCKETS,
         /* num_msgs     */   NUM_MSGS,
         /* num_pools    */   NUM_POOLS,
         /* num_qlevels  */   NUM_QLEVELS,
         /* num_pipes    */   NUM_PIPES,
         /* num_tmrs     */   NUM_TIMERS,
         /* num_tasks    */   NUM_TASKS,
         /* stk_sz       */   STACK_SIZE,
         /* num_stacks   */   NUM_STACKS,
         /* eb_size      */   EB_SIZE,
         /* ht_size      */   HT_SIZE,
#if defined(WIN32)
         /* Filled in by smxSimStackInit() */
         /* sp_min       */   0L,
         /* sp_max       */   0L,
         // For Win32, we use compiler heaps, not smx heaps, so these
         // are unneeded for the normal heap.
         /* nheap_min    */   0L,
         /* nheap_max    */   0L,
#else
         /* sp_min       */   (byte*) &bstack + PRE_PADDING,
         /* sp_max       */   (byte*) &bstack + ALIGN8(STACK_SPACE)+PRE_PADDING,
         /* nheap_min    */   (byte*) &bnheap,
         /* nheap_max    */   (byte*) &bnheap + ALIGN8(HEAP_SPACE)-sizeof(struct NHCB),
#endif
         /* ndar_min     */   0,  /* unused; now adar */
         /* ndar_max     */   0,  /* unused; now adar */

         /* fheap_min    */   0L,
         /* fheap_max    */   0L,
         /* nghs_size    */   NGHS_SIZE,
         /* nghs_flag    */   NGHS_FLAG,
         /* fghs_size    */   0L,
         /* fghs_flag    */   0,

#if defined(WIN32)
         /* These mark the start and end of the stack heap.
            They are set by smxSimStackInit().
         */
         /* sheap_min    */   0L,
         /* sheap_max    */   0L,
#endif
         /* stk_pad_sz   */   STACK_PAD_SIZE,
         /* stk_enh_test */   STACK_ENHANCED_TESTING,
         /* stk_clr_val  */   STACK_CLEAR_VALUE,
         /* evb_size     */   EVB_SIZE,
         /* adar_min     */   (byte*) &badar,
         /* adar_max     */   (byte*) &badar + ALIGN8(ADAR_SPACE),
         /* num_mtxs     */   NUM_MTXS
};

#ifdef __cplusplus
}
#endif


/******* MAIN
* Entry point from assembly startup code. For embedded targets, our
* startup code is used; for Win32 targets, the compiler startup code
* is used. When running smx and PEG under Win32, it runs as a GUI app
* rather than a console app, so WinMain() is used rather than main().
* WinMain() is defined in \peg\source\smxpegmt.cpp.  go_smx() performs
* some smx initialization and then enters the tasking environment;
* ainit() runs first. See the comments for ainit().
***********************************************************************/

int _cdecl main(void)
{
   /* Initialize smx. Must be done before any isr's are hooked that
      call INVOKE() since lq, tq, and other structures must be already
      set up before INVOKE() or keep_time() run, or corruption will occur.
   */
   bspMaskIRQs();            /* mask all interrupts (saves current mask for restore at top of ainit()) */
   go_smx();                 /* initialize smx and enter scheduler */

   /* ainit() runs next; go_smx() does not return */

   return(0);  /* should never reach here; keep compiler happy */
}


/******* APPLICATION INITIALIZATION
* This is the starting point for the task environment. go_smx() starts
* the idle task using ainit() as its code and gives it maximum priority
* so it runs first. The code below purposely does not call unlockx()
* so it remains locked. At the end of ainit(), the idle task code is
* changed to idle_main(), and its priority is lowered to MIN.
*
* Do not make smx calls that wait -- ainit() must be allowed to finish
* before other tasks run. Note that some smxNet, smxFile, etc calls test
* semaphores, so these should be avoided during initialization. If you
* experience IDLE TIMEOUT errors early on, this is the likely cause.
***********************************************************************/

void _cdecl ainit(void)
{
  #if defined(ARMM)
   /* Interrupt controller already initialized in startup code, for PendSV. */
  #else
   /* Initialize interrupt controller/dispatcher. If the startup code
      unmasks any interrupts, you should move this call before that.
   */
   bspInitInterruptCtrl();
  #endif

   /* Restore interrupt mask that was in effect where bspMaskIRQs()
      was called in main(). Do here so that any changes to the interrupt
      mask in the code below (e.g. bspInit calls and appl_init()) are kept.
   */
   bspUnmaskIRQs();

   /* exit if any errors occurred during smx init in go_smx() or scheduler */
   if(xerrno) exitx_main(xerrno);

   // smx does not support debugging with a software debug monitor. We tried
   // to get Angel working but there were complexities in the interaction
   // with our interrupt handling code in irq.s, and we gave up. Use a JTAG
   // unit instead.
   // #if !defined(WIN32)
   // #if (DEBUGGER_IRQ_RX >= 0 || DEBUGGER_IRQ_TX >= 0)  /* these are set out of range, if using JTAG */
   //  /* unmask debugger IRQs so stop button works, and may be needed for normal operation */
   //  bspUnmaskIRQ(DEBUGGER_IRQ_RX);
   //  bspUnmaskIRQ(DEBUGGER_IRQ_TX);
   // #endif
   // #endif

   bspInitPeripherals();     /* initialize peripherals */
   bspInitTick();            /* initialize tick used for all smx timing */

  #if (DEVICE_CONSOLE_OUT) && (DEVICE_CONSOLE_OUT_PORT != -1)
   /* "Late" screen init. Done here for targets that use serial port or
      similar device whose driver uses a hardware interrupt and that
      requires peripheral init (e.g. UART). For cases where output is to
      video memory that requires no init or interrupt, it is done in
      go_smx(). Called after bspInitPeripherals() since actual device
      may be partially initialized there.
   */
   bspInitConsoleOut();      /* call after bspInitPeripherals() */
  #endif

  #if defined(WIN32)
   smxSimInit();
  #endif

  #if defined(DEBUGVER) && !MIN_RAM
   /* debug pointers, use: *e  to examine error buffer
                           *ev to examine event buffer
                           *l  to examine lsr queue
                           *r  to examine ready queue */
   #if EB_SIZE
   e  = (struct EREC (far *)[EB_SIZE])ebi;
   #endif
   #if EVB_SIZE
   ev = (       EVBREC (far *)[EVB_SIZE])evbi;
   #endif
   l  = (struct LQ_CELL (near *)[LQ_SIZE])lqi;
   r  = (struct XCB (near *)[SMX_MAX_PRIORITY+1])rq;
  #endif /* DEBUGVER && !MIN_RAM */

   adar.dnext = cf.adar_min;     /* set up adar for create_dpool() calls */
   adar.dmax  = cf.adar_max;

   /* Initialize stimex. Must be done after C/C++ initializers run. */
   if (!bspSetStime())
      WR_STRING(0,1,LIGHTRED,BLACK,!BLINK,"bspSetStime() failed in ainit()");

   ENABLE();                     /* test point; see note 1 at end of file */

  #ifdef SMX_PROBE
   sp_init();                    /* initialize smxProbe */
  #endif

   bspInitConsoleIn();           /* initialize console input device */

   unlockx(self);                /* so can use smxProbe in ainit */
   /* DON'T USE SMXPROBE BEFORE HERE. */

  #if !MIN_RAM
   exitx = create_task((CODE_PTR)exitx_main, MAX, 0);
   BUILD_HT(exitx, "exitx");
  #endif
   in_clib = create_sema(1, NULL);  /* to be used for non-reen. C lib. calls */
   signalx(in_clib);

   /* Add symbols to handle table for smxProbe. (These are nop's if
      smxProbe is not present.)
   */
   BUILD_HT(in_clib, "in_clib");
   BUILD_HT(adar_ptr, "adar");
  #if (DEVICE_CONSOLE_IN) && !(defined(PEG) && defined(WIN32))
   BUILD_HT(op_pipe, "op_pipe");
  #endif

   /* Call initialization routines for all smx add-on products */
   if (!smx_modules_init())
      SMX_ERROR(INIT_MODULES_FAIL, XERRH_SMX)

   /* Call main application initialization routine */
   appl_init();

  #if defined(WIN32)
   smxSimTickEnable();  // This is equivalent to unmasking interrupts.
  #endif

  #if PROFILE_FREQ
   profile_task = create_task((CODE_PTR)profile_task_main, MAX, 0);
  #ifdef __cplusplus
   start_timer(profile_timer, (LSR_PTR)profile_lsr, 0, 0, PROFILE_FREQ);
  #else
   start_timer(&profile_timer, (LSR_PTR)profile_lsr, 0, 0, PROFILE_FREQ);
  #endif
   BUILD_HT(profile_task, "profile_task");
   BUILD_HT(profile_timer, "profile_timer");
  #endif

   start_new(self, idle_main, MIN);  /* become normal idle task */

   /* Execution never reaches here because now the idle task's main
      function is idle_main(). If it does, check that srnest == 0.
   */
   smxDisplayError("Error in ainit(): start_new() returned");
   DEBUGTRAP()
}


/******* IDLE
* The idle task runs when there is no other task to run. It can be used
* to compute processor utilization as well as to gather other informa-
* tion of interest. It can also be used to perform confidence tests
* such as heap checks, stack checks, etc.
***********************************************************************/

void _cdecl idle_main(void)
{
   TCB_PTR t;

   unlockx(self);                   /* permit this task to be preempted */

   while(TRUE)
   {
    #if defined(WIN32)
     #if defined(PEG)
      WinMesgLoop();
     #endif
     #if SIM_USE_HIGH_PERF_WINDOWS_TIMER
      /* Do the tick only every 1ms to avoid the extra null_ssr() events. */
      if (smxSimHiPerfCounter1msElapsed())
     #endif
      {
         CHECKWIN32();              /* Poll Windows keyboard, tick etc. */
      }
    #endif

      /* Scan for task other than self with REL_STK set or !SHWM_VALID. */
      for (t = tcbi; t <= tcbx; t++)
         if (t->cbtype && t != self)
            if ((t->flags & REL_STK) || (cf.stk_enh_test && !(t->flags & SHWM_VALID)))
               break;

      /* If a task was found that needs its stack released or scanned,
         or if there is another task in rq level 0 other than idle and
         smxStackTask, bump idle to the end of level 0 to let it run.
      */
      if (t <= tcbx || self->fl != (CB_PTR)smxStackTask || self->bl->bl != (CB_PTR)smxStackTask)
         bump_task(self, PRI_NOCHG);
   }
}


/****** EXIT SMX
* Call application and smx exit routines.
***********************************************************************/

void _cdecl exitx_main(ERRNO errnum)
{
   appl_exit();         /* call main application exit routine */
   smx_modules_exit();  /* exit smx add-on product modules */

  #if (DEVICE_CONSOLE_IN) && !(defined(PEG) && defined(WIN32))
   kbd_exit();
  #endif

  #ifdef SMX_PROBE
   sp_exit();
  #endif

   bspExit(errnum);
}


/****** TICK ISR
* ARM
*  It does not use the __irq keyword or ENTER_ISR()/EXIT_ISR() because
*  isrs are normal functions that are called either from the the IRQ
*  dispatcher (for ARM's that dispatch interrupts in software) or from
*  the shell isr written in assembly (for ARM's that dispatch interrupts
*  in hardware). In the first case, smx_irq_handler (irq.s) saves the
*  necessary registers and does the equivalent of these macros.
*
* ARMM
*  It does not use the __irq keyword but does use ENTER_ISR()/EXIT_ISR().
*  Much simpler than traditional ARM. No assembly shells are needed.
*
* WIN32 (smxSim)
*  The WIN32 conditionals are only used for smxSim, which allows running
*  this Protosystem in Visual Studio under Windows. (To support Borland,
*  the assembly tick isr is needed from the x86 version.)
***********************************************************************/

#ifdef __cplusplus
extern "C" {
#endif

#if defined(WIN32) && defined(_MSC_VER)
__declspec(naked) void tick(void)
#elif (IRQ_DISPATCH_IN_HDW)
void tickC(void)  /* called by tick assembly shell in irqshels.s */
#else
void tick(void)
#endif
{
  #if defined(ARMM) || defined(WIN32)
   ENTER_ISR();  /* not for traditional ARM since equivalent done in irq.s or irq.inc */
  #endif
   EVB_LOG_ISR(tick_isr_h, 0, 0);
  #if !defined(WIN32)
   bspClearIRQ(TICK_IRQ);
  #endif

   INVOKE(keep_time, TIME_SLICE);

  #if (PROFILE_FREQ)
   /* update profile counters */
   if (app_busy == 0)
      octr++;
   else if (ct == idle || ct == smxStackTask)
      ictr++;
   else
      wctr++;
  #endif

  #if !defined(WIN32)
   bspEndOfIRQ(TICK_IRQ);
  #endif

   EVB_LOG_ISR_RETURN(tick_isr_h, 0, 0);
  #if defined(ARMM) || defined(WIN32)
   EXIT_ISR();  /* not for traditional ARM since equivalent done in irq.s or irq.inc */
  #endif
}

#ifdef __cplusplus
}
#endif

#if PROFILE_FREQ

/* profile_lsr is invoked by a profile_timer, which runs every few seconds. */

void _cdecl profile_lsr(void)
{
   startx(profile_task);
}

/* profile_task calculates and outputs percentages for idle, overhead,
   and work (application). The calculations could be done more easily
   using floating point numbers. However, in many applications, it is
   undesirable to introduce floating point.  */

void _cdecl profile_task_main(void)
{
   dword  i, o, w;
   dword  total;

   /* USER: A smoothing technique is used below so that the counters
      do not change wildly. Whenever doing a major mode shift in your
      program, you may want to reset the counters by setting percent_work
      to 0 at these points in your code. You may want to add a hot key
      to opcon to do this. */

   DISABLE();
   i = ictr;
   o = octr;
   w = wctr;
   ictr = octr = wctr = 0;    /* clear counters */
   ENABLE();

   total = i + o + w;
   if (total == 0) return;    /* avoid divide by zero error if this runs before counters incremented */

   if(percent_work == 0)      /* first time */
   {
      percent_ovh  = DIVR(1000*o,total);
      percent_idle = DIVR(1000*i,total);
      percent_work = DIVR(1000*w,total);
   }
   else                       /* subsequent times -- average */
   {
      #define WT 2  /* USER: larger value ==> slower counter change */
      percent_ovh  = DIVR((DIVR(1000*o,total) + WT*percent_ovh),WT+1);
      percent_idle = DIVR((DIVR(1000*i,total) + WT*percent_idle),WT+1);
      percent_work = DIVR((DIVR(1000*w,total) + WT*percent_work),WT+1);
   }

   /* store counters in display[] buffer */
   _ltoa(percent_idle/10, &display[5][17], 10);
   _ltoa(percent_work/10, &display[6][17], 10);
   _ltoa(percent_ovh/10,  &display[7][17], 10);

 #if defined(PEG) && SMXPEG_PROFILE_WINDOW && PROFILE_FREQ
   // Call SendNewProfileMessage, in initmods.c, to send a new message
   // to the profile window to re-draw the bar graph
   SendNewProfileMessage();
 #endif // PEG
}
#endif // PROFILE_FREQ


/***** SUBROUTINES
**********************************************************************/

#if defined(__CC_ARM)

/* This routine is called by the ARM compiler's startup code. It returns
   the base and limit for the stack and heap. We don't want the compiler
   heap (we have our own) and we want to continue using the stack that
   the low-level startup code set. (It is used only for init; when
   multitasking starts, we switch to tasks' stacks.)
*/

#include <rt_misc.h>

__value_in_regs struct __initial_stackheap __user_initial_stackheap(
        unsigned /*R0*/, unsigned /*SP*/, unsigned /*R2*/, unsigned SL)
{
    struct __initial_stackheap config;

    /* Section 4.9.4 in the ADS Compiler, Linker, Utilies guide says to
       indicate no heap, set the heap base and limit to the stack limit
       passed in. We don't want the compiler heap since we use smx's.
    */
    config.heap_base  = SL;   /* no heap */
    config.heap_limit = SL;   /* no heap */

    /* At the app entry point, AXD initializes sp (r13) to the top of RAM
       the first time the app is loaded, or on an app reload, it remains
       where it was at the end of the previous run. Immediately prior
       to calling this routine, the ARM compiler's startup code sets sp
       to a small area of memory it allocates above .bss data. On return
       it sets sp to whatever this routine indicates in config.stack_base.
       Since AXD doesn't reload sp on each new session, it seems a bit
       dangerous to use whatever sp was from the last session, since it
       may have had a corrupted value and it may point to data or code,
       and that could cause corruption while this stack is temporarily
       in use (in main() and go_smx()). Instead, we set it here to point
       to a known location: the top of the stack pool. This area will
       later be used for a task stack, but this temporary stack is used
       only before multitasking begins, so it doesn't matter.
    */
    config.stack_base  = (unsigned) cf.nheap_max;         /* use top of heap space for temporary initialization stack */
    config.stack_limit = (unsigned) cf.nheap_max - 1000;  /* I don't think this is used, but if it is, it must be < stack_base */

    return config;
}

#endif /* __CC_ARM */


/*
   Notes:

   1. It is not necessary to explicitly call ENABLE() in ainit() since
      interrupts will be enabled as soon as the first smx ssr is called.
      However, we do so a little while after we enable the tick but before
      the first smx call because if the system hangs because interrupts
      are enabled, this will be clear. Otherwise, it might appear that
      the hang is due to the smx call, and you could waste a lot of time
      tracking into it only to discover this. This is especially useful
      when using LED's as markers to bring up the system in ROM/flash.
      If a marker is put before and after this ENABLE(), the second marker
      will not be reached if this is the problem (assuming enough time
      elapsed since the tick was enabled).

*/

