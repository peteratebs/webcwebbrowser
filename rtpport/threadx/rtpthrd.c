 /*
 | RTPTHRD.C - Runtime Platform Thread Services
 |
 | EBSnet - EBS_RT-Platform 
 |
 |  $Author: shane $
 |  $Date: 2004/10/05 18:51:31 $
 |  $Name:  $
 |  $Revision: 1.3 $
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
#include <string.h> // Oasis
#include <stdio.h> // Oasis
#include "rtpport.h"
#include "rtpdebug.h"
#include "rtipapi.h"
#include "rtpmem.h"
// #include "rtk32.h" // Oasis
#include "dc_type.h"
#define TX_DISABLE_ERROR_CHECKING 1
#include "tx_api.h" // Oasis
#include "dc_return_codes.h" // Oasis
#include "dc_net_setup_api.h" // Oasis
#include "dc_log_api.h" // Oasis
#include "dc_time_api.h" // Oasis
#include "dc_tls_api.h"        // Thread Local Storage API

/************************************************************************
* Defines
************************************************************************/

#define RTP_SIZESTACK_HUGE      RTP_CFG_SIZESTACK_HUGE
#define RTP_SIZESTACK_BIG       RTP_CFG_SIZESTACK_BIG
#define	RTP_SIZESTACK_NORMAL    RTP_CFG_SIZESTACK_NORMAL
#define	RTP_SIZESTACK_SMALL     RTP_CFG_SIZESTACK_SMALL

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
    int                         state;
    struct rtp_thread_context   *next;
    int                         priority;
    char                        name[CFG_NAME_SIZE];
	void                        (*entryPoint)(RTP_ENTRY_POINT_FN_PARAM);
	void                        *userData;

	int                         *stkBase;			
	int                         stkSize;
}
rtp_thread_context;

/************************************************************************
* Data
************************************************************************/
unsigned int TLS_ContextDataId;

#define RTP_CFG_N_STACKS_SMALL_LC 5

//int HugeStacksArray[RTP_CFG_N_STACKS_HUGE][RTP_SIZESTACK_HUGE];
//int BigStacksArray[RTP_CFG_N_STACKS_BIG][RTP_SIZESTACK_BIG];
//int NormalStacksArray[RTP_CFG_N_STACKS_NORMAL][RTP_SIZESTACK_NORMAL];
//int SmallStacksArray[RTP_CFG_N_STACKS_SMALL_LC][RTP_SIZESTACK_SMALL];
static char SmallStacksArray1[RTP_SIZESTACK_SMALL];
static char SmallStacksArray2[RTP_SIZESTACK_SMALL];
static char SmallStacksArray3[RTP_SIZESTACK_SMALL];
static char SmallStacksArray4[RTP_SIZESTACK_SMALL];
static char SmallStacksArray5[RTP_SIZESTACK_SMALL];

static int smallStackAddrs[RTP_CFG_N_STACKS_SMALL_LC];

const unsigned int stackSizeMap[RTP_CFG_N_STACKS_SMALL_LC] = {
                               RTP_SIZESTACK_SMALL,
							   RTP_SIZESTACK_SMALL, //RTP_SIZESTACK_NORMAL,
							   RTP_SIZESTACK_SMALL, //RTP_SIZESTACK_NORMAL,
							   RTP_SIZESTACK_SMALL, //RTP_SIZESTACK_BIG,
							   RTP_SIZESTACK_SMALL //RTP_SIZESTACK_HUGE
                             };


// EBSnet  -> 0 = LOWEST  / 31 = HIGHEST
// ThreadX -> 0 = HIGHEST / 31 = LOWEST
const int threadPriorityMap[32] = { 
                                 DC_THREAD_PRIORITY_MEDIUM, //DC_THREAD_PRIORITY_LOW,
                                 DC_THREAD_PRIORITY_MEDIUM, //DC_THREAD_PRIORITY_LOW,
                                 DC_THREAD_PRIORITY_MEDIUM, //DC_THREAD_PRIORITY_LOW,
                                 DC_THREAD_PRIORITY_MEDIUM, //DC_THREAD_PRIORITY_LOW,
                                 DC_THREAD_PRIORITY_MEDIUM, //DC_THREAD_PRIORITY_LOW,
                                 DC_THREAD_PRIORITY_MEDIUM, //DC_THREAD_PRIORITY_LOW,
                                 DC_THREAD_PRIORITY_MEDIUM,
                                 DC_THREAD_PRIORITY_MEDIUM,
                                 DC_THREAD_PRIORITY_MEDIUM,
                                 DC_THREAD_PRIORITY_MEDIUM,
                                 DC_THREAD_PRIORITY_MEDIUM,
                                 DC_THREAD_PRIORITY_MEDIUM,
                                 DC_THREAD_PRIORITY_MEDIUM,
                                 DC_THREAD_PRIORITY_MEDIUM,
                                 DC_THREAD_PRIORITY_MEDIUM,
                                 DC_THREAD_PRIORITY_MEDIUM,
                                 DC_THREAD_PRIORITY_MEDIUM,
                                 DC_THREAD_PRIORITY_MEDIUM,
                                 DC_THREAD_PRIORITY_MEDIUM,
                                 DC_THREAD_PRIORITY_MEDIUM,
                                 DC_THREAD_PRIORITY_MEDIUM,
                                 DC_THREAD_PRIORITY_MEDIUM,
                                 DC_THREAD_PRIORITY_MEDIUM,
                                 DC_THREAD_PRIORITY_MEDIUM,
                                 DC_THREAD_PRIORITY_MEDIUM,
                                 DC_THREAD_PRIORITY_MEDIUM,
                                 DC_THREAD_PRIORITY_HIGH,
                                 DC_THREAD_PRIORITY_HIGH,
                                 DC_THREAD_PRIORITY_HIGH,
                                 DC_THREAD_PRIORITY_HIGH,
                                 DC_THREAD_PRIORITY_HIGH,
                                 DC_THREAD_PRIORITY_HIGH,
                               };

