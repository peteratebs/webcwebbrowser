//
// DMANAGER.CPP - Methods for class DisplayManager
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

#include "dmanager.hpp"
#include "wtscroll.hpp"
#include "dbgtimer.h"
#include "rtptime.h"
#include "objref.cpp"
#include "webcmem.h"
#include "wgcolors.hpp"
#include "webcassert.h"
#include "webcdefault.h"
#include "webcfg.h"

/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/

// tbd - what is a reasonable value for this?
#define DIRTY_RECT_OVERHEAD     10000 // (units == pixels)


/*****************************************************************************/
// Local Types
/*****************************************************************************/

enum DirtyRectAddMethod
{
	ADD_RECT,
	MERGE_RECT,
	SPLIT_RECT
};


/*****************************************************************************/
// Global Data
/*****************************************************************************/

static WEBC_READONLY int _splitCost[] =
{
	2, 1, 1, 0, 1, 2, 2, 1, 1, 2, 2, 1, 0, 1, 1, 2
};


/*****************************************************************************/
// Local functions
/*****************************************************************************/

/*****************************************************************************/
// DisplayManager::DisplayManager ()
//
//   Default Constructor
//
/*****************************************************************************/

DisplayManager::DisplayManager (void)
{
	mpPointerOwner = 0;
	mpFocus = 0;
	mpMouseOverElement = 0;
	mpMouseDownElement = 0;
	mbClickTimeValid = 0;
	miClickTime = 0;
	mpGC = 0;
	mpVScroll = 0;
	mpHScroll = 0;
	mScrollMode = SCROLL_MODE_NONE;
	mManagerFlags = 0;
	mNoRefresh = 0;
	mViewportChanged = WEBC_FALSE;
    mbDrawDone = WEBC_FALSE;
	mBrowser = 0;
  #ifdef WEBC_BUFFER_SCROLLING
	mManagerFlags |= MANAGER_FLAG_BUFFER_SCROLL;
	mScrollBuffer = 0;
	mViewportBuffer = 0;
  #endif

	vector_init(&mDirtyList, sizeof(WebRect));

	mViewRect.Set(0,0,0,0);

  #ifdef WEBC_DEBUG
	mRefreshLogIndex = 0;
  #endif
}


/*****************************************************************************/
// DisplayManager::~DisplayManager ()
//
//   Destructor
//
/*****************************************************************************/

DisplayManager::~DisplayManager ()
{
	if (mpVScroll)
	{
		Remove(mpVScroll);
		WEBC_DELETE(mpVScroll);
	}

	if (mpHScroll)
	{
		Remove(mpHScroll);
		WEBC_DELETE(mpHScroll);
	}

	SetGraphics(0);

	vector_delete(&mDirtyList);
}



/*****************************************************************************/
// DisplayManager::SetRoot (
//   DisplayElement *pRoot - DisplayElement to set as "root" element
//   )
//
//   This method sets the root element for a display manager.  This element
//    serves as the root of the DisplayElement tree contained by this
//    DisplayManager.  This element is assumed to reside beneath all other
//    elements, so it is always drawn before other elements.  It also
//    is the default recipient of events and keyboard focus.
//
// Returns nothing.
//
/*****************************************************************************/

void DisplayManager::SetRoot (DisplayElement *pRoot)
{
	if (mRoot.Get() && (mRoot.Get()->GetManager() == this))
	{
		mRoot.Get()->SetManager(0);
	}
	mRoot.Set(pRoot);
	if (mRoot.Get())
	{
		mRoot.Get()->SetManager(this);
		mViewportChanged = WEBC_TRUE;
	}
}

DisplayElement * DisplayManager::GetRoot (void)
{
	return (mRoot.Get());
}

void DisplayManager::SetGraphics (WebGraphics *pGC)
{
	if (mpGC != pGC)
	{
		mpGC = pGC;
		if (mRoot.Get())
		{
			NotifyEvent e(NOTIFIER_TYPE_DISPLAY_MANAGER, NOTIFY_WEB_GRAPHICS_CHANGE);
			mRoot.Get()->Notify((DisplayManager*) this, &e);
		}
	}
}

void DisplayManager::SetViewport (WebRect *pViewRect)
{
	if (!mViewRect.Equals(pViewRect))
	{
		PresetWebRect oldRect(&mViewRect);

		mViewportChanged = WEBC_TRUE;
		mViewRect.Set(pViewRect);

		CorrectViewportPosition();

	  #ifdef WEBC_BUFFER_SCROLLING
		if (mManagerFlags & MANAGER_FLAG_BUFFER_SCROLL)
		{
			WebGraphics* gc = this->GetGraphics();
			if (gc)
			{
				if (mScrollBuffer == 0)
				{
					mScrollBuffer = gc->CreateBuffer(mViewRect.Width() * 2, mViewRect.Height() * 2);
				}
				else
				{
					if (oldRect.Width() != mViewRect.Width() ||
					    oldRect.Height() != mViewRect.Height())
					{
						mScrollBuffer = gc->ResizeBuffer(mScrollBuffer, mViewRect.Width() * 2, mViewRect.Height() * 2);
						AddDirtyRect(&mViewRect);
						SetManagerFlag(MANAGER_FLAG_DIRTY);
						return;
					}
				}

				if (mScrollBuffer)
				{
					// find non-overlapping regions
					if (mViewRect.Overlaps(&oldRect))
					{
						PresetWebRect viewRect(&mViewRect);
						WebRect extraRect;

						int count = viewRect.Split(&oldRect, &extraRect);
						if (count > 0)
						{
							if (count > 1)
							{
								if (count > 2)
								{
									if (!extraRect.Empty())
									{
										AddDirtyRect(&extraRect);
									}
								}
								if (!oldRect.Empty())
								{
									AddDirtyRect(&oldRect);
								}
							}
							if (!viewRect.Empty())
							{
								AddDirtyRect(&viewRect);
							}
						}
					}
					else
					{
						AddDirtyRect(&mViewRect);
					}

					SetManagerFlag(MANAGER_FLAG_DIRTY);
				}
			}

		}
	  #endif
	}
}

void DisplayManager::GetRootBounds (WebRect* bounds)
{
	if (mRoot.Get())
	{
		mRoot.Get()->GetBounds(bounds);

		if (bounds->left < 0)
		{
			bounds->left = 0;
		}

		if (bounds->top < 0)
		{
			bounds->top = 0;
		}
	}
	else
	{
		bounds->Set(0,0,0,0);
	}
}

