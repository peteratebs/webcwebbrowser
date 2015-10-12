//
// DHIFRAME.CPP - Methods for class HTMLIFrameDisplay
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

#include "hiframe.hpp"
#include "dhiframe.hpp"
#include "dhelem.cpp"


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
#if (WEBC_SUPPORT_IFRAMES)

HTMLIFrameDisplay::HTMLIFrameDisplay (HTMLInlineFrame *pElem)
	: HTMLFrameDisplay (pElem)
{
	SetDimensions();
}

HTMLIFrameDisplay::~HTMLIFrameDisplay ()
{
}

DisplayElement *HTMLIFrameDisplay::FocusDefault (void)
{
	return (mpParent);
}

DISPLAY_INT HTMLIFrameDisplay::GetMinWidth(void)
{
	HTMLInlineFrame *pFrame = (HTMLInlineFrame *) mpHtmlElement;
	if (HTML_LengthGetUnit(pFrame->FrameWidth()) == HTML_LENGTH_UNIT_PERCENT)
	{
		return (0);
	}
	return GetPixelWidth();
}

DISPLAY_INT HTMLIFrameDisplay::GetMaxWidth(void)
{
	HTMLInlineFrame *pFrame = (HTMLInlineFrame *) mpHtmlElement;
	if (HTML_LengthGetUnit(pFrame->FrameWidth()) == HTML_LENGTH_UNIT_PERCENT)
	{
		return (0);
	}
	return GetPixelWidth();
}

void HTMLIFrameDisplay::SetParentWidth(DISPLAY_INT ignored, DISPLAY_INT iWidth)
{
	HTMLInlineFrame *pFrame = (HTMLInlineFrame *) mpHtmlElement;

	mBorder.SetParentWidth(iWidth);

	if (pFrame && HTML_LengthGetUnit(pFrame->FrameWidth()) == HTML_LENGTH_UNIT_PERCENT)
	{
		DISPLAY_INT iPixelWidth = (iWidth * HTML_LengthGetValue(pFrame->FrameWidth())) / 100;
		SetWidth(iPixelWidth);
	}
}

void HTMLIFrameDisplay::SetParentHeight(DISPLAY_INT iHeight)
{
	HTMLInlineFrame *pFrame = (HTMLInlineFrame *) mpHtmlElement;
	if (pFrame && HTML_LengthGetUnit(pFrame->FrameHeight()) == HTML_LENGTH_UNIT_PERCENT)
	{
		DISPLAY_INT iPixelHeight = (iHeight * HTML_LengthGetValue(pFrame->FrameHeight())) / 100;
		SetHeight(iPixelHeight);
	}
}

void HTMLIFrameDisplay::SetDimensions (void)
{
	if (!mpHtmlElement)
	{
		return;
	}

	HTMLInlineFrame *pFrame = (HTMLInlineFrame *) mpHtmlElement;
	WebRect rect(mRect);

	DISPLAY_INT iPixelWidth, iPixelHeight;

	switch (HTML_LengthGetUnit(pFrame->FrameWidth()))
	{
		case HTML_LENGTH_UNIT_PIXELS:
			iPixelWidth = HTML_LengthGetValue(pFrame->FrameWidth());
			break;

		case HTML_LENGTH_UNIT_NONE:
			iPixelWidth = 300; // tbd - move this to webcfg.h
			break;

		default:
			iPixelWidth = Width();
			break;
	}

	switch (HTML_LengthGetUnit(pFrame->FrameHeight()))
	{
		case HTML_LENGTH_UNIT_PIXELS:
			iPixelHeight = HTML_LengthGetValue(pFrame->FrameHeight());
			break;

		case HTML_LENGTH_UNIT_NONE:
			iPixelHeight = 150; // tbd - move this to webcfg.h
			break;

		default:
			iPixelHeight = Height();
			break;
	}

	rect.SizeTo(iPixelWidth, iPixelHeight);
	Move(&rect);
}

WEBC_BOOL HTMLIFrameDisplay::SetElementStyle(void)
{
	WEBC_BOOL b = HTMLFrameDisplay::SetElementStyle();
	SetDimensions();
	return (b);
}

// returns the formatted content height
DISPLAY_INT HTMLIFrameDisplay::FormatContentPositioned (
		DISPLAY_INT widthValue,
		DISPLAY_INT heightValue,
		WEBC_BOOL   heightIsAuto,
		DISPLAY_INT borderLeftValue,
		DISPLAY_INT borderTopValue,
		DISPLAY_INT borderRightValue,
		DISPLAY_INT borderBottomValue,
		DISPLAY_INT paddingLeftValue,
		DISPLAY_INT paddingTopValue,
		DISPLAY_INT paddingRightValue,
		DISPLAY_INT paddingBottomValue
	)
{
	return (heightIsAuto? 150 : heightValue);
}

WEBC_BOOL HTMLIFrameDisplay::GetIntrinsicWidth (
		DISPLAY_INT* intrinsicWidth,
		DISPLAY_INT  height,
		WEBC_BOOL    heightIsAuto
	)
{
	*intrinsicWidth = 300;
	return (WEBC_TRUE);
}

/*---------------------------------------------------------------------------*/
DISPLAY_INT HTMLIFrameDisplay::GetTopMargin(void)
{
	return mBorder.GetMarginWidth(CSS_TOP);
}

/*---------------------------------------------------------------------------*/
DISPLAY_INT HTMLIFrameDisplay::GetLeftMargin(void)
{
	return mBorder.GetMarginWidth(CSS_LEFT);
}

/*---------------------------------------------------------------------------*/
DISPLAY_INT HTMLIFrameDisplay::GetBottomMargin(void)
{
	return mBorder.GetMarginWidth(CSS_BOTTOM);
}

/*---------------------------------------------------------------------------*/
DISPLAY_INT HTMLIFrameDisplay::GetRightMargin(void)
{
	return mBorder.GetMarginWidth(CSS_RIGHT);
}

#endif // WEBC_SUPPORT_IFRAMES
