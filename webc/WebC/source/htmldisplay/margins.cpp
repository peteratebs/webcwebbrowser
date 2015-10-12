//
// MARGINS.CPP - Methods for MarginContext - helper class for HTMLFlow::Format
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

#include "margins.hpp"
#include "webcmem.h"


/*****************************************************************************/
// Macros
/*****************************************************************************/

#define ELEMENT_X_TO_DOCUMENT(X)   (X - miElementX)
#define ELEMENT_Y_TO_DOCUMENT(Y)   (Y - miElementY)
#define DOCUMENT_X_TO_ELEMENT(X)   (X + miElementX)
#define DOCUMENT_Y_TO_ELEMENT(Y)   (Y + miElementY)


/*****************************************************************************/
// Local functions
/*****************************************************************************/


/******************************************************************************
* MarginContext::MarginContext() -
*
* Parameters
*
*
* Description -
*
*
* Returns ()
*
******************************************************************************/
MarginContext::MarginContext (DISPLAY_INT iLeft, DISPLAY_INT iRight,
		HTMLFlowDisplay *pFormatRoot)
{
	miWrapHold = 0;
	miElementX = 0;
	miElementY = 0;
	mpLeftUnits = 0;
	mpRightUnits = 0;
	mpFormatRoot = pFormatRoot;

  #if(WEBC_SUPPORT_SMALL_SCREEN)
	if (iRight - iLeft + 1 > WEBC_SCREEN_WIDTH)
	{
		iRight = iLeft + WEBC_SCREEN_WIDTH - 1;
	}
  #endif // WEBC_SUPPORT_SMALL_SCREEN

	Init (iLeft, iRight);
}

/******************************************************************************
* MarginContext::~MarginContext() -
*
* Parameters
*
*
* Description -
*
*
* Returns ()
*
******************************************************************************/
MarginContext::~MarginContext (void)
{
TUFloatRect *pRect;

	while (mpLeftUnits)
	{
		pRect = mpLeftUnits->pNext;
		FreeFloatRect(mpLeftUnits);
		mpLeftUnits = pRect;
	}

	while (mpRightUnits)
	{
		pRect = mpRightUnits->pNext;
		FreeFloatRect(mpRightUnits);
		mpRightUnits = pRect;
	}
}


// #define DISPLAY_MARGIN_INFO(P) printf("%s miLeft==%d miRight=%d miElementX=%d,miLeftBottom=%d, miRightBottom =%d miElementY=%d\n",P,miLeft, miRight,miElementX,miLeftBottom,miRightBottom,miElementY);
#define DISPLAY_MARGIN_INFO(P)
/******************************************************************************
* MarginContext::Init() -
*
* Parameters
*
*
* Description -
*
*
* Returns ()
*
******************************************************************************/
void MarginContext::Init (DISPLAY_INT iLeft, DISPLAY_INT iRight)
{
	miLeft = ELEMENT_X_TO_DOCUMENT(iLeft);
	miRight = ELEMENT_X_TO_DOCUMENT(iRight);

	miLeftBottom = ELEMENT_Y_TO_DOCUMENT(0);
	miRightBottom = ELEMENT_Y_TO_DOCUMENT(0);
	DISPLAY_MARGIN_INFO("Init")
	InitTextUnitQueue(mpUnitQueue);
}

/******************************************************************************
* MarginContext::EnqueueUnit() -
*
* Parameters
*
*
* Description -
*
*
* Returns ()
*
******************************************************************************/
void MarginContext::QueueUnit (TextUnit *pTU)
{
	EnqueueTextUnit(mpUnitQueue, pTU, miElementX, miElementY);
}

/******************************************************************************
* MarginContext::AlignUnitLeft() -
*
* Parameters
*
*
* Description -
*
*
* Returns ()
*
******************************************************************************/
void MarginContext::AlignUnitLeft (TextUnit *pTU, DISPLAY_INT iYPos)
{
	_AlignUnitLeft(pTU, iYPos, miElementX, miElementY);
}

