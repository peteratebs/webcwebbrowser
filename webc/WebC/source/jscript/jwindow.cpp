//
// JWINDOW.C -
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//    This module contains routines to preprocess and parse HTML text.
//

/*****************************************************************************/
// Headers
/*****************************************************************************/

#include "webcjhconfig.h"
#include "jwindow.h"
#if (WEBC_SUPPORT_JSCRIPT)
#include "jhutils.h"
#include "webload.hpp"
#include "webctmo.h"
#include "htmldoc.hpp"
#include "htmlfind.hpp"
#include "htmlbrow.hpp"
#include "hform.hpp"
#include "hframe.hpp"
#include "hradio.hpp"
#include "wjscript.hpp"
#include "webcstr.h"
#include "webcassert.h"
#include "objref.cpp"
#include "webcmem.h"
#include "jhcoll.h"
#include "jhelem.h"
#include "rtpprint.h"
#if (WEBC_SUPPORT_AJAX)
#include "jhXMLHttpRequestFactory.h"
#endif

extern "C" {
void JS_ReportError(JSContext *cx, const char *format, ...);
}

/*****************************************************************************/
// Types
/*****************************************************************************/

//set the unique ids for the properties of the window class
enum jwindow_tinyid
{
	WINDOW_CLOSED=-1,
	WINDOW_DEFAULTSTATUS=-2,
	WINDOW_DIALOGARGUMENTS=-3,
	WINDOW_DIALOGHEIGHT=-4,
	WINDOW_DIALOGLEFT=-5,
	WINDOW_DIALOGTOP=-6,
	WINDOW_DIALOGWIDTH=-7,
	WINDOW_FRAMEELEMENT=-8,
	WINDOW_LENGTH=-9,
	WINDOW_NAME=-10,
	WINDOW_OFFSCREENBUFFERING=-11,
	WINDOW_ONLOAD=-12,
	WINDOW_OPENER=-13,
	WINDOW_PARENT=-14,
	WINDOW_RETURNVALUE=-15,
	WINDOW_SCREENLEFT=-16,
	WINDOW_SCREENTOP=-17,
	WINDOW_SELF=-18,
	WINDOW_STATUS=-19,
	WINDOW_TOP=-20,
	WINDOW_WINDOW=-21,
	WINDOW_FRAMES=-22,
	WINDOW_ONERROR=-23
#define INCLUDE_DOCUMENT 1
#if (INCLUDE_DOCUMENT)
	,WINDOW_DOCUMENT=-24
#endif
#if (WEBC_SUPPORT_AJAX)
	,WINDOW_XMLHTTPREQUEST=-25
#endif
};


/*****************************************************************************/
// Macros
/*****************************************************************************/

#define WEBC_JWINDOW_MAX_NUM_TIMEOUTS    100


/*****************************************************************************/
// Data
/*****************************************************************************/

static int giTimeoutId = 0;
static int giNumTimeouts = 0;
#if (WEBC_SUPPORT_AJAX)
static JSObject *g_XMLHTTPRequestFactorySingleton=0;
#endif
// Handy for resulve to return a dummy object when call is not implemented
JSClass null_class = {
    "Object", 0,
    JS_PropertyStub,JS_PropertyStub,JS_PropertyStub,JS_PropertyStub,
    JS_EnumerateStub,JS_ResolveStub,JS_ConvertStub,JS_FinalizeStub
};
//JS_DefineObject(cx, obj, "screen", &null_class, 0, 0);


/*****************************************************************************/
// Functions
/*****************************************************************************/

static int _matchId (TimeoutInfo* timeout, void* data)
{
	WEBC_UINT16 id = *((WEBC_UINT16*)data);
	return (timeout->wId == id);
}


