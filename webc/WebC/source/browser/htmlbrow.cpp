//
// HTMLBROW.CPP - Methods for class HTMLBrowser (htmldom.hpp)
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
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

#include "webcjhconfig.h"
#include "htmldom.hpp"
#include "webload.hpp"
#include "htmldoc.hpp"
#include "htmlbrow.hpp"
#include "webcstr.h"
#include "webcmem.h"
#include "webcassert.h"

#if (WEBC_SUPPORT_HTTP)
#include "httpc.h"
#endif // WEBC_SUPPORT_HTTP

#if (WEBC_SUPPORT_JSCRIPT)
#include "wjsbrowser.h"


struct JSMgrStruct
{
	WebcJSDocumentContext *data;
};
#endif // WEBC_SUPPORT_JSCRIPT

HTMLBrowser *gpFocusBrowser = 0;

#if (WEBC_SUPPORT_LOADING_ICON)
char gpLoadingBitmapUrl[] = "file://loading.gif";
WebcBitmap *gpLoadingBitmap = 0;
WebcBitmapCollection *gpLBCollection = 0;
#endif // WEBC_SUPPORT_LOADING_ICON

const WEBC_CHAR gDefaultStatusString[] = {0};

#if (WEBC_SUPPORT_HTTP)
int _BrowserTimeoutConnections (void* data, long msecError);
#endif // WEBC_SUPPORT_HTTP

/*****************************************************************************/
// Local data types
/*****************************************************************************/

/*****************************************************************************/
// Methods
/*****************************************************************************/

HTMLBrowser::HTMLBrowser(HTMLBrowserConfig *config, WEBC_BOOL CreateWindow, const WEBC_CHAR* windowName, const WEBC_CHAR* homePage)
{
	mpHomePage = 0;
	mbLoading = 0;
	mbDestroyed = 0;
	miDestroyStatus = 0;
	miLockCount = 1; // start out with 1 lock
	mpDocument = 0;
	mpPrivateData = config->privateData;
	vector_init(&mLoadQueue,sizeof(LoadJobInfo));
	vector_init(&mPrivateDataStack,sizeof(void *));
	mpDefaultStatus = 0;
	mpTempStatus = 0;
	mpTempStatusOwner = 0;
    WebGraphics *pGc = 0; // <EDIT
	mProcessInputQueueFn = config->processInputQueue;
	mHtmlGraphics = config->graphics;
	//mEventHandler = config->browserEventHandler;
	mEventHandler = (HTMLEventStatus (*)(HBROWSER_HANDLE, HTMLBrowserEvent *))config->browserEventHandler;
	if (!CreateWindow)
    {
		WEBC_ASSERT(config->WindowHandle != 0);
	    SetWindowHandle(config->WindowHandle);
    }

	//Implementation note:
	// When CreateWindow is requested, this exended method creates a window and allocates webC and GUI drawing contexts
	// and initializes fields in config with values retrieved from the GUI layer
	// The data member config->graphics can be zero for implementations (GDI) that create the graphics context "late"
	// i.e. at the time GUINAME_WindowOpen is called

	if (CreateWindow)
	{
        config->BrowserHandle = (HBROWSER_HANDLE) this;
         /* Call alternate WebGraphics() constructor that calls the GUI abstraction layer
            to create a new window and a new GC for that window
            The new GUI is returned in config->graphics */
        // Clear the browser window handle and the window handle field in the config structure
        // The GUI window opener will set these values for us
        config->WindowHandle = 0;
	    SetWindowHandle(config->WindowHandle);
         /* Note: mHtmlGraphics may be zero 0 for this call */
	    WEBC_NEW_VERBOSE(pGc, WebGraphics(config->graphics, config, 0 /**title*/),"WebGraphics");

         /* The GUI layer must set these two */
		WEBC_ASSERT(config->WindowHandle != 0);
		WEBC_ASSERT(GetWindowHandle() != 0);
        /* Update info that was returned from the GUI binding layer
           in the config structure */
	    mProcessInputQueueFn = config->processInputQueue;
	    mHtmlGraphics = config->graphics;
    }

	tc_memset(mFontCache, 0, sizeof(mFontCache));
	void* font;
	font = (*mHtmlGraphics)->getDefaultFont(mHtmlGraphics);
	if (font)
	{
		WebFont defFont = (WebFont) WEBC_MALLOC_VERBOSE(sizeof(s_WebFont),"DefaultFont");
		if (defFont)
		{
			defFont->font = font;
			defFont->height = (*mHtmlGraphics)->getFontHeight(mHtmlGraphics, font);
			WEBC_NEW_VERBOSE(mDefaultFont, WebGraphFont(defFont),"mDefaultFont");
			if (!mDefaultFont)
			{
				WEBC_FREE(defFont);
			}
		}
	}

  #if (WEBC_SUPPORT_USER_EVENTS)
	mTagEventHandlerArray = 0;
  #endif

	mGC = 0;
	WEBC_NEW_VERBOSE(mDisplayManager, DisplayManager(),"DisplayManager");

	if (mDisplayManager)
	{
        if (pGc) // <EDIT
		    mGC = pGc;
		else // <EDIT
		{
		    WEBC_NEW_VERBOSE(mGC, WebGraphics(config->graphics),"WebGraphics");
        }
		if (mGC)
		{
			WEBC_NEW_VERBOSE(mRootDocDisplay, DisplayManager(),"DisplayManager");

			if (mRootDocDisplay)
			{
				WebRect canvasRect;

				mGC->GetRect(&canvasRect);

				mRootDocDisplay->SetScrollMode(SCROLL_MODE_AUTO);

				mDisplayManager->SetGraphics(mGC);
				mDisplayManager->ClearManagerFlag(MANAGER_FLAG_BUFFER_SCROLL);
				mDisplayManager->SetRoot(mRootDocDisplay);
				mDisplayManager->Move(&canvasRect);

				// Now set the viewport and invalidate
				canvasRect.MoveTo(0,0);
				mDisplayManager->SetViewport(&canvasRect);
				mDisplayManager->Invalidate();
			}
			else
			{
				WEBC_DELETE(mGC);
				WEBC_DELETE(mDisplayManager);
				mGC = 0;
				mDisplayManager = 0;
			}
		}
		else
		{
			WEBC_DELETE(mDisplayManager);
			mDisplayManager = 0;
		}
	}

	// Initialize the root document for this browser
	WEBC_NEW_VERBOSE(mpDocument,HTMLDocument(),"HTMLDocument");
	if (mpDocument)
	{
		mpDocument->SetBrowser(this);
		mpDocument->Clear();
		mpDocument->SetDisplayManager(mRootDocDisplay);
	}
	/* Set the Body background color */
	if (config->WindowConfig.SetBodyBackGroundColor )
		mpDocument->Body()->SetBodyBackgroundColor(config->WindowConfig.BackGroundColor);


	mpHomePage = webc_malloc_string_copy(homePage, __FILE__, __LINE__);
	mpWindowName = webc_malloc_string_copy(windowName, __FILE__, __LINE__);
	SetDefaultStatus(gDefaultStatusString);
}


