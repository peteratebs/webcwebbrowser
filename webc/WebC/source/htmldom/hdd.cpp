//
// HDD.CPP - Methods for HTML DOM class HTMLDefinition
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
#include "hdd.hpp"

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
// HTMLDefinition Constructors
/*****************************************************************************/

HTMLDefinition::HTMLDefinition(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLFlow(td, pDocument)
{
}


/*****************************************************************************/
// HTMLDefinition Destructor
/*****************************************************************************/

HTMLDefinition::~HTMLDefinition(void)
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
void HTMLDefinition::Update(Observable o, WEBC_PFBYTE data)
{
}
*/

/*
void HTMLDefinition::Refresh(PegThing *pThing)
{
}
*/
/*
int HTMLDefinition::DefaultEvent(HTMLEvent *pEvent)
{
    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	return (0);
}
*/
/*
HTMLDocument *HTMLDefinition::GetDocument(void)
{
	return (0);
}
*/
/*
HTMLElement *HTMLDefinition::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

/*
void HTMLDefinition::Reset(void)
{
}
*/
/*
int HTMLDefinition::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
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
/*
int HTMLDefinition::GetProperty(HTMLAttributeType Property, HTMLProperty *pValue)
{
	switch (Property)
	{
		default:
			break;
	}

	return (HTMLFlow::GetProperty(Property, pValue));
}
*/

/******************************************************************************
* HTMLDefinition::GetStyleFromHTML() -
*
* Parameters
*
* Description
*
* Returns ()
*
******************************************************************************/

CSSValueType HTMLDefinition::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
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
// HTMLDefinition-Specific Methods
/*****************************************************************************/
