/*
|  JHFRAME.CPP -
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
#include "jhframe.h"
#include "jhutils.h"
#if (WEBC_SUPPORT_JSCRIPT)
#if (WEBC_SUPPORT_FRAMES)
#include "hframe.hpp"
#include "wjscript.hpp"
#include "htmldoc.hpp"
#include "objref.cpp"
#include "webcmem.h"
#include "jhelem.h"
#include "jhinput.h"
#include "wjsdebug.h"

//set the unique ids for the properties of the frame class
enum jhtml_frame_tinyid
{
	HTMLFRAMEELEMENT_FRAMEBORDER  = -1,
	HTMLFRAMEELEMENT_LOCATION     = -2,
	HTMLFRAMEELEMENT_LONGDESC     = -3,
	HTMLFRAMEELEMENT_MARGINHEIGHT = -4,
	HTMLFRAMEELEMENT_MARGINWIDTH  = -5,
	HTMLFRAMEELEMENT_NAME         = -6,
	HTMLFRAMEELEMENT_NORESIZE     = -7,
	HTMLFRAMEELEMENT_SCROLLING    = -8,
	HTMLFRAMEELEMENT_SRC          = -9
};

/**
 * Call this method when getting a Property from HTMLFrameElement
 */
static JSBool jhtml_frame_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLFrame *pFrame = (pElem)? (HTMLFrame *) pElem : 0;

	if (!pFrame || (pFrame->Type() != HTML_FRAME_ELEMENT))
		return JS_TRUE;

	switch (JSVAL_TO_INT(id))
	{
		case HTMLFRAMEELEMENT_FRAMEBORDER:
		{
			*vp = INT_TO_JSVAL((int)pFrame->Frameborder());
			return JS_TRUE;
		}
		case HTMLFRAMEELEMENT_LONGDESC:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pFrame->LongDesc()));
			return JS_TRUE;
		}
		case HTMLFRAMEELEMENT_MARGINHEIGHT:
		{
			*vp = INT_TO_JSVAL(pFrame->GetFrameMarginHeight());
			return JS_TRUE;
		}
		case HTMLFRAMEELEMENT_MARGINWIDTH:
		{
			*vp = INT_TO_JSVAL(pFrame->GetFrameMarginWidth());
			return JS_TRUE;
		}
		case HTMLFRAMEELEMENT_NAME:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pFrame->Name()));
			return JS_TRUE;
		}
		case HTMLFRAMEELEMENT_NORESIZE:
		{
			*vp = BOOLEAN_TO_JSVAL(false); // stub
			return JS_TRUE;
		}
		case HTMLFRAMEELEMENT_SCROLLING:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pFrame->ScrollMode()));
			return JS_TRUE;
		}
		case HTMLFRAMEELEMENT_SRC:
		case HTMLFRAMEELEMENT_LOCATION:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pFrame->Src()));
			return JS_TRUE;
		}//end case
	}//end switch

	return JS_TRUE;
}

/**
 * This method is called when setting a Property in HTMLFrame
 */
