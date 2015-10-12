/*
|  JHXMLHttpRequest.CPP -
|
|  EBS - WebC
|
|  Copyright EBS Inc. , 2009
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
|
|  Module description:
|   [tbd]
*/
#include "webcjhconfig.h"
#include "jhXMLHttpRequest.h"
#include "jhutils.h"
#if (WEBC_SUPPORT_JSCRIPT)
#if (WEBC_SUPPORT_AJAX)

#include "hXMLHttpRequest.hpp"
#include "jhelem.h"
#include "htmldoc.hpp"
#include "wjscript.hpp"
#include "objref.cpp"
#include "webcmem.h"
#include "wjsdebug.h"
#include "jsParamUtils.h"
extern "C" {
#include "jsxml.h"
};

//set the unique ids for the properties of the element class
enum jhtml_XMLHttpRequest_tinyid
{
	XMLHTTPREQUEST_READYSTATE=-1, // Retrieves the current state of the request operation.
	XMLHTTPREQUEST_RESPONSEBODY=-2, // Retrieves the response body as an array of unsigned bytes.
	XMLHTTPREQUEST_RESPONSETEXT=-3, // Retrieves the response body as a string.
	XMLHTTPREQUEST_RESPONSEXML=-4, // Retrieves the response body as an XML Document Object Model (DOM) object.
	XMLHTTPREQUEST_STATUS=-5, // Retrieves the HTTP status code of the request.
	XMLHTTPREQUEST_STATUSTEXT=-6, // Retrieves the friendly HTTP status of the request.
	XMLHTTPREQUEST_TIMEOUT=-7, //   Gets or sets the time-out value.
	XMLHTTPREQUEST_ONREADYSTATECHANGE=-8 // state change event handler property
};

/**
 * Call this method when getting a Property from HTMLElement
 */
static JSBool jhtml_XMLHttpRequest_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	XMLHttpRequest *pAJAXObject = (XMLHttpRequest *) pElem;
	int iID = JSVAL_TO_INT(id);

	if (!pAJAXObject)
		return JS_TRUE;

	switch (JSVAL_TO_INT(id))
	{
		case XMLHTTPREQUEST_READYSTATE:
			{
				WEBC_INT state=0;
				pAJAXObject->get_readyState(&state);
				*vp = INT_TO_JSVAL(state);
			}
			break;
		case XMLHTTPREQUEST_RESPONSEBODY:
		case XMLHTTPREQUEST_RESPONSETEXT:
			{
				WEBC_CHAR *strXMLText;
				pAJAXObject->get_responseText(&strXMLText);
				*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, strXMLText));
			}
			break;
		case XMLHTTPREQUEST_RESPONSEXML:
			{
				JSObject *jsObj;
				WEBC_CHAR *strXMLText;
				jsval jsStrXMLText;
				//pAJAXObject->get_responseXML(&jsObj);
				pAJAXObject->get_responseText(&strXMLText);

				if(strXMLText)
				{
					//jsStrXMLText = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, strXMLText));
					// TODO need to deal with varying character sets
					jsStrXMLText = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, (char *)strXMLText));

					jsObj = js_ValueToXMLObject(cx, jsStrXMLText);

					*vp = OBJECT_TO_JSVAL(jsObj);
				}
			}
			break;
		case XMLHTTPREQUEST_STATUS:
			{
				WEBC_INT status=0;
				pAJAXObject->get_status(&status);
				*vp = INT_TO_JSVAL(status);
			}
			break;
		case XMLHTTPREQUEST_STATUSTEXT:
			break;
		case XMLHTTPREQUEST_TIMEOUT:
			break;
		case XMLHTTPREQUEST_ONREADYSTATECHANGE:
			break;
		}
	return JS_TRUE;
}

/**
 * This method is called when setting a Property in HTMLElement
 */
