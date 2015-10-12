/*
|  FORMAT.CPP -
|
|  EBS - WebC
|
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
|
|  Module description:
|   [tbd]
*/

#include "format.hpp"
#include "textunit.hpp"
#include "util.h"
#include "webcmem.h"
#include "webcassert.h"

TextUnitHolder *LineContext::mpHolderPool = 0;


LineContext::LineContext (WEBC_UINT8 textAlign)
{
	miLeft = 0;
	miRight = 0;
	miTop = 0;
	miAvailableWidth = 0;
	miInitialWidth = 0;
	mTextAlign = textAlign;
	mpFirstUnit = 0;
	mpLastUnit = 0;
	mpBreakPoint = 0;
	mbFirstInsideNoBr = WEBC_FALSE;
	miWidthSinceBreakPoint = 0;
	mbCalculateTopMargin = WEBC_FALSE;
	miPrevLineBottomMargin = 0;
}

LineContext::~LineContext (void)
{
	if (mpFirstUnit)
	{
		TextUnitHolder *pNext;

		while (mpFirstUnit)
		{
			mpFirstUnit->pTU->Release();
			pNext = mpFirstUnit->pNext;
			FreeHolder(mpFirstUnit);
			mpFirstUnit = pNext;
		}
	}
}


TextUnitHolder *LineContext::AllocHolder (void)
{
TextUnitHolder *pNew;

	if (mpHolderPool)
	{
		pNew = mpHolderPool;
		mpHolderPool = mpHolderPool->pNext;
	}
	else
	{
		pNew = (TextUnitHolder *) WEBC_MALLOC(sizeof(TextUnitHolder));
	}

	return (pNew);
}

void LineContext::FreeHolder (TextUnitHolder *pHolder)
{
	if (pHolder)
	{
		pHolder->pNext = mpHolderPool;
		mpHolderPool = pHolder;
	}

}


WEBC_BOOL LineContext::Empty (void)
{
	return (mpFirstUnit == 0);
}

void LineContext::SetTop (DISPLAY_INT iTop)
{
	miTop = iTop;
}

DISPLAY_INT LineContext::UsedWidth (void)
{
	return (miInitialWidth - miAvailableWidth);
}


void LineContext::AddUnit(TextUnit *tu, WEBC_UINT16 tuType)
{

	if (tuType == TU_TYPE_OPEN_NOBR)
	{
		if (mpMargin->miWrapHold == 0)
		{
			mbFirstInsideNoBr = WEBC_TRUE;
		}

		mpMargin->miWrapHold++;

		return;
	}
	else if (tuType == TU_TYPE_CLOSE_NOBR)
	{
		if (mpMargin->miWrapHold > 0)
		{
			if (mpMargin->miWrapHold == 1)
			{
				if (mpLastUnit && mpLastUnit->pTU->BreakAllowedAfter())
				{
					SetBreakPoint();
				}
			}
			mpMargin->miWrapHold--;
		}
		return;
	}

	// If not a <BR clear=???>, and clear is specified through CSS, then
	//  clear the given margin and finalize the current line if necessary
	if (tuType != TU_TYPE_BREAK)
	{
		switch (tu->GetClear())
		{
		case TU_CLEAR_LEFT:
			if (mpMargin->GetLeftBottom() > miTop)
			{
				if (!Empty())
				{
					Finalize();
				}
				ClearLeft();
			}
			break;

		case TU_CLEAR_RIGHT:
			if (mpMargin->GetRightBottom() > miTop)
			{
				if (!Empty())
				{
					Finalize();
				}
				ClearRight();
			}
			break;

		case TU_CLEAR_ALL:
			if ((mpMargin->GetLeftBottom() > miTop) ||
				(mpMargin->GetRightBottom() > miTop))
			{
				if (!Empty())
				{
					Finalize();
				}
				ClearAll();
			}
			break;

		default:
			break;
		}
	}

	if (tu->GetBreak() & TU_BREAK_BEFORE)
	{
		Finalize();
	}

	TextUnit *firstHalf, *secondHalf;


	while (tu && (tu->GetMinWidth() > EBSMAX(0, miAvailableWidth)))
	{
		if ((mpMargin->miWrapHold == 0) && tu->BreakAtOrBefore(miAvailableWidth, &firstHalf, &secondHalf))
		{
			if (firstHalf)
			{
				InsertLast(firstHalf, firstHalf->GetUnitType());
			}
			Finalize();
			tu = secondHalf;
		}
		else if (!Empty() && (mbFirstInsideNoBr || !mpMargin->miWrapHold) && tu->BreakAllowedBefore())
		{
			Finalize();
			tu->BreakBefore();
		}
		else
		{
			if (mpBreakPoint)
			{
				Break();
			}
		  #if (WEBC_NO_TABLE_AUTO_CLEAR)
			else if ((tuType == TU_TYPE_TABLE) || !ClearWidth(UsedWidth() + tu->MinBreakWidth()))
		  #else
			else if (!ClearWidth(UsedWidth() + tu->MinBreakWidth()))
		  #endif
			{
				if ((mpMargin->miWrapHold == 0) && tu->BreakAtFirstPoint(&firstHalf, &secondHalf))
				{
					if (firstHalf)
					{
						InsertLast(firstHalf, firstHalf->GetUnitType());
					}
					Finalize();
					tu = secondHalf;
				}
				else
				{
					break;
				}
			}
		}
	}
	if (tu)
	{
		if (tu->GetBreak() & TU_BREAK_AFTER)
		{
			InsertLast(tu, tu->GetUnitType());
			Finalize();
		}
		else
		{
			if (mpMargin->miWrapHold == 0)
			{
				if (tu->BreakAllowedAfter() || tu->BreakAllowedInside())
				{
					InsertLast(tu, tu->GetUnitType());
					SetBreakPoint();
				}
				else
				{
					if (tu->BreakAllowedBefore())
					{
						SetBreakPoint();
					}
					InsertLast(tu, tu->GetUnitType());
				}

			}
			else
			{
				if (mbFirstInsideNoBr && tu->BreakAllowedBefore())
				{
					SetBreakPoint();
				}

				InsertLast(tu, tu->GetUnitType());
			}
		}
	}

}

