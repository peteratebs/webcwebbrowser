//
// wcmetatagapi.cpp -
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
/**
 	@memo
<pre>
 	Metatags provide a means give web app developers the ability to call native 'C' handler from web pages.
</pre>
 	@memo
<pre>
<ul>
<li> Native 'C' functions are included in the target build. </li>
<li> These functions are then registered by the target code to be called when a named Metatag is encountered in an HTML header.</li>
<li> The method is simple and managable and has been used several times to pubish device control APIs accesible to back office developers.
<li> Content developers can then call these pages by linking to content containing the proper metatatags.</li>
<li> The contents of the metatag EQUIV property are passed to the 'C' function allowing content developers can pass parameters to the callback functions.</li>
<li> Simple HTML url based menu or button arrays may be used to link pages that invoke callbacks with specific device side resposibilities.</li>
<li> Examples:
<ul>
<li> Initiate a barcode scan operation which is then posted back to the server.</li>
<li> Enable, disable or change a device accessory seeting (lower volume, dim backlight, etc.).</li>
<li> Allow access to device functions after succesful password validation.</li>
<li> Based on device specific paramaters, allow or cancel loading of pages with security metatags in their headers.</li>
<li> Push update instructions to a device on a page load.</li>
<li> Allow "out of band" call operations to a device by injecting metatags into normal application pages as they are loaded.</li>
</ul>
 */
struct Introduction{ int xx; }; /* Fooling doc++ to display "Introduction" */

/*****************************************************************************/
// Header files
/*****************************************************************************/

//#include "../include/NewApi.h"
#include "wcapi.h"

#include "webc.h"
#include "webcstr.h"
#include "html.h"
#include "htmldom.hpp"
#include "htmldoc.hpp"
#include "css.h"
#include "webfonts.hpp"
#include "webload.hpp"
#include "wgkeys.hpp"
#include "htmlbrow.hpp"
#include "hedit.hpp"
#include "strbld.hpp"
#include "htmlfind.hpp"
#include "wcconst.hpp"
#include "hhidden.hpp"
#include "hpasswd.hpp"
#include "hchkbox.hpp"
#include "webcmem.h"

#include "webcassert.h"
#include "rtpprint.h"


/*****************************************************************************/
// Type definitions
/*****************************************************************************/

struct MetaCallbackEntry
{
	char pMetaName[WEBC_CFG_MAX_META_FUNCTION_NAMELEN];
	HTMLMetaCallbackFn Callback;
} ;

#if (WEBC_CFG_META_TABLE_SIZE)
MetaCallbackEntry gMetaCallbackTable[WEBC_CFG_META_TABLE_SIZE] =
{
	{{0}, 0}
};
#endif // WEBC_CFG_META_TABLE_SIZE


/*****************************************************************************/
// wcRegisterMetaCallback
/**

 @memo   Register a user supplied Metatag handler.
 @doc    Register a 'C' callback routine to be called when the named metatag is encountered in an html header.


 Metatags callbacks are an easy way to provide
 		 users with access to 'C' native functions from their callable from
 		 Load optional initial content and sets a user supplied handler function to be called when the view is created.
		 The Window config structure can be used to set the border color, background color, and border style.
		 The Window config structure can be used to control window properties including if it is modal and if it is resizable.


 @param  	const char *pMetaName 			Name of the metatag to process when encountered in an html header.
 @param  	HTMLMetaCallbackFn Callback		Function to call when the metatag is encounetered.
<pre>

The callback function must be of the form:

   HTMLMetaStatus *CallbackFn(const char* pContent, wcCtx *pCtx)

       where:
       &nbsp;pContent is the value from the content= field in the document.
       &nbsp;pCtxt is an initialize webC application context.

	   The callback must return one of these values:
	   &nbsp;&nbsp;  HTML_META_STATUS_CONTINUE - Instructs webC to continue processing the page including processing any additional metatags
	   &nbsp;&nbsp;  in the header or to continue to the body.
	   &nbsp;&nbsp;  HTML_META_STATUS_STOP - Behaves the same as HTML_META_STATUS_CONTINUE.
	   &nbsp;&nbsp;  HTML_META_STATUS_HALT - Stops loading the page immediately.

</pre>
 <pre>
Example:
</pre>


 @return rwcRegisterMetaCallback returns 0 if it was able to register the calback, -1 otherwise.

 */

