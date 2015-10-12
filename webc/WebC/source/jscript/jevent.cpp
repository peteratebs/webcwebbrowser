/*
|  JEVENT.CPP -
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

#include "jhutils.h"
#include "jevent.h"
#if (WEBC_SUPPORT_JSCRIPT)

#include "helement.hpp"
#include "delement.hpp"
#include "htmldoc.hpp"
#include "htmldom.hpp"
#include "wjsdebug.h"

#define HBROWSER_OBJ(X)   ((HTMLBrowser *)(X))
#define HDOC_OBJ(X)       ((HTMLDocument *)(X))
#define HELEMENT_OBJ(X)   ((HTMLElement *)(X))


//set the unique ids for the properties of the event class
enum jevent_tinyid
{
	EVENT_ALTKEY=-1,
	EVENT_ALTLEFT=-2,
	EVENT_BUTTON=-3,
	EVENT_CANCELBUBBLE=-4,
	EVENT_CLIENTX=-5,
	EVENT_CLIENTY=-6,
	EVENT_CONTENTOVERFLOW=-7,
	EVENT_CTRLKEY=-8,
	EVENT_CTRLLEFT=-9,
	EVENT_DATAFLD=-10,
	EVENT_FROMELEMENT=-11,
	EVENT_KEYCODE=-12,
	EVENT_NEXTPAGE=-13,
	EVENT_OFFSETX=-14,
	EVENT_OFFSETY=-15,
	EVENT_PROPERTYNAME=-16,
	EVENT_QUALIFIER=-17,
	EVENT_REASON=-18,
	EVENT_RECORDSET=-19,
	EVENT_REPEAT=-20,
	EVENT_RETURNVALUE=-21,
	EVENT_SAVETYPE=-22,
	EVENT_SCREENX=-23,
	EVENT_SCREENY=-24,
	EVENT_SHIFTKEY=-25,
	EVENT_SHIFTLEFT=-26,
	EVENT_SRCELEMENT=-27,
	EVENT_SRCFILTER=-28,
	EVENT_SRCURN=-29,
	EVENT_TOELEMENT=-30,
	EVENT_TYPE=-31,
	EVENT_WHEELDELTA=-32,
	EVENT_X=-33,
	EVENT_Y=-34
};

/**
 * Call this method when getting a Property from Event
 */
