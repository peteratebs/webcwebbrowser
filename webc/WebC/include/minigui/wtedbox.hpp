#ifndef __WTEDBOX_HPP__
#define __WTEDBOX_HPP__

#include "wtedstr.hpp"
#include "wtscroll.hpp"

class WebEditBox : public WebEditString
{
public:
	WebEditBox ();
	virtual ~WebEditBox ();

	virtual HTMLEventStatus Event (HTMLEvent *e);
	virtual void DrawThisOnly (DISPLAY_INT x, DISPLAY_INT y, WebGraphics *gc);
	virtual WEBC_BOOL GetFocusRect (WebRect* rect);
	virtual void Move (WebRect *pRect); 
	virtual void SizeTo (DISPLAY_INT w, DISPLAY_INT h);
	virtual void SetHeight (DISPLAY_INT h);
	virtual void SetWidth (DISPLAY_INT w);
	virtual void Disable (void);
	virtual void Enable (void);
	virtual void SetText (const WebChar *label);
	virtual void SetFont (WebGraphFont *font);
	
	virtual void SetCursorPosition (long index);
	virtual void InsertChar (WebChar c);
	virtual void LineUp (void);
	virtual void LineDown (void);
	virtual void LineHome (void);
	virtual void LineEnd (void);
	virtual void PageUp (void);
	virtual void PageDown (void);
	virtual void ScrollUp (void);
	virtual void ScrollDown (void);
	virtual void ScrollLeft (void);
	virtual void ScrollRight (void);
	virtual void ScrollToX (DISPLAY_INT x);
	virtual void ScrollToY (DISPLAY_INT y);	
	
	virtual void Notify (Notifier *pWho, NotifyEvent *what);

	WebChar     *GetLineText   (long line);
	SCROLL_MODE  GetScrollMode (void);
	void         SetScrollMode (SCROLL_MODE m);
	
protected:
	virtual void DeleteChars(long begin, long end);
	virtual long GetIndexAtXY(DISPLAY_INT x, DISPLAY_INT y);
	virtual void EnsureCursorVisible (void);
	virtual void GetFrameRect(WebRect *rect);
	virtual void GetTextRect(WebRect *rect);
	virtual void GetCornerRect(WebRect *rect);
	virtual void DrawFrame(WebRect *box, WebGraphics *gc);

	void Format (void);
	WEBC_BOOL UpdateScrollBars(void);
	int  AddLine(long iBegin); // helper for format
	void ReallocLines (long iNewSize);	
	long FindLine (long index);
	long GetLineOffset(long iLine);
	void SetLineOffset(long iLine, long iOffset);

	long  miOffsetArraySize;
	long  miNumLines;
	long  miCurrentLine;
	long *mpLineOffsets;
	DISPLAY_INT  miYOffset;
	long  miMaxLineWidth;

	WebVScroll *mpVScroll;
	WebHScroll *mpHScroll;
	
	SCROLL_MODE mScrollMode;
};


#endif // __WTEDBOX_HPP__
