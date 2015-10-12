#ifndef __DELEMENT_HPP__
#define __DELEMENT_HPP__

#include "webgraph.hpp"
#include "webc.h"
#include "listener.hpp"
#include "ref.hpp"
#include "webfonts.hpp"
#include "webcinline.h"


typedef WEBC_UINT8 SCROLL_MODE;

#define SCROLL_MODE_NONE    0
#define SCROLL_MODE_HSCROLL 1
#define SCROLL_MODE_VSCROLL 2
#define SCROLL_MODE_BOTH    3
#define SCROLL_MODE_AUTO    4

#define DISPLAYELEMENT_INLINE WEBC_INLINE

typedef WEBC_UINT16 DISPLAY_FLAGS;

#define DISPLAY_FLAG_FOCUS           0x0001 // has keyboard focus
#define DISPLAY_FLAG_DIRTY           0x0002 // needs refresh
#define DISPLAY_FLAG_DISABLED        0x0004 // is disabled (means element does not receive focus, also)
#define DISPLAY_FLAG_DEFAULT         0x0008 // is the default control (as in OK button)
#define DISPLAY_FLAG_ACCEPTS_FOCUS   0x0010 // accepts keyboard focus
#define DISPLAY_FLAG_UPDATE_LIST     0x0020 // set if element is on its manager's update list
#define DISPLAY_FLAG_OPAQUE          0x0040 // element is a completely opaque rectangle - no transparent spots
#define DISPLAY_FLAG_FIXED_POS       0x0080 // set if this element's position is relative to the viewport
#define DISPLAY_FLAG_BOUNDS_VALID    0x0100 // set if bounds rectangle is up to date
#define DISPLAY_FLAG_READONLY        0x0200 // text value can be modified (specific to widget)
#define DISPLAY_FLAG_FIXED_POS_CHILD 0x0400 // set if this element has an descendant

#define WG_CFG_DEFAULT_FRAME_WIDTH   2

#define DISPLAY_MIN_Z_INDEX          (-0x7fffffffL)
#define DISPLAY_MAX_Z_INDEX          (+0x7fffffffL)

enum DisplayOverflow
{
	DISPLAY_OVERFLOW_VISIBLE = 0, // no clipping
	DISPLAY_OVERFLOW_HIDDEN,      // clip to clip rectangle
	DISPLAY_OVERFLOW_SCROLL,      // always display scroll bars
	DISPLAY_OVERFLOW_AUTO         // display scroll bars if needed
};

enum FormatResult
{
	DISPLAY_FORMAT_SUCCESS = 0,
	DISPLAY_FORMAT_NEEDS_VSCROLL,
	DISPLAY_FORMAT_NEEDS_HSCROLL
};

class DisplayManager;

class DisplayElement : public Listener, public Notifier, public Referable
{
protected:
	void                    Unlink             (DisplayElement *pChild);

	ObjectReference<DisplayElement> mManager;
	PresetWebRect           mBounds;
public:
	DISPLAY_FLAGS           mFlags;

	DisplayElement();
	~DisplayElement();
#ifdef WEBC_DEBUG
	void WEBC_DEBUG_ReceiveEvent(DisplayElement *TheReciever, HTMLEvent *TheEvent);
	void WEBC_DEBUG_ReceiveNotify(DisplayElement *TheReciever, Notifier *who, NotifyEvent *what);
#else
#undef WEBC_DEBUG_ReceiveEvent
#define WEBC_DEBUG_ReceiveEvent(x, y)
#undef WEBC_DEBUG_ReceiveNotify
#define WEBC_DEBUG_ReceiveNotify(x, y, z)
#endif
	virtual DISPLAYElementType Type (void) { return DISPLAY_ELEMENT; }

