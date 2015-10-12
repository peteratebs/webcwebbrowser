/*
|  JHIFRAME.CPP -
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
#include "jhiframe.h"
#include "jhutils.h"

#if (WEBC_SUPPORT_JSCRIPT && WEBC_SUPPORT_FRAMES && WEBC_SUPPORT_IFRAMES)

#include "hiframe.hpp"
#include "htmldoc.hpp"
#include "wjscript.hpp"
#include "objref.cpp"
#include "webcmem.h"
#include "jhelem.h"
#include "jhinput.h"
#include "wjsdebug.h"

//set the unique ids for the properties of the frame class
enum jhtml_iframe_tinyid
{
	HTMLIFRAMEELEMENT_ALIGN=-1,
	HTMLIFRAMEELEMENT_DOCUMENT=-2,
	HTMLIFRAMEELEMENT_FRAMEBORDER=-3,
	HTMLIFRAMEELEMENT_HEIGHT=-4,
	HTMLIFRAMEELEMENT_LONGDESC=-5,
	HTMLIFRAMEELEMENT_MARGINHEIGHT=-6,
	HTMLIFRAMEELEMENT_MARGINWIDTH=-7,
	HTMLIFRAMEELEMENT_NAME=-8,
	HTMLIFRAMEELEMENT_SCROLLING=-9,
	HTMLIFRAMEELEMENT_SRC=-10,
	HTMLIFRAMEELEMENT_WIDTH=-11
};

/**
 * Call this method when getting a Property from HTMLFrameElement
 */
