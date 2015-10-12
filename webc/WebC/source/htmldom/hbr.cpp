//
// HBR.CPP - Methods for HTML DOM class HTMLBr
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
#include "hbr.hpp"
#include "webcstr.h"


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
// HTMLBr Constructors
/*****************************************************************************/

HTMLBr::HTMLBr(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLElement(td, pDocument)
{
	mClear = CSS_CLEAR_NONE;
}


/*****************************************************************************/
// HTMLBr Destructor
/*****************************************************************************/

HTMLBr::~HTMLBr(void)
{
}


/*****************************************************************************/
// TextUnit Interface
/*****************************************************************************/

WEBC_UINT16 HTMLBr::GetUnitType(void)
{
	return (TU_TYPE_BREAK);
}

WEBC_UINT16 HTMLBr::GetBreak(void)
{
	return (TU_BREAK_AFTER);
}


/******************************************************************************
* DISPLAY_INT HTMLBr::GetPixelHeight() -
*
* Parameters
*    none
*
* Description - This method returns the height of this element.  This method
*               returns 0 unless it is overridden.
*
*
* Returns (DISPLAY_INT)
*	the height
******************************************************************************/
DISPLAY_INT HTMLBr::GetPixelHeight(void)
{
	WebFont font = GetWebFont();
	return (font? WEB_FONT_BASELINE(font) : 0);
}

DISPLAY_INT HTMLBr::GetBaseline(void)
{
	WebFont font = GetWebFont();
	return (font? WEB_FONT_BASELINE(font) : 0);
}

/*****************************************************************************/
// TextUnit Helper Methods
/*****************************************************************************/



/*****************************************************************************/
// Override-able HTMLElement Methods
/*****************************************************************************/

/*
void HTMLBr::Update(Observable o, WEBC_PFBYTE data)
{
}
*/

/*
void HTMLBr::Refresh(PegThing *pThing)
{
}
*/
/*
int HTMLBr::DefaultEvent(HTMLEvent *pEvent)
{
    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	return (0);
}
*/
/*
HTMLElement *HTMLBr::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

/*
void HTMLBr::Reset(void)
{
}
*/

int HTMLBr::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		case HTML_ATTRIB_CLEAR:
			if (!webc_c_stricmp(value, "LEFT"))
			{
				mClear = CSS_CLEAR_LEFT;
			}
			else if (!webc_c_stricmp(value, "RIGHT"))
			{
				mClear = CSS_CLEAR_RIGHT;
			}
			else if (!webc_c_stricmp(value, "ALL"))
			{
				mClear = CSS_CLEAR_BOTH;
			}
			else
			{
				mClear = CSS_CLEAR_NONE;
			}
			break;

		default:
			break;
	}

	if (call_base)
	{
		return (HTMLElement::SetProperty(Property, value, pDocument, call_base));
	}

	return (0);
}


CSSValueType HTMLBr::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{
	switch (Property)
	{
		case CSS_PROPERTY_CLEAR:
			pValue->clear = (CSSClear) mClear;
			return (CSS_VALUE_SPECIFIED);

		default:
			break;
	}

	return (CSS_VALUE_UNKNOWN);
}


/*****************************************************************************/
// HTMLBr-Specific Methods
/*****************************************************************************/
