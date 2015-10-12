/*
|  JHTAREA.CPP -
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
#include "jhtarea.h"
#include "jhutils.h"
#if (WEBC_SUPPORT_JSCRIPT)

#include "htxtarea.hpp"
#include "wjscript.hpp"
#include "htmldoc.hpp"
#include "objref.cpp"
#include "webcmem.h"
#include "jhelem.h"
#include "wjsdebug.h"

//set the unique ids for the properties of the textarea class
enum jhtml_textarea_tinyid
{
	HTMLTEXTAREAELEMENT_DEFAULTVALUE=-1,
	HTMLTEXTAREAELEMENT_FORM=-2,
	HTMLTEXTAREAELEMENT_ACCESSKEY=-3,
	HTMLTEXTAREAELEMENT_COLS=-4,
	HTMLTEXTAREAELEMENT_DISABLED=-5,
	HTMLTEXTAREAELEMENT_NAME=-6,
	HTMLTEXTAREAELEMENT_READONLY=-7,
	HTMLTEXTAREAELEMENT_ROWS=-8,
	HTMLTEXTAREAELEMENT_TABINDEX=-9,
	HTMLTEXTAREAELEMENT_TYPE=-10,
	HTMLTEXTAREAELEMENT_VALUE=-11
};

/**
 * Call this method when getting a Property from HTMLTextArea
 */
static JSBool jhtml_textarea_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLTextArea *pText = (pElem)? (HTMLTextArea *) pElem : 0;

	if (!pText)
		return JS_TRUE;

	switch (JSVAL_TO_INT(id))
	{
		case HTMLTEXTAREAELEMENT_COLS:
			*vp = INT_TO_JSVAL(pText->Cols());
			return JS_TRUE;
		case HTMLTEXTAREAELEMENT_ROWS:
			*vp = INT_TO_JSVAL(pText->Rows());
			return JS_TRUE;
		case HTMLTEXTAREAELEMENT_TYPE:
			*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, "text"));
			return JS_TRUE;
		default:
			break;
	}//end switch
	return JS_TRUE;
}

/**
 * This method is called when setting a Property in HTMLTextArea
 */
static JSBool jhtml_textarea_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLTextArea *pText = (pElem)? (HTMLTextArea *) pElem : 0;

	if (!pText)
		return JS_TRUE;

	switch (JSVAL_TO_INT(id))
	{
		case HTMLTEXTAREAELEMENT_COLS:
			if (JSVAL_IS_INT(*vp))
			{
				int32 cols;
				JS_ValueToInt32(cx, *vp, &cols);
				pText->SetCols(cols);
				pText->Update(0,0);
				return JS_TRUE;
			}
			break;
		case HTMLTEXTAREAELEMENT_ROWS:
			if (JSVAL_IS_INT(*vp))
			{
				int32 rows;
				JS_ValueToInt32(cx, *vp, &rows);
				pText->SetRows(rows);
				pText->Update(0,0);
				return JS_TRUE;
			}
			break;
		default:
			break;
	}//end switch
	return JS_TRUE;
}

/**
 * This JSClass is the javascript wrapper for the HTMLTextArea class defined in the DOM Level 1
 */
JSClass JSHtmlTextAreaElement =
{
	"HTMLTextAreaElement", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty,
	(JSPropertyOp) jutil_delProperty, (JSPropertyOp) jhtml_textarea_getProperty,
	(JSPropertyOp) jhtml_textarea_setProperty, (JSEnumerateOp) jutil_enumerate,
	(JSResolveOp) jhtml_element_resolve, (JSConvertOp) jutil_convert,
	(JSFinalizeOp) jutil_finalize
};


JSPropertySpec jhtml_textarea_props[] = {
	{"cols", HTMLTEXTAREAELEMENT_COLS, JSPROP_ENUMERATE},
	{"rows", HTMLTEXTAREAELEMENT_ROWS, JSPROP_ENUMERATE},
	{"type", HTMLTEXTAREAELEMENT_TYPE, JSPROP_ENUMERATE | JSPROP_READONLY},
	{0}
};

JSFunctionSpec jhtml_textarea_methods[] =
{
	//all the methods that are needed by this class are defined in
	//JHTMLInputElement
	{0}
};

static JSBool JHTMLTextArea(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	HTMLTextArea *pEditbox;

	if (jsmgr)
	{
		HTMLTagParse tag[1];
		HTML_InitTagParse(tag, HTML_TAG_TEXTAREA);

		WEBC_NEW_VERBOSE(pEditbox, HTMLTextArea(tag, jsmgr->GetDocument()),"HTMLTextArea");
		if (pEditbox)
		{
			pEditbox->SetJSObject(obj);
			pEditbox->SetTagProperties(tag, jsmgr->GetDocument());
			jsmgr->AddNewElement(pEditbox);
		}
	}
	return JS_TRUE;
}

JSClass *getHtmlTextAreaElement()
{
	return &JSHtmlTextAreaElement;
}

JSObject *jhtml_textarea_Init(JSContext *cx, JSObject *proto)
{
	//initialize the HTMLTextAreaElement Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSHtmlTextAreaElement,
    				(JSNative) JHTMLTextArea, 0,
    				jhtml_textarea_props, jhtml_textarea_methods,
    				WEBC_NULL, WEBC_NULL);
}

#endif //(WEBC_SUPPORT_JSCRIPT)
