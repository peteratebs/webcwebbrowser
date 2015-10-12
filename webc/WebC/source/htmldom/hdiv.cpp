//
// HDIV.CPP - Methods for HTML DOM class HTMLDiv
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
#include "hdiv.hpp"

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
// HTMLDiv Constructors
/*****************************************************************************/

HTMLDiv::HTMLDiv(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLFlow(td, pDocument)
{
	mDivAlign = HTML_ALIGN_NONE;
}

HTMLDiv::HTMLDiv(HTMLDiv &copy)
	:HTMLFlow(copy)
{
	mDivAlign = copy.mDivAlign;
}

/*****************************************************************************/
// HTMLDiv Destructor
/*****************************************************************************/

HTMLDiv::~HTMLDiv(void)
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
void HTMLDiv::Update(Observable o, WEBC_PFBYTE data)
{
}
*/

/*
void HTMLDiv::Refresh(PegThing *pThing)
{
}
*/
/*
int HTMLDiv::DefaultEvent(HTMLEvent *pEvent)
{
    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	return (0);
}
*/
/*
HTMLDocument *HTMLDiv::GetDocument(void)
{
	return (0);
}
*/
/*
HTMLElement *HTMLDiv::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

/*
void HTMLDiv::Reset(void)
{
}
*/

int HTMLDiv::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		case HTML_ATTRIB_ALIGN:
			SetHtmlAlign(HTML_ParseAlignType(value, 0));
			return (1);

		default:
			break;
	}

	if (call_base)
	{
		return (HTMLFlow::SetProperty(Property, value, pDocument, call_base));
	}

	return (0);
}

void HTMLDiv::SetHtmlAlign(WEBC_UINT8 bAlign)
{
	SetFlag(HDIV_FLAG_ALIGN_SET);
	mDivAlign = bAlign;
}

CSSValueType HTMLDiv::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{
	switch (Property)
	{

		case CSS_PROPERTY_TEXT_ALIGN:
			if (mFlags & HDIV_FLAG_ALIGN_SET)
			{
				switch (mDivAlign)
				{
					case HTML_ALIGN_CENTER:
						pValue->textAlign = CSS_TEXT_ALIGN_CENTER;
						break;

					case HTML_ALIGN_RIGHT:
						pValue->textAlign = CSS_TEXT_ALIGN_RIGHT;
						break;

					case HTML_ALIGN_JUSTIFY:
						pValue->textAlign = CSS_TEXT_ALIGN_JUSTIFY;
						break;

					case HTML_ALIGN_LEFT:
					default:
						pValue->textAlign = CSS_TEXT_ALIGN_LEFT;
						break;
				}
				return (CSS_VALUE_SPECIFIED);
			}
			break;

		default:
			break;
	}

	return (CSS_VALUE_UNKNOWN);
}


/*****************************************************************************/
// HTMLDiv-Specific Methods
/*****************************************************************************/
