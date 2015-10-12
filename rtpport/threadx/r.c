 /*
 | RTPTHRD.C - Runtime Platform Thread Services
 |
 | EBSnet - EBS_RT-Platform 
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
#include "rtpport.h"
#include "rtpdebug.h"
#include "type.h" 
#include "os\os.h" 

/************************************************************************
* Defines
************************************************************************/

#define     RTP_CFG_N_STACKS_SMALL          4

#define     RTP_SIZESTACK_HUGE          RTP_CFG_SIZESTACK_HUGE
#define     RTP_SIZESTACK_BIG           RTP_CFG_SIZESTACK_BIG
#define     RTP_SIZESTACK_NORMAL            RTP_CFG_SIZESTACK_NORMAL
#define     RTP_SIZESTACK_SMALL         RTP_CFG_SIZESTACK_SMALL

/* Thread status flag values. May be by the kernel background timer task
   to provide an orderly approach to ending tasks and freeing task stacks */
#define RTP_THREAD_IS_FREE             0
#define RTP_THREAD_STARTING            1
#define RTP_THREAD_IS_RUNNING          2
#define RTP_THREAD_MUST_BE_KILLED      3
#define RTP_THREAD_STACK_MUST_BE_FREED 4

/* information to help debug threads */
#define RTP_THREAD_DEBUG_CREATE        1

/************************************************************************
* Type definitions
*************************************************************************/

typedef struct _rtp_thread_context
{
    int                   state;
    struct rtp_thread_context   *next;
    RTP_HANDLE            priority;             /* in UCOS, the priority is the handle */
    char                  name[CFG_NAME_SIZE];
    void                (*entryPoint)(void *);
    void                 *userData;
    
    int                  *stkBase;          
    int                   stkSize;
}
rtp_thread_context;

/************************************************************************
* Data
************************************************************************/

//int HugeStacksArray[RTP_CFG_N_STACKS_HUGE][RTP_SIZESTACK_HUGE];
//int BigStacksArray[RTP_CFG_N_STACKS_BIG][RTP_SIZESTACK_BIG];
//int NormalStacksArray[RTP_CFG_N_STACKS_NORMAL][RTP_SIZESTACK_NORMAL];
int SmallStacksArray[RTP_CFG_N_STACKS_SMALL][RTP_SIZESTACK_SMALL];

const unsigned int stackSizeMap [5] = {
                               RTP_SIZESTACK_SMALL,
                               RTP_SIZESTACK_NORMAL,
                               RTP_SIZESTACK_NORMAL,
                               RTP_SIZESTACK_BIG,
                               RTP_SIZESTACK_HUGE
                             };
                             
const unsigned char threadPriorityMap [32] = { 
                              31,   //2,    /* THREAD_PRIORITY_LOWEST */
                              30,   //5,
                              29,   //6,
                              28,   //7,
                              27,   //10,
                              26,   //12,
                              25,   //14,   /* THREAD_PRIORITY_BELOW_NORMAL */
                              24,   //16,
                              23,   //18,
                              22,   //20,
                              21,   //22,
                              20,   //24,
                              19,   //26,   /* THREAD_PRIORITY_NORMAL */
                              18,   //28,
                              17,   //30,
                              16,   //32,
                              15,   //34,
                              14,   //36,
                              13,   //38,
                              12,   //40,
                              11,   //42,   /* THREAD_PRIORITY_ABOVE_NORMAL */
                              10,   //44,
                              9,    //46,
                              8,    //48,
                              7,    //50,
                              6,    //52,
                              5,    //54,   /* THREAD_PRIORITY_HIGHEST */
                              4,    //56,
                              3,    //58,
                              2,    //60,
                              1,    //62,
                              0,    //63,
                               };                             
                             
//rtp_thread_context    hugeContexts[RTP_CFG_N_STACKS_HUGE] = {0};
//rtp_thread_context    bigContexts[RTP_CFG_N_STACKS_BIG] = {0};
//rtp_thread_context    normalContexts[RTP_CFG_N_STACKS_NORMAL] = {0};
rtp_thread_context  smallContexts[RTP_CFG_N_STACKS_SMALL] = {0};

rtp_thread_context  *hugeFree = 0;
rtp_thread_context  *bigFree = 0;
rtp_thread_context  *normalFree = 0;
rtp_thread_context  *smallFree = 0;
rtp_thread_context  *allTrack = 0;

static int rtp_threads_up = 0;
static int rtp_threads_up_cooperative = 0;

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

