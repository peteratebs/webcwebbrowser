/*
|  JSSHEET.CPP -
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
#include "jssheet.h"
#include "jhutils.h"
#if (WEBC_SUPPORT_JSCRIPT)

#include "jhcoll.h"
#include "htmldoc.hpp"
#include "wjscript.hpp"
#include "webcstr.h"
#include "objref.cpp"
#include "wjsdebug.h"

//set the unique ids for the properties of the Navigator class
enum jstylesheet_tinyid
{
	STYLESHEET_RULES = -1,
	STYLESHEET_CANHAVEHTML = -2,
	STYLESHEET_DISABLED = -3,
	STYLESHEET_HREF = -4,
	STYLESHEET_ID = -5,
	STYLESHEET_ISCONTENTEDITABLE = -6,
	STYLESHEET_ISDISABLED = -7,
	STYLESHEET_OWNINGELEMENT = -8,
	STYLESHEET_PARENTSTYLESHEET = -9,
	STYLESHEET_READONLY = -10,
	STYLESHEET_TITLE = -11,
	STYLESHEET_TYPE = -12,
	STYLESHEET_IMPORTS = -13,
	STYLESHEET_PAGES = -14,
	STYLESHEET_PAGE = -15,
	STYLESHEET_CSSTEXT = -16,
	STYLESHEET_MEDIA = -17,
	STYLESHEET_TEXTAUTOSPACE = -18
};


JSBool jstylesheet_resolve(JSContext *cx, JSObject *obj, jsval id)
{
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	if (!jsmgr)
	{
		return JS_TRUE;
	}

	if (JSVAL_IS_STRING(id))
	{
		WEBC_CHAR *name = WEBC_JS_STRING_TO_WEBC_STRING(JSVAL_TO_STRING(id));

		if (!webc_c_strcmp(name, "rules"))
		{
			jhtml_collection_New(jsmgr, obj, "rules", 0, SSHEET_RULES);
		}
		else if (!webc_c_strcmp(name, "imports"))
		{
			jhtml_collection_New(jsmgr, obj, "imports", 0, SSHEET_IMPORTS);
		}
	}

    return JS_TRUE;
}


/**
 * Call this method when getting a Property from Navigator
 */
static JSBool jstylesheet_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
#if (WEBC_SUPPORT_STYLE_SHEETS)
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	if (!jsmgr)
	{
		return JS_FALSE;
	}
	switch (JSVAL_TO_INT(id))
	{

		case STYLESHEET_CANHAVEHTML:
			*vp = BOOLEAN_TO_JSVAL(JS_FALSE);
			return JS_TRUE;

		case STYLESHEET_ISDISABLED:
		case STYLESHEET_DISABLED:
			//this is not implemented in our browser
			*vp = BOOLEAN_TO_JSVAL(JS_FALSE);
			return JS_TRUE;

		case STYLESHEET_HREF:
			//Links are not implemented in our browser so this always returns WEBC_NULL
			*vp = JSVAL_NULL;
			return JS_TRUE;

		case STYLESHEET_ISCONTENTEDITABLE:
			*vp = BOOLEAN_TO_JSVAL(JS_FALSE);
			return JS_TRUE;

		case STYLESHEET_OWNINGELEMENT:
			//GMP to do
			return JS_TRUE;

		case STYLESHEET_PARENTSTYLESHEET:
			*vp = JSVAL_NULL;
			return JS_TRUE;

		case STYLESHEET_READONLY:
			*vp = BOOLEAN_TO_JSVAL(JS_FALSE);
			return JS_TRUE;

		case STYLESHEET_TITLE:
			//we dont support this because we dont support links
			return JS_TRUE;

		case STYLESHEET_TYPE:
			*vp = STRING_TO_JSVAL(JS_InternString(cx, "text/css"));
			return JS_TRUE;

		case STYLESHEET_PAGES:
		case STYLESHEET_PAGE:
			//pages are implemented in CCS2.0 and later
			return JS_TRUE;

		case STYLESHEET_CSSTEXT:
			//GMP to do
			return JS_TRUE;

		case STYLESHEET_MEDIA:
			//This is not used in MSIE or Netscape so we don't support it
			return JS_TRUE;

		case STYLESHEET_TEXTAUTOSPACE:
			*vp = STRING_TO_JSVAL(JS_InternString(cx, "none"));
			return JS_TRUE;
	}//end switch
#endif
	return JS_TRUE;
}

/**
 * This method is called when setting a Property in navigator
 */
static JSBool jstylesheet_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
#if (WEBC_SUPPORT_STYLE_SHEETS)
	CSSDocumentContext *pCssDocCtx = (CSSDocumentContext*)jhutil_GetPrivate(cx, obj);
	HTMLDocument* pDoc = (pCssDocCtx)? pCssDocCtx->GetHTMLDocument() : 0;
	if (!pDoc)
	{
		return JS_TRUE;
	}

	switch (JSVAL_TO_INT(id))
	{
		case STYLESHEET_HREF:
			//Links are not implemented so this attribute does nothing
			return JS_TRUE;

		case STYLESHEET_ID:
			//we dont store a style tag as an HTML element so this does nothing
			return JS_TRUE;

		case STYLESHEET_TITLE:
			//we dont store a style tag as an HTML element so this does nothing
			return JS_TRUE;

		case STYLESHEET_PAGE:
			//this will be implimented when CSS2.0 is implimented
			return JS_TRUE;

		case STYLESHEET_CSSTEXT:
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				pCssDocCtx->ParseStyleSheet (
						WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)),
						CSS_ORIGIN_INTERNAL,
						pDoc->CurrentURL(),
						1 /* last chunk? */
					);
				pDoc->Body()->Update(0,0);
			}
			return JS_TRUE;

		case STYLESHEET_MEDIA:
			//this is not implimented by any major browser so we dont support it
			return JS_TRUE;

		case STYLESHEET_TEXTAUTOSPACE:
			//this is not implimented in our browser
			return JS_TRUE;
	}//end switch
