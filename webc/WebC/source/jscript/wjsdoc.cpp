/*
|  WJSDOC.C -
|
|  EBS - WebC
|
|
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

#include "webcjhconfig.h"
#if (WEBC_SUPPORT_JSCRIPT)
/*****************************************************************************/
/* Header files
 *****************************************************************************/
#include "wjsdoc.h"
#include "wjsbrowser.h"
#include "vector.h"
#include "jhanchor.h"
#include "jhbody.h"
#include "jhcoll.h"
#include "jhdoc.h"
#include "jhelem.h"
#include "jhform.h"
#include "jhflow.h"
#include "jhframe.h"
#include "jhfrset.h"
#include "jhiframe.h"
#include "jhimg.h"
#include "jhinput.h"
#include "jhistory.h"
#include "jhoption.h"
#include "jhselect.h"
#include "jhtable.h"
#include "jhtarea.h"
#include "jhbutton.h"
#include "jhtblrow.h"
#include "jhtitle.h"
#include "jharea.h"
#include "jhmap.h"
#include "jhtcell.h"
#include "jloc.h"
#include "jnav.h"
#include "jrule.h"
#include "jssheet.h"
#include "jstyle.h"
#include "jwindow.h"
#include "jevent.h"
#include "jsobjref.hpp"
#include "webctmo.h"
#include "wjscript.hpp"
#include "webcmem.h"
#include "hbody.hpp"
#include "jhutils.h"
#include "htmldoc.hpp"
#include "objref.cpp"
#include "htmlbrow.hpp"
#if (WEBC_SUPPORT_AJAX)
#include "jhXMLHttpRequest.h"
#include "jhXMLHttpRequestFactory.h"
#endif

/*****************************************************************************/
/* Macros
 *****************************************************************************/

/*****************************************************************************/
/* Types
 *****************************************************************************/

/*****************************************************************************/
/* Function Prototypes
 *****************************************************************************/
static int _JScriptBranchCallback(JSContext* cx, JSScript* script);

extern "C" int _jwin_callFunction(void*, long);

int _jsmgr_matchMyTimeouts(TimeoutInfo* timeout, void* data)
{
	WebcJSScript* script = (WebcJSScript*) timeout->pData;
	WebcJSDocumentContext* jsDocCtx = (WebcJSDocumentContext*) data;

	return (timeout->Func == _jwin_callFunction && script && jsDocCtx && script->GetJSMgr() == jsDocCtx);
}


#if (WEBC_DEBUG_JSCRIPT)

extern FILE *gpJSLogFile;
#define WEBC_CFG_JS_LOG_FILE        "jslog.txt"
#define JS_DEBUG_REPORT_JSOBJ(X,Y)  {gpJSLogFile = fopen(WEBC_CFG_JS_LOG_FILE, "a+");\
                                     fprintf(gpJSLogFile, "%s, %08x\n", (X), (long)(Y));\
                                     fclose(gpJSLogFile);}

#else // !WEBC_DEBUG_JSCRIPT

#define JS_DEBUG_REPORT_JSOBJ(X,Y)

#endif // WEBC_DEBUG_JSCRIPT

struct voidptr
{
	void* data;
};

#if (WEBC_SUPPORT_JS_ERROR_REPORT)

void JScriptError(JSContext *cx, const char *message, JSErrorReport *report)
{
	char *str = 0;
	if (message)
	{
		str = (char *) WEBC_MALLOC(tc_strlen(message) + 34);
		if (str)
		{
			tc_strcpy(str, "A JavaScript Error Has Occured:\n");
			tc_strcat(str, message);
			webc_user_debug_window((char *)str);
			webc_user_alert_box((char *)str);
			WEBC_FREE(str);
		}
	}
	else
	{
		webc_user_alert_box("A JavaScript Error Has Occured");
	}
}

#endif // WEBC_SUPPORT_JS_ERROR_REPORT
/*****************************************************************************/
/* Data
 *****************************************************************************/

/*****************************************************************************/
/* Function Definitions
 *****************************************************************************/

/*---------------------------------------------------------------------------*/
WebcJSDocumentContext::WebcJSDocumentContext (WebcJSBrowserContext* browserCtx, HTMLDocument* doc)
{
	mpCx = 0;
	mpFakeBody = 0;
	mpDefaultScript = 0;
	mErrorFunction = 0;
	mBrowserContext = browserCtx;
	rtp_memset(&mHtmlEvent, 0, sizeof(HTMLEvent));
	mHtmlEvent.type = HTML_EVENT_NONE;

	//initialize the vectors
	vector_init(mpCollections, sizeof(jhtml_collection));

	Init(doc);
}

