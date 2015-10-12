// RTPtest.c : 
//

/************************************************************************
* Define RTP Services Used
************************************************************************/
#define RTP_INCLUDE_IRQ
#define RTP_INCLUDE_KERN
#define RTP_INCLUDE_MEM
#define RTP_INCLUDE_SIGNL
#define RTP_INCLUDE_TERM
#define RTP_INCLUDE_THRD
#define RTP_INCLUDE_TIME
#include "rtpconfig.h"

/************************************************************************
* Allocate and Configure Signal and Mutex resource pools
* (See rtpconfig.h for structures and defines)
************************************************************************/
/* Configure and allocate Signal and Mutex resources */
RTP_SIGNAL_CONTEXT  rtpSignalContext[RTP_CFG_N_SIGNAL];
RTP_MUTEX_CONTEXT   rtpMutexContext[RTP_CFG_N_MUTEX];

#ifdef RTP_INC_DYNAMIC_SIGNAL
RTP_SIGNAL_ALLOC rtpSignalAlloc = {_rtp_malloc,_rtp_free};
#endif

#ifdef RTP_INC_DYNAMIC_MUTEX
RTP_MUTEX_ALLOC  rtpMutexAlloc  = {_rtp_malloc,_rtp_free};
#endif

/************************************************************************
* Allocate and Configure Thread and Stack resource pools
* (See rtpconfig.h for structures and defines)
************************************************************************/
RTP_THREAD_CONTEXT rtp_thread_context[RTP_CFG_N_THREADS];

#if RTP_CFG_N_STACKS_HUGE
int rtp_stacks_huge[RTP_CFG_N_STACKS_HUGE][(sizeof(RTP_STACK_CONTEXT) + RTP_CFG_SIZESTACK_HUGE)/4];
#endif
#if RTP_CFG_N_STACKS_BIG
int rtp_stacks_big[RTP_CFG_N_STACKS_BIG][(sizeof(RTP_STACK_CONTEXT) + RTP_CFG_SIZESTACK_BIG)/4];
#endif
#if RTP_CFG_N_STACKS_NORMAL
int rtp_stacks_normal[RTP_CFG_N_STACKS_NORMAL][(sizeof(RTP_STACK_CONTEXT) + RTP_CFG_SIZESTACK_NORMAL)/4];
#endif
#if RTP_CFG_N_STACKS_SMALL
int rtp_stacks_small[RTP_CFG_N_STACKS_SMALL][(sizeof(RTP_STACK_CONTEXT) + RTP_CFG_SIZESTACK_SMALL)/4];
#endif
#if RTP_CFG_N_STACKS_TINY
int rtp_stacks_tiny[RTP_CFG_N_STACKS_TINY][(sizeof(RTP_STACK_CONTEXT) + RTP_CFG_SIZESTACK_TINY)/4];
#endif

/************************************************************************
* Global task and signal handles
************************************************************************/
RTP_SEMAPHORE TestSignal;
RTP_SEMAPHORE TestSignald;

RTP_MUTEX TestMutex;
RTP_MUTEX TestMutexd;

RTP_HANDLE TCtx0, TCtx1, TCtx2, TCtx3;               /* Task-contexts */
void *Udata0 = (void *)0xDEADBEEF;
void *Udata1 = (void *)0x12345678;
void *Udata2 = (void *)0xDEADDEAD;
void *Udata3 = (void *)0xBEEFBEEF;

/************************************************************************
* Function Prototypes
************************************************************************/
void Task0( void *userData);
void Task1( void *userData);
void Task2( void *userData);
void Task3( void *userData);

/************************************************************************
* 
************************************************************************/
void Task0( void *userData) {

  rtp_term_puts("Task0 start\n");
  if (rtp_sig_semaphore_alloc(&TestSignal, "LED OWNER") < 0)
  {
        rtp_term_puts("Sig Error!\n");
        while (1) {};
  }

#ifdef RTP_INC_DYNAMIC_SIGNAL
  if (rtp_sig_semaphore_alloc_dynamic (&TestSignald, "LED OWNER2", &rtpSignalAlloc
         ) < 0)
  {
        rtp_term_puts("Sig Error!\n");
        while (1) {};
  }
#endif

  if (rtp_sig_mutex_alloc(&TestMutex, "LED OWNER") < 0)
  {
        rtp_term_puts("Mutex Error!\n");
        while (1) {};
  }
  

#ifdef RTP_INC_DYNAMIC_MUTEX
  if (rtp_sig_mutex_alloc_dynamic (&TestMutexd, "LED OWNER2", &rtpMutexAlloc
         ) < 0)
  {
        rtp_term_puts("Mutex Error!\n");
        while (1) {};
  }
#endif

  while (1) {
    rtp_term_puts("Task0->Task2\n");
        if(rtp_thread_spawn (&TCtx2,
	               (RTP_ENTRY_POINT_FN)Task2,
                       "RTP 2 Task",
                       CFG_STACK_SIZE_SMALL,
                       10,
                       Udata2 ) < 0)
        {
            rtp_term_puts("Thread spawn 2 Error!\n");
            while (1) {};
        }
    rtp_term_puts("Task0 Waits\n");
    rtp_sig_semaphore_wait (TestSignal);
    rtp_sig_semaphore_signal (TestSignal);
    rtp_term_puts("Task0 Delay\n");
    OS_Delay (10000);
  }
}

