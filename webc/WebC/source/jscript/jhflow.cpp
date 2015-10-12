/*
|  JHFLOW.CPP -
|
|  EBS - WebC
|
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
|
|  Module description:
|   [tbd]
*/

#include "webcjhconfig.h"
#include "jhflow.h"
#include "jhutils.h"
#if (WEBC_SUPPORT_JSCRIPT)

#include "hflow.hpp"
#include "jhelem.h"
#include "wjsdebug.h"

//this class will be the defined class for htmldiv, htmlparagraph and htmlheading

//set the unique ids for the properties of the flow class
enum jhtml_flow_tinyid
{
	HTMLFLOWELEMENT_ALIGN=-1
};

/**
 * Call this method when getting a Property from HTMLFlowElement
 */
static JSBool jhtml_flow_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLFlow *pFlow = (pElem)? (HTMLFlow *) pElem : 0;

	if (!pFlow)
		return JS_TRUE;

	switch (JSVAL_TO_INT(id))
	{
		case HTMLFLOWELEMENT_ALIGN:
		{
			JSString *jsstr = 0;
			WEBC_UINT16 align = pFlow->GetChildTextAlign();
			switch (align)
			{
				case TU_ALIGN_LEFT:
					jsstr = JS_InternString(cx, "left");
					break;

				case TU_ALIGN_RIGHT:
					jsstr = JS_InternString(cx, "right");
					break;

				case TU_ALIGN_CENTER:
					jsstr = JS_InternString(cx, "center");
					break;
			}
			if (!jsstr)
			{
				jsstr = JS_InternString(cx, "undefined");
			}
			*vp = STRING_TO_JSVAL(jsstr);
			return JS_TRUE;
		}//end case
	}//end switch
	return JS_TRUE;
}

/**
 * This method is called when setting a Property in HTMLFlow
 */
static JSBool jhtml_flow_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);

	if (!pElem)
		return (JS_TRUE);

	switch (JSVAL_TO_INT(id))
	{
		case HTMLFLOWELEMENT_ALIGN:
		{
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				WEBC_CHAR *str = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp));
				pElem->SetProperty(HTML_ATTRIB_ALIGN, str);
				pElem->StyleUpdate();
				return JS_TRUE;
			}
		}//end case
	}//end switch
	return JS_TRUE;
}

/**
 * This JSClass is the javascript wrapper for the HTMLFlow class defined in the DOM Level 1
 */
JSClass JSHtmlFlowElement =
{
	"HTMLFlowElement", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty,
	(JSPropertyOp) jutil_delProperty, (JSPropertyOp) jhtml_flow_getProperty,
	(JSPropertyOp) jhtml_flow_setProperty, (JSEnumerateOp) jutil_enumerate,
	(JSResolveOp) jhtml_element_resolve, (JSConvertOp) jutil_convert,
	(JSFinalizeOp) jutil_finalize
};



JSPropertySpec jhtml_flow_props[] = {
	{"align", HTMLFLOWELEMENT_ALIGN, JSPROP_ENUMERATE},
	{0}
};

JSFunctionSpec jhtml_flow_methods[] =
{
	{0}
};

static JSBool JHTMLFlow(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	//Flow is not a tag just a base class WebC uses
	return JS_TRUE;
}

JSClass *getHtmlFlowElement()
{
	return &JSHtmlFlowElement;
}

JSObject *jhtml_flow_Init(JSContext *cx, JSObject *proto)
{
	//initialize the HTMLFlowElement Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSHtmlFlowElement,
    				(JSNative) JHTMLFlow, 0,
    				jhtml_flow_props, jhtml_flow_methods,
    				WEBC_NULL, WEBC_NULL);
}

#endif //(WEBC_SUPPORT_JSCRIPT)
