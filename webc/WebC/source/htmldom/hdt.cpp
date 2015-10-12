//
// HDT.CPP - Methods for HTML DOM class HTMLDefinitionTerm
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
#include "hdt.hpp"

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
// HTMLDefinitionTerm Constructors
/*****************************************************************************/

HTMLDefinitionTerm::HTMLDefinitionTerm(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLFlow(td, pDocument)
{
}


/*****************************************************************************/
// HTMLDefinitionTerm Destructor
/*****************************************************************************/

HTMLDefinitionTerm::~HTMLDefinitionTerm(void)
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
void HTMLDefinitionTerm::Update(Observable o, WEBC_PFBYTE data)
{
}
*/

/*
void HTMLDefinitionTerm::Refresh(PegThing *pThing)
{
}
*/
/*
int HTMLDefinitionTerm::DefaultEvent(HTMLEvent *pEvent)
{
    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	return (0);
}
*/
/*
HTMLDocument *HTMLDefinitionTerm::GetDocument(void)
{
	return (0);
}
*/
/*
HTMLElement *HTMLDefinitionTerm::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

/*
void HTMLDefinitionTerm::Reset(void)
{
}
*/
/*
int HTMLDefinitionTerm::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
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
// HTMLDefinitionTerm-Specific Methods
/*****************************************************************************/