HTMLBrowser::~HTMLBrowser(void)
{
	// Release the browser focus if it is locked to this instance.
	//
	if (mGC)
	{ // Relase any exclusive claim to the pointer that this browser may have
		mGC->ReleasePointer();
	}
	if (mpDocument)
	{
		WEBC_DELETE(mpDocument);
		mpDocument = 0;
	}

 	// Delete the browser display element (do this AFTER deleting the
	//  HTMLDocument, so that the HTMLDocument's DisplayManager
	//  will still be valid, in case it uses it for something)
	if (mDisplayManager)
	{
		WEBC_DELETE(mDisplayManager);
		mDisplayManager = 0;
	}

	if (mRootDocDisplay)
	{
		WEBC_DELETE(mRootDocDisplay);
		mRootDocDisplay = 0;
	}

	webc_free_string_copy(mpHomePage, __FILE__, __LINE__);
	mpHomePage = 0;
	webc_free_string_copy(mpWindowName, __FILE__, __LINE__);
	mpWindowName = 0;

  #if (WEBC_SUPPORT_USER_EVENTS)
	if (mTagEventHandlerArray)
	{
		int tag;

		for (tag = 0; tag < HTML_NUM_TAGS; tag++)
		{
			if (mTagEventHandlerArray[tag])
			{
				WEBC_DELETE(mTagEventHandlerArray[tag]);
			}
		}

		WEBC_FREE(mTagEventHandlerArray);
	}
  #endif

	webc_free_string_copy(mpTempStatus, __FILE__, __LINE__);
	webc_free_string_copy(mpDefaultStatus, __FILE__, __LINE__);
	mpTempStatus = 0;
	mpDefaultStatus = 0;
	mpTempStatusOwner = 0;
// <EDIT 20080120 Move mGC->Destroy down below  (*mHtmlGraphics)->destroyFont()
//	if (mGC)
//	{
//		mGC->Destroy();
//	}

	// Destroy all cached fonts
	int n;
	for (n=0; n < WEBC_FONT_CACHE_SIZE; n++)
	{
		if (mFontCache[n])
		{
			WEBC_ASSERT(mFontCache[n]->UseCount() == 0);
			(*mHtmlGraphics)->destroyFont(mHtmlGraphics, mFontCache[n]->mFont->font);
			WEBC_FREE(mFontCache[n]->mFont);
			WEBC_DELETE(mFontCache[n]);
		}
	}

	if (mDefaultFont)
	{
		WEBC_FREE(mDefaultFont->mFont);
		WEBC_DELETE(mDefaultFont);
	}

	vector_delete(&mLoadQueue);
	vector_delete(&mPrivateDataStack);

	// Remove the browser from the active window chain
    webc_UnSetBrowserWindow(GetWindowHandle());

// <EDIT 20080120 Move mGC->Destroy down below  (*mHtmlGraphics)->destroyFont()
	if (mGC)
	{
		mGC->Destroy();
	}
}

void HTMLBrowser::SetScrollMode(SCROLL_MODE	ScrollMode)
{
	if (mRootDocDisplay)
		mRootDocDisplay->SetScrollMode(ScrollMode);
	if (mDisplayManager)
		mDisplayManager->SetScrollMode(ScrollMode);
}

void HTMLBrowser::Destroy(int iStatus)
{
	vector_iterator vi;
	LoadJobInfo* jobInfo = (LoadJobInfo*) vector_get_first(&mLoadQueue, &vi);
	while (jobInfo)
	{
		if (jobInfo->pJob)
		{
			jobInfo->pJob->Abort();
			if (!Loading())
			{
				WEBC_DELETE(jobInfo->pJob);
				jobInfo->pJob = 0;
			}
		}

		jobInfo = (LoadJobInfo*) vector_get_next(&vi);
	}

	if (!Loading())
	{
		vector_delete(&mLoadQueue);
	}
	vector_delete(&mPrivateDataStack);

	miDestroyStatus = iStatus;

	if (Loading() || (miLockCount > 0))
	{
		mbDestroyed = 1;
		return;
	}

	WEBC_DELETE(this);
}


int HTMLBrowser::Back(void)
{
	int status  = Go(WEBC_BROWSER_BACK);
	return (status);
}


