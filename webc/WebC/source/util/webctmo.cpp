//
// WEBCTMO.CPP - WebC timeout routines
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//    This module contains code to download, process, and display web pages,
//		as well as process browser messages, etc.
//

/*****************************************************************************/
// Header files
/*****************************************************************************/

#include "webctmo.h"
#include "webcassert.h"
#include "rtptime.h"
#include "webcmem.h"

/*****************************************************************************/
// Macros
/*****************************************************************************/

#define WEBC_MAX_NUM_TIMEOUTS 200
#define MSEC_DIFF(A,B)        ((WEBC_INT32)(((WEBC_UINT32)(A))-((WEBC_UINT32)(B))))
#define TOI_FLAG_INSIDE_FUNC  0x01
#define TOI_FLAG_CANCELED     0x02


/*****************************************************************************/
// Local Types
/*****************************************************************************/

struct matchFnInfo
{
	int (*match)(webcTimeoutFn func, WEBC_PFBYTE data, webcTimeoutFn parm_fn, WEBC_PFBYTE param_data);
	webcTimeoutFn param_fn;
	WEBC_PFBYTE param_data;
};

static int _webc_match_any_timeout (TimeoutInfo* timeout, void* data);

/*****************************************************************************/
// Global Data
/*****************************************************************************/

/*****************************************************************************/
// Function Bodies
/*****************************************************************************/

/*****************************************************************************/
// Methods for class WebcTimeoutManager
/*****************************************************************************/

WebcTimeoutManager::WebcTimeoutManager (void)
{
	mpTimeoutList = 0;
	mpNewList = 0;
	mpCancelledList = 0;
	mNextId = 0;
	mInsideTickHandler = 0;
	miNumTimeouts = 0;
}

WebcTimeoutManager::~WebcTimeoutManager (void)
{

	CancelTimeout(_webc_match_any_timeout, 0, 1);
	DeleteCancelled();

	WEBC_ASSERT(miNumTimeouts == 0);
}

void WebcTimeoutManager::DeleteCancelled (void)
{
	while (mpCancelledList)
	{
		DeleteTimeout(mpCancelledList, 0, &mpCancelledList);
	}
}


WEBC_INT32 WebcTimeoutManager::NextExpiration (void)
{
	WEBC_UINT32 currentTimeMsec = rtp_get_system_msec();
	WEBC_INT32 msecDiff;

	if (mpTimeoutList)
	{
		msecDiff = MSEC_DIFF(currentTimeMsec, mpTimeoutList->iMsecTimeout);
	}
	else
	{
		msecDiff = WEBC_TIMEOUT_INFINITE;
	}

	return (msecDiff);
}


void WebcTimeoutManager::ProcessTime (void)
{
	if (mInsideTickHandler)
	{
		return;
	}

	InsertNewTimeouts();

	mInsideTickHandler = 1;

	WEBC_UINT32 currentTimeMsec = rtp_get_system_msec();
	WEBC_INT32 msecDiff;

	TimeoutInfo *current;

	while (mpTimeoutList)
	{
		current = mpTimeoutList;

		msecDiff = MSEC_DIFF(currentTimeMsec, current->iMsecTimeout);

		if (msecDiff >= 0)
		{
			mpTimeoutList = mpTimeoutList->pNext;

			current->pNext = mpNewList;
			mpNewList = current;

			if (current->iLoop != WEBC_LOOP_FOREVER)
			{
				current->iLoop--;
			}

			if ((current->Func) && !(current->flags & TOI_FLAG_CANCELED))
			{
				current->flags |= TOI_FLAG_INSIDE_FUNC;
				current->Func(current->pData, msecDiff);
				current->flags &= ~TOI_FLAG_INSIDE_FUNC;
			}

			if ((current->iLoop != WEBC_LOOP_FOREVER) && (current->iLoop < 0))
			{
				current->flags |= TOI_FLAG_CANCELED;
			}
			else
			{
				// carry the error over to the next interval
				current->iMsecTimeout += current->iMsecInterval;
			}
		}
		else
		{
			break;
		}
	}

	mInsideTickHandler = 0;

	DeleteCancelled();
	InsertNewTimeouts();
}

WEBC_UINT16 WebcTimeoutManager::SetTimeout (
		webcTimeoutFn func,
		webcIdleFn cancelfn,
		void* data,
		WEBC_INT32 msec,
		int loop
	)
{
	return SetTimeoutDebug(func, cancelfn, data, msec, loop, __FILE__, __LINE__);
}

