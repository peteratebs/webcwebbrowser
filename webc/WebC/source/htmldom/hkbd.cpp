//
// HKBD.CPP - Methods for HTML DOM class HTMLKbd
//
// EBS - WebC
//
// Copyright EBS Inc. , 2003
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
#include "hkbd.hpp"

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
// HTMLKbd Constructors
/*****************************************************************************/

HTMLKbd::HTMLKbd(HTMLTagParse* td, HTMLDocument *pDocument)
	: HTMLFlow(td, pDocument)
{
}


/*****************************************************************************/
// HTMLKbd Destructor
/*****************************************************************************/

HTMLKbd::~HTMLKbd(void)
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


/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

CSSValueType HTMLKbd::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{
	switch (Property)
	{
		case CSS_PROPERTY_FONT_FAMILY:
			pValue->name = webc_GetDefaultString(WEBC_DEFAULT_MONOSPACE_FONT_FAMILY);
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_FONT_STYLE:
			pValue->fontStyle = webc_GetDefaultFontStyle(WEBC_DEFAULT_MONOSPACE_FONT_STYLE);
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_FONT_VARIANT:
			break;

		case CSS_PROPERTY_FONT_WEIGHT:
			pValue->fontWeight =  webc_GetDefaultFontWeight(WEBC_DEFAULT_MONOSPACE_FONT_WEIGHT);
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_FONT_SIZE:
			pValue->fontSize.type = CSS_FONT_SIZE_ABSOLUTE;
			pValue->fontSize.val.absolute =  webc_GetDefaultUint16(WEBC_DEFAULT_MONOSPACE_FONT_SIZE); 
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_DISPLAY:
			pValue->display = CSS_DISPLAY_INLINE;
			return (CSS_VALUE_SPECIFIED);		
			
		default:
			break;
	}
	
	return (CSS_VALUE_UNKNOWN);
}


/*****************************************************************************/
// HTMLKbd-Specific Methods
/*****************************************************************************/
