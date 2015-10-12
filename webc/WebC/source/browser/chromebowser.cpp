//
// WEBCDEMO.CPP - Browser Demo
//
// EBS - WebC
//
// Copyright EBS Inc. , 2004
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//

#include "webcjhconfig.h"

#include "chromebrowser.hpp"
#include "webc.h"
#include "webcstr.h"
#include "webcassert.h"
#if (WEBC_SUPPORT_JSCRIPT)
#include "jsapi.h"
#endif
#include "webcmem.h"
#include "wcapi.h"
#include <stdio.h>

#define WEBC_STRING_MAX 0xffff

// START PAGE FOR CHROME BROWSER
//const char* gpHomePage = "webc://welcome.html";


//const char* gpHomePage = "http://192.168.1.202/index.html";
//const char* gpHomePage = "file://\\htdocs\\index.html";
//const char* gpHomePage = "file://\\Test_page\\img_src.html";
//const char* gpHomePage = "file://\\tsd_new\\homescreen.html";
//const char* gpHomePage = "http://10.1.10.6/CGI_TEST2.html";
// const char* gpHomePage = "http://10.1.10.6/embed_test3.html";
/////(char *)"http://10.1.10.6:8080/tsd_new/home.html"
//const char* gpHomePage = "file://d:\\motopages\\bike_bmp\\Bike_GIMP_X1.bmp";
//const char* gpHomePage = "file://d:\\motopages\\bike_bmp\\test16.bmp";
//const char* gpHomePage = "http://10.1.10.6/wiki";
//const char* gpHomePage = "file://d:\\motopages\\bike_bmp\\Bike_GIMP_16_R5.bmp";
//Bike_GIMP_16_R5.bmp
//file://d:\motopages\\bike_bmp\\Bike_GIMP_X1.bmp
const char* gpHomePage = "http://www.ebsembeddedsoftware.com";
extern "C" int WebcDefaultWindowHeight = 768;
extern "C" int WebcDefaultWindowWidth =  1024;

HBROWSER_HANDLE webc_start_chrome_browser(HTMLBrowserConfig *pconfig, WEBC_BOOL CreateNewWindow)
{
HBROWSER_HANDLE Browser;
HDOC_HANDLE hdoc;

	pconfig->browserEventHandler = chromeBrowserEventHandler;
	pconfig->privateData = 0;

	/* 11/23/09 - Make the window larger and resizable. Had been using defaults. */
	pconfig->WindowConfig.top = 0;
    pconfig->WindowConfig.left = 0;
    pconfig->WindowConfig.height = WebcDefaultWindowHeight;
    pconfig->WindowConfig.width = WebcDefaultWindowWidth;
	pconfig->WindowConfig.is_resizable = true;
 //   pconfig->WindowConfig.height = 480;
 //   pconfig->WindowConfig.width = 640;
	pconfig->WindowConfig.WindowFlags |= WF_RESIZABLE;
	Browser = webc_CreateBrowser(pconfig, CreateNewWindow, "ChromeBrowser", "");
    webc_RegisterEventCallback ((HTMLEventCallback) chromeLoadHandler,  "chromeLoadHandler");
	hdoc = webc_BrowserGetDocument(Browser);

	#if (WEBC_SUPPORT_JSCRIPT)
	if (hdoc)
	{
		webc_JScriptDefineFunction(hdoc, "DebugLog", AppendFile, 2);
		webc_JScriptDefineFunction(hdoc, "getDefaultUrl", GetDefaultUrl, 0);
	}
	#endif // WEBC_SUPPORT_JSCRIPT

#if (WEBC_SUPPORT_SMALL_SCREEN)
    webc_BrowserLoadUrl(Browser, "webc://loadChrome_pda.html");
#else
	webc_BrowserLoadUrl(Browser, "webc://loadChrome.html");
#endif

	HELEMENT_HANDLE helem = webc_DocFindElement(hdoc, "statusBar", 0, HTML_ELEMENT_ANY, 0);
	if (helem)
	{
		WEBC_CHAR* value = webc_ElementGetAttribute(helem, (WEBC_CHAR *)_WEBC_STR("maxlength"));
		if (value)
		{
			webc_FreeString(value);
		}
	}
    return(Browser);
}

