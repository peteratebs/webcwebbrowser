 /*
 | RTPSEMDB.C - Runtime Platform Semaphore (Signaling) System Services
 |
 |   UNIVERSAL CODE - DO NOT CHANGE (except for debug defines)
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: shane $
 |  $Date: 2004/10/26 15:15:24 $
 |  $Name:  $
 |  $Revision: 1.8 $
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
#ifdef RTP_TRACK_LOCAL_SEMAPHORES

 #ifdef RTP_SEM_DEBUG_STDIO
  #include <stdio.h>
  #include <stdlib.h>
  #define RTP_SEM_DEBUG_FOPEN   fopen
  #define RTP_SEM_DEBUG_FPRINTF fprintf
  #define RTP_SEM_DEBUG_FCLOSE  fclose
 #endif
 
#ifndef RTP_DEBUG
#define RTP_DEBUG
#endif

#include "rtpdebug.h"
#include "rtpscnv.h"
#include "rtpstr.h"
/************************************************************************
* Defines
************************************************************************/

/************************************************************************
* Types
************************************************************************/

/************************************************************************
* Data
************************************************************************/
unsigned  rtpSemaphoreDebugInitialized = 0;
RTP_MUTEX rtpSemaphoreDebugLock;

/************************************************************************
* Structures
************************************************************************/
typedef struct __rtp_semaphore_header
{
	struct __rtp_semaphore_header*  prev;       /* pointer to prev semaphore in the chain     */
	struct __rtp_semaphore_header*  next;       /* pointer to next semaphore in the chain     */
	unsigned long                   seq;        /* sequence number                            */
	long                            wait;       /* time to wait in msecs (-1 for infinite)    */
	const char*                     state;      /* current state                              */
	char                            file[256];
	long                            line;
	RTP_SEMAPHORE                   sem;
} RTPSemaphoreHeader;

/************************************************************************
* Data
************************************************************************/
RTPSemaphoreHeader  gSemaphorePool[RTP_SEMAPHORE_NUM_TO_TRACK];
RTPSemaphoreHeader* gpSemaphoreFreeList = 0;
unsigned long giNumSemaphoresTracking   = 0;
unsigned long giAllocSemaphoreFail      = 0;
unsigned long giAllocSemaphoreHeadFail  = 0;
unsigned long giFreeSemaphoreHeadFail   = 0;
unsigned long guWaitSequence            = 0;
const char*   gcSemaphoreAllocated      = "ALLOCATED     ";
const char*   gcSemaphoreWaitTimed      = "WAIT_TIMED    ";
const char*   gcSemaphoreWait           = "WAIT          ";
const char*   gcSemaphoreSignaled       = "SIGNALED      ";
const char*   gcSemaphoreSignaledISR    = "SIGNALED_ISR  ";
const char*   gcSemaphoreCleared        = "CLEARED       ";
const char*   gcSemaphoreFreed          = "FREED         ";

/* ----------------------------------- */
/* gpSemaphoreList is a pointer to the */
/* most recently added semaphore.      */
/* Follow the chain through the next   */
/* pointers to get to the other        */
/* semaphores.                         */
/* ----------------------------------- */
RTPSemaphoreHeader* gpSemaphoreList = 0;

/************************************************************************
* Semaphore Debug Utility Function Prototypes
************************************************************************/
static unsigned __rtp_InitSemaphoreDebug      (void);
static int      __rtp_AddSemaphoreHeader      (RTPSemaphoreHeader** pNewSemaphore, const char* file, long line);
static int      __rtp_FreeSemaphoreHeader     (RTPSemaphoreHeader* pFreedSemaphore, const char* file, long line);
static unsigned __rtp_TrackingSemaphoreHeader (RTPSemaphoreHeader* pSemaphore);

/************************************************************************
* Semaphore Debug Function Bodies
************************************************************************/

/*----------------------------------------------------------------------*
                              
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    
    
    @precondition 

    @return 
 */
