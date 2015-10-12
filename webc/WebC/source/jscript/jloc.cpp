/*
|  JLOC.CPP -
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
#include "jloc.h"
#include "jhutils.h"
#include "webload.hpp"

#if (WEBC_SUPPORT_JSCRIPT)

#include "htmldoc.hpp"
#include "htmlbrow.hpp"
#include "webcstr.h"
#include "webcmem.h"
#include "wjsdebug.h"

//set the unique ids for the properties of the location class
enum jlocation_tinyid
{
	LOCATION_HASH=-1,
	LOCATION_HOST=-2,
	LOCATION_HOSTNAME=-3,
	LOCATION_HREF=-4,
	LOCATION_PATHNAME=-5,
	LOCATION_PORT=-6,
	LOCATION_PROTOCOL=-7,
	LOCATION_SEARCH=-8
};

/**
 * Call this method when getting a Property from Location
 */
static JSBool jlocation_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLDocument *pDoc = (HTMLDocument *)jhutil_GetPrivate(cx, obj);
	URLDescriptor *url = (pDoc)? pDoc->CurrentURL() : 0;
	if (!url)
	{
		return JS_TRUE;
	}

	JSString *str = 0;

	switch (JSVAL_TO_INT(id))
	{
		case LOCATION_HASH:
			*vp = STRING_TO_JSVAL(JS_InternString(cx, ""));
			break;

		case LOCATION_HOST:
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, url->getHost()));
			break;

		case LOCATION_HOSTNAME:
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, url->getHost()));
			break;

		case LOCATION_HREF:
		{
			if (url)
			{
				UrlStringBuilder ubld(url);
				WEBC_CHAR *str = ubld.MallocString(__FILE__, __LINE__);
				if (str)
				{
					*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, str));
					ubld.FreeString(str, __FILE__, __LINE__);
				}
				else
				{
					*vp = STRING_TO_JSVAL(JS_InternString(cx, ""));
				}
			}
			else
			{
				*vp = STRING_TO_JSVAL(JS_InternString(cx, ""));
			}
			return JS_TRUE;
		}

		case LOCATION_PATHNAME:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, url->getPath()));
			break;
		}

		case LOCATION_PORT:
			*vp = INT_TO_JSVAL(url->getPort());
			break;

		case LOCATION_PROTOCOL:
			switch(url->getProtocol())
			{
				case WEBC_PROTOCOL_NONE:
					str = JS_InternString(cx, "none");
					break;
				case WEBC_PROTOCOL_HTTP:
					str = JS_InternString(cx, "http:");
					break;
				case WEBC_PROTOCOL_FTP:
					str = JS_InternString(cx, "ftp:");
					break;
				case WEBC_PROTOCOL_TELNET:
					str = JS_InternString(cx, "telnet:");
					break;
				case WEBC_PROTOCOL_FILE:
					str = JS_InternString(cx, "file:");
					break;
				case WEBC_PROTOCOL_LOCAL:
					str = JS_InternString(cx, "local:");
					break;
				case WEBC_PROTOCOL_HTTPS:
					str = JS_InternString(cx, "https:");
					break;
				case WEBC_PROTOCOL_WEBC:
					str = JS_InternString(cx, "webc:");
					break;
				case WEBC_PROTOCOL_WEBS:
					str = JS_InternString(cx, "webs:");
					break;
				case WEBC_PROTOCOL_CACHE:
					str = JS_InternString(cx, "cache:");
					break;
				default:
					str = JS_InternString(cx, "none");
			}
			*vp = STRING_TO_JSVAL(str);
			break;

		case LOCATION_SEARCH:
			*vp = JSVAL_NULL;
			break;
	}//end switch

	return JS_TRUE;
}

/**
 * This method is called when setting a Property in Location
 */