// ----------------------------------------------------------------------------
const char* gpUrlList = 0;//"urllist.txt";
int         giUrlTest = 0;

int GoogleUrlMacro(WEBC_CHAR *param, WEBC_CHAR *replace, int maxLen)
{
	webc_c_strncpy(replace, "http://www.google.com/", maxLen);
	return (EBSMIN(22, maxLen));
}


struct ChromeAuthInfo
{
	HDOC_HANDLE    hdoc;
	HTTPAuthDomain authDomain;
};

#if (WEBC_SUPPORT_JSCRIPT)
JSBool chromeSuspendDraw(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_NULL;

	HDOC_HANDLE hdoc = webc_JScriptGetDoc (cx);
	if (hdoc)
	{
		HBROWSER_HANDLE hbrowser = webc_DocGetBrowser(hdoc);
		if (hbrowser)
		{
			webc_BrowserSuspendDraw(hbrowser);
		}
	}

	return (JS_TRUE);
}

JSBool chromeResumeDraw(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_NULL;

	HDOC_HANDLE hdoc = webc_JScriptGetDoc (cx);
	if (hdoc)
	{
		HBROWSER_HANDLE hbrowser = webc_DocGetBrowser(hdoc);
		if (hbrowser)
		{
		    if (webc_BrowserGetDrawState (hbrowser) > 0)
			    webc_BrowserResumeDraw(hbrowser);
		}
	}

	return (JS_TRUE);
}

JSBool chromeInvalidate(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_NULL;

	HDOC_HANDLE hdoc = webc_JScriptGetDoc (cx);
	if (hdoc)
	{
		HBROWSER_HANDLE hbrowser = webc_DocGetBrowser(hdoc);
		if (hbrowser)
		{
			webc_BrowserInvalidate(hbrowser);
		}
	}

	return (JS_TRUE);
}

JSBool chromeDraw(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_NULL;

	HDOC_HANDLE hdoc = webc_JScriptGetDoc (cx);
	if (hdoc)
	{
		HBROWSER_HANDLE hbrowser = webc_DocGetBrowser(hdoc);
		if (hbrowser)
		{
			webc_BrowserDraw(hbrowser);
		}
	}

	return (JS_TRUE);
}

JSBool GetDefaultUrl(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = STRING_TO_JSVAL(JS_InternString(cx, gpHomePage));
	return (JS_TRUE);
}
#ifdef __IAR_SYSTEMS_ICC__
JSBool WriteFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	return (JS_TRUE);
}


JSBool ReadFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_NULL;
	return (JS_TRUE);
}

JSBool AppendFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
		return (JS_TRUE);
}
#else
JSBool WriteFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (argc > 1)
	{
		JSString* fileName = JS_ValueToString(cx, argv[0]);
		JS_AddRoot(cx, &fileName);

		JSString* data = JS_ValueToString(cx, argv[1]);
		JS_AddRoot(cx, &data);

		FILE *fp;
		fp = fopen(JS_GetStringBytes(fileName), "wb");
		if (fp)
		{
			fwrite(JS_GetStringChars(data), sizeof(jschar), JS_GetStringLength(data), fp);
			fclose(fp);
		}

		JS_RemoveRoot(cx, &fileName);
		JS_RemoveRoot(cx, &data);
	}

	return (JS_TRUE);
}


