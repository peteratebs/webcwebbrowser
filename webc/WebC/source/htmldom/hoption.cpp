//
// HOPTION.CPP - Methods for HTML DOM class HTMLOption
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

#include "webcjhconfig.h"
#include "htmldoc.hpp"
#include "hoption.hpp"
#include "hselect.hpp"
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
// HTMLOption Constructors
/*****************************************************************************/

#if (WEBC_SUPPORT_JSCRIPT)
HTMLOption::HTMLOption(HTMLTagParse *td, HTMLDocument *pDocument, JSObject *pJSObj)
	: HTMLInput(td, pDocument, pJSObj)
#else
HTMLOption::HTMLOption(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLInput(td, pDocument)
#endif // WEBC_SUPPORT_JSCRIPT
{
	mpText = 0;
	mpValue = 0;
	mbDefaultSelected = 0;
}

HTMLOption::HTMLOption(HTMLOption &copy)
	: HTMLInput(copy)
{
	mpText = 0;
	SetText(copy.mpText);

	mpValue = 0;
	SetValue(copy.mpValue);

	mbDefaultSelected = copy.mbDefaultSelected;
}


/*****************************************************************************/
// HTMLOption Destructor
/*****************************************************************************/

HTMLOption::~HTMLOption(void)
{
	webc_free_string_copy(mpValue, __FILE__, __LINE__);
	webc_free_string_copy(mpText, __FILE__, __LINE__);
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
void HTMLOption::Refresh(PegThing *pThing)
{
}
*/
/*
int HTMLOption::DefaultEvent(HTMLEvent *pEvent)
{
    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	return (0);
}
*/
/*
HTMLDocument *HTMLOption::GetDocument(void)
{
	return (0);
}
*/
/*
HTMLElement *HTMLOption::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

/*
void HTMLOption::Reset(void)
{
}
*/

int HTMLOption::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		case HTML_ATTRIB_SELECTED:
			mbDefaultSelected = 1;
			break;

		case HTML_ATTRIB_VALUE:
			SetValue(value);
			return (1);

		default:
			break;
	}

	if (call_base)
	{
		return (HTMLInput::SetProperty(Property, value, pDocument, call_base));
	}

	return (0);
}


WEBC_CHAR *HTMLOption::Value(void)
{
	return (mpValue);
}

void HTMLOption::SetValue(const WEBC_CHAR *pValue)
{
	if(!pValue)
		return;

	WEBC_CHAR *pStr = webc_malloc_string_copy(pValue, __FILE__, __LINE__);
	webc_free_string_copy(mpValue, __FILE__, __LINE__);
	mpValue = pStr;
}


/*****************************************************************************/
// HTMLOption-Specific Methods
/*****************************************************************************/

void HTMLOption::SetText(WEBC_CHAR *pText)
{
	if(!pText)
		return;

	WEBC_CHAR *copy = webc_malloc_string_copy(pText, __FILE__, __LINE__);

	if (copy)
	{
		webc_free_string_copy(mpText, __FILE__, __LINE__);

		int n = webc_strlen(copy) - 1;

		for (;(copy[n] == ' ' || copy[n] == '\r' || copy[n] == '\n') && n>0; n--)
		{
			copy[n] = '\0';
		}

		for (n=0; copy[n] != '\0'; n++)
		{
			if (copy[n] == (WEBC_CHAR) HTML_NBSP_CHAR)
			{
				copy[n] = ' ';
			}
		}

		mpText = copy;
	}
}


WEBC_CHAR *HTMLOption::Text()
{
	return (mpText);
}

HTMLSelect *HTMLOption::GetSelectElement (void)
{
HTMLElement *parent = mpParent;

	while (parent)
	{
		if (parent->Type() == HTML_SELECT_ELEMENT)
		{
			return ((HTMLSelect *) parent);
		}
		parent = parent->mpParent;
	}

	return (0);
}

int HTMLOption::GetIndex(void)
{
HTMLSelect *pSelect = GetSelectElement();

	return (pSelect->GetIndexOfOption(this));
}

void HTMLOption::SetForm(HTMLForm* pForm)
{
	HTMLElement::SetForm(pForm);
}
