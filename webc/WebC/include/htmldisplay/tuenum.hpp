#ifndef __TEXT_UNIT_ENUMERATOR_INCLUDED__
#define __TEXT_UNIT_ENUMERATOR_INCLUDED__

#include "textunit.hpp"
#include "htmldom.hpp"
#include "dhstring.hpp"


class TUOpenNoBr : public TextUnit
{
public:
	// External Property methods
	virtual WEBC_UINT16 GetUnitType(void)       { return TU_TYPE_OPEN_NOBR; }
	virtual WEBC_UINT16 GetBreak(void)          { return TU_BREAK_NONE; }
	virtual WEBC_UINT16 GetClear(void)          { return TU_CLEAR_NONE; }
	virtual WEBC_UINT16 GetDir(void)            { return TU_DIR_LTR; }
	virtual WEBC_UINT16 GetWrap(void)           { return TU_WRAP_NORMAL; }
	virtual WEBC_UINT16 GetVisibility(void)     { return TU_VISIBILITY_HIDE; }
	virtual WEBC_UINT16 GetPosition(void)       { return TU_POSITION_STATIC; }
	virtual WEBC_UINT8  GetFloat(void)          { return TU_FLOAT_NONE; }
	virtual WEBC_UINT8  GetTextAlign(void)      { return TU_ALIGN_NONE; }
	virtual WEBC_UINT8  GetVAlign(void)         { return TU_VALIGN_TOP; }
	virtual DISPLAY_INT GetLeft(void)           { return 0; }
	virtual DISPLAY_INT GetTop(void)            { return 0; }
	virtual DISPLAY_INT GetMinWidth(void)       { return 0; }
	virtual DISPLAY_INT GetMaxWidth(void)       { return 0; }
	virtual DISPLAY_INT GetPixelWidth(void)     { return 0; }
	virtual DISPLAY_INT GetPixelHeight(void)    { return 0; }
	virtual DISPLAY_INT GetTopMargin(void)      { return 0; }
	virtual DISPLAY_INT GetLeftMargin(void)     { return 0; }
	virtual DISPLAY_INT GetBottomMargin(void)   { return 0; }
	virtual DISPLAY_INT GetRightMargin(void)    { return 0; }
	virtual DISPLAY_INT GetBaseline(void)       { return 0; }
	virtual DISPLAY_INT GetParagraphSpace(void) { return 0; }
	
	// Action methods
	virtual void SetParentWidth(DISPLAY_INT iWidth, DISPLAY_INT iAvail) {}
	virtual void SetParentHeight(DISPLAY_INT iHeight) {}
	virtual void SetXYPosition(DISPLAY_INT iXPos, DISPLAY_INT iYPos) {}
	virtual void Release(void) {}
	virtual void SetParentDisplay(DisplayElement *parent, DisplayElement* insertBefore) {}
	virtual void SetDisplayContext(DisplayElement *parent) {}	
};

class TUCloseNoBr : public TUOpenNoBr
{
public:
	virtual WEBC_UINT16 GetUnitType(void) { return TU_TYPE_CLOSE_NOBR; }
};

class DisplayTUEnumerator : public TextUnitEnumerator
{
public:
	DisplayTUEnumerator(HTMLElement *pRoot);
	virtual ~DisplayTUEnumerator(void);
	
	virtual TextUnit *GetNextUnit(void);
	
protected:
	HTMLElement *GetNext(void);
	
	HTMLElement       * mpCurrent;
	int                 miDepth;
	WEBC_BOOL           mbOpenNoBr;
	WEBC_BOOL           mbCloseNoBr;
	WEBC_UINT16         mUnitType;
};

class MinTUEnumerator : public DisplayTUEnumerator
{
public:
	MinTUEnumerator(HTMLElement *pRoot);
	virtual ~MinTUEnumerator(void);
	
	virtual TextUnit *GetNextUnit(void);
	
private:
	WEBC_SIZE miOffset;
	HTMLStringDisplay * mpLastString;
	HTMLMinStringDisplay mStringDisplay;
};

class MaxTUEnumerator : public DisplayTUEnumerator
{
public:
	MaxTUEnumerator(HTMLElement *pRoot);
	virtual ~MaxTUEnumerator(void);
	
	virtual TextUnit *GetNextUnit(void);
	
private:
	WEBC_SIZE miOffset;
	HTMLStringDisplay * mpLastString;
	HTMLMaxStringDisplay mStringDisplay;
};

extern TUOpenNoBr*  gpOpenNoBr;
extern TUCloseNoBr* gpCloseNoBr;

class DisplayElement;

class PositionedTUEnumerator : public TextUnitEnumerator
{
public:
	PositionedTUEnumerator (HTMLElement* root, DisplayElement* displayRoot, DISPLAY_INT topPadding);
	virtual ~PositionedTUEnumerator (void);
	
	virtual TextUnit* GetNextUnit (void);

	DISPLAY_INT getAutoIndentTop (void);
	
protected:	
	HTMLElement*        mpCurrent;
	int                 miDepth;
	DisplayElement*     mDisplayRoot;
	DISPLAY_INT         mAutoIndentTop;

	void updateAutoIndent (HTMLElement* elem);
};

#endif // TEXT_UNIT_ENUMERATOR_INCLUDED