#if (RTP_CFG_N_STACKS_HUGE > 0)
    /* setup the huge stacks */
    for(counter = 0; counter < RTP_CFG_N_STACKS_HUGE; counter++)
    {
        hugeContexts[counter].state = RTP_THREAD_IS_FREE;
        hugeContexts[counter].stkBase = &HugeStacksArray[counter][0];
        hugeContexts[counter].stkSize = RTP_SIZESTACK_HUGE;
        _rtp_addSingleList((void **)&hugeFree,&hugeContexts[counter]);
    }
#endif  
#if (RTP_CFG_N_STACKS_BIG > 0)
    /* setup the big stacks */
    for(counter = 0; counter < RTP_CFG_N_STACKS_BIG; counter++)
    {
        bigContexts[counter].state = RTP_THREAD_IS_FREE;
        bigContexts[counter].stkBase = &BigStacksArray[counter][0];
        bigContexts[counter].stkSize = RTP_SIZESTACK_BIG;
        _rtp_addSingleList((void **)&bigFree,&bigContexts[counter]);
    }
#endif  
#if (RTP_CFG_N_STACKS_NORMAL > 0)
    /* setup the normal stacks */
    for(counter = 0; counter < RTP_CFG_N_STACKS_NORMAL; counter++)
    {
        normalContexts[counter].state = RTP_THREAD_IS_FREE;
        normalContexts[counter].stkBase = &NormalStacksArray[counter][0];
        normalContexts[counter].stkSize = RTP_SIZESTACK_NORMAL;
        _rtp_addSingleList((void **)&normalFree,&normalContexts[counter]);
    }
#endif
#if (RTP_CFG_N_STACKS_SMALL > 0)
    /* setup the small stacks */
    for(counter = 0; counter < RTP_CFG_N_STACKS_SMALL; counter++)
    {
        smallContexts[counter].state = RTP_THREAD_IS_FREE;
        smallContexts[counter].stkBase = &SmallStacksArray[counter][0];
        smallContexts[counter].stkSize = RTP_SIZESTACK_SMALL;
        _rtp_addSingleList((void **)&smallFree,&smallContexts[counter]);
    }
#endif
    
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
int             status ;   
int             stackSize;
int             counter;
unsigned int    stkTop;
rtp_thread_context *tc = 0;

#if (RTP_THREAD_DEBUG_CREATE)

static int      countSmall  = 0;
static int      countNormal = 0;
static int      countBig    = 0;
static int      countHuge   = 0;

#endif

    rtp_irq_disable();
    if (!rtp_threads_up)                /* initialize threads */
    {
      rtp_threads_init();
      rtp_threads_up++;
    }
    rtp_irq_enable();
                                        /* extract the exact stack size from the table */
    stackSize = stackSizeMap[stackSizeIndex];
    
    rtp_irq_disable();
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

    rtp_irq_enable();
    
    if (!tc)
    {
        return(-1);
    }
    
    tc->state = RTP_THREAD_STARTING;
    
    if (name)
    {    
      rtp_strncpy (tc->name, name, CFG_NAME_SIZE-1);
    }
      
    tc->entryPoint  = entryPoint;
    tc->userData    = userData;
    
   /* Get the stack top as an even alligned offset from the base   */
//    stkTop = (unsigned int)(tc->stkBase +  tc->stkSize - 2);
    stkTop = (unsigned int)(tc->stkBase);

    /* Loop from the priority we have on up until the spawn succeeds
      ucos will return an error if the priority level is already occupied
      we set the task handle each time so when the spawn succeeds it will
      be set */
    rtp_irq_disable();
//    for (counter = threadPriorityMap[priorityIndex]; counter < 63; counter++)
//    {
    tc->priority = 16;
    tc->state = RTP_THREAD_IS_RUNNING;

    *newThread = (RTP_HANDLE)_rtp_malloc (sizeof(TX_THREAD));
    OSRUN_Dpf("RTIP - %x - %x", *newThread, newThread) ;
    status = tx_thread_create((TX_THREAD *)(*newThread), (char *)name, (void (*)(ULONG))entryPoint, 0,
        (void *)stkTop, 4*1024, 16, 16, TX_NO_TIME_SLICE, TX_AUTO_START);

    OSRUN_Dpf("RTIP ThrStatus - %x", status) ;
    if (status == TX_SUCCESS)
    {
        rtp_irq_enable();
        return (0);
    }
    tc->state = RTP_THREAD_STARTING;

//    }     
    rtp_irq_enable();

    /* didn't create the task.  Clear the stack */
    rtp_irq_disable();

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
    rtp_irq_enable();
    return (-1) ;
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
    *currentThread = (RTP_HANDLE)tx_thread_identify() ; 
    return(0);
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
    TX_THREAD * TxThread ;
    TxThread = tx_thread_identify() ;
    rtp_strcpy(*name, TxThread->tx_thread_name);
    return(0);
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
    rtp_strcpy(*name, ((TX_THREAD *)(&handle))->tx_thread_name);
    return(0);
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
    tx_thread_sleep(msecs) ;
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
//  poll_os_cycle();
//  
//#ifdef INCLUDE_POLLOS_COOPERATIVE
//  // callback to application so it can yield
//  // there are no events pending, so yield to the operating
//  // system; the operating system should return when a send
//  // complete or receive interrupt occurs or when a time
//  // period has elapsed with no interrupts occurring;
//  // the time period can be as much as one second, but
//  // ideally 100 msec of inactivity should cause the
//  // stack to run again
//  rtp_thread_yield_cooperative();
//#endif
    tx_thread_relinquish() ;

    
}

/*----------------------------------------------------------------------*
                            rtp_threads_init_cooperative
 *----------------------------------------------------------------------*/
/** @memo   Prepare for use of the RTPlatform thread API.

    @doc    Prepare for use of the RTPlatform thread API.

    @return 0 on success, -1 otherwise.
 */
int rtp_threads_init_cooperative(void)
{
    return 0 ;  
}


/*----------------------------------------------------------------------*
                           rtp_threads_shutdown_cooperative
 *----------------------------------------------------------------------*/
/** @memo   Shutdown the RTPlatform thread API.

    @doc    When use of the RTPlatform thread API is no longer 
    needed, this function clears and/or releases any allocated 
    resources setup by the rtp_threads_init function call.

    @return void
 */
void rtp_threads_shutdown_cooperative (void)
{
    if (rtp_threads_up_cooperative > 0)
    {
        rtp_threads_up_cooperative --;
    }
}


/*----------------------------------------------------------------------*
                            rtp_thread_spawn_cooperative
 *----------------------------------------------------------------------*/
/** @memo   Spawn a thread.

    @doc    Spawn a thread and sets its priority level.

    @return 0 on success, -1 on failure, and -2 on non-fatal
    error.  If a -1 is returned the value of *newThread is 
    undefined.  An example of a non-fatal error would be if 
    the thread was spawned but the priority level could not 
    be set.
 */
int rtp_thread_spawn_cooperative (
  RTP_HANDLE  * newThread,              /** Storage location for the handle to the new thread. */
  RTP_ENTRY_POINT_FN entryPoint,        /** A function pointer to the threads entry point. */
  const char  * name,                   /** The name of the thread. */
  int           stackSizeIndex,         /** An index into the array of possible stack sizes. */
  int           priorityIndex,          /** An index into the array of possible priorities. */
  void        * userData                /** User data to be passed to the entry point function. */
  )
{

return 0 ;
   
//int           stackSize;
//int             priority;
//int           counter;
//unsigned int  stkTop;
//RTP_HANDLE        old_level;
//
//  RTIP_DEBUG_ERROR("!!!! Thread spawned as a task", 0, 0, 0);
//  old_level = rtp_irq_push_disable_cooperative();
//    if (!rtp_threads_up_cooperative)              /* initialize threads */
//    {
//      rtp_threads_init();
//      rtp_threads_up_cooperative++;
//    }
//  rtp_irq_pop_cooperative(old_level);
//                                      /* extract the exact stack size from the table */
//  
//  stackSize = stackSizeMap[stackSizeIndex];
//    priority  = threadPriorityMap[priorityIndex];
//    
//  //rtp_irq_disable();
//    
//    *newThread = (long)RTKCreateThread((RTKThreadFunction)entryPoint,
//                                  priority,
//                                  stackSize,
//                                  TF_NO_MATH_CONTEXT,
//                                  userData,
//                                  (char *)name);
//    
//  //rtp_irq_enable();
//    
//    if (*newThread)
//    {
//   //     RTKSetUserData((struct rtkTCBType *)*newThread,0,userData);
//        return(0);
//    }
//
//    return(-1);

}


/*----------------------------------------------------------------------*
                            rtp_thread_handle_cooperative
 *----------------------------------------------------------------------*/
/** @memo   Get the current thread handle.

    @doc    Retrieves the handle to the current thread.

    @return 0 on success, -1 otherwise.
 */
int rtp_thread_handle_cooperative(
  RTP_HANDLE * currentThread            /** Storage location for the thread handle. */
  )
{


    currentThread = (RTP_HANDLE *) OSRUN_curProcess();

    
    if(currentThread)
    {
        return(0);
    }   

    return(-1);
}


/*----------------------------------------------------------------------*
                          rtp_thread_user_data_cooperative
 *----------------------------------------------------------------------*/
/** @memo   Gets the user data associated with the current thread.

    @doc    Retrieves the user data from the current thread.

    @return 0 on success, -1 otherwise.
 */
int rtp_thread_user_data_cooperative (
  void ** userData                      /** Storage location for the user data. */
  )
{
    return 0 ;
//  RTP_HANDLE *currentTask;
//  
//  currentTask = (RTP_HANDLE *) RTKCurrentTaskHandle();
//  *userData =  ((RTKTaskHandle) *currentTask)->Parameter;
//  
//  if (*userData)
//  {
//      return(0);
//    }
//  
//  return (-1);

}


/*----------------------------------------------------------------------*
                       rtp_thread_user_data_by_handle_cooperative
 *----------------------------------------------------------------------*/
/** @memo   Gets the user data associated with the thread handle.

    @doc    Retrieves the user data from the thread with the 
    associated thread handle.

    @return 0 on success, -1 otherwise.
 */
int rtp_thread_user_data_by_handle_cooperative(
  RTP_HANDLE handle,                    /** Handle to the thread to find its user data. */
  void ** userData                      /** Storage location for the user data. */
  )
{
    return 0 ;
//    *userData = (void *)RTKGetUserData((struct rtkTCBType *)handle,0);
//    
//    if (*userData)
//    {
//        return(0);
//    }
//    return (-1);

}


/*----------------------------------------------------------------------*
                              rtp_thread_name
 *----------------------------------------------------------------------*/
/** @memo   Gets the name associated with the current thread.

    @doc    Retrieves the name from the current thread.

    @return 0 on success, -1 otherwise.
 */
int rtp_thread_name_cooperative(
  char ** name                          /** Storage location for the name. */
  )
{

    return 0 ;
//
//  rtp_strcpy ((char *)*name,(const char *) RTKGetTaskName(RTKCurrentTaskHandle()));
//  
//  if (*name)
//  {
//      return(0);
//  }
//  
//  return(-1);

}


/*----------------------------------------------------------------------*
                          rtp_thread_name_by_handle_cooperative
 *----------------------------------------------------------------------*/
/** @memo   Gets the name associated with the thread handle.

    @doc    Retrieves the name from the thread with the 
    associated thread handle.

    @return 0 on success, -1 otherwise.
 */
int rtp_thread_name_by_handle_cooperative (
  RTP_HANDLE handle,                    /** Handle to the thread to find its name. */
  char ** name                          /** Storage location for the name. */
  )
{

    return 0 ;
//    if (handle)
//  {
//      rtp_strcpy (*name, (const char *)RTKGetTaskName((struct rtkTCBType *)handle));
//      return(0);
//  }
//  
//  return(-1);

}


/*----------------------------------------------------------------------*
                             rtp_thread_sleep_cooperative
 *----------------------------------------------------------------------*/
/** @memo   Put the current thread to sleep.

    @doc    Suspend the current thread for the millisecond 
    timeout period.

    @return void
 */
void rtp_thread_sleep_cooperative(
  long msecs                            /** Timeout value in milliseconds. */
  )
{

    long noTicks;

    if (!msecs)
    {
        return;
    }
    
    noTicks = (msecs * rtp_get_ticks_p_sec())/1000;    

    if (noTicks < 1)
    {
      noTicks = 1;
    }
    

    OSRUN_Sleep(noTicks);   

}


/*----------------------------------------------------------------------*
                             rtp_thread_yield_cooperative
 *----------------------------------------------------------------------*/
/** @memo   Yield the current thread to any other of equal priority.

    @doc    Check if any other threads are attempting to run that 
    are of equal priority.  If there is one of equal proritey ready
    to run, let it, otherwise the yield returns and the current 
    thread continues.

    @return void
 */
void rtp_thread_yield_cooperative(void)
{
    rtp_thread_sleep_cooperative(1);
}


rtp_thread_context * rtp_get_current_task_context()
{
    rtp_thread_context *temp;
    char * name;
    TX_THREAD * TxThread ;
    int cmp;
    TxThread = tx_thread_identify() ;
    name = TxThread->tx_thread_name ;
    
    temp = allTrack;

    if(temp)
    {
        do
        {
            cmp = rtp_strcmp(temp->name,name);
            if ((temp->state == RTP_THREAD_IS_RUNNING) &&
                (cmp==0))
            {
                return(temp);
            }

            temp = (rtp_thread_context *)temp->next;
            if(!temp)
            {
                temp = allTrack;
            }
        } while (temp != allTrack);
    }
    
    return(0);
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