static JSBool jhtml_iframe_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLInlineFrame *pFrame = (pElem)? (HTMLInlineFrame *) pElem : 0;

	if (!pFrame || (pFrame->Type() != HTML_IFRAME_ELEMENT))
		return JS_TRUE;

	switch (JSVAL_TO_INT(id))
	{
		case HTMLIFRAMEELEMENT_ALIGN:
		{
			WEBC_UINT16 a = pFrame->GetAlign();
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, HTML_AlignTypeToString((HTMLAlignType)a)));
			return (JS_TRUE);
		}
		case HTMLIFRAMEELEMENT_DOCUMENT:
		{
			HTMLDocument *pDoc = pFrame->FrameDocument();
			if (pDoc && pDoc->GetJSObject())
			{
				*vp = OBJECT_TO_JSVAL(pDoc->GetJSObject());
			}
			return JS_TRUE;
		}
		case HTMLIFRAMEELEMENT_FRAMEBORDER:
		{
			*vp = INT_TO_JSVAL(pFrame->Frameborder());
			return JS_TRUE;
		}
		case HTMLIFRAMEELEMENT_LONGDESC:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pFrame->LongDesc()));
			return JS_TRUE;
		}
		case HTMLIFRAMEELEMENT_HEIGHT:
		{
			*vp = INT_TO_JSVAL(pFrame->GetClientHeight());
			return JS_TRUE;
		}
		case HTMLIFRAMEELEMENT_WIDTH:
		{
			*vp = INT_TO_JSVAL(pFrame->GetClientWidth());
			return JS_TRUE;
		}

		case HTMLIFRAMEELEMENT_MARGINHEIGHT:
		{
			*vp = INT_TO_JSVAL(pFrame->GetFrameMarginHeight());
			return JS_TRUE;
		}
		case HTMLIFRAMEELEMENT_MARGINWIDTH:
		{
			*vp = INT_TO_JSVAL(pFrame->GetFrameMarginWidth());
			return JS_TRUE;
		}
		case HTMLIFRAMEELEMENT_NAME:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pFrame->Name()));
			return JS_TRUE;
		}
		case HTMLIFRAMEELEMENT_SCROLLING:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pFrame->ScrollMode()));
			return JS_TRUE;
		}
		case HTMLIFRAMEELEMENT_SRC:
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
static JSBool jhtml_iframe_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLInlineFrame *pFrame = (pElem)? (HTMLInlineFrame *) pElem : 0;

	if (!pFrame || (pFrame->Type() != HTML_IFRAME_ELEMENT))
		return JS_TRUE;

	switch (JSVAL_TO_INT(id))
	{
	case HTMLIFRAMEELEMENT_ALIGN:
		if (JSVAL_IS_STRINGABLE(*vp))
		{
			pFrame->SetAlign(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
			return JS_TRUE;
		}
		break;

	case HTMLIFRAMEELEMENT_FRAMEBORDER:
		if (JSVAL_IS_NUMBER(*vp))
		{
			int32 b = 1;
			JS_ValueToInt32(cx, *vp, &b);
			pFrame->SetBorder((WEBC_UINT8) EBSCLIP(b, 0, 255));
			pFrame->Update(0,0);
			return JS_TRUE;
		}
		break;

	case HTMLIFRAMEELEMENT_LONGDESC:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pFrame->LongDesc()));
			return JS_TRUE;
		}

	case HTMLIFRAMEELEMENT_MARGINHEIGHT:
		if (JSVAL_IS_STRINGABLE(*vp))
		{
			pFrame->SetFrameMarginHeight(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
			return JS_TRUE;
		}
		break;

	case HTMLIFRAMEELEMENT_MARGINWIDTH:
		if (JSVAL_IS_STRINGABLE(*vp))
		{
			pFrame->SetFrameMarginWidth(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
			return JS_TRUE;
		}
		break;

	case HTMLIFRAMEELEMENT_NAME:
		if (JSVAL_IS_STRINGABLE(*vp))
		{
			pFrame->SetName(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
			return JS_TRUE;
		}
		break;

	case HTMLIFRAMEELEMENT_SCROLLING:
		if (JSVAL_IS_STRINGABLE(*vp))
		{
			pFrame->SetScrollMode(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
			pFrame->Update(0,0);
			return JS_TRUE;
		}
		break;

	case HTMLIFRAMEELEMENT_SRC:
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
JSClass JSHtmlIFrameElement =
{
	"InlineFrame", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty,
	(JSPropertyOp) jutil_delProperty, (JSPropertyOp) jhtml_iframe_getProperty,
	(JSPropertyOp) jhtml_iframe_setProperty, (JSEnumerateOp) jutil_enumerate,
	(JSResolveOp) jhtml_element_resolve, (JSConvertOp) jutil_convert,
	(JSFinalizeOp) jutil_finalize
};



JSPropertySpec jhtml_iframe_props[] = {
	{"align",        HTMLIFRAMEELEMENT_ALIGN        ,JSPROP_ENUMERATE},
	{"document",     HTMLIFRAMEELEMENT_DOCUMENT     ,JSPROP_ENUMERATE | JSPROP_READONLY},
	{"frameBorder",  HTMLIFRAMEELEMENT_FRAMEBORDER  ,JSPROP_ENUMERATE},
	{"height",       HTMLIFRAMEELEMENT_HEIGHT       ,JSPROP_ENUMERATE},
	{"longDesc",     HTMLIFRAMEELEMENT_LONGDESC     ,JSPROP_ENUMERATE},
	{"marginHeight", HTMLIFRAMEELEMENT_MARGINHEIGHT ,JSPROP_ENUMERATE},
	{"marginWidth",  HTMLIFRAMEELEMENT_MARGINWIDTH  ,JSPROP_ENUMERATE},
	{"name",         HTMLIFRAMEELEMENT_NAME         ,JSPROP_ENUMERATE | JSPROP_READONLY},
	{"scrolling",    HTMLIFRAMEELEMENT_SCROLLING    ,JSPROP_ENUMERATE},
	{"src",          HTMLIFRAMEELEMENT_SRC          ,JSPROP_ENUMERATE},
	{"width",        HTMLIFRAMEELEMENT_WIDTH        ,JSPROP_ENUMERATE},
	{0}
};

JSFunctionSpec jhtml_iframe_methods[] =
{
	{0}
};

static JSBool JHTMLIFrame(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	HTMLInlineFrame *pFrame;

	if (jsmgr)
	{
		HTMLTagParse tag[1];
		HTML_InitTagParse(tag, HTML_TAG_IFRAME);

		WEBC_NEW(pFrame, HTMLInlineFrame(tag, jsmgr->GetDocument()));
		if (pFrame)
		{
			pFrame->SetJSObject(obj);
			pFrame->SetTagProperties(tag, jsmgr->GetDocument());
			jsmgr->AddNewElement(pFrame);
		}
	}

	return JS_TRUE;
}

JSClass *getHtmlIFrameElement()
{
	return &JSHtmlIFrameElement;
}

JSObject *jhtml_iframe_Init(JSContext *cx, JSObject *proto)
{
	//initialize the HTMLIFrameElement Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSHtmlIFrameElement,
    				(JSNative) JHTMLIFrame, 0,
    				jhtml_iframe_props, jhtml_iframe_methods,
    				WEBC_NULL, WEBC_NULL);
}

#endif // (WEBC_SUPPORT_JSCRIPT && WEBC_SUPPORT_FRAMES && WEBC_SUPPORT_IFRAMES)
