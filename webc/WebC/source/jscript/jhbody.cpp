/*
|  JHBODY.CPP -
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
#include "jhbody.h"
#if (WEBC_SUPPORT_JSCRIPT)

#include "jhutils.h"
#include "jhelem.h"
#include "hbody.hpp"
#include "htmldoc.hpp"
#include "wjscript.hpp"
#include "objref.cpp"
#include "webcmem.h"
#include "jhinput.h"
#include "wjsdebug.h"
#include "webcLog.h"
#include "webcassert.h"


extern JSPropertySpec jhtml_body_props[];

//set the unique ids for the properties of the body class
enum jhtml_body_tinyid
{
	HTMLBODYELEMENT_ALINK=      -1,
	HTMLBODYELEMENT_ALINKCOLOR= -2,
	HTMLBODYELEMENT_BACKGROUND= -3,
	HTMLBODYELEMENT_BGCOLOR=    -4,
	HTMLBODYELEMENT_LINK=       -5,
	HTMLBODYELEMENT_LINKCOLOR=  -6,
	HTMLBODYELEMENT_SCROLLLEFT= -7,
	HTMLBODYELEMENT_SCROLLTOP=  -8,
	HTMLBODYELEMENT_TEXT=       -9,
	HTMLBODYELEMENT_VLINK=      -10,
	HTMLBODYELEMENT_VLINKCOLOR= -11
};

static WEBC_READONLY HTMLAttributeType _jhbody_id_to_param[] =
{
	HTML_ATTRIB_ALINK,
	HTML_ATTRIB_ALINK,
	HTML_ATTRIB_BACKGROUND,
	HTML_ATTRIB_BGCOLOR,
	HTML_ATTRIB_LINK,
	HTML_ATTRIB_LINK,
	HTML_UNKNOWN_ATTRIB,
	HTML_UNKNOWN_ATTRIB,
	HTML_ATTRIB_TEXT,
	HTML_ATTRIB_VLINK,
	HTML_ATTRIB_VLINK
};

/**
 * Call this method when getting a Property from HTMLBody
 */
static JSBool jhtml_body_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	int index = JSVAL_TO_INT(id);

#ifdef WEBC_DEBUG
	if(index < 0)
		LOG_HTMLDOM("jhtml_body_getProperty %s\n", jhtml_body_props[-(index+1)].name);
	else
		LOG_HTMLDOM("jhtml_body_getProperty %d\n", index);
#endif

	if (index)
	{
		HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
		HTMLBody *body = (HTMLBody*) pElem;

		if (!pElem)
		{
			return JS_TRUE;
		}

		switch (JSVAL_TO_INT(id))
		{
			case HTMLBODYELEMENT_ALINKCOLOR:
			case HTMLBODYELEMENT_ALINK:
			case HTMLBODYELEMENT_BGCOLOR:
			case HTMLBODYELEMENT_LINKCOLOR:
			case HTMLBODYELEMENT_LINK:
			case HTMLBODYELEMENT_TEXT:
			case HTMLBODYELEMENT_VLINKCOLOR:
			case HTMLBODYELEMENT_VLINK:
			{
				HTMLColor c;

				switch (JSVAL_TO_INT(id))
				{
					case HTMLBODYELEMENT_ALINKCOLOR:
					case HTMLBODYELEMENT_ALINK:
						c = body->ALinkColor();
						break;

					case HTMLBODYELEMENT_BGCOLOR:
						c = body->BodyBackgroundColor();
						break;

					case HTMLBODYELEMENT_LINKCOLOR:
					case HTMLBODYELEMENT_LINK:
						c = body->LinkColor();
						break;

					case HTMLBODYELEMENT_TEXT:
						c = body->TextColor();
						break;

					case HTMLBODYELEMENT_VLINKCOLOR:
					case HTMLBODYELEMENT_VLINK:
						c = body->VLinkColor();

					default:
						c = 0;
						break;
				}

				WEBC_CHAR str[8];
				HTML_PrintColor(str, c);
				*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, str));
				return JS_TRUE;
			}

			case HTMLBODYELEMENT_SCROLLLEFT:
			{
				HTMLDocument* doc = body->GetDocument();
				*vp = INT_TO_JSVAL(doc? doc->GetScrollLeft() : 0);
				return JS_TRUE;
			}

			case HTMLBODYELEMENT_SCROLLTOP:
			{
				HTMLDocument* doc = body->GetDocument();
				*vp = INT_TO_JSVAL(doc? doc->GetScrollTop() : 0);
				return JS_TRUE;
			}

			case HTMLBODYELEMENT_BACKGROUND:
			{
			  #if (WEBC_SUPPORT_IMAGES && WEBC_SUPPORT_BACKGROUND_IMAGES)
				*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, body->Background()));
			  #endif // WEBC_SUPPORT_IMAGES
				break;
			}


		}
	}

	return JS_TRUE;
}

