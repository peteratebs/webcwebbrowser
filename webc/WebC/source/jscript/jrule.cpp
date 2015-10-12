/*
|  JRULE.CPP -
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
#include "jrule.h"
#include "jhutils.h"
#if (WEBC_SUPPORT_JSCRIPT)
#include "jhcoll.h"
#include "wjsdebug.h"

//set the unique ids for the properties of the rule class
enum jrule_tinyid
{
	RULE_SELECTORTEXT = -1,
	RULE_READONLY = -2,
	RULE_STYLE = -3
};

/**
 * Call this method when getting a Property from rule
 */
static JSBool jrule_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
#if (WEBC_SUPPORT_STYLE_SHEETS)
/*	CSSPropertyDescriptor *pCSSPD = (CSSPropertyDescriptor*)jhutil_GetPrivate(cx, obj);
	switch (JSVAL_TO_INT(id))
	{
		case RULE_SELECTORTEXT:
		{
			char *str = pCSSPD->EnumFirstClass();
			//if (!str)
			//str = pCSSPD->EnumFirstTagList();
			if (!str)
				str = pCSSPD->id();
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, str));
			return JS_TRUE;
		}
		case RULE_READONLY:
			//GMP return to this when import is enabled
			*vp = BOOLEAN_TO_JSVAL(JS_TRUE);
			return JS_TRUE;
		case RULE_STYLE:
			*vp = OBJECT_TO_JSVAL(pCSSPD->GetJSObject());
			return JS_TRUE;
		default:
			char *str = JS_GetStringBytes(JS_ValueToString(cx, id));
			break;
			}*/
#endif //(WEBC_SUPPORT_STYLE_SHEETS)
	return JS_TRUE;
}

static JSBool jrule_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
#if (WEBC_SUPPORT_STYLE_SHEETS)
	switch (JSVAL_TO_INT(id))
	{
		case RULE_SELECTORTEXT:
			//GMP this is not possible with our current CSS implementation
			return JS_TRUE;
	}
#endif //(WEBC_SUPPORT_STYLE_SHEETS)
	return JS_TRUE;
}

JSBool jrule_resolve(JSContext *cx, JSObject *obj, jsval id)
{
    return(jutil_resolve(cx, obj, id, "Rule"));
}

/**
 * This JSClass is the javascript wrapper for the rule class defined by MS
 */
JSClass JSRule =
{
	"Rule", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty,
	(JSPropertyOp) jutil_delProperty, (JSPropertyOp) jrule_getProperty,
	(JSPropertyOp) jrule_setProperty, (JSEnumerateOp) jutil_enumerate,
	(JSResolveOp) jrule_resolve, (JSConvertOp) jutil_convert,
	(JSFinalizeOp) jutil_finalize
};


JSPropertySpec jrule_props[] = {
	{"selectorText", RULE_SELECTORTEXT, JSPROP_ENUMERATE | JSPROP_READONLY},
	{"readonly",     RULE_READONLY,     JSPROP_ENUMERATE | JSPROP_READONLY},
	{"style",        RULE_STYLE,        JSPROP_ENUMERATE | JSPROP_READONLY},
	{0}
};

JSFunctionSpec jrule_methods[] =
{
	{0}
};

static JSBool JRule_Const(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	return JS_TRUE;
}

JSClass *getJRule()
{
	return &JSRule;
}

JSObject *jrule_Init(JSContext *cx, JSObject* proto)
{
	//initialize the rule Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSRule,
    				(JSNative) JRule_Const, 0,
    				jrule_props, jrule_methods,
    				WEBC_NULL, WEBC_NULL);
}

#endif //(WEBC_SUPPORT_JSCRIPT)
