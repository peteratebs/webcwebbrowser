//
// HTABLE.CPP - Methods for HTML DOM class HTMLTable
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
#include "htable.hpp"
#include "htr.hpp"
#include "htd.hpp"
#include "util.h"
#include "webload.hpp"
#include "htmlbrow.hpp"
#include "webcstr.h"
#include "htmlfind.hpp"
#include "webcassert.h"
#include "buffer.h"
#include "webcmem.h"

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
// HTMLTable Constructors
/*****************************************************************************/

HTMLTable::HTMLTable(HTMLTagParse *td, HTMLDocument *pDocument)
	: HTMLElementContainer(td, pDocument)
{
  #if (WEBC_SUPPORT_BACKGROUND_IMAGES)
	mBackgroundURL = 0;
	mBackgroundImage = 0;
  #endif
	mChildPropertyFlags = 0;
	mChildAlign = 0;
	mTableAlign = HTML_ALIGN_NONE;
	mTableBackgroundColor = HTML_RGBAToColor(0,0,0,0xff);
	mTableDisplay = 0;
	mCellSpacing = 0;
	mCellPadding = 0;
	mCellBorder = 0;

	// TODO, we need a deeper copy here
	mFirstRow = 0;
	mLastRow = 0;

	SetTableWidth(WEBC_NULL);
	SetTableHeight(WEBC_NULL);

	for(int side=0; side < 4; side++)
		mBorder[side] = 0;

}
HTMLTable::HTMLTable(HTMLTable &copy)
	:HTMLElementContainer(copy)
{
  #if (WEBC_SUPPORT_BACKGROUND_IMAGES)
	mBackgroundURL = 0;
	mBackgroundImage = 0;
  #endif
	mChildPropertyFlags = copy.mChildPropertyFlags;
	mChildAlign = copy.mChildAlign;
	mTableAlign = copy.mTableAlign;
	mTableBackgroundColor = copy.mTableBackgroundColor;
	mTableDisplay = 0;
	mCellSpacing = copy.mCellSpacing;
	mCellPadding = copy.mCellPadding;
	mCellBorder = copy.mCellBorder;

	mFirstRow = 0;
	mLastRow = 0;

	mWidth = copy.mWidth;
	mHeight = copy.mHeight;

	for(int side=0; side < 4; side++)
		mBorder[side] = copy.mBorder[side];
}

/*****************************************************************************/
// HTMLTable Destructor
/*****************************************************************************/

HTMLTable::~HTMLTable(void)
{
#if(WEBC_SUPPORT_BACKGROUND_IMAGES)
	SetBackgroundImage(0);
	SetBackground(0);
#endif

	if (mTableDisplay)
	{
		WEBC_DELETE(mTableDisplay);
	}
}


/*****************************************************************************/
// TextUnit Interface
/*****************************************************************************/


WEBC_UINT16 HTMLTable::GetUnitType(void)
{
	if (HTMLElement::GetUnitType() == TU_TYPE_NODISPLAY)
	{
		return (TU_TYPE_NODISPLAY);
	}

	return (TU_TYPE_TABLE);
}


WEBC_UINT16 HTMLTable::GetBreak(void)
{
	return (TU_BREAK_BOTH);
}


WEBC_UINT8 HTMLTable::GetTextAlign(void)
{
	if (mTableAlign == HTML_ALIGN_CENTER)
	{
		return (TU_ALIGN_CENTER);
	}
	return (TU_ALIGN_NONE);
}


WEBC_UINT8 HTMLTable::GetVAlign(void)
{
	return (TU_VALIGN_BASELINE);
}


/*****************************************************************************/
// TextUnit Helper Methods
/*****************************************************************************/



/*****************************************************************************/
// Override-able HTMLElement Methods
/*****************************************************************************/