JSBool jwindow_resolve(JSContext *cx, JSObject *obj, jsval id)
{
	WebcJSDocumentContext *jsmgr = (WebcJSDocumentContext *) jhutil_GetPrivate(cx, obj);
	HTMLDocument *pDoc = jsmgr? jsmgr->GetDocument() : 0;

	if (!pDoc)
	{
		return JS_TRUE;
	}
	if (JSVAL_IS_STRING(id))
	{
		WEBC_CHAR *name = WEBC_JS_STRING_TO_WEBC_STRING(JSVAL_TO_STRING(id));
		if (!webc_c_strcmp(name, "frames"))
		{
			if (pDoc->Body())
			{
				jhtml_collection_New(jsmgr, obj, "frames", pDoc->Body(), WIN_FRAMES);
			}
		}
		else if (!webc_c_strcmp(name, "screen"))
		{
        // Implement window.screen
            // Create an object and populate it with screen info
            JSObject *pScreenobject = JS_DefineObject(cx, obj, "screen", &null_class, 0, JSPROP_READONLY);
			if (pScreenobject)
			{
			HTMLScreenInfo ScreenInfo;
      	    DisplayManager *pDisp = pDoc->GetDisplayManager();
			if(pDisp)
			{
      			WebGraphics *pGc = pDisp->GetGraphics();
				if(pGc)
				{
					jsval vp;
      				pGc->GetScreenInfo(&ScreenInfo);
					vp = INT_TO_JSVAL(ScreenInfo.width);
					JS_SetProperty(cx, pScreenobject, "width", &vp);
					vp = INT_TO_JSVAL(ScreenInfo.availWidth);
					JS_SetProperty(cx, pScreenobject, "availWidth", &vp);
					vp = INT_TO_JSVAL(ScreenInfo.height);
					JS_SetProperty(cx, pScreenobject, "height", &vp);
					vp = INT_TO_JSVAL(ScreenInfo.availHeight);
					JS_SetProperty(cx, pScreenobject, "availHeight", &vp);
					vp = INT_TO_JSVAL(ScreenInfo.colorDepth);
					JS_SetProperty(cx, pScreenobject, "colorDepth", &vp);
				}
			}
			}

   		}
		else if (!(pDoc->GetFlags() & HDOC_FLAG_CREATING_JMANAGER))
		{
			// see if 'name' refers to a custom javascript function
			JSNative native;
			WEBC_INT32 numArgs;
			if (pDoc->ResolveJSFunction(&native, &numArgs, name))
			{
				// define the function as a property of this object
    			if (JS_DefineFunction (cx, obj, JS_GetStringBytes(JSVAL_TO_STRING(id)),
						native, numArgs, 0 /* flags: deprecated */) != NULL)
    			{
   			    	return JS_TRUE;
   			    }
   			    // a strange case: if define function fails, try to resolve the property
   			    //  in some other way
			}
			// try to find the element by NAME or ID

	        HTMLElement *pElem = 0;
#if (WEBC_SUPPORT_FRAMES)
	        // Implement find frame names
	        HTMLDocument *pParent = pDoc;
			if (pParent)
            {
				HTMLTypeAndNameOrIdFinder framefinder(name, HTML_FRAME_ELEMENT);
				HTMLTypeAndNameOrIdFinder iframefinder(name, HTML_IFRAME_ELEMENT);
				pElem = pParent->FindElement(&framefinder);
				if(!pElem) // try <iframe>
					pElem = pParent->FindElement(&iframefinder);
				if (pElem)
				{
					HTMLFrame *pFrame = (HTMLFrame *) pElem;
                    if (pFrame->FrameDocument() && pFrame->FrameDocument()->GetJSMgr())
                    {
                        JSObject *jsObj = pFrame->FrameDocument()->GetJSMgr()->GetGlobalObject();
                        if (jsObj)
                        {
					  #if (WEBC_SUPPORT_UNICODE)
						JS_DefineUCProperty(cx, obj, name, webc_strlen(name),
						                  OBJECT_TO_JSVAL(jsObj),
						                  0 /*getter*/, 0 /*setter*/, JSPROP_READONLY);
					  #else
						JS_DefineProperty(cx, obj, name, OBJECT_TO_JSVAL(jsObj),
										  0 /*getter*/, 0 /*setter*/, JSPROP_READONLY);
					  #endif
					    }
					    return(JS_TRUE);
                    }
                    else
                        pElem = 0;
				}
//                pParent = pParent->Parent();
            }
#endif
			if (!pElem && pDoc->Body())
			{
				HTMLNameOrIdFinder nameFinder(name);
				HTMLElementFormFinder formFinder;
				HTMLElementNotFinder noFormFinder(&formFinder);
				HTMLElementAndFinder f(&nameFinder, &noFormFinder);

				if (!pElem)
				    pElem = pDoc->Body()->FindElement(&f,  0 /* don't search subframes */);
				if (!pElem)
				{
					HTMLElementTypeFinder ff(HTML_FORM_ELEMENT);
					HTMLElementAndFinder formWithName(&ff, &nameFinder);

					// maybe it's a form name/id
					pElem = pDoc->Body()->FindElement(&formWithName,  0 /* don't search subframes */);
					if (!pElem)
					{
						pElem = jsmgr->GetFakeBody()->FindElement(&f, 0  /* don't search subframes */);
					}
				}
				if (pElem)
				{
					if (pElem->Type() == HTML_RADIO_BUTTON_ELEMENT)
					{
						HTMLRadioButton *pRadio = (HTMLRadioButton *) pElem;
						if ((pRadio->mpGroupNext != pRadio) && pRadio->Name() &&
						    !webc_strcmp(name, pRadio->Name()))
						{
							// make a collection for the radio button group elements
							jhtml_collection_New(jsmgr, obj, JS_GetStringBytes(JSVAL_TO_STRING(id)), pElem, RADIO_BUTTONS);
							return JS_TRUE;
						}
					}

					// found it; define a property for this object
					JSObject *jsObj = pElem->CreateJSObject();
					if (jsObj)
					{
					  #if (WEBC_SUPPORT_UNICODE)
						JS_DefineUCProperty(cx, obj, name, webc_strlen(name),
						                  OBJECT_TO_JSVAL(jsObj),
						                  0 /*getter*/, 0 /*setter*/, JSPROP_READONLY);
					  #else
						JS_DefineProperty(cx, obj, name, OBJECT_TO_JSVAL(jsObj),
										  0 /*getter*/, 0 /*setter*/, JSPROP_READONLY);
					  #endif
					}
				}
			}
		}
	}

    return JS_TRUE;
}


/**
 * This method is called when getting a Property from Window
 */
