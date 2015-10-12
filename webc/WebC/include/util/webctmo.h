//
// WEBCTMO.H - Structs, prototypes, and defines for WEBCTMO.CPP
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//

#ifndef __WEBCTMO_H_INCLUDED__
#define __WEBCTMO_H_INCLUDED__

#include "webc.h"

#define WEBC_TIMEOUT_INFINITE 0x7fffffffL
#define WEBC_LOOP_FOREVER     -100
#define WEBC_TIMEOUT_INVALID  ((WEBC_UINT16)(-1))

#ifdef __cplusplus
extern "C" {
#endif
typedef int (*webcTimeoutFn)(void* data, long msecError);
#ifdef __cplusplus
}
#endif
#ifdef __cplusplus

struct TimeoutInfo
{
	struct TimeoutInfo* pNext;
	WEBC_UINT16         wId;
	webcTimeoutFn       Func;
	webcIdleFn          CancelFunc;
	void*               pData;
	WEBC_INT32          iMsecInterval;
	WEBC_UINT32         iMsecTimeout;
	long                iLoop;
	WEBC_UINT8          flags;
};

class WebcTimeoutManager
{
 public:
	WebcTimeoutManager (void);
	~WebcTimeoutManager (void);
	
	void        ProcessTime     (void);	
  
	WEBC_UINT16 SetTimeoutDebug (webcTimeoutFn func, 
	                             webcIdleFn cancelfn, 
	                             void* data, 
	                             WEBC_INT32 msec, 
	                             int loop, 
	                             const char* file, 
	                             int line);

	WEBC_UINT16 SetTimeout      (webcTimeoutFn func, 
	                             webcIdleFn cancelfn, 
	                             void* data, 
	                             WEBC_INT32 msec, 
	                             int loop);

	int         CancelTimeout   (int (*match)(TimeoutInfo* timeout, void* data),
                                 void* data,
                                 WEBC_BOOL cancelAll);
                               
	void        DeleteCancelled (void);
	
	WEBC_INT32  NextExpiration  (void);
	
 protected:

	int DeleteTimeout(TimeoutInfo *pCurrent, TimeoutInfo *pPrev, TimeoutInfo** list);
	void InsertTimeout(TimeoutInfo* info);
	void InsertNewTimeouts(void);

	TimeoutInfo* mpTimeoutList;
	TimeoutInfo* mpNewList;
	TimeoutInfo* mpCancelledList;
	WEBC_UINT16  mNextId;
	WEBC_BOOL    mInsideTickHandler;
	WEBC_INT32   miNumTimeouts;
};

#endif

#endif // __WEBCTMO_H_INCLUDED__
