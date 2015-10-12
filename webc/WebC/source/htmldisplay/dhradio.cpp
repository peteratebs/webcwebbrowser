//
// DHRADIO.CPP - Methods for class HTMLRadioButtonDisplay
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

#include "dhradio.hpp"
#include "hradio.hpp"
#include "dhelem.hpp"
#include "dhelem.cpp"

/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/

#define RADIO_ELEMENT  ((HTMLRadioButton*)mpHtmlElement)


/*****************************************************************************/
// Local Types
/*****************************************************************************/


/*****************************************************************************/
// Global Data
/*****************************************************************************/


/*****************************************************************************/
// Local functions
/*****************************************************************************/

HTMLRadioButtonDisplay::HTMLRadioButtonDisplay (HTMLElement *pElem)
	: HTMLElementDisplay<WebRadioButton> (pElem)
{
}

HTMLRadioButtonDisplay::~HTMLRadioButtonDisplay ()
{
}

void HTMLRadioButtonDisplay::SetSelected (WEBC_BOOL status)
{
	if (status && (status != GetSelected()))
	{
		RADIO_ELEMENT->UnselectSiblings();
	}
	WebRadioButton::SetSelected(status);
}

DISPLAY_INT HTMLRadioButtonDisplay::GetBaseline (void)
{
	return Height() - 5;
}

void HTMLRadioButtonDisplay::SetParentDisplay(DisplayElement *parent, DisplayElement* insertBefore)
{
	HTMLElementDisplay<WebRadioButton>::SetParentDisplay(parent, insertBefore);
	if (!(this->mHtmlElementDisplayFlags & HELEM_DISPLAY_FLAG_STYLE_SET))
	{
		SetElementStyle();
	}
}

WEBC_BOOL HTMLRadioButtonDisplay::SetElementStyle (void)
{
	mHtmlElementDisplayFlags |= HELEM_DISPLAY_FLAG_STYLE_SET;

	if (mpHtmlElement->mFlags & HINPUT_FLAG_DISABLED)
	{
		Disable();
	}
	else
	{
		Enable();
	}

	return (WEBC_TRUE);
}