/******************************************************************************
* void HTMLTable::LocalStyleUpdate()
*
* Parameters
*
* Description
*    Update cached properties that can ONLY be defined locally (i.e. which are
*     NEVER inherited from a parent).
*
* Returns (void)
*
******************************************************************************/
void HTMLTable::LocalStyleUpdate (void)
{
	HTMLElement::LocalStyleUpdate();

	CSSPropertyValue value;

  #if (WEBC_SUPPORT_BACKGROUND_IMAGES)
	if (GetStyleFromCSS(CSS_PROPERTY_BACKGROUND_IMAGE, &value) == CSS_VALUE_SPECIFIED)
	{
		SetBackground(value.url);
	}
  #endif // WEBC_SUPPORT_BACKGROUND_IMAGES

	if (GetStyleFromCSS(CSS_PROPERTY_WIDTH, &value) == CSS_VALUE_SPECIFIED)
	{
		mWidthFromCSS = CSS_LengthToHTML(&(value.length), WEBC_CFG_DEFAULT_TEXT_HEIGHT, WEBC_CFG_DEFAULT_TEXT_HEIGHT/2);
		SetFlag(HTABLE_FLAG_WIDTH_FROM_CSS);
	}
	else
	{
		ClearFlag(HTABLE_FLAG_WIDTH_FROM_CSS);
	}

	if (GetStyleFromCSS(CSS_PROPERTY_HEIGHT, &value) == CSS_VALUE_SPECIFIED)
	{
		mHeightFromCSS = CSS_LengthToHTML(&(value.length), WEBC_CFG_DEFAULT_TEXT_HEIGHT, WEBC_CFG_DEFAULT_TEXT_HEIGHT/2);
		SetFlag(HTABLE_FLAG_HEIGHT_FROM_CSS);
	}
	else
	{
		ClearFlag(HTABLE_FLAG_HEIGHT_FROM_CSS);
	}
}

void HTMLTable::Update(WEBC_PFBYTE o, WEBC_PFBYTE data)
{
  #if (WEBC_SUPPORT_BACKGROUND_IMAGES)
	// if the src dirty bit is set, then reload our bitmap
	if (mFlags & HTABLE_FLAG_BG_DIRTY)
	{
		ClearFlag(HTABLE_FLAG_BG_DIRTY);
		if (Background())
		{
			HTMLDocument *pDoc = GetDocument();
			if (pDoc)
			{
				SetFlag(HELEM_FLAG_LOADING_IMAGE);
				pDoc->LoadImage(Background(), this);
			}
		}
	}
  #endif // WEBC_SUPPORT_BACKGROUND_IMAGES

	HTMLElement::Update(o,data);
}


/*
void HTMLTable::Refresh(PegThing *pThing)
{
}
*/
/*
HTMLEventStatus HTMLTable::DefaultEvent(HTMLEvent *pEvent)
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
HTMLElement *HTMLTable::FindElement(HTMLElementFinder *pFinder)
{
	return (0);
}
*/

/*****************************************************************************/
// Property Management Methods
/*****************************************************************************/

/*
void HTMLTable::Reset(void)
{
}
*/

