#ifndef RTOS_H_INCLUDED
#define RTOS_H_INCLUDED

/*********************************************************************
*                                                                    *
* RTOS.H : include file for embOS                                    *
*                                                                    *
* to be included in every C-module accessing embOS-routines          *
*                                                                    *
**********************************************************************

**********************************************************************
*                                                                    *
*     Part of embOS real-time operating system                       *
*                                                                    *
*     (c) Copyright SEGGER Microcontrollersysteme GmbH               *
*                                                                    *
*     www.segger.com                                                 *
*                                                                    *
**********************************************************************
*/

/*********************************************************************
*
*     Chip specifics embOS Simulation on PC
*
**********************************************************************
*/


#include <string.h>

#define OS_CPU "Simulation"

#define OS_I32 int
#define OS_U32 unsigned int

#define OS_DI() OS__di()
#define OS_EI() OS__ei()

#define OS_SIZEOF_INT 4        /* Size of integer in bytes */ 
#define OS_BIG_ENDIAN 0        /* 0 => little endian       */
#define OS_SP_ADJUST  0
#define OS_GetTime()   ((int)OS_Time)

/* Int stack is not needed in Simulation */
#define OS_EnterIntStack()
#define OS_LeaveIntStack()

#define OS_LIBMODE_DP

/* Define whether OS_Alloc is supported                             */
#define OS_SUPPORT_OS_ALLOC (1)  

/* CPU / Compiler specific functions                                */

#define OS_TCB_CPU_EX /* tcb extension for CPU port */ \
  void* hThread;                                     \
  unsigned long ThreadID;                            \
  void* pStart;                                      \
  int DICnt;                                         \
  int RegionCnt;


#define OS_EnterInterrupt() { \
  OS_DI();                    \
  OS_MARK_IN_ISR();           \
  OS_RegionCnt++;             \
  OS_DICnt++;                 \
}

#define OS_LeaveInterrupt() { \
  OS_MARK_OUTOF_ISR();        \
  OS_DICnt--; /* Must have been zero initially ! (We could put =0 instead) */\
  if (OS_RegionCnt==1) {      \
    if (OS_Pending) {         \
      OS_SwitchFromInt();     \
    }                         \
  }                           \
  OS_RegionCnt--;             \
  OS_EI();                    \
}



/*********************************************************************
*
*     Simulation specific, add. API functions
*/
OS_U32 OS_SIM_CreateISRThread(void (*pStartAddress)(void));
void   OS_SIM_Log            (const char * s);

/*****  End of chip specific part  **********************************/

/********************************************************************
*
*       Generic portion of embOS starts here
*
*********************************************************************
This file (original name OS_RAW.h) is part of RTOS.h, the include
file for embOS.
*/

#define __EMBOS__ 1          /* Enable application to find out    */
#define   EMBOS   1          /* that embOS is compiled and linked */

#define OS_VERSION (32004)   /* 3.20d */

/*********************************************************************
*
*       Defines for library types
*
**********************************************************************
*/

/* If library type is not selected yet (on project level, 
   for example), then select DP by default 
*/
#if (!defined(OS_LIBMODE_R)  && !defined(OS_LIBMODE_RP) &&   \
     !defined(OS_LIBMODE_S)  && !defined(OS_LIBMODE_SP) &&   \
     !defined(OS_LIBMODE_D)  && !defined(OS_LIBMODE_DP) &&   \
     !defined(OS_LIBMODE_ST) && !defined(OS_LIBMODE_DT) &&   \
     !defined(OS_LIBMODE_T)) 
#define OS_LIBMODE_DP        
#endif           

#if defined(OS_LIBMODE_R)
  #define OS_CHECKSTACK 0
  #define OS_DEBUG 0
  #define OS_PROFILE 0
  #define OS_SUPPORT_TICKSTEP 0
  #define OS_TRACE 0
  #define OS_CreateTask OS_CreateTask_R
  #define OS_LIBMODE "R"
#elif defined(OS_LIBMODE_RP)    /* Not normally generated */
  #define OS_CHECKSTACK 0
  #define OS_DEBUG 0
  #define OS_PROFILE 1
  #define OS_SUPPORT_TICKSTEP 1
  #define OS_TRACE 0
  #define OS_CreateTask OS_CreateTask_RP
  #define OS_LIBMODE "RP"
#elif defined(OS_LIBMODE_S)
  #define OS_CHECKSTACK 1
  #define OS_DEBUG 0
  #define OS_PROFILE 0
  #define OS_SUPPORT_TICKSTEP 0
  #define OS_TRACE 0
  #define OS_CreateTask OS_CreateTask_S
  #define OS_LIBMODE "S"
#elif defined(OS_LIBMODE_SP)
  #define OS_PROFILE 1
  #define OS_CHECKSTACK 1
  #define OS_DEBUG 0
  #define OS_SUPPORT_TICKSTEP 1
  #define OS_TRACE 0
  #define OS_CreateTask OS_CreateTask_SP
  #define OS_LIBMODE "SP"
#elif defined(OS_LIBMODE_D)
  #define OS_PROFILE 0
  #define OS_CHECKSTACK 1
  #define OS_DEBUG 1
  #define OS_SUPPORT_TICKSTEP 1
  #define OS_TRACE 0
  #define OS_CreateTask OS_CreateTask_D
  #define OS_LIBMODE "D"
#elif defined(OS_LIBMODE_DP)
  #define OS_PROFILE 1
  #define OS_CHECKSTACK 1
  #define OS_DEBUG 1
  #define OS_SUPPORT_TICKSTEP 1
  #define OS_TRACE 0
  #define OS_CreateTask OS_CreateTask_DP
  #define OS_LIBMODE "DP"
#elif defined(OS_LIBMODE_ST)
  #define OS_CHECKSTACK 1
  #define OS_DEBUG 0
  #define OS_PROFILE 1
  #define OS_SUPPORT_TICKSTEP 1
  #define OS_TRACE 1
  #define OS_CreateTask OS_CreateTask_ST
  #define OS_LIBMODE "ST"
#elif defined(OS_LIBMODE_DT)
  #define OS_CHECKSTACK 1
  #define OS_DEBUG 1
  #define OS_PROFILE 1
  #define OS_SUPPORT_TICKSTEP 1
  #define OS_TRACE 1
  #define OS_CreateTask OS_CreateTask_DT
  #define OS_LIBMODE "DT"
#elif defined(OS_LIBMODE_T)
  #error This libmode has been renamed. T -> DT
#else
  #error Please define library type used !
#endif

/*********************************************************************
*
*       embOS compile time switches
*
**********************************************************************

These compile time switches can be modified when recompiling
the library. Usually, the values are in OSCHIP.H, which is then
merged with OS_RAW.H to form RTOS.H.
If the values are not defined in OSCHIP.H, the default values
below are used.
*/

/* Make it possible to place const data in RAM via
   define and therefore via command line. This is required
   for some smaller CPUs in small memory models
   It is defined to be "const" by default, but can be defined to nothing
   by simply adding a -DOS_CONST_DATA on the commandline
*/

#ifndef OS_CONST_DATA
  #define OS_CONST_DATA const  /* Default      */
#else
  #undef OS_CONST_DATA
  #define OS_CONST_DATA        /* define blank */
#endif

#ifndef OS_STACK_ADR
  #define OS_STACK_ADR OS_U32
#endif

#ifndef OS_MODEL
  #define OS_MODEL ""
#endif

#ifndef OS_MEMSET
  #define OS_MEMSET(a,v,s) memset(a,v,s)
#endif

#ifndef OS_MEMCPY
  #define OS_MEMCPY(dest,src,cnt) memcpy(dest,src,cnt)
#endif