int wcRegisterMetaCallback(const char *pMetaName, HTMLMetaCallbackFn Callback)
{
#if (WEBC_CFG_META_TABLE_SIZE)
int i;

	for (i=0; i<WEBC_CFG_META_TABLE_SIZE; i++)
	{
		if (gMetaCallbackTable[i].Callback == 0)
		{
			tc_strncpy(gMetaCallbackTable[i].pMetaName, pMetaName, WEBC_CFG_MAX_META_FUNCTION_NAMELEN-1);
			gMetaCallbackTable[i].pMetaName[WEBC_CFG_MAX_META_FUNCTION_NAMELEN-1] = 0;
			gMetaCallbackTable[i].Callback = Callback;
			return (0);
		}
	}
#endif
  	return (-1);
}

//HTMLMetaStatus HTML_ProcessMetaTag(const char *pName, const WEBC_CHAR *pContent, HTMLDocument *pDocument)
static HTMLMetaCallbackFn HTML_FindMetatagHandler(const WEBC_CHAR *pName, const WEBC_CHAR *pContent, HTMLDocument *pDocument);

HTMLMetaStatus HTML_ProcessMetaTag(const WEBC_CHAR *pName, const WEBC_CHAR *pContent, HTMLDocument *pDocument)
{
	HTMLMetaStatus status = HTML_META_STATUS_CONTINUE;
#if (WEBC_CFG_META_TABLE_SIZE)
int i;
wcCtx Ctx;


	Ctx.hdoc = (wcDOC) pDocument;
	Ctx.hbrowser = (wcBROW) pDocument->GetBrowser();


	for (i=0; i<WEBC_CFG_META_TABLE_SIZE; i++)
	{
		//if (gMetaCallbackTable[i].Callback && (gMetaCallbackTable[i].Hash_Code==h) &&!webc_c_strnicmp(pName, gMetaCallbackTable[i].pMetaName, WEBC_CFG_MAX_META_FUNCTION_NAMELEN))
		if (gMetaCallbackTable[i].Callback  &&!webc_c_strnicmp(pName, gMetaCallbackTable[i].pMetaName, WEBC_CFG_MAX_META_FUNCTION_NAMELEN))
		{
			WEBC_CHAR *pTemp = (WEBC_CHAR *) WEBC_MALLOC(sizeof(WEBC_CHAR) * (webc_strlen(pContent) + 1));
			if (pTemp)
			{
				HTML_UnescapeString(pContent, pTemp);
				char *pCopy = (char *) WEBC_MALLOC(sizeof(char) * (webc_strlen(pTemp) + 1));
				if (pCopy)
				{
					// copy from WEBC_CHAR to ASCII
					int n;
					for (n=0; pTemp[n]; n++)
					{
						pCopy[n] = (char) (pTemp[n] & 0x7f);
					}
					pCopy[n] = 0;
					status = gMetaCallbackTable[i].Callback(pCopy, &Ctx);
					wcCtxtRelease(&Ctx); 		// Garbage collect if needed
					WEBC_FREE(pCopy);
				}
				WEBC_FREE(pTemp);
			}
			break;
		}
	}
	return (status);

#endif
}


static HTMLMetaCallbackFn HTML_FindMetatagHandler(const WEBC_CHAR *pName, const WEBC_CHAR *pContent, HTMLDocument *pDocument)
{
#if (WEBC_CFG_META_TABLE_SIZE)
int i;

	for (i=0; i<WEBC_CFG_META_TABLE_SIZE; i++)
	{
		//if (gMetaCallbackTable[i].Callback && (gMetaCallbackTable[i].Hash_Code==h) &&!webc_c_strnicmp(pName, gMetaCallbackTable[i].pMetaName, WEBC_CFG_MAX_META_FUNCTION_NAMELEN))
		if (gMetaCallbackTable[i].Callback  &&!webc_c_strnicmp(pName, gMetaCallbackTable[i].pMetaName, WEBC_CFG_MAX_META_FUNCTION_NAMELEN))
		{
			return (gMetaCallbackTable[i].Callback);
		}
	}

#endif
	return (0);
}


