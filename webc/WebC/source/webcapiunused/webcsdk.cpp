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
#include "rtpfile.h"
#include "webcdefault.h"
#include "webvfile.hpp"
#ifdef WEBC_DEBUG
#include "webcLog.h"
#endif
#if (WEBC_DEBUG_JSCRIPT)
#include "wjsdebug.h"
#endif


/*****************************************************************************/
// Local types
/*****************************************************************************/
char nphoney[1];
#include "webcsdk.hpp"


/*****************************************************************************/
// Global Data
/*****************************************************************************/

int giWebcInitNesting;

#if (WEBC_SUPPORT_JSCRIPT)
WebcJSBrowserContext* gJSBrowserContext;
#endif


struct vector gBrowserWindowVector;

/*****************************************************************************/
// External Declarations
/*****************************************************************************/

extern HTMLEventQueue*            gpEventQueue;
extern TUOpenNoBr*                gpOpenNoBr;
extern TUCloseNoBr*               gpCloseNoBr;


/*****************************************************************************/
// Macros
/*****************************************************************************/

/*****************************************************************************/
// HTML C API Functions
/*****************************************************************************/

#if (WEBC_SUPPORT_ERROR_PAGE_REDIRECT)
extern "C" {
	void webc_ClearErrorRedirects(void);
}
#endif
/*****************************************************************************/
// webc_Init
/**

 @memo  Initialize webC. This function must be called first before any other webC API calls are made.
 @doc   This function initializes webC. It must be called before any other API calls are made.

 @return 0 if successful, -1 otherwise
 @see    example projects, webc_Exit
 */
/*****************************************************************************/

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
		// WEBC_NEW_VERBOSE(gpEventQueue, HTMLEventQueue,"gpEventQueue");

		//-----------------------------------------------------------
		// initialize misc objects used in page formatting
		//-----------------------------------------------------------

//		WEBC_NEW_VERBOSE(gpOpenNoBr, TUOpenNoBr,"gpOpenNoBr");
//		WEBC_NEW_VERBOSE(gpCloseNoBr, TUCloseNoBr,"gpCloseNoBr");

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

	  // Initialize the virtual file system so webc: and rom: urls can be accessed
	  wload_WebcVirtualFileSystemInitialize();

	  #if(WEBC_SUPPORT_ERROR_PAGE_REDIRECT)
	  /* Initialize arrary used to redirect page and server not found events to an application specific url */
	  webc_ClearErrorRedirects();
	  #endif

	}
	return (0);
}





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

#ifdef RTP_TRACK_LOCAL_MEMORY
char*           _webc_MallocASCIIString (
                                     const WEBC_CHAR* str,
	                                 const char* file,
									 const long line
                                 )
#else
char* webc_MallocASCIIString(const WEBC_CHAR *str)
#endif
{
	size_t size = (webc_strlen(str) + 1) * sizeof(char);
#ifdef RTP_TRACK_LOCAL_MEMORY
	char *copy  = (char *) WEBC_DEBUG_MALLOC(size,file,line,"AsciStringFromUnicode",0);
#else
	char *copy = (char *) WEBC_MALLOC(size);
#endif
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


HBROWSER_HANDLE webc_GetBrowserWithFocus()
{
	return((HBROWSER_HANDLE)gpFocusBrowser);
}

/*****************************************************************************/
// webc_clear_http_cache
/**

 @memo   Release all cached pages and images.
 @doc    An HTMLBrowser may allocate certain resources that are not released
         as soon as the browser is destroyed. This function is different
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
