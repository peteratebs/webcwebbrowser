//
// RTPTHRD.C - Runtime Platform Thread Services
//
//   PORTED TO THE Segger embOS simulation for Windows 32 bit system
//                 and Visual C++.
//
// EBSnet - RT-Platform
//
//  $Author: tom $
//  $Date: 2004/10/22 18:07:01 $
//  $Name:  $
//  $Revision: 1.15 $
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
#define RTP_INCLUDE_THRD
#include "rtpconfig.h"

/************************************************************************
* Defines
************************************************************************/
/* Thread status flag values. May be used by the kernel background timer
   task to provide an orderly approach to ending tasks and freeing task
   stacks */
#define RTP_THREAD_IS_FREE             0
#define RTP_THREAD_STARTING            1
#define RTP_THREAD_IS_RUNNING          2
#define RTP_THREAD_MUST_BE_KILLED      3
#define RTP_THREAD_STACK_MUST_BE_FREED 4

#define RTP_STACK_IS_FREE              0
#define RTP_STACK_IS_USED              1

#define RTP_CLEANUP_PRIORITY           1 /* lowest */

/************************************************************************
* Type definitions
*************************************************************************/

/************************************************************************
* Data
************************************************************************/
/* Priorities range 0 < priority <= 255 ; 255 == highest */
const unsigned char threadPriorityMap [32] = { 
                                 1,    /* THREAD_PRIORITY_LOWEST */
                                 2,
                                 3,
                                 4,
                                 5,
                                 6,
                                 61,   /* THREAD_PRIORITY_BELOW_NORMAL */
                                 62,
                                 63,
                                 64,
                                 65,
                                 66,
                                 124,   /* THREAD_PRIORITY_NORMAL */
                                 125,
                                 126,
                                 127,
                                 128,
                                 129,
                                 130,
                                 131,
                                 190,   /* THREAD_PRIORITY_ABOVE_NORMAL */
                                 191,
                                 192,
                                 193,
                                 194,
                                 195,
                                 250,   /* THREAD_PRIORITY_HIGHEST */
                                 251,
                                 252,
                                 253,
                                 254,
                                 255,
                               };
static int rtp_threads_up = 0;

static RTP_THREAD_CONTEXT *rtp_thread_context_free = 0;
static RTP_THREAD_CONTEXT *rtp_thread_context_track = 0;

static RTP_STACK_CONTEXT *rtp_stack_context_free = 0;
static RTP_STACK_CONTEXT *rtp_stack_context_track = 0;

static OS_RSEMA rtpResourceLock;

static OS_STACKPTR int CleanupStack[128]; /* Cleanup Task stack */
static OS_TASK CleanupTcb;               /* Cleanup Task-control-block */

/************************************************************************
* Macros
************************************************************************/
#define LOCK_LIST    OS_Use(&rtpResourceLock);
#define UNLOCK_LIST  OS_Unuse(&rtpResourceLock);

/************************************************************************
* Utility Function Types & Prototypes
************************************************************************/

typedef struct s_SingleList
{
    struct s_SingleList *next;
}_SingleList;

static void _rtp_addSingleList(void **List, _SingleList *Member);
static _SingleList *_rtp_removeSingleList(void **List);

static void _rtp_ThreadEntry(void);       /* Thread entry fn to retreive userdata */
static void _rtp_ThreadCleanup(void);     /* Thread timer task to clean up Task and Stack Ctx's */

/************************************************************************
* Function Bodies
************************************************************************/

/************************************************************************
 * Thread services                                                      *
 ************************************************************************/
// OS_STACKPTR int Stack0[128], Stack1[128]; /* Task stacks */
//OS_TASK TCB0, TCB1;               /* Task-control-blocks */
//void Taskx(void) {
//  while (1) {
//    OS_Delay (100);
//  }
//}

/*----------------------------------------------------------------------*
                            rtp_threads_init
 *----------------------------------------------------------------------*/
int rtp_threads_init (void)
{   
    if (rtp_threads_up == 0)
    {
        OS_CREATERSEMA(&rtpResourceLock);
        OS_CREATETASK(&CleanupTcb,
                      "Task Cleanup",
                      _rtp_ThreadCleanup,
                      RTP_CLEANUP_PRIORITY,
                      CleanupStack);
    }
    rtp_threads_up++;
    
    return (0);
}

/*----------------------------------------------------------------------*
                           rtp_threads_alloc
 *----------------------------------------------------------------------*/
int  rtp_threads_pool_init (int nThread, RTP_THREADP resourceBlock)
{
    int i;
    RTP_THREAD_CONTEXT  *rtp_thread_context;

    LOCK_LIST
	
	rtp_thread_context = (RTP_THREAD_CONTEXT  *)resourceBlock;
    for (i = 0; i < nThread; i++)
    {
        _rtp_addSingleList((void **)&rtp_thread_context_free, 
                           (_SingleList *)rtp_thread_context);
        rtp_thread_context_free->state = RTP_THREAD_IS_FREE;
        rtp_thread_context++;
    }

    UNLOCK_LIST
    return(0);
}	

/*----------------------------------------------------------------------*
                           rtp_threads_stacks_alloc
 *----------------------------------------------------------------------*/
int  rtp_threads_stacks_pool_init (int nStack, int stackSizeIndex,
                                     int stackSize,
                                     RTP_STACKP resourceBlock)
{
    int i;
    RTP_STACK_CONTEXT *rtp_stack_context;

    LOCK_LIST

	rtp_stack_context = (RTP_STACK_CONTEXT *)resourceBlock;
    for (i = 0; i < nStack; i++)
    {
        _rtp_addSingleList((void **)&rtp_stack_context_free, 
                           (_SingleList *)rtp_stack_context);
        rtp_stack_context++;
        rtp_stack_context_free->state = RTP_STACK_IS_FREE;
        rtp_stack_context_free->size = stackSize;
        rtp_stack_context_free->size_index = stackSizeIndex;
        rtp_stack_context_free->stack = (int *)rtp_stack_context;
        rtp_stack_context = (RTP_STACK_CONTEXT *)(
                             (unsigned long)rtp_stack_context + stackSize); 
    }

    UNLOCK_LIST
    return(0);
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
                     OS_Terminate(&CleanupTcb);
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
    RTP_STACK_CONTEXT **newStack;
    RTP_STACK_CONTEXT *tempStack;

    LOCK_LIST

    newStack = &rtp_stack_context_free;
    if((*newThread = (RTP_THREAD)(_rtp_removeSingleList((void *)&rtp_thread_context_free))) == 0)
    {
        UNLOCK_LIST
        return (-1);
    }
    while(*newStack)
    {
        tempStack = (RTP_STACK_CONTEXT *)*newStack;

    	if(tempStack->size_index == stackSizeIndex)
    	{
	    _rtp_removeSingleList((void *)newStack);
    	    ((RTP_THREAD_CONTEXT *)*newThread)->name = (char *)name;
    	    ((RTP_THREAD_CONTEXT *)*newThread)->state = RTP_THREAD_STARTING;
    	    ((RTP_THREAD_CONTEXT *)*newThread)->priority = threadPriorityMap[priorityIndex];
    	    ((RTP_THREAD_CONTEXT *)*newThread)->entryPoint = entryPoint;
    	    ((RTP_THREAD_CONTEXT *)*newThread)->userData = userData;
    	    ((RTP_THREAD_CONTEXT *)*newThread)->stackContext = tempStack;

            _rtp_addSingleList((void **)&rtp_thread_context_track, 
                               (_SingleList *)*newThread);
            _rtp_addSingleList((void **)&rtp_stack_context_track, 
                               (_SingleList *)tempStack);
    	    
    	    tempStack->state = RTP_STACK_IS_USED;
    	    tempStack->threadContext = ((RTP_THREAD_CONTEXT *)*newThread);

            OS_CreateTask(
                           &((RTP_THREAD_CONTEXT *)*newThread)->os_tcb,
#if OS_TRACKNAME /* Defined in RTOS.H */
                           (char *)name,
#endif
                           (unsigned char)threadPriorityMap[priorityIndex],
                           _rtp_ThreadEntry,
                           tempStack->stack,
                           tempStack->size
#if __OS_RRSUPPORTED__ /* Defined in RTOS.H */
                           ,2
#endif
                           );
            UNLOCK_LIST
            return (0);
        }
        else
            newStack = (RTP_STACK_CONTEXT **)(*newStack);
    }
    _rtp_addSingleList((void **)&rtp_thread_context_free, 
                           (_SingleList *)newThread);
    UNLOCK_LIST
    return (-1);
}