JSBool ReadFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_NULL;

	if (argc > 0)
	{
		JSString* fileName = JS_ValueToString(cx, argv[0]);
		JS_AddRoot(cx, &fileName);

		jschar* data = (jschar*) WEBC_MALLOC(sizeof(jschar) * (WEBC_STRING_MAX + 1));

		if (data)
		{
			FILE *fp;
			fp = fopen(JS_GetStringBytes(fileName), "rb");
			if (fp)
			{
				size_t numRead = fread(data, sizeof(jschar), WEBC_STRING_MAX, fp);
				if (numRead > 0)
				{
					data[numRead] = 0;
					*rval = STRING_TO_JSVAL(JS_InternUCString(cx, data));
				}
				fclose(fp);
			}

			WEBC_FREE(data);
		}

		JS_RemoveRoot(cx, &fileName);
	}

	return (JS_TRUE);
}

JSBool AppendFile(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (argc > 1)
	{
		JSString *fileName, *data;
		fileName = JS_ValueToString(cx, *argv);
		JS_AddRoot(cx, &fileName);
		argv++;

		data = JS_ValueToString(cx, *argv);
		JS_AddRoot(cx, &data);

		FILE *fp;
		fp = fopen(JS_GetStringBytes(fileName), "ab");
		fprintf(fp, "%s", JS_GetStringBytes(data));
		fclose(fp);

		JS_RemoveRoot(cx, &fileName);
		JS_RemoveRoot(cx, &data);
	}

	return (JS_TRUE);
}
#endif
JSBool SaveContentForOffline(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_NULL;

	HDOC_HANDLE hdoc = webc_JScriptGetDoc (cx);
	if (hdoc)
	{
		HBROWSER_HANDLE hbrowser = webc_DocGetBrowser(hdoc);
		if (hbrowser)
		{
			hdoc = webc_BrowserGetDocument(hbrowser);
			if (hdoc)
			{
				HELEMENT_HANDLE hframe = webc_DocFindElement(hdoc, "contentFrame", 0, HTML_ELEMENT_ANY, 0);
				if (hframe)
				{
					hdoc = webc_ElementGetFrameDocument(hframe);
					if (hdoc)
					{
						webc_DocSaveForOffline(hdoc);
					}
				}
			}
		}
	}

	return (JS_TRUE);
}

JSBool EnableOnlineStatusRestore(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HDOC_HANDLE hdoc = webc_JScriptGetDoc(cx);
	if (hdoc)
	{
		HBROWSER_HANDLE hbrowser = webc_DocGetBrowser(hdoc);
		if (hbrowser)
		{
			void* privateData = webc_BrowserGetPrivateData(hbrowser);
			WEBC_BOOL* onlineStatus = WEBC_NULL;

			if (!privateData)
			{
				privateData = WEBC_MALLOC(sizeof(WEBC_BOOL));
			}

			if (privateData)
			{
				webc_BrowserSetPrivateData(hbrowser, privateData);
				onlineStatus = (WEBC_BOOL*) privateData;
			}

			if (onlineStatus)
			{
				*onlineStatus = webc_GetOnlineStatus();
			}
		}
	}

	return (JS_TRUE);
}

#endif /* WEBC_SUPPORT_JSCRIPT */

void chromeSetStatus (HBROWSER_HANDLE hbrowser, const WEBC_CHAR* str)
{
	HDOC_HANDLE hdoc = hbrowser? webc_BrowserGetDocument(hbrowser) : 0;
	if (hdoc)
	{
		HELEMENT_HANDLE helem = webc_DocFindElement(hdoc, "statusBar", 0, HTML_ELEMENT_ANY, 0);
		if (helem)
		{
			webc_ElementSetValue(helem, str, WEBC_TRUE);
			wcElSetStyle(helem, "font-weight: normal", (wcBOOL) WEBC_TRUE);

			if (!str || str[0] == 0)
			{
				HELEMENT_HANDLE htitle = webc_DocFindElement(hdoc, "documentTitle", 0, HTML_ELEMENT_ANY, 0);
				if (htitle)
				{
					WEBC_CHAR* docTitle = webc_ElementGetValue(htitle);
					if (!docTitle || docTitle[0] == 0)
					{
						wcElSetValue(helem, "untitled document");
					}
					else
					{
						webc_ElementSetValue(helem, docTitle, WEBC_TRUE);
					}
					if (docTitle)
					{
						webc_FreeString(docTitle);
					}
					wcElSetStyle(helem, "font-weight: bold",  (wcBOOL) WEBC_TRUE);
				}
			}
		}
	}
}

