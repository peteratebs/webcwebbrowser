/*
|  JHFRSET.CPP -
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
#include "jhfrset.h"
#include "jhutils.h"
#if (WEBC_SUPPORT_JSCRIPT)
#if (WEBC_SUPPORT_FRAMES)
#include "hframset.hpp"
#include "wjscript.hpp"
#include "htmldoc.hpp"
#include "objref.cpp"
#include "webcmem.h"
#include "jhelem.h"
#include "wjsdebug.h"

//set the unique ids for the properties of the frameset class
enum jhtml_frameset_tinyid
{
	HTMLFRAMESETELEMENT_COLS=-1,
	HTMLFRAMESETELEMENT_ROWS=-2
};

/**
 * Call this method when getting a Property from HTMLFramesetElement
 */
static JSBool jhtml_frameset_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLFrameSet *pFrameset = (pElem)? (HTMLFrameSet *) pElem : 0;

	if (!pFrameset)
	{
		return JS_TRUE;
	}

	switch (JSVAL_TO_INT(id))
	{
		case HTMLFRAMESETELEMENT_COLS:
			*vp = INT_TO_JSVAL(pFrameset->NumCols());
			return JS_TRUE;

		case HTMLFRAMESETELEMENT_ROWS:
			*vp = INT_TO_JSVAL(pFrameset->NumRows());
			return JS_TRUE;
	}//end switch
	return JS_TRUE;
}

/**
 * This method is called when setting a Property in HTMLFrameSet
 */
static JSBool jhtml_frameset_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLFrameSet *pFrameset = (pElem)? (HTMLFrameSet *) pElem : 0;

	if (!pFrameset)
	{
		return JS_TRUE;
	}

	switch (JSVAL_TO_INT(id))
	{
		case HTMLFRAMESETELEMENT_COLS:
			if (JSVAL_IS_INT(*vp))
			{
				int32 c = 0;
				if (JS_ValueToInt32(cx, *vp, &c) == JS_TRUE)
				{
					pFrameset->SetNumCols((WEBC_UINT16)c);
					pFrameset->Update(0,0);
					return JS_TRUE;
				}
			}
			break;

		case HTMLFRAMESETELEMENT_ROWS:
			if (JSVAL_IS_INT(*vp))
			{
				int32 r = 0;
				if (JS_ValueToInt32(cx, *vp, &r) == JS_TRUE)
				{
					pFrameset->SetNumRows((WEBC_UINT16)r);
					pFrameset->Update(0,0);
					return JS_TRUE;
				}
			}
			break;
	}//end switch
	return JS_TRUE;
}

/**
 * This JSClass is the javascript wrapper for the HTMLFrameset class defined in the DOM Level 1
 */
JSClass JSHtmlFramesetElement =
{
	"HTMLFrameSetElement", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty,
	(JSPropertyOp) jutil_delProperty, (JSPropertyOp) jhtml_frameset_getProperty,
	(JSPropertyOp) jhtml_frameset_setProperty, (JSEnumerateOp) jutil_enumerate,
	(JSResolveOp) jhtml_element_resolve, (JSConvertOp) jutil_convert,
	(JSFinalizeOp) jutil_finalize
};



JSPropertySpec jhtml_frameset_props[] = {
	{"cols", HTMLFRAMESETELEMENT_COLS,          JSPROP_ENUMERATE},
	{"rows", HTMLFRAMESETELEMENT_ROWS,          JSPROP_ENUMERATE},
	{0}
};

JSFunctionSpec jhtml_frameset_methods[] =
{
	{0}
};

static JSBool JHTMLFrameset(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	HTMLFrameSet *pFrameSet;

	if (jsmgr)
	{
		HTMLTagParse tag[1];
		HTML_InitTagParse(tag, HTML_TAG_FRAMESET);

		WEBC_NEW(pFrameSet, HTMLFrameSet(tag, jsmgr->GetDocument()));
		if (pFrameSet)
		{
			pFrameSet->SetJSObject(obj);
			pFrameSet->SetTagProperties(tag, jsmgr->GetDocument());
			jsmgr->AddNewElement(pFrameSet);
		}
	}
	return JS_TRUE;
}

JSClass *getHtmlFramesetElement()
{
	return &JSHtmlFramesetElement;
}

JSObject *jhtml_frameset_Init(JSContext *cx, JSObject *proto)
{
	//initialize the HTMLFramesetElement Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSHtmlFramesetElement,
    				(JSNative) JHTMLFrameset, 0,
    				jhtml_frameset_props, jhtml_frameset_methods,
    				WEBC_NULL, WEBC_NULL);
}

#endif // (WEBC_SUPPORT_FRAMES)
#endif //(WEBC_SUPPORT_JSCRIPT)