/*---------------------------------------------------------------------------*/
WebcJSDocumentContext::~WebcJSDocumentContext (void)
{
	CancelTimeouts();
	Clear();
}

/*---------------------------------------------------------------------------*/
void WebcJSDocumentContext::recycle (void)
{
	WEBC_DELETE(this);
}

/*---------------------------------------------------------------------------*/
void WebcJSDocumentContext::CancelTimeouts (void)
{
	if (GetDocument())
	{
		if (GetDocument()->GetBrowser())
		{
			GetDocument()->GetBrowser()->TimeoutManager()->CancelTimeout (
					_jsmgr_matchMyTimeouts,
					this,
					WEBC_TRUE
				);
		}
	}
}

/*---------------------------------------------------------------------------*/
void WebcJSDocumentContext::Destroy (void)
{
	CancelTimeouts();
#if (WEBC_INCLUDE_SEPT2010_BUGFIXES)
	if (mpFakeBody) // 10/05/2010 add missing WEBC_DELETE(mpFakeBody) call.
	{
		WEBC_DELETE(mpFakeBody);
		mpFakeBody = 0;
	}
#endif
	release();
}

/*---------------------------------------------------------------------------*/
void WebcJSDocumentContext::Init (HTMLDocument* htmlDoc)
{
	WEBC_DEBUG_LOG_TIME_HANDLE(TimeInside)
	WEBC_DEBUG_LOG_TIME_ENTER(TimeInside,"WebcJSDocumentContext::Init","")

	mDocument.Set(htmlDoc);

	WEBC_NEW_VERBOSE(mpFakeBody, HTMLBody(htmlDoc),"WebcJSDocumentContext:mpFakeBody");
	WEBC_NEW_VERBOSE(mpDefaultScript, WebcJSScript(this),"WebcJSDocumentContext:mpDefaultScript");

	if (mBrowserContext && mpFakeBody && mpDefaultScript)
	{
	WEBC_DEBUG_LOG_TIME_HANDLE(TimeInside)
		//initialize a context
		mpCx = mBrowserContext->AllocContext();
		if (!mpCx || !htmlDoc)
		{
			return;
		}
		jhutil_SetContextPrivate(mpCx, mpDefaultScript);

		WEBC_DEBUG_LOG_TIME_ENTER(TimeInside,"jwindow_Init","")
		//initialize the global object and standard classes
		//NOTE:  The window object IS the Global Object!
		//  (this is set in jwindow_Init by JS_InitStandardClasses
		JSObject *win = jwindow_Init(mpCx, 0);
		JS_DEBUG_REPORT_JSOBJ("Window",win);
		WEBC_DEBUG_LOG_TIME_EXIT(TimeInside,"jwindow_Init","")

		if (!win)
		{
			return;
		}
		mWindow.Set(win);
		mWindow.SetPrivate(this);

		//initialize the DOM objects
		//
		// Note: every init returns another JSObject * which can
		// be passes to any other init method which will make that
		// class inherit the first
		//
		JSObject *proto = jstyle_Init(mpCx, 0);

		proto = jhtml_element_Init (mpCx, 0 /*proto*/);

		//these objects inherit from HTMLElement
		jhtml_anchor_Init	    (mpCx, proto);
		jhtml_body_Init         (mpCx, proto);
		jhtml_flow_Init         (mpCx, proto);
		jhtml_form_Init         (mpCx, proto);
  #if (WEBC_SUPPORT_FRAMES)
		jhtml_frame_Init        (mpCx, proto);
	  #if (WEBC_SUPPORT_IFRAMES)
		jhtml_iframe_Init       (mpCx, proto);
	  #endif // WEBC_SUPPORT_IFRAMES
		jhtml_frameset_Init     (mpCx, proto);
  #endif // WEBC_SUPPORT_FRAMES
		jhtml_image_Init        (mpCx, proto);
		jhtml_table_Init        (mpCx, proto);
		jhtml_tablerow_Init     (mpCx, proto);
		jhtml_tablecell_Init    (mpCx, proto);
		jhtml_title_Init        (mpCx, proto);
		jhtml_map_Init			(mpCx, proto);
		jhtml_area_Init			(mpCx, proto);
		jhtml_button_Init		(mpCx, proto);

		proto = jhtml_input_Init(mpCx, proto);
		//These Objects inherit from Input
		jhtml_select_Init       (mpCx, proto);
		jhtml_textarea_Init	    (mpCx, proto);
		jhtml_option_Init       (mpCx, proto);

		//These Objects do not inherit from anything
		jhtml_doc_Init          (mpCx, 0);
		jlocation_Init          (mpCx, 0);
		jnavigator_Init         (mpCx, 0);
		jstylesheet_Init        (mpCx, 0);
		jrule_Init				(mpCx, 0);
		jhistory_Init			(mpCx, 0);
		jevent_Init             (mpCx, 0);
		jhtml_collection_Init   (mpCx, 0);

#if (WEBC_SUPPORT_AJAX)
		jhtml_XMLHttpRequest_Init(mpCx, 0);
		jhtml_XMLHttpRequestFactory_Init(mpCx, 0);
#endif

		// create the document object to be used by scripts
		JSObject *doc = JS_DefineObject(mpCx, win, "document", GetJHtmlDocument(), 0, 0);
		htmlDoc->SetJSObject(doc);
		JS_DEBUG_REPORT_JSOBJ("Document",doc);

		// create the event object
		mEventObj.Set(JS_DefineObject(mpCx, win, "event", getJEventClass(), 0,0));
		if (mEventObj.Get())
		{
			jhutil_SetPrivate(mpCx, mEventObj.Get(), &mHtmlEvent);
		}
		JS_DEBUG_REPORT_JSOBJ("Event",mEventObj.Get());

		// create the location object
		mLocation.Set(JS_DefineObject(mpCx, win, "location", getJLocation(), 0,0));
		if (mLocation.Get())
		{
			jhutil_SetPrivate(mpCx, mLocation.Get(), htmlDoc);
		}
		JS_DEBUG_REPORT_JSOBJ("Location",mLocation.Get());

		// create the navigation object
		mNavigator.Set(JS_DefineObject(mpCx, win, "navigator", getJNavigator(), 0,0));
		if (mNavigator.Get())
		{
			jhutil_SetPrivate(mpCx, mNavigator.Get(), 0);
		}
		JS_DEBUG_REPORT_JSOBJ("Navigator",mNavigator.Get());

		// create the history object
		mHistory.Set(JS_DefineObject(mpCx, win, "history", getJHistory(), 0,0));
		if (mHistory.Get())
		{
			jhutil_SetPrivate(mpCx, mHistory.Get(), htmlDoc->GetBrowser());
		}
		JS_DEBUG_REPORT_JSOBJ("History",mHistory.Get());


		// Create the Object associated with the Body Element
		if (htmlDoc->Body())
		{
			htmlDoc->Body()->SetJSObject(CreateElement(HTML_BODY_ELEMENT));
		}
		mpFakeBody->SetJSObject(CreateElement(HTML_BODY_ELEMENT));

	  #if (WEBC_SUPPORT_STYLE_SHEETS)

		if (htmlDoc->GetCSSContext())
		{
			// Create the style sheet object
			JSObject *ssheet = JS_DefineObject(mpCx, JS_GetGlobalObject(mpCx), "__stylesheet", getJStyleSheet(), 0, 0);
			if (ssheet)
			{
				jhutil_SetPrivate(mpCx, ssheet, htmlDoc->GetCSSContext());
			}
			htmlDoc->GetCSSContext()->SetJSObject(ssheet);
			JS_DEBUG_REPORT_JSOBJ("Stylesheet", ssheet);
		}

	  #endif //(WEBC_SUPPORT_STYLE_SHEETS)

		JS_SetBranchCallback(mpCx, (JSBranchCallback) _JScriptBranchCallback);

	  #if (WEBC_SUPPORT_JS_ERROR_REPORT)
		JS_SetErrorReporter(mpCx, JScriptError);
	  #endif // WEBC_SUPPORT_JS_ERROR_REPORT
	}
	WEBC_DEBUG_LOG_TIME_EXIT(TimeInside,"WebcJSDocumentContext::Init","")

}

