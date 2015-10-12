//
// WEBCSDKBROWSER.CPP -
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
#include "htmlsdk.h"
#include "httpc.h"
#if (WEBC_SUPPORT_JSCRIPT)
 #include "wjsdoc.h"
 #include "wjsbrowser.h"
 #include "wjscript.hpp"
#endif
#if (WEBC_SUPPORT_COOKIES)
 #include "cookie.h"
#endif
#include "hchkbox.hpp"
#include "helement.hpp"
#include "hframe.hpp"
#include "himage.hpp"
#include "hobject.hpp"
#include "hinput.hpp"
#include "hhidden.hpp"
#include "hedit.hpp"
#include "hpasswd.hpp"
#include "hselect.hpp"
#include "htmldoc.hpp"
#include "htmlfind.hpp"
#include "format.hpp"
#include "webctmo.h"
#include "htmlbrow.hpp"
#include "webcstr.h"
#include "prothttp.h"
#include "wgkeys.hpp"
#include "jhutils.h"
#include "tuenum.hpp"
#include "objref.cpp"
#include "webcmem.h"

#include "webcdefault.h"
#ifdef WEBC_DEBUG
#include "webcLog.h"
#endif
#if (WEBC_DEBUG_JSCRIPT)
#include "wjsdebug.h"
#endif
#include "wcapi.h"

/*****************************************************************************/
// Local types
/*****************************************************************************/
#include "webcsdk.hpp"



/*****************************************************************************/
// Macros
/*****************************************************************************/

/*****************************************************************************/
// HTML C API Functions
/*****************************************************************************/

/*****************************************************************************/
// HTML Browser API Functions
/*****************************************************************************/



/* Match on display manager callback */

extern "C" {
static int _matchBrowserManager(WEBC_PFBYTE data, WEBC_PFBYTE param) {
    struct WindowInfo *pi = (struct WindowInfo *) data;
    if (pi)
    {
        HTMLBrowser *pBrowser = (HTMLBrowser *) pi->Browser;
		return (pBrowser->GetDisplayManager() == (DisplayManager *) param);
    }
    return(0);
}
}
HBROWSER_HANDLE webc_FindBrowserByDisplayManager(void *manager)
{
    HBROWSER_HANDLE Browser = 0;
    struct WindowInfo *pi =
        (struct WindowInfo *) vector_find_node(&gBrowserWindowVector, _matchBrowserManager, (WEBC_PFBYTE) manager);
    if (pi)
        Browser = pi->Browser;
    return (Browser);
}

/* Match on window name callback, used by webc_FindBrowserByName()
   Return true if browser's stored name matches the suppiled name */

extern "C" {

static int _matchBrowserName(WEBC_PFBYTE data, WEBC_PFBYTE param) {
    struct WindowInfo *pi = (struct WindowInfo *) data;
    if (pi)
    {
        HTMLBrowser *pBrowser = (HTMLBrowser *) pi->Browser;
        WEBC_CHAR *name =  (WEBC_CHAR *) param;
        return (int) (pBrowser && pBrowser->GetName() && webc_strcmp(pBrowser->GetName(), name) == 0);
    }
    return(0);
}

}

/* Find named browser . Used by window open and link targets */
HBROWSER_HANDLE webc_FindBrowserByName(WEBC_CHAR *windowname)
{
    HBROWSER_HANDLE Browser = 0;
    struct WindowInfo *pi =
        (struct WindowInfo *) vector_find_node(&gBrowserWindowVector, _matchBrowserName, (WEBC_PFBYTE) windowname);
    if (pi)
        Browser = pi->Browser;
    return (Browser);
}


HBROWSER_HANDLE webc_FindBrowserByNameAscii(char* _windowname)
{
    if (_windowname)
    {
  #if (WEBC_SUPPORT_UNICODE)
    	WEBC_CHAR windowname[HTML_TEMP_STR_LEN];
    	webc_c_strncpy(windowname, _windowname, HTML_TEMP_STR_LEN-1);
    	windowname[HTML_TEMP_STR_LEN-1] = 0;
  #else
      	WEBC_CHAR *windowname = _windowname;
  #endif
        return(webc_FindBrowserByName(windowname));
    }
    return(0);
}



