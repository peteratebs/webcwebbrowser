#ifndef __WTRADIO_HPP__
#define __WTRADIO_HPP__

#include "delement.hpp"
#include "listener.hpp"

class WebRadioButton : public DisplayElement
{
protected:
	WEBC_BOOL mbSelected;
	Listener *mpListener;

public:
	WebRadioButton ();
	virtual ~WebRadioButton ();

	virtual HTMLEventStatus Event (HTMLEvent *e);
	virtual void DrawThisOnly (DISPLAY_INT x, DISPLAY_INT y, WebGraphics *gc);
	virtual DISPLAYElementType Type (void) { return DISPLAY_ELEMENT_WEBRADIOBUTTON; }

	virtual void SetSelected (WEBC_BOOL status);
	WEBC_BOOL GetSelected (void);
	void SetListener (Listener *listener);
};

#endif // __WTRADIO_HPP__
