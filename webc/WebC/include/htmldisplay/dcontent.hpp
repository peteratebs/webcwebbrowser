#ifndef __DCONTENT_HPP__
#define __DCONTENT_HPP__

#include "delement.hpp"

// This class is designed to be the child of an HTMLFlowDisplay element
//  and pass notifications/events up to its parent
class FlowDisplayContentHolder : public DisplayElement
{
public:
	FlowDisplayContentHolder ();

	// methods overridden from DisplayElement
	virtual DISPLAYElementType Type (void) { return DISPLAY_ELEMENT_CONTENTHOLDER; }
	virtual HTMLEventStatus Event (HTMLEvent *e);
	virtual void Notify(Notifier *who, NotifyEvent *what);
	virtual WEBC_BOOL TrapEventInThis (DISPLAY_INT x, DISPLAY_INT y);
};

#endif // __DCONTENT_HPP__
