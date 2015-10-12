/*
* CONF (ARM)                                                Version 3.7.1
*
* CONFIGURATION PARAMETERS. Most of these settings control memory usage
* and are used to initialize the smx configuration table (cf) in core.c.
* Fields in the cf table are referenced from the smx library code.
*
* The constants below are set to accommodate the demos that can be added
* to the Protosystem, and should serve as a good start for your initial
* application development. See the Quick Start manual for more information
* about tuning smx for your system.
*
* Copyright (c) 1989-2009 Micro Digital Associates, Inc.
*               All Rights Reserved.
*
* Author: Ralph Moore
*
***************************************************************************/

#ifndef SMX_CONF_H
#define SMX_CONF_H 1

#ifndef MIN_RAM  /* This check ensures MIN_RAM has already been defined. */
#ifndef WIN32
#error Include bsp.h in all files that include conf.h (ahead of it).
#endif
#endif

/*------------------------------------------------------------------------*/

#define  SMX_MAX_PRIORITY  5  /* < 128. Also increase NUM_QLEVELS */
/*
   Highest priority level. This value + 1 (for the 0 level) determines
   the number of priority levels in the system and in the ready queue (rq).
   It is best to use as few levels as are actually necessary. NOTE: The
   highest priority level is normally reserved for smxProbe. If this setting
   is increased, also increase NUM_QLEVELS by the same amount.
*/
/*------------------------------------------------------------------------*/

#define  TIME_SLICE      0
/*
   If non-zero, enables timeslicing of priority level 0 tasks. Set to 0
   to disable. The value it is set to specifies the number of ticks per
   timeslice. Note that this does not guarantee a task will be given
   this many ticks every time; in the case where level 0 task suspends
   itself, the next task gets only the remainder of the first task's
   timeslice, since the keep_time lsr is not aware of this and does not
   clear the counter. Use an smx timer to achieve periodic scheduling.
*/
/*------------------------------------------------------------------------*/

#if MIN_RAM
#define  LQ_SIZE        50    /* <= 255 */
#else
#define  LQ_SIZE       255    /* <= 255 */
#endif
/*
   Maximum number of lsr's which can be enqueued at once in the lsr
   queue (lq). Increase if LQ_OVERFLOW error occurs. Entries in lq are 8
   bytes -- see definition of LQ_CELL in xtypes32.h.
*/
/*------------------------------------------------------------------------*/

#define  SEC (BSP_TICKS_PER_SEC)
/*
   Ticks per second. Be sure to increase this number if the tick
   interval is reduced. Determines how often system time (stimex) is
   incremented and how often the timeout task checks for task timeouts.
   bspInitTick() programs the timer to this rate.
*/
/*------------------------------------------------------------------------*/

#define  TIMEOUT_FREQ (SEC/10)  /* ticks */
/*
   Specifies how often the timeout task runs, in ticks. Set as desired
   to a fraction of a second or to any integer value. This controls the
   resolution of the timeout mechanism (i.e. the timeout parameter of
   smx calls). Increasing the frequency gives more accurate timeouts,
   but at the expense of additional overhead (due to resuming and running
   the timeout task so frequently).
*/
/*------------------------------------------------------------------------*/

#if SMX_PROFILING
#if MIN_RAM
#define  PROFILE_FREQ 0        /* 0 eliminates profile_task */
#else
#define  PROFILE_FREQ (3*SEC)  /* ticks. Set to 0 for release. */
#endif
#else
#define  PROFILE_FREQ 0        /* don't change */
#endif
/*
   Specifies how often the profile task runs, in ticks. Set as desired
   to a fraction of a second or to any integer value. This is the amount
   of time between runs of the task. Set to a higher number to run less
   and decrease overhead. Set to 0 to disable profiling.
*/
/*------------------------------------------------------------------------*/


/*************** NUMBER OF KERNEL OBJECTS / CONTROL BLOCKS ****************/

#define  NUM_BLOCKS     30
/*
   Number of blocks of all sizes, excluding stacks and messages.
   Determines number of block control blocks (bcb's) allocated.
   Increase if OUT_OF_BCBS error occurs.
*/
/*------------------------------------------------------------------------*/