int HTMLBrowser::Forward(void)
{
	int status = Go(WEBC_BROWSER_FORWARD);
	return (status);
}

int HTMLBrowser::Go(int index)
{
HistoryNode current,prev,next;
HTMLDocument *pDoc;

	if (index < 0)
	{
		int dist = -index;

		if (dist > mBackHistory.Top())
		{
			return (-1);
		}

		mBackHistory.Pop(&prev);

		pDoc = prev.pDoc;
		current.pDoc = pDoc;

		if (mpDocument->GetDisplayManager())
		{
			current.x = pDoc->GetDisplayManager()->mViewRect.left;
			current.y = pDoc->GetDisplayManager()->mViewRect.top;
		}

		UrlStringBuilder ubld(pDoc->CurrentURL());
		WEBC_CHAR *url_str = ubld.MallocString(__FILE__, __LINE__);
		if (url_str)
		{
			current.pHRef = webc_malloc_string_copy(url_str,__FILE__,__LINE__);
			ubld.FreeString(url_str, __FILE__, __LINE__);
		}
		else
		{
			current.pHRef = 0;
		}

		if (!current.pHRef)
		{
			return (-1);
		}

		mForwardHistory.Push(&current);
		current = prev;
		dist--;

		while (dist > 0)
		{
			mForwardHistory.Push(&current);
			mBackHistory.Pop(&current);
			dist--;
		}
	}
	else if (index > 0)
	{
		int dist = index;

		if (dist > mForwardHistory.Top())
		{
			return (-1);
		}

		mForwardHistory.Pop(&next);

		pDoc = next.pDoc;
		current.pDoc = pDoc;

		if (mpDocument->GetDisplayManager())
		{
			current.x = pDoc->GetDisplayManager()->mViewRect.left;
			current.y = pDoc->GetDisplayManager()->mViewRect.top;
		}

		UrlStringBuilder ubld(pDoc->CurrentURL());
		WEBC_CHAR *url_str = ubld.MallocString(__FILE__, __LINE__);
		if (url_str)
		{
			current.pHRef = webc_malloc_string_copy(url_str,__FILE__,__LINE__);
			ubld.FreeString(url_str, __FILE__, __LINE__);
		}
		else
		{
			current.pHRef = 0;
		}

		if (!current.pHRef)
		{
			return (-1);
		}

		mBackHistory.Push(&current);
		current = next;
		dist--;

		while (dist > 0)
		{
			mBackHistory.Push(&current);
			mForwardHistory.Pop(&current);
			dist--;
		}
	}
	else
	{
		return (0);
	}

	// now current is the location to load
	if (current.pHRef)
	{
		if (current.pDoc)
		{
			current.pDoc->SetSrc(current.pHRef, 0 /*baseUrl*/, current.x, current.y);
			current.pDoc->Update(1,0);
		}

		webc_free_string_copy(current.pHRef, __FILE__, __LINE__);
	}
	return (0);
}


WEBC_CHAR *HTMLBrowser::GetName(void)
{
	return (mpWindowName);
}

void HTMLBrowser::ChangeName(WEBC_CHAR *newName)
{
	webc_free_string_copy(mpWindowName, __FILE__, __LINE__);
	mpWindowName = webc_malloc_string_copy(newName, __FILE__, __LINE__);
}

void *HTMLBrowser::GetWindowHandle(void)
{
	return (mpWindowHandle);
}
void HTMLBrowser::SetWindowHandle(void *phandle)
{
    mpWindowHandle = phandle;
}

int HTMLBrowser::Home(void)
{
	return (LoadUrl(mpHomePage));
}


int HTMLBrowser::LoadUrl(WEBC_CHAR *url_str, WEBC_BOOL saveInHistory)
{
	if (url_str && mpDocument)
	{
		mpDocument->LoadUrl(url_str, saveInHistory);
	}

	return (0);
}

int HTMLBrowser::StopAll (void)
{
	Stop(mpDocument);
	return (0);
}

int HTMLBrowser::Stop (HTMLDocument* docToStop)
{
	if (docToStop->GetFlags() & HDOC_FLAG_STOPPING)
	{
		return 0;
	}

	docToStop->SetFlag(HDOC_FLAG_STOPPING);

	if (docToStop->IsOpen())
	{
		docToStop->Close();
	}

	docToStop->ClearRefreshTimers();

	vector_iterator vi;
	LoadJobInfo* jobInfo = (LoadJobInfo*) vector_get_first(&mLoadQueue, &vi);
	while (jobInfo)
	{
		if (jobInfo->pJob && (jobInfo->ownerDocument == docToStop || docToStop->IsAncestorOf(jobInfo->ownerDocument)))
		{
			jobInfo->pJob->Abort();
			if (!Loading())
			{
				WEBC_DELETE(jobInfo->pJob);
				jobInfo = (LoadJobInfo*) vector_delete_and_get_next(&vi);
				continue;
			}
		}
		jobInfo = (LoadJobInfo*) vector_get_next(&vi);
	}

	docToStop->ClearFlag(HDOC_FLAG_STOPPING);

	return (0);
}

int HTMLBrowser::Refresh(void)
{
	if (!mpDocument)
	{
		return (-1);
	}

	mpDocument->QueueLoad(mpDocument->CurrentURL(), WEBC_TRUE);

	return (0);
}


int HTMLBrowser::GetConfig(HTMLBrowserConfig* config)
{
	config->graphics = mHtmlGraphics;
	config->processInputQueue = mProcessInputQueueFn;
	config->privateData = mpPrivateData;

	return (0);
}


