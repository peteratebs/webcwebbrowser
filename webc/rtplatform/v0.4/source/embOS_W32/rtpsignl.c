//
// RTPSIGNL.C - Runtime Platform Signal Services
//
//   PORTED TO THE Segger embOS simulation for Windows 32 bit system
//                 and Visual C++.
//
// EBSnet - RT-Platform
//
//  $Author: tom $
//  $Date: 2004/10/22 18:11:57 $
//  $Name:  $
//  $Revision: 1.5 $
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
#define RTP_INCLUDE_SIGNL
#include "rtpconfig.h"

/************************************************************************
* Defines
************************************************************************/

/************************************************************************
* Types
************************************************************************/

/************************************************************************
* Data
************************************************************************/
RTP_SIGNAL_CONTEXT *rtp_signal_free = 0; /* Freelist for pre-allocated signals */
RTP_MUTEX_CONTEXT *rtp_mutex_free = 0;   /* Freelist for pre-allocated mutexes */

/************************************************************************
* Macros
************************************************************************/

/************************************************************************
* Utility Function Types & Prototypes
************************************************************************/

typedef struct s_SingleList
{
    struct s_SingleList *next;
}_SingleList;

static void _rtp_addSingleList(void **List, _SingleList *Member);
static _SingleList *_rtp_removeSingleList(void **List);

/************************************************************************
* API Function Bodies
************************************************************************/

/************************************************************************
 * Semaphore [signalling] services                                      *
 ************************************************************************/

/*----------------------------------------------------------------------*
                         rtp_sig_semaphore_init
 *----------------------------------------------------------------------*/
int  rtp_sig_semaphore_init (int nSem, RTP_SIGNAL_CONTEXTP ctxBlock)
{
    int i;
    RTP_SIGNAL_CONTEXT  *rtp_signal_context = (RTP_SIGNAL_CONTEXT  *)ctxBlock;
    for (i = 0; i < nSem; i++)
    {
#ifdef RTP_INC_DYNAMIC_SIGNAL
    	rtp_signal_context->func.AllocFn = 0;
    	rtp_signal_context->func.FreeFn = 0;
#endif
        _rtp_addSingleList((void **)&rtp_signal_free, (_SingleList *)rtp_signal_context);
        rtp_signal_context++;
    }
    return(0);
}	

/*----------------------------------------------------------------------*
                         rtp_sig_semaphore_shutdown
 *----------------------------------------------------------------------*/
void rtp_sig_semaphore_shutdown (void)
{
    rtp_not_yet_implemented();
}

/*----------------------------------------------------------------------*
                         rtp_sig_semaphore_alloc
 *----------------------------------------------------------------------*/
int rtp_sig_semaphore_alloc (RTP_SEMAPHORE *newSem, const char *name)
{
    if((*newSem = (RTP_SEMAPHORE)(_rtp_removeSingleList((void *)&rtp_signal_free))) != 0)
    {
        ((RTP_SIGNAL_CONTEXT *)*newSem)->name = (char *)name;
        OS_CREATECSEMA(&((RTP_SIGNAL_CONTEXT *)*newSem)->semaResource);
        return(0);
    }

    else
        return(-1);
}

/*----------------------------------------------------------------------*
                         rtp_sig_semaphore_free
 *----------------------------------------------------------------------*/
void rtp_sig_semaphore_free (RTP_SEMAPHORE semHandle)
{
    OS_DeleteCSema(&((RTP_SIGNAL_CONTEXT *)semHandle)->semaResource);

#ifdef RTP_INC_DYNAMIC_SIGNAL
    if(((RTP_SIGNAL_CONTEXT *)semHandle)->func.FreeFn)
       ((RTP_SIGNAL_CONTEXT *)semHandle)->func.FreeFn((void *)semHandle);
    else
#endif
    _rtp_addSingleList((void **)&rtp_signal_free, (_SingleList *)semHandle);
}

/*----------------------------------------------------------------------*
                         rtp_sig_semaphore_alloc_dynamic
 *----------------------------------------------------------------------*/