#ifndef OS_STRLEN
  #define OS_STRLEN(s) strlen(s)
#endif

#ifndef OS_DUMMY_STATEMENT
  #define OS_DUMMY_STATEMENT()
#endif

#ifndef OS_USEPARA
  #define OS_USEPARA(para) para=para
#endif

#ifndef OS_SUPPORT_TICKSTEP
  #define OS_SUPPORT_TICKSTEP 0
#endif

#ifndef OS_SUPPORT_TIMEOUT
  #define OS_SUPPORT_TIMEOUT 1
#endif

#ifndef OS_SUPPORT_DIAG
  #ifdef __OS_SUPPORT_DIAG__
    #define OS_SUPPORT_DIAG 1
  #else
    #define OS_SUPPORT_DIAG 0
  #endif
#endif

#ifndef OS_TRACKNAME
  #define OS_TRACKNAME (1)
#endif

#ifndef OS_TRACE
  #define OS_TRACE (0)
#endif

#ifndef __OS_RRSUPPORTED__
  #define __OS_RRSUPPORTED__ (1)
#endif

#ifndef __OS_CREATETASK0__
  #define  __OS_CREATETASK0__
#endif

#ifndef __OS_STACK_AT_BOTTOM__
  #define __OS_STACK_AT_BOTTOM__ (0)
#endif

#ifndef __OS_STACK_BOTTOM_USED
  #define __OS_STACK_BOTTOM_USED (0)
#endif

/*  Settings for NON-ANSI compiler syntax
    The default behaviour complies with IAR.
*/
#ifndef OS_COMPILER_LOCATION_LEFT
  #define OS_COMPILER_LOCATION_LEFT 1  /* (req. for IAR) */
#endif

#ifndef OS_COMPILER_STORAGE_MODIFIER_LEFT
  #define OS_COMPILER_STORAGE_MODIFIER_LEFT 1  /* default for all except KEIL */
#endif

/*********************************************************************
*
*       Check Compile time switches
*
**********************************************************************
*/

#ifndef OS_SIZEOF_INT
  #error "Please define OS_SIZEOF_INT (OSChip.h)"
#endif
#ifndef OS_BIG_ENDIAN
  #error "Please define OS_BIG_ENDIAN (OSChip.h)"
#endif

/*********************************************************************
*
*       Basic type defines
*
**********************************************************************
*/

#ifndef OS_I8
  #define OS_I8 signed char
#endif  

#ifndef OS_U8
  #define OS_U8 unsigned char
#endif

#ifndef OS_I16
  #define OS_I16 signed short
#endif

#ifndef OS_U16
  #define OS_U16 unsigned short
#endif

#ifndef OS_I32
  #define OS_I32 long
#endif

#ifndef OS_U32
  #define OS_U32 unsigned OS_I32
#endif

/* Defines a true integer. This type is guaranteed
   a) to have at least 8 bits,
   b) to compile and execute best on the target CPU
   It will normally be the same as an int, but on most
   8-bit CPUs it will be replaced by a character because
   the CPU can deal with 8 bits more efficient than with
   16 bit integers.
   Use with care !
*/
#ifndef OS_INT
  #define OS_INT       int
#endif

#ifndef OS_UINT
  #define OS_UINT       unsigned OS_INT
#endif

/*********************************************************************
*
*       Error codes
*
**********************************************************************
*/

/* stack overflow or invalid stack */
#define OS_ERR_STACK                          (120)

/* invalid or non-initialized data structures */
#define OS_ERR_INV_TASK                       (128)
#define OS_ERR_INV_TIMER                      (129)
#define OS_ERR_INV_MAILBOX                    (130)
#define OS_ERR_INV_CSEMA                      (132)
#define OS_ERR_INV_RSEMA                      (133)

/* Using GetMail1 or PutMail1 or GetMailCond1 or PutMailCond1 on
   a non-1 byte mailbox  */
#define OS_ERR_MAILBOX_NOT1                   (135)

/* internal errors, please contact SEGGER Microcontrollersysteme */
#define OS_ERR_MAILBOX_NOT_IN_LIST            (140)
#define OS_ERR_TASKLIST_CORRUPT               (142)

/* not matching routine calls or macro usage */
#define OS_ERR_UNUSE_BEFORE_USE               (150)
#define OS_ERR_LEAVEREGION_BEFORE_ENTERREGION (151)
#define OS_ERR_LEAVEINT                       (152)
#define OS_ERR_DICNT                          (153)
#define OS_ERR_INTERRUPT_DISABLED             (154)
#define OS_ERR_TASK_ENDS_WITHOUT_TERMINATE    (155)

#define OS_ERR_ILLEGAL_IN_ISR                 (160)  /* not a legal system call during interrupt */
#define OS_ERR_ILLEGAL_IN_TIMER               (161)  /* not a legal system call during timer */
#define OS_ERR_ILLEGAL_OUT_ISR                (162)  /* not a legal system call outside interrupt */

/* double used data structures */
#define OS_ERR_2USE_TASK                      (170)
#define OS_ERR_2USE_TIMER                     (171)
#define OS_ERR_2USE_MAILBOX                   (172)
#define OS_ERR_2USE_BSEMA                     (173)
#define OS_ERR_2USE_CSEMA                     (174)
#define OS_ERR_2USE_RSEMA                     (175)

/* Communication errors (OSCOM) */
#define OS_ERR_NESTED_RX_INT                  (180)

/* Fixed block memory pool  */
#define OS_ERR_MEMF_INV                       (190)
#define OS_ERR_MEMF_INV_PTR                   (191)
#define OS_ERR_MEMF_PTR_FREE                  (192)
#define OS_ERR_MEMF_RELEASE                   (193)

/* Task suspend / resume errors */
#define OS_ERR_SUSPEND_TOO_OFTEN              (200)
#define OS_ERR_RESUME_BEFORE_SUSPEND          (201)

/*********************************************************************
*
*       Trace identifier
*
**********************************************************************

Identifier from 0 to 99 are reserved for embOS
Even when not all of those are currently used,
they may be defined in the future

*/

#define OS_TRACE_ID_DEACTIVATE      (1)
#define OS_TRACE_ID_ACTIVATE        (2)
#define OS_TRACE_ID_TIMERCALLBACK   (3)

/* Task group */
#define OS_TRACE_ID_DELAY           (10)
#define OS_TRACE_ID_DELAYUNTIL      (11)
#define OS_TRACE_ID_SETPRIORITY     (12)
#define OS_TRACE_ID_WAKETASK        (13)
#define OS_TRACE_ID_CREATETASK      (14)
#define OS_TRACE_ID_TERMINATETASK   (15)
#define OS_TRACE_ID_SUSPENDTASK     (16)
#define OS_TRACE_ID_RESUMETASK      (17)

/* Event group */
#define OS_TRACE_ID_WAITEVENT       (20)
#define OS_TRACE_ID_WAITEVENT_TIMED (21)
#define OS_TRACE_ID_SIGNALEVENT     (22)
#define OS_TRACE_ID_CLEAREVENTS     (23)

/* Mailbox group  */
#define OS_TRACE_ID_CREATEMB        (30)
#define OS_TRACE_ID_CLEARMB         (31)
#define OS_TRACE_ID_DELETEMB        (32)
#define OS_TRACE_ID_PUTMAIL         (33)
#define OS_TRACE_ID_GETMAIL         (34)  
#define OS_TRACE_ID_PUTMAILCOND     (35)
#define OS_TRACE_ID_GETMAILCOND     (36)
#define OS_TRACE_ID_PUTMAIL_TIMED   (37)
#define OS_TRACE_ID_GETMAIL_TIMED   (38)
#define OS_TRACE_ID_WAITMAIL        (39)

#define OS_TRACE_ID_PUTMAIL1        (40)
#define OS_TRACE_ID_GETMAIL1        (41)
#define OS_TRACE_ID_PUTMAILCOND1    (42)
#define OS_TRACE_ID_GETMAILCOND1    (43)
#define OS_TRACE_ID_PUTMAIL_TIMED1  (44)
#define OS_TRACE_ID_GETMAIL_TIMED1  (45)

/* Resource group */
#define OS_TRACE_ID_CREATERSEMA     (50)
#define OS_TRACE_ID_USE             (51)
#define OS_TRACE_ID_UNUSE           (52)
#define OS_TRACE_ID_REQUEST         (53)
#define OS_TRACE_ID_GETSEMAVALUE    (54)

/* Counting Semaphore group */
#define OS_TRACE_ID_CREATECSEMA     (60)
#define OS_TRACE_ID_DELETECSEMA     (61)
#define OS_TRACE_ID_SIGNALCSEMA     (62)
#define OS_TRACE_ID_WAITCSEMA       (63)
#define OS_TRACE_ID_WAITCSEMATIMED  (64)

/* Timer group    */
#define OS_TRACE_ID_CREATETIMER     (70)
#define OS_TRACE_ID_DELETETIMER     (71)
#define OS_TRACE_ID_STARTTIMER      (72)
#define OS_TRACE_ID_STOPTIMER       (73)
#define OS_TRACE_ID_RETRIGGERTIMER  (74)
#define OS_TRACE_ID_SETTIMERPERIOD  (75)

/* Heap type memory management  */
#define OS_TRACE_ID_MALLOC          (80)
#define OS_TRACE_ID_FREE            (81)
#define OS_TRACE_ID_REALLOC         (82)

/* Fixed block type memory management  */
#define OS_TRACE_ID_MEMF_CREATE     (90)
#define OS_TRACE_ID_MEMF_DELETE     (91)
#define OS_TRACE_ID_MEMF_ALLOC      (92)

/*********************************************************************
*
*       Assertions
*
**********************************************************************

  Assertions are used to generate code in the debug version
  of embOS in order catch programming faults like
  bad pointers or uninitialized data structures
*/

#if OS_DEBUG
  #define OS_ASSERT(Exp, ErrCode) { if (!(Exp)) OS_Error(ErrCode); }
#else
  #define OS_ASSERT(Exp, ErrCode)
#endif

#define OS_ASSERT_INT()       OS_ASSERT((OS_InInt !=0),  OS_ERR_ILLEGAL_OUT_ISR)
#define OS_ASSERT_NOINT()     OS_ASSERT((OS_InInt ==0),  OS_ERR_ILLEGAL_IN_ISR)
#define OS_ASSERT_NOTIMER()   OS_ASSERT((OS_InTimer==0), OS_ERR_ILLEGAL_IN_TIMER)
#define OS_ASSERT_DICnt()     OS_ASSERT(((OS_DICnt&0xf0)==0) , OS_ERR_DICNT)

/*********************************************************************
*
*       Memory attributes
*
**********************************************************************
*/

#ifndef OS_STACKPTR
  #define OS_STACKPTR
#endif

#ifndef OS_SADDR
  #ifdef SADDR
    #define OS_SADDR SADDR
  #else
    #define OS_SADDR
  #endif
#endif

#ifndef OS_DEFFUNC
  #define OS_DEFFUNC
#endif

#ifndef OS_TICK_FUNC
  #define OS_TICK_FUNC OS_DEFFUNC
#endif

#ifndef OS_ASM_FUNC 
  #define OS_ASM_FUNC 
#endif

/* pointer modifier for data items */
#ifndef OS_DATAPTR
  #define OS_DATAPTR
#endif

#ifndef OS_RSEMAPTR
  #define OS_RSEMAPTR
#endif

#ifndef OS_CSEMAPTR
  #define OS_CSEMAPTR
#endif

#ifndef OS_MBPTR
  #define OS_MBPTR
#endif

#ifndef OS_TCBPTR
  #define OS_TCBPTR
#endif

#ifndef OS_TIMERPTR
  #define OS_TIMERPTR
#endif

#ifdef __cplusplus
  extern "C" {
#endif

/*********************************************************************
*
*       Data structures
*
**********************************************************************
*/

typedef void voidRoutine(void);
typedef void OS_RX_CALLBACK(OS_U8 Data);
typedef void OS_TIMERROUTINE(void);

typedef union {
  int v;
  void OS_DATAPTR * p;
} OS_TASKDATA;

typedef struct OS_task OS_TASK;
struct OS_task {
/* Start of assembly relevant section */
  OS_TASK *pNext;                    /*  Off = 0/0/0 */
  OS_U8   OS_STACKPTR *pStack;       /*  Off = 2/4/4 */
/* End of assembly relevant section */
  OS_TASKDATA Data;                  /*  Off = 4/6/8 */
  OS_U8 Priority;                    /*  Off = 6/10/12 */
  OS_U8 Stat;                        /*  Off = 7/11/13 */
  #if __OS_RRSUPPORTED__
    OS_U8  TimeSliceRem;
    OS_U8  TimeSliceReload;
  #endif
  #if OS_TRACKNAME
    const char * Name;
  #endif
  OS_TASK *pPrev;
  #if OS_SUPPORT_TIMEOUT
    int   Timeout;
  #endif  
  #if OS_CHECKSTACK
    OS_UINT  StackSize;
    #if !__OS_STACK_AT_BOTTOM__
      OS_U8 OS_STACKPTR* pStackBot;
    #endif
  #endif
  #if OS_PROFILE
    OS_U32 ExecTotal;              /* Time spent executing */ 
    OS_U32 ExecLast;               /* Time spent executing (Reference) */ 
    OS_U32 NumActivations;
    OS_U32 NumPreemptions;
    OS_U32 Load;
    #if OS_PROFILE >= 2
      OS_U32 ReactionTimeSys_TS;           /* Time stamp for reaction measurement */
      OS_U32 ReactionTimeSys_Cycles;
      OS_U32 ReactionTimeSysMax_Cycles;
    #endif
  #endif
  #if OS_SUPPORT_DIAG
    voidRoutine* LastCall;
  #endif
  OS_U8  Events;
  #if OS_DEBUG
    char Id;
  #endif
  #ifdef OS_TCB_CPU_EX
    OS_TCB_CPU_EX
  #endif
};

typedef struct OS_timer OS_TIMER;
struct OS_timer {
  OS_TIMER * pNext;
  voidRoutine* Hook;
  int   Time;
  int   Period;
  char  Active;
#if OS_DEBUG
  char Id;
#endif
};

typedef struct OS_rsema OS_RSEMA;
struct OS_rsema {
  OS_TASK * pTask;
  OS_U8 UseCnt;
  OS_U8 WaitCnt;
#if OS_DEBUG
  OS_RSEMA * pNext;
  char Id;
#endif
};

typedef struct OS_csema OS_CSEMA;
struct OS_csema {
  OS_U8 Cnt;
  OS_U8 WaitCnt;
#if OS_DEBUG
  OS_CSEMA * pNext;
  char Id;
#endif
};

typedef struct {
/* void *pNext; */
  char *pData;
  OS_UINT nofMsg;
  OS_UINT maxMsg;
  OS_UINT iRd;
  OS_U8  sizeofMsg;
  OS_U8  WaitCntNE;
  OS_U8  WaitCntNF;
#if OS_DEBUG
  char Id;
#endif
} OS_MAILBOX;

typedef struct OS_q OS_Q;
struct OS_q {
  OS_Q*    pNext;          /* ptr to next queue (for debugging / monitoring) */
  OS_U8*   pData;
  OS_UINT  Size;
  OS_UINT  MsgCnt;
  OS_UINT  offFirst;
  OS_UINT  offLast;
  OS_U8    WaitCnt;        /* Number of waiting tasks */
#if OS_DEBUG
  char Id;
#endif
};

typedef struct {
  OS_U32  Time;
  void    *pCurrentTask;
  void    *p;
  OS_U32  v;
  OS_U8   iRout;
} OS_TRACE_ENTRY;

#ifndef OS_MEMF_MAX_ITEMS
  #define OS_MEMF_MAX_ITEMS 20
#endif

#define OS_MEMF_SIZEOF_BLOCKCONTROL 0  /* sizeof(int) in future version for debugging */

typedef struct OS_memf OS_MEMF;
struct OS_memf {
  OS_MEMF* pNext;          /* ptr to next memory pool */
  void*    pPool;
  OS_U16   NumBlocks;
  OS_U16   BlockSize;
  OS_U16   NumFreeBlocks;
  OS_U16   MaxUsed;
  OS_U8    WaitCnt;
  void*    pFree;
  #if OS_DEBUG
    int  aiPurpose[OS_MEMF_MAX_ITEMS];
    char Id;
  #endif
};

/*********************************************************************
*
*       Globals
*
**********************************************************************
*/

/*
    Handle DEFinition (storage is actually allocted) versus DEClaration
    (reference) of RTOS variables depending upon who includes this header file.
*/
#ifdef __OSGLOBAL_C__
  #define OS_EXTERN        /* Declare variables if included by RTOSKERN.C */
#else
  #define OS_EXTERN extern /* Define variables if included by anyone else */
#endif

/*** Start of memory pragma areas ***
 Some compilers can not deal with memory attributes and need pragmas
*/
#if defined(__ghs) && defined(__V850)
  #pragma ghs startzda
#endif

#if OS_COMPILER_STORAGE_MODIFIER_LEFT  /* default */
  OS_EXTERN OS_SADDR  volatile OS_I32 OS_Time;
  OS_EXTERN OS_SADDR  int             OS_TimeDex;
  OS_EXTERN OS_SADDR  union { int Dummy; struct { OS_U8 Region, DI; } Cnt; } OS_Counters;

  OS_EXTERN OS_SADDR  OS_U8  OS_IntMSInc;
  OS_EXTERN OS_SADDR  OS_U8  OS_TimeSlice;
  OS_EXTERN OS_SADDR  OS_U8  OS_TimeSliceAtStart;
  OS_EXTERN OS_SADDR  OS_U8  OS_Pending;
  OS_EXTERN OS_SADDR  volatile OS_U8  OS_Status;

  #if OS_SUPPORT_TICKSTEP
    OS_EXTERN OS_SADDR  volatile OS_U8   OS_TickStep;
    OS_EXTERN OS_SADDR  volatile int     OS_TickStepTime;
  #endif
  #ifdef __RTOS_ALLOWFRACT__
    OS_EXTERN OS_SADDR        OS_UINT OS_TicksPerMS;
    OS_EXTERN OS_SADDR        int     OS_IntTicksInc;
  #endif
  #if OS_PROFILE
    OS_EXTERN OS_SADDR OS_U32 OS_TS_ExecStart;
    OS_EXTERN OS_SADDR OS_U8  ProfilingOn;
  #endif
  #if OS_DEBUG
    OS_EXTERN OS_SADDR int  OS_TaskSwitchFromIntCnt;
    OS_EXTERN OS_SADDR OS_U8 OS_InInt;
    OS_EXTERN OS_SADDR OS_U8 OS_InTimer;
  #endif

  /*** Pointers ***
    Unfortunately there is no real standard on how to define/declare
    pointers properly with extended memory attributes. There are
    currently 2 different standards, which we both support.
  */
  #if OS_COMPILER_LOCATION_LEFT
    OS_EXTERN OS_SADDR  OS_TASK    * OS_pTask;             /* linked list of all Tasks */
    OS_EXTERN OS_SADDR  OS_TASK    * OS_pCurrentTask;      /* pointer to current Task */
    OS_EXTERN OS_SADDR  OS_TASK volatile * volatile OS_pActiveTask;
    OS_EXTERN OS_SADDR  OS_TIMER   * OS_pTimer;            /* linked list of all active Timers */
    OS_EXTERN OS_SADDR  OS_TIMER   * OS_pCurrentTimer;     /* Actual expired timer which called callback */
    #if OS_DEBUG
      OS_EXTERN OS_SADDR OS_RSEMA  * OS_pRSema;
      OS_EXTERN OS_SADDR OS_TASK   * OS_pTask2Debug;       /* linked list of all Tasks */
      OS_EXTERN OS_SADDR voidRoutine         * OS_pfOnTask2Debug;
      extern OS_CSEMA OS_CSEMAPTR* OS_pCSema;
    #endif
  #else
    OS_EXTERN OS_TASK             * OS_SADDR OS_pTask;          /* linked list of all Tasks */
    OS_EXTERN OS_TASK             * OS_SADDR OS_pCurrentTask;   /* pointer to current Task */
    OS_EXTERN OS_TASK    volatile * OS_SADDR volatile OS_pActiveTask;
    OS_EXTERN OS_TIMER            * OS_SADDR OS_pTimer;         /* linked list of all active Timers */
    OS_EXTERN OS_TIMER            * OS_SADDR OS_pCurrentTimer;  /* Actual expired timer which called callback */
    #if OS_DEBUG
      OS_EXTERN OS_RSEMA          * OS_SADDR OS_pRSema;
      OS_EXTERN OS_TASK           * OS_SADDR OS_pTask2Debug;    /* linked list of all Tasks */
      OS_EXTERN voidRoutine                 * OS_SADDR OS_pfOnTask2Debug;
      extern OS_CSEMA OS_CSEMAPTR* OS_pCSema;
    #endif
  #endif

#else  /*  OS_COMPILER_STORAGE_MODIFIER_LEFT == 0, needed for Keil  */
  OS_EXTERN volatile OS_I32 OS_SADDR OS_Time;
  OS_EXTERN int             OS_SADDR OS_TimeDex;
  OS_EXTERN union { int Dummy; struct { OS_U8 Region, DI; } Cnt; } OS_SADDR OS_Counters;

  OS_EXTERN OS_U8 OS_SADDR OS_IntMSInc;
  OS_EXTERN OS_U8 OS_SADDR OS_TimeSlice;
  OS_EXTERN OS_U8 OS_SADDR OS_TimeSliceAtStart;
  OS_EXTERN OS_U8 OS_SADDR OS_Pending;
  OS_EXTERN volatile OS_U8 OS_SADDR OS_Status;

  #if OS_SUPPORT_TICKSTEP
    OS_EXTERN volatile OS_U8 OS_SADDR OS_TickStep;
    OS_EXTERN volatile int   OS_SADDR OS_TickStepTime;
  #endif
  #ifdef __RTOS_ALLOWFRACT__
    OS_EXTERN OS_UINT OS_SADDR OS_TicksPerMS;
    OS_EXTERN int     OS_SADDR OS_IntTicksInc;
  #endif
  #if OS_PROFILE
    OS_EXTERN OS_U32 OS_SADDR OS_TS_ExecStart;
    OS_EXTERN OS_U8  OS_SADDR ProfilingOn;
  #endif
  #if OS_DEBUG
    OS_EXTERN int   OS_SADDR OS_TaskSwitchFromIntCnt;
    OS_EXTERN OS_U8 OS_SADDR OS_InInt;
    OS_EXTERN OS_U8 OS_SADDR OS_InTimer;
  #endif

  /*** Pointers ***
    Unfortunately there is no real standard on how to define/declare
    pointers properly with extended memory attributes. There are
    currently 2 different standards, which we both support.
  */
  #if OS_COMPILER_LOCATION_LEFT
    OS_EXTERN OS_SADDR  OS_TASK    * OS_pTask;             /* linked list of all Tasks */
    OS_EXTERN OS_SADDR  OS_TASK    * OS_pCurrentTask;      /* pointer to current Task */
    OS_EXTERN OS_SADDR  OS_TASK volatile * volatile OS_pActiveTask;
    OS_EXTERN OS_SADDR  OS_TIMER   * OS_pTimer;            /* linked list of all active Timers */
    OS_EXTERN OS_SADDR  OS_TIMER   * OS_pCurrentTimer;     /* Actual expired timer which called callback */
    #if OS_DEBUG
      OS_EXTERN OS_SADDR OS_RSEMA  * OS_pRSema;
      OS_EXTERN OS_SADDR OS_TASK   * OS_pTask2Debug;       /* linked list of all Tasks */
      OS_EXTERN OS_SADDR voidRoutine         * OS_pfOnTask2Debug;
      extern OS_CSEMA OS_CSEMAPTR* OS_pCSema;
    #endif
  #else
    OS_EXTERN OS_TASK             * OS_SADDR OS_pTask;        /* linked list of all Tasks */
    OS_EXTERN OS_TASK             * OS_SADDR OS_pCurrentTask; /* pointer to current Task */
    OS_EXTERN OS_TASK    volatile * volatile OS_SADDR OS_pActiveTask;
    OS_EXTERN OS_TIMER            * OS_SADDR OS_pTimer;       /* linked list of all active Timers */
    OS_EXTERN OS_TIMER            * OS_SADDR OS_pCurrentTimer;  /* Actual expired timer which called callback */
    #if OS_DEBUG
      OS_EXTERN OS_RSEMA          * OS_SADDR OS_pRSema;
      OS_EXTERN OS_TASK           * OS_SADDR OS_pTask2Debug;  /* linked list of all Tasks */
      OS_EXTERN voidRoutine                 * OS_SADDR OS_pfOnTask2Debug;
      extern OS_CSEMA OS_CSEMAPTR* OS_pCSema;
    #endif
  #endif

#endif  /*  OS_COMPILER_STORAGE_MODIFIER_LEFT  */

/*** End of memory pragma areas ****/
#if defined(__ghs) && defined(__V850)
  #pragma ghs endzda
#endif

#define OS_RegionCnt OS_Counters.Cnt.Region
#define OS_DICnt     OS_Counters.Cnt.DI

/*********************************************************************
*
*       Create_Task routine and macros (Module OsKern.c)
*
**********************************************************************
*/

#if OS_TRACKNAME
  #define CTPARA_NAME "unnamed task",
#else
  #define CTPARA_NAME
#endif

#if __OS_RRSUPPORTED__
  #define CTPARA_TIMESLICE ,2
#else
  #define CTPARA_TIMESLICE 
#endif

/*
   This is the old form. For compatibility reasons it is
   still supported.
*/
#define CREATETASK(pTask, Hook, Priority, pStack) \
OS_CreateTask (pTask,           \
               CTPARA_NAME      \
               Priority,        \
               Hook,            \
               (void OS_STACKPTR*)pStack, \
               sizeof(pStack)   \
               CTPARA_TIMESLICE \
               )

#if OS_TRACKNAME
  #define OS_CREATETASK(pTask, Name, Hook, Priority, pStack) \
  OS_CreateTask (pTask,            \
                  Name,            \
                  Priority,        \
                  Hook,            \
                  (void OS_STACKPTR*)pStack, \
                  sizeof(pStack)   \
                  CTPARA_TIMESLICE \
               )
#else
  #define OS_CREATETASK(pTask, Name, Hook, Priority, pStack) \
  OS_CreateTask (pTask,            \
                  Priority,        \
                  Hook,            \
                  (void OS_STACKPTR*)pStack, \
                  sizeof(pStack)   \
                  CTPARA_TIMESLICE \
               )
#endif

OS_DEFFUNC void OS_CreateTask(
        OS_TASK * pTask,
#if OS_TRACKNAME
        const char*Name,
#endif
        OS_U8 Priority,
        voidRoutine*pRoutine,
        void OS_STACKPTR *pStack,
        OS_UINT StackSize
#if __OS_RRSUPPORTED__
        ,OS_UINT TimeSlice
#endif
        );

/*********************************************************************
*
*       Task related routines
*
**********************************************************************
*/

OS_DEFFUNC void OS_Delay(int ms);                    /* OSKern.c     */
OS_DEFFUNC void OS_DelayUntil(int t);                /* OSKern.c     */
OS_DEFFUNC void OS_Terminate      (OS_TASK* pTask);  /* OSTerm.c     */
OS_DEFFUNC void OS_TerminateError (void);            /* OSTermE.c    */
OS_DEFFUNC void OS_WakeTask       (OS_TASK* pTask);  /* OSWake.c     */
OS_DEFFUNC void OS_GetState       (OS_TASK* pTask);
OS_DEFFUNC void OS_SetPriority    (OS_TASK* pTask, OS_U8 Prio); /* OSSetP.c */
OS_DEFFUNC char OS_IsTask         (OS_TASK* pTask);  /* OSIsTask.c   */
OS_DEFFUNC void OS_Suspend        (OS_TASK* pTask);  /* OS_Suspend.c */
OS_DEFFUNC void OS_Resume         (OS_TASK* pTask);  /* OS_Suspend.c */

/*********************************************************************
*
*       Info routines (OSInfo.c)
*
**********************************************************************
*/

OS_DEFFUNC const char* OS_GetCPU(void);
OS_DEFFUNC const char* OS_GetLibMode(void);
OS_DEFFUNC const char* OS_GetModel(void);
OS_DEFFUNC const char* OS_GetLibName(void);
#define OS_GetVersion() ((unsigned int)OS_VERSION)
#define OS_GetType() OS_GetLibMode()

#define OS_TASKID OS_TASK *
#define OS_GetTaskID() OS_pCurrentTask

#define OS_GetpCurrentTask()  OS_pCurrentTask
#define OS_GetpCurrentTimer() OS_pCurrentTimer

#if OS_TRACKNAME
  OS_DEFFUNC const char* OS_GetTaskName(OS_TASK *);
#else
  #define OS_GetTaskName(pt)  ((char*)NULL)
#endif

#if OS_CHECKSTACK
  OS_DEFFUNC void OS_CheckStack(void);              /* internal usage */
  OS_DEFFUNC int  OS_GetStackSpace(OS_TASK * pTask);
  OS_DEFFUNC int  OS_GetStackUsed (OS_TASK * pTask);
  OS_DEFFUNC int  OS_GetStackSize (OS_TASK OS_TCBPTR * pTask);
  OS_DEFFUNC void OS_STACKPTR* OS_GetStackBase(OS_TASK OS_TCBPTR *pTask);
#else
  #define OS_CheckStack()
  #define OS_GetStackSpace(pt) 0
  #define OS_GetStackUsed(pt)  0
  #define OS_GetStackSize(pt)  0
  #define OS_GetStackBase(pt)  0
#endif

OS_ASM_FUNC OS_STACK_ADR OS_GetSysStackBase(void);  /* RTOS asm part */
OS_ASM_FUNC unsigned int OS_GetSysStackSize(void);  /* RTOS asm part */
OS_ASM_FUNC OS_STACK_ADR OS_GetIntStackBase(void);  /* RTOS asm part */
OS_ASM_FUNC unsigned int OS_GetIntStackSize(void);  /* RTOS asm part */

#if __OS_RRSUPPORTED__
  OS_U8 OS_SetTimeSlice(OS_TASK * pTask, OS_U8 TimeSlice);
  OS_DEFFUNC OS_U8 OS_GetTimeSliceRem(OS_TASK OS_TCBPTR * pTask);
  #define OS_GetTimeSliceReload(pTask)  pTask->TimeSliceReload
#else
  #define OS_GetTimeSliceRem(pTask)     0
  #define OS_GetTimeSliceReload(pTask)  0
#endif

OS_DEFFUNC OS_U8    OS_GetPriority(OS_TASK * pTask);  /* OS_GetPrio.c       */
#define OS_GET_PRIORITY(pt) pt->Priority 
                                                      /* OS_GetNumTasks.c   */
OS_DEFFUNC int   OS_GetNumTasks(void);
OS_DEFFUNC OS_TASK* OS_TaskIndex2Ptr(int TaskIndex);  /* OS_TaskIndex2Ptr.c */

#if OS_PROFILE >=1
  OS_DEFFUNC OS_U32 OS_STAT_GetExecTime_Cycles (OS_TASK *pTask);  /* OSStat.c   */
  OS_DEFFUNC int    OS_STAT_GetLoad            (OS_TASK *pTask);  /* OSStatGL.c */
  OS_DEFFUNC OS_U32 OS_STAT_GetNumActivations  (OS_TASK *pTask);  /* OSStat.c   */
  OS_DEFFUNC OS_U32 OS_STAT_GetNumPreemptions  (OS_TASK *pTask);  /* OSStat.c   */
  OS_DEFFUNC void   OS_STAT_NotifyReadyStart   (OS_TASK *pTask);  /* OSStat.c   */
  OS_DEFFUNC void   OS_STAT_Sample(void);                         /* OSStat.c   */
#else
  #define OS_STAT_NotifyExecEnd()
  #define OS_STAT_GetExecTime_Cycles(pTask)   0
  #define OS_STAT_GetLoad(pTask)              0
  #define OS_STAT_GetNumActivations(pTask)    0
  #define OS_STAT_GetNumPreemptions(pTask)    0
  #define OS_STAT_Sample()
  #define OS_STAT_SetClocksPerUnit(ClocksPerUnit)
  #define OS_STAT_NOTIFYREADYSTART(pTask)
#endif

#if OS_PROFILE >=2
  #define OS_STAT_NOTIFYREADYSTART(pTask) OS_STAT_NotifyReadyStart(pTask)
#else
  #define OS_STAT_NOTIFYREADYSTART(pTask)
#endif

/*********************************************************************
*
*       Critical regions (OsKern.c)
*
**********************************************************************
*/

#define OS_EnterRegion() {OS_RegionCnt++; }
OS_DEFFUNC void OS_LeaveRegion(void);

/* Macro for OS_Leaveregion.
   Main purposes:
   - Used in OS-Kernel
   - Offers the high speed variant (macro) instead of the function call
*/

#define OS_LEAVEREGION_STD() \
 OS_DI(); \
 if (!--OS_RegionCnt) { \
   if (OS_Pending) OS_Switch(); \
  } OS_RestoreI();

#if OS_DEBUG
 #define OS_LEAVEREGION() \
   if (!OS_RegionCnt) OS_Error(OS_ERR_LEAVEREGION_BEFORE_ENTERREGION); \
   OS_LEAVEREGION_STD()
#else
 #define OS_LEAVEREGION()  OS_LEAVEREGION_STD()
#endif

/*********************************************************************
*
*       Interrupt save/disable/restore macros
*
**********************************************************************
*/
#define OS_IncDI()       { OS_ASSERT_DICnt(); OS_DI(); OS_DICnt++; }
#define OS_DecRI()       { OS_ASSERT_DICnt(); if (--OS_DICnt==0) OS_EI(); }
#define OS_RestoreI()    { OS_ASSERT_DICnt(); if (OS_DICnt==0)   OS_EI(); }

/*********************************************************************
*
*       ISR (Interrupt service routine) support
*
**********************************************************************
*/

#define RTOS_PPENDING           (1)     /*      Preemption  pending */
#define RTOS_TSPENDING          (2)     /*      Task switch pending */
#define RTOS_RRPENDING          (4)     /*      Round robin pending */

#if OS_DEBUG
  #define OS_MARK_IN_ISR()         {OS_InInt++;}
  #define OS_MARK_OUTOF_ISR()      {if (!OS_InInt--) OS_Error(OS_ERR_LEAVEINT);}
#else
  #define OS_MARK_IN_ISR()
  #define OS_MARK_OUTOF_ISR()
#endif

#ifndef OS_EnterInterrupt   /* Allow override in OSChip.h */
  #define OS_EnterInterrupt() { \
    OS_MARK_IN_ISR(); \
    OS_RegionCnt++; \
    OS_DICnt++; \
  }
#endif

#ifndef OS_EnterNestableInterrupt   /* Allow override in OSChip.h */
  #define OS_EnterNestableInterrupt() { \
    OS_MARK_IN_ISR();  \
    OS_RegionCnt++; \
    OS_EI(); \
  }
#endif

#ifndef OS_LeaveInterrupt   /* Allow override in OSChip.h */
  #define OS_LeaveInterrupt() { \
    OS_MARK_OUTOF_ISR(); \
    OS_DICnt--; /* Must have been zero initially ! (We could put =0 instead) */\
    if (OS_RegionCnt==1) { \
      if (OS_Pending) { \
        OS_SwitchFromInt(); \
      } \
    } \
    OS_RegionCnt--; \
  }
#endif

#ifndef OS_LeaveNestableInterrupt   /* Allow override in OSChip.h */
  #define OS_LeaveNestableInterrupt() { \
    OS_DI();                \
    OS_MARK_OUTOF_ISR();    \
    if (OS_RegionCnt==1) {  \
      if (OS_Pending) {     \
        OS_SwitchFromInt(); \
      }                     \
    }                       \
    OS_RegionCnt--;         \
  }
#endif

#ifndef OS_LeaveInterruptNoSwitch   /* Allow override in OSChip.h */
  #define OS_LeaveInterruptNoSwitch() { \
    OS_MARK_OUTOF_ISR(); \
    OS_DICnt--; \
    OS_RegionCnt--; \
  }
#endif

#ifndef OS_LeaveNestableInterruptNoSwitch   /* Allow override in OSChip.h */
  #define OS_LeaveNestableInterruptNoSwitch() { \
    OS_DI(); \
    OS_MARK_OUTOF_ISR(); \
    OS_RegionCnt--; \
  }
#endif

#ifndef OS_EnterIntStack
  OS_DEFFUNC void OS__EnterIntStack(void);
  #define OS_EnterIntStack() {OS_DI(); OS__EnterIntStack(); OS_RestoreI(); }
#endif

#ifndef OS_LeaveIntStack
  OS_DEFFUNC void OS__LeaveIntStack(void);
  #define OS_LeaveIntStack() {OS_DI(); OS__LeaveIntStack(); }
#endif

/*********************************************************************
*
*       Resource semaphores
*
**********************************************************************
*/

OS_DEFFUNC int      OS_Use             (OS_RSEMA * pRSema); /* OSRsem.c   */
OS_DEFFUNC void     OS_Unuse           (OS_RSEMA * pRSema); /* OSRsem.c   */
OS_DEFFUNC char     OS_Request         (OS_RSEMA * pRSema); /* OSRsemRQ.c */
OS_DEFFUNC int      OS_GetSemaValue    (OS_RSEMA * pRSema); /* OSRSemGV.c */
OS_DEFFUNC OS_TASK* OS_GetResourceOwner(OS_RSEMA * pRSema); /* OSRsemGO.c */

OS_DEFFUNC void     OS_CreateRSema     (OS_RSEMA * pRSema); /* OSRsem.c   */

#define OS_CREATERSEMA(ps) OS_CreateRSema(ps)

/*********************************************************************
*
*       Counting semaphores
*
**********************************************************************
*/

OS_DEFFUNC void OS_CreateCSema   (OS_CSEMA * pCSema, OS_U8 InitValue);  /* OSCSEM.c   */
OS_DEFFUNC void OS_DeleteCSema   (OS_CSEMA * pCSema);                   /* OSDELCS.c  */
OS_DEFFUNC int  OS_GetCSemaValue (OS_CSEMA * pCSema);                   /* OSCSEMGV.c */
OS_DEFFUNC void OS_SignalCSema   (OS_CSEMA * pCSema);                   /* OSCSEM.c   */
OS_DEFFUNC void OS_WaitCSema     (OS_CSEMA * pCSema);                   /* OSCSEM.c   */
OS_DEFFUNC int  OS_WaitCSemaTimed(OS_CSEMA * pCSema, int TimeOut);      /* OSCSEMT.c  */

#define OS_CREATECSEMA(ps) OS_CreateCSema(ps,0)

/*********************************************************************
*
*       Mailboxes
*
**********************************************************************
*/
#define CREATEMB(MAILBOX, size, max, Buffer) OS_CreateMB(MAILBOX,size, max, Buffer);
OS_DEFFUNC void OS_CreateMB     (OS_MAILBOX * pMB, OS_U8 sizeofMsg, OS_UINT maxnofMsg, void* Buffer);    /* initialize mailbox */
OS_DEFFUNC void OS_ClearMB      (OS_MAILBOX * pMB);
OS_DEFFUNC void OS_PutMail      (OS_MAILBOX * pMB, void* pMail);
OS_DEFFUNC char OS_PutMailCond  (OS_MAILBOX * pMB, void* pMail);
OS_DEFFUNC void OS_GetMail      (OS_MAILBOX * pMB, void* pDest);
OS_DEFFUNC char OS_GetMailCond  (OS_MAILBOX * pMB, void* pDest);
OS_DEFFUNC void OS_PutMail1     (OS_MAILBOX * pMB, const char* pMail);
OS_DEFFUNC char OS_PutMailCond1 (OS_MAILBOX * pMB, const char* pMail);
OS_DEFFUNC void OS_GetMail1     (OS_MAILBOX * pMB, char* pDest);
OS_DEFFUNC char OS_GetMailCond1 (OS_MAILBOX * pMB, char* pDest);
OS_DEFFUNC char OS_GetMailTimed (OS_MAILBOX * pMB, void* pDest, int Timeout);
OS_DEFFUNC void OS_DeleteMB     (OS_MAILBOX * pMB);
OS_DEFFUNC void OS_WaitMail     (OS_MAILBOX * pMB);

#if OS_DEBUG == 0
  #define OS_GetMessageCnt(pMB) (*pMB).nofMsg
#else
  OS_DEFFUNC int OS_GetMessageCnt(OS_MAILBOX * pMB);   /* get no. of available Messages */
#endif

/*********************************************************************
*
*       Message Queues (OSQ.c)
*
**********************************************************************
*/

OS_DEFFUNC void OS_Q_Create(OS_Q* pQ, void*pData, OS_UINT Size);
OS_DEFFUNC void OS_Q_Clear(OS_Q* pQ);                    /* OSQCL.c  */
OS_DEFFUNC int  OS_Q_GetMessageCnt(OS_Q* pQ);            /* OSQGMC.c */
OS_DEFFUNC int  OS_Q_Put(OS_Q* pQ, const void* pSrc, OS_UINT Size);
OS_DEFFUNC int  OS_Q_GetPtr(OS_Q* pQ, void**ppData);
OS_DEFFUNC int  OS_Q_GetPtrCond(OS_Q* pQ, void**ppData); /* OSQGPC.c */
OS_DEFFUNC void OS_Q_Purge(OS_Q* pQ);

/*********************************************************************
*
*       Events
*
**********************************************************************
*/

OS_DEFFUNC char OS_ClearEvents          (OS_TASK * pTask);             /* OSEVENCL.c  */
OS_DEFFUNC char OS_GetEventsOccured     (OS_TASK * pTask);             /* OSEVENGE.c  */
OS_DEFFUNC void OS_SignalEvent          (char Event, OS_TASK * pTask); /* OSENENS.c   */
OS_DEFFUNC char OS_WaitEvent            (char EventMask);              /* OSEVENW.c   */
OS_DEFFUNC char OS_WaitEventTimed       (char EventMask, int TimeOut); /* OSEVENT.c   */
OS_DEFFUNC char OS_WaitSingleEvent      (char EventMask);              /* OSEVENWS.c  */
OS_DEFFUNC char OS_WaitSingleEventTimed (char EventMask, int TimeOut); /* OSEVENWST.c */

/*********************************************************************
*
*       Timers(OSTIME.c)
*
**********************************************************************
*/

OS_DEFFUNC void    OS_CreateTimer    (OS_TIMER * pTimer, OS_TIMERROUTINE* Callback, OS_UINT Timeout);
OS_DEFFUNC void    OS_RetriggerTimer (OS_TIMER * pTimer);
OS_DEFFUNC void    OS_StartTimer     (OS_TIMER * pTimer);
OS_DEFFUNC void    OS_StopTimer      (OS_TIMER * pTimer);
OS_DEFFUNC void    OS_DeleteTimer    (OS_TIMER * pTimer);                  /* OSTIMED.c  */
OS_DEFFUNC int     OS_GetTimerPeriod (OS_TIMER * pTimer);                  /* OSTIMEGP.c */
OS_DEFFUNC OS_U8   OS_GetTimerStatus (OS_TIMER * pTimer);                  /* OSTIMEGS.c */
OS_DEFFUNC int     OS_GetTimerValue  (OS_TIMER * pTimer);                  /* OSTIMEGV.c */
OS_DEFFUNC void    OS_SetTimerPeriod (OS_TIMER * pTimer, OS_UINT Period);  /* OSTIMES.c */

#define OS_CREATETIMER(pTimer,c,d) \
        OS_CreateTimer(pTimer,c,d); \
        OS_StartTimer(pTimer);

/*********************************************************************
*
*       Heap type memory management (OS_Alloc.c)
*
**********************************************************************

  This functions might not be implemented in all embOS ports.
  Therefore declaration depends on condition OS_SUPPORT_OS_ALLOC
  which has to be defined in CPU specific part

*/

#if OS_SUPPORT_OS_ALLOC
  OS_DEFFUNC void* OS_malloc(unsigned int);
  OS_DEFFUNC void  OS_free  (void* pMemBlock);
  OS_DEFFUNC void* OS_realloc  (void* pMemBlock, unsigned NewSize);
#endif

/*********************************************************************
*
*       Fixed Block memory management
*
**********************************************************************
*/

OS_DEFFUNC void  OS_MEMF_Create(OS_MEMF* pMEMF, void* pPool, OS_U16 NumBlocks, OS_U16 BlockSize);
OS_DEFFUNC void  OS_MEMF_Delete(OS_MEMF* pMEMF);
OS_DEFFUNC void* OS_MEMF_Alloc(OS_MEMF* pMEMF, int Purpose);
OS_DEFFUNC void* OS_MEMF_AllocTimed(OS_MEMF* pMEMF, int Timeout, int Purpose);
OS_DEFFUNC void* OS_MEMF_Request(OS_MEMF* pMEMF, int Purpose);
OS_DEFFUNC void  OS_MEMF_Release(OS_MEMF* pMEMF, void* pMemBlock);
OS_DEFFUNC void  OS_MEMF_FreeBlock(void* pMemBlock);
OS_DEFFUNC int   OS_MEMF_GetNumFreeBlocks(OS_MEMF* pMEMF);
OS_DEFFUNC char  OS_MEMF_IsInPool(OS_MEMF* pMEMF, void* pMemBlock);
OS_DEFFUNC int   OS_MEMF_GetMaxUsed(OS_MEMF* pMEMF);
OS_DEFFUNC int   OS_MEMF_GetNumBlocks(OS_MEMF* pMEMF);
OS_DEFFUNC int   OS_MEMF_GetBlockSize(OS_MEMF* pMEMF);

