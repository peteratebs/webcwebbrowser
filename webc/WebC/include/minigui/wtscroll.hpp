#ifndef __WTSCROLL_HPP__
#define __WTSCROLL_HPP__

#include "wtslider.hpp"
#include "wtbutton.hpp"
#include "listener.hpp"

class WebScroll : public DisplayElement
{
protected:
	Web3DBitmapButton mUpButton, mDownButton;
	WebSlider * mpSlider;
	Listener * mpListener;

	WEBC_UINT32 GetScrollbarFlags ();
#if (INCLUDE_EXPERIMENTAL_DIV_SCROLLBARS)
	WEBC_BOOL		mIncludeInFlow;
#endif
public:
	WebScroll();
	virtual ~WebScroll();

	void SetPosition(long i);
	void SetRange(long r);
	void SetWindow(long w);
	void SetStep(long s);
	void Notify (Notifier * who, NotifyEvent *what);
	virtual HTMLEventStatus Event(HTMLEvent * e);

	virtual void Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC);
	virtual DISPLAYElementType Type (void) { return DISPLAY_ELEMENT_WEBSCROLL; }

	long GetPosition(void);

	long GetRange(void);
	long GetWindow(void);
	long GetStep(void);

	void Up (void);
	void Down (void);
	void PageUp(void);
	void PageDown(void);
	virtual void Move(WebRect *p);
	virtual void Disable (void);
	virtual void Enable (void);
	virtual void UpdateRect(void) = 0;
	void SetListener(Listener * l);

#if (INCLUDE_EXPERIMENTAL_DIV_SCROLLBARS)
	void				SetIncludeInFlow	(WEBC_BOOL IncludeInFlow) {mIncludeInFlow=IncludeInFlow;}
	WEBC_BOOL				IncludeInFlow	(void) {return mIncludeInFlow;}
#endif
};


class WebVScroll : public WebScroll
{
public:
	WebVScroll();
	~WebVScroll();

	virtual void SetManager (DisplayManager* manager);
	virtual void Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC);

	void UpdateRect(void);
};


class WebHScroll : public WebScroll
{
public:
	WebHScroll();
	~WebHScroll();

	virtual void SetManager (DisplayManager* manager);
	virtual void Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC);

	void UpdateRect(void);
};

#endif // __WTSCROLL_HPP__
