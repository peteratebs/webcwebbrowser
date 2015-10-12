 /*
 | RTPTHRD.C - Runtime Platform Thread Services
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: shane $
 |  $Date: 2004/10/05 18:51:30 $
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



/************************************************************************
* Headers
************************************************************************/
#include "rtp.h"
#include "rtpthrd.h"

#include "nucleus.h"

/************************************************************************
* Defines
************************************************************************/
/* Thread status flag values. May be by the kernel background timer task
   to provide an orderly approach to ending tasks and freeing task stacks */
#define RTP_THREAD_IS_FREE             0
#define RTP_THREAD_STARTING            1
#define RTP_THREAD_IS_RUNNING          2
#define RTP_THREAD_MUST_BE_KILLED      3
#define RTP_THREAD_STACK_MUST_BE_FREED 4

/************************************************************************
* Type definitions
*************************************************************************/
typedef struct _rtp_thread_context
{
    int        state;
    int        priority;
    RTP_HANDLE threadHandle;
    char       name[CFG_NAME_SIZE];
	void     (*entryPoint)(void *);
	void      *userData;
}
rtp_thread_context;

/************************************************************************
* Data
************************************************************************/

/************************************************************************
* Macros
************************************************************************/

/************************************************************************
* Function Prototypes
************************************************************************/
void _rtp_threadFunction(unsigned argc, void* argv)

/************************************************************************
* Function Bodies
************************************************************************/

/************************************************************************
 * Thread services                                                      *
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
    rtp_not_yet_implemented();
    return (-1);
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
    rtp_not_yet_implemented();
    return (-1);
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
    rtp_not_yet_implemented();
    return (-1);
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
    rtp_not_yet_implemented();
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
	rtp_not_yet_implemented();
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
    rtp_not_yet_implemented();
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
    rtp_not_yet_implemented();
}


void _rtp_threadFunction(unsigned argc, void* argv)
{
	struct _wrapper *w = argv;
	w->func(w->context);
	
}



/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
