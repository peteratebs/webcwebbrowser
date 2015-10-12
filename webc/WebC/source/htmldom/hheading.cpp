//
// HHEADING.CPP - Methods for HTML DOM class HTMLHeading
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
#include "hheading.hpp"

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
// HTMLHeading Constructors
/*****************************************************************************/

HTMLHeading::HTMLHeading(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLParagraph(td, pDocument)
{
}


/*****************************************************************************/
// HTMLHeading Destructor
/*****************************************************************************/

HTMLHeading::~HTMLHeading(void)
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
void HTMLHeading::Update(Observable o, WEBC_PFBYTE data)
{
}
*/

/*
void HTMLHeading::Refresh(PegThing *pThing)
{
}
*/
/*
int HTMLHeading::DefaultEvent(HTMLEvent *pEvent)
{
    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	return (0);
}
*/
/*
HTMLDocument *HTMLHeading::GetDocument(void)
{
	return (0);
}
*/
/*
HTMLElement *HTMLHeading::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

/*
void HTMLHeading::Reset(void)
{
}
*/
/*
int HTMLHeading::SetProperty(HTMLAttributeType Property, PFCHAR value, HTMLDocument *pDocument, int call_base)
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

CSSValueType HTMLHeading::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{
	switch (Property)
	{
		case CSS_PROPERTY_FONT_WEIGHT:
			pValue->fontWeight = CSS_FONT_WEIGHT_BOLD;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_FONT_SIZE:
			pValue->fontSize.type = CSS_FONT_SIZE_ABSOLUTE;
			switch (mTag)
			{
				case HTML_TAG_H2:
					pValue->fontSize.val.absolute = CSS_FONT_SIZE_LARGE;
					break;

				case HTML_TAG_H3:
					pValue->fontSize.val.absolute = CSS_FONT_SIZE_MEDIUM;
					break;

				case HTML_TAG_H4:
					pValue->fontSize.val.absolute = CSS_FONT_SIZE_SMALL;
					break;

				case HTML_TAG_H5:
					pValue->fontSize.val.absolute = CSS_FONT_SIZE_XSMALL;
					break;

				case HTML_TAG_H6:
					pValue->fontSize.val.absolute = CSS_FONT_SIZE_XXSMALL;
					break;

				case HTML_TAG_H1:
				default:
					pValue->fontSize.val.absolute = CSS_FONT_SIZE_XLARGE;
					break;
			}
			return (CSS_VALUE_SPECIFIED);

		default:
			break;
	}

	return (HTMLParagraph::GetStyleFromHTML(Property, pValue));
}


/*****************************************************************************/
// HTMLHeading-Specific Methods
/*****************************************************************************/
