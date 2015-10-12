#ifndef __DHEDIT_CPP__
#define __DHEDIT_CPP__

//
// DHEDIT.CPP - Template class methods for all edit-type widgets
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

#include "dhedit.hpp"
#include "hcbox.hpp"
#include "dmanager.hpp"
#include "css.h"
#include "hedit.hpp"

/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/


/*****************************************************************************/
// Local Types
/*****************************************************************************/


/*****************************************************************************/
// Global Data
/*****************************************************************************/


/*****************************************************************************/
// Local functions
/*****************************************************************************/

template<class T> HTMLEditDisplay<T>::HTMLEditDisplay (HTMLElement *pElem)
	: HTMLElementDisplay<T>(pElem)
{
	miParentWidth = 0;
	miParentHeight = 0;
}

template<class T> HTMLEditDisplay<T>::~HTMLEditDisplay ()
{
}

// DisplayElement methods
template<class T> void HTMLEditDisplay<T>::Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC)
{
  if (this->GetVisibility() != TU_VISIBILITY_HIDE)
	{
		HTMLElementDisplay<T>::Draw(x,y,pViewport,pGC);
	}
}

template<class T> DisplayElement *HTMLEditDisplay<T>::TrapEventPoint (DISPLAY_INT x, DISPLAY_INT y, DISPLAY_INT viewX, DISPLAY_INT viewY)
{
	if (this->GetVisibility() == TU_VISIBILITY_HIDE)
	{
		return (0);
	}

	return (HTMLElementDisplay<T>::TrapEventPoint(x,y,viewX,viewY));
}

// TextUnit methods
template<class T> DISPLAY_INT HTMLEditDisplay<T>::GetMinWidth(void)
{
CSSLength cssWidth;

	this->mpHtmlElement->GetWidth(&cssWidth);
	if (cssWidth.type == CSS_LENGTH_UNIT_PERCENT)
	{
		return (0);
	}

	return GetPixelWidth();
}

template<class T> DISPLAY_INT HTMLEditDisplay<T>::GetMaxWidth(void)
{
CSSLength cssWidth;

	this->mpHtmlElement->GetWidth(&cssWidth);
	if (cssWidth.type == CSS_LENGTH_UNIT_PERCENT)
	{
		return (0);
	}

	return GetPixelWidth();
}

template<class T> DISPLAY_INT HTMLEditDisplay<T>::GetPixelWidth(void)
{
	if (!(this->mHtmlElementDisplayFlags & HELEM_DISPLAY_FLAG_STYLE_SET))
	{
		SetElementStyle();
	}
	else if (!SizeIsValid())
	{
		WebGraphics *gc = T::GetGraphics();
		if (gc)
		{
			SetDimensions(gc);
		}
	}

	return T::Width();
}

template<class T> DISPLAY_INT HTMLEditDisplay<T>::GetPixelHeight(void)
{
	if (!(this->mHtmlElementDisplayFlags & HELEM_DISPLAY_FLAG_STYLE_SET))
	{
		SetElementStyle();
	}
	else if (!SizeIsValid())
	{
		WebGraphics *gc = T::GetGraphics();
		if (gc)
		{
			SetDimensions(gc);
		}
	}

	return T::Height();
}

template<class T> void HTMLEditDisplay<T>::SetParentDisplay(DisplayElement *parent, DisplayElement* insertBefore)
{
	HTMLElementDisplay<T>::SetParentDisplay(parent, insertBefore);
	if (!(this->mHtmlElementDisplayFlags & HELEM_DISPLAY_FLAG_STYLE_SET))
	{
		SetElementStyle();
	}
}

template<class T> void HTMLEditDisplay<T>::SetParentHeight(DISPLAY_INT parentHeight)
{
	if (parentHeight != miParentHeight)
	{
		SetSizeValid(WEBC_FALSE);
		miParentHeight = parentHeight;
	}
}

template<class T> void HTMLEditDisplay<T>::SetParentWidth(DISPLAY_INT parentWidth, DISPLAY_INT ignored)
{
	if (parentWidth != miParentWidth)
	{
		SetSizeValid(WEBC_FALSE);
		miParentWidth = parentWidth;
	}
}

template<class T> void HTMLEditDisplay<T>::GetFrameRect(WebRect *rect)
{
	rect->Set(0, 0, T::mRect.Width() - 1, T::mRect.Height() - 1);
}

template<class T> void HTMLEditDisplay<T>::GetTextRect(WebRect *rect)
{
	WebGraphics *gc = T::GetGraphics();

	if (!SizeIsValid() && gc)
	{
		SetDimensions(gc);
	}

	rect->Set(&mTextRect);
}

template<class T> void HTMLEditDisplay<T>::DrawFrame(WebRect *box, WebGraphics *gc)
{
HTMLContentBox b(this->mpHtmlElement, miParentWidth);

	b.DrawBorder(box, gc);
}

template<class T> DISPLAY_INT HTMLEditDisplay<T>::FindPixelHeight (HTMLContentBox *b, DISPLAY_INT parentHeight, WebGraphics *gc)
{
CSSLength cssHeight;
DISPLAY_INT pixelHeight;
WebFont font = T::mFont.GetFont();
DISPLAY_INT fontHeight = font? WEB_FONT_HEIGHT(font) : WEBC_CFG_DEFAULT_TEXT_HEIGHT;

	this->mpHtmlElement->GetHeight(&cssHeight);

	switch (cssHeight.type)
	{
	case CSS_LENGTH_UNIT_AUTO:
		pixelHeight = fontHeight;
		pixelHeight += b->GetBorderWidth(CSS_TOP) + b->GetBorderWidth(CSS_BOTTOM);
		pixelHeight += b->GetPaddingWidth(CSS_TOP) + b->GetPaddingWidth(CSS_BOTTOM);
		break;

	default:
		pixelHeight = CSS_LengthToPixels(&cssHeight, parentHeight, b->GetEmHeight(), b->GetExHeight());
		break;
	}

	return (pixelHeight);
}

template<class T> void HTMLEditDisplay<T>::FindTextRect(WebRect *textRect, WebRect *elemRect, HTMLContentBox *b)
{
	textRect->Set(
		(b->GetBorderWidth(CSS_LEFT) + b->GetPaddingWidth(CSS_LEFT)),
		(b->GetBorderWidth(CSS_TOP)  + b->GetPaddingWidth(CSS_TOP)),
		elemRect->Width()  - 1 - (b->GetBorderWidth(CSS_RIGHT)  + b->GetPaddingWidth(CSS_RIGHT)),
		elemRect->Height() - 1 - (b->GetBorderWidth(CSS_BOTTOM) + b->GetPaddingWidth(CSS_BOTTOM)));

	textRect->right  = EBSMAX(0, textRect->right);
	textRect->bottom = EBSMAX(0, textRect->bottom);
	textRect->left   = EBSMIN(textRect->left, textRect->right);
	textRect->top    = EBSMIN(textRect->top, textRect->bottom);
}

template<class T> void HTMLEditDisplay<T>::SetDimensions(WebGraphics *gc)
{
HTMLContentBox b(this->mpHtmlElement, miParentWidth);

	SetSizeValid();

	WebRect rect(T::mRect);
	rect.SizeTo(FindPixelWidth(&b, gc), FindPixelHeight(&b, miParentHeight, gc));
	FindTextRect(&mTextRect, &rect, &b);
	T::Move(&rect);
}

template<class T> WEBC_BOOL HTMLEditDisplay<T>::SetElementStyle (void)
{
	if (T::GetManager())
	{
		WebGraphics *gc = T::GetManager()->GetGraphics();
		if (!gc)
		{
			return (WEBC_FALSE);
		}

		this->mHtmlElementDisplayFlags |= HELEM_DISPLAY_FLAG_STYLE_SET;


		// Set font
		this->SetFont(this->mpHtmlElement->GetFont());

		// Set background color
		HTMLColor rgbColor = this->mpHtmlElement->GetBackgroundColor();
		mBackgroundColor = gc->RGBToColor(rgbColor);
		// Set text color
		rgbColor = this->mpHtmlElement->GetColor();
		mTextColor = gc->RGBToColor(rgbColor);

		// Set element dimensions
		SetDimensions(gc);

		if (this->mpHtmlElement->mFlags & HINPUT_FLAG_DISABLED)
		{
			T::Disable();
		}
		else
		{
			T::Enable();
		}

		if (this->mpHtmlElement->mFlags & HINPUT_FLAG_READONLY)
		{
			T::SetFlag(DISPLAY_FLAG_READONLY);
		}
		else
		{
			T::ClearFlag(DISPLAY_FLAG_READONLY);
		}
	}

	return (WEBC_TRUE);
}

template<class T> DISPLAY_INT HTMLEditDisplay<T>::GetParentWidth(void)
{
	return (miParentWidth);
}

template<class T> WebColor HTMLEditDisplay<T>::GetTextColor (WebGraphics* gc)
{
	return mTextColor;
}

template<class T> WebColor HTMLEditDisplay<T>::GetBgColor (WebGraphics* gc)
{
	return mBackgroundColor;
}

#endif // __DHEDIT_CPP__
