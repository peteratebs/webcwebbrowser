/*
|  JHINPUT.CPP -
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
#include "jhinput.h"
#include "jhutils.h"
#include "htmldom.hpp"
#if (WEBC_SUPPORT_JSCRIPT)

#include "hinput.hpp"
#include "hform.hpp"
#include "htmldoc.hpp"
#include "wjscript.hpp"
#include "objref.cpp"
#include "jhelem.h"
#include "wjsdebug.h"

#include "hibutton.hpp"
#include "hchkbox.hpp"
#include "hedit.hpp"
#include "hhidden.hpp"
#include "hpasswd.hpp"
#include "hradio.hpp"
#include "hreset.hpp"
#include "hsubmit.hpp"
#include "webcmem.h"

//set the unique ids for the properties of the input class
enum jhtml_input_tinyid
{
	HTMLINPUTELEMENT_DEFAULTVALUE=-1,
	HTMLINPUTELEMENT_DEFAULTCHECKED=-2,
	HTMLINPUTELEMENT_FORM=-3,
	HTMLINPUTELEMENT_ACCEPT=-4,
	HTMLINPUTELEMENT_ACCESSKEY=-5,
	HTMLINPUTELEMENT_ALIGN=-6,
	HTMLINPUTELEMENT_ALT=-7,
	HTMLINPUTELEMENT_CHECKED=-8,
	HTMLINPUTELEMENT_DISABLED=-9,
	HTMLINPUTELEMENT_MAXLENGTH=-10,
	HTMLINPUTELEMENT_NAME=-11,
	HTMLINPUTELEMENT_READONLY=-12,
	HTMLINPUTELEMENT_SIZE=-13,
	HTMLINPUTELEMENT_SRC=-14,
	HTMLINPUTELEMENT_TABINDEX=-15,
	HTMLINPUTELEMENT_TYPE=-16,
	HTMLINPUTELEMENT_VALUE=-17
};

JSPropertySpec jhtml_input_props[] = {
	{"defaultValue",   HTMLINPUTELEMENT_DEFAULTVALUE, JSPROP_ENUMERATE},
	{"defaultChecked", HTMLINPUTELEMENT_DEFAULTCHECKED, JSPROP_ENUMERATE},
	{"form",           HTMLINPUTELEMENT_FORM, JSPROP_ENUMERATE},
	{"accept",         HTMLINPUTELEMENT_ACCEPT, JSPROP_ENUMERATE},
	{"accessKey",      HTMLINPUTELEMENT_ACCESSKEY, JSPROP_ENUMERATE},
	{"align",          HTMLINPUTELEMENT_ALIGN, JSPROP_ENUMERATE},
	{"alt",            HTMLINPUTELEMENT_ALT, JSPROP_ENUMERATE},
	{"checked",        HTMLINPUTELEMENT_CHECKED, JSPROP_ENUMERATE},
	{"disabled",       HTMLINPUTELEMENT_DISABLED, JSPROP_ENUMERATE},
	{"maxLength",      HTMLINPUTELEMENT_MAXLENGTH, JSPROP_ENUMERATE},
	{"name",           HTMLINPUTELEMENT_NAME, JSPROP_ENUMERATE},
	{"readOnly",       HTMLINPUTELEMENT_READONLY, JSPROP_ENUMERATE},
	{"size",           HTMLINPUTELEMENT_SIZE, JSPROP_ENUMERATE},
	{"src",            HTMLINPUTELEMENT_SRC, JSPROP_ENUMERATE},
	{"tabIndex",       HTMLINPUTELEMENT_TABINDEX, JSPROP_ENUMERATE},
	{"type",           HTMLINPUTELEMENT_TYPE, JSPROP_ENUMERATE},
	{"value",          HTMLINPUTELEMENT_VALUE, JSPROP_ENUMERATE},
	{0}
};

JSFunctionSpec jhtml_input_methods[] =
{
	{"blur",   (JSNative) jhtml_input_blur, 0},
	{"focus",  (JSNative) jhtml_input_focus, 0},
	{"select", (JSNative) jhtml_input_select, 0},
	{"click",  (JSNative) jhtml_input_click, 0},
	{"setAttribute",  (JSNative) jhtml_input_setAttribute, 0},
  	{0}
};


// local prototypes
HTMLInputType jhtml_input_determine_type(HTML_CHAR *pAttributeValue);
HTMLInput *jhtml_input_change_type(HTMLInput *pInput, HTMLInputType newInputType, HTMLDocument *pDocument);


/**
 * Call this method when getting a Property from HTMLInput
 */
