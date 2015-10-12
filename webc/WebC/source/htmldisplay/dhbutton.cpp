//
// DHBUTTON.CPP - Methods for class HTMLButtonDisplay
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

#include "dflow.hpp"
#include "margins.hpp"
#include "textunit.hpp"
#include "tuenum.hpp"
#include "zdelem.hpp"
#include "hbutton.hpp"
#include "dhbutton.hpp"
#include "dhelem.cpp"

/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/


#define BUTTON_ELEMENT   ((HTMLButton *) mpHtmlElement)


/*****************************************************************************/
// Local Types
/*****************************************************************************/


/*****************************************************************************/
// Global Data
/*****************************************************************************/


/*****************************************************************************/
// Local functions
/*****************************************************************************/

HTMLButtonDisplay::HTMLButtonDisplay(HTMLButton * f)
	: HTMLFlowDisplay (f)
{
}

HTMLButtonDisplay::~HTMLButtonDisplay()
{
}

void HTMLButtonDisplay::Format(void)
{
    SetWidth(GetPixelWidth());
    HTMLFlowDisplay::Format();
}

// Button display's TrapEventPoint overrides the default behavior by capturing the event for itself first
// normally children recieve the event first.

DisplayElement *HTMLButtonDisplay::TrapEventPoint (DISPLAY_INT x, DISPLAY_INT y, DISPLAY_INT viewX, DISPLAY_INT viewY)
{
	// if we contain the point, trap it
	if (TrapEventInThis(x,y))
	{
		return (this);
	}
    return(DisplayElement::TrapEventPoint (x, y, viewX, viewY));
}


// Maxwidth finds the maximum width of all children
DISPLAY_INT HTMLButtonDisplay::GetMaxWidth(void)
{
	if (!MaxWidthValid())
	{
		{
			miMaxWidth = FindMaxWidth(0x7fff);
		}

		mFlowFlags |= FD_FLAG_MAXWIDTH_VALID;
	}
	return (miMaxWidth);
}

// We return the widest child as our pixelwidth
DISPLAY_INT HTMLButtonDisplay::GetPixelWidth(void)
{
    return (GetMaxWidth());
}

/* The rest we are just calling the base class, these are here for expermentation */
void HTMLButtonDisplay::Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC)
{
    HTMLFlowDisplay::Draw (x, y, pViewport, pGC);
}

/*---------------------------------------------------------------------------*/
void HTMLButtonDisplay::DrawThisOnly(DISPLAY_INT x, DISPLAY_INT y, WebGraphics * gc)
{
    HTMLFlowDisplay::DrawThisOnly(x, y, gc);

}
DISPLAY_INT HTMLButtonDisplay::Width(void)
{
	return GetPixelWidth();
}
void HTMLButtonDisplay::SetParentWidth(DISPLAY_INT ignored, DISPLAY_INT newParentWidth)
{
    HTMLFlowDisplay::SetParentWidth(ignored, newParentWidth);
}
// Minwidth finds the minimum width of all children (always returns 1)
DISPLAY_INT HTMLButtonDisplay::GetMinWidth(void)
{
    return (HTMLFlowDisplay::GetMinWidth());
}


DISPLAY_INT HTMLButtonDisplay::GetPixelHeight(void)
{
    return(HTMLFlowDisplay::GetPixelHeight());
}
