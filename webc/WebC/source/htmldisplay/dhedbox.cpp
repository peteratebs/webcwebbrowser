//
// DHEDBOX.CPP - Methods for class HTMLEditBoxDisplay
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//

/*****************************************************************************/
// Header Files
/*****************************************************************************/

#include "dhedbox.hpp"
#include "hcbox.hpp"
#include "dmanager.hpp"
#include "css.h"
#include "htxtarea.hpp"
#include "dhelem.cpp"
#include "dhedit.cpp"


/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/

#define TEXTAREA_ELEMENT   ((HTMLTextArea*)mpHtmlElement)


/*****************************************************************************/
// Local Types
/*****************************************************************************/


/*****************************************************************************/
// Global Data
/*****************************************************************************/


/*****************************************************************************/
// Local functions
/*****************************************************************************/

HTMLEditBoxDisplay::HTMLEditBoxDisplay (HTMLElement *pElem)
	: HTMLEditDisplay<WebEditBox> (pElem)
{
}

HTMLEditBoxDisplay::~HTMLEditBoxDisplay ()
{
}

WEBC_BOOL HTMLEditBoxDisplay::SetElementStyle (void)
{
	// Set wrap
	switch (TEXTAREA_ELEMENT->Wrap())
	{
	case TEXTAREA_WRAP_OFF:
		SetWrap(0);
		break;

	default:
		SetWrap(1);
		break;
	}
	CSSPropertyValue overflowValue;
		if (TEXTAREA_ELEMENT->GetStyleFromCSS(CSS_PROPERTY_OVERFLOW, &overflowValue) == CSS_VALUE_SPECIFIED)
		{
			if (CSS_OVERFLOW_HIDDEN == overflowValue.overflow)
			{
				// WebEditBox::
				SetScrollMode (SCROLL_MODE_NONE);
			}
		}
	return (HTMLEditDisplay<WebEditBox>::SetElementStyle());
}

DISPLAY_INT HTMLEditBoxDisplay::FindPixelWidth (HTMLContentBox *b, WebGraphics *gc)
{
CSSLength cssWidth;
WebChar str[2] = {(WebChar) 'M', 0};
DISPLAY_INT pixelWidth;
WebFont font = mFont.GetFont();

	TEXTAREA_ELEMENT->GetWidth(&cssWidth);

	switch (cssWidth.type)
	{
	case CSS_LENGTH_UNIT_AUTO:
		pixelWidth = (font? gc->TextWidth(str, font) : 0) * TEXTAREA_ELEMENT->Cols() + webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_WIDTH);
		pixelWidth += b->GetBorderWidth(CSS_LEFT) + b->GetBorderWidth(CSS_RIGHT);
		pixelWidth += b->GetPaddingWidth(CSS_LEFT) + b->GetPaddingWidth(CSS_RIGHT);
		break;

	default:
		pixelWidth = CSS_LengthToPixels(&cssWidth, b->GetParentWidth(), b->GetEmHeight(), b->GetExHeight());
		break;
	}

	return (pixelWidth);
}

DISPLAY_INT HTMLEditBoxDisplay::FindPixelHeight (HTMLContentBox *b, DISPLAY_INT parentHeight, WebGraphics *gc)
{
CSSLength cssHeight;
DISPLAY_INT pixelHeight;
WebFont font = mFont.GetFont();
DISPLAY_INT fontHeight = font? WEB_FONT_HEIGHT(font) : WEBC_CFG_DEFAULT_TEXT_HEIGHT;

	TEXTAREA_ELEMENT->GetHeight(&cssHeight);

	switch (cssHeight.type)
	{
	case CSS_LENGTH_UNIT_AUTO:
		pixelHeight = fontHeight * TEXTAREA_ELEMENT->Rows() + webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_WIDTH);
		pixelHeight += b->GetBorderWidth(CSS_TOP) + b->GetBorderWidth(CSS_BOTTOM);
		pixelHeight += b->GetPaddingWidth(CSS_TOP) + b->GetPaddingWidth(CSS_BOTTOM);
		break;

	default:
		pixelHeight = CSS_LengthToPixels(&cssHeight, parentHeight, b->GetEmHeight(), b->GetExHeight());
		break;
	}

	return (pixelHeight);
}

void HTMLEditBoxDisplay::GetTextRect(WebRect *rect)
{
	HTMLEditDisplay<WebEditBox>::GetTextRect(rect);

	if (mpVScroll)
	{
		rect->right -= mpVScroll->Width();
	}

	if (mpHScroll)
	{
		rect->bottom -= mpHScroll->Height();
	}
}

WEBC_BOOL HTMLEditBoxDisplay::SizeIsValid (void)
{
	return (!(mEditFlags & EDIT_FLAG_SIZE_INVALID));
}

void HTMLEditBoxDisplay::SetSizeValid (WEBC_BOOL valid)
{
	if (valid)
	{
		mEditFlags &= ~EDIT_FLAG_SIZE_INVALID;
	}
	else
	{
		mEditFlags |= EDIT_FLAG_SIZE_INVALID;
	}
}
