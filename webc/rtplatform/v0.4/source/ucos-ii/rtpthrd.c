 /*
 | RTPTHRD.C - Runtime Platform Thread Services
 |
 | EBSnet - RT-Platform 
 |
 |  $Author: yohannes $
 |  $Date: 2005/04/07 16:06:52 $
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
#include "includes.h"	/* UCOS include file */

/************************************************************************
* Defines
************************************************************************/
#define		RTP_CFG_N_STACKS_HUGE			4
#define 	RTP_CFG_N_STACKS_BIG			4
#define 	RTP_CFG_N_STACKS_NORMAL			10
#define 	RTP_CFG_N_STACKS_SMALL			4

#define		RTP_SIZESTACK_HUGE				16*1024
#define		RTP_SIZESTACK_BIG				 8*1024
#define		RTP_SIZESTACK_NORMAL			 4*1024
#define		RTP_SIZESTACK_SMALL				 2*1024

/* Thread status flag values. May be by the kernel background timer task
   to provide an orderly approach to ending tasks and freeing task stacks */
#define RTP_THREAD_IS_FREE             0
#define RTP_THREAD_STARTING            1
#define RTP_THREAD_IS_RUNNING          2
#define RTP_THREAD_MUST_BE_KILLED      3
#define RTP_THREAD_STACK_MUST_BE_FREED 4

/* information to help debug threads */
#define RTP_THREAD_DEBUG_CREATE		   1

/************************************************************************
* Type definitions
*************************************************************************/

typedef struct _rtp_thread_context
{
    int        			  state;
    struct rtp_thread_context   *next;
    RTP_HANDLE 			  priority;				/* in UCOS, the priority is the handle */
    char       			  name[CFG_NAME_SIZE];
	void     			(*entryPoint)(void *);
	void      			 *userData;
	
	int		  			 *stkBase;			
	int		   			  stkSize;
}
rtp_thread_context;

/************************************************************************
* Data
************************************************************************/

int HugeStacksArray[RTP_CFG_N_STACKS_HUGE][RTP_SIZESTACK_HUGE];
int BigStacksArray[RTP_CFG_N_STACKS_BIG][RTP_SIZESTACK_BIG];
int NormalStacksArray[RTP_CFG_N_STACKS_NORMAL][RTP_SIZESTACK_NORMAL];
int SmallStacksArray[RTP_CFG_N_STACKS_SMALL][RTP_SIZESTACK_SMALL];

const unsigned int stackSizeMap [5] = {
                               RTP_SIZESTACK_SMALL,
							   RTP_SIZESTACK_NORMAL,
							   RTP_SIZESTACK_NORMAL,
							   RTP_SIZESTACK_BIG,
							   RTP_SIZESTACK_HUGE
                             };
                             
const unsigned char threadPriorityMap [32] = { 
                                 62,    /* THREAD_PRIORITY_LOWEST */
                                 62,
                                 62,
                                 60,
                                 58,
                                 56,
                                 54,   /* THREAD_PRIORITY_BELOW_NORMAL */
                                 52,
                                 50,
                                 48,
                                 46,
                                 44,
                                 42,   /* THREAD_PRIORITY_NORMAL */
                                 40,
                                 38,
                                 36,
                                 34,
                                 32,
                                 30,
                                 28,
                                 26,   /* THREAD_PRIORITY_ABOVE_NORMAL */
                                 24,
                                 22,
                                 20,
                                 18,
                                 16,
                                 14,   /* THREAD_PRIORITY_HIGHEST */
                                 12,
                                 10,
                                 8,
                                 8,
                                 8,
                               };                             
                             
rtp_thread_context	hugeContexts[RTP_CFG_N_STACKS_HUGE] = {0};
rtp_thread_context	bigContexts[RTP_CFG_N_STACKS_BIG] = {0};
rtp_thread_context	normalContexts[RTP_CFG_N_STACKS_NORMAL] = {0};
rtp_thread_context	smallContexts[RTP_CFG_N_STACKS_SMALL] = {0};

rtp_thread_context  *hugeFree = 0;
rtp_thread_context  *bigFree = 0;
rtp_thread_context  *normalFree = 0;
rtp_thread_context  *smallFree = 0;
rtp_thread_context  *allTrack = 0;

static int rtp_threads_up = 0;

/************************************************************************
* Macros
************************************************************************/

/************************************************************************
 * Thread services                                                      *
 ************************************************************************/
 
