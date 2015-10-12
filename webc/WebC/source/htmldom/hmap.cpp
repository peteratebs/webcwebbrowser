//
// HMAP.CPP - Methods for HTML DOM class HTMLMap
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
#include "hmap.hpp"
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
// HTMLMap Constructors
/*****************************************************************************/

HTMLMap::HTMLMap(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLElementContainer(td, pDocument)
{
	mpName = 0;
}


/*****************************************************************************/
// HTMLMap Destructor
/*****************************************************************************/

HTMLMap::~HTMLMap(void)
{
	webc_free_string_copy(mpName, __FILE__, __LINE__);
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
void HTMLMap::Update(Observable o, WEBC_PFBYTE data)
{
}
*/

/*
void HTMLMap::Refresh(PegThing *pThing)
{
}
*/
/*
int HTMLMap::DefaultEvent(HTMLEvent *pEvent)
{
    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	return (0);
}
*/
/*
HTMLDocument *HTMLMap::GetDocument(void)
{
	return (0);
}
*/
/*
HTMLElement *HTMLMap::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

/*
void HTMLMap::Reset(void)
{
}
*/

int HTMLMap::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		case HTML_ATTRIB_NAME:
			SetName(value);
			return (1);

		default:
			break;
	}

	if (call_base)
	{
		return (HTMLElement::SetProperty(Property, value, pDocument, call_base));
	}

	return (0);
}

void HTMLMap::SetName(const WEBC_CHAR *pName)
{
	WEBC_CHAR *pStr = webc_malloc_string_copy(pName, __FILE__, __LINE__);
	webc_free_string_copy(mpName, __FILE__, __LINE__);
	mpName = pStr;
}

WEBC_CHAR *HTMLMap::Name(void)
{
	return (mpName);
}


/*****************************************************************************/
// HTMLMap-Specific Methods
/*****************************************************************************/


HTMLArea *HTMLMap::GetArea(int x, int y)
{
HTMLArea *pArea;

	// find enclosing rectangle, if any
	pArea = (HTMLArea *) mpFirstChild;
	while (pArea)
	{
		if (pArea->Type() != HTML_AREA_ELEMENT)
		{
			pArea = (HTMLArea *) pArea->mpNext;
			continue;
		}

		if (pArea->Contains(x,y))
		{
			return (pArea);
		}

		pArea = (HTMLArea *) pArea->mpNext;
	}

	return (0);
}
