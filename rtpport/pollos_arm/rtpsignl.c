//
// RTPSIGNL.C - Runtime Platform Signal Services
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
#include "rtpmem.h"
#include "rtpdebug.h"
#include "pollos.h"


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

P_POLLOS_SIGNAL	poll_sig;
 	
	poll_sig = (P_POLLOS_SIGNAL)rtp_malloc((unsigned long )sizeof(struct pollos_signal), 0);
	
	if (poll_sig)
	{
		poll_sig->signal_count = 0; 
		poll_sig->signal_timeout = 0;
		*newSem = (RTP_HANDLE)poll_sig;

    	/* success, return */
    	return(0);
    }

   	RTP_DEBUG_OUTPUT_STR("rtp_sig_semaphore_alloc: Semaphore not Created!");

    return (-1);
}

/*----------------------------------------------------------------------*
                         rtp_sig_semaphore_free
 *----------------------------------------------------------------------*/
void rtp_sig_semaphore_free (RTP_SEMAPHORE semHandle)
{

	rtp_free((void *)semHandle);

}

/*----------------------------------------------------------------------*
                       rtp_sig_semaphore_wait_timed
 *----------------------------------------------------------------------*/
/* Return 1 on timeout */
int rtp_sig_semaphore_wait_timed (RTP_SEMAPHORE semHandle, long msecs)
{

	
	if (msecs == RTP_TIMEOUT_INFINITE)
	{
		return(!pollos_test((P_POLLOS_SIGNAL)semHandle, (TIME_TICKS) RTP_TIMEOUT_INFINITE));
	}
	
	return (!pollos_test((P_POLLOS_SIGNAL)semHandle, msecs));

}


/*----------------------------------------------------------------------*
                          rtp_sig_semaphore_clear
 *----------------------------------------------------------------------*/
void rtp_sig_semaphore_clear (RTP_SEMAPHORE semHandle)
{

	pollos_clear_q((P_POLLOS_SIGNAL)semHandle);

}


/*----------------------------------------------------------------------*
                          rtp_sig_semaphore_wait
 *----------------------------------------------------------------------*/
int rtp_sig_semaphore_wait (RTP_SEMAPHORE semHandle)
{

    return (!pollos_test((P_POLLOS_SIGNAL)semHandle, RTP_TIMEOUT_INFINITE));

}


/*----------------------------------------------------------------------*
                         rtp_sig_semaphore_signal
 *----------------------------------------------------------------------*/
void rtp_sig_semaphore_signal (RTP_SEMAPHORE semHandle)
{

	pollos_signal((P_POLLOS_SIGNAL)semHandle);
    

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
                         rtp_sig_mutex_init
 *----------------------------------------------------------------------*/
int  rtp_sig_mutex_init (int nMutex, RTP_MUTEX_CONTEXTP ctxBlock)
{
	// nothing to do - POLLOS does not need semaphores
    return(0);
}	

/*----------------------------------------------------------------------*
                         rtp_sig_mutex_shutdown
 *----------------------------------------------------------------------*/
void rtp_sig_mutex_shutdown (void)
{
	// nothing to do - POLLOS does not need semaphores
}
/*----------------------------------------------------------------------*
                          rtp_sig_mutex_alloc
 *----------------------------------------------------------------------*/
int rtp_sig_mutex_alloc (RTP_MUTEX *newMutex, const char *name)
{
	// nothing to do - POLLOS does not need semaphores
    return(0);
}

/*----------------------------------------------------------------------*
                          rtp_sig_mutex_free
 *----------------------------------------------------------------------*/
void rtp_sig_mutex_free (RTP_MUTEX mutexHandle)
{
	// nothing to do - POLLOS does not need semaphores
}

/*----------------------------------------------------------------------*
                          rtp_sig_mutex_release
 *----------------------------------------------------------------------*/
void rtp_sig_mutex_release (RTP_MUTEX mutexHandle)
{
	// nothing to do - POLLOS does not need semaphores
}


/*----------------------------------------------------------------------*
                         rtp_sig_mutex_alloc_dynamic
 *----------------------------------------------------------------------*/
#ifdef RTP_INC_DYNAMIC_MUTEX
int  rtp_sig_mutex_alloc_dynamic (RTP_MUTEX *newMutex, const char *name, 
                                       RTP_MUTEX_ALLOC *funcptr)
{
	// nothing to do - POLLOS does not need semaphores
    return(0);
}                                       
#endif


/*----------------------------------------------------------------------*
                       rtp_sig_mutex_claim
 *----------------------------------------------------------------------*/
int _rtp_sig_mutex_claim(RTP_MUTEX mutexHandle)
{
    return (0);
}

/*----------------------------------------------------------------------*
                       rtp_sig_mutex_claim_timed
 *----------------------------------------------------------------------*/
int _rtp_sig_mutex_claim_timed (RTP_MUTEX mutexHandle, long msecs)
{
    return (0);
}


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