static JSBool jhtml_frame_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLFrame *pFrame = (pElem)? (HTMLFrame *) pElem : 0;

	if (!pFrame || (pFrame->Type() != HTML_FRAME_ELEMENT))
		return JS_TRUE;

	switch (JSVAL_TO_INT(id))
	{
		case HTMLFRAMEELEMENT_FRAMEBORDER:
			if (JSVAL_IS_NUMBER(*vp))
			{
				int32 b = 1;
				JS_ValueToInt32(cx, *vp, &b);
				pFrame->SetBorder((WEBC_UINT8)b);
				pFrame->Update(0,0);
				return JS_TRUE;
			}
			break;

		case HTMLFRAMEELEMENT_LONGDESC:
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				pFrame->SetLongDesc(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
				return JS_TRUE;
			}
			break;

		case HTMLFRAMEELEMENT_MARGINHEIGHT:
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				pFrame->SetFrameMarginHeight(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
				return JS_TRUE;
			}
			break;

		case HTMLFRAMEELEMENT_MARGINWIDTH:
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				pFrame->SetFrameMarginWidth(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
				return JS_TRUE;
			}
			break;

		case HTMLFRAMEELEMENT_NAME:
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				pFrame->SetName(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
				return JS_TRUE;
			}
			break;

		case HTMLFRAMEELEMENT_NORESIZE:
			//this is not supported
			break;

		case HTMLFRAMEELEMENT_SCROLLING:
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				pFrame->SetScrollMode(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
				pFrame->Update(0,0);
				return JS_TRUE;
			}
			break;

		case HTMLFRAMEELEMENT_SRC:
		case HTMLFRAMEELEMENT_LOCATION:
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				pFrame->SetSrc(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
				pFrame->Update(0,0);
			}
			break;

	}//end switch

	return jutils_CheckContextState(cx);
}

/**
 * This JSClass is the javascript wrapper for the HTMLFrame class defined in the DOM Level 1
 */
JSClass JSHtmlFrameElement =
{
	"HTMLFrameElement", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty,
	(JSPropertyOp) jutil_delProperty, (JSPropertyOp) jhtml_frame_getProperty,
	(JSPropertyOp) jhtml_frame_setProperty, (JSEnumerateOp) jutil_enumerate,
	(JSResolveOp) jhtml_element_resolve, (JSConvertOp) jutil_convert,
	(JSFinalizeOp) jutil_finalize
};



JSPropertySpec jhtml_frame_props[] = {
	{"frameBorder",  HTMLFRAMEELEMENT_FRAMEBORDER  ,JSPROP_ENUMERATE},
	{"location",     HTMLFRAMEELEMENT_LOCATION     ,JSPROP_ENUMERATE},
	{"longDesc",     HTMLFRAMEELEMENT_LONGDESC     ,JSPROP_ENUMERATE},
	{"marginHeight", HTMLFRAMEELEMENT_MARGINHEIGHT ,JSPROP_ENUMERATE},
	{"marginWidth",  HTMLFRAMEELEMENT_MARGINWIDTH  ,JSPROP_ENUMERATE},
	{"name",         HTMLFRAMEELEMENT_NAME         ,JSPROP_ENUMERATE | JSPROP_READONLY},
	{"noResize",     HTMLFRAMEELEMENT_NORESIZE     ,JSPROP_ENUMERATE},
	{"scrolling",    HTMLFRAMEELEMENT_SCROLLING    ,JSPROP_ENUMERATE},
	{"src",          HTMLFRAMEELEMENT_SRC          ,JSPROP_ENUMERATE},
	{0}
};

JSFunctionSpec jhtml_frame_methods[] =
{
	{0}
};

static JSBool JHTMLFrame(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	HTMLFrame *pFrame;

	if (jsmgr)
	{
		HTMLTagParse tag[1];
		HTML_InitTagParse(tag, HTML_TAG_FRAME);

		WEBC_NEW(pFrame, HTMLFrame(tag, jsmgr->GetDocument()));
		if (pFrame)
		{
			pFrame->SetJSObject(obj);
			pFrame->SetTagProperties(tag, jsmgr->GetDocument());
			jsmgr->AddNewElement(pFrame);
		}
	}
	return JS_TRUE;
}

JSClass *getHtmlFrameElement()
{
	return &JSHtmlFrameElement;
}

JSObject *jhtml_frame_Init(JSContext *cx, JSObject *proto)
{
	//initialize the HTMLFrameElement Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSHtmlFrameElement,
    				(JSNative) JHTMLFrame, 0,
    				jhtml_frame_props, jhtml_frame_methods,
    				WEBC_NULL, WEBC_NULL);
}

#endif // (WEBC_SUPPORT_FRAMES)
#endif //(WEBC_SUPPORT_JSCRIPT)