JSBool jwindow_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	if (JSVAL_IS_INT(id))
	{
		WebcJSDocumentContext *jsmgr = (WebcJSDocumentContext *) jhutil_GetPrivate(cx, obj);
		if (!jsmgr)
			return JS_TRUE;

		HTMLDocument *pDoc = jsmgr->GetDocument();
		WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
		HTMLDocument *pParent = 0;

		switch (JSVAL_TO_INT(id))
		{
			case WINDOW_CLOSED:
				// no, if we're here our window is not closed
				*vp = BOOLEAN_TO_JSVAL(JS_FALSE);
				break;

			case WINDOW_DEFAULTSTATUS:
				// tbd
				*vp = JSVAL_NULL;
				break;

			case WINDOW_STATUS:
				*vp = (pDoc)? STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pDoc->Status())) : JSVAL_NULL;
				return JS_TRUE;

			case WINDOW_DIALOGARGUMENTS:
				//we do not support dialogs
				break;

			case WINDOW_DIALOGHEIGHT:
				//we do not support dialogs
				break;

			case WINDOW_DIALOGLEFT:
				//we do not support dialogs
				break;

			case WINDOW_DIALOGTOP:
				//we do not support dialogs
				break;

			case WINDOW_DIALOGWIDTH:
				//we do not support dialogs
				break;

			case WINDOW_FRAMEELEMENT:
			{
			  #if (WEBC_SUPPORT_FRAMES)
				if (!pDoc->IsQuarantined())
				{
					HTMLElement *pElem = pDoc? pDoc->GetFrame() : WEBC_NULL;
					*vp = pElem? OBJECT_TO_JSVAL(pElem->CreateJSObject()) : WEBC_NULL;
					return JS_TRUE;
				}
			  #endif // WEBC_SUPPORT_FRAMES
				*vp = JSVAL_NULL;
				return JS_TRUE;
			}

			case WINDOW_LENGTH:
			{
				HTMLCountByTypeFinder finder(HTML_FRAME_ELEMENT);
				if (pDoc)
				{
					pDoc->FindElement(&finder);
					*vp = INT_TO_JSVAL(finder.Length());
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				return JS_TRUE;
			}

			case WINDOW_NAME:
				if (pDoc)
				{
				  #if (WEBC_SUPPORT_FRAMES)
					if (pDoc->GetFrame())
					{
						*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pDoc->GetFrame()->Name()));
					}
					else
				  #endif // WEBC_SUPPORT_FRAMES
					{
						*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pDoc->Name()));
					}
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				return JS_TRUE;

			case WINDOW_OFFSCREENBUFFERING:
				//we always use offscreen buffering
				*vp = BOOLEAN_TO_JSVAL(JS_TRUE);
				return JS_TRUE;
			case WINDOW_ONLOAD:
// <EDIT> 20080120  Unsure of what to do with WINDOW_ONLOAD
				return JS_TRUE;
			case WINDOW_ONERROR:
			    if (jsmgr->mErrorFunction)
                {
			        *vp = OBJECT_TO_JSVAL(jsmgr->mErrorFunction);
                }
                else
                    *vp = JSVAL_NULL;
				return JS_TRUE;
			case WINDOW_OPENER:
				//we don't keep track of who opened the web browser window.
				*vp = JSVAL_NULL;
				break;

			case WINDOW_PARENT:
				if (pDoc)
				{
				  #if WEBC_SUPPORT_FRAMES
					if (pDoc->IsQuarantined())
					{
						*vp = OBJECT_TO_JSVAL(obj);
						//*vp = JSVAL_NULL;
						return (JS_TRUE);
					}
				  #endif // WEBC_SUPPORT_FRAMES

					pParent = pDoc->Parent();
					*vp = JSVAL_NULL;
					if (pParent)
					{
						jsmgr = pParent->GetJSMgr();
						if (jsmgr)
						{
							*vp = OBJECT_TO_JSVAL(jsmgr->GetGlobalObject());
						}
					}
					else
					{
						*vp = OBJECT_TO_JSVAL(obj);
					}
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				return JS_TRUE;

			case WINDOW_RETURNVALUE:
				//we do not support dialogs
				break;

			case WINDOW_SCREENLEFT:
				// tbd
				//*vp = INT_TO_JSVAL(pWin->mReal.wLeft);
				return JS_TRUE;

			case WINDOW_SCREENTOP:
				// tbd
				//*vp = INT_TO_JSVAL(pWin->mReal.wTop);
				return JS_TRUE;

			case WINDOW_WINDOW:
			case WINDOW_SELF:
				//why would we want a pointer to something we already have?
				*vp = OBJECT_TO_JSVAL(obj);
				break;

			case WINDOW_TOP:
				if (pDoc)
				{
				  #if WEBC_SUPPORT_FRAMES
					if (pDoc->IsQuarantined())
					{
						*vp = OBJECT_TO_JSVAL(obj);
						return (JS_TRUE);
					}
				  #endif // WEBC_SUPPORT_FRAMES

					pParent = pDoc->RootParent();
					if (pParent)
					{
						jsmgr = pParent->GetJSMgr();
						if (jsmgr)
						{
							*vp = OBJECT_TO_JSVAL(jsmgr->GetGlobalObject());
						}
					}
					else
					{
						*vp = JSVAL_NULL;
					}
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				return JS_TRUE;

			case WINDOW_DOCUMENT:
#if (INCLUDE_DOCUMENT)
				*vp = OBJECT_TO_JSVAL(pDoc->GetJSObject());
				return JS_TRUE;
#endif

#if (WEBC_SUPPORT_AJAX)
			case WINDOW_XMLHTTPREQUEST:
				// return an instance of a XMLHTTLRequestFactory object
				if(g_XMLHTTPRequestFactorySingleton)
					*vp = OBJECT_TO_JSVAL(g_XMLHTTPRequestFactorySingleton);
				else
					*vp = OBJECT_TO_JSVAL(g_XMLHTTPRequestFactorySingleton = jhtml_XMLHttpRequestFactory_MakeNewObject(cx, WEBC_NULL, obj));

				break;
#endif

			default:
				break;
		}//end switch
	}

	return JS_TRUE;
}

/**
 * This method is called when setting a Property in Window
 */