void chromeLoadStart (HBROWSER_HANDLE hbrowser)
{
	HDOC_HANDLE hdoc = hbrowser? webc_BrowserGetDocument(hbrowser) : 0;
	if (hdoc)
	{
		HELEMENT_HANDLE helem = webc_DocFindElement(hdoc, "throbber", 0, HTML_ELEMENT_ANY, 0);
		if (helem)
		{
			wcElSetStyle(helem, "visibility: show",  (wcBOOL)WEBC_TRUE);
		}

		helem = webc_DocFindElement(hdoc, "saveToArchiveIcon", 0, HTML_ELEMENT_ANY, 0);
		if (helem)
		{
			webc_ElementSetSrcASCII(helem, "savetoarchive_disabled.png", WEBC_TRUE);
		}
	}
}

void chromeLoadDone (HBROWSER_HANDLE hbrowser)
{
	HDOC_HANDLE hdoc = hbrowser? webc_BrowserGetDocument(hbrowser) : 0;
	if (hdoc)
	{
		HELEMENT_HANDLE helem = webc_DocFindElement(hdoc, "throbber", 0, HTML_ELEMENT_ANY, 0);
		if (helem)
		{
			wcElSetStyle(helem, "visibility: hide",  (wcBOOL) WEBC_TRUE);
		}

		helem = webc_DocFindElement(hdoc, "saveToArchiveIcon", 0, HTML_ELEMENT_ANY, 0);
		if (helem)
		{
			webc_ElementSetSrcASCII(helem, "savetoarchive.png", WEBC_TRUE);
		}

		WEBC_BOOL* onlineStatus = (WEBC_BOOL*) webc_BrowserGetPrivateData(hbrowser);
		if (onlineStatus)
		{
			webc_BrowserSetPrivateData(hbrowser, WEBC_NULL);
			webc_SetOnlineStatus(*onlineStatus);

			helem = webc_DocFindElement(hdoc, "saveToArchiveIcon", 0, HTML_ELEMENT_ANY, 0);
			if (helem)
			{
				webc_ElementSetSrcASCII(helem, "savetoarchive_disabled.png", WEBC_TRUE);
			}

			helem = webc_DocFindElement(hdoc, "onlineStatusIcon", 0, HTML_ELEMENT_ANY, 0);
			if (helem)
			{
				char* src = webc_ElementGetSrcASCII(helem);
				if (src)
				{
					if (!rtp_stricmp(src, "online_disabled.png"))
					{
						webc_ElementSetSrcASCII(helem, "online.png", WEBC_TRUE);
					}
					else
					{
						webc_ElementSetSrcASCII(helem, "offline.png", WEBC_TRUE);
					}
					webc_FreeASCIIString(src);
				}
			}

			rtp_free(onlineStatus);
		}
	}
}

void chromeSetUrl (HDOC_HANDLE hdoc, const WEBC_CHAR* str)
{
	HBROWSER_HANDLE hbrowser = webc_DocGetBrowser(hdoc);
	hdoc = hbrowser? webc_BrowserGetDocument(hbrowser) : 0;
	if (hdoc)
	{
		HELEMENT_HANDLE helem = webc_DocFindElement(hdoc, "urlBar", 0, HTML_ELEMENT_ANY, 0);
		if (helem)
		{
			webc_ElementSetValue(helem, str, WEBC_TRUE);
		}
	}
}

