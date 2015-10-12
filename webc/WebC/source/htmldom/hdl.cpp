//
// HDL.CPP - Methods for HTML DOM class HTMLDefinitionList
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
#include "hdl.hpp"

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
// HTMLDefinitionList Constructors
/*****************************************************************************/

HTMLDefinitionList::HTMLDefinitionList(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLFlow(td, pDocument)
{
}


/*****************************************************************************/
// HTMLDefinitionList Destructor
/*****************************************************************************/

HTMLDefinitionList::~HTMLDefinitionList(void)
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
void HTMLDefinitionList::Update(Observable o, WEBC_PFBYTE data)
{
}
*/

/*
void HTMLDefinitionList::Refresh(PegThing *pThing)
{
}
*/
/*
int HTMLDefinitionList::DefaultEvent(HTMLEvent *pEvent)
{
    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	return (0);
}
*/
/*
HTMLDocument *HTMLDefinitionList::GetDocument(void)
{
	return (0);
}
*/
/*
HTMLElement *HTMLDefinitionList::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

/*
void HTMLDefinitionList::Reset(void)
{
}
*/
/*
int HTMLDefinitionList::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
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

/*****************************************************************************/
// HTMLDefinitionList-Specific Methods
/*****************************************************************************/