void LineContext::InsertLast(TextUnit *pTU, WEBC_UINT16 tuType)
{
	TextUnitHolder *pHolder = AllocHolder();
	if (!pHolder)
	{
		pTU->Release();
		return;
	}
	pTU->SetParentWidth(miParentWidth, miAvailableWidth);
	pHolder->pTU = pTU;
	pHolder->pNext = 0;
	if (mpLastUnit)
	{
		mpLastUnit->pNext = pHolder;
	}
	else
	{
		mpFirstUnit = pHolder;
	}
	mpLastUnit = pHolder;

	UpdateWidth(pTU);

	mbFirstInsideNoBr = WEBC_FALSE;

	if (pTU->GetTopMargin())
	{
		mbCalculateTopMargin = WEBC_TRUE;
	}
}

void LineContext::Finalize (void)
{
	if (!mpFirstUnit)
	{
		return;
	}

	DISPLAY_INT baselineHeight;
	DISPLAY_INT lineHeight;
	DISPLAY_INT textAboveBaseline;
	DISPLAY_INT textBelowBaseline;

	CalculateLineHeight (
			&baselineHeight,
			&lineHeight,
			&textAboveBaseline,
			&textBelowBaseline
		);

	Finalize (
			baselineHeight,
			lineHeight,
			textAboveBaseline,
			textBelowBaseline
		);
}