int _rtp_debug_sig_semaphore_alloc (
		RTP_SEMAPHORE *newSem, 
		const char *name, 
		const char *file, 
		long line_num
	)
{
RTPSemaphoreHeader* pNewHeader = 0;
RTP_SEMAPHORE nativeSem;
int result;

	result = _rtp_sig_semaphore_alloc(&nativeSem, name);
	if (result < 0)
	{
		giAllocSemaphoreFail++;
		return (result);
	}
	
	if (__rtp_AddSemaphoreHeader(&pNewHeader, file, line_num) < 0)
	{
		/* ----------------------------------- */
		/*  No more storage locations left to  */
		/*  keep track of this semaphore. Call */
		/*  _rtp_sig_semaphore_alloc with out  */
		/*  debug.                             */
		/*                                     */
		/* THIS SEMAPHORE WILL NOT BE TRACKED. */
		/* ----------------------------------- */
		giAllocSemaphoreHeadFail++;
		*newSem = nativeSem;
		return (result);
	}
	
	pNewHeader->sem = nativeSem;
	*newSem = (RTP_SEMAPHORE)pNewHeader;
	
	return (result);
}

/*----------------------------------------------------------------------*
                              
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    
    
    @precondition 

    @return 
 */
void _rtp_debug_sig_semaphore_free (
		RTP_SEMAPHORE semHandle,
		const char *file,
		long line_num
	)
{
RTPSemaphoreHeader* pFreedHeader = (RTPSemaphoreHeader*)semHandle;

	if (__rtp_FreeSemaphoreHeader(pFreedHeader, file, line_num) < 0)
	{
		/* ----------------------------------- */
		/*  This semHandle was not found in    */
		/*  the track list. Simply free and    */
		/*  return.                            */
		/* ----------------------------------- */
		giFreeSemaphoreHeadFail++;
		_rtp_sig_semaphore_free(semHandle);
		return;
	}
	
	_rtp_sig_semaphore_free(pFreedHeader->sem);
}

/*----------------------------------------------------------------------*
                              
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    
    
    @precondition 

    @return 
 */
int _rtp_debug_sig_semaphore_wait_timed (
		RTP_SEMAPHORE semHandle,
		long msecs,
		const char *file,
		long line_num
	)
{
RTPSemaphoreHeader* pSemaphoreHead = (RTPSemaphoreHeader*)semHandle;
int result;

	if (!__rtp_TrackingSemaphoreHeader(pSemaphoreHead))
	{
		/* ----------------------------------- */
		/*       This semaphore does not       */
		/*       exist on the track list.      */
		/* ----------------------------------- */
		return (_rtp_sig_semaphore_wait_timed(semHandle, msecs));
	}

	if (__rtp_InitSemaphoreDebug())
	{
		_rtp_sig_mutex_claim(rtpSemaphoreDebugLock);
		
		pSemaphoreHead->seq   = guWaitSequence++;
		pSemaphoreHead->wait  = msecs;
		pSemaphoreHead->state = gcSemaphoreWaitTimed;
		rtp_strcpy(pSemaphoreHead->file, file);
		pSemaphoreHead->line  = line_num;

		_rtp_sig_mutex_release(rtpSemaphoreDebugLock);
		
		result = _rtp_sig_semaphore_wait_timed(pSemaphoreHead->sem, msecs);
	}
	return (result);
}

/*----------------------------------------------------------------------*
                              
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    
    
    @precondition 

    @return 
 */
void _rtp_debug_sig_semaphore_clear (
		RTP_SEMAPHORE semHandle,
		const char *file,
		long line_num
	)
{
RTPSemaphoreHeader* pSemaphoreHead = (RTPSemaphoreHeader*)semHandle;

	if (!__rtp_TrackingSemaphoreHeader(pSemaphoreHead))
	{
		/* ----------------------------------- */
		/*       This semaphore does not       */
		/*       exist on the track list.      */
		/* ----------------------------------- */
		_rtp_sig_semaphore_clear(semHandle);
		return;
	}

	if (__rtp_InitSemaphoreDebug())
	{
		_rtp_sig_mutex_claim(rtpSemaphoreDebugLock);
		
		pSemaphoreHead->seq   = 0;
		pSemaphoreHead->wait  = 0;
		pSemaphoreHead->state = gcSemaphoreCleared;
		rtp_strcpy(pSemaphoreHead->file, file);
		pSemaphoreHead->line  = line_num;
		
		_rtp_sig_mutex_release(rtpSemaphoreDebugLock);
		
		_rtp_sig_semaphore_clear(pSemaphoreHead->sem);
	}
}

