/*
|  JHMAP.CPP -
|
|  EBS - WebC
|
|  Copyright EBS Inc. , 2008
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
|
|  Module description:
|   [tbd]
*/

#include "webcjhconfig.h"
#include "jhmap.h"
#include "jhutils.h"
#if (WEBC_SUPPORT_JSCRIPT)

#include "hmap.hpp"
#include "wjscript.hpp"
#include "htmldoc.hpp"
#include "objref.cpp"
#include "webcmem.h"
#include "jhelem.h"
#include "wjsdebug.h"
#include "jhcoll.h"

//set the unique ids for the properties of the image map class
enum jhtml_map_tinyid
{
	HTMLMAPELEMENT_DEFAULTVALUE=-1,
	HTMLMAPELEMENT_NAME=-2
};

/**
 * Call this method when getting a Property from HTMLMap
 */
static JSBool jhtml_map_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLMap *pMap = (pElem)? (HTMLMap *) pElem : 0;

	if (!pMap)
		return JS_TRUE;

	switch (JSVAL_TO_INT(id))
	{
		case HTMLMAPELEMENT_NAME:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pMap->Name()));
		}
		break;
		default:
			break;
	}//end switch
	return JS_TRUE;
}

/**
 * This method is called when setting a Property in HTMLMap
 */
static JSBool jhtml_map_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLMap *pMap = (pElem)? (HTMLMap *) pElem : 0;

	if (!pMap)
		return JS_TRUE;

	switch (JSVAL_TO_INT(id))
	{
		case HTMLMAPELEMENT_NAME:
		{
			pMap->SetName(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
			return JS_TRUE;
		}
			break;
		default:
			break;
	}//end switch
	return JS_TRUE;
}



JSBool jhtml_map_resolve(JSContext *cx, JSObject *obj, jsval id)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLMap *pMap = (pElem)? (HTMLMap *) pElem : 0;
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	if (!pMap || !jsmgr)
		return JS_TRUE;

	if (JSVAL_IS_STRING(id))
	{
		WEBC_CHAR *name = WEBC_JS_STRING_TO_WEBC_STRING(JSVAL_TO_STRING(id));

		if (!webc_c_strcmp(name, "areas"))
		{
			jhtml_collection_New(jsmgr, obj, "areas", pMap, MAP_AREAS);
			return JS_TRUE;
		}
		else
		{
			// sorry, we just don't have a property of this name
		}
	}

	return (jhtml_element_resolve(cx, obj, id));
}

/**
 * This JSClass is the javascript wrapper for the HTMLMap class defined in the DOM Level 1
 */
JSClass JSHtmlMapElement =
{
	"HTMLMapElement", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty,
	(JSPropertyOp) jutil_delProperty, (JSPropertyOp) jhtml_map_getProperty,
	(JSPropertyOp) jhtml_map_setProperty, (JSEnumerateOp) jutil_enumerate,
	(JSResolveOp) jhtml_map_resolve, (JSConvertOp) jutil_convert,
	(JSFinalizeOp) jutil_finalize
};


JSPropertySpec jhtml_map_props[] = {
	{"name", HTMLMAPELEMENT_NAME, JSPROP_ENUMERATE},
	{0}
};

JSFunctionSpec jhtml_map_methods[] =
{
	//all the methods that are needed by this class are defined in
	//JHTMLInputElement
	{0}
};

static JSBool JHTMLMap(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	HTMLMap *pMap;

	if (jsmgr)
	{
		HTMLTagParse tag[1];
		HTML_InitTagParse(tag, HTML_TAG_MAP);

		WEBC_NEW_VERBOSE(pMap, HTMLMap(tag, jsmgr->GetDocument()),"JHTMLMap");
		if (pMap)
		{
			pMap->SetJSObject(obj);
			pMap->SetTagProperties(tag, jsmgr->GetDocument());
			jsmgr->AddNewElement(pMap);
		}
	}
	return JS_TRUE;
}

JSClass *getHtmlMapElement()
{
	return &JSHtmlMapElement;
}

JSObject *jhtml_map_Init(JSContext *cx, JSObject *proto)
{
	//initialize the HTMLMapElement Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSHtmlMapElement,
    				(JSNative) JHTMLMap, 0,
    				jhtml_map_props, jhtml_map_methods,
    				WEBC_NULL, WEBC_NULL);
}

#endif //(WEBC_SUPPORT_JSCRIPT)
