 /*
 | RTPSIGNL.C - Runtime Platform Signal Services
 |
 |   PORTED TO THE WIN32 PLATFORM
 |
 | EBSnet - EBS_RT-Platform
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
#include "rtpsigin.h"
#include "rtpsignl.h"
#include "rtpdebug.h"

#include <windows.h>

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
 * Semaphore [signalling] services                           RTPSIGIN.H *
 ************************************************************************/

/*----------------------------------------------------------------------*
                         _rtp_sig_semaphore_alloc
 *----------------------------------------------------------------------*/
int _rtp_sig_semaphore_alloc (RTP_SEMAPHORE_INSTANCE* newSem, const char* name)
{
#ifdef RTP_DEBUG
    int  result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    if (!(*newSem = CreateSemaphore (NULL, 0, 10000, NULL)))
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_sig_semaphore_alloc: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return(-1);
    }
    
    return(0);
}


/*----------------------------------------------------------------------*
                         _rtp_sig_semaphore_get_handle
 *----------------------------------------------------------------------*/
RTP_SEMAPHORE _rtp_sig_semaphore_get_handle (RTP_SEMAPHORE_INSTANCE* sem)
{
	return ((RTP_SEMAPHORE)sem);
}


/*----------------------------------------------------------------------*
                         _rtp_sig_semaphore_get_instance
 *----------------------------------------------------------------------*/
RTP_SEMAPHORE_INSTANCE* _rtp_sig_semaphore_get_instance (RTP_SEMAPHORE semHandle)
{
	return ((RTP_SEMAPHORE_INSTANCE*)semHandle);
}


/*----------------------------------------------------------------------*
                         _rtp_sig_semaphore_free
 *----------------------------------------------------------------------*/
void _rtp_sig_semaphore_free (RTP_SEMAPHORE_INSTANCE sem)
{
#ifdef RTP_DEBUG
    int  result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    if (!CloseHandle(sem))
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_sig_semaphore_free: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
    }
}

/************************************************************************
 * Semaphore [signalling] services                           RTPSIGNL.H *
 ************************************************************************/

/*----------------------------------------------------------------------*
                       _rtp_sig_semaphore_wait_timed
 *----------------------------------------------------------------------*/
int _rtp_sig_semaphore_wait_timed (RTP_HANDLE semHandle, long msecs)
{
	HANDLE* winSemaphore;
    DWORD result;

#ifdef RTP_DEBUG
    int  err;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    winSemaphore = rtp_sig_semaphore_get_instance(semHandle);
    if (msecs == (-1))
    {
        result = WaitForSingleObject(winSemaphore, INFINITE);
    }
    else
    {
        result = WaitForSingleObject(winSemaphore, msecs);
    }
    
    if (result == WAIT_FAILED)
    {
#ifdef RTP_DEBUG
        err = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_sig_semaphore_wait_timed: error returned ");
        RTP_DEBUG_OUTPUT_INT(err);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    else if (result == WAIT_ABANDONED)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_sig_semaphore_wait_timed: wait abandoned.\n");
#endif
        return (-1);
    }
    else if (result == WAIT_TIMEOUT)
    {
        return (-1);
    }
    
    return (0);
}


/*----------------------------------------------------------------------*
                          _rtp_sig_semaphore_clear
 *----------------------------------------------------------------------*/
void _rtp_sig_semaphore_clear (RTP_HANDLE semHandle)
{
    HANDLE* winSemaphore;
    DWORD result;

#ifdef RTP_DEBUG
    int  err;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    winSemaphore = rtp_sig_semaphore_get_instance(semHandle);
    while ((result = WaitForSingleObject(winSemaphore, 0)) == WAIT_OBJECT_0)
    {
        ;
    }

#ifdef RTP_DEBUG
    if (result == WAIT_FAILED)
    {
        err = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_sig_semaphore_clear: error returned ");
        RTP_DEBUG_OUTPUT_INT(err);
        RTP_DEBUG_OUTPUT_STR(".\n");
    }
#endif
}


/*----------------------------------------------------------------------*
                          _rtp_sig_semaphore_wait
 *----------------------------------------------------------------------*/