static JSBool jhtml_XMLHttpRequest_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	XMLHttpRequest *pAJAXObject = (XMLHttpRequest *) pElem;

	if (!pAJAXObject)
		return JS_TRUE;

	switch (JSVAL_TO_INT(id))
	{
		case XMLHTTPREQUEST_TIMEOUT:
		{
			int32 timeout=0;
			JS_ValueToInt32(cx,*vp,&timeout);
			pAJAXObject->put_timeout(timeout);
			break;
		}
		case XMLHTTPREQUEST_ONREADYSTATECHANGE:
		{
			JSFunction * fn = 0;
			JS_ConvertValue(cx, *vp, JSTYPE_FUNCTION, (jsval *) &fn );
			WEBC_CHAR *pEventHandlerText = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp));
			HTMLDocument *pDoc = pAJAXObject->GetDocument();
			if (fn && pDoc)
			{
				pAJAXObject->AddJSEventHandler(HTML_EVENT_ONREADYSTATECHANGE, "onreadystatechange", fn, pEventHandlerText, pDoc);
			}
			break;
		}

			break;

	}//end switch

	return jutils_CheckContextState(cx);
}

/**
 * This JSClass is the javascript wrapper for the HTMLElement class defined in the DOM Level 1
 */
JSClass JSHtmlXMLHttpRequestElement =
{
	"XMLHttpRequest", JSCLASS_HAS_PRIVATE, jutil_addProperty,
	jutil_delProperty, jhtml_XMLHttpRequest_getProperty,
	jhtml_XMLHttpRequest_setProperty, jutil_enumerate,
	jhtml_element_resolve, jutil_convert,
	jutil_finalize
};