static JSBool jlocation_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLDocument *pDoc = (HTMLDocument *)jhutil_GetPrivate(cx, obj);
	URLDescriptor *url = (pDoc)? pDoc->CurrentURL() : 0;
	if (!url)
	{
		return JS_TRUE;
	}

	switch (JSVAL_TO_INT(id))
	{
		case LOCATION_HASH:
			*vp = JSVAL_NULL;
			break;

		case LOCATION_HOSTNAME:
		case LOCATION_HOST:
			if (JSVAL_IS_STRING(*vp))
			{
				if (url)
				{
					URLDescriptor newUrl(url);
					newUrl.setHost(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
					UrlStringBuilder ubld(&newUrl);
					WEBC_CHAR *str = ubld.MallocString(__FILE__,__LINE__);
					if (str)
					{
						jutils_LoadDocumentUrl(cx, pDoc, str);
						ubld.FreeString(str, __FILE__, __LINE__);
					}
				}
			}
			break;

		case LOCATION_HREF:
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				jutils_LoadDocumentUrl(cx, pDoc, WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
			}
			break;

		case LOCATION_PATHNAME:
			if(JSVAL_IS_STRINGABLE(*vp))
			{
				if (url)
				{
					URLDescriptor newUrl(url);
					newUrl.setPath(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
					UrlStringBuilder ubld(&newUrl);
					WEBC_CHAR *str = ubld.MallocString(__FILE__,__LINE__);
					if (str)
					{
						jutils_LoadDocumentUrl(cx, pDoc, str);
						ubld.FreeString(str, __FILE__, __LINE__);
					}
				}
			}
			break;

		case LOCATION_PORT:
			if (JSVAL_IS_INT(*vp))
			{
				if (url)
				{
					int32 p;
					JS_ValueToInt32(cx, *vp, &p);
					URLDescriptor newUrl(url);
					newUrl.setPort((int) p);
					UrlStringBuilder ubld(&newUrl);
					WEBC_CHAR *str = ubld.MallocString(__FILE__,__LINE__);
					if (str)
					{
						jutils_LoadDocumentUrl(cx, pDoc, str);
						ubld.FreeString(str, __FILE__, __LINE__);
					}
				}
			}
			break;

		case LOCATION_PROTOCOL:
			if (JSVAL_IS_STRING(*vp))
			{
				if (url)
				{
					URLDescriptor newUrl(url);
					char *proto = JS_GetStringBytes(JS_ValueToString(cx, *vp));
					if (tc_stricmp(proto, "http"))
					{
						newUrl.setProtocol(WEBC_PROTOCOL_HTTP);
					}
					else if (tc_stricmp(proto, "ftp"))
					{
						newUrl.setProtocol(WEBC_PROTOCOL_FTP);
					}
					else if (tc_stricmp(proto, "telnet"))
					{
						newUrl.setProtocol(WEBC_PROTOCOL_TELNET);
					}
					else if (tc_stricmp(proto, "file"))
					{
						newUrl.setProtocol(WEBC_PROTOCOL_FILE);
					}
					else if (tc_stricmp(proto, "local"))
					{
						newUrl.setProtocol(WEBC_PROTOCOL_LOCAL);
					}
					else if (tc_stricmp(proto, "https"))
					{
						newUrl.setProtocol(WEBC_PROTOCOL_HTTPS);
					}
					else if (tc_stricmp(proto, "webc"))
					{
						newUrl.setProtocol(WEBC_PROTOCOL_WEBC);
					}
					else if (tc_stricmp(proto, "webs"))
					{
						newUrl.setProtocol(WEBC_PROTOCOL_WEBS);
					}
					else if (tc_stricmp(proto, "cache"))
					{
						newUrl.setProtocol(WEBC_PROTOCOL_CACHE);
					}
					else
					{
						newUrl.setProtocol(WEBC_PROTOCOL_NONE);
					}

					UrlStringBuilder ubld(&newUrl);
					WEBC_CHAR *str = ubld.MallocString(__FILE__,__LINE__);
					if (str)
					{
						jutils_LoadDocumentUrl(cx, pDoc, str);
						ubld.FreeString(str, __FILE__, __LINE__);
					}
				}
			}
			break;

		case LOCATION_SEARCH:
			break;
	}//end switch

	return jutils_CheckContextState(cx);
}

JSBool jlocation_assign(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLDocument *pDoc = (HTMLDocument *)jhutil_GetPrivate(cx, obj);

	if (pDoc && (argc > 0 && JSVAL_IS_STRING(argv[0])))
	{
		WEBC_CHAR *pUrlStr = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, argv[0]));
		jutils_LoadDocumentUrl(cx, pDoc, pUrlStr);
	}
	*rval = JSVAL_VOID;
	return jutils_CheckContextState(cx);
}