/*----------------------------------------------------------------------*
                              
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    
    
    @precondition 

    @return 
 */
int _rtp_debug_sig_semaphore_wait (
		RTP_SEMAPHORE semHandle,
		const char *file,
		long line_num
	)
{
RTPSemaphoreHeader* pSemaphoreHead = (RTPSemaphoreHeader*)semHandle;
int result;

	if (!__rtp_TrackingSemaphoreHeader(pSemaphoreHead))
	{
		/* ----------------------------------- */
		/*       This semaphore does not       */
		/*       exist on the track list.      */
		/* ----------------------------------- */
		return (_rtp_sig_semaphore_wait(semHandle));
	}

	if (__rtp_InitSemaphoreDebug())
	{
		_rtp_sig_mutex_claim(rtpSemaphoreDebugLock);
		
		pSemaphoreHead->seq   = guWaitSequence++;
		pSemaphoreHead->wait  = -1;
		pSemaphoreHead->state = gcSemaphoreWait;
		rtp_strcpy(pSemaphoreHead->file, file);
		pSemaphoreHead->line  = line_num;
		
		_rtp_sig_mutex_release(rtpSemaphoreDebugLock);
		
		result = _rtp_sig_semaphore_wait(pSemaphoreHead->sem);
	}
	return (result);
}

/*----------------------------------------------------------------------*
                              
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    
    
    @precondition 

    @return 
 */
void _rtp_debug_sig_semaphore_signal (
		RTP_SEMAPHORE semHandle,
		const char *file,
		long line_num
	)
{
RTPSemaphoreHeader* pSemaphoreHead = (RTPSemaphoreHeader*)semHandle;

	if (!__rtp_TrackingSemaphoreHeader(pSemaphoreHead))
	{
		/* ----------------------------------- */
		/*       This semaphore does not       */
		/*       exist on the track list.      */
		/* ----------------------------------- */
		_rtp_sig_semaphore_signal(semHandle);
		return;
	}

	if (__rtp_InitSemaphoreDebug())
	{
		_rtp_sig_mutex_claim(rtpSemaphoreDebugLock);
		
		pSemaphoreHead->seq   = 0;
		pSemaphoreHead->wait  = 0;
		pSemaphoreHead->state = gcSemaphoreSignaled;
		rtp_strcpy(pSemaphoreHead->file, file);
		pSemaphoreHead->line  = line_num;
		
		_rtp_sig_mutex_release(rtpSemaphoreDebugLock);
		
		_rtp_sig_semaphore_signal(pSemaphoreHead->sem);
	}
}

/*----------------------------------------------------------------------*
                              
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    
    
    @precondition 

    @return 
 */
void _rtp_debug_sig_semaphore_signal_isr (
		RTP_SEMAPHORE semHandle,
		const char *file,
		long line_num
	)
{
RTPSemaphoreHeader* pSemaphoreHead = (RTPSemaphoreHeader*)semHandle;

	if (!__rtp_TrackingSemaphoreHeader(pSemaphoreHead))
	{
		/* ----------------------------------- */
		/*       This semaphore does not       */
		/*       exist on the track list.      */
		/* ----------------------------------- */
		_rtp_sig_semaphore_signal(semHandle);
		return;
	}

	if (__rtp_InitSemaphoreDebug())
	{
		_rtp_sig_mutex_claim(rtpSemaphoreDebugLock);
		
		pSemaphoreHead->seq   = 0;
		pSemaphoreHead->wait  = 0;
		pSemaphoreHead->state = gcSemaphoreSignaledISR;
		rtp_strcpy(pSemaphoreHead->file, file);
		pSemaphoreHead->line  = line_num;
		
		_rtp_sig_mutex_release(rtpSemaphoreDebugLock);
		
		_rtp_sig_semaphore_signal(pSemaphoreHead->sem);
	}
}


