#ifndef __DHAREA_HPP__
#define __DHAREA_HPP__

#include "dhelem.hpp"
#include "delement.hpp"

class HTMLArea;

class HTMLAreaDisplay : public HTMLElementDisplay<DisplayElement>
{
public:
	HTMLAreaDisplay (HTMLArea *pElem, HTMLElement *pAreaParent);
	virtual ~HTMLAreaDisplay ();

	// DisplayElement methods
	virtual void Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC);
	virtual HTMLEventStatus Event (HTMLEvent *e);
	virtual DISPLAYElementType Type (void) { return DISPLAY_ELEMENT_HTMLAREADISPLAY; }

	HTMLArea    *GetAreaElement (void);
	HTMLElement *GetAreaParent (void);

	HTMLAreaDisplay *mpAreaNext;

private:
	void SetAreaDimensions (void);

	HTMLElement *mpEventParent;
};

#endif // __DHAREA_HPP__
