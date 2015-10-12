#ifndef __WTLIST_HPP__
#define __WTLIST_HPP__

#include "webctypes.h"
#include "delement.hpp"
#include "listener.hpp"
#include "wtscroll.hpp"

#define LISTBOX_BORDER         WG_CFG_DEFAULT_FRAME_WIDTH
#define LISTBOX_PADDING        2
#define LISTBOX_MARGIN         2
#define LISTBOX_SPACING        3

#define LISTBOX_STYLE_NOT3D          0x01
#define LISTBOX_STYLE_NOFOCUSFRAME   0x02
#define LISTBOX_STYLE_SIZE_INVALID   0x04
#define LISTBOX_STYLE_FOLLOW_MOUSE   0x08
#define LISTBOX_STYLE_LIMITED_NOTIFY 0x10

class WebListBox : public DisplayElement
{
protected:
	long         miMouseOver;
	long         miNumOptions;
	WebChar   ** mppOption;
	long         miSelected;
	DISPLAY_INT  miTextHeight;
	DISPLAY_INT  miTotalTextHeight, miTotalTextWidth;
	DISPLAY_INT  miSliderWidth;
	long         miLongestOption;
	WebFontRef   mFont;
	WebVScroll * mpVScroll;
	WebHScroll * mpHScroll;
	WEBC_UINT8   mStyle;
	Listener   * mpListener;
	DISPLAY_INT  miBorder,miPadding,miSpacing,miMargin;

	WEBC_BOOL UseVScroll(void);
	WEBC_BOOL UseHScroll(void);
	long OptionAt(DISPLAY_INT x, DISPLAY_INT y);
	void GetScrollOffset(DISPLAY_INT * x, DISPLAY_INT * y);
	void FitSelection(void);
	void ResizeScrollBars(void);

	virtual void GetContentRect(WebRect * rect);
	virtual void GetFrameRect(WebRect * r);
	virtual void GetOptionsRect(WebRect * rect);
	virtual void DrawFrame(WebRect *rect, WebGraphics *gc);

public:
	WebListBox ();
	virtual ~WebListBox ();

	virtual HTMLEventStatus Event (HTMLEvent *e);
	virtual void DrawThisOnly (DISPLAY_INT x, DISPLAY_INT y, WebGraphics *gc);
	virtual void Move(WebRect * rect);
	virtual void Disable (void);
	virtual void Enable (void);
	virtual DISPLAYElementType Type (void) { return DISPLAY_ELEMENT_WEBLISTBOX; }

	void SetNumOptions (long i);
	void SetOptionText (long n, const WebChar *text);
	WebChar *GetOptionText (long n);
	void SetFont (WebGraphFont *fnt);
	void SetSliderWidth(DISPLAY_INT NewWidth);
    DISPLAY_INT  GetSliderWidth(void);
	WEBC_BOOL SetSelected (long i);
	long NextIndexStartingWith (WebChar ch);
	long GetSelected (void);
	void AddListener(Listener * l);
	void SetStyle(WEBC_UINT8 style);
	void CalculateTextMetrics(WebGraphics * gc);

	void Notify(Notifier * who, NotifyEvent *what);
	void NotifySelectChange(void);

	void SetupScrollBars(WebGraphics * gc);
	int GetTotalTextHeight(void);
	int GetTotalTextWidth(void);

	void SetBorder(DISPLAY_INT l);
	void SetPadding(DISPLAY_INT l);
	void SetSpacing(DISPLAY_INT l);
	void SetMargin(DISPLAY_INT l);

	DISPLAY_INT GetBorder(void);
	DISPLAY_INT GetPadding(void);
	DISPLAY_INT GetMargin(void);
	DISPLAY_INT GetSpacing(void);

	virtual WebColor GetTextColor   (WebGraphics* gc);
	virtual WebColor GetBgColor     (WebGraphics* gc);
	virtual WebColor GetSelectColor (WebGraphics* gc);
};


#endif // __WTLIST_HPP__
