 /*
 | RTPTHRD.C - Runtime Platform Thread Services
 |
 |   PORTED TO THE EBSnet Inc RTIP TCP/IP PLATFORM
 |
 | EBSnet - RT-Platform
 |
 |  $Author: shane $
 |  $Date: 2004/10/14 18:33:38 $
 |  $Name:  $
 |  $Revision: 1.5 $
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
#include "rtpthrd.h"
#include "rtpsignl.h"

#include <rtip.h>

/************************************************************************
* Defines
************************************************************************/
/* Thread status flag values. May be by the kernel background timer task
   to provide an orderly approach to ending tasks and freeing task stacks */
#define RTP_THREAD_IS_FREE             0
#define RTP_THREAD_STARTING            1
#define RTP_THREAD_IS_RUNNING          2
#define RTP_THREAD_MUST_BE_KILLED      3
#define RTP_THREAD_STACK_MUST_BE_FREED 4

/************************************************************************
* Type definitions
*************************************************************************/
typedef struct _rtp_thread_context
{
    int        state;
    int        priority;
    RTP_HANDLE threadHandle;
	DWORD      dwThreadId;     
    char       name[CFG_NAME_SIZE];
	void     (*entryPoint)(void *);
	void      *userData;
}
rtp_thread_context;

/************************************************************************
* Data
************************************************************************/

/* THIS HAS NO USE HERE BECAUSE OF THE TASKCLASS */
const int threadPriorityMap [32] = { 
                                 PRIOTASK_NORMAL,
                                 PRIOTASK_NORMAL,
                                 PRIOTASK_NORMAL,
                                 PRIOTASK_NORMAL,
                                 PRIOTASK_NORMAL,
                                 PRIOTASK_NORMAL,
                                 PRIOTASK_NORMAL,
                                 PRIOTASK_NORMAL,
                                 PRIOTASK_NORMAL,
                                 PRIOTASK_NORMAL,
                                 PRIOTASK_NORMAL,
                                 PRIOTASK_HI,
                                 PRIOTASK_HI,
                                 PRIOTASK_HI,
                                 PRIOTASK_HI,
                                 PRIOTASK_HI,
                                 PRIOTASK_HI,
                                 PRIOTASK_HI,
                                 PRIOTASK_HI,
                                 PRIOTASK_HI,
                                 PRIOTASK_HI,
                                 PRIOTASK_HI,
                                 PRIOTASK_HIGHEST,
                                 PRIOTASK_HIGHEST,
                                 PRIOTASK_HIGHEST,
                                 PRIOTASK_HIGHEST,
                                 PRIOTASK_HIGHEST,
                                 PRIOTASK_HIGHEST,
                                 PRIOTASK_HIGHEST,
                                 PRIOTASK_HIGHEST,
                                 PRIOTASK_HIGHEST,
                                 PRIOTASK_HIGHEST
                               };

/* THIS HAS NO USE HERE BECAUSE OF THE TASKCLASS */
const unsigned int stackSizeMap [5] = {
                               SIZESTACK_NORMAL,
							   SIZESTACK_NORMAL,
							   SIZESTACK_BIG,
							   SIZESTACK_BIG,
							   SIZESTACK_HUGE
                             };
                             
static int rtp_threads_up = 0;

/************************************************************************
* Macros
************************************************************************/

/************************************************************************
* Utility Function Prototypes
************************************************************************/
void _rtp_threadFunction(void* ctx);
rtp_thread_context * _rtp_allocThreadContext(void);
void _rtp_freeThreadContext(rtp_thread_context *ctx);

/************************************************************************
* Function Bodies
************************************************************************/

/************************************************************************
 * Thread services                                                      *
 ************************************************************************/

/*----------------------------------------------------------------------*
                            rtp_threads_init
 *----------------------------------------------------------------------*/
int rtp_threads_init (void)
{   
    rtp_threads_up++;
    return (0);
}


/*----------------------------------------------------------------------*
                           rtp_threads_shutdown
 *----------------------------------------------------------------------*/
void rtp_threads_shutdown (void)
{
	if (rtp_threads_up > 0)
	{
		rtp_threads_up--;
	}
}


/*----------------------------------------------------------------------*
                            rtp_thread_spawn
 *----------------------------------------------------------------------*/
