 /*
 | RTPCONFIG.H - Target specific setup and configuration parameters
 |
 |
 |   PORTED TO THE Segger embOS simulation for Windows 32 bit system
 |                 and Visual C++.
 |
 |  $Author: tom $
 |  $Date: 2004/10/22 18:11:57 $
 |  $Name:  $
 |  $Revision: 1.3 $
 |
 | Copyright EBSnet Inc. , 2003
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
 |
 | Module description:
 |  [tbd]
*/

#ifndef __RTPCONFIG_H__
#define __RTPCONFIG_H__

#include "rtp.h"

/************************************************************************
* Target Specific Headers
************************************************************************/
#include "RTOS.H"

/************************************************************************
* RTPIRQ.C - Runtime Platform IRQ Services
************************************************************************/
#ifdef RTP_INCLUDE_IRQ
#include "rtpirq.h"

#endif /* RTP_INCLUDE_IRQ */

/************************************************************************
* RTPKERN.C - Runtime Platform Kernel Services
************************************************************************/
#ifdef RTP_INCLUDE_KERN
#include "rtpkern.h"

#endif /* RTP_INCLUDE_KERN */

/************************************************************************
* RTPMEM.C - Runtime Platform Memory System Services
************************************************************************/
#ifdef RTP_INCLUDE_MEM
#include "rtpmem.h"

#endif /* RTP_INCLUDE_MEM */

/************************************************************************
* RTPSIGNL.C - Runtime Platform Semaphore and Mutex Services
************************************************************************/
#ifdef RTP_INCLUDE_SIGNL
#include "rtpsignl.h"

#define RTP_CFG_N_SIGNAL 20 /* Maximum number of signals (counting semaphores) available */
#define RTP_CFG_N_MUTEX  20 /* Maximum number of mutexes (resource locks) available */

#define RTP_INC_DYNAMIC_SIGNAL  /* Define to include support for dynamic signal allocation */
#define RTP_INC_DYNAMIC_MUTEX   /* Define to include support for dynamic mutex allocation */

/* The following may be added to the API but are left here for now.
   This section provides a dynamic allocation scheme for signals and mutexes */
#ifdef RTP_INC_DYNAMIC_SIGNAL
typedef struct s_RTP_SIGNAL_ALLOC RTP_SIGNAL_ALLOC;
struct s_RTP_SIGNAL_ALLOC {
    void* (*AllocFn) (unsigned long size);
    void  (*FreeFn)  (void * ptr);
};
int  rtp_sig_semaphore_alloc_dynamic (RTP_SEMAPHORE *newSem, const char *name, 
                                       RTP_SIGNAL_ALLOC *funcptr);
#endif /* RTP_INC_DYNAMIC_SIGNAL */
#ifdef RTP_INC_DYNAMIC_MUTEX
typedef struct s_RTP_MUTEX_ALLOC RTP_MUTEX_ALLOC;
struct s_RTP_MUTEX_ALLOC {
    void* (*AllocFn) (unsigned long size);
    void  (*FreeFn)  (void * ptr);
};
int  rtp_sig_mutex_alloc_dynamic (RTP_MUTEX *newMutex, const char *name, 
                                       RTP_MUTEX_ALLOC *funcptr);
#endif /* RTP_INC_DYNAMIC_MUTEX */
/* end dynamic allocation scheme */

/* Used to manage, allocate and free signalling (Counting) semaphores   */
typedef struct s_RTP_SIGNAL_CONTEXT RTP_SIGNAL_CONTEXT;
#define RTP_SIGNAL_RESOURCE    OS_CSEMA
struct  s_RTP_SIGNAL_CONTEXT
{
    RTP_SIGNAL_CONTEXT  *next;
    char                *name;
#ifdef RTP_INC_DYNAMIC_SIGNAL
    RTP_SIGNAL_ALLOC    func;
#endif    
    RTP_SIGNAL_RESOURCE semaResource;
};

/* Used to manage, allocate and free mutex (Lock) semaphores   */
typedef struct s_RTP_MUTEX_CONTEXT RTP_MUTEX_CONTEXT;
#define RTP_MUTEX_RESOURCE    OS_RSEMA
struct  s_RTP_MUTEX_CONTEXT
{
    RTP_MUTEX_CONTEXT  *next;
    char               *name;
#ifdef RTP_INC_DYNAMIC_MUTEX
    RTP_MUTEX_ALLOC    func;
#endif    
    RTP_MUTEX_RESOURCE mutexResource;
};

#endif /* RTP_INCLUDE_SIGNL */

/************************************************************************
* RTPTERM.C - Runtime Platform Terminal I/O Services
************************************************************************/
#ifdef RTP_INCLUDE_TERM
#include "rtpterm.h"
#include "rtpstr.h"
#include <conio.h>
#include <windows.h>
#include <process.h>

#define USE_RESOURCE_LOCKS /* define to use resource locks to share the terminal resource */

#endif /* RTP_INCLUDE_TERM */

/************************************************************************
* RTPTHRD.C - Runtime Platform Thread Services
************************************************************************/
#ifdef RTP_INCLUDE_THRD
#include "rtpthrd.h"

typedef struct s_rtp_thread_context RTP_THREAD_CONTEXT;
typedef struct s_rtp_stack_context RTP_STACK_CONTEXT;

typedef struct s_rtp_thread_context
{
    RTP_THREAD_CONTEXT *next;
    int                state;
    char               *name;
    int                priority;
    void               (*entryPoint)(void *);
    void               *userData;
    RTP_STACK_CONTEXT  *stackContext;
    OS_TASK            os_tcb; 
};

typedef struct s_rtp_stack_context
{
    RTP_STACK_CONTEXT *next;
    int                state;
    int                size_index;
    int                size;
    RTP_THREAD_CONTEXT *threadContext;
    OS_STACKPTR        int *stack;
};

#define RTP_CFG_N_THREADS 6 /* Maximum number of threads which can be spawned available */

#define RTP_CFG_N_STACKS_HUGE      0
#define RTP_CFG_SIZESTACK_HUGE     0x8000
#define RTP_CFG_N_STACKS_BIG       2
#define RTP_CFG_SIZESTACK_BIG      0x4000
#define RTP_CFG_N_STACKS_NORMAL    4
#define RTP_CFG_SIZESTACK_NORMAL   0x2000
#define RTP_CFG_N_STACKS_SMALL     4
#define RTP_CFG_SIZESTACK_SMALL    0x1000
#define RTP_CFG_N_STACKS_TINY      2
#define RTP_CFG_SIZESTACK_TINY     0x200

#endif /* RTP_INCLUDE_THRD */

/************************************************************************
* RTPTIME.C - Runtime Platform Timing Services
************************************************************************/
#ifdef RTP_INCLUDE_TIME
#include "rtptime.h"

#endif /* RTP_INCLUDE_TIME */

/************************************************************************
************************************************************************/
#endif /* __RTPCONFIG_H__ */

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