void chromeSetTitle (HDOC_HANDLE hdoc, const WEBC_CHAR* str)
{
	HBROWSER_HANDLE hbrowser = webc_DocGetBrowser(hdoc);
	hdoc = hbrowser? webc_BrowserGetDocument(hbrowser) : 0;
	if (hdoc)
	{
		HELEMENT_HANDLE helem = webc_DocFindElement(hdoc, "documentTitle", 0, HTML_ELEMENT_ANY, 0);
		if (helem)
		{
			webc_ElementSetValue(helem, str, WEBC_TRUE);
		}
	}
}

#if (WEBC_SUPPORT_JSCRIPT)

JSBool chromeAlert(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = JSVAL_NULL;

	if (argc > 0)
	{
		JSString* message = JS_ValueToString(cx, argv[0]);
		JS_AddRoot(cx, &message);

		HDOC_HANDLE hdoc = webc_JScriptGetDoc (cx);
		if (hdoc)
		{
			HBROWSER_HANDLE hbrowser = webc_DocGetBrowser(hdoc);
			hdoc = hbrowser? webc_BrowserGetDocument(hbrowser) : 0;

			if (hdoc)
			{
				HELEMENT_HANDLE helem = webc_DocFindElement(hdoc, "alertBar", 0, HTML_ELEMENT_ANY, 0);
				if (helem)
				{
					wcElSetStyle(helem, "visibility: visible",  (wcBOOL) WEBC_FALSE);

					helem = webc_DocFindElement(hdoc, "alertMessage", 0, HTML_ELEMENT_ANY, 0);
					if (helem)
					{
						wcElSetValue(helem, JS_GetStringBytes(message));
					}
				}
			}
		}

		JS_RemoveRoot(cx, &message);
	}

	return (JS_TRUE);
}

#endif /* WEBC_SUPPORT_JSCRIPT */

HTMLEventStatus chromeBrowserEventHandler (
        HBROWSER_HANDLE hbrowser,
		HTMLBrowserEvent* event
	)
{
	HELEMENT_HANDLE helem;
	HDOC_HANDLE hdoc;

	switch (event->type)
	{
		case HTML_BROWSER_EVENT_STATUS_CHANGE:
			chromeSetStatus(hbrowser, event->data.string);
			break;

		case HTML_BROWSER_EVENT_LOAD_START:
			chromeLoadStart(hbrowser);
			break;

		case HTML_BROWSER_EVENT_LOAD_DONE:
			chromeLoadDone(hbrowser);
			break;

		case HTML_BROWSER_EVENT_DOCUMENT_TITLE_CHANGE:
			hdoc = webc_BrowserGetDocument(hbrowser);
			if (hdoc)
			{
				helem = webc_DocFindElement(hdoc, "contentFrame", 0, HTML_ELEMENT_ANY, 0);
				if (helem)
				{
					hdoc = webc_ElementGetFrameDocument(helem);
					if (hdoc == event->hdoc)
					{
						chromeSetTitle(hdoc, event->data.string);
					}
				}
			}
			break;

		case HTML_BROWSER_EVENT_DOCUMENT_URL_CHANGE:
			hdoc = webc_BrowserGetDocument(hbrowser);
			if (hdoc)
			{
				helem = webc_DocFindElement(hdoc, "contentFrame", 0, HTML_ELEMENT_ANY, 0);
				if (helem)
				{
					hdoc = webc_ElementGetFrameDocument(helem);
					if (hdoc == event->hdoc)
					{
						chromeSetUrl(hdoc, event->data.string);
					}
				}
			}
			break;

		case HTML_BROWSER_EVENT_HTTP_NOT_AUTHORIZED:
		case HTML_BROWSER_EVENT_HTTP_AUTHORIZATION_REJECTED:
		{
			ChromeAuthInfo* info = (ChromeAuthInfo*) WEBC_MALLOC(sizeof(ChromeAuthInfo));
			if (info)
			{
				info->authDomain = event->data.authDomain;
				info->hdoc = event->hdoc;

				hdoc = webc_BrowserGetDocument(hbrowser);
				if (hdoc)
				{
					helem = webc_DocFindElement(hdoc, "authDialog", 0, HTML_ELEMENT_ANY, 0);
					if (helem)
					{
						wcElSetStyle(helem, "visibility: visible;",  (wcBOOL) WEBC_TRUE);

						helem = webc_DocFindElement(hdoc, "authOkButton", 0, HTML_ELEMENT_ANY, 0);
						if (helem)
						{
							ChromeAuthInfo* oldInfo = (ChromeAuthInfo*) webc_ElementGetPrivateData(helem);
							if (oldInfo)
							{
								rtp_free(oldInfo);
							}
							webc_ElementSetPrivateData(helem, info);
							return (HTML_EVENT_STATUS_CONTINUE);
						}
					}
				}

				rtp_free(info);
			}
			break;
		}

		case HTML_BROWSER_EVENT_BLANK_DOCUMENT_REQUESTED:
			hdoc = webc_BrowserGetDocument(hbrowser);
			if (hdoc)
			{
				helem = webc_DocFindElement(hdoc, "contentFrame", 0, HTML_ELEMENT_ANY, 0);
				if (helem)
				{
					// when the target of a link is "_blank", just use the main content frame
					event->out.blankDocument = webc_ElementGetFrameDocument(helem);
				}
			}
			break;
	}

	return (HTML_EVENT_STATUS_CONTINUE);
}

