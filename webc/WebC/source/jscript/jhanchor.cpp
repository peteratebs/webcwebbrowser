/*
|  JHANCHOR.CPP -
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
#include "jhanchor.h"
#include "jhutils.h"
#if (WEBC_SUPPORT_JSCRIPT)

#include "hanchor.hpp"
#include "jhelem.h"
#include "htmldoc.hpp"
#include "wjscript.hpp"
#include "objref.cpp"
#include "webcmem.h"
#include "wjsdebug.h"
#include "webcassert.h"
#include "webcLog.h"


extern JSPropertySpec jhtml_anchor_props[];


//set the unique ids for the properties of the element class
enum jhtml_anchor_tinyid
{
	HTMLANCHORELEMENT_ACCESSKEY=-1,
	HTMLANCHORELEMENT_CHARSET=-2,
	HTMLANCHORELEMENT_COORDS=-3,
	HTMLANCHORELEMENT_HREF=-4,
	HTMLANCHORELEMENT_HREFLANG=-5,
	HTMLANCHORELEMENT_NAME=-6,
	HTMLANCHORELEMENT_REL=-7,
	HTMLANCHORELEMENT_REV=-8,
	HTMLANCHORELEMENT_SHAPE=-9,
	HTMLANCHORELEMENT_TABINDEX=-10,
	HTMLANCHORELEMENT_TARGET=-11,
	HTMLANCHORELEMENT_TYPE=-12
};

/**
 * Call this method when getting a Property from HTMLElement
 */
static JSBool jhtml_anchor_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	int index = JSVAL_TO_INT(id);

#ifdef WEBC_DEBUG
	if(index < 0)
		LOG_HTMLDOM("jhtml_anchor_getProperty %s\n", jhtml_anchor_props[-(index+1)].name);
	else
		LOG_HTMLDOM("jhtml_anchor_getProperty %d\n", index);
#endif

	if (index)
	{
		HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
		if (pElem->Type() != HTML_ANCHOR_ELEMENT)
		{
			return JS_TRUE;
		}
		HTMLAnchor *anchor = (HTMLAnchor *) pElem;

		if (!pElem)
		{
			return JS_TRUE;
		}

		switch (index)
		{
			case(HTMLANCHORELEMENT_ACCESSKEY):
			{
				char ak[2];
				ak[0] = (char)anchor->AccessKey();
				ak[1] = '\0';
				JSString *str = JS_NewStringCopyZ(cx,ak);
				*vp = STRING_TO_JSVAL(str);
				return JS_TRUE;
			}
			case(HTMLANCHORELEMENT_CHARSET):
				*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, anchor->CharSet()));
				return JS_TRUE;
			case(HTMLANCHORELEMENT_COORDS):
			{
				WEBC_CHAR *pCoordString = (WEBC_CHAR *)HTML_PrintLengthList(anchor->Coordinates(), anchor->CoordinateCount());
				if(pCoordString)
				{
					*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pCoordString));
					WEBC_FREE(pCoordString);
					return JS_TRUE;
				}
			}
				break;
			case(HTMLANCHORELEMENT_HREF):
				*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, anchor->HRef()));
				return JS_TRUE;
			case(HTMLANCHORELEMENT_HREFLANG):
				*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, anchor->HRefLang()));
				return JS_TRUE;
			case(HTMLANCHORELEMENT_NAME):
				*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, anchor->Name()));
				return JS_TRUE;
			case(HTMLANCHORELEMENT_REL):
				*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, anchor->Rel()));
				return JS_TRUE;
			case(HTMLANCHORELEMENT_REV):
				*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, anchor->Rev()));
				return JS_TRUE;
			case(HTMLANCHORELEMENT_SHAPE):
				*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, anchor->ShapeStr()));
				return JS_TRUE;
			case(HTMLANCHORELEMENT_TABINDEX):
				*vp = INT_TO_JSVAL(anchor->TabIndex());
				return JS_TRUE;
			case(HTMLANCHORELEMENT_TARGET):
				*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, anchor->Target()));
				return JS_TRUE;
			case(HTMLANCHORELEMENT_TYPE):
				*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, anchor->MimeType()));
				return JS_TRUE;
		}//end switch
	}

	return JS_TRUE;
}

/**
 * This method is called when setting a Property in HTMLElement
 */
static JSBool jhtml_anchor_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLAnchor *anchor = (HTMLAnchor *) pElem;

	int index = JSVAL_TO_INT(id);

#ifdef WEBC_DEBUG
	if(index < 0)
		LOG_HTMLDOM("jhtml_anchor_setProperty %s\n", jhtml_anchor_props[-(index+1)].name);
	else
		LOG_HTMLDOM("jhtml_anchor_setProperty %d\n", index);
