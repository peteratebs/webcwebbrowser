/*
|  JHBUTTON.CPP -
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
#include "jhbutton.h"
#include "jhutils.h"
#if (WEBC_SUPPORT_JSCRIPT)

#include "hbutton.hpp"
#include "wjscript.hpp"
#include "htmldoc.hpp"
#include "objref.cpp"
#include "webcmem.h"
#include "jhelem.h"
#include "wjsdebug.h"

//set the unique ids for the properties of thebutton class
enum jhtml_button_tinyid
{
	HTMLBUTTONELEMENT_DEFAULTVALUE=-1,
	HTMLBUTTONELEMENT_ACCESSKEY=-2,
	HTMLBUTTONELEMENT_DISABLED=-3,
	HTMLBUTTONELEMENT_FORM=-4,
	HTMLBUTTONELEMENT_NAME=-5,
	HTMLBUTTONELEMENT_TABINDEX=-6,
	HTMLBUTTONELEMENT_TYPE=-7,
	HTMLBUTTONELEMENT_VALUE=-8
};

/**
 * Call this method when getting a Property from HTMLButton
 */
static JSBool jhtml_button_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLButton *pButton = (pElem)? (HTMLButton *) pElem : 0;

	if (!pButton)
		return JS_TRUE;

	switch (JSVAL_TO_INT(id))
	{
		case HTMLBUTTONELEMENT_ACCESSKEY:
		{
			char ak[2];
			ak[0] = (char)pButton->AccessKey();
			ak[1] = '\0';
			JSString *str = JS_NewStringCopyZ(cx,ak);
			*vp = STRING_TO_JSVAL(str);
			return JS_TRUE;
		}
		case HTMLBUTTONELEMENT_DISABLED:
		{
			WEBC_BOOL bDisabled = pButton->Disabled();
			*vp = BOOLEAN_TO_JSVAL(bDisabled);
			return JS_TRUE;
		}
		case HTMLBUTTONELEMENT_FORM:
		{
			*vp = (pButton->Form())? OBJECT_TO_JSVAL(pButton->Form()->CreateJSObject()) : JSVAL_NULL;
			return JS_TRUE;
		}
		case HTMLBUTTONELEMENT_NAME:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pButton->Name()));
			return JS_TRUE;
		}
		case HTMLBUTTONELEMENT_TABINDEX:
		{
			int t = pButton->TabIndex();
			*vp = INT_TO_JSVAL(t);
			return JS_TRUE;
		}
		case HTMLBUTTONELEMENT_TYPE:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pButton->GetType()));
			return JS_TRUE;
		}
		case HTMLBUTTONELEMENT_VALUE:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pButton->Value()));
			return JS_TRUE;
		}
		default:
			break;
	}//end switch
	return JS_TRUE;
}

/**
 * This method is called when setting a Property in HTMLButton
 */
static JSBool jhtml_button_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLButton *pButton = (pElem)? (HTMLButton *) pElem : 0;

	if (!pButton)
		return JS_TRUE;

	switch (JSVAL_TO_INT(id))
	{
		case HTMLBUTTONELEMENT_ACCESSKEY:
		{
			char *str = JS_GetStringBytes(JS_ValueToString(cx, *vp));
			if (str && *str)
			{
				pButton->SetAccessKey((WEBC_UINT16)str[0]);
			}
			return JS_TRUE;
		}
		case HTMLBUTTONELEMENT_DISABLED:
		{
			if (JSVAL_IS_BOOLEAN(*vp))
			{
				pButton->SetDisabled((JSVAL_TO_BOOLEAN(*vp) == JS_TRUE)? 1 : 0);
				return JS_TRUE;
			}
		}
		break;

		case HTMLBUTTONELEMENT_NAME:
		{
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				pButton->SetName(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
				return JS_TRUE;
			}
			break;
		}

		case HTMLBUTTONELEMENT_TABINDEX:
			if (JSVAL_IS_INT(*vp))
			{
				int32 tab = -1;
				JS_ValueToInt32(cx, *vp, &tab);
				pButton->SetTabIndex((short)tab);
			}
			break;
	}//end switch
	return JS_TRUE;
}

/**
 * This JSClass is the javascript wrapper for the HTMLButton class defined in the DOM Level 1
 */
JSClass JSHtmlButtonElement =
{
	"HTMLButtonElement", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty,
	(JSPropertyOp) jutil_delProperty, (JSPropertyOp) jhtml_button_getProperty,
	(JSPropertyOp)jhtml_button_setProperty, (JSEnumerateOp) jutil_enumerate,
	(JSResolveOp) jhtml_element_resolve, (JSConvertOp) jutil_convert,
	(JSFinalizeOp) jutil_finalize
};




JSPropertySpec jhtml_button_props[] = {
	{"accessKey", HTMLBUTTONELEMENT_ACCESSKEY, JSPROP_ENUMERATE},
	{"disabled", HTMLBUTTONELEMENT_DISABLED, JSPROP_ENUMERATE},
	{"form", HTMLBUTTONELEMENT_FORM, JSPROP_ENUMERATE },
	{"name", HTMLBUTTONELEMENT_NAME, JSPROP_ENUMERATE },
	{"tabIndex", HTMLBUTTONELEMENT_TABINDEX, JSPROP_ENUMERATE },
	{"type", HTMLBUTTONELEMENT_TYPE, JSPROP_ENUMERATE },
	{"value", HTMLBUTTONELEMENT_VALUE, JSPROP_ENUMERATE },
	{0}
};

JSFunctionSpec jhtml_button_methods[] =
{
	//all the methods that are needed by this class are defined in
	//JHTMLInputElement
	{0}
};

static JSBool JHTMLButton(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	HTMLButton *pButton;

	if (jsmgr)
	{
		HTMLTagParse tag[1];
		HTML_InitTagParse(tag, HTML_TAG_BUTTON);

		WEBC_NEW_VERBOSE(pButton, HTMLButton(tag, jsmgr->GetDocument()),"HTMLButton");
		if (pButton)
		{
			pButton->SetJSObject(obj);
			pButton->SetTagProperties(tag, jsmgr->GetDocument());
			jsmgr->AddNewElement(pButton);
		}
	}
	return JS_TRUE;
}

JSClass *getHtmlButtonElement()
{
	return &JSHtmlButtonElement;
}

JSObject *jhtml_button_Init(JSContext *cx, JSObject *proto)
{
	//initialize the HTMLButtonElement Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSHtmlButtonElement,
    				(JSNative) JHTMLButton, 0,
    				jhtml_button_props, jhtml_button_methods,
    				WEBC_NULL, WEBC_NULL);
}

#endif //(WEBC_SUPPORT_JSCRIPT)