static JSBool jevent_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	if (JSVAL_IS_INT(id))
	{
		const char *str;
		HTMLEvent *pEvent = (HTMLEvent*) jhutil_GetPrivate(cx, obj);
		if (!pEvent)
		{
			return JS_TRUE;
		}

		switch (JSVAL_TO_INT(id))
		{
			case EVENT_ALTKEY:
				break;

			case EVENT_ALTLEFT:
				break;

			case EVENT_BUTTON:
				if (pEvent->type == HTML_EVENT_CLICK ||
					pEvent->type == HTML_EVENT_DBLCLICK)
				{
					*vp = INT_TO_JSVAL(1);
				}
				return JS_TRUE;

			case EVENT_CANCELBUBBLE:
				if (pEvent->flags & HTML_EVENT_FLAG_CANCEL_BUBBLE)
				{
					*vp = BOOLEAN_TO_JSVAL(JS_TRUE);
				}
				else
				{
					*vp = BOOLEAN_TO_JSVAL(JS_FALSE);
				}
				return JS_TRUE;

			case EVENT_X:       /* intentional fall-through */
			case EVENT_CLIENTX:
				if (pEvent->target)
				{
					HTMLDocument* doc = HELEMENT_OBJ(pEvent->target)->GetDocument();
					if (doc)
					{
						*vp = INT_TO_JSVAL(pEvent->data.position.x - doc->GetScrollLeft());
						return JS_TRUE;
					}
				}
				*vp = INT_TO_JSVAL(pEvent->data.position.x);
				return JS_TRUE;

			case EVENT_Y:       /* intentional fall-through */
			case EVENT_CLIENTY:
				if (pEvent->target)
				{
					HTMLDocument* doc = HELEMENT_OBJ(pEvent->target)->GetDocument();
					if (doc)
					{
						*vp = INT_TO_JSVAL(pEvent->data.position.y - doc->GetScrollTop());
						return JS_TRUE;
					}
				}
				*vp = INT_TO_JSVAL(pEvent->data.position.y);
				return JS_TRUE;

			case EVENT_CONTENTOVERFLOW:
				break;

			case EVENT_CTRLKEY:
				break;

			case EVENT_CTRLLEFT:
				break;

			case EVENT_DATAFLD:
				break;

			case EVENT_FROMELEMENT:
				// tbd - what should this be?
				*vp = OBJECT_TO_JSVAL(JS_GetParent(cx, obj));
				return JS_TRUE;

			case EVENT_KEYCODE:
				if (pEvent->type == HTML_EVENT_KEYPRESS ||
					pEvent->type == HTML_EVENT_KEYDOWN  ||
					pEvent->type == HTML_EVENT_KEYUP)
				{
					*vp = INT_TO_JSVAL(pEvent->data.key);
				}
				else
				{
					*vp = INT_TO_JSVAL(0);
				}
				return JS_TRUE;

			case EVENT_NEXTPAGE:
				break;

			case EVENT_OFFSETX:
				if (pEvent->srcElement)
				{
					DISPLAY_INT x, y;
					HTML_GetElementDocumentPosition(HELEMENT_OBJ(pEvent->srcElement), &x, &y);
					*vp = INT_TO_JSVAL(pEvent->data.position.x - x);
					return JS_TRUE;
				}
				*vp = INT_TO_JSVAL(pEvent->data.position.x);
				return JS_TRUE;

			case EVENT_OFFSETY:
				if (pEvent->srcElement)
				{
					DISPLAY_INT x, y;
					HTML_GetElementDocumentPosition(HELEMENT_OBJ(pEvent->srcElement), &x, &y);
					*vp = INT_TO_JSVAL(pEvent->data.position.y - y);
					return JS_TRUE;
				}
				*vp = INT_TO_JSVAL(pEvent->data.position.y);
				return JS_TRUE;

			case EVENT_PROPERTYNAME:
				break;

			case EVENT_QUALIFIER:
				break;

			case EVENT_REASON:
				break;

			case EVENT_RECORDSET:
				break;

			case EVENT_REPEAT:
				if (pEvent->type == HTML_EVENT_KEYDOWN)
				{
					*vp = BOOLEAN_TO_JSVAL(JS_TRUE);
				}
				else
				{
					*vp = BOOLEAN_TO_JSVAL(JS_FALSE);
				}
				return JS_TRUE;

			case EVENT_RETURNVALUE:
				break;

			case EVENT_SAVETYPE:
				break;

			case EVENT_SCREENX:
				if (pEvent->srcElement)
				{
					DISPLAY_INT docX, docY;
					DISPLAY_INT screenX, screenY;

					HTML_GetElementDocumentPosition(HELEMENT_OBJ(pEvent->srcElement), &docX, &docY);
					HTML_GetElementScreenPosition(HELEMENT_OBJ(pEvent->srcElement), &screenX, &screenY);

					*vp = INT_TO_JSVAL(pEvent->data.position.x - docX + screenX);
					return JS_TRUE;
				}
				*vp = INT_TO_JSVAL(pEvent->data.position.x);
				return JS_TRUE;

			case EVENT_SCREENY:
				if (pEvent->srcElement)
				{
					DISPLAY_INT docX, docY;
					DISPLAY_INT screenX, screenY;

					HTML_GetElementDocumentPosition(HELEMENT_OBJ(pEvent->srcElement), &docX, &docY);
					HTML_GetElementScreenPosition(HELEMENT_OBJ(pEvent->srcElement), &screenX, &screenY);

					*vp = INT_TO_JSVAL(pEvent->data.position.y - docY + screenY);
					return JS_TRUE;
				}
				*vp = INT_TO_JSVAL(pEvent->data.position.y);
				return JS_TRUE;

			case EVENT_SHIFTKEY:
				break;

			case EVENT_SHIFTLEFT:
				break;

			case EVENT_SRCELEMENT:
			{
				HTMLElement* srcElement = (HTMLElement*) pEvent->srcElement;
				*vp = OBJECT_TO_JSVAL(srcElement? srcElement->CreateJSObject() : WEBC_NULL);
				return JS_TRUE;
			}

			case EVENT_SRCFILTER:
				break;

			case EVENT_SRCURN:
				break;

			case EVENT_TOELEMENT:
				break;

			case EVENT_TYPE:
				str = jutils_EventTypeToString(pEvent->type);
				if (str)
				{
					//dont use the "on" portion of the event name
					str += 2;
					*vp = STRING_TO_JSVAL(JS_NewStringCopyZ(cx, str));
				}
				else
				{
					*vp = JSVAL_NULL;
				}
				break;

			case EVENT_WHEELDELTA:
				break;

			default:
				break;
		}//end switch
	}

	return JS_TRUE;
}

/**
 * This method is called when setting a Property in Event
 */
