/*
|  JNAV.CPP -
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
#include "jnav.h"
#include "jhutils.h"
#if (WEBC_SUPPORT_JSCRIPT)
#include "webcmem.h"
#include "wjsdebug.h"

//set the unique ids for the properties of the Navigator class
enum jnavigator_tinyid
{
	NAVIGATOR_APPCODENAME = -1,
	NAVIGATOR_APPMINORVERSION = -2,
	NAVIGATOR_APPNAME = -3,
	NAVIGATOR_APPVERSION = -4,
	NAVIGATOR_BROWSERLANGUAGE = -5,
	NAVIGATOR_COOKIEENABLED = -6,
	NAVIGATOR_CPUCLASS = -7,
	NAVIGATOR_ONLINE = -8,
	NAVIGATOR_PLATFORM = -9,
	NAVIGATOR_SYSTEMLANGUAGE = -10,
	NAVIGATOR_USERAGENT = -11,
	NAVIGATOR_USERLANGUAGE = -12
};

/**
 * Call this method when getting a Property from Navigator
 */
static JSBool jnavigator_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	switch (JSVAL_TO_INT(id))
	{
		case NAVIGATOR_APPCODENAME:
			*vp = STRING_TO_JSVAL(JS_InternString(cx, WEBC_APP_CODE_NAME));
			return JS_TRUE;

		case NAVIGATOR_APPMINORVERSION:
			*vp = STRING_TO_JSVAL(JS_InternString(cx, WEBC_APP_MINOR_VERSION));
			return JS_TRUE;

		case NAVIGATOR_APPNAME:
			*vp = STRING_TO_JSVAL(JS_InternString(cx, WEBC_APP_NAME));
			return JS_TRUE;

		case NAVIGATOR_APPVERSION:
			*vp = STRING_TO_JSVAL(JS_InternString(cx, WEBC_APP_VERSION));
			return JS_TRUE;

		case NAVIGATOR_USERAGENT:
		{
			char *str = (char*) WEBC_MALLOC((tc_strlen(WEBC_APP_CODE_NAME) +
			                                 tc_strlen(WEBC_APP_VERSION) + 2) * sizeof(char));
			if (str)
			{
				tc_strcpy(str, WEBC_APP_CODE_NAME);
				tc_strcat(str, "/");
				tc_strcat(str, WEBC_APP_VERSION);
				*vp = STRING_TO_JSVAL(JS_InternString(cx, str));
				WEBC_FREE(str);
			}
			return JS_TRUE;
		}

		case NAVIGATOR_USERLANGUAGE:
			*vp = STRING_TO_JSVAL(JS_InternString(cx, WEBC_USER_LANGUAGE));
			return JS_TRUE;

		case NAVIGATOR_SYSTEMLANGUAGE:
		case NAVIGATOR_BROWSERLANGUAGE:
			*vp = STRING_TO_JSVAL(JS_InternString(cx, WEBC_SYSTEM_LANGUAGE));
			return JS_TRUE;

		case NAVIGATOR_COOKIEENABLED:
#if(WEBC_SUPPORT_COOKIES)
			*vp = BOOLEAN_TO_JSVAL(JS_TRUE);
#else
			*vp = BOOLEAN_TO_JSVAL(JS_FALSE);
#endif //(WEBC_SUPPORT_COOKIES)
			return JS_TRUE;

		case NAVIGATOR_CPUCLASS:
			*vp = STRING_TO_JSVAL(JS_InternString(cx, WEBC_CPU_CLASS));
			return JS_TRUE;

		case NAVIGATOR_ONLINE:
#if (WEBC_SUPPORT_OFFLINE_BROWSING)
			*vp = BOOLEAN_TO_JSVAL((webc_online()? JS_TRUE : JS_FALSE));
#else
			*vp = BOOLEAN_TO_JSVAL(JS_TRUE);
#endif // WEBC_SUPPORT_OFFLINE_BROWSING
			return JS_TRUE;

		case NAVIGATOR_PLATFORM:
			*vp = STRING_TO_JSVAL(JS_InternString(cx, WEBC_PLATFORM));
			break;
	}//end switch
	return JS_TRUE;
}

