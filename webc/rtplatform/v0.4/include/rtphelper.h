 /*
 | RTPHELPER.H - 
 |
 | EBSnet - 
 |
 |  $Author: shane $
 |  $Date: 2004/10/05 18:51:29 $
 |  $Name:  $
 |  $Revision: 1.2 $
 |
 | Copyright EBSnet Inc. , 2004
 | All rights reserved.
 | This code may not be redistributed in source or linkable object form
 | without the consent of its author.
*/

#ifndef __RTPHELPER_H__
#define __RTPHELPER_H__

typedef struct s_RTP_HELPER_THREAD_CTX RTP_HELPER_THREAD_CTX;
typedef struct s_RTP_HELPER_THREAD_JOB RTP_HELPER_THREAD_JOB;

#include "rtpsignl.h"

struct s_RTP_HELPER_THREAD_CTX
{
	RTP_MUTEX              mutex;
	RTP_SEMAPHORE          signal;
	RTP_HELPER_THREAD_JOB* firstJob;
	RTP_HELPER_THREAD_JOB* lastJob;
	long                   numHelpersRunning;
	long                   numHelpersActive;
	long                   maxIdleTimeMsec;
	unsigned               stopping;
};

struct s_RTP_HELPER_THREAD_JOB
{
	RTP_HELPER_THREAD_JOB* next;
	
	void (*execute) (
			RTP_HELPER_THREAD_JOB* job
		);
};

#ifdef __cplusplus
extern "C" {
#endif

int rtp_helper_thread_ctx_init (
		RTP_HELPER_THREAD_CTX* ctx,
		RTP_MUTEX mutex,
		RTP_SEMAPHORE signal
	);

void rtp_helper_thread_ctx_destroy (
		RTP_HELPER_THREAD_CTX* ctx,
		RTP_MUTEX* releasedMutex,
		RTP_SEMAPHORE* releasedSignal		
	);	

int rtp_helper_thread_queue_job (
		RTP_HELPER_THREAD_CTX* ctx,
		RTP_HELPER_THREAD_JOB* job
	);
	
int rtp_helper_thread_start (
		RTP_HELPER_THREAD_CTX* ctx		
	);

int rtp_helper_thread_start_cond (
		RTP_HELPER_THREAD_CTX* ctx,
		long maxHelperThreads
	);
	
int rtp_helper_thread_stop_all (
		RTP_HELPER_THREAD_CTX* ctx,
		long maxTimeoutMsec
	);

#ifdef __cplusplus
}
#endif

#endif /* __RTPHELPER_H__ */