// if pDoc is non-null, then find the most recent entry in the back history for
//  pDoc
WEBC_CHAR *HTMLBrowser::Referer(HTMLDocument *pDoc)
{
HistoryNode last;

	if (!pDoc)
	{
		if (mBackHistory.Peek(&last))
		{
			return (last.pHRef);
		}
	}
	else
	{
		if (mBackHistory.PeekDoc(&last, pDoc))
		{
			return (last.pHRef);
		}
	}

	return (0);
}


int HTMLBrowser::GetHistorySize(void)
{
	return (mBackHistory.Size());
}


int HTMLBrowser::AddToHistory(WEBC_CHAR *pUrlString, DISPLAY_INT x, DISPLAY_INT y, HTMLDocument *doc)
{
	mBackHistory.Push(webc_malloc_string_copy(pUrlString, __FILE__, __LINE__), x, y, doc);
	mForwardHistory.Clear();
	return (0);
}


int HTMLBrowser::Focus(void)
{
	/* tbd
	if (mpBrowserWindow)
	{
		((PegThing *)0)->Presentation()->MoveFocusTree(mpBrowserWindow);
		return (0);
	}
	*/
	return (-1);
}


WEBC_CHAR *HTMLBrowser::LastUrl(void)
{
	/*
	if (miHistoryPosition > 0)
	{
		return (mpHistory[miHistoryPosition].url);
	}
	*/
	return (mpHomePage);
}


int HTMLBrowser::Execute(void)
{
	return (0);
}

int HTMLBrowser::Executing (void)
{
	return (0);
}


int HTMLBrowser::QueueLoadJob(LoadJob *pJob, HTMLDocument* docOwner)
{
	LoadJobInfo jinfo;

	if (pJob)
	{
		if (mbDestroyed)
		{
			WEBC_DELETE(pJob);
			return (-1);
		}

		HTMLDocument* parentDoc = docOwner;
		while (parentDoc)
		{
			if (parentDoc->GetFlags() & HDOC_FLAG_STOPPING)
			{
				WEBC_DELETE(pJob);
				return (-1);
			}
			parentDoc = parentDoc->Parent();
		}

		jinfo.pJob = pJob;
		jinfo.ownerDocument = docOwner;

		if (vector_add_node_to_rear(&mLoadQueue, (WEBC_PFBYTE) &jinfo) < 0)
		{
			WEBC_DELETE(pJob);
			return (-1);
		}

		return (0);
	}

	return (-1);
}


int HTMLBrowser::LoadAll(void)
{
int iJobsProcessed = 0;
vector_iterator vi;
struct LoadJobInfo jinfo, *p_jinfo;
vector deleteJobVect;

	// create a vector for saving jobs that need to be deleted
	vector_init(&deleteJobVect, sizeof(LoadJob *));

	if (Loading())
	{
		return (0);
	}
	p_jinfo = (struct LoadJobInfo *) vector_get_first(&mLoadQueue, &vi);

	if (!p_jinfo)
	{
		return (0);
	}

	mbLoading = 1;
	Lock();

  #ifdef WEBC_OLD_LOAD_METHOD
	if (mDisplayManager)
	{
		iRefreshState = mDisplayManager->GetRefreshState();
		DISPLAY_MANAGER_SET_REFRESH_STATE(mDisplayManager, 0);
	}
  #endif // WEBC_OLD_LOAD_METHOD

	SetDefaultStatus(WEBC_STR_LOADING);

	if (mEventHandler)
	{
		HTMLBrowserEvent e;
		e.type = HTML_BROWSER_EVENT_LOAD_START;
		mEventHandler((HBROWSER_HANDLE) this, &e);
	}

	while (p_jinfo)
	{
		jinfo = *p_jinfo;

		if (jinfo.pJob)
		{
			if (!jinfo.pJob->Aborted())
			{
				jinfo.pJob->Execute();
			}

			//WEBC_DELETE(jinfo.pJob);
			// Don't delete the job here (see note below), save it for later deletion
			vector_add_node_to_rear(&deleteJobVect, (WEBC_UINT8 *)&(jinfo.pJob));
		}

		iJobsProcessed++;

		vector_delete_node(&mLoadQueue, 0);
		p_jinfo = (struct LoadJobInfo *) vector_get_first(&mLoadQueue, &vi);
	}

	// now delete the jobs. This is because a side-effect of deleing the job is to
	// fire all of the onload handlers which should not be fired until all pages are loaded
	LoadJob **pJob = (LoadJob **)vector_get_first(&deleteJobVect, &vi);
	while(pJob)
	{
		WEBC_DELETE(*pJob);
		vector_delete_node(&deleteJobVect, 0);
		pJob = (LoadJob **)vector_get_first(&deleteJobVect, &vi);
	}

	if (mEventHandler)
	{
		HTMLBrowserEvent e;
		e.type = HTML_BROWSER_EVENT_LOAD_DONE;
		mEventHandler((HBROWSER_HANDLE) this, &e);
	}

	SetDefaultStatus(gDefaultStatusString);

  #if (WEBC_SUPPORT_HTTP)
	_BrowserTimeoutConnections (this, 0);
  #endif // WEBC_SUPPORT_HTTP

  #ifdef WEBC_OLD_LOAD_METHOD
	if (mDisplayManager)
	{
		DISPLAY_MANAGER_SET_REFRESH_STATE(mDisplayManager, iRefreshState);
	}
  #endif // WEBC_OLD_LOAD_METHOD

	mbLoading = 0;

	UnLock();

	return (iJobsProcessed);
}

HTMLDocument *HTMLBrowser::GetDocument(void)
{
	return mpDocument;
}

void HTMLBrowser::Lock(void)
{
	miLockCount++;
}