JSBool jwindow_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	WebcJSDocumentContext *jsmgr = (WebcJSDocumentContext *) jhutil_GetPrivate(cx, obj);
	if (!jsmgr)
	{
		return JS_TRUE;
	}
	HTMLDocument *pDoc = jsmgr->GetDocument();
	HTMLBrowser *pBrowser = (pDoc)? pDoc->GetBrowser() : 0;

	switch (JSVAL_TO_INT(id))
	{
		case WINDOW_STATUS:
		case WINDOW_DEFAULTSTATUS:
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				WEBC_CHAR *str = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp));
				if (pBrowser)
				{
					pBrowser->SetDefaultStatus(str);
				}
				return JS_TRUE;
			}
			break;

		case WINDOW_DIALOGARGUMENTS:
			//we do not support dialogs
			break;

		case WINDOW_DIALOGHEIGHT:
			//we do not support dialogs
			break;

		case WINDOW_DIALOGLEFT:
			//we do not support dialogs
			break;

		case WINDOW_DIALOGTOP:
			//we do not support dialogs
			break;

		case WINDOW_DIALOGWIDTH:
			//we do not support dialogs
			break;

		case WINDOW_FRAMEELEMENT:
			break;

		case WINDOW_LENGTH:
			//we cannot set the length
			break;

		case WINDOW_NAME:
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				if (pDoc)
				{
				  #if (WEBC_SUPPORT_FRAMES)
				  	if (pDoc->GetFrame())
				  	{
						pDoc->GetFrame()->SetName(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
					}
					else
				  #endif // WEBC_SUPPORT_FRAMES
				  	{
						pDoc->SetName(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
						// also change the browser name to match so that window.open calls can find it
						pBrowser->ChangeName(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
					}
				}
				return JS_TRUE;
			}
			break;

		case WINDOW_OFFSCREENBUFFERING:
			//we cannot set the offscreen buffering
			break;

		case WINDOW_ONLOAD:
		{
			HTMLBody *pBody = pDoc->Body();
			JSFunction * fn = JS_ValueToFunction (cx, *vp);
			WEBC_CHAR *pEventHandlerText = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp));
			if (fn && pBody)
			{
#if (WEBC_INCLUDE_SEPT2010_BUGFIXES)
				JS_SetParent(cx, JS_GetFunctionObject(fn), obj);  // 10/05/2010 add missing JS_SetParent call.
#endif
				pBody->AddJSEventHandler(HTML_EVENT_LOAD, "onload", fn, pEventHandlerText, pDoc);
			}
			break;
		}
		case WINDOW_SELF:
        {
            if (jsmgr->mErrorFunction)
            {
                int argc = 3;
                jsval argv[3];
                WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
// function report_error(msg, url, line)
			    argv[0] = STRING_TO_JSVAL(JS_InternString(cx, "Setting Self"));
			    argv[1] = STRING_TO_JSVAL(JS_InternString(cx, "URL"));
			    argv[2] = STRING_TO_JSVAL(JS_InternString(cx, "LINE"));
                JS_CallFunction(cx, jsscr->GetParentObject(), jsmgr->mErrorFunction, argc, &argv[0], vp);
            }
            return JS_TRUE;
        }
        case WINDOW_ONERROR:
		{
			// Store user assigned error function
			jsmgr->mErrorFunction = JS_ValueToFunction (cx, *vp);
			break;
		}

		case WINDOW_OPENER:
			//we don't keep track of who opened the web browser window.
			break;

		case WINDOW_PARENT:
			//this is not supported by us
			break;

		case WINDOW_RETURNVALUE:
			//we do not support modal dialogs
			break;

		case WINDOW_SCREENLEFT:
			if (JSVAL_IS_INT(*vp))
			{
				int32 left = 0;
				if (JS_ValueToInt32(cx, *vp, &left) == JS_TRUE)
				{
					// tbd
					//pWin->mReal.wLeft = (short)left;
					//pWin->Refresh();
					return JS_TRUE;
				}
			}
			break;

		case WINDOW_SCREENTOP:
			if (JSVAL_IS_INT(*vp))
			{
				int32 top = 0;
				if (JS_ValueToInt32(cx, *vp, &top) == JS_TRUE)
				{
					// tbd
					//pWin->mReal.wTop = (short)top;
					//pWin->Refresh();
					return JS_TRUE;
				}
			}
			break;

		default:
			if (JSVAL_IS_STRING(id) &&
				tc_stricmp("location", JS_GetStringBytes(JS_ValueToString(cx, id)))==0)
			{
				WEBC_CHAR *pUrlStr = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp));
				if (pDoc)
				{
					jutils_LoadDocumentUrl(cx, pDoc, pUrlStr);
				}
			}
	}//end switch

	return jutils_CheckContextState(cx);
}



JSBool jwindow_attachEvent(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSDocumentContext *jsmgr = (WebcJSDocumentContext *) jhutil_GetPrivate(cx, obj);
	if (jsmgr)
	{
		HTMLDocument* doc = jsmgr->GetDocument();
		if (doc)
		{
			HTMLBody* body = doc->Body();
			if (body)
			{
				return jhtml_element_attachEvent(cx, body->CreateJSObject(), argc, argv, rval);
			}
		}
	}
	return JS_TRUE;
}

/**
 * blur() - This method shifts focus off the current window
 *
 * params - none
 * return - void
 */
JSBool jwindow_blur(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSDocumentContext *jsmgr = (WebcJSDocumentContext *) jhutil_GetPrivate(cx, obj);
	HTMLDocument *pDoc = (jsmgr)? jsmgr->GetDocument() : 0;
	if (pDoc && pDoc->Body())
	{
		HTMLEvent e;
		e.type = HTML_EVENT_UNFOCUS;
		pDoc->Body()->Event(&e);
	}
	return JS_TRUE;
}



extern "C" int _jwin_callFunction(void* data, long err);

// Helper function for the timer-related functions
int _jwin_callFunction(void* data, long err)
{
	WebcJSFunction *pFunc = (WebcJSFunction *) data;

	if (pFunc)
	{
		jsval rval;
		pFunc->ExecuteFunction(&rval, 0, 0);
	}

	return (0);
}

int _jwin_abortTimeout(WEBC_PFBYTE data)
{
	WebcJSFunction *pFunc = (WebcJSFunction *) data;

	if (pFunc)
	{
		pFunc->Destroy();
	}

	WEBC_ASSERT(giNumTimeouts > 0);
	giNumTimeouts--;

	return (0);

}

/**
 * This method clears an interval event
 *
 * @param id the id of the interval to clear
 */
JSBool jwindow_clearTimeout(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSDocumentContext *jsmgr = (WebcJSDocumentContext *) jhutil_GetPrivate(cx, obj);
	if (jsmgr)
	{
		HTMLBrowser* browser = jsmgr->GetDocument()->GetBrowser();
		if (browser)
		{
			int32 id = -1;
			if (argc > 0 && (JS_ValueToInt32(cx, argv[0], &id) == JS_TRUE))
			{
				WEBC_UINT16 timeoutId = (WEBC_UINT16) id;
				browser->TimeoutManager()->CancelTimeout(_matchId, &timeoutId, WEBC_FALSE);
			}
		}
	}
	*rval = JSVAL_VOID;
	return JS_TRUE;
}

JSBool jwindow_detachEvent(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSDocumentContext *jsmgr = (WebcJSDocumentContext*) jhutil_GetPrivate(cx, obj);
	if (jsmgr)
	{
		HTMLDocument* doc = jsmgr->GetDocument();
		if (doc)
		{
			HTMLBody* body = doc->Body();
			if (body)
			{
				return jhtml_element_detachEvent(cx, body->CreateJSObject(), argc, argv, rval);
			}
		}
	}
	return (JS_TRUE);
}

JSBool jwindow_execScript(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	//this is not supported
	//
	//this method is used to call code in other scripting languages, this should be implemented
	//if we ever support other script languages
	return JS_FALSE;
}

/**
 * focus - This method sets the focus on the current window
 *
 * params - none
 * return - void
 */
JSBool jwindow_focus(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSDocumentContext *jsmgr = (WebcJSDocumentContext *) jhutil_GetPrivate(cx, obj);
	HTMLDocument *pDoc = (jsmgr)? jsmgr->GetDocument() : 0;

	if (pDoc && pDoc->Body())
	{
		pDoc->Body()->Focus();
	}

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

/**
 * navigate - This method loads the url in the current window
 *
 * params - string url - the url to load
 * return - void
 */

JSBool jwindow_navigate(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (argc > 0 && JSVAL_IS_STRING(argv[0]))
	{
		WebcJSDocumentContext *jsmgr = (WebcJSDocumentContext *) jhutil_GetPrivate(cx, obj);
		HTMLDocument *pDoc = (jsmgr)? jsmgr->GetDocument() : 0;

		WEBC_CHAR *url_str = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, argv[0]));

		if (url_str && pDoc)
		{
			// save our current page into the history list
			jutils_LoadDocumentUrl(cx, pDoc, url_str);
		}
	}
	return jutils_CheckContextState(cx);
}

// open(url, name, features, replace)
//WEBC_CHAR * string

enum OpenFeatures
{
    /* Inters */
	FEATURE_LEFT = 0x0,
	FEATURE_TOP,
	FEATURE_HEIGHT,
	FEATURE_WIDTH,
    /* Booleans */
	FEATURE_LOCATION,
	FEATURE_MENUBAR,
	FEATURE_RESIZABLE,
	FEATURE_SCROLLARS,
	FEATURE_STATUS,
	FEATURE_TOOLBAR,
	FEATURE_NUMFEATURES
};

WEBC_READONLY char * gpFeatureValues[] = { "left", "top", "height", "width", "location", "menubar", "resizable",
"scrollbars", "status", "toolbar"};
extern "C" int _StringListItemCompareAscii(void *item, void *list, int index);

// char* jutils_ParseNameValuePair(char *pStr, char *pName, char *pValue)

// << EDIT 20080120 - added jwindow_moveTo() implement window.moveTo
JSBool jwindow_moveByorTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval, WEBC_BOOL is_absolute)
{
	if (argc == 2 && JSVAL_IS_INT(argv[0]) && JSVAL_IS_INT(argv[1]))
	{
		WebcJSDocumentContext *jsmgr = (WebcJSDocumentContext *) jhutil_GetPrivate(cx, obj);
		HTMLDocument *pDoc = (jsmgr)? jsmgr->GetDocument() : 0;

	    int x, y;
	    x = JSVAL_TO_INT(argv[0]);
	    y = JSVAL_TO_INT(argv[1]);
	    DisplayManager *pDisp = pDoc->GetDisplayManager();
	    WebGraphics *pGc = pDisp->GetGraphics();
	    pGc->MoveByorTo(x, y, is_absolute);  /* Absolute rather then delta */
        *rval = JSVAL_VOID;
	    return(JS_TRUE);
    }
    return(JS_FALSE);
}

JSBool jwindow_moveTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    return jwindow_moveByorTo(cx, obj, argc, argv, rval, WEBC_TRUE);
}
JSBool jwindow_moveBy(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    return jwindow_moveByorTo(cx, obj, argc, argv, rval, WEBC_FALSE);
}


// << EDIT 20080120 - added jwindow_close() implement window.close
JSBool jwindow_close(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSDocumentContext *jsmgr = (WebcJSDocumentContext *) jhutil_GetPrivate(cx, obj);
    HTMLDocument* doc = (jsmgr)? jsmgr->GetDocument() : 0;
	HTMLBrowser* browser = (doc)? doc->GetBrowser() : 0;

	if (browser)
	{ // Destroy the browser.. it kills the window
        webc_DestroyBrowser((HBROWSER_HANDLE) browser, 0);
// << EDIT 20080120 - BUGBUG Do we release the JS object now or does GC
	    *rval = JSVAL_VOID;
        return(JS_TRUE);
	}
    return(JS_FALSE);
}

// << EDIT 20080120 - added jwindow_open(url, name, featurelist, replace) implement window.open
JSBool jwindow_open(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	OpenFeatures which_feature = FEATURE_NUMFEATURES;
    JSBool is_replace = JS_FALSE;

	//the arguments to window.open are optional
    // Get the first argument, the URL
	char *url_str = NULL;
	if((argc >= 1) && JSVAL_IS_STRING(argv[0]))
		url_str = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
	else
		url_str = "";

        // get browser (window) name
	char *name_str = NULL;
	if((argc >= 2) && JSVAL_IS_STRING(argv[1]))
		name_str = JS_GetStringBytes(JS_ValueToString(cx, argv[1]));
	else
		name_str = "";

	// Get the featurelist
	char *features_str = NULL;
	if((argc >= 3) && JSVAL_IS_STRING(argv[2]))
		features_str = JS_GetStringBytes(JS_ValueToString(cx, argv[2]));
	else
		features_str = "";

    // Get the last argument, isreplace
	if (argc == 4)
	    if (!JS_ValueToBoolean(cx, argv[3], &is_replace))
	        is_replace = JS_FALSE;

	WebcJSDocumentContext *jsmgr = (WebcJSDocumentContext *) jhutil_GetPrivate(cx, obj);
	HTMLDocument *pDoc = (jsmgr)? jsmgr->GetDocument() : 0;

	// Clone the graphics abstraction layer function pointers so we know who to call
	HTMLBrowserConfig config, docConfig;
    rtp_memset(&config, 0, sizeof(config));
    rtp_memset(&docConfig, 0, sizeof(docConfig));
	pDoc->GetBrowser()->GetConfig(&docConfig);
	config.graphics = docConfig.graphics;
	char Name[100];
	char Value[100];

    while(features_str)
    {
	    Name[0] = Value[0] = 0;
	    features_str =  jutils_ParseNameValuePair(features_str, &Name[0], &Value[0]);
        if (Name[0] && Value[0])
        {
            int value = 0;
            which_feature = (OpenFeatures) ebs_LinearSearch(&Name[0], gpFeatureValues, FEATURE_NUMFEATURES, _StringListItemCompareAscii);

            if (which_feature >= FEATURE_LOCATION)
            {
                if (!jutils_ParseBoolVal(&Value[0], &value))
                    value = 0;
                switch (which_feature) {
                    case FEATURE_LOCATION: config.WindowConfig.has_location = (WEBC_BOOL) value; break;
                    case FEATURE_MENUBAR:   config.WindowConfig.has_menubar= (WEBC_BOOL) value; break;
                    case FEATURE_RESIZABLE:
						if (value)
							config.WindowConfig.WindowFlags |= WF_RESIZABLE;
						else
							config.WindowConfig.WindowFlags &= ~WF_RESIZABLE;
						break;
                    case FEATURE_SCROLLARS:
						if (value)
							config.WindowConfig.WindowFlags &= ~WF_DISABLESCROLLBARS;
						else
							config.WindowConfig.WindowFlags |= WF_DISABLESCROLLBARS;
						break;
                    case FEATURE_STATUS: config.WindowConfig.has_status= (WEBC_BOOL) value; break;
                    case FEATURE_TOOLBAR: config.WindowConfig.has_toolbar= (WEBC_BOOL) value; break;
                    default:
                        break; /* Put diags here */
                };
            }
            else
            {
                value = rtp_atoi(&Value[0]);
                switch (which_feature) {
                    case FEATURE_LEFT: config.WindowConfig.left = value; break;
                    case FEATURE_TOP: config.WindowConfig.top = value; break;
                    case FEATURE_HEIGHT: config.WindowConfig.height = value; break;
                    case FEATURE_WIDTH: config.WindowConfig.width = value; break;
                    default:
                        break; /* Put diags here */
                };
            }
        }
    }

    HTMLBrowser *pBrowser = 0;
	// AK_TODO this call doesn't work in the case where the script has updated the name of the
	// browser window by setting top.name. This is because webc_FindBrowserByNameAscii matches
	// against it's own array of names, not that which is in the HTMLDocument object
    // Find an existing browser (window) by name or create one
    pBrowser = (HTMLBrowser *) webc_FindBrowserByNameAscii(name_str);
    if (!pBrowser)
    {
        pBrowser = (HTMLBrowser *) webc_CreateBrowser(&config,  WEBC_TRUE, name_str, "");
    }

    // Start a new browser in a new window
    // config contains window build instructions
    // url_str contains the url to load (can be javascript: ) if any.
    if (pBrowser)
    {
        if (pBrowser->GetDocument())
        {
            webc_BrowserLoadUrl((HBROWSER_HANDLE) pBrowser, url_str);
			if(pBrowser)
			{
				HTMLDocument *pDoc = pBrowser->GetDocument();
				if(pDoc)
				{
					WebcJSDocumentContext *pMgr = pDoc->GetJSMgr();
					if(pMgr)
					{
						*rval = OBJECT_TO_JSVAL(pMgr->GetGlobalObject());
					}
				}
			}
        }
        return jutils_CheckContextState(cx);
    }

	return JS_FALSE;
}


/**
 * scrollBy() - This method scrolls the window by the specified x,y coordinate.
 *
 * params - int x - the amount to scroll on the x-axis
 *        - int y - the amount to scroll on the y-axis
 *
 * return - void
 */