void Task1( void *userData) {
  rtp_term_puts("Task1 start\n");

  while (1) {
    rtp_term_puts("Task1->Task3\n");
        if(rtp_thread_spawn (&TCtx3,
	               (RTP_ENTRY_POINT_FN)Task3,
                       "RTP 3 Task",
                       CFG_STACK_SIZE_SMALL,
                       10,
                       Udata3 ) < 0)
        {
          rtp_term_puts("Thread spawn 3 Error!\n");
          while (1) {};
        }
    rtp_sig_semaphore_signal (TestSignal);
    rtp_term_puts("Task 1 Waits\n");
    rtp_sig_semaphore_wait (TestSignal);
    rtp_term_puts("Task1 Delay\n");
    OS_Delay (20000);
  }
}
void Task2( void *userData) {
RTP_HANDLE currentThread;
  rtp_term_puts("Task 2 start\n");
  if(rtp_thread_handle (&currentThread) != 0)
  {
        rtp_term_puts("Handle Error!\n");
        while (1) {};
  }
  rtp_thread_yield(); 
  OS_Delay (1000);
}
void Task3( void *userData) {
  rtp_term_puts("Task 3 start\n");
  OS_Delay (2000);
}

/**********************************************************
*
*       main
*
**********************************************************/

int main(int argc, char* argv[])
{

  rtp_kern_init();

  if(rtp_term_init() < 0) while (1) {};







  rtp_term_puts("Hello World!\n");

  if(rtp_sig_semaphore_init (RTP_CFG_N_SIGNAL,
                             (RTP_SIGNAL_CONTEXTP)rtpSignalContext) < 0)
  {
        rtp_term_puts("Signal Init Error!\n");
        while (1) {};
  }

  if(rtp_sig_mutex_init (RTP_CFG_N_MUTEX,
                         (RTP_MUTEX_CONTEXTP)rtpMutexContext) < 0)
  {
        rtp_term_puts("Mutex Init Error!\n");
        while (1) {};
  }
                                   
  if (rtp_threads_init() < 0)
  {
        rtp_term_puts("Threads Init Error!\n");
        while (1) {};
  }
  rtp_threads_pool_init (RTP_CFG_N_THREADS, (RTP_THREADP)rtp_thread_context);
  rtp_threads_stacks_pool_init (RTP_CFG_N_STACKS_TINY, CFG_STACK_SIZE_TINY,
                                     RTP_CFG_SIZESTACK_TINY,
                                     (RTP_STACKP)rtp_stacks_tiny);

  rtp_threads_stacks_pool_init (RTP_CFG_N_STACKS_SMALL, CFG_STACK_SIZE_SMALL,
                                     RTP_CFG_SIZESTACK_SMALL,
                                     (RTP_STACKP)rtp_stacks_small);


  /* You need to create at least one task here !         */
  if(rtp_thread_spawn (&TCtx0,
	               (RTP_ENTRY_POINT_FN)Task0,
                       "RTP HP Task",
                       CFG_STACK_SIZE_TINY,
                       20,
                       Udata0 ) < 0)
  {
        rtp_term_puts("Thread spawn 0 Error!\n");
        while (1) {};
  }
  if(rtp_thread_spawn (&TCtx1,
	               Task1,
                       "RTP LP Task",
                       CFG_STACK_SIZE_TINY,
                       10,
                       Udata1 ) < 0)
  {
        rtp_term_puts("Thread spawn 1 Error!\n");
        while (1) {};
  }
//  if(rtp_thread_spawn (&TCtx2,
//	               (RTP_ENTRY_POINT_FN)Task2,
//                       "RTP 2 Task",
//                       CFG_STACK_SIZE_SMALL,
//                       10,
//                       Udata2 ) < 0)
//  {
//        rtp_term_puts("Thread spawn 2 Error!\n");
//        while (1) {};
//  }
//  if(rtp_thread_spawn (&TCtx3,
//	               (RTP_ENTRY_POINT_FN)Task3,
//                       "RTP 3 Task",
//                       CFG_STACK_SIZE_SMALL,
//                       10,
//                       Udata3 ) < 0)
//  {
//        rtp_term_puts("Thread spawn 3 Error!\n");
//        while (1) {};
//  }

  rtp_kern_run();           /* Start multitasking            */
  return 0;
}
