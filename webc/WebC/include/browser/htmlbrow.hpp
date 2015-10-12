#ifndef __HTMLBROW_HPP__
#define __HTMLBROW_HPP__

#include "webcfg.h"
#if (WEBC_SUPPORT_OFFLINE_BROWSING)
 #include "sramdisk.hpp"
 #include "wcache.h"
#endif
#include "htmldom.hpp"
#include "webgraph.hpp"
#include "webctmo.h"
#include "delement.hpp"

/*************************************************************************
* class HTMLBrowser
*
*************************************************************************/

class HTMLDocument;
class LoadJob;

struct HistoryNode
{
	WEBC_CHAR *pHRef;
	HTMLDocument *pDoc;
	DISPLAY_INT x,y;
};

class HistoryStack
{
public:
	HistoryStack(int size=WEBC_CFG_HISTORY_SIZE);
	~HistoryStack();
	void Push(HistoryNode *n);
	void Push(WEBC_CHAR *pUrlString, DISPLAY_INT iScrollX, DISPLAY_INT iScrollY, HTMLDocument *doc);
	WEBC_BOOL Pop(HistoryNode *node);
	WEBC_BOOL Peek(HistoryNode *node, int depth=0);
	WEBC_BOOL PeekDoc(HistoryNode *node, HTMLDocument *pDoc);
	int Clear();
	int Size();
	int Top();
	void PurgeDocumentReferences(HTMLDocument *pDoc);

private:
	void DeleteNode(int iDepth);
	void Compress(void);

	HistoryNode *mpStack;
	int miTop;
	int miSize;
};


#define WEBC_BROWSER_BACK -1
#define WEBC_BROWSER_FORWARD 1

#define HTMLBROW_FLAG_EXECUTING     0x01
#define HTMLBROW_FLAG_DESTROYED     0x02
#define HTMLBROW_FLAG_INSCRIPT      0x04
#define HTMLBROW_FLAG_LOADING       0x08
#define HTMLBROW_FLAG_LOADABORTED   0x10

class DisplayManager;
class WebcJSBrowserContext;

/**

 @memo HTML Client class
 @doc  This class is used as the context for an HTML client session (i.e. an
       individual web browser window).

 @see  HTMLElement, HTMLDocument
 */
class HTMLBrowser
{
protected:
	HTMLDocument*          mpDocument;
	WEBC_CHAR*             mpHomePage;
	WEBC_CHAR*             mpWindowName;
	void *                 mpWindowHandle;
	HistoryStack           mForwardHistory;
	HistoryStack           mBackHistory;
	int                    miLockCount;
	struct vector          mLoadQueue;
	WEBC_BOOL              mbLoading;
	WEBC_BOOL              mbDestroyed;
	int                    miDestroyStatus;
	void*                  mpPrivateData;
	vector 					mPrivateDataStack;
	WEBC_CHAR*             mpDefaultStatus;
	WEBC_CHAR*             mpTempStatus;
	void*                  mpTempStatusOwner;
	WebGraphFont*          mDefaultFont;
	WebGraphFont*          mFontCache[WEBC_FONT_CACHE_SIZE];

  #if (WEBC_SUPPORT_USER_EVENTS)
	HTMLEventHandler**     mTagEventHandlerArray;
  #endif

	int (**mProcessInputQueueFn) (
			void* fn,
			WEBC_BOOL block,
			WEBC_INT32* exitCode);

	HTMLEventStatus (*mEventHandler) (
			HBROWSER_HANDLE hbrowser,
			HTMLBrowserEvent* event);

	DisplayManager* mRootDocDisplay;
	DisplayManager* mDisplayManager;
	WebGraphics*    mGC;
	HTMLGraphicsContext** mHtmlGraphics;

	int GetFontFamilyIndex (
			const WEBC_CHAR *name,
			const WEBC_CHAR *genericName);

	WebcTimeoutManager mTimeoutManager;

public:
    /**
     @memo Constructor
     @param pConfig   Pointer to \Ref{HTMLBrowserConfig} structure; the initial configuration.
     @param CreateWindow If True create a new window
     @param pHomePage The initial URL to load in this window; NULL opens a blank window
     @see
     */
	HTMLBrowser(HTMLBrowserConfig *config, WEBC_BOOL CreateWindow, const WEBC_CHAR* windowName, const WEBC_CHAR* homePage);

    /**
     @memo Destructor
     @see
     */
	virtual ~HTMLBrowser(void);

    /**
     @memo Destroy this browser and free all memory used by it
     @param iStatus  The value to return from Execute
     @see
     */
     virtual void Destroy(int iStatus);
   
	 /**
     @memo Set the scroll mode for the main document in the window. 
     @param ScrollMode  The ScrollMode
     @see
     */
	 virtual void SetScrollMode(SCROLL_MODE	ScrollMode) ;

    /**
     @memo Protect against destruction
     @see
     */
     void Lock();

    /**
     @memo Allow destruction
     @see
     */
     void UnLock();

    /**
     @memo Load the previous URL in the history
     @see
     */
	int Back(void);

    /**
     @memo Load the next URL in the history
     @see
     */
	int Forward(void);

	/**
	 @memo Load a URL in the history list
	 @see
	 */
	int Go(int index);

    /**
     @memo Load the initial URL passed into the HTMLBrowser constructor
     @see
     */
	int Home(void);

    /**
     @memo Load a new URL
     @see
     */
	int LoadUrl(WEBC_CHAR* url_str, WEBC_BOOL saveInHistory = WEBC_TRUE);

    /**
     @memo Abort all jobs in the load queue
     @see
     */
	int Stop (HTMLDocument* docToStop);
	int StopAll (void);