#ifdef RTP_INC_DYNAMIC_SIGNAL
int  rtp_sig_semaphore_alloc_dynamic (RTP_SEMAPHORE *newSem, const char *name, 
                                       RTP_SIGNAL_ALLOC *funcptr)
{
    if((*newSem = (RTP_SEMAPHORE)(funcptr->AllocFn(sizeof(RTP_SIGNAL_CONTEXT)))) == 0)
        return(-1);
    ((RTP_SIGNAL_CONTEXT *)*newSem)->name = (char *)name;
    ((RTP_SIGNAL_CONTEXT *)*newSem)->func.AllocFn = funcptr->AllocFn;
    ((RTP_SIGNAL_CONTEXT *)*newSem)->func.FreeFn = funcptr->FreeFn;
    OS_CREATECSEMA(&((RTP_SIGNAL_CONTEXT *)*newSem)->semaResource);
    return(0);
}                                       
#endif

/*----------------------------------------------------------------------*
                       rtp_sig_semaphore_wait_timed
 *----------------------------------------------------------------------*/
int rtp_sig_semaphore_wait_timed (RTP_SEMAPHORE semHandle, long msecs)
{
    if (msecs == (-1))
    {
        OS_WaitCSema(&((RTP_SIGNAL_CONTEXT *)semHandle)->semaResource);
    }
    else
    {
        int result;
        result = OS_WaitCSema_Timed(&((RTP_SIGNAL_CONTEXT *)semHandle)->semaResource, (int)msecs);
        if(!result) return(-1);        
    }
    return (0);
}


/*----------------------------------------------------------------------*
                          rtp_sig_semaphore_clear
 *----------------------------------------------------------------------*/
void rtp_sig_semaphore_clear (RTP_SEMAPHORE semHandle)
{
    while (OS_GetCSemaValue(&((RTP_SIGNAL_CONTEXT *)semHandle)->semaResource))
    {
        OS_WaitCSema(&((RTP_SIGNAL_CONTEXT *)semHandle)->semaResource);
    }
}


/*----------------------------------------------------------------------*
                          rtp_sig_semaphore_wait
 *----------------------------------------------------------------------*/
int rtp_sig_semaphore_wait (RTP_SEMAPHORE semHandle)
{
    OS_WaitCSema(&((RTP_SIGNAL_CONTEXT *)semHandle)->semaResource);
    return (0);
}


/*----------------------------------------------------------------------*
                         rtp_sig_semaphore_signal
 *----------------------------------------------------------------------*/
void rtp_sig_semaphore_signal (RTP_SEMAPHORE semHandle)
{
    OS_SignalCSema(&((RTP_SIGNAL_CONTEXT *)semHandle)->semaResource);
}


/*----------------------------------------------------------------------*
                       rtp_sig_semaphore_signal_isr
 *----------------------------------------------------------------------*/
void rtp_sig_semaphore_signal_isr (RTP_SEMAPHORE semHandle)
{
    OS_SignalCSema(&((RTP_SIGNAL_CONTEXT *)semHandle)->semaResource);
}


/************************************************************************
 * Mutex [lock] services                                                *
 ************************************************************************/

/*----------------------------------------------------------------------*
                         rtp_sig_mutex_init
 *----------------------------------------------------------------------*/
int  rtp_sig_mutex_init (int nMutex, RTP_MUTEX_CONTEXTP ctxBlock)
{
    int i;
    RTP_MUTEX_CONTEXT  *rtp_mutex_context = (RTP_MUTEX_CONTEXT  *)ctxBlock;
    for (i = 0; i < nMutex; i++)
    {
#ifdef RTP_INC_DYNAMIC_MUTEX
    	rtp_mutex_context->func.AllocFn = 0;
    	rtp_mutex_context->func.FreeFn = 0;
#endif
        _rtp_addSingleList((void **)&rtp_mutex_free, (_SingleList *)rtp_mutex_context);
        rtp_mutex_context++;
    }
    return(0);
}	