WEBC_UINT16 WebcTimeoutManager::SetTimeoutDebug (
		webcTimeoutFn func,
		webcIdleFn cancelfn,
		void* data,
		WEBC_INT32 msec,
		int loop,
		const char* file,
		int line
	)
{
	struct TimeoutInfo *pInfo;

	if (miNumTimeouts >= WEBC_MAX_NUM_TIMEOUTS)
	{
		return (WEBC_TIMEOUT_INVALID);
	}

	pInfo = (struct TimeoutInfo *) WEBC_DEBUG_MALLOC(sizeof(struct TimeoutInfo), file, line,"SetTimeoutDebug",0);
	if (!pInfo)
	{
		return (WEBC_TIMEOUT_INVALID);
	}

	WEBC_UINT32 currentTimeMsec = rtp_get_system_msec();

	pInfo->wId           = mNextId++;
	if (mNextId == WEBC_TIMEOUT_INVALID)
	{
		mNextId++;
	}
	pInfo->Func          = func;
	pInfo->CancelFunc    = cancelfn;
	pInfo->pData         = data;
	pInfo->iMsecInterval = msec;
	pInfo->iLoop         = loop;
	pInfo->flags         = 0;
	pInfo->iMsecTimeout  = pInfo->iMsecInterval + currentTimeMsec;

	pInfo->pNext = mpNewList;

	mpNewList = pInfo;
	miNumTimeouts++;

	return (pInfo->wId);
}

void WebcTimeoutManager::InsertNewTimeouts (void)
{
	struct TimeoutInfo* current = mpNewList;

	while (mpNewList)
	{
		current = mpNewList;
		mpNewList = mpNewList->pNext;
		if (current->flags & TOI_FLAG_CANCELED)
		{
			if (current->CancelFunc)
			{
				current->CancelFunc((WEBC_PFBYTE)current->pData);
			}

			WEBC_FREE(current);
			WEBC_ASSERT(miNumTimeouts > 0);
			miNumTimeouts--;
		}
		else
		{
			InsertTimeout(current);
		}
	}
}

void WebcTimeoutManager::InsertTimeout (TimeoutInfo* info)
{
	// always put at front of list so set timer will be able to
	// access the information
	struct TimeoutInfo* prev = 0;
	struct TimeoutInfo* current = mpTimeoutList;

	while (current)
	{
		if (MSEC_DIFF(info->iMsecTimeout, current->iMsecTimeout) < 0)
		{
			break;
		}
		prev = current;
		current = current->pNext;
	}

	info->pNext = current;

	if (prev)
	{
		prev->pNext = info;
	}
	else
	{
		mpTimeoutList = info;
	}
}

int WebcTimeoutManager::CancelTimeout (int (*match)(TimeoutInfo*, void*), void* data, WEBC_BOOL cancelAll)
{
int result = 0;
struct TimeoutInfo *pCurrent, *pPrev = 0, *pNext;

	pCurrent = mpTimeoutList;
	while (pCurrent)
	{
		pNext = pCurrent->pNext;
		if (match(pCurrent, data))
		{
			if (!DeleteTimeout(pCurrent, pPrev, &mpTimeoutList))
			{
				pPrev = pCurrent;
			}
			result++;

			if (!cancelAll)
			{
				return (1);
			}
		}
		else
		{
			pPrev = pCurrent;
		}
		pCurrent = pNext;
	}

	pPrev = 0;
	pCurrent = mpNewList;
	while (pCurrent)
	{
		pNext = pCurrent->pNext;
		if (match(pCurrent, data))
		{
			if (!DeleteTimeout(pCurrent, pPrev, &mpNewList))
			{
				pPrev = pCurrent;
			}
			result++;

			if (!cancelAll)
			{
				return (1);
			}
		}
		else
		{
			pPrev = pCurrent;
		}
		pCurrent = pNext;
	}

	return (result);
}

int WebcTimeoutManager::DeleteTimeout (TimeoutInfo *pCurrent, TimeoutInfo *pPrev, TimeoutInfo** list)
{
	if (pPrev)
	{
		pPrev->pNext = pCurrent->pNext;
	}
	else
	{
		*list = pCurrent->pNext;
	}

	if (mInsideTickHandler)
	{
		pCurrent->pNext = mpCancelledList;
		mpCancelledList = pCurrent;
		pCurrent->flags |= TOI_FLAG_CANCELED;
		return (0);
	}
	else
	{
		if (pCurrent->CancelFunc)
		{
			pCurrent->CancelFunc((WEBC_PFBYTE)pCurrent->pData);
		}

		WEBC_FREE(pCurrent);
		WEBC_ASSERT(miNumTimeouts > 0);
		miNumTimeouts--;

		return (1);
	}
}

int _webc_match_any_timeout (
		TimeoutInfo* timeout,
		void* data
	)
{
	return 1;
}

// END WEBCTMO.CPP