/*----------------------------------------------------------------------*
                            rtp_thread_handle
 *----------------------------------------------------------------------*/
int rtp_thread_handle (RTP_HANDLE * currentThread)
{
    OS_TASK *currentTcb = OS_GetpCurrentTask();

    LOCK_LIST
    (RTP_THREAD_CONTEXT *)*currentThread = rtp_thread_context_track;
    while(*currentThread)
    {
        if(&((RTP_THREAD_CONTEXT *)*currentThread)->os_tcb == currentTcb)
        {
                UNLOCK_LIST
		return (0);
        }
        (RTP_THREAD_CONTEXT *)*currentThread = ((RTP_THREAD_CONTEXT *)*currentThread)->next;
    }

    UNLOCK_LIST
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
	*userData = ((RTP_THREAD_CONTEXT *) handle)->userData;
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
	*name = ((RTP_THREAD_CONTEXT *) handle)->name;
	return (0);
}


/*----------------------------------------------------------------------*
                             rtp_thread_sleep
 *----------------------------------------------------------------------*/
void rtp_thread_sleep (long msecs)
{

    if (msecs == (-1))
    {
      OS_Delay(0x7FFFFFFF);
    }
    else
    {
       OS_Delay((int) msecs);
    }
}


/*----------------------------------------------------------------------*
                             rtp_thread_yield
 *----------------------------------------------------------------------*/
void rtp_thread_yield (void)
{
       OS_Delay(1);
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

static void _rtp_ThreadEntry(void)
{
RTP_THREAD_CONTEXT *currentThread;
OS_TASK *currentTcb = OS_GetpCurrentTask(); 

    LOCK_LIST
    currentThread = rtp_thread_context_track;
    while(currentThread)
    {
        if(&currentThread->os_tcb == currentTcb)
        {
            currentThread->state = RTP_THREAD_IS_RUNNING;
            UNLOCK_LIST
            currentThread->entryPoint(currentThread->userData);
            currentThread->state = RTP_THREAD_STACK_MUST_BE_FREED;
            OS_Terminate(0);
        }
        currentThread = currentThread->next;
    }
    UNLOCK_LIST
}

static void _rtp_ThreadCleanup(void)
{
RTP_THREAD_CONTEXT *tempThread;
RTP_THREAD_CONTEXT **freeThread;

    while(1)
    {
        LOCK_LIST
        tempThread = rtp_thread_context_track;
        freeThread = &rtp_thread_context_track;
        while(tempThread)
        {
            if(tempThread->state == RTP_THREAD_STACK_MUST_BE_FREED)
            {
                RTP_STACK_CONTEXT *tempStack = tempThread->stackContext;
                RTP_STACK_CONTEXT **freeStack = &rtp_stack_context_track;
                while(*freeStack)
                {
                    if(*freeStack == tempStack)
                    {
	                    _rtp_removeSingleList((void *)freeStack);
                        tempStack->state = RTP_STACK_IS_FREE;
                        _rtp_addSingleList((void **)&rtp_stack_context_free, 
                                           (_SingleList *)tempStack);
                        _rtp_removeSingleList((void *)freeThread);
                        tempThread->state = RTP_THREAD_IS_FREE;
                        _rtp_addSingleList((void **)&rtp_thread_context_free, 
                                           (_SingleList *)tempThread);
						tempThread = *freeThread;
					    break;
                     }
                     freeStack = (RTP_STACK_CONTEXT **)(*freeStack);
                }
            }
			else
			{
			    freeThread = (RTP_THREAD_CONTEXT **)(*freeThread);
                tempThread = tempThread->next;
			}
            
        }
        UNLOCK_LIST
        OS_Delay (1000);
    }
}

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */

