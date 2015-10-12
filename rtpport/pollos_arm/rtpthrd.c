//
// RTPTHRD.C - Runtime Platform Thread Services
//
//   PORTED TO THE POLLOS system
//
// EBSnet - RT-Platform
//
// Copyright EBSnet Inc. , 2003
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//  [tbd]
//

/************************************************************************
* Headers
************************************************************************/
#include "rtpport.h"
#include "rtpdebug.h"
#include "rtip.h"
#include "pollos.h"
#include "rtipext.h"
#include "rtpprint.h"
#include "rtpmem.h"

/************************************************************************
* Defines
************************************************************************/

/************************************************************************
* Type definitions
*************************************************************************/

/************************************************************************
* Data
************************************************************************/
CONTEXT_DATA RTP_FAR globalContext;
RTP_HANDLE thread_handle = 0;

#if (MS_FRAMEWORK)

#if (INCLUDE_RUN_TIME_CONFIG)
HAL_CONTINUATION    *IPTaskContinuation;
HAL_CONTINUATION    *InterruptTaskContinuation;
#else
HAL_CONTINUATION    IPTaskContinuation[CFG_NIFACES];
HAL_CONTINUATION    InterruptTaskContinuation[CFG_NIFACES];
#endif

HAL_CONTINUATION    TimerTaskContinuation;
HAL_CONTINUATION    DHCPTaskContinuation;
HAL_COMPLETION      TimerTaskCompletion;


#if (INCLUDE_RUN_TIME_CONFIG)
int                 *IPTaskInitialized;
int                 *InterruptTaskContInitialized;
#else
int                 IPTaskInitialized[CFG_NIFACES];
int                 InterruptTaskContInitialized[CFG_NIFACES];
#endif
int                 TimerTaskContInitialized;
int                 TimerTaskCompInitialized;
int                 DHCPTaskInitialized;



/************************************************************************
* Macros
************************************************************************/

/************************************************************************
* Utility Function Types & Prototypes
************************************************************************/

/************************************************************************
* Function Bodies
************************************************************************/

/************************************************************************
 * Thread services                                                      *
 ************************************************************************/

extern "C" {
void rtp_thrd_timer_completion(void* arg);
void rtp_thrd_timer_continuation(void* arg);
void rtp_thrd_ip_continuation(int index);
void rtp_thrd_interrupt_continuation(int index);
void tc_ip_process(int* iface_no); 
void rtp_thrd_ip_process(void* arg);
}

#endif /* MS_FRAMEWORK */


/************************************************************************
* Function Bodies
************************************************************************/

#if (MS_FRAMEWORK)  /* exclude rest of file */

/*----------------------------------------------------------------------*
                            rtp_thrd_timer_continuation
 *----------------------------------------------------------------------*/
void rtp_thrd_timer_continuation(void* arg)
{
    
    /* Step the time in the stack and handle timeouts if any */
    tc_timer_main();
    
}

/*----------------------------------------------------------------------*
                            rtp_thrd_timer_completion
 *----------------------------------------------------------------------*/
void rtp_thrd_timer_completion(void* arg)
{
    /* Schedule the timer task to run as a continuation */
    if(!TimerTaskContinuation.IsLinked())
    {
        TimerTaskContinuation.Enqueue();
    }
    /////////////////////////////////////////////////////////////////
    // DO NT CHANGE!
    // KEEP the timeout IN SYNC with _CFG_TIMER_FREQ
    TimerTaskCompletion.EnqueueDelta(20000);    
    // DO NT CHANGE!
    /////////////////////////////////////////////////////////////////
}


/*----------------------------------------------------------------------*
                            rtp_thrd_ip_continuation
 *----------------------------------------------------------------------*/
void rtp_thrd_ip_continuation(int index)
{
    GLOBAL_LOCK(irq);
    
    if(!IPTaskContinuation[index].IsLinked())
    {
        IPTaskContinuation[index].Enqueue();
    }
}

/*----------------------------------------------------------------------*
                            rtp_thrd_interrupt_continuation
 *----------------------------------------------------------------------*/
void rtp_thrd_interrupt_continuation(int index)
{
    GLOBAL_LOCK(irq);
    
    if(!InterruptTaskContinuation[index].IsLinked())
    {
        InterruptTaskContinuation[index].Enqueue();
    }
}

/*----------------------------------------------------------------------*
                            rtp_thrd_ip_process
 *----------------------------------------------------------------------*/
void rtp_thrd_ip_process(void* arg)
{
    
    tc_ip_process((int*)arg);
    SOCKETS_ProcessSocketActivity(SOCK_SOCKET_ERROR);
    
}


