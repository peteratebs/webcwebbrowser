 /*
 | RTPSIGNL.C - Runtime Platform Signal Services
 |
 |   PORTED TO THE LINUX PLATFORM
 |
 | EBSnet - RT-Platform
 |
 |  $Author: tony $
 |  $Date: 2005/04/26 18:35:52 $
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
#include "rtpsignl.h"
#include "rtpdebug.h"

#include <pthread.h>

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
#ifdef LINUXTOBEIMPLEMENTED
    HANDLE winSemaphore;

#ifdef RTP_DEBUG
    int  result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    if (!(winSemaphore = CreateSemaphore (NULL, 0, 10000, NULL)))
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_sig_semaphore_alloc: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return(-1);
    }
    
    *newSem = (RTP_HANDLE)winSemaphore;
    return(0);
#else
    return(0);
#endif
}


/*----------------------------------------------------------------------*
                         _rtp_sig_semaphore_free
 *----------------------------------------------------------------------*/
void _rtp_sig_semaphore_free (RTP_HANDLE semHandle)
{
#ifdef LINUXTOBEIMPLEMENTED
#ifdef RTP_DEBUG
    int  result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    if (CloseHandle((HANDLE)semHandle) != 0)
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_sig_semaphore_free: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
    }
#endif
}


/*----------------------------------------------------------------------*
                       _rtp_sig_semaphore_wait_timed
 *----------------------------------------------------------------------*/
int _rtp_sig_semaphore_wait_timed (RTP_HANDLE semHandle, long msecs)
{
#ifdef LINUXTOBEIMPLEMENTED
    DWORD result;

#ifdef RTP_DEBUG
    int  err;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    if (msecs == (-1))
    {
        result = WaitForSingleObject((HANDLE)semHandle, INFINITE);
    }
    else
    {
        result = WaitForSingleObject((HANDLE)semHandle, msecs);
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
#else
    return (0);
#endif
}


/*----------------------------------------------------------------------*
                          _rtp_sig_semaphore_clear
 *----------------------------------------------------------------------*/
void _rtp_sig_semaphore_clear (RTP_HANDLE semHandle)
{
#ifdef LINUXTOBEIMPLEMENTED
    DWORD result;

#ifdef RTP_DEBUG
    int  err;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    while ((result = WaitForSingleObject((HANDLE)semHandle, 0)) == WAIT_OBJECT_0)
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
#endif
}


/*----------------------------------------------------------------------*
                          _rtp_sig_semaphore_wait
 *----------------------------------------------------------------------*/
int _rtp_sig_semaphore_wait (RTP_HANDLE semHandle)
{
#ifdef LINUXTOBEIMPLEMENTED
#ifdef RTP_DEBUG
    int  err;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif
    
    if (WaitForSingleObject((HANDLE)semHandle, INFINITE) == WAIT_FAILED)
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
#else
    return (0);
#endif
}


/*----------------------------------------------------------------------*
                         _rtp_sig_semaphore_signal
 *----------------------------------------------------------------------*/
void _rtp_sig_semaphore_signal (RTP_HANDLE semHandle)
{
#ifdef LINUXTOBEIMPLEMENTED
#ifdef RTP_DEBUG
    int  err;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif
    
    if (ReleaseSemaphore((HANDLE)semHandle, 1, NULL) == 0)
    {
#ifdef RTP_DEBUG
        err = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_sig_semaphore_signal: error returned ");
        RTP_DEBUG_OUTPUT_INT(err);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
    }
#endif
}


/*----------------------------------------------------------------------*
                       _rtp_sig_semaphore_signal_isr
 *----------------------------------------------------------------------*/
void _rtp_sig_semaphore_signal_isr (RTP_HANDLE semHandle)
{
#ifdef LINUXTOBEIMPLEMENTED
#ifdef RTP_DEBUG
    int  err;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif
    
    if (ReleaseSemaphore((HANDLE)semHandle, 1, NULL) == 0)
    {
#ifdef RTP_DEBUG
        err = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_sig_semaphore_signal_isr: error returned ");
        RTP_DEBUG_OUTPUT_INT(err);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
    }
#endif
}


/************************************************************************
 * Mutex [lock] services                                                *
 ************************************************************************/

/*----------------------------------------------------------------------*
                          _rtp_sig_mutex_alloc
 *----------------------------------------------------------------------*/
int _rtp_sig_mutex_alloc (RTP_HANDLE *newMutex, const char *name)
{
    pthread_mutex_t *linMutex;
    pthread_mutexattr_t attrs;

    linMutex = (pthread_mutex_t *) malloc (sizeof (pthread_mutex_t));
    if (!linMutex)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_sig_mutex_alloc: error could not allocate memory.\n");
#endif
        return(-1);
    }

    pthread_mutexattr_init (&attrs);
    pthread_mutexattr_settype (&attrs, PTHREAD_MUTEX_RECURSIVE_NP);
    pthread_mutex_init (linMutex, &attrs);
    pthread_mutexattr_destroy (&attrs);

    *newMutex = (RTP_HANDLE)linMutex;
    return(0);
}


/*----------------------------------------------------------------------*
                          _rtp_sig_mutex_free
 *----------------------------------------------------------------------*/
void _rtp_sig_mutex_free (RTP_HANDLE mutexHandle)
{
    if (pthread_mutex_destroy((pthread_mutex_t *)mutexHandle) != 0)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_sig_mutex_free: error mutex is locked.\n");
#endif
    }

    free((void *)mutexHandle);
}


/*----------------------------------------------------------------------*
                       _rtp_sig_mutex_claim_timed
 *----------------------------------------------------------------------*/
int _rtp_sig_mutex_claim_timed (RTP_HANDLE mutexHandle, long msecs)
{
#ifdef LINUXTOBEIMPLEMENTED
    DWORD result;

#ifdef RTP_DEBUG
    int  err;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    if (msecs == (-1))
    {
        result = WaitForSingleObject((HANDLE)mutexHandle, INFINITE);
    }
    else
    {
        result = WaitForSingleObject((HANDLE)mutexHandle, msecs);
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
#else
    return (0);
#endif
}


/*----------------------------------------------------------------------*
                          _rtp_sig_mutex_claim
 *----------------------------------------------------------------------*/
int _rtp_sig_mutex_claim (RTP_HANDLE mutexHandle)
{
    if (pthread_mutex_lock((pthread_mutex_t *)mutexHandle) != 0)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_sig_mutex_claim: error invalid mutex.\n");
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
    if (pthread_mutex_unlock((pthread_mutex_t *)mutexHandle) != 0)
    {
#ifdef RTP_DEBUG
        RTP_DEBUG_OUTPUT_STR("rtp_sig_mutex_release: error invalid mutex.\n");
#endif
    }
}


/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