#if MIN_RAM
#define  NUM_BUCKETS     0
#else
#define  NUM_BUCKETS     2
#endif
/*
   Number of buckets. Determines number of bucket control blocks (bxcb's)
   allocated. Increase if OUT_OF_CXCBS error occurs (cxcb's include buckets
   and pipes).
*/
/*------------------------------------------------------------------------*/

#ifdef PEG
#define  NUM_PEG_MSGS   32
#else
#define  NUM_PEG_MSGS   0
#endif

#if MIN_RAM
#define  NUM_MSGS       10
#else
#define  NUM_MSGS       (25 + NUM_PEG_MSGS)
#endif
/*
   Number of messages of all sizes, whether in pools or heaps.
   Determines number of message control blocks (mcb's) allocated.
   Increase if OUT_OF_MCBS error occurs.
*/
/*------------------------------------------------------------------------*/

#if MIN_RAM
#define  NUM_MTXS       0
#else
#define  NUM_MTXS       4
#endif
/*
   Number of mutexes. Increase if OUT_OF_MTXS error occurs.
*/
/*------------------------------------------------------------------------*/

#ifdef SMXFILE
#define  NUM_SMXF_QLEVELS  20
// The formula is: NDRIVES*2 + N_FLOPPY_UNITS + N_ATA_UNITS + 1
// but easier to just use a constant -- don't need to be precise,
// QCB's are small. Don't use formula, since there is no .inc file
// where NDRIVES, etc. are defined.
#else
#define  NUM_SMXF_QLEVELS  0
#endif

#ifdef SMXNETX
#define  NUM_SMXNET_QLEVELS  180
#else
#define  NUM_SMXNET_QLEVELS  0
#endif

#ifdef SMXNS
#define  NUM_SMXNS_QLEVELS   64
#else
#define  NUM_SMXNS_QLEVELS   0
#endif

#ifdef SMXUSB
#define  NUM_SMXUSB_QLEVELS  20
#else
#define  NUM_SMXUSB_QLEVELS  0
#endif

#ifdef SMXUSBD
#define  NUM_SMXUSBD_QLEVELS  5
#else
#define  NUM_SMXUSBD_QLEVELS  0
#endif

#ifdef SMXUSBO
#define  NUM_SMXUSBO_QLEVELS  2
#else
#define  NUM_SMXUSBO_QLEVELS  0
#endif

#if MIN_RAM
#define  NUM_QLEVELS    (15 + NUM_SMXNS_QLEVELS + NUM_SMXUSB_QLEVELS \
                            + NUM_SMXUSBD_QLEVELS + NUM_SMXUSBO_QLEVELS)
#else
#define  NUM_QLEVELS    (30 + NUM_SMXF_QLEVELS + NUM_SMXNET_QLEVELS \
                            + NUM_SMXNS_QLEVELS + NUM_SMXUSB_QLEVELS \
                            + NUM_SMXUSBD_QLEVELS + NUM_SMXUSBO_QLEVELS)
#endif
/*
   Number of queue levels. This includes all levels of rq (SMX_MAX_PRIORITY
   + 1), plus all levels of all multi-level queues, plus the number of
   single-level queues. The term "queues" includes exchanges, semaphores,
   event queues, the timer queue and the ready queue. A queue will be a single
   level unless more than one level is specified (i.e. not just "0") when it is
   created. Determines the number of queue control blocks (qcb's) allocated.
   Increase if OUT_OF_QCBS occurs.
*/
/*------------------------------------------------------------------------*/

#if MIN_RAM
#define  NUM_PIPES       2
#else
#define  NUM_PIPES       8
#endif
/*
   Number of pipes. Determines number of pipe control blocks (pxcb's)
   allocated. Increase if OUT_OF_CXCBS error occurs (cxcb's include buckets
   and pipes).
*/
/*------------------------------------------------------------------------*/

#if MIN_RAM
#define  NUM_POOLS       1
#else
#define  NUM_POOLS       6
#endif
/*
   Number of block and message pools. Determines the number of pool
   control blocks (pcb's) allocated. Increase if OUT_OF_PCBS occurs.
*/
/*------------------------------------------------------------------------*/

