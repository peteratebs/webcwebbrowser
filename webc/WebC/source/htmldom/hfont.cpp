//
// HFONT.CPP - Methods for HTML DOM class HTMLFont
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
#include "hfont.hpp"
#include "webcstr.h"


/*****************************************************************************/
// External declarations
/*****************************************************************************/

extern "C" int _StringListItemCompare(void *item, void *list, int index);


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
// HTMLFont Constructors
/*****************************************************************************/

HTMLFont::HTMLFont(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLFlow(td, pDocument)
{
	mpFontFace = 0;
	mFontSize.type = CSS_FONT_SIZE_NONE;
	mFontColor = HTML_RGBAToColor(0,0,0,255); // transparent; use parent's color
}


/*****************************************************************************/
// HTMLFont Destructor
/*****************************************************************************/

HTMLFont::~HTMLFont(void)
{
	if (mpFontFace)
	{
		webc_free_string_copy(mpFontFace, __FILE__, __LINE__);
	}
}

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

int HTMLFont::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}
	int val;
	switch (Property)
	{
		case HTML_ATTRIB_FACE:
			webc_free_string_copy(mpFontFace, __FILE__, __LINE__);
			mpFontFace = webc_malloc_string_copy(value, __FILE__, __LINE__);
			return (1);

		case HTML_ATTRIB_SIZE:
	  		mFontSize.type = CSS_FONT_SIZE_ABSOLUTE;
			val = ebs_LinearSearch(value, gpFontSizeValues, CSS_NUM_FONT_SIZE_VALUES, _StringListItemCompare);
			if (val >= 0)
			{
				val++;
			}
			else
			{
				val = webc_atoi(value);
			}

			if (val < -2)
			{
				val = -2;
			}

	  		if ((*value=='+') || (*value=='-'))
	  		{
	  			mFontSize.val.absolute = CSS_FONT_SIZE_MEDIUM + val;
	  		}
	  		else
	  		{
	  			mFontSize.val.absolute = val-1;
	  		}
			mFontSize.val.absolute = EBSCLIP(mFontSize.val.absolute, CSS_FONT_SIZE_XXSMALL, CSS_FONT_SIZE_XXLARGE);
			return (1);

		case HTML_ATTRIB_COLOR:
			HTML_ParseColor(value, 0, &mFontColor);
			return (1);

		default:
			break;
	}

	if (call_base)
	{
		return (HTMLFlow::SetProperty(Property, value, pDocument, call_base));
	}

	return (0);
}


CSSValueType HTMLFont::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{
	switch (Property)
	{
		case CSS_PROPERTY_FONT_SIZE:
			if (mFontSize.type == CSS_FONT_SIZE_NONE)
			{
				break;
			}
			pValue->fontSize = mFontSize;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_FONT_FAMILY:
			if (!mpFontFace)
			{
				break;
			}
			pValue->name = mpFontFace;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_COLOR:
			if (HTML_ColorGetAlpha(mFontColor) == 255)
			{
				break;
			}
			pValue->color = mFontColor;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_DISPLAY:
  			pValue->display = CSS_DISPLAY_INLINE; // font tag isn't a block element
			return (CSS_VALUE_SPECIFIED);

		default:
			break;
	}

	return (CSS_VALUE_UNKNOWN);
}


/*****************************************************************************/
// HTMLFont-Specific Methods
/*****************************************************************************/
