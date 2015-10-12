//
// HEDIT.CPP - Methods for HTML DOM class HTMLEditString
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//  [tbd]
//

/*****************************************************************************/
// Header files
/*****************************************************************************/

#include "htmldoc.hpp"
#include "hedit.hpp"
#include "hform.hpp"
#include "util.h"
#include "webload.hpp"
#include "wcconst.hpp"
#include "wgkeys.hpp"
#include "webcstr.h"
#include "htmlfind.hpp"
#include "webcmem.h"


/*****************************************************************************/
// Local Constants
/*****************************************************************************/

/*****************************************************************************/
// Local type definitions
/*****************************************************************************/

/*****************************************************************************/
// Local Functions
/*****************************************************************************/

/*****************************************************************************/
// Global Data
/*****************************************************************************/

/*****************************************************************************/
// HTMLEditString Constructors
/*****************************************************************************/

HTMLEditString::HTMLEditString(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLInput(td, pDocument)
{
	mSize = WEBC_DEFAULT_INPUT_SIZE;
	mDefaultValue = 0;
	mEditStringDisplay = 0;
	mInputType = HTML_INPUT_TYPE_TEXT;
}


/*****************************************************************************/
// HTMLEditString Destructor
/*****************************************************************************/

HTMLEditString::~HTMLEditString(void)
{
	webc_free_string_copy(mDefaultValue, __FILE__, __LINE__);

	if (mEditStringDisplay)
	{
		WEBC_DELETE(mEditStringDisplay);
	}
}


/*****************************************************************************/
// TextUnit Interface
/*****************************************************************************/


/*****************************************************************************/
// TextUnit Helper Methods
/*****************************************************************************/


/*****************************************************************************/
// Override-able HTMLElement Methods
/*****************************************************************************/

/*
void HTMLEditString::Refresh(PegThing *pThing)
{
}
*/

HTMLEventStatus HTMLEditString::DefaultEvent(HTMLEvent *pEvent) // tbd
{

    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	switch (pEvent->type)
	{
		case HTML_EVENT_KEYPRESS:
			if (pEvent->data.key == WGK_CR)
			{
				HTMLElement *pElem = 0;
				vector_iterator it;

				HTMLForm *pForm = Form();
				if (pForm)
				{
					WEBC_BOOL onlyControl = WEBC_TRUE;
					HTMLInput* submitButton = WEBC_NULL;

					pElem = (HTMLElement*) ObjectListGetFirst(pForm->GetFieldVector(), &it);

					while (pElem)
					{
						if ((pElem->Type() == HTML_EDIT_STR_ELEMENT ||
							 pElem->Type() == HTML_EDITBOX_ELEMENT ||
							 pElem->Type() == HTML_CHECKBOX_ELEMENT ||
							 pElem->Type() == HTML_RADIO_BUTTON_ELEMENT ||
							 pElem->Type() == HTML_INPUT_PASSWORD_ELEMENT) && pElem != this)
						{
							onlyControl = WEBC_FALSE;
							break;
						}

						if (pElem->Type() == HTML_BUTTON_ELEMENT && !submitButton)
						{
							submitButton = (HTMLInput*) pElem;
						}

						pElem = (HTMLElement*) ObjectListGetNext(&it);
					}

					if(1) //  (submitButton && onlyControl)
					{
						HTMLEvent e;
						e.type = HTML_EVENT_SUBMIT;
						e.data.position.x = pEvent->data.position.x;
						e.data.position.y = pEvent->data.position.y;
						e.elem = (HELEMENT_HANDLE) submitButton;

						return (pForm->Event(&e));
					}
				}
			}
			break;

		case HTML_EVENT_FOCUS:
		{
			HTMLSetFlagFinder f(HELEM_FLAG_HAS_FOCUS, 1);
			FindElement(&f);
			SetFlag(HINPUT_FLAG_ACTIVE);
			Update(0,0);
			break;
		}

		case HTML_EVENT_UNFOCUS:
		{
			HTMLSetFlagFinder f(HELEM_FLAG_HAS_FOCUS, 0);
			FindElement(&f);
			ClearFlag(HINPUT_FLAG_ACTIVE);
			Update(0,0);
			break;
		}

		case HTML_EVENT_KEYDOWN:
		{
			HTMLEventStatus retval = DefaultInputKeyHandler(pEvent);
			if (retval != HTML_EVENT_STATUS_CONTINUE)
			{
				return (retval);
			}

			switch (pEvent->data.key)
			{
				case WGK_LEFT:
				case WGK_RIGHT:
					pEvent->flags |= HTML_EVENT_FLAG_CANCEL_BUBBLE;
					break;
			}
			break;
		}

	default:
		break;
	}
	return (HTMLInput::DefaultEvent(pEvent));
}

/*
HTMLDocument *HTMLEditString::GetDocument(void)
{
	return (0);
}
*/
/*
HTMLElement *HTMLEditString::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/


void HTMLEditString::Reset(void)
{
	SetValue(DefValue());
}

/******************************************************************************
* void HTMLEditString::Select()
*
* Parameters
*
* Description
*
*
* Returns (RETURNTYPE)
*
******************************************************************************/
void HTMLEditString::Select(void)
{
	if (mEditStringDisplay)
	{
		mEditStringDisplay->SelectAll();
	}
}


int HTMLEditString::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		case HTML_ATTRIB_SIZE:
			SetSize(webc_atoi(value));
			return (1);

		case HTML_ATTRIB_MAXLENGTH:
			SetMaxLength(webc_atoi(value));
			return (1);

		case HTML_ATTRIB_ALT:
			SetAlt(value);
			return (1);

		default:
			break;
	}

	if (call_base)
	{
		return (HTMLInput::SetProperty(Property, value, pDocument, call_base));
	}

	return (0);
}

CSSValueType HTMLEditString::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{
	switch (Property)
	{
		case CSS_PROPERTY_PADDING_LEFT:
			pValue->length.type = CSS_LENGTH_UNIT_PX;
			pValue->length.val.dec = WEBC_CFG_TEXT_INPUT_PADDING_LEFT;
			pValue->length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_PADDING_RIGHT:
			pValue->length.type = CSS_LENGTH_UNIT_PX;
			pValue->length.val.dec = WEBC_CFG_TEXT_INPUT_PADDING_RIGHT;
			pValue->length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_PADDING_BOTTOM:
			pValue->length.type = CSS_LENGTH_UNIT_PX;
			pValue->length.val.dec = WEBC_CFG_TEXT_INPUT_PADDING_BOTTOM;
			pValue->length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_PADDING_TOP:
			pValue->length.type = CSS_LENGTH_UNIT_PX;
			pValue->length.val.dec = WEBC_CFG_TEXT_INPUT_PADDING_TOP;
			pValue->length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_BORDER_LEFT_STYLE:
		case CSS_PROPERTY_BORDER_RIGHT_STYLE:
		case CSS_PROPERTY_BORDER_TOP_STYLE:
		case CSS_PROPERTY_BORDER_BOTTOM_STYLE:
			pValue->borderStyle = CSS_BORDER_STYLE_INSET;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_BORDER_LEFT_WIDTH:
		case CSS_PROPERTY_BORDER_RIGHT_WIDTH:
		case CSS_PROPERTY_BORDER_TOP_WIDTH:
		case CSS_PROPERTY_BORDER_BOTTOM_WIDTH:
			pValue->borderThickness.type = CSS_BORDER_THICKNESS_LENGTH;
			pValue->borderThickness.val.length.type = CSS_LENGTH_UNIT_PX;
			pValue->borderThickness.val.length.val.dec = 2;
			pValue->borderThickness.val.length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_BORDER_LEFT_COLOR:
		case CSS_PROPERTY_BORDER_RIGHT_COLOR:
		case CSS_PROPERTY_BORDER_TOP_COLOR:
		case CSS_PROPERTY_BORDER_BOTTOM_COLOR:
		{
			pValue->color = HTML_RGBAToColor(0x99, 0x99, 0x99, 0x00); // gray
			return (CSS_VALUE_SPECIFIED);
		}

		case CSS_PROPERTY_FONT_FAMILY:
			pValue->name = webc_GetDefaultString(WEBC_DEFAULT_TEXT_INPUT_FONT_FAMILY);
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_FONT_STYLE:
			pValue->fontStyle = webc_GetDefaultFontStyle(WEBC_DEFAULT_TEXT_INPUT_FONT_STYLE);
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_FONT_VARIANT:
			break;

		case CSS_PROPERTY_FONT_WEIGHT:
			pValue->fontWeight =  webc_GetDefaultFontWeight(WEBC_DEFAULT_TEXT_INPUT_FONT_WEIGHT);
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_FONT_SIZE:
			pValue->fontSize.type = CSS_FONT_SIZE_ABSOLUTE;
			pValue->fontSize.val.absolute =  webc_GetDefaultUint16(WEBC_DEFAULT_TEXT_INPUT_FONT_SIZE); 
			return (CSS_VALUE_SPECIFIED);


		case CSS_PROPERTY_COLOR:
		{
			pValue->color = HTML_RGBAToColor(0, 0, 0, 0); // block
			return (CSS_VALUE_SPECIFIED);
		}

		case CSS_PROPERTY_BACKGROUND_COLOR:
		{
			pValue->color = HTML_RGBAToColor(0xff, 0xff, 0xff, 0); // white
			return (CSS_VALUE_SPECIFIED);
		}

		default:
			break;
	}

	return HTMLInput::GetStyleFromHTML(Property, pValue); // check with base class
}

