//
// DHFRAMESET.CPP - Methods for class HTMLFrameSetDisplay
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
#include "hframset.hpp"
#include "dhframeset.hpp"
#include "dhframe.hpp"
#include "webcmem.h"


/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/


/*****************************************************************************/
// Local Types
/*****************************************************************************/


/*****************************************************************************/
// Global Data
/*****************************************************************************/


/*****************************************************************************/
// Local functions
/*****************************************************************************/

void _ClipTotalWidth (
		DISPLAY_INT* pRealWidth,
		HTMLLength* columnWidth,
		int wColumns,
		DISPLAY_INT target
	);

/*****************************************************************************/
// Class methods
/*****************************************************************************/

#if (WEBC_SUPPORT_FRAMES)

HTMLFrameSetDisplay::HTMLFrameSetDisplay(HTMLFrameSet * f)
	: HTMLElementDisplay<DisplayElement> (f)
{
	mFormatted = TU_FORMAT_INVALID;
}

HTMLFrameSetDisplay::~HTMLFrameSetDisplay()
{
}

HTMLEventStatus HTMLFrameSetDisplay::Event(HTMLEvent * event)
{
    WEBC_DEBUG_ReceiveEvent(this, event);
	return HTML_EVENT_STATUS_DONE;
}

void HTMLFrameSetDisplay::GetBounds (WebRect *rect)
{
	if (Formatted() != TU_FORMAT_VALID)
	{
		Format();
	}

	DisplayElement::GetBounds(rect);
}

void HTMLFrameSetDisplay::Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC)
{
	if (Formatted() != TU_FORMAT_VALID)
	{
		Format();
	}

	DisplayElement::Draw(x, y, pViewport, pGC);
}

DISPLAY_INT HTMLFrameSetDisplay::GetPixelWidth(void)
{
	if (Formatted() != TU_FORMAT_VALID)
	{
		Format();
	}

	return (Width());
}

DISPLAY_INT HTMLFrameSetDisplay::GetPixelHeight(void)
{
	if (Formatted() != TU_FORMAT_VALID)
	{
		Format();
	}

	return (Height());
}

void HTMLFrameSetDisplay::SetParentWidth(DISPLAY_INT ignored, DISPLAY_INT iWidth)
{
	if (iWidth != Width())
	{
		SetWidth(iWidth);
		SetFormatStatus(TU_FORMAT_INVALID);
	}
}

void HTMLFrameSetDisplay::SetParentHeight(DISPLAY_INT iHeight)
{
	if (iHeight != Height())
	{
		SetHeight(iHeight);
		SetFormatStatus(TU_FORMAT_INVALID);
	}
}

TUFormatStatus HTMLFrameSetDisplay::Formatted (void)
{
	return (mFormatted);
}

void HTMLFrameSetDisplay::SetFormatStatus(TUFormatStatus status)
{
/*	if (mpParent)
	{
		if (((status == TU_FORMAT_INVALID) || (status == TU_FORMAT_MAYBE_VALID)) &&
			(mFormatted == TU_FORMAT_VALID))
		{
			mFormatted = status;
			NotifyEvent what(NOTIFIER_TYPE_TEXT_UNIT, NOTIFY_FORMAT_VALID_CHANGE);
			mpParent->Notify((Notifier *) ((TextUnit *) this), &what);
			return;
		}
	}*/
	mFormatted = status;
}

