/*
|  JHOPTION.CPP -
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
#include "jhoption.h"
#include "jhutils.h"
#if (WEBC_SUPPORT_JSCRIPT)

#include "hselect.hpp"
#include "hoption.hpp"
#include "hform.hpp"
#include "wjscript.hpp"
#include "htmldoc.hpp"
#include "objref.cpp"
#include "webcmem.h"
#include "jhelem.h"
#include "wjsdebug.h"

//set the unique ids for the properties of the option class
enum jhtml_option_id
{
	HTMLOPTIONELEMENT_FORM=-1,
	HTMLOPTIONELEMENT_DEFAULTSELECTED=-2,
	HTMLOPTIONELEMENT_TEXT=-3,
	HTMLOPTIONELEMENT_INDEX=-4,
	HTMLOPTIONELEMENT_DISABLED=-5,
	HTMLOPTIONELEMENT_LABEL=-6,
	HTMLOPTIONELEMENT_SELECTED=-7,
	HTMLOPTIONELEMENT_VALUE=-8
};

/**
 * Call this method when getting a Property from HTMLOptionElement
 */
static JSBool jhtml_option_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLOption *pOption = (pElem)? (HTMLOption *) pElem : 0;

	if (!pOption)
	{
		return JS_TRUE;
	}

	//Get this Option's Select Element
	HTMLSelect *pSelect = pOption->GetSelectElement();
	switch (JSVAL_TO_INT(id))
	{
		case HTMLOPTIONELEMENT_DEFAULTSELECTED:
		{
			JSBool ans = (pOption->DefSelected())? JS_TRUE : JS_FALSE;
			*vp = BOOLEAN_TO_JSVAL(ans);
			return JS_TRUE;
		}

		case HTMLOPTIONELEMENT_SELECTED:
		{
			JSBool ans = (pSelect && (pOption == pSelect->GetOptionIndex(pSelect->SelectedIndex())))? JS_TRUE : JS_FALSE;
			*vp = BOOLEAN_TO_JSVAL(ans);
			return JS_TRUE;
		}

		case HTMLOPTIONELEMENT_TEXT:
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pOption->Text()));
			return JS_TRUE;

		case HTMLOPTIONELEMENT_INDEX:
			*vp = INT_TO_JSVAL(pOption->GetIndex());
			return JS_TRUE;

		case HTMLOPTIONELEMENT_LABEL:
			//we don't support labels
			break;

		case HTMLOPTIONELEMENT_FORM:
			*vp = (pSelect->Form())? OBJECT_TO_JSVAL(pSelect->Form()->CreateJSObject()) : JSVAL_NULL;
			break;

		case HTMLOPTIONELEMENT_DISABLED:
			// not implemented
			break;

		case HTMLOPTIONELEMENT_VALUE:
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pOption->Value()));
			return JS_TRUE;

	}//end switch

	return JS_TRUE;
}

/**
 * This method is called when setting a Property in HTMLOption
 */
static JSBool jhtml_option_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLOption *pOption = (pElem)? (HTMLOption *) pElem : 0;
	if (!pOption)
	{
		return JS_TRUE;
	}

	//Get this Option's Select Element
	HTMLSelect *pSelect = pOption->GetSelectElement();

	switch (JSVAL_TO_INT(id))
	{
		case HTMLOPTIONELEMENT_DEFAULTSELECTED:
			if (JSVAL_IS_BOOLEAN(*vp))
			{
				if (JSVAL_TO_BOOLEAN(*vp) == JS_TRUE)
				{
					if (pSelect)
					{
						pSelect->SetDefSelected(pOption->GetIndex());
					}
				}
			}
			break;

		case HTMLOPTIONELEMENT_SELECTED:
			if (JSVAL_IS_BOOLEAN(*vp))
			{
				if (JSVAL_TO_BOOLEAN(*vp) == JS_TRUE)
				{
					if (pSelect)
					{
						pSelect->SetSelectedIndex(pOption->GetIndex());
						pSelect->Refresh(WEBC_FALSE);
					}
				}
			}
			break;

		case HTMLOPTIONELEMENT_TEXT:
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				pOption->SetText(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
				if (pSelect)
				{
					pSelect->StyleUpdate();
				}
			}
			break;

		case HTMLOPTIONELEMENT_INDEX:
			if (JSVAL_IS_INT(*vp))
			{
				//GMP this will take some work. Talk to FT
			}
			break;

		case HTMLOPTIONELEMENT_VALUE:
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				pOption->SetValue(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx,*vp)));
			}
			break;
	}//end switch

	return JS_TRUE;
}