JSBool jhtml_body_setPropertyHtml(JSContext *cx, JSObject *obj, jsval *vp, HTMLAttributeType htmlAttribute)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLBody *body = (HTMLBody*) pElem;

	if (!pElem)
		return JS_TRUE;

	switch (htmlAttribute)
	{
		case HTML_UNKNOWN_ATTRIB:
            break;
        case HTML_ATTRIB_ALINK:
		case HTML_ATTRIB_BACKGROUND:
		case HTML_ATTRIB_BGCOLOR:
		case HTML_ATTRIB_LINK:
		case HTML_ATTRIB_TEXT:
		case HTML_ATTRIB_VLINK:
		case HTML_ATTRIB_WIDTH:
		case HTML_ATTRIB_HEIGHT:
		case HTML_ATTRIB_ONCLICK:
		case HTML_ATTRIB_ONDBLCLICK:
		case HTML_ATTRIB_ONKEYDOWN:
		case HTML_ATTRIB_ONKEYPRESS:
		case HTML_ATTRIB_ONKEYUP:
		case HTML_ATTRIB_ONMOUSEDOWN:
		case HTML_ATTRIB_ONMOUSEMOVE:
		case HTML_ATTRIB_ONMOUSEOUT:
		case HTML_ATTRIB_ONMOUSEOVER:
		case HTML_ATTRIB_ONMOUSEUP:
		case HTML_ATTRIB_ONFOCUS:
//		case HTML_ATTRIB_ONUNFOCUS:
		case HTML_ATTRIB_ONLOAD:
		case HTML_ATTRIB_ONUNLOAD:
		case HTML_ATTRIB_ONSUBMIT:
		case HTML_ATTRIB_ONCHANGE:
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

//			LOG_PRINTF("Id == %X %d (-1 - JSVAL_TO_INT(id)) == %d %d \n", id, JSVAL_TO_INT(id), -1 - JSVAL_TO_INT(id), _jhbody_id_to_param[-1 - JSVAL_TO_INT(id)]);
//			body->SetProperty(_jhbody_id_to_param[-1 - JSVAL_TO_INT(id)], str);
// << EDIT 20080101 - Fixed bug mapping id to HTMK id

            LOG_PRINTF("jhtml_body_setPropertyHtml setting %s: to %S \n", HTML_AttributeTypeToString (htmlAttribute), str);
  			HTMLDocument* doc = body->GetDocument();
            LOG_PRINTF("call SetProperty.. body == %X doc == %X\n", body, doc);
//            body->SetProperty(htmlAttribute, str, doc);
           body->SetProperty(htmlAttribute, str);
		   body->StyleUpdate();


 //           DisplayElement *pDisp =  body->GetDisplayElement(WEBC_FALSE);
//		HTMLBodyDisplay *bodyDisplay = (body)? (HTMLBodyDisplay *) body->GetDisplayElement(WEBC_FALSE) : 0;
//            bodyDisplay->SetLocalStyleModified();

 //           body->Udate(0,0);
//			HTMLDocument* doc = body->GetDocument();
            if (doc)
                doc->Refresh();

            HTMLFrameSet* framesetElement = body->GetFrameSet();
            HTMLFrameSetDisplay *mpFrameSet = (HTMLFrameSetDisplay*) ((framesetElement)? framesetElement->GetDisplayElement() : 0);
            if (0 && mpFrameSet)
            {
            LOG_PRINTF("frame set jhtml_body_setPropertyHtml setting %s: to %S \n", HTML_AttributeTypeToString (htmlAttribute), str);
// void HTMLFrame::
//	            mpFrameSet->Notify(NOTIFY_STYLE_CHANGE);

//                mpFrameSet->Update(0,0);
//                mpFrameSet->GetHTMLParentDisplay()->SetElementStyle();
//                mpFrameSet-> ->SetProperty(htmlAttribute, str, body->GetDocument(), 1 /* int call_base*/)
//                mpFrameSet->StyleUpdate();
            }
			JS_RemoveRoot(cx, &jsStr);
			break;
		}
	}
    return JS_TRUE;
}


static JSBool jhtml_body_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLBody *body = (HTMLBody*) pElem;
	int index = JSVAL_TO_INT(id);

#ifdef WEBC_DEBUG
	if(index < 0)
		LOG_HTMLDOM("jhtml_body_setProperty %s\n", jhtml_body_props[-(index+1)].name);
	else
		LOG_HTMLDOM("jhtml_body_setProperty %d\n", index);