void HTMLFrameSetDisplay::Format(void)
{
int r, c;
HTMLElement *pChild;
HTMLFrameSet *pFrameSet = (HTMLFrameSet *) mpHtmlElement;
HTMLFrameDisplay *pFrameDisplay;

	int numColumns = pFrameSet->NumCols();
	int numRows = pFrameSet->NumRows();

	DISPLAY_INT* columnWidth = (DISPLAY_INT*) WEBC_MALLOC(sizeof(DISPLAY_INT) * numColumns);
	if (!columnWidth)
	{
		return;
	}

	DISPLAY_INT* rowHeight = (DISPLAY_INT*) WEBC_MALLOC(sizeof(DISPLAY_INT) * numRows);
	if (!rowHeight)
	{
		WEBC_FREE(columnWidth);
		return;
	}

	// calculate the pixel dimensions of the columns and rows in our frameset
	DISPLAY_INT percentUsed = 0;
	DISPLAY_INT unusedSpace = Width() - EBSMAX(numColumns-1,0)*pFrameSet->Border();
	int starTotal = 0;

	for (c=0; c<numColumns; c++)
	{
		if (!pFrameSet->GetColumnWidth(c))
		{
			starTotal++;
			columnWidth[c] = 0;
		}
		else
		{
			if (HTML_LengthGetUnit(pFrameSet->GetColumnWidth(c)) == HTML_LENGTH_UNIT_STAR)
			{
				starTotal += HTML_LengthGetValue(pFrameSet->GetColumnWidth(c));
				columnWidth[c] = 0; // initial value
			}
			else
			{
				DISPLAY_INT pixelValue = HTML_LengthToPixels(pFrameSet->GetColumnWidth(c), Width() - EBSMAX(numColumns-1,0)*pFrameSet->Border());
				columnWidth[c] = pixelValue;
				if (columnWidth[c] >= 0)
				{
					unusedSpace -= columnWidth[c];
					if (HTML_LengthGetUnit(pFrameSet->GetColumnWidth(c)) == HTML_LENGTH_UNIT_PERCENT)
					{
						percentUsed += pixelValue;
					}
				}
			}
		}
	}

	if (unusedSpace > 0)
	{
		// Second pass for * columns...
		for (c=0; c<numColumns && (starTotal > 0); c++)
		{
			if (!pFrameSet->GetColumnWidth(c))
			{
				columnWidth[c] = (unusedSpace) / starTotal;
				starTotal--;
				unusedSpace -= columnWidth[c];
			}
			else
			{
				if (HTML_LengthGetUnit(pFrameSet->GetColumnWidth(c))== HTML_LENGTH_UNIT_STAR)
				{
					columnWidth[c] = (HTML_LengthGetValue(pFrameSet->GetColumnWidth(c)) * unusedSpace) / starTotal;
					starTotal -= HTML_LengthGetValue(pFrameSet->GetColumnWidth(c));
					unusedSpace -= columnWidth[c];
				}
			}
		}

		// if we still don't fill the allotted space, then add pixels proportionally
		if (unusedSpace > 0)
		{
			DISPLAY_INT iPortion;
			for (c=0; c<numColumns && (percentUsed > 0); c++)
			{
				if (HTML_LengthGetUnit(pFrameSet->GetColumnWidth(c))== HTML_LENGTH_UNIT_PERCENT)
				{
					iPortion = (columnWidth[c] * unusedSpace) / percentUsed;
					percentUsed -= columnWidth[c];
					columnWidth[c] += iPortion;
					unusedSpace -= iPortion;
				}
			}

			if (unusedSpace > 0)
			{
				DISPLAY_INT iRealTotal = Width() - EBSMAX(numColumns-1,0)*pFrameSet->Border() - unusedSpace;
				for (c=0; c<numColumns && (iRealTotal > 0); c++)
				{
					iPortion = (columnWidth[c] * unusedSpace) / iRealTotal;
					iRealTotal -= columnWidth[c];
					columnWidth[c] += iPortion;
					unusedSpace -= iPortion;
				}
			}
		}
	}

	// Make sure our frameset fits in the allotted space
	if (pFrameSet->GetColumnWidth(0))
	{
		_ClipTotalWidth (
				columnWidth,
				pFrameSet->GetColumnWidths(),
				numColumns,
				Width() - EBSMAX(numColumns-1,0)*pFrameSet->Border()
			);
	}

	percentUsed = 0;
	unusedSpace = Height() - EBSMAX(numRows-1,0)*pFrameSet->Border();
	starTotal = 0;

	for (r=0; r<numRows; r++)
	{
		if (!pFrameSet->GetRowHeight(r))
		{
			starTotal++;
			rowHeight[r] = 0;
		}
		else
		{
			if (HTML_LengthGetUnit(pFrameSet->GetRowHeight(r)) == HTML_LENGTH_UNIT_STAR)
			{
				starTotal += HTML_LengthGetValue(pFrameSet->GetRowHeight(r));
				rowHeight[r] = 0; // initial value
			}
			else
			{
				DISPLAY_INT pixelValue = HTML_LengthToPixels(pFrameSet->GetRowHeight(r), Height() - EBSMAX(numRows-1,0)*pFrameSet->Border());
				rowHeight[r] = pixelValue;
				if (rowHeight[r] >= 0)
				{
					unusedSpace -= rowHeight[r];
					if (HTML_LengthGetUnit(pFrameSet->GetRowHeight(r)) == HTML_LENGTH_UNIT_PERCENT)
					{
						percentUsed += pixelValue;
					}
				}
			}
		}
	}

	if (unusedSpace > 0)
	{
		// Second pass for * Rows...
		for (r=0; r<numRows && (starTotal > 0); r++)
		{
			if (!pFrameSet->GetRowHeight(r))
			{
				rowHeight[r] = (unusedSpace) / starTotal;
				starTotal--;
				unusedSpace -= rowHeight[r];
			}
			else
			{
				if (HTML_LengthGetUnit(pFrameSet->GetRowHeight(r)) == HTML_LENGTH_UNIT_STAR)
				{
					rowHeight[r] = (HTML_LengthGetValue(pFrameSet->GetRowHeight(r)) * unusedSpace) / starTotal;
					starTotal -= HTML_LengthGetValue(pFrameSet->GetRowHeight(r));
					unusedSpace -= rowHeight[r];
				}
			}
		}

		// if we still don't fill the allotted space, then add pixels proportionally
		if (unusedSpace > 0)
		{
			DISPLAY_INT iPortion;
			for (r=0; r<numRows && (percentUsed > 0); r++)
			{
				if (HTML_LengthGetUnit(pFrameSet->GetRowHeight(r)) == HTML_LENGTH_UNIT_PERCENT)
				{
					iPortion = (rowHeight[r] * unusedSpace) / percentUsed;
					percentUsed -= rowHeight[r];
					rowHeight[r] += iPortion;
					unusedSpace -= iPortion;
				}
			}

			if (unusedSpace > 0)
			{
				DISPLAY_INT iRealTotal = Height() - EBSMAX(numRows-1,0)*pFrameSet->Border() - unusedSpace;
				for (r=0; r<numRows && (iRealTotal > 0); r++)
				{
					iPortion = (rowHeight[r] * unusedSpace) / iRealTotal;
					iRealTotal -= rowHeight[r];
					rowHeight[r] += iPortion;
					unusedSpace -= iPortion;
				}
			}
		}
	}

	// Make sure our frameset fits in the allotted space
	if (pFrameSet->GetRowHeight(0))
	{
		_ClipTotalWidth (
				rowHeight,
				pFrameSet->GetRowHeights(),
				numRows,
				Height() - EBSMAX(numRows-1,0)*pFrameSet->Border()
			);
	}

	// Now add all children to the display list
	pChild = mpHtmlElement->FirstChild();
	r = 0;
	c = 0;
	while (pChild)
	{
		if ((pChild->Type() == HTML_FRAME_ELEMENT) ||
			(pChild->Type() == HTML_FRAMESET_ELEMENT))
		{
			pFrameDisplay = (HTMLFrameDisplay *) pChild->GetDisplayElement();
			if (pFrameDisplay)
			{
				pFrameDisplay->SetParentDisplay(this, 0);
				pFrameDisplay->SetParentWidth(columnWidth[c], columnWidth[c]);
				pFrameDisplay->SetParentHeight(rowHeight[r]);
				pFrameDisplay->SetXYPosition(GetSum(columnWidth,0,c) + (c*pFrameSet->Border()),
											 GetSum(rowHeight,0,r) + (r*pFrameSet->Border()));
			}

			c++;
			if (c == numColumns)
			{
				c = 0;
				r++;
				if (r == numRows)
				{
					break;
				}
			}
		}

		pChild = pChild->mpNext;
	}

	WEBC_FREE(columnWidth);
	WEBC_FREE(rowHeight);

	// Frameset formatting is now officially done
	SetFormatStatus(TU_FORMAT_VALID);
}