/**
 * This JSClass is the javascript wrapper for the HTMLOption class defined in the DOM Level 1
 */
JSClass JSHtmlOptionElement =
{
	"Option", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty,
	(JSPropertyOp) jutil_delProperty, (JSPropertyOp) jhtml_option_getProperty,
	(JSPropertyOp) jhtml_option_setProperty, (JSEnumerateOp) jutil_enumerate,
	(JSResolveOp) jhtml_element_resolve, (JSConvertOp) jutil_convert,
	(JSFinalizeOp) jutil_finalize
};



/**
 * HTMLOption Properties
 */
JSPropertySpec jhtml_option_props[] = {
	{"form",            HTMLOPTIONELEMENT_FORM,            JSPROP_READONLY | JSPROP_ENUMERATE},
	{"defaultSelected", HTMLOPTIONELEMENT_DEFAULTSELECTED, JSPROP_ENUMERATE},
	{"text",            HTMLOPTIONELEMENT_TEXT,            JSPROP_ENUMERATE},
	{"index",           HTMLOPTIONELEMENT_INDEX,           JSPROP_ENUMERATE},
	{"disabled",        HTMLOPTIONELEMENT_DISABLED,        JSPROP_ENUMERATE},
	{"label",           HTMLOPTIONELEMENT_LABEL,           JSPROP_ENUMERATE},
	{"selected",        HTMLOPTIONELEMENT_SELECTED,        JSPROP_READONLY | JSPROP_ENUMERATE},
	{"value",           HTMLOPTIONELEMENT_VALUE,           JSPROP_ENUMERATE},
	{0}
};

/**
 * HTMLoption Methods
 *
 * This class has no Methods
 */
JSFunctionSpec jhtml_option_methods[] =
{
	{0}
};

static JSBool JHTMLOption(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	HTMLOption *pOption;
	if (jsmgr)
	{
		HTMLTagParse tag[1];
		HTML_InitTagParse(tag, HTML_TAG_OPTION);

		WEBC_NEW_VERBOSE(pOption, HTMLOption(tag, jsmgr->GetDocument()),"HTMLOption");
		if (pOption)
		{
			pOption->SetJSObject(obj);
			pOption->SetTagProperties(tag, jsmgr->GetDocument());

			if (argc > 0)
			{
				if (JSVAL_IS_STRING(argv[0]))
				{
					pOption->SetText(WEBC_JS_STRING_TO_WEBC_STRING(JSVAL_TO_STRING(argv[0])));
				}

				if (argc > 1)
				{
					if (JSVAL_IS_STRING(argv[1]))
					{
						pOption->SetValue(WEBC_JS_STRING_TO_WEBC_STRING(JSVAL_TO_STRING(argv[1])));
					}
				}
			}

			jsmgr->AddNewElement(pOption);
		}
	}
	return JS_TRUE;
}

JSClass *getHtmlOptionElement()
{
	return &JSHtmlOptionElement;
}

JSObject *jhtml_option_Init(JSContext *cx, JSObject *proto)
{
	//initialize the HTMLOptionElement Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSHtmlOptionElement,
    				(JSNative) JHTMLOption, 1,
    				jhtml_option_props, jhtml_option_methods,
    				WEBC_NULL, WEBC_NULL);
}

#endif //(WEBC_SUPPORT_JSCRIPT)