void HTMLBrowser::UnLock(void)
{
	if (miLockCount > 0)
	{
		miLockCount--;
		if (miLockCount == 0)
		{
			if (Destroyed())
			{
				WEBC_ASSERT(!Loading());
				Destroy(miDestroyStatus);
			}
		}
	}
}

void HTMLBrowser::AbortDocumentChildJobs(HTMLDocument *pDocument)
{
	AbortDocumentJobs(pDocument, WEBC_TRUE);
}

void HTMLBrowser::AbortDocumentJobs(HTMLDocument *pDocument, WEBC_BOOL childJobsOnly)
{
vector_iterator vi;
struct LoadJobInfo *p_jinfo;
HTMLDocument *pDoc;

	p_jinfo = (struct LoadJobInfo *) vector_get_first(&mLoadQueue, &vi);

	while (p_jinfo)
	{
		if (p_jinfo->pJob && !(p_jinfo->pJob->Aborted()))
		{
			pDoc = p_jinfo->pJob->GetDocument();
			while (pDoc)
			{
				if (pDoc == pDocument && (!childJobsOnly || p_jinfo->pJob->GetElement()))
				{
					p_jinfo->pJob->Abort();
					if (!Loading())
					{
						WEBC_DELETE(p_jinfo->pJob);
						p_jinfo->pJob = 0;
					}
					break;
				}
				pDoc = pDoc->Parent();
			}
		}

		p_jinfo = (struct LoadJobInfo *) vector_get_next(&vi);
	}
}

void HTMLBrowser::AbortElementJobs(HTMLElement *pElement)
{
vector_iterator vi;
struct LoadJobInfo *p_jinfo;
HTMLElement *pCurrent;

	p_jinfo = (struct LoadJobInfo *) vector_get_first(&mLoadQueue, &vi);

	while (p_jinfo)
	{
		if (p_jinfo->pJob && !(p_jinfo->pJob->Aborted()))
		{
			pCurrent = p_jinfo->pJob->GetElement();
			if (pCurrent == pElement)
			{
				p_jinfo->pJob->Abort();
				if (!Loading())
				{
					WEBC_DELETE(p_jinfo->pJob);
					p_jinfo->pJob = 0;
				}
			}
		}

		p_jinfo = (struct LoadJobInfo *) vector_get_next(&vi);
	}
}


DisplayManager* HTMLBrowser::GetDisplayManager (void)
{
	return mDisplayManager;
}

void HTMLBrowser::SetStatus (const WEBC_CHAR *text)
{
	if (mEventHandler)
	{
		HTMLBrowserEvent e;

		e.type = HTML_BROWSER_EVENT_STATUS_CHANGE;
		e.data.string = text;

		mEventHandler((HBROWSER_HANDLE) this, &e);
	}

	/* disabled as a performance optimization
	if (mDisplayManager)
	{
		mDisplayManager->Refresh();
	}
	*/
}

HTMLEventStatus HTMLBrowser::TriggerBrowserEvent (HTMLBrowserEvent* e)
{
	if (mEventHandler)
	{
		return mEventHandler((HBROWSER_HANDLE) this, e);
	}

	return (HTML_EVENT_STATUS_CONTINUE);
}

int HTMLBrowser::ProcessLoadJob (
		LoadJob* job,
		URLDescriptor* url,
		URLDescriptor* base,
		HTMLDocument* requestingDocument,
		WEBC_BOOL executeImmediately)
{
	// generate a browser-level event to given the WebC-embedding application
	//  an opportunity to override the default processing of this URL

	HTMLBrowserEvent e;

	e.type = HTML_BROWSER_EVENT_URL_REQUEST;
	WEBC_CHAR* urlStr  = (WEBC_CHAR*) (e.data.request.url  = url->toString());
	WEBC_CHAR* baseStr = (WEBC_CHAR*) (e.data.request.base = base->toString());
	e.data.request.id = (WEBC_UINT32) job;
	e.hdoc = (HDOC_HANDLE) requestingDocument;
	e.out.urlStream.readFrom = WEBC_NULL;
	e.out.urlStream.postTo = WEBC_NULL;

	HTMLEventStatus status = TriggerBrowserEvent (&e);

	url->freeString(urlStr);
	base->freeString(baseStr);

	switch (status)
	{
		case HTML_EVENT_STATUS_CONTINUE:
			if (executeImmediately)
			{
				job->SetStream(wload_StreamAlloc(1));
				long executeResult = job->Execute();
				WEBC_DELETE(job);
				return (executeResult < 0)? executeResult : 0;
			}
			else
			{
				if (url->getProtocol() != WEBC_PROTOCOL_JSCRIPT)
				{
					wload_WaitForStream(job);
				}
				return QueueLoadJob(job, requestingDocument);
			}
			break;

		case HTML_EVENT_STATUS_DONE:
			break;

		case HTML_EVENT_STATUS_HALT:
			break;
	}

	WEBC_DELETE(job);

	return (-1);
}

WEBC_CHAR *HTMLBrowser::GetStatus (void)
{
	return (0);
}

void HTMLBrowser::SetTempStatus (const WEBC_CHAR *text, void *who)
{
	WEBC_CHAR *str = webc_malloc_string_copy(text, __FILE__, __LINE__);
	webc_free_string_copy(mpTempStatus, __FILE__, __LINE__);
	mpTempStatus = str;
	mpTempStatusOwner = who;

	SetStatus (mpTempStatus);
}

void HTMLBrowser::ClearTempStatus (void *who)
{
	if (who == mpTempStatusOwner)
	{
		webc_free_string_copy(mpTempStatus, __FILE__, __LINE__);
		mpTempStatus = 0;

		if (mpDefaultStatus)
		{
			SetStatus (mpDefaultStatus);
		}
	}
}

void HTMLBrowser::SetDefaultStatus (const WEBC_CHAR *text)
{
	WEBC_CHAR *str = webc_malloc_string_copy(text, __FILE__, __LINE__);
	webc_free_string_copy(mpDefaultStatus, __FILE__, __LINE__);
	mpDefaultStatus = str;

	SetStatus (mpDefaultStatus);
}

void HTMLBrowser::PurgeDocumentFromHistory(HTMLDocument *doc)
{
	mBackHistory.PurgeDocumentReferences(doc);
	mForwardHistory.PurgeDocumentReferences(doc);
}

void HTMLBrowser::SetTitle (const WEBC_CHAR* title)
{
}

#if (WEBC_SUPPORT_USER_EVENTS)
HTMLEventHandler* HTMLBrowser::GetDefaultElementEventHandler (
		HTMLElement* element
	)
{
	if (mTagEventHandlerArray)
	{
		if (element->GetTag() >= 0 && element->GetTag() < HTML_NUM_TAGS)
		{
			return (mTagEventHandlerArray[element->GetTag()]);
		}
	}

	return (0);
}

int HTMLBrowser::SetTagEventHandler (
		HTMLTagType tagType,
		HTMLEventHandler* eventHandler
	)
{
	if (tagType >= 0 && tagType < HTML_NUM_TAGS)
	{
		if (!mTagEventHandlerArray)
		{
			mTagEventHandlerArray = (HTMLEventHandler**) WEBC_MALLOC_VERBOSE(sizeof(HTMLEventHandler*) * HTML_NUM_TAGS,"TagHandlerArray");
			if (mTagEventHandlerArray)
			{
				tc_memset(mTagEventHandlerArray, 0, sizeof(HTMLEventHandler*) * HTML_NUM_TAGS);
			}
		}

		if (mTagEventHandlerArray)
		{
			mTagEventHandlerArray[tagType] = eventHandler;
			return (0);
		}
	}

	return (-1);
}

#endif

int HTMLBrowser::GetFontFamilyIndex (
		const WEBC_CHAR* name,
		const WEBC_CHAR* genericName
	)
{
	int numFonts = (*mHtmlGraphics)->getNumFontFamilies(mHtmlGraphics);
	const WEBC_CHAR** fontNames = (*mHtmlGraphics)->getFontFamilyNames(mHtmlGraphics);
	const WEBC_CHAR** fontGenericNames = (*mHtmlGraphics)->getFontFamilyGenericNames(mHtmlGraphics);

	return webc_font_family_index(name, genericName, fontNames, fontGenericNames, numFonts);
}

WebcFontDescriptor HTMLBrowser::GetFontDescriptor (
		CSSPropertyValue* family,
		CSSPropertyValue* size,
		CSSPropertyValue* style,
		CSSPropertyValue* weight,
		CSSPropertyValue* variant,
		WebcFontDescriptor baseDesc)
{
	if (family)
	{
		int n;

		n = GetFontFamilyIndex(family->name, 0);
		if (n < 0)
		{
			n = 0;
		}

		WEBC_SET_FONT_FAMILY(baseDesc, n);
	}

	if (size)
	{
		switch (size->fontSize.type)
		{
			case CSS_FONT_SIZE_ABSOLUTE:
				WEBC_SET_FONT_SIZE(baseDesc, giFontSizeAbsoluteToPoints[EBSCLIP((int) size->fontSize.val.absolute, 0, 6)]);
				break;

			case CSS_FONT_SIZE_RELATIVE:
				switch (size->fontSize.val.relative)
				{
					case CSS_FONT_SIZE_SMALLER:
					{
						int ptSize = WEBC_FONT_SIZE(baseDesc);
						int absSize = CSS_FontSizePointsToAbsolute(ptSize);
						absSize--;
						if (absSize < 0)
						{
							absSize = 0;
						}
						ptSize = giFontSizeAbsoluteToPoints[absSize];
						WEBC_SET_FONT_SIZE(baseDesc, ptSize);
						break;
					}

					case CSS_FONT_SIZE_LARGER:
					{
						int ptSize = WEBC_FONT_SIZE(baseDesc);
						int absSize = CSS_FontSizePointsToAbsolute(ptSize);
						absSize++;
						if (absSize > 6)
						{
							absSize = 6;
						}
						ptSize = giFontSizeAbsoluteToPoints[absSize];
						WEBC_SET_FONT_SIZE(baseDesc, ptSize);
						break;
					}

					default:
						break;
				}
				break;

			case CSS_FONT_SIZE_LENGTH:
			{
				int parentFontSize = WEBC_FONT_SIZE(baseDesc);
				int ptSize = CSS_LengthToPoints(&size->fontSize.val.length, parentFontSize, parentFontSize, parentFontSize>>1);
				WEBC_SET_FONT_SIZE(baseDesc, ptSize);
				break;
			}

			default:
				break;
		}
	}

	if (style)
	{
		baseDesc &= ~WEBC_FONT_STYLE_MASK;
		if (style->fontStyle == CSS_FONT_STYLE_ITALIC)
		{
			baseDesc |= WEBC_FONT_STYLE_ITALIC;
		}
		else
		{
			baseDesc |= WEBC_FONT_STYLE_NORMAL;
		}
	}

	if (weight)
	{
		baseDesc &= ~WEBC_FONT_WEIGHT_MASK;
		if (((weight->fontWeight >= CSS_FONT_WEIGHT_BOLD) &&
		     (weight->fontWeight <= CSS_FONT_WEIGHT_900)) ||
		    (weight->fontWeight == CSS_FONT_WEIGHT_BOLDER))
		{
			baseDesc |= WEBC_FONT_WEIGHT_BOLD;
		}
		else
		{
			baseDesc |= WEBC_FONT_WEIGHT_NORMAL;
		}
	}

	if (variant)
	{
		// tbd - font variants not implemented
	}

	return (baseDesc);
}

