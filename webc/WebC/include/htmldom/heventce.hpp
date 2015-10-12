#ifndef __HEVENTCE_HPP__
#define __HEVENTCE_HPP__

#include "helement.hpp"

/*************************************************************************
* class HTMLEventCouplerElement
* 
*************************************************************************/

class HTMLEventCouplerElement : public HTMLElement
{
private:
	HTMLElement *mpHandler;
	HTMLElement *mpEventParent;

public:
	HTMLEventCouplerElement (HTMLElement *pElement, HTMLElement *pParent)
		: HTMLElement (0, pParent->GetDocument())
	{
		mpHandler = pElement; 
		mpEventParent = pParent;
	}
	
	virtual HTMLEventStatus UserEvent(HTMLEvent *e)    { return mpHandler->UserEvent(e); }
	virtual HTMLEventStatus JSEvent(HTMLEvent *e)      { return mpHandler->JSEvent(e); }
	virtual HTMLEventStatus DefaultEvent(HTMLEvent *e) { return mpHandler->DefaultEvent(e); }
	virtual HTMLElement *EventParent(void)             { return mpEventParent; }
};

#endif // __HEVENTCE_HPP__