static JSBool jevent_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLEvent *pEvent = (HTMLEvent *)jhutil_GetPrivate(cx, obj);
	if (!pEvent)
	{
		return JS_TRUE;
	}

	switch (JSVAL_TO_INT(id))
	{
		case EVENT_ALTKEY:
			break;
		case EVENT_ALTLEFT:
			break;
		case EVENT_BUTTON:
			break;
		case EVENT_CANCELBUBBLE:
			if (JSVAL_TO_BOOLEAN(*vp) == JS_TRUE)
			{
				pEvent->flags |= HTML_EVENT_FLAG_CANCEL_BUBBLE;
			}
			else
			{
				pEvent->flags &= ~HTML_EVENT_FLAG_CANCEL_BUBBLE;
			}
			break;
		case EVENT_CLIENTX:
			break;
		case EVENT_CLIENTY:
			break;
		case EVENT_CONTENTOVERFLOW:
			break;
		case EVENT_CTRLKEY:
			break;
		case EVENT_CTRLLEFT:
			break;
		case EVENT_DATAFLD:
			break;
		case EVENT_FROMELEMENT:
			break;
		case EVENT_KEYCODE:
			break;
		case EVENT_NEXTPAGE:
			break;
		case EVENT_OFFSETX:
			break;
		case EVENT_OFFSETY:
			break;
		case EVENT_PROPERTYNAME:
			break;
		case EVENT_QUALIFIER:
			break;
		case EVENT_REASON:
			break;
		case EVENT_RECORDSET:
			break;
		case EVENT_REPEAT:
			break;
		case EVENT_RETURNVALUE:
			if (JSVAL_IS_BOOLEAN(*vp))
			{
				JSBool result = JSVAL_TO_BOOLEAN(*vp);
				if (pEvent->type == HTML_EVENT_MOUSEOVER)
				{
					if (result == JS_FALSE)
					{
						pEvent->flags &= ~HTML_EVENT_FLAG_CANCEL_DEFAULT;
					}
					else
					{
						pEvent->flags |= HTML_EVENT_FLAG_CANCEL_DEFAULT;
					}
				}
				else
				{
					if (result == JS_TRUE)
					{
						pEvent->flags &= ~HTML_EVENT_FLAG_CANCEL_DEFAULT;
					}
					else
					{
						pEvent->flags |= HTML_EVENT_FLAG_CANCEL_DEFAULT;
					}
				}
			}
			else if (JSVAL_IS_INT(*vp))
			{
				pEvent->returnValue = JSVAL_TO_INT(*vp);
			}
			else if (JSVAL_IS_STRING(*vp))
			{
			}
			else if (JSVAL_IS_OBJECT(*vp))
			{
			}

			break;
		case EVENT_SAVETYPE:
			break;
		case EVENT_SCREENX:
			break;
		case EVENT_SCREENY:
			break;
		case EVENT_SHIFTKEY:
			break;
		case EVENT_SHIFTLEFT:
			break;
		case EVENT_SRCELEMENT:
			break;
		case EVENT_SRCFILTER:
			break;
		case EVENT_SRCURN:
			break;
		case EVENT_TOELEMENT:
			break;
		case EVENT_TYPE:
			break;
		case EVENT_WHEELDELTA:
			break;
		case EVENT_X:
			break;
		case EVENT_Y:
			break;
	}//end switch
	return JS_TRUE;
}

JSBool jevent_resolve(JSContext *cx, JSObject *obj, jsval id)
{
	HTMLEvent *pEvent = (HTMLEvent *)jhutil_GetPrivate(cx, obj);
	WEBC_CHAR *name=0;

	if (!pEvent)
	{
		return JS_TRUE;
	}
	if (JSVAL_IS_STRING(id))
	{
		name = WEBC_JS_STRING_TO_WEBC_STRING(JSVAL_TO_STRING(id));
	}

	return(jutil_resolve(cx, obj, id, "event"));
}

/**
 * This JSClass is the javascript wrapper for the Event class defined in the DOM Level 1
 */
JSClass JSEvent=
{
	"Event", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty,
	(JSPropertyOp) jutil_delProperty, (JSPropertyOp) jevent_getProperty,
	(JSPropertyOp) jevent_setProperty, (JSEnumerateOp) jutil_enumerate,
	(JSResolveOp) jevent_resolve, (JSConvertOp) jutil_convert,
	(JSFinalizeOp) jutil_finalize
};


