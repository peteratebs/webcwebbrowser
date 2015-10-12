#ifndef __KEYCATCH_HPP__
#define __KEYCATCH_HPP__

#include "delement.hpp"

class HTMLElement;

class DisplayKeyCatcher : public DisplayElement
{
public:
	DisplayKeyCatcher(void);
	~DisplayKeyCatcher(void);

	// Methods from DisplayElement
	virtual void Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC);
	virtual HTMLEventStatus Event (HTMLEvent *pEvent);
	virtual DisplayElement *TrapEventPoint (DISPLAY_INT x, DISPLAY_INT y, DISPLAY_INT viewX, DISPLAY_INT viewY);

	void SetElement(HTMLElement *pElem);
	HTMLElement *GetElement(void);
	virtual DISPLAYElementType Type (void) { return DISPLAY_ELEMENT_KEYCATCHER; }

protected:
	HTMLElement *mpHtmlElement;
};


#endif // __KEYCATCH_HPP__
