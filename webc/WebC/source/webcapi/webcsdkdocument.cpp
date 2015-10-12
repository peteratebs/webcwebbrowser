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
#if (0)
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
#include "webcsdk.hpp"


/*****************************************************************************/
// Global Data
/*****************************************************************************/

/*****************************************************************************/
// External Declarations
/*****************************************************************************/

/*****************************************************************************/
// Macros
/*****************************************************************************/

/*****************************************************************************/
// HTML C API Functions
/*****************************************************************************/

// HEREHERE Begin webcsdkdocument.cpp

/*****************************************************************************/
// HTML Document API Functions
/*****************************************************************************/

/*****************************************************************************/
// webc_DocFindElement
/*X Deprecated from published API

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

// Deprecated - please remove
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
	HDOC_OBJ(doc)->Update(1,0);
	if (refresh)
	{
		WEBC_BROWSER_LOAD_ALL(HDOC_OBJ(doc)->GetBrowser());
	}
}



/*****************************************************************************/
// webc_DocGetBrowser
/* X Deprecated from published API

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
// webc_DocSaveForOffline
/*X Deprecated from published API

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
// HEREHERE End webcsdkdocument.cpp
#endif