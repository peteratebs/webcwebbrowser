//
// HTMLSDK.CPP -
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
#include "rtpprint.h"
#include "webcdefault.h"
#ifdef WEBC_DEBUG
#include "webcLog.h"
#endif
#if (WEBC_DEBUG_JSCRIPT)
#include "wjsdebug.h"
#endif


/*****************************************************************************/
// Local types
/*****************************************************************************/
static char nphoney[1] = {0};
class CFunctionHandler : public HTMLEventHandler
{
public:
	CFunctionHandler(WEBC_CHAR *pName, HTMLEventCallback Function)
	{
		if (pName)
		{
			webc_strncpy(mpName, pName, 35);
			mpName[35] = '\0';
		}
		else
		{
			mpName[0] = '\0';
		}
		mFunction = Function;
	}
	~CFunctionHandler() {}

	virtual HTMLEventStatus Event(HTMLElement *pElement, HTMLEvent *pEvent, WEBC_CHAR *pParam = 0)
	{
		HTMLEventStatus result = HTML_EVENT_STATUS_CONTINUE;

		if (mFunction)
		{
			HTMLDocument *pDoc;
			HTMLBrowser  *pBrowser;

			pDoc = (pElement)? pElement->GetDocument() : 0;
			pBrowser = (pDoc)? pDoc->GetBrowser() : 0;


			char *param = webc_arg_to_char(pParam);

			if (!param)
			{
				param = nphoney;
			}

			result = mFunction((HBROWSER_HANDLE)pBrowser, (HDOC_HANDLE)pDoc, (HELEMENT_HANDLE)pElement, pEvent, param);

			if (param != nphoney)
			{
				webc_arg_done(param);
			}
		}

		return (result);
	}

	virtual WEBC_CHAR *Name(void) { return mpName; }

	WEBC_CHAR mpName[36];
	HTMLEventCallback mFunction;
};





#if (WEBC_SUPPORT_HTTP)




class SetAuthHandler : public HTMLEventHandler
{

public:
	virtual HTMLEventStatus Event(HTMLElement *pElement, HTMLEvent *pEvent, WEBC_CHAR *pParam = 0)
	{
		if ((pEvent->type == HTML_EVENT_CLICK) ||
		    (pEvent->type == HTML_EVENT_KEYDOWN && pEvent->data.key == WGK_CR))
		{
			HTMLDocument *pDoc;
			HTMLBrowser *pBrowser;
			HTMLInputHidden *pUrlInput;
			HTMLInputHidden *pRealmInput;
			HTMLEditString *pUserInput;
			HTMLInputPassword *pPassInput;

			pDoc = pElement->GetDocument();
			if (pDoc)
			{
				HTMLNameTypeFinder f1((WEBC_CHAR *) WEBC_STR_URL,  HTML_HIDDEN_INPUT_ELEMENT);
				HTMLNameTypeFinder f2((WEBC_CHAR *) WEBC_STR_USER, HTML_EDIT_STR_ELEMENT);
				HTMLNameTypeFinder f3((WEBC_CHAR *) WEBC_STR_PASS, HTML_INPUT_PASSWORD_ELEMENT);
				HTMLNameTypeFinder f4((WEBC_CHAR *) WEBC_STR_REALM,  HTML_HIDDEN_INPUT_ELEMENT);

				pUrlInput   = (HTMLInputHidden *)   pDoc->FindElement(&f1);
				pUserInput  = (HTMLEditString *)    pDoc->FindElement(&f2);
				pPassInput  = (HTMLInputPassword *) pDoc->FindElement(&f3);
				pRealmInput = (HTMLInputHidden *)   pDoc->FindElement(&f4);

				if (pUrlInput && pUserInput && pPassInput && pRealmInput)
				{
					pBrowser = pDoc->GetBrowser();
					if (pBrowser)
					{
						WEBC_CHAR * url = pUrlInput->Value();
						URLDescriptor urlDesc;
						if (urlDesc.parseURL(url) >= 0)
						{
							long ttl = 0;

							WEBC_CHAR ttlEnable[] = {'t','t','l','E','n','a','b','l','e',0};
							HTMLNameTypeFinder f5(ttlEnable,  HTML_CHECKBOX_ELEMENT);
							HTMLCheckbox *pCheck = (HTMLCheckbox *) pDoc->FindElement(&f5);
							if (pCheck)
							{
								WEBC_CHAR ttlMinutes[] = {'t','t','l','M','i','n','u','t','e','s',0};
								HTMLNameTypeFinder f6(ttlMinutes,  HTML_EDIT_STR_ELEMENT);
								HTMLEditString *pMin = (HTMLEditString *) pDoc->FindElement(&f6);
								if (pMin && pMin->Value())
								{
									long minutes;
									HTML_ParseDecNumber(pMin->Value(), 12, &minutes);
									ttl = minutes * 60;
								}
							}

							char *host      = webc_arg_to_char(urlDesc.getHost());
							char *path      = webc_arg_to_char(urlDesc.getPath());
							char *realmName = webc_arg_to_char(pRealmInput->Value());
							char *userName  = webc_arg_to_char(pUserInput->Value());
							char *password  = webc_arg_to_char(pPassInput->Value());

							http_set_authorization(host, urlDesc.getPort(), path, realmName, HTTP_AUTHENTICATION_BASIC,
							                       userName, password, ttl);

							webc_arg_done(password);
							webc_arg_done(userName);
							webc_arg_done(realmName);
							webc_arg_done(path);
							webc_arg_done(host);

							pBrowser->LoadUrl(url);
						}
					}
				}
			}

			return (HTML_EVENT_STATUS_DONE);
		}

		return (HTML_EVENT_STATUS_CONTINUE);
	}

	virtual WEBC_CHAR *Name(void) { return (WEBC_CHAR *) WEBC_STR_SETAUTH; }
};

#endif // WEBC_SUPPORT_HTTP


/*****************************************************************************/
// Global Data
/*****************************************************************************/
#if (WEBC_SUPPORT_USER_EVENTS)
static CFunctionHandler *gpCHandlerTable[WEBC_CFG_MAX_EVENT_HANDLERS] = {0};
#endif // WEBC_SUPPORT_USER_EVENTS

static int giWebcInitNesting = 0;

#if (WEBC_SUPPORT_JSCRIPT)
extern "C" WebcJSBrowserContext* gJSBrowserContext;
WebcJSBrowserContext* gJSBrowserContext = 0;
#endif

// Vector for keeping track of windows
struct WindowInfo
{
	HBROWSER_HANDLE Browser;
};
struct vector gBrowserWindowVector;



/*****************************************************************************/
// External Declarations
/*****************************************************************************/

extern HTMLEventQueue*            gpEventQueue;
extern TUOpenNoBr*                gpOpenNoBr;
extern TUCloseNoBr*               gpCloseNoBr;

#if (WEBC_SUPPORT_HTTPS)
extern WEBC_BOOL       gWebcSSLInitialized;
extern RTP_SSL_CONTEXT gWebcSSLContext;
#endif // WEBC_SUPPORT_HTTPS

/*****************************************************************************/
// Macros
/*****************************************************************************/

#define HBROWSER_OBJ(X)           ((HTMLBrowser *)(X))
#define HDOC_OBJ(X)               ((HTMLDocument *)(X))
#define HELEMENT_OBJ(X)           ((HTMLElement *)(X))
#define HTML_TEMP_STR_LEN         256

#ifndef WEBC_OLD_LOAD_METHOD
#define WEBC_BROWSER_LOAD_ALL(B)  HBROWSER_OBJ(B)->LoadAll()
#endif // WEBC_OLD_LOAD_METHOD

/*****************************************************************************/
// HTML C API Functions
/*****************************************************************************/

/*****************************************************************************/
// webc_Init
/**

 @memo  Initialize the Web Client
 @doc   This function must be called before any other API calls are made.

 @return 0 if successful, -1 otherwise
 @see    webc_Exit
 */
/*****************************************************************************/
#if (WEBC_SUPPORT_ERROR_PAGE_REDIRECT)
extern "C" {
	void webc_ClearErrorRedirects(void);
}
#endif

int webc_Init(void)
{
#ifdef WEBC_DEBUG
	// intialize logging
	CLoggingUtil::InitLogging();
#endif

	giWebcInitNesting++;
	if (giWebcInitNesting == 1)
	{
#if (WEBC_SUPPORT_FILE)
		rtp_file_init();
#endif
		//-----------------------------------------------------------
		// Restore all default values
		//-----------------------------------------------------------

		webc_RestoreDefaults ();

		//-----------------------------------------------------------
		// initialize the HTML event queue
		//-----------------------------------------------------------

		WEBC_NEW(gpEventQueue, HTMLEventQueue);

		//-----------------------------------------------------------
		// initialize misc objects used in page formatting
		//-----------------------------------------------------------

		WEBC_NEW(gpOpenNoBr, TUOpenNoBr);
		WEBC_NEW(gpCloseNoBr, TUCloseNoBr);

		webc_string_table_init(WEBC_CFG_STRING_TABLE_SIZE);

		//-----------------------------------------------------------
		// initialize javascript debugging.
		// Note:
		//
		// Registers stubs to replace native methods
		// must be called before creating the JScript
		// browser context.
		// Currently the tools can be useful but they need more work to be reliable
		//-----------------------------------------------------------
#if (WEBC_DEBUG_JSCRIPT)
        wjsdebug_init();
#endif

	  #if (WEBC_SUPPORT_JSCRIPT)
		WEBC_NEW(gJSBrowserContext, WebcJSBrowserContext());
	  #endif

      /* Initialize a vector to hold a list of open browser windows */
	  vector_init(&gBrowserWindowVector, sizeof(WindowInfo));

	  #if(WEBC_SUPPORT_ERROR_PAGE_REDIRECT)
	  /* Initialize arrary used to redirect page and server not found events to an application specific url */
	  webc_ClearErrorRedirects();
	  #endif

	}
	return (0);
}


/*****************************************************************************/
// webc_Exit
/**

 @memo  Free resources used by the Web Client
 @doc   This function should be called when then application exits

 @return 0 if successful
 @see    webc_Init
 */
/*****************************************************************************/
void webc_Fakeexit(void)
{

}
int webc_Exit(void)
{
	if (giWebcInitNesting > 0)
	{
		giWebcInitNesting--;

		if (giWebcInitNesting == 0)
		{

		  #if (WEBC_SUPPORT_USER_EVENTS)
			int n;

			for (n=0; n<WEBC_CFG_MAX_EVENT_HANDLERS; n++)
			{
				if (gpCHandlerTable[n])
				{
					WEBC_DELETE(gpCHandlerTable[n]);
					gpCHandlerTable[n] = 0;
				}
			}
		  #endif // WEBC_SUPPORT_USER_EVENTS

		  #if (WEBC_SUPPORT_HTTP)
			http_exit();
		  #endif

		  #if (WEBC_SUPPORT_HTTPS)
		  	if (gWebcSSLInitialized)
		  	{
		  		gWebcSSLInitialized = WEBC_FALSE;
		  		rtp_ssl_close_context(gWebcSSLContext);
		  	}
		  #endif // WEBC_SUPPORT_HTTPS

		  	// Release all TextUnitHolders used in page formatting algorithm
		  	//  (they are malloc'ed and then placed in a pool to be reused)
			LineContext::ReleaseAllHolders();

		  #if (WEBC_SUPPORT_COOKIES)
			webc_delete_cookies();
		  #endif

			HTMLElementClassEnumerator::CleanUp();

			webc_string_table_exit();

			webc_network_close();

			HTML_ResetEventHandlers();

			if (gpOpenNoBr)
			{
				WEBC_DELETE(gpOpenNoBr);
				gpOpenNoBr = 0;
			}

			if (gpCloseNoBr)
			{
				WEBC_DELETE(gpCloseNoBr);
				gpCloseNoBr = 0;
			}

			if (gpEventQueue)
			{
				WEBC_DELETE(gpEventQueue);
				gpEventQueue = 0;
			}

		  #if (WEBC_SUPPORT_JSCRIPT)
		  	if (gJSBrowserContext)
		  	{
		  		WEBC_DELETE(gJSBrowserContext);
		  	}
		  #endif
		}
	}

	return (0);
}

/*****************************************************************************/
// webc_clear_http_cache
/**

 @memo   Free any unnecessary resources
 @doc    An HTMLBrowser may allocate certain resources that are not released
         as soon as the browser is destroyed.  This function is different
         from webc_Exit because webc_Init need not be called after calling
         it and further API calls.

 @return nothing
 @see    webc_Exit, webc_Init
 */
/*****************************************************************************/

#if (WEBC_DEBUG_CACHING && WEBC_SUPPORT_CACHE)
extern void wcache_stats_clear(void);
#endif

void webc_clear_http_cache(void)
{
#if (WEBC_SUPPORT_HTTP && WEBC_SUPPORT_CACHE)
	http_clear_cache();
#if (WEBC_DEBUG_CACHING && WEBC_SUPPORT_CACHE)
	wcache_stats_clear();
#endif
#endif
}



void webc_FreeTempStore (void)
{
  #if (WEBC_SUPPORT_HTTP)
	http_exit();
  #endif

  #if (WEBC_SUPPORT_HTTPS)
  	if (gWebcSSLInitialized)
  	{
  		gWebcSSLInitialized = WEBC_FALSE;
  		rtp_ssl_close_context(gWebcSSLContext);
  	}
  #endif // WEBC_SUPPORT_HTTPS

  	// Release all TextUnitHolders used in page formatting algorithm
  	//  (they are malloc'ed and then placed in a pool to be reused)
	LineContext::ReleaseAllHolders();

  #if (WEBC_SUPPORT_COOKIES)
	webc_delete_cookies();
  #endif

	HTMLElementClassEnumerator::CleanUp();

  #if (WEBC_SUPPORT_JSCRIPT)
  	if (gJSBrowserContext)
  	{
  		gJSBrowserContext->FreeTempStore();
  	}
  #endif
}


