//
// DHIBUTTON.CPP - Methods for class HTMLInputButtonDisplay
//
// EBS - WebC
//
// Copyright EBS Inc. , 2004
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//

/*****************************************************************************/
// Header Files
/*****************************************************************************/

#include "dhibutton.hpp"
#include "hibutton.hpp"
#include "dmanager.hpp"
#include "htmldoc.hpp"
#include "dhelem.cpp"
#include "objref.cpp"
#include "textstyle.hpp"


/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/

#define BUTTON_ELEMENT         ((HTMLInputButton*)mpHtmlElement)

/*****************************************************************************/
// Local Types
/*****************************************************************************/


/*****************************************************************************/
// Global Data
/*****************************************************************************/


/*****************************************************************************/
// Local functions
/*****************************************************************************/

HTMLInputButtonDisplay::HTMLInputButtonDisplay (HTMLElement *pElem)
	: HTMLElementDisplay<WebButton>(pElem), mBorder(pElem)
{
	mTextStyle = 0;
}

HTMLInputButtonDisplay::~HTMLInputButtonDisplay ()
{
	if (mTextStyle)
	{
		mTextStyle->release();
	}
}

void HTMLInputButtonDisplay::Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect* viewPortRect, WebGraphics* gc)
{
	if (GetVisibility() == TU_VISIBILITY_HIDE)
	{
		return;
	}
	WebRect clip, screenRect(mRect);
	gc->GetClip(&clip);
	screenRect.MoveTo(x,y);
	if (!clip.Overlaps(&screenRect))
	{
		return;
	}

	if (mTextStyle)
	{
		if (mTextStyle->hasBackgroundColor())
		{
			gc->Rectangle (
					&screenRect,
					mTextStyle->getBackgroundColor(),
					mTextStyle->getBackgroundColor(),
					WEBC_TRUE
				);
		}

		WEBC_CHAR* text = mpHtmlElement->Value();
		if (text)
		{
			WEBC_SIZE textLen = webc_strlen(text);
			PresetWebRect textClip(&screenRect);

			textClip.And(&clip);
			gc->SetClip(&textClip);

			WebFont font = mTextStyle->getFont();
			DISPLAY_INT textWidthValue = font? gc->TextWidth(text, font) : 0;

			DISPLAY_INT contentWidthValue = screenRect.Width() -
			                                  (mBorder.GetBorderWidth(CSS_LEFT) +
			                                   mBorder.GetPaddingWidth(CSS_LEFT) +
			                                   mBorder.GetPaddingWidth(CSS_RIGHT) +
			                                   mBorder.GetBorderWidth(CSS_RIGHT));

			if (contentWidthValue > textWidthValue)
			{
				// center the text if there is extra space
				x += (contentWidthValue - textWidthValue) >> 1;
			}
			DISPLAY_INT textHeightValue = font? gc->TextHeight(text, font) : 0;

			DISPLAY_INT contentHeightValue = screenRect.Height() -
			                                  (mBorder.GetBorderWidth(CSS_TOP) +
			                                   mBorder.GetPaddingWidth(CSS_TOP) +
			                                   mBorder.GetPaddingWidth(CSS_BOTTOM) +
			                                   mBorder.GetBorderWidth(CSS_BOTTOM));
			if (contentHeightValue > textHeightValue)
			{
				// center the text if there is extra space
				y += (contentHeightValue - textHeightValue) >> 1;
			}

			mTextStyle->renderText (
					x + mBorder.GetBorderWidth(CSS_LEFT) + mBorder.GetPaddingWidth(CSS_LEFT),
					y + mBorder.GetBorderWidth(CSS_TOP) + mBorder.GetPaddingWidth(CSS_TOP),
					text,
					textLen,
					gc
				);

			gc->SetClip(&clip);
		}
	}

	mBorder.DrawBorder(&screenRect, gc);
}

DisplayElement* HTMLInputButtonDisplay::TrapEventPoint (DISPLAY_INT x, DISPLAY_INT y, DISPLAY_INT viewX, DISPLAY_INT viewY)
{
	if (GetVisibility() == TU_VISIBILITY_HIDE)
	{
		return (0);
	}

	return (WebButton::TrapEventPoint(x,y,viewX,viewY));
}

