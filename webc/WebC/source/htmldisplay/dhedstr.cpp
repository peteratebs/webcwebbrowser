//
// DHEDSTR.CPP - Methods for class HTMLEditStringDisplay
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

#include "dhedstr.hpp"
#include "hcbox.hpp"
#include "dmanager.hpp"
#include "css.h"
#include "hedit.hpp"
#include "htmldoc.hpp"
#include "dhelem.cpp"
#include "dhedit.cpp"
#include "objref.cpp"


/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/

#define EDIT_ELEMENT   ((HTMLEditString*)mpHtmlElement)


/*****************************************************************************/
// Local Types
/*****************************************************************************/


/*****************************************************************************/
// Global Data
/*****************************************************************************/


/*****************************************************************************/
// Local functions
/*****************************************************************************/

HTMLEditStringDisplay::HTMLEditStringDisplay (HTMLElement *pElem)
	: HTMLEditDisplay<WebEditString> (pElem)
{
}

HTMLEditStringDisplay::~HTMLEditStringDisplay ()
{
}

DISPLAY_INT HTMLEditStringDisplay::GetBaseline(void)
{
	if (!(mHtmlElementDisplayFlags & HELEM_DISPLAY_FLAG_STYLE_SET))
	{
		SetElementStyle();
	}

	WebGraphics *gc = GetGraphics();
	if (gc)
	{
		WebFont font = mFont.GetFont();
		if (font)
		{
			WebRect box;
			GetTextRect(&box);

			DISPLAY_INT textTop    = box.top;
			DISPLAY_INT textBase   = WEB_FONT_BASELINE(font);

			return (textTop + textBase);
		}
	}

	return (mRect.Height());
}

DISPLAY_INT HTMLEditStringDisplay::FindPixelWidth (HTMLContentBox *b, WebGraphics *gc)
{
CSSLength cssWidth;
WebChar str[2] = {(WebChar) 'E', 0};
DISPLAY_INT pixelWidth;

	EDIT_ELEMENT->GetWidth(&cssWidth);

	switch (cssWidth.type)
	{
		case CSS_LENGTH_UNIT_AUTO:
		{
			WebFont font = mFont.GetFont();
			pixelWidth = font? ((gc->TextWidth(str, font) - 2) * EDIT_ELEMENT->Size()) : 0;
			pixelWidth += b->GetBorderWidth(CSS_LEFT) + b->GetBorderWidth(CSS_RIGHT);
			pixelWidth += b->GetPaddingWidth(CSS_LEFT) + b->GetPaddingWidth(CSS_RIGHT);
			break;
		}

		default:
			pixelWidth = CSS_LengthToPixels(&cssWidth, b->GetParentWidth(), b->GetEmHeight(), b->GetExHeight());
			break;
	}

#if (WEBC_SUPPORT_SMALL_SCREEN)
	if (pixelWidth > WEBC_SCREEN_WIDTH)
	{
		pixelWidth  = WEBC_SCREEN_WIDTH;
	}
#endif

	return (pixelWidth);
}

WEBC_BOOL HTMLEditStringDisplay::SizeIsValid (void)
{
	return (!(mEditFlags & EDIT_FLAG_SIZE_INVALID));
}

void HTMLEditStringDisplay::SetSizeValid (WEBC_BOOL valid)
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
