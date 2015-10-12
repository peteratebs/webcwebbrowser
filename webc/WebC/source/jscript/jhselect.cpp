/*
|  JHSELECT.CPP -
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
#include "jhselect.h"
#include "jhutils.h"
#if (WEBC_SUPPORT_JSCRIPT)

#include "hselect.hpp"
#include "hoption.hpp"
#include "wjscript.hpp"
#include "webcstr.h"
#include "htmlfind.hpp"
#include "htmldoc.hpp"
#include "objref.cpp"
#include "webcmem.h"
#include "jhcoll.h"
#include "jhelem.h"
#include "wjsdebug.h"

//set the unique ids for the properties of the select class
enum jhtml_select_tinyid
{
	HTMLSELECTELEMENT_TYPE=-1,
	HTMLSELECTELEMENT_SELECTEDINDEX=-2,
	HTMLSELECTELEMENT_VALUE=-3,
	HTMLSELECTELEMENT_LENGTH=-4,
	HTMLSELECTELEMENT_FORM=-5,
	HTMLSELECTELEMENT_OPTIONS=-6,
	HTMLSELECTELEMENT_DISABLED=-7,
	HTMLSELECTELEMENT_MULTIPLE=-8,
	HTMLSELECTELEMENT_NAME=-9,
	HTMLSELECTELEMENT_SIZE=-10,
	HTMLSELECTELEMENT_TABINDEX=-11
};

JSBool jhtml_select_resolve(JSContext *cx, JSObject *obj, jsval id)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLSelect *pSelect = (pElem)? (HTMLSelect *) pElem : 0;
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	if (!pSelect || !jsmgr)
		return JS_TRUE;

	if (JSVAL_IS_STRING(id))
	{
		WEBC_CHAR *name = WEBC_JS_STRING_TO_WEBC_STRING(JSVAL_TO_STRING(id));

		if (!webc_c_strcmp(name, "options"))
		{
			jhtml_collection_New(jsmgr, obj, "options", pSelect, SELECT_OPTIONS);
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
 * Call this method when getting a Property from HTMLSelectElement
 */
static JSBool jhtml_select_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLSelect *pSelect = (pElem)? (HTMLSelect *) pElem : 0;
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	if (!pSelect || !jsmgr)
	{
		return JS_TRUE;
	}

	switch (JSVAL_TO_INT(id))
	{
		case HTMLSELECTELEMENT_TYPE:
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, "select"));
			return JS_TRUE;

		case HTMLSELECTELEMENT_SELECTEDINDEX:
			*vp = INT_TO_JSVAL(pSelect->SelectedIndex());
			return JS_TRUE;

		case HTMLSELECTELEMENT_VALUE:
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pSelect->Value()));
			return JS_TRUE;

		case HTMLSELECTELEMENT_LENGTH:
			*vp = INT_TO_JSVAL(pSelect->NumOptions());
			return JS_TRUE;

		case HTMLSELECTELEMENT_MULTIPLE:
			*vp = BOOLEAN_TO_JSVAL(JS_FALSE);
			return JS_TRUE;

		case HTMLSELECTELEMENT_SIZE:
			//GMP look at PEG
			break;

		case HTMLSELECTELEMENT_DISABLED:
			*vp = BOOLEAN_TO_JSVAL(pSelect->Disabled());
			// not implemented
			break;

		case HTMLSELECTELEMENT_NAME:
			// not implemented
			break;

		case HTMLSELECTELEMENT_TABINDEX:
			// not implemented
			break;

		case HTMLSELECTELEMENT_FORM:
			// not implemented
			break;

		default:
			if (JSVAL_IS_INT(id))
			{
				// return the option at the given index
				HTMLNthOfTypeFinder finder(HTML_OPTION_ELEMENT, JSVAL_TO_INT(id));
				HTMLElement* optionElement = pSelect->FindElement(&finder);

				if (optionElement)
				{
					*vp = OBJECT_TO_JSVAL(optionElement->CreateJSObject());
				}
				else
				{
					*vp = JSVAL_NULL;
				}
			}
			break;
	}//end switch
	return JS_TRUE;
}