JSPropertySpec jevent_props[] =
{
	{"altKey",          EVENT_ALTKEY,          JSPROP_READONLY | JSPROP_ENUMERATE},
	{"altLeft",         EVENT_ALTLEFT,         JSPROP_READONLY | JSPROP_ENUMERATE},
	{"button",          EVENT_BUTTON,          JSPROP_READONLY | JSPROP_ENUMERATE},
	{"cancleBubble",    EVENT_CANCELBUBBLE,    JSPROP_ENUMERATE},
	{"clientX",         EVENT_CLIENTX,         JSPROP_READONLY | JSPROP_ENUMERATE},
	{"clientY",         EVENT_CLIENTY,         JSPROP_READONLY | JSPROP_ENUMERATE},
	{"contentOverflow", EVENT_CONTENTOVERFLOW, JSPROP_READONLY | JSPROP_ENUMERATE},
	{"ctrlKey",         EVENT_CTRLKEY,         JSPROP_READONLY | JSPROP_ENUMERATE},
	{"ctrlLeft",        EVENT_CTRLLEFT,        JSPROP_READONLY | JSPROP_ENUMERATE},
	{"dataFld",         EVENT_DATAFLD,         JSPROP_READONLY | JSPROP_ENUMERATE},
	{"fromElement",     EVENT_FROMELEMENT,     JSPROP_READONLY | JSPROP_ENUMERATE},
	{"keyCode",         EVENT_KEYCODE,         JSPROP_READONLY | JSPROP_ENUMERATE},
	{"nextPage",        EVENT_NEXTPAGE,        JSPROP_READONLY | JSPROP_ENUMERATE},
	{"offsetX",         EVENT_OFFSETX,         JSPROP_READONLY | JSPROP_ENUMERATE},
	{"offsetY",         EVENT_OFFSETY,         JSPROP_READONLY | JSPROP_ENUMERATE},
	{"propertName",     EVENT_PROPERTYNAME,    JSPROP_READONLY | JSPROP_ENUMERATE},
	{"qualifier",       EVENT_QUALIFIER,       JSPROP_READONLY | JSPROP_ENUMERATE},
	{"reason",          EVENT_REASON,          JSPROP_READONLY | JSPROP_ENUMERATE},
	{"recordSet",       EVENT_RECORDSET,       JSPROP_READONLY | JSPROP_ENUMERATE},
	{"repeat",          EVENT_REPEAT,          JSPROP_READONLY | JSPROP_ENUMERATE},
	{"returnValue",     EVENT_RETURNVALUE,    JSPROP_ENUMERATE},
	{"saveType",        EVENT_SAVETYPE,        JSPROP_READONLY | JSPROP_ENUMERATE},
	{"screenX",         EVENT_SCREENX,         JSPROP_READONLY | JSPROP_ENUMERATE},
	{"screenY",         EVENT_SCREENY,         JSPROP_READONLY | JSPROP_ENUMERATE},
	{"shiftKey",        EVENT_SHIFTKEY,        JSPROP_READONLY | JSPROP_ENUMERATE},
	{"shiftLeft",       EVENT_SHIFTLEFT,       JSPROP_READONLY | JSPROP_ENUMERATE},
	{"srcElement",      EVENT_SRCELEMENT,      JSPROP_READONLY | JSPROP_ENUMERATE},
	{"srcFilter",       EVENT_SRCFILTER,       JSPROP_READONLY | JSPROP_ENUMERATE},
	{"srcUrn",          EVENT_SRCURN,          JSPROP_READONLY | JSPROP_ENUMERATE},
	{"toElement",       EVENT_TOELEMENT,       JSPROP_READONLY | JSPROP_ENUMERATE},
	{"type",            EVENT_TYPE,            JSPROP_READONLY | JSPROP_ENUMERATE},
	{"wheelDelta",      EVENT_WHEELDELTA,      JSPROP_READONLY | JSPROP_ENUMERATE},
	{"x",               EVENT_X,               JSPROP_READONLY | JSPROP_ENUMERATE},
	{"y",               EVENT_Y,               JSPROP_READONLY | JSPROP_ENUMERATE},
	{0}
};

JSFunctionSpec jevent_methods[] =
{
	{0}
};

static JSBool JEvent(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	return JS_TRUE;
}

JSClass *getJEventClass()
{
	return &JSEvent;
}

JSObject *jevent_Init(JSContext *cx, JSObject *proto)
{
	//initialize the Event Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSEvent,
    				(JSNative) JEvent, 0,
    				jevent_props, jevent_methods,
    				WEBC_NULL, WEBC_NULL);
}

#endif //(WEBC_SUPPORT_JSCRIPT)
