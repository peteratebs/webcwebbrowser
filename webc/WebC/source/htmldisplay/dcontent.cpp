//
// DCONTENT.CPP - Methods for class FlowDisplayContentHolder
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//

/*****************************************************************************/
// Header Files
/*****************************************************************************/

#include "dcontent.hpp"


/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/


/*****************************************************************************/
// Local Types
/*****************************************************************************/


/*****************************************************************************/
// Global Data
/*****************************************************************************/


/*****************************************************************************/
// FlowDisplayContentHolder methods
/*****************************************************************************/

FlowDisplayContentHolder::FlowDisplayContentHolder ()
	: DisplayElement()
{
}

HTMLEventStatus FlowDisplayContentHolder::Event(HTMLEvent *e)
{
    WEBC_DEBUG_ReceiveEvent(this, e);
	return (mpParent->Event(e));
}

void FlowDisplayContentHolder::Notify(Notifier *who, NotifyEvent *what)
{
    WEBC_DEBUG_ReceiveNotify(this, who, what);
	if (what && (what->type == NOTIFIER_TYPE_TEXT_UNIT))
	{
		mpParent->Notify(who, what);
	}
	else
	{
		DisplayElement::Notify(who, what);
	}
}

WEBC_BOOL FlowDisplayContentHolder::TrapEventInThis (DISPLAY_INT x, DISPLAY_INT y)
{
	return (WEBC_FALSE);
}