#endif

	if (!pElem)
		return JS_TRUE;

	switch (index)
	{
		case(HTMLANCHORELEMENT_ACCESSKEY):
			{
				char *str = JS_GetStringBytes(JS_ValueToString(cx, *vp));
				if (str && *str)
				{
					anchor->SetAccessKey((WEBC_UINT16)str[0]);
				}
				return JS_TRUE;
			}
		case(HTMLANCHORELEMENT_CHARSET):
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				anchor->SetCharSet(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
				return JS_TRUE;
			}
			break;
		case(HTMLANCHORELEMENT_COORDS):
		{
			anchor->SetCoordinates(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
			return JS_TRUE;
		}
		case(HTMLANCHORELEMENT_HREF):
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				anchor->SetHRef(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
				return JS_TRUE;
			}
			break;
		case(HTMLANCHORELEMENT_HREFLANG):
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				anchor->SetHRefLang(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
				return JS_TRUE;
			}
			break;
		case(HTMLANCHORELEMENT_NAME):
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				anchor->SetName(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
				return JS_TRUE;
			}
			break;
		case(HTMLANCHORELEMENT_REL):
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				anchor->SetRel(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
				return JS_TRUE;
			}
			break;
		case(HTMLANCHORELEMENT_REV):
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				anchor->SetRev(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
				return JS_TRUE;
			}
			break;
		case(HTMLANCHORELEMENT_SHAPE):
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				anchor->SetShape(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
				return JS_TRUE;
			}
			break;
		case(HTMLANCHORELEMENT_TABINDEX):
			if (JSVAL_IS_INT(*vp))
			{
				int32 tab = -1;
				JS_ValueToInt32(cx, *vp, &tab);
				anchor->SetTabIndex((short)tab);
			}
			break;
		case(HTMLANCHORELEMENT_TARGET):
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				anchor->SetTarget(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
				return JS_TRUE;
			}
			break;
		case(HTMLANCHORELEMENT_TYPE):
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				anchor->SetMimeType(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
				return JS_TRUE;
			}
			break;
	}//end switch
	return JS_TRUE;
}

/**
 * This JSClass is the javascript wrapper for the HTMLElement class defined in the DOM Level 1
 */
JSClass JSHtmlAnchorElement =
{
	"HTMLAnchorElement", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty,
	(JSPropertyOp) jutil_delProperty, (JSPropertyOp) jhtml_anchor_getProperty,
	(JSPropertyOp)jhtml_anchor_setProperty, (JSEnumerateOp) jutil_enumerate,
	(JSResolveOp) jhtml_element_resolve, (JSConvertOp)jutil_convert,
	(JSFinalizeOp) jutil_finalize
};

/**
 * blur - This method removes the focus from this element
 *
 * params - none;
 * return - void;
 */
JSBool jhtml_anchor_blur(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	LOG_HTMLDOM("jhtml_anchor_blur\n");

	if (JS_GetClass(obj) == &JSHtmlAnchorElement)
	{
		HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);

		if (!pElem)
		{
			return JS_TRUE;
		}

		HTMLDocument *pDoc = (pElem)? pElem->GetDocument() : 0;
		if (pDoc && pDoc->Body())
		{
			pDoc->Body()->Focus();
		}
	}

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

/**
 * focus() - This method puts the focus on this element
 *
 * params - none;
 * return - void
 */
JSBool jhtml_anchor_focus(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	LOG_HTMLDOM("jhtml_anchor_focus\n");

	if (JS_GetClass(obj) == &JSHtmlAnchorElement)
	{
		HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);

		if (!pElem)
		{
			return JS_TRUE;
		}

		pElem->Focus();
	}

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

JSPropertySpec jhtml_anchor_props[] = {
	{"accessKey", HTMLANCHORELEMENT_ACCESSKEY, JSPROP_ENUMERATE},
	{"charset",   HTMLANCHORELEMENT_CHARSET,   JSPROP_ENUMERATE},
	{"coords",    HTMLANCHORELEMENT_COORDS,    JSPROP_ENUMERATE},
	{"href",      HTMLANCHORELEMENT_HREF,      JSPROP_ENUMERATE},
	{"hreflang",  HTMLANCHORELEMENT_HREFLANG,  JSPROP_ENUMERATE},
	{"name",      HTMLANCHORELEMENT_NAME,      JSPROP_ENUMERATE},
	{"rel",       HTMLANCHORELEMENT_REL,       JSPROP_ENUMERATE},
	{"rev",       HTMLANCHORELEMENT_REV,       JSPROP_ENUMERATE},
	{"shape",     HTMLANCHORELEMENT_SHAPE,     JSPROP_ENUMERATE},
	{"tabIndex",  HTMLANCHORELEMENT_TABINDEX,  JSPROP_ENUMERATE},
	{"target",    HTMLANCHORELEMENT_TARGET,    JSPROP_ENUMERATE},
	{"type",    HTMLANCHORELEMENT_TYPE,    JSPROP_ENUMERATE},
    {0}
};

JSFunctionSpec jhtml_anchor_methods[] =
{
	{"blur",    (JSNative) jhtml_anchor_blur,           0},
  	{"focus",	(JSNative) jhtml_anchor_focus,          0},
	{0}
};


static JSBool JHTMLAnchorElement(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	if (jsmgr)
	{
		HTMLAnchor *pAnchor;
		HTMLTagParse tag[1];

		HTML_InitTagParse(tag, HTML_TAG_A);

		WEBC_NEW_VERBOSE(pAnchor, HTMLAnchor(tag, jsmgr->GetDocument()),"HTMLAnchor");
		if (pAnchor)
		{
			pAnchor->SetJSObject(obj);
			pAnchor->SetTagProperties(tag, jsmgr->GetDocument());
			jsmgr->AddNewElement(pAnchor);
		}
	}
	return JS_TRUE;
}

JSClass *getHtmlAnchorElement()
{
	return &JSHtmlAnchorElement;
}

JSObject *jhtml_anchor_Init(JSContext *cx, JSObject *proto)
{
	//initialize the HTMLElement Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSHtmlAnchorElement,
    				(JSNative) JHTMLAnchorElement, 0,
    				jhtml_anchor_props, jhtml_anchor_methods,
    				WEBC_NULL, WEBC_NULL);
}
#endif //(WEBC_SUPPORT_JSCRIPT)