/*---------------------------------------------------------------------------*/
void WebcJSDocumentContext::Clear(void)
{
	if (mpFakeBody)
	{
		WEBC_DELETE(mpFakeBody);
		mpFakeBody = 0;
	}

	mEventObj.SetPrivate(0);
	mLocation.SetPrivate(0);
	mNavigator.SetPrivate(0);
	mHistory.SetPrivate(0);
	mWindow.SetPrivate(0);

	if (mpDefaultScript)
	{
		WEBC_DELETE(mpDefaultScript);
		mpDefaultScript = 0;
		if (mpCx)
		{
			jhutil_SetContextPrivate(mpCx, mpDefaultScript);
		}
	}

	vector_iterator pvi[1];
	jhtml_collection *pColl = (jhtml_collection *) vector_get_first(mpCollections, pvi);
	while (pColl)
	{
		//if we allocated memory for the nameOfAll string dealloc' it
		if (pColl->nameOfAll)
		{
			webc_free_string_copy(pColl->nameOfAll, __FILE__, __LINE__);
			pColl->nameOfAll = 0;
		}
		if (pColl->jsObj.Get())
		{
			jhutil_SetPrivate(mpCx, pColl->jsObj.Get(), 0);
			pColl->jsObj.Set(0);
		}
		pColl = (jhtml_collection *)vector_get_next(pvi);
	}//end while
	//delete the vector
	vector_delete(mpCollections);

	CancelTimeouts();

	//delete the context associated with this WebcJSDocumentContext
	if (mpCx)
	{
		JS_DestroyContext(mpCx);
		mpCx = 0;
	}
}

