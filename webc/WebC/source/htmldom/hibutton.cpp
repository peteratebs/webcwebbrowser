//
// hibutton.CPP - Methods for HTML DOM class HTMLInputButton
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
#include "hibutton.hpp"
#include "wcconst.hpp"
#include "htmlfind.hpp"
#include "webcmem.h"

/*****************************************************************************/
// Local Constants
/*****************************************************************************/

#define WEBC_CFG_BUTTON_PADDING_LEFT          4
#define WEBC_CFG_BUTTON_PADDING_TOP           2
#define WEBC_CFG_BUTTON_PADDING_RIGHT         4
#define WEBC_CFG_BUTTON_PADDING_BOTTOM        1

#define WEBC_CFG_BUTTON_ACTIVE_PADDING_LEFT   5
#define WEBC_CFG_BUTTON_ACTIVE_PADDING_TOP    3
#define WEBC_CFG_BUTTON_ACTIVE_PADDING_RIGHT  3
#define WEBC_CFG_BUTTON_ACTIVE_PADDING_BOTTOM 0

#define WEBC_CFG_BUTTON_BORDER_WIDTH_LEFT     2
#define WEBC_CFG_BUTTON_BORDER_WIDTH_TOP      2
#define WEBC_CFG_BUTTON_BORDER_WIDTH_RIGHT    2
#define WEBC_CFG_BUTTON_BORDER_WIDTH_BOTTOM   2

#define WEBC_CFG_BUTTON_MARGIN_LEFT           0
#define WEBC_CFG_BUTTON_MARGIN_TOP            0
#define WEBC_CFG_BUTTON_MARGIN_RIGHT          0
#define WEBC_CFG_BUTTON_MARGIN_BOTTOM         0

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
// HTMLInputButton Constructors
/*****************************************************************************/

