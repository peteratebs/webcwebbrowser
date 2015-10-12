//
// HTD.CPP - Methods for HTML DOM class HTMLTableCell
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
#include "htd.hpp"
#include "htr.hpp"
#include "htable.hpp"
#include "util.h"
#include "webload.hpp"
#include "dhtd.hpp"
#include "htmlbrow.hpp"
#include "webcstr.h"
#include "webcmem.h"
#include "webcassert.h"

#if (WEBC_SUPPORT_TABLES)

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
// HTMLTableCell Constructors
/*****************************************************************************/

HTMLTableCell::HTMLTableCell(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLFlow(td, pDocument)
{
	mRow = 0;
	mPrevCell = 0;
	mNextCell = 0;

	// initialize all attributes to their default values
	mwColumnIndex = 0;
	mwColSpan = 1;
	mwRowSpan = 1;
	mbTdAlign = HTML_ALIGN_NONE;
	mbTdVAlign = HTML_VALIGN_NONE;
	
	SetCellWidth(WEBC_NULL);
	SetCellHeight(WEBC_NULL);
}


/*****************************************************************************/
// HTMLTableCell Destructor
/*****************************************************************************/

HTMLTableCell::~HTMLTableCell(void)
{
	if (mPrevCell)
	{
		mPrevCell->SetNextCell(mNextCell);
	}
	else
	{
		//WEBC_ASSERT(mRow);
		if (mRow)
		{
			mRow->SetFirstCell(mNextCell);
		}
	}

	if (mNextCell)
	{
		mNextCell->SetPrevCell(mPrevCell);
	}
	else
	{
		//WEBC_ASSERT(mRow);
		if (mRow)
		{
			mRow->SetLastCell(mPrevCell);
		}
	}
}


/*****************************************************************************/
// TextUnit Interface
/*****************************************************************************/

WEBC_UINT16 HTMLTableCell::GetUnitType(void)
{
	return (TU_TYPE_DEFAULT);
}

	
/*****************************************************************************/
// TextUnit Helper Methods
/*****************************************************************************/


/*****************************************************************************/
// Override-able HTMLElement Methods
/*****************************************************************************/


void HTMLTableCell::LocalStyleUpdate(void)
{
	HTMLFlow::LocalStyleUpdate();	

	CSSPropertyValue value;
	
	if (GetStyleFromCSS(CSS_PROPERTY_WIDTH, &value) == CSS_VALUE_SPECIFIED)
	{
		mWidthFromCSS = CSS_LengthToHTML(&(value.length), WEBC_CFG_DEFAULT_TEXT_HEIGHT, WEBC_CFG_DEFAULT_TEXT_HEIGHT/2);
		SetFlag(HTD_FLAG_WIDTH_FROM_CSS);
	}
	else
	{
		ClearFlag(HTD_FLAG_WIDTH_FROM_CSS);
	}
	
	if (GetStyleFromCSS(CSS_PROPERTY_HEIGHT, &value) == CSS_VALUE_SPECIFIED)
	{
		mHeightFromCSS = CSS_LengthToHTML(&(value.length), WEBC_CFG_DEFAULT_TEXT_HEIGHT, WEBC_CFG_DEFAULT_TEXT_HEIGHT/2);
		SetFlag(HTD_FLAG_HEIGHT_FROM_CSS);
	}  
	else
	{
		ClearFlag(HTD_FLAG_HEIGHT_FROM_CSS);
	}
}

/******************************************************************************
* void HTMLTableCell::GetFontSizeDesc() - 
*   
* Parameters
*    none
*
* Description
*
* Returns (WebcFontDescriptor)
*    
******************************************************************************/
WebcFontDescriptor HTMLTableCell::GetFontSizeDesc (void)
{
	if (!(mFlags & HELEM_FLAG_FONT_SIZE_CACHED))
	{
		CSSPropertyValue value;
		
		if ((GetStyleFromCSS(CSS_PROPERTY_FONT_SIZE, &value) == CSS_VALUE_SPECIFIED) ||
		    (GetStyleFromHTML(CSS_PROPERTY_FONT_SIZE, &value) == CSS_VALUE_SPECIFIED))
		{
			if ((value.fontSize.type == CSS_FONT_SIZE_RELATIVE) ||
				(value.fontSize.type == CSS_FONT_SIZE_LENGTH && 
				 (value.fontSize.val.length.type == CSS_LENGTH_UNIT_PERCENT ||
				  value.fontSize.val.length.type == CSS_LENGTH_UNIT_EM ||
				  value.fontSize.val.length.type == CSS_LENGTH_UNIT_EX)))
			{
				int ptSize = CSS_FontSizeAbsoluteToPoints(CSS_FONT_SIZE_MEDIUM);
				WEBC_SET_FONT_SIZE(mFontDesc, ptSize);
			}

			mFontDesc = GetFontDescriptor(0, &value, 0, 0, 0, mFontDesc);
		}
		else
		{
			WEBC_COPY_FONT_SIZE(mFontDesc, (mpParent)? mpParent->GetFontSizeDesc() : 0);
		}
		
		//mFlags |= HELEM_FLAG_FONT_SIZE_CACHED;
	}

	return (mFontDesc);	
}

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

int HTMLTableCell::SetProperty (
		HTMLAttributeType property, 
		WEBC_CHAR *value, 
		HTMLDocument* document, 
		int call_base
	)
{
	HTMLLength length;
	HTMLColor bgcolor;

	if (document == 0)
	{
		document = GetDocument();
	}

	switch (property)
	{
		case HTML_ATTRIB_COLSPAN:
			SetColSpan(webc_atoi(value));
			return (1);

		case HTML_ATTRIB_ROWSPAN:
			SetRowSpan(webc_atoi(value));
			return (1);

		case HTML_ATTRIB_WIDTH:
			HTML_ParseLength(value, 0, &length);
			SetCellWidth(length);
			return (1);

		case HTML_ATTRIB_HEIGHT:
			HTML_ParseLength(value, 0, &length);
			SetCellHeight(length);
			return (1);

		case HTML_ATTRIB_ALIGN:
			mbTdAlign = HTML_ParseAlignType(value, 0);
			return (1);

		case HTML_ATTRIB_VALIGN:
			mbTdVAlign = HTML_ParseVAlignType(value, 0);
			return (1);

		case HTML_ATTRIB_BGCOLOR:
			if (HTML_ParseColor(value, 0, &bgcolor) > 0)
			{
				SetTdBackgroundColor(bgcolor);
			}
			return (1);

		case HTML_ATTRIB_NOWRAP:
			SetFlag(HTD_FLAG_NOWRAP);
			return (1);
						
		default:
			break;
	}

	if (call_base)
	{
		return (HTMLFlow::SetProperty(property, value, document, call_base));
	}

	return (0);
}


void HTMLTableCell::SetColSpan(int iColSpan)
{
	mwColSpan = EBSMAX(1,iColSpan);
}


void HTMLTableCell::SetRowSpan(int iRowSpan)
{
	mwRowSpan = EBSMAX(1,iRowSpan);
}


void HTMLTableCell::SetCellWidth(HTMLLength width)
{
	mCellWidth = width;
}


void HTMLTableCell::SetCellHeight(HTMLLength height)
{
	// only table cell heights in pixels are valid HTML
	if (HTML_LengthGetUnit(height) == HTML_LENGTH_UNIT_PIXELS)
	{
		mCellHeight = height;
	}
	else
	{
		HTML_LengthSet(mCellHeight, 0, HTML_LENGTH_UNIT_NONE);
	}
}


CSSValueType HTMLTableCell::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{
	switch (Property)
	{
		case CSS_PROPERTY_TEXT_ALIGN:
			switch (mbTdAlign)
			{
				case HTML_ALIGN_RIGHT:
					pValue->textAlign = CSS_TEXT_ALIGN_RIGHT;
					return (CSS_VALUE_SPECIFIED);
					
				case HTML_ALIGN_MIDDLE:
				case HTML_ALIGN_CENTER:
					pValue->textAlign = CSS_TEXT_ALIGN_CENTER;
					return (CSS_VALUE_SPECIFIED);

				case HTML_ALIGN_NONE:
					break;
					
				default:
					pValue->textAlign = CSS_TEXT_ALIGN_LEFT;
					return (CSS_VALUE_SPECIFIED);
			}
			break;
			
		case CSS_PROPERTY_VERTICAL_ALIGN:
			switch (mbTdVAlign)
			{
				case HTML_VALIGN_TOP:
					pValue->verticalAlign = CSS_VERTICAL_ALIGN_TOP;
					return (CSS_VALUE_SPECIFIED);
					
				case HTML_VALIGN_BOTTOM:
					pValue->verticalAlign = CSS_VERTICAL_ALIGN_BOTTOM;
					return (CSS_VALUE_SPECIFIED);

				case HTML_VALIGN_NONE:
					break;

				default:
					pValue->verticalAlign = CSS_VERTICAL_ALIGN_MIDDLE;
					return (CSS_VALUE_SPECIFIED);
			}
			break;
			
		case CSS_PROPERTY_BACKGROUND_COLOR:
			if (mFlags & HTD_FLAG_BGCOLOR)
			{
				pValue->color = mTdBackgroundColor;
				return (CSS_VALUE_SPECIFIED);
			}
			else if (GetRow() &&
			         ((GetRow()->GetStyleFromCSS(Property, pValue) == CSS_VALUE_SPECIFIED) || 
			          (GetRow()->GetStyleFromHTML(Property, pValue) == CSS_VALUE_SPECIFIED)))
			{
				return (CSS_VALUE_SPECIFIED);
			}
			break;
		
		case CSS_PROPERTY_FONT_FAMILY:  // intentional fall-through
		case CSS_PROPERTY_COLOR:
			// TD font family and color default directly to the BODY element
			if (GetDocument()->Body()->GetStyleFromCSS(Property, pValue) != CSS_VALUE_SPECIFIED)
			{
				return (GetDocument()->Body()->GetStyleFromHTML(Property, pValue));
			}			
			return (CSS_VALUE_SPECIFIED);
			
		case CSS_PROPERTY_FONT_SIZE:
	  		pValue->fontSize.type = CSS_FONT_SIZE_ABSOLUTE;
	  		pValue->fontSize.val.absolute = CSS_FONT_SIZE_MEDIUM;
			return (CSS_VALUE_SPECIFIED);
			
		case CSS_PROPERTY_FONT_STYLE:
			pValue->fontStyle = CSS_FONT_STYLE_NORMAL;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_FONT_WEIGHT:
			pValue->fontWeight = CSS_FONT_WEIGHT_NORMAL;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_TEXT_DECORATION:
			pValue->textDecoration = CSS_TEXT_DECORATION_NONE;
			return (CSS_VALUE_SPECIFIED);

		case CSS_PROPERTY_WHITE_SPACE:
			if (mFlags & HTD_FLAG_NOWRAP)
			{
				pValue->whitespace = CSS_WHITE_SPACE_NOWRAP;
				return (CSS_VALUE_SPECIFIED);
			}
			break;

		case CSS_PROPERTY_PADDING_BOTTOM:
		case CSS_PROPERTY_PADDING_LEFT:		
		case CSS_PROPERTY_PADDING_RIGHT:
		case CSS_PROPERTY_PADDING_TOP:
		{
			HTMLTable* table = GetTable();
			if (table && table->CellPaddingDefined())
			{
				pValue->length.type = CSS_LENGTH_UNIT_PX;
				pValue->length.val.dec = table->GetCellPadding();
				pValue->length.val.frac = 0;
				return (CSS_VALUE_SPECIFIED);
			}
			break;
		}

		case CSS_PROPERTY_BORDER_BOTTOM_STYLE:
		{
			HTMLTable* table = GetTable();
			if (table)
			{
				if (table->TableBorder(CSS_BOTTOM) > 0)
				{
					// tbd - if num colors is less than 4 or 3D widgets is disabled,
					//  style should be CSS_BORDER_STYLE_SOLID
					pValue->borderStyle = CSS_BORDER_STYLE_INSET;
				}
				else
				{
					pValue->borderStyle = CSS_BORDER_STYLE_NONE;
				}
				return (CSS_VALUE_SPECIFIED);				
			}
			break;
		}
		case CSS_PROPERTY_BORDER_LEFT_STYLE:   // intentional fall-through
		{
			HTMLTable* table = GetTable();
			if (table)
			{
				if (table->TableBorder(CSS_LEFT) > 0)
				{
					// tbd - if num colors is less than 4 or 3D widgets is disabled,
					//  style should be CSS_BORDER_STYLE_SOLID
					pValue->borderStyle = CSS_BORDER_STYLE_INSET;
				}
				else
				{
					pValue->borderStyle = CSS_BORDER_STYLE_NONE;
				}
				return (CSS_VALUE_SPECIFIED);				
			}
			break;
		}
		case CSS_PROPERTY_BORDER_RIGHT_STYLE:  // intentional fall-through
		{
			HTMLTable* table = GetTable();
			if (table)
			{
				if (table->TableBorder(CSS_RIGHT) > 0)
				{
					// tbd - if num colors is less than 4 or 3D widgets is disabled,
					//  style should be CSS_BORDER_STYLE_SOLID
					pValue->borderStyle = CSS_BORDER_STYLE_INSET;
				}
				else
				{
					pValue->borderStyle = CSS_BORDER_STYLE_NONE;
				}
				return (CSS_VALUE_SPECIFIED);				
			}
			break;
		}
		case CSS_PROPERTY_BORDER_TOP_STYLE:
		{
			HTMLTable* table = GetTable();
			if (table)
			{
				if (table->TableBorder(CSS_TOP) > 0)
				{
					// tbd - if num colors is less than 4 or 3D widgets is disabled,
					//  style should be CSS_BORDER_STYLE_SOLID
					pValue->borderStyle = CSS_BORDER_STYLE_INSET;
				}
				else
				{
					pValue->borderStyle = CSS_BORDER_STYLE_NONE;
				}
				return (CSS_VALUE_SPECIFIED);				
			}
			break;
		}
		case CSS_PROPERTY_BORDER_BOTTOM_COLOR: // intentional fall-through
		case CSS_PROPERTY_BORDER_LEFT_COLOR:   // intentional fall-through
		case CSS_PROPERTY_BORDER_RIGHT_COLOR:  // intentional fall-through
		case CSS_PROPERTY_BORDER_TOP_COLOR:
			HTML_ColorSet(pValue->color, 0x80, 0x80, 0x80, 0);
			return (CSS_VALUE_SPECIFIED);
			
		case CSS_PROPERTY_BORDER_BOTTOM_WIDTH:
		{
			HTMLTable* table = GetTable();
			if (table->TableBorder(CSS_BOTTOM) > 0)
			{
				pValue->borderThickness.type = CSS_BORDER_THICKNESS_LENGTH;
				pValue->borderThickness.val.length.type = CSS_LENGTH_UNIT_PX;
				pValue->borderThickness.val.length.val.dec = 1;
				pValue->borderThickness.val.length.val.frac = 0;
				return (CSS_VALUE_SPECIFIED);
			}
			break;
		}
		case CSS_PROPERTY_BORDER_LEFT_WIDTH:
		{
			HTMLTable* table = GetTable();
			if (table->TableBorder(CSS_LEFT) > 0)
			{
				pValue->borderThickness.type = CSS_BORDER_THICKNESS_LENGTH;
				pValue->borderThickness.val.length.type = CSS_LENGTH_UNIT_PX;
				pValue->borderThickness.val.length.val.dec = 1;
				pValue->borderThickness.val.length.val.frac = 0;
				return (CSS_VALUE_SPECIFIED);
			}
			break;
		}
		case CSS_PROPERTY_BORDER_RIGHT_WIDTH:
		{
			HTMLTable* table = GetTable();
			if (table->TableBorder(CSS_RIGHT) > 0)
			{
				pValue->borderThickness.type = CSS_BORDER_THICKNESS_LENGTH;
				pValue->borderThickness.val.length.type = CSS_LENGTH_UNIT_PX;
				pValue->borderThickness.val.length.val.dec = 1;
				pValue->borderThickness.val.length.val.frac = 0;
				return (CSS_VALUE_SPECIFIED);
			}
			break;
		}
		case CSS_PROPERTY_BORDER_TOP_WIDTH:
		{
			HTMLTable* table = GetTable();
			if (table->TableBorder(CSS_TOP) > 0)
			{
				pValue->borderThickness.type = CSS_BORDER_THICKNESS_LENGTH;
				pValue->borderThickness.val.length.type = CSS_LENGTH_UNIT_PX;
				pValue->borderThickness.val.length.val.dec = 1;
				pValue->borderThickness.val.length.val.frac = 0;
				return (CSS_VALUE_SPECIFIED);
			}
			break;
		}
			
		case CSS_PROPERTY_HEIGHT:
			if (CSS_HTMLLengthToCSS(&pValue->length, mCellHeight) >= 0)
			{
				return (CSS_VALUE_SPECIFIED);
			}
			break;

		default:
			break;
	}
	
	return (CSS_VALUE_UNKNOWN);
}


/*****************************************************************************/
// HTMLTableCell-Specific Methods
/*****************************************************************************/


/******************************************************************************
* WEBC_UINT8 HTMLTableCell::GetChildVAlign() - 
*   
* Parameters
*    none
*
* Description - This method gets the vertical alignment type for this element
*    
*
* Returns (WEBC_UINT8)
*	the vertical alignment
******************************************************************************/
WEBC_UINT8 HTMLTableCell::GetChildVAlign(void)
{
CSSPropertyValue value;	
	
	if ((GetStyleFromCSS(CSS_PROPERTY_VERTICAL_ALIGN, &value) == CSS_VALUE_SPECIFIED) ||
	    (GetStyleFromHTML(CSS_PROPERTY_VERTICAL_ALIGN, &value) == CSS_VALUE_SPECIFIED) ||
	    (GetRow() && 
	     ((GetRow()->GetStyleFromCSS(CSS_PROPERTY_VERTICAL_ALIGN, &value) == CSS_VALUE_SPECIFIED) ||
	      (GetRow()->GetStyleFromHTML(CSS_PROPERTY_VERTICAL_ALIGN, &value) == CSS_VALUE_SPECIFIED))))
	{
		switch (value.verticalAlign)
		{
			case CSS_VERTICAL_ALIGN_TOP:
				return (TU_VALIGN_TOP);
				
			case CSS_VERTICAL_ALIGN_BOTTOM:
				return (TU_VALIGN_BOTTOM);

			case CSS_VERTICAL_ALIGN_MIDDLE:  // intentional fall-through
			default:
				return (TU_VALIGN_MIDDLE);
		}
	}

	// the default valign is centered
	return (TU_VALIGN_MIDDLE);
}

void HTMLTableCell::SetTdBackgroundColor(HTMLColor color)
{
	SetFlag(HTD_FLAG_BGCOLOR);
	mTdBackgroundColor = color;
}


HTMLFlowDisplay* HTMLTableCell::GetFlowDisplay (WEBC_BOOL create)
{
	if (!mpFlowDisplay && create)
	{
		WEBC_NEW_VERBOSE(mpFlowDisplay, HTMLTableCellDisplay(this),"HTMLTableCellDisplay");
	}
	
	return (mpFlowDisplay);
}

HTMLLength HTMLTableCell::CellWidth(void) 
{
	if (mFlags & HTD_FLAG_WIDTH_FROM_CSS)
	{
		return mWidthFromCSS;
	}
	
	return mCellWidth;
}

HTMLLength HTMLTableCell::CellHeight(void) 
{
	if (mFlags & HTD_FLAG_HEIGHT_FROM_CSS)
	{
		return mHeightFromCSS;
	}
	
	return mCellHeight;
}

#endif // WEBC_SUPPORT_TABLES
