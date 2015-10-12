//
// HTXTAREA.CPP - Methods for HTML DOM class HTMLTextArea
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
#include "htxtarea.hpp"
#include "wcconst.hpp"
#include "webcutf.h"
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
// HTMLTextArea Constructors
/*****************************************************************************/

HTMLTextArea::HTMLTextArea(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLInput(td, pDocument)
{
	mpDefValue = 0;
	miCols = WEBC_DEFAULT_TEXTAREA_COLS;
	miRows = WEBC_DEFAULT_TEXTAREA_ROWS;
	mpEditBoxDisplay = 0;
	mWrap = TEXTAREA_WRAP_VIRTUAL;
	mInputType = HTML_INPUT_TYPE_TEXT;

	//resize the editbox
	SetColsRows(miCols, miRows);

}


/*****************************************************************************/
// HTMLTextArea Destructor
/*****************************************************************************/

HTMLTextArea::~HTMLTextArea(void)
{
	webc_free_string_copy(mpDefValue, __FILE__, __LINE__);

	if (mpEditBoxDisplay)
	{
		WEBC_DELETE(mpEditBoxDisplay);
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
void HTMLTextArea::Refresh(PegThing *pThing)
{
}
*/

HTMLEventStatus HTMLTextArea::DefaultEvent(HTMLEvent *pEvent)
{
    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	switch (pEvent->type)
	{
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
			}
			break;

	default:
		break;
	}
	return (HTMLInput::DefaultEvent(pEvent));
}

/*
HTMLDocument *HTMLTextArea::GetDocument(void)
{
	return (0);
}
*/
/*
HTMLElement *HTMLTextArea::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/


void HTMLTextArea::Reset(void)
{
	SetValue(DefValue());
}


/******************************************************************************
* void HTMLTextArea::Select()
*
* Parameters
*
* Description
*
*
* Returns (RETURNTYPE)
*
******************************************************************************/
void HTMLTextArea::Select(void)
{
	if (mpEditBoxDisplay)
	{
		mpEditBoxDisplay->SelectAll();
	}
}


int HTMLTextArea::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		case HTML_ATTRIB_COLS:
			SetCols(webc_atoi(value));
			return (1);

		case HTML_ATTRIB_ROWS:
			SetRows(webc_atoi(value));
			return (1);

		case HTML_ATTRIB_WRAP:
			if (value && (webc_c_stricmp(value, "physical") == 0 ||
			              webc_c_stricmp(value, "hard") == 0) )
			{
				SetWrap(TEXTAREA_WRAP_PHYSICAL);
			}
			else if (value && webc_c_stricmp(value, "off") == 0)
			{
				SetWrap(TEXTAREA_WRAP_OFF);
			}
			else //set it to the default
			{
				SetWrap(TEXTAREA_WRAP_VIRTUAL);
			}
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

CSSValueType HTMLTextArea::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{
	switch (Property)
	{
	case CSS_PROPERTY_PADDING_LEFT:
	case CSS_PROPERTY_PADDING_RIGHT:
	case CSS_PROPERTY_PADDING_BOTTOM:
	case CSS_PROPERTY_PADDING_TOP:
		pValue->length.type = CSS_LENGTH_UNIT_PX;
		pValue->length.val.dec = 0;
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
		pValue->color = HTML_RGBAToColor(0x99, 0x99, 0x99, 0);
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
		pValue->color = HTML_RGBAToColor(0,0,0,0);
		return (CSS_VALUE_SPECIFIED);
	}

	case CSS_PROPERTY_BACKGROUND_COLOR:
	{
		pValue->color = HTML_RGBAToColor(0xff,0xff,0xff,0);
		return (CSS_VALUE_SPECIFIED);
	}

	default:
		break;
	}

	return (CSS_VALUE_UNKNOWN);
}


WEBC_CHAR *HTMLTextArea::Value(void)
{
HTMLEditBoxDisplay *pDisplay = GetEditBoxDisplay();

	if (pDisplay)
	{
		// tbd - convert from WebChar to WEBC_CHAR
		return (pDisplay->GetText());
	}

	return (0);
}

WEBC_CHAR * HTMLTextArea::DefValue(void)
{
	return (mpDefValue);
}

void HTMLTextArea::SetValue(const WEBC_CHAR *pValue)
{
HTMLEditBoxDisplay *pDisplay = GetEditBoxDisplay();

	if (pDisplay)
	{
		// tbd - convert from WEBC_CHAR to WebChar
		pDisplay->SetText(pValue);
	}
}

void HTMLTextArea::SetDefValue(const WEBC_CHAR *pValue)
{
WEBC_CHAR * pStr = webc_malloc_string_copy(pValue, __FILE__, __LINE__);
	webc_free_string_copy(mpDefValue, __FILE__, __LINE__);
	mpDefValue = pStr;
}

void HTMLTextArea::SetColsRows(int c, int r)
{
	if (c > 0 && r > 0)
	{
		miCols = c;
		miRows = r;
		NotifyDisplay(NOTIFY_STYLE_CHANGE);
	}
}

void HTMLTextArea::SetCols(int c)
{
	SetColsRows(c, miRows);
}

void HTMLTextArea::SetRows(int r)
{
	SetColsRows(miCols, r);
}


/*****************************************************************************/
// HTMLTextArea-Specific Methods
/*****************************************************************************/

WEBC_UINT8 HTMLTextArea::Wrap()
{
	return (mWrap);
}

void HTMLTextArea::SetWrap(WEBC_UINT8 w)
{
	mWrap = w;
}

DisplayElement *HTMLTextArea::GetDisplayElement (WEBC_BOOL create)
{
	return (GetEditBoxDisplay(create));
}

TextUnit *HTMLTextArea::GetTextUnit (void)
{
	return (GetEditBoxDisplay());
}

HTMLEditBoxDisplay *HTMLTextArea::GetEditBoxDisplay (WEBC_BOOL create)
{
	if (!mpEditBoxDisplay && create)
	{
		WEBC_NEW_VERBOSE(mpEditBoxDisplay, HTMLEditBoxDisplay(this),"HTMLEditBoxDisplay");
		if (mpEditBoxDisplay)
		{
			WebChar empty[1] = {0};
			mpEditBoxDisplay->SetText(empty);
			mpEditBoxDisplay->DisableSizeLimit();
		}
	}

	return (mpEditBoxDisplay);
}