/**
 * This method is called when setting a Property in navigator
 */
static JSBool jnavigator_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	switch (JSVAL_TO_INT(id))
	{
		case NAVIGATOR_ONLINE:
#if (WEBC_SUPPORT_OFFLINE_BROWSING)
			giWebcOnline = JSVAL_TO_BOOLEAN(*vp);
#endif
			return JS_TRUE;
	}
	//end switch

	return JS_TRUE;
}

JSBool jnavigator_javaEnabled(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = BOOLEAN_TO_JSVAL(JS_FALSE);
	return JS_TRUE;
}

JSBool jnavigator_taintEnabled(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	*rval = BOOLEAN_TO_JSVAL(JS_FALSE);
	return JS_TRUE;
}

JSBool jnavigator_resolve(JSContext *cx, JSObject *obj, jsval id)
{
    return(jutil_resolve(cx, obj, id, "Navigator"));
}

/**
 * This JSClass is the javascript wrapper for the Navigator class defined by MS
 */
JSClass JSNavigator =
{
	"Navigator", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty,
	(JSPropertyOp) jutil_delProperty, (JSPropertyOp) jnavigator_getProperty,
	(JSPropertyOp) jnavigator_setProperty, (JSEnumerateOp) jutil_enumerate,
	(JSResolveOp) jnavigator_resolve, (JSConvertOp) jutil_convert,
	(JSFinalizeOp) jutil_finalize
};


JSPropertySpec jnavigator_props[] = {
	{"appCodeName",     NAVIGATOR_APPCODENAME,     JSPROP_READONLY | JSPROP_ENUMERATE},
	{"appMinorVersion", NAVIGATOR_APPMINORVERSION, JSPROP_READONLY | JSPROP_ENUMERATE},
	{"appName",         NAVIGATOR_APPNAME,         JSPROP_READONLY | JSPROP_ENUMERATE},
	{"appVersion",      NAVIGATOR_APPVERSION,      JSPROP_READONLY | JSPROP_ENUMERATE},
	{"browserLanguage", NAVIGATOR_BROWSERLANGUAGE, JSPROP_READONLY | JSPROP_ENUMERATE},
	{"cookieEnabled",   NAVIGATOR_COOKIEENABLED,   JSPROP_READONLY | JSPROP_ENUMERATE},
	{"cpuClass",        NAVIGATOR_CPUCLASS,        JSPROP_READONLY | JSPROP_ENUMERATE},
	{"online",          NAVIGATOR_ONLINE,          JSPROP_ENUMERATE},
	{"platform",        NAVIGATOR_PLATFORM,        JSPROP_READONLY | JSPROP_ENUMERATE},
	{"systemLanguage",  NAVIGATOR_SYSTEMLANGUAGE,  JSPROP_READONLY | JSPROP_ENUMERATE},
	{"userAgent",       NAVIGATOR_USERAGENT,       JSPROP_READONLY | JSPROP_ENUMERATE},
	{"userLanguage",    NAVIGATOR_USERLANGUAGE,    JSPROP_READONLY | JSPROP_ENUMERATE},
	{0}
};

JSFunctionSpec jnavigator_methods[] =
{
	{"javaEnabled",    (JSNative) jnavigator_javaEnabled,    0},
	{"taintEnabled",   (JSNative) jnavigator_taintEnabled,   0},
	{0}
};

static JSBool JNavigator(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	return JS_TRUE;
}

JSClass *getJNavigator()
{
	return &JSNavigator;
}

JSObject *jnavigator_Init(JSContext *cx, JSObject *proto)
{
	//initialize the Navigator Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSNavigator,
    				(JSNative) JNavigator, 0,
    				jnavigator_props, jnavigator_methods,
    				WEBC_NULL, WEBC_NULL);
}

#endif //(WEBC_SUPPORT_JSCRIPT)
