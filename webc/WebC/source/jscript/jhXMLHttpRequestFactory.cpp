/*
|  JHXMLHttpRequestFactory.CPP -
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
#include "jhXMLHttpRequestFactory.h"
#include "jhXMLHttpRequest.h"
#include "jhutils.h"
#if (WEBC_SUPPORT_JSCRIPT)
#if (WEBC_SUPPORT_AJAX)

#include "jhelem.h"
#include "htmldoc.hpp"
#include "wjscript.hpp"
#include "objref.cpp"
#include "webcmem.h"
#include "wjsdebug.h"
#include "jsParamUtils.h"


static JSBool jhtml_XMLHTTPRequestFactory_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	return JS_TRUE;
}

static JSBool jhtml_XMLHTTPRequestFactory_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	return JS_TRUE;
}

/**
 * This JSClass is the javascript wrapper for the HTMLElement class defined in the DOM Level 1
 */
JSClass JSHtmlXMLHttpRequestFactoryElement =
{
	"IHTMLXMLHttpRequestFactory", JSCLASS_HAS_PRIVATE, jutil_addProperty,
	jutil_delProperty, jhtml_XMLHTTPRequestFactory_getProperty,
	jhtml_XMLHTTPRequestFactory_setProperty, jutil_enumerate,
	jhtml_element_resolve, jutil_convert,
	jutil_finalize
};

JSBool jXMLHttpRequestFactory_create(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	if (JS_GetClass(obj) == &JSHtmlXMLHttpRequestFactoryElement)
	{
		// return an instance of a XMLHTTLRequest object
		//*rval = OBJECT_TO_JSVAL(JS_ConstructObject(cx, getHtmlXMLHttpRequestElement(), WEBC_NULL, obj));
		//*rval = OBJECT_TO_JSVAL(JS_NewObject(cx, getHtmlXMLHttpRequestElement(), WEBC_NULL, obj));
		*rval = OBJECT_TO_JSVAL(jhtml_XMLHttpRequest_MakeNewObject(cx, WEBC_NULL, obj));
		//*rval = OBJECT_TO_JSVAL(jhtml_XMLHttpRequest_Init(cx, WEBC_NULL));
	}

	*rval = JSVAL_VOID;
	return JS_TRUE;
}


JSPropertySpec jhtml_XMLHttpRequestFactory_props[] = {
    {0}
};


JSFunctionSpec jhtml_XMLHttpRequestFactory_methods[] =
{
	{"create", jXMLHttpRequestFactory_create, 0}, //  creates a XMLHTTLRequest object.
	{0}
};

static JSBool JHTMLXMLHttpRequestFactoryElement(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	if (jsmgr)
	{
	}
	return JS_TRUE;
}

JSClass *getHtmlXMLHttpRequestFactoryElement()
{
	return &JSHtmlXMLHttpRequestFactoryElement;
}

JSObject *jhtml_XMLHttpRequestFactory_Init(JSContext *cx, JSObject *proto)
{
	//initialize the HTMLElement Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSHtmlXMLHttpRequestFactoryElement,
    				JHTMLXMLHttpRequestFactoryElement, 0,
    				jhtml_XMLHttpRequestFactory_props, jhtml_XMLHttpRequestFactory_methods,
    				WEBC_NULL, WEBC_NULL);
}

JSObject *jhtml_XMLHttpRequestFactory_MakeNewObject(JSContext *cx, JSObject *proto, JSObject *parent)
{
	JSObject *retVal;
	retVal = JS_NewObject(cx, &JSHtmlXMLHttpRequestFactoryElement, proto, parent);

	if (retVal)
	{
		JS_DefineProperties(cx, retVal, jhtml_XMLHttpRequestFactory_props);
		JS_DefineFunctions(cx, retVal, jhtml_XMLHttpRequestFactory_methods);
	}

	return retVal;
}


#endif //WEBC_SUPPORT_AJAX
#endif //(WEBC_SUPPORT_JSCRIPT)
