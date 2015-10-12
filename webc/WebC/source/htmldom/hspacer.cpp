//
// HSPACER.CPP - Methods for HTML DOM class HTMLSpacer
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//  [tbd]
//

/*****************************************************************************/
// Header files
/*****************************************************************************/

#include "htmldoc.hpp"
#include "hspacer.hpp"
#include "webcstr.h"


/*****************************************************************************/
// HTMLSpacer Constructors
/*****************************************************************************/

HTMLSpacer::HTMLSpacer(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLElement(td, pDocument)
{
	mwWidth = 0;
	mwHeight = 0;
}


/*****************************************************************************/
// HTMLSpacer Destructor
/*****************************************************************************/

HTMLSpacer::~HTMLSpacer(void)
{
}


/*****************************************************************************/
// TextUnit Interface
/*****************************************************************************/

DISPLAY_INT HTMLSpacer::GetPixelWidth(void)
{
	return mwWidth;
}


DISPLAY_INT HTMLSpacer::GetPixelHeight(void)
{
	return mwHeight;
}


/*****************************************************************************/
// TextUnit Helper Methods
/*****************************************************************************/


/*****************************************************************************/
// Override-able HTMLElement Methods
/*****************************************************************************/

/*
void HTMLSpacer::Update(Observable o, WEBC_PFBYTE data)
{
}
*/

/*
void HTMLSpacer::Refresh(PegThing *pThing)
{
}
*/
/*
int HTMLSpacer::DefaultEvent(HTMLEvent *pEvent)
{
    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	return (0);
}
*/
/*
HTMLDocument *HTMLSpacer::GetDocument(void)
{
	return (0);
}
*/
/*
HTMLElement *HTMLSpacer::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

/*
void HTMLSpacer::Reset(void)
{
}
*/

int HTMLSpacer::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		case HTML_ATTRIB_WIDTH:
			if (value)
			{
				SetWidth(webc_atoi(value));
			}
			return (1);

		case HTML_ATTRIB_HEIGHT:
			if (value)
			{
				SetHeight(webc_atoi(value));
			}
			return (1);

		default:
			break;
	}

	if (call_base)
	{
		return (HTMLElement::SetProperty(Property, value, pDocument, call_base));
	}

	return (0);
}


void HTMLSpacer::SetWidth(int iWidth)
{
	mwWidth = iWidth;
}


void HTMLSpacer::SetHeight(int iHeight)
{
	mwHeight = iHeight;
}


/*****************************************************************************/
// HTMLSpacer-Specific Methods
/*****************************************************************************/
