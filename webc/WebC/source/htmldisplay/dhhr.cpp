//
// DHHR.CPP - Methods for class HTMLHRDisplay
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

#include "dhhr.hpp"
#include "hhr.hpp"
#include "wgcolors.hpp"
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


/*****************************************************************************/
// HTMLHRDisplay methods
/*****************************************************************************/

HTMLHRDisplay::HTMLHRDisplay (HTMLHorizontalRule *pImage)
	: HTMLElementDisplay<DisplayElement> (pImage)
{
	SetElementStyle();
}

HTMLHRDisplay::~HTMLHRDisplay ()
{
}


void HTMLHRDisplay::Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC)
{
	if (GetVisibility() == TU_VISIBILITY_HIDE)
	{
		return;
	}

	WebRect clip, screenRect(mRect);
	pGC->GetClip(&clip);
	screenRect.MoveTo(x,y);
	if (!clip.Overlaps(&screenRect))
	{
		return;
	}

	HTMLHorizontalRule *pHR = (HTMLHorizontalRule*) mpHtmlElement;

//	pGC->StartBuffer();

	screenRect.Set(screenRect.left, screenRect.top + 8,
	               screenRect.right, screenRect.top + 8 + pHR->RuleSize() - 1);

  #if (WEBC_USE_3D_FRAMES)
	pGC->Frame3D(&screenRect, pGC->GetColorByIndex(WGC_DARKGRAY), pGC->GetColorByIndex(WGC_LIGHTGRAY), EBSMAX(1, pHR->RuleSize()/2));
  #else
	pGC->Rectangle(&screenRect, pGC->GetColorByIndex(WGC_BLACK), pGC->GetColorByIndex(WGC_BLACK), 1);
  #endif

//	pGC->EndBuffer();
}

DISPLAY_INT HTMLHRDisplay::GetMinWidth(void)
{
	HTMLHorizontalRule *pHR = (HTMLHorizontalRule *) mpHtmlElement;
	if (pHR && HTML_LengthGetUnit(pHR->RuleWidth()) == HTML_LENGTH_UNIT_PERCENT)
	{
		return (0);
	}
	return GetPixelWidth();
}

DISPLAY_INT HTMLHRDisplay::GetMaxWidth(void)
{
	HTMLHorizontalRule *pHR = (HTMLHorizontalRule *) mpHtmlElement;
	if (pHR && HTML_LengthGetUnit(pHR->RuleWidth()) == HTML_LENGTH_UNIT_PERCENT)
	{
		return (0);
	}
	return GetPixelWidth();
}

void HTMLHRDisplay::SetParentWidth(DISPLAY_INT ignored, DISPLAY_INT iWidth)
{
	HTMLHorizontalRule *pHR = (HTMLHorizontalRule *) mpHtmlElement;
	if (pHR && HTML_LengthGetUnit(pHR->RuleWidth()) == HTML_LENGTH_UNIT_PERCENT)
	{
		DISPLAY_INT iPixelWidth = (iWidth * HTML_LengthGetValue(pHR->RuleWidth())) / 100;
		SetWidth(iPixelWidth);
	}
}

WEBC_BOOL HTMLHRDisplay::SetElementStyle (void)
{
	WEBC_BOOL changed = WEBC_FALSE;

	DISPLAY_INT iNewWidth = Width(), iNewHeight = Height();

	WebRect oldRect(mRect);

	HTMLHorizontalRule *pHR = (HTMLHorizontalRule *) mpHtmlElement;
	if (pHR)
	{
		if (HTML_LengthGetUnit(pHR->RuleWidth()) == HTML_LENGTH_UNIT_PIXELS)
		{
			iNewWidth = HTML_LengthGetValue(pHR->RuleWidth());
		}

		iNewHeight = pHR->RuleSize() + (WEBC_HR_SPACING * 2);
	}

	SizeTo(iNewWidth, iNewHeight);

	if (!oldRect.Equals(&mRect))
	{
		changed = WEBC_TRUE;
	}

	return (changed);
}
