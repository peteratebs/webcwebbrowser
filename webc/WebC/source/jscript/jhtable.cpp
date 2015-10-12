/*
|  JHTABLE.CPP -
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
#include "jhtable.h"
#include "jhutils.h"

#if (WEBC_SUPPORT_JSCRIPT)

#include "htr.hpp"
#include "htable.hpp"
#include "wjscript.hpp"
#include "webcstr.h"
#include "htmldoc.hpp"
#include "objref.cpp"
#include "webcmem.h"
#include "jhcoll.h"
#include "jhelem.h"
#include "wjsdebug.h"

//set the unique ids for the properties of the table class
enum jhtml_table_tinyid
{
	HTMLTABLEELEMENT_CAPTION=-1,
	HTMLTABLEELEMENT_THEAD=-2,
	HTMLTABLEELEMENT_TFOOT=-3,
	HTMLTABLEELEMENT_ROWS=-4,
	HTMLTABLEELEMENT_TBODIES=-5,
	HTMLTABLEELEMENT_ALIGN=-6,
	HTMLTABLEELEMENT_BGCOLOR=-7,
	HTMLTABLEELEMENT_BORDER=-8,
	HTMLTABLEELEMENT_CELLPADDING=-9,
	HTMLTABLEELEMENT_CELLSPACING=-10,
	HTMLTABLEELEMENT_FRAME=-11,
	HTMLTABLEELEMENT_RULES=-12,
	HTMLTABLEELEMENT_SUMMARY=-13,
	HTMLTABLEELEMENT_WIDTH=-14,
	HTMLTABLEELEMENT_HEIGHT=-15,
	HTMLTABLEELEMENT_BACKGROUND=-16
};

static WEBC_READONLY HTMLAttributeType jtable_id_to_param[] =
{
	HTML_UNKNOWN_ATTRIB,
	HTML_UNKNOWN_ATTRIB,
	HTML_UNKNOWN_ATTRIB,
	HTML_UNKNOWN_ATTRIB,
	HTML_UNKNOWN_ATTRIB,
	HTML_ATTRIB_ALIGN,
	HTML_ATTRIB_BGCOLOR,
	HTML_ATTRIB_BORDER,
	HTML_ATTRIB_CELLPADDING,
	HTML_ATTRIB_CELLSPACING,
	HTML_UNKNOWN_ATTRIB,
	HTML_UNKNOWN_ATTRIB,
	HTML_UNKNOWN_ATTRIB,
	HTML_ATTRIB_WIDTH,
	HTML_ATTRIB_HEIGHT,
	HTML_ATTRIB_BACKGROUND
};

JSBool jhtml_table_resolve(JSContext *cx, JSObject *obj, jsval id)
{
#if (WEBC_SUPPORT_TABLES)
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLTable *pTable = (pElem)? (HTMLTable *) pElem : 0;
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	if (!pTable || !jsmgr)
		return JS_TRUE;


	if (JSVAL_IS_STRING(id))
	{
		WEBC_CHAR *name = WEBC_JS_STRING_TO_WEBC_STRING(JSVAL_TO_STRING(id));

		if (!webc_c_strcmp(name, "rows"))
		{
			jhtml_collection_New(jsmgr, obj, "rows", pTable, TABLE_ROWS);
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
 * Call this method when getting a Property from HTMLTableElement
 */
static JSBool jhtml_table_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
#if (WEBC_SUPPORT_TABLES)
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLTable *pTable = (pElem)? (HTMLTable *) pElem : 0;
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	if (!pTable || !jsmgr)
		return JS_TRUE;

	WEBC_UINT8 align = 0;

	switch (JSVAL_TO_INT(id))
	{
		case HTMLTABLEELEMENT_CAPTION:
			//this is not supported by our browser
			break;
		case HTMLTABLEELEMENT_THEAD:
			//this is not supported by our browser
			break;
		case HTMLTABLEELEMENT_TFOOT:
			//this is not supported by our browser
			break;
		case HTMLTABLEELEMENT_TBODIES:
			//this is not supported by our browser
			break;
		case HTMLTABLEELEMENT_ALIGN:
			align = pTable->GetTableAlign();
			switch (align)
			{
				case HTML_ALIGN_LEFT:
					*vp = STRING_TO_JSVAL(JS_InternString(cx, "left"));
					break;
				case HTML_ALIGN_RIGHT:
					*vp = STRING_TO_JSVAL(JS_InternString(cx, "right"));
					break;
				case HTML_ALIGN_CENTER:
					*vp = STRING_TO_JSVAL(JS_InternString(cx, "center"));
					break;
				default:
					*vp = STRING_TO_JSVAL(JS_InternString(cx, "inline"));
					break;
			}
			return JS_TRUE;

		case HTMLTABLEELEMENT_BGCOLOR:
		{
			HTMLColor c = pTable->TableBackgroundColor();
			WEBC_CHAR str[8];
			HTML_PrintColor(str, c);
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, str));
			return JS_TRUE;
		}

		case HTMLTABLEELEMENT_BORDER:
			*vp = INT_TO_JSVAL(pTable->TableBorder(CSS_LEFT));
			break;

		case HTMLTABLEELEMENT_CELLPADDING:
			*vp = INT_TO_JSVAL(pTable->GetCellPadding());
			return JS_TRUE;

		case HTMLTABLEELEMENT_CELLSPACING:
			*vp = INT_TO_JSVAL(pTable->GetCellSpacing());
			return JS_TRUE;

		case HTMLTABLEELEMENT_FRAME:
			//this is not supported in our browser
			break;

		case HTMLTABLEELEMENT_RULES:
			//this is not supported in our browser
			break;

		case HTMLTABLEELEMENT_SUMMARY:
			//this is not implemented in our browser
			break;

		case HTMLTABLEELEMENT_WIDTH:
		{
			WEBC_CHAR str[16];
			HTML_PrintLength(str, 16, pTable->TableWidth());
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, str));
			return JS_TRUE;
		}

		case HTMLTABLEELEMENT_HEIGHT:
		{
			WEBC_CHAR str[16];
			HTML_PrintLength(str, 16, pTable->TableHeight());
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, str));
			return JS_TRUE;
		}

		case HTMLTABLEELEMENT_BACKGROUND:
		  #if (WEBC_SUPPORT_IMAGES && WEBC_SUPPORT_BACKGROUND_IMAGES)
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pTable->Background()));
		  #endif // WEBC_SUPPORT_IMAGES
			return JS_TRUE;
	}//end switch