int HTMLBrowser::GetFontProperty (
		WebcFontDescriptor desc,
		CSSPropertyType property,
		CSSPropertyValue* value)
{
	switch (property)
	{
		case CSS_PROPERTY_FONT_FAMILY:
		{
			const WEBC_CHAR** fontNames = (*mHtmlGraphics)->getFontFamilyNames(mHtmlGraphics);
			if (fontNames)
			{
				value->name = fontNames[EBSCLIP(WEBC_FONT_FAMILY(desc),0,WEBC_CFG_MAX_FONT_FAMILIES-1)];
				return (0);
			}
			break;
		}

		case CSS_PROPERTY_FONT_STYLE:
			if (WEBC_FONT_STYLE(desc) == WEBC_FONT_STYLE_ITALIC)
			{
				value->fontStyle = CSS_FONT_STYLE_ITALIC;
			}
			else
			{
				value->fontStyle = CSS_FONT_STYLE_NORMAL;
			}
			return (0);

		case CSS_PROPERTY_FONT_VARIANT:
			break;

		case CSS_PROPERTY_FONT_WEIGHT:
			if (WEBC_FONT_WEIGHT(desc) == WEBC_FONT_WEIGHT_BOLD)
			{
				value->fontWeight = CSS_FONT_WEIGHT_BOLD;
			}
			else
			{
				value->fontWeight = CSS_FONT_WEIGHT_NORMAL;
			}
			return (0);

		case CSS_PROPERTY_FONT_SIZE:
			value->fontSize.type = CSS_FONT_SIZE_LENGTH;
			value->fontSize.val.length.type = CSS_LENGTH_UNIT_PT;
			value->fontSize.val.length.val.dec = WEBC_FONT_SIZE(desc);
			value->fontSize.val.length.val.frac = 0;
			return (0);

		default:
			break;
	}

	return (-1);
}
void HTMLBrowser::PushPrivateData(void* data)
{
    vector_add_node_to_front(&mPrivateDataStack, (WEBC_PFBYTE) &data);
}
void HTMLBrowser::PopPrivateData (void)
{
	if (GetFromPrivateDataStack())
		vector_delete_node(&mPrivateDataStack,0);
}
void * HTMLBrowser::GetFromPrivateDataStack (void)
{
vector_iterator vi;
WEBC_PFBYTE *data;
	data = (WEBC_PFBYTE *) vector_get_first(&mPrivateDataStack,&vi);
	if (data)
		return (void *) *data;
	else
		return 0;
}


WebGraphFont* HTMLBrowser::GetWebGraphFont (WebcFontDescriptor desc)
{
	CSSPropertyValue style, weight, variant;
	CSSPropertyValue *pStyle = 0, *pWeight = 0, *pVariant = 0, *pSize = 0;
	WEBC_UINT16 index, fam;

	int ptSize = WEBC_FONT_SIZE(desc);

	ptSize = EBSCLIP(ptSize, WEBC_SMALLEST_FONT_SIZE_PT, WEBC_LARGEST_FONT_SIZE_PT);

	index = WEBC_FONT_FAMILY(desc) * (WEBC_LARGEST_FONT_SIZE_PT - WEBC_SMALLEST_FONT_SIZE_PT + 1) << 2;
	index += desc & 0x3;
	index += (ptSize - WEBC_SMALLEST_FONT_SIZE_PT) << 2;

	if (!mFontCache[index])
	{
		WebFont wf = (WebFont) WEBC_MALLOC_VERBOSE(sizeof(struct s_WebFont),"CachedFont");
		if (wf)
		{
			if (GetFontProperty(desc, CSS_PROPERTY_FONT_STYLE, &style) >= 0)
			{
				pStyle = &style;
			}

			if (GetFontProperty(desc, CSS_PROPERTY_FONT_WEIGHT, &weight) >= 0)
			{
				pWeight = &weight;
			}

			if (GetFontProperty(desc, CSS_PROPERTY_FONT_VARIANT, &variant) >= 0)
			{
				pVariant = &variant;
			}

			fam = WEBC_FONT_FAMILY(desc);
			fam = EBSCLIP(fam,0,WEBC_CFG_MAX_FONT_FAMILIES-1);

			const WEBC_CHAR** fontNames = (*mHtmlGraphics)->getFontFamilyNames(mHtmlGraphics);
			const WEBC_CHAR** fontGenericNames = (*mHtmlGraphics)->getFontFamilyGenericNames(mHtmlGraphics);

			wf->font = (*mHtmlGraphics)->createFont (
					mHtmlGraphics,
					fontNames[fam],
					fontGenericNames[fam],
					ptSize,
					desc & 0x3,
					fam
				);

			if (wf->font != WEB_FONT_NULL)
			{
				wf->height = (*mHtmlGraphics)->getFontHeight(mHtmlGraphics, wf->font);
				if ((*mHtmlGraphics)->getFontBaseline)
				{
					wf->baseline = (*mHtmlGraphics)->getFontBaseline(mHtmlGraphics, wf->font);
				}
				else
				{
					wf->baseline = (wf->height << 2) / 5 + 1;
				}

				WEBC_NEW_VERBOSE(mFontCache[index], WebGraphFont(wf),"WebGraphFont");
				if (mFontCache[index])
				{
					return mFontCache[index];
				}

				(*mHtmlGraphics)->destroyFont(mHtmlGraphics, wf->font);
			}
			else
			{
				wf->height = 0;
				wf->baseline = 0;
			}

			WEBC_FREE(wf);
		}
	}

	return mFontCache[index];
}