HBROWSER_HANDLE webc_GetBrowserWithFocus()
{
	return((HBROWSER_HANDLE)gpFocusBrowser);
}


/*****************************************************************************/
// HTML Browser API Functions
/*****************************************************************************/

/*****************************************************************************/
// webc_CreateBrowser
/**

 @memo   Create a new HTML Browser window
 @doc    Allocates resources for and displays a new web browser window with
         the specified home page document.

 @param  config  The initial configuration of the new browser window
 @param  home    URL of the initial page to load in this window.  The browser
                 will also return to this page when its 'Home' button (if it
                 is configured to have one) is pressed, or when
                 \Ref{webc_BrowserHome} is called.
 @return A handle to the newly created HTML Browser if successful, WEBC_NULL otherwise
 @see    webc_DestroyBrowser, HTMLBrowserConfig
 */
/*****************************************************************************/

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


HBROWSER_HANDLE  webc_CreateBrowser(struct HTMLBrowserConfig *config, WEBC_BOOL CreateWindow, char *_windowname, char *_home)
{
    return (HBROWSER_HANDLE) webc_CreateBrowserPlus(config, CreateWindow,_windowname, _home);
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
// webc_DestroyBrowser
/**

 @memo   Close an HTML Browser window and free all its resources
 @doc    Call this to manually close an HTML Browser.  DO NOT JUST CALL delete!

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


/*****************************************************************************/
// webc_BrowserBack
/**

 @memo   Load the previous location in the history
 @doc    This function is functionally identical to pressing the "Back" button
         in an HTML Browser window.

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
         in an HTML Browser window.

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
         in an HTML Browser window.

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


/*****************************************************************************/
// webc_BrowserLoadUrl
/**

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
/**

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
/**

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
/**

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
// webc_BrowserAddToHistory
/**

 @memo   Add a location to the HTML Browser history
 @doc    Can be used to directly modify the URL history.

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
// webc_BrowserExecute
/**

 @memo   Process the input queue until this Browser is destroyed.
 @doc    This function will continuously process the input queue associated
         with the given browser until the browser has been destroyed via
         \Ref{webc_DestroyBrowser}

 @param  browser   The HTML Browser handle
 @return The value passed into \Ref{webc_DestoryBrowser}
 @see    webc_DestoryBrowser
 */
/*****************************************************************************/

int webc_BrowserExecute(HBROWSER_HANDLE browser)
{
	// First unlock this browser so it can be closed
	HBROWSER_OBJ(browser)->UnLock();
	return HBROWSER_OBJ(browser)->Execute();
}


/*****************************************************************************/
// webc_BrowserSetPrivateData
/**

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
/**

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

 @memo   Get an HTML browser's root document handle
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
// webc_BrowserGetDocFocus
/**

 @memo
 @doc

 @param
 @return
 @see
 */
/*****************************************************************************/
HDOC_HANDLE webc_BrowserGetDocFocus(HBROWSER_HANDLE browser)
{
//	return ((HDOC_HANDLE)HBROWSER_OBJ(browser)->GetFocusDoc());
	return 0;
}

/*****************************************************************************/
// webc_DocFocus
/**

 @memo
 @doc

 @param
 @return
 @see
 */
/*****************************************************************************/
void webc_DocFocus(HDOC_HANDLE doc, WEBC_BOOL refresh)
{
	if (!doc)
		return;
	HELEMENT_HANDLE hbody = webc_DocFindElement(doc, 0, 0, HTML_BODY_ELEMENT, 0);
	if (hbody)
	{
		webc_ElementFocus(hbody, refresh);
	}
}


/*****************************************************************************/
// webc_BrowserSetTagEventHandler
/**

 @memo   Set the default event handler for an HTML tag type
 @doc

 @param
 @return nothing
 @see
 */
/*****************************************************************************/
int webc_BrowserSetTagEventHandler (
		HBROWSER_HANDLE wc,
		HTMLTagType tagType,
		HTMLEventCallback cb
	)
{
  #if (WEBC_SUPPORT_USER_EVENTS)
	CFunctionHandler* eventHandler;

	WEBC_NEW(eventHandler, CFunctionHandler(0, cb));
	if (eventHandler)
	{
		if (HBROWSER_OBJ(wc)->SetTagEventHandler(tagType, eventHandler) >= 0)
		{
			return (0);
		}

		WEBC_DELETE(eventHandler);
	}
  #endif // WEBC_SUPPORT_USER_EVENTS

	return (-1);
}


/*****************************************************************************/
// webc_BrowserGetDocFocus
/**

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
// webc_BrowserGetDocFocus
/**

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
// webc_BrowserGetDocFocus
/**

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
// webc_BrowserGetDocFocus
/**

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
// webc_BrowserGetDocFocus
/**

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

/*****************************************************************************/
// webc_BrowserGetDocFocus
/**

 @memo
 @doc

 @param
 @return
 @see
 */
/*****************************************************************************/
#if (USE_EXPERIMENTAL_TIMER_EVENT)
static int evdepth;
#endif
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
// webc_BrowserGetDocFocus
/**

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
// webc_BrowserGetDocFocus
/**

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

/*****************************************************************************/
// webc_BrowserResetDrawDone
/**

 @memo
 @doc

 @param
 @return
 @see
 */
/*****************************************************************************/
void webc_BrowserResetDrawDone (
        HBROWSER_HANDLE hbrowser
    )
{
    if (hbrowser)
    {
        DisplayManager * manager = ((HTMLBrowser *)hbrowser)->GetDisplayManager();
        if (manager)
            manager->ResetDrawDone();
    }
}

/*****************************************************************************/
// webc_BrowserWasDrawDone
/**

 @memo
 @doc

 @param
 @return
 @see
 */
/*****************************************************************************/
HTML_BOOL webc_BrowserWasDrawDone (
        HBROWSER_HANDLE hbrowser
    )
{
    if (hbrowser)
    {
        DisplayManager * manager = ((HTMLBrowser *)hbrowser)->GetDisplayManager();
        if (manager)
            return manager->WasDrawDone() ? WEBC_TRUE : WEBC_FALSE;
    }
    return WEBC_FALSE;
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
    WEBC_FREE(ThisTimedEvent);
	return (0);
}

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

/*****************************************************************************/
// HTML Document API Functions
/*****************************************************************************/

/*****************************************************************************/
// webc_DocFindElement
/**

 @memo			Search for an element by id, name, type, and/or index
 @doc			Does a depth-first search of the document tree for an element
                matching all of the search criteria

 @param doc		The handle of the document to search
 @param id      The \Ref{ID} of the element to search for (WEBC_NULL to match any id)
 @param name	The \Ref{NAME} of the element to search for (WEBC_NULL to match any name)
 @param type    The \Ref{TYPE} of element to search for (HTML_ELEMENT_ANY to match any type)
 @param index   The index of the element to find (HTML_INDEX_LAST for the last
                element in the document)
 @return		The handle of the Nth element with the specified id, name, and/or type,
                where N is the above specified index; WEBC_NULL if not found.
 @see           HDOC_HANDLE, HELEMENT_HANDLE
 */
/*****************************************************************************/

HELEMENT_HANDLE webc_DocFindElement (HDOC_HANDLE doc, char *id, char *name, HTMLElementType type, long index)
{
HTMLIdNameTypeIndexFinder8 f(id, name, type, index);
	if (!doc)
		return (0);
	return (HELEMENT_HANDLE) (HDOC_OBJ(doc)->FindElement(&f));
}


/*****************************************************************************/
// webc_DocSetUrl
/**

 @memo			Set the URL of a document
 @doc			Sets the URL of the specified document.  The new page will not be loaded
                (unless 'refresh' is set to WEBC_TRUE) until \Ref{webc_DocRefresh} is called
                on this document.

 @param doc		The handle of the document to set the url for.
 @param url     The URL of the document to load
 @param refresh If TRUE, then load the given URL immediately and refresh the display;
                otherwise wait until \Ref{webc_DocRefresh} is called.
 @see           HDOC_HANDLE, webc_DocRefresh
 */
/*****************************************************************************/

void webc_DocSetUrl(HDOC_HANDLE doc, char *_url, HTML_BOOL refresh)
{
  #if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR url[HTML_TEMP_STR_LEN];
	webc_c_strncpy(url, _url, HTML_TEMP_STR_LEN-1);
	url[HTML_TEMP_STR_LEN-1] = 0;
  #else
  	WEBC_CHAR *url = _url;
  #endif

	HDOC_OBJ(doc)->SetSrc(url);
	HDOC_OBJ(doc)->Update(1);
	if (refresh)
	{
		WEBC_BROWSER_LOAD_ALL(HDOC_OBJ(doc)->GetBrowser());
	}
}


/*****************************************************************************/
// webc_DocRefresh
/**

 @memo			Update the document and refresh the display
 @doc			Loads any outstanding URLs (including the source for the document)
                and redraws the document.

 @param doc		The handle of the document to refresh.
 @see           HDOC_HANDLE
 */
/*****************************************************************************/

void webc_DocRefresh(HDOC_HANDLE doc)
{
	HDOC_OBJ(doc)->Update(1);
	WEBC_BROWSER_LOAD_ALL(HDOC_OBJ(doc)->GetBrowser());
}


/*****************************************************************************/
// webc_DocReload
/**

 @memo			Reloads the current URL
 @doc

 @param doc		The handle of the document to refresh.
 @see           HDOC_HANDLE
 */
/*****************************************************************************/

void webc_DocReload(HDOC_HANDLE doc)
{
	HDOC_OBJ(doc)->QueueLoad(HDOC_OBJ(doc)->CurrentURL(), WEBC_TRUE);
	WEBC_BROWSER_LOAD_ALL(HDOC_OBJ(doc)->GetBrowser());
}


/*****************************************************************************/
// webc_DocClear
/**

 @memo			Clear all document content
 @doc			Will discard the content of this document and free all associated
                memory.

 @param doc		The handle of the document to clear.
 @param refresh Boolean - whether to redraw the document window
 @see           HDOC_HANDLE
 */
/*****************************************************************************/

void webc_DocClear(HDOC_HANDLE doc, HTML_BOOL refresh)
{
	HDOC_OBJ(doc)->Clear();
	if (refresh)
	{
		HDOC_OBJ(doc)->Refresh();
	}
}


/*****************************************************************************/
// webc_DocWriteHtml
/**

 @memo			Write html code directly to a document
 @doc			This function will append html code onto the end of the
                existing document.  The string passed to this function is
                treated exactly like html code arriving via HTTP or FILE.

 @param doc		 The handle of the document
 @param html_src Pointer to a null-terminated buffer of html source.
 @param length   The length of html_src.
 @param refresh  Whether to redraw the document window
 @return The number of characters successfully written to the document.
 @see           HDOC_HANDLE, webc_DocClear
 */
/*****************************************************************************/

long webc_DocWriteHtml(HDOC_HANDLE doc, char *html_src, long length, HTML_BOOL refresh)
{
int closedoc = 0;
long result;
WEBC_CHAR *to_write = 0;

	if (!HDOC_OBJ(doc)->IsOpen())
	{
		HDOC_OBJ(doc)->Open();
		closedoc = 1;
	}

	//allocate the right amount of memory
	to_write = (WEBC_CHAR *) WEBC_MALLOC((length + 1) * sizeof(WEBC_CHAR));
	if (!to_write)
	{
		return (-1);
	}

	webc_c_strncpy(to_write, html_src, length);
	to_write[length] = 0;

	result = HDOC_OBJ(doc)->Write(to_write, length);

	WEBC_FREE(to_write);

	if (closedoc)
	{
		HDOC_OBJ(doc)->Close();
	}

	if (refresh)
	{
		WEBC_BROWSER_LOAD_ALL(HDOC_OBJ(doc)->GetBrowser());
		HDOC_OBJ(doc)->Refresh();
	}

	return result;
}


/*****************************************************************************/
// webc_DocWriteString
/**

 @memo			Write string directly to a document as is
 @doc			This function will copy and change this string so that it will
				appear as is in the document.

 @param doc		 The handle of the document
 @param str      Pointer to a null-terminated string.
 @param length   The length of html_src.
 @param refresh  Whether to redraw the document window
 @return The number of characters successfully written to the document.
 @see           HDOC_HANDLE, webc_DocClear
 */
/*****************************************************************************/

long webc_DocWriteString(HDOC_HANDLE doc, char *str, long length, HTML_BOOL refresh)
{
int closedoc = 0;
long result = 0;
long new_str_size = 0;
WEBC_CHAR *to_write = 0;

	//find the size of the new string
	new_str_size = webc_escape_string(0, str);

	//allocate the right amount of memory
	to_write = (WEBC_CHAR *) WEBC_MALLOC((new_str_size + 1) * sizeof(WEBC_CHAR));
	if (!to_write)
	{
		return (-1); // malloc error
	}

	webc_escape_string(to_write, str);

	if (!HDOC_OBJ(doc)->IsOpen())
	{
		HDOC_OBJ(doc)->Open();
		closedoc = 1;
	}

	result = HDOC_OBJ(doc)->Write(to_write);

	if (closedoc)
	{
		HDOC_OBJ(doc)->Close();
	}

	if (refresh)
	{
		WEBC_BROWSER_LOAD_ALL(HDOC_OBJ(doc)->GetBrowser());
		HDOC_OBJ(doc)->Refresh();
	}

	WEBC_FREE(to_write);
	return result;
}


/*****************************************************************************/
// webc_DocOpen
/**

 @memo		Open an HTML Document for writing
 @doc			This function must preceed any calls to webc_DocWriteHtml and
            webc_DocWriteString

 @param doc		 The handle of the document to be opened
 @return    0 if the operation was successful, -1 otherwise.
 @see           HDOC_HANDLE, webc_DocClear, webc_DocClose, webc_DocWriteHtml,
                webc_DocWriteString
 */
/*****************************************************************************/

int webc_DocOpen(HDOC_HANDLE doc)
{
	return HDOC_OBJ(doc)->Open();
}


/*****************************************************************************/
// webc_DocClose
/**

 @memo		Close an HTML Document after writing content
 @doc			This function should be called on an HTML Document as soon as
            the user is finnished writing HTML and text content to it
            (\Ref{webc_DocWriteHtml}, \Ref{webc_DocWriteString}).

 @param doc		 The handle of the document to be opened
 @return    0 if the operation was successful, -1 otherwise.
 @see       HDOC_HANDLE, webc_DocClear, webc_DocOpen, webc_DocWriteHtml,
            webc_DocWriteString
 */
/*****************************************************************************/

void webc_DocClose(HDOC_HANDLE doc)
{
	HDOC_OBJ(doc)->Close();
}


/*****************************************************************************/
// webc_DocGetBrowser
/**

 @memo		Retrieve an HTML Document's parent HTML Browser
 @doc

 @param doc	The handle of the document
 @return    The \Ref{HBROWSER_HANDLE} of the HTML Browser containing the given document.
 @see       HDOC_HANDLE
 */
/*****************************************************************************/

HBROWSER_HANDLE webc_DocGetBrowser(HDOC_HANDLE doc)
{
	return ((HBROWSER_HANDLE)HDOC_OBJ(doc)->GetBrowser());
}


/*****************************************************************************/
// webc_DocGetTitleASCII
/**

 @memo		tbd
 @doc	    tbd

 @param doc	The handle of the document
 @return    tbd
 @see       tbd
 */
/*****************************************************************************/

char* webc_DocGetTitleASCII (HDOC_HANDLE doc)
{
	return webc_MallocASCIIString(HDOC_OBJ(doc)->GetTitle());
}


/*****************************************************************************/
// webc_DocSaveForOffline
/**

 @memo		Retrieve an HTML Document's parent HTML Browser
 @doc

 @param doc	The handle of the document
 @return    The \Ref{HBROWSER_HANDLE} of the HTML Browser containing the given document.
 @see       HDOC_HANDLE
 */
/*****************************************************************************/

void webc_DocSaveForOffline(HDOC_HANDLE hdoc)
{
  #if (WEBC_SUPPORT_OFFLINE_BROWSING)
	HDOC_OBJ(hdoc)->SaveContentForOffline();
  #endif // WEBC_SUPPOR_TOFFLINE_BROWSING
}

void webc_SetOnlineStatus (
		WEBC_BOOL isOnline
	)
{
  #if (WEBC_SUPPORT_OFFLINE_BROWSING)
	giWebcOnline = isOnline;
  #endif // WEBC_SUPPOR_TOFFLINE_BROWSING
}

WEBC_BOOL webc_GetOnlineStatus (void)
{
  #if (WEBC_SUPPORT_OFFLINE_BROWSING)
	return (webc_online());
  #else
	return WEBC_TRUE;
  #endif // WEBC_SUPPOR_TOFFLINE_BROWSING
}


/*****************************************************************************/
// HTML Element API Functions
/*****************************************************************************/

/*****************************************************************************/
// webc_ElementGetType
/**
 @memo          Get element type
 @doc           Returns element's type
 @param element The element whose type to return
 @return        The element type
 @see           HELEMENT_HANDLE, HTMLElementType
 */
/*****************************************************************************/

HTMLElementType webc_ElementGetType(HELEMENT_HANDLE element)
{
	if (!element)
		return (HTMLElementType)0;
	return (HELEMENT_OBJ(element)->Type());
}

int webc_ElementSetTagEventHandler (HELEMENT_HANDLE element, HTMLEventCallback cb)
{
#if (WEBC_SUPPORT_USER_EVENTS)
	if (element)
	{
		CFunctionHandler* eventHandler;
		WEBC_NEW(eventHandler, CFunctionHandler(0, cb));
		if (eventHandler)
		{
	    	HTMLEvent the_event;
	    	HELEMENT_OBJ(element)->SetElementEventHandler(eventHandler);
	    	the_event.type = HTML_EVENT_OBJECT_CREATED;
	    	webc_TriggerEvent(element, &the_event);
	    	return(0);
		}
   }
#endif // WEBC_SUPPORT_USER_EVENTS
   return(-1);
}

/*****************************************************************************/
// webc_ElementGetDocument
/**
 @memo          Get the handle of the document containing a given element
 @doc           Gets the handle of the document that contains the given
                element.
 @param element The element whose document handle to return
 @return        Handle of the parent document of this element.
 @see           HELEMENT_HANDLE, HDOC_HANDLE
 */
/*****************************************************************************/

HDOC_HANDLE webc_ElementGetDocument(HELEMENT_HANDLE element)
{
	if (!element)
		return (0);
	return (HDOC_HANDLE) (HELEMENT_OBJ(element)->GetDocument());
}


/*****************************************************************************/
// webc_ElementNext
/**
 @memo          Find next element in document
 @doc           Searches the document this element is a part of, starting
                at the given element, for the next element that matches
                all the given criteria
 @param element The element to search from
 @param id      The \Ref{ID} of the element to search for (WEBC_NULL to match any id)
 @param name	The \Ref{NAME} of the element to search for (WEBC_NULL to match any name)
 @param type    The \Ref{TYPE} of element to search for (HTML_ELEMENT_ANY to match any type)
 @param index   The offset from the given element of the element to find (0 to match the
                first element after this one matching all the other criteria)
 @return        The handle of the matching element or WEBC_NULL if not found
 @see           HELEMENT_HANDLE, webc_ElementPrev
 */
/*****************************************************************************/

HELEMENT_HANDLE webc_ElementNext(HELEMENT_HANDLE element, char *id, char *name, HTMLElementType type, long index)
{
	if (!element)
		return (0);
HTMLNextByIdNameTypeIndexFinder f(HELEMENT_OBJ(element), id, name, type, index);
HTMLDocument *pDoc = HELEMENT_OBJ(element)->GetDocument();

	if (pDoc)
	{
		return (HELEMENT_HANDLE) (pDoc->FindElement(&f));
	}

	return (0);
}


/*****************************************************************************/
// webc_ElementPrev
/**
 @memo          Find prev element in document
 @doc           Searches the document this element is a part of, starting
                at the given element, for the previous element that matches
                all the given criteria
 @param element The element to search from
 @param id      The \Ref{ID} of the element to search for (WEBC_NULL to match any id)
 @param name	The \Ref{NAME} of the element to search for (WEBC_NULL to match any name)
 @param type    The \Ref{TYPE} of element to search for (HTML_ELEMENT_ANY to match any type)
 @param index   The offset from the given element of the element to find (0 to match the
                last element before this one matching all the other criteria)
 @return        The handle of the matching element or WEBC_NULL if not found
 @see           HELEMENT_HANDLE, webc_ElementNext
 */
/*****************************************************************************/

HELEMENT_HANDLE webc_ElementPrev(HELEMENT_HANDLE element, char *id, char *name, HTMLElementType type, long index)
{
	if (!element)
		return (0);
HTMLPrevByIdNameTypeIndexFinder f(HELEMENT_OBJ(element), id, name, type, index);
HTMLDocument *pDoc = HELEMENT_OBJ(element)->GetDocument();

	if (pDoc)
	{
		pDoc->FindElement(&f);
		return (HELEMENT_HANDLE) (f.Found());
	}

	return (0);
}


/*****************************************************************************/
// webc_ElementGetChild
/**
 @memo          Search for a child of an element by id, name, type, and/or index
 @doc           Searches the document subtree at the given element for a child
                element that matches the given search criteria.
 @param element The parent element whose children to search
 @param id      The \Ref{ID} of the element to search for (WEBC_NULL to match any id)
 @param name	The \Ref{NAME} of the element to search for (WEBC_NULL to match any name)
 @param type    The \Ref{TYPE} of element to search for (HTML_ELEMENT_ANY to match any type)
 @param index   The index of the element to find (HTML_INDEX_LAST for the last
                element in the document)
 @return        The handle of the matching element or WEBC_NULL if not found
 @see           HELEMENT_HANDLE
 */
/*****************************************************************************/

HELEMENT_HANDLE webc_ElementGetChild (HELEMENT_HANDLE element, char *id, char *name, HTMLElementType type, long index)
{
	if (!element)
		return (0);
HTMLIdNameTypeIndexFinder8 f(id, name, type, index);
	if (!element)
		return (0);
	return (HELEMENT_HANDLE) (HELEMENT_OBJ(element)->FindElement(&f));
}


/*****************************************************************************/
// webc_ElementGetParent
/**
 @memo          Get the immediate parent of an element
 @doc           Returns the direct parent of an element in the document tree.
 @param element The element whose parent to get.
 @return        The handle of the parent element or WEBC_NULL if none.
 @see           HELEMENT_HANDLE, webc_ElementGetFirstChild, webc_ElementGetLastChild,
                webc_ElementGetNextSibling, webc_ElementGetPrevSibling
 */
/*****************************************************************************/

HELEMENT_HANDLE webc_ElementGetParent (HELEMENT_HANDLE element)
{
	if (!element)
		return (0);
	return (HELEMENT_HANDLE) (HELEMENT_OBJ(element)->mpParent);
}


/*****************************************************************************/
// webc_ElementGetFirstChild
/**
 @memo          Get the first child of an element
 @doc           Returns the first child element of the given element.
 @param element The parent element
 @return        The handle of the first child element or WEBC_NULL if no children.
 @see           HELEMENT_HANDLE, webc_ElementGetParent, webc_ElementGetLastChild,
                webc_ElementGetNextSibling, webc_ElementGetPrevSibling
 */
/*****************************************************************************/

HELEMENT_HANDLE webc_ElementGetFirstChild (HELEMENT_HANDLE element)
{
	if (!element)
		return (0);
	return (HELEMENT_HANDLE) (HELEMENT_OBJ(element)->FirstChild());
}


/*****************************************************************************/
// webc_ElementGetLastChild
/**
 @memo          Get the last child of an element
 @doc           Returns the last child element of the given element.
 @param element The parent element
 @return        The handle of the last child element or WEBC_NULL if no children.
 @see           HELEMENT_HANDLE, webc_ElementGetParent, webc_ElementGetFirstChild,
                webc_ElementGetNextSibling, webc_ElementGetPrevSibling
 */
/*****************************************************************************/

HELEMENT_HANDLE webc_ElementGetLastChild (HELEMENT_HANDLE element)
{
	if (!element)
		return (0);
	return (HELEMENT_HANDLE) (HELEMENT_OBJ(element)->LastChild());
}


/*****************************************************************************/
// webc_ElementGetNextSibling
/**
 @memo          Get the next sibling of an element
 @doc           Returns the next sibling element of the given element.
 @param element The element whose sibling to get
 @return        The handle of the next sibling element or WEBC_NULL if this is the
                last child.
 @see           HELEMENT_HANDLE, webc_ElementGetParent, webc_ElementGetFirstChild,
                webc_ElementGetLastChild, webc_ElementGetPrevSibling
 */
/*****************************************************************************/

HELEMENT_HANDLE webc_ElementGetNextSibling (HELEMENT_HANDLE element)
{
	if (!element)
		return (0);
	return (HELEMENT_HANDLE) (HELEMENT_OBJ(element)->mpNext);
}


/*****************************************************************************/
// webc_ElementGetPrevSibling
/**
 @memo          Get the prev sibling of an element
 @doc           Returns the prev sibling element of the given element.
 @param element The element whose sibling to get
 @return        The handle of the prev sibling element or WEBC_NULL if this is the
                first child.
 @see           HELEMENT_HANDLE, webc_ElementGetParent, webc_ElementGetFirstChild,
                webc_ElementGetLastChild, webc_ElementGetNextSibling
 */
/*****************************************************************************/

HELEMENT_HANDLE webc_ElementGetPrevSibling (HELEMENT_HANDLE element)
{
	if (!element)
		return (0);
	return (HELEMENT_HANDLE) (HELEMENT_OBJ(element)->mpPrev);
}


/*****************************************************************************/
// webc_ElementGetName
/**
 @memo          Get an element's name
 @doc           Returns the name string of this element.
 @param element The element whose name to get
 @return        The element's name or WEBC_NULL if it doesn't have one.
 @see           HELEMENT_HANDLE, NAME, webc_ElementSetName
 */
/*****************************************************************************/

char *webc_ElementGetNameASCII(HELEMENT_HANDLE element)
{
    if (element && HELEMENT_OBJ(element)->Name())
	    return webc_MallocASCIIString(HELEMENT_OBJ(element)->Name());
	else
	    return(0);
}


/*****************************************************************************/
// webc_ElementGetId
/**
 @memo          Get an element's unique id
 @doc           Returns the unique id string of this element.
 @param element The element whose id to get
 @return        The element's id or WEBC_NULL if it doesn't have one.
 @see           HELEMENT_HANDLE, ID
 */
/*****************************************************************************/

char *webc_ElementGetIdASCII(HELEMENT_HANDLE element)
{
    if (element && HELEMENT_OBJ(element)->Id())
	    return webc_MallocASCIIString(HELEMENT_OBJ(element)->Id());
	else
	    return(0);
}


/*****************************************************************************/
// webc_ElementGetValueASCII
/**
 @memo          Get the value of an input widget
 @doc           If the given element handle is an HTML_EDIT_STR_ELEMENT (text field), HTML_EDITBOX_ELEMENT (text box),
                or HTML_SELECT_ELEMENT (combo box), then this function will
                return the currently entered value of the widget.  For element types that have
                no input value (everything that is not a widget), this function will return WEBC_NULL.
                To retrieve the status of boolean type input widgets (such as radio buttons and
                checkboxes), use \Ref{webc_ElementGetChecked}.
 @param element The element whose value to get
 @return        The element's value or WEBC_NULL if it doesn't have one.
 @see           HELEMENT_HANDLE, VALUE, webc_ElementSetValue, webc_ElementGetChecked
 */
/*****************************************************************************/

char* webc_ElementGetValueASCII(HELEMENT_HANDLE element)
{
    if (element && HELEMENT_OBJ(element)->Value())
    {
   	    return webc_MallocASCIIString(HELEMENT_OBJ(element)->Value());
   	}
   	else
   	    return(WEBC_NULL);
}


/*****************************************************************************/
// webc_ElementGetValueUC
/**
 @memo          Get the value of an input widget
 @doc           If the given element handle is an HTML_EDIT_STR_ELEMENT (text field), HTML_EDITBOX_ELEMENT (text box),
                or HTML_SELECT_ELEMENT (combo box), then this function will
                return the currently entered value of the widget.  For element types that have
                no input value (everything that is not a widget), this function will return WEBC_NULL.
                To retrieve the status of boolean type input widgets (such as radio buttons and
                checkboxes), use \Ref{webc_ElementGetChecked}.
 @param element The element whose value to get
 @return        The element's value or WEBC_NULL if it doesn't have one.
 @see           HELEMENT_HANDLE, VALUE, webc_ElementSetValue, webc_ElementGetChecked
 */
/*****************************************************************************/

WEBC_CHAR16* webc_ElementGetValueUC(HELEMENT_HANDLE element)
{
  if (element && HELEMENT_OBJ(element)->Value())
  {
  #if (WEBC_SUPPORT_UNICODE)
   	return webc_MallocUCString(HELEMENT_OBJ(element)->Value());
  #else
	return WEBC_NULL;
  #endif
  }
  else
  	return WEBC_NULL;
}

/*****************************************************************************/
// webc_ElementGetChecked
/**
 @memo          Get the status of a checkbox or radio button
 @doc           If the given element handle is an HTML_RADIO_BUTTON_ELEMENT or a
                HTML_CHECKBOX_ELEMENT, then this function will return the element's
                value string (specified in the source HTML or through a call
                to \Ref{webc_ElementSetValue}) IF it is currently selected; if
                it is not currently selected, this function will return WEBC_NULL.
 @param element The element whose value to get
 @return        The element's value if checked or WEBC_NULL if it is not checked
 @see           HELEMENT_HANDLE, VALUE, CHECKED, webc_ElementSetChecked, webc_ElementSetValue
 */
/*****************************************************************************/

WEBC_BOOL webc_ElementGetChecked(HELEMENT_HANDLE element)
{
	if (!element)
		return (WEBC_FALSE);
	return (HELEMENT_OBJ(element)->Checked());
}


/*****************************************************************************/
// webc_ElementGetSrcASCII
/**
 @memo          Get the source url for an image or frame element
 @doc           This function only returns meaningful values for frame and
                image elements.
 @param element The element whose src to get
 @return        The element's src or WEBC_NULL if it has none.
 @see           HELEMENT_HANDLE, SRC, webc_ElementSetSrc,
 */
/*****************************************************************************/

char* webc_ElementGetSrcASCII(HELEMENT_HANDLE element)
{
    if (element && HELEMENT_OBJ(element)->Src())
	    return webc_MallocASCIIString(HELEMENT_OBJ(element)->Src());
	else
	    return(WEBC_NULL);
}


/*****************************************************************************/
// webc_ElementGetColor
/**
 @memo          Get an element's foreground color
 @doc           Get the element's foreground color
 @param element The element whose color to get
 @param color   Pointer to an html_color struct to fill in with the
                color information for this element.
 @return        WEBC_TRUE if color was filled with a valid color,
                WEBC_FALSE otherwise
 @see           HELEMENT_HANDLE, COLOR, BGCOLOR, webc_ElementSetColor,
 */
/*****************************************************************************/

HTML_BOOL webc_ElementGetColor(HELEMENT_HANDLE element, HTMLColor* color)
{
CSSPropertyValue value;

	if (!element)
		return (WEBC_FALSE);
	if ((HELEMENT_OBJ(element)->GetStyleFromCSS(CSS_PROPERTY_COLOR, &value) >= 0) ||
		(HELEMENT_OBJ(element)->GetStyleFromHTML(CSS_PROPERTY_COLOR, &value) >= 0))
	{
		*color = value.color;
		return (WEBC_TRUE);
	}

	return (WEBC_FALSE);
}


/*****************************************************************************/
// webc_ElementGetBgColor
/**
 @memo          Get an element's background color
 @doc           Get the element's background color
 @param element The element whose background color to get
 @param color   Pointer to an html_color struct to fill in with the
                background color information for this element.
 @return        WEBC_TRUE if color was filled with a valid color,
                WEBC_FALSE otherwise
 @see           HELEMENT_HANDLE, COLOR, BGCOLOR, webc_ElementSetColor,
 */
/*****************************************************************************/

HTML_BOOL webc_ElementGetBgColor(HELEMENT_HANDLE element, HTMLColor* color)
{
CSSPropertyValue value;

	if (!element)
		return (WEBC_FALSE);
	if ((HELEMENT_OBJ(element)->GetStyleFromCSS(CSS_PROPERTY_BACKGROUND_COLOR, &value) >= 0) ||
		(HELEMENT_OBJ(element)->GetStyleFromHTML(CSS_PROPERTY_BACKGROUND_COLOR, &value) >= 0))
	{
		*color = value.color;
		return (WEBC_TRUE);
	}

	return (WEBC_FALSE);
}


/*****************************************************************************/
// webc_ElementGetBgImage
/**
 @memo          Get an element's background image url
 @doc           This function only returns meaningful values for body,
                table, tr, and td elements.
 @param element The element whose background image url to get
 @return        The element's background url or WEBC_NULL if it has none.
 @see           HELEMENT_HANDLE, BGIMAGE, webc_ElementSetBgImage,
 */
/*****************************************************************************/

char* webc_ElementGetBgImageASCII(HELEMENT_HANDLE element)
{
CSSPropertyValue value;

	if (!element)
		return 0;
	if ((HELEMENT_OBJ(element)->GetStyleFromCSS(CSS_PROPERTY_BACKGROUND_IMAGE, &value) >= 0) ||
		(HELEMENT_OBJ(element)->GetStyleFromHTML(CSS_PROPERTY_BACKGROUND_IMAGE, &value) >= 0))
	{
		return webc_MallocASCIIString(value.url);
	}

	return (0);
}


/*****************************************************************************/
// webc_ElementGetWidth
/**
 @memo          Get an element's width in pixels
 @doc           Gets an element's width in pixels
 @param element The element whose width to get
 @return        The element's width in pixels
 @see           HELEMENT_HANDLE, WIDTH, HEIGHT, webc_ElementSetWidth,
                webc_ElementGetHeight, webc_ElementSetHeight
 */
/*****************************************************************************/

HTMLLength webc_ElementGetWidth(HELEMENT_HANDLE element)
{
CSSPropertyValue value;
HTMLLength w = 0;
	if (!element)
		return 0;
	if ((HELEMENT_OBJ(element)->GetStyleFromCSS(CSS_PROPERTY_WIDTH, &value) >= 0) ||
		(HELEMENT_OBJ(element)->GetStyleFromHTML(CSS_PROPERTY_WIDTH, &value) >= 0))
	{
		w = value.length.val.dec;
	}
	return (w);
}


/*****************************************************************************/
// webc_ElementGetHeight
/**
 @memo          Get an element's height in pixels
 @doc           Gets an element's height in pixels
 @param element The element whose height to get
 @return        The element's height in pixels
 @see           HELEMENT_HANDLE, HEIGHT, WIDTH, webc_ElementSetHeight,
                webc_ElementGetWidth, webc_ElementSetWidth
 */
/*****************************************************************************/

HTMLLength webc_ElementGetHeight(HELEMENT_HANDLE element)
{
CSSPropertyValue value;
HTMLLength h = 0;
	if (!element)
		return 0;
	if ((HELEMENT_OBJ(element)->GetStyleFromCSS(CSS_PROPERTY_HEIGHT, &value) >= 0) ||
		(HELEMENT_OBJ(element)->GetStyleFromHTML(CSS_PROPERTY_HEIGHT, &value) >= 0))
	{
		h = value.length.val.dec;
	}
	return (h);
}

/*****************************************************************************/
// webc_ElementGetFrameDocument
/**
 @memo          Get a frame's child document
 @doc           Gets a frame's child document.
 @param element The frame element handle
 @return        The handle of the HTML Document or WEBC_NULL if this element is not a frame.
 @see           HELEMENT_HANDLE, HDOC_HANDLE, webc_BrowserGetDocument
 */
/*****************************************************************************/

HDOC_HANDLE webc_ElementGetFrameDocument(HELEMENT_HANDLE element)
{
	if (!element)
		return 0;
  #if (WEBC_SUPPORT_FRAMES)
	if (HELEMENT_OBJ(element)->Type() == HTML_FRAME_ELEMENT ||
	    HELEMENT_OBJ(element)->Type() == HTML_IFRAME_ELEMENT)
	{
		return (HDOC_HANDLE) (((HTMLFrame*)(element))->FrameDocument());
	}
  #endif // WEBC_SUPPORT_FRAMES

	return WEBC_NULL;
}


/*****************************************************************************/
// webc_ElementGetInnerHTML
/**
 @memo          Get an element's inner HTML
 @doc           Get an element's inner HTML
 @param element The element handle
 @return        A string containing the inner HTML or WEBC_NULL
 @see           HELEMENT_HANDLE
 */
/*****************************************************************************/

WEBC_CHAR* webc_ElementGetInnerHTML(HELEMENT_HANDLE element)
{
	WEBC_CHAR *retVal = WEBC_NULL;
	if (!element)
		return 0;
#if (WEBC_SUPPORT_INNER_HTML)

	 WebString tempString;

	((HTMLElement*)(element))->AppendInnerHTML(&tempString);

	retVal = (WEBC_CHAR *)WEBC_MALLOC((tempString.getLength() + 1) * sizeof(WEBC_CHAR)); // caller must free

	webc_strcpy(retVal, tempString.getChars());

#endif // WEBC_SUPPORT_INNER_HTML

	return retVal;
}


/*****************************************************************************/
// webc_ElementGetOuterHTML
/**
 @memo          Get an element's outer HTML
 @doc           Get an element's outer HTML
 @param element The element handle
 @return        A string containing the outer HTML or WEBC_NULL
 @see           HELEMENT_HANDLE
 */
/*****************************************************************************/

WEBC_CHAR* webc_ElementGetOuterHTML(HELEMENT_HANDLE element)
{
	WEBC_CHAR *retVal = WEBC_NULL;

	if (!element)
		return 0;

#if (WEBC_SUPPORT_INNER_HTML)

	 WebString tempString;

	 ((HTMLElement*)(element))->AppendOuterHTML(&tempString);

	retVal = (WEBC_CHAR *)WEBC_MALLOC((tempString.getLength() + 1) * sizeof(WEBC_CHAR)); // caller must free

	webc_strcpy(retVal, tempString.getChars());

#endif // WEBC_SUPPORT_INNER_HTML

	return retVal;
}

// webc_ElementSetValue
/**
 @memo          Set the current value for an input widget
 @doc           For text entry widgets (HTML_EDIT_STR_ELEMENT, HTML_EDITBOX_ELEMENT),
                this function sets the current text in the widget.  For boolean-type
                widgets (HTML_CHECKBOX_ELEMENT, HTML_RADIO_BUTTON_ELEMENT), this
                function will set the value string that will be returned by
                \Ref{webc_ElementGetChecked} if the element's status is 'checked'.
                To set the currently selected item in combo boxes (HTML_SELECT_ELEMENT),
                use \Ref{webc_ElementSetSelected}.
 @param element The element whose value to get
 @param value   The string to set the element's value to
 @param refresh If WEBC_TRUE, then redraw this element
 @see           HELEMENT_HANDLE, VALUE, webc_ElementGetValue, webc_ElementGetChecked,
                webc_ElementSetChecked, webc_ElementSetSelected, webc_ElementRefresh
 */
/*****************************************************************************/

int webc_ElementSetValueASCII (HELEMENT_HANDLE element, const char *_value, HTML_BOOL refresh)
{
	if (!element)
		return -1;
  #if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR value[HTML_TEMP_STR_LEN];
	webc_c_strncpy(value, _value, HTML_TEMP_STR_LEN-1);
	value[HTML_TEMP_STR_LEN-1] = 0;
  #else
  	const WEBC_CHAR* value = _value;
  #endif

    HELEMENT_OBJ(element)->SetValue(value);
    if (refresh)
    {
		HELEMENT_OBJ(element)->Update(0,(WEBC_PFBYTE)1);
		HELEMENT_OBJ(element)->Refresh();
    }
	return 0;
}

/*****************************************************************************/
// webc_ElementSetValueUC
/**
 @memo          Set the current value for an input widget
 @doc           For text entry widgets (HTML_EDIT_STR_ELEMENT, HTML_EDITBOX_ELEMENT),
                this function sets the current text in the widget.  For boolean-type
                widgets (HTML_CHECKBOX_ELEMENT, HTML_RADIO_BUTTON_ELEMENT), this
                function will set the value string that will be returned by
                \Ref{webc_ElementGetChecked} if the element's status is 'checked'.
                To set the currently selected item in combo boxes (HTML_SELECT_ELEMENT),
                use \Ref{webc_ElementSetSelected}.
 @param element The element whose value to get
 @param value   The string to set the element's value to
 @param refresh If WEBC_TRUE, then redraw this element
 @see           HELEMENT_HANDLE, VALUE, webc_ElementGetValue, webc_ElementGetChecked,
                webc_ElementSetChecked, webc_ElementSetSelected, webc_ElementRefresh
 */
/*****************************************************************************/
int webc_ElementSetValueUC (
		HELEMENT_HANDLE element,
		const WEBC_CHAR16* value,
		HTML_BOOL refresh
	)
{
	if (!element)
		return -1;
  #if (WEBC_SUPPORT_UNICODE)
    HELEMENT_OBJ(element)->SetValue(value);
    if (refresh)
    {
		HELEMENT_OBJ(element)->Update(0,(WEBC_PFBYTE)1);
		HELEMENT_OBJ(element)->Refresh();
    }
  #endif
    return 0;
}

/*****************************************************************************/
// webc_ElementSetChecked
/**
 @memo          Set checked status for checkbox or radio button
 @doc           If the element handle passed to this routine is that of an HTML_CHECKBOX_ELEMENT,
                or an HTML_RADIO_BUTTON_ELEMENT, then the 'checked' status of the element
                will be set according to the boolean 'checked' parameter.  If this function
                is called with a 'checked' value of WEBC_TRUE, then subsequent calls to
                \Ref{webc_ElementGetChecked} will return the string set as the value for this
                element, via \Ref{webc_ElementSetValue}.  If WEBC_FALSE is passed to this
                routine, then subsequent calls to \Ref{webc_ElementGetChecked} will return
                WEBC_NULL.
 @param element The element to set
 @param checked Whether or not the element should be checked
 @param refresh If WEBC_TRUE, then redraw this element
 @see           HELEMENT_HANDLE, CHECKED, webc_ElementGetChecked, webc_ElementSetValue, webc_ElementRefresh
 */
/*****************************************************************************/

int webc_ElementSetChecked(HELEMENT_HANDLE element, HTML_BOOL checked, HTML_BOOL refresh)
{
	if (!element)
		return -1;

	HELEMENT_OBJ(element)->SetChecked(checked);
	if (refresh)
	{
		HELEMENT_OBJ(element)->Update(0,0);
		HELEMENT_OBJ(element)->Refresh();
	}
	return 0;
}


/*****************************************************************************/
// webc_ElementSetName
/**
 @memo          Set an element's name
 @doc           Sets the name attribute for the specified element
 @param element The element to set
 @param name    The new name
 @param refresh If WEBC_TRUE, then redraw this element
 @see           HELEMENT_HANDLE, NAME, webc_ElementGetName, webc_ElementRefresh
 */
/*****************************************************************************/

int webc_ElementSetName(HELEMENT_HANDLE element, char *_name, HTML_BOOL refresh)
{
	if (!element)
		return -1;

  #if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR name[HTML_TEMP_STR_LEN];
	webc_c_strncpy(name, _name, HTML_TEMP_STR_LEN-1);
	name[HTML_TEMP_STR_LEN-1] = 0;
  #else
  	WEBC_CHAR *name = _name;
  #endif

	HELEMENT_OBJ(element)->SetName(name);
	if (refresh)
	{
		HELEMENT_OBJ(element)->Update(0,(WEBC_PFBYTE)1);
		HELEMENT_OBJ(element)->Refresh();
	}
	return 0;
}


/*****************************************************************************/
// webc_ElementSetSrc
/**
 @memo          Set the source url for an image or frame element
 @doc           Sets the src attribute for the specified element.  Only affects
                HTML_FRAME_ELEMENT and HTML_IMAGE_ELEMENT.  Doesn't load or redraw
                (unless 'refresh' is set to WEBC_TRUE) until \Ref{webc_ElementRefresh} is called.
 @param element The element to set
 @param src     The new src
 @param refresh If WEBC_TRUE, then load the new url and redraw this element
 @see           HELEMENT_HANDLE, SRC, webc_ElementGetSrc, webc_ElementRefresh
 */
/*****************************************************************************/

int webc_ElementSetSrcASCII(HELEMENT_HANDLE element, char *_src, HTML_BOOL refresh)
{
	if (!element)
		return -1;
  #if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR src[HTML_TEMP_STR_LEN];
	webc_c_strncpy(src, _src, HTML_TEMP_STR_LEN-1);
	src[HTML_TEMP_STR_LEN-1] = 0;
  #else
  	WEBC_CHAR *src = _src;
  #endif

  	HELEMENT_OBJ(element)->SetSrc(src);

	if (refresh)
	{
		HELEMENT_OBJ(element)->Update(0,0);
		WEBC_BROWSER_LOAD_ALL(HELEMENT_OBJ(element)->GetBrowser());
		HELEMENT_OBJ(element)->Refresh();
	}
	return 0;
}


/*****************************************************************************/
// webc_ElementSetColor
/**
 @memo          Set an element's foreground color
 @doc           Sets the foreground color for the given element and its children.
 @param element The element to set
 @param color   Pointer to html_color struct with new foreground color
 @param refresh If WEBC_TRUE, then redraw this element
 @see           HELEMENT_HANDLE, COLOR, webc_ElementGetColor, webc_ElementRefresh
 */
/*****************************************************************************/

void webc_ElementSetColor(HELEMENT_HANDLE element, HTMLColor color, HTML_BOOL refresh)
{
	char ascii_colorStr[32];
    /* sprintf .. not tested, not good solution */
	rtp_sprintf(ascii_colorStr, "#%02x%02x%02x",
			HTML_ColorGetRed(color),
			HTML_ColorGetGreen(color),
			HTML_ColorGetBlue(color));
//     webc_ElementSetAttributeASCII (
//		element,
//		"color",
//		ascii_colorStr);
//		return;
  #if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR colorStr[HTML_TEMP_STR_LEN];
	webc_c_strncpy(colorStr, ascii_colorStr, HTML_TEMP_STR_LEN-1);
	colorStr[HTML_TEMP_STR_LEN-1] = 0;
  #else
  	WEBC_CHAR *colorStr = ascii_colorStr;
  #endif

	HELEMENT_OBJ(element)->SetProperty(HTML_ATTRIB_COLOR, colorStr);

	if (refresh)
	{
		HELEMENT_OBJ(element)->Update(0,0);
		HELEMENT_OBJ(element)->Refresh();
	}
}


/*****************************************************************************/
// webc_ElementSetBgColor
/**
 @memo          Set an element's background color
 @doc           Sets the background color for the given element and its children.
 @param element The element to set
 @param color   Pointer to html_color struct with new background color
 @param refresh If WEBC_TRUE, then redraw this element
 @see           HELEMENT_HANDLE, BGCOLOR, webc_ElementGetBgColor, webc_ElementRefresh
 */
/*****************************************************************************/

void webc_ElementSetBgColor(HELEMENT_HANDLE element, HTMLColor color, HTML_BOOL refresh)
{
	char ascii_colorStr[32];
    /* sprintf .. not tested, not good solution */
	rtp_sprintf(ascii_colorStr, "#%02x%02x%02x",
			HTML_ColorGetRed(color),
			HTML_ColorGetGreen(color),
			HTML_ColorGetBlue(color));
  #if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR colorStr[HTML_TEMP_STR_LEN];
	webc_c_strncpy(colorStr, ascii_colorStr, HTML_TEMP_STR_LEN-1);
	colorStr[HTML_TEMP_STR_LEN-1] = 0;
  #else
  	WEBC_CHAR *colorStr = ascii_colorStr;
  #endif

	HELEMENT_OBJ(element)->SetProperty(HTML_ATTRIB_BGCOLOR, colorStr);

	if (refresh)
	{
		HELEMENT_OBJ(element)->Update(0,0);
		HELEMENT_OBJ(element)->Refresh();
	}
}


/*****************************************************************************/
// webc_ElementSetBgImage
/**
 @memo          Set an element's background image url
 @doc           Sets the background image url for the specified element.  Only affects
                HTML_BODY_ELEMENT, HTML_TABLE_ELEMENT, HTML_TABLE_CELL_ELEMENT,
                and HTML_TABLE_ROW_ELEMENT.  Doesn't load the new bitmap or redraw
                (unless 'refresh' is set to WEBC_TRUE) until \Ref{webc_ElementRefresh} is called.
 @param element The element to set
 @param bgimg   The new background image url
 @param refresh If WEBC_TRUE, then load the new url and redraw this element
 @see           HELEMENT_HANDLE, BGIMAGE, webc_ElementGetBgImage, webc_ElementRefresh
 */
/*****************************************************************************/

void webc_ElementSetBgImageASCII(HELEMENT_HANDLE element, char *_bgimg, HTML_BOOL refresh)
{
  #if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR bgimg[HTML_TEMP_STR_LEN];
	webc_c_strncpy(bgimg, _bgimg, HTML_TEMP_STR_LEN-1);
	bgimg[HTML_TEMP_STR_LEN-1] = 0;
  #else
  	WEBC_CHAR *bgimg = _bgimg;
  #endif

	HELEMENT_OBJ(element)->SetProperty(HTML_ATTRIB_BACKGROUND, bgimg);
	if (refresh)
	{
		HELEMENT_OBJ(element)->Update(0,0);
		WEBC_BROWSER_LOAD_ALL(HELEMENT_OBJ(element)->GetBrowser());
		HELEMENT_OBJ(element)->Refresh();
	}
}

/*****************************************************************************/
// webc_ElementEnable
/**
 @memo          Set an element to be selectable
 @doc           Will shade in an input widget and make it modifiable.

 @param element The element to set
 @param refresh If WEBC_TRUE, then redraw this element
 @see           HELEMENT_HANDLE, VISIBILITY, webc_ElementSetHidden
 */
/*****************************************************************************/

int webc_ElementEnable(HELEMENT_HANDLE element, HTML_BOOL refresh)
{
	if (!element)
		return -1;
	HELEMENT_OBJ(element)->SetDisabled(WEBC_FALSE);
	if (refresh)
	{
		HELEMENT_OBJ(element)->Update(0,0);
		HELEMENT_OBJ(element)->Refresh();
	}
	return 0;
}


/*****************************************************************************/
// webc_ElementDisable
/**
 @memo          Set an element to not be selectable
 @doc           Will shade out an input widget and make it non-modifiable.

 @param element The element to set
 @param refresh If WEBC_TRUE, then redraw this element
 @see           HELEMENT_HANDLE, VISIBILITY, webc_ElementSetHidden
 */
/*****************************************************************************/

int webc_ElementDisable(HELEMENT_HANDLE element, HTML_BOOL refresh)
{
	if (!element)
		return -1;
	HELEMENT_OBJ(element)->SetDisabled(WEBC_TRUE);
	if (refresh)
	{
		HELEMENT_OBJ(element)->Update(0,0);
		HELEMENT_OBJ(element)->Refresh();
	}
	return 0;
}


/*****************************************************************************/
// webc_ElementFocus
/**
 @memo          Set the current input focus to an element
 @doc           Makes the given element the input focus for keyboard and
                mouse events.
 @param element The element to set
 @param refresh If WEBC_TRUE, then redraw this element
 @see           HELEMENT_HANDLE, webc_ElementRefresh
 */
/*****************************************************************************/

int webc_ElementFocus(HELEMENT_HANDLE element, HTML_BOOL refresh)
{
	if (element)
	{
		HELEMENT_OBJ(element)->Focus();
		if (refresh)
		{
			HELEMENT_OBJ(element)->Refresh();
			WEBC_BROWSER_LOAD_ALL(HELEMENT_OBJ(element)->GetBrowser());
		}
		return 0;
	}
	return -1;
}

/*****************************************************************************/
// webc_DocGetElementFocus
/**
 @memo
 @doc
 @see
 */
/*****************************************************************************/

HELEMENT_HANDLE webc_DocGetElementFocus(HDOC_HANDLE doc)
{
	if (doc)
		return ((HELEMENT_HANDLE)HDOC_OBJ(doc)->GetFocusElement());
	return 0;
}

/*****************************************************************************/
// webc_ElementSetPrivateData
/**
 @memo
 @doc
 @param element The element to set
 @see           HELEMENT_HANDLE, webc_ElementGetPrivateData
 */
/*****************************************************************************/

int webc_ElementSetPrivateData (
		HELEMENT_HANDLE helem,
		void* data
	)
{
	if (!helem)
		return -1;
	HELEMENT_OBJ(helem)->SetPrivateData(data);
	return 0;
}

/*****************************************************************************/
// webc_ElementGetPrivateData
/**
 @memo
 @doc
 @param element The element to set
 @param
 @see           HELEMENT_HANDLE, webc_ElementSetPrivateData
 */
/*****************************************************************************/

void* webc_ElementGetPrivateData (
		HELEMENT_HANDLE helem
	)
{
	return HELEMENT_OBJ(helem)->GetPrivateData();
}

/*****************************************************************************/
// webc_ElementSetWidth
/**
 @memo          Set an element's width (Deprecated)
 @doc           Sets an element's width in pixels
 @param element The element to set
 @param w       Width in pixels
 @param refresh If WEBC_TRUE, then redraw this element
 @see           HELEMENT_HANDLE, WIDTH, webc_ElementGetWidth, webc_ElementSetHeight,
                webc_ElementGetHeight
 */
/*****************************************************************************/
void webc_ElementSetWidthINT(HELEMENT_HANDLE element, int value, HTML_BOOL refresh)
{
	char ascii_widthStr[32];
    /* sprintf .. not tested, not good /solution */
	rtp_sprintf(ascii_widthStr, "%d", value);
    webc_ElementSetWidthASCII(element, ascii_widthStr, refresh);
}

void webc_ElementSetWidthASCII(HELEMENT_HANDLE element, char* _value, HTML_BOOL refresh)
{
  #if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR value[HTML_TEMP_STR_LEN];
	webc_c_strncpy(value, _value, HTML_TEMP_STR_LEN-1);
	value[HTML_TEMP_STR_LEN-1] =0;
  #else
	WEBC_CHAR* value = _value;
  #endif

	HELEMENT_OBJ(element)->SetProperty(HTML_ATTRIB_WIDTH, value);
	if (refresh)
	{
		HELEMENT_OBJ(element)->Update(0,0);
		HELEMENT_OBJ(element)->Refresh();
	}
}


/*****************************************************************************/
// webc_ElementSetHeight
/**
 @memo          Set an element's height (Deprecated)
 @doc           Sets an element's height in pixels
 @param element The element to set
 @param h       Height in pixels
 @param refresh If WEBC_TRUE, then redraw this element
 @see           HELEMENT_HANDLE, HEIGHT, webc_ElementGetHeight, webc_ElementGetWidth,
                webc_ElementSetWidth
 */
/*****************************************************************************/
void webc_ElementSetHeightINT(HELEMENT_HANDLE element, int value, HTML_BOOL refresh)
{
	char ascii_heightStr[32];
    /* sprintf .. not tested, not good solution */
	rtp_sprintf(ascii_heightStr, "%d", value);
    webc_ElementSetHeightASCII(element, ascii_heightStr, refresh);
}

void webc_ElementSetHeightASCII(HELEMENT_HANDLE element, char* _value, HTML_BOOL refresh)
{
  #if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR value[HTML_TEMP_STR_LEN];
	webc_c_strncpy(value, _value, HTML_TEMP_STR_LEN-1);
	value[HTML_TEMP_STR_LEN-1] =0;
  #else
	WEBC_CHAR* value = _value;
  #endif

	HELEMENT_OBJ(element)->SetProperty(HTML_ATTRIB_HEIGHT, value);
	if (refresh)
	{
		HELEMENT_OBJ(element)->Update(0,0);
		HELEMENT_OBJ(element)->Refresh();
	}
}


/*****************************************************************************/
// webc_ElementSetStyleASCII
/**
 @memo          Set style properties for an element
 @doc           This function will be operational once CSS support has
                been added to WebC.
 @param element The element to set style info for.
 @param style   Cascading style sheet source data (e.g. "a.class1 { font-size: 12pt; }")
 @param refresh If WEBC_TRUE, then redraw this element
 @see           HELEMENT_HANDLE, STYLE
 */
/*****************************************************************************/

int webc_ElementSetStyleASCII(HELEMENT_HANDLE element, char *_style, HTML_BOOL refresh)
{
#if (WEBC_SUPPORT_STYLE_SHEETS)
HTMLDocument *pDocument;

	if (!element)
		return -1;
  #if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR style[HTML_TEMP_STR_LEN];
	webc_c_strncpy(style, _style, HTML_TEMP_STR_LEN-1);
	style[HTML_TEMP_STR_LEN-1] = 0;
  #else
  	WEBC_CHAR *style = _style;
  #endif

	pDocument = HELEMENT_OBJ(element)->GetDocument();
	if (pDocument)
	{
		pDocument->GetCSSContext()->ParseElementStyle(HELEMENT_OBJ(element), style, pDocument->CurrentURL());
		HELEMENT_OBJ(element)->StyleUpdate();

		if (refresh == WEBC_TRUE)
		{
			HELEMENT_OBJ(element)->Update(0,(WEBC_PFBYTE)1);
			HELEMENT_OBJ(element)->Refresh();
//			if ((HELEMENT_OBJ(element)->mpParent))
//			{
//			printf("Refresh parent\n");
//				(HELEMENT_OBJ(element)->mpParent)->StyleUpdate();
//
//				(HELEMENT_OBJ(element)->mpParent)->Update(0,(WEBC_PFBYTE)1);
//				(HELEMENT_OBJ(element)->mpParent)->Refresh();
//			}
//			HELEMENT_OBJ(element)->Refresh();
//			WEBC_BROWSER_LOAD_ALL(HELEMENT_OBJ(element)->GetBrowser());
		}
	}
#endif // (WEBC_SUPPORT_STYLE_SHEETS)
	return 0;
}


/*****************************************************************************/
// webc_ElementSetSelected
/**
 @memo           Set the selected option in a combo box
 @doc            Sets the index (beginning with 0) of the selected item in a combo
                 box.  The value (string) of the selected item of a combo box can
                 be retrieved via webc_ElementGetValue.
 @param element  The element to set.
 @param selected Index of the option to select.
 @param refresh  If WEBC_TRUE, then redraw this element
 @see           HELEMENT_HANDLE, SELECTED, webc_ElementGetValue
 */
/*****************************************************************************/

int webc_ElementSetSelected(HELEMENT_HANDLE element, int selected, HTML_BOOL refresh)
{
	if (!element)
		return -1;
	if (HELEMENT_OBJ(element)->Type() == HTML_SELECT_ELEMENT)
	{
		HTMLSelect *pSelect = (HTMLSelect *) HELEMENT_OBJ(element);

		pSelect->SetSelectedIndex(selected);
		if (refresh)
		{
			pSelect->Refresh();
		}
	}
	return 0;
}


/*****************************************************************************/
// webc_RegisterEventCallback
/**
 @memo           Set a function to use as a custom event handler
 @doc            After this function is called, whenever an HTML tag loaded
                 into WebC specifies its 'eventhandler' attribute (a WebC
                 HTML extension) as the name passed in, all events that happen
                 to that tag will be passed on to the C function specified as
                 cb.

 @param cb       The event handler callback.
 @param name     The name of this event handler
 @return         0 if the callback was successfully registered; -1 otherwise
 @see           HELEMENT_HANDLE, SELECTED, webc_ElementGetValue
 */
/*****************************************************************************/

int webc_RegisterEventCallback (HTMLEventCallback cb, char *argName)
{
  #if (WEBC_SUPPORT_USER_EVENTS)
	int n;
	WEBC_CHAR* name;

   #if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR unicodeName[64];
	webc_c_strncpy(unicodeName, argName, 64);
	unicodeName[63] = 0;
	name = unicodeName;
   #else
	name = argName;
   #endif // WEBC_SUPPORT_UNICODE

	for (n=0; n<WEBC_CFG_MAX_EVENT_HANDLERS; n++)
	{
		if (gpCHandlerTable[n] && !webc_strcmp(gpCHandlerTable[n]->Name(), name))
		{
			gpCHandlerTable[n]->mFunction = cb;
			return (0);
		}

		if (!gpCHandlerTable[n])
		{
			WEBC_NEW(gpCHandlerTable[n], CFunctionHandler(name, cb));
			if (gpCHandlerTable[n])
			{
				HTML_RegisterEventHandler(gpCHandlerTable[n]);
				return (0);
			}
			break;
		}
	}
  #endif // WEBC_SUPPORT_USER_EVENTS

	return (-1);
}

HTML_BOOL webc_ElementIdMatches(HELEMENT_HANDLE element, char *string)
{
	if (HELEMENT_OBJ(element)->Id() && string && !webc_c_stricmp(HELEMENT_OBJ(element)->Id(), string))
	{
		return (WEBC_TRUE);
	}

	return (WEBC_FALSE);
}

#if (WEBC_SUPPORT_JSCRIPT)


/*****************************************************************************/
/**
 @memo Call Javascript from native 'C' and execute the provided javascript source code

 @doc This function allows a C function to called javascript to compile and execute source code.
		This function returns a handle to a script.
		The function webc_JScriptDestroyScript() must be called to stop the script and free
        the script resources.

 @param doc any created document
 @param source the Javascript source code to compile and execute
 @return a handle to the script
*/
/*****************************************************************************/

HSCRIPT_HANDLE webc_JScriptExecuteSource(HDOC_HANDLE doc, const char *_source)
{
  #if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR source[HTML_TEMP_STR_LEN];
	webc_c_strncpy(source, _source, HTML_TEMP_STR_LEN-1);
	source[HTML_TEMP_STR_LEN-1] = 0;
  #else
  	const WEBC_CHAR *source = _source;
  #endif
  /* Okay.. here we go */
  WebcJSDocumentContext *jsmgr = HDOC_OBJ(doc)->GetJSMgr();
  JSObject *scopeobj = HDOC_OBJ(doc)->GetJSObject();
  JSContext *cx = HDOC_OBJ(doc)->GetJSContext();

  WebcJSScript *pScript;
  WEBC_NEW(pScript, WebcJSScript(jsmgr, scopeobj, source, webc_strlen(source)));
  if (pScript)
  {
    jsval Result;
    pScript->ExecuteScript(&Result);
  }
  return (HSCRIPT_HANDLE) pScript;
}



/*****************************************************************************/
/**
 @memo Destroy a and free reources of a script that was created by webc_JScriptExecuteSource

 @doc This function allows a C function to called javascript to compile and execute source code.
		This function returns a handle to a script.
		The function webc_JScriptDestroyScript() must be called to stop the script and free
        the script resources.

 @param hscript a script handle returned from webc_JScriptExecuteSource
 @return void
*/
/*****************************************************************************/
void webc_JScriptDestroyScript(HSCRIPT_HANDLE  hscript)
{
  WebcJSScript *pScript = (WebcJSScript *) hscript;
  if (pScript)
    pScript->Destroy();   /* Free it */
}



/*****************************************************************************/
/**
 @memo Call a native C function from Javascript

 @doc This function allows a native C function to be called by javascript.
		This function must have this signature:
		JSBool someName(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
		When the function executes, cx will be the JSContext that is associated with the
		executing script.  obj is the global object that can access all other objects
		in running scripts.  argc is the number of arguments passed into the function.
		argv is an array of jsvalues that contain the parameters passed into the function.
		rval should be set by the user and will be the return value.  The function should
		return JS_TRUE.

 @param doc any created document
 @param name the name that the javascript interpretor will recognize.
 @param func the native function that will be called.
 @param argc parameter count
*/
/*****************************************************************************/

void webc_JScriptDefineFunction(HDOC_HANDLE doc, const char *name, JSNative func, int argc)
{
	HDOC_OBJ(doc)->DefineJSFunction(func, argc, name);
}


/*****************************************************************************/
/**
 @memo Get the HTMLDocument handle from a JSFunction

 @doc  This function should be used to get the HDOC_HANDLE associated with the javascript
       context that was running.  Use this handle to access the rest of the API's functionality.

 @param cx The javascript context that was running
 @return a handle to the document
 */
/*****************************************************************************/

HDOC_HANDLE webc_JScriptGetDoc(struct JSContext *cx)
{
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	if (jsmgr)
	{
		return (HBROWSER_HANDLE)(jsmgr->GetDocument());
	}
	return 0;
}

#endif // WEBC_SUPPORT_JSCRIPT


#if (WEBC_SUPPORT_COOKIES)


/*****************************************************************************/
/**
 @memo Create a cookie with the specified parameters

 @doc Given a cookie name, its value, host name and path, this function
 	  will return a handle to the newly created cookie.

 @param name The name of the cookie
 @param value Value the cookie will store
 @param host Domain the cookie was received from
 @param path Path of the cookie
 @return COOKIE_HANDLE: Handle to the new cookie
 @see webc_DestroyCookie
*/
/*****************************************************************************/

COOKIE_HANDLE webc_CreateCookie(char *name, char *value, char *host, char *path)
{
    return (COOKIE_HANDLE)webc_create_cookie(name, value, host, path, 0);

}

/*****************************************************************************/
/**
 @memo Destroys a cookie previously created with webc_CreateCookie

 @doc This function removes a cookie that was instantiated with webc_CreateCookie

 @param name The name of the cookie
 @param value Value the cookie will store
 @param host Domain the cookie was received from
 @param path Path of the cookie
 @return 0 if the cookie was successfully destroyed, -1 otherwise
 @see webc_CreateCookie
*/
/*****************************************************************************/

int webc_DestroyCookie(COOKIE_HANDLE cookie)
{

    return webc_delete_cookie((CookieInfo*)cookie);

}


/*****************************************************************************/
/**
 @memo Find a cookie that has the specified attributes

 @doc Find a cookie that has the specified attributes.  If more than one cookie matches,
      then the NUMth cookie will be returned.

 @param name The name of the cookie
 @param value Value the cookie stores
 @param host Domain the cookie was received from
 @param path Path of the cookie
 @param num Sequence number of the cookie (beginning from 0)
 @return 0 if the cookie was not found, or a handle to the cookie otherwise
 */
/*****************************************************************************/

COOKIE_HANDLE webc_FindCookie(const char *name, const char *host, const char *path, int num)
{
    return (COOKIE_HANDLE) webc_find_cookie(name, host, path, num);
}


/*****************************************************************************/
/**
 @memo Retrieve the name of a cookie

 @doc Retrieve the name of a cookie

 @param cookie Handle of the cookie
 @return The name of the cookie
 @see webc_CookieGetValue, webc_CookieGetHost, webc_CookieGetPath
 */
/*****************************************************************************/

const char *webc_CookieGetName(COOKIE_HANDLE cookie)
{
    return webc_cookie_get_name((CookieInfo*)cookie);
}


/*****************************************************************************/
/**
 @memo Retrieve a cookie's value

 @doc Retrieve a cookie's value

 @param cookie Handle of the cookie
 @return The cookie's value
 @see webc_CookieGetName, webc_CookieGetHost, webc_CookieGetPath
 */
/*****************************************************************************/

const char *webc_CookieGetValue(COOKIE_HANDLE cookie)
{
    return webc_cookie_get_value((CookieInfo*)cookie);
}


/*****************************************************************************/
/**
 @memo Get the host that a cookie was received from

 @doc Retrieve a cookie's value

 @param cookie Handle of the cookie
 @return The cookie's host
 @see webc_CookieGetName, webc_CookieGetValue, webc_CookieGetPath
 */
/*****************************************************************************/

const char *webc_CookieGetHost(COOKIE_HANDLE cookie)
{
    return webc_cookie_get_host((CookieInfo*)cookie);
}


/*****************************************************************************/
/**
 @memo Get the path of a cookie

 @doc Get the path of a cookie

 @param cookie Handle of the cookie
 @return The cookie's path
 @see webc_CookieGetName, webc_CookieGetValue, webc_CookieGetHost
 */
/*****************************************************************************/

const char *webc_CookieGetPath(COOKIE_HANDLE cookie)
{
    return webc_cookie_get_path((CookieInfo*)cookie);
}


/*****************************************************************************/
/**
 @memo Set the name of a cookie

 @doc Set the name of a cookie

 @param cookie Handle of the cookie
 @param name Pointer to the string containing the cookie's new name
 @see webc_CookieSetValue, webc_CookieSetHost, webc_CookieSetPath
 */
/*****************************************************************************/

void webc_CookieSetName(COOKIE_HANDLE cookie, const char *name)
{
    webc_cookie_set_name((CookieInfo*)cookie, name);
}


/*****************************************************************************/
/**
 @memo Set the value of a cookie

 @doc Set the value of a cookie

 @param cookie Handle of the cookie
 @param value Pointer to the string containing the cookie's new value
 @see webc_CookieSetName, webc_CookieSetHost, webc_CookieSetPath
 */
/*****************************************************************************/

void webc_CookieSetValue(COOKIE_HANDLE cookie, const char *value)
{
    webc_cookie_set_value((CookieInfo*)cookie, value);
}


/*****************************************************************************/
/**
 @memo Set a cookie's host

 @doc Set a cookie's host

 @param cookie Handle of the cookie
 @param host Pointer to the string containing the cookie's new host
 @see webc_CookieSetName, webc_CookieSetValue, webc_CookieSetPath
 */
/*****************************************************************************/

void webc_CookieSetHost(COOKIE_HANDLE cookie, const char *host)
{
    webc_cookie_set_host((CookieInfo*)cookie, host);
}


/*****************************************************************************/
/**
 @memo Set the path of a cookie

 @doc Set the path of a cookie

 @param cookie Handle of the cookie
 @param path Pointer to the string containing the cookie's new path
 @see webc_CookieSetName, webc_CookieSetValue, webc_CookieSetHost
 */
/*****************************************************************************/

void webc_CookieSetPath(COOKIE_HANDLE cookie, const char *path)
{
    webc_cookie_set_path((CookieInfo*)cookie, path);
}


#endif // (WEBC_SUPPORT_COOKIES)


/*****************************************************************************/
/**
 @memo Manually trigger an HTMLEvent

 @doc This method can be used to manually trigger event handling for a
      particular event on a specific element.

 @param element    Handle of the element to receive the event
 @param pEvent     Pointer to already-initialized \Ref{HTMLEvent} structure
 @see HTMLEvent, HTMLEventStatus, HTMLEventType
 */
/*****************************************************************************/

HTMLEventStatus webc_TriggerEvent(HELEMENT_HANDLE element, HTMLEvent *pEvent)
{
	HTMLEventStatus status = HTML_EVENT_STATUS_ERROR;

	if (element)
	{
		status = HELEMENT_OBJ(element)->Event(pEvent);
	}

	return (status);
}


void webc_FreeASCIIString(char *str)
{
	WEBC_FREE(str);
}

char* webc_MallocASCIIString(const WEBC_CHAR *str)
{
	size_t size = (webc_strlen(str) + 1) * sizeof(char);
	char *copy = (char *) WEBC_MALLOC(size);
	if (copy)
	{
		int n;
		for (n=0; str[n]; n++)
		{
			copy[n] = (char) (str[n] & 0xff);
		}
		copy[n] = 0;
	}
	return (copy);
}

#if (WEBC_SUPPORT_UNICODE)

void webc_FreeUCString(unsigned short *str)
{
	webc_free_string_copy(str, __FILE__, __LINE__);
}

unsigned short *webc_MallocUCString(const WEBC_CHAR *str)
{
	return (webc_malloc_string_copy(str, __FILE__, __LINE__));
}

#endif // WEBC_SUPPORT_UNICODE

WEBC_CHAR* webc_MallocStringFromInt (WEBC_INT32 i)
{
	WEBC_CHAR tempStr[16];
	webc_print_dec(tempStr, i, 0, 15);
	return webc_MallocString(tempStr);
}

/*****************************************************************************/
/**
 @memo Set Authorization for HTTP site

 @doc This method is used to set a username and password to access a protected
      HTTP server

 @param domain     pointer to a struct decribing the authentication domain
 @param userName   user name to set for this domain (string will be copied)
 @param password   password to set for this domain (string will be copied)
 @param ttlSec     time to live in seconds (0 for infinite) for user/password
 @see HTTPAuthDomain, webc_HttpClearAuthorization, webc_HttpSetAuthCallback,
      webc_HttpClearAllAuthorizations
 */
/*****************************************************************************/

int webc_HttpSetAuthorization (
		HTTPAuthDomain* domain,
		const char* userName,
		const char* password,
		long ttlSec
	)
{
  #if (WEBC_SUPPORT_HTTP)
	HTTPAuthenticationScheme scheme = HTTP_AUTHENTICATION_UNKNOWN;

	if (!tc_stricmp(domain->scheme, "Basic"))
	{
		scheme = HTTP_AUTHENTICATION_BASIC;
	}
	else if (!tc_stricmp(domain->scheme, "Digest"))
	{
		scheme = HTTP_AUTHENTICATION_DIGEST;
	}

	URLDescriptor urlDesc(domain->url);

	char* path = webc_arg_to_char(urlDesc.getPath());
	char* host = webc_arg_to_char(urlDesc.getHost());

	int result = http_set_authorization (
			host,
			urlDesc.getPort(),
			path,
			domain->realm,
			scheme,
			(char*) userName,
			(char*) password,
			ttlSec
		);

	webc_arg_done(host);
	webc_arg_done(path);

	return result;
  #else
	return (-1);
  #endif // WEBC_SUPPORT_HTTP
}


/*****************************************************************************/
/**
 @memo Discard username/password for an HTTP host

 @doc This method is used to discard the current username and password for a
      certain HTTP authentication domain

 @param domain     pointer to a struct decribing the authentication domain
 @see HTTPAuthDomain, webc_HttpSetAuthorization, webc_HttpSetAuthCallback,
      webc_HttpClearAllAuthorizations
 */
/*****************************************************************************/

void webc_HttpClearAuthorization (HTTPAuthDomain *domain)
{
  #if (WEBC_SUPPORT_HTTP)
	//http_reset_authorization_realm ((char *) domain->host, domain->port,
	 //(char *) domain->realm);
  #endif // WEBC_SUPPORT_HTTP
}


/*****************************************************************************/
/**
 @memo Discard all HTTP username/passwords

 @doc This method is used to discard all usernames and passwords on all HTTP
      authentication domains

 @see webc_HttpSetAuthorization, webc_HttpSetAuthCallback,
      webc_HttpClearAuthorization
 */
/*****************************************************************************/

void webc_HttpClearAllAuthorizations (void)
{
  #if (WEBC_SUPPORT_HTTP)
	http_reset_all_authorizations();
  #endif // WEBC_SUPPORT_HTTP
}

void webc_CreateKeyMap (
		WebcKeyMap* map,
		WEBC_UINT32* keyData,
		WEBC_SIZE numKeys
	)
{
	int n;

	(*map)[0] = (WEBC_UINT32) numKeys;

	for (n = 0; n < numKeys; n++)
	{
		(*map)[1 + n] = keyData[n*2+1];
		(*map)[1 + n + numKeys] = keyData[n*2];
	}
}

WebKey webc_TranslateKey (
		WebcKeyMap* map,
		WEBC_UINT32 rawKey
	)
{
	unsigned int n;

	for (n = 0; n < (*map)[0]; n++)
	{
		if ((*map)[1 + n] == rawKey)
		{
			return ((*map)[1 + n + (*map)[0]]);
		}
	}

	return (rawKey);
}

void webc_DestroyKeyMap (
		WebcKeyMap* map
	)
{
	// do nothing
}

void webc_ElementGetXYPosition(HELEMENT_HANDLE element, long *current_xpos, long *current_ypos)
{
CSSPropertyValue value;

	if ((HELEMENT_OBJ(element)->GetStyleFromCSS(CSS_PROPERTY_TOP, &value) >= 0) ||
		(HELEMENT_OBJ(element)->GetStyleFromHTML(CSS_PROPERTY_TOP, &value) >= 0))
	{
		*current_ypos = value.length.val.dec;
		if ((HELEMENT_OBJ(element)->GetStyleFromCSS(CSS_PROPERTY_LEFT, &value) >= 0) ||
			(HELEMENT_OBJ(element)->GetStyleFromHTML(CSS_PROPERTY_LEFT, &value) >= 0))
		{
			*current_xpos = value.length.val.dec;
//			return (WEBC_TRUE);
		}
	}
//	return (WEBC_FALSE);
}

void webc_ElementSetXYPosition(HELEMENT_HANDLE element, long current_xpos, long current_ypos, WEBC_BOOL refresh)
{

    HELEMENT_OBJ(element)->SetXYPosition(current_xpos, current_ypos);

    if (HELEMENT_OBJ(element)->GetDisplayElement())
    {
	DisplayElement *pdispelement;
	DisplayElement *parent;
        pdispelement = HELEMENT_OBJ(element)->GetDisplayElement();
        /* Invalidate the current locations covered by the element */
        pdispelement->SetFlag(DISPLAY_FLAG_DIRTY);
        pdispelement->Invalidate();
        parent = pdispelement;
        while (parent->mpParent)
        {
		    parent = parent->mpParent;
        }
        if (parent)
            parent->InsertLast(pdispelement);
        pdispelement->MoveTo(current_xpos,current_ypos);
    }
    if (refresh)
    {
		HELEMENT_OBJ(element)->Update(0,(WEBC_PFBYTE)1);
		HELEMENT_OBJ(element)->Refresh();
    }
}

int webc_ElementMoveAbs(HELEMENT_HANDLE element, long x, long y)
{
    webc_ElementSetXYPosition(element, x, y, WEBC_TRUE);
    return(0);
}

int webc_ElementMove(HELEMENT_HANDLE element, long deltax, long deltay)
{
    long current_xpos, current_ypos;
    webc_ElementGetXYPosition(element, &current_xpos, &current_ypos);
    current_xpos += deltax;
    current_ypos += deltay;
    webc_ElementSetXYPosition(element, current_xpos, current_ypos,WEBC_TRUE);
    return(0);
}

static void _webc_ElementClaimPointer(HELEMENT_HANDLE hElement, WEBC_BOOL doclaim)
{
    HTMLElement    *phtmlelement;
    DisplayElement *pdispelement;
    HTMLDocument *pDoc;
	HTMLBrowser  *pBrowser;


    phtmlelement = HELEMENT_OBJ(hElement);
    if (phtmlelement)
        pdispelement = phtmlelement->GetDisplayElement();
    else
        pdispelement = 0;
    if (!pdispelement)
        return;

	if (!pdispelement->GetManager())
        return;


	if (doclaim)
	    pdispelement->GetManager()->ClaimPointer(pdispelement);
    else
	    pdispelement->GetManager()->ReleasePointer(pdispelement);
    return;

	pDoc = (phtmlelement)? phtmlelement->GetDocument() : 0;
	pBrowser = (pDoc)? pDoc->GetBrowser() : 0;
    if (pBrowser)
    {
	    if (doclaim)
            pBrowser->GetDisplayManager()->ClaimPointer(pdispelement);
        else
            pBrowser->GetDisplayManager()->ReleasePointer(pdispelement);
    }
}

void webc_ElementClaimPointer(HELEMENT_HANDLE hElement)
{
    _webc_ElementClaimPointer(hElement, WEBC_TRUE);
}
void webc_ElementReleasePointer(HELEMENT_HANDLE hElement)
{
    _webc_ElementClaimPointer(hElement, WEBC_FALSE);
}


/*****************************************************************************/
/**
 @memo Get an HTML element attribute

 @doc

 @see webc_ElementSetAttribute
 */
/*****************************************************************************/

/* not tested and what about refresh */

WEBC_CHAR* webc_ElementGetAttribute (
		HELEMENT_HANDLE element,
		WEBC_CHAR* attrName
	)
{
	WEBC_CHAR* attrValueCopy = 0;
	HTMLAttributeType attrType = HTML_ParseAttributeType(attrName, 0);
	if (attrType > HTML_UNKNOWN_ATTRIB)
	{
		switch (attrType)
		{
			case HTML_ATTRIB_ABBR:
			case HTML_ATTRIB_ACCEPT_CHARSET:
			case HTML_ATTRIB_ACCEPT:
			case HTML_ATTRIB_ACCESSKEY:
			case HTML_ATTRIB_ACTION:
			case HTML_ATTRIB_ALIGN:
			case HTML_ATTRIB_ALINK:
			case HTML_ATTRIB_ALT:
			case HTML_ATTRIB_ARCHIVE:
			case HTML_ATTRIB_AXIS:
			case HTML_ATTRIB_BACKGROUND:
			case HTML_ATTRIB_BGCOLOR:
			case HTML_ATTRIB_BORDER:
			case HTML_ATTRIB_CELLPADDING:
			case HTML_ATTRIB_CELLSPACING:
			case HTML_ATTRIB_CHAR:
			case HTML_ATTRIB_CHAROFF:
			case HTML_ATTRIB_CHARSET:
			case HTML_ATTRIB_CHECKED:
			case HTML_ATTRIB_CITE:
			case HTML_ATTRIB_CLASS:
			case HTML_ATTRIB_CLASSID:
			case HTML_ATTRIB_CLEAR:
			case HTML_ATTRIB_CODE:
			case HTML_ATTRIB_CODEBASE:
			case HTML_ATTRIB_CODETYPE:
			case HTML_ATTRIB_COLOR:
			case HTML_ATTRIB_COLS:
			case HTML_ATTRIB_COLSPAN:
			case HTML_ATTRIB_COMPACT:
			case HTML_ATTRIB_CONTENT:
			case HTML_ATTRIB_COORDS:
			case HTML_ATTRIB_DATA:
			case HTML_ATTRIB_DATETIME:
			case HTML_ATTRIB_DECLARE:
			case HTML_ATTRIB_DEFER:
			case HTML_ATTRIB_DIR:
			case HTML_ATTRIB_DISABLED:
			case HTML_ATTRIB_ENCTYPE:
			case HTML_ATTRIB_EVENTHANDLER:
			case HTML_ATTRIB_FACE:
			case HTML_ATTRIB_FOR:
			case HTML_ATTRIB_FRAME:
			case HTML_ATTRIB_FRAMEBORDER:
			case HTML_ATTRIB_HEADERS:
			case HTML_ATTRIB_HEIGHT:
			case HTML_ATTRIB_HREF:
			case HTML_ATTRIB_HREFLANG:
			case HTML_ATTRIB_HSPACE:
			case HTML_ATTRIB_HTTP_EQUIV:
			case HTML_ATTRIB_ID:
			case HTML_ATTRIB_ISMAP:
			case HTML_ATTRIB_LABEL:
			case HTML_ATTRIB_LANG:
			case HTML_ATTRIB_LANGUAGE:
			case HTML_ATTRIB_LINK:
			case HTML_ATTRIB_LONGDESC:
			case HTML_ATTRIB_MARGINHEIGHT:
			case HTML_ATTRIB_MARGINWIDTH:
				break;

			case HTML_ATTRIB_MAXLENGTH:
				if (HELEMENT_OBJ(element)->MaxLength() >= 0)
				{
					attrValueCopy = webc_MallocStringFromInt(HELEMENT_OBJ(element)->MaxLength());
				}
				else
				{
					attrValueCopy = webc_MallocString((WEBC_CHAR*) _WEBC_STR(""));
				}
				break;

			case HTML_ATTRIB_MEDIA:
			case HTML_ATTRIB_METHOD:
			case HTML_ATTRIB_MULTIPLE:
			case HTML_ATTRIB_NAME:
			case HTML_ATTRIB_NOBACKGROUND:
			case HTML_ATTRIB_NOHREF:
			case HTML_ATTRIB_NORESIZE:
			case HTML_ATTRIB_NOSHADE:
			case HTML_ATTRIB_NOTAB:
			case HTML_ATTRIB_NOWRAP:
			case HTML_ATTRIB_OBJECT:
			case HTML_ATTRIB_ONBLUR:
			case HTML_ATTRIB_ONCHANGE:
			case HTML_ATTRIB_ONCLICK:
			case HTML_ATTRIB_ONDBLCLICK:
			case HTML_ATTRIB_ONFOCUS:
			case HTML_ATTRIB_ONKEYDOWN:
			case HTML_ATTRIB_ONKEYPRESS:
			case HTML_ATTRIB_ONKEYUP:
			case HTML_ATTRIB_ONLOAD:
			case HTML_ATTRIB_ONLOSECAPTURE:
			case HTML_ATTRIB_ONMOUSEDOWN:
			case HTML_ATTRIB_ONMOUSEMOVE:
			case HTML_ATTRIB_ONMOUSEOUT:
			case HTML_ATTRIB_ONMOUSEOVER:
			case HTML_ATTRIB_ONMOUSEUP:
			case HTML_ATTRIB_ONRESET:
			case HTML_ATTRIB_ONSELECT:
			case HTML_ATTRIB_ONSUBMIT:
			case HTML_ATTRIB_ONUNLOAD:
			case HTML_ATTRIB_PROFILE:
			case HTML_ATTRIB_PROMPT:
			case HTML_ATTRIB_QUARANTINE:
				break;

			case HTML_ATTRIB_READONLY:
				if (HELEMENT_OBJ(element)->ReadOnly() == WEBC_TRUE)
				{
					attrValueCopy = webc_MallocString((WEBC_CHAR*)_WEBC_STR("true"));
				}
				else
				{
					attrValueCopy = webc_MallocString((WEBC_CHAR*)_WEBC_STR("false"));
				}
				break;

			case HTML_ATTRIB_REL:
			case HTML_ATTRIB_REV:
			case HTML_ATTRIB_ROWS:
			case HTML_ATTRIB_ROWSPAN:
			case HTML_ATTRIB_RULES:
			case HTML_ATTRIB_SCHEME:
			case HTML_ATTRIB_SCOPE:
			case HTML_ATTRIB_SCROLLING:
			case HTML_ATTRIB_SELECTED:
			case HTML_ATTRIB_SHAPE:
			case HTML_ATTRIB_SIZE:
			case HTML_ATTRIB_SPAN:
			case HTML_ATTRIB_SRC:
			case HTML_ATTRIB_STANDBY:
			case HTML_ATTRIB_START:
			case HTML_ATTRIB_STYLE:
			case HTML_ATTRIB_SUMMARY:
			case HTML_ATTRIB_TABINDEX:
			case HTML_ATTRIB_TARGET:
			case HTML_ATTRIB_TEXT:
			case HTML_ATTRIB_TITLE:
			case HTML_ATTRIB_TYPE:
			case HTML_ATTRIB_USEMAP:
			case HTML_ATTRIB_VALIGN:
			case HTML_ATTRIB_VALUE:
			case HTML_ATTRIB_VALUETYPE:
			case HTML_ATTRIB_VERSION:
			case HTML_ATTRIB_VLINK:
			case HTML_ATTRIB_VSPACE:
			case HTML_ATTRIB_WIDTH:
			case HTML_ATTRIB_WRAP:
				break;
		}
	}

	return attrValueCopy;
}

/*****************************************************************************/
/**
 @memo Set an HTML element attribute

 @doc

 @see webc_ElementGetAttribute
 */
/*****************************************************************************/
WEBC_BOOL webc_ElementSetAttributeASCII (
		HELEMENT_HANDLE element,
		char * _attrName,
		char * _attrValue
	)
{
  #if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR attrName[HTML_TEMP_STR_LEN];
	WEBC_CHAR attrValue[HTML_TEMP_STR_LEN];
	webc_c_strncpy(attrName, _attrName, HTML_TEMP_STR_LEN-1);
	webc_c_strncpy(attrValue, _attrValue, HTML_TEMP_STR_LEN-1);
	attrName[HTML_TEMP_STR_LEN-1] = 0;
	attrValue[HTML_TEMP_STR_LEN-1] = 0;
  #else
	  	const WEBC_CHAR* attrName = _attrName;
	  	const WEBC_CHAR*attrValue = _attrValue;
  #endif
	WEBC_BOOL refresh = WEBC_TRUE;
	HTMLAttributeType attrType = HTML_ParseAttributeType((HTML_CHAR *)attrName, 0);
	if (attrType > HTML_UNKNOWN_ATTRIB)
	{
		HELEMENT_OBJ(element)->SetProperty(attrType, (WEBC_CHAR *) attrValue);
		if (refresh)
		{
		    HELEMENT_OBJ(element)->Update(0,(WEBC_PFBYTE)1);
		    HELEMENT_OBJ(element)->Refresh();
		}
		return WEBC_TRUE;
	}

	return WEBC_FALSE;
}



/*****************************************************************************/
// webc_FindFocusElement
/**

 @memo			Search for the element with focus.
 @doc			Does a depth-first search of the document tree of the document with focus for the element
                having focus.

 @return		The handle of the element with focus. WEBC_NULL if none found.
 @see           HELEMENT_HANDLE, webc_GetBrowserWithFocus()
 */
/*****************************************************************************/
#if (WEBC_INCLUDE_FOCUS_FINDER)

HELEMENT_HANDLE webc_FindFocusElement(HBROWSER_HANDLE hBrowser)
{
	if (!hBrowser)
		hBrowser = webc_GetBrowserWithFocus();
	if (hBrowser)
	{
		HDOC_HANDLE doc;
		doc = webc_BrowserGetDocument(hBrowser);
		if (doc)
		{
			HTMLFocusElementFinder f;
			HTMLElement *pElement;

			pElement = (HDOC_OBJ(doc)->FindElement(&f));

			return (HELEMENT_HANDLE) pElement;
		}
	}
	return (HELEMENT_HANDLE) (0);
}
#endif /* (WEBC_INCLUDE_FOCUS_FINDER) */


#if (WEBC_SUPPORT_POSTFILE)
int webc_post_UploadFileASCII(HBROWSER_HANDLE postBrowser, char *SourceFileName,  char *pUrl, char* DestApplication, char *DestFileName)
{
	//WEBC_CHAR *query;
	long queryLen;
    URLDescriptor desc;
	HDOC_HANDLE pDoc;
	int bDocDestroyed;
	char postActionASCII[1024];
#if (WEBC_SUPPORT_UNICODE)
	WEBC_CHAR postActionUnicode[1024];
#endif
	rtp_strcpy(postActionASCII,DestApplication);
	rtp_strcat(postActionASCII,"?");
	rtp_strcat(postActionASCII,DestFileName);
	queryLen = rtp_strlen(postActionASCII);

	pDoc = webc_BrowserGetDocument(postBrowser);

	HTMLDocument *pMyDoc      =  (HTMLDocument *)pDoc;
	HTMLDocument *pTargDoc    =  pMyDoc;
	HTMLBrowser *pTargBrowser =  0;

	if(!pMyDoc)
	{
		return(-1);
	}

	bDocDestroyed = (pMyDoc == pTargDoc)? 1:0;

#if (WEBC_SUPPORT_UNICODE)
		webc_c_strcpy(postActionUnicode,&postActionASCII[0]);
		desc.parseURL(&postActionUnicode[0], pMyDoc->BaseURL());

#else
		desc.parseURL(&postActionASCII[0], pMyDoc->BaseURL());
#endif
		pTargBrowser = pTargDoc->GetBrowser();
		//pTargBrowser = (HTMLBrowser *) postBrowser;   //tested worked?? why the overhead??
		if (pTargBrowser)
			{
				WEBC_CHAR temp[48];
				LoadJob *pJob;

				// don't save the current url to the history if protocol is JavaScript,
				//  because javascript:// url's are not really page locations
				if (desc.getProtocol() != WEBC_PROTOCOL_JSCRIPT)
				{
					pTargBrowser->Stop(pTargDoc);
					pTargDoc->SaveLocation();
				}

				webc_c_strcpy(temp, "application/x-www-form-urlencoded");
				WEBC_NEW(pJob, PostUploadRawFile(pTargDoc, &desc, temp, &postActionASCII[0], queryLen, SourceFileName, WEBC_CHARSET_UNKNOWN));

				if (pJob)
				{
					pTargBrowser->ProcessLoadJob(pJob, &desc, pMyDoc->BaseURL(), pTargDoc);
				  #ifdef WEBC_OLD_LOAD_METHOD
					webc_thread_load_browser(pTargBrowser);
				  #endif // WEBC_OLD_LOAD_METHOD
				}
				else
				{
					bDocDestroyed = 0;
				}
				return(bDocDestroyed);
			}
	return 0;
}
#endif /* (WEBC_SUPPORT_POSTFILE) */


/*****************************************************************************/
/**
 @memo Set an HTML element's inner html

 	void webc_ElementSetInnerHtmlASCII ( HELEMENT_HANDLE element,	char * newhtml)
 @doc

 */
/*****************************************************************************/
void webc_ElementSetInnerHtmlASCII ( HELEMENT_HANDLE element,	char * newhtml)
{
  #if (WEBC_SUPPORT_UNICODE)
   	WEBC_CHAR *pustring;
	pustring = (WEBC_CHAR *) WEBC_MALLOC((rtp_strlen(newhtml)+1)*2);
	if (pustring)
	{
		webc_c_strcpy(pustring, newhtml);
		HELEMENT_OBJ(element)->SetInnerHTML(pustring);
		WEBC_FREE(pustring);
	}
  #else
	HELEMENT_OBJ(element)->SetInnerHTML(newhtml);
  #endif
}



/*****************************************************************************/
/**
 @memo Set an HTML element's inner html

 	char *webc_ElementGetInnerHtmlASCII ( HELEMENT_HANDLE element)
 @doc

 */
/*****************************************************************************/
char * webc_ElementGetInnerHtmlASCII ( HELEMENT_HANDLE element )
{
	char *retVal = 0;
#if (WEBC_SUPPORT_INNER_HTML)
	 WebString tempString;
	((HTMLElement*)(element))->AppendInnerHTML(&tempString);
	//retVal = (char *)WEBC_MALLOC((tempString.getLength() + 1) * sizeof(char)); // caller must free
    return webc_MallocASCIIString(tempString.getChars());
#endif // WEBC_SUPPORT_INNER_HTML

	return retVal;
}
