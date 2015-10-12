 /*
 | RTPSIGNL.C - Runtime Platform Signal Services
 |
 |   PORTED TO THE WIN32 PLATFORM
 |
 | EBSnet - RT-Platform
 |
 |  $Author: shane $
 |  $Date: 2004/10/05 18:51:31 $
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
 * Semaphore [signalling] services                                      *
 ************************************************************************/

/*----------------------------------------------------------------------*
                         _rtp_sig_semaphore_alloc
 *----------------------------------------------------------------------*/
int _rtp_sig_semaphore_alloc (RTP_HANDLE *newSem, const char *name)
{
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
}


/*----------------------------------------------------------------------*
                         _rtp_sig_semaphore_free
 *----------------------------------------------------------------------*/
void _rtp_sig_semaphore_free (RTP_HANDLE semHandle)
{
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
}


/*----------------------------------------------------------------------*
                       _rtp_sig_semaphore_wait_timed
 *----------------------------------------------------------------------*/
int _rtp_sig_semaphore_wait_timed (RTP_HANDLE semHandle, long msecs)
{
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
}


/*----------------------------------------------------------------------*
                          _rtp_sig_semaphore_clear
 *----------------------------------------------------------------------*/
void _rtp_sig_semaphore_clear (RTP_HANDLE semHandle)
{
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
}


/*----------------------------------------------------------------------*
                          _rtp_sig_semaphore_wait
 *----------------------------------------------------------------------*/
int _rtp_sig_semaphore_wait (RTP_HANDLE semHandle)
{
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
}


/*----------------------------------------------------------------------*
                         _rtp_sig_semaphore_signal
 *----------------------------------------------------------------------*/
void _rtp_sig_semaphore_signal (RTP_HANDLE semHandle)
{
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
}


/*----------------------------------------------------------------------*
                       _rtp_sig_semaphore_signal_isr
 *----------------------------------------------------------------------*/
void _rtp_sig_semaphore_signal_isr (RTP_HANDLE semHandle)
{
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
}


/************************************************************************
 * Mutex [lock] services                                                *
 ************************************************************************/

/*----------------------------------------------------------------------*
                          _rtp_sig_mutex_alloc
 *----------------------------------------------------------------------*/
int _rtp_sig_mutex_alloc (RTP_HANDLE *newMutex, const char *name)
{
    HANDLE winMutex;

#ifdef RTP_DEBUG
    int  result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    if (!(winMutex = CreateMutex (NULL, FALSE, NULL)))
    {
#ifdef RTP_DEBUG
        result = GetLastError();
        RTP_DEBUG_OUTPUT_STR("rtp_sig_mutex_alloc: error returned ");
        RTP_DEBUG_OUTPUT_INT(result);
        RTP_DEBUG_OUTPUT_STR(".\n");
#endif
        return(-1);
    }
    
    *newMutex = (RTP_HANDLE)winMutex;
    return(0);
}


/*----------------------------------------------------------------------*
                          _rtp_sig_mutex_free
 *----------------------------------------------------------------------*/
void _rtp_sig_mutex_free (RTP_HANDLE mutexHandle)
{
#ifdef RTP_DEBUG
    int  result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    if (CloseHandle((HANDLE)mutexHandle) != 0)
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
}


/*----------------------------------------------------------------------*
                          _rtp_sig_mutex_claim
 *----------------------------------------------------------------------*/
int _rtp_sig_mutex_claim (RTP_HANDLE mutexHandle)
{
#ifdef RTP_DEBUG
    int  result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    if (WaitForSingleObject((HANDLE)mutexHandle, INFINITE) == WAIT_FAILED)
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
#ifdef RTP_DEBUG
    int  result;
    /* ----------------------------------- */
    /*  Clear the error state by setting   */
    /*  to 0.                              */
    /* ----------------------------------- */
    SetLastError (0);
#endif

    if (!ReleaseMutex((HANDLE)mutexHandle))
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