#if MIN_RAM
#define  NUM_STACKS      2
#else
#define  NUM_STACKS     30
#endif
/*
   Number of stacks in stack pool. It is not necessary to allocate a stack
   per task. Instead, it is necessary only to allocate enough stacks for the
   maximum number of unbound tasks at any time. Increase if OUT_OF_STACKS
   occurs.
*/

#ifdef ARMM
#define  STACK_SIZE     400
/*
   ARM-M case is simple. Don't need to add space for isrs and lsrs of
   other modules since they run on main stack not task stack.
*/

#else
#ifdef SMXNETX
#define  SS_SN    200          /* (bytes) MULTIPLE OF 4 */
#else
#define  SS_SN      0
#endif
#ifdef SMXNS
#define  SS_SNS   400          /* (bytes) MULTIPLE OF 4 */
#else
#define  SS_SNS     0
#endif
#ifdef SMXSSLS
#define  SS_SSLS 1024          /* (bytes) MULTIPLE OF 4 */
#else
#define  SS_SSLS    0
#endif
#ifdef SMXUSB
#define  SS_SU    300          /* (bytes) MULTIPLE OF 4 */
#else
#define  SS_SU      0
#endif
#ifdef SMXUSBD
#define  SS_SUD   100          /* (bytes) MULTIPLE OF 4 */
#else
#define  SS_SUD     0
#endif

#ifdef WIN32
#define  STACK_SIZE     8000   /* (bytes) MULTIPLE OF 4 */
#else
#if MIN_RAM
#define  STACK_SIZE     (500 + SS_SNS + SS_SU + SS_SUD + SS_SSLS)          /* (bytes) MULTIPLE OF 4 */
#else
#define  STACK_SIZE     (800 + SS_SN + SS_SNS + SS_SU + SS_SUD + SS_SSLS)  /* (bytes) MULTIPLE OF 4 */
#endif
#endif
#endif /* ARMM */
/*
   Size of each stack in the stack pool. Each stack is the same size.
   The space allocated must be large enough to handle: (1) the maximum
   nesting of functions for any task -- including local storage for each
   function (which can be very large for recursive C library functions);
   (2) the maximum nesting of interrupt service routines (isr's); (3) the
   greatest link service routine (lsr) requirement; (4) the debugger
   requirement. Increase if STACK_OVERFLOW occurs. CAUTION: ASSIGN
   MULTIPLE OF 4 SO ALL STACKS WILL START ON DWORD BOUNDARIES.
*/

#ifdef WIN32
#define  STACK_PAD_SIZE  2000  /* (bytes) MULTIPLE OF 4 */
#else
#if MIN_RAM
#define  STACK_PAD_SIZE   100  /* (bytes) MULTIPLE OF 4 */
#else
#define  STACK_PAD_SIZE   500  /* (bytes) MULTIPLE OF 4 */
#endif
#endif
/*
   Size of pad added before every stack (pool and heap stacks). Use during
   development and set to 0 for release. Stacks grow toward pad, so system
   will continue to run, unless stack overflows past the whole pad.
*/

#define  STACK_ENHANCED_TESTING  1  /* set to 0 for release */
/*
   This option enables/disables these features:

      1. stack clearing
      2. stack scanning to update the high-water mark (tcb.shwm)

   If 1, all stacks (and pads) are cleared to STACK_CLEAR_VALUE before use.
   Bound stacks are cleared when allocated. Stacks from the stack pool
   are initially cleared and cleared when returned to the pool. This
   option gives a much more accurate determination of stack usage,
   since it scans the stack (and pad) to find the first changed value
   and sets the task's high-water mark there. With this feature disabled,
   the high-water mark is only updated based on the value of the stack
   pointer during task switches. This is likely to miss a time when the
   stack pointer has been moved to an extreme. Scanning the stacks will
   catch this since the changed value remains in the stack. Clearing and
   scanning adds overhead so this setting should be disabled for release.
*/