/*----------------------------------------------------------------------*
                              
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    
    
    @precondition

    @return 
 */
void _rtp_debug_sig_semaphore_print_log (void)
{
RTPSemaphoreHeader* currentSemaphore;

	if (__rtp_InitSemaphoreDebug())
	{
		_rtp_sig_mutex_claim(rtpSemaphoreDebugLock);
		
		RTP_DEBUG_OUTPUT_STR("Tracking:        ");
		RTP_DEBUG_OUTPUT_INT(giNumSemaphoresTracking);
		RTP_DEBUG_OUTPUT_STR("\n");
		
		RTP_DEBUG_OUTPUT_STR("Alloc Fail:      ");
		RTP_DEBUG_OUTPUT_INT(giAllocSemaphoreFail);
		RTP_DEBUG_OUTPUT_STR("\n");
		
		RTP_DEBUG_OUTPUT_STR("Alloc Head Fail: ");
		RTP_DEBUG_OUTPUT_INT(giAllocSemaphoreHeadFail);
		RTP_DEBUG_OUTPUT_STR("\n");
		
		RTP_DEBUG_OUTPUT_STR("Free Head Fail:  ");
		RTP_DEBUG_OUTPUT_INT(giFreeSemaphoreHeadFail);
		RTP_DEBUG_OUTPUT_STR("\n");
		
		if (gpSemaphoreList)
		{
			currentSemaphore = gpSemaphoreList;
			
			do {

#ifdef EBS_INTERNAL_TEST_ONLY
				if (currentSemaphore->wait || (currentSemaphore->wait == -1))
				{
#endif

				RTP_DEBUG_OUTPUT_STR("\n");
				RTP_DEBUG_OUTPUT_STR("Semaphore ID:    ");
				RTP_DEBUG_OUTPUT_INT((long)currentSemaphore);
				RTP_DEBUG_OUTPUT_STR("\n");
				
				RTP_DEBUG_OUTPUT_STR("Wait:            ");
				RTP_DEBUG_OUTPUT_INT(currentSemaphore->wait);
				RTP_DEBUG_OUTPUT_STR("\n");
				
				RTP_DEBUG_OUTPUT_STR("State:           ");
				RTP_DEBUG_OUTPUT_STR((char*)currentSemaphore->state);
				RTP_DEBUG_OUTPUT_STR("\n");
				
				RTP_DEBUG_OUTPUT_STR("File:            ");
				RTP_DEBUG_OUTPUT_STR(currentSemaphore->file);
				RTP_DEBUG_OUTPUT_STR("\n");
				
				RTP_DEBUG_OUTPUT_STR("Line:            ");
				RTP_DEBUG_OUTPUT_INT(currentSemaphore->line);
				RTP_DEBUG_OUTPUT_STR("\n");

#ifdef EBS_INTERNAL_TEST_ONLY
				}
#endif

				currentSemaphore = currentSemaphore->next;
			} while (currentSemaphore);
		}
		
		
		_rtp_sig_mutex_release(rtpSemaphoreDebugLock);
	}
}


/*----------------------------------------------------------------------*
                              
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    
    
    @precondition

    @return 
 */
