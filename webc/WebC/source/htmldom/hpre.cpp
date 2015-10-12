//
// HPRE.CPP - Methods for HTML DOM class HTMLPreformatted
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
#include "hpre.hpp"

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
// HTMLPreformatted Constructors
/*****************************************************************************/

HTMLPreformatted::HTMLPreformatted(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLFlow(td, pDocument)
{
}


/*****************************************************************************/
// HTMLPreformatted Destructor
/*****************************************************************************/

HTMLPreformatted::~HTMLPreformatted(void)
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
void HTMLPreformatted::Update(Observable o, WEBC_PFBYTE data)
{
}
*/

/*
void HTMLPreformatted::Refresh(PegThing *pThing)
{
}
*/
/*
int HTMLPreformatted::DefaultEvent(HTMLEvent *pEvent)
{
    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	return (0);
}
*/
/*
HTMLElement *HTMLPreformatted::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

/*
void HTMLPreformatted::Reset(void)
{
}
*/
/*
int HTMLPreformatted::SetProperty(HTMLAttributeType Property, PFCHAR value, HTMLDocument *pDocument, int call_base)
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

CSSValueType HTMLPreformatted::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{
	switch (Property)
	{
		case CSS_PROPERTY_WHITE_SPACE:
			pValue->whitespace = CSS_WHITE_SPACE_PRE;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_FONT_FAMILY:
			pValue->name = webc_GetDefaultString(WEBC_DEFAULT_MONOSPACE_FONT_FAMILY);
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_FONT_STYLE:
			pValue->fontStyle = webc_GetDefaultFontStyle(WEBC_DEFAULT_MONOSPACE_FONT_STYLE);
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_FONT_VARIANT:
			break;

		case CSS_PROPERTY_FONT_WEIGHT:
			pValue->fontWeight =  webc_GetDefaultFontWeight(WEBC_DEFAULT_MONOSPACE_FONT_WEIGHT);
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_FONT_SIZE:
			pValue->fontSize.type = CSS_FONT_SIZE_ABSOLUTE;
			pValue->fontSize.val.absolute =  webc_GetDefaultUint16(WEBC_DEFAULT_MONOSPACE_FONT_SIZE); 
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
// HTMLPreformatted-Specific Methods
/*****************************************************************************/