/* Match on window handle callback, used by webc_FindBrowserByHandle()
   Return true if browser's stored handle matches the supplied handle
   the handle is void *.. */
extern "C" {
static int _matchBrowserWindowHandle(WEBC_PFBYTE data, WEBC_PFBYTE param){
    struct WindowInfo *pi = (struct WindowInfo *) data;
    if (pi)
    {
        HTMLBrowser *pBrowser = (HTMLBrowser *) pi->Browser;
        void *handle = (void *) param;
        return ((int) (pBrowser->GetWindowHandle() == handle));
    }
    return(0);
}
}
/* Find browser by window . Used by the GUI layer to match events to browser windows  */

HBROWSER_HANDLE webc_FindBrowserByWindow (void *WindowHandle)
{
    HBROWSER_HANDLE Browser = 0;
    struct WindowInfo *pi =
        (struct WindowInfo *) vector_find_node(&gBrowserWindowVector, _matchBrowserWindowHandle, (WEBC_PFBYTE) WindowHandle);
    if (pi)
    {
        Browser = pi->Browser;
    }
    return (Browser);
}

// webc_SetBrowserWindow. Used by the GUI to register a window handle with a browser window when it creates one
void webc_SetBrowserWindow (HBROWSER_HANDLE Browser, void *WindowHandle)
{
    HTMLBrowser *pBrowser = (HTMLBrowser *) Browser;
    if (pBrowser)
    {
        struct WindowInfo wi;
        wi.Browser = Browser;
        pBrowser->SetWindowHandle(WindowHandle);
        vector_add_node_to_front(&gBrowserWindowVector, (unsigned char*) &wi);
    }
}

// webc_UnSetBrowserWindow. Used by the browser destructor to detach the browser so the GUI doesn't match any more
void webc_UnSetBrowserWindow (void *WindowHandle)
{ /* Search for the handle and remove it from the vector */
    vector_delete_cond(&gBrowserWindowVector, _matchBrowserWindowHandle, (WEBC_UINT8*)WindowHandle);
}

// Get browser by window index. Returns 0 if index > num windows. Not used yet
DECL_WEBCDLL HBROWSER_HANDLE webc_FindBrowserByIndex (int index)
{
    HBROWSER_HANDLE Browser = (HBROWSER_HANDLE) vector_get_node(&gBrowserWindowVector,index);
    return (Browser);
}



/*****************************************************************************/
// webc_BrowserAddToHistory
/**

 @memo   webC document history interface. Add a location to the HTML Browser history
 @doc    Document history is used mainly in web browser applications but it can be useful in other applications with navigation requirements. If screens are being loaded through URLs a history is automatically maintained. This function can be used to manually add a url to the history.


 @param  browser   The HTML Browser handle
 @param  url_str   The URL to put in the history (a copy of this
                   string will be created)
 @return 0 on success, -1 otherwise
 @see    webc_BrowserRefresh, webc_BrowserBack, webc_BrowserForward
 */
/*****************************************************************************/

int webc_BrowserAddToHistory(HBROWSER_HANDLE browser, char *_url_str)
{
  #if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR url_str[HTML_TEMP_STR_LEN];
	webc_c_strncpy(url_str, _url_str, HTML_TEMP_STR_LEN-1);
	url_str[HTML_TEMP_STR_LEN-1] = 0;
  #else
  	WEBC_CHAR *url_str = _url_str;
  #endif

 	HTMLDocument *pDoc = HBROWSER_OBJ(browser)->GetDocument();

	return HBROWSER_OBJ(browser)->AddToHistory(url_str, 0, 0, pDoc);
}



