 /*
 | RTPTHRD.C - Runtime Platform Thread Services
 |
 | EBSnet - EBS_RT-Platform 
 |
 |  $Author: yohannes $
 |  $Date: 2005//06/14 16:06:52 $
 |  $Name:  $
 |  $Revision: 1.1 $
 |
 | Copyright EBSnet Inc. , 2003
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
 |
 | Module description:
 |  [tbd]
*/



/************************************************************************
* Headers
************************************************************************/
#include "rtp.h"
#include "rtpthrd.h"
#include "rtk32.h"
/************************************************************************
* Defines
************************************************************************/

#define		RTP_SIZESTACK_HUGE				 32*1024
#define		RTP_SIZESTACK_BIG				 16*1024
#define		RTP_SIZESTACK_NORMAL			 8*1024
#define		RTP_SIZESTACK_SMALL				 4*1024

const unsigned int stackSizeMap [5] = {
                RTP_SIZESTACK_SMALL,
				RTP_SIZESTACK_NORMAL,
				RTP_SIZESTACK_NORMAL,
				RTP_SIZESTACK_BIG,
				RTP_SIZESTACK_HUGE
                             };
                             
const unsigned char threadPriorityMap [32] = { 
                                 2,    /* THREAD_PRIORITY_LOWEST */
                                 5,
                                 6,
                                 7,
                                 10,
                                 12,
                                 14,   /* THREAD_PRIORITY_BELOW_NORMAL */
                                 16,
                                 18,
                                 20,
                                 22,
                                 24,
                                 26,   /* THREAD_PRIORITY_NORMAL */
                                 28,
                                 30,
                                 32,
                                 34,
                                 36,
                                 38,
                                 40,
                                 42,   /* THREAD_PRIORITY_ABOVE_NORMAL */
                                 44,
                                 46,
                                 48,
                                 50,
                                 52,
                                 54,   /* THREAD_PRIORITY_HIGHEST */
                                 56,
                                 58,
                                 60,
                                 62,
                                 63,
                               };                             
                             
static int rtp_threads_up = 0;

/************************************************************************
* Macros
************************************************************************/

/************************************************************************
 * Thread services                                                      *
 ************************************************************************/
 
/************************************************************************
* Function Bodies
************************************************************************/

/*----------------------------------------------------------------------*
                            rtp_threads_init
 *----------------------------------------------------------------------*/
/** @memo   Prepare for use of the RTPlatform thread API.

    @doc    Prepare for use of the RTPlatform thread API.

    @return 0 on success, -1 otherwise.
 */
int rtp_threads_init (void)
{
	
}


/*----------------------------------------------------------------------*
                           rtp_threads_shutdown
 *----------------------------------------------------------------------*/
/** @memo   Shutdown the RTPlatform thread API.

    @doc    When use of the RTPlatform thread API is no longer 
    needed, this function clears and/or releases any allocated 
    resources setup by the rtp_threads_init function call.

    @return void
 */
void rtp_threads_shutdown (void)
{
	if (rtp_threads_up > 0)
	{
		rtp_threads_up--;
	}
}


/*----------------------------------------------------------------------*
                            rtp_thread_spawn
 *----------------------------------------------------------------------*/
/** @memo   Spawn a thread.

    @doc    Spawn a thread and sets its priority level.

    @return 0 on success, -1 on failure, and -2 on non-fatal
    error.  If a -1 is returned the value of *newThread is 
    undefined.  An example of a non-fatal error would be if 
    the thread was spawned but the priority level could not 
    be set.
 */
int rtp_thread_spawn (
  RTP_HANDLE  * newThread,              /** Storage location for the handle to the new thread. */
  RTP_ENTRY_POINT_FN entryPoint,        /** A function pointer to the threads entry point. */
  const char  * name,                   /** The name of the thread. */
  int           stackSizeIndex,         /** An index into the array of possible stack sizes. */
  int           priorityIndex,          /** An index into the array of possible priorities. */
  void        * userData                /** User data to be passed to the entry point function. */
  )
{
   
int 			stackSize;
int             priority;
int 			counter;
unsigned int	stkTop;
RTP_HANDLE		old_level;

	old_level = rtp_irq_push_disable();
    if (!rtp_threads_up)				/* initialize threads */
    {
      rtp_threads_init();
      rtp_threads_up++;
    }
	rtp_irq_pop(old_level);
 										/* extract the exact stack size from the table */
	
	stackSize = stackSizeMap[stackSizeIndex];
    priority  = threadPriorityMap[priorityIndex];
    
	//rtp_irq_disable();
    
    *newThread = (long)RTKCreateThread((RTKThreadFunction)entryPoint,
                                  priority,
                                  stackSize,
                                  TF_NO_MATH_CONTEXT,
                                  userData,
                                  (char *)name);
    
	//rtp_irq_enable();
    
    if (*newThread)
    {
   //     RTKSetUserData((struct rtkTCBType *)*newThread,0,userData);
        return(0);
    }

    return(-1);
}


