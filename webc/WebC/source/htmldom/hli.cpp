//
// HLI.CPP - Methods for HTML DOM class HTMLListItem
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
#include "dhli.hpp"
#include "hlist.hpp"
#include "tuenum.hpp"
#include "strbld.hpp"
#include "wcconst.hpp"
#include "webcstr.h"
#include "htmlfind.hpp"
#include "webcmem.h"

CSSListStyleType _atolisttype(WEBC_CHAR *type);


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
// HTMLListItem Constructors
/*****************************************************************************/

HTMLListItem::HTMLListItem(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLDiv(td, pDocument)
{
	mListStyleType = CSS_LIST_STYLE_NONE;
	miValue = 1;
	mpLIDisplay = 0;
}


/*****************************************************************************/
// HTMLListItem Destructor
/*****************************************************************************/

HTMLListItem::~HTMLListItem()
{
	if (mpLIDisplay)
	{
		WEBC_DELETE(mpLIDisplay);
	}
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


/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

int HTMLListItem::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
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
			
		case HTML_ATTRIB_VALUE:
		{
			long i = webc_atoi(value);
			SetListOrder(UTIL_SAFE_LONG_TO_SHORT(i));
			return (1);
		}
			
		default:
			break;
	}

	if (call_base)
	{
		return (HTMLDiv::SetProperty(Property, value, pDocument, call_base));
	}

	return (0);
}

CSSValueType HTMLListItem::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{
	switch (Property)
	{
		case CSS_PROPERTY_LIST_STYLE_TYPE:
			if (mListStyleType == CSS_LIST_STYLE_NONE)
			{
				HTMLList *parent = ParentList();
				if (parent)
				{
					pValue->listStyleType = parent->GetListStyleType();
					return (CSS_VALUE_SPECIFIED);
				}
			}
			else
			{
				pValue->listStyleType = mListStyleType;
				return (CSS_VALUE_SPECIFIED);
			}
			break;

		case CSS_PROPERTY_LIST_STYLE_POSITION:
		{
			HTMLList *parent = ParentList();
			if (parent)
			{
				pValue->listStylePosition = parent->GetListStylePosition();
			}
			else
			{
				pValue->listStylePosition = CSS_LIST_STYLE_POSITION_INSIDE;
			}
			return (CSS_VALUE_SPECIFIED);			
		}
			
		default:
			break;
	}
	
	return (CSS_VALUE_UNKNOWN);
}

/*****************************************************************************/
// HTMLListItem-Specific Methods
/*****************************************************************************/
void HTMLListItem::SetListStyleType(CSSListStyleType styleType)
{
	mListStyleType = styleType;
}

void HTMLListItem::SetListOrder(short iValue)
{
	miValue = iValue;
}

short HTMLListItem::ListOrder(void)
{
	return (miValue);
}

DisplayElement *HTMLListItem::GetDisplayElement (WEBC_BOOL create)
{
	return (GetLIDisplay(create));
}

TextUnit *HTMLListItem::GetTextUnit (void)
{
	return (GetLIDisplay());
}

HTMLListItemDisplay *HTMLListItem::GetLIDisplay (WEBC_BOOL create)
{
	if (!mpLIDisplay && create)
	{
		WEBC_NEW_VERBOSE(mpLIDisplay, HTMLListItemDisplay(this),"HTMLListItemDisplay");
	}
	
	return (mpLIDisplay);
}

HTMLList *HTMLListItem::ParentList (void)
{
	HTMLParentListFinder f;		
	FindParent(&f);
	return (f.Found());
}
