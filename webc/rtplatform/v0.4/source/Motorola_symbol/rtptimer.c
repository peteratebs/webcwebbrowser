 /*
 | RTPTIMER.C - 
 |
 | EBSnet - 
 |
 |  $Author: shane $
 |  $Date: 2004/10/05 18:51:30 $
 |  $Name:  $
 |  $Revision: 1.2 $
 |
 | Copyright EBSnet Inc. , 2004
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
*/

/*****************************************************************************
 * Header files
 *****************************************************************************/

#include "rtptimer.h"
#include "rtpthrd.h"
#include "rtpsignl.h"
#include "rtpmem.h"
#include "rtptime.h"

/*****************************************************************************
 * Macros
 *****************************************************************************/

#define DLLIST_INIT(X)            { (X)->next = (X); (X)->prev = (X); }

/* insert Y after X */
#define DLLIST_INSERT_AFTER(X,Y)  { (X)->next->prev = (Y); \
                                    (Y)->next = (X)->next; \
                                    (Y)->prev = (X); \
                                    (X)->next = (Y); \
                                  }

/* insert Y before X */                                  
#define DLLIST_INSERT_BEFORE(X,Y) { (X)->prev->next = (Y); \
                                    (Y)->prev = (X)->prev; \
                                    (Y)->next = (X); \
                                    (X)->prev = (Y); \
                                  }
                                  
#define DLLIST_REMOVE(X)          { (X)->prev->next = (X)->next; \
                                    (X)->next->prev = (X)->prev; \
                                  }

/* replace X with R */                                  
#define DLLIST_REPLACE(X,R)       { (R)->next = (X)->next; \
                                    (R)->prev = (X)->prev; \
                                    (X)->next->prev = (R); \
                                    (X)->prev->next = (R); \
                                  }

#define RTP_TIMER_LIST_NONE       0
#define RTP_TIMER_LIST_NEW        1
#define RTP_TIMER_LIST_ACTIVE     2

/*****************************************************************************
 * Types
 *****************************************************************************/

/*****************************************************************************
 * Data
 *****************************************************************************/

static int           rtpTimerInitialized = 0;
static unsigned      rtpTimerThreadAwake = 0;
static RTP_MUTEX     rtpTimerLock;
static RTP_SEMAPHORE rtpTimerSignal;
static RTPTimerJob   rtpTimerNewList;
static RTPTimerJob   rtpTimerActiveList;
static RTPTimerJob*  rtpTimerNextToProcess;

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

void _rtp_timer_thread (void*);
long _rtp_timer_find_msecs_to_next_timer (RTPTimerJob* timerList);
void _rtp_timer_process_expired_jobs (RTPTimerJob* timerList);
void _rtp_timer_add_new_jobs (RTPTimerJob* toTimerList, RTPTimerJob* fromTimerList);

/*****************************************************************************
 * Function Definitions
 *****************************************************************************/

/*---------------------------------------------------------------------------*/
int  rtp_timer_init     (void)
{
	if (rtpTimerInitialized == 0)
	{
		if (rtp_sig_mutex_alloc(&rtpTimerLock, 0) >= 0)
		{
			if (rtp_sig_semaphore_alloc(&rtpTimerSignal, 0) >= 0)
			{
				RTP_THREAD newThread;
				
				DLLIST_INIT(&rtpTimerNewList);
				DLLIST_INIT(&rtpTimerActiveList);
				
				rtpTimerNextToProcess = 0;
				
				if (rtp_thread_spawn (&newThread,
				                      _rtp_timer_thread,
				                      0, 
				                      0, 
				                      2, 
				                      0) >= 0)
				{
					rtpTimerInitialized = 1;
					return (0);
				}
				
				rtp_sig_semaphore_free(rtpTimerSignal);
			}
			
			rtp_sig_mutex_free(rtpTimerLock);
		}
		
		return (-1);
	}
	
	rtpTimerInitialized++;
	return (0);	
}

/*---------------------------------------------------------------------------*/
void rtp_timer_shutdown (void)
{
}

/*---------------------------------------------------------------------------*/
int  rtp_timer_start    (RTP_TIMER* newJob, 
                         void (*fn)(int,void*), 
                         void* data, 
                         long msecInterval,
                         unsigned long repeat)
{
	newJob->scheduledTimeMsec = msecInterval;
	newJob->timerFunction = fn;
	newJob->timerData = data;
	newJob->repeat = repeat;

	if (rtp_sig_mutex_claim(rtpTimerLock) >= 0)
	{		
		DLLIST_INSERT_BEFORE(&rtpTimerNewList, newJob);
		
		newJob->listId = RTP_TIMER_LIST_NEW;
		
		if (!rtpTimerThreadAwake)
		{
			rtpTimerThreadAwake = 1;
			rtp_sig_semaphore_signal(rtpTimerSignal);
		}

		rtp_sig_mutex_release(rtpTimerLock);
		
		return (0);
	}
	
	return (-1);
}
                        