#define  STACK_CLEAR_VALUE  0x11111111  /* (32-bit value) */
/*
   Value used to initialize every dword of the stack (except guard bands),
   if STACK_ENHANCED_TESTING is enabled. This should be a value that
   rarely occurs in the application. 0 is a poor choice because it is
   extremely common.
*/
/*------------------------------------------------------------------------*/

#if MIN_RAM
#define  NUM_TASKS      12  /* allows for additional modules (smxFS, etc) */
#else
#define  NUM_TASKS      50
#endif
/*
   Number of tasks. Determines number of task control blocks (tcb's)
   allocated. Increase if OUT_OF_TCBS error occurs. NOTE: It is not
   desirable to allocate many more tasks than necessary since the timeout
   task checks all timers that exist.
*/
/*------------------------------------------------------------------------*/

#if MIN_RAM
#define  NUM_TIMERS      3
#else
#define  NUM_TIMERS      5
#endif
/*
   Number of timers. Determines number of timer control blocks (tmcb's)
   allocated. Increase if OUT_OF_TMCBS error occurs. NOTE: Set to 0 if timers
   are not used to avoid unnecessary timer tick overhead.
*/
/*------------------------------------------------------------------------*/


/************************ DYNAMIC SPACE ALLOCATION ************************/
/*
   The values below are used to set address markers which are used
   in the configuration table (cf) -- see core.c. Alternatively,
   actual target addresses can be loaded into cf. Sizes are in bytes.
*/
/*------------------------------------------------------------------------*/

#define  STACK_SPACE   (STACK_SIZE + STACK_PAD_SIZE) * NUM_STACKS
/*
   Space allocated for the Stack Pool.
*/
/*------------------------------------------------------------------------*/

#ifdef WIN32
/* Note: Increasing the settings may require increasing the Stack Reserve
   and Stack Commit linker settings. See STACK_AREA_NEEDED.
*/
#define  STACK_HEAP_SPACE   0x80000
/*
   Space allocated for the Stack Heap area. This is where heap stacks
   come from in Win32, since this area has to be in the range Windows
   gives the app for its stack space, not in the normal heap or else a
   Stack Fault results on a Win32 call in Win9x.
*/

#define  INIT_STACK_SIZE  0x2000
/*
   Space needed by startup code, initializers, and go_smx(). Increase
   this value if the program crashes during init. It appears less than
   0x1000 (4KB) is needed, but we set it to 0x2000 (8KB) to be sure.
*/

#define  STACK_AREA_PAD  0x2000
/*
   This pad is at the low end of the stack area. It may be needed for exit,
   since the stack pointer could be near the bottom of the lowest heap
   stack, near the boundary of the stack area, so if Windows needs more
   stack, it can't expand the space, and a page fault results. This pad
   gives extra space for the stack to grow, on exit. Usually ct will be
   the idle task when the user closes the window, so we made its stack
   large. In the case ct is not the idle task, this pad prevents a page
   fault on exit. An smx stack overflow would be reported, though, which
   is probably not a big deal, but could be avoided by making the task's
   stack larger, if you can determine which it is, or make all larger.
*/

#define  STACK_AREA_NEEDED (STACK_SPACE + STACK_HEAP_SPACE + INIT_STACK_SIZE + STACK_AREA_PAD)
/*
   This must fit in the stack area Windows gives the application, which
   is controlled by the Stack Reserve and Stack Commit linker settings.
   We recommend setting the Stack Commit == Stack Reserve.
*/
#endif  /* WIN32 */
/*------------------------------------------------------------------------*/

#if MIN_RAM
#define  ADAR_SPACE     500
#else
#define  ADAR_SPACE    5000
#endif
/*
   Space for the app dynamically allocated region (adar). This must
   be large enough for all block and message pools created with
   create_dpool() calls. Increase if INSUFF_DAR occurs when using adar.
*/
/*------------------------------------------------------------------------*/

#if MIN_RAM
#define  EB_SIZE          5
#else
#define  EB_SIZE         50
#endif
/*
   Number of error records in the error buffer. Error information is
   stored cyclically, so this determines how many errors it is possible
   to look back.
*/
/*------------------------------------------------------------------------*/