WEBC_SIZE HTMLEditString::Size(void)
{
 	return (mSize);
}


void HTMLEditString::SetSize(WEBC_SIZE i)
{
	mSize = i;
	SetFlag(HEDIT_FLAG_SIZE);
	NotifyDisplay(NOTIFY_STYLE_CHANGE);
}

WEBC_SIZE HTMLEditString::MaxLength(void)
{
HTMLEditStringDisplay *pDisplay = GetEditStringDisplay();

	if (pDisplay && (mFlags & HEDIT_FLAG_MAXLENGTH))
	{
		return (pDisplay->GetMaxLength());
	}

	return (-1);
}

void HTMLEditString::SetMaxLength(WEBC_SIZE i)
{
HTMLEditStringDisplay *pDisplay = GetEditStringDisplay();

	if (pDisplay)
	{
		pDisplay->EnableSizeLimit();
		pDisplay->SetMaxLength(i);
		SetFlag(HEDIT_FLAG_MAXLENGTH);
	}
}

void HTMLEditString::SetDisabled(WEBC_BOOL d)
{
HTMLEditStringDisplay *pDisplay = GetEditStringDisplay();

	if (pDisplay)
	{
		if (d)
		{
			pDisplay->SetFlag(DISPLAY_FLAG_DISABLED);
		}
		else
		{
			pDisplay->ClearFlag(DISPLAY_FLAG_DISABLED);
		}
		pDisplay->Invalidate();
	}

	HTMLInput::SetDisabled(d);
}

WEBC_CHAR *HTMLEditString::Value(void)
{
HTMLEditStringDisplay *pDisplay = GetEditStringDisplay();

	if (pDisplay)
	{
		// tbd - convert from WebChar to WEBC_CHAR
		return (pDisplay->GetText());
	}

	return (0);
}


void HTMLEditString::SetValue(const WEBC_CHAR *pValue)
{
HTMLEditStringDisplay *pDisplay = GetEditStringDisplay();

	if (pDisplay)
	{
		// tbd - convert from WEBC_CHAR to WebChar
		pDisplay->SetText(pValue);
	}
}


WEBC_CHAR *HTMLEditString::DefValue(void)
{
	return (mDefaultValue);
}


void HTMLEditString::SetDefValue(const WEBC_CHAR *pValue)
{
WEBC_CHAR *pStr = webc_malloc_string_copy(pValue, __FILE__, __LINE__);
	webc_free_string_copy(mDefaultValue, __FILE__, __LINE__);
	mDefaultValue = pStr;
}


/*****************************************************************************/
// HTMLEditString-Specific Methods
/*****************************************************************************/

DisplayElement *HTMLEditString::GetDisplayElement (WEBC_BOOL create)
{
	return (GetEditStringDisplay(create));
}

TextUnit *HTMLEditString::GetTextUnit (void)
{
	return (GetEditStringDisplay());
}

HTMLEditStringDisplay *HTMLEditString::GetEditStringDisplay (WEBC_BOOL create)
{
	if (!mEditStringDisplay && create)
	{
		WEBC_NEW_VERBOSE(mEditStringDisplay, HTMLEditStringDisplay(this),"HTMLEditStringDisplay");
		if (mEditStringDisplay)
		{
			WebChar empty[1] = {0};
			mEditStringDisplay->SetText(empty);
			mEditStringDisplay->DisableSizeLimit();
			mEditStringDisplay->SetListener(this);
		}
	}

	return (mEditStringDisplay);
}

void HTMLEditString::Notify (Notifier* who, NotifyEvent* what)
{
    WEBC_DEBUG_ReceiveNotify(this, who, what);
	if ((DisplayElement*) who == (DisplayElement*) mEditStringDisplay && what->type == NOTIFIER_TYPE_EDIT)
	{
		switch (what->event)
		{
			case NOTIFY_TEXT_EDIT:
			{
				HTMLEvent e;
				e.type = HTML_EVENT_CHANGE;
				Event(&e);
				break;
			}
			default:
				break;
		}
	}
}