void _rtp_debug_sig_semaphore_print_stdio (
		const char* logFile,
		const char* mode
	)
{
#ifdef RTP_SEM_DEBUG_STDIO
RTPSemaphoreHeader* currentSemaphore;
FILE* fp;

	if (__rtp_InitSemaphoreDebug())
	{	
		fp = RTP_SEM_DEBUG_FOPEN(logFile, mode);
		if (fp)
		{
			long n;
			
			_rtp_sig_mutex_claim(rtpSemaphoreDebugLock);
			
			RTP_SEM_DEBUG_FPRINTF(fp, "\"Tracking\",%d\n", giNumSemaphoresTracking);
			RTP_SEM_DEBUG_FPRINTF(fp, "\"Alloc Fail\",%d\n", giAllocSemaphoreFail);
			RTP_SEM_DEBUG_FPRINTF(fp, "\"Alloc Head Fail\",%d\n", giAllocSemaphoreHeadFail);
			RTP_SEM_DEBUG_FPRINTF(fp, "\"Free Head Fail\",%d\n\n", giFreeSemaphoreHeadFail);
			
			if (gpSemaphoreList)
			{
				currentSemaphore = gpSemaphoreList;
				RTP_SEM_DEBUG_FPRINTF(fp, "\"Semaphore ID\",\"Wait\",\"State\",\"File\",\"Line\"\n");
				
				do {
	
#ifdef EBS_INTERNAL_TEST_ONLY
					if (currentSemaphore->wait || (currentSemaphore->wait == -1))
					{
#endif
					RTP_SEM_DEBUG_FPRINTF(fp, "\"0x%08\",\"%d\",\"%s\",\"%s\",\"%d\"\n",
							currentSemaphore,
							currentSemaphore->wait,
							currentSemaphore->state,
							currentSemaphore->file,
							currentSemaphore->line
						);
#ifdef EBS_INTERNAL_TEST_ONLY
					}
#endif
	
					currentSemaphore = currentSemaphore->next;
				} while (currentSemaphore);
			}
			
			_rtp_sig_mutex_release(rtpSemaphoreDebugLock);
			
			RTP_SEM_DEBUG_FCLOSE(fp);
		}
	}
#else
	RTP_DEBUG_OUTPUT_STR("Must define RTP_SEM_DEBUG_STDIO to make use of this call.\n");
	RTP_DEBUG_OUTPUT_STR("Call being rerouted to: rtp_debug_sig_semaphore_print_log\n");
	RTP_DEBUG_OUTPUT_STR("\n");

	_rtp_debug_sig_semaphore_print_log( );
#endif /* RTP_SEM_DEBUG_STDIO */
}


/************************************************************************
* Semaphore Debug Utility Function Bodies
************************************************************************/
static unsigned __rtp_InitSemaphoreDebug (void)
{
	if (!rtpSemaphoreDebugInitialized)
	{
		if (_rtp_sig_mutex_alloc(&rtpSemaphoreDebugLock, 0) >= 0)
		{
			int i;
			rtp_memset(gSemaphorePool, 0, sizeof(struct __rtp_semaphore_header) * RTP_SEMAPHORE_NUM_TO_TRACK);
			
			gSemaphorePool[0].prev = 0;
			gSemaphorePool[0].next = &gSemaphorePool[1%RTP_SEMAPHORE_NUM_TO_TRACK];
			for (i = 1; i < RTP_SEMAPHORE_NUM_TO_TRACK; i++)
			{
				gSemaphorePool[i].next = &gSemaphorePool[(i+1)%RTP_SEMAPHORE_NUM_TO_TRACK];
				gSemaphorePool[i].prev = &gSemaphorePool[(i-1)%RTP_SEMAPHORE_NUM_TO_TRACK];
			}
			gSemaphorePool[i-1].next = 0;
			
			gpSemaphoreFreeList = gSemaphorePool;
			rtpSemaphoreDebugInitialized = 1;
		}
	}

	return (rtpSemaphoreDebugInitialized);
}

static int __rtp_AddSemaphoreHeader (RTPSemaphoreHeader** pNewSemaphore, const char* file, long line)
{
	if (__rtp_TrackingSemaphoreHeader(*pNewSemaphore))
	{
		/* ----------------------------------- */
		/*      This semaphore is already      */
		/*           being tracked.            */
		/* ----------------------------------- */
		return (-1);
	}

	if (__rtp_InitSemaphoreDebug())
	{
		_rtp_sig_mutex_claim(rtpSemaphoreDebugLock);
		
		if (!gpSemaphoreFreeList)
		{
			_rtp_sig_mutex_release(rtpSemaphoreDebugLock);
			return (-1);
		}
		
		/* ----------------------------------- */
		/*    Grab a semaphore head storage    */
		/*     location from the free list.    */
		/* ----------------------------------- */
		*pNewSemaphore = gpSemaphoreFreeList;
		gpSemaphoreFreeList = gpSemaphoreFreeList->next;
		
		if (gpSemaphoreFreeList)
		{
			gpSemaphoreFreeList->prev = 0;
		}
		
		(*pNewSemaphore)->seq   = 0;
		(*pNewSemaphore)->wait  = 0;
		(*pNewSemaphore)->state = gcSemaphoreAllocated;
		rtp_strcpy((*pNewSemaphore)->file, file);
		(*pNewSemaphore)->line  = line;

		/* ----------------------------------- */
		/*       Link *pNewSemaphore onto      */
		/*         the top of the list.        */
		/* ----------------------------------- */
		(*pNewSemaphore)->prev = 0;
		(*pNewSemaphore)->next = gpSemaphoreList;
		
		if (gpSemaphoreList)
		{
			gpSemaphoreList->prev = *pNewSemaphore;
		}
		gpSemaphoreList = *pNewSemaphore;
		giNumSemaphoresTracking++;

		_rtp_sig_mutex_release(rtpSemaphoreDebugLock);
		
		return (0);
	}
	return (-1);
}