int rtp_thread_spawn (RTP_HANDLE         * newThread,
                      RTP_ENTRY_POINT_FN   entryPoint,
                      const char         * name,
                      int                  stackSizeIndex,
                      int                  priorityIndex,
                      void               * userData)
{
rtp_thread_context *ctx;
long stackSize;
int result;

	ctx = _rtp_allocThreadContext( );
	if (!ctx)
	{
		return (-1);
	}

    if (stackSizeIndex < -2)
	{
		stackSizeIndex = -2;
	}
	
	if (stackSizeIndex > 2)
	{
		stackSize = stackSizeIndex;
	}
	else
	{
		stackSize = stackSizeMap[stackSizeIndex + 2];
	}

	if (name)
	{    
    	rtp_strncpy (ctx->name, name, CFG_NAME_SIZE-1);
    	ctx->name[CFG_NAME_SIZE-1] = '\0';
    }
    else
    {
    	ctx->name[0] = 0;
    }
    
    if (priorityIndex < -16)
    {
    	priorityIndex = -16;
    }
    
    if (priorityIndex > 15)
    {
    	priorityIndex = 15;
    }
    
	ctx->priority   = threadPriorityMap[priorityIndex + 16];
	ctx->entryPoint = entryPoint;
	ctx->userData   = userData;

	result = os_spawn_task (
					TASKCLASS_RTPLATFORM,   /* task class */
					_rtp_threadFunction,    /* thread function */
					(PFVOID) ctx,           /* argument to thread function */
					NULL,
					NULL,
					NULL
				);
	
	/* Check the return value for success. */
	if (!result)
	{
		_rtp_freeThreadContext(ctx);
		return (-1);
	}

	*newThread = (RTP_HANDLE) ctx;
    return (0);
}


/*----------------------------------------------------------------------*
                            rtp_thread_handle
 *----------------------------------------------------------------------*/
int rtp_thread_handle (RTP_HANDLE * currentThread)
{
    rtp_not_yet_implemented( );
    return (-1);
}


/*----------------------------------------------------------------------*
                          rtp_thread_user_data
 *----------------------------------------------------------------------*/
int rtp_thread_user_data (void ** userData)
{
    rtp_not_yet_implemented( );
    return (-1);
}


/*----------------------------------------------------------------------*
                       rtp_thread_user_data_by_handle
 *----------------------------------------------------------------------*/
int rtp_thread_user_data_by_handle (RTP_HANDLE handle, void ** userData)
{
	*userData = ((rtp_thread_context *) handle)->userData;
	return (0);
}


/*----------------------------------------------------------------------*
                              rtp_thread_name
 *----------------------------------------------------------------------*/
int rtp_thread_name (char ** name)
{
    rtp_not_yet_implemented( );
    return (-1);
}


/*----------------------------------------------------------------------*
                          rtp_thread_name_by_handle
 *----------------------------------------------------------------------*/
int rtp_thread_name_by_handle (RTP_HANDLE handle, char ** name)
{
	*name = ((rtp_thread_context *) handle)->name;
	return (0);
}


/*----------------------------------------------------------------------*
                             rtp_thread_sleep
 *----------------------------------------------------------------------*/
void rtp_thread_sleep (long msecs)
{
    if (msecs == (-1))
    {
        ks_sleep((word)RTIP_INF);
    }
    else
    {
        ks_sleep((word)msecs);
    }
}


/*----------------------------------------------------------------------*
                             rtp_thread_yield
 *----------------------------------------------------------------------*/
void rtp_thread_yield (void)
{
    ks_yield( );
}


/************************************************************************
* Internal utility functions
************************************************************************/

void _rtp_threadFunction (void* ctx)
{
rtp_thread_context *t;
PSYSTEM_USER userCtx = os_get_user( );

	t = (rtp_thread_context *)userCtx->udata0;
    t->state = RTP_THREAD_IS_RUNNING;
	t->entryPoint(t->userData);
	_rtp_freeThreadContext(t);
	
	os_exit_task( );
}


rtp_thread_context * _rtp_allocThreadContext(void)
{
	return ((rtp_thread_context *) ks_malloc(1, sizeof(rtp_thread_context), 0));
}

void _rtp_freeThreadContext(rtp_thread_context *ctx)
{
	ks_free((PFBYTE)ctx, 0, 0);
}

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
