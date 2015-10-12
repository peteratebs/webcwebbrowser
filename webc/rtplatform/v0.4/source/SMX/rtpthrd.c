 /*
 | RTPTHRD.C - Runtime Platform Thread Services
 |
 |   PORTED TO THE LINUX PLATFORM
 |
 | EBSnet - RT-Platform
 |
 |  $Author: tony $
 |  $Date: 2005/01/25 17:11:59 $
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

//#include <pthread.h>
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
    char       name[CFG_NAME_SIZE];
	void     (*entryPoint)(void *);
	void      *userData;
}
rtp_thread_context;

/************************************************************************
* Data
************************************************************************/
const unsigned int stackSizeMap [5] = {
                               0x1000,
							   0x2000,
							   0x4000,
							   0x8000,
							   0xffff
                             };

/************************************************************************
* Macros
************************************************************************/

/************************************************************************
* Utility Function Prototypes
************************************************************************/
void *_rtp_threadFunction(void *ctx);
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
    return (0);
}


/*----------------------------------------------------------------------*
                           rtp_threads_shutdown
 *----------------------------------------------------------------------*/
void rtp_threads_shutdown (void)
{
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

    return (0);
}


/*----------------------------------------------------------------------*
                            rtp_thread_handle
 *----------------------------------------------------------------------*/
int rtp_thread_handle (RTP_HANDLE * currentThread)
{
return (0);

}


/*----------------------------------------------------------------------*
                          rtp_thread_user_data
 *----------------------------------------------------------------------*/
int rtp_thread_user_data (void ** userData)
{
return (0);

}


/*----------------------------------------------------------------------*
                       rtp_thread_user_data_by_handle
 *----------------------------------------------------------------------*/
int rtp_thread_user_data_by_handle (RTP_HANDLE handle, void ** userData)
{
return (0);
}


/*----------------------------------------------------------------------*
                              rtp_thread_name
 *----------------------------------------------------------------------*/
int rtp_thread_name (char ** name)
{
return (0);

}


/*----------------------------------------------------------------------*
                          rtp_thread_name_by_handle
 *----------------------------------------------------------------------*/
int rtp_thread_name_by_handle (RTP_HANDLE handle, char ** name)
{
return (0);
}


/*----------------------------------------------------------------------*
                             rtp_thread_sleep
 *----------------------------------------------------------------------*/
void rtp_thread_sleep (long msecs)
{

}


/*----------------------------------------------------------------------*
                             rtp_thread_yield
 *----------------------------------------------------------------------*/
void rtp_thread_yield (void)
{

}


/************************************************************************
* Internal utility functions
************************************************************************/

void *_rtp_threadFunction(void * ctx )
{
return (0);

}


rtp_thread_context * _rtp_allocThreadContext(void)
{
return (0);
}

void _rtp_freeThreadContext(rtp_thread_context *ctx)
{

}

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
