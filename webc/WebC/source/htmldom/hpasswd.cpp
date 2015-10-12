//
// HPASSWD.CPP - Methods for HTML DOM class HTMLInputPassword
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
#include "hpasswd.hpp"
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
// HTMLInputPassword Constructors
/*****************************************************************************/

HTMLInputPassword::HTMLInputPassword(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLInput(td, pDocument)
{
	miSize = WEBC_DEFAULT_INPUT_SIZE;
	mpDefValue = 0;
	mpPasswordDisplay = 0;
	mInputType = HTML_INPUT_TYPE_PASSWORD;
}


/*****************************************************************************/
// HTMLInputPassword Destructor
/*****************************************************************************/

HTMLInputPassword::~HTMLInputPassword(void)
{
	webc_free_string_copy(mpDefValue, __FILE__, __LINE__);

	if (mpPasswordDisplay)
	{
		WEBC_DELETE(mpPasswordDisplay);
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
void HTMLInputPassword::Refresh(PegThing *pThing)
{
}
*/

HTMLEventStatus HTMLInputPassword::DefaultEvent(HTMLEvent *pEvent) // tbd
{
	WEBC_UINT8 onlyBox = 1;
	WEBC_UINT8 hasSubmit = 0;
	HTMLElement *pElem = 0;
	HTMLInput *pSubmit = 0;
	vector_iterator it;
    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	switch (pEvent->type)
	{
		case HTML_EVENT_CHANGE:
			break;

		case HTML_EVENT_EDIT:
		{
			HTMLForm *pForm = Form();
			if (pForm)
			{
				pElem = (HTMLElement*)ObjectListGetFirst(pForm->GetFieldVector(), &it);
				while (pElem)
				{
					if ( (pElem->Type() == HTML_EDIT_STR_ELEMENT ||
						pElem->Type() == HTML_EDITBOX_ELEMENT)
						&& pElem != this)
					{
						onlyBox = 0;
						break;
					}
					if (pElem->Type() == HTML_BUTTON_ELEMENT
						&& !hasSubmit)
					{
						hasSubmit = 1;
						pSubmit = (HTMLInput *)pElem;
					}
					pElem = (HTMLElement*)ObjectListGetNext(&it);
				}
				if (pSubmit && onlyBox)
				{
					HTMLEvent e;
					e.type = HTML_EVENT_SUBMIT;
					e.data.position.x = pEvent->data.position.x;
					e.data.position.y = pEvent->data.position.y;
					e.elem = (HELEMENT_HANDLE)pSubmit;

					return (pForm->Event(&e));
				}
			}
			break;
		}

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
				case WGK_LNDN:
				case WGK_LNUP:
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
HTMLDocument *HTMLInputPassword::GetDocument(void)
{
	return (0);
}
*/
/*
HTMLElement *HTMLInputPassword::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/


void HTMLInputPassword::Reset(void)
{
	SetValue(DefValue());
}

/******************************************************************************
* void HTMLInputPassword::Select()
*
* Parameters
*
* Description
*
*
* Returns (RETURNTYPE)
*
******************************************************************************/
void HTMLInputPassword::Select(void)
{
	if (mpPasswordDisplay)
	{
		mpPasswordDisplay->SelectAll();
	}
}


int HTMLInputPassword::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
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

		default:
			break;
	}

	if (call_base)
	{
		return (HTMLInput::SetProperty(Property, value, pDocument, call_base));
	}

	return (0);
}

CSSValueType HTMLInputPassword::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
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
			pValue->color = HTML_RGBAToColor(0x99,0x99,0x99,0);  // gray
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
			pValue->color = HTML_RGBAToColor(0,0,0,0);  // black
			return (CSS_VALUE_SPECIFIED);
		}

		case CSS_PROPERTY_BACKGROUND_COLOR:
		{
			pValue->color = HTML_RGBAToColor(0xff,0xff,0xff,0);  // white
			return (CSS_VALUE_SPECIFIED);
		}

		default:
			break;
	}

	return (CSS_VALUE_UNKNOWN);
}

WEBC_SIZE HTMLInputPassword::Size(void)
{
 	return (miSize);
}


void HTMLInputPassword::SetSize(WEBC_SIZE i)
{
	miSize = i;
	NotifyDisplay(NOTIFY_STYLE_CHANGE);
}

WEBC_SIZE HTMLInputPassword::MaxLength(void)
{
HTMLPasswordDisplay *pDisplay = GetPasswordDisplay();

	if (pDisplay)
	{
		return (pDisplay->GetMaxLength());
	}

	return (0);
}

void HTMLInputPassword::SetMaxLength(WEBC_SIZE i)
{
HTMLPasswordDisplay *pDisplay = GetPasswordDisplay();

	if (pDisplay)
	{
		pDisplay->EnableSizeLimit();
		pDisplay->SetMaxLength(i);
	}
}

WEBC_CHAR *HTMLInputPassword::Value(void)
{
HTMLPasswordDisplay *pDisplay = GetPasswordDisplay();

	if (pDisplay)
	{
		// tbd - convert from WebChar to WEBC_CHAR
		return (pDisplay->GetText());
	}

	return (0);
}


void HTMLInputPassword::SetValue(const WEBC_CHAR *pValue)
{
HTMLPasswordDisplay *pDisplay = GetPasswordDisplay();

	if (pDisplay)
	{
		// tbd - convert from WEBC_CHAR to WebChar
		pDisplay->SetText(pValue);
	}
}


WEBC_CHAR *HTMLInputPassword::DefValue(void)
{
	return (mpDefValue);
}


void HTMLInputPassword::SetDefValue(const WEBC_CHAR *pValue)
{
WEBC_CHAR *pStr = webc_malloc_string_copy(pValue, __FILE__, __LINE__);
	webc_free_string_copy(mpDefValue, __FILE__, __LINE__);
	mpDefValue = pStr;
}


/*****************************************************************************/
// HTMLInputPassword-Specific Methods
/*****************************************************************************/

DisplayElement *HTMLInputPassword::GetDisplayElement (WEBC_BOOL create)
{
	return (GetPasswordDisplay(create));
}

TextUnit *HTMLInputPassword::GetTextUnit (void)
{
	return (GetPasswordDisplay());
}

HTMLPasswordDisplay *HTMLInputPassword::GetPasswordDisplay (WEBC_BOOL create)
{
	if (!mpPasswordDisplay && create)
	{
		WEBC_NEW(mpPasswordDisplay, HTMLPasswordDisplay(this));
		if (mpPasswordDisplay)
		{
			WebChar empty[1] = {0};
			mpPasswordDisplay->SetText(empty);
			mpPasswordDisplay->DisableSizeLimit();
		}
	}

	return (mpPasswordDisplay);
}