/************************************************************************
* Utility Function Types & Prototypes
************************************************************************/
static rtp_thread_context *_rtp_removeSingleList(void **List);
static void _rtp_addSingleList(void **List, rtp_thread_context *Member);
rtp_thread_context * rtp_get_current_task_context(void);
rtp_thread_context * rtp_get_task_context(RTP_HANDLE handle);

/************************************************************************
* Function Bodies
************************************************************************/

/*----------------------------------------------------------------------*
                            rtp_threads_init
 *----------------------------------------------------------------------*/
/** @memo   Prepare for use of the RTPlatform thread API.

    @doc    Prepare for use of the RTPlatform thread API.

    @return 0 on success, -1 otherwise.
 */
int rtp_threads_init (void)
{
int counter;

	/* setup the huge stacks */
    for(counter = 0; counter < RTP_CFG_N_STACKS_HUGE; counter++)
    {
    	hugeContexts[counter].state = RTP_THREAD_IS_FREE;
    	hugeContexts[counter].stkBase = &HugeStacksArray[counter][0];
    	hugeContexts[counter].stkSize = RTP_SIZESTACK_HUGE;
    	_rtp_addSingleList((void **)&hugeFree,&hugeContexts[counter]);
    }
	
	/* setup the big stacks */
    for(counter = 0; counter < RTP_CFG_N_STACKS_BIG; counter++)
    {
    	bigContexts[counter].state = RTP_THREAD_IS_FREE;
    	bigContexts[counter].stkBase = &BigStacksArray[counter][0];
    	bigContexts[counter].stkSize = RTP_SIZESTACK_BIG;
    	_rtp_addSingleList((void **)&bigFree,&bigContexts[counter]);
    }
	
	/* setup the normal stacks */
    for(counter = 0; counter < RTP_CFG_N_STACKS_NORMAL; counter++)
    {
    	normalContexts[counter].state = RTP_THREAD_IS_FREE;
    	normalContexts[counter].stkBase = &NormalStacksArray[counter][0];
    	normalContexts[counter].stkSize = RTP_SIZESTACK_NORMAL;
    	_rtp_addSingleList((void **)&normalFree,&normalContexts[counter]);
    }
	
	/* setup the small stacks */
    for(counter = 0; counter < RTP_CFG_N_STACKS_SMALL; counter++)
    {
    	smallContexts[counter].state = RTP_THREAD_IS_FREE;
    	smallContexts[counter].stkBase = &SmallStacksArray[counter][0];
    	smallContexts[counter].stkSize = RTP_SIZESTACK_SMALL;
    	_rtp_addSingleList((void **)&smallFree,&smallContexts[counter]);
    }

	
	return(0);
}


/*----------------------------------------------------------------------*
                           rtp_threads_shutdown
 *----------------------------------------------------------------------*/
/** @memo   Shutdown the RTPlatform thread API.

    @doc    When use of the RTPlatform thread API is no longer 
    needed, this function clears and/or releases any allocated 
    resources setup by the rtp_threads_init function call.

    @return void
 */
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
/** @memo   Spawn a thread.

    @doc    Spawn a thread and sets its priority level.

    @return 0 on success, -1 on failure, and -2 on non-fatal
    error.  If a -1 is returned the value of *newThread is 
    undefined.  An example of a non-fatal error would be if 
    the thread was spawned but the priority level could not 
    be set.
 */
