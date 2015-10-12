//
// HEMPHSZ.CPP - Methods for HTML DOM class HTMLEmphasize
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
#include "hemphsz.hpp"

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
// HTMLEmphasize Constructors
/*****************************************************************************/

HTMLEmphasize::HTMLEmphasize(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLFlow(td, pDocument)
{
}


/*****************************************************************************/
// HTMLEmphasize Destructor
/*****************************************************************************/

HTMLEmphasize::~HTMLEmphasize(void)
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
void HTMLEmphasize::Update(Observable o, WEBC_PFBYTE data)
{
}
*/

/*
void HTMLEmphasize::Refresh(PegThing *pThing)
{
}
*/
/*
int HTMLEmphasize::DefaultEvent(HTMLEvent *pEvent)
{
    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	return (0);
}
*/
/*
HTMLElement *HTMLEmphasize::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

/*
void HTMLEmphasize::Reset(void)
{
}
*/
/*
int HTMLEmphasize::SetProperty(HTMLAttributeType Property, PFCHAR value, HTMLDocument *pDocument, int call_base)
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

CSSValueType HTMLEmphasize::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{
	switch (Property)
	{
		case CSS_PROPERTY_FONT_STYLE:
			pValue->fontStyle = CSS_FONT_STYLE_ITALIC;
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
// HTMLEmphasize-Specific Methods
/*****************************************************************************/
