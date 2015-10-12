//
// RTPTHRD.C - Runtime Platform Thread Services
//
//   PORTED TO THE POLLOS system
//
// EBSnet - RT-Platform
//
//  $Author: sarah $
//  $Date: 2005/05/14 02:43:30 $
//  $Name:  $
//  $Revision: 1.1 $
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

/*----------------------------------------------------------------------*
                            rtp_threads_init
 *----------------------------------------------------------------------*/
int rtp_threads_init (void)
{   
    return (0);
}

/*----------------------------------------------------------------------*
                           rtp_threads_shutdown
 *----------------------------------------------------------------------*/
void rtp_threads_shutdown (void)
{
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
	* newThread = thread_handle;
	if (entryPoint)
	{
		entryPoint(userData);
		return(0);
	}
    return (-1);
}

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
    /* pollos_sleep takes ticks. */
    pollos_sleep((msecs/1000) * rtp_get_ticks_p_sec());
}

/*----------------------------------------------------------------------*
                             rtp_thread_yield
 *----------------------------------------------------------------------*/
void rtp_thread_yield (void)
{
	poll_os_cycle();
	
#ifdef INCLUDE_POLLOS_COOPERATIVE
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

#ifdef INCLUDE_POLLOS_COOPERATIVE
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
   
int 			stackSize;
int             priority;
int 			counter;
unsigned int	stkTop;
RTP_HANDLE		old_level;

	RTP_DEBUG_ERROR("!!!! Thread spawned as a task", 0, 0, 0);
	old_level = rtp_irq_push_disable_cooperative();
    if (!rtp_threads_up_cooperative)				/* initialize threads */
    {
      rtp_threads_init();
      rtp_threads_up_cooperative++;
    }
	rtp_irq_pop_cooperative(old_level);
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

	currentThread = (RTP_HANDLE *) RTKCurrentTaskHandle();
	
	if(currentThread)
	{
		return(0);
	}	

	return(-1);
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
	RTP_HANDLE *currentTask;
	
	currentTask = (RTP_HANDLE *) RTKCurrentTaskHandle();
	*userData =  ((RTKTaskHandle) *currentTask)->Parameter;
	
	if (*userData)
	{
	    return(0);
    }
	
	return (-1);
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
int rtp_thread_name_cooperative(
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
    if (handle)
	{
		rtp_strcpy (*name, (const char *)RTKGetTaskName((struct rtkTCBType *)handle));
		return(0);
	}
	
	return(-1);
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

