#ifndef __DHELEM_HPP__
#define __DHELEM_HPP__

#include "dhtumirror.hpp"

typedef WEBC_UINT8 HELEM_DISPLAY_FLAGS;

#define HELEM_DISPLAY_FLAG_STYLE_SET     0x01  // set if style is up to date

template<class T> class HTMLElementDisplay : public HTMLTextUnitMirror, public T
{
protected:
	HELEM_DISPLAY_FLAGS mHtmlElementDisplayFlags;

	virtual WEBC_BOOL SetElementStyle(void);

	virtual IPositionedFormatContext* FormatSelfPositioned (
			DisplayElement* displayParent,
			IPositionedFormatContext* rootContext,
			IPositionedFormatContext* parentContext,
			FormatContextFactory* childContextFactory,
			WEBC_BOOL generateChildContext
		);

	// returns the formatted content height
	virtual DISPLAY_INT FormatContentPositioned (
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
		) { SetElementStyle(); return GetPixelHeight() - (borderTopValue + borderBottomValue + paddingTopValue + paddingBottomValue); }
  
	virtual void FormatPositionedChildren (
			IPositionedFormatContext* childContext,
			IPositionedFormatContext* rootContext,
			FormatContextFactory* childContextFactory		
		);

public:
	HTMLElementDisplay (HTMLElement *pElem);
	virtual ~HTMLElementDisplay ();
	
	// DisplayElement methods
	virtual void Move (WebRect * pRect);
	virtual void SizeTo (DISPLAY_INT w, DISPLAY_INT h);
	virtual void SetHeight (DISPLAY_INT h);
	virtual void SetWidth (DISPLAY_INT w);
	virtual void DrawThisOnly (DISPLAY_INT x, DISPLAY_INT y, WebGraphics *gc);
	virtual HTMLEventStatus Event (HTMLEvent *e);
	virtual void Notify(Notifier *who, NotifyEvent *what);
	virtual WEBC_INT32 GetZIndex (void);
	virtual WEBC_BOOL TrapEventInThis (DISPLAY_INT x, DISPLAY_INT y);

	// TextUnit methods
	virtual DISPLAY_INT GetMinWidth(void);
	virtual DISPLAY_INT GetMaxWidth(void);
	virtual DISPLAY_INT GetPixelWidth(void);
	virtual DISPLAY_INT GetPixelHeight(void);
	virtual DISPLAY_INT GetBaseline(void);
	virtual void SetXYPosition(DISPLAY_INT iXPos, DISPLAY_INT iYPos);
	virtual void SetParentDisplay(DisplayElement *parent, DisplayElement* insertBefore);
	virtual void SetDisplayContext(DisplayElement *parent);

	virtual WEBC_BOOL NeedsAutoIndentTop (WEBC_BOOL parentHeightIsAuto);
  
	virtual IPositionedFormatContext* FormatPositioned (
			DisplayElement* displayParent,
			IPositionedFormatContext* rootContext,
			IPositionedFormatContext* parentContext,
			FormatContextFactory* childContextFactory
		);

	// HTMLElementDisplay methods
	virtual void SetHtmlElement(HTMLElement *pElement);
};

#endif // __DHELEM_HPP__