static int __rtp_FreeSemaphoreHeader (RTPSemaphoreHeader* pFreedSemaphore, const char* file, long line)
{
	if (!__rtp_TrackingSemaphoreHeader(pFreedSemaphore))
	{
		/* ----------------------------------- */
		/*       This semaphore does not       */
		/*       exist on the track list.      */
		/* ----------------------------------- */
		return (-1);
	}
	
	if (__rtp_InitSemaphoreDebug())
	{
		_rtp_sig_mutex_claim(rtpSemaphoreDebugLock);
		
		/* ----------------------------------- */
		/*     Remove pFreedSemaphore from     */
		/*            the track list.          */
		/* ----------------------------------- */
		if (pFreedSemaphore->prev)
		{
			pFreedSemaphore->prev->next = pFreedSemaphore->next;
		}
		else
		{
			/* ----------------------------------- */
			/*  Freeing the head, must reasign     */
			/*  and clear prev pointer to now      */
			/*  old head.                          */
			/* ----------------------------------- */
			if (gpSemaphoreList->next)
			{
				gpSemaphoreList = gpSemaphoreList->next;
				gpSemaphoreList->prev = 0;
			}
			else
			{
				gpSemaphoreList = 0;
			}
		}
		
		if (pFreedSemaphore->next)
		{
			pFreedSemaphore->next->prev = pFreedSemaphore->prev;
		}

		/* ----------------------------------- */
		/*      Link pFreedSemaphore onto      */
		/*      the top of the free list.      */
		/* ----------------------------------- */
		pFreedSemaphore->prev = 0;
		pFreedSemaphore->next = gpSemaphoreFreeList;
		
		if (gpSemaphoreFreeList)
		{
			gpSemaphoreFreeList->prev = pFreedSemaphore;
		}
		gpSemaphoreFreeList = pFreedSemaphore;
		giNumSemaphoresTracking--;

		pFreedSemaphore->seq   = 0;
		pFreedSemaphore->wait  = 0;
		pFreedSemaphore->state = gcSemaphoreFreed;
		rtp_strcpy(pFreedSemaphore->file, file);
		pFreedSemaphore->line  = line;

		_rtp_sig_mutex_release(rtpSemaphoreDebugLock);
		
		return (0);
	}
	return (-1);
}

static unsigned __rtp_TrackingSemaphoreHeader (RTPSemaphoreHeader* pSemaphore)
{
RTPSemaphoreHeader* pNextSemaphore;
	
	if (__rtp_InitSemaphoreDebug())
	{
		_rtp_sig_mutex_claim(rtpSemaphoreDebugLock);
		
		if (gpSemaphoreList)
		{
			pNextSemaphore = gpSemaphoreList;
			while (pNextSemaphore)
			{
				if (pSemaphore == pNextSemaphore)
				{
					_rtp_sig_mutex_release(rtpSemaphoreDebugLock);
					return (1);
				}
				pNextSemaphore = pNextSemaphore->next;
			}
		}

		_rtp_sig_mutex_release(rtpSemaphoreDebugLock);
	}
	
	/* ----------------------------------- */
	/*       This semaphore does not       */
	/*       exist on the track list.      */
	/* ----------------------------------- */
	return (0);
}
#endif /* RTP_TRACK_LOCAL_SEMAPHORES */



/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