/*---------------------------------------------------------------------------*/
int  rtp_timer_stop    (RTP_TIMER* timer)
{
	int result = -1;
	
	rtp_sig_mutex_claim(rtpTimerLock);
	
	switch (timer->listId)
	{
		case RTP_TIMER_LIST_NONE:
			break;

		case RTP_TIMER_LIST_ACTIVE:
			if (rtpTimerNextToProcess == timer)
			{
				rtpTimerNextToProcess = timer->next;
			}
			/* intentional fall-through */
			
		case RTP_TIMER_LIST_NEW:
			DLLIST_REMOVE(timer);
			DLLIST_INIT(timer);
			timer->listId = RTP_TIMER_LIST_NONE;
			result = 0;

			if (!rtpTimerThreadAwake)
			{
				rtpTimerThreadAwake = 1;
				rtp_sig_semaphore_signal(rtpTimerSignal);		
			}
			break;
	}
	
	rtp_sig_mutex_release(rtpTimerLock);
		
	return (result);
}
                        
/*---------------------------------------------------------------------------*/
void _rtp_timer_thread (void* data)
{
	int result;
	long msecsUntilNextTimer;

	rtp_sig_mutex_claim(rtpTimerLock);

	while (1)
	{
		msecsUntilNextTimer = _rtp_timer_find_msecs_to_next_timer(&rtpTimerActiveList);

		if (msecsUntilNextTimer != 0)
		{			
			rtpTimerThreadAwake = 0;
			rtp_sig_mutex_release(rtpTimerLock);
			result = rtp_sig_semaphore_wait_timed(rtpTimerSignal, msecsUntilNextTimer);
			rtp_sig_mutex_claim(rtpTimerLock);
			rtpTimerThreadAwake = 1;
		}
		else
		{
			result = 1;
		}

		if (result != 0)
		{
			rtpTimerThreadAwake = 1;
			_rtp_timer_process_expired_jobs(&rtpTimerActiveList);		
			rtpTimerThreadAwake = 0;
		}
		
		_rtp_timer_add_new_jobs(&rtpTimerActiveList, &rtpTimerNewList);		
	}
	
}

/*---------------------------------------------------------------------------*/
long _rtp_timer_find_msecs_to_next_timer (RTPTimerJob* timerList)
{
	long timeout = RTP_TIMEOUT_INFINITE;
	RTPTimerJob *job = timerList->next;
	
	if (job != timerList)
	{
		timeout = (long) (job->scheduledTimeMsec - rtp_get_system_msec());
		if (timeout < 0)
		{
			timeout = 0;
		}
	}
	
	return (timeout);
}

/*---------------------------------------------------------------------------*/
void _rtp_timer_process_expired_jobs (RTPTimerJob* timerList)
{
	RTPTimerJob *job = timerList->next;
	long timeDifferenceMsec;
	unsigned long currentMsec = rtp_get_system_msec();
	void (*timerFunction) (int,void*);
	void* timerData;
	
	while (job != timerList)
	{
		rtpTimerNextToProcess = job->next;
		
		timeDifferenceMsec = (long) (currentMsec - job->scheduledTimeMsec);
		if (timeDifferenceMsec > 0)
		{
			DLLIST_REMOVE(job);

			if (job->repeat == RTP_TIMER_REPEAT_INFINITE)
			{
				DLLIST_INSERT_BEFORE(&rtpTimerNewList, job);				
				job->listId = RTP_TIMER_LIST_NEW;				
			}
			else
			{
				if (job->repeat > 0)
				{
					/* if this timer has more repeats left, add it to the 
					   new list */
					job->repeat--;
					DLLIST_INSERT_BEFORE(&rtpTimerNewList, job);				
					job->listId = RTP_TIMER_LIST_NEW;				
				}
				else
				{
					DLLIST_INIT(job);
					job->listId = RTP_TIMER_LIST_NONE;
				}
			}

			if (job->timerFunction)
			{
				timerFunction = job->timerFunction;
				timerData = job->timerData;
				rtp_sig_mutex_release(rtpTimerLock);
				timerFunction(0, timerData);
				rtp_sig_mutex_claim(rtpTimerLock);
			}
			
		}
		else
		{
			break;
		}

		job = rtpTimerNextToProcess;
	}

	rtpTimerNextToProcess = 0;
}

/*---------------------------------------------------------------------------*/
void _rtp_timer_add_new_jobs (RTPTimerJob* toTimerList, RTPTimerJob* fromTimerList)
{
	RTPTimerJob *next;
	RTPTimerJob *newJob;
	RTPTimerJob *job;
	long timeDifference;		
	
	newJob = fromTimerList->next;
	while (newJob != fromTimerList)
	{
		next = newJob->next;

		newJob->scheduledTimeMsec += rtp_get_system_msec();
		
		job = toTimerList->next;		
		while (job != toTimerList)
		{
			timeDifference = (long) (job->scheduledTimeMsec - newJob->scheduledTimeMsec);
			if (timeDifference > 0)
			{
				break;
			}
			job = job->next;
		}
		
		DLLIST_REMOVE(newJob);
		DLLIST_INSERT_BEFORE(job, newJob);
		
		newJob->listId = RTP_TIMER_LIST_ACTIVE;

		newJob = next;
	}
}