int _rtp_sig_semaphore_wait (RTP_HANDLE semHandle)
{
    HANDLE* winSemaphore;
    
#ifdef RTP_DEBUG
    int  err;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif
    
    winSemaphore = rtp_sig_semaphore_get_instance(semHandle);
    if (WaitForSingleObject(winSemaphore, INFINITE) == WAIT_FAILED)
    {
#ifdef RTP_DEBUG
        err = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_sig_semaphore_wait: error returned ");
        RTP_DEBUG_OUTPUT_INT(err);
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
    HANDLE* winSemaphore;
    
#ifdef RTP_DEBUG
    int  err;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif
    
    winSemaphore = rtp_sig_semaphore_get_instance(semHandle);
    if (!ReleaseSemaphore(winSemaphore, 1, NULL))
    {
#ifdef RTP_DEBUG
        err = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_sig_semaphore_signal: error returned ");
        RTP_DEBUG_OUTPUT_INT(err);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
    }
}


/*----------------------------------------------------------------------*
                       _rtp_sig_semaphore_signal_isr
 *----------------------------------------------------------------------*/
void _rtp_sig_semaphore_signal_isr (RTP_HANDLE semHandle)
{
    HANDLE* winSemaphore;
    
#ifdef RTP_DEBUG
    int  err;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif
    
    winSemaphore = rtp_sig_semaphore_get_instance(semHandle);
    if (!ReleaseSemaphore(winSemaphore, 1, NULL))
    {
#ifdef RTP_DEBUG
        err = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_sig_semaphore_signal_isr: error returned ");
        RTP_DEBUG_OUTPUT_INT(err);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
    }
}


/************************************************************************
 * Mutex [lock] services                                     RTPSIGIN.H *
 ************************************************************************/

/*----------------------------------------------------------------------*
                          _rtp_sig_mutex_alloc
 *----------------------------------------------------------------------*/
int _rtp_sig_mutex_alloc (RTP_MUTEX_INSTANCE* newMutex, const char* name)
{
#ifdef RTP_DEBUG
    int  result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    if (!(*newMutex = CreateMutex (NULL, FALSE, NULL)))
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_sig_mutex_alloc: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return(-1);
    }
    
    return(0);
}


/*----------------------------------------------------------------------*
                          _rtp_sig_mutex_get_handle
 *----------------------------------------------------------------------*/
RTP_MUTEX _rtp_sig_mutex_get_handle (RTP_MUTEX_INSTANCE* mutex)
{
    return ((RTP_MUTEX)mutex);
}


/*----------------------------------------------------------------------*
                          _rtp_sig_mutex_get_instance
 *----------------------------------------------------------------------*/
RTP_MUTEX_INSTANCE* _rtp_sig_mutex_get_instance (RTP_MUTEX mutexHandle)
{
    return ((RTP_MUTEX_INSTANCE*)mutexHandle);
}


/*----------------------------------------------------------------------*
                          _rtp_sig_mutex_free
 *----------------------------------------------------------------------*/
void _rtp_sig_mutex_free (RTP_MUTEX_INSTANCE* mutex)
{
#ifdef RTP_DEBUG
    int  result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    if (!CloseHandle(mutex))
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_sig_mutex_release: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
    }
}


/*----------------------------------------------------------------------*
                       _rtp_sig_mutex_claim_timed
 *----------------------------------------------------------------------*/
int _rtp_sig_mutex_claim_timed (RTP_HANDLE mutexHandle, long msecs)
{
    HANDLE* winMutex;
    DWORD result;

#ifdef RTP_DEBUG
    int  err;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    winMutex = rtp_sig_mutex_get_instance(mutexHandle);
    if (msecs == RTP_TIMEOUT_INFINITE)
    {
        result = WaitForSingleObject(winMutex, INFINITE);
    }
    else
    {
        result = WaitForSingleObject(winMutex, msecs);
    }
    
    if (result == WAIT_FAILED || result == WAIT_TIMEOUT)
    {
#ifdef RTP_DEBUG
        err = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_sig_mutex_claim_timed: error returned ");
        RTP_DEBUG_OUTPUT_INT(err);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    
    return (0);
}


/*----------------------------------------------------------------------*
                          _rtp_sig_mutex_claim
 *----------------------------------------------------------------------*/
int _rtp_sig_mutex_claim (RTP_HANDLE mutexHandle)
{
    HANDLE* winMutex;

#ifdef RTP_DEBUG
    int  result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    winMutex = rtp_sig_mutex_get_instance(mutexHandle);
    if (WaitForSingleObject(winMutex, INFINITE) == WAIT_FAILED)
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_sig_mutex_claim: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return (-1);
    }
    
    return (0);
}


/*----------------------------------------------------------------------*
                          _rtp_sig_mutex_release
 *----------------------------------------------------------------------*/
void _rtp_sig_mutex_release (RTP_HANDLE mutexHandle)
{
    HANDLE* winMutex;
    
#ifdef RTP_DEBUG
    int  result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    winMutex = rtp_sig_mutex_get_instance(mutexHandle);
    if (!ReleaseMutex(winMutex))
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_sig_mutex_release: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
    }
}


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
