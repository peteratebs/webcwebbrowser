/*
|  JHTCELL.CPP -
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
#include "jhtcell.h"
#include "jhutils.h"

#if (WEBC_SUPPORT_JSCRIPT)

#include "htd.hpp"
#include "htable.hpp"
#include "wjscript.hpp"
#include "htmldoc.hpp"
#include "objref.cpp"
#include "webcmem.h"
#include "jhelem.h"
#include "jhcoll.h"
#include "wjsdebug.h"

//set the unique ids for the properties of the table class
enum jhtml_tablecell_tinyid
{
	HTMLTABLECELLELEMENT_CELLINDEX=-1,
	HTMLTABLECELLELEMENT_ABBR=-2,
	HTMLTABLECELLELEMENT_ALIGN=-3,
	HTMLTABLECELLELEMENT_AXIS=-4,
	HTMLTABLECELLELEMENT_BGCOLOR=-5,
	HTMLTABLECELLELEMENT_CH=-6,
	HTMLTABLECELLELEMENT_CHOFF=-7,
	HTMLTABLECELLELEMENT_COLSPAN=-8,
	HTMLTABLECELLELEMENT_HEADERS=-9,
	HTMLTABLECELLELEMENT_HEIGHT=-10,
	HTMLTABLECELLELEMENT_NOWRAP=-11,
	HTMLTABLECELLELEMENT_ROWSPAN=-12,
	HTMLTABLECELLELEMENT_SCOPE=-13,
	HTMLTABLECELLELEMENT_VALIGN=-14,
	HTMLTABLECELLELEMENT_WIDTH=-15
};

/**
 * Call this method when getting a Property from HTMLTableCellElement
 */
static JSBool jhtml_tablecell_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
  #if (WEBC_SUPPORT_TABLES)
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLTableCell *pTableCell = (pElem)? (HTMLTableCell *) pElem : 0;

	if (!pTableCell)
	{
		return JS_TRUE;
	}

	JSString *jsstr = 0;
	switch (JSVAL_TO_INT(id))
	{
		case HTMLTABLECELLELEMENT_CELLINDEX:
		{
			HTMLTable* tableElement = pTableCell->GetTable();
			if (tableElement)
			{
				tableElement->CalculateCellPositions();
			}
			*vp = INT_TO_JSVAL(pTableCell->ColIndex());
			return JS_TRUE;
		}

		case HTMLTABLECELLELEMENT_ABBR:
			//not implimented in our browser
			break;

		case HTMLTABLECELLELEMENT_ALIGN:
			switch (pTableCell->GetTextAlign())
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
			{
				jsstr = JS_InternString(cx, "undefined");
			}

			*vp = STRING_TO_JSVAL(jsstr);

			return JS_TRUE;

		case HTMLTABLECELLELEMENT_AXIS:
			//this is not implimented in our browser
			break;

		case HTMLTABLECELLELEMENT_BGCOLOR:
			//this attribute is deprecated
			break;

		case HTMLTABLECELLELEMENT_CH:
			//this is not implimented in our browser
			break;

		case HTMLTABLECELLELEMENT_CHOFF:
			//this is not implimented in our browser
			break;

		case HTMLTABLECELLELEMENT_COLSPAN:
			*vp = INT_TO_JSVAL(pTableCell->ColSpan());
			return JS_TRUE;

		case HTMLTABLECELLELEMENT_HEADERS:
			//this is not implimented in our browser
			break;

		case HTMLTABLECELLELEMENT_HEIGHT:
			//this attribute is deprecated in HTML 4.0
			break;

		case HTMLTABLECELLELEMENT_NOWRAP:
			//this is not implimented in our browser
			break;

		case HTMLTABLECELLELEMENT_ROWSPAN:
			*vp = INT_TO_JSVAL(pTableCell->RowSpan());
			return JS_TRUE;

		case HTMLTABLECELLELEMENT_SCOPE:
			//this is not implimented in our browser
			break;

		case HTMLTABLECELLELEMENT_VALIGN:
			switch (pTableCell->GetVAlign())
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
					jsstr = JS_InternString(cx, "bottom");
					break;
			}

			if (!jsstr)
			{
				jsstr = JS_InternString(cx, "undefined");
			}

			*vp = STRING_TO_JSVAL(jsstr);
			return JS_TRUE;

		case HTMLTABLECELLELEMENT_WIDTH:
			//this attribute is deprecated in HTML 4.0
			break;
	} //end switch

	return JS_TRUE;

  #else // WEBC_SUPPORT_TABLES
    return JS_FALSE;
  #endif // WEBC_SUPPORT_TABLES
}

/**
 * This method is called when setting a Property in HTMLTable
 */
static JSBool jhtml_tablecell_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
  #if (WEBC_SUPPORT_TABLES)
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLTableCell *pTableCell = (pElem)? (HTMLTableCell *) pElem : 0;

	if (!pTableCell)
		return JS_TRUE;

	switch (JSVAL_TO_INT(id))
	{
		case HTMLTABLECELLELEMENT_CELLINDEX:
			//GMP this is crazy!
			break;
		case HTMLTABLECELLELEMENT_ABBR:
			//not implimented in browser
			break;
		case HTMLTABLECELLELEMENT_ALIGN:
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				WEBC_CHAR *str = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp));
				pTableCell->SetProperty(HTML_ATTRIB_ALIGN, str);
				pTableCell->StyleUpdate();
				return JS_TRUE;
			}
			break;
		case HTMLTABLECELLELEMENT_AXIS:
			//this is not implimented in our browser
			break;
		case HTMLTABLECELLELEMENT_BGCOLOR:
			//this attribute is deprecated in HTML 4.0
			break;
		case HTMLTABLECELLELEMENT_CH:
			//this is not implimented in our browser
			break;
		case HTMLTABLECELLELEMENT_CHOFF:
			//this is not implimented in our browser
			break;
		case HTMLTABLECELLELEMENT_COLSPAN:
			if (JSVAL_IS_INT(*vp))
			{
				int32 cspan = 0;
				if  (JS_ValueToInt32(cx, *vp, &cspan) == JS_TRUE)
				{
					pTableCell->SetColSpan((int)cspan);
					pTableCell->StyleUpdate();
					return JS_TRUE;
				}
			}
			break;
		case HTMLTABLECELLELEMENT_HEADERS:
			//this is not implimented in our browser
			break;
		case HTMLTABLECELLELEMENT_HEIGHT:
			//this attribute is deprecated in HTML 4.0
			break;
		case HTMLTABLECELLELEMENT_NOWRAP:
			//this is not implimented in our browser
			break;
		case HTMLTABLECELLELEMENT_ROWSPAN:
			if (JSVAL_IS_NUMBER(*vp))
			{
				int32 rspan = 0;
				if  (JS_ValueToInt32(cx, *vp, &rspan) == JS_TRUE)
				{
					pTableCell->SetRowSpan((int)rspan);
					return JS_TRUE;
				}
			}
			break;
		case HTMLTABLECELLELEMENT_SCOPE:
			//this is not implimented in our browser
			break;
		case HTMLTABLECELLELEMENT_VALIGN:
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				WEBC_CHAR *str = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp));
				pTableCell->SetProperty(HTML_ATTRIB_VALIGN, str);
				pTableCell->StyleUpdate();
				return JS_TRUE;
			}
			break;
		case HTMLTABLECELLELEMENT_WIDTH:
			//this attribute is deprecated in HTML 4.0
			break;
	}//end switch
	return JS_TRUE;
  #else // WEBC_SUPPORT_TABLES
    return JS_FALSE;
  #endif // WEBC_SUPPORT_TABLES
}

/**
 * This JSClass is the javascript wrapper for the HTMLTableCell class defined in the DOM Level 1
 */
JSClass JSHtmlTableCellElement =
{
	"HTMLTableCellElement", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty,
	(JSPropertyOp) jutil_delProperty, (JSPropertyOp) jhtml_tablecell_getProperty,
	(JSPropertyOp) jhtml_tablecell_setProperty, (JSEnumerateOp) jutil_enumerate,
	(JSResolveOp) jhtml_element_resolve, (JSConvertOp) jutil_convert,
	(JSFinalizeOp) jutil_finalize
};


JSPropertySpec jhtml_tablecell_props[] = {
	{"cellIndex", HTMLTABLECELLELEMENT_CELLINDEX, JSPROP_ENUMERATE | JSPROP_READONLY},
//	{"abbr",      HTMLTABLECELLELEMENT_ABBR,      JSPROP_ENUMERATE},
	{"align",     HTMLTABLECELLELEMENT_ALIGN,     JSPROP_ENUMERATE},
//	{"axis",      HTMLTABLECELLELEMENT_AXIS,      JSPROP_ENUMERATE},
//	{"bgColor",   HTMLTABLECELLELEMENT_BGCOLOR,   JSPROP_ENUMERATE},
//	{"ch",        HTMLTABLECELLELEMENT_CH,        JSPROP_ENUMERATE},
//	{"chOff",     HTMLTABLECELLELEMENT_CHOFF,     JSPROP_ENUMERATE},
	{"colSpan",   HTMLTABLECELLELEMENT_COLSPAN,   JSPROP_ENUMERATE},
//	{"headers",   HTMLTABLECELLELEMENT_HEADERS,   JSPROP_ENUMERATE},
//	{"height",    HTMLTABLECELLELEMENT_HEIGHT,    JSPROP_ENUMERATE},
//	{"noWrap",    HTMLTABLECELLELEMENT_NOWRAP,    JSPROP_ENUMERATE},
	{"rowSpan",   HTMLTABLECELLELEMENT_ROWSPAN,   JSPROP_ENUMERATE},
//	{"scope",     HTMLTABLECELLELEMENT_SCOPE,     JSPROP_ENUMERATE},
	{"vAlign",    HTMLTABLECELLELEMENT_VALIGN,    JSPROP_ENUMERATE},
//	{"width",     HTMLTABLECELLELEMENT_WIDTH,     JSPROP_ENUMERATE},
	{0}
};

JSFunctionSpec jhtml_tablecell_methods[] =
{
	{0}
};

static JSBool JHTMLTableCell(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
  #if (WEBC_SUPPORT_TABLES)
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	HTMLTableCell *pTableCell;

	if (jsmgr)
	{
		HTMLTagParse tag[1];
		HTML_InitTagParse(tag, HTML_TAG_TD);

		WEBC_NEW_VERBOSE(pTableCell, HTMLTableCell(tag, jsmgr->GetDocument()),"JHTMLTableCell");
		if (pTableCell)
		{
			pTableCell->SetJSObject(obj);
			pTableCell->SetTagProperties(tag, jsmgr->GetDocument());
			jsmgr->AddNewElement(pTableCell);
		}
	}
	return JS_TRUE;
  #else // WEBC_SUPPORT_TABLES
    return JS_FALSE;
  #endif // WEBC_SUPPORT_TABLES
}

JSClass *getHtmlTableCellElement()
{
	return &JSHtmlTableCellElement;
}

JSObject *jhtml_tablecell_Init(JSContext *cx, JSObject *proto)
{
	//initialize the HTMLTableCellElement Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSHtmlTableCellElement,
    				(JSNative) JHTMLTableCell, 0,
    				jhtml_tablecell_props, jhtml_tablecell_methods,
    				WEBC_NULL, WEBC_NULL);
}

#endif //(WEBC_SUPPORT_JSCRIPT)