void LineContext::CalculateLineHeight (
		DISPLAY_INT* baselineHeight,
		DISPLAY_INT* lineHeight,
		DISPLAY_INT* textAboveBaseline,
		DISPLAY_INT* textBelowBaseline
	)
{
	TextUnitHolder *pHolder;

	// calculate heights
	DISPLAY_INT sizeNeededAboveBaseline = 0;
	DISPLAY_INT sizeNeededBelowBaseline = 0;
	DISPLAY_INT sizeNeededAboveTextBottom = 0;
	DISPLAY_INT sizeNeededBelowTextTop = 0;
	DISPLAY_INT maxHeight = 0;
	DISPLAY_INT maxMiddleHeight = 0;

	*textBelowBaseline = 0; // relative to the baseline
	*textAboveBaseline = 0;    // relative to the baseline (positive only)

	pHolder = mpFirstUnit;
	int isFirstOnLine = 1;
	while (pHolder)
	{
		TextUnit* tu = pHolder->pTU;
		WEBC_UINT16 tuType = tu->GetUnitType();
		WEBC_UINT16 tuVAlign = tu->GetVAlign();
		DISPLAY_INT tuHeight = (!isFirstOnLine && (tuType == TU_TYPE_BREAK))? 0 : tu->GetPixelHeight();
		DISPLAY_INT tuBaseline = tu->GetBaseline();

		if (tuType == TU_TYPE_TEXT)
		{
			*textAboveBaseline = EBSMAX(*textAboveBaseline, tuBaseline);
			*textBelowBaseline = EBSMAX(*textBelowBaseline, tuHeight - tuBaseline);
		}
		else
		{
			switch (tuVAlign)
			{
				case TU_VALIGN_BASELINE:
					sizeNeededAboveBaseline = EBSMAX(sizeNeededAboveBaseline, tuBaseline);
					sizeNeededBelowBaseline = EBSMAX(sizeNeededBelowBaseline, tuHeight - tuBaseline);
					break;

				case TU_VALIGN_TEXTBOTTOM:
					sizeNeededAboveTextBottom = EBSMAX(sizeNeededAboveTextBottom, tuHeight);
					break;

				case TU_VALIGN_TEXTTOP:
					sizeNeededBelowTextTop = EBSMAX(sizeNeededBelowTextTop, tuHeight);
					break;

				case TU_VALIGN_MIDDLE:
					maxMiddleHeight = EBSMAX(maxMiddleHeight, tuHeight);
					break;

				default:
					break;
			}

			maxHeight = EBSMAX(maxHeight, tuHeight);
		}

		isFirstOnLine = 0;
		pHolder = pHolder->pNext;
	}

	// find the baseline
	//  account for text
	DISPLAY_INT iBaseline = *textAboveBaseline;

	//  account for valign == baseline units
	if (iBaseline < sizeNeededAboveBaseline)
	{
		iBaseline = sizeNeededAboveBaseline;
	}

	//  account for valign == textbottom units
	if (iBaseline < sizeNeededAboveTextBottom - *textBelowBaseline)
	{
		iBaseline = sizeNeededAboveTextBottom - *textBelowBaseline;
	}

	//  account for valign == texttop units
	// (no effect on baseline)

	//  account for valign == middle units
	if (iBaseline < ((maxMiddleHeight - (*textAboveBaseline + *textBelowBaseline)) / 2 + *textAboveBaseline))
	{
		iBaseline = ((maxMiddleHeight - (*textAboveBaseline + *textBelowBaseline)) / 2 + *textAboveBaseline);
	}

	// find height
	DISPLAY_INT iHeight = EBSMAX(maxHeight, iBaseline);

	//  account for text
	if (iHeight < iBaseline + *textBelowBaseline)
	{
		iHeight = iBaseline + *textBelowBaseline;
	}

	//  account for valign == baseline units
	if (iHeight < iBaseline + sizeNeededBelowBaseline)
	{
		iHeight = iBaseline + sizeNeededBelowBaseline;
	}

	//  account for valign == texttop units
	if (iHeight < iBaseline - *textAboveBaseline + sizeNeededBelowTextTop)
	{
		iHeight = iBaseline - *textAboveBaseline + sizeNeededBelowTextTop;
	}

	//  account for valign == middle units
	if (*textAboveBaseline == 0 && *textBelowBaseline == 0)
	{
		if (iHeight < maxMiddleHeight)
		{
			iHeight = maxMiddleHeight;
		}
	}
	else
	{
		if (iHeight < iBaseline + *textBelowBaseline - (*textAboveBaseline + *textBelowBaseline + maxMiddleHeight) / 2 + maxMiddleHeight)
		{
			iHeight = iBaseline + *textBelowBaseline - (*textAboveBaseline + *textBelowBaseline + maxMiddleHeight) / 2 + maxMiddleHeight;
		}
	}

	*baselineHeight = iBaseline;
	*lineHeight = iHeight;
}