static JSBool jhtml_input_getProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLInput *input = (HTMLInput *) pElem;

	if (!input)
		return JS_TRUE;

	switch (JSVAL_TO_INT(id))
	{
		case HTMLINPUTELEMENT_VALUE:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, input->Value()));
			return JS_TRUE;
		}
		case HTMLINPUTELEMENT_DEFAULTCHECKED:
		{
			// AK_TODO this is not quite right
			if (input->DefValue())
			{
				*vp = BOOLEAN_TO_JSVAL(JS_TRUE);
			}
			else
			{
				*vp = BOOLEAN_TO_JSVAL(JS_FALSE);
			}
			return JS_TRUE;
		}
		case HTMLINPUTELEMENT_DEFAULTVALUE:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, input->DefValue()));
			return JS_TRUE;
		}
		case HTMLINPUTELEMENT_CHECKED:
		{
			JSBool ans = JS_FALSE;
			if (input->Checked())
			{
				ans = JS_TRUE;
			}
			*vp = BOOLEAN_TO_JSVAL(ans);
			return JS_TRUE;
		}
		case HTMLINPUTELEMENT_FORM:
		{
			*vp = (input->Form())? OBJECT_TO_JSVAL(input->Form()->CreateJSObject()) : JSVAL_NULL;
			return JS_TRUE;
		}

		case HTMLINPUTELEMENT_ACCEPT:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, input->Accept()));
			return JS_TRUE;
		}
		case HTMLINPUTELEMENT_ACCESSKEY:
		{
			char ak[2];
			ak[0] = (char)input->AccessKey();
			ak[1] = '\0';
			JSString *str = JS_NewStringCopyZ(cx,ak);
			*vp = STRING_TO_JSVAL(str);
			return JS_TRUE;
		}
		case HTMLINPUTELEMENT_ALIGN:
		{
			switch(input->GetFloat())
			{
			case TU_FLOAT_LEFT:
				*vp = STRING_TO_JSVAL(JS_InternString(cx, "left"));
				break;
			case TU_FLOAT_RIGHT:
				*vp = STRING_TO_JSVAL(JS_InternString(cx, "right"));
				break;
			case TU_FLOAT_NONE:
				*vp = STRING_TO_JSVAL(JS_InternString(cx, "undefined"));
				break;
			}
			return JS_TRUE;
		}
		case HTMLINPUTELEMENT_ALT:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, input->Alt()));
			return JS_TRUE;
		}
		case HTMLINPUTELEMENT_DISABLED:
		{
			JSBool ans = (input->mFlags & HINPUT_FLAG_DISABLED)? JS_TRUE : JS_FALSE;
			*vp = BOOLEAN_TO_JSVAL(ans);
			return JS_TRUE;
		}
		case HTMLINPUTELEMENT_MAXLENGTH:
		{
			WEBC_SIZE maxLen = input->MaxLength();
			*vp = INT_TO_JSVAL(maxLen);
			return JS_TRUE;
		}
		case HTMLINPUTELEMENT_NAME:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, input->Name()));
			return JS_TRUE;
		}
		case HTMLINPUTELEMENT_READONLY:
		{
			JSBool ans = (input->mFlags & HINPUT_FLAG_READONLY)? JS_TRUE : JS_FALSE;
			*vp = BOOLEAN_TO_JSVAL(ans);
			return JS_TRUE;
		}
		case HTMLINPUTELEMENT_SIZE:
		{
			WEBC_SIZE sizeVal = input->Size();
			*vp = INT_TO_JSVAL(sizeVal);
			return JS_TRUE;
		}
		case HTMLINPUTELEMENT_SRC:
		{
			*vp = STRING_TO_JSVAL(WEBC_JS_NEW_STRING_COPY_Z(cx, input->Src()));
			return JS_TRUE;
		}

		case HTMLINPUTELEMENT_TABINDEX:
		{
			int t = input->TabIndex();
			*vp = INT_TO_JSVAL(t);
			return JS_TRUE;
		}
		case HTMLINPUTELEMENT_TYPE:
		{
			const char *str = HTML_InputTypeToString(input->GetInputType());
			*vp = STRING_TO_JSVAL(JS_InternString(cx, str));
			return JS_TRUE;
		}
		default:
			break;
	}//end switch
	return JS_TRUE;
}//end GetProperty function

/**
 * This method is called when setting a Property in HTMLInput
 */
static JSBool jhtml_input_setProperty(JSContext *cx, JSObject *obj, jsval id, jsval *vp)
{
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	HTMLInput *element = (HTMLInput *) jhutil_GetPrivate(cx, obj);

	if (!element)
	{
		return JS_TRUE;
	}

	switch (JSVAL_TO_INT(id))
	{
		case HTMLINPUTELEMENT_VALUE:
		{
			element->SetValue(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
			return JS_TRUE;
		}
		case HTMLINPUTELEMENT_DEFAULTVALUE:
		{
			element->SetDefValue(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
			return JS_TRUE;
		}
		case HTMLINPUTELEMENT_CHECKED:
		case HTMLINPUTELEMENT_DEFAULTCHECKED:
		{
			int on = 0;
			if (JSVAL_IS_BOOLEAN(*vp))
			{
				if (JSVAL_TO_BOOLEAN(*vp) == JS_TRUE)
				{
					on = 1;
				}
				element->SetChecked(on);
				return JS_TRUE;
			}
		}
		break;

		case HTMLINPUTELEMENT_ACCEPT:
		{
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				element->SetAccept(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
				return JS_TRUE;
			}
		}
		break;

		case HTMLINPUTELEMENT_ACCESSKEY:
		{
			char *str = JS_GetStringBytes(JS_ValueToString(cx, *vp));
			if (str && *str)
			{
				element->SetAccessKey((WEBC_UINT16)str[0]);
			}
			return JS_TRUE;
		}

		case HTMLINPUTELEMENT_ALIGN:
		{
			WEBC_CHAR *str = WEBC_JS_STRING_TO_WEBC_STRING(JSVAL_TO_STRING(*vp));
			if(str)
				element->SetHtmlAlign(HTML_ParseAlignType(str, 0));
		}
		break;

		case HTMLINPUTELEMENT_ALT:
		{
			element->SetAlt(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
			return JS_TRUE;
		}

		case HTMLINPUTELEMENT_SRC:
		{
			element->SetSrc(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
			element->Update(0,0);
			return JS_TRUE;
		}

		case HTMLINPUTELEMENT_DISABLED:
		{
			if (JSVAL_IS_BOOLEAN(*vp))
			{
				element->SetDisabled((JSVAL_TO_BOOLEAN(*vp) == JS_TRUE)? 1 : 0);
				return JS_TRUE;
			}
		}
		break;

		case HTMLINPUTELEMENT_MAXLENGTH:
		{
			long newMaxLength = webc_atoi(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
			element->SetMaxLength(newMaxLength);
			return JS_TRUE;
		}

		case HTMLINPUTELEMENT_NAME:
		{
			if (JSVAL_IS_STRINGABLE(*vp))
			{
				element->SetName(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
				return JS_TRUE;
			}
		}
		break;

		case HTMLINPUTELEMENT_READONLY:
		{
			if (JSVAL_IS_BOOLEAN(*vp))
			{
				if (JSVAL_TO_BOOLEAN(*vp) == JS_FALSE)
				{
					element->ClearFlag(HINPUT_FLAG_READONLY);
				}
				else
				{
					element->SetFlag(HINPUT_FLAG_READONLY);
				}
				element->StyleUpdate();
				return JS_TRUE;
			}
		}
		break;

		case HTMLINPUTELEMENT_SIZE:
		{
			long newSize = webc_atoi(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));
			element->SetSize(newSize);
			return JS_TRUE;
		}

		case HTMLINPUTELEMENT_TABINDEX:
		{
			if (JSVAL_IS_NUMBER(*vp))
			{
				int32 ti;
				JS_ValueToInt32(cx, *vp, &ti);
				element->SetTabIndex((short)ti);
				return JS_TRUE;
			}
		}
		break;
		case HTMLINPUTELEMENT_TYPE:
		{
			HTMLElement *pNewElement = 0;
			HTMLInputType newInputType = jhtml_input_determine_type(WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, *vp)));

			pNewElement = jhtml_input_change_type((HTMLInput *) element, newInputType, jsmgr->GetDocument());
			// WARNING: pInput has potentially been deleted by the call to jhtml_input_change_type above
			if(pNewElement)
			{
				jhutil_SetPrivate(cx, obj, pNewElement);
			}
		}
		break;

		default:
			break;
	}
	//end switch

    return JS_TRUE;
}

// setAttribute method
JSBool jhtml_input_setAttribute(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	HTMLElement *pElement = (HTMLElement *)jhutil_GetPrivate(cx, obj);
	// parameters are Name, value, casesensitive[optional]
	if (pElement && (argc > 1))
	{
        int CaseSensitive = 1;
	    HTML_CHAR *pAttributeName  = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, argv[0]));
	    HTML_CHAR *pAttributeValue = WEBC_JS_STRING_TO_WEBC_STRING(JS_ValueToString(cx, argv[1]));
	    if (argc > 2)
        {
            CaseSensitive = JSVAL_TO_INT(argv[2]);
        }
        HTMLAttributeType attributeType =  HTML_ParseAttributeType (pAttributeName, webc_strlen(pAttributeName));

		if(pElement->GetTag() == HTML_TAG_INPUT)
		{
			HTMLInput *pInput = (HTMLInput *)pElement;
			jsval propertyID = HTML_UNKNOWN_ATTRIB;

			switch (attributeType)
			{
				case HTML_ATTRIB_TYPE:
					propertyID = HTMLINPUTELEMENT_TYPE;
					break;
				case HTML_ATTRIB_SIZE:
					propertyID = HTMLINPUTELEMENT_SIZE;
					break;
				case HTML_ATTRIB_MAXLENGTH:
					propertyID = HTMLINPUTELEMENT_MAXLENGTH;
					break;
				case HTML_ATTRIB_VALUE:
					propertyID = HTMLINPUTELEMENT_VALUE;
					break;
				case HTML_ATTRIB_ACCEPT:
					propertyID = HTMLINPUTELEMENT_ACCEPT;
					break;
				case HTML_ATTRIB_ACCESSKEY:
					propertyID = HTMLINPUTELEMENT_ACCESSKEY;
					break;
				case HTML_ATTRIB_ALIGN:
					propertyID = HTMLINPUTELEMENT_ALIGN;
					break;
				case HTML_ATTRIB_ALT:
					propertyID = HTMLINPUTELEMENT_ALT;
					break;
				case HTML_ATTRIB_DISABLED:
					propertyID = HTMLINPUTELEMENT_DISABLED;
					break;
				case HTML_ATTRIB_NAME:
					propertyID = HTMLINPUTELEMENT_NAME;
					break;
				case HTML_ATTRIB_READONLY:
					propertyID = HTMLINPUTELEMENT_READONLY;
					break;
				case HTML_ATTRIB_SRC:
					propertyID = HTMLINPUTELEMENT_SRC;
					break;
				case HTML_ATTRIB_TABINDEX:
					propertyID = HTMLINPUTELEMENT_TABINDEX;
					break;
				case HTMLINPUTELEMENT_DEFAULTVALUE:
				case HTMLINPUTELEMENT_DEFAULTCHECKED:
				case HTMLINPUTELEMENT_FORM:
				case HTMLINPUTELEMENT_CHECKED:
					// these aren't in the e_HTMLAttributeType enumeration - not sure if they should be added
				default:
						break;
			}

			if(HTML_UNKNOWN_ATTRIB != propertyID)
			{
				// this is admittedly ass-backwards since properties are a superset if attributes (so
				// the calling sequence should be the other-way-round), but since the property method
				// is already implemented, we'll just call back into it to service the setAttribute call.
				return jhtml_input_setProperty(cx, obj, INT_TO_JSVAL(propertyID), &(argv[1]));
			}
			else
			{
				return jhtml_element_setAttribute(cx, obj, argc, argv, rval); // defer to base class
			}
		}
	}
	return JS_FALSE; //failure
}

/**
 * This JSClass is the javascript wrapper for the HTMLInput class defined in the DOM Level 1
 */
JSClass JSHtmlInput =
{
	"Input", JSCLASS_HAS_PRIVATE, (JSPropertyOp) jutil_addProperty,
	(JSPropertyOp) jutil_delProperty, (JSPropertyOp) jhtml_input_getProperty,
	(JSPropertyOp) jhtml_input_setProperty, (JSEnumerateOp) jutil_enumerate,
	(JSResolveOp) jhtml_element_resolve, (JSConvertOp) jutil_convert,
	(JSFinalizeOp) jutil_finalize
};



/**
 * blur - this method removes the focus from this element
 *
 * params - none
 * return - void
 */
JSBool jhtml_input_blur(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);

	if (!pElem)
	{
		return JS_TRUE;
	}
	HTMLDocument *pDoc = (pElem)? pElem->GetDocument() : 0;
	if (pDoc && pDoc->Body())
	{
		pDoc->Body()->Focus();
	}
	*rval = JSVAL_VOID;
	return JS_TRUE;
}

