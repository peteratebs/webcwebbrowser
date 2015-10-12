//
// DHTABLE.CPP - Methods for class HTMLTableDisplay
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//

/*****************************************************************************/
// Header Files
/*****************************************************************************/

#include "dhelem.cpp"
#include "htable.hpp"
#include "htd.hpp"
#include "htr.hpp"
#include "dhtable.hpp"
#include "dhtd.hpp"
#include "wgcolors.hpp"
#include "dbgtimer.h"
#include "webcassert.h"
#include "webcmem.h"


/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/

//#define DHTABLE_ASSERT(X)          WEBC_ASSERT(X)
#define DHTABLE_ASSERT(X)

#define UNIT_AUTO_AFFECTED         0x01
#define UNIT_PX_AFFECTED           0x02
#define UNIT_PERCENT_AFFECTED      0x04

#define FLEX_LEVEL_AUTO_ONLY       3
#define FLEX_LEVEL_PX_ONLY         2
#define FLEX_LEVEL_PX_AND_PERCENT  1  // tbd - see if this actually causes problems
#define FLEX_LEVEL_PERCENT_ONLY    0

#define GET_COLUMN_WIDTH(start, c) (start[c+1] - start[c])

#define SET_COLUMN_WIDTH(start, c, width, nc) \
	{ \
		int temp, diff = width - GET_COLUMN_WIDTH(start, c); \
		if (diff != 0) \
		{ \
			for (temp = c+1; temp <= nc; temp++) \
			{ \
				start[temp] += diff; \
			} \
		} \
	}

#define ALLOC_AUTO_ARRAY(T,V,N)  T* V = (T*) WEBC_MALLOC(sizeof(T) * (N)); if (V) {
#define FREE_AUTO_ARRAY(V)       WEBC_FREE(V); }


/*****************************************************************************/
// Local Data
/*****************************************************************************/

//                   bit mask  flexibility
// (invalid)           000          3
// auto                001          3
// px auto             011          2
// px                  010          2
// percent px auto     111          1
// percent px          110          1
// percent auto        101          0
// percent             100          0

static WEBC_INT8 unitCaseFlag[] =
{
	UNIT_AUTO_AFFECTED,    // HTML_LENGTH_UNIT_NONE = 0,
	UNIT_PX_AFFECTED,      // HTML_LENGTH_UNIT_PIXELS,
	UNIT_PERCENT_AFFECTED, // HTML_LENGTH_UNIT_PERCENT,
	UNIT_AUTO_AFFECTED     // HTML_LENGTH_UNIT_STAR,
};

static WEBC_INT8 unitCaseFlexibility [8] =
{
	FLEX_LEVEL_AUTO_ONLY,      // 000
	FLEX_LEVEL_AUTO_ONLY,      // 001
	FLEX_LEVEL_PX_ONLY,        // 010
	FLEX_LEVEL_PX_ONLY,        // 011
	FLEX_LEVEL_PERCENT_ONLY,   // 100
	FLEX_LEVEL_PERCENT_ONLY,   // 101
	FLEX_LEVEL_PX_AND_PERCENT, // 110
	FLEX_LEVEL_PX_AND_PERCENT  // 111
};


/*****************************************************************************/
// Local Types
/*****************************************************************************/

#if (WEBC_SUPPORT_TABLES)

void CalculateDistribution (
		DISPLAY_INT* columnWidth,
		DISPLAY_INT* baseStart,
		DISPLAY_INT* columnWeight,
		DISPLAY_INT* minStart,
		WEBC_INT8* columnFlex,
		int numColumns,
		WEBC_INT8 flexLevel,
		DISPLAY_INT targetWidth
	);

void CalculateDistributionInterpolated (
		DISPLAY_INT* columnWidth,
		DISPLAY_INT* lowStart,
		DISPLAY_INT* highStart,
		DISPLAY_INT* minStart,
		WEBC_INT8* columnFlex,
		int numColumns,
		WEBC_INT8 highestFlex,
		DISPLAY_INT targetWidth,
		DISPLAY_INT* columnWeight, // scratch data
		DISPLAY_INT* workingMinStart  // scratch data
	);


DISPLAY_INT StartsToWidths (
		DISPLAY_INT* width, // out
		DISPLAY_INT* start, // in
		int numColumns
	);

DISPLAY_INT WidthsToStarts (
		DISPLAY_INT* start, // out
		DISPLAY_INT* width, // in
		int numColumns
	);

void ExpandColumnsAtFlexLevel (
		DISPLAY_INT* columnWidth,
		DISPLAY_INT* baseStart,
		WEBC_INT8*   columnFlex,
		int          numColumns,
		WEBC_INT8    flexLevel,
		DISPLAY_INT  targetWidth
	);

/*---------------------------------------------------------------------------*/
class ITableCellWidthMethod
{
 public:
	virtual DISPLAY_INT calculate (HTMLTableCell* td) = 0;
};

/*---------------------------------------------------------------------------*/
class ITableWidthDistributeMethod
{
 public:
 	virtual void distribute (
 			DISPLAY_INT* columnWidth,
 			DISPLAY_INT* currentStart,
 			int colIndex,
 			int colSpan,
 			DISPLAY_INT targetWidth
 		) = 0;
};

/*---------------------------------------------------------------------------*/
class NaturalWidthDistributeMethod : public ITableWidthDistributeMethod
{
 private:
 	DISPLAY_INT* specifiedWidth;
	DISPLAY_INT* specifiedStart;
 	WEBC_INT8*   columnFlex;
	int          numColumns;

 public:
	NaturalWidthDistributeMethod (DISPLAY_INT* specWidth, DISPLAY_INT* specStart, WEBC_INT8* colFlex, int numCol)
	{
	 	specifiedWidth = specWidth;
		specifiedStart = specStart;
	 	columnFlex = colFlex;
		numColumns = numCol;
	}

	~NaturalWidthDistributeMethod () {}

 	virtual void distribute (
 			DISPLAY_INT* columnWidth,
 			DISPLAY_INT* currentStart,
 			int colIndex,
 			int colSpan,
 			DISPLAY_INT targetWidth
 		)
 	{
		WEBC_INT8 flexLevel = 0;
		int c;

		for (c = colIndex; c < colIndex + colSpan; c++)
		{
			if (columnFlex[c] > flexLevel)
			{
				flexLevel = columnFlex[c];
			}
		}

		CalculateDistribution (
				columnWidth + colIndex,    // output
				specifiedStart + colIndex, // base
				specifiedWidth + colIndex, // weight
				currentStart + colIndex,   // minimum
				columnFlex + colIndex,     // flex
				colSpan,                   // number of columns
				flexLevel,
				targetWidth
			);
 	}
};

/*---------------------------------------------------------------------------*/
class MinWidthDistributeMethod : public ITableWidthDistributeMethod
{
 private:
	DISPLAY_INT* columnWeight;
	DISPLAY_INT* absMinStart;
	DISPLAY_INT* naturalStart;
	DISPLAY_INT* tempStart;
	WEBC_INT8*   columnFlex;
	WEBC_INT8    highestFlex;

 public:
 	MinWidthDistributeMethod (DISPLAY_INT* minStart, DISPLAY_INT* natStart, WEBC_INT8* colFlex, WEBC_INT8 hiFlex, int numColumns)
 	{
		absMinStart = minStart;
		naturalStart = natStart;
		columnFlex = colFlex;
		highestFlex = hiFlex;
		// optimize: calculate columnWeight once right here and reuse
		columnWeight = (DISPLAY_INT*) WEBC_MALLOC(sizeof(DISPLAY_INT) * numColumns);
		tempStart = (DISPLAY_INT*) WEBC_MALLOC(sizeof(DISPLAY_INT) * (numColumns + 1));
 	}

 	~MinWidthDistributeMethod ()
	{
		if (columnWeight)
		{
			WEBC_FREE(columnWeight);
		}

		if (tempStart)
		{
			WEBC_FREE(tempStart);
		}
	}

 	virtual void distribute (
 			DISPLAY_INT* columnWidth,
 			DISPLAY_INT* currentStart,
 			int colIndex,
 			int colSpan,
 			DISPLAY_INT targetWidth
 		)
 	{
		if (columnWeight && tempStart)
		{
			DISPLAY_INT naturalSpannedWidth = naturalStart[colIndex + colSpan] - naturalStart[colIndex];
			DISPLAY_INT expandBy = 0;

			if (targetWidth > naturalSpannedWidth)
			{
				expandBy = targetWidth - naturalSpannedWidth;
				targetWidth = naturalSpannedWidth;
 			}

			CalculateDistributionInterpolated (
 					columnWidth + colIndex,
 					absMinStart + colIndex,
 					naturalStart + colIndex,
 					currentStart + colIndex,
 					columnFlex + colIndex,
 					colSpan,
 					highestFlex,
 					targetWidth,
					columnWeight,
					tempStart
 				);

			if (expandBy > 0)
			{
				ALLOC_AUTO_ARRAY(DISPLAY_INT, tempStart, colSpan+1);

				WEBC_INT8 flexLevel = 0;
				int c;

				WidthsToStarts(tempStart, columnWidth + colIndex, colSpan);

				for (c = colIndex; c < colIndex + colSpan; c++)
				{
					if (columnFlex[c] > flexLevel)
					{
						flexLevel = columnFlex[c];
					}
				}

				ExpandColumnsAtFlexLevel (
						columnWidth + colIndex,
						tempStart,
						columnFlex + colIndex,
						colSpan,
						flexLevel,
						targetWidth + expandBy
					);

				FREE_AUTO_ARRAY(tempStart);
			}
		}
 	}
};

/*---------------------------------------------------------------------------*/
class AbsoluteMinWidthMethod : public ITableCellWidthMethod
{
 public:
 	AbsoluteMinWidthMethod (void) {}

 	~AbsoluteMinWidthMethod (void) {}

	DISPLAY_INT calculate (HTMLTableCell* td)
	{
		HTMLTableCellDisplay* cellDisplay;

		cellDisplay = (HTMLTableCellDisplay*) td->GetDisplayElement();

		if (cellDisplay)
		{
			return (cellDisplay->GetMinWidth());
		}

		return (0);
	}
};

/*---------------------------------------------------------------------------*/
class SpecifiedWidthMethod : public ITableCellWidthMethod
{
 private:
 	DISPLAY_INT parentWidth;
 	WEBC_INT8*  columnFlex;
	int         numColumns;

 public:
 	SpecifiedWidthMethod (DISPLAY_INT w, WEBC_INT8* colFlex, int numCol)
 	{
 		parentWidth = w;
 		columnFlex = colFlex;
		numColumns = numCol;
 	}

 	~SpecifiedWidthMethod (void) {}

	DISPLAY_INT calculate (HTMLTableCell* td)
	{
		HTMLTableCellDisplay* cellDisplay;

		cellDisplay = (HTMLTableCellDisplay *) td->GetDisplayElement();

		if (cellDisplay)
		{
			HTMLLength cellWidth = td->CellWidth();

			switch (HTML_LengthGetUnit(cellWidth))
			{
				case HTML_LENGTH_UNIT_NONE:
				case HTML_LENGTH_UNIT_STAR:
				{
					int colIndex = td->ColIndex();
					int colSpan = td->ColSpan();

					if (colSpan + colIndex > numColumns)
					{
						colSpan = numColumns - colIndex;
					}

					for (int c = colIndex; c < colIndex + colSpan; c++)
					{
						if (columnFlex[c] == FLEX_LEVEL_AUTO_ONLY)
						{
							return (cellDisplay->GetMaxWidth());
						}
					}

					return (cellDisplay->GetMinWidth());
				}

				case HTML_LENGTH_UNIT_PIXELS:
				case HTML_LENGTH_UNIT_PERCENT:
					return (HTML_LengthToPixels(cellWidth, parentWidth));
			}
		}

		return (0);
	}
};

/*---------------------------------------------------------------------------*/
// Natural case: give every cell its specified width (or min width, if that is
//  greater);
//    if no width is specified for the cell:
//      if any the columns the cell spans also specify no width
//         natural width is the max format width of the cell
//      else
//         natural width is the min width of the cell
class NaturalWidthMethod : public ITableCellWidthMethod
{
 private:
 	DISPLAY_INT parentWidth;
 	WEBC_INT8*  columnFlex;
	int         numColumns;

 public:
 	NaturalWidthMethod (DISPLAY_INT w, WEBC_INT8* colFlex, int numCol)
 	{
 		parentWidth = w;
 		columnFlex = colFlex;
		numColumns = numCol;
 	}

 	~NaturalWidthMethod (void) {}

	DISPLAY_INT calculate (HTMLTableCell* td)
	{
		HTMLTableCellDisplay* cellDisplay;

		cellDisplay = (HTMLTableCellDisplay *) td->GetDisplayElement();

		if (cellDisplay)
		{
			HTMLLength cellWidth = td->CellWidth();

			switch (HTML_LengthGetUnit(cellWidth))
			{
				case HTML_LENGTH_UNIT_NONE:
				case HTML_LENGTH_UNIT_STAR: // tbd - handle star units correctly
				{
					int colIndex = td->ColIndex();
					int colSpan = td->ColSpan();

					if (colSpan + colIndex > numColumns)
					{
						colSpan = numColumns - colIndex;
					}

					for (int c = colIndex; c < colIndex + colSpan; c++)
					{
						if (columnFlex[c] == FLEX_LEVEL_AUTO_ONLY)
						{
							return (cellDisplay->GetMaxWidth());
						}
					}

					return (cellDisplay->GetMinWidth());
				}

				case HTML_LENGTH_UNIT_PIXELS:
				case HTML_LENGTH_UNIT_PERCENT:
				{
					DISPLAY_INT cellWidthValue = HTML_LengthToPixels(cellWidth, parentWidth);
					DISPLAY_INT cellMinWidth = cellDisplay->GetMinWidth();
					return (EBSMAX(cellWidthValue, cellMinWidth));
				}
                default:
                    break;
			}
		}

		return (0);
	}
};

#endif // WEBC_SUPPORT_TABLES

/*****************************************************************************/
// Global Data
/*****************************************************************************/

/*****************************************************************************/
// Local functions
/*****************************************************************************/

/*****************************************************************************/
// HTMLTableDisplay class methods
/*****************************************************************************/

#if (WEBC_SUPPORT_TABLES)

HTMLTableDisplay::HTMLTableDisplay(HTMLTable * t)
	: HTMLElementDisplay<DisplayElement> (t), mBorder(t)
{
	mFormatted = TU_FORMAT_INVALID;
	miMaxWidth = 0;
	miMinWidth = 0;
	mTableFlags = 0;

	SetTableElement(t);
}

HTMLTableDisplay::~HTMLTableDisplay()
{
}

void HTMLTableDisplay::SetTableElement(HTMLTable * t)
{
	mpTable = t;
}

DisplayOverflow HTMLTableDisplay::GetOverflow(void)
{
	return (DisplayElement::GetOverflow());
}

HTMLEventStatus HTMLTableDisplay::Event(HTMLEvent * event)
{
    WEBC_DEBUG_ReceiveEvent(this, event);
	return HTML_EVENT_STATUS_DONE;
}

void HTMLTableDisplay::Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC)
{
	if (Formatted() != TU_FORMAT_VALID)
	{
		Format();
	}

	DisplayElement::Draw(x, y, pViewport, pGC);
}

void HTMLTableDisplay::DrawThisOnly(DISPLAY_INT x, DISPLAY_INT y, WebGraphics * gc)
{
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLTABLEDISPLAY_DRAWTHISONLY);

	if (GetVisibility() == TU_VISIBILITY_HIDE)
	{
		WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLTABLEDISPLAY_DRAWTHISONLY);
		return;
	}

	WebRect screenRect(mRect);
	screenRect.MoveTo(x,y);

	DrawBackground(&screenRect, &screenRect, gc);

	WebRect box(mRect);
	box.MoveTo(x,y);
	mBorder.DrawBorder(&box, gc);

	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLTABLEDISPLAY_DRAWTHISONLY);
}

void HTMLTableDisplay::DrawBackground(WebRect *fillRect, WebRect *viewRect, WebGraphics *pGC)
{
	// Draw the background color
	HTMLColor rgbColor;
	rgbColor = mpTable->GetBackgroundColor();
	if (HTML_ColorGetAlpha(rgbColor) != 0xff)
	{
		WebColor Color = pGC->RGBToColor(rgbColor);
		pGC->Rectangle(fillRect,Color,Color,WEBC_TRUE);
	}

  #if (WEBC_SUPPORT_BACKGROUND_IMAGES)

	WebcImage * bitmap = mpTable->GetBackgroundImage();
	if (bitmap && (bitmap->State() == WEBIMAGE_OKAY))
	{
		WebRect bitmapRect;
		int offX, offY;
		offX = 0;
		offY = 0;
		/*
		mpTable->GetBitmapOffset(&offX, &offY);

		if (mpTable->GetBlockFlags() & HTMLFLOW_FLAG_BMPXISPERCENT)
		{
			offX = offX * (fillRect->Width() - bitmap->Width()) / 100;
		}
		if (mpTable->GetBlockFlags() & HTMLFLOW_FLAG_BMPYISPERCENT)
		{
			offY = offY * (fillRect->Height() - bitmap->Height()) / 100;
		}

		if (mpTable->GetBlockFlags() & HTMLFLOW_FLAG_BMPNOSCROLL)
		{
			offX -= viewRect->left - fillRect->left;
			offY -= viewRect->top - fillRect->top;
		}

		// if repeat isn't set, the box needs to be clipped to the bitmap's width or height

		if (mpTable->GetBlockFlags() & HTMLFLOW_FLAG_BMPREPEATX)
		{
			bitmapRect.Set(fillRect->left, bitmapRect.top, fillRect->right, bitmapRect.bottom);
		}
		else
		{
			bitmapRect.Set(fillRect->left + offX, bitmapRect.top,
			               fillRect->left + offX + bitmap->Width() - 1, bitmapRect.bottom);
			offX = 0;
		}

		if (mpTable->GetBlockFlags() & HTMLFLOW_FLAG_BMPREPEATY)
		{
			bitmapRect.Set(bitmapRect.left, fillRect->top, bitmapRect.right, fillRect->bottom);
		}
		else
		{
			bitmapRect.Set(bitmapRect.left, fillRect->top + offY,
			               bitmapRect.right, fillRect->top + offY + bitmap->Height() - 1);
			offY = 0;
		}
		*/
		bitmapRect.Set(fillRect);

		// Draw the bitmap fill
		WebcImageFrame *f = bitmap->DrawTiled(0, &bitmapRect, offX, offY, pGC);
		if (f)
		{
			bitmap->ReleaseFrame(f);
		}
	}

  #endif // WEBC_SUPPORT_BACKGROUND_IMAGES
}

WEBC_BOOL HTMLTableDisplay::MinWidthValid (void)
{
	return ((mTableFlags & TD_FLAG_MINWIDTH_VALID) != 0);
}

WEBC_BOOL HTMLTableDisplay::MaxWidthValid (void)
{
	return ((mTableFlags & TD_FLAG_MAXWIDTH_VALID) != 0);
}

DISPLAY_INT HTMLTableDisplay::GetMinWidth(void)
{
	if (!MinWidthValid())
	{
		DISPLAY_INT iMinWidth, iSpacingWidth;
		DISPLAY_INT *pMinStart;
		int r, c, iNumColumns, iNumRows, iColumn, iColSpan;
		HTMLTableCell **ppRow;
		HTMLTableRow *pRow;
		HTMLTableCellDisplay *pCellDisplay;

		if (mpTable->CalculateCellPositions() < 0)
		{
			return (0);
		}

		miMinWidth = 0;

		iNumColumns = mpTable->GetNumColumns();
		iNumRows = mpTable->GetNumRows();
		if(!iNumColumns || !iNumRows)
		{
			return (0);
		}

		////////////////////////////////////////////////////////////////////////////////////////////////
		// Malloc the tables we'll need for width calculations
		pMinStart   = (DISPLAY_INT *) WEBC_MALLOC(sizeof(DISPLAY_INT) * (iNumColumns+1));
		ppRow = (HTMLTableCell **) WEBC_MALLOC(sizeof(HTMLTableCell *) * iNumRows);

		if (!pMinStart || !ppRow)
		{
			goto clean_up_and_return;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////
		// Calculate Column Widths

		// Set up row pointers

		for (pRow = mpTable->FirstRow(), r = 0;
		     r < iNumRows && pRow;
			 r++, pRow = pRow->NextRow())
		{
			ppRow[r] = pRow->FirstCell();
		}

		// column 0 always starts at pixel 0
		pMinStart[0] = 0;

		for (c=1; c<=iNumColumns; c++)
		{
			// start this column at the beginning of the last one (width=0)
			pMinStart[c] = pMinStart[c-1];

			for (r=0; r<iNumRows; r++)
			{
				if (ppRow[r])
				{
					iColumn = ppRow[r]->ColIndex();
					iColSpan = EBSMIN(ppRow[r]->ColSpan(), iNumColumns - iColumn);
					if (iColumn + iColSpan == c)
					{
						// Find Min width
						pCellDisplay = (HTMLTableCellDisplay *) ppRow[r]->GetDisplayElement();
						if (pCellDisplay)
						{
							// We need to set the parent display here so that elements needing to get at the graphics
							//  context can report the correct dimensions
							pCellDisplay->SetParentDisplay(this, 0);

							iMinWidth = pCellDisplay->GetMinWidth();

							// Account for the pixels taken up by cell spacing within the span of this cell
							iSpacingWidth = mpTable->GetCellSpacing() * (iColSpan-1);

							// Update column starts for Min
							pMinStart[c] = EBSMAX(pMinStart[c], pMinStart[iColumn] + iMinWidth - iSpacingWidth);
						}

						// Go to next cell in row
						ppRow[r] = ppRow[r]->NextCell();
					}
				}
			}

			// At this point pMinStart[c] is correct
		}
		// At this point, pMinStart[iNumColumns] == the total width
		miMinWidth = pMinStart[iNumColumns];
		miMinWidth += mpTable->TableBorder(CSS_LEFT) + mpTable->TableBorder(CSS_RIGHT) + mpTable->GetCellSpacing()*(iNumColumns+1);

		// if width is in pixels, then min is at least width
		if (HTML_LengthGetUnit(mpTable->TableWidth()) == HTML_LENGTH_UNIT_PIXELS)
		{
			miMinWidth = EBSMAX(miMinWidth, HTML_LengthGetValue(mpTable->TableWidth()));
		}

		// Set minwidth valid
		mTableFlags |= TD_FLAG_MINWIDTH_VALID;

clean_up_and_return:
		if (pMinStart) { WEBC_FREE(pMinStart); }
		if (ppRow)     { WEBC_FREE(ppRow); }
	}

	return (miMinWidth);
}

DISPLAY_INT HTMLTableDisplay::GetMaxWidth(void)
{
	if (!MaxWidthValid())
	{
		// if width is in pixels, then max is MAX(min, width)
		if (HTML_LengthGetUnit(mpTable->TableWidth()) == HTML_LENGTH_UNIT_PIXELS)
		{
			miMaxWidth = EBSMAX(GetMinWidth(), HTML_LengthGetValue(mpTable->TableWidth()));

			// Set maxwidth valid
			mTableFlags |= TD_FLAG_MAXWIDTH_VALID;

			return (miMaxWidth);
		}

		if (mpTable->CalculateCellPositions() < 0)
		{
			return (0);
		}

		DISPLAY_INT iMaxWidth, iSpacingWidth;
		DISPLAY_INT *pMaxStart;
		int r, c, iNumColumns, iNumRows, iColumn, iColSpan;
		HTMLTableCell **ppRow;
		HTMLTableRow *pRow;
		HTMLTableCellDisplay *pCellDisplay;

		miMaxWidth = 0;

		iNumColumns = mpTable->GetNumColumns();
		iNumRows = mpTable->GetNumRows();
		if(!iNumColumns || !iNumRows)
		{
			return (0);
		}

		////////////////////////////////////////////////////////////////////////////////////////////////
		// Malloc the tables we'll need for width calculations
		pMaxStart   = (DISPLAY_INT *) WEBC_MALLOC(sizeof(DISPLAY_INT) * (iNumColumns+1));
		ppRow = (HTMLTableCell **) WEBC_MALLOC(sizeof(HTMLTableCell *) * iNumRows);

		if (!pMaxStart || !ppRow)
		{
			goto clean_up_and_return;
		}

		////////////////////////////////////////////////////////////////////////////////////////////////
		// Calculate Column Widths

		// Set up row pointers

		for (pRow = mpTable->FirstRow(), r = 0; pRow; r++, pRow = pRow->NextRow())
		{
			ppRow[r] = pRow->FirstCell();
		}

		// column 0 always starts at pixel 0
		pMaxStart[0] = 0;

		for (c=1; c<=iNumColumns; c++)
		{
			// start this column at the beginning of the last one (width=0)
			pMaxStart[c] = pMaxStart[c-1];

			for (r=0; r<iNumRows; r++)
			{
				if (ppRow[r])
				{
					iColumn = ppRow[r]->ColIndex();
					iColSpan = EBSMIN(ppRow[r]->ColSpan(), iNumColumns - iColumn);
					if (iColumn + iColSpan == c)
					{
						// Find Max width
						pCellDisplay = (HTMLTableCellDisplay *) ppRow[r]->GetDisplayElement();
						if (pCellDisplay)
						{
							// We need to set the parent display here so that elements needing to get at the graphics
							//  context can report the correct dimensions
							pCellDisplay->SetParentDisplay(this, 0);

							iMaxWidth = pCellDisplay->GetMaxWidth();

							HTMLLength htmlCellWidth = ppRow[r]->CellWidth();

							if (HTML_LengthGetUnit(htmlCellWidth) == HTML_LENGTH_UNIT_PERCENT &&
							    HTML_LengthGetValue(htmlCellWidth) > 0)
							{
								miMaxWidth = EBSMAX(miMaxWidth, (iMaxWidth * 100)/(HTML_LengthGetValue(htmlCellWidth)));
								miMaxWidth = EBSMIN(9999, miMaxWidth);
							}

							// Account for the pixels taken up by cell spacing within the span of this cell
							iSpacingWidth = mpTable->GetCellSpacing() * (iColSpan-1);

							// Update column starts for Max
							pMaxStart[c] = EBSMAX(pMaxStart[c], pMaxStart[iColumn] + iMaxWidth - iSpacingWidth);
						}

						// Go to next cell in row
						ppRow[r] = ppRow[r]->NextCell();
					}
				}
			}

			// At this point pMaxStart[c] is correct
		}
		// At this point, pMaxStart[iNumColumns] == the total width
		miMaxWidth = EBSMAX(miMaxWidth, pMaxStart[iNumColumns]);
		miMaxWidth += mpTable->TableBorder(CSS_LEFT) + mpTable->TableBorder(CSS_RIGHT) + mpTable->GetCellSpacing()*(iNumColumns+1);

		// Set maxwidth valid
		mTableFlags |= TD_FLAG_MAXWIDTH_VALID;

clean_up_and_return:
		if (pMaxStart) { WEBC_FREE(pMaxStart); }
		if (ppRow)     { WEBC_FREE(ppRow); }
	}

	return (miMaxWidth);
}

DISPLAY_INT HTMLTableDisplay::GetPixelWidth(void)
{
	if (Formatted() != TU_FORMAT_VALID)
	{
		Format();
	}

	return (Width());
}

DISPLAY_INT HTMLTableDisplay::GetPixelHeight(void)
{
	if (Formatted() != TU_FORMAT_VALID)
	{
		Format();
	}

	return (Height());
}

DISPLAY_INT HTMLTableDisplay::GetBaseline(void)
{
	return (GetPixelHeight());
}

void HTMLTableDisplay::SetParentWidth(DISPLAY_INT iTotal, DISPLAY_INT iAvailable)
{
DISPLAY_INT iTableWidth;

	mBorder.SetParentWidth(iTotal);

	////////////////////////////////////////////////////////////////////////////////////////////////
	// Calculate Table Width available to cells

	switch (HTML_LengthGetUnit(mpTable->TableWidth()))
	{
		case HTML_LENGTH_UNIT_PERCENT:
			iTableWidth = (iTotal * HTML_LengthGetValue(mpTable->TableWidth()))/100;
			break;

		case HTML_LENGTH_UNIT_PIXELS:
			iTableWidth = HTML_LengthGetValue(mpTable->TableWidth());
			break;

		case HTML_LENGTH_UNIT_NONE:
		default:
			iTableWidth = EBSMIN(iAvailable, GetMaxWidth());
			break;
	};
	iTableWidth = EBSMAX(GetMinWidth(), iTableWidth);

	if (iTableWidth != Width())
	{
		SetWidth(iTableWidth);
		SetFormatStatus(TU_FORMAT_INVALID);
	}
}

void HTMLTableDisplay::SetParentHeight(DISPLAY_INT iHeight)
{
}

/*---------------------------------------------------------------------------*/
IPositionedFormatContext* HTMLTableDisplay::FormatSelfPositioned (
		DisplayElement* displayParent,
		IPositionedFormatContext* rootContext,
		IPositionedFormatContext* parentContext,
		FormatContextFactory* childContextFactory,
		WEBC_BOOL generateChildContext
	)
{
	WebRect rect;
	WEBC_UINT16 position = GetPosition();
	IPositionedFormatContext* childContext = 0;

	if (position == TU_POSITION_RELATIVE)
	{
		return (HTMLElementDisplay<DisplayElement>::FormatSelfPositioned (
				displayParent,
				rootContext,
				parentContext,
				childContextFactory,
				generateChildContext
			));
	}
	else
	{
		WebRect containingBlock;
		IPositionedFormatContext* containingContext;
		DISPLAY_INT borderLeftValue;
		DISPLAY_INT borderTopValue;
		DISPLAY_INT borderRightValue;
		DISPLAY_INT borderBottomValue;

		if (displayParent)
		{
			if (displayParent != mpParent)
			{
				mHtmlElementDisplayFlags &= ~HELEM_DISPLAY_FLAG_STYLE_SET;
			}
			displayParent->InsertOrdered(this);
		}

		if (position == TU_POSITION_FIXED)
		{
			SetFixedPosition(WEBC_TRUE);
			containingContext = rootContext;
		}
		else
		{
			SetFixedPosition(WEBC_FALSE);
			containingContext = parentContext;
		}

		containingContext->getContainingBlock(&containingBlock);

		WEBC_BOOL   parentHeightIsAuto = containingContext->parentHeightIsAuto();
		int         textDirection      = containingContext->getTextDirection();
		DISPLAY_INT parentBorderLeft   = containingContext->getParentBorderLeft();
		DISPLAY_INT parentBorderTop    = containingContext->getParentBorderTop();
		DISPLAY_INT parentBorderRight  = containingContext->getParentBorderRight();
		DISPLAY_INT parentBorderBottom = containingContext->getParentBorderBottom();

		DISPLAY_INT autoIndentLeft     = parentContext->getAutoIndentLeft();
		DISPLAY_INT autoIndentTop      = parentContext->getAutoIndentTop();
		DISPLAY_INT autoIndentRight    = parentContext->getAutoIndentRight();

		if (position == TU_POSITION_FIXED)
		{
			WebRect parentRect;

			parentContext->getContainingBlock(&parentRect);

			autoIndentLeft  += parentRect.left;
			autoIndentTop   += parentRect.top;
			autoIndentRight += (containingBlock.right - parentRect.right);
		}

		DISPLAY_INT parentWidth = containingBlock.Width() - (parentBorderLeft + parentBorderRight);
		DISPLAY_INT parentHeight = containingBlock.Height() - (parentBorderTop + parentBorderBottom);

		WebFont font = mpHtmlElement->GetWebFont();
		DISPLAY_INT emHeight = font? WEB_FONT_HEIGHT(font) : WEBC_CFG_DEFAULT_TEXT_HEIGHT;
		DISPLAY_INT exHeight = emHeight >> 1;

		// First find the left edge and width

		CSSLength left;
		CSSLength right;
		CSSLength width;

		// Find value for left, right, width, marginLeft, marginRight

		GetCSSLength(&left,  CSS_PROPERTY_LEFT);
		GetCSSLength(&right, CSS_PROPERTY_RIGHT);
		GetCSSLength(&width, CSS_PROPERTY_WIDTH);

		DISPLAY_INT leftValue;
		DISPLAY_INT rightValue;
		DISPLAY_INT widthValue;
		DISPLAY_INT marginLeftValue;
		DISPLAY_INT paddingLeftValue;
		DISPLAY_INT paddingTopValue;
		DISPLAY_INT paddingRightValue;
		DISPLAY_INT paddingBottomValue;

		// Find value for padding and border

		paddingLeftValue   = GetPixelLength(CSS_PROPERTY_PADDING_LEFT,   parentWidth, emHeight, exHeight, 0);
		paddingTopValue    = GetPixelLength(CSS_PROPERTY_PADDING_TOP,    parentWidth, emHeight, exHeight, 0);
		paddingRightValue  = GetPixelLength(CSS_PROPERTY_PADDING_RIGHT,  parentWidth, emHeight, exHeight, 0);
		paddingBottomValue = GetPixelLength(CSS_PROPERTY_PADDING_BOTTOM, parentWidth, emHeight, exHeight, 0);

		borderLeftValue    = GetBorderWidth(CSS_LEFT,   parentWidth, emHeight, exHeight, parentBorderLeft);
		borderTopValue     = GetBorderWidth(CSS_TOP,    parentWidth, emHeight, exHeight, parentBorderTop);
		borderRightValue   = GetBorderWidth(CSS_RIGHT,  parentWidth, emHeight, exHeight, parentBorderRight);
		borderBottomValue  = GetBorderWidth(CSS_BOTTOM, parentWidth, emHeight, exHeight, parentBorderBottom);

		CSSLength height;
		CSSLength top;
		CSSLength bottom;
		DISPLAY_INT heightValue = 0;
		WEBC_BOOL heightIsAuto = 0;

		GetCSSLength(&height, CSS_PROPERTY_HEIGHT);
		GetCSSLength(&top, CSS_PROPERTY_TOP);

		WEBC_UINT8 verticalCase;

		verticalCase =  (height.type == CSS_LENGTH_UNIT_AUTO ||
		                 (height.type == CSS_LENGTH_UNIT_PERCENT && parentHeightIsAuto))? 0 : HEIGHT_KNOWN;
		verticalCase |= (top.type == CSS_LENGTH_UNIT_AUTO)? 0 : TOP_KNOWN;

		if (verticalCase != (HEIGHT_KNOWN|TOP_KNOWN))
		{
			GetCSSLength(&bottom, CSS_PROPERTY_BOTTOM);
			verticalCase |= (bottom.type == CSS_LENGTH_UNIT_AUTO || parentHeightIsAuto)? 0 : BOTTOM_KNOWN;
		}

		DISPLAY_INT topValue = 0;
		DISPLAY_INT bottomValue = 0;
		DISPLAY_INT marginTopValue = 0;
		DISPLAY_INT marginBottomValue = 0;

		switch (verticalCase)
		{
			case NOTHING_KNOWN:
				// top = auto indent top
				// height = content height
				topValue = autoIndentTop;
				heightValue = 0;
				heightIsAuto = WEBC_TRUE;
				marginTopValue = GetPixelLength(CSS_PROPERTY_MARGIN_TOP, parentWidth, emHeight, exHeight, 0);
				break;

			case HEIGHT_KNOWN:
				// top = auto indent top
				// height = height
				topValue = autoIndentTop;
				heightValue = CSS_LengthToPixels(&height, parentHeight, emHeight, exHeight);
				heightIsAuto = WEBC_FALSE;
				marginTopValue = GetPixelLength(CSS_PROPERTY_MARGIN_TOP, parentWidth, emHeight, exHeight, 0);
				break;

			case TOP_KNOWN:
				// top = top
				// height = content height
				if (top.type == CSS_LENGTH_UNIT_PERCENT && parentHeightIsAuto)
				{
					topValue = autoIndentTop;
				}
				else
				{
					topValue = CSS_LengthToPixels(&top, parentHeight, emHeight, exHeight);
				}
				heightValue = 0;
				heightIsAuto = WEBC_TRUE;
				marginTopValue = GetPixelLength(CSS_PROPERTY_MARGIN_TOP, parentWidth, emHeight, exHeight, 0);
				break;

			case HEIGHT_KNOWN|TOP_KNOWN:
				// top = top
				// height = height
				if (top.type == CSS_LENGTH_UNIT_PERCENT && parentHeightIsAuto)
				{
					topValue = autoIndentTop;
				}
				else
				{
					topValue = CSS_LengthToPixels(&top, parentHeight, emHeight, exHeight);
				}
				heightValue = CSS_LengthToPixels(&height, parentHeight, emHeight, exHeight);
				heightIsAuto = WEBC_FALSE;
				marginTopValue = GetPixelLength(CSS_PROPERTY_MARGIN_TOP, parentWidth, emHeight, exHeight, 0);
				break;

			case BOTTOM_KNOWN:
				// bottom = bottom
				// height = content height
				// top = parentHeight - (height + bottom)
				bottomValue = GetPixelLength(&bottom, parentHeight, emHeight, exHeight, 0);
				marginBottomValue = GetPixelLength(CSS_PROPERTY_MARGIN_BOTTOM, parentWidth, emHeight, exHeight, 0);
				heightValue = 0;
				heightIsAuto = WEBC_TRUE;
				break;

			case HEIGHT_KNOWN|BOTTOM_KNOWN:
				// bottom = bottom
				// height = height
				// top = parentHeight - (height + bottom)
				bottomValue = GetPixelLength(&bottom, parentHeight, emHeight, exHeight, 0);
				heightValue = CSS_LengthToPixels(&height, parentHeight, emHeight, exHeight);
				heightIsAuto = WEBC_FALSE;

				marginBottomValue = GetPixelLength(CSS_PROPERTY_MARGIN_BOTTOM, parentWidth, emHeight, exHeight, 0);

				topValue = parentHeight -
			               (borderTopValue +
			                paddingTopValue +
			                heightValue +
			                paddingBottomValue +
			                borderBottomValue +
			                marginBottomValue +
			                bottomValue);
				break;

			case TOP_KNOWN|BOTTOM_KNOWN:
				// top = top
				// bottom = bottom
				// height = parentHeight - (top + bottom)
				if (top.type == CSS_LENGTH_UNIT_PERCENT && parentHeightIsAuto)
				{
					topValue = autoIndentTop;
				}
				else
				{
					topValue = CSS_LengthToPixels(&top, parentHeight, emHeight, exHeight);
				}
				bottomValue = GetPixelLength(&bottom, parentHeight, emHeight, exHeight, 0);

				marginTopValue = GetPixelLength(CSS_PROPERTY_MARGIN_TOP, parentWidth, emHeight, exHeight, 0);
				marginBottomValue = GetPixelLength(CSS_PROPERTY_MARGIN_BOTTOM, parentWidth, emHeight, exHeight, 0);

				heightValue = parentHeight -
				              (topValue +
							   marginTopValue +
							   borderTopValue +
							   paddingTopValue +
				               paddingBottomValue +
							   borderBottomValue +
							   marginBottomValue +
							   bottomValue);
				heightIsAuto = WEBC_FALSE;
				break;
            default:
                break;
		}

		if (width.type != CSS_LENGTH_UNIT_AUTO)
		{
			widthValue = CSS_LengthToPixels(&width, parentWidth, emHeight, exHeight);

			WEBC_BOOL leftAligned = (textDirection == TU_DIR_RTL)?
			                            (right.type == CSS_LENGTH_UNIT_AUTO && left.type  != CSS_LENGTH_UNIT_AUTO) :
										(left.type  != CSS_LENGTH_UNIT_AUTO || right.type == CSS_LENGTH_UNIT_AUTO);

			if (leftAligned)
			{
				// ignore right and margin-right
				leftValue = GetPixelLength(&left, parentWidth, emHeight, exHeight, autoIndentLeft);
				marginLeftValue = GetPixelLength(CSS_PROPERTY_MARGIN_LEFT, parentWidth, emHeight, exHeight, 0);
			}
			else
			{
				DISPLAY_INT marginRightValue;

				// ignore left and margin-left
				rightValue = GetPixelLength(&right, parentWidth, emHeight, exHeight, autoIndentRight);
				marginRightValue = GetPixelLength(CSS_PROPERTY_MARGIN_RIGHT, parentWidth, emHeight, exHeight, 0);
				leftValue = parentWidth -
				            (rightValue +
							 marginRightValue +
							 borderRightValue +
							 paddingRightValue +
				             widthValue +
							 paddingLeftValue +
							 borderLeftValue);

				marginLeftValue = 0;
			}
		}
		else
		{
			// width.type == CSS_LENGTH_UNIT_AUTO

			// try to get the intrinsic width of the element.  This might rely on the
			//  calculated height of the element; for example, in the case of an image with
			//  specified height and auto width, the aspect ratio of the source image determines
			//  the resulting intrinsic width.

			leftValue = GetPixelLength(&left,  parentWidth, emHeight, exHeight, autoIndentLeft);
			marginLeftValue = GetPixelLength(CSS_PROPERTY_MARGIN_LEFT,  parentWidth, emHeight, exHeight, 0);

			if ((right.type != CSS_LENGTH_UNIT_AUTO && left.type != CSS_LENGTH_UNIT_AUTO) ||
			    !GetIntrinsicWidth(&widthValue, heightValue, heightIsAuto))
			{
				DISPLAY_INT marginRightValue;

				// element has no intrinsic width:
				//  calculate left, right, and margins then extrapolate width
				rightValue = GetPixelLength(&right, parentWidth, emHeight, exHeight, autoIndentRight);
				marginRightValue = GetPixelLength(CSS_PROPERTY_MARGIN_RIGHT, parentWidth, emHeight, exHeight, 0);

				widthValue = parentWidth -
				             (leftValue +
							  marginLeftValue +
							  borderLeftValue +
							  paddingLeftValue +
				              paddingRightValue +
							  borderRightValue +
							  marginRightValue +
							  rightValue);

				// we don't want to stretch the width beyond what the maximum content
				//  width; only give the element as much space as it actually needs.
				if (right.type == CSS_LENGTH_UNIT_AUTO || left.type == CSS_LENGTH_UNIT_AUTO)
				{
					// to speed this up, bail out of GetMaxWidth when we exceed widthValue
					DISPLAY_INT maxWidth = GetMaxWidthSmallerThan(widthValue);

					// NOTE: GetMaxWidth and GetMaxWidthSmallerThan return the total width
					//  of the element, including borders and padding.  So we must subtract
					//  borders and padding to make it correspond to the contentWidth

					maxWidth -= (borderLeftValue + paddingLeftValue + paddingRightValue + borderRightValue);

					if (maxWidth < widthValue)
					{
						WEBC_BOOL leftAligned = (textDirection == TU_DIR_RTL)?
													(right.type == CSS_LENGTH_UNIT_AUTO && left.type  != CSS_LENGTH_UNIT_AUTO) :
													(left.type  != CSS_LENGTH_UNIT_AUTO || right.type == CSS_LENGTH_UNIT_AUTO);

						if (!leftAligned)
						{
							// box is right-aligned
							//  adjust leftValue to account for the unused space
							leftValue += (widthValue - maxWidth);
						}
						widthValue = maxWidth;
					}
				}
			}
		}

		// Now format the content of this element

		DISPLAY_INT contentHeight = FormatContentPositioned (
				widthValue,
				heightValue,
				heightIsAuto,
				borderLeftValue,
				borderTopValue,
				borderRightValue,
				borderBottomValue,
				paddingLeftValue,
				paddingTopValue,
				paddingRightValue,
				paddingBottomValue
			);

		// Find the top edge and height

		//  NOTE: top and bottom percentages are relative to parent's HEIGHT, whereas
		//   margin-top and margin-bottom are relative to parent's WIDTH, except in the root
		//   context (tbd - handle this special case correctly)

		switch (verticalCase)
		{
			case NOTHING_KNOWN:
			case TOP_KNOWN:
				heightValue = contentHeight;
				break;

			case BOTTOM_KNOWN:
				// height = content height
				// top = parentHeight - (height + bottom)
				heightValue = contentHeight;
				topValue = parentHeight -
			               (borderTopValue +
			                paddingTopValue +
			                heightValue +
			                paddingBottomValue +
			                borderBottomValue +
			                marginBottomValue +
			                bottomValue);
				break;
		}

		rect.Set (0, 0,
				borderLeftValue + paddingLeftValue + widthValue  + paddingRightValue  + borderRightValue - 1,
				borderTopValue  + paddingTopValue  + heightValue + paddingBottomValue + borderBottomValue - 1
			);

		rect.MoveTo (
				parentBorderLeft + leftValue + marginLeftValue,
				parentBorderTop + topValue  + marginTopValue
			);

		if (position == TU_POSITION_FIXED)
		{
			rect.Shift(containingBlock.left, containingBlock.top);
		}

		Move(&rect);

		if (generateChildContext)
		{
			childContext = childContextFactory->newPositioned (
					&rect,
					heightIsAuto,
					TU_DIR_LTR,
					borderLeftValue,
					borderTopValue,
					borderRightValue,
					borderBottomValue,
					paddingLeftValue,
					paddingTopValue,
					paddingRightValue
				);
		}
	}

	return (childContext);
}

/*---------------------------------------------------------------------------*/
IPositionedFormatContext* HTMLTableDisplay::FormatPositioned (
		DisplayElement* displayParent,
		IPositionedFormatContext* rootContext,
		IPositionedFormatContext* parentContext,
		FormatContextFactory* childContextFactory
	)
{
	IPositionedFormatContext* generatedContext;

	generatedContext = HTMLElementDisplay<DisplayElement>::FormatPositioned (
			displayParent,
			rootContext,
			parentContext,
			childContextFactory
		);

	return (generatedContext);
}

/*---------------------------------------------------------------------------*/
void HTMLTableDisplay::Format(void)
{
	FormatContentPositioned (
			Width(),
			Height(),
			WEBC_TRUE, // heightIsAuto
			mBorder.GetBorderWidth(CSS_LEFT),
			mBorder.GetBorderWidth(CSS_TOP),
			mBorder.GetBorderWidth(CSS_RIGHT),
			mBorder.GetBorderWidth(CSS_BOTTOM),
			mBorder.GetPaddingWidth(CSS_LEFT),
			mBorder.GetPaddingWidth(CSS_TOP),
			mBorder.GetPaddingWidth(CSS_RIGHT),
			mBorder.GetPaddingWidth(CSS_BOTTOM)
		);

	SetFormatStatus(TU_FORMAT_VALID);
}

/*---------------------------------------------------------------------------*/
DISPLAY_INT HTMLTableDisplay::FormatContentPositioned (
		DISPLAY_INT widthValue,
		DISPLAY_INT heightValue,
		WEBC_BOOL   heightIsAuto,
		DISPLAY_INT borderLeftValue,
		DISPLAY_INT borderTopValue,
		DISPLAY_INT borderRightValue,
		DISPLAY_INT borderBottomValue,
		DISPLAY_INT paddingLeftValue,
		DISPLAY_INT paddingTopValue,
		DISPLAY_INT paddingRightValue,
		DISPLAY_INT paddingBottomValue
	)
{
	if (mpTable->CalculateCellPositions() < 0)
	{
		return (0);
	}

	int numColumns = mpTable->GetNumColumns();
	int numRows = mpTable->GetNumRows();
	if (numColumns <= 0 || numRows <= 0)
	{
		return (0);
	}

	DISPLAY_INT formattedHeight = 0;
	DISPLAY_INT cellspacing = mpTable->GetCellSpacing();

	DISPLAY_INT targetWidth =
			widthValue -
			(borderLeftValue +
			 paddingLeftValue +
			 cellspacing * (numColumns - 1) +
			 paddingRightValue +
			 borderRightValue);

	////////////////////////////////////////////////////////////////////////////////////////////////
	// Calculate Column Widths

	AttachTableCells();

	WEBC_BOOL columnWidthsValid = WEBC_FALSE;
	WEBC_BOOL tableHasSpanningCells = HasSpanningCells(numColumns);

	ALLOC_AUTO_ARRAY(DISPLAY_INT, columnWidth, numColumns);
	ALLOC_AUTO_ARRAY(WEBC_INT8, columnFlex, numColumns);

	WEBC_INT8 highestFlex = CalculateColumnFlexibility(columnFlex, numColumns, WEBC_TRUE);

	if (!tableHasSpanningCells)
	{
		ALLOC_AUTO_ARRAY(DISPLAY_INT, naturalColumnWidth, numColumns);
		ALLOC_AUTO_ARRAY(DISPLAY_INT, naturalStart, numColumns + 1);

		NaturalWidthMethod naturalMethod(targetWidth, columnFlex, numColumns);
		FindSingleColumnWidths(naturalColumnWidth, numColumns, &naturalMethod);
		WidthsToStarts(naturalStart, naturalColumnWidth, numColumns);

		DISPLAY_INT naturalWidth = naturalStart[numColumns];

		if (targetWidth == naturalWidth)
		{
			tc_memcpy(columnWidth, naturalColumnWidth, sizeof(DISPLAY_INT) * numColumns);
			columnWidthsValid = WEBC_TRUE;
		}
		else if (targetWidth > naturalWidth)
		{
			ExpandColumnsAtFlexLevel (
					columnWidth,
					naturalStart,
					columnFlex,
					numColumns,
					highestFlex,
					targetWidth
				);

			columnWidthsValid = WEBC_TRUE;
		}
		else
		{
			// optimization: only calculate absMinWidth case if needed
			ALLOC_AUTO_ARRAY(DISPLAY_INT, absMinColumnWidth, numColumns);
			ALLOC_AUTO_ARRAY(DISPLAY_INT, absMinStart, numColumns + 1);

			AbsoluteMinWidthMethod absMinMethod;
			FindSingleColumnWidths(absMinColumnWidth, numColumns, &absMinMethod);
			WidthsToStarts(absMinStart, absMinColumnWidth, numColumns);

			DISPLAY_INT absMinWidth = absMinStart[numColumns];

			DHTABLE_ASSERT(absMinWidth <= naturalWidth);

			if (targetWidth <= absMinWidth)
			{
				tc_memcpy(columnWidth, absMinColumnWidth, sizeof(DISPLAY_INT) * numColumns);
				columnWidthsValid = WEBC_TRUE;
			}
			else
			{
				CalculateCompromise (
						columnWidth,
						absMinStart,
						naturalStart,
						columnFlex,
						numColumns,
						highestFlex,
						targetWidth
					);

				columnWidthsValid = WEBC_TRUE;
			}

			FREE_AUTO_ARRAY(absMinStart);
			FREE_AUTO_ARRAY(absMinColumnWidth);
		}

		FREE_AUTO_ARRAY(naturalStart);
		FREE_AUTO_ARRAY(naturalColumnWidth);
	}
	else
	{
		// tableHasSpanningCells == WEBC_TRUE
		ALLOC_AUTO_ARRAY(DISPLAY_INT, maxStart, numColumns + 1);
		ALLOC_AUTO_ARRAY(DISPLAY_INT, absMinStart, numColumns + 1);
		ALLOC_AUTO_ARRAY(DISPLAY_INT, naturalStart, numColumns + 1);
		ALLOC_AUTO_ARRAY(HTMLTableCell*, rowArray, numRows);
		ALLOC_AUTO_ARRAY(WEBC_INT8, noSpanFlex, numColumns);
		ALLOC_AUTO_ARRAY(DISPLAY_INT, naturalWeight, numColumns);
		ALLOC_AUTO_ARRAY(DISPLAY_INT, naturalWeightStart, numColumns+1);
		ALLOC_AUTO_ARRAY(DISPLAY_INT, absMinWeight, numColumns);
		ALLOC_AUTO_ARRAY(DISPLAY_INT, absMinWeightStart, numColumns+1);

		WEBC_INT8 highestNoSpanFlex = CalculateColumnFlexibility(noSpanFlex, numColumns, WEBC_FALSE);

		// tbd - optimize case with no spanning cells by not doing any of this crap
		SpecifiedWidthMethod specifiedMethod(targetWidth, columnFlex, numColumns);
		FindSingleColumnWidths(naturalWeight, numColumns, &specifiedMethod);
		WidthsToStarts(naturalWeightStart, naturalWeight, numColumns);

		NaturalWidthDistributeMethod distNatural(naturalWeight, naturalWeightStart, noSpanFlex, numColumns);
		NaturalWidthMethod naturalMethod(targetWidth, columnFlex, numColumns);
		DISPLAY_INT naturalWidth;

		naturalWidth = CalculateMinStarts (
				naturalStart,
				numColumns,
				numRows,
				&naturalMethod,
				&distNatural,
				maxStart,
				rowArray,
				columnWidth
			);

		if (targetWidth == naturalWidth)
		{
			DISPLAY_INT result = StartsToWidths(columnWidth, naturalStart, numColumns);
			DHTABLE_ASSERT(result == targetWidth);
			columnWidthsValid = WEBC_TRUE;
		}
		else if (targetWidth > naturalWidth)
		{
			ExpandColumnsAtFlexLevel (
					columnWidth,
					naturalStart,
					columnFlex,
					numColumns,
					highestFlex,
					targetWidth
				);

			columnWidthsValid = WEBC_TRUE;
		}
		else
		{
			// optimization: only calculate absMinWidth case if needed
			AbsoluteMinWidthMethod absMinMethod;
			FindSingleColumnWidths(absMinWeight, numColumns, &absMinMethod);
			WidthsToStarts(absMinWeightStart, absMinWeight, numColumns);

			MinWidthDistributeMethod distMin(absMinWeightStart, naturalStart, noSpanFlex, highestNoSpanFlex, numColumns);

			DISPLAY_INT absMinWidth;

			absMinWidth = CalculateMinStarts (
					absMinStart,
					numColumns,
					numRows,
					&absMinMethod,
					&distMin,
					maxStart,
					rowArray,
					columnWidth
				);

			DHTABLE_ASSERT(absMinWidth <= naturalWidth);

			if (targetWidth <= absMinWidth)
			{
				DISPLAY_INT result = StartsToWidths(columnWidth, absMinStart, numColumns);
				DHTABLE_ASSERT(result == absMinWidth);
				columnWidthsValid = WEBC_TRUE;
			}
			else
			{
				CalculateCompromise (
						columnWidth,
						absMinStart,
						naturalStart,
						columnFlex,
						numColumns,
						highestFlex,
						targetWidth
					);

				columnWidthsValid = WEBC_TRUE;
			}
		}

		FREE_AUTO_ARRAY(absMinWeightStart);
		FREE_AUTO_ARRAY(absMinWeight);
		FREE_AUTO_ARRAY(naturalWeightStart);
		FREE_AUTO_ARRAY(naturalWeight);
		FREE_AUTO_ARRAY(noSpanFlex);
		FREE_AUTO_ARRAY(rowArray);
		FREE_AUTO_ARRAY(naturalStart);
		FREE_AUTO_ARRAY(absMinStart);
		FREE_AUTO_ARRAY(maxStart);
	}

	if (columnWidthsValid)
	{
		HTMLTableRow* row = mpTable->FirstRow();
		while (row)
		{
			HTMLTableCell* cell = row->FirstCell();
			while (cell)
			{
				int colIndex = cell->ColIndex();
				int colSpan = cell->ColSpan();

				if (colIndex + colSpan > numColumns)
				{
					colSpan = numColumns - colIndex;
				}

				DISPLAY_INT cellWidthValue;

				cellWidthValue = mpTable->GetCellSpacing() * (colSpan-1);
				cellWidthValue += GetSum(columnWidth, colIndex, colIndex + colSpan);

				HTMLTableCellDisplay* cellDisplay = (HTMLTableCellDisplay*) cell->GetDisplayElement();
				if (cellDisplay)
				{
					cellDisplay->FormatTableCell(cellWidthValue);
				}

				cell = cell->NextCell();
			}

			row = row->NextRow();
		}

		/////////////////////////////////////////////////////////////////////////////////
		// Calculate Row Heights

		int rowIndex;
		HTMLTableCell* cell;

		WEBC_BOOL doSecondPass = WEBC_FALSE;
		DISPLAY_INT* rowHeight = (DISPLAY_INT*) WEBC_MALLOC(sizeof(DISPLAY_INT) * numRows);
		if (rowHeight)
		{
			tc_memset(rowHeight, 0, numRows * sizeof(DISPLAY_INT));

			// find row heights; first pass is for non-spanning cells
			row = mpTable->FirstRow();
			while (row)
			{
				rowIndex = row->RowIndex();
				DISPLAY_INT htmlRowHeight = row->GetRowHeight();

				if (HTML_LengthGetUnit(htmlRowHeight) == HTML_LENGTH_UNIT_PIXELS ||
				    (!heightIsAuto && HTML_LengthGetUnit(htmlRowHeight) == HTML_LENGTH_UNIT_PERCENT))
				{
					rowHeight[rowIndex] = HTML_LengthToPixels(htmlRowHeight, heightValue);
				}
				else
				{
					rowHeight[rowIndex] = 0;
				}

				HTMLTableCell* cell = row->FirstCell();
				while (cell)
				{
					int rowSpan = cell->RowSpan();

					if (rowIndex + rowSpan > numRows)
					{
						rowSpan = numRows - rowIndex;
					}

					if (rowSpan == 1)
					{
						HTMLTableCellDisplay* cellDisplay = (HTMLTableCellDisplay*) cell->GetDisplayElement();
						if (cellDisplay)
						{
							cellDisplay->SetParentHeight(0);
							DISPLAY_INT cellHeightValue = cellDisplay->GetPixelHeight();
							if (cellHeightValue > rowHeight[rowIndex])
							{
								rowHeight[rowIndex] = cellHeightValue;
							}
						}
					}
					else
					{
						doSecondPass = WEBC_TRUE;
					}

					cell = cell->NextCell();
				}
				row = row->NextRow();
			}

			if (doSecondPass)
			{
				// second pass is for spanning cells
				row = mpTable->FirstRow();
				while (row)
				{
					rowIndex = row->RowIndex();
					cell = row->FirstCell();
					while (cell)
					{
						int colIndex = cell->ColIndex();

						int rowSpan = cell->RowSpan();

						if (rowIndex + rowSpan > numRows)
						{
							rowSpan = numRows - rowIndex;
						}

						if (rowSpan > 1)
						{
							if (rowIndex + rowSpan > numRows)
							{
								rowSpan = numRows - rowIndex;
							}

							HTMLTableCellDisplay* cellDisplay = (HTMLTableCellDisplay*) cell->GetDisplayElement();
							if (cellDisplay)
							{
								cellDisplay->SetParentHeight(0);
								DISPLAY_INT heightSpanned = cellDisplay->GetPixelHeight();
								DISPLAY_INT heightAvailable = GetSum(rowHeight, rowIndex, rowIndex + rowSpan);
								heightAvailable += (rowSpan - 1) * cellspacing;
								if (heightAvailable < heightSpanned)
								{
									DistributeWeighted(rowHeight, rowIndex, rowIndex + rowSpan, heightSpanned - heightAvailable);
								}
							}
						}
						cell = cell->NextCell();
					}
					row = row->NextRow();
				}
			}

			formattedHeight = GetSum(rowHeight, 0, numRows) +
			                  cellspacing * (numRows - 1) +
			                  borderTopValue + borderBottomValue +
			                  paddingTopValue + paddingBottomValue;

			if (!heightIsAuto && heightValue > formattedHeight)
			{
				DistributeWeighted(rowHeight, 0, numRows, heightValue - formattedHeight);
				formattedHeight = heightValue;
			}

			SetHeight(formattedHeight);

			// assign cell heights
			row = mpTable->FirstRow();
			while (row)
			{
				int rowIndex = row->RowIndex();

				DISPLAY_INT cellTopValue = ((rowIndex > 0)? (GetSum(rowHeight, 0, rowIndex) + cellspacing * rowIndex) : 0) +
				                            borderTopValue + paddingTopValue;

				cell = row->FirstCell();
				while (cell)
				{
					int colIndex = cell->ColIndex();
					int colSpan = cell->ColSpan();
					int rowSpan = cell->RowSpan();

					if (colIndex + colSpan > numColumns)
					{
						colSpan = numColumns - colIndex;
					}

					if (rowIndex + rowSpan > numRows)
					{
						rowSpan = numRows - rowIndex;
					}

					HTMLTableCellDisplay* cellDisplay = (HTMLTableCellDisplay*) cell->GetDisplayElement();
					if (cellDisplay)
					{
						DISPLAY_INT cellHeightValue = cellspacing * (rowSpan - 1) +
						                              GetSum(rowHeight, rowIndex, rowIndex + rowSpan);

						DISPLAY_INT cellLeftValue   = ((colIndex > 0)? (GetSum(columnWidth, 0, colIndex) + cellspacing * colIndex) : 0) +
						                              borderLeftValue + paddingLeftValue;

						cellDisplay->SetHeight(cellHeightValue);
						cellDisplay->SetXYPosition(cellLeftValue, cellTopValue);
					}

					cell = cell->NextCell();
				}
				row = row->NextRow();
			}

			WEBC_FREE(rowHeight);
		}
	}

	FREE_AUTO_ARRAY(columnFlex);
	FREE_AUTO_ARRAY(columnWidth);

	return (formattedHeight);
}

/*---------------------------------------------------------------------------*/
void HTMLTableDisplay::FindSingleColumnWidths (
		DISPLAY_INT* columnWidth,
		int numColumns,
		ITableCellWidthMethod* widthMethod
	)
{
	tc_memset(columnWidth, 0, sizeof(DISPLAY_INT) * numColumns);

	HTMLTableRow* row;
	HTMLTableCell* cell;

	// calculate the cases
	for (row = mpTable->FirstRow(); row; row = row->NextRow())
	{
		for (cell = row->FirstCell(); cell; cell = cell->NextCell())
		{
			int cellColumn  = cell->ColIndex();
			int cellColspan = cell->ColSpan();

			if (cellColspan + cellColumn > numColumns)
			{
				cellColspan = numColumns - cellColumn;
			}

			if (cellColspan == 1)
			{
				DISPLAY_INT cellWidthValue = widthMethod->calculate(cell);

			  #if(WEBC_SUPPORT_SMALL_SCREEN)
				if (cellWidthValue > WEBC_SCREEN_WIDTH)
				{
					cellWidthValue = WEBC_SCREEN_WIDTH;
				}
			  #endif // WEBC_SUPPORT_SMALL_SCREEN

				if (cellWidthValue > columnWidth[cellColumn])
				{
					columnWidth[cellColumn] = cellWidthValue;
				}
			}
		}
	}
}

/*---------------------------------------------------------------------------*/
DISPLAY_INT HTMLTableDisplay::CalculateMinWidth (
		DISPLAY_INT* minStart,
		int numColumns,
		int numRows,
		HTMLTableCell** rowArray,
		ITableCellWidthMethod* widthMethod
	)
{
	HTMLTableRow* row;
	int rowIndex;
	int columnIndex;

	/////////////////////////////////////////////////////////////////////
	// Calculate Column Widths

	// Set up row pointers

	for (row = mpTable->FirstRow(), rowIndex = 0;
	     row && rowIndex < numRows;
		 row = row->NextRow(), rowIndex++)
	{
		rowArray[rowIndex] = row->FirstCell();
	}

	// column 0 always starts at pixel 0
	minStart[0] = 0;

	DISPLAY_INT tableCellSpacing = mpTable->GetCellSpacing();

	for (columnIndex = 1; columnIndex <= numColumns; columnIndex++)
	{
		// start this column at the beginning of the last one (width=0)
		minStart[columnIndex] = minStart[columnIndex-1];

		for (rowIndex = 0; rowIndex < numRows; rowIndex++)
		{
			if (rowArray[rowIndex])
			{
				int cellColumn  = rowArray[rowIndex]->ColIndex();
				int cellColspan = rowArray[rowIndex]->ColSpan();

				if (cellColumn + cellColspan > numColumns)
				{
					 cellColspan = numColumns - cellColumn;
				}

				if (cellColumn + cellColspan == columnIndex)
				{
					DISPLAY_INT cellWidthValue = widthMethod->calculate(rowArray[rowIndex]);

				  #if (WEBC_SUPPORT_SMALL_SCREEN)
					if (cellWidthValue > WEBC_SCREEN_WIDTH)
					{
						cellWidthValue = WEBC_SCREEN_WIDTH;
					}
				  #endif // WEBC_SUPPORT_SMALL_SCREEN

					// update min start
					DISPLAY_INT cellRight = minStart[cellColumn] +
					                        cellWidthValue -
					                        tableCellSpacing * (cellColspan-1);

					// Update column starts for Min
					if (cellRight > minStart[columnIndex])
					{
						minStart[columnIndex] = cellRight;
					}

					// Go to next cell in row
					rowArray[rowIndex] = rowArray[rowIndex]->NextCell();
				}
			}
		}
	}

	return (minStart[numColumns]);
}

/*---------------------------------------------------------------------------*/
DISPLAY_INT HTMLTableDisplay::CalculateMinStarts (
		DISPLAY_INT* minStart,  // out
		int numColumns,         // in
		int numRows,            // in
		ITableCellWidthMethod* widthMethod, // in
		ITableWidthDistributeMethod* distributeMethod, // in
		DISPLAY_INT* maxStart,    // scratch data, DISPLAY_INT[numColumns + 1]
		HTMLTableCell** rowArray, // scratch data, HTMLTableCell*[numRows]
		DISPLAY_INT* columnWidth  // scratch data, DISPLAY_INT[numColumns]
	)
{
	HTMLTableRow* row;
	int rowIndex;
	int columnIndex;

	/////////////////////////////////////////////////////////////////////
	// Calculate Column Widths

	// Set up row pointers

	for (row = mpTable->FirstRow(), rowIndex = 0;
	     row && rowIndex < numRows;
		 row = row->NextRow(), rowIndex++)
	{
		// set to the right-most cell in the row
		rowArray[rowIndex] = row->LastCell();
	}

	maxStart[numColumns] = 0;

	DISPLAY_INT tableCellSpacing = mpTable->GetCellSpacing();

	for (columnIndex = 0; columnIndex <= numColumns; columnIndex++)
	{
		minStart[columnIndex] = 0;
	}

	WEBC_BOOL spanningCellsSeen = WEBC_FALSE;

	// calculate maxStart for each column to make sure we hit calculatedMinWidth
	for (columnIndex = numColumns - 1; columnIndex >= 0; columnIndex--)
	{
		// start this column at the beginning of the last one (width=0)
		maxStart[columnIndex] = maxStart[columnIndex + 1];

		for (rowIndex = 0; rowIndex < numRows; rowIndex++)
		{
			if (rowArray[rowIndex])
			{
				int cellColumn = rowArray[rowIndex]->ColIndex();

				DHTABLE_ASSERT(cellColumn < numColumns);

				if (cellColumn == columnIndex)
				{
					int cellColspan = rowArray[rowIndex]->ColSpan();

					if (cellColumn + cellColspan > numColumns)
					{
						cellColspan = numColumns - cellColumn;
					}

					DISPLAY_INT cellWidthValue = widthMethod->calculate(rowArray[rowIndex]);

				  #if (WEBC_SUPPORT_SMALL_SCREEN)
					if (cellWidthValue > WEBC_SCREEN_WIDTH)
					{
						cellWidthValue = WEBC_SCREEN_WIDTH;
					}
				  #endif // WEBC_SUPPORT_SMALL_SCREEN

					if (cellColspan == 1)
					{
						if (cellWidthValue > GET_COLUMN_WIDTH(minStart, cellColumn))
						{
							SET_COLUMN_WIDTH(minStart, cellColumn, cellWidthValue, numColumns);
						}
					}
					else
					{
						spanningCellsSeen = WEBC_TRUE;
					}

					DHTABLE_ASSERT(cellColspan > 0);

					// update max start
					DISPLAY_INT cellLeft = maxStart[cellColumn + cellColspan] -
					                       (cellWidthValue - tableCellSpacing * (cellColspan-1));

					// Update column starts for max
					if (cellLeft < maxStart[columnIndex])
					{
						maxStart[columnIndex] = cellLeft;
					}

					// Go to previous cell in row
					rowArray[rowIndex] = rowArray[rowIndex]->PrevCell();
				}
			}
		}
	}

	DISPLAY_INT calculatedMinWidth = 0 - maxStart[0];
	for (columnIndex = 0; columnIndex <= numColumns; columnIndex++)
	{
		maxStart[columnIndex] += calculatedMinWidth;
	}

	// now do a second pass to get the spanning cells
	if (spanningCellsSeen)
	{
		for (row = mpTable->FirstRow(), rowIndex = 0;
			 row && rowIndex < numRows;
			 row = row->NextRow(), rowIndex++)
		{
			rowArray[rowIndex] = row->FirstCell();
		}

		for (columnIndex = 1; columnIndex <= numColumns; columnIndex++)
		{
			for (rowIndex = 0; rowIndex < numRows; rowIndex++)
			{
				if (rowArray[rowIndex])
				{
					int cellColumn  = rowArray[rowIndex]->ColIndex();
					int cellColspan = rowArray[rowIndex]->ColSpan();

					if (cellColumn + cellColspan > numColumns)
					{
						 cellColspan = numColumns - cellColumn;
					}

					if (cellColumn + cellColspan == columnIndex)
					{
						if (cellColspan > 1)
						{
							DISPLAY_INT cellWidthValue = widthMethod->calculate(rowArray[rowIndex]);

						  #if (WEBC_SUPPORT_SMALL_SCREEN)
							if (cellWidthValue > WEBC_SCREEN_WIDTH)
							{
								cellWidthValue = WEBC_SCREEN_WIDTH;
							}
						  #endif // WEBC_SUPPORT_SMALL_SCREEN

							// update min start
							DISPLAY_INT cellRight = minStart[cellColumn] +
													(cellWidthValue - tableCellSpacing * (cellColspan-1));

							// Update column starts for Min
							if (cellRight > minStart[columnIndex])
							{
								ExpandColumns (
										minStart,
										cellColumn,
										cellColspan,
										numColumns,
										maxStart,
										cellRight - minStart[cellColumn],
										distributeMethod,
										columnWidth
									);

								minStart[columnIndex] = cellRight;
							}
						}

						// Go to next cell in row
						rowArray[rowIndex] = rowArray[rowIndex]->NextCell();
					}
				}
			}
		}
	}

	return (minStart[numColumns]);
}

/*---------------------------------------------------------------------------*/
void HTMLTableDisplay::AttachTableCells (void)
{
	HTMLTableRow* row;
	HTMLTableCell* cell;

	for (row = mpTable->FirstRow(); row; row = row->NextRow())
	{
		for (cell = row->FirstCell(); cell; cell = cell->NextCell())
		{
			HTMLTableCellDisplay* cellDisplay;

			cellDisplay = (HTMLTableCellDisplay *) cell->GetDisplayElement();

			if (cellDisplay)
			{
				cellDisplay->SetParentDisplay(this, 0);
			}
		}
	}
}

/*---------------------------------------------------------------------------*/
WEBC_BOOL HTMLTableDisplay::HasSpanningCells (int numColumns)
{
	HTMLTableRow* row;
	HTMLTableCell* cell;

	for (row = mpTable->FirstRow(); row; row = row->NextRow())
	{
		for (cell = row->FirstCell(); cell; cell = cell->NextCell())
		{
			int cellColumn  = cell->ColIndex();
			int cellColspan = cell->ColSpan();

			if (cellColumn + cellColspan > numColumns)
			{
				 cellColspan = numColumns - cellColumn;
			}

			if (cellColspan > 1)
			{
				return (WEBC_TRUE);
			}
		}
	}

	return (WEBC_FALSE);
}

/*---------------------------------------------------------------------------*/
WEBC_INT8 HTMLTableDisplay::CalculateColumnFlexibility (
		WEBC_INT8* columnFlex,
		int numColumns,
		WEBC_BOOL includeSpan
	)
{
	DHTABLE_ASSERT(unitCaseFlag[HTML_LENGTH_UNIT_NONE] == UNIT_AUTO_AFFECTED);
	DHTABLE_ASSERT(unitCaseFlag[HTML_LENGTH_UNIT_STAR] == UNIT_AUTO_AFFECTED);
	DHTABLE_ASSERT(unitCaseFlag[HTML_LENGTH_UNIT_PIXELS] == UNIT_PX_AFFECTED);
	DHTABLE_ASSERT(unitCaseFlag[HTML_LENGTH_UNIT_PERCENT] == UNIT_PERCENT_AFFECTED);

	int columnIndex;
	WEBC_INT8* unitCase = columnFlex;

	for (columnIndex = 0; columnIndex < numColumns; columnIndex++)
	{
		unitCase[columnIndex] = 0;
	}

	HTMLTableRow* row;
	HTMLTableCell* cell;

	// calculate the cases
	for (row = mpTable->FirstRow(); row; row = row->NextRow())
	{
		for (cell = row->FirstCell(); cell; cell = cell->NextCell())
		{
			int cellColumn  = cell->ColIndex();
			int cellColspan = cell->ColSpan();

			DHTABLE_ASSERT(cellColumn >= 0 && cellColspan > 0 && cellColumn < numColumns);

			if (cellColspan + cellColumn > numColumns)
			{
				cellColspan = numColumns - cellColumn;
			}

			if (includeSpan || cellColspan == 1)
			{
				for (columnIndex = cellColumn; columnIndex < cellColumn + cellColspan; columnIndex++)
				{
					HTMLLengthUnit cellWidthUnit = HTML_LengthGetUnit(cell->CellWidth());
					unitCase[columnIndex] |= unitCaseFlag[cellWidthUnit];
				}
			}
		}
	}

	WEBC_INT8 bestFlexibility = 0;

	for (columnIndex = 0; columnIndex < numColumns; columnIndex++)
	{
		columnFlex[columnIndex] = unitCaseFlexibility[unitCase[columnIndex]];

		if (columnFlex[columnIndex] > bestFlexibility)
		{
			bestFlexibility = columnFlex[columnIndex];
		}
	}

	return (bestFlexibility);
}

/*---------------------------------------------------------------------------*/
DISPLAY_INT StartsToWidths (
		DISPLAY_INT* width, // out
		DISPLAY_INT* start,
		int numColumns
	)
{
	for (int c=0; c<numColumns; c++)
	{
		width[c] = start[c+1] - start[c];
	}

	return (start[numColumns]);
}

/*---------------------------------------------------------------------------*/
DISPLAY_INT WidthsToStarts (
		DISPLAY_INT* start, // out
		DISPLAY_INT* width, // in
		int numColumns
	)
{
	start[0] = 0;
	for (int c=0; c<numColumns; c++)
	{
		start[c+1] = start[c] + width[c];
	}

	return (start[numColumns]);
}

/*---------------------------------------------------------------------------*/
void HTMLTableDisplay::ExpandColumns (
		DISPLAY_INT* columnStart,            // current start; update this value
		int colIndex,                        // start at this column
		int colSpan,                         // distribute to this many columns
		int numColumns,                      // total size of the table
		DISPLAY_INT* maxStart,               // ensure columnStart[c] <= maxStart[c]
		DISPLAY_INT targetWidth,             // target width for the spanned columns
		ITableWidthDistributeMethod* method,
		DISPLAY_INT* columnWidth             // scratch data used by this algorithm
	)
{
	DHTABLE_ASSERT(columnStart[colIndex] + targetWidth <= maxStart[colSpan + colIndex]);

	method->distribute (
			columnWidth,
			columnStart,
			colIndex,
			colSpan,
			targetWidth
		);

	DISPLAY_INT workingTotalWidth = 0;
	int colStart = colIndex;
	int c;

	for (c = colIndex; c < colIndex + colSpan; c++)
	{
		workingTotalWidth += columnWidth[c];
		if (columnStart[colStart] + workingTotalWidth > maxStart[c+1])
		{
			workingTotalWidth = maxStart[c+1] - columnStart[colStart];

			int leftSpan  = (c + 1) - colStart;
			int rightSpan = colIndex + colSpan - (c + 1);

			DHTABLE_ASSERT(colStart + leftSpan + rightSpan == colIndex + colSpan);
			DHTABLE_ASSERT(c + 1 + rightSpan == colIndex + colSpan);

			method->distribute (
					columnWidth,
					columnStart,
					colStart,
					leftSpan,
					workingTotalWidth
				);

			method->distribute (
					columnWidth,
					columnStart,
					c + 1,
					rightSpan,
					targetWidth - workingTotalWidth
				);

			targetWidth -= workingTotalWidth;
			workingTotalWidth = 0;
			colStart = c + 1;
		}
	}

	for (c = colIndex; c < colIndex + colSpan; c++)
	{
		// tbd - optimize
		DHTABLE_ASSERT(columnWidth[c] >= 0);
		SET_COLUMN_WIDTH(columnStart, c, columnWidth[c], numColumns);
	}
}

/*---------------------------------------------------------------------------*/
void ExpandColumnsAtFlexLevel (
		DISPLAY_INT* columnWidth,
		DISPLAY_INT* baseStart,
		WEBC_INT8*   columnFlex,
		int          numColumns,
		WEBC_INT8    flexLevel,
		DISPLAY_INT  targetWidth
	)
{
	int c;
	DISPLAY_INT totalToAdd = targetWidth - (baseStart[numColumns] - baseStart[0]);
	DISPLAY_INT totalAtFlexLevel = 0;
	int numAtFlexLevel = 0;

	for (c = 0; c < numColumns; c++)
	{
		if (columnFlex[c] == flexLevel)
		{
			numAtFlexLevel++;
			totalAtFlexLevel += GET_COLUMN_WIDTH(baseStart,c);
		}
	}

	for (c = 0; c < numColumns; c++)
	{
		if (columnFlex[c] == flexLevel)
		{
			DISPLAY_INT toAdd;

			if (totalAtFlexLevel)
			{
				toAdd = (totalToAdd * GET_COLUMN_WIDTH(baseStart,c)) / totalAtFlexLevel;
			}
			else
			{
				toAdd = totalToAdd / numAtFlexLevel;
			}

			columnWidth[c] = GET_COLUMN_WIDTH(baseStart,c) + toAdd;

			numAtFlexLevel--;
			totalAtFlexLevel -= GET_COLUMN_WIDTH(baseStart,c);
			totalToAdd -= toAdd;
		}
		else
		{
			columnWidth[c] = GET_COLUMN_WIDTH(baseStart,c);
		}
	}
}

/*---------------------------------------------------------------------------*/
void CalculateDistribution (
		DISPLAY_INT* columnWidth,
		DISPLAY_INT* baseStart,
		DISPLAY_INT* columnWeight,
		DISPLAY_INT* minStart,
		WEBC_INT8* columnFlex,
		int numColumns,
		WEBC_INT8 flexLevel,
		DISPLAY_INT targetWidth
	)
{
	int c;
	int numAtFlexLevel = 0;
	DISPLAY_INT totalWeight = 0;
	DISPLAY_INT totalToDistribute = targetWidth - (baseStart[numColumns] - baseStart[0]);
	DISPLAY_INT distributeToColumn;

	if (minStart[numColumns] - minStart[0] >= targetWidth)
	{
		// handle trivial case where (minStart[numColumns] - minStart[0]) >= targetWidth
		for (c = 0; c < numColumns; c++)
		{
			columnWidth[c] = GET_COLUMN_WIDTH(minStart,c);
		}
		return;
	}

	for (c = 0; c < numColumns; c++)
	{
		if (columnFlex[c] == flexLevel)
		{
			numAtFlexLevel++;
			totalWeight += columnWeight[c];
		}
	}

	DISPLAY_INT amountToSubtract = 0;

	for (c = 0; c < numColumns; c++)
	{
		columnWidth[c] = GET_COLUMN_WIDTH(baseStart,c);

		if (columnFlex[c] == flexLevel)
		{
			if (totalWeight)
			{
				distributeToColumn = (totalToDistribute * columnWeight[c]) / totalWeight;
			}
			else
			{
				distributeToColumn = totalToDistribute / numAtFlexLevel;
			}

			columnWidth[c] += distributeToColumn;
			totalToDistribute -= distributeToColumn;
			totalWeight -= columnWeight[c];
			numAtFlexLevel--;
		}

		if (columnWidth[c] < GET_COLUMN_WIDTH(minStart,c))
		{
			amountToSubtract += GET_COLUMN_WIDTH(minStart,c) - columnWidth[c];
			columnWidth[c] = GET_COLUMN_WIDTH(minStart,c);
		}
	}

	while (amountToSubtract > 0)
	{
		numAtFlexLevel = 0;
		totalWeight = 0;

		// determine the total base width of all columns to subtract the extra width from
		for (c = 0; c < numColumns; c++)
		{
			if (columnFlex[c] == flexLevel && columnWidth[c] > GET_COLUMN_WIDTH(minStart,c))
			{
				numAtFlexLevel++;
				totalWeight += columnWeight[c];
			}
		}

		totalToDistribute = -amountToSubtract;
		amountToSubtract = 0;

		// adjust width down on the other columns to allow minWidths to be satisfied
		for (c = 0; c < numColumns && totalToDistribute; c++)
		{
			if (columnFlex[c] == flexLevel && columnWidth[c] > GET_COLUMN_WIDTH(minStart,c))
			{
				if (totalWeight)
				{
					distributeToColumn = (totalToDistribute * columnWeight[c]) / totalWeight;
				}
				else
				{
					distributeToColumn = totalToDistribute / numAtFlexLevel;
				}

				columnWidth[c] += distributeToColumn;
				totalToDistribute -= distributeToColumn;
				totalWeight -= columnWeight[c];
				numAtFlexLevel--;

				if (columnWidth[c] < GET_COLUMN_WIDTH(minStart,c))
				{
					amountToSubtract += GET_COLUMN_WIDTH(minStart,c) - columnWidth[c];
					columnWidth[c] = GET_COLUMN_WIDTH(minStart,c);
				}

			}
		}
	}
}

/*---------------------------------------------------------------------------*/
void CalculateDistributionInterpolated (
		DISPLAY_INT* columnWidth,
		DISPLAY_INT* lowStart,
		DISPLAY_INT* highStart,
		DISPLAY_INT* minStart,
		WEBC_INT8* columnFlex,
		int numColumns,
		WEBC_INT8 highestFlex,
		DISPLAY_INT targetWidth,
		DISPLAY_INT* columnWeight, // scratch data
		DISPLAY_INT* workingMinStart  // scratch data
	)
{
	WEBC_INT8 flexLevel;
	DISPLAY_INT totalToAdd = targetWidth - (lowStart[numColumns] - lowStart[0]);

	DHTABLE_ASSERT(highStart[numColumns] - highStart[0] >= minStart[numColumns] - minStart[0]);
	DHTABLE_ASSERT(highStart[numColumns] - highStart[0] >= targetWidth && totalToAdd >= 0);

	if (totalToAdd == 0)
	{
		// tbd - WARNING: we don't know that this is >= minStart
		StartsToWidths(columnWidth, lowStart, numColumns);
		return;
	}

	tc_memcpy(workingMinStart, minStart, sizeof(DISPLAY_INT) * (numColumns + 1));

	for (flexLevel = 0; flexLevel <= highestFlex && totalToAdd; flexLevel++)
	{
		DISPLAY_INT totalWeight = 0;
		int c;

		for (c = 0; c < numColumns; c++)
		{
			if (columnFlex[c] == flexLevel && GET_COLUMN_WIDTH(highStart,c) > GET_COLUMN_WIDTH(lowStart,c))
			{
				columnWeight[c] = GET_COLUMN_WIDTH(highStart,c) - GET_COLUMN_WIDTH(lowStart,c);
				totalWeight += columnWeight[c];
			}
			else
			{
				columnWeight[c] = 0;
			}
		}

		if (totalWeight)
		{
			DISPLAY_INT workingTotalToAdd = EBSMIN(totalToAdd, totalWeight);
			totalToAdd -= workingTotalToAdd;

			CalculateDistribution (
					columnWidth,
					lowStart,
					columnWeight,
					workingMinStart,
					columnFlex,
					numColumns,
					flexLevel,
					targetWidth - totalToAdd
				);

			// update workingMinStart
			if (totalToAdd)
			{
				WidthsToStarts(workingMinStart, columnWidth, numColumns);
			}
		}
	}
}

/*---------------------------------------------------------------------------*/
void HTMLTableDisplay::CalculateCompromise (
		DISPLAY_INT* columnWidth,
		DISPLAY_INT* lowStart,
		DISPLAY_INT* highStart,
		WEBC_INT8* columnFlex,
		int numColumns,
		WEBC_INT8 highestFlex,
		DISPLAY_INT targetWidth
	)
{
	int c;
	WEBC_INT8 flexLevel;
	DISPLAY_INT totalToAdd = targetWidth - (lowStart[numColumns] - lowStart[0]);

	StartsToWidths(columnWidth, lowStart, numColumns);

	for (flexLevel = 0; flexLevel <= highestFlex && totalToAdd; flexLevel++)
	{
		DISPLAY_INT totalWeight = 0;

		for (c = 0; c < numColumns; c++)
		{
			if (columnFlex[c] == flexLevel && GET_COLUMN_WIDTH(highStart,c) > GET_COLUMN_WIDTH(lowStart,c))
			{
				DISPLAY_INT columnWeight = GET_COLUMN_WIDTH(highStart,c) - GET_COLUMN_WIDTH(lowStart,c);
				totalWeight += columnWeight;
			}
		}

		if (totalWeight)
		{
			DISPLAY_INT workingTotalToAdd = EBSMIN(totalToAdd, totalWeight);
			totalToAdd -= workingTotalToAdd;

			// add workingTotalToAdd to base width for each column in the current
			//  flex level
			for (c = 0; c < numColumns; c++)
			{
				if (columnFlex[c] == flexLevel && GET_COLUMN_WIDTH(highStart,c) > GET_COLUMN_WIDTH(lowStart,c))
				{
					DISPLAY_INT columnWeight = GET_COLUMN_WIDTH(highStart,c) - GET_COLUMN_WIDTH(lowStart,c);
					DISPLAY_INT toAdd = (columnWeight * workingTotalToAdd) / totalWeight;
					columnWidth[c] += toAdd;
					workingTotalToAdd -= toAdd;
					totalWeight -= columnWeight;
				}
			}
		}
	}
}


TUFormatStatus HTMLTableDisplay::Formatted (void)
{
	return (mFormatted);
}

void HTMLTableDisplay::SetFormatStatus(TUFormatStatus status)
{
	if (((status == TU_FORMAT_INVALID) || (status == TU_FORMAT_MAYBE_VALID)) &&
		(mFormatted == TU_FORMAT_VALID))
	{
		mFormatted = status;

		DisplayElement *parent = mpHtmlElement? mpHtmlElement->GetDisplayParent() : 0;
		if (parent)
		{
			NotifyEvent e(NOTIFIER_TYPE_TEXT_UNIT, NOTIFY_STYLE_CHANGE);

			e.data.oldStyle.display = GetUnitType();
			e.data.oldStyle.position = GetPosition();

			InvalidateTUMirror();

			parent->Notify((TextUnit *) this, &e);
		}
		return;
	}
	mFormatted = status;
}

void HTMLTableDisplay::Notify(Notifier *who, NotifyEvent *what)
{
	if (who == mpHtmlElement || (what && (what->type == NOTIFIER_TYPE_CHILD_TABLE_ROW)))
	{
        WEBC_DEBUG_ReceiveNotify(this, who, what);
		if (!what)
		{
			return;
		}

		switch (what->event)
		{
			case NOTIFY_CONTENT_ADDED:        // intentional fall-through
			case NOTIFY_CONTENT_REMOVED:      // intentional fall-through
			case NOTIFY_FORMAT_STYLE_CHANGE:  // intentional fall-through
			case NOTIFY_PARENT_STYLE_CHANGE:  // intentional fall-through
			case NOTIFY_STYLE_CHANGE:
				SetFormatStatus(TU_FORMAT_INVALID);
				mTableFlags &= ~(TD_FLAG_MINWIDTH_VALID|TD_FLAG_MAXWIDTH_VALID);
				break;

			case NOTIFY_DISPLAY_STYLE_CHANGE:
			case NOTIFY_PARENT_DISPLAY_STYLE_CHANGE:
				Invalidate();
				break;

			default:
				break;
		}
	}
	else if (what && (what->type == NOTIFIER_TYPE_TEXT_UNIT))
	{
        WEBC_DEBUG_ReceiveNotify(this, who, what);
		switch (what->event)
		{
			case NOTIFY_FLOW_CHILD_STYLE_CHANGE: // intentional fall-through
			case NOTIFY_FORMAT_STYLE_CHANGE:     // intentional fall-through
			case NOTIFY_MAX_WIDTH_CHANGE:        // intentional fall-through
			case NOTIFY_MIN_WIDTH_CHANGE:        // intentional fall-through
			case NOTIFY_SIZE_CHANGE:             // intentional fall-through
			case NOTIFY_STYLE_CHANGE:
				// tbd - there should be a way to tell whether this element
				//  is inline/relative positioned
				SetFormatStatus(TU_FORMAT_INVALID);
				mTableFlags &= ~(TD_FLAG_MINWIDTH_VALID|TD_FLAG_MAXWIDTH_VALID);
				break;

			default:
				break;
		}
	}

	HTMLElementDisplay<DisplayElement>::Notify(who, what);
}

DISPLAY_INT HTMLTableDisplay::GetTopMargin(void)
{
	return mBorder.GetMarginWidth(CSS_TOP);
}

DISPLAY_INT HTMLTableDisplay::GetLeftMargin(void)
{
	return mBorder.GetMarginWidth(CSS_LEFT);
}

DISPLAY_INT HTMLTableDisplay::GetBottomMargin(void)
{
	return mBorder.GetMarginWidth(CSS_BOTTOM);
}

DISPLAY_INT HTMLTableDisplay::GetRightMargin(void)
{
	return mBorder.GetMarginWidth(CSS_RIGHT);
}

WEBC_BOOL HTMLTableDisplay::SetElementStyle(void)
{
	return (mBorder.ResetProperties());
}

DISPLAY_INT HTMLTableDisplay::GetParentWidth(void)
{
	return (mBorder.GetParentWidth());
}

#endif // WEBC_SUPPORT_TABLES
