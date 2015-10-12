 /*
 | RTPSIGNL.C - Runtime Platform Signal Services
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: shane $
 |  $Date: 2004/10/14 18:33:37 $
 |  $Name:  $
 |  $Revision: 1.4 $
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
#include "rtpsignl.h"

/************************************************************************
* Defines
************************************************************************/

/************************************************************************
* Types
************************************************************************/

/************************************************************************
* Data
************************************************************************/

/************************************************************************
* Macros
************************************************************************/

/************************************************************************
* Function Prototypes
************************************************************************/

/************************************************************************
* Function Bodies
************************************************************************/

/************************************************************************
 * Semaphore [signalling] services                                      *
 ************************************************************************/

/*----------------------------------------------------------------------*
                         rtp_sig_semaphore_alloc
 *----------------------------------------------------------------------*/
/** @memo   Create and itialize a semaphore.

    @doc    Create and initalize a semaphore in the non-signalled
    state.  For example a counting semaphore with an initial
    count of zero.

    @return 0 if successful, -1 otherwise.  If a -1 is returned the 
    value of *newSem is undefined.  For debugging purposes; 
    if the cause of the error is obtainable at the native Kernel 
    layer, turn on RTP_DEBUG in rtpsignl.c to display the 
    native error value.
 */
int rtp_sig_semaphore_alloc (
  RTP_HANDLE *newSem,                   /** Storage location for the handle of the newly created semaphore.  */
  const char *name                      /** The name of the semaphore. */
  )
{
    rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                         rtp_sig_semaphore_free
 *----------------------------------------------------------------------*/
/** @memo   Free a semaphore.

    @doc    Free a semaphore using the handle returned from a 
    successful call to rtp_sig_semaphore_alloc.

    @return void
 */
void rtp_sig_semaphore_free (
  RTP_HANDLE semHandle                  /** Handle to the semaphore to be freed. */
  )
{
    rtp_not_yet_implemented ();
}


/*----------------------------------------------------------------------*
                       rtp_sig_semaphore_wait_timed
 *----------------------------------------------------------------------*/
/** @memo   Wait for a semaphore to be signaled.

    @doc    Wait for a semaphore to be signaled.  If the 
    semaphore has already been signaled this function returns
    immediatley, otherwise it should block until the semaphore
    is signaled or the millisecond time value has expired. 
    Use 0 to poll the signal and return with its state 
    immediately.  Use -1 to indicate that an infinite timeout
    value should be used, blocking indefinitely for the 
    signal.

    @return 0 if successful, -1 on error, and 1 if the call timed
    out.  For debugging purposes; if the cause of the error is 
    obtainable at the native Kernel layer, turn on 
    RTP_DEBUG in rtpsignl.c to display the native error 
    value.
 */
int rtp_sig_semaphore_wait_timed (
  RTP_HANDLE semHandle,                 /** Handle to the semaphore to be checked. */
  long msecs                            /** Timeout value in milliseconds:<br>
	<pre>
|		 0   To poll.
|		-1   To indicate use of infinite.
	</pre> */
  )
{
    rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                          rtp_sig_semaphore_clear
 *----------------------------------------------------------------------*/
/** @memo   Clear the state of a semaphore.

    @doc    Clear the state of a semaphore.  An example would
    be to set a counting semaphore to zero.
    
    @precondition Do not call this function unless past events
    have already been handled or their states are not needed. 
    This will clear the signal and prepare to notify when the 
    next event occurs.

    @return void
 */
void rtp_sig_semaphore_clear (
  RTP_HANDLE semHandle                  /** Handle of the semaphore to clear. */
  )
{
    rtp_not_yet_implemented ();
}


/*----------------------------------------------------------------------*
                          rtp_sig_semaphore_wait
 *----------------------------------------------------------------------*/
/** @memo   Wait for a semaphore to be signaled.

    @doc    Wait for a semaphore to be signaled.  If the 
    semaphore has already been signaled this function returns
    immediatley, otherwise it should block indefinitely until 
    the semaphore is signaled.  This calls functionality is 
    identical to calling rtp_sig_semaphore_wait_timed with a 
    timeout value of -1 (INFINITE).

    @return 0 if successful, -1 otherwise.  For debugging 
    purposes; if the cause of the error is obtainable at the 
    native Kernel layer, turn on RTP_DEBUG in 
    rtpsignl.c to display the native error value.
 */
int rtp_sig_semaphore_wait (
  RTP_HANDLE semHandle                  /** Handle to the semaphore to be checked. */
  )
{
    rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                         rtp_sig_semaphore_signal
 *----------------------------------------------------------------------*/
/** @memo   Signal a semaphore.

    @doc    Signal a semaphore.  An example would be to 
    increment a counting semaphore.

    @return void
 */
void rtp_sig_semaphore_signal (
  RTP_HANDLE semHandle                  /** Handle to the semaphore to be signaled. */
  )
{
    rtp_not_yet_implemented ();
}


/*----------------------------------------------------------------------*
                       rtp_sig_semaphore_signal_isr
 *----------------------------------------------------------------------*/
/** @memo   Signal an interrupt service routine semaphore.

    @doc    Signal an interrupt service routine semaphore.  In 
    most implementations this will be identical to 
    rtp_sig_semaphore_signal.

    @return void
 */
void rtp_sig_semaphore_signal_isr (
  RTP_HANDLE semHandle                  /** Handle to the semaphore to be signaled. */
  )
{
    rtp_not_yet_implemented ();
}


/************************************************************************
 * Mutex [lock] services                                                *
 ************************************************************************/

/*----------------------------------------------------------------------*
                          rtp_sig_mutex_alloc
 *----------------------------------------------------------------------*/
/** @memo   Create and itialize a mutex.

    @doc    Create and initalize a mutex in the non-claimed
    state.

    @return 0 if successful, -1 otherwise.  If a -1 is returned the 
    value of *newMutex is undefined.  For debugging purposes; 
    if the cause of the error is obtainable at the native Kernel 
    layer, turn on RTP_DEBUG in rtpsignl.c to display the 
    native error value.
 */
int rtp_sig_mutex_alloc (
  RTP_HANDLE *newMutex,                 /** Storage location for the handle of the newly created mutex. */
  const char *name                      /** The name of the mutex. */
  )
{
    NU_SEMAPHORE nucleusMutex;

	if (NU_Create_Semaphore(&nucleusMutex, name, 1, NU_FIFO) != NU_SUCCESS)
    {
        return(-1);
    }
    
    *newMutex = (RTP_HANDLE)nucleusMutex;
    return(0);
}


/*----------------------------------------------------------------------*
                          rtp_sig_mutex_free
 *----------------------------------------------------------------------*/
/** @memo   Free a mutex.

    @doc    Free a mutex using the handle returned from a 
    successful call to rtp_sig_mutex_alloc.

    @return void
 */
void rtp_sig_mutex_free (
  RTP_HANDLE mutexHandle                /** [tbd] */
  )
{
    rtp_not_yet_implemented ();
}


/*----------------------------------------------------------------------*
                       rtp_sig_mutex_claim_timed
 *----------------------------------------------------------------------*/
/** @memo   Wait for a mutex to be available.

    @doc    Wait for a mutex to be available.  If the mutex is 
    already available this function returns immediatley, 
    otherwise it should block until the mutex is available or 
    the millisecond time value has expired. Use 0 to poll the 
    mutex and return with its state immediately.  Use -1 to 
    indicate that an infinite timeout value should be used, 
    blocking indefinitely for the mutex to become available.

    @postconditon For every time this function is successful 
    on a designated mutex handle, a respective call to 
    rtp_sig_mutex_release must be made.

    @return 0 if successful, -1 on error, and 1 if the call timed
    out.  For debugging purposes; if the cause of the error is 
    obtainable at the native Kernel layer, turn on 
    RTP_DEBUG in rtpsignl.c to display the native error 
    value.
 */
int rtp_sig_mutex_claim_timed (
  RTP_HANDLE mutexHandle,               /** Handle to the mutex to be checked. */
  long msecs                            /** Timeout value in milliseconds:<br>
	<pre>
|		 0   To poll.
|		-1   To indicate use of infinite.
	</pre> */
  )
{
    rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                          rtp_sig_mutex_claim
 *----------------------------------------------------------------------*/
/** @memo   Wait for a mutex to be available.

    @doc    Wait for a mutex to be available.  If the mutex is 
    already available this function returns immediatley, 
    otherwise it should block indefinitely or until the mutex 
    is available.

    @postconditon For every time this function is successful 
    on a designated mutex handle, a respective call to 
    rtp_sig_mutex_release must be made.

    @return 0 if successful, -1 otherwise.  For debugging 
    purposes; if the cause of the error is obtainable at the 
    native Kernel layer, turn on RTP_DEBUG in rtpsignl.c 
    to display the native error value.
 */
int rtp_sig_mutex_claim (
  RTP_HANDLE mutexHandle                /** Handle to the mutex to be claimed. */
  )
{
    NU_Obtain_Semaphore((NU_SEMAPHORE*)&mutexHandle, NU_SUSPEND);
    /* No error return available, so we simply return success. */
    
    return (0);
}


/*----------------------------------------------------------------------*
                          rtp_sig_mutex_release
 *----------------------------------------------------------------------*/
/** @memo   Release a mutex.

    @doc    Release a mutex

    @precondition A successful call to rtp_sig_mutex_claim 
    should be made before calling this function.

    @return void
 */
void rtp_sig_mutex_release (
  RTP_HANDLE mutexHandle                /** Handle to the mutex to be released. */
  )
{
    NU_Release_Semaphore((NU_SEMAPHORE*)&mutexHandle);
}


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */