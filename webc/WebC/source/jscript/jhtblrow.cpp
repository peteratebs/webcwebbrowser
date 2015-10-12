/*
|  JHTBLROW.CPP -
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
#include "jhtblrow.h"
#include "jhutils.h"
#include "htable.hpp"
#include "htr.hpp"
#include "htd.hpp"
#if (WEBC_SUPPORT_JSCRIPT)

#include "wjscript.hpp"
#include "webcstr.h"
#include "htmldoc.hpp"
#include "objref.cpp"
#include "webcmem.h"
#include "jhcoll.h"
#include "jhelem.h"
#include "wjsdebug.h"

//set the unique ids for the properties of the tableRow class
enum jhtml_tablerow_tinyid
{
	HTMLTABLEROWELEMENT_ROWINDEX=-1,
	HTMLTABLEROWELEMENT_SECTIONROWINDEX=-2,
	HTMLTABLEROWELEMENT_ALIGN=-3,
	HTMLTABLEROWELEMENT_BGCOLOR=-4,
	HTMLTABLEROWELEMENT_CH=-5,
	HTMLTABLEROWELEMENT_CHOFF=-6,
	HTMLTABLEROWELEMENT_VALIGN=-7
};

JSBool jhtml_tablerow_resolve(JSContext *cx, JSObject *obj, jsval id)
{
#if (WEBC_SUPPORT_TABLES)
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLTableRow *pTableRow = (pElem)? (HTMLTableRow *) pElem : 0;
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	if (!pTableRow || !jsmgr)
		return JS_TRUE;

	if (JSVAL_IS_STRING(id))
	{
		WEBC_CHAR *name = WEBC_JS_STRING_TO_WEBC_STRING(JSVAL_TO_STRING(id));

		if (!webc_c_strcmp(name, "cells"))
		{
			jhtml_collection_New(jsmgr, obj, "cells", pTableRow, TABLEROW_CELLS);
			return JS_TRUE;
		}
		else
		{
			// sorry, we just don't have a property of this name
		}
	}
#endif // WEBC_SUPPORT_TABLES

    return (jhtml_element_resolve(cx, obj, id));
}

/**
 * This method is called when getting a Property from HTMLTableRowElement
 */
static JSBool jhtml_tablerow_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
  #if (WEBC_SUPPORT_TABLES)
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLTableRow *pTableRow = (pElem)? (HTMLTableRow *) pElem : 0;
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	if (!pTableRow || !jsmgr)
		return JS_TRUE;

	JSString *jsstr = 0;
	switch (JSVAL_TO_INT(id))
	{
		case HTMLTABLEROWELEMENT_ROWINDEX:
			*vp = INT_TO_JSVAL(pTableRow->RowIndex());
		case HTMLTABLEROWELEMENT_SECTIONROWINDEX:
			//this is not implimented in our browser
			break;
		case HTMLTABLEROWELEMENT_ALIGN:
			switch (pTableRow->GetTextAlign())
			{
				case TU_ALIGN_LEFT:
					jsstr = JS_InternString(cx, "left");
					break;
				case TU_ALIGN_RIGHT:
					jsstr = JS_InternString(cx, "right");
					break;
				case TU_ALIGN_CENTER:
					jsstr = JS_InternString(cx, "center");
					break;
//				case TU_ALIGN_JUSTIFY:
//					jsstr = JS_InternString(cx, "justify");
//					break;
			}
			if (!jsstr)
				jsstr = JS_InternString(cx, "undefined");
			*vp = STRING_TO_JSVAL(jsstr);
			return JS_TRUE;
		case HTMLTABLEROWELEMENT_BGCOLOR:
			//this has been deprecated
			break;
		case HTMLTABLEROWELEMENT_CH:
			//this is not implemented in our browser
			break;
		case HTMLTABLEROWELEMENT_CHOFF:
			//this is not implemented in our browser
			break;
		case HTMLTABLEROWELEMENT_VALIGN:
			switch (pTableRow->GetVAlign())
			{
				case TU_VALIGN_TOP:
					jsstr = JS_InternString(cx, "top");
					break;
				case TU_VALIGN_MIDDLE:
					jsstr = JS_InternString(cx, "center");
					break;
				case TU_VALIGN_BOTTOM:
					jsstr = JS_InternString(cx, "bottom");
					break;
				case TU_VALIGN_BASELINE:
					jsstr = JS_InternString(cx, "baseline");
					break;
				default:
					jsstr = JS_InternString(cx, "undefined");
					break;
			}
			*vp = STRING_TO_JSVAL(jsstr);
			return JS_TRUE;
	}//end switch
	return JS_TRUE;
  #else // WEBC_SUPPORT_TABLES
    return JS_FALSE;
  #endif // WEBC_SUPPORT_TABLES
}

/**
 * This method is called when setting a Property in HTMLTableRow
 */
static JSBool jhtml_tablerow_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
  #if (WEBC_SUPPORT_TABLES)
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLTableRow *pTableRow = (pElem)? (HTMLTableRow *) pElem : 0;
	if (!pTableRow)
		return JS_TRUE;

	switch (JSVAL_TO_INT(id))
	{
		case HTMLTABLEROWELEMENT_ROWINDEX:
			//this cannot be done with our browser
			break;
		case HTMLTABLEROWELEMENT_SECTIONROWINDEX:
			//this is not implimented in our browser
			break;
		case HTMLTABLEROWELEMENT_ALIGN:
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				WEBC_CHAR *str = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp));
				pTableRow->SetProperty(HTML_ATTRIB_ALIGN, str);
				pTableRow->StyleUpdateChildren();
				return JS_TRUE;
			}
			break;
		case HTMLTABLEROWELEMENT_BGCOLOR:
			//this has been deprecated
			break;
		case HTMLTABLEROWELEMENT_CH:
			//not needed
			break;
		case HTMLTABLEROWELEMENT_CHOFF:
			//not needed
			break;
		case HTMLTABLEROWELEMENT_VALIGN:
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				WEBC_CHAR *str = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp));
				pTableRow->SetProperty(HTML_ATTRIB_VALIGN, str);
				pTableRow->StyleUpdateChildren();
				return JS_TRUE;
			}
			break;
	}//end switch
	return JS_TRUE;
  #else // WEBC_SUPPORT_TABLES
    return JS_FALSE;
  #endif // WEBC_SUPPORT_TABLES
}

/**
 * InsertCell - insert a cell into this row
 *
 * params - int index - this index tells where to put the new cell.
 * return - HTMLElement - the newly added Cell
 */
JSBool jhtml_tablerow_insertCell(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
  #if (WEBC_SUPPORT_TABLES)
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLTableRow *ptrow = (pElem)? (HTMLTableRow *) pElem : 0;
	if (ptrow && (argc > 0 && JSVAL_IS_INT(argv[0])))
	{
		int32 index = 0;
		JS_ValueToInt32(cx, argv[0], &index);

		HTMLElement *pElem = ptrow->InsertCell(index);
		ptrow->Update(0,0);
		if (pElem)
		{
			*rval = OBJECT_TO_JSVAL(pElem->CreateJSObject());
		}
		else
		{
			*rval = JSVAL_NULL;
		}
	}
	return JS_TRUE;
  #else // WEBC_SUPPORT_TABLES
    return JS_FALSE;
  #endif // WEBC_SUPPORT_TABLES
}

/**
 * DeleteCell() - delete the specified cell from this row
 *
 * params - int index - the cell to be deleted
 * returns- void
 */
JSBool jhtml_tablerow_deleteCell(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
  #if (WEBC_SUPPORT_TABLES)
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLTableRow *ptrow = (pElem)? (HTMLTableRow *) pElem : 0;
	if (ptrow && (argc > 0 && JSVAL_IS_INT(argv[0])))
	{
		int32 index = 0;
		JS_ValueToInt32(cx, argv[0], &index);

		ptrow->DeleteCell(index);
		ptrow->Update(0,0);
	}
	*rval = JSVAL_VOID;
	return JS_TRUE;
  #else // WEBC_SUPPORT_TABLES
    return JS_FALSE;
  #endif // WEBC_SUPPORT_TABLES
}

/**
 * This JSClass is the javascript wrapper for the HTMLTableRow class defined in the DOM Level 1
 */
JSClass JSHtmlTableRowElement =
{
	"HTMLTableRowElement", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty,
	(JSPropertyOp) jutil_delProperty, (JSPropertyOp) jhtml_tablerow_getProperty,
	(JSPropertyOp) jhtml_tablerow_setProperty, (JSEnumerateOp) jutil_enumerate,
	(JSResolveOp) jhtml_tablerow_resolve, (JSConvertOp) jutil_convert,
	(JSFinalizeOp) jutil_finalize
};


/**
 * The properties for this class
 */
JSPropertySpec jhtml_tablerow_props[] = {
	{"rowIndex",        HTMLTABLEROWELEMENT_ROWINDEX,        JSPROP_ENUMERATE | JSPROP_READONLY},
	{"sectionRowIndex", HTMLTABLEROWELEMENT_SECTIONROWINDEX, JSPROP_ENUMERATE | JSPROP_READONLY},
	{"align",           HTMLTABLEROWELEMENT_ALIGN,           JSPROP_ENUMERATE},
	{"bgColor",         HTMLTABLEROWELEMENT_BGCOLOR,         JSPROP_ENUMERATE | JSPROP_READONLY},
	{"ch",              HTMLTABLEROWELEMENT_CH,              JSPROP_ENUMERATE},
	{"chOff",           HTMLTABLEROWELEMENT_CHOFF,           JSPROP_ENUMERATE},
	{"vAlign",          HTMLTABLEROWELEMENT_VALIGN,          JSPROP_ENUMERATE},
	{0}
};

/**
 * The methods for this class
 */
JSFunctionSpec jhtml_tablerow_methods[] =
{
	{"insertCell", (JSNative) jhtml_tablerow_insertCell, 1},
	{"deleteCell", (JSNative) jhtml_tablerow_deleteCell, 1},
	{0}
};

/**
 * The constructor for this class
 */
static JSBool JHTMLTableRow(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
  #if (WEBC_SUPPORT_TABLES)
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	HTMLTableRow *pTableRow;

	if (jsmgr)
	{
		HTMLTagParse tag[1];
		HTML_InitTagParse(tag, HTML_TAG_TR);

		WEBC_NEW_VERBOSE(pTableRow, HTMLTableRow(tag, jsmgr->GetDocument()),"JHTMLTableRow");
		if (pTableRow)
		{
			pTableRow->SetJSObject(obj);
			pTableRow->SetTagProperties(tag, jsmgr->GetDocument());
			jsmgr->AddNewElement(pTableRow);
		}
	}
	return JS_TRUE;
  #else // WEBC_SUPPORT_TABLES
    return JS_FALSE;
  #endif // WEBC_SUPPORT_TABLES
}

/**
 * This function returns a pointer to the JSClass for HTMLTableRowElement
 */
JSClass *getHtmlTableRowElement()
{
	return &JSHtmlTableRowElement;
}

/**
 * initialize this JSClass
 */
JSObject *jhtml_tablerow_Init(JSContext *cx, JSObject *proto)
{
	//initialize the HTMLTableRowElement Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSHtmlTableRowElement,
    				(JSNative) JHTMLTableRow, 0,
    				jhtml_tablerow_props, jhtml_tablerow_methods,
    				WEBC_NULL, WEBC_NULL);
}

#endif //(WEBC_SUPPORT_JSCRIPT)