/**
 * This method is called when setting a Property in HTMLSelect
 */
static JSBool jhtml_select_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLSelect *pSelect = (pElem)? (HTMLSelect *) pElem : 0;
	if (!pSelect)
	{
		return (JS_TRUE);
	}

	switch (JSVAL_TO_INT(id))
	{
		case HTMLSELECTELEMENT_SELECTEDINDEX:
			if (JSVAL_IS_INT(*vp))
			{
				int32 index = 0;
				JS_ValueToInt32(cx, *vp, &index);
				pSelect->SetSelectedIndex((short)index);
				return JS_TRUE;
			}
			break;
		case HTMLSELECTELEMENT_VALUE:
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				pSelect->SetValue(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
				pSelect->Update(0,0);
				return JS_TRUE;
			}
			break;
		case HTMLSELECTELEMENT_DISABLED:
			if (JSVAL_IS_BOOLEAN(*vp))
			{
				pSelect->SetDisabled((JSVAL_TO_BOOLEAN(*vp) == JS_TRUE)? 1 : 0);
				return JS_TRUE;
			}
			break;
		case HTMLSELECTELEMENT_MULTIPLE:
			//we only support single select
			return JS_TRUE;
		case HTMLSELECTELEMENT_SIZE:
			//GMP ??????????
			break;
	}//end switch
	return JS_TRUE;
}

JSBool jhtml_select_add(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	//This is not implemented because we do not impliment OptGroup elements in the web browser
	return JS_FALSE;
}
JSBool jhtml_select_remove(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	//This is not implemented because we do not impliment OptGroup elements in the web browser
	return JS_FALSE;
}

/**
 * This JSClass is the javascript wrapper for the HTMLSelect class defined in the DOM Level 1
 */
JSClass JSHtmlSelectElement =
{
	"Select", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty,
	(JSPropertyOp) jutil_delProperty, (JSPropertyOp) jhtml_select_getProperty,
	(JSPropertyOp) jhtml_select_setProperty, (JSEnumerateOp) jutil_enumerate,
	(JSResolveOp) jhtml_select_resolve, (JSConvertOp) jutil_convert,
	(JSFinalizeOp) jutil_finalize
};



JSPropertySpec jhtml_select_props[] = {
	{"type",          HTMLSELECTELEMENT_TYPE,          JSPROP_ENUMERATE},
	{"selectedIndex", HTMLSELECTELEMENT_SELECTEDINDEX, JSPROP_ENUMERATE},
	{"value",         HTMLSELECTELEMENT_VALUE,         JSPROP_ENUMERATE},
	{"length",        HTMLSELECTELEMENT_LENGTH,        JSPROP_ENUMERATE},
	{"multiple",      HTMLSELECTELEMENT_MULTIPLE,      JSPROP_READONLY | JSPROP_ENUMERATE},
	{"size",          HTMLSELECTELEMENT_SIZE,          JSPROP_ENUMERATE},
	{0}
};

JSFunctionSpec jhtml_select_methods[] =
{
//	{"add",    jhtml_select_add,    2},
//	{"remove", jhtml_select_remove, 1},
	{0}
};

static JSBool JHTMLSelect(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	HTMLSelect *pSelect;

	HTMLTagParse tag[1];
	HTML_InitTagParse(tag, HTML_TAG_SELECT);

	WEBC_NEW_VERBOSE(pSelect, HTMLSelect(tag, jsmgr->GetDocument()),"JHTMLSelect");
	if (pSelect)
	{
		pSelect->SetJSObject(obj);
		pSelect->SetTagProperties(tag, jsmgr->GetDocument());
		jsmgr->AddNewElement(pSelect);
	}
	return JS_TRUE;
}

JSClass *getHtmlSelectElement()
{
	return &JSHtmlSelectElement;
}

JSObject *jhtml_select_Init(JSContext *cx, JSObject *proto)
{
	//initialize the HTMLSelectElement Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSHtmlSelectElement,
    				(JSNative) JHTMLSelect, 0,
    				jhtml_select_props, jhtml_select_methods,
    				WEBC_NULL, WEBC_NULL);
}

#endif //(WEBC_SUPPORT_JSCRIPT)