#if SMX_EVB
#if MIN_RAM
#define  EVB_SIZE       300    /* set to 0 if RAM very tight */
#else
#define  EVB_SIZE      1500
#endif
#else
#define  EVB_SIZE         0    /* don't change; avoids smxAware warnings */
#endif
/*
   Number of records in the smx event buffer. Event information is stored
   cyclically, so this determines how many events it is possible to
   look back. Note that EVBREC_SIZE is typically 24 bytes, so increasing
   EVB_SIZE to more than a few thousand can use up a lot of memory and
   slow the smxAware timeline display since it has to read the buffer via
   the BDM/JTAG connection. Set to 0 to disable the event buffer.
*/
/*------------------------------------------------------------------------*/

#if MIN_RAM
#define  HT_SIZE         50
#else
#define  HT_SIZE        100
#endif
/*
   Number of handles in the handle table. The handle table is used by
   smxProbe and smxDLM.
*/
/*------------------------------------------------------------------------*/
/*
   Static Heap Initialization: bnheap[HEAP_SPACE] in CORE.C is used
   as the heap. The following settings are added to produce HEAP_SPACE.

   Below HEAP_SPACE is an alternative way to allocate the heap, by
   scanning from a fixed address until the end of RAM. This is done
   by bspGetHeapSpace() if cf.nheap_max == 0.

   A third way to initialize the heap is to simply set cf.nheap_min
   and cf.nheap_max to the desired addresses, in CORE.C.
*/

#ifdef PEG
#define  PEG_NEARHEAP   30000  /* PegTask and PegTouchTask stacks plus a little extra. */
#else
#define  PEG_NEARHEAP       0
#endif

#ifdef SMXFILE
#define  XFILE_NEARHEAP     0
#else
#define  XFILE_NEARHEAP     0
#endif

#ifdef SMXFFS
#define  XFFS_NEARHEAP 600000  /* Varies for cache size. Depends on the block size */
#else
#define  XFFS_NEARHEAP      0
#endif

#ifdef SMXFS
#define  XFS_NEARHEAP   23000  /* Varies for cache size. See SFS_xxx_CACHE_SIZE in fcfg.h. */
#else
#define  XFS_NEARHEAP       0
#endif

#ifdef SMXNETX
#define  XNET_NEARHEAP  80000
#else
#define  XNET_NEARHEAP      0
#endif

#ifdef SMXNS
#define  XNS_NEARHEAP   50000
#else
#define  XNS_NEARHEAP       0
#endif

#ifdef SMXUSB
#define  XUSB_NEARHEAP  20000  /* Varies for HCD. See SU_HEAPSIZE in ucfg.h. */
#else
#define  XUSB_NEARHEAP      0
#endif

#ifdef SMXUSBD
#define  XUSBD_NEARHEAP  5000
#else
#define  XUSBD_NEARHEAP     0
#endif

#if MIN_RAM
#define  DEMO_HS         1500  /* 1000 for usbddemo, 500 for nsdemo using int RAM disk */
#else
#define  DEMO_HS        0x200000 // 2Mb for WebC demo
#endif

// These defines just avoid assembler "line too long" error for HEAP_SPACE
#define  BCB_HS        BCB_SIZE  * NUM_BLOCKS
#define  BXCB_HS       BXCB_SIZE * NUM_BUCKETS
#define  MCB_HS        MCB_SIZE  * NUM_MSGS
#define  PCB_HS        PCB_SIZE  * NUM_POOLS
#define  PXCB_HS       PXCB_SIZE * NUM_PIPES
#define  QCB_HS        QCB_SIZE  * NUM_QLEVELS
#define  TCB_HS        TCB_SIZE  * NUM_TASKS
#define  TMCB_HS       TMCB_SIZE * NUM_TIMERS
#define  CB_HS         BCB_HS + BXCB_HS + MCB_HS + PCB_HS + PXCB_HS + QCB_HS + TCB_HS + TMCB_HS
#define  LQ_CELL_HS    LQ_CELL_SIZE * LQ_SIZE
#define  EVB_HS        EVBREC_SIZE * EVB_SIZE
#define  PROD_HS       PEG_NEARHEAP + XFILE_NEARHEAP + XNET_NEARHEAP
#define  PROD1_HS      XFFS_NEARHEAP + XFS_NEARHEAP + XNS_NEARHEAP + XUSB_NEARHEAP + XUSBD_NEARHEAP