/*****************************************************************************/
// webc_BrowserBack
/**

 @memo   Load the previous location in the history
 @doc    This function is functionally identical to pressing the "Back" button
         in an HTML Browser window and may be used to traverse hierarchical page structures.

 @param  browser  The handle of the HTML Browser
 @return 0 on success, -1 otherwise
 @see webc_BrowserForward, webc_BrowserHome, webc_BrowserRefresh, webc_BrowserLoadUrl
 */
/*****************************************************************************/

int webc_BrowserBack(HBROWSER_HANDLE browser)
{
	int result = HBROWSER_OBJ(browser)->Back();
	WEBC_BROWSER_LOAD_ALL(browser);
	return result;
}


/*****************************************************************************/
// webc_BrowserForward
/**

 @memo   Load the next location in the history
 @doc    This function is functionally identical to pressing the "Forward" button
         in an HTML Browser window, and may be used to traverse hierarchical page structures.

 @param  browser  The handle of the HTML Browser
 @return 0 on success, -1 otherwise
 @see webc_BrowserBack, webc_BrowserHome, webc_BrowserRefresh, webc_BrowserLoadUrl
 */
/*****************************************************************************/

int webc_BrowserForward(HBROWSER_HANDLE browser)
{
	int result = HBROWSER_OBJ(browser)->Forward();
	WEBC_BROWSER_LOAD_ALL(browser);
	return result;
}


/*****************************************************************************/
// webc_BrowserHome
/**

 @memo   Reload the home page
 @doc    This function is functionally identical to pressing the "Home" button
         in an HTML Browser window. <br><i>
		 Note: The home page is the url that was specified (if any) when a browser/window was first created.

 @param  browser  The handle of the HTML Browser
 @return 0 on success, -1 otherwise
 @see webc_BrowserForward, webc_BrowserBack, webc_BrowserRefresh, webc_BrowserLoadUrl
 */
/*****************************************************************************/

int webc_BrowserHome(HBROWSER_HANDLE browser)
{
	int result = HBROWSER_OBJ(browser)->Home();
	WEBC_BROWSER_LOAD_ALL(browser);
	return result;
}

WEBC_CHAR* webc_BrowserGetUrl(HBROWSER_HANDLE browser)
{
	if(HBROWSER_OBJ(browser)->LastUrl())
	{
		return HBROWSER_OBJ(browser)->LastUrl();
	}
	return WEBC_NULL;
}

/*****************************************************************************/
// webc_BrowserLoadUrl
/*X Removed from public API, see wcWinLoadUrl

 @memo   Load a location
 @doc    This function is functionally identical to entering a string in the
         URL bar of an HTML Browser window.

 @param  browser  The handle of the HTML Browser
 @param  url_str  The URL to load
 @return 0 on success, -1 otherwise
 @see webc_BrowserForward, webc_BrowserHome, webc_BrowserRefresh, webc_BrowserBack
 */
/*****************************************************************************/

int webc_BrowserLoadUrl(HBROWSER_HANDLE browser, char *_url_str)
{
  #if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR url_str[HTML_TEMP_STR_LEN];
	webc_c_strncpy(url_str, _url_str, HTML_TEMP_STR_LEN-1);
	url_str[HTML_TEMP_STR_LEN-1] = 0;
  #else
  	WEBC_CHAR *url_str = _url_str;
  #endif

 	WEBC_DEBUG_LOG_TIME_RESET("webc_BrowserLoadUrl")
	WEBC_DEBUG_LOG_TIME_HANDLE(TimeInside)
	WEBC_DEBUG_LOG_TIME_ENTER(TimeInside,"webc_BrowserLoadUrl",_url_str)

	int result = HBROWSER_OBJ(browser)->LoadUrl(url_str);
	WEBC_DEBUG_LOG_TIME_EXIT(TimeInside,"webc_BrowserLoadUrl",_url_str)

	WEBC_BROWSER_LOAD_ALL(browser);
	return result;
}