void DisplayManager::CorrectViewportPosition (void)
{
	if (mRoot.Get())
	{
		WebRect bounds;
		PresetWebRect rect(&mViewRect);

		GetRootBounds(&bounds);
		rect.MoveTo(0,0);
		bounds.Or(&rect);

		if (mViewRect.left < bounds.left)
		{
			mViewRect.Shift(bounds.left - mViewRect.left, 0);
		}

		if (mViewRect.right > bounds.right)
		{
			mViewRect.Shift(bounds.right - mViewRect.right, 0);
		}

		if (mViewRect.top < bounds.top)
		{
			mViewRect.Shift(0, bounds.top - mViewRect.top);
		}

		if (mViewRect.bottom > bounds.bottom)
		{
			mViewRect.Shift(0, bounds.bottom - mViewRect.bottom);
		}

		if (mpVScroll)
		{
			// Range and Window must be set BEFORE Position,
			//  so position is not unjustly clipped!
			mpVScroll->SetRange(bounds.Height());
			mpVScroll->SetWindow(mViewRect.Height());
			mpVScroll->SetPosition(mViewRect.top);
		}

		if (mpHScroll)
		{
			// Range and Window must be set BEFORE Position,
			//  so position is not unjustly clipped!
			mpHScroll->SetRange(bounds.Width());
			mpHScroll->SetWindow(mViewRect.Width());
			mpHScroll->SetPosition(mViewRect.left);
		}
	}
}

WebGraphics *DisplayManager::GetGraphics (void)
{
	return (mpGC);
}

void DisplayManager::Invalidate (void)
{
	if (mpParent || GetManager())
	{
		DisplayElement::Invalidate();
	}
	else
	{
		InvalidateViewport();
	}
}

void DisplayManager::InvalidateViewportRegion (WebRect* pDirty)
{
	if (mViewRect.Overlaps(pDirty))
	{
		WebRect padding;
		GetPaddingWidth(&padding);

		WebRect dirty(*pDirty);
		dirty.And(&mViewRect);
		DisplayManager* myManager = this->GetManager();

		WEBC_BOOL addRect = (!mpParent && !myManager);

	  #ifdef WEBC_BUFFER_SCROLLING
		if (mManagerFlags & MANAGER_FLAG_BUFFER_SCROLL)
		{
			addRect = WEBC_TRUE;
		}
	  #endif

		if (addRect)
		{
			SetManagerFlag(MANAGER_FLAG_DIRTY);
			AddDirtyRect(&dirty);
		}

		if (mpParent)
		{
			dirty.Shift((mRect.left + padding.left) - mViewRect.left, (mRect.top + padding.top) - mViewRect.top);
			mpParent->InvalidateChild(this, &dirty);
		}
		else if (myManager)
		{
			dirty.Shift((mRect.left + padding.left) - mViewRect.left, (mRect.top + padding.top) - mViewRect.top);
			myManager->InvalidateViewportRegion(&dirty);
		}
	}
}


void DisplayManager::AddDirtyRect (WebRect *newRect)
{
DISPLAY_INT bestCost, newRectArea, mergeCost, splitCost;
DirtyRectAddMethod bestMethod = ADD_RECT;
unsigned char rectCase, bestCase = '\0';
vector_iterator vi;
WebRect _rect,
        *currentRect,
        *bestRect = 0,
        *unionRect = &_rect,
        *intersectRect = &_rect,
        *extraRect = &_rect;
	// cost for just adding a new rectangle;
	newRectArea = newRect->Area();
	bestCost = newRectArea + DIRTY_RECT_OVERHEAD;

	SetManagerFlag(MANAGER_FLAG_BUFFER_DIRTY);

	currentRect = (WebRect *) vector_get_first(&mDirtyList, &vi);
	while (currentRect)
	{
		unionRect->Set(newRect);
		unionRect->Or(currentRect);
		mergeCost = unionRect->Area() - currentRect->Area();

		if (mergeCost < bestCost)
		{
			bestCost = mergeCost;
			bestMethod = MERGE_RECT;
			bestRect = currentRect;
			if (bestCost <= 0)
			{
				break;
			}
		}

		if (newRect->Overlaps(currentRect))
		{
			rectCase = currentRect->Compare(newRect);

			intersectRect->Set(newRect);
			intersectRect->And(currentRect);
			splitCost = newRectArea - intersectRect->Area() +
					(DISPLAY_INT) (_splitCost[rectCase] * DIRTY_RECT_OVERHEAD);

			if (splitCost < bestCost)
			{
				bestCost = splitCost;
				bestMethod = SPLIT_RECT;
				bestRect = currentRect;
				bestCase = rectCase;
			}
		}

		currentRect = (WebRect *) vector_get_next(&vi);
	}

	switch (bestMethod)
	{
		case ADD_RECT:
			if (vector_add_node_to_front(&mDirtyList, (WEBC_PFBYTE) newRect) < 0)
			{
				if (bestRect)
				{
					bestRect->Or(newRect);
				}
			}
			break;

		case MERGE_RECT:
			bestRect->Or(newRect);
			break;

		case SPLIT_RECT:
		{
			// split either bestRect or newRect
			int numRects = bestRect->MergeCase(newRect, extraRect, bestCase);
			if (numRects == 3)
			{
				if (vector_add_node_to_front(&mDirtyList, (WEBC_PFBYTE) extraRect) < 0)
				{
					if (bestRect)
					{
						bestRect->Or(extraRect);
					}
				}

				if (vector_add_node_to_front(&mDirtyList, (WEBC_PFBYTE) newRect) < 0)
				{
					if (bestRect)
					{
						bestRect->Or(newRect);
					}
				}
			}
			else if (numRects == 2)
			{
				if (vector_add_node_to_front(&mDirtyList, (WEBC_PFBYTE) newRect) < 0)
				{
					if (bestRect)
					{
						bestRect->Or(newRect);
					}
				}
			}
			break;
		}
	}
}


void DisplayManager::InvalidateViewport (void)
{
	InvalidateViewportRegion(&mViewRect);
}

extern "C" unsigned long WebCItemDrawCount = 0; 

void wcDisplayManagerDrawCallback(DisplayManager *manager);

void DisplayManager::Refresh (void)
{
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_DISPLAYMANAGER_REFRESH);

	if (mNoRefresh)
	{
		WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_DISPLAYMANAGER_REFRESH);
		return;
	}

	// if we are not the master DisplayManager, pass the Draw up the chain
	//  (this is so sibling DisplayElements that overlap us will be updated correctly)
	if (GetManager())
	{
		GetManager()->Refresh();
	}
	else
	{
		// if the canvas size has changed, resize ourselves here and invalidate
		//  our rect so we do a clean re-draw
		if (mpGC)
		{
			WebRect canvasRect;
			mpGC->GetRect(&canvasRect);
			if (!mRect.Equals(&canvasRect))
			{
				Move(&canvasRect);
				Invalidate();
			}
		}

	  #ifdef WEBC_BUFFER_SCROLLING
		if (GetManagerFlags() & MANAGER_FLAG_BUFFER_DIRTY)
		{
			UpdateScrollBuffer();
		}
	  #endif

		if (GetManagerFlags() & MANAGER_FLAG_DIRTY)
		{
			WebRect *pDirty;

			// move the dirty rectangle list onto the stack so it can't be corrupted
			vector dirtyListCopy = mDirtyList;

			mDirtyList.first = 0;
			mDirtyList.last = 0;
			ClearManagerFlag(MANAGER_FLAG_DIRTY);

			vector_iterator vi;

			pDirty = (WebRect*) vector_get_first(&dirtyListCopy, &vi);
			while (pDirty)
			{
				// else just call our DisplayElement Draw method
				WebRect ScreenView(mViewRect), saveClip, dirty;
				ScreenView.MoveTo(mRect.left, mRect.top);
				dirty.Set(pDirty);
				dirty.Shift(ScreenView.left - mViewRect.left, ScreenView.top - mViewRect.top);
				if (ScreenView.Overlaps(&dirty) && mpGC)
				{
					dirty.And(&ScreenView);
					mpGC->Invalidate(&dirty);
					mpGC->StartBuffer();
					mpGC->GetClip(&saveClip);
					mpGC->SetClip(&dirty);
					Draw(mRect.left, mRect.top, &ScreenView, mpGC);
					mpGC->SetClip(&saveClip);
					mpGC->EndBuffer();
					WebCItemDrawCount+=1;
				}

				pDirty = (WebRect*) vector_get_next(&vi);
			}

            mbDrawDone = WEBC_TRUE;
			vector_delete(&dirtyListCopy);
           /* The refresh operation clears dirty.. but some draw operations
              may set dirty and add to the dirty list. Not needed here
              so clear dirty status again and delete the list  */
			if (GetManagerFlags() & MANAGER_FLAG_DIRTY)
			{
                /* Bug fix Jan 2008, fix leak if dirty records were added during refresh */
			    vector_delete(&mDirtyList);
		        ClearManagerFlag(MANAGER_FLAG_DIRTY);
	        }
			// Tell the API that we have redrawn. It can key off of this to be sure its positioning database is uo to date.
	        wcDisplayManagerDrawCallback(this);
		}
	}

	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_DISPLAYMANAGER_REFRESH);
}


void DisplayManager::RefreshElement (DisplayElement *pElem)
{
	pElem->Invalidate();
	Refresh();
}

WEBC_BOOL DisplayManager::ClaimPointer (DisplayElement *pElem)
{
	if (!mpPointerOwner)
	{
		mpPointerOwner = pElem;
		if (GetManager())
		{
			GetManager()->ClaimPointer(this);
		}
		else
		{
			if (mpGC)
			{
				mpGC->ClaimPointer();
			}
		}

		return (1);
	}

	// someone else already has it
	return (0);
}

void DisplayManager::ReleasePointer (DisplayElement *pElem)
{
	if (mpPointerOwner == pElem)
	{
		mpPointerOwner = 0;
		if (GetManager())
		{
			GetManager()->ReleasePointer(this);
		}
		else
		{
			if (mpGC)
			{
				mpGC->ReleasePointer();
			}
		}
	}
}

DisplayElement *DisplayManager::GetFocus (void)
{
	return (mpFocus);
}


HTMLEventStatus DisplayManager::SetFocus (DisplayElement *pFocus)
{
	if (mpFocus != pFocus)
	{
		HTMLEvent e;
		DisplayElement *oldFocus = mpFocus;

		if (oldFocus)
		{
			oldFocus->ClearFlag(DISPLAY_FLAG_FOCUS);
			e.type = HTML_EVENT_UNFOCUS;
			if (oldFocus->Event(&e) == HTML_EVENT_STATUS_HALT)
			{
				return (HTML_EVENT_STATUS_HALT);
			}
		}

		if (pFocus)
		{
			e.type = HTML_EVENT_FOCUS;
			if (pFocus->Event(&e) == HTML_EVENT_STATUS_HALT)
			{
				return (HTML_EVENT_STATUS_HALT);
			}
			pFocus->SetFlag(DISPLAY_FLAG_FOCUS);
		}

		mpFocus = pFocus;

		if (GetManager())
		{
			GetManager()->SetFocus(this);
		}
	}

	return (HTML_EVENT_STATUS_CONTINUE);
}

HTMLEventStatus DisplayManager::RemoveFocus (DisplayElement *pFocus)
{
	if (mpFocus && (mpFocus == pFocus))
	{
		mpFocus->ClearFlag(DISPLAY_FLAG_FOCUS);

		HTMLEvent e;

		e.type = HTML_EVENT_UNFOCUS;
		if (mpFocus->Event(&e) == HTML_EVENT_STATUS_HALT)
		{
			return (HTML_EVENT_STATUS_HALT);
		}

		mpFocus = 0;
	}

	return (HTML_EVENT_STATUS_CONTINUE);
}

void DisplayManager::RemoveElementReferences (DisplayElement *pElem)
{
	if (mpFocus && (mpFocus == pElem))
	{
		mpFocus->ClearFlag(DISPLAY_FLAG_FOCUS);
		mpFocus = 0;
	}

	if (mpMouseOverElement == pElem)
	{
		mpMouseOverElement = 0;
	}

	if (mpMouseDownElement == pElem)
	{
		mpMouseDownElement = 0;
	}

	ReleasePointer(pElem);

	if (GetRoot() == pElem)
	{
		SetRoot(0);
	}
}

// All (x,y) coordinates in events passed into DispatchEvent are given in
//  the coordinate space of the viewport rectangle of this DisplayManager
//  (so (0,0) maps to DisplayManager coordinates (mViewRect.left, mViewRect.top) etc.)
// When the DisplayManager calls the Event method of the proper DisplayElement,
//  it gives the (x,y) coordinates in DisplayManager coordinates (the same
//  coordinate space as the DisplayElement's mRect).

HTMLEventStatus DisplayManager::DispatchEvent (HTMLEvent *pEvent)
{
	HTMLEventStatus status;

	switch (pEvent->type)
	{
		case HTML_EVENT_MOUSEDOWN:
		case HTML_EVENT_MOUSEOVER:
		case HTML_EVENT_MOUSEMOVE:
		case HTML_EVENT_MOUSEOUT:
		case HTML_EVENT_MOUSEUP:
		case HTML_EVENT_CLICK:
		case HTML_EVENT_DBLCLICK:
			pEvent->data.position.x += mViewRect.left;
			pEvent->data.position.y += mViewRect.top;
			break;

		default:
			break;
	};

	switch (pEvent->type)
	{
		case HTML_EVENT_KEYUP:

			if (!mpFocus)
			{
				mpFocus = mRoot.Get();
			}

			if (mpFocus)
			{
				return (mpFocus->Event(pEvent));
			}
			break;

		case HTML_EVENT_KEYDOWN:

			if (!mpFocus)
			{
				mpFocus = mRoot.Get();
			}

			if (mpFocus)
			{
				ObjectReference<DisplayElement> focus(mpFocus);

				status = mpFocus->Event(pEvent);

				if ((status != HTML_EVENT_STATUS_HALT) && focus.Get())
				{
					HTMLEvent e = *pEvent;
					e.type = HTML_EVENT_KEYPRESS;
					return (focus.Get()->Event(&e));
				}

				return (status);
			}
			break;

		case HTML_EVENT_MOUSEDOWN:
			if (mpPointerOwner)
			{
				return (mpPointerOwner->Event(pEvent));
			}
			else
			{
				DisplayElement *pElem = FindTopElement(pEvent->data.position.x, pEvent->data.position.y);

				if (!pElem)
				{
					pElem = mRoot.Get();
					if (!pElem)
					{
						return (HTML_EVENT_STATUS_DONE);
					}

				}

				mpMouseDownElement = pElem;

				HTMLEventStatus result = pElem->Event(pEvent);

				if (result != HTML_EVENT_STATUS_HALT &&
				    !(pEvent->flags & HTML_EVENT_FLAG_CANCEL_SET_FOCUS))
				{
					if (mpFocus != pElem)
					{
						DisplayElement *pNewFocus = pElem;

						/* Find the closest ancestor (including pElem) of pElem that is
						   able to receive the keyboard FOCUS */

						while (pNewFocus &&
						         (!(pNewFocus->mFlags & DISPLAY_FLAG_ACCEPTS_FOCUS) ||
						           (pNewFocus->mFlags & DISPLAY_FLAG_DISABLED)))
						{
							pNewFocus = pNewFocus->FocusDefault();
						}

						if (pNewFocus && (pNewFocus != mpFocus))
						{
							if (SetFocus(pNewFocus) == HTML_EVENT_STATUS_HALT)
							{
								return (HTML_EVENT_STATUS_HALT);
							}
						}
					}
				}

				return (result);
			}
			break;

		case HTML_EVENT_MOUSEOVER:
		case HTML_EVENT_MOUSEMOVE:
		{
			DisplayElement *pElem = FindTopElement(pEvent->data.position.x, pEvent->data.position.y);
			if (!pElem)
			{
				pElem = mRoot.Get();
				if (!pElem)
				{
					return (HTML_EVENT_STATUS_DONE);
				}
			}
			if (pElem != mpMouseOverElement)
			{
				DisplayElement *pOld = mpMouseOverElement;
				mpMouseOverElement = pElem;
				HTMLEvent e = *pEvent;

				if (pOld)
				{
					e.type = HTML_EVENT_MOUSEOUT;
					if (pOld->Event(&e) == HTML_EVENT_STATUS_HALT)
					{
						return (HTML_EVENT_STATUS_HALT);
					}
				}

				// if this is a mouseover event, then we send it to pElem below
				if (mpMouseOverElement && pEvent->type != HTML_EVENT_MOUSEOVER)
				{
					e.type = HTML_EVENT_MOUSEOVER;
					if (mpMouseOverElement->Event(&e) == HTML_EVENT_STATUS_HALT)
					{
						return (HTML_EVENT_STATUS_HALT);
					}
				}
				return (HTML_EVENT_STATUS_CONTINUE); // April2013 - Return here, otherwise crashes below
			}

			// if the pointer is owned by an element, we shouldn't give WEBC_FALSE mouseover events
			//  but always pass along move events (WEBC_TRUE mouseover events will be caught above).
			if (pEvent->type == HTML_EVENT_MOUSEMOVE)
			{
				if (mpPointerOwner)
				{
					return (mpPointerOwner->Event(pEvent));
				}
			}

			if (pElem)
			{
				return (pElem->Event(pEvent));
			}

			return (HTML_EVENT_STATUS_CONTINUE);
		}

		case HTML_EVENT_MOUSEOUT:
			if (mpMouseOverElement)
			{
				DisplayElement *pOld = mpMouseOverElement;
				mpMouseOverElement = 0;

				return (pOld->Event(pEvent));
			}
			break;

		case HTML_EVENT_MOUSEUP:
		{
			DisplayElement *pElem;

			if (mpPointerOwner)
			{
				pElem = mpPointerOwner;
			}
			else
			{
				pElem = FindTopElement(pEvent->data.position.x, pEvent->data.position.y);
				if (!pElem)
				{
					pElem = mRoot.Get();
					if (!pElem)
					{
						return (HTML_EVENT_STATUS_DONE);
					}
				}
			}

			if (pElem->Event(pEvent) == HTML_EVENT_STATUS_HALT)
			{
				return (HTML_EVENT_STATUS_HALT);
			}

			if (pElem == mpMouseDownElement)
			{
				HTMLEventStatus result;
				HTMLEvent e = *pEvent;
				e.type = HTML_EVENT_CLICK;

				mpMouseDownElement = 0;

				result = pElem->Event(&e);
				if (result != HTML_EVENT_STATUS_HALT)
				{

					if (mbClickTimeValid && ((rtp_get_system_msec() - miClickTime) <= WEBC_DBLCLICK_TIME))
					{
						mbClickTimeValid = 0;
						e.type = HTML_EVENT_DBLCLICK;
						result = pElem->Event(&e);
					}
					else
					{
						mbClickTimeValid = 1;
						miClickTime = rtp_get_system_msec();
					}
				}
				return (result);
			}

			mpMouseDownElement = 0;
		}
		break;

		// we don't expect to get any of these from the OS
		case HTML_EVENT_FOCUS:    // intentional fall-through
		case HTML_EVENT_UNFOCUS:  // intentional fall-through
		case HTML_EVENT_CLICK:    // intentional fall-through
		case HTML_EVENT_DBLCLICK: // intentional fall-through
		case HTML_EVENT_KEYPRESS: // intentional fall-through
		case HTML_EVENT_LOAD:     // intentional fall-through
		case HTML_EVENT_UNLOAD:   // intentional fall-through
		case HTML_EVENT_SUBMIT:   // intentional fall-through
		case HTML_EVENT_CHANGE:   // intentional fall-through
		case HTML_EVENT_EDIT:     // intentional fall-through
		case HTML_EVENT_RESET:    // intentional fall-through
		default:
			break;
	}

	return (HTML_EVENT_STATUS_CONTINUE);
}

DisplayElement *DisplayManager::FindTopElement (DISPLAY_INT iX, DISPLAY_INT iY)
{
	// if the caller has defined a capture object (via JavaScript) then
	// send all mouse events to that
	if(mCaptureRoot.Get())
		return mCaptureRoot.Get();

	if (mRoot.Get())
	{
		return (mRoot.Get()->TrapEventPoint(iX, iY, mViewRect.left, mViewRect.top));
	}

	return (0);
}

void DisplayManager::SetScrollMode (SCROLL_MODE m)
{
	mScrollMode = m;
}

SCROLL_MODE DisplayManager::GetScrollMode (void)
{
	return (mScrollMode);
}



/*****************************************************************************/
/*****************************************************************************/
//
// DisplayManager's Listener implementation
//
/*****************************************************************************/
/*****************************************************************************/

void DisplayManager::Notify (Notifier *who, NotifyEvent *what)
{
    WEBC_DEBUG_ReceiveNotify(this, who, what);
	if ((mpVScroll && (who == mpVScroll)) || (mpHScroll && (who == mpHScroll)))
	{
		if (what && what->event == NOTIFY_SLIDER_MOVE)
		{
			PresetWebRect view(&mViewRect);

			if (who == mpVScroll)
			{
				view.MoveTo(mViewRect.left, mpVScroll->GetPosition());
			}
			else
			{
				view.MoveTo(mpHScroll->GetPosition(), mViewRect.top);
			}

			SetViewport(&view);
			SetFlag(DISPLAY_FLAG_DIRTY);
			Invalidate();
			return;
		}
	}

	if (what && what->type == NOTIFIER_TYPE_DISPLAY_MANAGER)
	{
		switch (what->event)
		{
		case NOTIFY_WEB_GRAPHICS_CHANGE:
			if (GetManager())
			{
				SetGraphics(GetManager()->GetGraphics());
			}
			break;
        default:
            break;
		}
	}

	DisplayElement::Notify(who, what);
}


/*****************************************************************************/
/*****************************************************************************/
//
// DisplayManager's DisplayElement implementation
//
/*****************************************************************************/
/*****************************************************************************/

HTMLEventStatus DisplayManager::Event (HTMLEvent *pEvent)
{
    WEBC_DEBUG_ReceiveEvent(this, pEvent);
	switch (pEvent->type)
	{
		case HTML_EVENT_FOCUS:
			return (HTML_EVENT_STATUS_CONTINUE);

		case HTML_EVENT_UNFOCUS:
			if (mpFocus)
			{
				return (RemoveFocus(mpFocus));
			}
			return (HTML_EVENT_STATUS_CONTINUE);

		case HTML_EVENT_MOUSEDOWN:
		case HTML_EVENT_MOUSEOVER:
		case HTML_EVENT_MOUSEMOVE:
		case HTML_EVENT_MOUSEOUT:
		case HTML_EVENT_MOUSEUP:
		case HTML_EVENT_CLICK:
		case HTML_EVENT_DBLCLICK:
		{
			// Adjust mouse coordinates from our parent DisplayManager to our viewport
			WebRect padding;
			GetPaddingWidth(&padding);

			DISPLAY_INT x = mRect.left, y = mRect.top;
			if (mpParent)
			{
				mpParent->GetDisplayPosition(this, &x, &y);
			}
			pEvent->data.position.x -= x + padding.left;
			pEvent->data.position.y -= y + padding.top;
			break;
		}

		default:
			break;
	};

	// Now hand off to the event dispatcher
	return (DispatchEvent(pEvent));
}

void DisplayManager::AttachVScroll (void)
{
	if (!mpVScroll)
	{
		WEBC_NEW_VERBOSE(mpVScroll, WebVScroll(),"AttachVScroll");
		if (mpVScroll)
		{
			mpVScroll->SetListener(this);
			this->InsertLast(mpVScroll);
		}
	}
}

void DisplayManager::AttachHScroll (void)
{
	if (!mpHScroll)
	{
		WEBC_NEW_VERBOSE(mpHScroll, WebHScroll(),"AttachHScroll");
		if (mpHScroll)
		{
			mpHScroll->SetListener(this);
			this->InsertLast(mpHScroll);
		}
	}
}

void DisplayManager::DetachVScroll (void)
{
	if (mpVScroll)
	{
		Remove(mpVScroll);
		WEBC_DELETE(mpVScroll);
		mpVScroll = 0;
	}
}

void DisplayManager::DetachHScroll (void)
{
	if (mpHScroll)
	{
		Remove(mpHScroll);
		WEBC_DELETE(mpHScroll);
		mpHScroll = 0;
	}
}

WebColor DisplayManager::GetBackgroundColor(WebGraphics* gc)
{
	return (gc->GetColorByIndex(WGC_WHITE));
}


void DisplayManager::Draw(DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC)
{
	if (mNoRefresh)
	{
		DisplayManager* parentDisplayManager = GetManager();

		if (parentDisplayManager)
		{
			// This handles the case where a DisplayManager that is NOT the root
			//  DisplayManager has a positive refresh lock, prohibiting its being
			//  drawn, but the parent DisplayManager does not (otherwise we would
			//  not even be in our Draw method); in this case:
			//
			// IF this element is within the collective invalid region being
			//    painted, THEN we want to re-invalidate the invalid part of (this),
			//    which will cause said region to re-paint when the refresh lock
			//    is removed from this element and this->Refresh is eventually called.

			WebRect clipRect;
			pGC->GetClip(&clipRect);

			WebRect screenRect(mRect);
			screenRect.MoveTo(x,y);

			if (clipRect.Overlaps(&screenRect))
			{
				SetManagerFlag(MANAGER_FLAG_TRIED_TO_DRAW);
				//screenRect.And(&clipRect);
				//screenRect.Shift(-pViewport->left, -pViewport->top);
				//parentDisplayManager->InvalidateViewportRegion(&screenRect);
			}
		}

		return;
	}

	FormatResult result;

	switch (GetScrollMode())
	{
		case SCROLL_MODE_NONE:
			if (mpVScroll || mpHScroll)
			{
				DetachVScroll();
				DetachHScroll();
				UpdateViewport();
			}
			break;

		case SCROLL_MODE_HSCROLL:
			if (mpVScroll || !mpHScroll)
			{
				DetachVScroll();
				AttachHScroll();
				UpdateViewport();
			}
			break;

		case SCROLL_MODE_VSCROLL:
			if (!mpVScroll || mpHScroll)
			{
				AttachVScroll();
				DetachHScroll();
				UpdateViewport();
			}
			break;

		case SCROLL_MODE_BOTH:
			if (!mpVScroll || !mpHScroll)
			{
				AttachVScroll();
				AttachHScroll();
				UpdateViewport();
			}
			break;

		case SCROLL_MODE_AUTO:
			break;
	}

	if (mRoot.Get())
	{
		int i = 0;

		do
		{
			result = mRoot.Get()->FormatForViewport(&mViewRect, (mpHScroll != 0), (mpVScroll != 0));

			if (GetScrollMode() == SCROLL_MODE_AUTO)
			{
				if (result == DISPLAY_FORMAT_NEEDS_VSCROLL)
				{
					// create a vscroll bar
					if (mpVScroll)
					{
						// what the... we already have a vscroll bar!
						break;
					}
					AttachVScroll();

					// check for creation failure - may indicate low memory condition
					if (!mpVScroll)
					{
						// can't go on...
						break;
					}

					UpdateViewport();
				}
				else if (result == DISPLAY_FORMAT_NEEDS_HSCROLL)
				{
					// create a hscroll bar
					if (mpHScroll)
					{
						// what the... we already have a hscroll bar!
						break;
					}
					AttachHScroll();

					// check for creation failure - may indicate low memory condition
					if (!mpHScroll)
					{
						// can't go on...
						break;
					}

					UpdateViewport();
				}
			}
			else
			{
				break;
			}

			// put a hard upper bound on the number of times we try to re-format
			//  (4, since this means we've covered all combinations of horizontal
			//   and vertical scroll bars on the window)
			i++;
		}
		while (result != DISPLAY_FORMAT_SUCCESS && i < 5);

		if (GetScrollMode() == SCROLL_MODE_AUTO)
		{
			WebRect rootBounds;
			GetRootBounds(&rootBounds);

			if (rootBounds.Height() <= mViewRect.Height() && mpVScroll)
			{
				DetachVScroll();
				UpdateViewport();
				mRoot.Get()->FormatForViewport(&mViewRect, (mpHScroll != 0), (mpVScroll != 0));
				GetRootBounds(&rootBounds);
			}

			if (rootBounds.Width() <= mViewRect.Width() && mpHScroll)
			{
				DetachHScroll();
				UpdateViewport();
				mRoot.Get()->FormatForViewport(&mViewRect, (mpHScroll != 0), (mpVScroll != 0));
				GetRootBounds(&rootBounds);
			}

			if (rootBounds.Height() <= mViewRect.Height() && mpVScroll)
			{
				DetachVScroll();
				UpdateViewport();
				mRoot.Get()->FormatForViewport(&mViewRect, (mpHScroll != 0), (mpVScroll != 0));
				GetRootBounds(&rootBounds);
			}
		}

		mViewportChanged = WEBC_FALSE;

		CorrectViewportPosition();

		if (mpFocus && (mpVScroll!=0) && (mpHScroll!=0))
		{
			WebRect focusRect;
			WebRect elemRect;
			WEBC_BOOL ensureFocusVisible = mpFocus->GetFocusRect (&focusRect);
			if (ensureFocusVisible)
			{
				GetElementRect(&elemRect, mpFocus);
				focusRect.Shift(elemRect.left, elemRect.top);

				// this will adjust the scroll bars so that the given rect is in view
				EnsureRectVisible(&focusRect);
			}
		}
	}

	DisplayElement::Draw(x, y, pViewport, pGC);
}

void DisplayManager::DrawThisOnly (DISPLAY_INT x, DISPLAY_INT y, WebGraphics *gc)
{
	DISPLAY_INT scrollLeft = (mpHScroll)? mpHScroll->GetPosition() : mViewRect.left;
	DISPLAY_INT scrollTop  = (mpVScroll)? mpVScroll->GetPosition() : mViewRect.top;

	mViewRect.MoveTo(scrollLeft, scrollTop);

	WebRect padding;
	GetPaddingWidth(&padding);

	// find our viewport rect in screen coordinates
	WebRect screenRect(mRect);
	screenRect.MoveTo(x,y);

	WebRect screenView(screenRect);
	screenView.Shift(padding.left, padding.top);
	screenView.SizeTo(mViewRect.Width(), mViewRect.Height());

	WebRect saveClip;
	gc->GetClip(&saveClip);

	WebRect screenDirty(screenView);
	if (saveClip.Overlaps(&screenDirty))
	{
		screenDirty.And(&saveClip);

	  #ifdef WEBC_BUFFER_SCROLLING

		if ((GetManagerFlags() & MANAGER_FLAG_BUFFER_SCROLL) &&
		    mScrollBuffer)
		{
			if (GetManagerFlags() & MANAGER_FLAG_BUFFER_DIRTY)
			{
				UpdateScrollBuffer();
			}

			// instead of invoking the Draw method of mRoot, just copy the contents of the
			//  buffer at this location.
			gc->CopyBufferRegion (
					&screenDirty,
					mScrollBuffer,
					screenDirty.left - screenView.left + (mViewRect.left % mViewRect.Width()),
					screenDirty.top  - screenView.top  + (mViewRect.top  % mViewRect.Height()));
		}
		else
		{

	  #endif

			gc->SetClip(&screenDirty);

			if (mRoot.Get())
			{
				mRoot.Get()->Draw(screenView.left + mRoot.Get()->mRect.left - scrollLeft,
				                  screenView.top  + mRoot.Get()->mRect.top  - scrollTop,
								  &screenView, gc);
			}

	  #ifdef WEBC_BUFFER_SCROLLING
		}
	  #endif

		gc->SetClip(&saveClip);
	}

	// draw the little gray rectangle
	if (mpVScroll && mpHScroll && webc_GetDefaultBoolean(WEBC_DEFAULT_DRAW_SCROLL_CORNER))
	{
		WebRect box;
		WebColor c = gc->RGBToColor(webc_GetDefaultColor(WEBC_DEFAULT_SCROLL_CORNER_COLOR));
		box.SetSizeAndPosition(screenView.right + 1, screenView.bottom + 1, mpVScroll->Width(), mpHScroll->Height());
		gc->Rectangle(&box, c, c, 1);
	}
}

void DisplayManager::UpdateViewport (void)
{
	WebRect view;
	WebRect padding;
	WebRect vscrollRect;
	WebRect hscrollRect;

	GetPaddingWidth(&padding);

	view.Set (mRect.left + padding.left,
	          mRect.top + padding.top,
	          mRect.right - padding.right,
	          mRect.bottom - padding.bottom);

	if (mpVScroll)
	{
		vscrollRect.Set (mRect.Width() - padding.right - webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_WIDTH), padding.top,
		                 mRect.Width() - 1 - padding.right, mRect.Height() - 1 - padding.bottom);

		view.right -= webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_WIDTH);

		if (mpHScroll)
		{
			hscrollRect.Set (padding.left, mRect.Height() - padding.bottom - webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_WIDTH),
			                 mRect.Width() - padding.right - webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_WIDTH),
			                 mRect.Height() - 1 - padding.bottom);

			vscrollRect.bottom -= webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_WIDTH) - 1;
			view.bottom -= webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_WIDTH);

			mpHScroll->Move(&hscrollRect);
		}

		mpVScroll->Move(&vscrollRect);
	}
	else
	{
		if (mpHScroll)
		{
			hscrollRect.Set (padding.left, mRect.Height() - padding.bottom - webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_WIDTH),
			                 mRect.Width() - 1 - padding.right,
			                 mRect.Height() - 1 - padding.bottom);

			view.bottom -= webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_WIDTH);

			mpHScroll->Move(&hscrollRect);
		}
	}

	view.MoveTo(mViewRect.left, mViewRect.top);
	SetViewport(&view);
}

FormatResult DisplayManager::FormatForViewport (
		WebRect* viewportRect,
		WEBC_BOOL hscrollPresent,
		WEBC_BOOL vscrollPresent)
{
	Move(viewportRect);
	return DISPLAY_FORMAT_SUCCESS;
}


void DisplayManager::Move (WebRect *pRect)
{
	DisplayElement::Move(pRect);
	UpdateViewport();
}

void DisplayManager::GetPaddingWidth (WebRect* r)
{
	r->left = 0;
	r->top = 0;
	r->right = 0;
	r->bottom = 0;
}

void DisplayManager::SetHeight (DISPLAY_INT h)
{
	if (Height() != h)
	{
		DisplayElement::SetHeight(h);
		UpdateViewport();
	}
}

void DisplayManager::SetWidth (DISPLAY_INT w)
{
	if (Width() != w)
	{
		DisplayElement::SetWidth(w);
		UpdateViewport();
	}
}

void DisplayManager::SizeTo (DISPLAY_INT w, DISPLAY_INT h)
{
	if (Height() != h || Width() != w)
	{
		DisplayElement::SizeTo(w, h);
		UpdateViewport();
	}
}

DisplayOverflow DisplayManager::GetOverflow(void)
{
	return (DisplayElement::GetOverflow());
}

void DisplayManager::GetClipRect(WebRect *pClipRect)
{
	*pClipRect = mRect;
}


void DisplayManager::SetManager(DisplayManager *pManager)
{
	if (pManager)
	{
		SetGraphics(pManager->GetGraphics());
	}

	DisplayElement::SetManager(pManager);
}

MANAGER_FLAGS DisplayManager::GetManagerFlags(void)
{
	return mManagerFlags;
}

void DisplayManager::SetManagerFlag(MANAGER_FLAGS mask)
{
	mManagerFlags |= mask;
}

void DisplayManager::ClearManagerFlag(MANAGER_FLAGS mask)
{
	mManagerFlags &= ~mask;
}

void DisplayManager::GetElementRect (WebRect* rect, DisplayElement* elem)
{
	if (elem->GetManager() == this)
	{
		DISPLAY_INT left = 0, top = 0;

		if (elem->mpParent)
		{
			elem->mpParent->GetDisplayPosition(elem, &left, &top);
		}
		else
		{
			left = elem->mRect.left;
			top = elem->mRect.top;
		}

		rect->Set(&elem->mRect);
		rect->MoveTo(left, top);
	}
	else
	{
		rect->Set(0,0,0,0);
	}
}

void DisplayManager::EnsureRectVisible (WebRect* r)
{
	WebRect rect(*r);
	DISPLAY_INT left = mViewRect.left;
	DISPLAY_INT top  = mViewRect.top;

	if (rect.bottom > mViewRect.bottom)
	{
		top = rect.bottom - mViewRect.Height();
	}

	if (rect.top < top)
	{
		top = rect.top;
	}

	if (rect.right > mViewRect.right)
	{
		left = rect.right - mViewRect.Width();
	}

	if (rect.left < left)
	{
		left = rect.left;
	}

	rect.Set(&mViewRect);
	rect.MoveTo(left, top);
	SetViewport(&rect);
}

void DisplayManager::EnsureVisible(DisplayElement *pElem)
{
	if (pElem->GetManager() == this)
	{
		WebRect rect;
		GetElementRect(&rect, pElem);
		EnsureRectVisible(&rect);
	}
}

WEBC_BOOL DisplayManager::IsVisible(DisplayElement *pElem)
{
	if (pElem->GetManager() == this)
	{
		DISPLAY_INT left = 0, top = 0;

		if (pElem == mRoot.Get())
		{
			// the root object is always visible
			return WEBC_TRUE;
		}

		if (pElem->mpParent)
		{
			pElem->mpParent->GetDisplayPosition(pElem, &left, &top);
		}
		else
		{
			left = pElem->mRect.left;
			top = pElem->mRect.top;
		}

		WebRect rect(pElem->mRect);
		rect.MoveTo(left, top);

		return (RectVisible(&rect));
	}

	return WEBC_FALSE;
}

WEBC_BOOL DisplayManager::RectVisible (WebRect *pRect)
{
	if (mViewRect.Overlaps(pRect))
	{
		DisplayManager *parentManager = GetManager();

		if (parentManager)
		{
			PresetWebRect rect(pRect);
			rect.And(&mViewRect);

			WebRect padding;
			GetPaddingWidth(&padding);

			DISPLAY_INT x = mRect.left, y = mRect.top;
			if (mpParent)
			{
				mpParent->GetDisplayPosition(this, &x, &y);
			}

			rect.Shift(x + padding.left - mViewRect.left, y + padding.top - mViewRect.top);

			return (parentManager->RectVisible(&rect));
		}
		else
		{
			return (WEBC_TRUE);
		}
	}

	return (WEBC_FALSE);
}

void DisplayManager::ResumeRefresh(void)
{
	WEBC_ASSERT(mNoRefresh > 0);

	if (mNoRefresh > 0)
	{
		mNoRefresh--;
		if (mNoRefresh == 0)
		{
			if (mManagerFlags & MANAGER_FLAG_TRIED_TO_DRAW)
			{
				ClearManagerFlag(MANAGER_FLAG_TRIED_TO_DRAW);
				this->Invalidate();
			}
		}
	}
}

void DisplayManager::StopRefresh(void)
{
	WEBC_ASSERT(mNoRefresh < 0x7fff);
	mNoRefresh++;
}

int DisplayManager::GetRefreshState(void)
{
	return ((int) mNoRefresh);
}
/*
void DisplayManager::SetRefreshState(int level)
{
	mNoRefresh = (unsigned char) EBSCLIP(level, 0, 0x7fff);
}
*/

void DisplayManager::ResetDrawDone(void)
{
    mbDrawDone = WEBC_FALSE;
}

WEBC_BOOL DisplayManager::WasDrawDone(void)
{
    return mbDrawDone;
}

void DisplayManager::SetCapture (DisplayElement *pCaptureRoot)
{
	mCaptureRoot.Set(pCaptureRoot);
}


#ifdef WEBC_DEBUG

void DisplayManager::DebugStopRefresh (const char* file, int line)
{
	if (mRefreshLogIndex < DISPLAY_MANAGER_REFRESH_LOG_SIZE)
	{
		mRefreshLog[mRefreshLogIndex].file = file;
		mRefreshLog[mRefreshLogIndex].line = line;
		mRefreshLog[mRefreshLogIndex].type = "stop";
		mRefreshLogIndex++;
	}

	StopRefresh();
}

void DisplayManager::DebugResumeRefresh (const char* file, int line)
{
	if (mRefreshLogIndex < DISPLAY_MANAGER_REFRESH_LOG_SIZE)
	{
		mRefreshLog[mRefreshLogIndex].file = file;
		mRefreshLog[mRefreshLogIndex].line = line;
		mRefreshLog[mRefreshLogIndex].type = "resume";
		mRefreshLogIndex++;
	}

	ResumeRefresh();

	if (mNoRefresh == 0)
	{
		mRefreshLogIndex = 0;
	}
}
#endif

#ifdef WEBC_BUFFER_SCROLLING

void DisplayManager::UpdateScrollBuffer (void)
{
	if ((GetManagerFlags() & MANAGER_FLAG_DIRTY) &&
	    mRoot.Get() &&
		mScrollBuffer)
	{
		WebRect* dirty;

		ClearManagerFlag(MANAGER_FLAG_BUFFER_DIRTY);

		// move the dirty rectangle list onto the stack so it can't be corrupted
		vector dirtyListCopy = mDirtyList;

		mDirtyList.first = 0;
		mDirtyList.last = 0;

		vector_iterator vi;

		DISPLAY_INT viewWidth  = mViewRect.Width();
		DISPLAY_INT viewHeight = mViewRect.Height();

		PresetWebRect viewCorner(&mViewRect);
		viewCorner.MoveTo(0,0);

		PresetWebRect bufferView(&mViewRect);
		bufferView.MoveTo(mViewRect.left % viewWidth, mViewRect.top % viewHeight);

		DISPLAY_INT bufferLeft = mViewRect.left - (mViewRect.left % viewWidth);
		DISPLAY_INT bufferTop  = mViewRect.top  - (mViewRect.top  % viewHeight);

		dirty = (WebRect*) vector_get_first(&dirtyListCopy, &vi);
		while (dirty)
		{
			if (mViewRect.Overlaps(dirty))
			{
				WebRect bufferDirty;
				bufferDirty.left = dirty->left - bufferLeft;
				bufferDirty.top  = dirty->top  - bufferTop;
				bufferDirty.SizeTo(dirty->Width(), dirty->Height());
				bufferDirty.And(&bufferView);

				mScrollBuffer->SetClip(&bufferDirty);

				DISPLAY_INT x = mRoot.Get()->mRect.left - bufferLeft;
				DISPLAY_INT y = mRoot.Get()->mRect.top  - bufferTop;

				mRoot.Get()->Draw (x, y, &bufferView, mScrollBuffer);

				PresetWebRect defaultClip(0, 0, viewWidth * 2 - 1, viewHeight * 2 - 1);
				mScrollBuffer->SetClip(&defaultClip);

				// copy bufferDirty 3 times, split if necessary
				PresetWebRect northwest (0, 0, viewWidth - 1, viewHeight - 1);
				PresetWebRect northeast (viewWidth, 0, viewWidth * 2 - 1, viewHeight - 1);
				PresetWebRect southeast (viewWidth, viewHeight, viewWidth * 2 - 1, viewHeight * 2 - 1);
				PresetWebRect southwest (0, viewHeight, viewWidth - 1, viewHeight * 2 - 1);

				if (bufferDirty.Overlaps(&northwest))
				{
					PresetWebRect copyFrom (&bufferDirty);
					copyFrom.And(&northwest);

					PresetWebRect copyTo (&copyFrom);

					copyTo.ShiftOver(mViewRect.Width());
					mScrollBuffer->CopyBufferRegion(&copyTo, mScrollBuffer, copyFrom.left, copyFrom.top);

					copyTo.ShiftDown(mViewRect.Height());
					mScrollBuffer->CopyBufferRegion(&copyTo, mScrollBuffer, copyFrom.left, copyFrom.top);

					copyTo.ShiftOver(-mViewRect.Width());
					mScrollBuffer->CopyBufferRegion(&copyTo, mScrollBuffer, copyFrom.left, copyFrom.top);
				}

				if (bufferDirty.Overlaps(&northeast))
				{
					PresetWebRect copyFrom (&bufferDirty);
					copyFrom.And(&northeast);

					PresetWebRect copyTo (&copyFrom);

					copyTo.ShiftDown(mViewRect.Height());
					mScrollBuffer->CopyBufferRegion(&copyTo, mScrollBuffer, copyFrom.left, copyFrom.top);

					copyTo.ShiftOver(-mViewRect.Width());
					mScrollBuffer->CopyBufferRegion(&copyTo, mScrollBuffer, copyFrom.left, copyFrom.top);

					copyTo.ShiftDown(-mViewRect.Height());
					mScrollBuffer->CopyBufferRegion(&copyTo, mScrollBuffer, copyFrom.left, copyFrom.top);
				}

				if (bufferDirty.Overlaps(&southeast))
				{
					PresetWebRect copyFrom (&bufferDirty);
					copyFrom.And(&southeast);

					PresetWebRect copyTo (&copyFrom);

					copyTo.ShiftDown(-mViewRect.Height());
					mScrollBuffer->CopyBufferRegion(&copyTo, mScrollBuffer, copyFrom.left, copyFrom.top);

					copyTo.ShiftOver(-mViewRect.Width());
					mScrollBuffer->CopyBufferRegion(&copyTo, mScrollBuffer, copyFrom.left, copyFrom.top);

					copyTo.ShiftDown(mViewRect.Height());
					mScrollBuffer->CopyBufferRegion(&copyTo, mScrollBuffer, copyFrom.left, copyFrom.top);
				}

				if (bufferDirty.Overlaps(&southwest))
				{
					PresetWebRect copyFrom (&bufferDirty);
					copyFrom.And(&southwest);

					PresetWebRect copyTo (&copyFrom);

					copyTo.ShiftDown(-mViewRect.Height());
					mScrollBuffer->CopyBufferRegion(&copyTo, mScrollBuffer, copyFrom.left, copyFrom.top);

					copyTo.ShiftOver(mViewRect.Width());
					mScrollBuffer->CopyBufferRegion(&copyTo, mScrollBuffer, copyFrom.left, copyFrom.top);

					copyTo.ShiftDown(mViewRect.Height());
					mScrollBuffer->CopyBufferRegion(&copyTo, mScrollBuffer, copyFrom.left, copyFrom.top);
				}
			}

			dirty = (WebRect*) vector_get_next(&vi);
		}

		// this ensures the dirty list is just the same as when we entered
		vector_delete(&mDirtyList);

		if (!this->GetManager())
		{
			// if we're the root manager, we'll need this list in a sec.
			mDirtyList = dirtyListCopy;
		}
		else
		{
			ClearManagerFlag(MANAGER_FLAG_DIRTY);
			vector_delete(&dirtyListCopy);
		}
	}
}

#endif // WEBC_BUFFER_SCROLLING
