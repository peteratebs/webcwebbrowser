//
// HTR.CPP - Methods for HTML DOM class HTMLTableRow
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
#include "htr.hpp"
#include "htd.hpp"
#include "htable.hpp"
#include "util.h"
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
// HTMLTableRow Constructors
/*****************************************************************************/

HTMLTableRow::HTMLTableRow(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLElementContainer(td, pDocument)
{
	mTable = 0;
	mPrevRow = 0;
	mNextRow = 0;
	mFirstCell = 0;
	mLastCell = 0;

	mRowIndex = 0;
	mRowBackgroundColor = HTML_RGBAToColor(0,0,0,0xff);
	SetRowHeight(WEBC_NULL);
}

/*****************************************************************************/
// HTMLTableRow Destructor
/*****************************************************************************/

HTMLTableRow::~HTMLTableRow(void)
{
	if (mPrevRow)
	{
		mPrevRow->SetNextRow(mNextRow);
	}
	else
	{
		if (mTable)
		{
			mTable->SetFirstRow(mNextRow);
		}
	}

	if (mNextRow)
	{
		mNextRow->SetPrevRow(mPrevRow);
	}
	else
	{
		if (mTable)
		{
			mTable->SetLastRow(mPrevRow);
		}
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

/*
void HTMLTableRow::Update(Observable o, WEBC_PFBYTE data)
{
}
*/

/*
void HTMLTableRow::Refresh(PegThing *pThing)
{
}
*/
/*
HTMLEventStatus HTMLTableRow::DefaultEvent(HTMLEvent *pEvent)
{
    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	switch (pEvent->type)
	{
		default:
			// ignore other events
			break;
	}

	return (HTML_EVENT_STATUS_CONTINUE);
}
*/
/*
HTMLElement *HTMLTableRow::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

/*
void HTMLTableRow::Reset(void)
{
}
*/

int HTMLTableRow::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
	WEBC_UINT8 align;
	HTMLColor bgcolor;
	HTMLLength length;

	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
		case HTML_ATTRIB_ALIGN:
			align = HTML_ParseAlignType(value, 0);
			switch (align)
			{
				case HTML_ALIGN_MIDDLE:
				case HTML_ALIGN_CENTER:
					SetRowTextAlign(CSS_TEXT_ALIGN_CENTER);
					break;

				case HTML_ALIGN_RIGHT:
					SetRowTextAlign(CSS_TEXT_ALIGN_RIGHT);
					break;

				case HTML_ALIGN_LEFT:
				default:
					SetRowTextAlign(CSS_TEXT_ALIGN_LEFT);
					break;
			}
			return (1);

		case HTML_ATTRIB_VALIGN:
			align = HTML_ParseAlignType(value, 0);
			switch (align)
			{
				case HTML_ALIGN_ABSBOTTOM:
				case HTML_ALIGN_TEXTBOTTOM:
				case HTML_ALIGN_BASELINE:
				case HTML_ALIGN_BOTTOM:
					SetRowVAlign(CSS_VERTICAL_ALIGN_BOTTOM);
					break;

				case HTML_ALIGN_ABSMIDDLE:
				case HTML_ALIGN_MIDDLE:
				case HTML_ALIGN_CENTER:
					SetRowVAlign(CSS_VERTICAL_ALIGN_MIDDLE);
					break;

				case HTML_ALIGN_TOP:
				default:
					SetRowVAlign(CSS_VERTICAL_ALIGN_TOP);
					break;
			}
			return (1);

		case HTML_ATTRIB_BGCOLOR:
			if (HTML_ParseColor(value, 0, &bgcolor) > 0)
			{
				SetRowBackgroundColor(bgcolor);
			}
			return (1);

		case HTML_ATTRIB_HEIGHT:
			HTML_ParseLength(value, 0, &length);
			SetRowHeight(length);
			return (1);

		case HTML_ATTRIB_NOWRAP:
			SetFlag(HTR_FLAG_NOWRAP);
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

void HTMLTableRow::SetRowBackgroundColor(HTMLColor color)
{
	mRowBackgroundColor = color;
}

HTMLColor HTMLTableRow::RowBackgroundColor (void)
{
	return (mRowBackgroundColor);
}


CSSValueType HTMLTableRow::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{
	switch (Property)
	{
		case CSS_PROPERTY_VERTICAL_ALIGN:
			if (mFlags & HTR_FLAG_VALIGN)
			{
				pValue->verticalAlign = GetRowVAlign();
				return (CSS_VALUE_SPECIFIED);
			}
			break;

		case CSS_PROPERTY_TEXT_ALIGN:
			if (mFlags & HTR_FLAG_TEXTALIGN)
			{
				pValue->textAlign = GetRowTextAlign();
				return (CSS_VALUE_SPECIFIED);
			}
			break;

		case CSS_PROPERTY_BACKGROUND_COLOR:
			if (HTML_ColorGetAlpha(mRowBackgroundColor) != 0xff)
			{
				pValue->color = mRowBackgroundColor;
				return (CSS_VALUE_SPECIFIED);
			}
			break;

		case CSS_PROPERTY_WHITE_SPACE:
			if (mFlags & HTR_FLAG_NOWRAP)
			{
				pValue->whitespace = CSS_WHITE_SPACE_NOWRAP;
				return (CSS_VALUE_SPECIFIED);
			}
			break;

		default:
			break;
	}

	return (CSS_VALUE_UNKNOWN);
}


void HTMLTableRow::SetRowHeight(HTMLLength height)
{
	// only table heights in pixels are valid HTML
	if (HTML_LengthGetUnit(height) == HTML_LENGTH_UNIT_PIXELS)
	{
		mRowHeight = height;
	}
	else
	{
		HTML_LengthSet(mRowHeight, 0, HTML_LENGTH_UNIT_NONE);
	}
}


/*****************************************************************************/
// HTMLTableRow-Specific Methods
/*****************************************************************************/


HTMLTableCell *HTMLTableRow::InsertCell(int iIndex)
{
	HTMLTableCell *pNewCell, *pIndexCell;
	HTMLTagParse dummyParseStruct;
	HTML_InitTagParse(&dummyParseStruct, HTML_TAG_TD);

	pIndexCell = GetCell(iIndex);

	WEBC_NEW_VERBOSE(pNewCell,HTMLTableCell(&dummyParseStruct, GetDocument()),"HTMLTableCell");
	if (!pNewCell)
	{
		return (0);
	}

	if(pIndexCell)
		InsertBefore(pNewCell, pIndexCell);
	else
		InsertLast(pNewCell);

	return (pNewCell);
}


HTMLTableCell *HTMLTableRow::GetCell(int iIndex)
{
HTMLElement *pChild;
int iColIndex;

	HTMLTable* tableElement = GetTable();
	if (tableElement)
	{
		if (tableElement->CalculateCellPositions() < 0)
		{
			return (0);
		}
	}

	pChild = mpFirstChild;
	while (pChild)
	{
		if (pChild->Type() == HTML_TABLE_CELL_ELEMENT)
		{
			iColIndex = ((HTMLTableCell*) pChild)->ColIndex();
			if (iColIndex == iIndex)
			{
				return ((HTMLTableCell*)pChild);
			}
			// columns are always in ascending column index order
			else if (iColIndex > iIndex)
			{
				return (0);
			}
		}
		pChild = pChild->mpNext;
	}

	return (0);
}


void HTMLTableRow::DeleteCell(int iIndex)
{
}


int HTMLTableRow::GetNumColumns(void)
{
	HTMLTable* tableElement = GetTable();
	if (tableElement)
	{
		if (tableElement->CalculateCellPositions() < 0)
		{
			return (0);
		}
	}

	return (mLastCell)? (mLastCell->ColIndex() + 1) : 0;
}


/******************************************************************************
* void HTMLTableRow::SetRowTextAlign(WEBC_UINT16 wAlign) -
*
* Parameters
*    WEBC_UINT16 wAlign - The new text alignment for this element.
*
* Description - This method sets the horizontal alignment type for this element
*
*
* Returns (void)
*
******************************************************************************/
void HTMLTableRow::SetRowTextAlign(CSSTextAlign align)
{
	mTextAlign = align;
	SetFlag(HTR_FLAG_TEXTALIGN);
}


/******************************************************************************
* WEBC_UINT8 HTMLTableRow::GetRowTextAlign() -
*
* Parameters
*    none
*
* Description - This method gets the horizontal text alignment type.
*
*
* Returns (WEBC_UINT8)
*	the horizontal text alignment
******************************************************************************/
CSSTextAlign HTMLTableRow::GetRowTextAlign(void)
{
	return (mTextAlign);
}


/******************************************************************************
* void HTMLTableRow::SetRowVAlign(WEBC_UINT16 wAlign)
*
* Parameters
*    WEBC_UINT16 wAlign - the new vertical alignment type
*
* Description - This method sets the vertical alignment type of this element
*
*
* Returns (void)
*
******************************************************************************/
void HTMLTableRow::SetRowVAlign(CSSVerticalAlign vAlign)
{
	mVerticalAlign = vAlign;
	SetFlag(HTR_FLAG_VALIGN);
}

/******************************************************************************
* WEBC_UINT8 HTMLTableRow::GetRowVAlign() -
*
* Parameters
*    none
*
* Description - This method gets the vertical text alignment type.
*
*
* Returns (WEBC_UINT8)
*	the horizontal text alignment
******************************************************************************/
CSSVerticalAlign HTMLTableRow::GetRowVAlign(void)
{
	return (mVerticalAlign);
}

void HTMLTableRow::NotifyChildAdded(void)
{
HTMLTable *pTable = GetTable();

	if (pTable)
	{
		pTable->SetFlag(HTABLE_FLAG_CELL_POS_INVALID);

		if (pTable->GetDisplayElement(WEBC_FALSE))
		{
			NotifyEvent what(NOTIFIER_TYPE_CHILD_TABLE_ROW, NOTIFY_CONTENT_ADDED);
			pTable->GetDisplayElement(WEBC_FALSE)->Notify(this, &what);
		}
	}
}

void HTMLTableRow::NotifyChildRemoved(void)
{
	NotifyChildAdded();
}


#endif // WEBC_SUPPORT_TABLES