DISPLAY_INT GetTextUnitYPos (
		WEBC_UINT16 tuVAlign,
		DISPLAY_INT tuHeight,
		DISPLAY_INT tuBaseline,
		DISPLAY_INT lineTop,
		DISPLAY_INT lineBaseline,
		DISPLAY_INT lineBottom,
		DISPLAY_INT textAboveBaseline,
		DISPLAY_INT textBelowBaseline
	)
{
	switch (tuVAlign)
	{
		case TU_VALIGN_BASELINE:
			return (lineBaseline - tuBaseline);

		case TU_VALIGN_MIDDLE:
			if (textAboveBaseline == 0 && textBelowBaseline == 0)
			{
				return ((lineTop + lineBottom  + 1 - tuHeight) / 2);
			}
			return (lineBaseline + (textBelowBaseline - (textAboveBaseline + tuHeight)) / 2);

		case TU_VALIGN_TEXTBOTTOM:
			return (lineBaseline + textBelowBaseline - tuHeight);

		case TU_VALIGN_TEXTTOP:
			return (lineBaseline - textAboveBaseline);

		case TU_VALIGN_TOP:
			return (lineTop);

		default:
			break;
	}

	return (lineBottom - tuHeight);
}

void LineContext::Finalize (
		DISPLAY_INT iBaseline,
		DISPLAY_INT iHeight,
		DISPLAY_INT textAboveBaseline,
		DISPLAY_INT textBelowBaseline
	)
{
	TextUnitHolder *pHolder;

	iBaseline += miTop;

	DISPLAY_INT iBottom = miTop + iHeight;

	DISPLAY_INT iXPos, iYPos;

	// account for horizontal alignment
	WEBC_UINT16 hAlign = mpFirstUnit->pTU->GetTextAlign();
	if (hAlign == TU_ALIGN_NONE)
	{
		hAlign = mTextAlign;
	}

	switch (hAlign)
	{
		case TU_ALIGN_CENTER:
			iXPos = miLeft + (miAvailableWidth >> 1);
			break;

		case TU_ALIGN_RIGHT:
			iXPos = miLeft + miAvailableWidth;
			break;

		default:
			iXPos = miLeft;
			break;
	}

	// calculate margin top
	DISPLAY_INT iTopMargin = 0;
	if (mbCalculateTopMargin)
	{
		DISPLAY_INT iMarginTop = miTop;
		pHolder = mpFirstUnit;
		while (pHolder)
		{
			iYPos = GetTextUnitYPos (
					pHolder->pTU->GetVAlign(),
					pHolder->pTU->GetPixelHeight(),
					pHolder->pTU->GetBaseline(),
					miTop,
					iBaseline,
					iBottom,
					textAboveBaseline,
					textBelowBaseline
				);

			DISPLAY_INT tuMarginTop = iYPos - pHolder->pTU->GetTopMargin();
			if (tuMarginTop < iMarginTop)
			{
				iMarginTop = tuMarginTop;
			}

			pHolder = pHolder->pNext;
		}

		if (iMarginTop < miTop)
		{
			iTopMargin = (miTop - iMarginTop);
		}

		mbCalculateTopMargin = WEBC_FALSE;
	}
	else
	{
		iTopMargin = 0;
	}

	if (miPrevLineBottomMargin > iTopMargin)
	{
		iTopMargin = miPrevLineBottomMargin;
	}

	iBaseline += iTopMargin;
	iBottom += iTopMargin;
	miTop += iTopMargin;

	// make sure starting x position is at or to the right of the left margin
	iXPos = EBSMAX(iXPos, miLeft);

	DISPLAY_INT iNextLine = iBottom;
	mpMargin->PositionQueuedUnits(iNextLine);

	// clear the margins
	if (mpLastUnit->pTU->GetUnitType() == TU_TYPE_BREAK)
	{
		switch (mpLastUnit->pTU->GetClear())
		{
			case TU_CLEAR_LEFT:
				iNextLine = mpMargin->ClearLeft(iBottom);
				break;

			case TU_CLEAR_RIGHT:
				iNextLine = mpMargin->ClearRight(iBottom);
				break;

			case TU_CLEAR_ALL:
				iNextLine = mpMargin->ClearAll(iBottom);
				break;

			default:
				break;
		}
	}

	DISPLAY_INT thisLineMarginBottom = iBottom;

	// position units
	pHolder = mpFirstUnit;
	while (pHolder)
	{
		DISPLAY_INT tuHeight = pHolder->pTU->GetPixelHeight();
		DISPLAY_INT tuBottomMargin = pHolder->pTU->GetBottomMargin();

		iYPos = GetTextUnitYPos (
				pHolder->pTU->GetVAlign(),
				tuHeight,
				pHolder->pTU->GetBaseline(),
				miTop,
				iBaseline,
				iBottom,
				textAboveBaseline,
				textBelowBaseline
			);

		iXPos += pHolder->pTU->GetLeftMargin();

		pHolder->pTU->SetXYPosition(iXPos, iYPos);
		iXPos += pHolder->pTU->GetPixelWidth() + pHolder->pTU->GetRightMargin();

		DISPLAY_INT tuMarginBottom = iYPos + tuHeight + tuBottomMargin;
		if (tuMarginBottom > thisLineMarginBottom)
		{
			thisLineMarginBottom = tuMarginBottom;
		}

		pHolder = pHolder->pNext;
		mpFirstUnit->pTU->Release();
		FreeHolder(mpFirstUnit);
		mpFirstUnit = pHolder;
	}
	mpLastUnit = 0;

	miPrevLineBottomMargin = thisLineMarginBottom - iBottom;

	//ClearBreakPoint();
	mpBreakPoint = 0;
	miWidthSinceBreakPoint = 0;
	//

	miTop = iNextLine + WEBC_LINE_SPACING;
	SetLeftAndRight();
}

void LineContext::UpdateWidth(TextUnit *pTU)
{
	DISPLAY_INT iWidth = pTU->GetPixelWidth() + pTU->GetLeftMargin() + pTU->GetRightMargin();

	miAvailableWidth -= iWidth;
	miWidthSinceBreakPoint += iWidth;
}

void LineContext::ReleaseAllHolders(void)
{
TextUnitHolder *pHolder;

	pHolder = mpHolderPool;
	while (pHolder)
	{
		mpHolderPool = pHolder->pNext;
		WEBC_FREE(pHolder);
		pHolder = mpHolderPool;
	}
}

void LineContext::Clear (DISPLAY_INT top)
{
	miTop = top;
	SetLeftAndRight();
}

void LineContext::ClearLeft (void)
{
	Clear(mpMargin->ClearLeft(miTop));
}

void LineContext::ClearRight (void)
{
	Clear(mpMargin->ClearRight(miTop));
}

void LineContext::ClearAll (void)
{
	Clear(mpMargin->ClearAll(miTop));
}

WEBC_BOOL LineContext::ClearWidth (DISPLAY_INT width)
{
	DISPLAY_INT newTop = mpMargin->ClearWidth(miTop, width);
	if (newTop != miTop)
	{
		Clear(newTop);
		return (WEBC_TRUE);
	}

	return (WEBC_FALSE);
}

void LineContext::SetLeftAndRight (void)
{
	miLeft = mpMargin->GetLeftMargin(miTop + miPrevLineBottomMargin);
	miRight = mpMargin->GetRightMargin(miTop + miPrevLineBottomMargin);
	miInitialWidth = miRight - miLeft + 1;
	miAvailableWidth = miInitialWidth;
}

void LineContext::SetBreakPoint (void)
{
	mpBreakPoint = mpLastUnit;
	miWidthSinceBreakPoint = 0;
}