JSBool jlocation_reload(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLDocument *pDoc = (HTMLDocument *)jhutil_GetPrivate(cx, obj);
	URLDescriptor *url = (pDoc)? pDoc->CurrentURL() : 0;

	if (url)
	{
		UrlStringBuilder ubld(url);
		WEBC_CHAR *str = ubld.MallocString(__FILE__,__LINE__);
		if (str)
		{
			jutils_LoadDocumentUrl(cx, pDoc, str);
			ubld.FreeString(str, __FILE__, __LINE__);
		}
	}
	*rval = JSVAL_VOID;

	return jutils_CheckContextState(cx);
}

JSBool jlocation_replace(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLDocument *pDoc = (HTMLDocument *)jhutil_GetPrivate(cx, obj);

	if (pDoc && (argc > 0 && JSVAL_IS_STRING(argv[0])))
	{
		WEBC_CHAR *pUrlStr = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, argv[0]));
		jutils_LoadDocumentUrl(cx, pDoc, pUrlStr);
	}
	*rval = JSVAL_VOID;
	return jutils_CheckContextState(cx);
}

JSBool jlocation_stop(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
    HTMLDocument* doc = (HTMLDocument*) jhutil_GetPrivate(cx, obj);
	HTMLBrowser* browser = (doc)? doc->GetBrowser() : 0;

	if (browser)
	{
		browser->Stop(doc);
	}

	*rval = JSVAL_VOID;
	return jutils_CheckContextState(cx);
}


JSBool jlocation_toString(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLDocument *pDoc = (HTMLDocument*) jhutil_GetPrivate(cx, obj);
	URLDescriptor *url = (pDoc)? pDoc->CurrentURL() : 0;
	if (url)
	{
		UrlStringBuilder ubld(url);
		WEBC_CHAR *str = ubld.MallocString(__FILE__,__LINE__);
		if (str)
		{
			*rval = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, str));
			ubld.FreeString(str, __FILE__, __LINE__);
		}
	}
	return JS_TRUE;
}

JSBool jlocation_resolve(JSContext *cx, JSObject *obj, jsval id)
{
    return(jutil_resolve(cx, obj, id, "Location"));
}


/**
 * This JSClass is the javascript wrapper for the Location class defined in the DOM Level 1
 */
JSClass JSLocation =
{
	"Location", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty,
	(JSPropertyOp) jutil_delProperty, (JSPropertyOp) jlocation_getProperty,
	(JSPropertyOp) jlocation_setProperty, (JSEnumerateOp) jutil_enumerate,
	(JSResolveOp) jlocation_resolve, (JSConvertOp) jutil_convert,
	(JSFinalizeOp) jutil_finalize
};


JSPropertySpec jlocation_props[] = {
	{"hash",     LOCATION_HASH,     JSPROP_ENUMERATE | JSPROP_READONLY},
	{"host",     LOCATION_HOST,     JSPROP_ENUMERATE},
	{"hostname", LOCATION_HOSTNAME, JSPROP_ENUMERATE},
	{"href",     LOCATION_HREF,     JSPROP_ENUMERATE},
	{"pathname", LOCATION_PATHNAME, JSPROP_ENUMERATE},
	{"port",     LOCATION_PORT,     JSPROP_ENUMERATE},
	{"protocol", LOCATION_PROTOCOL, JSPROP_ENUMERATE},
//	{"search",   LOCATION_SEARCH,   JSPROP_ENUMERATE | JSPROP_READONLY},
	{0}
};

JSFunctionSpec jlocation_methods[] =
{
	{"assign",   (JSNative) jlocation_assign,   1},
	{"reload",   (JSNative) jlocation_reload,   0},
	{"replace",  (JSNative) jlocation_replace,  1},
	{"stop",     (JSNative) jlocation_stop,     0},
	{"toString", (JSNative) jlocation_toString, 0},
	{0}
};

static JSBool JLocation(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	return JS_TRUE;
}

JSClass *getJLocation()
{
	return &JSLocation;
}

JSObject *jlocation_Init(JSContext *cx, JSObject *proto)
{
	//initialize the Location Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSLocation,
    				(JSNative) JLocation, 0,
    				jlocation_props, jlocation_methods,
    				WEBC_NULL, WEBC_NULL);
}

#endif //(WEBC_SUPPORT_JSCRIPT)
