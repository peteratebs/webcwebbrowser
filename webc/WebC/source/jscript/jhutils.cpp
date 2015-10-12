//
// JHUTILS.C - Javascript-related utilities
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//    This module contains routines to preprocess and parse HTML text.
//

/*****************************************************************************/
// Headers
/*****************************************************************************/

#include "webcjhconfig.h"
#include "webcstr.h"
#include "jhutils.h"
#include "jhdoc.h"
#include "wjscript.hpp"

#include "webcmem.h"

#if (WEBC_SUPPORT_JSCRIPT)

#include "hform.hpp"
#include "helement.hpp"
#include "rtpprint.h"
#include "htmldoc.hpp"
#include "objref.cpp"
#include "jhanchor.h"
#include "jhbody.h"
#include "jhcoll.h"
#include "jhdoc.h"
#include "jhelem.h"
#include "jhform.h"
#include "jhflow.h"
#include "jhframe.h"
#include "jhfrset.h"
#include "jhiframe.h"
#include "jhimg.h"
#include "jhinput.h"
#include "jhistory.h"
#include "jhoption.h"
#include "jhselect.h"
#include "jhtable.h"
#include "jhtarea.h"
#include "jhtblrow.h"
#include "jhtitle.h"
#include "jhtcell.h"
#include "jhmap.h"
#include "jharea.h"
#include "jhbutton.h"
#include "jloc.h"
#include "jnav.h"
#include "jrule.h"
#include "jssheet.h"
#include "jstyle.h"
#include "jwindow.h"
#include "jevent.h"
#include "jsobjref.hpp"
#include "dmanager.hpp"
#include "wjsdebug.h"
#include "webcLog.h"

/*****************************************************************************/
// Data
/*****************************************************************************/

WEBC_READONLY HTMLEventType gOrderedEventTypes [] =
{
	HTML_EVENT_CHANGE,
	HTML_EVENT_CLICK,
	HTML_EVENT_DBLCLICK,
	HTML_EVENT_EDIT,
	HTML_EVENT_FOCUS,
	HTML_EVENT_KEYDOWN,
	HTML_EVENT_KEYPRESS,
	HTML_EVENT_KEYUP,
	HTML_EVENT_LOAD,
	HTML_EVENT_LOSECAPTURE,
	HTML_EVENT_MOUSEDOWN,
	HTML_EVENT_MOUSEMOVE,
	HTML_EVENT_MOUSEOUT,
	HTML_EVENT_MOUSEOVER,
	HTML_EVENT_MOUSEUP,
#if (WEBC_SUPPORT_AJAX)
	/// XMLHttpRequest.onreadystatechange event
	HTML_EVENT_ONREADYSTATECHANGE,
#endif
	HTML_EVENT_RESET,
	HTML_EVENT_SUBMIT,
	HTML_EVENT_UNFOCUS,
	HTML_EVENT_UNLOAD,
};

/* VERY IMPORTANT - this list MUST be alphabetized */
WEBC_READONLY char *gppEventHandlerNames [] =
{
	"onchange",
	"onclick",
	"ondblclick",
	"onedit",
	"onfocus",
	"onkeydown",
	"onkeypress",
	"onkeyup",
	"onload",
	"onlosecapture",
	"onmousedown",
	"onmousemove",
	"onmouseout",
	"onmouseover",
	"onmouseup",
#if (WEBC_SUPPORT_AJAX)
	/// XMLHttpRequest.onreadystatechange event
	"onreadystatechange",
#endif
	"onreset",
	"onsubmit",
	"onunfocus",
	"onunload"
};

#define NUM_NAMED_HANDLERS (sizeof(gppEventHandlerNames)/sizeof(char*))

int giUniqueNameCounter = 0;

/*****************************************************************************/
// Public Functions
/*****************************************************************************/
extern "C" int _StringListItemCompareAscii(void *item, void *list, int index);

int _StringListItemCompareAscii(void *item, void *list, int index)
{
	return (rtp_stricmp((char *) item, ((char **) list)[index]));
}

/**
 * This function gets the JSClass associated with the
 * element type.
 *
 * param HTMLElementType type - this is the type of the
 *                              element.
 * return - the JSClass * associated with this type
 */
JSClass *getJSClassByElementType(HTMLElementType type)
{
	switch (type)
	{
		case HTML_ELEMENT:
		case HTML_ELEMENT_NONE:
		case HTML_STRING_ELEMENT:
			return getHtmlElement();

		//input elements
		case HTML_CHECKBOX_ELEMENT:
		case HTML_EDIT_STR_ELEMENT:
		case HTML_HIDDEN_INPUT_ELEMENT:
		case HTML_INPUT_BUTTON_ELEMENT:
		case HTML_RADIO_BUTTON_ELEMENT:
			return getHtmlInputElement();

		case HTML_SELECT_ELEMENT:
			return getHtmlSelectElement();

		case HTML_OPTION_ELEMENT:
			return getHtmlOptionElement();

		case HTML_EDITBOX_ELEMENT:
			return getHtmlTextAreaElement();

		case HTML_BUTTON_ELEMENT:
			return getHtmlButtonElement();

		case HTML_TABLE_ELEMENT:
			return getHtmlTableElement();

		case HTML_TABLE_ROW_ELEMENT:
			return getHtmlTableRowElement();

		case HTML_TABLE_CELL_ELEMENT:
			return getHtmlTableCellElement();

		case HTML_IMAGE_ELEMENT:
			return getHtmlImageElement();

		case HTML_HORIZONTAL_RULE_ELEMENT:
			return getHtmlElement();

		case HTML_ANCHOR_ELEMENT:
			return getHtmlAnchorElement();

		case HTML_BOLD_ELEMENT:
		case HTML_CENTER_ELEMENT:
		case HTML_CITE_ELEMENT:
		case HTML_CODE_ELEMENT:
		case HTML_DEL_ELEMENT:
		case HTML_EMPHASIZE_ELEMENT:
		case HTML_FONT_ELEMENT:
		case HTML_INS_ELEMENT:
		case HTML_ITALIC_ELEMENT:
		case HTML_KBD_ELEMENT:
		case HTML_LABEL_ELEMENT:
		case HTML_SPAN_ELEMENT:
		case HTML_STRIKE_ELEMENT:
		case HTML_STRONG_ELEMENT:
		case HTML_UNDERLINE_ELEMENT:
			return getHtmlElement();

	  #if (WEBC_SUPPORT_FRAMES)
		case HTML_FRAMESET_ELEMENT:
			return getHtmlFramesetElement();

		case HTML_FRAME_ELEMENT:
			return getHtmlFrameElement();

	  #if (WEBC_SUPPORT_IFRAMES)
		case HTML_IFRAME_ELEMENT:
			return getHtmlIFrameElement();
	  #endif // WEBC_SUPPORT_IFRAMES
	  #endif // WEBC_SUPPORT_FRAMES

		case HTML_NOBR_ELEMENT:
		case HTML_BR_ELEMENT:
			return getHtmlElement();

		case HTML_BODY_ELEMENT:
			return getHtmlBodyElement();

		case HTML_LIST_ITEM_ELEMENT:
		case HTML_LIST_ELEMENT:
		case HTML_BASE_ELEMENT:
		case HTML_SMALL_ELEMENT:
		case HTML_BIG_ELEMENT:
			return getHtmlElement();

		//the classes that use the align attr.
		case HTML_ADDRESS_ELEMENT:
		case HTML_DIV_ELEMENT:
		case HTML_HEADING_ELEMENT:
		case HTML_FLOW_ELEMENT:
		case HTML_PARAGRAPH_ELEMENT:
			return getHtmlFlowElement();

		case HTML_MAP_ELEMENT:
			return getHtmlMapElement();

		case HTML_AREA_ELEMENT:
			return getHtmlAreaElement();

		case HTML_SCRIPT_ELEMENT:
			return getHtmlElement();

		case HTML_FORM_ELEMENT:
			return getHtmlFormElement();

		case HTML_PRE_ELEMENT:
			return getHtmlElement();

		case HTML_STYLE_ELEMENT:
			return getJStyle();

		default:
			return getHtmlElement();
	}
}

void nameGenerator(JSContext *cx, char *str)
{
	rtp_sprintf(str, "_nameless%x", giUniqueNameCounter++);
}

/**
 * This function takes a string of the form "name1=value1,name2=value2...",
 * and puts name1 into pName and value1 into pValue.
 *
 * params - string pStr - this is the string that contains the name value
 *                        pairs to be parsed
 *        - string pName - this will hold the name at the end of this function
 *        - string pValue - this will hold the value at the end of thid function
 * return - char* - this is the first characted after the next comma found. This
 *                  can be used to call this function multiple times.  If this is the
 *                 last name value pair then the function returns 0
 */


char* jutils_ParseNameValuePair(char *pStr, char *pName, char *pValue)
{
	char *eq = 0;
	char *comma = 0;
    char *pTemp = 0;
	if (pStr && *pStr)
	{
		eq = tc_strchrs(pStr, "=");
		comma = tc_strchrs(pStr, ",");

		//make sure we have an '=' AND we either DON'T have a comma OR the
		//comma comes BEFORE the '='
		if (eq && (!comma || eq < comma))
		{
			pTemp = jutils_FirstNonWS(pStr);
			tc_strncpy(pName, pTemp, eq - pTemp);
			pName[eq - pTemp] = '\0';
			jutils_NullOutTrailingWS(pName);
			if (comma)
			{
				pTemp = jutils_FirstNonWS(eq+1);
				tc_strncpy(pValue, pTemp, comma - pTemp);
				pValue[comma - pTemp] = '\0';
				jutils_NullOutTrailingWS(pValue);
				return(comma + 1);
			}
			else
			{
				pTemp = jutils_FirstNonWS(eq+1);
				tc_strcpy(pValue, pTemp);
				jutils_NullOutTrailingWS(pValue);
			}
		}//end if
	}//end if
	return 0;
}//end jutils_parseNameValuePair

/**
 * jutils_FirstNonWS - This function returns the first instance of a non-whitespace character
 *
 * params - string pFind - The string to trim
 * return - string The location of the first non-whitespace character, is there is only ws,
 *                 return 0
 */
char * jutils_FirstNonWS(char *pFind)
{
	int i = 0;
	if (!pFind)
		return 0;
	i = tc_strspn(pFind, " \t\r\n");
	return (pFind += i);
}

/**
 * jutils_NullOutTrailingWS - This function takes a string and puts a \0 after
 *                            the last non whitespace character
 * params - string pStr - string to be modified
 * returns void
 */
void jutils_NullOutTrailingWS(char *pStr)
{
	if (!pStr)
		return;
	int i = tc_strlen(pStr);
	for (i--; pStr[i] <= ' '; i--)
	{
		pStr[i] = '\0';
	}
}
/**
 * This method parses a boolean value from the string
 *
 * params - string pStr - the string with a bool in the form
 *                      - yes | no | 0 | 1 | true | false
 *          int value - the result of the parse goes here
 *
 * returns 0 if the string cant be parsed, 1 otherwise
 */
int jutils_ParseBoolVal(char *pStr, int *value)
{
	if (tc_stricmp(pStr, "yes") == 0 ||
		tc_stricmp(pStr, "1") == 0   ||
		tc_stricmp(pStr, "true") == 0 )
	{
		*value = 1;
		return 1;
	}
	if (tc_stricmp(pStr, "no") == 0   ||
		tc_stricmp(pStr, "0") == 0    ||
		tc_stricmp(pStr, "false") == 0 )
	{
		*value = 0;
		return 1;
	}
	return 0;
}

HTMLElement *jutils_CheckObjectList(vector *pVec, int (*match)(HTMLElement *data, WEBC_PFBYTE p), WEBC_PFBYTE param)
{
	vector_iterator pvi[1];

	HTMLElement *p_data;
	p_data = (HTMLElement*)ObjectListGetFirst(pVec, pvi);
	while (p_data)
	{
		if (match(p_data, param))
		{
			return p_data;
		}
		p_data = (HTMLElement *)ObjectListGetNext(pvi);
	}
	return 0;
}


int jutils_CountObjectList(vector *pVec, int (*match)(HTMLElement * data, WEBC_PFBYTE p), WEBC_PFBYTE param)
{
	vector_iterator pvi[1];

	HTMLElement *p_data;
	int count = 0;

	p_data = (HTMLElement *)ObjectListGetFirst(pVec, pvi);
	while (p_data)
	{
		if (match(p_data, param))
		{
			count++;
		}
		p_data = (HTMLElement *)ObjectListGetNext(pvi);
	}
	return count;
}

HTMLElement* jutils_CheckAllFormsAndInputs(vector *pVec, int (*match)(HTMLElement *data, WEBC_PFBYTE p), WEBC_PFBYTE param)
{
	vector_iterator pvi[1];
	HTMLForm *pForm;
	HTMLElement *p_data;

	pForm = (HTMLForm *)ObjectListGetFirst(pVec, pvi);
	while (pForm)
	{
		if (match(pForm, param))
		{
			return pForm;
		}
		p_data = jutils_CheckObjectList(pForm->GetFieldVector(), match, param);
		if (p_data)
			return p_data;
		pForm = (HTMLForm *)ObjectListGetNext(pvi);
	}
	return 0;
}

int jutils_CountAllFormsAndInputs(vector *pVec, int (*match)(HTMLElement *data, WEBC_PFBYTE p), WEBC_PFBYTE param)
{
	vector_iterator pvi[1];
	HTMLForm *pForm;

	int count = 0;

	pForm = (HTMLForm *)ObjectListGetFirst(pVec, pvi);
	while (pForm)
	{
		if (match(pForm, param))
		{
			count++;
		}
		count += jutils_CountObjectList(pForm->GetFieldVector(), match, param);
		pForm = (HTMLForm *)ObjectListGetNext(pvi);
	}
	return 0;
}

HTMLElement* jutils_GetNthOfObjectList(vector *pVec, int index, int (*match)(HTMLElement * data, WEBC_PFBYTE p), WEBC_PFBYTE param)
{
	vector_iterator pvi[1];

	HTMLElement *p_data;
	int count = 0;

	p_data = (HTMLElement *)ObjectListGetFirst(pVec, pvi);
	while (p_data)
	{
		if (match(p_data, param))
		{
			count++;
			if (index == count - 1)
			{
				return p_data;
			}
		}
		p_data = (HTMLElement *)ObjectListGetNext(pvi);
	}
	return 0;
}


HTMLElement* jutils_GetNthOfFormsAndInputs(vector *pVec, int index, int (*match)(HTMLElement * data, WEBC_PFBYTE p), WEBC_PFBYTE param)
{
	vector_iterator pvi[1];

	HTMLForm *pForm;

	pForm = (HTMLForm *)ObjectListGetFirst(pVec, pvi);
	int count = 0;
	while (pForm)
	{
		if (match(pForm, param))
		{
			count++;
			if (index == count - 1)
			{
				return pForm;
			}
		}
		jutils_GetNthOfObjectList(pForm->GetFieldVector(), index, match, param);
		pForm = (HTMLForm *)ObjectListGetNext(pvi);
	}
	return 0;
}

HTMLEventType jutils_StringToEventType(char *pEventName)
{
	long n = tc_strlistcmp(pEventName, gppEventHandlerNames, NUM_NAMED_HANDLERS);

	if (n >= 0)
	{
		return (gOrderedEventTypes[n]);
	}

	return (HTML_EVENT_NONE);
}

const char *jutils_EventTypeToString(HTMLEventType type)
{
	long n;

	for (n=0; n<NUM_NAMED_HANDLERS; n++)
	{
		if (type == gOrderedEventTypes[n])
		{
			return (gppEventHandlerNames[n]);
		}
	}

	return (0);
}

/////////////////////////////////////////////
//set up the global_class to be used by js //
/////////////////////////////////////////////
static JSBool global_enumerate(JSContext *cx, JSObject *obj)
{
  return JS_TRUE;
}

static JSBool global_resolve(JSContext *cx, JSObject *obj, jsval id)
{
  jutil_resolve(cx, obj, id, "global_resolve was called");
  return JS_TRUE;
}

static JSClass global_class = {
    "global", JSCLASS_HAS_PRIVATE,
    JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,  JS_PropertyStub,
    (JSEnumerateOp) global_enumerate, (JSResolveOp) global_resolve, JS_ConvertStub,   JS_FinalizeStub};

JSClass* GetGlobalClass()
{
	return &global_class;
}

/////////////////////////////////////////////
// These methods simply return WEBC_TRUE without//
// doing a thing.  Great for placeholders! //
/////////////////////////////////////////////
/**
 * Call this method when adding a Property
 */
JSBool jutil_addProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    return JS_TRUE;
}

/**
 * Call this method when deleting a Property
 */
JSBool jutil_delProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    return JS_TRUE;
}


/**
 * Call this method when getting a Property
 */
JSBool jutil_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	return JS_TRUE;
}

/**
 * This method is called when setting a Property
 */
JSBool jutil_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    return JS_TRUE;
}

JSBool jutil_enumerate(JSContext *cx, JSObject *obj)
{
    return JS_TRUE;
}


JSBool jutil_resolve(JSContext *cx, JSObject *obj, jsval id, char *whocalled)
{
	//int intVal = JSVAL_TO_INT(id);
    //LOG_HTMLDOM("jutil_resolve called on behalf of class: %s to resolve %d\n", whocalled, intVal);
    return JS_TRUE;
}

JSBool jutil_convert(JSContext *cx, JSObject *obj, JSType type, jsval *vp)
{
    return JS_TRUE;
}

void jutil_finalize(JSContext *cx, JSObject *obj)
{
}


// This function is used on exit of all callback routines; it checks the associated
//  WebcJSScript object of a context (if it has one) and returns JS_TRUE if it is
//  not stopped or deleted.  If the script is non-null and either stopped or deleted
//  it returns JS_FALSE.

JSBool jutils_CheckContextState(JSContext *cx)
{
WebcJSScript *pScr = (WebcJSScript *) jhutil_GetContextPrivate(cx);

	if (pScr && pScr->CheckFlag(JS_SCRIPT_FLAG_STOPPED|JS_SCRIPT_FLAG_DELETED))
	{
		return (JS_FALSE);
	}

	return (JS_TRUE);
}

void jutils_LoadDocumentUrl(JSContext* cx, HTMLDocument* doc, const WEBC_CHAR* url)
{
	doc->SaveLocation();  // save our current page into the history list
	doc->SetSrc(url);
	doc->Update(1,0);
}

/*---------------------------------------------------------------------------*/
// jhutil_SetPrivate, jhutil_GetPrivate
/*---------------------------------------------------------------------------*/

void jhutil_SetPrivate(JSContext *cx, JSObject *obj, void *ptr)
{
	JS_SetPrivate(cx, obj, ptr);
}

void *jhutil_GetPrivate(JSContext *cx, JSObject *obj)
{
	return (JS_GetPrivate(cx, obj));
}

void jhutil_SetContextPrivate(JSContext *cx, void *ptr)
{
	JS_SetContextPrivate(cx, ptr);
}

void *jhutil_GetContextPrivate(JSContext *cx)
{
	return (JS_GetContextPrivate(cx));
}

/*---------------------------------------------------------------------------*/
// jhutil_SetPrivate, jhutil_GetPrivate
/*---------------------------------------------------------------------------*/

static HTMLElement *jhutil_CreateElementChar(JSContext *cx, JSObject *obj, WEBC_CHAR *name);
HTMLElement *jhutil_CreateElement(HTMLElementType eType, HTMLTagParse *tag, HTMLDocument *pDoc);
// <EDIT> 20080120 _jhutil_CreateElementFromText().. Implements implements doc.createElement()
JSBool jhutil_CreateElementFromText(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLDocument *pDoc = (HTMLDocument *)jhutil_GetPrivate(cx, obj);
	if (argc > 0 && JSVAL_IS_STRINGABLE(argv[0]))
	{
		WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
		WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
		if (jsmgr)
        {
		    WEBC_CHAR *name = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, argv[0]));
// <EDIT> 20080120  create an element
		    HTMLElement *pElement = jhutil_CreateElementChar(cx, obj, name);
		    if (pElement)
		    {
// <EDIT> 20080120  create a javascript object and assign a pointer to this element to the object private pointer
                pElement->CreateJSObject();
                *rval = OBJECT_TO_JSVAL(pElement->GetJSObject());
                return JS_TRUE;
		    }
// <EDIT> 20080120  BUGBUG - ??? Verify we are not causing a leak.
// <EDIT> 20080120  Need to test if WebcJSDocumentContext::AddNewElement (HTMLElement* elem) should be called her
// <EDIT> 20080120  WebcJSDocumentContext::AddNewElement (HTMLElement* elem) adds the element to a fake
// <EDIT> 20080120  document body that is cleared when the context is destroyed.
        }
    }
	return JS_FALSE;
}

#include "haddress.hpp"
#include "harea.hpp"
#include "hanchor.hpp"
#include "hbase.hpp"
#include "hbig.hpp"
#include "hblquote.hpp"
#include "hbold.hpp"
#include "hbody.hpp"
#include "hbr.hpp"
#include "hcenter.hpp"
#include "hchkbox.hpp"
#include "hcite.hpp"
#include "hcode.hpp"
#include "hdd.hpp"
#include "hdel.hpp"
#include "hdl.hpp"
#include "hdt.hpp"
#include "hdiv.hpp"
#include "hedit.hpp"
#include "hemphsz.hpp"
#include "hfont.hpp"
#include "hform.hpp"
#include "hframe.hpp"
#include "hframset.hpp"
#include "hheading.hpp"
#include "hhidden.hpp"
#include "hhr.hpp"
#include "hiframe.hpp"
#include "himage.hpp"
#include "hobject.hpp"
#include "hins.hpp"
#include "hitalic.hpp"
#include "hkbd.hpp"
#include "hlabel.hpp"
#include "hli.hpp"
#include "hlist.hpp"
#include "hmap.hpp"
#include "hnobr.hpp"
#include "hoption.hpp"
#include "hpasswd.hpp"
#include "hpre.hpp"
#include "hradio.hpp"
#include "hreset.hpp"
#include "hselect.hpp"
#include "hsmall.hpp"
#include "hscript.hpp"
#include "hspacer.hpp"
#include "hspan.hpp"
#include "hstrike.hpp"
#include "hstring.hpp"
#include "hstrong.hpp"
#include "hsubmit.hpp"
#include "htable.hpp"
#include "htd.hpp"
#include "htr.hpp"
#include "htxtarea.hpp"
#include "hundline.hpp"


static HTMLElement *jhutil_CreateElementChar(JSContext *cx, JSObject *obj, WEBC_CHAR *name)
{
	HTMLTagType hType = HTML_ParseTagType(name, webc_strlen(name));
	HTMLElementType eType = TagToHTMLElementType[hType];
	HTMLTagParse tag[1];
	HTMLDocument *pDoc = (HTMLDocument *)jhutil_GetPrivate(cx, obj);

	HTML_InitTagParse(tag, hType);

	HTMLElement *pElement = jhutil_CreateElement(eType, tag, pDoc);

	return (pElement);
}
#ifdef RTP_TRACK_LOCAL_MEMORY
#define wcCreateElement(TY,TA,DOC) _wcCreateElement(TY,TA,DOC,__FILE__,__LINE__);
HTMLElement *_wcCreateElement(HTMLElementType eType, HTMLTagParse *tag, HTMLDocument *pDoc, const char *file, const long line);
#else
HTMLElement *wcCreateElement(HTMLElementType eType, HTMLTagParse *tag, HTMLDocument *pDoc);
#endif

HTMLElement *jhutil_CreateElement(HTMLElementType eType, HTMLTagParse *tag, HTMLDocument *pDoc)
{
	return wcCreateElement(eType, tag, pDoc);
}


static HTMLElement *jhutil_CopyElement(HTMLElementType eType, HTMLElement *pOrigElement)
{
    HTMLElement *pElement = 0; /* Set by NEWELEMENT(OFTYPE) */
	switch (eType)
	{
        case HTML_ADDRESS_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLAddress(*(HTMLAddress *)pOrigElement),"HTMLAddress");
			break;
        case HTML_ANCHOR_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLAnchor(*(HTMLAnchor *)pOrigElement),"HTMLAnchor");
			break;
        case HTML_AREA_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLArea(*(HTMLArea *)pOrigElement),"HTMLArea");
			break;
        case HTML_BASE_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLBase(*(HTMLBase *)pOrigElement),"HTMLBase");
			break;
        case HTML_BIG_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLBig(*(HTMLBig *)pOrigElement),"HTMLBig");
			break;
        case HTML_BLOCKQUOTE_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLBlockquote(*(HTMLBlockquote *)pOrigElement),"HTMLBlockquote");
			break;
        case HTML_CITE_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLCite(*(HTMLCite *)pOrigElement),"HTMLCite");
			break;
        case HTML_CODE_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLCode(*(HTMLCode *)pOrigElement),"HTMLCode");
			break;
        case HTML_DEL_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLDel(*(HTMLDel *)pOrigElement),"HTMLDel");
			break;
        case HTML_DIV_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLDiv(*(HTMLDiv *)pOrigElement),"HTMLDiv");
			break;
        case HTML_EDIT_STR_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLEditString(*(HTMLEditString *)pOrigElement),"HTMLEditString");
			break;
        case HTML_EDITBOX_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLTextArea(*(HTMLTextArea *)pOrigElement),"HTMLTextArea");
			break;
        case HTML_EMPHASIZE_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLEmphasize(*(HTMLEmphasize *)pOrigElement),"HTMLEmphasize");
			break;
        case HTML_FLOW_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLFlow(*(HTMLFlow *)pOrigElement),"HTMLFlow");
			break;
        case HTML_FONT_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLFont(*(HTMLFont *)pOrigElement),"HTMLFont");
			break;
        case HTML_FORM_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLForm(*(HTMLForm *)pOrigElement),"HTMLForm");
			break;
	  #if (WEBC_SUPPORT_FRAMES)
        case HTML_FRAME_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLFrame(*(HTMLFrame *)pOrigElement),"HTMLFrame");
			break;
        case HTML_FRAMESET_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLFrameSet(*(HTMLFrameSet *)pOrigElement),"HTMLFrameSet");
			break;
      #endif
        case HTML_HEADING_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLHeading(*(HTMLHeading *)pOrigElement),"HTMLHeading");
			break;
        case HTML_HORIZONTAL_RULE_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLHorizontalRule(*(HTMLHorizontalRule *)pOrigElement),"HTMLHorizontalRule");
			break;
        case HTML_IMAGE_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLImage(*(HTMLImage *)pOrigElement),"HTMLImage");
			break;
        case HTML_INPUT_BUTTON_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLEditString(*(HTMLEditString *)pOrigElement),"HTMLEditString");
			break;
        case HTML_INS_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLIns(*(HTMLIns *)pOrigElement),"HTMLIns");
			break;
        case HTML_ITALIC_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLItalic(*(HTMLItalic *)pOrigElement),"HTMLItalic");
			break;
        case HTML_KBD_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLKbd(*(HTMLKbd *)pOrigElement),"HTMLKbd");
			break;
        case HTML_LIST_ITEM_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLListItem(*(HTMLListItem *)pOrigElement),"HTMLListItem");
			break;
        case HTML_LIST_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLList(*(HTMLList *)pOrigElement),"HTMLList");
			break;
        case HTML_LABEL_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLLabel(*(HTMLLabel *)pOrigElement),"HTMLLabel");
			break;
        case HTML_MAP_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLMap(*(HTMLMap *)pOrigElement),"HTMLMap");
			break;
        case HTML_NOBR_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLNoBr(*(HTMLNoBr *)pOrigElement),"HTMLNoBr");
			break;
        case HTML_OPTION_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLOption(*(HTMLOption *)pOrigElement),"HTMLOption");
			break;
        case HTML_PARAGRAPH_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLParagraph(*(HTMLParagraph *)pOrigElement),"HTMLParagraph");
			break;
        case HTML_PRE_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLPreformatted(*(HTMLPreformatted *)pOrigElement),"HTMLPreformatted");
			break;
        case HTML_RADIO_BUTTON_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLRadioButton(*(HTMLRadioButton *)pOrigElement),"HTMLRadioButton");
			break;
        case HTML_SCRIPT_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLScript(*(HTMLScript *)pOrigElement),"HTMLScript");
			break;
        case HTML_SELECT_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLSelect(*(HTMLSelect *)pOrigElement),"HTMLSelect");
			break;
        case HTML_SMALL_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLSmall(*(HTMLSmall *)pOrigElement),"HTMLSmall");
			break;
        case HTML_SPAN_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLSpan(*(HTMLSpan *)pOrigElement),"HTMLSpan");
			break;
        case HTML_STRIKE_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLStrike(*(HTMLStrike *)pOrigElement),"HTMLStrike");
			break;
        case HTML_STRONG_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLStrong(*(HTMLStrong *)pOrigElement),"HTMLStrong");
			break;
        case HTML_TABLE_CELL_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLTableCell(*(HTMLTableCell *)pOrigElement),"HTMLTableCell");
			break;
        case HTML_TABLE_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLTable(*(HTMLTable *)pOrigElement),"HTMLTable");
			break;
        case HTML_TABLE_ROW_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLTableRow(*(HTMLTableRow *)pOrigElement),"HTMLTableRow");
			break;
        case HTML_UNDERLINE_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLUnderline(*(HTMLUnderline *)pOrigElement),"HTMLUnderline");
			break;
		case HTML_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLElement(*(HTMLElement *)pOrigElement),"HTMLElement");
			break;

	  #if (WEBC_SUPPORT_FRAMES)
	  #if (WEBC_SUPPORT_IFRAMES)
		case HTML_IFRAME_ELEMENT:
			WEBC_NEW_VERBOSE(pElement, HTMLInlineFrame(*(HTMLInlineFrame *)pOrigElement),"HTMLInlineFrame");
			break;
	  #endif // WEBC_SUPPORT_IFRAMES
	  #endif // WEBC_SUPPORT_FRAMES
        case HTML_STRING_ELEMENT:
		case HTML_STYLE_ELEMENT:
		case HTML_BUTTON_ELEMENT:
		case HTML_HIDDEN_INPUT_ELEMENT:
		case HTML_ELEMENT_NONE:
		default:
            LOG_JAVASCRIPT("jhutil_CreateElementChar: Type %d not implemented \n",eType);
            break;
	}
    return (pElement);
}

HTMLElement *jhutil_CloneElement(HTMLElement *clone, WEBC_BOOL bCloneChildren)
{
	HTMLTagParse tag[1];
	HTML_InitTagParse(tag, clone->GetTag());

	if(!clone)
		return WEBC_NULL;

	HTMLElement *pClonedElement = jhutil_CopyElement(TagToHTMLElementType[clone->GetTag()], clone);

	if(pClonedElement)
	{
		if(bCloneChildren)
		{
			HTMLElement *pChild = clone->FirstChild();
			while (pChild)
			{
				HTMLElement *pChildClone = jhutil_CloneElement(pChild, WEBC_TRUE);
				if(pChildClone)
					pClonedElement->InsertLast(pChildClone);
//					pClonedElement->InsertFirst(pChildClone);

				pChild = pChild->mpNext;
			}
		}
	}

	return pClonedElement;
}


#endif //WEBC_SUPPORT_JSCRIPT