/*********************************************************************
*
*       Timing support (OSTiming.c)
*
**********************************************************************

 New in V3.04, but not yet documented to the end user. Great for
 run-time measurement.
*/

#define OS_TIMING OS_U32
OS_DEFFUNC void    OS_Timing_Start(OS_TIMING* pCycle);
OS_DEFFUNC void    OS_Timing_End(OS_TIMING* pCycle);
OS_DEFFUNC OS_U32  OS_Timing_Getus(OS_TIMING* pCycle);
#define OS_Timing_GetCycles(pPara) (*pPara)

/*********************************************************************
*
*       Advanced profiling support (Module OsKern.c)
*
**********************************************************************
*/
#if OS_PROFILE >= 1
  OS_DEFFUNC void OS_EnableProfiling(int Period);
  #define OS_DisableProfiling() { OS_ProfilingOn=0; }
#else
  #define OS_EnableProfiling(Period);
  #define OS_DisableProfiling()
#endif

/*********************************************************************
*
*       Configuration
*
**********************************************************************
*/

#ifdef __RTOS_ALLOWFRACT__  /* For K0 only */
  #define OS_CONFIG(f, div) \
  OS_TicksPerMS  = f/1000; \
  OS_IntMSInc    = div*1000L/f; \
  OS_IntTicksInc = div-(f/1000)*(div*1000L/f);
#else
  #define OS_CONFIG(f, div) \
  OS_IntMSInc    = div*1000L/f;
#endif

/*********************************************************************
*
*       Trace support (OSTrace.c)
*
**********************************************************************

Trace support is enabled by defining OS_TRACE 1.
This is automatically done, when OS_LIBMODE_T is defined.

*/

#if OS_TRACE
  OS_DEFFUNC void OS_TraceVoid (OS_U8 id);
  OS_DEFFUNC void OS_TracePtr  (OS_U8 id, void* p);
  OS_DEFFUNC void OS_TraceData (OS_U8 id, int v);
  OS_DEFFUNC void OS_TraceDataPtr (OS_U8 id, int v, void*p);
  OS_DEFFUNC void OS_TraceU32Ptr  (OS_U8 id, OS_U32 p0, void*p1);
  OS_DEFFUNC void OS_TraceEnable(void);
  OS_DEFFUNC void OS_TraceDisable(void);
  OS_DEFFUNC void OS_TraceEnableAll(void);                              /* OSTrace1.c */
  OS_DEFFUNC void OS_TraceDisableAll(void);                             /* OSTrace1.c */
  OS_DEFFUNC void OS_TraceEnableId(OS_U8 id);                           /* OSTrace1.c */
  OS_DEFFUNC void OS_TraceDisableId(OS_U8 id);                          /* OSTrace1.c */
  OS_DEFFUNC void OS_TraceEnableFilterId(OS_U8 FilterIndex, OS_U8 id);  /* OSTrace1.c */
  OS_DEFFUNC void OS_TraceDisableFilterId(OS_U8 FilterIndex, OS_U8 id); /* OSTrace1.c */
#else
  #define OS_TraceVoid(id)
  #define OS_TracePtr(id, p)
  #define OS_TraceData(id, v)
  #define OS_TraceDataPtr(id, v, p)
  #define OS_TraceU32Ptr(id, p0, p1)
  #define OS_TraceEnable()
  #define OS_TraceDisable()
  #define OS_TraceEnableAll()
  #define OS_TraceDisableAll()
  #define OS_TraceEnableId(id)
  #define OS_TraceDisableId(id)
  #define OS_TraceEnableFilterId(FilterIndex, id)
  #define OS_TraceDisableFilterId(FilterIndex, id)
#endif  

/*********************************************************************
*
*        RTOSInit and misc.
*
**********************************************************************

The routines declared below are available on all targets.

*/

/* Routines in RTOSInit.c */
void   OS_InitHW(void);
void   OS_Idle(void);
OS_U32 OS_ConvertCycles2us(OS_U32 Cycles);
OS_U32 OS_GetTime_Cycles(void);
void   OS_COM_Init(void);
void   OS_COM_Send1(unsigned char c);

/* Routines in OS_Error.c, delivered as source file */
void   OS_Error(int code);

/* Routines in library */
OS_DEFFUNC  void OS_ChangeTask(void);

OS_ASM_FUNC void OS_Start(void);                /* RTOS ASM part   */
OS_ASM_FUNC void OS_Switch(void);               /* RTOS ASM part   */
OS_ASM_FUNC void OS_SwitchFromInt(void);        /* RTOS ASM part   */

OS_DEFFUNC void  OS_InitKern(void);             /* OSKern.c        */
OS_DEFFUNC OS_U8 OS_OnTx(void);                 /* OSComRxTx1.c Returns 1 on last byte */
OS_DEFFUNC void  OS_OnRx(unsigned char c);      /* OSComRxTx1.c    */
OS_DEFFUNC void  OS_Send1(OS_U8 c);             /* not implemented */
OS_DEFFUNC void  OS_SendString(const char* s);  /* OSSend.c        */
OS_DEFFUNC OS_RX_CALLBACK* OS_SetRxCallback(OS_RX_CALLBACK* cb); 
OS_DEFFUNC OS_U8 OS_EvaPacketEx(const OS_U8 * pSrc, OS_U16 SrcLen, OS_U8** pReturn);  /* OS_EvaPacketEx.c */

OS_DEFFUNC void  OS_COM_ClearTxActive(void);    /* OSCom2.c */

/*********************************************************************
*
*        RTOS misc. optional 
*
**********************************************************************

The routines declared below are available on some targets
only. (Because they do not make much sense on others)
*/

/* Timing routines. Their existance depends on the CPU. In general,
  8-bit CPUs require both routines, where 16-bit CPUs require one
  and 32-bit CPUs require none of these.
*/
#ifndef OS_GetTime
  int OS_GetTime(void);
#endif
#ifndef OS_GetTime32
  OS_I32 OS_GetTime32(void);
#endif

#ifndef OS_SUB_INT
  #define OS_SUB_INT(a, b) (a-b)
#endif
  
void OS__di(void);
void OS__ei(void);
void OS__SaveIPL(void);
void OS__RestIPL(void);
void OS_SIM_Init(void);

OS_TICK_FUNC void OS_TickHandler(void);    
OS_TICK_FUNC void OS_HandleTickDI(void);
OS_TICK_FUNC void OS_HandleTick(void);     /* New version of OS_TickHandler() without Enter- Leave-Interrupt */

/* Routines for the PC-version running in native mode (non-windows) */
void OS_SetIntFreq(OS_UINT f);
void OS_Exit(int code);
void OS_UseDos(void);
void OS_UnuseDos(void);
int  OS_GetKey(void);
int  OS_WaitKey(void);

/* Compatibility with manual */
#define OS_CREATEMB          OS_CreateMB
#define OS_GetUseCnt         OS_GetSemaValue     
#define OS_WaitCSema_Timed   OS_WaitCSemaTimed
#define OS_WaitEvent_Timed   OS_WaitEventTimed

/********************************************************************/

#ifdef __cplusplus
  }
#endif

#endif /* RTOS_H_INCLUDED */

/*****  EOF  ********************************************************/