/*----------------------------------------------------------------------*
                            rtp_thread_handle
 *----------------------------------------------------------------------*/
/** @memo   Get the current thread handle.

    @doc    Retrieves the handle to the current thread.

    @return 0 on success, -1 otherwise.
 */
int rtp_thread_handle (
  RTP_HANDLE * currentThread            /** Storage location for the thread handle. */
  )
{

	currentThread = (RTP_HANDLE *) RTKCurrentTaskHandle();
	
	if(currentThread)
	{
		return(0);
	}	

	return(-1);
}


/*----------------------------------------------------------------------*
                          rtp_thread_user_data
 *----------------------------------------------------------------------*/
/** @memo   Gets the user data associated with the current thread.

    @doc    Retrieves the user data from the current thread.

    @return 0 on success, -1 otherwise.
 */
int rtp_thread_user_data (
  void ** userData                      /** Storage location for the user data. */
  )
{
	RTP_HANDLE currentTask;
	
	currentTask = (RTP_HANDLE ) RTKCurrentTaskHandle();
	*userData =  ((RTKTaskHandle) currentTask)->Parameter;
	
	if (*userData)
	{
	    return(0);
    }
	
	return (-1);
}


/*----------------------------------------------------------------------*
                       rtp_thread_user_data_by_handle
 *----------------------------------------------------------------------*/
/** @memo   Gets the user data associated with the thread handle.

    @doc    Retrieves the user data from the thread with the 
    associated thread handle.

    @return 0 on success, -1 otherwise.
 */
int rtp_thread_user_data_by_handle (
  RTP_HANDLE handle,                    /** Handle to the thread to find its user data. */
  void ** userData                      /** Storage location for the user data. */
  )
{
    *userData = (void *)RTKGetUserData((struct rtkTCBType *)handle,0);
    
    if (*userData)
    {
        return(0);
    }
    return (-1);
}


/*----------------------------------------------------------------------*
                              rtp_thread_name
 *----------------------------------------------------------------------*/
/** @memo   Gets the name associated with the current thread.

    @doc    Retrieves the name from the current thread.

    @return 0 on success, -1 otherwise.
 */
int rtp_thread_name (
  char ** name                          /** Storage location for the name. */
  )
{

	rtp_strcpy ((char *)*name,(const char *) RTKGetTaskName(RTKCurrentTaskHandle()));
	
	if (*name)
	{
		return(0);
	}
	
	return(-1);
}


/*----------------------------------------------------------------------*
                          rtp_thread_name_by_handle
 *----------------------------------------------------------------------*/
/** @memo   Gets the name associated with the thread handle.

    @doc    Retrieves the name from the thread with the 
    associated thread handle.

    @return 0 on success, -1 otherwise.
 */
int rtp_thread_name_by_handle (
  RTP_HANDLE handle,                    /** Handle to the thread to find its name. */
  char ** name                          /** Storage location for the name. */
  )
{
    if (handle)
	{
		rtp_strcpy (*name, (const char *)RTKGetTaskName((struct rtkTCBType *)handle));
		return(0);
	}
	
	return(-1);
}


/*----------------------------------------------------------------------*
                             rtp_thread_sleep
 *----------------------------------------------------------------------*/
/** @memo   Put the current thread to sleep.

    @doc    Suspend the current thread for the millisecond 
    timeout period.

    @return void
 */
void rtp_thread_sleep (
  long msecs                            /** Timeout value in milliseconds. */
  )
{

    long noTicks;

    if (!msecs)
    {
    	return;
    }
    
	noTicks = (msecs * rtp_get_ticks_p_sec())/1000;    

	if (noTicks < 1)
	{
	  noTicks = 1;
	}
	
    RTKDelay( noTicks);

}


/*----------------------------------------------------------------------*
                             rtp_thread_yield
 *----------------------------------------------------------------------*/
/** @memo   Yield the current thread to any other of equal priority.

    @doc    Check if any other threads are attempting to run that 
    are of equal priority.  If there is one of equal proritey ready
    to run, let it, otherwise the yield returns and the current 
    thread continues.

    @return void
 */
void rtp_thread_yield (void)
{
    
}

void os_exit_task(void)   /*__fn__*/
{
}
/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
