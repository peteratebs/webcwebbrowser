//
// HLIST.CPP - Methods for HTML DOM class HTMLList
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

#include "hli.hpp"
#include "hlist.hpp"
#include "tuenum.hpp"
#include "strbld.hpp"
#include "wcconst.hpp"
#include "webcstr.h"
#include "webcdefault.h"

/*****************************************************************************/
// Local Constants 
/*****************************************************************************/

/*****************************************************************************/
// Local type definitions
/*****************************************************************************/

/*****************************************************************************/
// Local Functions
/*****************************************************************************/

CSSListStyleType _atolisttype(WEBC_CHAR *type);


/*****************************************************************************/
// Global Data
/*****************************************************************************/


/*****************************************************************************/
// HTMLList Constructors
/*****************************************************************************/

HTMLList::HTMLList(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLParagraph(td, pDocument)
{
	mListStyleType = CSS_LIST_STYLE_DISC;
	miStart = 1;
	miNesting = -1;
}


/*****************************************************************************/
// HTMLList Destructor
/*****************************************************************************/

HTMLList::~HTMLList()
{
}


/*****************************************************************************/
// HTMLList methods
/*****************************************************************************/

DISPLAY_INT HTMLList::GetParagraphSpace(void)
{
	if (Nesting() == 0)
	{
		return (HTMLParagraph::GetParagraphSpace());
	}
	
	return (0);
}

short HTMLList::Nesting(void)
{
	if (miNesting < 0)
	{
		FindNesting();
	}
	
	return (miNesting);
}

void HTMLList::FindNesting(void)
{
HTMLElement *pParent;
	
	miNesting = 0;
	
	pParent = mpParent;
	while (pParent)
	{
		if (pParent->Type() == HTML_LIST_ELEMENT)
		{
			miNesting++;
		}
		else if (pParent->Type() == HTML_TABLE_ELEMENT)
		{
			break;
		}
		pParent = pParent->mpParent;
	}
}

void HTMLList::SetNesting(short iNesting)
{
	miNesting = iNesting;
}

void HTMLList::SetStart(short iValue)
{
	miStart = iValue;
}

void HTMLList::SetListStyleType(CSSListStyleType styleType)
{
	mListStyleType = styleType;
}

short HTMLList::Start(void)
{
	return (miStart);
}

short HTMLList::Items(void)
{
HTMLElement *pChild = mpFirstChild;
int items = 0;

	while (pChild)
	{
		if (pChild->Type() == HTML_LIST_ITEM_ELEMENT)
		{
			items++;
		}
		pChild = pChild->mpNext;
	}
	
	return (items);
}

int HTMLList::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		case HTML_ATTRIB_TYPE:
			SetListStyleType(_atolisttype(value));
			return (1);
			
		case HTML_ATTRIB_START:
		{
			long i = webc_atoi(value);
			SetStart(UTIL_SAFE_LONG_TO_SHORT(i));
			return (1);
		}
			
		default:
			break;
	}

	if (call_base)
	{
		return (HTMLFlow::SetProperty(Property, value, pDocument, call_base));
	}

	return (0);
}

CSSValueType HTMLList::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{
	switch (Property)
	{
		case CSS_PROPERTY_PADDING_LEFT:
			pValue->length.type = CSS_LENGTH_UNIT_PX;
			pValue->length.val.dec = (short) WEBC_DEFAULT_LIST_INDENT;
			pValue->length.val.frac = 0;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_LIST_STYLE_TYPE:
			if (mListStyleType != CSS_LIST_STYLE_NONE)
			{
				pValue->listStyleType = mListStyleType;
				return (CSS_VALUE_SPECIFIED);
			}
			break;

		case CSS_PROPERTY_LIST_STYLE_POSITION:
			pValue->listStylePosition = CSS_LIST_STYLE_POSITION_OUTSIDE;
			return (CSS_VALUE_SPECIFIED);			

		case CSS_PROPERTY_MARGIN_BOTTOM:
			if (Nesting() == 0)
			{
				pValue->length.type = CSS_LENGTH_UNIT_EM;
				pValue->length.val.dec = 1;
				pValue->length.val.frac = 0;
				return (CSS_VALUE_SPECIFIED);
			}
			break;

		default:
			break;
	}
	
	return (CSS_VALUE_UNKNOWN);
}


/*****************************************************************************/
// Local function bodies
/*****************************************************************************/

CSSListStyleType _atolisttype(WEBC_CHAR *type) 
{
	switch (type[0])
	{
		case (WEBC_CHAR) 'a':
			if (type[1] == 0)
			{
				return (CSS_LIST_STYLE_LOWER_ALPHA);
			}
			break;

		case (WEBC_CHAR) 'A':
			if (type[1] == 0)
			{
				return (CSS_LIST_STYLE_UPPER_ALPHA);
			}
			break;

		case (WEBC_CHAR) 'i':
			if (type[1] == 0)
			{
				return (CSS_LIST_STYLE_LOWER_ROMAN);
			}
			break;

		case (WEBC_CHAR) 'I':
			if (type[1] == 0)
			{
				return (CSS_LIST_STYLE_UPPER_ROMAN);
			}
			break;

		case (WEBC_CHAR) '1':
			if (type[1] == 0)
			{
				return (CSS_LIST_STYLE_DECIMAL);
			}
			break;

		case (WEBC_CHAR) 'd':
		case (WEBC_CHAR) 'D':
			if (!webc_c_stricmp(type, "disc"))
			{
				return (CSS_LIST_STYLE_DISC);
			}
			break;

		case (WEBC_CHAR) 'c':
		case (WEBC_CHAR) 'C':
			if (!webc_c_stricmp(type, "circle"))
			{
				return (CSS_LIST_STYLE_CIRCLE);
			}
			break;

		case (WEBC_CHAR) 's':
		case (WEBC_CHAR) 'S':
			if (!webc_c_stricmp(type, "square"))
			{
				return (CSS_LIST_STYLE_SQUARE);
			}
			break;

		default:
			break;
	}
	
	return (CSS_LIST_STYLE_NONE);
}


/*****************************************************************************/
// HTMLOrderedList methods
/*****************************************************************************/

HTMLOrderedList::HTMLOrderedList(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLList (td, pDocument)
{
	SetListStyleType(CSS_LIST_STYLE_DECIMAL);
}

HTMLOrderedList::~HTMLOrderedList()
{
}



/*****************************************************************************/
// HTMLUnorderedList methods
/*****************************************************************************/

HTMLUnorderedList::HTMLUnorderedList(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLList (td, pDocument)
{
	SetListStyleType(CSS_LIST_STYLE_DISC);
}

HTMLUnorderedList::~HTMLUnorderedList()
{
}