int HTMLTable::SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument, int call_base)
{
	HTMLColor bgcolor;
	HTMLLength dim;

	if (pDocument == 0)
	{
		pDocument = GetDocument();
	}

	switch (Property)
	{
	  #if (WEBC_SUPPORT_BACKGROUND_IMAGES)
		case HTML_ATTRIB_BACKGROUND:
			SetBackground(value);
			return (1);
	  #endif

		case HTML_ATTRIB_ALIGN:
		    mTableAlign = HTML_ParseAlignType(value, 0);
			return (1);

		case HTML_ATTRIB_BORDER:
			if (!value || !(*value))
			{
				SetBorderWidth(1);
			}
			else
			{
				SetBorderWidth(webc_atoi(value));
			}
			return (1);

		case HTML_ATTRIB_CELLSPACING:
			SetCellSpacing(webc_atoi(value));
			return (1);

		case HTML_ATTRIB_CELLPADDING:
			SetCellPadding(webc_atoi(value));
			return (1);

		case HTML_ATTRIB_WIDTH:
			HTML_ParseLength(value, 0, &dim);
			SetTableWidth(dim);
			return (1);

		case HTML_ATTRIB_HEIGHT:
			HTML_ParseLength(value, 0, &dim);
			SetTableHeight(dim);
			return (1);

		case HTML_ATTRIB_BGCOLOR:
			if (HTML_ParseColor(value, 0, &bgcolor) > 0)
			{
				SetTableBackgroundColor(bgcolor);
			}
			return (1);

		case HTML_ATTRIB_NOWRAP:
			return (1);

		case HTML_ATTRIB_VALIGN:
			return (1);

		case HTML_ATTRIB_FRAME:
			{
				HTMLTableFrameType frameType = HTML_ParseTableFrameType(value, 0);
				for(int side=0; side < 4; side++)
					mBorder[side] = 0;

				switch(frameType)
				{
				case HTML_TABLEFRAME_UNKNOWN:
				case HTML_TABLEFRAME_VOID:
				default:
					break;
				case HTML_TABLEFRAME_ABOVE:
					mBorder[CSS_TOP] = 1;
					break;
				case HTML_TABLEFRAME_BELOW:
					mBorder[CSS_BOTTOM] = 1;
					break;
				case HTML_TABLEFRAME_BORDER:
				case HTML_TABLEFRAME_BOX:
					mBorder[CSS_LEFT] = 1;
					mBorder[CSS_RIGHT] = 1;
					mBorder[CSS_TOP] = 1;
					mBorder[CSS_BOTTOM] = 1;
					break;
				case HTML_TABLEFRAME_HSIDES:
					mBorder[CSS_TOP] = 1;
					mBorder[CSS_BOTTOM] = 1;
					break;
				case HTML_TABLEFRAME_LHS:
					mBorder[CSS_LEFT] = 1;
					break;
				case HTML_TABLEFRAME_RHS:
					mBorder[CSS_RIGHT] = 1;
					break;
				case HTML_TABLEFRAME_VSIDES:
					mBorder[CSS_LEFT] = 1;
					mBorder[CSS_RIGHT] = 1;
					break;
				}
			}
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


#if(WEBC_SUPPORT_BACKGROUND_IMAGES)
void HTMLTable::SetBackground(const WEBC_CHAR *pUrl)
{
	if (!mBackgroundURL || !pUrl || webc_stricmp(pUrl, mBackgroundURL))
	{
		SetFlag(HTABLE_FLAG_BG_DIRTY);
	}

	WEBC_CHAR *pStr = webc_malloc_string_copy(pUrl, __FILE__, __LINE__);
	webc_free_string_copy(mBackgroundURL, __FILE__, __LINE__);
 	mBackgroundURL = pStr;
}


/******************************************************************************
* void HTMLTable::SetBackgroundImage(WebcBitmap *pBitmap) - Set the background
*    bitmap for this element.
*
* Parameters
*    pBitmap - Pointer to bitmap to use (see webcbmp.hpp)
*
* Description
*    This method will not display the new bitmap; to redraw, call Refresh().
*
* Returns (void)
*    Nothing.
******************************************************************************/

void HTMLTable::SetBackgroundImage(WebcImage *pImage)
{
	if (pImage)
	{
		pImage->Claim();
	}

	if(mBackgroundImage)
	{
		mBackgroundImage->Release();
	}

	mBackgroundImage = pImage;

	if (mTableDisplay)
	{
		NotifyEvent what(NOTIFIER_TYPE_HTML_ELEMENT, NOTIFY_DISPLAY_STYLE_CHANGE);
		mTableDisplay->Notify(this, &what);
	}
}

#endif


void HTMLTable::SetBorderWidth(int iBorder)
{
	if (iBorder >= 0)
	{
		for(int side=0; side < 4; side++)
			mBorder[side] = iBorder;
		mFlags |= HTABLE_FLAG_BORDER_DEFINED;
	}
}

void HTMLTable::SetCellSpacing(int iSpacing)
{
	if (iSpacing >= 0)
	{
		mCellSpacing = iSpacing;
		mFlags |= HTABLE_FLAG_CELLSPACING_DEFINED;
	}
}

void HTMLTable::SetCellPadding(int iPadding)
{
	if (iPadding >= 0)
	{
		mCellPadding = iPadding;
		mFlags |= HTABLE_FLAG_CELLPADDING_DEFINED;
	}
}

void HTMLTable::SetTableWidth(HTMLLength width)
{
	mWidth = width;
}

void HTMLTable::SetTableHeight(HTMLLength height)
{
	// only table heights in pixels are valid HTML
	if (HTML_LengthGetUnit(height) == HTML_LENGTH_UNIT_PIXELS)
	{
		mHeight = height;
	}
	else
	{
		HTML_LengthSet(mHeight, 0, HTML_LENGTH_UNIT_NONE);
	}
}

void HTMLTable::SetTableBackgroundColor(HTMLColor color)
{
	mTableBackgroundColor = color;
}

HTMLColor HTMLTable::TableBackgroundColor (void)
{
	return (mTableBackgroundColor);
}


CSSValueType HTMLTable::GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue)
{
	CSSValueType retVal = CSS_VALUE_UNKNOWN;
	switch (Property)
	{
		case CSS_PROPERTY_TEXT_ALIGN:
			pValue->textAlign = CSS_TEXT_ALIGN_LEFT;
			retVal = CSS_VALUE_SPECIFIED;
			break;

		case CSS_PROPERTY_FLOAT:
			switch (mTableAlign)
			{
				case HTML_ALIGN_LEFT:
					pValue->floatType = CSS_FLOAT_LEFT;
					retVal = CSS_VALUE_SPECIFIED;
					break;

				case HTML_ALIGN_RIGHT:
					pValue->floatType = CSS_FLOAT_RIGHT;
					retVal = CSS_VALUE_SPECIFIED;
					break;

				default:
					break;
			}
			break;

		case CSS_PROPERTY_WHITE_SPACE:
			pValue->whitespace = CSS_WHITE_SPACE_NORMAL;
			retVal = CSS_VALUE_SPECIFIED;
			break;

		case CSS_PROPERTY_BACKGROUND_COLOR:
			if (HTML_ColorGetAlpha(mTableBackgroundColor) != 0xff)
			{
				pValue->color = mTableBackgroundColor;
				retVal = CSS_VALUE_SPECIFIED;
			}
			break;

		case CSS_PROPERTY_WIDTH:
			if (CSS_HTMLLengthToCSS(&pValue->length, mWidth) < 0)
			{
				pValue->length.type = CSS_LENGTH_UNIT_AUTO;
			}
			retVal = CSS_VALUE_SPECIFIED;
			break;

		case CSS_PROPERTY_HEIGHT:
			if (CSS_HTMLLengthToCSS(&pValue->length, mHeight) < 0)
			{
				pValue->length.type = CSS_LENGTH_UNIT_AUTO;
			}
			retVal = CSS_VALUE_SPECIFIED;
			break;

		case CSS_PROPERTY_BORDER_BOTTOM_STYLE:
			if (mBorder[CSS_BOTTOM] > 0)
			{
				// tbd - if num colors is less than 4 or 3D widgets is disabled,
				//  style should be CSS_BORDER_STYLE_SOLID
				pValue->borderStyle = CSS_BORDER_STYLE_OUTSET;
			}
			else
			{
				pValue->borderStyle = CSS_BORDER_STYLE_NONE;
			}
			retVal = CSS_VALUE_SPECIFIED;
			break;
		case CSS_PROPERTY_BORDER_LEFT_STYLE:
			if (mBorder[CSS_LEFT] > 0)
			{
				// tbd - if num colors is less than 4 or 3D widgets is disabled,
				//  style should be CSS_BORDER_STYLE_SOLID
				pValue->borderStyle = CSS_BORDER_STYLE_OUTSET;
			}
			else
			{
				pValue->borderStyle = CSS_BORDER_STYLE_NONE;
			}
			retVal = CSS_VALUE_SPECIFIED;
			break;
		case CSS_PROPERTY_BORDER_RIGHT_STYLE:
			if (mBorder[CSS_RIGHT] > 0)
			{
				// tbd - if num colors is less than 4 or 3D widgets is disabled,
				//  style should be CSS_BORDER_STYLE_SOLID
				pValue->borderStyle = CSS_BORDER_STYLE_OUTSET;
			}
			else
			{
				pValue->borderStyle = CSS_BORDER_STYLE_NONE;
			}
			retVal = CSS_VALUE_SPECIFIED;
			break;
		case CSS_PROPERTY_BORDER_TOP_STYLE:
			if (mBorder[CSS_TOP] > 0)
			{
				// tbd - if num colors is less than 4 or 3D widgets is disabled,
				//  style should be CSS_BORDER_STYLE_SOLID
				pValue->borderStyle = CSS_BORDER_STYLE_OUTSET;
			}
			else
			{
				pValue->borderStyle = CSS_BORDER_STYLE_NONE;
			}
			retVal = CSS_VALUE_SPECIFIED;
			break;
		case CSS_PROPERTY_BORDER_BOTTOM_COLOR: // intentional fall-through
		case CSS_PROPERTY_BORDER_LEFT_COLOR:   // intentional fall-through
		case CSS_PROPERTY_BORDER_RIGHT_COLOR:  // intentional fall-through
		case CSS_PROPERTY_BORDER_TOP_COLOR:
			HTML_ColorSet(pValue->color, 0x80, 0x80, 0x80, 0);
			retVal = CSS_VALUE_SPECIFIED;
			break;

		case CSS_PROPERTY_BORDER_BOTTOM_WIDTH:
			pValue->borderThickness.type = CSS_BORDER_THICKNESS_LENGTH;
			pValue->borderThickness.val.length.type = CSS_LENGTH_UNIT_PX;
			pValue->borderThickness.val.length.val.dec = mBorder[CSS_BOTTOM];
			pValue->borderThickness.val.length.val.frac = 0;
			retVal = CSS_VALUE_SPECIFIED;
			break;
		case CSS_PROPERTY_BORDER_LEFT_WIDTH:
			pValue->borderThickness.type = CSS_BORDER_THICKNESS_LENGTH;
			pValue->borderThickness.val.length.type = CSS_LENGTH_UNIT_PX;
			pValue->borderThickness.val.length.val.dec = mBorder[CSS_LEFT];
			pValue->borderThickness.val.length.val.frac = 0;
			retVal = CSS_VALUE_SPECIFIED;
			break;
		case CSS_PROPERTY_BORDER_RIGHT_WIDTH:
			pValue->borderThickness.type = CSS_BORDER_THICKNESS_LENGTH;
			pValue->borderThickness.val.length.type = CSS_LENGTH_UNIT_PX;
			pValue->borderThickness.val.length.val.dec = mBorder[CSS_RIGHT];
			pValue->borderThickness.val.length.val.frac = 0;
			retVal = CSS_VALUE_SPECIFIED;
			break;
		case CSS_PROPERTY_BORDER_TOP_WIDTH:
			pValue->borderThickness.type = CSS_BORDER_THICKNESS_LENGTH;
			pValue->borderThickness.val.length.type = CSS_LENGTH_UNIT_PX;
			pValue->borderThickness.val.length.val.dec = mBorder[CSS_TOP];
			pValue->borderThickness.val.length.val.frac = 0;
			retVal = CSS_VALUE_SPECIFIED;
			break;
		case CSS_PROPERTY_PADDING_TOP:  // intentional fall-through
		case CSS_PROPERTY_PADDING_RIGHT:  // intentional fall-through
		case CSS_PROPERTY_PADDING_BOTTOM:  // intentional fall-through
		case CSS_PROPERTY_PADDING_LEFT:
			pValue->length.type = CSS_LENGTH_UNIT_PX;
			pValue->length.val.dec = GetCellSpacing();
			pValue->length.val.frac = 0;
			retVal = CSS_VALUE_SPECIFIED;
			break;

		default:
			break;
	}

	return (retVal);
}


/*****************************************************************************/
// HTMLTable-Specific Methods
/*****************************************************************************/


HTMLTableRow *HTMLTable::InsertRow(int iIndex)
{
	HTMLTableRow *pNewRow, *pIndexRow;
	HTMLTagParse dummyParseStruct;

	HTML_InitTagParse(&dummyParseStruct, HTML_TAG_TR);

	pIndexRow = GetRow(iIndex);

	WEBC_NEW_VERBOSE(pNewRow, HTMLTableRow(&dummyParseStruct, GetDocument()),"HTMLTableRow");
	if (!pNewRow)
	{
		return (0);
	}

	if(pIndexRow)
		InsertBefore(pNewRow, pIndexRow);
	else
		InsertLast(pNewRow);

	return (pNewRow);
}

HTMLTableRow *HTMLTable::GetRow(int index)
{
	CalculateCellPositions();

	HTMLTableRow* tr = FirstRow();
	while (tr && index > 0)
	{
		index--;
		tr = tr->NextRow();
	}

	return (tr);
}

void HTMLTable::DeleteRow(int iIndex)
{
	HTMLTableRow *pDeleteRow;

	pDeleteRow = GetRow(iIndex);
	if (pDeleteRow)
	{
		pDeleteRow->DeleteChildren();
		Remove(pDeleteRow);
		WEBC_DELETE(pDeleteRow);
	}
}

int HTMLTable::GetNumRows(void)
{
	CalculateCellPositions();

	return (mLastRow)? (mLastRow->RowIndex() + 1) : 0;
}

int HTMLTable::GetNumColumns(void)
{
	CalculateCellPositions();

	int numColumns = 0;
	HTMLTableRow* tr = FirstRow();
	while (tr)
	{
		if (tr->GetNumColumns() > numColumns)
		{
			numColumns = tr->GetNumColumns();
		}
		tr = tr->NextRow();
	}

	return (numColumns);
}


/******************************************************************************
* void HTMLTable::SetChildTextAlign(WEBC_UINT16 wAlign) -
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
void HTMLTable::SetChildTextAlign(WEBC_UINT8 bAlign)
{
	mChildAlign = bAlign;
}


/******************************************************************************
* WEBC_UINT8 HTMLTable::GetChildTextAlign() -
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
WEBC_UINT8 HTMLTable::GetChildTextAlign(void)
{
	return (mChildAlign);
}

void HTMLTable::NotifyChildAdded(void)
{
	SetFlag(HTABLE_FLAG_CELL_POS_INVALID);

	if (mTableDisplay)
	{
		NotifyEvent what(NOTIFIER_TYPE_HTML_ELEMENT, NOTIFY_CONTENT_ADDED);
		mTableDisplay->Notify(this, &what);
	}
}

void HTMLTable::NotifyChildRemoved(void)
{
	NotifyChildAdded();
}

DisplayElement *HTMLTable::GetDisplayElement (WEBC_BOOL create)
{
	return (GetTableDisplay(create));
}

TextUnit *HTMLTable::GetTextUnit (void)
{
	return (GetTableDisplay());
}

HTMLTableDisplay *HTMLTable::GetTableDisplay (WEBC_BOOL create)
{
	if (!mTableDisplay && create)
	{
		WEBC_NEW_VERBOSE(mTableDisplay, HTMLTableDisplay(this),"HTMLTableDisplay");
	}

	return (mTableDisplay);
}

void HTMLTable::Notify (Notifier *who, NotifyEvent *what)
{
    WEBC_DEBUG_ReceiveNotify(this, who, what);
  #if (WEBC_SUPPORT_BACKGROUND_IMAGES)
	switch (what->type)
	{
		case NOTIFIER_TYPE_IMAGE:
			switch (what->event)
			{
				case NOTIFY_DONE:
					ClearFlag(HELEM_FLAG_LOADING_IMAGE);
					SetBackgroundImage((WebcImage *) who);
					break;

				default:
					break;
			}
			break;

		default:
			break;
	}
  #endif // WEBC_SUPPORT_IMAGES
}

int HTMLTable::CalculateCellPositions (void)
{
	int result = 0;

	if (mFlags & HTABLE_FLAG_CELL_POS_INVALID)
	{
		long* colHeight = 0;
		long  colHeightSize = 0;

		if (buffer_alloc_at_least((void**) &colHeight, &colHeightSize, 256 * sizeof(long), 0, 1) < 0)
		{
			// error!
			return (-1);
		}

		HTMLTableRow* tr;
		HTMLTableRow* prevRow = 0;
		HTMLTableCell* td;
		long  rowIndex = 0;
		long  colIndex = 0;
		long  numColumns = 0;

		HTMLElementTypeFinder trFinder(HTML_TABLE_ROW_ELEMENT);
		HTMLElementTypeFinder tdFinder(HTML_TABLE_CELL_ELEMENT);
		HTMLElementTypeFinder tableFinder(HTML_TABLE_ELEMENT);
		HTMLElementOrFinder   tableOrTdFinder(&tableFinder, &tdFinder);
		HTMLElementNotFinder  notTableOrTdFinder(&tableOrTdFinder);
		HTMLElementOrFinder   tableOrTrFinder(&tableFinder, &trFinder);
		HTMLElementNotFinder  notTableOrTrFinder(&tableOrTrFinder);

		HTMLElementTreeWalker tableWalker(this, this->FirstChild(), &trFinder, &notTableOrTdFinder);
		tr = (HTMLTableRow*) tableWalker.getNext();
		mFirstRow = tr;
		mLastRow = tr;
		while (tr)
		{
			mLastRow = tr;
			tr->SetRowIndex(rowIndex);
			tr->SetPrevRow(prevRow);
			tr->SetNextRow(0);
			tr->SetTable(this);
			if (prevRow)
			{
				prevRow->SetNextRow(tr);
			}
			prevRow = tr;

			rowIndex++;
			colIndex = 0;

			HTMLTableCell* prevCell = 0;

			HTMLElementTreeWalker trWalker(tr, tr->FirstChild(), &tdFinder, &notTableOrTrFinder);
			td = (HTMLTableCell*) trWalker.getNext();
			tr->SetFirstCell(td);
			tr->SetLastCell(td);
			while (td)
			{
				tr->SetLastCell(td);
				td->SetPrevCell(prevCell);
				td->SetNextCell(0);
				td->SetRow(tr);
				if (prevCell)
				{
					prevCell->SetNextCell(td);
				}
				prevCell = td;

				// calculate the column index for this table cell
				for (; colIndex < numColumns; colIndex++)
				{
					WEBC_ASSERT(colIndex < colHeightSize);
					if (colHeight[colIndex] < rowIndex)
					{
						break;
					}
				}

				td->SetColIndex(colIndex);

				// increase column height array if necessary
				if (buffer_alloc_at_least (
						(void**) &colHeight,
						&colHeightSize,
						(colIndex + td->ColSpan() + 1) * sizeof(long),
						1, /* preserve contents */
						1  /* zero new space */
					) < 0)
				{
					/* error! could not alloc enough space */
					result = -1;
					break;
				}

				// update number of columns
				if (colIndex + td->ColSpan() > numColumns)
				{
					numColumns = colIndex + td->ColSpan();
				}

				// update column heights based on the current TD
				long lastColumnSpanned = colIndex + td->ColSpan();
				long newHeight = tr->RowIndex() + td->RowSpan();
				for (; colIndex < lastColumnSpanned; colIndex++)
				{
					WEBC_ASSERT(colIndex < colHeightSize);
					if (newHeight > colHeight[colIndex])
					{
						colHeight[colIndex] = newHeight;
					}
				}

				td = (HTMLTableCell*) trWalker.getNext();
			} // per td

			tr = (HTMLTableRow*) tableWalker.getNext();
		} // per tr

		if (colHeight)
		{
			WEBC_FREE(colHeight);
		}

		if (result == 0)
		{
			mFlags &= ~HTABLE_FLAG_CELL_POS_INVALID;
		}
	}

	return (result);
}

HTMLLength HTMLTable::TableWidth(void)
{
	if (mFlags & HTABLE_FLAG_WIDTH_FROM_CSS)
	{
		return mWidthFromCSS;
	}

	return mWidth;
}

HTMLLength HTMLTable::TableHeight(void)
{
	if (mFlags & HTABLE_FLAG_HEIGHT_FROM_CSS)
	{
		return mHeightFromCSS;
	}

	return mHeight;
}

#endif // WEBC_SUPPORT_TABLES
