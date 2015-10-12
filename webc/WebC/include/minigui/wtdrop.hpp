#ifndef __WTDROP_HPP__
#define __WTDROP_HPP__

#include "delement.hpp"
#include "wtbutton.hpp"
#include "listener.hpp"
#include "wtlist.hpp"

#define DROPBOX_MARGIN  2
#define DROPBOX_PADDING 1
#define DROPBOX_FRAME_WIDTH WG_CFG_DEFAULT_FRAME_WIDTH

enum WebDropBoxState
{
	DROPBOX_STATE_NORMAL,
	DROPBOX_STATE_DROPPED
};

class WebDropBox;



class WebDropList : public WebListBox
{
public:
	virtual WebColor GetTextColor   (WebGraphics* gc);
	virtual WebColor GetBgColor     (WebGraphics* gc);
	virtual WebColor GetSelectColor (WebGraphics* gc);

	virtual DisplayElement *FocusDefault (void) { return (DisplayElement *) mpListener; }
};

class WebDropBox : public DisplayElement
{
protected:
	WebDropBoxState   mState;
	Web3DBitmapButton mButton;
	WebDropList       mListbox;
	WebChar*          mpText;
	WebFontRef        mFont;
	WEBC_BOOL         mSizeInvalid;
	int               miSelected;
	int               miLastSelected;
	Listener*         mpListener;

	virtual void GetContentRect(WebRect * rect);
	virtual void GetFrameRect(WebRect * r);
	virtual void DrawFrame(WebRect *rect, WebGraphics *gc);

	void NotifySelectChange(void);

public:
	WebDropBox ();
	virtual ~WebDropBox ();

	virtual HTMLEventStatus Event (HTMLEvent *e);
	virtual void DrawThisOnly (DISPLAY_INT x, DISPLAY_INT y, WebGraphics *gc);
	virtual void Disable (void);
	virtual void Enable (void);
	virtual void Move (WebRect * rect);
	virtual void SizeTo (DISPLAY_INT w, DISPLAY_INT h);
	virtual void SetHeight (DISPLAY_INT h);
	virtual void SetWidth (DISPLAY_INT w);
	virtual void SetManager (DisplayManager* manager);
	virtual DISPLAYElementType Type (void) { return DISPLAY_ELEMENT_WEBDROPBOX; }


	void SetSelected(long l);
	long GetSelected(void);
	void SetFont(WebGraphFont *f);
	void SetHighlighted(WEBC_BOOL high);
	void CalculateAvailableSpace(int * up, int * down);

	void Drop(void);
	void Undrop(void);

	void SetNumOptions(long i);
	void SetOptionText(long n, const WebChar *text);
	WebChar *GetOptionText(long n);
	void Notify(Notifier * n, NotifyEvent *what);
	void AddListener(Listener * l);

	virtual WebColor GetTextColor   (WebGraphics* gc);
	virtual WebColor GetBgColor     (WebGraphics* gc);
	virtual WebColor GetSelectColor (WebGraphics* gc);
};


#endif // __WTDROP_HPP__