/*****************************************************************************/
// webc_BrowserStop
/*X Removed from public API

 @memo   Abort all pending load jobs in an HTML Browser
 @doc    This function stops an in-progress load by cancelling all outstanding
         jobs in the HTML Browser's load queue.  It is functionally identical
         to pressing the "Stop" button in the HTML Browser window.

 @param  browser  The handle of the HTML Browser
 @return 0 on success, -1 otherwise
 @see webc_BrowserForward, webc_BrowserHome, webc_BrowserRefresh, webc_BrowserBack, webc_BrowserLoadUrl
 */
/*****************************************************************************/

int webc_BrowserStop(HBROWSER_HANDLE browser)
{
	return HBROWSER_OBJ(browser)->StopAll();
}


/*****************************************************************************/
// webc_BrowserRefresh
/*X Removed from public API

 @memo   Reload the current page
 @doc    This function is functionally identical to pressing the "Refresh"
         button in the HTML Browser window.

 @param  browser  The handle of the HTML Browser
 @param  url_str  The URL to load
 @return 0 on success, -1 otherwise
 @see webc_BrowserForward, webc_BrowserHome, webc_BrowserRefresh, webc_BrowserBack, webc_BrowserLoadUrl
 */
/*****************************************************************************/

int webc_BrowserRefresh(HBROWSER_HANDLE browser)
{
	int result = HBROWSER_OBJ(browser)->Refresh();
	WEBC_BROWSER_LOAD_ALL(browser);
	return result;
}


/*****************************************************************************/
// webc_BrowserGetConfig
/*X Removed from public API

 @memo   Retrieve the current browser configuration
 @doc    This function can be used to obtain information about the current
         configuration of an HTML Browser.  The structure that is set by this
         function can also be modified and passed to webc_BrowserSetConfig to
         change the current configuration.

 @param  browser   The HTML Browser handle
 @param  config    Pointer to a pre-allocated instance of struct \Ref{HTMLBrowserConfig}
                   to fill with the current configuration.
 @return 0 on success, -1 otherwise
 @see    webc_BrowserSetConfig, HTMLBrowserConfig
 */
/*****************************************************************************/

int webc_BrowserGetConfig(HBROWSER_HANDLE browser, struct HTMLBrowserConfig *config)
{
	return HBROWSER_OBJ(browser)->GetConfig(config);
}




/*****************************************************************************/
// webc_BrowserSetPrivateData
/*X Removed from public API - see wcWin api

 @memo   Associate a data pointer with an HTML Browser
 @doc    The data passed into this function is not used by
         WebC.  The purpose of this routine is to associate
         application-specific data on a per-HTML Browser
         basis.  This data can be retrieved at any time using
         \Ref{webc_BrowserGetPrivateData}.

 @param  browser   The HTML Browser handle
 @param  data      The data pointer to associate
 @see    webc_BrowserGetPrivateData
 */
/*****************************************************************************/

void webc_BrowserSetPrivateData(HBROWSER_HANDLE browser, void* data)
{

	HBROWSER_OBJ(browser)->SetPrivateData(data);
}


/*****************************************************************************/
// webc_BrowserGetPrivateData
/*X Removed from public API

 @memo   Retrieve the user-specified data pointer associated with
         an HTML Browser
 @doc

 @param  browser   The HTML Browser handle
 @return The pointer passed into \Ref{webc_BrowserSetPrivateData} if there is one,
         WEBC_NULL otherwise
 @see webc_BrowserSetPrivateData
 */
/*****************************************************************************/

void* webc_BrowserGetPrivateData(HBROWSER_HANDLE browser)
{
	return HBROWSER_OBJ(browser)->GetPrivateData();
}


/*****************************************************************************/
// webc_BrowserGetDocument
/**

 @memo   (Deprecated for most uses) Get an HTML browser's root document handle
 @doc

 @param  browser   The HTML Browser handle
 @return The handle of the root document for this HTML browser
 @see
 */
/*****************************************************************************/

