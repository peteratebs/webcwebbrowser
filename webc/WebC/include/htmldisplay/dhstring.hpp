#ifndef __DHSTRING_HPP__
#define __DHSTRING_HPP__

#include "dhelem.hpp"
#include "delement.hpp"
#include "css.h"

class HTMLString;
class ITextDisplayStyle;

class HTMLStringDisplay : public HTMLElementDisplay<DisplayElement>
{
public:
	HTMLStringDisplay (HTMLString *pStrElem);
	virtual ~HTMLStringDisplay ();

	// DisplayElement methods
	virtual void Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC);
	virtual HTMLEventStatus Event (HTMLEvent *e);
	virtual DISPLAYElementType Type (void) { return DISPLAY_ELEMENT_STRINGDISPLAY; }

	// TextUnit methods
	virtual WEBC_UINT16 GetUnitType(void);
	virtual WEBC_UINT16 GetBreak(void);
	virtual DISPLAY_INT GetBaseline(void);
	virtual void SetParentDisplay(DisplayElement *parent, DisplayElement* insertBefore);
	virtual void SetDisplayContext(DisplayElement *parent);
	virtual WEBC_BOOL BreakAtOrBefore(DISPLAY_INT iPosition, TextUnit **firstHalf, TextUnit **secondHalf);
	virtual WEBC_BOOL BreakAtFirstPoint(TextUnit **firstHalf, TextUnit **secondHalf);
	virtual WEBC_BOOL BreakAtLastPoint(TextUnit **firstHalf, TextUnit **secondHalf);
	virtual WEBC_BOOL BreakAllowedBefore(void);
	virtual WEBC_BOOL BreakAllowedAfter(void);
	virtual WEBC_BOOL BreakAllowedInside(void);
	virtual void BreakAfter(void);
	virtual void BreakBefore(void);
	virtual DISPLAY_INT MinBreakWidth (void);
	virtual DisplayElement* FocusDefault (void); // who we default to if we don't accept focus

	// HTMLStringDisplay methods
	virtual void SetStringPosition(WEBC_SIZE iIndex, WEBC_SIZE iLength, WebGraphics *gc = 0);
	WEBC_SIZE GetStringIndex (void);
	WEBC_SIZE GetStringLength (void);
	HTMLStringDisplay *GetNextSubString (void);
	void  SetNextSubString(HTMLStringDisplay *pNext);
	void  SetStringElement (HTMLString *s);
	HTMLString *GetStringElement (void);

protected:
	virtual WEBC_BOOL SetElementStyle(void);
	WEBC_SIZE         FindMaxLength (WebChar *pStr);
	WEBC_SIZE         FindMinLength (WebChar *pStr, WEBC_BOOL bWrap);
	void              SetDimensions (WebGraphics *gc);
	WebFont           GetFont (void);
	WEBC_BOOL         _BreakAt (int index, TextUnit **firstHalf, TextUnit **secondHalf);
	WebChar * MakeFillJustifiedString(WebChar *str, long strLen, DISPLAY_INT remainingPad, WebGraphics *pGC, long &retStrLen);

	HTMLStringDisplay* mpNextSub;
	ITextDisplayStyle* mTextStyle;
	WEBC_SIZE miIndex;
	WEBC_SIZE miLength;
	WEBC_SIZE miFirstBreak;
	WEBC_SIZE miLastBreak;
};

class HTMLMinStringDisplay : public HTMLStringDisplay
{
public:
	HTMLMinStringDisplay (HTMLString *pStrElem) : HTMLStringDisplay(pStrElem) {}
	virtual void SetStringPosition(WEBC_SIZE iIndex, WEBC_SIZE iLength, WebGraphics *gc = 0);
	virtual WEBC_BOOL BreakAllowedBefore(void);
};

class HTMLMaxStringDisplay : public HTMLStringDisplay
{
public:
	HTMLMaxStringDisplay (HTMLString *pStrElem) : HTMLStringDisplay(pStrElem) {}
	virtual void  SetStringPosition(WEBC_SIZE iIndex, WEBC_SIZE iLength, WebGraphics *gc = 0);
};

#endif // __DHSTRING_HPP__
