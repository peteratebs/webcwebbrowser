 /*
 | RTPSIGNL.C - Runtime Platform Signal Services
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: shane $
 |  $Date: 2004/10/21 19:40:47 $
 |  $Name:  $
 |  $Revision: 1.7 $
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
                         _rtp_sig_semaphore_alloc
 *----------------------------------------------------------------------*/
/** @memo   Create and itialize a semaphore.

    @doc    Create and initalize a semaphore in the non-signalled
    state.  For example a counting semaphore with an initial
    count of zero.

	@precondition Must not call directly. Use the 
	<b>rtp_sig_semaphore_alloc</b> macro in rtpsignl.h.

    @return 0 if successful, -1 otherwise.  If a -1 is returned the 
    value of *newSem is undefined.  For debugging purposes; 
    if the cause of the error is obtainable at the native Kernel 
    layer, turn on RTP_DEBUG in rtpdebug.h to display the 
    native error value.
 */
int _rtp_sig_semaphore_alloc (
  RTP_HANDLE *newSem,                   /** Storage location for the handle of the newly created semaphore.  */
  const char *name                      /** The name of the semaphore. */
  )
{
    rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                         _rtp_sig_semaphore_free
 *----------------------------------------------------------------------*/
/** @memo   Free a semaphore.

    @doc    Free a semaphore using the handle returned from a 
    successful call to _rtp_sig_semaphore_alloc.

	@precondition Must not call directly. Use the 
	<b>rtp_sig_semaphore_free</b> macro in rtpsignl.h.

    @return void
 */
void _rtp_sig_semaphore_free (
  RTP_HANDLE semHandle                  /** Handle to the semaphore to be freed. */
  )
{
    rtp_not_yet_implemented ();
}


/*----------------------------------------------------------------------*
                       _rtp_sig_semaphore_wait_timed
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

	@precondition Must not call directly. Use the 
	<b>rtp_sig_semaphore_wait_timed</b> macro in rtpsignl.h.
	
    @return 0 if successful, -1 on error, and 1 if the call timed
    out.  For debugging purposes; if the cause of the error is 
    obtainable at the native Kernel layer, turn on 
    RTP_DEBUG in rtpdebug.h to display the native error 
    value.
 */
int _rtp_sig_semaphore_wait_timed (
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
                          _rtp_sig_semaphore_clear
 *----------------------------------------------------------------------*/
/** @memo   Clear the state of a semaphore.

    @doc    Clear the state of a semaphore.  An example would
    be to set a counting semaphore to zero.
    
    @precondition Must not call directly. Use the 
	<b>rtp_sig_semaphore_clear</b> macro in rtpsignl.h.
    
    @precondition Do not call this function unless past events
    have already been handled or their states are not needed. 
    This will clear the signal and prepare to notify when the 
    next event occurs.

    @return void
 */
void _rtp_sig_semaphore_clear (
  RTP_HANDLE semHandle                  /** Handle of the semaphore to clear. */
  )
{
    rtp_not_yet_implemented ();
}


/*----------------------------------------------------------------------*
                          _rtp_sig_semaphore_wait
 *----------------------------------------------------------------------*/
/** @memo   Wait for a semaphore to be signaled.

    @doc    Wait for a semaphore to be signaled.  If the 
    semaphore has already been signaled this function returns
    immediatley, otherwise it should block indefinitely until 
    the semaphore is signaled.  This calls functionality is 
    identical to calling _rtp_sig_semaphore_wait_timed with a 
    timeout value of -1 (INFINITE).

	@precondition Must not call directly. Use the 
	<b>rtp_sig_semaphore_wait</b> macro in rtpsignl.h.

    @return 0 if successful, -1 otherwise.  For debugging 
    purposes; if the cause of the error is obtainable at the 
    native Kernel layer, turn on RTP_DEBUG in 
    rtpdebug.h to display the native error value.
 */
int _rtp_sig_semaphore_wait (
  RTP_HANDLE semHandle                  /** Handle to the semaphore to be checked. */
  )
{
    rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                         _rtp_sig_semaphore_signal
 *----------------------------------------------------------------------*/
/** @memo   Signal a semaphore.

    @doc    Signal a semaphore.  An example would be to 
    increment a counting semaphore.

	@precondition Must not call directly. Use the 
	<b>rtp_sig_semaphore_signal</b> macro in rtpsignl.h.

    @return void
 */
void _rtp_sig_semaphore_signal (
  RTP_HANDLE semHandle                  /** Handle to the semaphore to be signaled. */
  )
{
    rtp_not_yet_implemented ();
}


/*----------------------------------------------------------------------*
                       _rtp_sig_semaphore_signal_isr
 *----------------------------------------------------------------------*/
/** @memo   Signal an interrupt service routine semaphore.

    @doc    Signal an interrupt service routine semaphore.  In 
    most implementations this will be identical to 
    _rtp_sig_semaphore_signal.

	@precondition Must not call directly. Use the 
	<b>rtp_sig_semaphore_signal_isr</b> macro in rtpsignl.h.

    @return void
 */
void _rtp_sig_semaphore_signal_isr (
  RTP_HANDLE semHandle                  /** Handle to the semaphore to be signaled. */
  )
{
    rtp_not_yet_implemented ();
}


/************************************************************************
 * Mutex [lock] services                                                *
 ************************************************************************/

/*----------------------------------------------------------------------*
                          _rtp_sig_mutex_alloc
 *----------------------------------------------------------------------*/
/** @memo   Create and itialize a mutex.

    @doc    Create and initalize a mutex in the non-claimed
    state.

	@precondition Must not call directly. Use the 
	<b>rtp_sig_mutex_alloc</b> macro in rtpsignl.h.

    @return 0 if successful, -1 otherwise.  If a -1 is returned the 
    value of *newMutex is undefined.  For debugging purposes; 
    if the cause of the error is obtainable at the native Kernel 
    layer, turn on RTP_DEBUG in rtpdebug.h to display the 
    native error value.
 */
int _rtp_sig_mutex_alloc (
  RTP_HANDLE *newMutex,                 /** Storage location for the handle of the newly created mutex. */
  const char *name                      /** The name of the mutex. */
  )
{
    rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                          _rtp_sig_mutex_free
 *----------------------------------------------------------------------*/
/** @memo   Free a mutex.

    @doc    Free a mutex using the handle returned from a 
    successful call to _rtp_sig_mutex_alloc.

	@precondition Must not call directly. Use the 
	<b>rtp_sig_mutex_free</b> macro in rtpsignl.h.

    @return void
 */
void _rtp_sig_mutex_free (
  RTP_HANDLE mutexHandle                /** [tbd] */
  )
{
    rtp_not_yet_implemented ();
}


/*----------------------------------------------------------------------*
                       _rtp_sig_mutex_claim_timed
 *----------------------------------------------------------------------*/
/** @memo   Wait for a mutex to be available.

    @doc    Wait for a mutex to be available.  If the mutex is 
    already available this function returns immediatley, 
    otherwise it should block until the mutex is available or 
    the millisecond time value has expired. Use 0 to poll the 
    mutex and return with its state immediately.  Use -1 to 
    indicate that an infinite timeout value should be used, 
    blocking indefinitely for the mutex to become available.

    @precondition Must not call directly. Use the 
	<b>rtp_sig_mutex_claim_timed</b> macro in rtpsignl.h.
    Also note that for every time this function is successful 
    on a designated mutex handle, a respective call to 
    _rtp_sig_mutex_release must be made.

    @return 0 if successful, -1 on error, and 1 if the call timed
    out.  For debugging purposes; if the cause of the error is 
    obtainable at the native Kernel layer, turn on 
    RTP_DEBUG in rtpdebug.h to display the native error 
    value.
 */
int _rtp_sig_mutex_claim_timed (
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
                          _rtp_sig_mutex_claim
 *----------------------------------------------------------------------*/
/** @memo   Wait for a mutex to be available.

    @doc    Wait for a mutex to be available.  If the mutex is 
    already available this function returns immediatley, 
    otherwise it should block indefinitely or until the mutex 
    is available.

    @precondition Must not call directly. Use the 
	<b>rtp_sig_mutex_claim</b> macro in rtpsignl.h.
	Also note that for every time this function is successful 
    on a designated mutex handle, a respective call to 
    _rtp_sig_mutex_release must be made.

    @return 0 if successful, -1 otherwise.  For debugging 
    purposes; if the cause of the error is obtainable at the 
    native Kernel layer, turn on RTP_DEBUG in rtpdebug.h 
    to display the native error value.
 */
int _rtp_sig_mutex_claim (
  RTP_HANDLE mutexHandle                /** Handle to the mutex to be claimed. */
  )
{
    rtp_not_yet_implemented ();
    return (-1);
}


/*----------------------------------------------------------------------*
                          _rtp_sig_mutex_release
 *----------------------------------------------------------------------*/
/** @memo   Release a mutex.

    @doc    Release a mutex

    @precondition Must not call directly. Use the 
	<b>rtp_sig_mutex_release</b> macro in rtpsignl.h.
	A successful call to _rtp_sig_mutex_claim 
    should be made before calling this function.

    @return void
 */
void _rtp_sig_mutex_release (
  RTP_HANDLE mutexHandle                /** Handle to the mutex to be released. */
  )
{
    rtp_not_yet_implemented ();
}


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */