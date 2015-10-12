/*
|  JHELEM.CPP -
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
#include "jhelem.h"
#include "jhutils.h"
#if (WEBC_SUPPORT_JSCRIPT)

#include "helement.hpp"
#include "htmldoc.hpp"
#include "wjscript.hpp"
#include "webcstr.h"
#include "objref.cpp"
#include "webcmem.h"
#include "jhcoll.h"
#include "jstyle.h"
#include "wjsdebug.h"
#include "rtpchar.h"
#include "webcassert.h"
#include "webcLog.h"

//set the unique ids for the properties of the element class
enum jhtml_element_tinyid
{
	HTMLELEMENT_ID =              -1,
	HTMLELEMENT_TITLE =           -2,
	HTMLELEMENT_LANG =            -3,
	HTMLELEMENT_DIR =             -4,
	HTMLELEMENT_CLASSNAME =       -5,
	HTMLELEMENT_FIRSTCHILD =      -6,
	HTMLELEMENT_LASTCHILD =       -7,
	HTMLELEMENT_NEXTSIBLING =     -8,
	HTMLELEMENT_NODENAME =        -9,
	HTMLELEMENT_NODETYPE =        -10,
	HTMLELEMENT_NODEVALUE =       -11,
	HTMLELEMENT_DOCUMENT =        -12,
	HTMLELEMENT_OWNERDOCUMENT =   -13,
	HTMLELEMENT_PARENTELEMENT =   -14,
	HTMLELEMENT_PARENTNODE =      -15,
	HTMLELEMENT_PREVIOUSSIBLING = -16,
	HTMLELEMENT_UNIQUEID =        -17,
	HTMLELEMENT_CLIENTHEIGHT =    -18,
	HTMLELEMENT_CLIENTWIDTH =     -19,
	HTMLELEMENT_CLIENTLEFT =      -20,
	HTMLELEMENT_CLIENTTOP =       -21,
	HTMLELEMENT_OFFSETHEIGHT =    -22,
	HTMLELEMENT_OFFSETWIDTH =     -23,
	HTMLELEMENT_OFFSETLEFT =      -24,
	HTMLELEMENT_OFFSETTOP =       -25,
	HTMLELEMENT_SCROLLHEIGHT =    -26,
	HTMLELEMENT_SCROLLWIDTH =     -27,
	HTMLELEMENT_SCROLLLEFT =      -28,
	HTMLELEMENT_SCROLLTOP =       -29,
	HTMLELEMENT_ONCLICK =         -30,
	HTMLELEMENT_ONDBLCLICK =      -31,
	HTMLELEMENT_ONKEYDOWN =       -32,
	HTMLELEMENT_ONKEYPRESS =      -33,
	HTMLELEMENT_ONKEYUP =         -34,
	HTMLELEMENT_ONMOUSEDOWN =     -35,
	HTMLELEMENT_ONMOUSEMOVE =     -36,
	HTMLELEMENT_ONMOUSEOUT =      -37,
	HTMLELEMENT_ONMOUSEOVER =     -38,
	HTMLELEMENT_ONMOUSEUP =       -39,
	HTMLELEMENT_ONFOCUS =         -40,
	HTMLELEMENT_ONUNFOCUS =       -41,
	HTMLELEMENT_ONLOAD =          -42,
	HTMLELEMENT_ONUNLOAD =        -43,
	HTMLELEMENT_ONSUBMIT =        -44,
	HTMLELEMENT_ONCHANGE =        -45,
	HTMLELEMENT_ONLOSECAPTURE =   -46,
	HTMLELEMENT_INNERHTML =       -47,
	HTMLELEMENT_OUTERHTML =       -48,
	HTMLELEMENT_INNERTEXT =       -49,
	HTMLELEMENT_OUTERTEXT =       -50,
	HTMLELEMENT_OFFSETPARENT =    -51,
	HTMLELEMENT_TAGNAME   =       -52
};

/******************************************************************************
* jhtml_element_props - properties set/retrieved through jhtml_element_get/set
*
*    Properties that are commented out are TBD
*
******************************************************************************/

JSPropertySpec jhtml_element_props[] = {
	{"id",              HTMLELEMENT_ID,              JSPROP_ENUMERATE},
	{"title",           HTMLELEMENT_TITLE,           JSPROP_ENUMERATE},
	{"lang",            HTMLELEMENT_LANG,            JSPROP_ENUMERATE},
	{"dir",             HTMLELEMENT_DIR,             JSPROP_ENUMERATE},
	{"className",       HTMLELEMENT_CLASSNAME,       JSPROP_ENUMERATE},
	{"firstChild",      HTMLELEMENT_FIRSTCHILD,      JSPROP_ENUMERATE | JSPROP_READONLY},
	{"lastChild",       HTMLELEMENT_LASTCHILD,       JSPROP_ENUMERATE | JSPROP_READONLY},
	{"nextSibling",     HTMLELEMENT_NEXTSIBLING,     JSPROP_ENUMERATE | JSPROP_READONLY},
	{"nodeName",        HTMLELEMENT_NODENAME,        JSPROP_ENUMERATE | JSPROP_READONLY},
	{"nodeType",        HTMLELEMENT_NODETYPE,        JSPROP_ENUMERATE | JSPROP_READONLY},
	{"nodeValue",       HTMLELEMENT_NODEVALUE,       JSPROP_ENUMERATE | JSPROP_READONLY},
	{"document",        HTMLELEMENT_DOCUMENT,        JSPROP_ENUMERATE | JSPROP_READONLY},
	{"ownerDocument",   HTMLELEMENT_OWNERDOCUMENT,   JSPROP_ENUMERATE | JSPROP_READONLY},
	{"parentElement",   HTMLELEMENT_PARENTELEMENT,   JSPROP_ENUMERATE | JSPROP_READONLY},
	{"parentNode",      HTMLELEMENT_PARENTNODE,      JSPROP_ENUMERATE | JSPROP_READONLY},
	{"previousSibling", HTMLELEMENT_PREVIOUSSIBLING, JSPROP_ENUMERATE | JSPROP_READONLY},
	{"uniqueId",        HTMLELEMENT_UNIQUEID,        JSPROP_ENUMERATE | JSPROP_READONLY},
	{"clientHeight",    HTMLELEMENT_CLIENTHEIGHT,    JSPROP_ENUMERATE | JSPROP_READONLY},
	{"clientWidth",     HTMLELEMENT_CLIENTWIDTH,     JSPROP_ENUMERATE | JSPROP_READONLY},
	{"clientLeft",      HTMLELEMENT_CLIENTLEFT,      JSPROP_ENUMERATE | JSPROP_READONLY},
	{"clientTop",       HTMLELEMENT_CLIENTTOP,       JSPROP_ENUMERATE | JSPROP_READONLY},
	{"offsetHeight",    HTMLELEMENT_OFFSETHEIGHT,    JSPROP_ENUMERATE | JSPROP_READONLY},
	{"offsetWidth",     HTMLELEMENT_OFFSETWIDTH,     JSPROP_ENUMERATE | JSPROP_READONLY},
	{"offsetLeft",      HTMLELEMENT_OFFSETLEFT,      JSPROP_ENUMERATE | JSPROP_READONLY},
	{"offsetTop",       HTMLELEMENT_OFFSETTOP,       JSPROP_ENUMERATE | JSPROP_READONLY},
	{"scrollHeight",    HTMLELEMENT_SCROLLHEIGHT,    JSPROP_ENUMERATE | JSPROP_READONLY},
	{"scrollWidth",     HTMLELEMENT_SCROLLWIDTH,     JSPROP_ENUMERATE | JSPROP_READONLY},
	{"scrollLeft",      HTMLELEMENT_SCROLLLEFT,      JSPROP_ENUMERATE},
	{"scrollTop",       HTMLELEMENT_SCROLLTOP,       JSPROP_ENUMERATE},
	{"onclick",         HTMLELEMENT_ONCLICK,         JSPROP_ENUMERATE},
	{"ondblclick",      HTMLELEMENT_ONDBLCLICK,      JSPROP_ENUMERATE},
	{"onkeydown",       HTMLELEMENT_ONKEYDOWN,		 JSPROP_ENUMERATE},
	{"onkeypress",      HTMLELEMENT_ONKEYPRESS,  	 JSPROP_ENUMERATE},
	{"onkeyup",         HTMLELEMENT_ONKEYUP,		 JSPROP_ENUMERATE},
	{"onmousedown",     HTMLELEMENT_ONMOUSEDOWN,	 JSPROP_ENUMERATE},
	{"onmousemove",     HTMLELEMENT_ONMOUSEMOVE,	 JSPROP_ENUMERATE},
	{"onmouseout",      HTMLELEMENT_ONMOUSEOUT,		 JSPROP_ENUMERATE},
	{"onmouseover",     HTMLELEMENT_ONMOUSEOVER,	 JSPROP_ENUMERATE},
	{"onmouseup",       HTMLELEMENT_ONMOUSEUP,		 JSPROP_ENUMERATE},
	{"onfocus",         HTMLELEMENT_ONFOCUS,		 JSPROP_ENUMERATE},
	{"onunfocus",       HTMLELEMENT_ONUNFOCUS,		 JSPROP_ENUMERATE},
	{"onload",          HTMLELEMENT_ONLOAD,	     	 JSPROP_ENUMERATE},
	{"onunload",        HTMLELEMENT_ONUNLOAD,		 JSPROP_ENUMERATE},
	{"onsubmit",        HTMLELEMENT_ONSUBMIT,		 JSPROP_ENUMERATE},
	{"onchange",        HTMLELEMENT_ONCHANGE,        JSPROP_ENUMERATE},
	{"onlosecapture",   HTMLELEMENT_ONLOSECAPTURE,        JSPROP_ENUMERATE},
	{"innerHTML",       HTMLELEMENT_INNERHTML,       JSPROP_ENUMERATE},
	{"outerHTML",       HTMLELEMENT_OUTERHTML,       JSPROP_ENUMERATE},
	{"innerText",       HTMLELEMENT_INNERTEXT,       JSPROP_ENUMERATE},
	{"outerText",       HTMLELEMENT_OUTERTEXT,       JSPROP_ENUMERATE},
	{"offsetParent",	HTMLELEMENT_OFFSETPARENT,   JSPROP_ENUMERATE | JSPROP_READONLY},
	{"tagName",			HTMLELEMENT_TAGNAME,   		JSPROP_ENUMERATE | JSPROP_READONLY},
    {0}
};


/******************************************************************************
* jhtml_element_resolve - resolve JHTMLElement property
*
* Parameters
*
* Description
*    Used to implement lazy resolution of JSObject properties, such as
*    collection objects and the style object
*
* Returns (JSBool)
*    JS_TRUE
******************************************************************************/

JSBool jhtml_element_resolve(JSContext *cx, JSObject *obj, jsval id)
{
	HTMLElement *pElem  = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	if (!pElem || !jsmgr)
	{
		return JS_TRUE;
	}

	if (JSVAL_IS_STRING(id))
	{
		WEBC_CHAR *name = WEBC_JS_STRING_TO_WEBC_STRING(JSVAL_TO_STRING(id));

		if (!webc_c_strcmp(name, "childNodes"))
		{
			LOG_HTMLDOM("jhtml_element_resolve jhtml_collection_New(childNodes)\n");

			jhtml_collection_New(jsmgr, obj, "childNodes", pElem, ELEMENT_NODES);
		}
		else if (!webc_c_strcmp(name, "children"))
		{
			LOG_HTMLDOM("jhtml_element_resolve jhtml_collection_New(children)\n");

			jhtml_collection_New(jsmgr, obj, "children", pElem, ELEMENT_NODES);
		}
		else if (!webc_c_strcmp(name, "style"))
		{
			JSObject *styleObj = JS_DefineObject(cx, obj, "style", getJStyle(), 0, JSPROP_READONLY);

			LOG_HTMLDOM("jhtml_element_resolve(style)\n");

			if (styleObj)
			{
				jhutil_SetPrivate(cx, styleObj, pElem);
			}
		}
		else if (!webc_c_strcmp(name, "all"))
		{
			LOG_HTMLDOM("jhtml_element_resolve jhtml_collection_New(all)\n");

			jhtml_collection_New(jsmgr, obj, "all", pElem, DOCUMENT_ALL);
		}
		else
		{
		    jutil_resolve(cx, obj, id, "jhtml_element_resolve");
			// sorry, we just don't have a property of this name
		}
	}

    return JS_TRUE;
}


/**
 * Call this method when getting a Property from HTMLElement
 */
static JSBool jhtml_element_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *)jhutil_GetPrivate(cx, obj);
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	int index = JSVAL_TO_INT(id);

#ifdef WEBC_DEBUG
#ifdef LOG_HTMLDOM_ELEMENT_GETPROPERTY
	if(index != HTMLELEMENT_ONMOUSEMOVE)
	{
		if(index < 0)
			LOG_HTMLDOM("jhtml_element_getProperty %s\n", jhtml_element_props[-(index+1)].name);
		else
			LOG_HTMLDOM("jhtml_element_getProperty %d\n", index);
	}
#endif
#endif

	if (!pElem || !jsmgr)
		return JS_TRUE;

	switch (index)
	{
		case HTMLELEMENT_UNIQUEID:
		case HTMLELEMENT_ID:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pElem->Id()));
			return JS_TRUE;
		}

		case HTMLELEMENT_TITLE:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pElem->Title()));
			return JS_TRUE;
		}

		case HTMLELEMENT_LANG:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pElem->Language()));
			return JS_TRUE;
		}

		case HTMLELEMENT_DIR:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pElem->TextDirection()));
			return JS_TRUE;
		}

		case HTMLELEMENT_CLASSNAME:
		{
			WEBC_CHAR * elemClass = 0;
			WEBC_CHAR * concatenatedClasses = (WEBC_CHAR*) WEBC_MALLOC(sizeof(WEBC_CHAR) * 2);
			Enumerator *e = pElem->GetClasses();

			webc_strcpy(concatenatedClasses, (WEBC_CHAR *)L""); // start out empty

			// return a space-delimited list of the classes
			if (e && concatenatedClasses)
			{
				bool bFirstItem=true;
				do
				{
					elemClass = (WEBC_CHAR *) e->Next();

					if(elemClass)
					{
						WEBC_CHAR *newstr = (WEBC_CHAR*) WEBC_MALLOC(sizeof(WEBC_CHAR) * (webc_strlen(concatenatedClasses) + webc_strlen(elemClass) + 2));
						webc_strcpy(newstr, concatenatedClasses);
						if(!bFirstItem)
							webc_strcat(newstr, (WEBC_CHAR *)L" ");
						else
							bFirstItem = false;
						webc_strcat(newstr, elemClass);
						WEBC_DEBUG_FREE(concatenatedClasses, __FILE__, __LINE__);
						concatenatedClasses = newstr;
					}
				}
				while(elemClass);
				*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, concatenatedClasses));
				WEBC_DEBUG_FREE(concatenatedClasses, __FILE__, __LINE__);
				e->Dispose();
				return JS_TRUE;
			}
			else
			{
				//basically an error return value
				*vp = JSVAL_VOID;
				break;
			}
		}

		case HTMLELEMENT_OFFSETHEIGHT: // approx TODO need to factor in the border thickness here
		case HTMLELEMENT_CLIENTHEIGHT:
		{
			*vp = INT_TO_JSVAL(pElem->GetClientHeight());
			break;
		}

		case HTMLELEMENT_OFFSETWIDTH: // approx
		case HTMLELEMENT_CLIENTWIDTH:
		{
			*vp = INT_TO_JSVAL(pElem->GetClientWidth());
			break;
		}
		case HTMLELEMENT_OFFSETLEFT: // approx
		{
		DisplayElement *pDisp;
			pDisp=pElem->GetLastDisplayElement();
			if (pDisp)
			{

				DISPLAY_INT h = pDisp->mScreenRect.left; // April2013 - save the screenrect of last known draw
				*vp = INT_TO_JSVAL(h);
			}
			else
				*vp = INT_TO_JSVAL(pElem->GetClientLeft());
			break;
		}
		case HTMLELEMENT_CLIENTLEFT:
		{
			*vp = INT_TO_JSVAL(pElem->GetClientLeft());
			break;
		}

		case HTMLELEMENT_OFFSETTOP: // approx
		{
		DisplayElement *pDisp;
			pDisp=pElem->GetLastDisplayElement();
			if (pDisp)
			{

				DISPLAY_INT h = pDisp->mScreenRect.top; // April2013 - save the screenrect of last known draw
				*vp = INT_TO_JSVAL(h);
			}
			else
				*vp = INT_TO_JSVAL(pElem->GetClientTop());
			break;
		}
		case HTMLELEMENT_CLIENTTOP:
		{
			*vp = INT_TO_JSVAL(pElem->GetClientTop());
			break;
		}

		case HTMLELEMENT_FIRSTCHILD:
		{
			*vp = (pElem->FirstChild())? OBJECT_TO_JSVAL(pElem->FirstChild()->CreateJSObject()) : JSVAL_NULL;
			return JS_TRUE;
		}

		case HTMLELEMENT_LASTCHILD:
		{
			*vp = (pElem->LastChild())? OBJECT_TO_JSVAL(pElem->LastChild()->CreateJSObject()) : JSVAL_NULL;
			return JS_TRUE;
		}

		case HTMLELEMENT_NEXTSIBLING:
		{
			*vp = (pElem->mpNext)? OBJECT_TO_JSVAL(pElem->mpNext->CreateJSObject()) : JSVAL_NULL;
			return JS_TRUE;
		}

		case HTMLELEMENT_TAGNAME:
		case HTMLELEMENT_NODENAME:
		{
			const char *pTypeStr = HTML_TagTypeToString(pElem->GetTag());
			if (pTypeStr)
			{
				WEBC_CHAR *pTempStr = 0, *pUpperChar;
				int _s = webc_c_strlen(pTypeStr) + 1;
				WEBC_NEW_SIZED(pTempStr, WEBC_CHAR[_s],(_s*sizeof(WEBC_CHAR)));

				webc_c_strcpy(pTempStr, pTypeStr);

				pUpperChar = pTempStr;
				do
				{
					*pUpperChar = rtp_toupper(*pUpperChar);
				}
				while(*pUpperChar++);

				*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pTempStr));

				WEBC_DELETE(pTempStr);
			}
			else *vp = JSVAL_NULL;
			return JS_TRUE;
		}

		case HTMLELEMENT_NODETYPE:
		{
			*vp = INT_TO_JSVAL(1);
			return JS_TRUE;
		}

		case HTMLELEMENT_NODEVALUE:
		{
			*vp = JSVAL_NULL;
			return JS_TRUE;
		}

		case HTMLELEMENT_DOCUMENT:
		case HTMLELEMENT_OWNERDOCUMENT:
		{
			HTMLDocument *pDoc = pElem->GetDocument();
			if (pDoc)
			{
				*vp = OBJECT_TO_JSVAL(pDoc->GetJSObject());
			}
			else
			{
				*vp = JSVAL_NULL;
			}
			return JS_TRUE;
		}

		case HTMLELEMENT_PARENTNODE:
		case HTMLELEMENT_PARENTELEMENT:
		case HTMLELEMENT_OFFSETPARENT:
		{
			*vp = (pElem->mpParent)? OBJECT_TO_JSVAL(pElem->mpParent->CreateJSObject()) : JSVAL_NULL;
			return JS_TRUE;
		}

		case HTMLELEMENT_PREVIOUSSIBLING:
		{
			*vp = (pElem->mpPrev)? OBJECT_TO_JSVAL(pElem->mpPrev->CreateJSObject()) : JSVAL_NULL;
			return JS_TRUE;
		}

	  #if (WEBC_SUPPORT_INNER_HTML)
		case HTMLELEMENT_INNERHTML:
		{
			WebString innerHTML;
			pElem->AppendInnerHTML(&innerHTML);
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, innerHTML.getChars()));
			return JS_TRUE;
		}
		case HTMLELEMENT_INNERTEXT:
		case HTMLELEMENT_OUTERTEXT: // same as innerText property
		{
			WebString innerText;
			pElem->AppendRenderedText(&innerText);
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, innerText.getChars()));
			return JS_TRUE;
		}
	  #endif // WEBC_SUPPORT_INNER_HTML

	  #if (WEBC_SUPPORT_OUTER_HTML)
		case HTMLELEMENT_OUTERHTML:
		{
			WebString outerHTML;
			pElem->AppendOuterHTML(&outerHTML);
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, outerHTML.getChars()));
			return JS_TRUE;
		}
	  #endif // WEBC_SUPPORT_OUTER_HTML

		default:
			break;
	}//end switch
	return JS_TRUE;
}

/**
 * This method is called when setting a Property in HTMLElement
 */
static JSBool jhtml_element_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
    HTMLElement* pElem = (HTMLElement*) jhutil_GetPrivate(cx, obj);
	int index = JSVAL_TO_INT(id);

	if(index < 0)
		LOG_HTMLDOM("jhtml_element_setProperty %s\n", jhtml_element_props[-(JSVAL_TO_INT(id)+1)].name);
	else
		LOG_HTMLDOM("jhtml_element_setProperty %d\n", index);

	if (!pElem)
    {
    	return (JS_TRUE);
    }

	switch (index)
	{
		case HTMLELEMENT_ID:
		{
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				WEBC_CHAR *id = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp));
				pElem->SetId(id);
				return JS_TRUE;
			}
		}
		case HTMLELEMENT_TITLE:
		{
			WEBC_CHAR *title = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp));
			pElem->SetTitle(title);
			return JS_TRUE;
		}
		case HTMLELEMENT_LANG:
		{
			WEBC_CHAR *language = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp));
			pElem->SetLanguage(language);
			return JS_TRUE;

		}
		case HTMLELEMENT_DIR:
		{
			WEBC_CHAR *textDirection = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp));
			pElem->SetTextDirection(textDirection);
			return JS_TRUE;
		}
		case HTMLELEMENT_CLASSNAME:
		{
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				WEBC_CHAR *c = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp));
				pElem->SetClass(c);
				//pElem->StyleUpdateChildren();
				pElem->StyleUpdate();
				pElem->Update(0,0);
				return JS_TRUE;
			}
			break;
		}
		case HTMLELEMENT_ONCLICK:
		case HTMLELEMENT_ONDBLCLICK:
		case HTMLELEMENT_ONKEYDOWN:
		case HTMLELEMENT_ONKEYPRESS:
		case HTMLELEMENT_ONKEYUP:
		case HTMLELEMENT_ONMOUSEDOWN:
		case HTMLELEMENT_ONMOUSEMOVE:
		case HTMLELEMENT_ONMOUSEOUT:
		case HTMLELEMENT_ONMOUSEOVER:
		case HTMLELEMENT_ONMOUSEUP:
		case HTMLELEMENT_ONFOCUS:
		case HTMLELEMENT_ONUNFOCUS:
		case HTMLELEMENT_ONLOAD:
		case HTMLELEMENT_ONUNLOAD:
		case HTMLELEMENT_ONSUBMIT:
		case HTMLELEMENT_ONCHANGE:
		case HTMLELEMENT_ONLOSECAPTURE:
		{
			int32 index = 0;
			jsval args[2];
			jsval rval;
			if (JS_ValueToInt32(cx, id, &index) == JS_TRUE)
			{
				const char* name = jhtml_element_props[-(index+1)].name;
				args[0] = STRING_TO_JSVAL(JS_InternString(cx, name));
				args[1] = *vp;
			}
			return jhtml_element_attachEvent(cx, obj, 2, args, &rval);
		}

	  #if (WEBC_SUPPORT_INNER_HTML)
		case HTMLELEMENT_INNERHTML:
		{
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				WEBC_CHAR* innerHTML = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp));
				LOG_HTMLDOM("    innerHTML = %S\n", innerHTML);
				pElem->SetInnerHTML(innerHTML);
				return jutils_CheckContextState(cx);;
			}
		}
	  #endif // WEBC_SUPPORT_INNER_HTML

	  #if (WEBC_SUPPORT_OUTER_HTML)
		case HTMLELEMENT_OUTERHTML:
		{
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				WEBC_CHAR* outerHTML = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp));
				pElem->SetOuterHTML(outerHTML);
				return jutils_CheckContextState(cx);;
			}
		}

		case HTMLELEMENT_INNERTEXT:
		case HTMLELEMENT_OUTERTEXT: // same as innerText property
		{
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				WEBC_CHAR* newText = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp));
				pElem->SetRenderedText(newText);
				return jutils_CheckContextState(cx);;
			}
		}

	  #endif // WEBC_SUPPORT_OUTER_HTML

	}//end switch

	return JS_TRUE;
}

JSBool jhtml_element_attachEvent(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLElement *pElem = (HTMLElement*)jhutil_GetPrivate(cx, obj);

	LOG_HTMLDOM("jhtml_element_attachEvent\n");

	if (pElem && (argc > 1))
	{
		char *pEventName = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
		JSFunction *pFunc = 0;
		WEBC_CHAR *pEventHandlerText = 0;

		pEventHandlerText = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, argv[1]));



		//if ((pFunc = JS_ValueToFunction(cx, argv[1])))
		if (JS_ConvertValue(cx, argv[1], JSTYPE_FUNCTION, (jsval *) &pFunc /* &tempVal */))
		{
			pElem->AddJSEventHandler(jutils_StringToEventType(pEventName), pEventName, pFunc, pEventHandlerText);
		}
	}
	return JS_TRUE;
}

JSBool jhtml_element_detachEvent(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLElement *pElem = (HTMLElement*)jhutil_GetPrivate(cx, obj);

	LOG_HTMLDOM("jhtml_element_detachEvent\n");

	if (pElem && (argc > 1))
	{
		char *pEventName = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));
		WEBC_CHAR *pEventHandlerText = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, argv[1]));

		pElem->RemoveJSEventHandler(jutils_StringToEventType(pEventName), pEventHandlerText);
	}
	return JS_TRUE;
}

JSBool jhtml_element_removeChild(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLElement *pParent = (HTMLElement *)jhutil_GetPrivate(cx, obj);

	LOG_HTMLDOM("jhtml_element_removeChild\n");

	if (pParent && (argc > 0))
	{
	    JSObject *jvalobj = JSVAL_IS_OBJECT(argv[0])? JSVAL_TO_OBJECT(argv[0]) : 0;
        if (jvalobj)
        {
            HTMLElement *pChild = (HTMLElement *)jhutil_GetPrivate(cx, jvalobj);
            if (pChild)
            {
#ifdef WEBC_DEBUG
						LOG_HTMLDOM(" Removing child\n");
//						pChild->Dump();

						LOG_HTMLDOM(" Parent Node before remove child\n");
//						pParent->Dump();
#endif
                pParent->Remove(pChild);
#ifdef WEBC_DEBUG
						LOG_HTMLDOM(" Parent Node before remove child\n");
//						pParent->Dump();
#endif
				*rval = OBJECT_TO_JSVAL(jvalobj);
                return JS_TRUE;
            }
        }
	}
	return JS_FALSE;
}

JSBool jhtml_element_appendChild(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLElement *pParent = (HTMLElement *)jhutil_GetPrivate(cx, obj);

	LOG_HTMLDOM("jhtml_element_appendChild\n");

	if (pParent && (argc > 0))
	{
	    JSObject *jvalobj = JSVAL_IS_OBJECT(argv[0])? JSVAL_TO_OBJECT(argv[0]) : 0;
        if (jvalobj)
        {
            HTMLElement *pChild = (HTMLElement *)jhutil_GetPrivate(cx, jvalobj);
            if (pChild)
            {
				// make sure the child we're about to add isn't already a child of the parent node
				if(pChild->IsInserted())
				{
#define REMOVE_FROM_PARENT 1
#if (REMOVE_FROM_PARENT)
					if (pChild->mpParent)
					{
						pChild->mpParent->Remove(pChild);
					}
#endif
					if (pChild->IsInserted())
					{
					return JS_TRUE; // NOOP
					}
				}

				// if the new node is a body, then we must have a fragment created with createDocumentFragment
				if(HTML_TAG_BODY == pChild->GetTag())
				{
					// add all the children of the new node (body/fragment) to the parent
					HTMLElement *pNewNodesChild = pChild->FirstChild();
					while (pNewNodesChild)
					{
						HTMLElement *pTempElement = pNewNodesChild;
						pNewNodesChild = pNewNodesChild->mpNext;
						pChild->Remove(pTempElement); // remove the element from the body/fragment
						pTempElement->mpParent = pParent;
#ifdef WEBC_DEBUG
						LOG_HTMLDOM(" Parent Node before insert fragment:\n");
#endif
						pParent->InsertLast(pTempElement); // now add it to the "live" document
					}
					*rval = argv[0]; // return the element added
					return JS_TRUE;
				}
				else
				{
					pParent->InsertLast(pChild);
					*rval = argv[0]; // return the child appended
					return JS_TRUE;
				}
            }
        }
	}
	return JS_FALSE;
}

JSBool jhtml_element_insertBefore(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLElement *pParent = (HTMLElement *)jhutil_GetPrivate(cx, obj);

	LOG_HTMLDOM("jhtml_element_insertBefore\n");

	if (pParent && (argc > 0))
	{
	    JSObject *jNewNodeObj = JSVAL_IS_OBJECT(argv[0])? JSVAL_TO_OBJECT(argv[0]) : 0;
	    JSObject *jChildNodeObj = JSVAL_IS_OBJECT(argv[1])? JSVAL_TO_OBJECT(argv[1]) : 0; // optional
        if (jNewNodeObj)
        {
            HTMLElement *pNewNode = (HTMLElement *)jhutil_GetPrivate(cx, jNewNodeObj);
			HTMLElement *pChildNode = jChildNodeObj ? ((HTMLElement *)jhutil_GetPrivate(cx, jChildNodeObj)) : 0; // optional child to insert before
            if (pNewNode)
            {
				// if the new node is a body, then we must have a fragment created with createDocumentFragment
				if(HTML_TAG_BODY == pNewNode->GetTag())
				{
#ifdef WEBC_DEBUG
//					LOG_HTMLDOM(" Parent Node:\n");
//					pParent->Dump();
//					LOG_HTMLDOM(" Fragment Node:\n");
//					pNewNode->Dump();
//					if(pChildNode)
//					{
//						LOG_HTMLDOM(" Insert Before Node:\n");
//						pChildNode->Dump();
//					}
#endif

					// add all the children of the new node (body/fragment) to the parent
					HTMLElement *pNewNodesChild = pNewNode->FirstChild();
					while (pNewNodesChild)
					{
						HTMLElement *pTempElement = pNewNodesChild;
						pNewNodesChild = pNewNodesChild->mpNext;
						pNewNode->Remove(pTempElement); // remove the element from the body/fragment
						pTempElement->mpParent = pParent;
#ifdef WEBC_DEBUG
				LOG_HTMLDOM(" Parent Node before insert:\n");
#endif
// Hack - null mpPrev if child is first.. broken
if (pChildNode && (pParent->FirstChild() == pChildNode) && pChildNode->mpPrev)
{
	LOG_HTMLDOM(" fixing child node before insert:\n");
	pChildNode->mpPrev = 0;
}
						pParent->InsertBefore(pTempElement, pChildNode); // now add it to the "live" document
					}
					*rval = argv[0]; // return the element added
		           return JS_TRUE;

				}

                pParent->InsertBefore(pNewNode, pChildNode);
				*rval = argv[0]; // return the element added
                return JS_TRUE;
            }
        }
	}
	return JS_FALSE;
}

static JSBool jhtml_element_getElementsByTagName(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLElement *pParent = (HTMLElement *)jhutil_GetPrivate(cx, obj);
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;

	if (pParent && (argc > 0))
	{
		HTMLDocument *pDoc = pParent->GetDocument();
		if (pDoc && argc > 0 && JSVAL_IS_STRINGABLE(argv[0]))
		{
			WEBC_CHAR *tagName = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, argv[0]));
			jhtml_collection *coll;

			LOG_HTMLDOM("jhtml_element_getElementsByTagName %S\n", tagName);

			if (!webc_c_strcmp(tagName, "*"))
			{
				coll = jhtml_collection_New(jsmgr, obj, "elementsByTagName", pParent, ELEMENT_NODES); // TODO is this the right collection?
				if (coll)
				{
					*rval = OBJECT_TO_JSVAL(coll->jsObj.Get());
					return JS_TRUE;
				}
			}
			else
			{
				coll = jhtml_collection_New(jsmgr, obj, "elementsByTagName", pParent, ALL_TAGS_BY_TAGNAME);
				if (coll)
				{
					coll->nameOfAll = webc_malloc_string_copy(tagName, __FILE__, __LINE__);
					*rval = OBJECT_TO_JSVAL(coll->jsObj.Get());
					return JS_TRUE;
				}
			}
		}
	}

	*rval = JSVAL_NULL;
	return JS_TRUE;
}


JSBool jhtml_element_getAttribute(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLElement *pElement = (HTMLElement *)jhutil_GetPrivate(cx, obj);

	LOG_HTMLDOM("jhtml_element_getAttribute\n");

	*rval = JSVAL_NULL;

	// Name, value, casesensitive
	if (pElement && (argc > 0))
	{
        int CaseSensitive = 1;
	    HTML_CHAR *pAttributeName  = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, argv[0]));

		if(argc > 1)
			JS_ValueToInt32(cx, argv[1], (int32 *) &CaseSensitive);

		HTMLAttributeType attrType = HTML_ParseAttributeType(pAttributeName, 0);

		WEBC_CHAR* pAttrValue = webc_ElementGetAttribute( (HELEMENT_HANDLE)pElement, pAttributeName);

		if(pAttrValue && pAttrValue[0])
			*rval = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, pAttrValue));
	}

	return JS_TRUE;
}


JSBool jhtml_element_getAttributeNode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	LOG_HTMLDOM("jhtml_element_getAttributeNode\n");

	WEBC_ASSERT(0);
	// AK_TODO implement this
		return JS_TRUE;
}

JSBool jhtml_element_cloneNode(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WEBC_BOOL bCloneChildren = WEBC_FALSE;
	HTMLElement *pElement = (HTMLElement *)jhutil_GetPrivate(cx, obj);

	LOG_HTMLDOM("jhtml_element_cloneNode\n");

	if (argc > 0)
	{
	    JSBool bTemp = JS_FALSE;

		if(!JS_ValueToBoolean(cx, argv[0], &bTemp))
			bTemp = JS_FALSE;

		bCloneChildren = (WEBC_BOOL)bTemp;
	}

	if (pElement )
	{
		HTMLElement *pClonedElement = jhutil_CloneElement(pElement, bCloneChildren);
#ifdef WEBC_DEBUG
		LOG_HTMLDOM(" Element to clone\n");
		if (bCloneChildren)
			LOG_HTMLDOM(" The clone with children\n");
		else
			LOG_HTMLDOM(" The clone no children\n");
#endif
		if(pClonedElement)
		{
			pClonedElement->CreateJSObject();
			*rval = OBJECT_TO_JSVAL(pClonedElement->GetJSObject());
			return JS_TRUE;
		}

	}

		return JS_TRUE;
}


// implements Element.setAttribute()
// AK_TODO this method needs the rest of the HTML_ATTRIB_ items implemented
JSBool jhtml_element_setAttribute(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLElement *pElement = (HTMLElement *)jhutil_GetPrivate(cx, obj);

	LOG_HTMLDOM("jhtml_element_setAttribute\n");

	// Name, value, casesensitive
	if (pElement && (argc > 1))
	{
        int CaseSensitive = 1;
	    HTML_CHAR *pAttributeName  = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, argv[0]));
	    HTML_CHAR *pAttributeValue = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, argv[1]));
	    if (argc > 2)
        {
            CaseSensitive = JSVAL_TO_INT(argv[2]);
        }
        HTMLAttributeType myType =  HTML_ParseAttributeType (pAttributeName, webc_strlen(pAttributeName));

		switch (myType)
		{
			case HTML_ATTRIB_ID:
				pElement->SetId(pAttributeValue);
				break;
			case HTML_ATTRIB_NAME:
				pElement->SetName(pAttributeValue);
				break;
			case HTML_ATTRIB_TITLE:
				pElement->SetTitle(pAttributeValue);
				break;
			case HTML_ATTRIB_LANG:
				pElement->SetLanguage(pAttributeValue);
				break;
			case HTML_ATTRIB_DIR:
				pElement->SetTextDirection(pAttributeValue);
				break;
			case HTML_ATTRIB_CLASS:
				pElement->SetClass(pAttributeValue);
				break;
		}

        return JS_TRUE;
	}
	return JS_FALSE;
}

JSBool jhtml_element_setCapture(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLElement *pElement = (HTMLElement *)jhutil_GetPrivate(cx, obj);
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;

	LOG_HTMLDOM("jhtml_element_setCapture\n");

	if (!pElement || !jsmgr)
	{
		return JS_TRUE;
	}

	if (pElement && (argc <= 1))
	{
		DisplayManager *pDisplayMgr = jsscr->GetDisplayManager();
		pDisplayMgr->SetCapture(pElement->GetDisplayElement());
		return JS_TRUE;
	}
	return JS_FALSE;
}

JSBool jhtml_element_releaseCapture(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLElement *pElement = (HTMLElement *)jhutil_GetPrivate(cx, obj);
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;

	LOG_HTMLDOM("jhtml_element_releaseCapture\n");

	if (!pElement || !jsmgr)
	{
		return JS_TRUE;
	}

	if (pElement && (argc == 0))
	{
		DisplayManager *pDisplayMgr = jsscr->GetDisplayManager();
		pDisplayMgr->SetCapture(WEBC_NULL);

		// trigger the onlosecapture event handler
		HTMLEvent e;
		e.type = HTML_EVENT_LOSECAPTURE;
		pElement->Event(&e);

		return JS_TRUE;
	}
	return JS_FALSE;
}

JSBool jhtml_element_fireEvent(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLElement *pElem = (HTMLElement *)jhutil_GetPrivate(cx, obj);

	LOG_HTMLDOM("jhtml_element_fireEvent\n");

	if (pElem && (argc > 0))
	{
		char *pEventName = JS_GetStringBytes(JS_ValueToString(cx, argv[0]));

		HTMLEventType type = jutils_StringToEventType(pEventName);
		if(type != HTML_EVENT_ANY && type != HTML_EVENT_NONE)
		{
			HTMLEvent e;
			e.type = type;

			pElem->Event(&e);
		}
	}
	return JS_TRUE;
}

/**
 * This JSClass is the javascript wrapper for the HTMLElement class defined in the DOM Level 1
 */
JSClass JSHtmlElement =
{
	"HTMLElement", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty,
	(JSPropertyOp) jutil_delProperty, (JSPropertyOp) jhtml_element_getProperty,
	(JSPropertyOp) jhtml_element_setProperty, (JSEnumerateOp) jutil_enumerate,
	(JSResolveOp) jhtml_element_resolve, (JSConvertOp) jutil_convert,
	(JSFinalizeOp) jutil_finalize
};



JSFunctionSpec jhtml_element_methods[] =
{
	{"attachEvent", (JSNative) jhtml_element_attachEvent, 2},
	{"detachEvent", (JSNative) jhtml_element_detachEvent, 2},
	{"fireEvent", (JSNative) jhtml_element_fireEvent, 1},
	{"removeChild", (JSNative) jhtml_element_removeChild, 1},
	{"appendChild", (JSNative) jhtml_element_appendChild, 1},
	{"setAttribute", (JSNative) jhtml_element_setAttribute, 3},
	{"setCapture", (JSNative) jhtml_element_setCapture, 1},
	{"releaseCapture", (JSNative) jhtml_element_releaseCapture, 0},
	{"insertBefore", (JSNative) jhtml_element_insertBefore, 2},
  	{"getElementsByTagName",  (JSNative) jhtml_element_getElementsByTagName, 1},
  	{"getAttribute",  (JSNative) jhtml_element_getAttribute, 1},
	{"getAttributeNode",    (JSNative) jhtml_element_getAttributeNode,    1},
	{"cloneNode",    (JSNative) jhtml_element_cloneNode,    1},
	{0}
};


static JSBool JHTMLElement(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	HTMLElement *pElement;

	LOG_HTMLDOM("JHTMLElement (constructor)\n");

	if (jsmgr)
	{
		WEBC_NEW_VERBOSE(pElement, HTMLElement(0, jsmgr->GetDocument()),"HTMLElement");
		if (pElement)
		{
			pElement->SetJSObject(obj);
			jsmgr->AddNewElement(pElement);
		}
	}
	return JS_TRUE;
}

JSClass *getHtmlElement()
{
	return &JSHtmlElement;
}

JSObject *jhtml_element_Init(JSContext *cx, JSObject *proto)
{
	//initialize the HTMLElement Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSHtmlElement,
    				(JSNative) JHTMLElement, 0,
    				jhtml_element_props, jhtml_element_methods,
    				WEBC_NULL, WEBC_NULL);
}

#endif //(WEBC_SUPPORT_JSCRIPT)