HDOC_HANDLE webc_BrowserGetDocument(HBROWSER_HANDLE browser)
{
	return (HDOC_HANDLE) (HBROWSER_OBJ(browser)->GetDocument());
}




/*****************************************************************************/
// webc_BrowserSuspendDraw
/*X removed from public API

 @memo
 @doc

 @param
 @return
 @see
 */
/*****************************************************************************/
void webc_BrowserSuspendDraw (
         HBROWSER_HANDLE hbrowser
     )
{
	HTMLBrowser* browser = (HTMLBrowser*) hbrowser;
	DisplayManager* manager = (browser)? browser->GetDisplayManager() : 0;
	if (manager)
	{
		DISPLAY_MANAGER_STOP_REFRESH(manager);
	}
}

/*****************************************************************************/
// webc_BrowserResumeDraw
/*X removed from public API

 @memo
 @doc

 @param
 @return
 @see
 */
/*****************************************************************************/
void webc_BrowserResumeDraw (
		HBROWSER_HANDLE hbrowser
	)
{
	HTMLBrowser* browser = (HTMLBrowser*) hbrowser;
	DisplayManager* manager = (browser)? browser->GetDisplayManager() : 0;
	if (manager)
	{
		DISPLAY_MANAGER_RESUME_REFRESH(manager);
	}
}

/*****************************************************************************/
// webc_BrowserGetDrawState
/*X removed from public API

 @memo
 @doc

 @param
 @return
 @see
 */
/*****************************************************************************/
int webc_BrowserGetDrawState (
		HBROWSER_HANDLE hbrowser
	)
{
	HTMLBrowser* browser = (HTMLBrowser*) hbrowser;
	DisplayManager* manager = (browser)? browser->GetDisplayManager() : 0;
	if (manager)
	{
		int refreshState = manager->GetRefreshState();
		return (refreshState);
	}

	return (-1);
}

/*****************************************************************************/
// webc_BrowserInvalidate
/*X removed from public API

 @memo
 @doc

 @param
 @return
 @see
 */
/*****************************************************************************/
void webc_BrowserInvalidate (
		HBROWSER_HANDLE hbrowser
	)
{
	HTMLBrowser* browser = (HTMLBrowser*) hbrowser;
	DisplayManager* manager = (browser)? browser->GetDisplayManager() : 0;
	if (manager)
	{
		manager->Invalidate();
	}
}

/*****************************************************************************/
// webc_BrowserDraw
/*X removed from public API

 @memo
 @doc

 @param
 @return
 @see
 */
/*****************************************************************************/
void webc_BrowserDraw (
		HBROWSER_HANDLE hbrowser
	)
{
	HTMLBrowser* browser = (HTMLBrowser*) hbrowser;
	DisplayManager* manager = (browser)? browser->GetDisplayManager() : 0;
	if (manager)
	{
		manager->Refresh();
	}
}

#if (USE_EXPERIMENTAL_TIMER_EVENT)
static int evdepth;
#endif
/*****************************************************************************/
// webc_BrowserDispatchEvent
/*X removed from public API, called by GUI layer to pass HID events to webC

 @memo
 @doc

 @param
 @return
 @see
 */
/*****************************************************************************/

HTMLEventStatus webc_BrowserDispatchEvent (
		HBROWSER_HANDLE hbrowser,
		HTMLEvent* event
	)
{
	HTMLBrowser* browser = (HTMLBrowser*) hbrowser;
	DisplayManager* manager = (browser)? browser->GetDisplayManager() : 0;
	if (manager)
	{
		HTMLEventStatus s;
#if (USE_EXPERIMENTAL_TIMER_EVENT)
		evdepth += 1;
		if (event->type == HTML_EVENT_TIMER)
		{
			if (evdepth > 1)
			{
				evdepth -= 1;
				return (HTML_EVENT_STATUS_DONE);
			}
			else
			{
				webc_BrowserProcessTimers((HBROWSER_HANDLE)browser);
				webc_BrowserDraw((HBROWSER_HANDLE)browser);
				webc_BrowserProcessLoadQueueAll((HBROWSER_HANDLE)browser);
				webc_BrowserDraw((HBROWSER_HANDLE)browser);
				evdepth -= 1;
				return (HTML_EVENT_STATUS_DONE);
			}
		}
		else
		{
			s = manager->DispatchEvent(event);
			evdepth -= 1;
		}
#else
		s = manager->DispatchEvent(event);
#endif
		return s;
	}

	return HTML_EVENT_STATUS_CONTINUE;
}