#endif

	if (!pElem)
		return JS_TRUE;

	switch (index)
	{
		case HTMLBODYELEMENT_ALINKCOLOR:
		case HTMLBODYELEMENT_ALINK:
		case HTMLBODYELEMENT_BACKGROUND:
		case HTMLBODYELEMENT_BGCOLOR:
		case HTMLBODYELEMENT_LINKCOLOR:
		case HTMLBODYELEMENT_LINK:
		case HTMLBODYELEMENT_TEXT:
		case HTMLBODYELEMENT_VLINKCOLOR:
		case HTMLBODYELEMENT_VLINK:
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

//			LOG_PRINTF("Id == %X %d (-1 - JSVAL_TO_INT(id)) == %d %d \n", id, JSVAL_TO_INT(id), -1 - JSVAL_TO_INT(id), _jhbody_id_to_param[-1 - JSVAL_TO_INT(id)]);
//			body->SetProperty(_jhbody_id_to_param[-1 - JSVAL_TO_INT(id)], str);
// << EDIT 20080101 - Fixed bug mapping id to HTMK id
			body->SetProperty(_jhbody_id_to_param[-JSVAL_TO_INT(id)], str);
			body->StyleUpdate();

			JS_RemoveRoot(cx, &jsStr);

			break;
		}

		case HTMLBODYELEMENT_SCROLLLEFT:
		{
			HTMLDocument* doc = body->GetDocument();
			int32 i;

			if (doc && JS_ValueToInt32(cx, *vp, &i))
			{
				doc->SetScrollLeft(i);
			}
			break;
		}

		case HTMLBODYELEMENT_SCROLLTOP:
		{
			HTMLDocument* doc = body->GetDocument();
			int32 i;

			if (doc && JS_ValueToInt32(cx, *vp, &i))
			{
				doc->SetScrollTop(i);
			}
			break;
		}
	}
    return JS_TRUE;
}

/**
 * This JSClass is the javascript wrapper for the HTMLBody class defined in the DOM Level 1
 */
JSClass JSHtmlBody =
{
	"Body", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty,
	(JSPropertyOp) jutil_delProperty, (JSPropertyOp) jhtml_body_getProperty,
	(JSPropertyOp) jhtml_body_setProperty, (JSEnumerateOp) jutil_enumerate,
	(JSResolveOp) jhtml_element_resolve, (JSConvertOp) jutil_convert,
	(JSFinalizeOp) jutil_finalize
};




JSClass *getHtmlBodyElement()
{
	return &JSHtmlBody;
}

JSPropertySpec jhtml_body_props[] =
{
	{"aLink",        HTMLBODYELEMENT_ALINK,       JSPROP_ENUMERATE},
	{"alinkColor",   HTMLBODYELEMENT_ALINKCOLOR,  JSPROP_ENUMERATE},
	{"background",   HTMLBODYELEMENT_BACKGROUND,  JSPROP_ENUMERATE},
	{"bgColor",      HTMLBODYELEMENT_BGCOLOR,     JSPROP_ENUMERATE},
	{"link",         HTMLBODYELEMENT_LINK,        JSPROP_ENUMERATE},
	{"linkColor",    HTMLBODYELEMENT_LINKCOLOR,   JSPROP_ENUMERATE},
	{"scrollLeft",   HTMLBODYELEMENT_SCROLLLEFT,  JSPROP_ENUMERATE},
	{"scrollTop",    HTMLBODYELEMENT_SCROLLTOP,   JSPROP_ENUMERATE},
	{"text",         HTMLBODYELEMENT_TEXT,        JSPROP_ENUMERATE},
	{"vLink",        HTMLBODYELEMENT_VLINK,       JSPROP_ENUMERATE},
	{"vlinkColor",   HTMLBODYELEMENT_VLINKCOLOR,  JSPROP_ENUMERATE},
	{0}
};

JSFunctionSpec jhtml_body_methods[] =
{
	{"blur",   (JSNative) jhtml_input_blur, 0},
	{"focus",  (JSNative) jhtml_input_focus, 0},
	{0}
};


static JSBool JHTMLBody(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	if (jsmgr)
	{
		HTMLBody *pBody;
		WEBC_NEW_VERBOSE(pBody, HTMLBody(jsmgr->GetDocument()),"HTMLBody");

		HTMLTagParse tag[1];
		HTML_InitTagParse(tag, HTML_TAG_BODY);

		if (pBody)
		{
			pBody->SetJSObject(obj);
			pBody->SetTagProperties(tag, jsmgr->GetDocument());
			jsmgr->AddNewElement(pBody);
		}
	}
	return JS_TRUE;
}

JSObject *jhtml_body_Init(JSContext *cx, JSObject *proto)
{
	//initialize the HTMLBody Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSHtmlBody,
    				(JSNative) JHTMLBody, 0,
    				jhtml_body_props, jhtml_body_methods,
    				WEBC_NULL, WEBC_NULL);
}
#endif //(WEBC_SUPPORT_JSCRIPT)