JSBool jXMLHttpRequest_abort(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (JS_GetClass(obj) == &JSHtmlXMLHttpRequestElement)
	{
		HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
		XMLHttpRequest *pAJAXObject = (XMLHttpRequest *) pElem;

		if (!pElem)
		{
			return JS_TRUE;
		}
		pAJAXObject->abort();
	}

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

JSBool jXMLHttpRequest_getAllResponseHeaders(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (JS_GetClass(obj) == &JSHtmlXMLHttpRequestElement)
	{
		HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
		XMLHttpRequest *pAJAXObject = (XMLHttpRequest *) pElem;

		if (!pElem)
		{
			return JS_TRUE;
		}
		// TODO implement this
	}

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

JSBool jXMLHttpRequest_getResponseHeader(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	char *str = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));

	if (rtp_stricmp(str, "content-type") == 0)
	{
		*rval = STRING_TO_JSVAL(JS_InternString(cx, "xml"));
		return JS_TRUE;
	}
	if (JS_GetClass(obj) == &JSHtmlXMLHttpRequestElement)
	{
		HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
		XMLHttpRequest *pAJAXObject = (XMLHttpRequest *) pElem;

		if (!pElem)
		{
			return JS_TRUE;
		}
		// TODO implement this
	}

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

JSBool jXMLHttpRequest_open(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (JS_GetClass(obj) == &JSHtmlXMLHttpRequestElement)
	{
		HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
		XMLHttpRequest *pAJAXObject = (XMLHttpRequest *) pElem;

		WEBC_CHAR *strMethod=0, *strUrl=0, *strUser=0, *strPassword=0;
		jsVarUtil methodVar(cx), urlVar(cx), asyncVar(cx), userVar(cx), passwordVar(cx);
		WEBC_BOOL bAsync=WEBC_TRUE;

		if (!pElem)
		{
			return JS_TRUE;
		}

		if(argc > 1)
		{
			methodVar.setVal(argv[0]);
			strMethod = methodVar.toString();
			urlVar.setVal(argv[1]);
			strUrl = urlVar.toString();
				if(argc > 2)
				{
					asyncVar.setVal(argv[2]);
					bAsync = asyncVar.toBool();
					if(argc > 3)
					{
						userVar.setVal(argv[3]);
						strUser = userVar.toString();
						if(argc > 4)
						{
							passwordVar.setVal(argv[4]);
							strPassword = userVar.toString();
						}
					}
				}
			pAJAXObject->open(strMethod, strUrl, bAsync, strUser, strPassword);
		}
	}

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

JSBool jXMLHttpRequest_send(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (JS_GetClass(obj) == &JSHtmlXMLHttpRequestElement)
	{
		HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
		XMLHttpRequest *pAJAXObject = (XMLHttpRequest *) pElem;
		WEBC_CHAR *strBodyText=0;
		jsVarUtil bodyTextVar(cx);

		if (!pElem)
		{
			return JS_TRUE;
		}
		if(argc) //  > 1) // PVOPVO
		{
			if(JSVAL_IS_STRING(argv[0]))
			{
				bodyTextVar.setVal(argv[0]);
				strBodyText = bodyTextVar.toString();
				pAJAXObject->send(strBodyText);
			}
			else if(JSVAL_IS_OBJECT(argv[0]))
			{
				// TODO figure out how to send an XML object
			}
		}
		else
		{
			pAJAXObject->send((WEBC_CHAR *)WEBC_NULL);
		}
	}

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

JSBool jXMLHttpRequest_setRequestHeader(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (JS_GetClass(obj) == &JSHtmlXMLHttpRequestElement)
	{
		HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);

		if (!pElem)
		{
			return JS_TRUE;
		}
		// TODO implement this
	}

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

JSPropertySpec jhtml_XMLHttpRequest_props[] = {
	{"readyState",     XMLHTTPREQUEST_READYSTATE, JSPROP_ENUMERATE | JSPROP_READONLY},
	{"responseBody",   XMLHTTPREQUEST_RESPONSEBODY, JSPROP_ENUMERATE | JSPROP_READONLY},
	{"responseText",   XMLHTTPREQUEST_RESPONSETEXT, JSPROP_ENUMERATE | JSPROP_READONLY},
	{"responseXML",    XMLHTTPREQUEST_RESPONSEXML,JSPROP_ENUMERATE | JSPROP_READONLY},
	{"status",         XMLHTTPREQUEST_STATUS, JSPROP_ENUMERATE | JSPROP_READONLY},
	{"statusText",     XMLHTTPREQUEST_STATUSTEXT, JSPROP_ENUMERATE | JSPROP_READONLY},
	{"timeout",        XMLHTTPREQUEST_TIMEOUT, JSPROP_ENUMERATE},
	{"onreadystatechange", XMLHTTPREQUEST_ONREADYSTATECHANGE, JSPROP_ENUMERATE},
    {0}
};


JSFunctionSpec jhtml_XMLHttpRequest_methods[] =
{
	{"abort", jXMLHttpRequest_abort, 0}, //  Cancels the current HTTP request.
	{"getAllResponseHeaders", jXMLHttpRequest_getAllResponseHeaders, 0}, //  Returns the complete list of response headers.
	{"getResponseHeader", jXMLHttpRequest_getResponseHeader, 0}, //  Returns the specified response header.
	{"open", jXMLHttpRequest_open, 2}, //  Assigns method, destination URL, and other optional attributes of a pending request.
	{"send", jXMLHttpRequest_send, 0}, //  Sends an HTTP request to the server and receives a response.
	{"setRequestHeader", jXMLHttpRequest_setRequestHeader, 1}, //  Adds custom HTTP headers to the request.
	{0}
};

static JSBool JHTMLXMLHttpRequestElement(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	if (jsmgr)
	{
		XMLHttpRequest *pXMLHttpRequest;

		WEBC_NEW(pXMLHttpRequest, XMLHttpRequest(0, jsmgr->GetDocument(), obj));
		if (pXMLHttpRequest)
		{
			pXMLHttpRequest->SetJSObject(obj);
			jsmgr->AddNewElement(pXMLHttpRequest);
		}
	}
	return JS_TRUE;
}

JSClass *getHtmlXMLHttpRequestElement()
{
	return &JSHtmlXMLHttpRequestElement;
}

JSObject *jhtml_XMLHttpRequest_Init(JSContext *cx, JSObject *proto)
{
	//initialize the HTMLElement Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSHtmlXMLHttpRequestElement,
    				JHTMLXMLHttpRequestElement, 0,
    				jhtml_XMLHttpRequest_props, jhtml_XMLHttpRequest_methods,
    				WEBC_NULL, WEBC_NULL);
}

JSObject *jhtml_XMLHttpRequest_MakeNewObject(JSContext *cx, JSObject *proto, JSObject *parent)
{
	JSObject *retVal;
	retVal = JS_NewObject(cx, &JSHtmlXMLHttpRequestElement, proto, parent);

	if (retVal)
	{
		JS_DefineProperties(cx, retVal, jhtml_XMLHttpRequest_props);
		JS_DefineFunctions(cx, retVal, jhtml_XMLHttpRequest_methods);
	}

	return retVal;
}

#endif //WEBC_SUPPORT_AJAX
#endif //(WEBC_SUPPORT_JSCRIPT)