/******************************************************************************
* MarginContext::AlignUnitLeft() -
*
* Parameters
*
*
* Description -
*
*
* Returns ()
*
******************************************************************************/
void MarginContext::_AlignUnitLeft (TextUnit *pTU, DISPLAY_INT iYPos, DISPLAY_INT iXOffset, DISPLAY_INT iYOffset)
{
	DISPLAY_INT elementTopMargin = pTU->GetTopMargin();
	DISPLAY_INT elementBottomMargin = pTU->GetBottomMargin();
	DISPLAY_INT elementLeftMargin = pTU->GetLeftMargin();
	DISPLAY_INT elementRightMargin = pTU->GetRightMargin();
	DISPLAY_INT width = pTU->GetPixelWidth();
	DISPLAY_INT iXPos;

	iYPos = ClearUnit(pTU, iYPos);
  #if (WEBC_NO_TABLE_AUTO_CLEAR)
	if (pTU->GetUnitType() != TU_TYPE_TABLE)
  #endif
	{
		iYPos = ClearWidth(iYPos, width);
	}
	iXPos = GetLeftMargin(iYPos) + elementLeftMargin;
	iXPos = ELEMENT_X_TO_DOCUMENT(iXPos);
	iYPos = ELEMENT_Y_TO_DOCUMENT(iYPos);
	pTU->SetXYPosition(iXPos + iXOffset, iYPos + iYOffset + elementTopMargin);

	DISPLAY_INT top = iYPos;
	DISPLAY_INT bottom = top + pTU->GetPixelHeight() + elementTopMargin + elementBottomMargin - 1;
	DISPLAY_INT right = iXPos + width + elementRightMargin - 1;

	_AddRect(&mpLeftUnits, top, bottom, right);
	miLeftBottom = EBSMAX(miLeftBottom, bottom);
	DISPLAY_MARGIN_INFO("_AlignUnitLeft")
}

/******************************************************************************
* MarginContext::AlignUnitRight() -
*
* Parameters
*
*
* Description -
*
*
* Returns ()
*
******************************************************************************/
void MarginContext::AlignUnitRight (TextUnit *pTU, DISPLAY_INT iYPos)
{
	_AlignUnitRight(pTU, iYPos, miElementX, miElementY);
}

/******************************************************************************
* MarginContext::AlignUnitRight() -
*
* Parameters
*
*
* Description -
*
*
* Returns ()
*
******************************************************************************/
void MarginContext::_AlignUnitRight (TextUnit *pTU, DISPLAY_INT iYPos, DISPLAY_INT iXOffset, DISPLAY_INT iYOffset)
{
	DISPLAY_INT elementTopMargin = pTU->GetTopMargin();
	DISPLAY_INT elementBottomMargin = pTU->GetBottomMargin();
	DISPLAY_INT elementLeftMargin = pTU->GetLeftMargin();
	DISPLAY_INT elementRightMargin = pTU->GetRightMargin();
	DISPLAY_INT width = pTU->GetPixelWidth();
	DISPLAY_INT iXPos;

	iYPos = ClearUnit(pTU, iYPos);
  #if (WEBC_NO_TABLE_AUTO_CLEAR)
	if (pTU->GetUnitType() != TU_TYPE_TABLE)
  #endif
  	{
		iYPos = ClearWidth(iYPos, width);
	}
	iXPos = GetRightMargin(iYPos) - (elementRightMargin + width - 1);
	iXPos = ELEMENT_X_TO_DOCUMENT(iXPos);
	iXPos = EBSMAX(0, iXPos);
	iYPos = ELEMENT_Y_TO_DOCUMENT(iYPos);
	pTU->SetXYPosition(iXPos + iXOffset, iYPos + iYOffset + elementTopMargin);

	DISPLAY_INT top = iYPos;
	DISPLAY_INT bottom = top + pTU->GetPixelHeight() + elementTopMargin + elementBottomMargin - 1;
	DISPLAY_INT left = iXPos - elementLeftMargin;

	_AddRect(&mpRightUnits, top, bottom, left);
	miRightBottom = EBSMAX(miRightBottom, bottom);
	DISPLAY_MARGIN_INFO("_AlignUnitLeft")
}

/******************************************************************************
* MarginContext::ClearUnit() -
*
* Parameters
*
*
* Description -
*
*
* Returns ()
*
******************************************************************************/
DISPLAY_INT MarginContext::ClearUnit (TextUnit *pTU, DISPLAY_INT iYPos)
{
	switch (pTU->GetClear())
	{
		case TU_CLEAR_LEFT:
			return (ClearLeft(iYPos));

		case TU_CLEAR_RIGHT:
			return (ClearRight(iYPos));

		case TU_CLEAR_ALL:
			return (ClearAll(iYPos));

		default:
			return (iYPos);
	}
}

/******************************************************************************
* MarginContext::DequeueUnits() -
*
* Parameters
*
*
* Description -
*
*
* Returns ()
*
******************************************************************************/
void MarginContext::PositionQueuedUnits (DISPLAY_INT iYPos)
{
TextUnit *pTextUnit;
DISPLAY_INT iXOffset, iYOffset;

	// Dequeue all aligned units in mpUnitQueue, and position in left or right margin
	pTextUnit = DequeueTextUnit(mpUnitQueue, &iXOffset, &iYOffset);
	while (pTextUnit)
	{
		switch (pTextUnit->GetFloat())
		{
			case TU_FLOAT_LEFT:
				_AlignUnitLeft(pTextUnit, iYPos, iXOffset, iYOffset);
				break;

			case TU_FLOAT_RIGHT:
				_AlignUnitRight(pTextUnit, iYPos, iXOffset, iYOffset);
				break;

			default:
				break;
		}

		pTextUnit = DequeueTextUnit(mpUnitQueue, &iXOffset, &iYOffset);
	}
}

/******************************************************************************
* MarginContext::ClearLeft() -
*
* Parameters
*
*
* Description -
*
*
* Returns ()
*
******************************************************************************/
DISPLAY_INT MarginContext::ClearLeft (DISPLAY_INT iYPos)
{
	return (EBSMAX(iYPos, DOCUMENT_Y_TO_ELEMENT(miLeftBottom) + 1));
}

/******************************************************************************
* MarginContext::ClearRight() -
*
* Parameters
*
*
* Description -
*
*
* Returns ()
*
******************************************************************************/
DISPLAY_INT MarginContext::ClearRight (DISPLAY_INT iYPos)
{
	return (EBSMAX(iYPos, DOCUMENT_Y_TO_ELEMENT(miRightBottom) + 1));
}

/******************************************************************************
* MarginContext::ClearAll() -
*
* Parameters
*
*
* Description -
*
*
* Returns ()
*
******************************************************************************/
DISPLAY_INT MarginContext::ClearAll (DISPLAY_INT iYPos)
{
	return (EBSMAX(iYPos, DOCUMENT_Y_TO_ELEMENT(EBSMAX(miLeftBottom, miRightBottom)) + 1));
}