DISPLAY_INT HTMLInputButtonDisplay::GetTopMargin(void)
{
	return mBorder.GetMarginWidth(CSS_TOP);
}

DISPLAY_INT HTMLInputButtonDisplay::GetLeftMargin(void)
{
	return mBorder.GetMarginWidth(CSS_LEFT);
}

DISPLAY_INT HTMLInputButtonDisplay::GetBottomMargin(void)
{
	return mBorder.GetMarginWidth(CSS_BOTTOM);
}

DISPLAY_INT HTMLInputButtonDisplay::GetRightMargin(void)
{
	return mBorder.GetMarginWidth(CSS_RIGHT);
}

DISPLAY_INT HTMLInputButtonDisplay::GetMinWidth(void)
{
CSSLength cssWidth;

	BUTTON_ELEMENT->GetWidth(&cssWidth);
	if (cssWidth.type == CSS_LENGTH_UNIT_PERCENT)
	{
		return (0);
	}

	return GetPixelWidth();
}

DISPLAY_INT HTMLInputButtonDisplay::GetMaxWidth(void)
{
CSSLength cssWidth;

	BUTTON_ELEMENT->GetWidth(&cssWidth);
	if (cssWidth.type == CSS_LENGTH_UNIT_PERCENT)
	{
		return (0);
	}

	return GetPixelWidth();
}

DISPLAY_INT HTMLInputButtonDisplay::GetPixelWidth(void)
{
	if (!(mHtmlElementDisplayFlags & HELEM_DISPLAY_FLAG_STYLE_SET))
	{
		SetElementStyle();
	}

	return Width();
}

DISPLAY_INT HTMLInputButtonDisplay::GetBaseline(void)
{
	if (!(mHtmlElementDisplayFlags & HELEM_DISPLAY_FLAG_STYLE_SET))
	{
		SetElementStyle();
	}

	WebGraphics *gc = GetGraphics();
	if (gc && mTextStyle)
	{
		WEBC_CHAR* text = mpHtmlElement->Value();
		WebFont font = mTextStyle->getFont();

		static WebChar emStr[2] = {'M',0};
		if (!text || !text[0])
		{
			text = emStr;
		}

		if (font && text)
		{
			DISPLAY_INT textTop  = mBorder.GetBorderWidth(CSS_TOP) + mBorder.GetPaddingWidth(CSS_TOP);
			DISPLAY_INT textBase = WEB_FONT_BASELINE(font);

			return (textTop + textBase);
		}
	}

	return (mRect.Height());
}

DISPLAY_INT HTMLInputButtonDisplay::GetPixelHeight(void)
{
	if (!(mHtmlElementDisplayFlags & HELEM_DISPLAY_FLAG_STYLE_SET))
	{
		SetElementStyle();
	}

	return Height();
}

void HTMLInputButtonDisplay::SetParentDisplay (DisplayElement *parent, DisplayElement* insertBefore)
{
	HTMLElementDisplay<WebButton>::SetParentDisplay(parent, insertBefore);
	if (!(mHtmlElementDisplayFlags & HELEM_DISPLAY_FLAG_STYLE_SET))
	{
		SetElementStyle();
	}
}

void HTMLInputButtonDisplay::SetParentHeight(DISPLAY_INT parentHeight)
{
	CSSLength cssHeight;

	BUTTON_ELEMENT->GetHeight(&cssHeight);
	if (cssHeight.type == CSS_LENGTH_UNIT_PERCENT)
	{
		SetButtonDimensions(mBorder.GetParentWidth(), parentHeight, WEBC_FALSE);
	}
}

void HTMLInputButtonDisplay::SetParentWidth (DISPLAY_INT parentWidth, DISPLAY_INT ignored)
{
	CSSLength cssWidth;

	BUTTON_ELEMENT->GetWidth(&cssWidth);
	if (cssWidth.type == CSS_LENGTH_UNIT_PERCENT)
	{
		SetButtonDimensions(parentWidth, 0, WEBC_TRUE);
	}
}

WEBC_BOOL HTMLInputButtonDisplay::SetElementStyle (void)
{
	if (GetManager())
	{
		mHtmlElementDisplayFlags |= HELEM_DISPLAY_FLAG_STYLE_SET;

		ITextDisplayStyle* textStyle = mpHtmlElement->GetTextDisplayStyle();
		if (textStyle)
		{
			if (!mTextStyle || !mTextStyle->equals(textStyle))
			{
				if (mTextStyle)
				{
					mTextStyle->release();
				}

				mTextStyle = textStyle;
			}
			else
			{
				textStyle->release();
			}
		}

		mBorder.ResetProperties();
		SetButtonDimensions(0, 0, WEBC_TRUE);

		mHtmlElementDisplayFlags |= HELEM_DISPLAY_FLAG_STYLE_SET; // TEST

		if (BUTTON_ELEMENT->mFlags & HINPUT_FLAG_DISABLED)
		{
			Disable();
		}
		else
		{
			Enable();
		}

		return (WEBC_TRUE);
	}

	return (WEBC_FALSE);
}

void HTMLInputButtonDisplay::SetButtonDimensions (
		DISPLAY_INT parentWidth,
		DISPLAY_INT parentHeight,
		WEBC_BOOL parentHeightIsAuto
	)
{
	DISPLAY_INT textWidthValue = 0;
	DISPLAY_INT emHeightValue = 0;
	DISPLAY_INT exHeightValue = 0;

	CSSLength cssWidth;
	CSSLength cssHeight;

	BUTTON_ELEMENT->GetWidth(&cssWidth);
	BUTTON_ELEMENT->GetHeight(&cssHeight);

	if (parentHeightIsAuto && cssHeight.type == CSS_LENGTH_UNIT_PERCENT)
	{
		cssHeight.type = CSS_LENGTH_UNIT_AUTO;
	}

	if (mTextStyle)
	{
		WebFont font = mTextStyle->getFont();

		if (font)
		{
			if (cssWidth.type == CSS_LENGTH_UNIT_AUTO ||
				cssHeight.type == CSS_LENGTH_UNIT_AUTO)
			{
				WebGraphics* gc = GetGraphics();
				WEBC_CHAR* buttonText = mpHtmlElement->Value();

				if(buttonText)
					textWidthValue = gc->TextWidth(buttonText, font);
				else
					textWidthValue = 0;
			}

			emHeightValue = WEB_FONT_HEIGHT(font);
		}
		else
		{
			textWidthValue = 0;
			emHeightValue = WEBC_CFG_DEFAULT_TEXT_HEIGHT;
		}

		exHeightValue = emHeightValue >> 1;
	}

	DISPLAY_INT widthValue;
	DISPLAY_INT heightValue;

	mBorder.SetParentWidth(parentWidth);

	switch (cssWidth.type)
	{
		case CSS_LENGTH_UNIT_AUTO:
			widthValue = mBorder.GetBorderWidth(CSS_LEFT) +
			             mBorder.GetPaddingWidth(CSS_LEFT) +
			             textWidthValue +
			             mBorder.GetPaddingWidth(CSS_RIGHT) +
			             mBorder.GetBorderWidth(CSS_RIGHT);
			break;

		default:
			widthValue = CSS_LengthToPixels(&cssWidth, parentWidth, emHeightValue, exHeightValue);
			break;
	}

	switch (cssHeight.type)
	{
		case CSS_LENGTH_UNIT_AUTO:
			heightValue = mBorder.GetBorderWidth(CSS_TOP) +
			              mBorder.GetPaddingWidth(CSS_TOP) +
			              emHeightValue +
			              mBorder.GetPaddingWidth(CSS_BOTTOM) +
			              mBorder.GetBorderWidth(CSS_BOTTOM);
			break;

		default:
			heightValue = CSS_LengthToPixels(&cssHeight, parentHeight, emHeightValue, exHeightValue);
			break;
	}

	SizeTo(widthValue, heightValue);
}
