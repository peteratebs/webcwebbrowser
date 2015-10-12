/*
|  JHFORM.CPP -
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
#include "jhform.h"
#include "jhutils.h"
#if (WEBC_SUPPORT_JSCRIPT)

#include "hform.hpp"
#include "htmlfind.hpp"
#include "htmldoc.hpp"
#include "wjscript.hpp"
#include "webcstr.h"
#include "objref.cpp"
#include "jhcoll.h"
#include "jhelem.h"
#include "wjsdebug.h"

//set the unique ids for the properties of the form element class
enum jhtml_form_tinyid
{
	HTMLFORMELEMENT_ELEMENTS=-1,
	HTMLFORMELEMENT_LENGTH=-2,
	HTMLFORMELEMENT_NAME=-3,
	HTMLFORMELEMENT_ACCEPTCHARSET=-4,
	HTMLFORMELEMENT_ACTION=-5,
	HTMLFORMELEMENT_ENCTYPE=-6,
	HTMLFORMELEMENT_METHOD=-7,
	HTMLFORMELEMENT_TARGET=-8,
	HTMLFORMELEMENT_NUMIDS=-9
};

JSBool jhtml_form_resolve(JSContext *cx, JSObject *obj, jsval id)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLForm *form = (pElem)? (HTMLForm *) pElem : 0;
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	if (!form || !jsmgr)
		return JS_TRUE;

	if (JSVAL_IS_STRING(id))
	{
		WEBC_CHAR *name = WEBC_JS_STRING_TO_WEBC_STRING(JSVAL_TO_STRING(id));
		if (!webc_c_strcmp(name, "elements"))
		{
			jhtml_collection_New(jsmgr, obj, "elements", form, FORM_INPUTS);
			return JS_TRUE;
		}
		else
		{
			HTMLElement *pElem = 0;

			// try to find a field whose name is <name> in this form
			HTMLElementByNameFinder f(name);
			HTMLElementByIdFinder fid(name);
			pElem = form->FindField(&f);

			// try by ID
			if(!pElem)
				pElem = form->FindField(&fid);

			if (pElem)
			{
				// found it; define a property for this object
			  #if (WEBC_SUPPORT_UNICODE)
				JS_DefineUCProperty(cx, obj, name, webc_strlen(name),
				                  OBJECT_TO_JSVAL(pElem->CreateJSObject()),
				                  0 /*getter*/, 0 /*setter*/, JSPROP_READONLY);
			  #else
				JS_DefineProperty(cx, obj, name, OBJECT_TO_JSVAL(pElem->CreateJSObject()),
				                  0 /*getter*/, 0 /*setter*/, JSPROP_READONLY);
			  #endif
				return JS_TRUE;
			}
		}
	}

    return (jhtml_element_resolve(cx, obj, id));
}

/**
 * Call this method when getting a Property from HTMLFormElement
 */
static JSBool jhtml_form_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLForm *form = (pElem)? (HTMLForm *) pElem : 0;
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	if (!form || !jsmgr)
		return JS_TRUE;

	switch (JSVAL_TO_INT(id))
	{
		case HTMLFORMELEMENT_LENGTH:
		{
			int size = vector_get_size(form->GetFieldVector());
			*vp = INT_TO_JSVAL(size);
			return JS_TRUE;
		}
		case HTMLFORMELEMENT_NAME:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, form->Name()));
			return JS_TRUE;
		}
		case HTMLFORMELEMENT_ACCEPTCHARSET:
		{
			const char * pStrCharSet = webc_charset_string(form->AcceptCharset());
			//*vp = STRING_TO_JSVAL(JS_InternString(cx, "unknown"));
			*vp = STRING_TO_JSVAL(JS_InternString(cx, pStrCharSet));
			return JS_TRUE;
		}
		case HTMLFORMELEMENT_ACTION:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, form->Action()));
			return JS_TRUE;
		}
		case HTMLFORMELEMENT_ENCTYPE:
		{
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, form->EncType())); // always char*
			return JS_TRUE;
		}
		case HTMLFORMELEMENT_METHOD:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, form->Method()));
			return JS_TRUE;
		}
		case HTMLFORMELEMENT_TARGET:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, form->Target()));
			return JS_TRUE;
		}
	}
    return JS_TRUE;
}

/**
 * This method is called when setting a Property in HTMLFormElement
 */
static JSBool jhtml_form_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLForm *form = (pElem)? (HTMLForm *) pElem : 0;

	if (!form)
		return JS_TRUE;

	switch (JSVAL_TO_INT(id))
	{
		case HTMLFORMELEMENT_NAME:
		{
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				form->SetName(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
				return JS_TRUE;
			}
			break;
		}
		case HTMLFORMELEMENT_ACCEPTCHARSET:
		{
			form->SetAcceptCharset(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
			return JS_TRUE;
		}
		case HTMLFORMELEMENT_ACTION:
		{
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				form->SetAction(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
				return JS_TRUE;
			}
			break;
		}
		case HTMLFORMELEMENT_ENCTYPE:
		{
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				form->SetEncType(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
				return JS_TRUE;
			}
		}
		case HTMLFORMELEMENT_METHOD:
		{
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				form->SetMethod(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
				return JS_TRUE;
			}
			break;
		}
		case HTMLFORMELEMENT_TARGET:
		{
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				form->SetTarget(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
				return JS_TRUE;
			}
			break;
		}
		default:
			break;
	}
    return JS_TRUE;
}//end jhtml_form_setProperty

/**
 * This JSClass is the javascript wrapper for the HTMLFormElement class defined in the DOM Level 1
 */
JSClass JSHtmlFormElement =
{
	"HTMLFormElement", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty,
	(JSPropertyOp) jutil_delProperty, (JSPropertyOp) jhtml_form_getProperty,
	(JSPropertyOp) jhtml_form_setProperty, (JSEnumerateOp) jutil_enumerate,
	(JSResolveOp) jhtml_form_resolve, (JSConvertOp) jutil_convert,
	(JSFinalizeOp) jutil_finalize
};



JSClass *getHtmlFormElement()
{
	return &JSHtmlFormElement;
}

/**
 * This method is executes the submit event
 *
 * params - none
 * return - void
 */
JSBool jhtml_form_submit(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	JSBool retval = JS_TRUE;
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLForm *pForm = (pElem)? (HTMLForm *) pElem : 0;
	if (pForm)
	{
		HTMLDocument *doc = pForm->GetDocument();
		if (doc)
		{
			if (pForm->Submit(0,0,0) == 1)
			{
				retval = JS_FALSE;
			}
		}
	}

	return (retval);
}

JSBool jhtml_form_reset (JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLForm *form = (pElem)? (HTMLForm *) pElem : 0;
	if (form)
	{
		form->Reset();
	}
	*rval = JSVAL_VOID;
	return JS_TRUE;
}

JSPropertySpec jhtml_form_props[] = {
	{"length",        HTMLFORMELEMENT_LENGTH,        JSPROP_ENUMERATE | JSPROP_READONLY},
	{"name",          HTMLFORMELEMENT_NAME,          JSPROP_ENUMERATE},
	{"acceptCharset", HTMLFORMELEMENT_ACCEPTCHARSET, JSPROP_ENUMERATE},
	{"action",        HTMLFORMELEMENT_ACTION,        JSPROP_ENUMERATE},
	{"enctype",       HTMLFORMELEMENT_ENCTYPE,       JSPROP_ENUMERATE},
	{"method",        HTMLFORMELEMENT_METHOD,        JSPROP_ENUMERATE},
	{"target",        HTMLFORMELEMENT_TARGET,        JSPROP_ENUMERATE | JSPROP_READONLY},
	{0}
};

JSFunctionSpec jhtml_form_methods[] =
{
	{"submit",  (JSNative) jhtml_form_submit, 1},
  	{"reset" ,  (JSNative) jhtml_form_reset,  1},
	{0}
};

static JSBool JHTMLFormElement(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	//HTMLForm *pForm tbd;

	if (jsmgr)
	{
		HTMLTagParse tag[1];
		HTML_InitTagParse(tag, HTML_TAG_FORM);

		//WEBC_NEW(pForm, HTMLForm(tag, jsmgr->GetDocument()),__FILE__,__LINE__);
		//if (pForm)
		//{
			// tbd
			//pForm->SetJSObject(obj);
			//jsmgr->AddNewElement(pForm);
		//}
	}
	return JS_TRUE;
}

JSObject *jhtml_form_Init(JSContext *cx, JSObject *proto)
{
	//initialize the HTMLFormElement Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSHtmlFormElement,
    				(JSNative) JHTMLFormElement, 0,
    				jhtml_form_props, jhtml_form_methods,
    				WEBC_NULL, WEBC_NULL);
}
#endif //(WEBC_SUPPORT_JSCRIPT)