/******************************************************************************
* MarginContext::ClearWidth() -
*
* Parameters
*
*
* Description -
*
*
* Returns ()
*
******************************************************************************/
DISPLAY_INT MarginContext::ClearWidth (DISPLAY_INT iYPos, DISPLAY_INT iWidth)
{
TUFloatRect *pLeft, *pRight;
DISPLAY_INT space;

	iYPos = ELEMENT_Y_TO_DOCUMENT(iYPos);

	pLeft = _GetLeftUnit(iYPos);
	pRight = _GetRightUnit(iYPos);
	space = (pRight? pRight->side : miRight) - (pLeft? pLeft->side : miLeft);

	while ((pLeft || pRight) && (space < iWidth))
	{
		if (pLeft && pRight)
		{
			iYPos = EBSMIN(pLeft->bottom, pRight->bottom) + 1;
		}
		else if (pLeft)
		{
			iYPos = pLeft->bottom + 1;
		}
		else
		{
			iYPos = pRight->bottom + 1;
		}

		pLeft = _GetLeftUnit(iYPos);
		pRight = _GetRightUnit(iYPos);
		space = (pRight? pRight->side : miRight) - (pLeft? pLeft->side : miLeft);
	}

	return (DOCUMENT_Y_TO_ELEMENT(iYPos));
}

/******************************************************************************
* MarginContext::GetLeftBottom() -
*
* Parameters
*
*
* Description -
*
*
* Returns ()
*
******************************************************************************/
DISPLAY_INT MarginContext::GetLeftBottom (void)
{
	return (DOCUMENT_Y_TO_ELEMENT(miLeftBottom));
}

/******************************************************************************
* MarginContext::GetRightBottom() -
*
* Parameters
*
*
* Description -
*
*
* Returns ()
*
******************************************************************************/
DISPLAY_INT MarginContext::GetRightBottom (void)
{
	return (DOCUMENT_Y_TO_ELEMENT(miRightBottom));
}

/******************************************************************************
* MarginContext::GetLeftMargin() -
*
* Parameters
*
*
* Description -
*
*
* Returns ()
*
******************************************************************************/
DISPLAY_INT MarginContext::GetLeftMargin (DISPLAY_INT iYPos)
{
	TUFloatRect *pUnit = _GetLeftUnit(ELEMENT_Y_TO_DOCUMENT(iYPos));
	if (pUnit && pUnit->side > miLeft)
	{
		return (DOCUMENT_X_TO_ELEMENT(pUnit->side));
	}
	return (DOCUMENT_X_TO_ELEMENT(miLeft));
}

/******************************************************************************
* MarginContext::GetRightMargin() -
*
* Parameters
*
*
* Description -
*
*
* Returns ()
*
******************************************************************************/
DISPLAY_INT MarginContext::GetRightMargin (DISPLAY_INT iYPos)
{
	TUFloatRect *pUnit = _GetRightUnit(ELEMENT_Y_TO_DOCUMENT(iYPos));
	if (pUnit && pUnit->side < miRight)
	{
		return (DOCUMENT_X_TO_ELEMENT(pUnit->side));
	}
	return (DOCUMENT_X_TO_ELEMENT(miRight));
}


/******************************************************************************
* MarginContext::GetLeft() -
*
* Parameters
*
*
* Description -
*
*
* Returns ()
*
******************************************************************************/
DISPLAY_INT MarginContext::GetLeft (void)
{
	return (DOCUMENT_X_TO_ELEMENT(miLeft));
}

/******************************************************************************
* MarginContext::GetRight() -
*
* Parameters
*
*
* Description -
*
*
* Returns ()
*
******************************************************************************/
DISPLAY_INT MarginContext::GetRight (void)
{
	return (DOCUMENT_X_TO_ELEMENT(miRight));
}

/******************************************************************************
* MarginContext::EnterElement() -
*
* Parameters
*
*
* Description -
*
*
* Returns ()
*
******************************************************************************/
void MarginContext::EnterElement (DISPLAY_INT iRelativeX, DISPLAY_INT iRelativeY)
{
	miElementX -= iRelativeX;
	miElementY -= iRelativeY;
	DISPLAY_MARGIN_INFO("EnterElement")
}

/******************************************************************************
* MarginContext::ExitElement() -
*
* Parameters
*
*
* Description -
*
*
* Returns ()
*
******************************************************************************/
void MarginContext::ExitElement (DISPLAY_INT iRelativeX, DISPLAY_INT iRelativeY)
{
	miElementX += iRelativeX;
	miElementY += iRelativeY;
	DISPLAY_MARGIN_INFO("ExitElement")
}