#define  HEAP_SPACE    CB_HS + LQ_CELL_HS + EVB_HS + PROD_HS + PROD1_HS + DEMO_HS + 3000
/*
   Space for smx objects plus near heap space. Near heap space is
   needed for pools created by create_hpooln(), for messages created
   by create_hmsgn(), for bound near stacks created by create_taskn(),
   and for blocks allocated by nmallocx(). Increase if INSUFF_HEAP
   error occurs on a near (n) call.
*/

/*-----------*/
/*
   Alternate way to set the heap: If cf.nheap_min is set to NHEAP_BEGIN
   and cf.nheap_max == 0, bspGetHeapSpace() in BSP.C will use these
   settings to scan for the end of RAM to set the end of the heap there.
   This is an alternative method to allocating the space statically
   using bnheap[HEAP_SIZE], as is done in CORE.C.
*/
#define NHEAP_BEGIN       0x00100000  /* starting address of near heap */
#define NHEAP_END         0x00000000  /* ending address of near heap. 0 means scan for end of RAM, starting at HNEAP_BEGIN */
#define RAM_SCAN_INCR     0x1000      /* scan increment can be whatever is convenient */
#define RAM_SCAN_PATTERN  0x5A5A5A5A  /* chose this value only so distinct from smx guard bands etc (but doesn't matter) */
#define NGHS_SIZE         0           /* unused in this case */
#define NGHS_FLAG         0           /* unused in this case */

/*------------------------------------------------------------------------*/


/***************************** OTHER SETTINGS *****************************/

#ifdef WIN32
#define  AINIT_STACK_SIZE  8000  /* Windows needs a bit of stack when closing the main window, */
                                 /* or else idle task gets STACK OVERFLOW on exit */
#else
#ifdef SMXNETX
#define  AINIT_STACK_SIZE  1500  /* xn_rtip_init() needs a lot of stack */
#else
#define  AINIT_STACK_SIZE  STACK_SIZE
#endif
#endif
/*
   Size of stack used by ainit() and the init routines it calls.
*/
/*------------------------------------------------------------------------*/

#define  NUM_SMXPP_GP_OBJECTS  5
/*
   Number of smx++ objects that can be allocated from the GlobalPool
   using the new() operator. Global objects (those defined at global scope
   or with the ::new operator) do not use blocks from GlobalPool.
   The smx++ demo currently uses 3.
*/
/*------------------------------------------------------------------------*/

#if MIN_RAM
#define SMX_ERROR_STRINGS      0
#else
#define SMX_ERROR_STRINGS      1
#endif
/*
   If 1, smx error strings are enabled (see xesr.c). If 0, the smx error
   message just gives the error number (see ERRNO enum in xdef.h). Setting
   this to 0 minimizes memory usage. The strings are defined as const,
   so they should be in ROM, but verify this is true for your tools.
*/
/*------------------------------------------------------------------------*/

#define SMXPEG_ERROR_WINDOW    0
#define SMXPEG_CONSOLE_WINDOW  0
#define SMXPEG_PROFILE_WINDOW  0
/*
   These make it easy to enable or disable display of these standard
   smxPEG windows.
*/
/*------------------------------------------------------------------------*/

#define PC_RTC            0   /* Leave this 0. */

/*------------------------------------------------------------------------*/

#define STRESS_TEST       0
/*
   Set to 1 to enable stress testing. The idea is to have a randomly
   generated interrupt rapidly coming in to put strain on the scheduler.
   Each interrupt causes stress_isr and stress_lsr to run, which starts
   stress_task, which will preempt the current task (if unlocked) since
   it has MAX priority. This is for Micro Digital use, but you may find
   it useful.
*/
/*------------------------------------------------------------------------*/

#endif /* SMX_CONF_H */

