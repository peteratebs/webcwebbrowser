#ifndef __DHFRAMESET_HPP__
#define __DHFRAMESET_HPP__

#include "dhelem.hpp"
#include "delement.hpp"

class HTMLFrameSet;

class HTMLFrameSetDisplay : public HTMLElementDisplay<DisplayElement>
{
public:
	HTMLFrameSetDisplay(HTMLFrameSet * f);
	virtual ~HTMLFrameSetDisplay();

	virtual void Draw(DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC);
	virtual HTMLEventStatus Event(HTMLEvent * event);
	virtual void GetBounds (WebRect *rect);
	virtual void Notify(Notifier *who, NotifyEvent *what);
	virtual DISPLAYElementType Type (void) { return DISPLAY_ELEMENT_FRAMESATETDISPLAY; }

	// methods overriden from TextUnit
	virtual DISPLAY_INT GetPixelWidth(void);
	virtual DISPLAY_INT GetPixelHeight(void);
	virtual void SetParentWidth(DISPLAY_INT iWidth, DISPLAY_INT iAvail);
	virtual void SetParentHeight(DISPLAY_INT iHeight);
	virtual void Format (void);
	virtual TUFormatStatus Formatted (void);

protected:
	TUFormatStatus  mFormatted;

	void SetFormatStatus(TUFormatStatus status);
};

#endif // __DHFRAMESET_HPP__