/*----------------------------------------------------------------------*
                            rtp_threads_init
 *----------------------------------------------------------------------*/
int rtp_threads_init (void)
{   
    int i;
    
#if (INCLUDE_RUN_TIME_CONFIG)
    IPTaskContinuation = (HAL_CONTINUATION  *) rtp_malloc(sizeof(HAL_CONTINUATION) * CFG_NIFACES);           
    InterruptTaskContinuation = (HAL_CONTINUATION  *) rtp_malloc(sizeof(HAL_CONTINUATION) * CFG_NIFACES);
    IPTaskInitialized = (int *) rtp_malloc(sizeof(int) * CFG_NIFACES);  
    InterruptTaskContInitialized = (int *) rtp_malloc(sizeof(int) * CFG_NIFACES);  

    if ( ((int)IPTaskContinuation== -1) || ((int)InterruptTaskContinuation== -1) || 
         ((int)IPTaskInitialized== -1) || ((int)InterruptTaskContInitialized== -1) )
    {
        return (-1);
    }
#endif

    for (i = 0; i < CFG_NIFACES; i++)
    {
        IPTaskInitialized[i] = 0;
        InterruptTaskContInitialized[i] = 0;
    }
    TimerTaskContInitialized = 0;        
    TimerTaskCompInitialized = 0;                 
    DHCPTaskInitialized = 0;                     
    
    return (0);
}   
    
/*----------------------------------------------------------------------*
                           rtp_threads_shutdown
 *----------------------------------------------------------------------*/
void rtp_threads_shutdown (void)
{
    int i;
    
    for (i = 0; i < CFG_NIFACES; i++)
    {
        IPTaskInitialized[i] = 0;
        InterruptTaskContInitialized[i] = 0;
    }
    TimerTaskContInitialized = 0;        
    TimerTaskCompInitialized = 0;                 
    DHCPTaskInitialized = 0;                     
    
#if (INCLUDE_RUN_TIME_CONFIG)
     rtp_free(IPTaskContinuation);           
     rtp_free(InterruptTaskContinuation);
     rtp_free(IPTaskInitialized);  
     rtp_free(InterruptTaskContInitialized);  
#endif
}

/*----------------------------------------------------------------------*
                            rtp_thread_spawn
 *----------------------------------------------------------------------*/
int rtp_thread_spawn (RTP_HANDLE         * newThread,
                      RTP_ENTRY_POINT_FN   entryPoint,
                      const char         * name,
                      int                  stackSizeIndex,
                      int                  priorityIndex,
                      void               * userData)
{
    PIFACE pi;
    int * index = (int *) userData;
    
    if ((rtp_strcmp(name, "ip interpret") == 0) && !IPTaskInitialized[*index])
    {
        IPTaskInitialized[*index] = 1;

        IPTaskContinuation[*index].InitializeCallback((HAL_CALLBACK_FPN) entryPoint, userData);
    }
    
    else if ((rtp_strcmp(name, "timer comp") == 0) && !TimerTaskCompInitialized)
    {
        TimerTaskCompInitialized = 1;
        
        TimerTaskCompletion.InitializeForUserMode((HAL_CALLBACK_FPN) entryPoint, userData);
            
        TimerTaskCompletion.EnqueueDelta(20000);
    }       
    
    else if ((rtp_strcmp(name, "timer cont") == 0) && !TimerTaskContInitialized)
    {      
        TimerTaskContInitialized = 1;

        TimerTaskContinuation.InitializeCallback((HAL_CALLBACK_FPN) entryPoint,userData);    
    } 
    
    else if ((rtp_strcmp(name, "interrupt task") == 0) && !InterruptTaskContInitialized[*index])
    {
        pi = (PIFACE) &ifaces[*index];
        if (!pi)
        {
            return -1;
        }

        InterruptTaskContInitialized[*index] = 1;
        
        InterruptTaskContinuation[*index].InitializeCallback((HAL_CALLBACK_FPN) entryPoint,pi);
    }
    
    else if ((rtp_strcmp(name, "dhcp") == 0) && !DHCPTaskInitialized)
    {
        DHCPTaskInitialized = 1;
        
        DHCPTaskContinuation.InitializeCallback((HAL_CALLBACK_FPN) entryPoint,userData);
    }
    
    return (0);    
}

/*----------------------------------------------------------------------*
                            rtp_thread_kill
 *----------------------------------------------------------------------*/