TUFloatRect *MarginContext::_GetLeftUnit(DISPLAY_INT iYPos)
{
	if (iYPos > miLeftBottom)
	{
		return (0);
	}

	TUFloatRect *pRect, *pMatch = 0;
	DISPLAY_INT leftEdge = miLeft;

	pRect = mpLeftUnits;
	while (pRect)
	{
		if (iYPos >= pRect->top && iYPos <= pRect->bottom)
		{
			if (!pMatch || leftEdge < pRect->side)
			{
				pMatch = pRect;
				leftEdge = pRect->side;
			}
		}
		pRect = pRect->pNext;
	}

	return (pMatch);
}

TUFloatRect *MarginContext::_GetRightUnit(DISPLAY_INT iYPos)
{
	if (iYPos > miRightBottom)
	{
		return (0);
	}

	TUFloatRect *pRect, *pMatch = 0;
	DISPLAY_INT rightEdge = 0;

	pRect = mpRightUnits;
	while (pRect)
	{
		if (iYPos >= pRect->top && iYPos <= pRect->bottom)
		{
			if (!pMatch || rightEdge > pRect->side)
			{
				pMatch = pRect;
				rightEdge = pRect->side;
			}
		}
		pRect = pRect->pNext;
	}

	return (pMatch);
}

void MarginContext::_AddRect (
		TUFloatRect** list,
		DISPLAY_INT top,
		DISPLAY_INT bottom,
		DISPLAY_INT side
	)
{
TUFloatRect *pRect = AllocFloatRect();

	if (pRect)
	{
		pRect->top = top;
		pRect->bottom = bottom;
		pRect->side = side;
		pRect->pNext = *list;
		*list = pRect;
	}
}

void MarginContext::AddPadding(DISPLAY_INT iLeftPad, DISPLAY_INT iRightPad)
{
	miLeft += iLeftPad;
	miRight -= iRightPad;
	DISPLAY_MARGIN_INFO("AddPadding")
}

void MarginContext::RemovePadding(DISPLAY_INT iLeftPad, DISPLAY_INT iRightPad)
{
	miLeft -= iLeftPad;
	miRight += iRightPad;
	DISPLAY_MARGIN_INFO("RemovePadding")
}

TUFloatRect *MarginContext::AllocFloatRect(void)
{
	return (TUFloatRect*) WEBC_MALLOC(sizeof(TUFloatRect));
}

void MarginContext::FreeFloatRect(TUFloatRect *p)
{
	WEBC_FREE(p);
}


HTMLFlowDisplay *MarginContext::GetFormatRoot (void)
{
	return (mpFormatRoot);
}

void MarginContext::SetFormatRoot (HTMLFlowDisplay *pRoot)
{
	mpFormatRoot = pRoot;
}



/***************************************************************************************/
/***************************************************************************************/


/******************************************************************************
* void InitTextUnitStack(TextUnitStack *pStack) - Initialize a stack
*
* Parameters
*    pStack - Pointer to TextUnitStack struct to initialize
*
* Description
*    TextUnit stacks and queues are used in the HTMLFlow::Format method.
*
* Returns (void)
*    Nothing.
******************************************************************************/

void InitTextUnitStack(TextUnitStack *pStack)
{
	pStack->iTop = -1;
}

/******************************************************************************
* void InitTextUnitQueue(TextUnitQueue *pQueue) - Initialize a queue
*
* Parameters
*    pQueue - Pointer to TextUnitQueue struct to initialize
*
* Description
*    TextUnit stacks and queues are used in the HTMLFlow::Format method.
*
* Returns (void)
*    Nothing.
******************************************************************************/

void InitTextUnitQueue(TextUnitQueue *pQueue)
{
	pQueue->iBegin = 0;
	pQueue->iEnd = 0;
}