void LineContext::Break (void)
{
	TextUnitHolder *pNextLineFirst = 0;
	TextUnitHolder *pNextLineLast = 0;
	DISPLAY_INT iNextLineWidth = 0;

	WEBC_ASSERT(mpBreakPoint && mpBreakPoint->pTU);

	if (mpBreakPoint->pTU->BreakAllowedAfter())
	{
		iNextLineWidth = miWidthSinceBreakPoint;
		miAvailableWidth += iNextLineWidth;
		miAvailableWidth += mpBreakPoint->pTU->GetPixelWidth() +
		                    mpBreakPoint->pTU->GetLeftMargin() +
		                    mpBreakPoint->pTU->GetRightMargin();

		mpBreakPoint->pTU->BreakAfter();

		miAvailableWidth -= mpBreakPoint->pTU->GetPixelWidth() +
		                    mpBreakPoint->pTU->GetLeftMargin() +
		                    mpBreakPoint->pTU->GetRightMargin();

		pNextLineFirst = mpBreakPoint->pNext;
		pNextLineLast = (mpBreakPoint == mpLastUnit)? 0 : mpLastUnit;
		mpLastUnit = mpBreakPoint;
		mpBreakPoint->pNext = 0;
	}
	else if (mpBreakPoint->pNext && mpBreakPoint->pNext->pTU->BreakAllowedBefore())
	{
		iNextLineWidth = miWidthSinceBreakPoint;
		miAvailableWidth += iNextLineWidth;
		iNextLineWidth -= mpBreakPoint->pNext->pTU->GetPixelWidth() +
		                  mpBreakPoint->pNext->pTU->GetLeftMargin() +
		                  mpBreakPoint->pNext->pTU->GetRightMargin();

		mpBreakPoint->pNext->pTU->BreakBefore();

		iNextLineWidth += mpBreakPoint->pNext->pTU->GetPixelWidth() +
		                  mpBreakPoint->pNext->pTU->GetLeftMargin() +
		                  mpBreakPoint->pNext->pTU->GetRightMargin();

		pNextLineFirst = mpBreakPoint->pNext;
		pNextLineLast = mpLastUnit;
		mpLastUnit = mpBreakPoint;
		mpBreakPoint->pNext = 0;
	}
	else if (mpBreakPoint->pTU->BreakAllowedInside())
	{
		iNextLineWidth = miWidthSinceBreakPoint;
		miAvailableWidth += iNextLineWidth;
		miAvailableWidth += mpBreakPoint->pTU->GetPixelWidth() +
		                    mpBreakPoint->pTU->GetLeftMargin() +
		                    mpBreakPoint->pTU->GetRightMargin();

		TextUnit *firstHalf, *secondHalf;
		mpBreakPoint->pTU->BreakAtLastPoint(&firstHalf, &secondHalf);
		if (firstHalf)
		{
			// there should always be a first half
			mpBreakPoint->pTU = firstHalf;
			miAvailableWidth -= firstHalf->GetPixelWidth() + firstHalf->GetLeftMargin() + firstHalf->GetRightMargin();
		}

		if (secondHalf)
		{
			TextUnitHolder *pHolder = AllocHolder();
			if (!pHolder)
			{
				// tbd
				// exception!
				return;
			}
			pNextLineFirst = pHolder;
			pHolder->pNext = mpBreakPoint->pNext;
			pHolder->pTU = secondHalf;
			iNextLineWidth += secondHalf->GetPixelWidth() + secondHalf->GetLeftMargin() + secondHalf->GetRightMargin();
		}
		else
		{
			pNextLineFirst = mpBreakPoint->pNext;
		}

		pNextLineLast = (mpBreakPoint == mpLastUnit)? pNextLineFirst : mpLastUnit;
		mpLastUnit = mpBreakPoint;
		mpBreakPoint->pNext = 0;
	}

	Finalize();

	mpFirstUnit = pNextLineFirst;
	mpLastUnit = pNextLineLast;
	miAvailableWidth -= iNextLineWidth;
}

/*****************************************************************************/
// class NonBreakingLineContext
//
/*****************************************************************************/

NonBreakingLineContext::NonBreakingLineContext (WEBC_UINT8 textAlign)
	: LineContext(textAlign)
{
}

void NonBreakingLineContext::AddUnit (TextUnit *pTU, WEBC_UINT16 tuType)
{
	if ((tuType == TU_TYPE_OPEN_NOBR) || (tuType == TU_TYPE_CLOSE_NOBR))
	{
		return;
	}

	// If not a <BR clear=???>, and clear is specified through CSS, then
	//  clear the given margin and finalize the current line if necessary
	if (tuType != TU_TYPE_BREAK)
	{
		switch (pTU->GetClear())
		{
		case TU_CLEAR_LEFT:
			if (mpMargin->GetLeftBottom() > miTop)
			{
				if (!Empty())
				{
					Finalize();
				}
				ClearLeft();
			}
			break;

		case TU_CLEAR_RIGHT:
			if (mpMargin->GetRightBottom() > miTop)
			{
				if (!Empty())
				{
					Finalize();
				}
				ClearRight();
			}
			break;

		case TU_CLEAR_ALL:
			if ((mpMargin->GetLeftBottom() > miTop) ||
				(mpMargin->GetRightBottom() > miTop))
			{
				if (!Empty())
				{
					Finalize();
				}
				ClearAll();
			}
			break;

		default:
			break;
		}
	}

	WEBC_UINT16 tuBreak = pTU->GetBreak();

	if (tuBreak & TU_BREAK_BEFORE)
	{
		Finalize();
	}

	InsertLast(pTU, tuType);

	if (tuBreak & TU_BREAK_AFTER)
	{
		Finalize();
	}
}
