//
// DHPASSWD.CPP - Methods for class HTMLPasswordDisplay
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

#include "dhpasswd.hpp"
#include "hcbox.hpp"
#include "dmanager.hpp"
#include "css.h"
#include "hpasswd.hpp"
#include "htmldoc.hpp"
#include "dhelem.cpp"
#include "dhedit.cpp"
#include "objref.cpp"


/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/

#define PASSWD_ELEMENT   ((HTMLInputPassword*)mpHtmlElement)


/*****************************************************************************/
// Local Types
/*****************************************************************************/


/*****************************************************************************/
// Global Data
/*****************************************************************************/


/*****************************************************************************/
// Local functions
/*****************************************************************************/

HTMLPasswordDisplay::HTMLPasswordDisplay (HTMLElement *pElem)
	: HTMLEditDisplay<WebPasswordString> (pElem)
{
}

HTMLPasswordDisplay::~HTMLPasswordDisplay ()
{
}

DISPLAY_INT HTMLPasswordDisplay::FindPixelWidth (HTMLContentBox *b, WebGraphics *gc)
{
CSSLength cssWidth;
WebChar str[2] = {(WebChar) 'E', 0};
DISPLAY_INT pixelWidth;

	PASSWD_ELEMENT->GetWidth(&cssWidth);

	switch (cssWidth.type)
	{
		case CSS_LENGTH_UNIT_AUTO:
			pixelWidth = gc->TextWidth(str, mFont.GetFont()) * PASSWD_ELEMENT->Size();
			pixelWidth += b->GetBorderWidth(CSS_LEFT) + b->GetBorderWidth(CSS_RIGHT);
			pixelWidth += b->GetPaddingWidth(CSS_LEFT) + b->GetPaddingWidth(CSS_RIGHT);
			break;

		default:
			pixelWidth = CSS_LengthToPixels(&cssWidth, b->GetParentWidth(), b->GetEmHeight(), b->GetExHeight());
			break;
	}

	return (pixelWidth);
}

WEBC_BOOL HTMLPasswordDisplay::SizeIsValid (void)
{
	return (!(mEditFlags & EDIT_FLAG_SIZE_INVALID));
}

void HTMLPasswordDisplay::SetSizeValid (WEBC_BOOL valid)
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

DISPLAY_INT HTMLPasswordDisplay::GetBaseline(void)
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