    /**
     @memo Get the name of the browser, used in links and window.open
     @see
     */
	WEBC_CHAR *GetName(void);

    /**
     @memo Get the name of the browser, used in links and window.open
     @see
     */
	void ChangeName(WEBC_CHAR *newName);

    /**
     @memo Get the underlying window handle (void *) Used by GUI to find the browser during event dispatch
     @see
     */
	void *GetWindowHandle(void);

    /**
     @memo Set the underlying handle used by GUI to bind the window and browser during window create
     @see
     */
	void SetWindowHandle(void *phandle);

    /**
     @memo Reload the current URL
     @see
     */
	int Refresh(void);

    /**
     @memo Get the current browser configuration
     @see
     */
	virtual int GetConfig(HTMLBrowserConfig* config);

    /**
     @memo Add a URL string to the history
     Note: after you add a location to the history, pressing back will immediately get you
      to the last location you added; so you should add a location when you LEAVE a page,
      NOT when you load it for the first time.
     @see
     */
	int AddToHistory(WEBC_CHAR *pUrlString, DISPLAY_INT iScrollX, DISPLAY_INT iScrollY, HTMLDocument *doc);

	void PurgeDocumentFromHistory(HTMLDocument *doc);

    /**
     @memo Move the input focus to this client
     @see
     */
	int Focus(void);

    /**
     @memo Get the last URL loaded (i.e. the current URL)
     @see
     */
	WEBC_CHAR *LastUrl(void);

    /**
     @memo Wait for this client to terminate
     @see
     */
	virtual int Execute(void);
	virtual int Executing (void);

    /**
     @memo Place a job in the load queue
     @see

     This function is DEPRECATED for use external to HTMLBrowser class and its
      derived classes.  Use ProcessLoadJob instead.

     */
	int QueueLoadJob(LoadJob *pJob, HTMLDocument* docOwner);

	/**
	 @memo Process a load job related to a particular URL request
	 @see HTMLBrowser::QueueLoadJob
	 */
	int ProcessLoadJob (
			LoadJob* job,
			URLDescriptor* url,
			URLDescriptor* base,
			HTMLDocument* requestingDocument,
			WEBC_BOOL executeImmediately = WEBC_FALSE);

    /**
     @memo Execute all pending jobs in the load queue
     @see
     */
	virtual int LoadAll(void);

    /**
     @memo Return whether this browser currently has pending load jobs
     @see
     */
	WEBC_BOOL Loading(void)
		{ return mbLoading; }

    /**
     @memo Return whether this browser has been destroyed
     @see
     */
	WEBC_BOOL Destroyed(void)
		{ return mbDestroyed; }

    /**
     @memo Get an HTML Browser's root document
     @see
     */
	HTMLDocument *GetDocument(void);

	/**
	 @memo Get the Browser's history
	 @see
	 */
//	HistoryNode *GetHistory(void)
//		{ return mpHistory; }

	/**
	 @memo Get the history position
	 @see
	 */
//	int GetHistoryPosition(void)
//		{ return miHistoryPosition; }

	/**
	 @memo Get the history size
	 @see
	 */
	int GetHistorySize(void);
//		{ return miHistorySize; }

	/**
	 @memo Get private data pointer
	 @see
	 */
	void* GetPrivateData(void)
		{ return mpPrivateData; }

	/**
	 @memo Set private data pointer
	 @see
	 */
	void SetPrivateData(void* ptr)
		{ mpPrivateData = ptr; }

	void  PushPrivateData (void* data);
	void  PopPrivateData (void);
	void* GetFromPrivateDataStack(void);


	/**
	 @memo Abort all load jobs (after the currently executing one) associated
	       with a particular HTMLDocument
	 @see
	 */
	void AbortDocumentJobs(HTMLDocument *pDocument, WEBC_BOOL childJobsOnly = WEBC_FALSE);
	void AbortDocumentChildJobs(HTMLDocument *pDocument);
	void AbortElementJobs(HTMLElement *pElement);

	WEBC_CHAR *Referer(HTMLDocument *pDoc = 0);

	//void SetUrlBarText (const WEBC_CHAR *url);
	void SetStatus (const WEBC_CHAR *text);
	void SetTitle (const WEBC_CHAR *title);

	WEBC_CHAR *GetStatus (void);
	void SetTempStatus (const WEBC_CHAR *text, void *who);
	void ClearTempStatus (void *who);
	void SetDefaultStatus (const WEBC_CHAR *text);

	DisplayManager* GetDisplayManager (void);

  #if (WEBC_SUPPORT_USER_EVENTS)
	HTMLEventHandler* GetDefaultElementEventHandler (
			HTMLElement* element
		);

	int SetTagEventHandler (
			HTMLTagType tagType,
			HTMLEventHandler* eventHandler
		);
  #endif

	int ProcessInputQueue (WEBC_BOOL blocking);

	WebcFontDescriptor GetFontDescriptor (
			CSSPropertyValue* family,
			CSSPropertyValue* size,
			CSSPropertyValue* style,
			CSSPropertyValue* weight,
			CSSPropertyValue* variant,
			WebcFontDescriptor baseDesc
		);

	int GetFontProperty (
			WebcFontDescriptor desc,
			CSSPropertyType property,
			CSSPropertyValue* value
		);

	WebGraphFont* GetWebGraphFont (WebcFontDescriptor desc);
	WebGraphFont* GetDefaultFont (void);

	HTMLEventStatus TriggerBrowserEvent (HTMLBrowserEvent* e);

	WebcTimeoutManager* TimeoutManager (void) { return &mTimeoutManager; }
};

#endif // __HTMLBROW_HPP__