/**
 * focus - this method adds the focus to this element
 *
 * params - none
 * return - void
 */
JSBool jhtml_input_focus(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);

	if (!pElem)
	{
		return JS_TRUE;
	}

	pElem->Focus();

	*rval = JSVAL_VOID;
	return JS_TRUE;
}

/**
 * click - This method is the same as having a click event on the input
 *
 * params - none
 * return - void
 */
JSBool jhtml_input_click(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLInput *input = (pElem)? (HTMLInput *) pElem : 0;
	if (input)
	{
		//send an event to this input
		HTMLEvent e;
		e.type = HTML_EVENT_CLICK;
		e.data.position.x = 0;
		e.data.position.y = 0;

		input->Event(&e);
	}
	*rval = JSVAL_VOID;
	return JS_TRUE;
}

/**
 * select - This method selects all the text in an editbox
 *
 * params - none
 * return - void
 */
JSBool jhtml_input_select(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	HTMLElement *pElem = (HTMLElement *) jhutil_GetPrivate(cx, obj);
	HTMLInput *input = (pElem)? (HTMLInput *) pElem : 0;

	if (input)
	{
		input->Select();
		input->Focus();
	}
	*rval = JSVAL_VOID;
	return JS_TRUE;
}

static JSBool JHTMLInput(JSContext *cx, JSObject *obj, uintN argc, jsval *argv, jsval *rval)
{
	WebcJSScript *jsscr = (WebcJSScript *) jhutil_GetContextPrivate(cx);
	WebcJSDocumentContext *jsmgr = (jsscr)? jsscr->GetJSMgr() : 0;
	HTMLInput *pInput;

	if (jsmgr)
	{
		// tbd - create the appropriate object type
		HTMLTagParse tag[1];
		//tag->tagnum = HTML_TAG_INPUT;
		//tag->off = 1;
		//tag->num_params = 0;
		HTML_InitTagParse(tag, HTML_TAG_INPUT);
		WEBC_NEW_VERBOSE(pInput, HTMLInput(tag, jsmgr->GetDocument(), obj),"JHTMLInput");

        if (pInput)
        {
		    pInput->SetTagProperties(tag, jsmgr->GetDocument());
		    jsmgr->AddNewElement(pInput);
		    return JS_TRUE;
        }
	}
	return JS_FALSE;
}

