//
// wcdom.CPP -
//
// EBS - WebC
//
// Copyright EBS Inc. , 2011
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//  [tbd]
//

/*****************************************************************************/
// Header files
/*****************************************************************************/


//#include "../include/NewApi.h"
#include "wcapi.h"

#include "dflow.hpp"
#include "helement.hpp"
#include "htmlbrow.hpp"
#include "webcassert.h"
#include "htmlfind.hpp"
#include "htmldoc.hpp"
#include "webcmem.h"
#include "rtptime.h"
#include "webvfile.hpp"
#include "rtpmem.h"

/*****************************************************************************/
// wcCtxtInit
/**
 @memo  Initialize a webC context for use.
 @doc   Initialize a webC context for use. This function must be called by any routines that were not passed a context pointer and
        wish to use the "wc" API. The routine must also call wcCtxtRelease before it returns.

		See the void TestDomApi(wcBROW browser, wcDOC doc)

        Note: It is usually unnecessary to call wcCtxtInit because most Event handlers, those that were hooked with wcElSetEventHandler,
        don't need to call wcCtxtInit because they are passed an initialized context block which is automatically released when the user's
        handler returns.

 @return		nothing

 @see			wcCtxtRelease
 */
/*****************************************************************************/

void wcCtxtInit(wcCtx* Ctx, wcBROW browser, wcDOC doc)
{
	rtp_memset(Ctx, 0, sizeof(*Ctx));
	Ctx->hdoc = doc;
	Ctx->hbrowser = browser;
}

/*****************************************************************************/
// wcCtxtRelease
/**

 @memo  Garbage collect resources associated with a webC context.
 @doc   Release resources associated with a webC context that was initialized by an earlier call to wctxtInit.
 		The resources that accumulate in the context block are mainly string buffers used to return data that has
		been converted from internal unicode form to Ascii.

 <pre>
         Note: It is usually unnecessary to call wcCtxtRelease because most Event handlers, those that were hooked with wcElSetEventHandler,
         because the event dispatch code automatically calls wcCtxtRelease after the user's callback function returns.
        handler returns.

        Note: wcCtxtRelease may be called multiple times, even in a routine that was passed a context structure and did not even call wcCtxtInit().
        Each wcCtxtRelease is called it frees all accumulated return buffers, typically this can wait until an event handler returns but if a handler
		is performing many many calls to the API in a loop it can force a garbage collect by calling wcCtxtRelease.
  </pre>

 @param  wcCtx*    		Ctx    		-   The wc context to release.

 @return		nothing

 @see			wcCtxtInit
 */
/*****************************************************************************/

void wcCtxtRelease(wcCtx* Ctx)
{
	for (int i = 0; i < MAXGARBAGEPOINTERS; i++)
	{
		if (Ctx->GarbageList[i])
			WEBC_FREE(Ctx->GarbageList[i]);
		Ctx->GarbageList[i]=0;
	}
	// Nothing yet.
}

char * wcCtxtAppendGarbage(wcCtx* Ctx, char *p)
{
	for (int i = 0; i < MAXGARBAGEPOINTERS; i++)
	{
		if (!Ctx->GarbageList[i])
		{
			Ctx->GarbageList[i] = p;
			return p;
		}
	}
	WEBC_ASSERT(0);
	return p;
}

/*****************************************************************************/
// wcAddFsTree
/**

 @memo Add a compiled image of a Windows subdirectory tree to webc's internal rom based file system.
 @doc webC provides a handy rom based file system form managing content. Subdirectories on a Windows based PC can be compiled into C structure declarations by the tool \Ref{makewebcfs} and then inserted into the webC intoernal rom file
 system with this call. wcAddFsTree may be called multiple timesallowing software subsystems to individually register their content, independently of other modules.

 @param  const struct WebcVFileEntry *TableName	-   The webc: file entry table name generated by \Ref{makewebcfs}.<br>
 <pre>
 Example:
 	You typed:
	&nbsp;makewebcfs -n=Examples \MyContent<br>

	This created the header files Examplesfiletable.h and Examplesfiledata.h.<br>

	Examplesfiledata.h contains the following declaration:<br>

	const struct WebcVFileEntry *Example[23] = { ...(if content had 22 files total..)<br>

	You edit a source file in your application startup sequence to include this line:<br>

	#include "Examplesfiletable.h"<br>

	Now add this call to your code so it executes before you try to access the files:<br>

	wcAddFsTree(Examples);

 </pre>
 @return 0 or -1 on failure.

 @see webcFsIntroduction makewebcfs ExDemonstrateWebcVirtualFiles
 */
/*****************************************************************************/
int wcAddFsTree(const struct WebcVFileEntry *TableName)
{
	return wload_WebcVirtualFileSystemTableInstall(TableName);
}

typedef struct _wcTimedCallbackRecord
{
	struct _wcTimedCallbackRecord* pNext;
	WEBC_UINT32			TriggerTime;
	wcTimedCallbackfn	pfn;
	int					IntParm;
	void *				VoidParm;
} wcTimedCallbackRecord;
wcTimedCallbackRecord *gPTimedCallbackRecords;