HTMLInputButton::HTMLInputButton(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLInput(td, pDocument)
{
	mpDisplayButton = 0;
	mValue = 0;
	mInputType = HTML_INPUT_TYPE_BUTTON;
}


/*****************************************************************************/
// HTMLInputButton Destructor
/*****************************************************************************/

HTMLInputButton::~HTMLInputButton(void)
{
	if (mValue)
	{
		webc_free_string_copy(mValue, __FILE__, __LINE__);
	}

	if (mpDisplayButton)
	{
		WEBC_DELETE(mpDisplayButton);
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

HTMLEventStatus HTMLInputButton::DefaultEvent(HTMLEvent *pEvent)
{
HTMLDocument *pDoc = GetDocument();

    WEBC_DEBUG_ReceiveEvent(this, pEvent);

	switch (pEvent->type)
	{
		case HTML_EVENT_FOCUS:
		{
			HTMLSetFlagFinder f(HELEM_FLAG_HAS_FOCUS, 1);
			FindElement(&f);
			StyleUpdate();
			break;
		}

		case HTML_EVENT_UNFOCUS:
		{
			HTMLSetFlagFinder f(HELEM_FLAG_HAS_FOCUS, 0);
			FindElement(&f);
			StyleUpdate();
			break;
		}

		case HTML_EVENT_KEYDOWN:
		{
			HTMLEventStatus retval = DefaultInputKeyHandler(pEvent);
			if (retval != HTML_EVENT_STATUS_CONTINUE)
			{
				return (retval);
			}
			break;
		}

		default:
			// ignore other event types
			break;
	}

	return (HTMLInput::DefaultEvent(pEvent));
}

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

CSSValueType HTMLInputButton::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{
	switch (Property)
	{
		case CSS_PROPERTY_PADDING_LEFT:
			pValue->length.type = CSS_LENGTH_UNIT_PX;
			pValue->length.val.dec = Active()? WEBC_CFG_BUTTON_ACTIVE_PADDING_LEFT : WEBC_CFG_BUTTON_PADDING_LEFT;
			pValue->length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_PADDING_RIGHT:
			pValue->length.type = CSS_LENGTH_UNIT_PX;
			pValue->length.val.dec = Active()? WEBC_CFG_BUTTON_ACTIVE_PADDING_RIGHT : WEBC_CFG_BUTTON_PADDING_RIGHT;
			pValue->length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_PADDING_TOP:
			pValue->length.type = CSS_LENGTH_UNIT_PX;
			pValue->length.val.dec = Active()? WEBC_CFG_BUTTON_ACTIVE_PADDING_TOP : WEBC_CFG_BUTTON_PADDING_TOP;
			pValue->length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_PADDING_BOTTOM:
			pValue->length.type = CSS_LENGTH_UNIT_PX;
			pValue->length.val.dec = Active()? WEBC_CFG_BUTTON_ACTIVE_PADDING_BOTTOM : WEBC_CFG_BUTTON_PADDING_BOTTOM;
			pValue->length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_MARGIN_LEFT:
			pValue->length.type = CSS_LENGTH_UNIT_PX;
			pValue->length.val.dec = WEBC_CFG_BUTTON_MARGIN_LEFT;
			pValue->length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_MARGIN_RIGHT:
			pValue->length.type = CSS_LENGTH_UNIT_PX;
			pValue->length.val.dec = WEBC_CFG_BUTTON_MARGIN_RIGHT;
			pValue->length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_MARGIN_TOP:
			pValue->length.type = CSS_LENGTH_UNIT_PX;
			pValue->length.val.dec = WEBC_CFG_BUTTON_MARGIN_TOP;
			pValue->length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_MARGIN_BOTTOM:
			pValue->length.type = CSS_LENGTH_UNIT_PX;
			pValue->length.val.dec = WEBC_CFG_BUTTON_MARGIN_BOTTOM;
			pValue->length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_FONT_FAMILY:
			pValue->name = webc_GetDefaultString(WEBC_DEFAULT_BUTTON_FONT_FAMILY);
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_FONT_STYLE:
			pValue->fontStyle = webc_GetDefaultFontStyle(WEBC_DEFAULT_BUTTON_FONT_STYLE);
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_FONT_VARIANT:
			break;

		case CSS_PROPERTY_FONT_WEIGHT:
			pValue->fontWeight =  webc_GetDefaultFontWeight(WEBC_DEFAULT_BUTTON_FONT_WEIGHT);
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_FONT_SIZE:
			pValue->fontSize.type = CSS_FONT_SIZE_ABSOLUTE;
			pValue->fontSize.val.absolute =  webc_GetDefaultUint16(WEBC_DEFAULT_BUTTON_FONT_SIZE); 
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_COLOR:
			pValue->color = HTML_RGBAToColor(0,0,0,0); // black
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_BACKGROUND_COLOR:
			pValue->color = HTML_RGBAToColor(0xb6,0xb6,0xb6,0);  // a light gray color
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_BORDER_BOTTOM_STYLE: // intentional fall-through
		case CSS_PROPERTY_BORDER_LEFT_STYLE:   // intentional fall-through
		case CSS_PROPERTY_BORDER_RIGHT_STYLE:  // intentional fall-through
		case CSS_PROPERTY_BORDER_TOP_STYLE:
			if (Active())
			{
				// tbd - if num colors is less than 4 or 3D widgets is disabled,
				//  style should be CSS_BORDER_STYLE_SOLID
				pValue->borderStyle = CSS_BORDER_STYLE_INSET;
			}
			else
			{
				pValue->borderStyle = CSS_BORDER_STYLE_OUTSET;
			}
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_BORDER_BOTTOM_COLOR: // intentional fall-through
		case CSS_PROPERTY_BORDER_LEFT_COLOR:   // intentional fall-through
		case CSS_PROPERTY_BORDER_RIGHT_COLOR:  // intentional fall-through
		case CSS_PROPERTY_BORDER_TOP_COLOR:
			HTML_ColorSet(pValue->color, 0x80, 0x80, 0x80, 0);
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_BORDER_BOTTOM_WIDTH:
			pValue->borderThickness.type = CSS_BORDER_THICKNESS_LENGTH;
			pValue->borderThickness.val.length.type = CSS_LENGTH_UNIT_PX;
			pValue->borderThickness.val.length.val.dec = WEBC_CFG_BUTTON_BORDER_WIDTH_BOTTOM;
			pValue->borderThickness.val.length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_BORDER_LEFT_WIDTH:
			pValue->borderThickness.type = CSS_BORDER_THICKNESS_LENGTH;
			pValue->borderThickness.val.length.type = CSS_LENGTH_UNIT_PX;
			pValue->borderThickness.val.length.val.dec = WEBC_CFG_BUTTON_BORDER_WIDTH_LEFT;
			pValue->borderThickness.val.length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_BORDER_RIGHT_WIDTH:
			pValue->borderThickness.type = CSS_BORDER_THICKNESS_LENGTH;
			pValue->borderThickness.val.length.type = CSS_LENGTH_UNIT_PX;
			pValue->borderThickness.val.length.val.dec = WEBC_CFG_BUTTON_BORDER_WIDTH_RIGHT;
			pValue->borderThickness.val.length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_BORDER_TOP_WIDTH:
			pValue->borderThickness.type = CSS_BORDER_THICKNESS_LENGTH;
			pValue->borderThickness.val.length.type = CSS_LENGTH_UNIT_PX;
			pValue->borderThickness.val.length.val.dec = WEBC_CFG_BUTTON_BORDER_WIDTH_TOP;
			pValue->borderThickness.val.length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_OUTLINE_STYLE:
  			pValue->borderStyle = (mFlags & HELEM_FLAG_HAS_FOCUS)? CSS_BORDER_STYLE_DOTTED : CSS_BORDER_STYLE_NONE;
		  	return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_OUTLINE_COLOR:
			HTML_ColorSet(pValue->color, 0, 0, 0, 0);
		  	return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_OUTLINE_WIDTH:
			pValue->borderThickness.type = CSS_BORDER_THICKNESS_LENGTH;
			pValue->borderThickness.val.length.type = CSS_LENGTH_UNIT_PX;
			pValue->borderThickness.val.length.val.dec = 1;
			pValue->borderThickness.val.length.val.frac = 0;
		  	return (CSS_VALUE_SPECIFIED);

		default:
			break;
	}

	return (CSS_VALUE_UNKNOWN);
}

WEBC_CHAR* HTMLInputButton::Value(void)
{
	return (mValue);
}

void HTMLInputButton::SetValue(const WEBC_CHAR *pValue)
{
	WEBC_CHAR* newValue = webc_malloc_string_copy(pValue, __FILE__, __LINE__);
	if (mValue)
	{
		webc_free_string_copy(mValue, __FILE__, __LINE__);
	}
	mValue = newValue;
}

/*****************************************************************************/
// HTMLInputButton-Specific Methods
/*****************************************************************************/

DisplayElement *HTMLInputButton::GetDisplayElement (WEBC_BOOL create)
{
	return (GetInputButtonDisplay(create));
}

TextUnit *HTMLInputButton::GetTextUnit (void)
{
	return (GetInputButtonDisplay());
}

HTMLInputButtonDisplay *HTMLInputButton::GetInputButtonDisplay (WEBC_BOOL create)
{
	if (!mpDisplayButton && create)
	{
		WEBC_NEW_VERBOSE(mpDisplayButton, HTMLInputButtonDisplay(this),"HTMLInputButtonDisplay");
		if (mpDisplayButton)
		{
			mpDisplayButton->SetListener(this);
		}
	}

	return (mpDisplayButton);
}

WEBC_BOOL HTMLInputButton::Active (void)
{
	return ((mFlags & HINPUT_FLAG_ACTIVE) != 0);
}

void HTMLInputButton::Notify (Notifier* who, NotifyEvent* what)
{
    WEBC_DEBUG_ReceiveNotify(this, who, what);
	if ((DisplayElement*) who == (DisplayElement*) mpDisplayButton && what->type == NOTIFIER_TYPE_BUTTON)
	{
		switch (what->event)
		{
			case NOTIFY_BUTTON_DOWN:
				SetFlag(HINPUT_FLAG_ACTIVE);
				StyleUpdate();
				break;

			case NOTIFY_BUTTON_UP:
				ClearFlag(HINPUT_FLAG_ACTIVE);
				StyleUpdate();
				break;
            default:
                break;
		}
	}
}
