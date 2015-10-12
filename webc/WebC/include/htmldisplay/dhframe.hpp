#ifndef __DHFRAME_HPP__
#define __DHFRAME_HPP__

#include "dhelem.hpp"
#include "dmanager.hpp"
#include "hcbox.hpp"

class HTMLFrame;

class HTMLFrameDisplay : public HTMLElementDisplay<DisplayManager>
{
public:
	HTMLFrameDisplay (HTMLFrame *pElem);
	virtual ~HTMLFrameDisplay ();

	virtual void Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *view, WebGraphics *gc);
	virtual DisplayElement *TrapEventPoint (DISPLAY_INT x, DISPLAY_INT y, DISPLAY_INT viewX, DISPLAY_INT viewY);
	virtual HTMLEventStatus Event (HTMLEvent *pEvent);
	virtual DisplayElement *FocusDefault (void);
	
	// TextUnit methods
	virtual void SetParentWidth(DISPLAY_INT iWidth, DISPLAY_INT iAvail);
	virtual void SetParentHeight(DISPLAY_INT iHeight);
	
protected:
	HTMLContentBox mBorder;

	virtual WEBC_BOOL SetElementStyle(void);
	virtual void GetPaddingWidth (WebRect* padding);
};

#endif // __DHFRAME_HPP__