/*---------------------------------------------------------------------------*/
int WebcJSDocumentContext::AddNewElement (HTMLElement* elem)
{
	if (mpFakeBody && elem)
	{
		mpFakeBody->InsertLast(elem);
		return (1);
	}

	return (0);
}

/*---------------------------------------------------------------------------*/
JSContext* WebcJSDocumentContext::AllocContext (void)
{
	if (mBrowserContext && mpDefaultScript && mpCx)
	{
		JSContext *cx = mBrowserContext->AllocContext();
		if (cx)
		{
			// If this context is going to be used to run a script or function,
			//  then the ContextPrivate ptr will be set to the WebcJSScript executing
			jhutil_SetContextPrivate(cx, mpDefaultScript);

			// All contexts that come from a single WebcJSDocumentContext share the same global
			//  object, the HTML document's "window" object
			JS_SetGlobalObject(cx, JS_GetGlobalObject(mpCx));

			// This branch callback allows the script to be halted if the escape key is
			//  depressed.  This is useful for terminating blatantly non-terminal
			//  scripts.
			JS_SetBranchCallback(cx, (JSBranchCallback) _JScriptBranchCallback);

		  #if (WEBC_SUPPORT_JS_ERROR_REPORT)
			// Set the error reporting function for this context
			JS_SetErrorReporter(cx, JScriptError);
		  #endif // WEBC_SUPPORT_JS_ERROR_REPORT

			claim();

			return(cx);
		}
	}

	return (WEBC_NULL);
}

/*---------------------------------------------------------------------------*/
JSObject* WebcJSDocumentContext::CreateElement (HTMLElementType type)
{
	if (mpCx)
	{
		return JS_NewObject(mpCx, getJSClassByElementType(type), 0, 0);
	}

	return (WEBC_NULL);
}

/*---------------------------------------------------------------------------*/
void WebcJSDocumentContext::FreeContext (JSContext* cx)
{
	if (mBrowserContext)
	{
		mBrowserContext->FreeContext(cx);
	}

	release();
}

/*---------------------------------------------------------------------------*/
void WebcJSDocumentContext::GarbageCollect (void)
{
	if (mpCx)
	{
		JS_GC(mpCx);
	}
}

/*---------------------------------------------------------------------------*/
void WebcJSDocumentContext::SetWindowEvent  (HTMLEvent* event)
{
	mHtmlEvent = *event;
}

/*---------------------------------------------------------------------------*/
int _JScriptBranchCallback(JSContext *cx, JSScript *pScript)
{
	WebcJSScript *pJSScr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	if (pJSScr)
	{
		/* tbd - update browser state to detect when the escape key is down
		HTMLBrowser* browser = pJSScr->GetBrowser();
		if (browser)
		{
			browser->ProcessInputQueue(WEBC_FALSE);

			if (browser->KeyDown(WGK_ESC))
			{
        		pJSScr->Stop();
			}
		}
		*/

		pJSScr->IncBranchCount();
		pJSScr->CheckRunningTime();

		if (pJSScr->CheckFlag(JS_SCRIPT_FLAG_STOPPED))
		{
			return (JS_FALSE);
		}
	}
	else
	{
		return (JS_FALSE);
	}

	return (JS_TRUE);
}

#endif
