/*
|  HLABEL.CPP -
| 
|  EBS - WebC
| 
| 
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

/*****************************************************************************/
/* Header files
 *****************************************************************************/

#include "hlabel.hpp"
#include "htmldoc.hpp"
#include "htmlfind.hpp"
#include "objref.cpp"

/*****************************************************************************/
/* Macros
 *****************************************************************************/

//#define WEBC_CFG_DRAW_LABEL_FOCUS_BOX

/*****************************************************************************/
/* Types
 *****************************************************************************/

/*****************************************************************************/
/* Function Prototypes
 *****************************************************************************/

/*****************************************************************************/
/* Data
 *****************************************************************************/

/*****************************************************************************/
/* Function Definitions
 *****************************************************************************/

/*---------------------------------------------------------------------------*/
HTMLLabel::HTMLLabel (HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLSpan (td, pDocument)
{
	mFor = 0;
}

/*---------------------------------------------------------------------------*/
HTMLLabel::~HTMLLabel (void)
{
	if (mFor)
	{
		webc_free_string_copy(mFor, __FILE__, __LINE__);
	}
}

/*---------------------------------------------------------------------------*/
int HTMLLabel::SetProperty (HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		case HTML_ATTRIB_FOR:
		{
			WEBC_CHAR* s = webc_malloc_string_copy(value, __FILE__, __LINE__);
			webc_free_string_copy(mFor, __FILE__, __LINE__);
			mFor = s;
			return (1);
		}

		default:
			break;
	}

	if (call_base)
	{
		return (HTMLSpan::SetProperty(Property, value, pDocument, call_base));
	}

	return (0);	
}

/*---------------------------------------------------------------------------*/
HTMLElement* HTMLLabel::EventParent (void)
{
	HTMLElement* p = ForElement();
	
	return p? p : mpParent;
}

/*---------------------------------------------------------------------------*/
HTMLElement* HTMLLabel::ForElement (void)
{
	if (mFor && !mForElement.Get())
	{
		HTMLDocument* doc = GetDocument();
		
		if (doc)
		{
			HTMLNameOrIdFinder f(mFor);			
			mForElement.Set(doc->FindElement(&f));
		}
	}
	
	return (mForElement.Get());	
}

/*---------------------------------------------------------------------------*/
CSSValueType HTMLLabel::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{
  #ifdef WEBC_CFG_DRAW_LABEL_FOCUS_BOX
	HTMLElement* p = ForElement();	
	WEBC_BOOL forElementHasFocus = p? ((p->mFlags & HELEM_FLAG_HAS_FOCUS) == HELEM_FLAG_HAS_FOCUS) : WEBC_FALSE;
	
	switch (Property)
	{
		case CSS_PROPERTY_OUTLINE_STYLE:
			if (forElementHasFocus)
			{
	  			pValue->borderStyle = CSS_BORDER_STYLE_DOTTED;
	  		}
	  		else
	  		{
	  			pValue->borderStyle = CSS_BORDER_STYLE_NONE;
		  	}			  	
		  	return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_OUTLINE_COLOR:
			HTML_ColorSet(pValue->color, 0, 0, 0, 0);
		  	return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_OUTLINE_WIDTH:
			pValue->borderThickness.type = CSS_BORDER_THICKNESS_LENGTH;
			pValue->borderThickness.val.length.type = CSS_LENGTH_UNIT_PX;
			pValue->borderThickness.val.length.val.dec = 1;
			pValue->borderThickness.val.length.val.frac = 0;
		  	return (CSS_VALUE_SPECIFIED);

		default:
			break;
	}
  #endif // WEBC_CFG_DRAW_LABEL_FOCUS_BOX
	
	return (HTMLSpan::GetStyleFromHTML(Property, pValue));
}


