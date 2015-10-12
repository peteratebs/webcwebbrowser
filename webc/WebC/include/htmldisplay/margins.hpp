#ifndef __MARGINS_HPP_INCLUDED__
#define __MARGINS_HPP_INCLUDED__

#include "htmldom.hpp"
#include "wgtypes.hpp"


#define TEXT_UNIT_STACK_SIZE 30
#define TEXT_UNIT_QUEUE_SIZE 30

typedef TextUnit *TextUnitPtr;

struct TUStackLevel 
{
	DISPLAY_INT width;
	DISPLAY_INT bottom;
};

struct TUFloatRect
{
	DISPLAY_INT top;
	DISPLAY_INT bottom;
	DISPLAY_INT side;
	TUFloatRect *pNext;
};

struct TUQueuePos
{
	TextUnit *pTU;
	DISPLAY_INT iXOffset;
	DISPLAY_INT iYOffset;
};

struct TextUnitStack
{
	TUStackLevel pUnits[TEXT_UNIT_STACK_SIZE];
	short iTop;
};

struct TextUnitQueue
{
	TUQueuePos pUnits[TEXT_UNIT_QUEUE_SIZE];
	short iBegin;
	short iEnd;
};

void InitTextUnitStack(TextUnitStack *pStack);
void PushTextUnit (TextUnitStack *pStack, DISPLAY_INT width, DISPLAY_INT bottom);
void PopTextUnit (TextUnitStack *pStack);
TUStackLevel *StackPeek (TextUnitStack *pStack);

void InitTextUnitQueue(TextUnitQueue *pQueue);
void EnqueueTextUnit(TextUnitQueue *pQueue, TextUnit *TU, DISPLAY_INT iXOffset, DISPLAY_INT iYOffset);
TextUnit *DequeueTextUnit(TextUnitQueue *pQueue, DISPLAY_INT *piXOffset, DISPLAY_INT *piYOffset);

class HTMLFlowDisplay;

class MarginContext
{
public:
	MarginContext (DISPLAY_INT iLeft, DISPLAY_INT iRight, HTMLFlowDisplay *pFormatRoot);
	~MarginContext (void);

	// public methods
	void Init (DISPLAY_INT iLeft, DISPLAY_INT iRight);	
	void QueueUnit (TextUnit *pTU);
	void AlignUnitLeft (TextUnit *pTU, DISPLAY_INT iYPos);
	void AlignUnitRight (TextUnit *pTU, DISPLAY_INT iYPos);
	void PositionQueuedUnits (DISPLAY_INT iYPos);
	
	// Return the highest point below iYPos and the lowest left-aligned element
	DISPLAY_INT ClearLeft (DISPLAY_INT iYPos); 
	
	// Set current position to highest point below iYPos and the lowest right-aligned element
	DISPLAY_INT ClearRight (DISPLAY_INT iYPos);
	
	// Set current position to highest point below iYPos and any aligned elements
	DISPLAY_INT ClearAll (DISPLAY_INT iYPos);
	
	// Set current position to highest point below iYPos with at least iWidth pixels between the left and right margins
	DISPLAY_INT ClearWidth (DISPLAY_INT iYPos, DISPLAY_INT iWidth);	

	DISPLAY_INT ClearUnit (TextUnit *pTU, DISPLAY_INT iYPos);
	
	DISPLAY_INT GetLeftBottom (void);
	DISPLAY_INT GetRightBottom (void);
	DISPLAY_INT GetLeftMargin (DISPLAY_INT iYPos);
	DISPLAY_INT GetRightMargin (DISPLAY_INT iYPos);
	DISPLAY_INT GetLeft (void);
	DISPLAY_INT GetRight (void);
	
	void EnterElement (DISPLAY_INT iRelativeX, DISPLAY_INT iRelativeY);
	void ExitElement (DISPLAY_INT iRelativeX, DISPLAY_INT iRelativeY);
	
	void AddPadding (DISPLAY_INT iLeftPad, DISPLAY_INT iRightPad);
	void RemovePadding (DISPLAY_INT iLeftPad, DISPLAY_INT iRightPad);

	HTMLFlowDisplay *GetFormatRoot (void);
	void SetFormatRoot (HTMLFlowDisplay *pRoot);

	int miWrapHold;

private:
	// private methods
	void _AlignUnitLeft (TextUnit *pTU, DISPLAY_INT iYPos, DISPLAY_INT iXOffset, DISPLAY_INT iYOffset);
	void _AlignUnitRight (TextUnit *pTU, DISPLAY_INT iYPos, DISPLAY_INT iXOffset, DISPLAY_INT iYOffset);
	TUFloatRect *_GetLeftUnit(DISPLAY_INT iYPos);
	TUFloatRect *_GetRightUnit(DISPLAY_INT iYPos);
	void _AddRect(TUFloatRect **pList, DISPLAY_INT top, DISPLAY_INT bottom, DISPLAY_INT side);
	
	static TUFloatRect *AllocFloatRect(void);
	static void FreeFloatRect(TUFloatRect *p);
	
	// private data
	TextUnitQueue mpUnitQueue[1];
	TUFloatRect *mpLeftUnits, *mpRightUnits;	
	DISPLAY_INT miLeft;
	DISPLAY_INT miRight;
	DISPLAY_INT miLeftBottom;
	DISPLAY_INT miRightBottom;
	DISPLAY_INT miElementX;
	DISPLAY_INT miElementY;
	HTMLFlowDisplay *mpFormatRoot;
};

#endif // __MARGINS_HPP_INCLUDED__