/*----------------------------------------------------------------------*
                         rtp_sig_mutex_shutdown
 *----------------------------------------------------------------------*/
void rtp_sig_mutex_shutdown (void)
{
    rtp_not_yet_implemented();
}
/*----------------------------------------------------------------------*
                          rtp_sig_mutex_alloc
 *----------------------------------------------------------------------*/
int rtp_sig_mutex_alloc (RTP_MUTEX *newMutex, const char *name)
{
    if((*newMutex = (RTP_MUTEX)(_rtp_removeSingleList((void *)&rtp_mutex_free))) != 0)
    {
        ((RTP_MUTEX_CONTEXT *)*newMutex)->name = (char *)name;
        OS_CREATERSEMA(&((RTP_MUTEX_CONTEXT *)*newMutex)->mutexResource);
        return(0);
    }

    else
        return(-1);
}

/*----------------------------------------------------------------------*
                          rtp_sig_mutex_free
 *----------------------------------------------------------------------*/
void rtp_sig_mutex_free (RTP_MUTEX mutexHandle)
{
#ifdef RTP_INC_DYNAMIC_MUTEX
    if(((RTP_MUTEX_CONTEXT *)mutexHandle)->func.FreeFn)
       ((RTP_MUTEX_CONTEXT *)mutexHandle)->func.FreeFn((void *)mutexHandle);
    else
#endif
    _rtp_addSingleList((void **)&rtp_mutex_free, (_SingleList *)mutexHandle);
}


/*----------------------------------------------------------------------*
                         rtp_sig_mutex_alloc_dynamic
 *----------------------------------------------------------------------*/
#ifdef RTP_INC_DYNAMIC_MUTEX
int  rtp_sig_mutex_alloc_dynamic (RTP_MUTEX *newMutex, const char *name, 
                                       RTP_MUTEX_ALLOC *funcptr)
{
    if((*newMutex = (RTP_MUTEX)(funcptr->AllocFn(sizeof(RTP_MUTEX_CONTEXT)))) == 0)
        return(-1);
    ((RTP_MUTEX_CONTEXT *)*newMutex)->name = (char *)name;
    ((RTP_MUTEX_CONTEXT *)*newMutex)->func.AllocFn = funcptr->AllocFn;
    ((RTP_MUTEX_CONTEXT *)*newMutex)->func.FreeFn = funcptr->FreeFn;
    OS_CREATERSEMA(&((RTP_MUTEX_CONTEXT *)*newMutex)->mutexResource);
    return(0);
}                                       
#endif


/*----------------------------------------------------------------------*
                       rtp_sig_mutex_claim_timed
 *----------------------------------------------------------------------*/
int rtp_sig_mutex_claim_timed (RTP_MUTEX mutexHandle, long msecs)
{
    rtp_not_yet_implemented();
    return (-1);
}

/*----------------------------------------------------------------------*
                          rtp_sig_mutex_claim
 *----------------------------------------------------------------------*/
int rtp_sig_mutex_claim (RTP_MUTEX mutexHandle)
{
    OS_Use(&((RTP_MUTEX_CONTEXT *)mutexHandle)->mutexResource);
    return (0);
}

/*----------------------------------------------------------------------*
                          rtp_sig_mutex_release
 *----------------------------------------------------------------------*/
void rtp_sig_mutex_release (RTP_MUTEX mutexHandle)
{
    OS_Unuse(&((RTP_MUTEX_CONTEXT *)mutexHandle)->mutexResource);
}

/************************************************************************
* Utility Functions
************************************************************************/

static void _rtp_addSingleList(void **List, _SingleList *Member)
{
    Member->next = *List;
    *List = Member;
}
static _SingleList *_rtp_removeSingleList(void **List)
{
    if(*List)
    {
    	_SingleList *temp = ((_SingleList *)(List))->next;
    	*List = temp->next;
    	return(temp);
    }
    else return(0);
}

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */