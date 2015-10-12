//
// DELEMENT.CPP - Methods for class DisplayElement
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

#include "delement.hpp"
#include "dmanager.hpp"
#include "ref.hpp"
#include "webcassert.h"
#include "objref.cpp"

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
// Local functions
/*****************************************************************************/

DisplayElement::DisplayElement ()
{
	mpFirstChild = 0;
	mpLastChild = 0;
	mpNext = 0;
	mpPrev = 0;
	mpParent = 0;
	mFlags = 0;
}

DisplayElement::~DisplayElement()
{
	// Remove myself
	if (mpParent)
	{
		mpParent->Remove(this);
	}

	// Remove my children
	while (mpFirstChild)
	{
		Remove(mpFirstChild);
	}

	// if we are the root, then we need to let go of our DisplayManager
	if (GetManager())
	{
		SetManager(0);
	}
}

HTMLEventStatus DisplayElement::Event(HTMLEvent * e)
{
    WEBC_DEBUG_ReceiveEvent(this, e);
	return HTML_EVENT_STATUS_DONE;
}

void DisplayElement::Draw (DISPLAY_INT iScreenX, DISPLAY_INT iScreenY, WebRect *pViewport, WebGraphics *pGC)
{
	DisplayElement *pChild = mpFirstChild;
	WebRect clip, saveClip, screenClip;
	WEBC_BOOL clipRestore = WEBC_FALSE;

	pGC->GetClip(&saveClip);
	clip.Set(&saveClip);

	if (GetOverflow() == DISPLAY_OVERFLOW_HIDDEN)
	{
		GetClipRect(&screenClip);
		screenClip.Shift(iScreenX - mRect.left, iScreenY - mRect.top);
		if (!clip.Overlaps(&screenClip))
		{
			// we're not visible, so neither are our children
			return;
		}
		clip.And(&screenClip);
		pGC->SetClip(&clip);
		clipRestore = WEBC_TRUE;
	}

	WebRect screenRect(mRect);
	screenRect.MoveTo(iScreenX, iScreenY);

	// Start drawing
//	pGC->StartBuffer();

	// Draw negative-z children

	while (pChild && (pChild->GetZIndex() < 0))
	{
		if (pChild->mFlags & DISPLAY_FLAG_FIXED_POS)
		{
			pChild->Draw(pViewport->left + pChild->mRect.left, pViewport->top + pChild->mRect.top, pViewport, pGC);
		}
		else
		{
			pChild->Draw(iScreenX + pChild->mRect.left, iScreenY + pChild->mRect.top, pViewport, pGC);
		}
		pChild = pChild->mpNext;
	}

	// if I am visible
	if (screenRect.Overlaps(&clip))
	{
		// Draw myself
		DrawThisOnly(iScreenX, iScreenY, pGC);
	}

	// Draw positive-z children
	while (pChild)
	{
		if (pChild->mFlags & DISPLAY_FLAG_FIXED_POS)
		{
			pChild->Draw(pViewport->left + pChild->mRect.left, pViewport->top + pChild->mRect.top, pViewport, pGC);
		}
		else
		{
			pChild->Draw(iScreenX + pChild->mRect.left, iScreenY + pChild->mRect.top, pViewport, pGC);
		}
		pChild = pChild->mpNext;
	}

	if (clipRestore)
	{
		pGC->SetClip(&saveClip);
	}
//	pGC->EndBuffer();
}


void DisplayElement::Move(WebRect * pRect)
{
	mRect.Set(pRect);
	if (mFlags & DISPLAY_FLAG_BOUNDS_VALID)
	{
		InvalidateBounds();
	}
}

void DisplayElement::MoveTo(DISPLAY_INT x, DISPLAY_INT y)
{
	mRect.MoveTo(x, y);
	if (mFlags & DISPLAY_FLAG_BOUNDS_VALID)
	{
		InvalidateBounds();
	}
}

void DisplayElement::SizeTo(DISPLAY_INT w, DISPLAY_INT h)
{
	mRect.SizeTo(w,h);
	if (mFlags & DISPLAY_FLAG_BOUNDS_VALID)
	{
		InvalidateBounds();
	}
}


DisplayOverflow DisplayElement::GetOverflow(void)
{
	if (mFlags & DISPLAY_FLAG_FIXED_POS_CHILD)
	{
		return DISPLAY_OVERFLOW_VISIBLE;
	}
	return DISPLAY_OVERFLOW_HIDDEN;
}

