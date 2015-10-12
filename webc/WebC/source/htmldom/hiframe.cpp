//
// HIFRAME.CPP - Methods for HTML DOM class HTMLInlineFrame
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
#include "hiframe.hpp"
#include "webload.hpp"
#include "dhiframe.hpp"
#include "webcmem.h"

#if (WEBC_SUPPORT_FRAMES && WEBC_SUPPORT_IFRAMES)

/*****************************************************************************/
// Local Constants 
/*****************************************************************************/

#define WEBC_CFG_DEFAULT_FRAME_BORDER_WIDTH 2

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
// HTMLInlineFrame Constructors
/*****************************************************************************/

HTMLInlineFrame::HTMLInlineFrame(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLFrame(td, pDocument)
{
	mFrameAlign = HTML_ALIGN_NONE;
	mBorderWidth = WEBC_CFG_DEFAULT_FRAME_BORDER_WIDTH;
	
	SetFrameborder(WEBC_TRUE);
}


/*****************************************************************************/
// HTMLInlineFrame Destructor
/*****************************************************************************/

HTMLInlineFrame::~HTMLInlineFrame(void)
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


int HTMLInlineFrame::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		case HTML_ATTRIB_WIDTH:
			SetWidth(value);
			return (1);
		
		case HTML_ATTRIB_HEIGHT:
			SetHeight(value);
			return (1);
		
		case HTML_ATTRIB_ALIGN:
			SetAlign(value);
			return (1);
			
		case HTML_ATTRIB_BORDER:
			if (value && *value)
			{
				mBorderWidth = (WEBC_UINT8) webc_atoi(value);
			}
			else
			{
				mBorderWidth = WEBC_CFG_DEFAULT_FRAME_BORDER_WIDTH;
			}
			return (1);
			
		default:
			break;
	}

	if (call_base)
	{
		return (HTMLFrame::SetProperty(Property, value, pDocument, call_base));
	}

	return (0);
}

void HTMLInlineFrame::SetAlign(WEBC_CHAR *value)
{
	mFrameAlign = HTML_ParseAlignType(value, 0);
}

int HTMLInlineFrame::GetAlign(void)
{
	return (mFrameAlign);
}

void HTMLInlineFrame::SetWidth(WEBC_CHAR *value)
{
	HTML_ParseLength(value, 0, &mFrameWidth);
	// tbd - notify display element
}


void HTMLInlineFrame::SetHeight(WEBC_CHAR *value)
{
	HTML_ParseLength(value, 0, &mFrameHeight);
	// tbd - notify display element
}

WEBC_UINT8 HTMLInlineFrame::Border()
{	
	if (mFlags & HFRAME_FLAG_BORDER_DEFINED)
	{
		return Frameborder()? mBorderWidth : 0;
	}
	
	return 0;
}

CSSValueType HTMLInlineFrame::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{
	switch (Property)
	{
		case CSS_PROPERTY_FLOAT:
			switch (mFrameAlign)
			{
				case HTML_ALIGN_LEFT:
					pValue->floatType = CSS_FLOAT_LEFT;
					break;
		
				case HTML_ALIGN_RIGHT:
					pValue->floatType = CSS_FLOAT_RIGHT;
					break;

				default:
					pValue->floatType = CSS_FLOAT_NONE;
					break;
			}
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_VERTICAL_ALIGN:
			switch (mFrameAlign)
			{
				case HTML_ALIGN_TOP:
					pValue->verticalAlign = CSS_VERTICAL_ALIGN_TEXT_TOP;
					break;
					
				case HTML_ALIGN_BOTTOM:
					pValue->verticalAlign = CSS_VERTICAL_ALIGN_TEXT_BOTTOM;
					break;
				
				case HTML_ALIGN_MIDDLE:
				case HTML_ALIGN_CENTER:
					pValue->verticalAlign = CSS_VERTICAL_ALIGN_MIDDLE;
					break;
					
				case HTML_ALIGN_TEXTTOP:
					pValue->verticalAlign = CSS_VERTICAL_ALIGN_TEXT_TOP;
					break;

				case HTML_ALIGN_ABSBOTTOM:
					pValue->verticalAlign = CSS_VERTICAL_ALIGN_BOTTOM;
					break;

				case HTML_ALIGN_BASELINE:
					pValue->verticalAlign = CSS_VERTICAL_ALIGN_BASELINE;
					break;
				
				default:
					pValue->verticalAlign = CSS_VERTICAL_ALIGN_BASELINE;
					break;
			}					
			return (CSS_VALUE_SPECIFIED);
		
		case CSS_PROPERTY_OUTLINE_COLOR:
			return (CSS_VALUE_INHERIT);
			
		case CSS_PROPERTY_OUTLINE_STYLE:
			/* tbd
			if (GetFlags() & HIFRAME_FLAG_HAS_FOCUS)
			{
		  		pValue->borderStyle = CSS_BORDER_STYLE_DOTTED;
				return (CSS_VALUE_SPECIFIED);
			}
			*/
			return (CSS_VALUE_INHERIT);
			
		case CSS_PROPERTY_OUTLINE_WIDTH:
			return (CSS_VALUE_INHERIT);
		
		case CSS_PROPERTY_WIDTH:
			if (CSS_HTMLLengthToCSS(&pValue->length, mFrameWidth) < 0)
			{
				pValue->length.type = CSS_LENGTH_UNIT_AUTO;
			}
			return (CSS_VALUE_SPECIFIED);
			
		case CSS_PROPERTY_HEIGHT:
			if (CSS_HTMLLengthToCSS(&pValue->length, mFrameHeight) < 0)
			{
				pValue->length.type = CSS_LENGTH_UNIT_AUTO;
			}
			return (CSS_VALUE_SPECIFIED);
		
		default:
			break;
	}
	
	return (HTMLFrame::GetStyleFromHTML(Property, pValue));	
}


/*****************************************************************************/
// HTMLInlineFrame-Specific Methods
/*****************************************************************************/

HTMLLength HTMLInlineFrame::FrameWidth(void)
{
	return mFrameWidth;
}

HTMLLength HTMLInlineFrame::FrameHeight(void)
{
	return mFrameHeight;
}

HTMLFrameDisplay *HTMLInlineFrame::GetFrameDisplay(WEBC_BOOL create)
{
	if (!mpFrameDisplay && create)
	{
		WEBC_NEW_VERBOSE(mpFrameDisplay, HTMLIFrameDisplay(this),"GetFrameDisplay");
		if (mpFrameDocument)
		{
			mpFrameDocument->SetDisplayManager(mpFrameDisplay);
		}
	}
	
	return (mpFrameDisplay);	
}

#endif // WEBC_SUPPORT_FRAMES && WEBC_SUPPORT_IFRAMES