/******************************************************************************
* void PushTextUnit(TextUnitStack *pStack, TextUnit *TU) - Push a TextUnit
*    onto the stack.
*
* Parameters
*    pStack - The stack to push onto
*    TU - The TextUnit to push
*
* Description
*    TextUnit stacks and queues are used in the HTMLFlow::Format method.
*
* Returns (void)
*    Nothing.
******************************************************************************/

void PushTextUnit(TextUnitStack *pStack, DISPLAY_INT width, DISPLAY_INT bottom)
{
	if (pStack->iTop + 1 < TEXT_UNIT_STACK_SIZE)
	{
		pStack->iTop++;
		pStack->pUnits[pStack->iTop].width = width;
		pStack->pUnits[pStack->iTop].bottom = bottom;
	}
}

/******************************************************************************
* TextUnit * PopTextUnit(TextUnitStack *pStack) - Pop a TextUnit
*    off the stack.
*
* Parameters
*    pStack - The stack to pop a TU off of
*
* Description
*    TextUnit stacks and queues are used in the HTMLFlow::Format method.
*
* Returns ()
*    nothing
******************************************************************************/

void PopTextUnit(TextUnitStack *pStack)
{
	if (pStack->iTop >= 0)
	{
		pStack->iTop--;
	}
}

TUStackLevel *StackPeek(TextUnitStack *pStack)
{
	if (pStack->iTop >= 0)
	{
		return (&pStack->pUnits[pStack->iTop]);
	}

	return (0);
}


/******************************************************************************
* void EnqueueTextUnit(TextUnitQueue *pQueue, TextUnit *TU) - Enqueue a
*    TextUnit
*
* Parameters
*    pQueue - The queue to queue into
*    TU - The TextUnit to enqueue
*
* Description
*    TextUnit stacks and queues are used in the HTMLFlow::Format method.
*
* Returns (void)
*    Nothing.
******************************************************************************/

void EnqueueTextUnit(TextUnitQueue *pQueue, TextUnit *TU, DISPLAY_INT iXOffset, DISPLAY_INT iYOffset)
{
short iNewEnd;

	iNewEnd = pQueue->iEnd + 1;
	if (iNewEnd >= TEXT_UNIT_QUEUE_SIZE)
	{
		iNewEnd = 0;
	}

	if (iNewEnd != pQueue->iBegin)
	{
		pQueue->pUnits[iNewEnd].pTU = TU;
		pQueue->pUnits[iNewEnd].iXOffset = iXOffset;
		pQueue->pUnits[iNewEnd].iYOffset = iYOffset;
		pQueue->iEnd = iNewEnd;
	}
}

/******************************************************************************
* TextUnit * DequeueTextUnit(TextUnitQueue *pQueue) - Dequeue a TextUnit
*
* Parameters
*    pQueue - The queue to retreive a TextUnit from
*
* Description
*    TextUnit stacks and queues are used in the HTMLFlow::Format method.
*
* Returns (TextUnit *)
*    A pointer to the dequeued TextUnit or NULL if queue is empty.
******************************************************************************/

TextUnit *DequeueTextUnit(TextUnitQueue *pQueue, DISPLAY_INT *piXOffset, DISPLAY_INT *piYOffset)
{
TextUnit *TU = 0;

	if (pQueue->iBegin != pQueue->iEnd)
	{
		pQueue->iBegin++;
		if (pQueue->iBegin >= TEXT_UNIT_QUEUE_SIZE)
		{
			pQueue->iBegin = 0;
		}
		TU = pQueue->pUnits[pQueue->iBegin].pTU;
		if (piXOffset)
		{
			*piXOffset = pQueue->pUnits[pQueue->iBegin].iXOffset;
		}
		if (piYOffset)
		{
			*piYOffset = pQueue->pUnits[pQueue->iBegin].iYOffset;
		}
	}

	return (TU);
}
