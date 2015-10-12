//
// WEBCSDKEXIT.CPP -
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
#if (WEBC_SUPPORT_HTTPS)
extern WEBC_BOOL       gWebcSSLInitialized;
extern RTP_SSL_CONTEXT gWebcSSLContext;
#endif // WEBC_SUPPORT_HTTPS


/*****************************************************************************/
// Local types
/*****************************************************************************/
#include "webcsdk.hpp"


/*****************************************************************************/
// Global Data
/*****************************************************************************/

/*****************************************************************************/
// Macros
/*****************************************************************************/

/*****************************************************************************/
// HTML C API Functions
/*****************************************************************************/


// HEREHERE Begin webcsdkexit.cpp


/*****************************************************************************/
// webc_Exit
/**

 @memo  Free resources used by the Web Client
 @doc   This function should be called when then application exits

 @return 0 if successful
 @see    webc_Init
 */
/*****************************************************************************/
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

#if (WEBC_SUPPORT_SOCKETS)
			webc_network_close();
#endif

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
void webc_Fakeexit(void)
{

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


// HEREHERE End webcsdkexit.cpp
