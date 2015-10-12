#ifndef __DHLI_HPP__
#define __DHLI_HPP__

#include "delement.hpp"
#include "dflow.hpp"
#include "tuenum.hpp"

class HTMLListItem;
class HTMLListItemDisplay;

class HTMLListItemBulletDisplay :  public TextUnit, public DisplayElement
{
protected:
	HTMLListItem*        mpListItem;
	HTMLListItemDisplay* mpDisplayParent;
	WebChar mLabel[12];

	void GetLabel(WebChar *pLabel);

	virtual WEBC_BOOL SetElementStyle(void);

public:
	HTMLListItemBulletDisplay (HTMLListItem *pElem);
	virtual ~HTMLListItemBulletDisplay (void);

	// DisplayElement methods
	virtual void Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC);
	virtual DISPLAYElementType Type (void) { return DISPLAY_ELEMENT_ITEMBULLETDISPLAY; }

	// TextUnit methods
	virtual WEBC_UINT16 GetUnitType(void)       { return TU_TYPE_DEFAULT; }
	virtual WEBC_UINT16 GetBreak(void)          { return TU_BREAK_NONE; }
	virtual WEBC_UINT16 GetClear(void)          { return TU_CLEAR_NONE; }
	virtual WEBC_UINT16 GetDir(void)            { return TU_DIR_LTR; }
	virtual WEBC_UINT16 GetWrap(void)           { return TU_WRAP_NORMAL; }
	virtual WEBC_UINT16 GetChildWrap(void)      { return TU_WRAP_NORMAL; }
	virtual WEBC_UINT16 GetPosition(void)       { return TU_POSITION_STATIC; }
	virtual WEBC_UINT8  GetFloat(void)          { return TU_FLOAT_NONE; }
	virtual WEBC_UINT8  GetTextAlign(void)      { return TU_ALIGN_LEFT; }
	virtual WEBC_UINT8  GetVAlign(void)         { return TU_VALIGN_BASELINE; }
	virtual WEBC_UINT16 GetVisibility(void);
	virtual DISPLAY_INT GetMinWidth(void)       { return GetPixelWidth(); }
	virtual DISPLAY_INT GetMaxWidth(void)       { return GetPixelWidth(); }
	virtual DISPLAY_INT GetBaseline(void);
	virtual DISPLAY_INT GetLeft(void)           { return 0; }
	virtual DISPLAY_INT GetTop(void)            { return 0; }
	virtual DISPLAY_INT GetParagraphSpace(void) { return 0; }
	virtual DISPLAY_INT GetPixelWidth(void);
	virtual DISPLAY_INT GetPixelHeight(void);
	virtual DISPLAY_INT GetTopMargin(void)      { return 0; }
	virtual DISPLAY_INT GetLeftMargin(void)     { return 0; }
	virtual DISPLAY_INT GetBottomMargin(void)   { return 0; }
	virtual DISPLAY_INT GetRightMargin(void)    { return 0; }

	// Action methods
	virtual void SetParentWidth(DISPLAY_INT iWidth, DISPLAY_INT iAvail) {};
	virtual void SetParentHeight(DISPLAY_INT iHeight) {};
	virtual void Release(void) {};

	virtual void Format(void) {};

	virtual void SetXYPosition(DISPLAY_INT iXPos, DISPLAY_INT iYPos);
	virtual void SetParentDisplay(DisplayElement *parent, DisplayElement* insertBefore);
	virtual void SetDisplayContext(DisplayElement *parent);

	void SetParentListItemDisplay (HTMLListItemDisplay *pDisplayParent);
	CSSListStylePosition GetListStylePosition (void);
};

class HTMLListItemContentEnumerator : public DisplayTUEnumerator
{
protected:
	int mState;
	HTMLListItemBulletDisplay * mpBullet;

public:
	HTMLListItemContentEnumerator (HTMLElement *pRoot, HTMLListItemBulletDisplay *pBullet);
	virtual ~HTMLListItemContentEnumerator (void) {}

	virtual TextUnit *GetNextUnit (void);
};

class HTMLListItemDisplay : public HTMLFlowDisplay
{
protected:
	HTMLListItemBulletDisplay mBullet;

	virtual TextUnitEnumerator *GetTextUnits(void);
	virtual LineContext* CreateLineContext(void);

public:
	HTMLListItemDisplay(HTMLListItem *pLI);

	virtual DISPLAY_INT GetPixelHeight(void);
};

#endif // __DHLI_HPP__