int rtp_thread_kill (RTP_HANDLE         * newThread,
                      RTP_ENTRY_POINT_FN   entryPoint,
                      const char         * name,
                      int                  stackSizeIndex,
                      int                  priorityIndex,
                      void               * userData)
{
    int * index = (int *) userData;
    
    if ((rtp_strcmp(name, "ip interpret") == 0) && IPTaskInitialized[*index])
    {
        IPTaskInitialized[*index] = 0;

        IPTaskContinuation[*index].Abort();
    }
    
    else if ((rtp_strcmp(name, "timer comp") == 0) && TimerTaskCompInitialized)
    {
        TimerTaskCompInitialized = 0;
        
        TimerTaskCompletion.Abort();
    }       
    
    else if ((rtp_strcmp(name, "timer cont") == 0) && TimerTaskContInitialized)
    {      
        TimerTaskContInitialized = 0;

        TimerTaskContinuation.Abort();
    } 
    
    else if ((rtp_strcmp(name, "interrupt task") == 0) && InterruptTaskContInitialized[*index])
    {
        InterruptTaskContInitialized[*index] = 0;
        
        InterruptTaskContinuation[*index].Abort();
    }
    
    else if ((rtp_strcmp(name, "dhcp") == 0) && DHCPTaskInitialized)
    {
        DHCPTaskInitialized = 0;
        
        DHCPTaskContinuation.Abort();
    }
    
    return (0);    
}

#else
/*----------------------------------------------------------------------*
                            rtp_thread_spawn
 *----------------------------------------------------------------------*/
int rtp_thread_spawn (RTP_HANDLE         * newThread,
                      RTP_ENTRY_POINT_FN   entryPoint,
                      const char         * name,
                      int                  stackSizeIndex,
                      int                  priorityIndex,
                      void               * userData)
{
    * newThread = thread_handle;

    if (entryPoint)
    {
	entryPoint((unsigned long)userData);
	return(0);
    }
    return (-1);


}
#endif /* MS_FRAMEWORK */

/*----------------------------------------------------------------------*
                            rtp_thread_handle
 *----------------------------------------------------------------------*/
int rtp_thread_handle (RTP_HANDLE * currentThread)
{
    *currentThread = thread_handle;
    return(0);
}


/*----------------------------------------------------------------------*
                          rtp_thread_user_data
 *----------------------------------------------------------------------*/
int rtp_thread_user_data (void ** userData)
{
    *userData = &globalContext;
    return(0);
}


/*----------------------------------------------------------------------*
                       rtp_thread_user_data_by_handle
 *----------------------------------------------------------------------*/
int rtp_thread_user_data_by_handle (RTP_HANDLE handle, void ** userData)
{
    *userData = &globalContext;
    return(0);
}


/*----------------------------------------------------------------------*
                              rtp_thread_name
 *----------------------------------------------------------------------*/
int rtp_thread_name (char ** name)
{
    *name = "POLLOS THREAD";
    return(0);
}


/*----------------------------------------------------------------------*
                          rtp_thread_name_by_handle
 *----------------------------------------------------------------------*/
int rtp_thread_name_by_handle (RTP_HANDLE handle, char ** name)
{
    *name = "POLLOS THREAD";
    return(0);
}


/*----------------------------------------------------------------------*
                             rtp_thread_sleep
 *----------------------------------------------------------------------*/
void rtp_thread_sleep (long msecs)
{
    /* pollos_sleep takes msecs and converts to ticks. */
    pollos_sleep(msecs);
}


/*----------------------------------------------------------------------*
                             rtp_thread_yield
 *----------------------------------------------------------------------*/
void rtp_thread_yield (void)
{
    
    
#if (INCLUDE_POLLOS_COOPERATIVE)
    // callback to application so it can yield
    // there are no events pending, so yield to the operating
    // system; the operating system should return when a send
    // complete or receive interrupt occurs or when a time
    // period has elapsed with no interrupts occurring;
    // the time period can be as much as one second, but
    // ideally 100 msec of inactivity should cause the
    // stack to run again
    rtp_thread_yield_cooperative();
#endif
}

#if (INCLUDE_POLLOS_COOPERATIVE)
/*----------------------------------------------------------------------*
                            rtp_threads_init_cooperative
 *----------------------------------------------------------------------*/
/** @memo   Prepare for use of the RTPlatform thread API.

    @doc    Prepare for use of the RTPlatform thread API.

    @return 0 on success, -1 otherwise.
 */
int rtp_threads_init_cooperative(void)
{
    
}


/*----------------------------------------------------------------------*
                           rtp_threads_shutdown_cooperative
 *----------------------------------------------------------------------*/
