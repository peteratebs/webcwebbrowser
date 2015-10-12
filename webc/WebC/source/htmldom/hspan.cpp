//
// HSPAN.CPP - Methods for HTML DOM class HTMLSpan
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
#include "hspan.hpp"


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
// HTMLSpan Constructors
/*****************************************************************************/

HTMLSpan::HTMLSpan(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLFlow(td, pDocument)
{
//	SetFlag(TU_FLAG_INLINE);
}


/*****************************************************************************/
// HTMLSpan Destructor
/*****************************************************************************/

HTMLSpan::~HTMLSpan(void)
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
void HTMLSpan::Update(Observable o, WEBC_PFBYTE data)
{
}
*/

/*
void HTMLSpan::Refresh(PegThing *pThing)
{
}
*/
/*
int HTMLSpan::DefaultEvent(HTMLEvent *pEvent)
{
    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	return (0);
}
*/
/*
HTMLElement *HTMLSpan::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

/*
void HTMLSpan::Reset(void)
{
}
*/
/*
int HTMLSpan::SetProperty(HTMLAttributeType Property, PFCHAR value, HTMLDocument *pDocument, int call_base)
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

CSSValueType HTMLSpan::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{
	switch (Property)
	{
		case CSS_PROPERTY_DISPLAY:
			pValue->display =  CSS_DISPLAY_INLINE_BLOCK;
			return (CSS_VALUE_SPECIFIED);

		default:
			break;
	}

	return (CSS_VALUE_UNKNOWN);
}

WEBC_UINT16 HTMLSpan::GetBreak(void)
{
	return (TU_BREAK_NONE);
}



/*****************************************************************************/
// HTMLSpan-Specific Methods
/*****************************************************************************/
