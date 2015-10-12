//
// HBASE.CPP - Methods for HTML DOM class HTMLBase
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

#include "htmldom.hpp"
#include "hbase.hpp"
#include "htmldoc.hpp"
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
// HTMLBase Constructors
/*****************************************************************************/

HTMLBase::HTMLBase(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLElement(td, pDocument)
{
	mpHRef = 0;
	mpTarget = 0;
}


/*****************************************************************************/
// HTMLBase Destructor
/*****************************************************************************/

HTMLBase::~HTMLBase(void)
{
	webc_free_string_copy(mpHRef, __FILE__, __LINE__);
	webc_free_string_copy(mpTarget, __FILE__, __LINE__);
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
void HTMLBase::Update(Observable o, WEBC_PFBYTE data)
{
}
*/

/*
void HTMLBase::Refresh(PegThing *pThing)
{
}
*/
/*
int HTMLBase::DefaultEvent(HTMLEvent *pEvent)
{
    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	return (0);
}
*/
/*
HTMLDocument *HTMLBase::GetDocument(void)
{
	return (0);
}
*/
/*
HTMLElement *HTMLBase::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

/*
void HTMLBase::Reset(void)
{
}
*/

int HTMLBase::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		case HTML_ATTRIB_HREF:
			SetHRef(value);
			return (1);

		case HTML_ATTRIB_TARGET:
			SetTarget(value);
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

/******************************************************************************
* void HTMLBase::SetHRef(WEBC_CHAR *pHRef) - Set href attribute
*
* Parameters
*    pHRef - NULL-terminated string: the new value of href
*
* Description
*    Creates a local copy of the given string.
*
* Returns (void)
*    Nothing
******************************************************************************/

void HTMLBase::SetHRef(WEBC_CHAR *pHRef)
{
WEBC_CHAR *pStr = webc_malloc_string_copy(pHRef, __FILE__, __LINE__);
	webc_free_string_copy(mpHRef, __FILE__, __LINE__);
	mpHRef = pStr;
}

/******************************************************************************
* void HTMLBase::SetTarget(WEBC_CHAR *pTarget) - Set target attribute
*
* Parameters
*    pName - NULL-terminated string: the new value of target
*
* Description
*    Creates a local copy of the given string.
*
* Returns (void)
*    Nothing
******************************************************************************/

void HTMLBase::SetTarget(WEBC_CHAR *pName)
{
WEBC_CHAR *pStr = webc_malloc_string_copy(pName, __FILE__, __LINE__);
	webc_free_string_copy(mpTarget, __FILE__, __LINE__);
	mpTarget = pStr;
}

/******************************************************************************
* WEBC_CHAR* HTMLBase::HRef() - Access href attribute
*
* Parameters
*
* Description
*
* Returns (WEBC_CHAR*)
*    NULL-terminated string; the anchor's href
******************************************************************************/

WEBC_CHAR *HTMLBase::HRef(void)
{
	return (mpHRef);
}

/******************************************************************************
* WEBC_CHAR* HTMLBase::Target() - Access target attribute
*
* Parameters
*
* Description
*
* Returns (WEBC_CHAR*)
*    NULL-terminated string; the anchor's target
******************************************************************************/

WEBC_CHAR *HTMLBase::Target(void)
{
	return (mpTarget);
}


/*****************************************************************************/
// HTMLBase-Specific Methods
/*****************************************************************************/
