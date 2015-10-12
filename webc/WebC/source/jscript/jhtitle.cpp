/*
|  JHTITLE.CPP -
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
#include "jhtitle.h"
#include "jhutils.h"
#if (WEBC_SUPPORT_JSCRIPT)

#include "htmldoc.hpp"
#include "jhelem.h"
#include "wjsdebug.h"

//GMP this class is not supported by internet explorer


//set the unique ids for the properties of the htmltitleelement class
enum jhtml_title_tinyid
{
	HTMLTITLEELEMENT_TEXT=-1
};

/**
 * Call this method when getting a Property from HTMLTitleElement
 */
static JSBool jhtml_title_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLDocument *pDoc = (HTMLDocument *)jhutil_GetPrivate(cx, obj);
	if (!pDoc)
		return JS_TRUE;

	switch (JSVAL_TO_INT(id))
	{
		case HTMLTITLEELEMENT_TEXT:
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pDoc->GetTitle()));
			return JS_TRUE;
		default:
			break;
	}//end switch
	return JS_TRUE;
}

/**
 * This method is called when setting a Property in HTMLETitle
 */
static JSBool jhtml_title_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLDocument *pDoc = (HTMLDocument *)jhutil_GetPrivate(cx, obj);
	if (!pDoc)
		return JS_TRUE;

	switch (JSVAL_TO_INT(id))
	{
		case HTMLTITLEELEMENT_TEXT:
			pDoc->SetTitle(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
			return JS_TRUE;
		default:
			break;
	}//end switch
	return JS_TRUE;
}


/**
 * This JSClass is the javascript wrapper for the HTMLTitleElement class defined in the DOM Level 1
 */
JSClass JSHtmlTitleElement =
{
	"HTMLTitleElement", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty,
	(JSPropertyOp) jutil_delProperty, (JSPropertyOp) jhtml_title_getProperty,
	(JSPropertyOp) jhtml_title_setProperty, (JSEnumerateOp) jutil_enumerate,
	(JSResolveOp) jhtml_element_resolve, (JSConvertOp) jutil_convert,
	(JSFinalizeOp) jutil_finalize
};


JSPropertySpec jhtml_title_props[] = {
	{"text", HTMLTITLEELEMENT_TEXT, 0},
	{0}
};

JSFunctionSpec jhtml_title_methods[] =
{
	{0}
};

static JSBool JHTMLTitle(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	return JS_TRUE;
}

JSClass *getHtmlTitleElement()
{
	return &JSHtmlTitleElement;
}

JSObject *jhtml_title_Init(JSContext *cx, JSObject *proto)
{
	//initialize the HTMLTitleElement Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSHtmlTitleElement,
    				(JSNative) JHTMLTitle, 0,
    				jhtml_title_props, jhtml_title_methods,
    				WEBC_NULL, WEBC_NULL);
}


#endif //(WEBC_SUPPORT_JSCRIPT)