/*****************************************************************************/
// webc_BrowserProcessLoadQueueAll
/*X removed from public API

 @memo
 @doc

 @param
 @return
 @see
 */
/*****************************************************************************/
void webc_BrowserProcessLoadQueueAll (
		HBROWSER_HANDLE hbrowser
	)
{
	WEBC_BROWSER_LOAD_ALL(hbrowser);
}

/*****************************************************************************/
// webc_BrowserProcessTimers
/*X removed from public API.. must be called every 100 milliseconds or so by the gui timer daemon see doFbBrowserTimeOut()

 @memo
 @doc

 @param
 @return
 @see
 */
/*****************************************************************************/
void webc_BrowserProcessTimers (
		HBROWSER_HANDLE hbrowser
	)
{
	HBROWSER_OBJ(hbrowser)->TimeoutManager()->ProcessTime();
}



/* Webc level event passer */
typedef struct TimedEventStruct
{
    HELEMENT_HANDLE element;
    HTMLEvent Event;
} TIMEDEVENT;

static int PostTimeoutFunc(void* data, long msecError)
{
    TIMEDEVENT *ThisTimedEvent;
    ThisTimedEvent = (TIMEDEVENT *) data;
    webc_TriggerEvent(ThisTimedEvent->element, &(ThisTimedEvent->Event));
	return (0);
}

static int PostCancelfn(WEBC_PFBYTE data)
{
    TIMEDEVENT *ThisTimedEvent;
    ThisTimedEvent = (TIMEDEVENT *) data;
    printf("Not freeing has a bug\n");
    // WEBC_FREE(ThisTimedEvent);
	return (0);
}

/*****************************************************************************/
// webc_BrowserQueueTimedEvent
/*X removed from public API, see wcTriggerEvent() instead

 @memo
 @doc

 @return
 @see
 */
/*****************************************************************************/
int webc_BrowserQueueTimedEvent (
		HBROWSER_HANDLE hbrowser,
        HELEMENT_HANDLE element,
        HTMLEvent *pEvent,
		WEBC_INT32 msec
	)
{
WEBC_UINT16 timer_id;
TIMEDEVENT *ThisTimedEvent;

	if (!hbrowser || !element)
        return (-1);
	ThisTimedEvent =(TIMEDEVENT *) WEBC_MALLOC(sizeof(TIMEDEVENT));
	if (!ThisTimedEvent)
        return(-1);
	ThisTimedEvent->Event   = *pEvent;
	ThisTimedEvent->element = element;
	timer_id = HBROWSER_OBJ(hbrowser)->TimeoutManager()->SetTimeout((webcTimeoutFn) PostTimeoutFunc,
	                PostCancelfn,ThisTimedEvent, msec, 0);
	if (timer_id == WEBC_TIMEOUT_INVALID)
	{
        WEBC_FREE(ThisTimedEvent);
        return (-1);
    }
    else
    {
        return ((int) timer_id);
    }
}
static int _matchElement (TimeoutInfo* timeout, void* data)
{
	HELEMENT_HANDLE el = *((HELEMENT_HANDLE*)data);
	TIMEDEVENT *pTe = (TIMEDEVENT *) timeout->pData;
	return (pTe->element== el);
}

void webc_ElementCancelTimers(HBROWSER_HANDLE hbrowser, HELEMENT_HANDLE el)
{
	HBROWSER_OBJ(hbrowser)->TimeoutManager()->CancelTimeout(_matchElement, &el, WEBC_TRUE);
}

