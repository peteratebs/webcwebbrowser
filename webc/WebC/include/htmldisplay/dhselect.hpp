#ifndef __DHSELECT_HPP__
#define __DHSELECT_HPP__

#include "dhelem.hpp"
#include "wtlist.hpp"
#include "wtdrop.hpp"

class HTMLSelect;
class HTMLContentBox;

class HTMLListBoxDisplay : public HTMLElementDisplay<WebListBox>
{
public:
	HTMLListBoxDisplay (HTMLSelect *pElem);
	virtual ~HTMLListBoxDisplay ();

	virtual void Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC);
	virtual DisplayElement *TrapEventPoint (DISPLAY_INT x, DISPLAY_INT y, DISPLAY_INT viewX, DISPLAY_INT viewY);

	// TextUnit methods
	virtual DISPLAY_INT GetMinWidth(void);
	virtual DISPLAY_INT GetMaxWidth(void);
	virtual DISPLAY_INT GetPixelWidth(void);
	virtual DISPLAY_INT GetPixelHeight(void);
	virtual DISPLAY_INT GetParentWidth(void);
	virtual void SetParentWidth(DISPLAY_INT iParentWidth, DISPLAY_INT iAvail);
	virtual void SetParentHeight(DISPLAY_INT iParentHeight);
	virtual void SetParentDisplay(DisplayElement *parent, DisplayElement* insertBefore);
	virtual void SetDisplayContext(DisplayElement *parent);

    // New 2-27-07
	virtual WEBC_BOOL GetIntrinsicWidth (
			DISPLAY_INT* intrinsicWidth,
			DISPLAY_INT  height,
			WEBC_BOOL    heightIsAuto
		);

	virtual WebColor GetTextColor   (WebGraphics* gc);
	virtual WebColor GetBgColor     (WebGraphics* gc);

protected:
	virtual void GetContentRect(WebRect * rect);
	virtual void DrawFrame(WebRect *rect, WebGraphics *gc);

	void SetHtmlOptions (void);
	WEBC_BOOL SetElementStyle (void);
	void SetDimensions (WebGraphics *gc);
	void FindContentRect(WebRect *textRect, WebRect *elemRect, HTMLContentBox *b);

	DISPLAY_INT miParentWidth;
	DISPLAY_INT miParentHeight;
	PresetWebRect mContentRect;
	WebColor mTextColor;
	WebColor mBackgroundColor;
};


class HTMLDropBoxDisplay : public HTMLElementDisplay<WebDropBox>
{
public:
	HTMLDropBoxDisplay (HTMLSelect *pElem);
	virtual ~HTMLDropBoxDisplay ();

	virtual void Notify(Notifier *who, NotifyEvent *what);
	virtual void Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC);
	virtual DisplayElement *TrapEventPoint (DISPLAY_INT x, DISPLAY_INT y, DISPLAY_INT viewX, DISPLAY_INT viewY);

	// TextUnit methods
	virtual DISPLAY_INT GetBaseline(void);
	virtual DISPLAY_INT GetMinWidth(void);
	virtual DISPLAY_INT GetMaxWidth(void);
	virtual DISPLAY_INT GetPixelWidth(void);
	virtual DISPLAY_INT GetPixelHeight(void);
	virtual DISPLAY_INT GetParentWidth(void);
	virtual void SetParentWidth(DISPLAY_INT iParentWidth, DISPLAY_INT iAvail);
	virtual void SetParentHeight(DISPLAY_INT iParentHeight);
	virtual void SetParentDisplay(DisplayElement *parent, DisplayElement* insertBefore);
	virtual void SetDisplayContext(DisplayElement *parent);

    // New 2-27-07
	virtual WEBC_BOOL GetIntrinsicWidth (
			DISPLAY_INT* intrinsicWidth,
			DISPLAY_INT  height,
			WEBC_BOOL    heightIsAuto
		);
	virtual WebColor GetTextColor   (WebGraphics* gc);
	virtual WebColor GetBgColor     (WebGraphics* gc);

protected:
	virtual void GetContentRect(WebRect * rect);
	virtual void DrawFrame(WebRect *rect, WebGraphics *gc);

	void SetHtmlOptions (void);
	WEBC_BOOL SetElementStyle (void);
	void SetDimensions (WebGraphics *gc);
	void FindContentRect(WebRect *textRect, WebRect *elemRect, HTMLContentBox *b);

	DISPLAY_INT miParentWidth;
	DISPLAY_INT miParentHeight;
	PresetWebRect mContentRect;
	WebColor mTextColor;
	WebColor mBackgroundColor;
};

#endif // __DHSELECT_HPP__
