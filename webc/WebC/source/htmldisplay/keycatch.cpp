//
// KEYCATCH.CPP - Methods for class DisplayKeyCatcher
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

#include "helement.hpp"
#include "keycatch.hpp"
#include "assert.h"


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

DisplayKeyCatcher::DisplayKeyCatcher(void)
{
	mpHtmlElement = 0;
}

DisplayKeyCatcher::~DisplayKeyCatcher(void)
{
}

void DisplayKeyCatcher::Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC)
{
	// do nothing
}

HTMLEventStatus DisplayKeyCatcher::Event (HTMLEvent *pEvent)
{
    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	if (mpHtmlElement)
	{
		return mpHtmlElement->Event(pEvent);
	}
	return (HTML_EVENT_STATUS_CONTINUE);
}

DisplayElement *DisplayKeyCatcher::TrapEventPoint (DISPLAY_INT x, DISPLAY_INT y, DISPLAY_INT viewX, DISPLAY_INT viewY)
{
	return 0;
}

void DisplayKeyCatcher::SetElement(HTMLElement *pElem)
{
	mpHtmlElement = pElem;
}

HTMLElement *DisplayKeyCatcher::GetElement(void)
{
	return mpHtmlElement;
}
