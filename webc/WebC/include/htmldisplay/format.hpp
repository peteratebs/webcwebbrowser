#ifndef __FORMAT_INCLUDED__
#define __FORMAT_INCLUDED__

#include "wgtypes.hpp"
#include "textunit.hpp"
#include "margins.hpp"

#define LINE_FORMAT_OVERFLOW 1
#define LINE_FORMAT_BREAK    2


struct TextUnitHolder
{
	TextUnit *pTU;
	TextUnitHolder *pNext;
};

class LineContext
{
public:
	LineContext (WEBC_UINT8 textAlign);
	~LineContext (void);
	
	virtual void AddUnit (TextUnit *pTU, WEBC_UINT16 tuType);
	virtual void Finalize (void);

	void Clear (DISPLAY_INT top);
	void ClearLeft (void);
	void ClearRight (void);
	void ClearAll (void);
	WEBC_BOOL ClearWidth (DISPLAY_INT width);
	void SetTop (DISPLAY_INT iTop);
	void SetLeftAndRight (void);	
	WEBC_BOOL Empty (void);
	DISPLAY_INT UsedWidth (void);

	static void ReleaseAllHolders(void);

	DISPLAY_INT miParentWidth;
	DISPLAY_INT miAvailableWidth;
	DISPLAY_INT miLeft;
	DISPLAY_INT miRight;
	DISPLAY_INT miTop;
	
	MarginContext* mpMargin;
	DISPLAY_INT miPrevLineBottomMargin;
protected:
	static TextUnitHolder *AllocHolder (void);
	static void FreeHolder (TextUnitHolder *pHolder);
	
	static TextUnitHolder *mpHolderPool;

	void Break (void);
	void InsertLast (TextUnit *tu, WEBC_UINT16 tuType);
	void UpdateWidth(TextUnit *pTU);
	void SetBreakPoint (void);
	
	void Finalize (
			DISPLAY_INT iBaseline, 
			DISPLAY_INT iHeight,
			DISPLAY_INT textTopPosition,
			DISPLAY_INT textBottomPosition
		);

	void CalculateLineHeight (
			DISPLAY_INT* iBaseline, 
			DISPLAY_INT* iHeight,
			DISPLAY_INT* textTopPosition,
			DISPLAY_INT* textBottomPosition
		);
			
	WEBC_BOOL mbFirstInsideNoBr;
	WEBC_BOOL mbCalculateTopMargin;
	WEBC_UINT8 mTextAlign;
	DISPLAY_INT miWidthSinceBreakPoint;
	DISPLAY_INT miInitialWidth;
	TextUnitHolder* mpFirstUnit;
	TextUnitHolder* mpLastUnit;
	TextUnitHolder* mpBreakPoint;
};

class NonBreakingLineContext : public LineContext
{
public:
	NonBreakingLineContext (WEBC_UINT8 textAlign);
	
	virtual void AddUnit (TextUnit *pTU, WEBC_UINT16 tuType);
};

#endif
