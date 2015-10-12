//
// RTPSIGNL.C - Runtime Platform Signal Services
//
//   PORTED TO THE POLLOS system
//
// EBSnet - EBS_RT-Platform
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
#include "rtpmem.h"
#include "rtpdebug.h"
#include "pollos.h"
//#include "net.h" 
#include "dc_type.h"
#include "dc_return_codes.h"
#include "dc_log_api.h"

#define TX_DISABLE_ERROR_CHECKING 1
#include "tx_api.h" // Oasis

/************************************************************************
* Defines
************************************************************************/

/************************************************************************
* Types
************************************************************************/

/************************************************************************
* Data
************************************************************************/
int rtpSemaphores;
int rtpMutexes;
/************************************************************************
* Macros
************************************************************************/

/************************************************************************
* Utility Function Types & Prototypes
************************************************************************/

/************************************************************************
* API Function Bodies
************************************************************************/

/************************************************************************
 * Semaphore [signalling] services                                      *
 ************************************************************************/

/*----------------------------------------------------------------------*
                         rtp_sig_semaphore_alloc
 *----------------------------------------------------------------------*/
int rtp_sig_semaphore_alloc (RTP_SEMAPHORE *newSem, const char *name)
{
    // Oasis -----
    int result ;
    *newSem = (RTP_HANDLE)_rtp_malloc (sizeof(TX_SEMAPHORE));

    result = tx_semaphore_create((TX_SEMAPHORE *)(*newSem), (char *)name, 0) ;
    if (result != TX_SUCCESS)

    {
		if(*newSem) _rtp_free((void*)*newSem) ;

        dc_log_printf("SemAlloc Failed - %d", result) ;
        return (-1) ;
    }

	rtpSemaphores++;
    return 0 ;
    // ----- Oasis
}

/*----------------------------------------------------------------------*
                         rtp_sig_semaphore_free
 *----------------------------------------------------------------------*/
void rtp_sig_semaphore_free (RTP_SEMAPHORE semHandle)
{
    int result ;
    result = tx_semaphore_delete((TX_SEMAPHORE *)semHandle) ; // Oasis
    if (result != TX_SUCCESS)
    {
        dc_log_printf("SemFree Failed - %d", result) ;
    }
    _rtp_free((void *)semHandle) ;
	rtpSemaphores--;
}

/*----------------------------------------------------------------------*
                       rtp_sig_semaphore_wait_timed
 *----------------------------------------------------------------------*/
int rtp_sig_semaphore_wait_timed (RTP_SEMAPHORE semHandle, long msecs)
{
    // Oasis -----
	long ticks ;
    UINT32 utime=1 ;
    
    dc_utime_get_from_tick(&utime);
    if (msecs >= 0)
        ticks = (msecs*1000)/utime;
    else
        ticks = msecs ;
        
	return (tx_semaphore_get((TX_SEMAPHORE *)semHandle, ticks) == TX_SUCCESS ? 0 : -1) ;
    // ----- Oasis
}


/*----------------------------------------------------------------------*
                          rtp_sig_semaphore_clear
 *----------------------------------------------------------------------*/
void rtp_sig_semaphore_clear (RTP_SEMAPHORE semHandle)
{
    while (tx_semaphore_get((TX_SEMAPHORE *)semHandle, 0) == TX_SUCCESS); // Oasis
}


/*----------------------------------------------------------------------*
                          rtp_sig_semaphore_wait
 *----------------------------------------------------------------------*/
int rtp_sig_semaphore_wait (RTP_SEMAPHORE semHandle)
{
    dc_log_printf("SemClaim - %s", (char *)(((TX_SEMAPHORE *)semHandle)->tx_semaphore_name)) ;
    return (tx_semaphore_get((TX_SEMAPHORE *)semHandle, TX_WAIT_FOREVER) == TX_SUCCESS ? 0 : -1) ; // Oasis
}


/*----------------------------------------------------------------------*
                         rtp_sig_semaphore_signal
 *----------------------------------------------------------------------*/
void rtp_sig_semaphore_signal (RTP_SEMAPHORE semHandle)
{
#if 0
    TX_INTERRUPT_SAVE_AREA
    char *name;
    unsigned long current_value;
    TX_THREAD *first_suspended;
    unsigned long suspended_count;
    TX_SEMAPHORE *next_semaphore;
    
    TX_DISABLE
    tx_semaphore_info_get((TX_SEMAPHORE *)semHandle, &name, &current_value,
                          &first_suspended, &suspended_count,
                          &next_semaphore);
    if (current_value <= 0)
        tx_semaphore_put((TX_SEMAPHORE *)semHandle) ; // Oasis
    TX_RESTORE
#else
    tx_semaphore_put((TX_SEMAPHORE *)semHandle) ; // Oasis
#endif
}


/*----------------------------------------------------------------------*
                       rtp_sig_semaphore_signal_isr
 *----------------------------------------------------------------------*/
void rtp_sig_semaphore_signal_isr (RTP_SEMAPHORE semHandle)
{
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
int rtp_sig_mutex_alloc (
  RTP_HANDLE *newMutex,                 /** Storage location for the handle of the newly created mutex. */
  const char *name                      /** The name of the mutex. */
  )
{
    // Oasis -----
    int result ;
    *newMutex = (RTP_HANDLE)_rtp_malloc (sizeof(TX_MUTEX));
    result = tx_mutex_create((TX_MUTEX *)(*newMutex), (char *)name, TX_NO_INHERIT) ;
    if (result != TX_SUCCESS)
    {
		if(*newMutex) _rtp_free((void*)*newMutex);

        dc_log_printf("MutexAlloc Failed - %d", result) ;
        return (-1) ;
    }

	rtpMutexes++;
    return 0 ;
    // ----- Oasis
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
void rtp_sig_mutex_free (
  RTP_HANDLE mutexHandle                
  )
{
    // Oasis -----
    int result ;
    result = tx_mutex_delete((TX_MUTEX *)mutexHandle) ; // till count is zero
    if (result != TX_SUCCESS)
    {
        dc_log_printf("MutexFree Failed - %d", result) ;
    }
    _rtp_free((void *)mutexHandle) ;
	rtpMutexes--;
    // ----- Oasis
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
int rtp_sig_mutex_claim_timed (
  RTP_HANDLE mutexHandle,               /** Handle to the mutex to be checked. */
  long msecs                            /** Timeout value in milliseconds:<br>
	<pre>
|		 0   To poll.
|		-1   To indicate use of infinite.
	</pre> */
  )
{
    // Oasis -----
	long ticks ;
    UINT32 utime=1 ;
    
    dc_utime_get_from_tick(&utime);
    if (msecs >= 0)
        ticks = (msecs*1000)/utime;
    else
        ticks = msecs ;
        
//    dc_log_printf("MutClaimT - %s", (char *)(((TX_MUTEX *)mutexHandle)->tx_mutex_name)) ;
    if (tx_mutex_get((TX_MUTEX *)mutexHandle, ticks))
        return (-1) ;
    return 0 ;
    // ----- Oasis
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
//    dc_log_printf("MutClaim - %s", (char *)(((TX_MUTEX *)mutexHandle)->tx_mutex_name)) ;
    return(tx_mutex_get((TX_MUTEX *)mutexHandle, TX_WAIT_FOREVER) == TX_SUCCESS ? 0 : -1) ; // Oasis
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
//    dc_log_printf("MutRel - %s", (char *)(((TX_MUTEX *)mutexHandle)->tx_mutex_name)) ;
    tx_mutex_put((TX_MUTEX *)mutexHandle) ; // Oasis
}
/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