WebGraphFont* HTMLBrowser::GetDefaultFont (void)
{
	return mDefaultFont;
}

int HTMLBrowser::ProcessInputQueue (WEBC_BOOL blocking)
{
	WEBC_INT32 status;
	int result;

	result = (*mProcessInputQueueFn)(mProcessInputQueueFn, blocking, &status);

	if (result < 0)
	{
		miDestroyStatus = status;
	}

	return result;
}


/*****************************************************************************/
// Methods for class HistoryStack
/*****************************************************************************/

HistoryStack::HistoryStack(int size)
{
	mpStack = (HistoryNode *) WEBC_MALLOC_VERBOSE(sizeof(HistoryNode) * size, "HistoryStack");
	miTop = 0;
	miSize = (mpStack)? size : 0;
}

HistoryStack::~HistoryStack()
{
	Clear();
	if (mpStack)
	{
		WEBC_FREE(mpStack);
	}
}

void HistoryStack::Push(HistoryNode *n)
{
	Push(n->pHRef, n->x, n->y, n->pDoc);
}

void HistoryStack::Push(WEBC_CHAR *pUrlString, DISPLAY_INT iScrollX, DISPLAY_INT iScrollY, HTMLDocument *doc)
{
HistoryNode *node;

	if (!mpStack || !pUrlString)
	{
		return;
	}

	if (miTop > 0)
	{
		// if the url we are adding is the last one we visited, then bail
		if (doc == mpStack[miTop-1].pDoc && !webc_strcmp(pUrlString, mpStack[miTop-1].pHRef))
		{
			mpStack[miTop-1].x = iScrollX;
			mpStack[miTop-1].y = iScrollY;
			webc_free_string_copy(pUrlString, __FILE__, __LINE__);
			return;
		}
	}

	// in this kind of stack, if we try to push and we are full, we just
	//  discard the item at the bottom and shift down.
	if (miTop == miSize)
	{
		DeleteNode(0);
		Compress();
	}

	node = &mpStack[miTop++];
	node->pHRef = pUrlString;
	node->pDoc = doc;
	node->x = iScrollX;
	node->y = iScrollY;
}

WEBC_BOOL HistoryStack::Pop(HistoryNode *node)
{
	if (!mpStack)
	{
		return (WEBC_FALSE);
	}

	if (miTop > 0)
	{
		miTop--;
		*node = mpStack[miTop];
		return (WEBC_TRUE);
	}
	return (WEBC_FALSE);
}

WEBC_BOOL HistoryStack::Peek(HistoryNode *node, int depth)
{
	if (!mpStack)
	{
		return (WEBC_FALSE);
	}

	if (miTop-depth > 0)
	{
		*node = mpStack[miTop-depth-1];
		return (WEBC_TRUE);
	}
	return (WEBC_FALSE);
}

WEBC_BOOL HistoryStack::PeekDoc(HistoryNode *node, HTMLDocument *pDoc)
{
	if (!mpStack)
	{
		return (WEBC_FALSE);
	}

	for (int depth = miTop-1; depth >= 0; depth--)
	{
		if (mpStack[depth].pDoc == pDoc)
		{
			*node = mpStack[depth];
			return (WEBC_TRUE);
		}
	}

	return (WEBC_FALSE);
}

int HistoryStack::Clear()
{
	if (!mpStack)
	{
		return (0);
	}

	for (int n=0; n<miTop; n++)
	{
		DeleteNode(n);
	}
	miTop = 0;

	return (0);
}

int HistoryStack::Size()
{
	return (miSize);
}

int HistoryStack::Top()
{
	return (miTop);
}

void HistoryStack::PurgeDocumentReferences(HTMLDocument *pDoc)
{
	WEBC_BOOL changed = WEBC_FALSE;

	if (!mpStack)
	{
		return;
	}

	for (int n=0; n<miTop; n++)
	{
		if (mpStack[n].pDoc == pDoc)
		{
			changed = WEBC_TRUE;
			DeleteNode(n);
		}
	}

	if (changed)
	{
		Compress();
	}
}

void HistoryStack::DeleteNode(int iDepth)
{
	if (mpStack && mpStack[iDepth].pHRef)
	{
		webc_free_string_copy(mpStack[iDepth].pHRef, __FILE__, __LINE__);
		mpStack[iDepth].pHRef = 0;
		mpStack[iDepth].pDoc = 0;
	}
}

void HistoryStack::Compress(void)
{
int from,to;

	if (!mpStack)
	{
		return;
	}

	to = 0;
	for (from=0; from<miTop; from++)
	{
		if (mpStack[from].pHRef)
		{
			if (from != to)
			{
				mpStack[to] = mpStack[from];
			}
			to++;
		}
	}
	miTop = to;
}


#if (WEBC_SUPPORT_HTTP)

int _BrowserTimeoutConnections (void* data, long msecError)
{
	HTMLBrowser* browser = (HTMLBrowser*) data;

	http_timeout_connections();

	long msecTimeout = http_msecs_to_next_timeout();
	if (msecTimeout != WEBC_TIMEOUT_INFINITE)
	{
		browser->TimeoutManager()->SetTimeout (
				(webcTimeoutFn) _BrowserTimeoutConnections,
				0,
				browser,
				msecTimeout,
				0
			);
	}

	return (0);
}

#endif // WEBC_SUPPORT_HTTP