void DisplayElement::GetClipRect(WebRect * pClipRect)
{
	GetBounds(pClipRect);
}

short DisplayElement::TabIndex(void)
{
	return -1;
}

void DisplayElement::InsertAfter(DisplayElement *pPlace, DisplayElement *pChild)
{
	WEBC_ASSERT(pPlace != pChild);
	WEBC_ASSERT(pPlace->mpParent == this);

	if (pChild->mpParent)
	{
		pChild->mpParent->Unlink(pChild);
		if (pChild->mpParent != this)
		{
			if (pChild->mpParent->mFlags & DISPLAY_FLAG_BOUNDS_VALID)
			{
				pChild->mpParent->InvalidateBounds();
			}
		}
	}

	if (pChild->mpParent != this)
	{
		if (mFlags & DISPLAY_FLAG_BOUNDS_VALID)
		{
			InvalidateBounds();
		}
		pChild->mpParent = this;
		pChild->SetManager(GetManager());
	}

	pChild->mpPrev = pPlace;
	pChild->mpNext = pPlace->mpNext;
	if (pPlace->mpNext)
	{
		pPlace->mpNext->mpPrev = pChild;
	}
	else
	{
		mpLastChild = pChild;
	}
	pPlace->mpNext = pChild;
}

void DisplayElement::InsertBefore (DisplayElement *pPlace, DisplayElement *pChild)
{
	WEBC_ASSERT(pPlace != pChild);
	WEBC_ASSERT(pPlace->mpParent == this);

	if (pChild->mpParent)
	{
		pChild->mpParent->Unlink(pChild);
		if (pChild->mpParent != this)
		{
			if (pChild->mpParent->mFlags & DISPLAY_FLAG_BOUNDS_VALID)
			{
				pChild->mpParent->InvalidateBounds();
			}
		}
	}

	if (pChild->mpParent != this)
	{
		if (mFlags & DISPLAY_FLAG_BOUNDS_VALID)
		{
			InvalidateBounds();
		}
		pChild->mpParent = this;
		pChild->SetManager(GetManager());
	}

	pChild->mpNext = pPlace;
	pChild->mpPrev = pPlace->mpPrev;
	if (pPlace->mpPrev)
	{
		pPlace->mpPrev->mpNext = pChild;
	}
	else
	{
		mpFirstChild = pChild;
	}
	pPlace->mpPrev = pChild;
}

void DisplayElement::InsertFirst(DisplayElement *pChild)
{
	if (pChild->mpParent)
	{
		pChild->mpParent->Unlink(pChild);
		if (pChild->mpParent != this)
		{
			if (pChild->mpParent->mFlags & DISPLAY_FLAG_BOUNDS_VALID)
			{
				pChild->mpParent->InvalidateBounds();
			}
		}
	}

	if (pChild->mpParent != this)
	{
		if (mFlags & DISPLAY_FLAG_BOUNDS_VALID)
		{
			InvalidateBounds();
		}
		pChild->mpParent = this;
		pChild->SetManager(GetManager());
	}

	pChild->mpNext = mpFirstChild;
	pChild->mpPrev = 0;
	if (mpFirstChild)
	{
		mpFirstChild->mpPrev = pChild;
	}
	else
	{
		mpLastChild = pChild;
	}
	mpFirstChild = pChild;
}

void DisplayElement::InsertLast(DisplayElement *pChild)
{
	if (pChild->mpParent)
	{
		pChild->mpParent->Unlink(pChild);
		if (pChild->mpParent != this)
		{
			if (pChild->mpParent->mFlags & DISPLAY_FLAG_BOUNDS_VALID)
			{
				pChild->mpParent->InvalidateBounds();
			}
		}
	}

	if (pChild->mpParent != this)
	{
		if (mFlags & DISPLAY_FLAG_BOUNDS_VALID)
		{
			InvalidateBounds();
		}
		pChild->mpParent = this;
		pChild->SetManager(GetManager());
	}

	pChild->mpPrev = mpLastChild;
	pChild->mpNext = 0;
	if (mpLastChild)
	{
		mpLastChild->mpNext = pChild;
	}
	else
	{
		mpFirstChild = pChild;
	}
	mpLastChild = pChild;
}

void DisplayElement::InsertOrdered (DisplayElement *pChild)
{
DisplayElement *place = mpLastChild;
long zIndex = pChild->GetZIndex();

	while (place)
	{
		if (place->GetZIndex() <= zIndex)
		{
			// check to make sure we aren't trying to insert child after itself
			if (place != pChild)
			{
				InsertAfter(place, pChild);
			}
			return;
		}
		place = place->mpPrev;
	}

	InsertFirst(pChild);
}

void DisplayElement::Remove(DisplayElement *pChild)
{
	Unlink(pChild);
	pChild->mpParent = 0;
	pChild->SetManager(0);
	if (mFlags & DISPLAY_FLAG_BOUNDS_VALID)
	{
		InvalidateBounds();
	}
}

void DisplayElement::Unlink (DisplayElement *pChild)
{
	if (pChild->mpPrev == WEBC_NULL)
	{
		mpFirstChild = pChild->mpNext;
	}
	else
	{
		pChild->mpPrev->mpNext = pChild->mpNext;
	}

	if (pChild->mpNext == WEBC_NULL)
	{
		mpLastChild = pChild->mpPrev;
	}
	else
	{
		pChild->mpNext->mpPrev = pChild->mpPrev;
	}
}

DISPLAY_INT DisplayElement::Width (void)
{
	return (mRect.Width());
}

DISPLAY_INT DisplayElement::Height (void)
{
	return (mRect.Height());
}

void DisplayElement::SetWidth (DISPLAY_INT w)
{
	mRect.SetWidth(w);
	if (mFlags & DISPLAY_FLAG_BOUNDS_VALID)
	{
		InvalidateBounds();
	}
}

void DisplayElement::SetHeight (DISPLAY_INT h)
{
	mRect.SetHeight(h);
	if (mFlags & DISPLAY_FLAG_BOUNDS_VALID)
	{
		InvalidateBounds();
	}
}


WEBC_INT32 DisplayElement::GetZIndex (void)
{
	return (0);
}

void DisplayElement::DrawThisOnly (DISPLAY_INT iScreenX, DISPLAY_INT iScreenY, WebGraphics *pGC)
{
}

void DisplayElement::GetDisplayPosition (DisplayElement *pChild, DISPLAY_INT *piX, DISPLAY_INT *piY)
{
	if (pChild->mFlags & DISPLAY_FLAG_FIXED_POS)
	{
		if (GetManager())
		{
			*piX = GetManager()->mViewRect.left + pChild->mRect.left;
			*piY = GetManager()->mViewRect.top + pChild->mRect.top;
		}
	}
	else
	{
		DISPLAY_INT x = 0, y = 0;

		if (mpParent)
		{
			mpParent->GetDisplayPosition(this, &x, &y);
		}

		*piX = pChild->mRect.left + x;
		*piY = pChild->mRect.top + y;
	}
}


// x,y are in same coordinate space as this->mRect; viewX, viewY is the upper left corner of the viewport in
//  the coordinate space of this->mRect
DisplayElement *DisplayElement::TrapEventPoint (DISPLAY_INT x, DISPLAY_INT y, DISPLAY_INT viewX, DISPLAY_INT viewY)
{
	// if we clip our children to our rect, and the point is outside our rect,
	//  then fail right away
	if (GetOverflow() == DISPLAY_OVERFLOW_HIDDEN)
	{
		WebRect clipRect;
		GetClipRect(&clipRect);
		if (!clipRect.Contains(x,y))
		{
			return (0);
		}
	}

	DisplayElement *pChild = mpLastChild, *pFound;

	// now search positive-z children in reverse order
	while (pChild && (pChild->GetZIndex() >= 0))
	{
		if (pChild->mFlags & DISPLAY_FLAG_FIXED_POS)
		{
			pFound = pChild->TrapEventPoint(x - (mRect.left + viewX), y - (mRect.top + viewY), -mRect.left, -mRect.top);
		}
		else
		{
			pFound = pChild->TrapEventPoint(x - mRect.left, y - mRect.top, viewX - mRect.left, viewY - mRect.top);
		}

		if (pFound)
		{
			return (pFound);
		}
		pChild = pChild->mpPrev;
	}

	// if we contain the point, trap it
	if (TrapEventInThis(x,y))
	{
		return (this);
	}

	// now search negative-z children in reverse order
	while (pChild)
	{
		if (pChild->mFlags & DISPLAY_FLAG_FIXED_POS)
		{
			pFound = pChild->TrapEventPoint(x - (mRect.left + viewX), y - (mRect.top + viewY), -mRect.left, -mRect.top);
		}
		else
		{
			pFound = pChild->TrapEventPoint(x - mRect.left, y - mRect.top, viewX - mRect.left, viewY - mRect.top);
		}

		if (pFound)
		{
			return (pFound);
		}
		pChild = pChild->mpPrev;
	}

	return (0);
}

WEBC_BOOL DisplayElement::TrapEventInThis (DISPLAY_INT x, DISPLAY_INT y)
{
	return (mRect.Contains(x,y));
}


void DisplayElement::GetBounds (WebRect *rect)
{
	if (mFlags & DISPLAY_FLAG_BOUNDS_VALID)
	{
		*rect = mBounds;
		return;
	}

	*rect = mRect;

	WebRect childRect;
	DisplayElement *pChild = mpFirstChild;

	while (pChild)
	{
		if (!(pChild->mFlags & DISPLAY_FLAG_FIXED_POS))
		{
			pChild->GetBounds(&childRect);
			childRect.Shift(mRect.left, mRect.top);
			rect->Or(&childRect);
		}

		pChild = pChild->mpNext;
	}

	SetFlag(DISPLAY_FLAG_BOUNDS_VALID);
	mBounds = *rect;
}

void DisplayElement::InvalidateChild (DisplayElement *pChild, WebRect *pDirty)
{
	WebRect dirty;
	dirty.Set(pDirty);
	dirty.Shift(mRect.left, mRect.top);

	if (GetOverflow() == DISPLAY_OVERFLOW_HIDDEN)
	{
		WebRect clipRect;
		GetClipRect(&clipRect);

		// If completely outside clipping rect, stop here
		if (!dirty.Overlaps(&clipRect))
		{
			return;
		}

		// clip pDirty to our rect
		dirty.And(&clipRect);
	}

	if ((mFlags & DISPLAY_FLAG_FIXED_POS) || !mpParent)
	{
		DisplayManager *pManager = GetManager();
		if (pManager)
		{
			if (mFlags & DISPLAY_FLAG_FIXED_POS)
			{
				dirty.Shift(pManager->mViewRect.left, pManager->mViewRect.top);
			}

			pManager->InvalidateViewportRegion(&dirty);
		}
	}
	else
	{
		mpParent->InvalidateChild(this, &dirty);
	}
 }

void DisplayElement::Invalidate (void)
{
	InvalidateRect(&mRect);
}

void DisplayElement::InvalidateRect(WebRect *pRect)
{
	if ((mFlags & DISPLAY_FLAG_FIXED_POS) || !mpParent)
	{
		DisplayManager *pManager = GetManager();
		if (pManager)
		{
			WebRect rect(*pRect);

			if (mFlags & DISPLAY_FLAG_FIXED_POS)
			{
				rect.Shift(pManager->mViewRect.left, pManager->mViewRect.top);
			}

			pManager->InvalidateViewportRegion(&rect);
		}
	}
	else
	{
		mpParent->InvalidateChild(this, pRect);
	}
}

void DisplayElement::SetManager (DisplayManager *pManager)
{
	if (pManager != GetManager())
	{
		DisplayManager *pOldManager = GetManager();
		mManager.Set(pManager);

		if (pOldManager)
		{
			pOldManager->RemoveElementReferences(this); // remove all references to this element
		}

		// Propogate down the tree
		DisplayElement *pChild = mpFirstChild;
		while (pChild)
		{
			pChild->SetManager(pManager);
			pChild = pChild->mpNext;
		}
	}
}

DisplayManager* DisplayElement::GetManager (void)
{
	return ((DisplayManager*) mManager.Get());
}

FormatResult DisplayElement::FormatForViewport (WebRect* viewportRect, WEBC_BOOL hscrollPresent, WEBC_BOOL vscrollPresent)
{
	return (DISPLAY_FORMAT_SUCCESS);
}

void DisplayElement::Notify(Notifier *who, NotifyEvent *what)
{
	if (!what)
		return;

    WEBC_DEBUG_ReceiveNotify(this, who, what);

	if (what->type == NOTIFIER_TYPE_DISPLAY_MANAGER)
	{
		switch (what->event)
		{
		case NOTIFY_WEB_GRAPHICS_CHANGE:
			if (GetManager())
			{
				DisplayElement *child = mpFirstChild;
				while (child)
				{
					child->Notify(who, what);
					child = child->mpNext;
				}
			}
			break;
        default:
            break;
		}
	}
}

void DisplayElement::InvalidateBounds (void)
{
	ClearFlag(DISPLAY_FLAG_BOUNDS_VALID);
	DisplayElement *elem = mpParent;
	while (elem && (elem->mFlags & DISPLAY_FLAG_BOUNDS_VALID))
	{
		elem->ClearFlag(DISPLAY_FLAG_BOUNDS_VALID);
		elem = elem->mpParent;
	}
}

void DisplayElement::SortByZIndex (void)
{
DisplayElement *unsorted = mpFirstChild, *child, *leastOfThese, *sorted = 0;

	// Bubble sort for now, just to test that it works
	while (unsorted)
	{
		leastOfThese = unsorted;
		child = unsorted;
		while (child)
		{
			if (child->GetZIndex() < leastOfThese->GetZIndex())
			{
				leastOfThese = child;
			}
			child = child->mpNext;
		}

		if (!sorted)
		{
			InsertFirst(leastOfThese);
		}
		else
		{
			if (sorted->mpNext != leastOfThese)
			{
				InsertAfter(sorted, leastOfThese);
			}
		}

		sorted = unsorted;
		unsorted = unsorted->mpNext;
	}
}

void DisplayElement::GetClientRect (WebRect *rect)
{
	*rect = mRect;
}

void DisplayElement::Disable (void)
{
	SetFlag(DISPLAY_FLAG_DISABLED);
}

void DisplayElement::Enable (void)
{
	ClearFlag(DISPLAY_FLAG_DISABLED);
}

DisplayElement *DisplayElement::FocusDefault (void)
{
	return (mpParent);
}

WebGraphics * DisplayElement::GetGraphics(void)
{
DisplayManager *m = GetManager();

	return (m? m->GetGraphics() : 0);
}

void DisplayElement::SetFixedPosition (WEBC_BOOL target)
{
	WEBC_BOOL changed = WEBC_FALSE;

	if (target)
	{
		if (!(mFlags & DISPLAY_FLAG_FIXED_POS))
		{
			mFlags |= DISPLAY_FLAG_FIXED_POS;
			changed = WEBC_TRUE;
		}
	}
	else
	{
		if (mFlags & DISPLAY_FLAG_FIXED_POS)
		{
			mFlags &= ~DISPLAY_FLAG_FIXED_POS;
			changed = WEBC_TRUE;
		}
	}

	if (changed)
	{
		DisplayElement* parent = mpParent;
		DisplayElement* child = this;

		while (parent)
		{
			DISPLAY_FLAGS oldFlags = parent->mFlags;

			WEBC_ASSERT(parent->mpFirstChild);

			if (child->mFlags & (DISPLAY_FLAG_FIXED_POS|DISPLAY_FLAG_FIXED_POS_CHILD))
			{
				parent->mFlags |= DISPLAY_FLAG_FIXED_POS_CHILD;
			}
			else
			{
				parent->mFlags &= ~DISPLAY_FLAG_FIXED_POS_CHILD;

				for (child = parent->mpFirstChild; child; child = child->mpNext)
				{
					if (child->mFlags & (DISPLAY_FLAG_FIXED_POS|DISPLAY_FLAG_FIXED_POS_CHILD))
					{
						parent->mFlags |= DISPLAY_FLAG_FIXED_POS_CHILD;
						break;
					}
				}
			}

			if (parent->mFlags == oldFlags)
			{
				break;
			}

			child = parent;
			parent = parent->mpParent;
		}
	}
}

#ifdef WEBC_DEBUG
void PrintRecievedDisplayElementHTMLEvent(DisplayElement *TheReciever, HTMLEvent *TheEvent);

void DisplayElement::WEBC_DEBUG_ReceiveEvent(DisplayElement *TheReciever, HTMLEvent *TheEvent)
{
    PrintRecievedDisplayElementHTMLEvent(TheReciever, TheEvent);
}
void PrintRecievedDisplayElementNotifyEvent(DisplayElement *TheReciever, Notifier *who, NotifyEvent *what);

void DisplayElement::WEBC_DEBUG_ReceiveNotify(DisplayElement *TheReciever, Notifier *who, NotifyEvent *what)
{
    PrintRecievedDisplayElementNotifyEvent(TheReciever, who, what);
}
#endif