//rtp_thread_context  hugeContexts[RTP_CFG_N_STACKS_HUGE] = {0};
//rtp_thread_context  bigContexts[RTP_CFG_N_STACKS_BIG] = {0};
//rtp_thread_context  normalContexts[RTP_CFG_N_STACKS_NORMAL] = {0};
rtp_thread_context  smallContexts[RTP_CFG_N_STACKS_SMALL_LC] = {0};

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
rtp_thread_context *rtp_get_current_task_context(void);
rtp_thread_context *rtp_get_task_context(RTP_THREAD handle);
static UINT32 _rtp_threadx_priority_get(DC_THREAD_PRIORITY priority);
static RTP_THREAD timerThread; // Oasis

/************************************************************************
* Function Bodies
************************************************************************/
RTP_THREAD dc__net_thread_timer_get(void)
{
    return timerThread;
}

RTP_THREAD dc__net_thread_iface_get(UINT32 index)
{
    //return ifaceThread[index];
    return timerThread;
}

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

    smallStackAddrs[0] = (int)&SmallStacksArray1[0];
    smallStackAddrs[1] = (int)&SmallStacksArray2[0];
    smallStackAddrs[2] = (int)&SmallStacksArray3[0];
    smallStackAddrs[3] = (int)&SmallStacksArray4[0];
    smallStackAddrs[4] = (int)&SmallStacksArray5[0];

    dc_log_printf("rtp_thr_init");

    dc_log_printf("ssaddr1: %x", smallStackAddrs[0]);
    dc_log_printf("ssaddr2: %x", smallStackAddrs[1]);
    dc_log_printf("ssaddr3: %x", smallStackAddrs[2]);
    dc_log_printf("ssaddr4: %x", smallStackAddrs[3]);
    dc_log_printf("ssaddr5: %x", smallStackAddrs[4]);

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
#if (RTP_CFG_N_STACKS_SMALL_LC > 0)
    /* setup the small stacks */
    for(counter = 0; counter < RTP_CFG_N_STACKS_SMALL_LC; counter++)
    {
        smallContexts[counter].state = RTP_THREAD_IS_FREE;
        smallContexts[counter].stkBase = (int *) smallStackAddrs[counter];
        smallContexts[counter].stkSize = RTP_SIZESTACK_SMALL;
        dc_log_printf("adding sm ctx[%d]: %x", counter, &smallContexts[counter]);
        dc_log_printf("ctx stkbase[%d]: %x", counter, smallContexts[counter].stkBase);
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
void rtp_threads_shutdown(void)
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
int rtp_thread_spawn(RTP_THREAD  *newThread,
                     RTP_ENTRY_POINT_FN entryPoint,
                     const char *name, 
                     int stackSizeIndex, 
                     int priorityIndex, 
                     void *userData)
{
    int status, counter;   
    UINT32 stackSize, priorityVal;
    int stkTop;
    rtp_thread_context *tc = 0;

#if (RTP_THREAD_DEBUG_CREATE)
    static int  countSmall = 0;
    static int  countNormal = 0;
    static int  countBig = 0;
    static int  countHuge = 0;
#endif

    dc_log_printf("--> RTIP SPAWN!!! <--");

	rtp_irq_disable();

    if (!rtp_threads_up)				/* initialize threads */
    {
        rtp_threads_init();
        rtp_threads_up++;
    }

    /* extract the exact stack size from the table */
    if((stackSizeIndex >= 0) && (stackSizeIndex <= sizeof(stackSizeMap))) {
        stackSize = stackSizeMap[stackSizeIndex];
        dc_log_printf("rtp_thr_spawn: stackSz[%d] --> 0x%x", stackSizeIndex, stackSize);
    }
    else {
        stackSize = stackSizeMap[0];
        //dc_log_printf("ERR stackSzIndex OUT OF RANGE!!! [%d]", stackSizeIndex);
        dc_log_printf("rtp_thr_spawn: stackSz --> %d", stackSize);
    }

    /* find a stack for the task */
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

    dc_log_printf("removed tc addr: %x", tc);
    dc_log_printf("tc->stkBase: %x", tc->stkBase);

    /* extract the exact thread priority from the table */
    if((priorityIndex >= 0) && (priorityIndex <= sizeof(threadPriorityMap))) {
        priorityVal = _rtp_threadx_priority_get((DC_THREAD_PRIORITY)threadPriorityMap[priorityIndex]);
        dc_log_printf("rtp_thr_spawn: priorityVal[%d] --> %d", priorityIndex, priorityVal);
    }
    else {
        priorityVal = _rtp_threadx_priority_get(DC_THREAD_PRIORITY_MEDIUM);
        dc_log_printf("ERR priorityIndex OUT OF RANGE!!!");
        dc_log_printf("rtp_thr_spawn: priorityVal[?] --> %d", priorityVal);
    }

    _rtp_addSingleList((void **)&allTrack, tc);

    rtp_irq_enable();
	  
    if(!tc)
    {
        dc_log_printf("ERROR tc NULL!");
        return(-1);
    }

    tc->state = RTP_THREAD_STARTING;
    
    if(name)
    {   
        dc_log_printf("name: %s", name);
        rtp_strncpy(tc->name, name, CFG_NAME_SIZE-1);
        dc_log_printf("tc->name: %s", tc->name);
    }
	  
	tc->entryPoint = entryPoint;
	tc->userData = userData;

    /* Get the stack top as an even aligned offset from the base   */
    stkTop = (int)(tc->stkBase);

    rtp_irq_disable();

    tc->priority = priorityVal;
    tc->state = RTP_THREAD_IS_RUNNING;

    *newThread = (RTP_THREAD)_rtp_malloc(sizeof(TX_THREAD));
    dc_log_printf("RTIP newThread - val:0x%x,addr:0x%x", *newThread, newThread);
    dc_log_printf("stackTop: 0x%x", stkTop);
    dc_log_printf("entryPoint: 0x%x", entryPoint);

    if(newThread) {
        
        status = tx_thread_create((TX_THREAD *)(*newThread), (char *)name, entryPoint, 
                                  (unsigned long)(*(int *)userData), (void *)stkTop, stackSize, priorityVal, 
                                  priorityVal, TX_NO_TIME_SLICE, TX_AUTO_START);
        
    }
    else {
        dc_log_printf("ERROR: newThread NULL!");
    }

    dc_log_printf("RTIP ThrStatus - %x", status);

    if (status == TX_SUCCESS)
    {
        rtp_irq_enable();
        return (0);
    }

    tc->state = RTP_THREAD_STARTING;
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
    
    return (-1);
}

/*----------------------------------------------------------------------*
                            rtp_thread_kill
 *----------------------------------------------------------------------*/
int rtp_thread_kill(RTP_THREAD *newThread)
{
    int status;

#if 0
    rtp_thread_context *temp, *tc;
    char *name;
    int cmp;
    unsigned char found;
    int stackSize;

    if(!newThread) {
        return(-1);
    }

    name = ((TX_THREAD *)(*newThread))->tx_thread_name;
    temp = allTrack;
    found = 0;

    if(temp)
    {
        do
        {
            cmp = rtp_strcmp(temp->name,name);
            if ((temp->state == RTP_THREAD_IS_RUNNING) &&
                (cmp==0))
            {
                found = 1;
                break;
            }

            temp = (rtp_thread_context *)temp->next;
            if(!temp)
            {
                temp = allTrack;
            }
        } while (temp != allTrack);
    }

    if(!found) {
        return(-1);
    }

    stackSize = temp->stkSize;
    temp->state = RTP_THREAD_STARTING;

    /* Clear the stack */
    rtp_irq_disable();

    tc = _rtp_removeSingleList((void **)&temp);

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
#endif

    status = tx_thread_delete((TX_THREAD *)(*newThread));

    if (status == TX_SUCCESS) {
        return(0);
    }

    return(-1);
}


/*----------------------------------------------------------------------*
                            rtp_thread_handle
 *----------------------------------------------------------------------*/
/** @memo   Get the current thread handle.

    @doc    Retrieves the handle to the current thread.

    @return 0 on success, -1 otherwise.
 */
int rtp_thread_handle (
  RTP_THREAD * currentThread            /** Storage location for the thread handle. */
  )
{
    *currentThread = (RTP_THREAD)tx_thread_identify();
	return(0);
}

/*----------------------------------------------------------------------*
                            rtp_thread_get_current
 *----------------------------------------------------------------------*/

RTP_THREAD rtp_thread_get_current(void)
{
    RTP_THREAD currentThread;

    rtp_thread_handle(&currentThread);
    return currentThread;
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
    //*userData = dc_tls_access(TLS_ContextDataId) ; // Oasis
    //return 0;
    
    rtp_thread_context *currentContext;
    
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
  RTP_THREAD handle,                    /** Handle to the thread to find its user data. */
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
    
    return(-1);
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
    TX_THREAD *TxThread;

    TxThread = tx_thread_identify();

    if(TxThread != TX_NULL) {
        rtp_strcpy(*name, TxThread->tx_thread_name);
        return(0);
    }
    else {
        dc_log_printf("ERR: rtp_thread_name - txThread NULL!");
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
  RTP_THREAD handle,                    /** Handle to the thread to find its name. */
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
    UINT32 ticks, utime=1;
    UINT ret;
    
    dc_utime_get_from_tick(&utime);

    /* compute number of time slices to sleep */
    if (msecs > 0) {
        ticks = (msecs*1000)/utime;
        
        ret = tx_thread_sleep(ticks);
        if (TX_SUCCESS != ret) {
            dc_log_printf("CAN'T SLEEP %x", ret);
        }
    }
    else {
        tx_thread_relinquish();
    }
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
    tx_thread_relinquish();
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
    return(-1);
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
        rtp_threads_up_cooperative--;
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
    return(-1);
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
    return(-1);
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
    return(-1);
}


/*----------------------------------------------------------------------*
                       rtp_thread_name_cooperative
 *----------------------------------------------------------------------*/
/** @memo   Gets the name associated with the current thread.

    @doc    Retrieves the name from the current thread.

    @return 0 on success, -1 otherwise.
 */
int rtp_thread_name_cooperative(
  char ** name                          /** Storage location for the name. */
  )
{
    return(-1);
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
    return(-1);
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
    return;
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


/************************************************************************
* Internal utility functions
************************************************************************/

rtp_thread_context *rtp_get_task_context(RTP_THREAD handle)
{
    rtp_thread_context *temp;
    char *name;
    int cmp;
   
    if(handle == 0) {
        return(0);
    }

    name = ((TX_THREAD *)(&handle))->tx_thread_name;
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

rtp_thread_context *rtp_get_current_task_context()
{
    rtp_thread_context *temp;
    char *name;
    TX_THREAD *TxThread;
    int cmp;
    
    TxThread = tx_thread_identify();

    if(TxThread == TX_NULL) {
        return(0);
    }

    name = TxThread->tx_thread_name;
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

//extern RTP_THREAD dc__net_thread_timer_get(void);
//extern RTP_THREAD dc__net_thread_iface_get(UINT32 index);
DC_RETURN_CODE dc_net_thread_get(char *name, 
                                 TX_THREAD **thread)
{
#if 0
    DC_RETURN_CODE result=DC_BAD_ARGUMENT;

    if (!name || !thread)
    {
        return DC_BAD_ARGUMENT;
    }
   
    if (!strcmp(name, DC_NET_TIMER_THREAD_NAME))
    {
        *thread = (TX_THREAD *)dc__net_thread_timer_get();
        dc_log_printf("timerThread %#x %#x", dc__net_thread_timer_get(), thread);
        result = DC_SUCCESS;
    }
    else 
    {
        int i;

        for (i=0; i<CFG_NIFACES; i++)
        {
            char ip_name[24];

            sprintf(ip_name, "%s%d", DC_NET_INTERFACE_THREAD_NAME, i);
            if (!strcmp(name, ip_name))
            {
                *thread = (TX_THREAD *) (dc__net_thread_iface_get(i));
                result = DC_SUCCESS;
                dc_log_printf("ifaceThread %#x %#x", dc__net_thread_iface_get(i), thread);
                break;
            }
        }
    }

    return result;
#endif
}

static UINT32 _rtp_threadx_priority_get(DC_THREAD_PRIORITY priority)
{
    UINT32 tx_priority = 0;

    switch(priority) {
        case DC_THREAD_PRIORITY_LOW:
            tx_priority = 20;
            break;
        case DC_THREAD_PRIORITY_MEDIUM:
            tx_priority = 16;
            break;
        case DC_THREAD_PRIORITY_HIGH:
            tx_priority = 10;
            break;
        default:
            dc_log_printf("ERROR: _rtp_threadx_priority_get");
            tx_priority = 16;
            break;
    }

    return tx_priority;
}

/* ----------------------------------- */
/*             END OF FILE             */
/* ----------------------------------- */
