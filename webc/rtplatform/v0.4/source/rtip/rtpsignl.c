 /*
 | RTPSIGNL.C - Runtime Platform Signal Services
 |
 |   PORTED TO THE EBSnet Inc RTIP TCP/IP PLATFORM
 |
 | EBSnet - RT-Platform
 |
 |  $Author: shane $
 |  $Date: 2004/10/14 18:33:37 $
 |  $Name:  $
 |  $Revision: 1.6 $
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
#include "rtpirq.h"
#include "rtpdebug.h"
#include <rtip.h>

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
int _rtp_sig_semaphore_alloc (RTP_HANDLE *newSem, const char *name)
{
	KS_RTIPSIG rtipSemaphore;

	/* CAN'T TRUST THE RETURN VALUE OF THIS FUNCTION */
	KS_SIGNAL_BUILD(rtipSemaphore)
    *newSem = (RTP_HANDLE)rtipSemaphore;
    return(0);
}


/*----------------------------------------------------------------------*
                         _rtp_sig_semaphore_free
 *----------------------------------------------------------------------*/
void _rtp_sig_semaphore_free (RTP_HANDLE semHandle)
{
    KS_SIGNAL_DELETE((KS_RTIPSIG)semHandle);
}


/*----------------------------------------------------------------------*
                       _rtp_sig_semaphore_wait_timed
 *----------------------------------------------------------------------*/
int _rtp_sig_semaphore_wait_timed (RTP_HANDLE semHandle, long msecs)
{
    int result;

#ifdef RTP_DEBUG
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);
#endif

    if (msecs == (-1))
    {
    	result = KS_SIGNAL_GET((KS_RTIPSIG)semHandle, RTIP_INF);
    }
    else
    {
        result = KS_SIGNAL_GET((KS_RTIPSIG)semHandle, msecs);
    }
    
    if (!result)
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror();
        RTP_DEBUG_OUTPUT_STR("rtp_sig_semaphore_wait_timed: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    
    return (0);
}


/*----------------------------------------------------------------------*
                          _rtp_sig_semaphore_clear
 *----------------------------------------------------------------------*/
void _rtp_sig_semaphore_clear (RTP_HANDLE semHandle)
{
	KS_SIGNAL_CLEAR((KS_RTIPSIG)semHandle);
}


/*----------------------------------------------------------------------*
                          _rtp_sig_semaphore_wait
 *----------------------------------------------------------------------*/
int _rtp_sig_semaphore_wait (RTP_HANDLE semHandle)
{
#ifdef RTP_DEBUG
    int  result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    set_errno (0);
#endif
    
    if (!KS_SIGNAL_GET((KS_RTIPSIG)semHandle, RTIP_INF))
    {
#ifdef RTP_DEBUG
        result = xn_getlasterror();
        RTP_DEBUG_OUTPUT_STR("rtp_sig_semaphore_wait: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    
    return (0);
}


/*----------------------------------------------------------------------*
                         _rtp_sig_semaphore_signal
 *----------------------------------------------------------------------*/
void _rtp_sig_semaphore_signal (RTP_HANDLE semHandle)
{
    KS_SIGNAL_SET((KS_RTIPSIG)semHandle);
}


/*----------------------------------------------------------------------*
                       _rtp_sig_semaphore_signal_isr
 *----------------------------------------------------------------------*/
void _rtp_sig_semaphore_signal_isr (RTP_HANDLE semHandle)
{
	RTP_HANDLE level;

    level = rtp_irq_push_disable( );
    KS_SIGNAL_SET((KS_RTIPSIG)semHandle);
	rtp_irq_pop(level);
}


/************************************************************************
 * Mutex [lock] services                                                *
 ************************************************************************/

/*----------------------------------------------------------------------*
                          _rtp_sig_mutex_alloc
 *----------------------------------------------------------------------*/
int _rtp_sig_mutex_alloc (RTP_HANDLE *newMutex, const char *name)
{
    KS_RTIPSEM rtipMutex;

	/* CAN'T TRUST THE RETURN VALUE OF THIS FUNCTION */
	KS_SEMAPHORE_BUILD(rtipMutex);    
    *newMutex = (RTP_HANDLE)rtipMutex;
    
    return(0);
}


/*----------------------------------------------------------------------*
                          _rtp_sig_mutex_free
 *----------------------------------------------------------------------*/
void _rtp_sig_mutex_free (RTP_HANDLE mutexHandle)
{
	KS_SEMAPHORE_FREE((KS_RTIPSEM)mutexHandle);
}


/*----------------------------------------------------------------------*
                       _rtp_sig_mutex_claim_timed
 *----------------------------------------------------------------------*/
int _rtp_sig_mutex_claim_timed (RTP_HANDLE mutexHandle, long msecs)
{
	/* RTIP ONLY SUPPORTS WAITING FOREVER */
    KS_SEMAPHORE_GET((KS_RTIPSEM)mutexHandle);    
    return (0);
}


/*----------------------------------------------------------------------*
                          _rtp_sig_mutex_claim
 *----------------------------------------------------------------------*/
int _rtp_sig_mutex_claim (RTP_HANDLE mutexHandle)
{
    KS_SEMAPHORE_GET((KS_RTIPSEM)mutexHandle);    
    return (0);
}


/*----------------------------------------------------------------------*
                          _rtp_sig_mutex_release
 *----------------------------------------------------------------------*/
void _rtp_sig_mutex_release (RTP_HANDLE mutexHandle)
{
    KS_SEMAPHORE_GIVE((KS_RTIPSEM)mutexHandle);
}


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */