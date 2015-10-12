 /*
 | RTPMTXDB.C - Runtime Platform Mutex Locking System Services
 |
 |   UNIVERSAL CODE - DO NOT CHANGE (except for debug defines)
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: sarah $
 |  $Date: 2005/05/14 02:43:29 $
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
#ifdef RTP_TRACK_LOCAL_MUTEXES

 #ifdef RTP_MTX_DEBUG_STDIO
  #include <stdio.h>
  #include <stdlib.h>
  #define RTP_MTX_DEBUG_FOPEN   fopen
  #define RTP_MTX_DEBUG_FPRINTF fprintf
  #define RTP_MTX_DEBUG_FCLOSE  fclose
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
unsigned  rtpMutexDebugInitialized = 0;
RTP_MUTEX rtpMutexDebugLock;

/************************************************************************
* Structures
************************************************************************/
typedef struct __rtp_mutex_header
{
	struct __rtp_mutex_header*  prev;       /* pointer to prev mutex in the chain     */
	struct __rtp_mutex_header*  next;       /* pointer to next mutex in the chain     */
	unsigned long               seq;        /* sequence number                            */
	long                        wait;       /* time to wait in msecs (-1 for infinite)    */
	const char*                 state;      /* current state                              */
	char                        file[256];
	long                        line;
	RTP_MUTEX                   mtx;
} RTPMutexHeader;

/************************************************************************
* Data
************************************************************************/
RTPMutexHeader  gMutexPool[RTP_MUTEX_NUM_TO_TRACK];
RTPMutexHeader* gpMutexFreeList     = 0;
unsigned long giNumMutexsTracking   = 0;
unsigned long giAllocMutexFail      = 0;
unsigned long giAllocMutexHeadFail  = 0;
unsigned long giFreeMutexHeadFail   = 0;
unsigned long guClaimSequence       = 0;
const char*   gcMutexAllocated      = "ALLOCATED     ";
const char*   gcMutexWaiting        = "WAITING       ";
const char*   gcMutexTimeClaimed    = "TIME_CLAIMED  ";
const char*   gcMutexClaimed        = "CLAIMED       ";
const char*   gcMutexClaimFailed    = "CLAIM_FAILED  ";
const char*   gcMutexReleased       = "RELEASED      ";
const char*   gcMutexFreed          = "FREED         ";

/* ----------------------------------- */
/* gpMutexList is a pointer to the     */
/* most recently added mutex. Follow   */
/* the chain through the next pointers */
/* to get to the other mutexes.        */
/* ----------------------------------- */
RTPMutexHeader* gpMutexList = 0;

/************************************************************************
* Mutex Debug Utility Function Prototypes
************************************************************************/
static unsigned __rtp_InitMutexDebug      (void);
static int      __rtp_AddMutexHeader      (RTPMutexHeader** pNewMutex, const char* file, long line);
static int      __rtp_FreeMutexHeader     (RTPMutexHeader* pFreedMutex, const char* file, long line);
static unsigned __rtp_TrackingMutexHeader (RTPMutexHeader* pMutex);

/************************************************************************
* Mutex Debug Function Bodies
************************************************************************/

/*----------------------------------------------------------------------*
                              
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    
    
    @precondition 

    @return 
 */
int _rtp_debug_sig_mutex_alloc (
		RTP_MUTEX *newMutex, 
		const char *name, 
		const char *file, 
		long line_num
	)
{
RTPMutexHeader* pNewHeader = 0;
RTP_MUTEX nativeMutex;
int result;

	result = _rtp_sig_mutex_alloc(&nativeMutex, name);
	if (result < 0)
	{
		giAllocMutexFail++;
		return (result);
	}
	
	if (__rtp_AddMutexHeader(&pNewHeader, file, line_num) < 0)
	{
		/* ----------------------------------- */
		/*  No more storage locations left to  */
		/*  keep track of this mutex. Call     */
		/*  _rtp_sig_mutex_alloc with out      */
		/*  debug.                             */
		/*                                     */
		/*   THIS MUTEX WILL NOT BE TRACKED.   */
		/* ----------------------------------- */
		giAllocMutexHeadFail++;
		*newMutex = nativeMutex;
		return (result);
	}
	
	pNewHeader->mtx = nativeMutex;
	*newMutex = (RTP_MUTEX)pNewHeader;
	
	return (result);
}

/*----------------------------------------------------------------------*
                              
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    
    
    @precondition 

    @return 
 */
void _rtp_debug_sig_mutex_free (
		RTP_MUTEX mtxHandle,
		const char *file,
		long line_num
	)
{
RTPMutexHeader* pFreedHeader = (RTPMutexHeader*)mtxHandle;

	if (__rtp_FreeMutexHeader(pFreedHeader, file, line_num) < 0)
	{
		/* ----------------------------------- */
		/*  This mtxHandle was not found in    */
		/*  the track list. Simply free and    */
		/*  return.                            */
		/* ----------------------------------- */
		giFreeMutexHeadFail++;
		_rtp_sig_mutex_free(mtxHandle);
		return;
	}
	
	_rtp_sig_mutex_free(pFreedHeader->mtx);
}

/*----------------------------------------------------------------------*
                              
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    
    
    @precondition 

    @return 
 */
int _rtp_debug_sig_mutex_claim_timed (
		RTP_MUTEX mtxHandle,
		long msecs,
		const char *file,
		long line_num
	)
{
RTPMutexHeader* pMutexHead = (RTPMutexHeader*)mtxHandle;
int result;

	if (!__rtp_TrackingMutexHeader(pMutexHead))
	{
		/* ----------------------------------- */
		/*         This mutex does not         */
		/*       exist on the track list.      */
		/* ----------------------------------- */
		return (_rtp_sig_mutex_claim_timed(mtxHandle, msecs));
	}

	if (__rtp_InitMutexDebug())
	{
		_rtp_sig_mutex_claim(rtpMutexDebugLock);
		
		pMutexHead->seq   = guClaimSequence++;
		pMutexHead->wait  = msecs;
		pMutexHead->state = gcMutexWaiting;
		rtp_strcpy(pMutexHead->file, file);
		pMutexHead->line  = line_num;

		_rtp_sig_mutex_release(rtpMutexDebugLock);
		
		result = _rtp_sig_mutex_claim_timed(pMutexHead->mtx, msecs);

		_rtp_sig_mutex_claim(rtpMutexDebugLock);
		
		if (result < 0)
		{
			pMutexHead->state = gcMutexClaimFailed;
		}
		else
		{
			pMutexHead->state = gcMutexTimeClaimed;
		}
		
		_rtp_sig_mutex_release(rtpMutexDebugLock);
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
int _rtp_debug_sig_mutex_claim (
		RTP_MUTEX mtxHandle,
		const char *file,
		long line_num
	)
{
RTPMutexHeader* pMutexHead = (RTPMutexHeader*)mtxHandle;
int result;

	if (!__rtp_TrackingMutexHeader(pMutexHead))
	{
		/* ----------------------------------- */
		/*         This mutex does not         */
		/*       exist on the track list.      */
		/* ----------------------------------- */
		return (_rtp_sig_mutex_claim(mtxHandle));
	}

	if (__rtp_InitMutexDebug())
	{
		_rtp_sig_mutex_claim(rtpMutexDebugLock);
		
		pMutexHead->seq   = guClaimSequence++;
		pMutexHead->wait  = -1;
		pMutexHead->state = gcMutexWaiting;
		rtp_strcpy(pMutexHead->file, file);
		pMutexHead->line  = line_num;
		
		_rtp_sig_mutex_release(rtpMutexDebugLock);
		
		result = _rtp_sig_mutex_claim(pMutexHead->mtx);
		
		_rtp_sig_mutex_claim(rtpMutexDebugLock);
		
		if (result < 0)
		{
			pMutexHead->state = gcMutexClaimFailed;
		}
		else
		{
			pMutexHead->state = gcMutexClaimed;
		}
		
		_rtp_sig_mutex_release(rtpMutexDebugLock);
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
void _rtp_debug_sig_mutex_release (
		RTP_MUTEX mtxHandle,
		const char *file,
		long line_num
	)
{
RTPMutexHeader* pMutexHead = (RTPMutexHeader*)mtxHandle;

	if (!__rtp_TrackingMutexHeader(pMutexHead))
	{
		/* ----------------------------------- */
		/*         This mutex does not         */
		/*       exist on the track list.      */
		/* ----------------------------------- */
		_rtp_sig_mutex_release(mtxHandle);
		return;
	}

	if (__rtp_InitMutexDebug())
	{
		_rtp_sig_mutex_claim(rtpMutexDebugLock);
		
		pMutexHead->seq   = 0;
		pMutexHead->wait  = 0;
		pMutexHead->state = gcMutexReleased;
		rtp_strcpy(pMutexHead->file, file);
		pMutexHead->line  = line_num;
		
		_rtp_sig_mutex_release(rtpMutexDebugLock);
		
		_rtp_sig_mutex_release(pMutexHead->mtx);
	}
}

/*----------------------------------------------------------------------*
                              
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    
    
    @precondition

    @return 
 */
void _rtp_debug_sig_mutex_print_log (void)
{
RTPMutexHeader* currentMutex;

	if (__rtp_InitMutexDebug())
	{
		_rtp_sig_mutex_claim(rtpMutexDebugLock);

		RTP_DEBUG_OUTPUT_STR("Tracking:        ");
		RTP_DEBUG_OUTPUT_INT(giNumMutexsTracking);
		RTP_DEBUG_OUTPUT_STR("\n");
		
		RTP_DEBUG_OUTPUT_STR("Alloc Fail:      ");
		RTP_DEBUG_OUTPUT_INT(giAllocMutexFail);
		RTP_DEBUG_OUTPUT_STR("\n");
		
		RTP_DEBUG_OUTPUT_STR("Alloc Head Fail: ");
		RTP_DEBUG_OUTPUT_INT(giAllocMutexHeadFail);
		RTP_DEBUG_OUTPUT_STR("\n");
		
		RTP_DEBUG_OUTPUT_STR("Free Head Fail:  ");
		RTP_DEBUG_OUTPUT_INT(giFreeMutexHeadFail);
		RTP_DEBUG_OUTPUT_STR("\n");
		
		if (gpMutexList)
		{
			currentMutex = gpMutexList;
			
			do {

#ifdef EBS_INTERNAL_TEST_ONLY
				if (currentMutex->wait || (currentMutex->wait == -1))
				{
#endif

				RTP_DEBUG_OUTPUT_STR("\n");
				RTP_DEBUG_OUTPUT_STR("Mutex ID:    ");
				RTP_DEBUG_OUTPUT_INT((long)currentMutex);
				RTP_DEBUG_OUTPUT_STR("\n");
				
				RTP_DEBUG_OUTPUT_STR("Wait:            ");
				RTP_DEBUG_OUTPUT_INT(currentMutex->wait);
				RTP_DEBUG_OUTPUT_STR("\n");
				
				RTP_DEBUG_OUTPUT_STR("State:           ");
				RTP_DEBUG_OUTPUT_STR((char*)currentMutex->state);
				RTP_DEBUG_OUTPUT_STR("\n");
				
				RTP_DEBUG_OUTPUT_STR("File:            ");
				RTP_DEBUG_OUTPUT_STR(currentMutex->file);
				RTP_DEBUG_OUTPUT_STR("\n");
				
				RTP_DEBUG_OUTPUT_STR("Line:            ");
				RTP_DEBUG_OUTPUT_INT(currentMutex->line);
				RTP_DEBUG_OUTPUT_STR("\n");

#ifdef EBS_INTERNAL_TEST_ONLY
				}
#endif

				currentMutex = currentMutex->next;
			} while (currentMutex);
		}
		
		
		_rtp_sig_mutex_release(rtpMutexDebugLock);
	}
}


/*----------------------------------------------------------------------*
                              
 *----------------------------------------------------------------------*/
/** @memo   

    @doc    
    
    @precondition

    @return 
 */
void _rtp_debug_sig_mutex_print_stdio (
		const char* logFile,
		const char* mode
	)
{
#ifdef RTP_MTX_DEBUG_STDIO
RTPMutexHeader* currentMutex;
FILE* fp;

	if (__rtp_InitMutexDebug())
	{	
		fp = RTP_MTX_DEBUG_FOPEN(logFile, mode);
		if (fp)
		{
			long n;
			
			_rtp_sig_mutex_claim(rtpMutexDebugLock);
			
			RTP_MTX_DEBUG_FPRINTF(fp, "\"Tracking\",%d\n", giNumMutexsTracking);
			RTP_MTX_DEBUG_FPRINTF(fp, "\"Alloc Fail\",%d\n", giAllocMutexFail);
			RTP_MTX_DEBUG_FPRINTF(fp, "\"Alloc Head Fail\",%d\n", giAllocMutexHeadFail);
			RTP_MTX_DEBUG_FPRINTF(fp, "\"Free Head Fail\",%d\n\n", giFreeMutexHeadFail);
			
			if (gpMutexList)
			{
				currentMutex = gpMutexList;
				RTP_MTX_DEBUG_FPRINTF(fp, "\"Mutex ID\",\"Wait\",\"State\",\"File\",\"Line\"\n");
				
				do {
	
#ifdef EBS_INTERNAL_TEST_ONLY
					if (currentMutex->wait || (currentMutex->wait == -1))
					{
#endif
					RTP_MTX_DEBUG_FPRINTF(fp, "\"0x%08\",\"%d\",\"%s\",\"%s\",\"%d\"\n",
							currentMutex,
							currentMutex->wait,
							currentMutex->state,
							currentMutex->file,
							currentMutex->line
						);
#ifdef EBS_INTERNAL_TEST_ONLY
					}
#endif
	
					currentMutex = currentMutex->next;
				} while (currentMutex);
			}
			
			_rtp_sig_mutex_release(rtpMutexDebugLock);
			
			RTP_MTX_DEBUG_FCLOSE(fp);
		}
	}
#else
	RTP_DEBUG_OUTPUT_STR("Must define RTP_MTX_DEBUG_STDIO to make use of this call.\n");
	RTP_DEBUG_OUTPUT_STR("Call being rerouted to: rtp_debug_sig_mutex_print_log\n");
	RTP_DEBUG_OUTPUT_STR("\n");

	_rtp_debug_sig_mutex_print_log( );
#endif /* RTP_MTX_DEBUG_STDIO */
}


/************************************************************************
* Mutex Debug Utility Function Bodies
************************************************************************/
static unsigned __rtp_InitMutexDebug (void)
{
	if (!rtpMutexDebugInitialized)
	{
		if (_rtp_sig_mutex_alloc(&rtpMutexDebugLock, 0) >= 0)
		{
			int i;
			rtp_memset(gMutexPool, 0, sizeof(struct __rtp_mutex_header) * RTP_MUTEX_NUM_TO_TRACK);
			
			gMutexPool[0].prev = 0;
			gMutexPool[0].next = &gMutexPool[1%RTP_MUTEX_NUM_TO_TRACK];
			for (i = 1; i < RTP_MUTEX_NUM_TO_TRACK; i++)
			{
				gMutexPool[i].next = &gMutexPool[(i+1)%RTP_MUTEX_NUM_TO_TRACK];
				gMutexPool[i].prev = &gMutexPool[(i-1)%RTP_MUTEX_NUM_TO_TRACK];
			}
			gMutexPool[i-1].next = 0;
			
			gpMutexFreeList = gMutexPool;
			rtpMutexDebugInitialized = 1;
		}
	}

	return (rtpMutexDebugInitialized);
}

static int __rtp_AddMutexHeader (RTPMutexHeader** pNewMutex, const char* file, long line)
{
	if (__rtp_TrackingMutexHeader(*pNewMutex))
	{
		/* ----------------------------------- */
		/*       This mutex is already         */
		/*           being tracked.            */
		/* ----------------------------------- */
		return (-1);
	}

	if (__rtp_InitMutexDebug())
	{
		_rtp_sig_mutex_claim(rtpMutexDebugLock);
		
		if (!gpMutexFreeList)
		{
			_rtp_sig_mutex_release(rtpMutexDebugLock);
			return (-1);
		}
		
		/* ----------------------------------- */
		/*      Grab a mutex head storage      */
		/*     location from the free list.    */
		/* ----------------------------------- */
		*pNewMutex = gpMutexFreeList;
		gpMutexFreeList = gpMutexFreeList->next;
		
		if (gpMutexFreeList)
		{
			gpMutexFreeList->prev = 0;
		}
		
		(*pNewMutex)->seq   = 0;
		(*pNewMutex)->wait  = 0;
		(*pNewMutex)->state = gcMutexAllocated;
		rtp_strcpy((*pNewMutex)->file, file);
		(*pNewMutex)->line  = line;

		/* ----------------------------------- */
		/*         Link *pNewMutex onto        */
		/*         the top of the list.        */
		/* ----------------------------------- */
		(*pNewMutex)->prev = 0;
		(*pNewMutex)->next = gpMutexList;
		
		if (gpMutexList)
		{
			gpMutexList->prev = *pNewMutex;
		}
		gpMutexList = *pNewMutex;
		giNumMutexsTracking++;

		_rtp_sig_mutex_release(rtpMutexDebugLock);
		
		return (0);
	}
	return (-1);
}

static int __rtp_FreeMutexHeader (RTPMutexHeader* pFreedMutex, const char* file, long line)
{
	if (!__rtp_TrackingMutexHeader(pFreedMutex))
	{
		/* ----------------------------------- */
		/*         This mutex does not         */
		/*       exist on the track list.      */
		/* ----------------------------------- */
		return (-1);
	}
	
	if (__rtp_InitMutexDebug())
	{
		_rtp_sig_mutex_claim(rtpMutexDebugLock);
		
		/* ----------------------------------- */
		/*       Remove pFreedMutex from       */
		/*            the track list.          */
		/* ----------------------------------- */
		if (pFreedMutex->prev)
		{
			pFreedMutex->prev->next = pFreedMutex->next;
		}
		else
		{
			/* ----------------------------------- */
			/*  Freeing the head, must reasign     */
			/*  and clear prev pointer to now      */
			/*  old head.                          */
			/* ----------------------------------- */
			if (gpMutexList->next)
			{
				gpMutexList = gpMutexList->next;
				gpMutexList->prev = 0;
			}
			else
			{
				gpMutexList = 0;
			}
		}
		
		if (pFreedMutex->next)
		{
			pFreedMutex->next->prev = pFreedMutex->prev;
		}

		/* ----------------------------------- */
		/*        Link pFreedMutex onto        */
		/*      the top of the free list.      */
		/* ----------------------------------- */
		pFreedMutex->prev = 0;
		pFreedMutex->next = gpMutexFreeList;
		
		if (gpMutexFreeList)
		{
			gpMutexFreeList->prev = pFreedMutex;
		}
		gpMutexFreeList = pFreedMutex;
		giNumMutexsTracking--;

		pFreedMutex->seq   = 0;
		pFreedMutex->wait  = 0;
		pFreedMutex->state = gcMutexFreed;
		rtp_strcpy(pFreedMutex->file, file);
		pFreedMutex->line  = line;

		_rtp_sig_mutex_release(rtpMutexDebugLock);
		
		return (0);
	}
	return (-1);
}

static unsigned __rtp_TrackingMutexHeader (RTPMutexHeader* pMutex)
{
RTPMutexHeader* pNextMutex;
	
	if (__rtp_InitMutexDebug())
	{
		_rtp_sig_mutex_claim(rtpMutexDebugLock);
		
		if (gpMutexList)
		{
			pNextMutex = gpMutexList;
			while (pNextMutex)
			{
				if (pMutex == pNextMutex)
				{
					_rtp_sig_mutex_release(rtpMutexDebugLock);
					return (1);
				}
				pNextMutex = pNextMutex->next;
			}
		}

		_rtp_sig_mutex_release(rtpMutexDebugLock);
	}
	
	/* ----------------------------------- */
	/*         This mutex does not         */
	/*       exist on the track list.      */
	/* ----------------------------------- */
	return (0);
}
#endif /* RTP_TRACK_LOCAL_MUTEXES */



/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
