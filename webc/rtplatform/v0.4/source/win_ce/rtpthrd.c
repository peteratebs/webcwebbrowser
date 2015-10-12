 /*
 | RTPTHRD.C - Runtime Platform Thread Services
 |
 |   PORTED TO THE WIN32 PLATFORM
 |
 | EBSnet - RT-Platform
 |
 |  $Author: tony $
 |  $Date: 2004/12/21 22:13:39 $
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
#include "rtpthrd.h"
#include "rtpsignl.h"

#include <windows.h>
#include <Winbase.h>
#include <string.h>

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
	DWORD      dwThreadId;
    char       name[CFG_NAME_SIZE];
	void     (*entryPoint)(void *);
	void      *userData;
}
rtp_thread_context;

/************************************************************************
* Data
************************************************************************/
const int threadPriorityMap [32] = {
                                 THREAD_PRIORITY_LOWEST,
                                 THREAD_PRIORITY_LOWEST,
                                 THREAD_PRIORITY_LOWEST,
                                 THREAD_PRIORITY_LOWEST,
                                 THREAD_PRIORITY_LOWEST,
                                 THREAD_PRIORITY_LOWEST,
                                 THREAD_PRIORITY_BELOW_NORMAL,
                                 THREAD_PRIORITY_BELOW_NORMAL,
                                 THREAD_PRIORITY_BELOW_NORMAL,
                                 THREAD_PRIORITY_BELOW_NORMAL,
                                 THREAD_PRIORITY_BELOW_NORMAL,
                                 THREAD_PRIORITY_BELOW_NORMAL,
                                 THREAD_PRIORITY_NORMAL,
                                 THREAD_PRIORITY_NORMAL,
                                 THREAD_PRIORITY_NORMAL,
                                 THREAD_PRIORITY_NORMAL,
                                 THREAD_PRIORITY_NORMAL,
                                 THREAD_PRIORITY_NORMAL,
                                 THREAD_PRIORITY_NORMAL,
                                 THREAD_PRIORITY_NORMAL,
                                 THREAD_PRIORITY_ABOVE_NORMAL,
                                 THREAD_PRIORITY_ABOVE_NORMAL,
                                 THREAD_PRIORITY_ABOVE_NORMAL,
                                 THREAD_PRIORITY_ABOVE_NORMAL,
                                 THREAD_PRIORITY_ABOVE_NORMAL,
                                 THREAD_PRIORITY_ABOVE_NORMAL,
                                 THREAD_PRIORITY_HIGHEST,
                                 THREAD_PRIORITY_HIGHEST,
                                 THREAD_PRIORITY_HIGHEST,
                                 THREAD_PRIORITY_HIGHEST,
                                 THREAD_PRIORITY_HIGHEST,
                                 THREAD_PRIORITY_HIGHEST,
                               };

const unsigned int stackSizeMap [5] = {
                               0x1000,
							   0x2000,
							   0x4000,
							   0x8000,
							   0xffff
                             };
static int rtp_threads_up = 0;
static DWORD tlsIndex;

/************************************************************************
* Macros
************************************************************************/

/************************************************************************
* Utility Function Prototypes
************************************************************************/
DWORD WINAPI _rtp_threadFunction( LPVOID ctx );
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
	if (rtp_threads_up == 0)
	{
    	tlsIndex = TlsAlloc();
    }

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

		if (rtp_threads_up == 0)
		{
			TlsFree(tlsIndex);
		}
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
HANDLE hThread;
rtp_thread_context *ctx;
long stackSize;

	ctx = _rtp_allocThreadContext();
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
    	strncpy (ctx->name, name, CFG_NAME_SIZE-1);
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

	hThread = CreateThread(NULL,                              /* default security attributes */
	                       stackSize,                         /* set stack size  */
	                       _rtp_threadFunction,               /* thread function */
	                       (LPVOID) ctx,                      /* argument to thread function */
	                       STACK_SIZE_PARAM_IS_A_RESERVATION, /* WIN CE FLAG to specify supplied stack size
	                                                             rather than default 64kb */
	                       &ctx->dwThreadId);                 /* returns the thread identifier */

	/* Check the return value for success. */
	if (hThread == NULL)
	{
		_rtp_freeThreadContext (ctx);
		return (-1);
	}

	*newThread = (RTP_HANDLE) ctx;

	if (!SetThreadPriority(hThread, ctx->priority))
	{
		/* non-fatal error: thread was created, but not at specified priority */
		CloseHandle(hThread);
		return (-2);
	}

	CloseHandle(hThread);

    return (0);
}


/*----------------------------------------------------------------------*
                            rtp_thread_handle
 *----------------------------------------------------------------------*/
int rtp_thread_handle (RTP_HANDLE * currentThread)
{
    *currentThread = (RTP_HANDLE) TlsGetValue(tlsIndex);

	if (*currentThread)
	{
		return (0);
	}

	return (-1);
}


/*----------------------------------------------------------------------*
                          rtp_thread_user_data
 *----------------------------------------------------------------------*/
int rtp_thread_user_data (void ** userData)
{
RTP_HANDLE currentHandle;

    if (rtp_thread_handle(&currentHandle) != 0)
    {
        return (-1);
    }

    return (rtp_thread_user_data_by_handle (currentHandle, userData));
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
RTP_HANDLE currentHandle;

    if (rtp_thread_handle(&currentHandle) != 0)
    {
        return (-1);
    }

    return (rtp_thread_name_by_handle (currentHandle, name));
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
        Sleep (INFINITE);
    }
    else
    {
        Sleep (msecs);
    }
}


/*----------------------------------------------------------------------*
                             rtp_thread_yield
 *----------------------------------------------------------------------*/
void rtp_thread_yield (void)
{
    Sleep (0);
}


/************************************************************************
* Internal utility functions
************************************************************************/

DWORD WINAPI _rtp_threadFunction( LPVOID ctx )
{
rtp_thread_context *t = (rtp_thread_context *) ctx;

    TlsSetValue(tlsIndex, ctx);

    t->state = RTP_THREAD_IS_RUNNING;
	t->entryPoint(t->userData);
	_rtp_freeThreadContext(t);

	ExitThread(0);

    return (0);
}


rtp_thread_context * _rtp_allocThreadContext(void)
{
	return ((rtp_thread_context *) malloc(sizeof(rtp_thread_context)));
}

void _rtp_freeThreadContext(rtp_thread_context *ctx)
{
	free(ctx);
}

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
