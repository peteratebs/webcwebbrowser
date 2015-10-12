//
// DHAREA.CPP - Methods for class HTMLAreaDisplay
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

#include "dhelem.cpp"
#include "dharea.hpp"
#include "harea.hpp"
#include "heventce.hpp"


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
// DisplayElement methods
/*****************************************************************************/

HTMLAreaDisplay::HTMLAreaDisplay (HTMLArea *pElem, HTMLElement *pAreaParent)
	: HTMLElementDisplay<DisplayElement> (pElem)
{
	mpEventParent = pAreaParent;
}

HTMLAreaDisplay::~HTMLAreaDisplay ()
{
}

void HTMLAreaDisplay::Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC)
{
	// areas are invisible for now (until we add focus indicators maybe?)
}

HTMLEventStatus HTMLAreaDisplay::Event (HTMLEvent *e)
{
    WEBC_DEBUG_ReceiveEvent(this, e);
	if (!mpHtmlElement)
		return HTML_EVENT_STATUS_CONTINUE;

	HTMLEventCouplerElement coupler(mpHtmlElement, mpEventParent);
	HTMLEventStatus status = coupler.Event(e);
	if (status == HTML_EVENT_STATUS_CONTINUE)
	{
		return DisplayElement::Event(e);
	}

	return (status);
}

HTMLElement *HTMLAreaDisplay::GetAreaParent (void)
{
	return (mpEventParent);
}

void HTMLAreaDisplay::SetAreaDimensions (void)
{
	// do nothing for the mean time.
}

HTMLArea *HTMLAreaDisplay::GetAreaElement (void)
{
	return ((HTMLArea *) mpHtmlElement);
}