HTMLEventStatus chromeAuthHandler (
        HBROWSER_HANDLE hbrowser,
        HDOC_HANDLE hdoc,
        HELEMENT_HANDLE helem,
        HTMLEvent* event,
        char* param
    )
{
	if (event->type == HTML_EVENT_CLICK)
	{
		HELEMENT_HANDLE authDialog = webc_DocFindElement(hdoc, "authDialog", 0, HTML_ELEMENT_ANY, 0);
		if (authDialog)
		{
			wcElSetStyle(authDialog, "visibility: hidden;",  (wcBOOL) WEBC_FALSE);

			ChromeAuthInfo* info = (ChromeAuthInfo*) webc_ElementGetPrivateData(helem);
			if (info)
			{
				webc_ElementSetPrivateData(helem, 0);

				HELEMENT_HANDLE authUser = webc_DocFindElement(hdoc, "authUser", 0, HTML_ELEMENT_ANY, 0);
				HELEMENT_HANDLE authPass = webc_DocFindElement(hdoc, "authPass", 0, HTML_ELEMENT_ANY, 0);
				if (authUser && authPass)
				{
					char* user = webc_ElementGetValueASCII(authUser);
					if (user)
					{
						char* pass = webc_ElementGetValueASCII(authPass);
						if (pass)
						{
							wcCtx Ctx;
    						wcCtxtInit(&Ctx, webc_DocGetBrowser(info->hdoc),info->hdoc);
							webc_HttpSetAuthorization(&info->authDomain, user, pass, 1800);
							wcDocSetUrl(&Ctx, info->authDomain.url, wcTRUE);
							webc_FreeASCIIString(pass);
						}
						webc_FreeASCIIString(user);
					}
				}

				rtp_free(info);
			}

			return (HTML_EVENT_STATUS_HALT);
		}
	}
	else if (event->type == HTML_EVENT_OBJECT_DESTROYED)
	{
		ChromeAuthInfo* info = (ChromeAuthInfo*) webc_ElementGetPrivateData(helem);
		if (info)
		{
			webc_ElementSetPrivateData(helem, 0);
			rtp_free(info);
		}
	}

	return (HTML_EVENT_STATUS_CONTINUE);
}

