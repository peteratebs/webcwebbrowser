/*
|  JHISTORY.CPP -
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
#include "jhistory.h"
#include "jhutils.h"
#if (WEBC_SUPPORT_JSCRIPT)
#include "htmlbrow.hpp"
#include "wjsdebug.h"

//set the unique ids for the properties of the History class
enum jhistory_tinyid
{
	HISTORY_LENGTH=-1
};

/**
 * Call this method when getting a Property from History
 */
static JSBool jhistory_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLBrowser *pBrowser = (HTMLBrowser *)jhutil_GetPrivate(cx, obj);

	if (!pBrowser)
		return JS_TRUE;

	switch (JSVAL_TO_INT(id))
	{
		case HISTORY_LENGTH:
			*vp = INT_TO_JSVAL(pBrowser->GetHistorySize());
			break;
	}//end switch
	return JS_TRUE;
}

/**
 * This method is called when setting a Property in History
 */
static JSBool jhistory_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	//ALL PROPERTIES OF THE HISTORY CLASS ARE READONLY
	return JS_TRUE;
}

JSBool jhistory_go(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLBrowser *pBrowser = (HTMLBrowser *)jhutil_GetPrivate(cx, obj);

	if (!pBrowser)
		return JS_TRUE;

	if (argc > 0 && JSVAL_IS_INT(argv[0]))
	{
		int32 index = -1;
		JS_ValueToInt32(cx, argv[0], &index);

		pBrowser->Go(index);
	}
	return JS_TRUE;
}

JSBool jhistory_back(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLBrowser *pBrowser = (HTMLBrowser *)jhutil_GetPrivate(cx, obj);

	if (!pBrowser)
		return JS_TRUE;

	if (argc > 0 && JSVAL_IS_INT(argv[0]))
	{
		int32 index = -1;
		JS_ValueToInt32(cx, argv[0], &index);

		if (index > 0)
			index = -index;

		pBrowser->Go(index);
	}
	else pBrowser->Go(WEBC_BROWSER_BACK);

	return JS_TRUE;
}

JSBool jhistory_forward(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLBrowser *pBrowser = (HTMLBrowser *)jhutil_GetPrivate(cx, obj);

	if (!pBrowser)
		return JS_TRUE;

	if (argc > 0 && JSVAL_IS_INT(argv[0]))
	{
		int32 index = -1;
		JS_ValueToInt32(cx, argv[0], &index);

		pBrowser->Go(index);
	}
	else pBrowser->Go(WEBC_BROWSER_FORWARD);

	return JS_TRUE;
}

JSBool jhistory_resolve(JSContext *cx, JSObject *obj, jsval id)
{
    return(jutil_resolve(cx, obj, id, "History"));
}


/**
 * This JSClass is the javascript wrapper for the History class defined by MS
 */
JSClass JSHistory =
{
	"History", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty,
	(JSPropertyOp) jutil_delProperty, (JSPropertyOp) jhistory_getProperty,
	(JSPropertyOp) jhistory_setProperty, (JSEnumerateOp) jutil_enumerate,
	(JSResolveOp) jhistory_resolve, (JSConvertOp) jutil_convert,
	(JSFinalizeOp) jutil_finalize
};


JSPropertySpec jhistory_props[] = {
	{"length", HISTORY_LENGTH, JSPROP_READONLY | JSPROP_ENUMERATE},
	{0}
};

JSFunctionSpec jhistory_methods[] =
{
	{"back",    (JSNative) jhistory_back,    1},
	{"forward", (JSNative) jhistory_forward, 1},
	{"go",      (JSNative) jhistory_go,      1},
	{0}
};

static JSBool JHistory_Const(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	return JS_TRUE;
}

JSClass *getJHistory()
{
	return &JSHistory;
}

JSObject *jhistory_Init(JSContext *cx, JSObject *proto)
{
	//initialize the History Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSHistory,
    				(JSNative) JHistory_Const, 0,
    				jhistory_props, jhistory_methods,
    				WEBC_NULL, WEBC_NULL);
}

#endif //(WEBC_SUPPORT_JSCRIPT)