#endif// (WEBC_SUPPORT_STYLE_SHEETS)

	return JS_TRUE;

}

JSBool jstylesheet_addImport(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	//GMP do import stuff when imports are working.
	return JS_TRUE;
}

JSBool jstylesheet_addPageRule(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	//Pages will be implimented when CSS2.0 is implimented
	return JS_TRUE;
}

JSBool jstylesheet_addRule(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	//GMP
	return JS_TRUE;
}

JSBool jstylesheet_fireEvent(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	//this makes no sense because the style tag does not have an element associated with it
	*rval = JSVAL_VOID;
	return JS_TRUE;
}

JSBool jstylesheet_hasFocus(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	//this makes no sense because the style tag does not have an element associated with it
	*rval = BOOLEAN_TO_JSVAL(JS_FALSE);
	return JS_TRUE;
}

JSBool jstylesheet_removeRule(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	//GMP

	return JS_TRUE;

}

/**
 * This JSClass is the javascript wrapper for the Navigator class defined by MS
 */
JSClass JSStyleSheet =
{
	"StyleSheet", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty,
	(JSPropertyOp) jutil_delProperty, (JSPropertyOp) jstylesheet_getProperty,
	(JSPropertyOp) jstylesheet_setProperty, (JSEnumerateOp) jutil_enumerate,
	(JSResolveOp) jstylesheet_resolve, (JSConvertOp) jutil_convert,
	(JSFinalizeOp) jutil_finalize
};


JSPropertySpec jstylesheet_props[] = {
	{"canHaveHTML",   	  STYLESHEET_CANHAVEHTML,       JSPROP_ENUMERATE | JSPROP_READONLY},
	{"disabled",          STYLESHEET_DISABLED,          JSPROP_ENUMERATE | JSPROP_READONLY},
	{"href",              STYLESHEET_HREF,              JSPROP_ENUMERATE | JSPROP_READONLY},
	{"id",                STYLESHEET_ID,                JSPROP_ENUMERATE | JSPROP_READONLY},
	{"isContentEditable", STYLESHEET_ISCONTENTEDITABLE, JSPROP_ENUMERATE | JSPROP_READONLY},
	{"isDisabled",        STYLESHEET_ISDISABLED,        JSPROP_ENUMERATE | JSPROP_READONLY},
//	{"owningElement",     STYLESHEET_OWNINGELEMENT,     JSPROP_ENUMERATE | JSPROP_READONLY},
	{"parentStyleSheet",  STYLESHEET_PARENTSTYLESHEET,  JSPROP_ENUMERATE | JSPROP_READONLY},
	{"readOnly",          STYLESHEET_READONLY,          JSPROP_ENUMERATE | JSPROP_READONLY},
//	{"title",             STYLESHEET_TITLE,             JSPROP_ENUMERATE | JSPROP_READONLY},
	{"type",              STYLESHEET_TYPE,              JSPROP_ENUMERATE | JSPROP_READONLY},
//	{"pages",             STYLESHEET_PAGES,             JSPROP_ENUMERATE | JSPROP_READONLY},
//	{"page",              STYLESHEET_PAGE,              JSPROP_ENUMERATE},
//	{"cssText",           STYLESHEET_CSSTEXT,           JSPROP_ENUMERATE},
//	{"media",             STYLESHEET_MEDIA,             JSPROP_ENUMERATE},
	{"textAutospace",     STYLESHEET_TEXTAUTOSPACE,     JSPROP_ENUMERATE | JSPROP_READONLY},
	{0}
};

JSFunctionSpec jstylesheet_methods[] =
{
//	{"addImport",   jstylesheet_addImport,   2},
//	{"addPageRule", jstylesheet_addPageRule, 3},
//	{"addRule",     jstylesheet_addRule,     1},
//	{"fireEvent",   jstylesheet_fireEvent,   2},
//	{"hasFocus",    jstylesheet_hasFocus,    0},
//	{"removeRule",  jstylesheet_removeRule,  1},
	{0}
};

static JSBool JStyleSheet_Const(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	return JS_TRUE;
}

JSClass *getJStyleSheet()
{
	return &JSStyleSheet;
}

JSObject *jstylesheet_Init(JSContext *cx, JSObject* proto)
{
	//initialize the Navigator Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSStyleSheet,
    				(JSNative) JStyleSheet_Const, 0,
    				jstylesheet_props, jstylesheet_methods,
    				WEBC_NULL, WEBC_NULL);
}

#endif //(WEBC_SUPPORT_JSCRIPT)
