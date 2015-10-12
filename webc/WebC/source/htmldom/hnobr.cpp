//
// HNOBR.CPP - Methods for HTML DOM class HTMLNoBr
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
#include "hnobr.hpp"

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
// HTMLNoBr Constructors
/*****************************************************************************/

HTMLNoBr::HTMLNoBr(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLFlow(td, pDocument)
{
//	SetWrap(0);
}


/*****************************************************************************/
// HTMLNoBr Destructor
/*****************************************************************************/

HTMLNoBr::~HTMLNoBr(void)
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
void HTMLNoBr::Update(Observable o, WEBC_PFBYTE data)
{
}
*/

/*
void HTMLNoBr::Refresh(PegThing *pThing)
{
}
*/
/*
int HTMLNoBr::DefaultEvent(HTMLEvent *pEvent)
{
    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	return (0);
}
*/
/*
HTMLElement *HTMLNoBr::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

/*
void HTMLNoBr::Reset(void)
{
}
*/
/*
int HTMLNoBr::SetProperty(HTMLAttributeType Property, PFCHAR value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		default:
			break;
	}

	if (call_base)
	{
		return (HTMLFlow::SetProperty(Property, value, pDocument, call_base));
	}

	return (0);
}
*/

CSSValueType HTMLNoBr::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{
	switch (Property)
	{
		case CSS_PROPERTY_WHITE_SPACE:
			pValue->whitespace = CSS_WHITE_SPACE_NOWRAP;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_DISPLAY:
			pValue->display = CSS_DISPLAY_INLINE;
			return (CSS_VALUE_SPECIFIED);

		default:
			break;
	}

	return (CSS_VALUE_UNKNOWN);
}


/*****************************************************************************/
// HTMLNoBr-Specific Methods
/*****************************************************************************/
