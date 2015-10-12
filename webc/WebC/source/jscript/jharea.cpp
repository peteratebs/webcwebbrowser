/*
|  JHAREA.CPP -
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
#include "jharea.h"
#include "jhutils.h"
#if (WEBC_SUPPORT_JSCRIPT)

#include "harea.hpp"
#include "wjscript.hpp"
#include "htmldoc.hpp"
#include "objref.cpp"
#include "webcmem.h"
#include "jhelem.h"
#include "wjsdebug.h"

//set the unique ids for the properties of the image map area class
enum jhtml_area_tinyid
{
	HTMLAREAELEMENT_DEFAULTVALUE=-1,
	HTMLAREAELEMENT_ACCESSKEY=-2,
	HTMLAREAELEMENT_ALT=-3,
	HTMLAREAELEMENT_COORDS=-4,
	HTMLAREAELEMENT_HREF=-5,
	HTMLAREAELEMENT_NOHREF=-6,
	HTMLAREAELEMENT_SHAPE=-7,
	HTMLAREAELEMENT_TABINDEX=-8,
	HTMLAREAELEMENT_TARGET=-9
};

/**
 * Call this method when getting a Property from HTMLArea
 */
static JSBool jhtml_area_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLArea *pArea = (pElem)? (HTMLArea *) pElem : 0;

	if (!pArea)
		return JS_TRUE;

	switch (JSVAL_TO_INT(id))
	{
		case HTMLAREAELEMENT_ACCESSKEY:
		{
			char ak[2];
			ak[0] = (char)pArea->AccessKey();
			ak[1] = '\0';
			JSString *str = JS_NewStringCopyZ(cx,ak);
			*vp = STRING_TO_JSVAL(str);
			return JS_TRUE;
		}
		case HTMLAREAELEMENT_ALT:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pArea->Alt()));
			return JS_TRUE;
		}
		case HTMLAREAELEMENT_COORDS:
			{
				WEBC_CHAR *pCoordString = (WEBC_CHAR *)HTML_PrintLengthList(pArea->Coordinates(), pArea->CoordinateCount());
				if(pCoordString)
				{
					*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pCoordString));
					WEBC_FREE(pCoordString);
					return JS_TRUE;
				}
			}
			break;
		case HTMLAREAELEMENT_HREF:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pArea->HRef()));
			return JS_TRUE;
		}
		case HTMLAREAELEMENT_NOHREF:
		{
			if (pArea->NoHRef())
			{
				*vp = BOOLEAN_TO_JSVAL(JS_TRUE);
			}
			else
			{
				*vp = BOOLEAN_TO_JSVAL(JS_FALSE);
			}
			return JS_TRUE;
		}
		case HTMLAREAELEMENT_SHAPE:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pArea->ShapeStr()));
			return JS_TRUE;
		}

		case HTMLAREAELEMENT_TABINDEX:
		{
			int t = pArea->TabIndex();
			*vp = INT_TO_JSVAL(t);
			return JS_TRUE;
		}
		case HTMLAREAELEMENT_TARGET:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pArea->Target()));
			return JS_TRUE;
		}
		default:
			break;
	}//end switch
	return JS_TRUE;
}

/**
 * This method is called when setting a Property in HTMLArea
 */
static JSBool jhtml_area_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLArea *pArea = (pElem)? (HTMLArea *) pElem : 0;

	if (!pArea)
		return JS_TRUE;

	switch (JSVAL_TO_INT(id))
	{
		case HTMLAREAELEMENT_ACCESSKEY:
		{
			char *str = JS_GetStringBytes(JS_ValueToString(cx, *vp));
			if (str && *str)
			{
				pArea->SetAccessKey((WEBC_UINT16)str[0]);
			}
			return JS_TRUE;
		}
		case HTMLAREAELEMENT_ALT:
		{
			pArea->SetAlt(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
			return JS_TRUE;
		}
		case HTMLAREAELEMENT_COORDS:
		{
			pArea->SetCoordinates(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
			return JS_TRUE;
		}
		case HTMLAREAELEMENT_HREF:
		{
			pArea->SetHRef(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
			return JS_TRUE;
		}
		case HTMLAREAELEMENT_NOHREF:
		{
			int noHref = 0;
			if (JSVAL_IS_BOOLEAN(*vp))
			{
				if (JSVAL_TO_BOOLEAN(*vp) == JS_TRUE)
				{
					noHref = 1;
				}
				pArea->SetNoHRef(noHref);
				return JS_TRUE;
			}
		}
		break;
		case HTMLAREAELEMENT_SHAPE:
			break;
		case HTMLAREAELEMENT_TABINDEX:
			if (JSVAL_IS_INT(*vp))
			{
				int32 tab = -1;
				JS_ValueToInt32(cx, *vp, &tab);
				pArea->SetTabIndex((short)tab);
			}
			break;
		case HTMLAREAELEMENT_TARGET:
		{
			pArea->SetTarget(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
			return JS_TRUE;
		}

		default:
			break;
	}//end switch
	return JS_TRUE;
}

/**
 * This JSClass is the javascript wrapper for the HTMLArea class defined in the DOM Level 1
 */
JSClass JSHtmlAreaElement =
{
	"HTMLAreaElement", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty,
	(JSPropertyOp) jutil_delProperty, (JSPropertyOp) jhtml_area_getProperty,
	(JSPropertyOp) jhtml_area_setProperty, (JSEnumerateOp) jutil_enumerate,
	(JSResolveOp) jhtml_element_resolve, (JSConvertOp) jutil_convert,
	(JSFinalizeOp) jutil_finalize
};

JSPropertySpec jhtml_area_props[] = {
	{"accessKey", HTMLAREAELEMENT_ACCESSKEY, JSPROP_ENUMERATE},
	{"alt", HTMLAREAELEMENT_ALT, JSPROP_ENUMERATE},
	{"coords", HTMLAREAELEMENT_COORDS, JSPROP_ENUMERATE },
	{"href", HTMLAREAELEMENT_HREF, JSPROP_ENUMERATE },
	{"noHref", HTMLAREAELEMENT_NOHREF, JSPROP_ENUMERATE },
	{"shape", HTMLAREAELEMENT_SHAPE, JSPROP_ENUMERATE },
	{"tabIndex", HTMLAREAELEMENT_TABINDEX, JSPROP_ENUMERATE },
	{"target", HTMLAREAELEMENT_TARGET, JSPROP_ENUMERATE },
	{0}
};

JSFunctionSpec jhtml_area_methods[] =
{
	//all the methods that are needed by this class are defined in
	//JHTMLInputElement
	{0}
};

static JSBool JHTMLArea(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	HTMLArea *pArea;

	if (jsmgr)
	{
		HTMLTagParse tag[1];
		HTML_InitTagParse(tag, HTML_TAG_AREA);

		WEBC_NEW_VERBOSE(pArea, HTMLArea(tag, jsmgr->GetDocument()),"HTMLArea");
		if (pArea)
		{
			pArea->SetJSObject(obj);
			pArea->SetTagProperties(tag, jsmgr->GetDocument());
			jsmgr->AddNewElement(pArea);
		}
	}
	return JS_TRUE;
}

JSClass *getHtmlAreaElement()
{
	return &JSHtmlAreaElement;
}

JSObject *jhtml_area_Init(JSContext *cx, JSObject *proto)
{
	//initialize the HTMLAreaElement Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSHtmlAreaElement,
    				(JSNative) JHTMLArea,  0,
    				jhtml_area_props, jhtml_area_methods,
    				WEBC_NULL, WEBC_NULL);
}

#endif //(WEBC_SUPPORT_JSCRIPT)