// <EDIT 20080120 - Created HTMLBrowser *pBrowser webc_CreateBrowserPlus() function which
// <EDIT 20080120 - returns a pointer to a browser instead of a handle. Called by c++
HTMLBrowser * webc_CreateBrowserPlus(struct HTMLBrowserConfig *config, WEBC_BOOL CreateWindow, char *_windowname, char *_home)
{
  #if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR home[HTML_TEMP_STR_LEN];
	webc_c_strncpy(home, _home, HTML_TEMP_STR_LEN-1);
	home[HTML_TEMP_STR_LEN-1] = 0;
  #else
  	WEBC_CHAR *home = _home;
  #endif
  #if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR windowname[HTML_TEMP_STR_LEN];
	webc_c_strncpy(windowname, _windowname, HTML_TEMP_STR_LEN-1);
	windowname[HTML_TEMP_STR_LEN-1] = 0;
  #else
  	WEBC_CHAR *windowname = _windowname;
  #endif

	HTMLBrowser *pBrowser = _webc_NewBrowser(config, CreateWindow, windowname, home);

	if (pBrowser && !gpFocusBrowser)
	{
		gpFocusBrowser = pBrowser;
	}

	WEBC_BROWSER_LOAD_ALL(pBrowser);

	return (pBrowser);
}

/*****************************************************************************/
// webc_CreateBrowser
/**

 @memo   (Deprecated for most uses) - Create a new HTML Browser window
 @doc    Allocates resources for and displays a new web browser window with
         the specified home page document.
		 <br><i>
		 Deprecated for most uses, use wcWinCreateEx() instead. Called by wcWinCreateEx(), webc_CreateBrowser() .
		 </i>
 @param  config  The initial configuration of the new browser window
 @param  CreateWindow If true call the GUI manager or frame buffer driver to create a new window.
 @param  windowname  Name of the window if one is being created.
 @param  home    URL of the initial page to load in this window.  The browser
                 will also return to this page when its 'Home' button (if it
                 is configured to have one) is pressed, or when
                 \Ref{webc_BrowserHome} is called.
 @return A handle to the newly created HTML Browser if successful, WEBC_NULL otherwise
 @see    webc_DestroyBrowser, HTMLBrowserConfig
 */
/*****************************************************************************/

HBROWSER_HANDLE  webc_CreateBrowser(struct HTMLBrowserConfig *config, WEBC_BOOL CreateWindow, char *_windowname, char *_home)
{
    return (HBROWSER_HANDLE) webc_CreateBrowserPlus(config, CreateWindow,_windowname, _home);
}


/*****************************************************************************/
// webc_DestroyBrowser
/**

 @memo    (Deprecated for most uses) - Close an HTML Browser window and free all its resources
 @doc    Call this to manually close an HTML Browser.  DO NOT JUST CALL delete!
 		 <br><i>
		 Deprecated for most uses, use wcWinClose() instead. Called by wcWinclose().
		 </i>
 @param  browser  The handle of the HTML Browser to close
 @param  status   If \Ref{webc_BrowserExecute} has been called on this browser,
                  then this parameter will determine the return value of
                  webc_BrowserExecute.  This is useful for simple dialog boxes
                  which ask the user to choose between several choices.
                  (see \Ref{How to use an HTML Browser as a dialog box})

 @see    webc_CreateBrowser, webc_BrowserExecute
 */
/*****************************************************************************/

void webc_DestroyBrowser(HBROWSER_HANDLE browser, int status)
{
	// this if structure is required, because UnLock will delete the object,
	//  if Destroyed() returns WEBC_TRUE.
	if (HBROWSER_OBJ(browser)->Destroyed())
	{
		HBROWSER_OBJ(browser)->UnLock();
	}
	else
	{
		HBROWSER_OBJ(browser)->UnLock();
		HBROWSER_OBJ(browser)->Destroy(status);
	}
}

// HEREHERE End webcsdkbrowser.cpp