JSBool jwindow_scrollBy(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	int32 x,y;

	if (argc > 1 &&
	    JS_ValueToInt32(cx, argv[0], &x) == JS_TRUE &&
	    JS_ValueToInt32(cx, argv[1], &y) == JS_TRUE)
	{
		WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
		if (jsscr)
		{
			WebcJSDocumentContext *jsmgr = jsscr->GetJSMgr();
			if (jsmgr)
			{
				HTMLDocument* doc = jsmgr->GetDocument();
				if (doc)
				{
					doc->ScrollBy(x,y);
				}
			}
		}
	}

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

/**
 * scrollTo() - This method scrolls the window to the specified x,y coordinate.
 *            It does the same thing as the scroll function.
 *
 * params - int x - the x-axis coordinate to scroll to
 *        - int y - the y-axis coordinate to scroll to
 *
 * return - void
 */
JSBool jwindow_scrollTo(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	int32 x,y;

	if (argc > 1 &&
	    JS_ValueToInt32(cx, argv[0], &x) == JS_TRUE &&
	    JS_ValueToInt32(cx, argv[1], &y) == JS_TRUE)
	{
		WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
		if (jsscr)
		{
			WebcJSDocumentContext *jsmgr = jsscr->GetJSMgr();
			if (jsmgr)
			{
				HTMLDocument* doc = jsmgr->GetDocument();
				if (doc)
				{
					doc->ScrollTo(x,y);
				}
			}
		}
	}

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

JSBool jwindow_setActive(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	//this is not supported
	return JS_FALSE;
}

static JSBool _setTimeout(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval, int loopCount)
{

	if (giNumTimeouts < WEBC_JWINDOW_MAX_NUM_TIMEOUTS)
	{
		WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
		if (jsscr)
		{
			WebcJSDocumentContext* jsmgr = jsscr->GetJSMgr();
			if (jsmgr)
			{
				HTMLDocument* doc = jsmgr->GetDocument();
				if (doc)
				{
					HTMLBrowser* browser = doc->GetBrowser();
					if (browser)
					{
						JSFunction* pJFunc = 0;
						if (argc > 1 && JSVAL_IS_INT(argv[1]))
						{
							int32 delay = 0;
							int id = -1;
							if (JS_ValueToInt32(cx, argv[1], &delay) == JS_TRUE)
							{
								WebcJSFunction *pFunc = 0;
								char fnname[20] = "_t1m30ut";
								tc_itoa(giTimeoutId++, fnname+8, 10); // generate a name for this function

								// First try to construct a function for the timeout
								if (JSVAL_IS_STRINGABLE(argv[0]))
								{
									WEBC_CHAR *src = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, argv[0]));
									WEBC_NEW_VERBOSE(pFunc, WebcJSFunction(jsmgr, obj, fnname, 0, 0, src, webc_strlen(src)),"WebcJSFunction");
								}
								else if (JS_ConvertValue(cx, argv[0], JSTYPE_FUNCTION, (jsval *) &pJFunc))
								{
									WEBC_NEW_VERBOSE(pFunc, WebcJSFunction(jsmgr, obj, fnname, pJFunc),"WebcJSFunction");
								}

								// Now set an OS timer to call it
								if (pFunc)
								{
									WebcJSScript *pScr = (WebcJSScript *) pFunc;
									id = browser->TimeoutManager()->SetTimeout (
											_jwin_callFunction,
											_jwin_abortTimeout,
											pScr,
											(WEBC_INT32) delay,
											loopCount
										);

									if (id == WEBC_TIMEOUT_INVALID)
									{
										pFunc->Destroy();
									}
									else
									{
										giNumTimeouts++;
									}
								}
								else
								{
									id = WEBC_TIMEOUT_INVALID;
								}
							}
							*rval = INT_TO_JSVAL(id);
						}
					}
				}
			}
		}
	}

	return JS_TRUE;
}

JSBool jwindow_setTimeout(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	return _setTimeout(cx, obj, argc, argv, rval, 0);
}

JSBool jwindow_setInterval(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	return _setTimeout(cx, obj, argc, argv, rval, WEBC_LOOP_FOREVER);
}

JSBool jwindow_back(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSDocumentContext* jsmgr = (WebcJSDocumentContext *) jhutil_GetPrivate(cx, obj);
	HTMLDocument* doc = (jsmgr)? jsmgr->GetDocument() : 0;
	HTMLBrowser* browser = (doc)? doc->GetBrowser() : 0;

	if (browser)
	{
		if (argc > 0 && JSVAL_IS_INT(argv[0]))
		{
			int32 index = -1;
			JS_ValueToInt32(cx, argv[0], &index);

			if (index > 0)
				index = -index;

			browser->Go(index);
		}
		else
		{
			browser->Go(WEBC_BROWSER_BACK);
		}
	}

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

JSBool jwindow_forward(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSDocumentContext *jsmgr = (WebcJSDocumentContext *) jhutil_GetPrivate(cx, obj);
	HTMLDocument* doc = (jsmgr)? jsmgr->GetDocument() : 0;
	HTMLBrowser* browser = (doc)? doc->GetBrowser() : 0;

	if (browser)
	{
		if (argc > 0 && JSVAL_IS_INT(argv[0]))
		{
			int32 index = -1;
			JS_ValueToInt32(cx, argv[0], &index);

			browser->Go(index);
		}
		else
		{
			browser->Go(WEBC_BROWSER_FORWARD);
		}
	}

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

JSBool jwindow_home(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSDocumentContext *jsmgr = (WebcJSDocumentContext *) jhutil_GetPrivate(cx, obj);
	HTMLBrowser *pBrowser = (jsmgr)? jsmgr->GetDocument()->GetBrowser() : 0;

	if (pBrowser)
	{
		pBrowser->Home();
	}

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

JSBool jwindow_stop(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSDocumentContext *jsmgr = (WebcJSDocumentContext *) jhutil_GetPrivate(cx, obj);
    HTMLDocument* doc = (jsmgr)? jsmgr->GetDocument() : 0;
	HTMLBrowser* browser = (doc)? doc->GetBrowser() : 0;

	if (browser)
	{
		browser->Stop(doc);
	}

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

/**
 * This JSClass is the javascript wrapper for the Window Class defined on the Microsoft Website
 */
JSClass JSWindow =
{
	"Window", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty,
	(JSPropertyOp) jutil_delProperty, (JSPropertyOp) jwindow_getProperty,
	(JSPropertyOp) jwindow_setProperty, (JSEnumerateOp) jutil_enumerate,
	(JSResolveOp) jwindow_resolve, (JSConvertOp) jutil_convert,
	(JSFinalizeOp) jutil_finalize
};


JSPropertySpec jwindow_props[] =
{
	{"closed",             WINDOW_CLOSED,             JSPROP_READONLY | JSPROP_ENUMERATE},
	{"defaultStatus",      WINDOW_DEFAULTSTATUS,      JSPROP_ENUMERATE},
	{"frameElement",       WINDOW_FRAMEELEMENT,       JSPROP_READONLY | JSPROP_ENUMERATE},
	{"length",             WINDOW_LENGTH,             JSPROP_READONLY | JSPROP_ENUMERATE},
	{"name",               WINDOW_NAME,               JSPROP_ENUMERATE},
	{"offScreenBuffering", WINDOW_OFFSCREENBUFFERING, JSPROP_READONLY | JSPROP_ENUMERATE},
	{"onload",             WINDOW_ONLOAD,             JSPROP_ENUMERATE},
	{"opener",             WINDOW_OPENER,             JSPROP_READONLY | JSPROP_ENUMERATE},
	{"parent",             WINDOW_PARENT,             JSPROP_READONLY | JSPROP_ENUMERATE},
	{"screenLeft",         WINDOW_SCREENLEFT,         JSPROP_ENUMERATE},
	{"screenTop",          WINDOW_SCREENTOP,          JSPROP_ENUMERATE},
	{"self",               WINDOW_SELF,               JSPROP_ENUMERATE },
	{"status",             WINDOW_STATUS,             JSPROP_ENUMERATE},
	{"top",                WINDOW_TOP,                JSPROP_ENUMERATE |JSPROP_READONLY},
	{"window",             WINDOW_WINDOW,             JSPROP_READONLY},
	/* Experimental */
	{"onerror",            WINDOW_ONERROR,            JSPROP_ENUMERATE},
#if (INCLUDE_DOCUMENT)
	{"document",           WINDOW_DOCUMENT,           JSPROP_ENUMERATE|JSPROP_READONLY},
#endif
#if (WEBC_SUPPORT_AJAX)
	{"XMLHttpRequestX", WINDOW_XMLHTTPREQUEST, JSPROP_ENUMERATE|JSPROP_READONLY},
#endif
	{0}
};

JSPropertySpec* GetWindowPropertySpec()
{
	return jwindow_props;
}
/* Go to wjdialogs.cpp for these functions */
JSBool jwindow_alert(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool jwindow_dbprint(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool jwindow_dblog(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool jwindow_confirm(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);
JSBool jwindow_prompt(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval);

JSFunctionSpec jwindow_methods[] =
{
	{"alert",              (JSNative) jwindow_alert,              1},
	{"confirm",            (JSNative) jwindow_confirm,            1},
	{"prompt",             (JSNative) jwindow_prompt,             1},
	{"attachEvent",        (JSNative) jwindow_attachEvent,        0},
	{"back",               (JSNative) jwindow_back,               1},
	{"blur",               (JSNative) jwindow_blur,               0},
	{"clearInterval",      (JSNative) jwindow_clearTimeout,       0},
	{"clearTimeout",       (JSNative) jwindow_clearTimeout,       0},
	{"detachEvent",        (JSNative) jwindow_detachEvent,        0},
	{"focus",              (JSNative) jwindow_focus,              0},
	{"forward",            (JSNative) jwindow_forward,            1},
	{"home",               (JSNative) jwindow_home,               0},
	{"navigate",           (JSNative) jwindow_navigate,           1},
	{"scroll",             (JSNative) jwindow_scrollTo,           2},
	{"scrollBy",           (JSNative) jwindow_scrollBy,           2},
	{"scrollTo",           (JSNative) jwindow_scrollTo,           2},
	{"setInterval",        (JSNative) jwindow_setInterval,        0},
	{"setTimeout",         (JSNative) jwindow_setTimeout,         0},
	{"stop",               (JSNative) jwindow_stop,               0},
	{"open",               (JSNative) jwindow_open,               4},
	{"close",              (JSNative) jwindow_close,              0},
	{"moveTo",             (JSNative) jwindow_moveTo,             3},
	{"moveBy",             (JSNative) jwindow_moveBy,             3},
	{"dbprint",            (JSNative) jwindow_dbprint,            1},
	{"dblog",              (JSNative) jwindow_dblog,              1},
	{0}
};

static JSBool JWindow(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	return JS_TRUE;
}

JSClass *getJWindow()
{
	return &JSWindow;
}

JSObject *jwindow_Init(JSContext *cx, JSObject *proto)
{
	JSObject *win = JS_NewObject(cx, getJWindow(), proto, WEBC_NULL);
	if (win)
	{
  		JS_InitStandardClasses(cx, win);

		JS_DefineProperties(cx, win, jwindow_props);
		JS_DefineFunctions(cx, win, jwindow_methods);
	}

	return win;
}

#endif //(WEBC_SUPPORT_JSCRIPT)