void HTMLFrameSetDisplay::Notify(Notifier *who, NotifyEvent *what)
{
	if (what && (who == mpHtmlElement))
	{
        WEBC_DEBUG_ReceiveNotify(this, who, what);
		switch (what->event)
		{
		case NOTIFY_PARENT_STYLE_CHANGE:  // intentional fall-through
		case NOTIFY_STYLE_CHANGE:         // intentional fall-through
		case NOTIFY_CONTENT_ADDED:        // intentional fall-through
		case NOTIFY_CONTENT_REMOVED:
			SetFormatStatus(TU_FORMAT_INVALID);
			break;

		default:
			break;
		}
	}

	HTMLElementDisplay<DisplayElement>::Notify(who, what);
}


void _ClipTotalWidth (
		DISPLAY_INT *pRealWidth,
		HTMLLength *pColumnWidth,
		int wColumns,
		DISPLAY_INT target
	)
{
int c, iTotalWidth;

	iTotalWidth = GetSum(pRealWidth, 0, wColumns);

	if (iTotalWidth > target)
	{
		int iToAdd, iTotalFlex, iPortion;
		// Distribute a weighted differential between the actual width and target

		// First pass is to find out the total flex space
		for (c=0, iTotalFlex=0; c<wColumns; c++)
		{
			iTotalFlex += (HTML_LengthGetUnit(pColumnWidth[c]) != HTML_LENGTH_UNIT_PIXELS)? pRealWidth[c] : 0;
		}

		iToAdd = target - iTotalWidth;

		// Now do a weighted distribution
		for (c=0; c<wColumns; c++)
		{
			if (iTotalFlex == 0) break;

			if (HTML_LengthGetUnit(pColumnWidth[c]) != HTML_LENGTH_UNIT_PIXELS)
			{
				iPortion = (pRealWidth[c] * iToAdd) / iTotalFlex;
				iTotalFlex -= pRealWidth[c];
				pRealWidth[c] += iPortion;
				iToAdd -= iPortion;
			}
		}

		// if we're still under, then start taking from the pixel columns
		iTotalWidth = GetSum(pRealWidth, 0, wColumns);
		if (iTotalWidth > target)
		{
			iToAdd = target - iTotalWidth;
			DistributeWeightedStrict(pRealWidth, 0, wColumns, iToAdd);
		}
	}
}

#endif // WEBC_SUPPORT_FRAMES