int rtp_thread_spawn (
  RTP_HANDLE  * newThread,              /** Storage location for the handle to the new thread. */
  RTP_ENTRY_POINT_FN entryPoint,        /** A function pointer to the threads entry point. */
  const char  * name,                   /** The name of the thread. */
  int           stackSizeIndex,         /** An index into the array of possible stack sizes. */
  int           priorityIndex,          /** An index into the array of possible priorities. */
  void        * userData                /** User data to be passed to the entry point function. */
  )
{
   
int 			stackSize;
int 			counter;
unsigned int	stkTop;
rtp_thread_context *tc = 0;

#if (RTP_THREAD_DEBUG_CREATE)

static int 		countSmall  = 0;
static int		countNormal = 0;
static int		countBig	= 0;
static int		countHuge	= 0;

#endif

	OS_ENTER_CRITICAL();
    if (!rtp_threads_up)				/* initialize threads */
    {
      rtp_threads_init();
      rtp_threads_up++;
    }
	OS_EXIT_CRITICAL();
 										/* extract the exact stack size from the table */
	stackSize = stackSizeMap[stackSizeIndex];
    
    OS_ENTER_CRITICAL();
    									/* find a stack for the rask */
	switch(stackSize)
	{
		case RTP_SIZESTACK_HUGE:
#if (RTP_THREAD_DEBUG_CREATE)
			countHuge++;
#endif
			tc = _rtp_removeSingleList((void *)&hugeFree);
			break;
		case RTP_SIZESTACK_BIG:
#if (RTP_THREAD_DEBUG_CREATE)
			countBig++;
#endif
			tc = _rtp_removeSingleList((void *)&bigFree);	
			break;
		case RTP_SIZESTACK_NORMAL:
#if (RTP_THREAD_DEBUG_CREATE)
			countNormal++;
#endif
			tc = _rtp_removeSingleList((void *)&normalFree);	
			break;
		case RTP_SIZESTACK_SMALL:
#if (RTP_THREAD_DEBUG_CREATE)
			countSmall++;
#endif
			tc = _rtp_removeSingleList((void *)&smallFree);	
			break;
		default:
		  break;
		
	}
		
	_rtp_addSingleList((void **)&allTrack, tc);

	OS_EXIT_CRITICAL();
	
    if (!tc)
    {
    	return(-1);
    }
    
    tc->state = RTP_THREAD_STARTING;
    
    if (name)
	{    
      rtp_strncpy (tc->name, name, CFG_NAME_SIZE-1);
    }
	  
	tc->entryPoint	= entryPoint;
	tc->userData	= userData;
	
	
   /* Get the stack top as an even alligned offset from the base   */
    stkTop = (unsigned int)(tc->stkBase +  tc->stkSize - 2);

    /* Loop from the priority we have on up until the spawn succeeds
      ucos will return an error if the priority level is already occupied
      we set the task handle each time so when the spawn succeeds it will
      be set */
    OS_ENTER_CRITICAL();
    for (counter = threadPriorityMap[priorityIndex]; counter < 63; counter++)
    {
        tc->priority = counter;
       	tc->state = RTP_THREAD_IS_RUNNING;

     	if (    OSTaskCreate(entryPoint,
                (void *) userData, (unsigned int *)stkTop,
                counter) == 0) 
        {
    		OS_EXIT_CRITICAL();
            return(0);
        }
		
		tc->state = RTP_THREAD_STARTING;

    }     
    OS_EXIT_CRITICAL();

    /* didn't create the task.  Clear the stack */
    OS_ENTER_CRITICAL();

    tc = _rtp_removeSingleList((void **)&allTrack);

    switch(stackSize)
	{
		case RTP_SIZESTACK_HUGE:
			_rtp_addSingleList((void **)&hugeFree,tc);
			break;
		case RTP_SIZESTACK_BIG:
			_rtp_addSingleList((void **)&bigFree,tc);	
			break;
		case RTP_SIZESTACK_NORMAL:
			_rtp_addSingleList((void **)&normalFree,tc);	
			break;
		case RTP_SIZESTACK_SMALL:
			_rtp_addSingleList((void **)&smallFree,tc);	
			break;
		default:
		  break;
		
	}
    tc->state = RTP_THREAD_IS_FREE;
    OS_EXIT_CRITICAL();
}


/*----------------------------------------------------------------------*
                            rtp_thread_handle
 *----------------------------------------------------------------------*/
/** @memo   Get the current thread handle.

    @doc    Retrieves the handle to the current thread.

    @return 0 on success, -1 otherwise.
 */
int rtp_thread_handle (
  RTP_HANDLE * currentThread            /** Storage location for the thread handle. */
  )
{

rtp_thread_context * currentContext;
	
	currentContext = rtp_get_current_task_context(); /* for UCOS, the priority is the handle */
	
	if(currentContext)
	{
		*currentThread = currentContext->priority;
		return(1);
	}	

	return(-1);
}


/*----------------------------------------------------------------------*
                          rtp_thread_user_data
 *----------------------------------------------------------------------*/
/** @memo   Gets the user data associated with the current thread.

    @doc    Retrieves the user data from the current thread.

    @return 0 on success, -1 otherwise.
 */
int rtp_thread_user_data (
  void ** userData                      /** Storage location for the user data. */
  )
{
rtp_thread_context * currentContext;

	currentContext = rtp_get_current_task_context();
	
	if(currentContext)
	{
    	*userData = currentContext->userData;
		return(0);
	}
	
	return (-1);
}


/*----------------------------------------------------------------------*
                       rtp_thread_user_data_by_handle
 *----------------------------------------------------------------------*/
/** @memo   Gets the user data associated with the thread handle.

    @doc    Retrieves the user data from the thread with the 
    associated thread handle.

    @return 0 on success, -1 otherwise.
 */
int rtp_thread_user_data_by_handle (
  RTP_HANDLE handle,                    /** Handle to the thread to find its user data. */
  void ** userData                      /** Storage location for the user data. */
  )
{
rtp_thread_context * currentContext;

	currentContext = rtp_get_task_context(handle);

	if(currentContext)
	{
	  *userData = currentContext->userData; 
	  return(0);
	}
	
	return (-1);
}


/*----------------------------------------------------------------------*
                              rtp_thread_name
 *----------------------------------------------------------------------*/
/** @memo   Gets the name associated with the current thread.

    @doc    Retrieves the name from the current thread.

    @return 0 on success, -1 otherwise.
 */
int rtp_thread_name (
  char ** name                          /** Storage location for the name. */
  )
{
rtp_thread_context * currentContext;

	currentContext = rtp_get_current_task_context();
	
	if (currentContext)
	{
		rtp_strncpy (*name, currentContext->name, CFG_NAME_SIZE-1);
		return(0);
	}
	return(-1);
}


/*----------------------------------------------------------------------*
                          rtp_thread_name_by_handle
 *----------------------------------------------------------------------*/
/** @memo   Gets the name associated with the thread handle.

    @doc    Retrieves the name from the thread with the 
    associated thread handle.

    @return 0 on success, -1 otherwise.
 */
int rtp_thread_name_by_handle (
  RTP_HANDLE handle,                    /** Handle to the thread to find its name. */
  char ** name                          /** Storage location for the name. */
  )
{
rtp_thread_context * context;

	context = rtp_get_task_context(handle);
	
	if(context)
	{
		rtp_strncpy (*name, context->name, CFG_NAME_SIZE-1);
		return(0);
	}
	
	return(-1);
}


/*----------------------------------------------------------------------*
                             rtp_thread_sleep
 *----------------------------------------------------------------------*/
/** @memo   Put the current thread to sleep.

    @doc    Suspend the current thread for the millisecond 
    timeout period.

    @return void
 */
void rtp_thread_sleep (
  long msecs                            /** Timeout value in milliseconds. */
  )
{

long noTicks;

    if (!msecs)
    {
    	return;
    }
    
	noTicks = msecs * OS_TICKS_PER_SEC / 1000;    
    OSTimeDly(noTicks);

}


/*----------------------------------------------------------------------*
                             rtp_thread_yield
 *----------------------------------------------------------------------*/
/** @memo   Yield the current thread to any other of equal priority.

    @doc    Check if any other threads are attempting to run that 
    are of equal priority.  If there is one of equal proritey ready
    to run, let it, otherwise the yield returns and the current 
    thread continues.

    @return void
 */
void rtp_thread_yield (void)
{
    
}

/************************************************************************
* Internal utility functions
************************************************************************/
rtp_thread_context * rtp_get_current_task_context()
{
rtp_thread_context *temp;

	temp = allTrack;
	
	if(temp)
	{
		while(1)
		{
		    if ((temp->state == RTP_THREAD_IS_RUNNING) &&
		        (temp->priority == OSTCBCur->OSTCBPrio))
			{
				return(temp);
			}
		
			temp = (rtp_thread_context *)temp->next;
			if(!temp)
			{
				temp = allTrack;
			}
		}
	}
}

rtp_thread_context *rtp_get_task_context(RTP_HANDLE handle)
{

rtp_thread_context *temp;

	temp = allTrack;
	
	while(temp)
	{
	    if ((temp->state == RTP_THREAD_IS_RUNNING) &&
	        (temp->priority == handle))
		{
			return(temp);
		}
		
		temp = (rtp_thread_context *)temp->next;
	}

}

static void _rtp_addSingleList(void **List, rtp_thread_context *Member)
{
    
    Member->next = *List;
    *List = Member;
}

static rtp_thread_context *_rtp_removeSingleList(void **List)
{
    if(*List)
    {
    	rtp_thread_context *temp = (rtp_thread_context *)*List;
    	*List = (void **)temp->next;
    	return(temp);
    }
    else return(0);
}
/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
