//
// HBLQUOTE.CPP - Methods for HTML DOM class HTMLBlockquote
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
#include "hblquote.hpp"

/*****************************************************************************/
// Local Constants
/*****************************************************************************/

/*****************************************************************************/
// Local type definitions
/*****************************************************************************/

/*****************************************************************************/
// Local Functions
/*****************************************************************************/

/*****************************************************************************/
// Global Data
/*****************************************************************************/

/*****************************************************************************/
// HTMLBlockquote Constructors
/*****************************************************************************/

HTMLBlockquote::HTMLBlockquote(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLParagraph(td, pDocument)
{
}


/*****************************************************************************/
// HTMLBlockquote Destructor
/*****************************************************************************/

HTMLBlockquote::~HTMLBlockquote(void)
{
}


/*****************************************************************************/
// TextUnit Interface
/*****************************************************************************/


/*****************************************************************************/
// TextUnit Helper Methods
/*****************************************************************************/


/*****************************************************************************/
// Override-able HTMLElement Methods
/*****************************************************************************/

/*
void HTMLBlockquote::Update(Observable o, WEBC_PFBYTE data)
{
}
*/

/*
void HTMLBlockquote::Refresh(PegThing *pThing)
{
}
*/
/*
int HTMLBlockquote::DefaultEvent(HTMLEvent *pEvent)
{
    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	return (0);
}
*/
/*
HTMLDocument *HTMLBlockquote::GetDocument(void)
{
	return (0);
}
*/
/*
HTMLElement *HTMLBlockquote::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

/*
void HTMLBlockquote::Reset(void)
{
}
*/

int HTMLBlockquote::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}
/*
	switch (Property)
	{
		default:
			break;
	}
*/
	if (call_base)
	{
		return (HTMLParagraph::SetProperty(Property, value, pDocument, call_base));
	}

	return (0);
}


/******************************************************************************
* HTMLBlockquote::GetStyleFromHTML() -
*
* Parameters
*
* Description
*
* Returns ()
*
******************************************************************************/

CSSValueType HTMLBlockquote::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{
	switch (Property)
	{
		case CSS_PROPERTY_MARGIN_LEFT:
			pValue->length.type = CSS_LENGTH_UNIT_PX;
			pValue->length.val.dec = 40;
			pValue->length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		default:
			break;
	}

	return (CSS_VALUE_UNKNOWN);
}

/*****************************************************************************/
// HTMLBlockquote-Specific Methods
/*****************************************************************************/
