#ifndef __WEBCSDK_INCLUDED__
#define __WEBCSDK_INCLUDED__
//
// HTMLSDK.H -
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

extern char nphoney[1];
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

// Vector for keeping track of windows
struct WindowInfo
{
	HBROWSER_HANDLE Browser;
};
extern struct vector gBrowserWindowVector;
extern int giWebcInitNesting;
#if (WEBC_SUPPORT_JSCRIPT)
extern "C" WebcJSBrowserContext* gJSBrowserContext;
extern WebcJSBrowserContext* gJSBrowserContext;
#if (WEBC_SUPPORT_USER_EVENTS)
extern CFunctionHandler *gpCHandlerTable[WEBC_CFG_MAX_EVENT_HANDLERS];
#endif // WEBC_SUPPORT_USER_EVENTS
#endif
#endif //____WEBCSDK_INCLUDED__
