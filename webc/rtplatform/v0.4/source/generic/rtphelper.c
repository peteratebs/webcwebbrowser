 /*
 | RTPHELPER.C - 
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

#include "rtphelper.h"
#include "rtpthrd.h"

/*****************************************************************************
 * Macros
 *****************************************************************************/

#define RTP_HELPER_THREAD_STACK_SIZE         0
#define RTP_HELPER_THREAD_PRIORITY           0
#define RTP_HELPER_THREAD_QUEUE_TIMEOUT_MSEC 100

/*****************************************************************************
 * Types
 *****************************************************************************/

/*****************************************************************************
 * Function Prototypes
 *****************************************************************************/

static void _rtp_helper_thread (void* ctx);

/*****************************************************************************
 * Data
 *****************************************************************************/

/*****************************************************************************
 * Function Definitions
 *****************************************************************************/

/*---------------------------------------------------------------------------*/
int rtp_helper_thread_ctx_init (
		RTP_HELPER_THREAD_CTX* ctx,
		RTP_MUTEX mutex,
		RTP_SEMAPHORE signal
	)
{
	ctx->mutex = mutex;
	ctx->signal = signal;
	ctx->firstJob = 0;
	ctx->lastJob = 0;
	ctx->numHelpersRunning = 0;
	ctx->numHelpersActive = 0;
	ctx->stopping = 0;
	ctx->maxIdleTimeMsec = 5000; 

	return (0);
}   

/*---------------------------------------------------------------------------*/
void rtp_helper_thread_ctx_destroy (
		RTP_HELPER_THREAD_CTX* ctx,
		RTP_MUTEX* releasedMutex,
		RTP_SEMAPHORE* releasedSignal		
	)
{
	rtp_helper_thread_stop_all(ctx, 10000);
	*releasedMutex = ctx->mutex;
	*releasedSignal = ctx->signal;
}
	
/*---------------------------------------------------------------------------*/
int rtp_helper_thread_start (
		RTP_HELPER_THREAD_CTX* ctx		
	)
{
	int result = -1;
	RTP_THREAD threadHandle;

	rtp_sig_mutex_claim(ctx->mutex);
	
	if (rtp_thread_spawn (
			&threadHandle, 
			_rtp_helper_thread, 
			0, 
			RTP_HELPER_THREAD_STACK_SIZE, 
			RTP_HELPER_THREAD_PRIORITY, 
			ctx
		) >= 0)
	{
		ctx->numHelpersRunning++;
		result = 0;
	}
	
	rtp_sig_mutex_release(ctx->mutex);
	
	return (result);
}

/*---------------------------------------------------------------------------*/
int rtp_helper_thread_start_cond (
		RTP_HELPER_THREAD_CTX* ctx,
		long maxHelperThreads
	)
{
	int result = -1;
	RTP_THREAD threadHandle;

	rtp_sig_mutex_claim(ctx->mutex);
	
	if (ctx->numHelpersRunning == ctx->numHelpersActive &&
	    ctx->numHelpersRunning < maxHelperThreads)
	{
		if (rtp_thread_spawn (
				&threadHandle, 
				_rtp_helper_thread, 
				0, 
				RTP_HELPER_THREAD_STACK_SIZE, 
				RTP_HELPER_THREAD_PRIORITY, 
				ctx
			) >= 0)
		{
			ctx->numHelpersRunning++;
			result = 0;
		}
	}

	rtp_sig_mutex_release(ctx->mutex);
	
	return (result);
}
	
/*---------------------------------------------------------------------------*/
int rtp_helper_thread_queue_job (
		RTP_HELPER_THREAD_CTX* ctx,
		RTP_HELPER_THREAD_JOB* job
	)
{
	rtp_sig_mutex_claim(ctx->mutex);	
	
	if (ctx->lastJob)
	{
		ctx->lastJob->next = job;		
	}	
	else
	{
		ctx->firstJob = job;
	}
	job->next = 0;
	ctx->lastJob = job;
	rtp_sig_semaphore_signal(ctx->signal);	
	
	rtp_sig_mutex_release(ctx->mutex);

	return (0);
}
	
/*---------------------------------------------------------------------------*/
int rtp_helper_thread_stop_all (
		RTP_HELPER_THREAD_CTX* ctx,
		long maxTimeoutMsec
	)
{	
	int result;
	long elapsedMsec = 0;
	
	rtp_sig_mutex_claim(ctx->mutex);	
	
	ctx->stopping = 1;
	
	while ((maxTimeoutMsec == RTP_TIMEOUT_INFINITE || elapsedMsec < maxTimeoutMsec) &&
	       ctx->numHelpersRunning > 0)
	{
		rtp_sig_mutex_release(ctx->mutex);
		rtp_thread_sleep(RTP_HELPER_THREAD_QUEUE_TIMEOUT_MSEC);		
		elapsedMsec += RTP_HELPER_THREAD_QUEUE_TIMEOUT_MSEC;
		rtp_sig_mutex_claim(ctx->mutex);			
	}

	ctx->stopping = 0;
	result = (ctx->numHelpersRunning == 0)? 0 : -1;

	rtp_sig_mutex_release(ctx->mutex);
	
	return (result);
}

/*---------------------------------------------------------------------------*/
void _rtp_helper_thread (void* d)
{
RTP_HELPER_THREAD_CTX* ctx = (RTP_HELPER_THREAD_CTX*) d;
RTP_HELPER_THREAD_JOB* job;
long idleMsec = 0;	
	
	/* loop: wait for a job, process it */
	
	rtp_sig_mutex_claim(ctx->mutex);
	
	while (!ctx->stopping && idleMsec < ctx->maxIdleTimeMsec)
	{		
		rtp_sig_mutex_release(ctx->mutex);
		
		if (rtp_sig_semaphore_wait_timed(ctx->signal, RTP_HELPER_THREAD_QUEUE_TIMEOUT_MSEC) < 0)
		{
			idleMsec += RTP_HELPER_THREAD_QUEUE_TIMEOUT_MSEC;
			rtp_sig_mutex_claim(ctx->mutex);
			continue;
		}
		
		rtp_sig_mutex_claim(ctx->mutex);
		
		job = ctx->firstJob;
		if (job)
		{
			idleMsec = 0;
			if (job == ctx->lastJob)
			{
				ctx->lastJob = 0;
			}
			ctx->firstJob = job->next;
			job->next = 0;
			ctx->numHelpersActive++;
			rtp_sig_mutex_release(ctx->mutex);
			
			job->execute(job);
			
			rtp_sig_mutex_claim(ctx->mutex);
			ctx->numHelpersActive--;
		}
	}
	
	ctx->numHelpersRunning--;
	
	rtp_sig_mutex_release(ctx->mutex);
}
