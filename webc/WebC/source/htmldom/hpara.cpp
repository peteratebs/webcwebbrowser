//
// HPARA.CPP - Methods for HTML DOM class HTMLParagraph
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
#include "hpara.hpp"

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
// HTMLParagraph Constructors
/*****************************************************************************/

HTMLParagraph::HTMLParagraph(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLFlow(td, pDocument)
{
	mPAlign = HTML_ALIGN_NONE;
}


/*****************************************************************************/
// HTMLParagraph Destructor
/*****************************************************************************/

HTMLParagraph::~HTMLParagraph(void)
{
}


/*****************************************************************************/
// TextUnit Interface
/*****************************************************************************/

/******************************************************************************
* WEBC_UINT16 HTMLParagraph::GetBreak() -
*
* Parameters
*    none
*
* Description - returns TextUnit line break behavior (see textunit.hpp)
*
* Returns (WEBC_UINT8)
*	line break type
******************************************************************************/
WEBC_UINT16 HTMLParagraph::GetBreak(void)
{
	return (TU_BREAK_BOTH);
}


/*****************************************************************************/
// TextUnit Helper Methods
/*****************************************************************************/


/*****************************************************************************/
// Override-able HTMLElement Methods
/*****************************************************************************/

/*
void HTMLParagraph::Update(Observable o, WEBC_PFBYTE data)
{
}
*/

/*
void HTMLParagraph::Refresh(PegThing *pThing)
{
}
*/
/*
int HTMLParagraph::DefaultEvent(HTMLEvent *pEvent)
{
    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	return (0);
}
*/
/*
HTMLDocument *HTMLParagraph::GetDocument(void)
{
	return (0);
}
*/
/*
HTMLElement *HTMLParagraph::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

/*
void HTMLParagraph::Reset(void)
{
}
*/

int HTMLParagraph::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		case HTML_ATTRIB_ALIGN:
			mPAlign = HTML_ParseAlignType(value, 0);
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

void HTMLParagraph::SetHtmlAlign(WEBC_UINT8 bAlign)
{
	mPAlign = bAlign;
}

CSSValueType HTMLParagraph::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{
	switch (Property)
	{
		case CSS_PROPERTY_DISPLAY:
			pValue->display = CSS_DISPLAY_BLOCK;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_TEXT_ALIGN:
			switch (mPAlign)
			{
				case HTML_ALIGN_CENTER:
					pValue->textAlign = CSS_TEXT_ALIGN_CENTER;
					return (CSS_VALUE_SPECIFIED);

				case HTML_ALIGN_RIGHT:
					pValue->textAlign = CSS_TEXT_ALIGN_RIGHT;
					return (CSS_VALUE_SPECIFIED);

				case HTML_ALIGN_JUSTIFY:
					pValue->textAlign = CSS_TEXT_ALIGN_JUSTIFY;
					return (CSS_VALUE_SPECIFIED);

				case HTML_ALIGN_NONE:
					break;

				case HTML_ALIGN_LEFT:
				default:
					pValue->textAlign = CSS_TEXT_ALIGN_LEFT;
					return (CSS_VALUE_SPECIFIED);
			}
			break;

		case CSS_PROPERTY_MARGIN_BOTTOM:
			pValue->length.type = CSS_LENGTH_UNIT_EM;
			pValue->length.val.dec = 1;
			pValue->length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		default:
			break;
	}

	return (CSS_VALUE_UNKNOWN);
}


/*****************************************************************************/
// HTMLParagraph-Specific Methods
/*****************************************************************************/

DISPLAY_INT HTMLParagraph::GetParagraphSpace(void)
{
	CSSPropertyValue unused;

	if (GetStyleFromCSS(CSS_PROPERTY_MARGIN_TOP, &unused) != CSS_VALUE_SPECIFIED)
	{
		WebFont font = GetWebFont();
		return ((font != WEB_FONT_NULL)? WEB_FONT_HEIGHT(font) : WEBC_CFG_DEFAULT_TEXT_HEIGHT);
	}

	return 0;
}