JSClass *getHtmlInputElement()
{
	return &JSHtmlInput;
}

JSObject *jhtml_input_Init(JSContext *cx, JSObject *proto)
{
	//initialize the HTMLInput Class
	return DEBUGJS_InitClass(cx, JS_GetGlobalObject(cx), proto, &JSHtmlInput,
    				(JSNative) JHTMLInput, 0,
    				jhtml_input_props, jhtml_input_methods,
    				WEBC_NULL, WEBC_NULL);
}

HTMLInputType jhtml_input_determine_type(HTML_CHAR *pAttributeValue)
{
	HTMLInputType retVal = HTML_INPUT_TYPE_UNKNOWN;

	if(pAttributeValue)
		retVal = HTML_ParseInputType(pAttributeValue, webc_strlen(pAttributeValue));

	return retVal;
}


// worker method to change the type of the input item
HTMLInput *jhtml_input_change_type(HTMLInput *pInput, HTMLInputType newInputType, HTMLDocument *pDocument)
{
	HTMLInputType currentInputType = pInput->GetInputType();
	HTMLElement *pParent = pInput->Parent();
	HTMLInput *pNewElement=WEBC_NULL;

	if(currentInputType == newInputType)
		return WEBC_NULL; // do nothing

	HTMLTagParse tag[1];
	HTML_InitTagParse(tag, HTML_TAG_INPUT);

	switch (newInputType)
	{
	case HTML_INPUT_TYPE_BUTTON:
		WEBC_NEW_VERBOSE(pNewElement, HTMLInputButton(tag, pDocument),"HTMLInputButton");
		break;
	case HTML_INPUT_TYPE_CHECKBOX:
		WEBC_NEW_VERBOSE(pNewElement, HTMLCheckbox(tag, pDocument),"HTMLCheckbox");
		break;
	case HTML_INPUT_TYPE_FILE:
		WEBC_NEW_VERBOSE(pNewElement, HTMLEditString(tag, pDocument),"HTMLEditString");	// TODO implement file upload
		break;
	case HTML_INPUT_TYPE_HIDDEN:
		WEBC_NEW_VERBOSE(pNewElement, HTMLInputHidden(tag, pDocument),"HTMLInputHidden");
		break;
	case HTML_INPUT_TYPE_IMAGE:
		WEBC_NEW_VERBOSE(pNewElement,  HTMLInputButton(tag, pDocument),"HTMLInputButton");	//?
		break;
	case HTML_INPUT_TYPE_PASSWORD:
		WEBC_NEW_VERBOSE(pNewElement, HTMLInputPassword(tag, pDocument),"HTMLInputPassword");
		break;
	case HTML_INPUT_TYPE_RADIO:
		WEBC_NEW_VERBOSE(pNewElement, HTMLRadioButton(tag, pDocument),"HTMLRadioButton");
		break;
	case HTML_INPUT_TYPE_RESET:
		WEBC_NEW_VERBOSE(pNewElement, HTMLInputReset(tag, pDocument),"HTMLInputReset");
		break;
	case HTML_INPUT_TYPE_SUBMIT:
		WEBC_NEW_VERBOSE(pNewElement, HTMLInputSubmit(tag, pDocument),"HTMLInputSubmit");
		break;
	case HTML_INPUT_TYPE_TEXT:
		WEBC_NEW_VERBOSE(pNewElement, HTMLEditString(tag, pDocument),"HTMLEditString");
		break;
	}

	// we may or may not be included in the DOM tree
	// if we are, then replace with the new element
	if(pParent)
	{
		pParent->InsertAfter(pNewElement, pInput);
		pParent->Remove(pInput);
	}
	WEBC_DELETE(pInput);

	return pNewElement;
}
#endif //(WEBC_SUPPORT_JSCRIPT)