/*****************************************************************************/
// wcTimedCallback
/**

 @memo Wait a number of miliseconds and then call the user callback function.
 @doc Wait the specified number of miliseconds and then call the user callback function. The function is
 called outside the context of a browser so it may be used to do things that otherwise will not work in a browser context.
 An example of such an operation is allowing browser Event handler to queue a window close from its timer event handler.
 like close browser instructs the enumerator to continue scanning or stop.

 @param  wcTimedCallbackfn 	pfn    		-   User supplied callback function.
 @param  int 				Delay		-   Milliseconds to delay before calling. If zero called on the next clock.
 @param  int 				Intparm 	-   Integer (command perhaps) passed to the callback fucntion.
 @param  void*				VoidParm 	-   Void pointer pass by reference.

 <pre>
  The user callback function is of type:
  	typedef void (*wcTimedCallbackfn)(int UserIntParm, void *UserVoidParm);

 Examples:
 	Use a timed callback function to close a window from inside an elements HTML_EVENT_TIMER handler and Queue a restart.

	Callback function
  	void MyAppCallback(int UserIntParm, void *UserVoidParm)
	{
		if (UserIntParm == MYCLOSE)
		{
			wcWinClose((wcCtx 	*)UserVoidParm);
			Queue up a restart of the demo in one second.
			wcTimedCallback(MyCloseCallback, 1000, MYRESTART, (void *) UserVoidParm);
		}
		else if (UserIntParm == MYRESTART)
		{
			RestartMyDemo((wcCtx *)UserVoidParm);
		}
	}

	.. Event handler fragment
	else if (event->type == HTML_EVENT_TIMER)
	{
		pcontrol = (struct flyingwindowcontrol *) wcGetData(Ctx, element);
		pcontrol->iteration += 1;
		if (pcontrol->iteration >= FLYINGWINDOWITERATIONS)
		{
			wcPopData(Ctx, element);
			rtp_free(pcontrol);
			// Close the window on the next tick
			// Make a private copy of the stack based context to pass to the Global timer handler
			wcCtx* KillCtx;
			KillCtx = (wcCtx*)rtp_malloc(sizeof(*Ctx));
			*KillCtx = *Ctx;
			// Tell the global timer to call our close callback routine
			wcTimedCallback(MyAppCallback, 0, MYCLOSE, (void *) KillCtx);
			return (HTML_EVENT_CONTINUE);
		}
	}
 </pre>

 @return Nothing.

 @see
 */
/*****************************************************************************/

void wcTimedCallback(wcTimedCallbackfn pfn,int delay, int IntParm, void *VoidParm)
{
wcTimedCallbackRecord *newRecord;
	newRecord = (wcTimedCallbackRecord *)WEBC_MALLOC(sizeof(wcTimedCallbackRecord));
	if (!newRecord)
		return;
	rtp_memset(newRecord, 0, sizeof(*newRecord));
	newRecord->pfn 		= pfn;
	newRecord->TriggerTime	= rtp_get_system_msec()+(WEBC_UINT32)delay;
	newRecord->IntParm 	= IntParm;
	newRecord->VoidParm = VoidParm;
	// If delay is zero put the record on the front of the list. Required because if we queue from inside a callback we want to
	// Process it on the next tick, not this tick.
	if (delay == 0)
	{
		newRecord->pNext = gPTimedCallbackRecords;
		gPTimedCallbackRecords = newRecord;
	}
	else if (!gPTimedCallbackRecords)
		gPTimedCallbackRecords = newRecord;
	else
	{
	wcTimedCallbackRecord *RecordEnumerator = gPTimedCallbackRecords;
		while (RecordEnumerator->pNext)
			RecordEnumerator = RecordEnumerator->pNext;
		RecordEnumerator->pNext = newRecord;
	}
}
// Called from the system timer before it enumerates the browser specific timere
void wcProcessTimedCallbacks(void)
{
wcTimedCallbackRecord *RecordEnumerator = gPTimedCallbackRecords;
WEBC_UINT32 ThisTriggerTime;
	if (RecordEnumerator)
	{
		ThisTriggerTime	= rtp_get_system_msec();
		while (RecordEnumerator)
		{
			if (ThisTriggerTime >= RecordEnumerator->TriggerTime)
			{
				wcTimedCallbackRecord *thisRecord = RecordEnumerator;
				RecordEnumerator = RecordEnumerator->pNext;
				if (thisRecord == gPTimedCallbackRecords)
				{
					gPTimedCallbackRecords = RecordEnumerator;
				}
				else
				{
					wcTimedCallbackRecord *_RecordEnumerator = gPTimedCallbackRecords;
					while (_RecordEnumerator->pNext)
					{
						if (_RecordEnumerator->pNext==thisRecord)
						{
							_RecordEnumerator->pNext = thisRecord->pNext;
							break;
						}
						_RecordEnumerator=_RecordEnumerator->pNext;
					}
				}
				wcTimedCallbackfn	pfn = thisRecord->pfn;
				int					IntParm = thisRecord->IntParm;
				void *				VoidParm = thisRecord->VoidParm;
				WEBC_FREE(thisRecord);
				pfn(IntParm, VoidParm);
			}
			else
			{
				RecordEnumerator = RecordEnumerator->pNext;
			}
		}
	}
}

/* Called from the Display Manager Draw after the display been refreshed */
static wcBOOL SendDrawEvents(wcCtx* Ctx, wcEL El, int UserIntParm, void *UserVoidParm)
{
   	HTMLEvent the_event;
   	the_event.type = HTML_EVENT_WINDOW_REFRESHED;
	wcElTriggerEvent(El, &the_event, 0);
	return wcFALSE;
}

void wcDisplayManagerDrawCallback(DisplayManager *manager)
{
wcBROW browser;
HTMLBrowser  *pBrowser;
wcCtx _Ctx;

	browser = (wcBROW) webc_FindBrowserByDisplayManager((void *) manager);
	if (!browser)
		return;
	pBrowser = (HTMLBrowser  *) browser;
	wcCtxtInit(& _Ctx, (wcBROW) browser, (wcDOC) pBrowser->GetDocument());
	wcDocEnumerate(&_Ctx, SendDrawEvents, 0, 0);
}
