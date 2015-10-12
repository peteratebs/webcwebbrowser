//
// DHCHECK.CPP - Methods for class HTMLCheckBoxDisplay
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

#include "dhcheck.hpp"
#include "hchkbox.hpp"
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

HTMLCheckBoxDisplay::HTMLCheckBoxDisplay (HTMLElement *pElem)
	: HTMLElementDisplay<WebCheckBox> (pElem)
{
}

HTMLCheckBoxDisplay::~HTMLCheckBoxDisplay ()
{
}

DISPLAY_INT HTMLCheckBoxDisplay::GetBaseline (void)
{
	return Height() - 3;
}

void HTMLCheckBoxDisplay::SetParentDisplay(DisplayElement *parent, DisplayElement* insertBefore)
{
	HTMLElementDisplay<WebCheckBox>::SetParentDisplay(parent, insertBefore);
	if (!(this->mHtmlElementDisplayFlags & HELEM_DISPLAY_FLAG_STYLE_SET))
	{
		SetElementStyle();
	}
}

WEBC_BOOL HTMLCheckBoxDisplay::SetElementStyle (void)
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