/** @memo   Shutdown the RTPlatform thread API.

    @doc    When use of the RTPlatform thread API is no longer 
    needed, this function clears and/or releases any allocated 
    resources setup by the rtp_threads_init function call.

    @return void
 */
void rtp_threads_shutdown_cooperative (void)
{
    if (rtp_threads_up_cooperative > 0)
    {
        rtp_threads_up_cooperative --;
    }
}


/*----------------------------------------------------------------------*
                            rtp_thread_spawn_cooperative
 *----------------------------------------------------------------------*/
/** @memo   Spawn a thread.

    @doc    Spawn a thread and sets its priority level.

    @return 0 on success, -1 on failure, and -2 on non-fatal
    error.  If a -1 is returned the value of *newThread is 
    undefined.  An example of a non-fatal error would be if 
    the thread was spawned but the priority level could not 
    be set.
 */
int rtp_thread_spawn_cooperative (
  RTP_HANDLE  * newThread,              /** Storage location for the handle to the new thread. */
  RTP_ENTRY_POINT_FN entryPoint,        /** A function pointer to the threads entry point. */
  const char  * name,                   /** The name of the thread. */
  int           stackSizeIndex,         /** An index into the array of possible stack sizes. */
  int           priorityIndex,          /** An index into the array of possible priorities. */
  void        * userData                /** User data to be passed to the entry point function. */
  )
{
   

}


/*----------------------------------------------------------------------*
                            rtp_thread_handle_cooperative
 *----------------------------------------------------------------------*/
/** @memo   Get the current thread handle.

    @doc    Retrieves the handle to the current thread.

    @return 0 on success, -1 otherwise.
 */
int rtp_thread_handle_cooperative(
  RTP_HANDLE * currentThread            /** Storage location for the thread handle. */
  )
{


}


/*----------------------------------------------------------------------*
                          rtp_thread_user_data_cooperative
 *----------------------------------------------------------------------*/
/** @memo   Gets the user data associated with the current thread.

    @doc    Retrieves the user data from the current thread.

    @return 0 on success, -1 otherwise.
 */
int rtp_thread_user_data_cooperative (
  void ** userData                      /** Storage location for the user data. */
  )
{

}


/*----------------------------------------------------------------------*
                       rtp_thread_user_data_by_handle_cooperative
 *----------------------------------------------------------------------*/
/** @memo   Gets the user data associated with the thread handle.

    @doc    Retrieves the user data from the thread with the 
    associated thread handle.

    @return 0 on success, -1 otherwise.
 */
int rtp_thread_user_data_by_handle_cooperative(
  RTP_HANDLE handle,                    /** Handle to the thread to find its user data. */
  void ** userData                      /** Storage location for the user data. */
  )
{
 
}


/*----------------------------------------------------------------------*
                              rtp_thread_name
 *----------------------------------------------------------------------*/
/** @memo   Gets the name associated with the current thread.

    @doc    Retrieves the name from the current thread.

    @return 0 on success, -1 otherwise.
 */
int rtp_thread_name_cooperative(
  char ** name                          /** Storage location for the name. */
  )
{


}


/*----------------------------------------------------------------------*
                          rtp_thread_name_by_handle_cooperative
 *----------------------------------------------------------------------*/
/** @memo   Gets the name associated with the thread handle.

    @doc    Retrieves the name from the thread with the 
    associated thread handle.

    @return 0 on success, -1 otherwise.
 */
int rtp_thread_name_by_handle_cooperative (
  RTP_HANDLE handle,                    /** Handle to the thread to find its name. */
  char ** name                          /** Storage location for the name. */
  )
{

}


/*----------------------------------------------------------------------*
                             rtp_thread_sleep_cooperative
 *----------------------------------------------------------------------*/
/** @memo   Put the current thread to sleep.

    @doc    Suspend the current thread for the millisecond 
    timeout period.

    @return void
 */
void rtp_thread_sleep_cooperative(
  long msecs                            /** Timeout value in milliseconds. */
  )
{



}


/*----------------------------------------------------------------------*
                             rtp_thread_yield_cooperative
 *----------------------------------------------------------------------*/
/** @memo   Yield the current thread to any other of equal priority.

    @doc    Check if any other threads are attempting to run that 
    are of equal priority.  If there is one of equal proritey ready
    to run, let it, otherwise the yield returns and the current 
    thread continues.

    @return void
 */
void rtp_thread_yield_cooperative(void)
{
    rtp_thread_sleep_cooperative(1);
}
#endif


/************************************************************************
* Utility Functions
************************************************************************/

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */

