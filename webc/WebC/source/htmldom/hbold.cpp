//
// HBOLD.CPP - Methods for HTML DOM class HTMLBold
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

#include "hbold.hpp"
#include "htmldoc.hpp"

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
// HTMLBold Constructors
/*****************************************************************************/

HTMLBold::HTMLBold(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLFlow(td, pDocument)
{
//CSSPropertyValue value;

	//value.font_weight = CSS_FONT_WEIGHT_BOLD;
	//SetFontWeight(&value);
}


/*****************************************************************************/
// HTMLBold Destructor
/*****************************************************************************/

HTMLBold::~HTMLBold(void)
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
void HTMLBold::Update(Observable o, WEBC_PFBYTE data)
{
}
*/

/*
void HTMLBold::Refresh(PegThing *pThing)
{
}
*/
/*
int HTMLBold::DefaultEvent(HTMLEvent *pEvent)
{
    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	return (0);
}
*/
/*
HTMLElement *HTMLBold::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

/*
void HTMLBold::Reset(void)
{
}
*/
/*
int HTMLBold::SetProperty(HTMLAttributeType Property, HTMLchar *value, HTMLDocument *pDocument, int call_base)
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

CSSValueType HTMLBold::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{
	switch (Property)
	{
		case CSS_PROPERTY_FONT_WEIGHT:
			pValue->fontWeight = CSS_FONT_WEIGHT_BOLD;
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
// HTMLBold-Specific Methods
/*****************************************************************************/