#endif // WEBC_SUPPORT_TABLES

	return JS_TRUE;
}

/**
 * This method is called when setting a Property in HTMLESelect
 */
static JSBool jhtml_table_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
#if (WEBC_SUPPORT_TABLES)
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLTable *pTable = (pElem)? (HTMLTable *) pElem : 0;

	if (!pTable)
		return JS_TRUE;

	switch (JSVAL_TO_INT(id))
	{
		case HTMLTABLEELEMENT_CAPTION:
			//this is not supported by our browser
			break;
		case HTMLTABLEELEMENT_THEAD:
			//this is not supported by our browser
			break;
		case HTMLTABLEELEMENT_TFOOT:
			//this is not supported by our browser
			break;
		case HTMLTABLEELEMENT_TBODIES:
			//this is not supported by our browser
			break;

		case HTMLTABLEELEMENT_ALIGN:
		case HTMLTABLEELEMENT_BGCOLOR:
		case HTMLTABLEELEMENT_BORDER:
		case HTMLTABLEELEMENT_CELLPADDING:
		case HTMLTABLEELEMENT_CELLSPACING:
		case HTMLTABLEELEMENT_WIDTH:
		case HTMLTABLEELEMENT_HEIGHT:
		case HTMLTABLEELEMENT_BACKGROUND:
		{
			JSString *jsStr;
			if (JSVAL_IS_STRING(*vp))
			{
				jsStr = JSVAL_TO_STRING(*vp);
			}
			else
			{
				jsStr = JS_ValueToString(cx, *vp);
			}

			JS_AddRoot(cx, &jsStr);

			WEBC_CHAR *str = WEBC_JS_STRING_TO_WEBC_STRING(jsStr);
			pTable->SetProperty(jtable_id_to_param[-1 - JSVAL_TO_INT(id)], str);
			pTable->Update(0,(WEBC_PFBYTE)1);
			pTable->StyleUpdate();

			JS_RemoveRoot(cx, &jsStr);

			break;
		}

		case HTMLTABLEELEMENT_FRAME:
			//this is not supported by our browser
			break;
		case HTMLTABLEELEMENT_RULES:
			//this is not supported by our browser
			break;
		case HTMLTABLEELEMENT_SUMMARY:
			//this is not supported by our browser
			break;
	}//end switch
#endif // (WEBC_SUPPORT_TABLES)
	return JS_TRUE;
}

JSBool jhtml_table_createTHead(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	//this is not supported by our browser
	return JS_FALSE;
}
JSBool jhtml_table_deleteTHead(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	//this is not supported by our browser
	return JS_FALSE;
}
JSBool jhtml_table_createTFoot(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	//this is not supported by our browser
	return JS_FALSE;
}
JSBool jhtml_table_deleteTFoot(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	//this is not supported by our browser
	return JS_FALSE;
}
JSBool jhtml_table_createCaption(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	//this is not supported by our browser
	return JS_FALSE;
}
JSBool jhtml_table_deleteCaption(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	//this is not supported by our browser
	return JS_FALSE;
}

/**
 * insertRow - This method inserts a row at the specified index
 *
 * params - int index - the row to insert after
 * return - the new table row element
 */
JSBool jhtml_table_insertRow(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
  #if (WEBC_SUPPORT_TABLES)
	if (argc > 0 && JSVAL_IS_INT(argv[0]))
	{
		HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
		HTMLTable *pTable = (pElem)? (HTMLTable *) pElem : 0;
		int32 index = 0;
		if (pTable && (JS_ValueToInt32(cx, argv[0], &index) == JS_TRUE))
		{
			JSObject *tableRow = (pTable->InsertRow((int)index))->CreateJSObject();
			*rval = OBJECT_TO_JSVAL(tableRow);
			return JS_TRUE;
		}
	}//end if
  #endif // WEBC_SUPPORT_TABLES

	*rval = JSVAL_NULL;
	return JS_TRUE;
}

/**
 * deleteRow - This method deletes a row at the specified index
 *
 * params - int index - the row to delete
 * return - void
 */
JSBool jhtml_table_deleteRow(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
  #if (WEBC_SUPPORT_TABLES)
	if (argc > 0 && JSVAL_IS_INT(argv[0]))
	{
		HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
		HTMLTable *pTable = (pElem)? (HTMLTable *) pElem : 0;
		int32 index = 0;
		if (pTable && (JS_ValueToInt32(cx, argv[0], &index) == JS_TRUE))
		{
			pTable->DeleteRow((int)index);
		}
	}//end if
  #endif // WEBC_SUPPORT_TABLES

	*rval = JSVAL_VOID;
	return JS_TRUE;
}


/**
 * This JSClass is the javascript wrapper for the HTMLTable class defined in the DOM Level 1
 */
JSClass JSHtmlTableElement =
{
	"HTMLTableElement", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty,
	(JSPropertyOp) jutil_delProperty, (JSPropertyOp) jhtml_table_getProperty,
	(JSPropertyOp) jhtml_table_setProperty, (JSEnumerateOp) jutil_enumerate,
	(JSResolveOp) jhtml_table_resolve, (JSConvertOp) jutil_convert,
	(JSFinalizeOp) jutil_finalize
};



JSPropertySpec jhtml_table_props[] = {
	{"caption",     HTMLTABLEELEMENT_CAPTION,    JSPROP_ENUMERATE},
	{"tHead",       HTMLTABLEELEMENT_THEAD,      JSPROP_ENUMERATE},
	{"tFoot",       HTMLTABLEELEMENT_TFOOT,      JSPROP_ENUMERATE},
	{"rows",        HTMLTABLEELEMENT_ROWS,      JSPROP_ENUMERATE},
	{"tBodies",     HTMLTABLEELEMENT_TBODIES,    JSPROP_ENUMERATE},
	{"align",       HTMLTABLEELEMENT_ALIGN,      JSPROP_ENUMERATE},
	{"bgColor",     HTMLTABLEELEMENT_BGCOLOR,    JSPROP_ENUMERATE},
	{"border",      HTMLTABLEELEMENT_BORDER,     JSPROP_ENUMERATE},
	{"cellPadding", HTMLTABLEELEMENT_CELLPADDING,JSPROP_ENUMERATE},
	{"cellSpacing", HTMLTABLEELEMENT_CELLSPACING,JSPROP_ENUMERATE},
	{"frame",       HTMLTABLEELEMENT_FRAME,      JSPROP_ENUMERATE},
	{"rules",       HTMLTABLEELEMENT_RULES,      JSPROP_ENUMERATE},
	{"summary",     HTMLTABLEELEMENT_SUMMARY,    JSPROP_ENUMERATE},
	{"width",       HTMLTABLEELEMENT_WIDTH,	     JSPROP_ENUMERATE},
	{"height",      HTMLTABLEELEMENT_HEIGHT,     JSPROP_ENUMERATE},
	{"background",  HTMLTABLEELEMENT_BACKGROUND, JSPROP_ENUMERATE},
	{0}
};

JSFunctionSpec jhtml_table_methods[] =
{
//	{"createTHead",   jhtml_table_createTHead,   0},
//	{"deleteTHead",   jhtml_table_deleteTHead,   0},
//	{"createTFoot",   jhtml_table_createTFoot,   0},
//	{"deleteTFoot",   jhtml_table_deleteTFoot,   0},
//	{"createCaption", jhtml_table_createCaption, 0},
//	{"deleteCaption", jhtml_table_deleteCaption, 0},
	{"insertRow",     (JSNative) jhtml_table_insertRow,     1},
	{"deleteRow",     (JSNative) jhtml_table_deleteRow,     1},
	{0}
};

static JSBool JHTMLTable(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
  #if (WEBC_SUPPORT_TABLES)
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	HTMLTable *pTable;

	if (!jsmgr)
	{
		HTMLTagParse tag[1];
		HTML_InitTagParse(tag, HTML_TAG_TABLE);

		WEBC_NEW_VERBOSE(pTable, HTMLTable(tag, jsmgr->GetDocument()),"JHTMLTable");
		if (pTable)
		{
			pTable->SetJSObject(obj);
			pTable->SetTagProperties(tag, jsmgr->GetDocument());
			jsmgr->AddNewElement(pTable);
		}
	}
  #endif // WEBC_SUPPORT_TABLES

	return JS_TRUE;
}

JSClass *getHtmlTableElement()
{
	return &JSHtmlTableElement;
}

JSObject *jhtml_table_Init(JSContext *cx, JSObject *proto)
{
	//initialize the HTMLTableElement Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSHtmlTableElement,
    				(JSNative) JHTMLTable, 0,
    				jhtml_table_props, jhtml_table_methods,
    				WEBC_NULL, WEBC_NULL);
}

#endif //(WEBC_SUPPORT_JSCRIPT)
