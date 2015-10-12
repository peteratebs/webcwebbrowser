/*********************************************************************
*  
*   IAR PowerPac
*
*   (c) Copyright IAR Systems 2008.  All rights reserved.
*
**********************************************************************
----------------------------------------------------------------------
File    : IP_OS_RTOS.c
Purpose : Kernel abstraction for RTOS
          Do not modify to allow easy updates !
--------  END-OF-HEADER  ---------------------------------------------
*/


#include "IP_Int.h"
#include "RTOS.h"

/*********************************************************************
*
*       Data types
*
**********************************************************************
*/
typedef struct TCP_WAIT {
  struct TCP_WAIT * pNext;
  struct TCP_WAIT * pPrev;
  void            * pWaitItem;
  OS_EVENT        Event;
} TCP_WAIT;

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static OS_EVENT _EventNet;
static OS_EVENT _EventRx;
static OS_RSEMA _Lock;
static TCP_WAIT * _pTCPWait;

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/


/**********************************************************
*
*       IP_OS_Init
*
*  Function description
*    Initialize (create) all objects required for task syncronisation.
*    These are 2 events (for IP_Task and RxTask)
*    and one semaphore for protection of critical code which  may not be
*    executed from multiple task at the same time.
*/
void IP_OS_Init(void) {
  OS_EVENT_Create(&_EventNet);
  OS_EVENT_Create(&_EventRx);
  OS_CREATERSEMA(&_Lock);
}

/*********************************************************************
*
*       IP_OS_DisableInterrupt
*/
void IP_OS_DisableInterrupt(void) {
  OS_IncDI();
}

/*********************************************************************
*
*       IP_OS_EnableInterrupt
*/
void IP_OS_EnableInterrupt(void) {
  OS_DecRI();
}

/*********************************************************************
*
*       IP_OS_GetTime32()
*
*  Function description
*    Return the current system time in ms.
*    The value will wrap around after app. 49.7 days. This is taken into
*    account by the stack.
*/
U32  IP_OS_GetTime32(void) {
  return OS_GetTime32();
}

/*********************************************************************
*
*       IP_OS_Delay
*
*  Function description
*    Blocks the calling task for a given time.
*/
void IP_OS_Delay(unsigned ms) {
  OS_Delay(ms + 1);
}

/*********************************************************************
*
*       IP_OS_Lock
*
*  Function description
*    The stack requires a single lock, typically a resource semaphore or mutex.
*    This function locks this object, guarding sections of the stack code
*    against other threads.
*    If the entire stack executes from a single task, no functionality is required here.
*/
void IP_OS_Lock(void) {
  OS_Use(&_Lock);
}

/*********************************************************************
*
*       IP_OS_Unlock
*
*  Function description
*    Unlocks the single lock used locked by a previous call to IP_OS_Lock().
*/
void IP_OS_Unlock(void) {
  OS_Unuse(&_Lock);
}

/*********************************************************************
*
*       IP_OS_WaitNetEvent
*
*  Function description
*    Called from IP_Task() only.
*    Blocks until the timeout expires or a NET-event occurs,
*    meaning IP_OS_SignalNetEvent() is called from an other task or ISR.
*/
void IP_OS_WaitNetEvent(unsigned ms) {
  OS_EVENT_WaitTimed(&_EventNet, ms + 1);
}

/*********************************************************************
*
*       IP_OS_SignalNetEvent
*
*  Function description
*    Wakes the IP_Task if it is waiting for a NET-event or timeout in
*    the fucntion IP_OS_WaitNetEvent().
*/
void IP_OS_SignalNetEvent(void) {
  OS_EVENT_Set(&_EventNet);
}

/*********************************************************************
*
*       IP_OS_WaitRxEvent
*/
void IP_OS_WaitRxEvent(void) {
  OS_EVENT_Wait(&_EventRx);
}

/*********************************************************************
*
*       IP_OS_SignalRxEvent
*/
void IP_OS_SignalRxEvent(void) {
  OS_EVENT_Set(&_EventRx);
}

/*********************************************************************
*
*       IP_OS_WaitItemTimed()
*
*  Function description
*    Suspend a task which needs to wait for a object.
*    This object is identified by a pointer to it and can be of any type, e.g. socket.
*
*  Context
*    Function is called from an application task.
*/
void IP_OS_WaitItemTimed(void * pWaitItem, unsigned Timeout) {
  TCP_WAIT TCPWait;

  //
  // Create the wait object which contains the OS-Event object
  //
  TCPWait.pPrev     = NULL;
  TCPWait.pWaitItem = pWaitItem;
  OS_EVENT_Create(&TCPWait.Event);
  //
  // Add to doubly-linked list with temp. disabled interrupts
  //
  IP_OS_DisableInterrupt();
  TCPWait.pNext = _pTCPWait;
  _pTCPWait = &TCPWait;
  if (TCPWait.pNext) {
    TCPWait.pNext->pPrev = &TCPWait;
  }
  IP_OS_EnableInterrupt();
  //
  // Unlock mutex and suspend this task
  //
  IP_OS_Unlock();
  if (Timeout == 0) {
    OS_EVENT_Wait(&TCPWait.Event);
  } else {
    OS_EVENT_WaitTimed(&TCPWait.Event, Timeout);
  }
  //
  // Remove it from doubly linked list with temp. disabled interrupts
  //
  IP_OS_DisableInterrupt();
  if (TCPWait.pPrev) {
    TCPWait.pPrev->pNext = TCPWait.pNext;
  } else {
    _pTCPWait = TCPWait.pNext;
  }
  if (TCPWait.pNext) {
    TCPWait.pNext->pPrev = TCPWait.pPrev;
  }
  IP_OS_EnableInterrupt();
  //
  // Delete the event object & lock the mutex again
  //
  OS_EVENT_Delete(&TCPWait.Event);
  IP_OS_Lock();
}

/*********************************************************************
*
*       IP_OS_WaitItem()
*
*  Function description
*    Suspend a task which needs to wait for a object.
*    This object is identified by a pointer to it and can be of any type, e.g. socket.
*
*  Context
*    Function is called from an application task.
*/
void IP_OS_WaitItem(void * pWaitItem) {
  IP_OS_WaitItemTimed(pWaitItem, 0);
}

/*********************************************************************
*
*       IP_OS_SignalItem()
*
*  Context
*    Function is called from a task, not an ISR
*/
void IP_OS_SignalItem(void * pWaitItem) {
  TCP_WAIT * pTCPWait;
  OS_EnterRegion();                        // Make sure that adding to list and suspending is an atomic opration for tasks (interrupts allowed)
  for (pTCPWait = _pTCPWait; pTCPWait; pTCPWait = pTCPWait->pNext) {
    if (pTCPWait->pWaitItem == pWaitItem) {
      OS_EVENT_Set(&pTCPWait->Event);
    }
  }
  OS_LeaveRegion();
}

/*********************************************************************
*
*       IP_OS_AddTickHook()
*
*  Function description
*    Add tick hook. This is a function which is called from the tick handler, typically because the driver's
*    interrupt handler is not called via it's own hardware ISR. (We poll 1000 times per second)
*
*  Context
*    Function is called from a task, not an ISR
*/
void IP_OS_AddTickHook(void (* pfHook)(void)) {
#if OS_VERSION >= 36000
  static OS_TICK_HOOK _cb;
  OS_AddTickHook(&_cb, pfHook);
#else
  IP_PANIC("IP_OS_AddTickHook() requires an OS version >= 3.60");      // This requires a newer version of the OS.
#endif
}


/*************************** End of file ****************************/