	virtual void            Disable            (void);
	virtual void            Draw               (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC);
	virtual void            DrawThisOnly       (DISPLAY_INT x, DISPLAY_INT y, WebGraphics *pGC);
	virtual void            Enable             (void);
	virtual HTMLEventStatus Event              (HTMLEvent *pEvent);
	virtual DisplayElement* FocusDefault       (void); // who we default to if we don't accept focus
	virtual FormatResult    FormatForViewport  (WebRect* viewportRect, WEBC_BOOL hscrollPresent, WEBC_BOOL vscrollPresent);
	virtual void            GetBounds          (WebRect* rect);
	virtual void            GetClientRect      (WebRect* rect);
	virtual void            GetClipRect        (WebRect* pClipRect);
	virtual void            GetDisplayPosition (DisplayElement *pChild, DISPLAY_INT *x, DISPLAY_INT *y);
	virtual WEBC_BOOL       GetFocusRect       (WebRect* rect) { return WEBC_FALSE; }
	WebGraphics*            GetGraphics        (void);
	DisplayManager*         GetManager         (void);
	virtual DisplayOverflow GetOverflow        (void);
	virtual WEBC_INT32      GetZIndex          (void);
	DISPLAY_INT             Height             (void);
	void                    InvalidateBounds   (void);
	virtual void            InsertAfter        (DisplayElement *pPlace, DisplayElement *pChild);
	virtual void            InsertBefore       (DisplayElement *pPlace, DisplayElement *pChild);
	virtual void            InsertFirst        (DisplayElement *pChild);
	virtual void            InsertLast         (DisplayElement *pChild);
	virtual void            InsertOrdered      (DisplayElement *pChild);
	virtual void            Invalidate         (void);
	virtual void            InvalidateChild    (DisplayElement *pChild, WebRect *pRect);
	virtual void            InvalidateRect     (WebRect *pRect);
	virtual void            Move               (WebRect *pRect);
	virtual void            MoveTo             (DISPLAY_INT x, DISPLAY_INT y);
	virtual void            Notify             (Notifier *who, NotifyEvent *what);
	virtual void            Remove             (DisplayElement* child);
	void                    SetFixedPosition   (WEBC_BOOL isFixed);
	virtual void            SetHeight          (DISPLAY_INT h);
	virtual void            SetManager         (DisplayManager *pManager);
	virtual void            SetWidth           (DISPLAY_INT w);
	virtual void            SizeTo             (DISPLAY_INT w, DISPLAY_INT h);
	virtual void            SortByZIndex       (void);
	virtual short           TabIndex           (void);
	virtual WEBC_BOOL       TrapEventInThis    (DISPLAY_INT x, DISPLAY_INT y);
	virtual DisplayElement* TrapEventPoint     (DISPLAY_INT x, DISPLAY_INT y, DISPLAY_INT viewX, DISPLAY_INT viewY);
	DISPLAY_INT             Width              (void);
#if (INCLUDE_EXPERIMENTAL_DIV_SCROLLBARS)
	virtual DISPLAY_INT             vScrollWidth    (void) {return 0;}
	virtual DISPLAY_INT             hScrollWidth    (void) {return 0;}
	virtual void					*GetHScroll		(void) {return 0;}
	virtual void					*GetVScroll		(void) {return 0;}
	virtual DisplayElement			*GetInlineContentHolder (void) {return 0;}
	virtual WEBC_BOOL				IncludeInFlow	(void) {return WEBC_TRUE;}
#endif
	DISPLAYELEMENT_INLINE DISPLAY_FLAGS GetFlags (void) { return mFlags; }
	DISPLAYELEMENT_INLINE void ClearFlag  (DISPLAY_FLAGS f) { mFlags &= ~f; }
	DISPLAYELEMENT_INLINE void SetFlag    (DISPLAY_FLAGS f) { mFlags |= f; }
	DISPLAYELEMENT_INLINE void ToggleFlag (DISPLAY_FLAGS f) { mFlags ^= f; }

	DisplayElement         *mpFirstChild;
	DisplayElement         *mpLastChild;
	DisplayElement         *mpNext;
	DisplayElement         *mpParent;
	DisplayElement         *mpPrev;
	PresetWebRect           mRect;
	WebRect					mScreenRect; // April2013 - save the screenrect of last known draw

};


#endif // __DELEMENT_HPP__