HTMLEventStatus chromeLoadHandler2 (
        HBROWSER_HANDLE hbrowser,
        HDOC_HANDLE hdoc,
        HELEMENT_HANDLE helem,
        HTMLEvent* event,
        char* param
    )
{
  #if (WEBC_SUPPORT_JSCRIPT)
	if (event->type == HTML_EVENT_LOAD)
	{
		HELEMENT_HANDLE hframeElem = webc_DocFindElement(hdoc, "contentFrame", 0, HTML_ELEMENT_ANY, 0);
		if (hframeElem)
		{
			hdoc = webc_ElementGetFrameDocument (hframeElem);
			if (hdoc)
			{
				webc_JScriptDefineFunction(hdoc, "alert", (JSNative) chromeAlert, 1);
			}
		}
	}
  #endif /* WEBC_SUPPORT_JSCRIPT */

	return (HTML_EVENT_STATUS_CONTINUE);
}

HTMLEventStatus chromeLoadHandler (
        HBROWSER_HANDLE hbrowser,
        HDOC_HANDLE hdoc,
        HELEMENT_HANDLE helem,
        HTMLEvent* event,
        char* param
    )
{
	if (event->type == HTML_EVENT_LOAD)
	{
	  #if (WEBC_SUPPORT_JSCRIPT)
		webc_JScriptDefineFunction(hdoc, "WriteFile", (JSNative) WriteFile, 2);
		webc_JScriptDefineFunction(hdoc, "ReadFile", (JSNative) ReadFile, 1);
		webc_JScriptDefineFunction(hdoc, "suspendDraw", (JSNative) chromeSuspendDraw, 0);
		webc_JScriptDefineFunction(hdoc, "resumeDraw", (JSNative) chromeResumeDraw, 0);
		webc_JScriptDefineFunction(hdoc, "invalidate", (JSNative) chromeInvalidate, 0);
		webc_JScriptDefineFunction(hdoc, "draw", (JSNative) chromeDraw, 0);
		webc_JScriptDefineFunction(hdoc, "SaveContentForOffline", (JSNative) SaveContentForOffline, 0);
		webc_JScriptDefineFunction(hdoc, "EnableOnlineStatusRestore", (JSNative) EnableOnlineStatusRestore, 0);
	  #endif /* WEBC_SUPPORT_JSCRIPT */

		webc_RegisterEventCallback (chromeLoadHandler2, "chromeLoadHandler2");
		webc_RegisterEventCallback (chromeAuthHandler, "chromeAuthHandler");
	}

	return (HTML_EVENT_STATUS_CONTINUE);
}

HTMLEventStatus bodyHandler (
        HBROWSER_HANDLE hbrowser,
        HDOC_HANDLE hdoc,
        HELEMENT_HANDLE helem,
        HTMLEvent *pEvent,
        char *pParam
    )
{
    switch (pEvent->type)
    {
       case HTML_EVENT_OBJECT_CREATED:
          webc_ElementSetPrivateData(helem, 0);
          break;

       case HTML_EVENT_OBJECT_DESTROYED:
       {
       	  void* blah = webc_ElementGetPrivateData(helem);
          break;
		}

       case HTML_EVENT_ATTRIBUTE_PARSED:
       {
       	  void* blah = webc_ElementGetPrivateData(helem);
          break;
       }
    }

    return (HTML_EVENT_STATUS_CONTINUE);
}

HTMLEventStatus inputHandler (
        HBROWSER_HANDLE hbrowser,
        HDOC_HANDLE hdoc,
        HELEMENT_HANDLE helem,
        HTMLEvent *pEvent,
        char *pParam
    )
{
    switch (pEvent->type)
    {
       case HTML_EVENT_OBJECT_CREATED:
          webc_ElementSetPrivateData(helem, 0);
          break;

       case HTML_EVENT_OBJECT_DESTROYED:
       {
       	  void* blah = webc_ElementGetPrivateData(helem);
          break;
		}

       case HTML_EVENT_ATTRIBUTE_PARSED:
       {
       	  void* blah = webc_ElementGetPrivateData(helem);
          break;
       }
    }

    return (HTML_EVENT_STATUS_CONTINUE);
}
