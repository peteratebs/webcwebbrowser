#ifndef __DMANAGER_HPP__
#define __DMANAGER_HPP__

#include "webgraph.hpp"
#include "delement.hpp"
#include "webrect.hpp"
#include "listener.hpp"
#include "htmlbrow.hpp"

class WebVScroll;
class WebHScroll;

typedef WEBC_UINT8 MANAGER_FLAGS;

#define MANAGER_FLAG_BORDER          0x01
#define MANAGER_FLAG_DIRTY           0x02
#define MANAGER_FLAG_BUFFER_SCROLL   0x04
#define MANAGER_FLAG_BUFFER_DIRTY    0x08
#define MANAGER_FLAG_TRIED_TO_DRAW   0x10

#if WEBC_DEBUG
 #define DISPLAY_MANAGER_STOP_REFRESH(M)         (M)->DebugStopRefresh(__FILE__, __LINE__)
 #define DISPLAY_MANAGER_RESUME_REFRESH(M)       (M)->DebugResumeRefresh(__FILE__,__LINE__)
#else
 #define DISPLAY_MANAGER_STOP_REFRESH(M)         (M)->StopRefresh()
 #define DISPLAY_MANAGER_RESUME_REFRESH(M)       (M)->ResumeRefresh()
#endif

#define DISPLAY_MANAGER_REFRESH_LOG_SIZE 1000

class DisplayManager : public DisplayElement
{
protected:
	DisplayElement* FindTopElement    (DISPLAY_INT iX, DISPLAY_INT iY);
	void            UpdateViewport    (void);
	void            AddDirtyRect      (WebRect *newRect);
	void            AttachVScroll     (void);
	void            AttachHScroll     (void);
	void            DetachVScroll     (void);
	void            DetachHScroll     (void);
	void            GetRootBounds     (WebRect* bounds);

	virtual WebColor GetBackgroundColor (WebGraphics* gc);
	void CorrectViewportPosition (void);

	WEBC_BOOL       mbClickTimeValid;
	WEBC_BOOL       mViewportChanged;
	long            miClickTime;
	WebGraphics*    mpGC;
	DisplayElement* mpMouseOverElement;
	DisplayElement* mpMouseDownElement;
	WebVScroll*     mpVScroll;
	WebHScroll*     mpHScroll;
	vector          mDirtyList;
	MANAGER_FLAGS   mManagerFlags;
	SCROLL_MODE     mScrollMode;
	WEBC_INT16      mNoRefresh;
	ObjectReference<DisplayElement> mRoot;
	WEBC_BOOL       mbDrawDone;
	ObjectReference<DisplayElement> mCaptureRoot; // used when the root event element is overridden w/ setCapture

#ifdef WEBC_DEBUG
	struct {
		const char* file;
		int         line;
		const char* type;
	} mRefreshLog[DISPLAY_MANAGER_REFRESH_LOG_SIZE];
	int mRefreshLogIndex;
#endif

#ifdef WEBC_BUFFER_SCROLLING
	WebGraphics*    mScrollBuffer;
	WebGraphics*    mViewportBuffer;
	void UpdateScrollBuffer (void);
#endif

 public:
	DisplayManager (void);

	~DisplayManager ();
	// DisplayElement overridden methods
	virtual void            Draw             (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC);
	virtual void            DrawThisOnly     (DISPLAY_INT iX, DISPLAY_INT iY, WebGraphics *pGC);
	virtual HTMLEventStatus Event            (HTMLEvent *pEvent);
	virtual FormatResult    FormatForViewport(WebRect* viewportRect, WEBC_BOOL hscrollPresent, WEBC_BOOL vscrollPresent);
	virtual void            GetClipRect      (WebRect *pClipRect);
	virtual DisplayOverflow GetOverflow      (void);
	virtual void            Invalidate       (void);
	virtual void            Move             (WebRect *pRect);
	virtual void            SetManager       (DisplayManager *pManager);
	virtual void            SetHeight        (DISPLAY_INT h);
	virtual void            SetWidth         (DISPLAY_INT w);
	virtual void            SizeTo           (DISPLAY_INT w, DISPLAY_INT h);
	virtual DISPLAYElementType Type (void) { return DISPLAY_ELEMENT_DISPLAYMANAGER; }

	// Listener interface
	virtual void Notify (Notifier *pWho, NotifyEvent *what);

	// DisplayManager-unique methods
	WEBC_BOOL       ClaimPointer             (DisplayElement *pElem);
	void            ClearManagerFlag         (MANAGER_FLAGS mask);
	HTMLEventStatus DispatchEvent            (HTMLEvent* pEvent);
	void            EnsureRectVisible        (WebRect* rect);
	void            EnsureVisible            (DisplayElement* pElem);
	DisplayElement *GetFocus                 (void);
	void            GetElementRect           (WebRect* rect, DisplayElement* elem);
	WebGraphics    *GetGraphics              (void);
	MANAGER_FLAGS   GetManagerFlags          (void);
	virtual void    GetPaddingWidth          (WebRect* padding);
	int             GetRefreshState          (void);
	DisplayElement *GetRoot                  (void);
	SCROLL_MODE     GetScrollMode            (void);
    void            InvalidateViewport       (void);
	void            InvalidateViewportRegion (WebRect *pDirty);
	WEBC_BOOL       IsVisible                (DisplayElement *pElem);
	WEBC_BOOL       RectVisible              (WebRect *pRect);
	void            Refresh                  (void);
	void            RefreshElement           (DisplayElement *pElem);
	void            ReleasePointer           (DisplayElement *pElem);
	HTMLEventStatus RemoveFocus              (DisplayElement *pFocus);
	void            RemoveElementReferences  (DisplayElement *pElem);
	void            ResumeRefresh            (void);
	HTMLEventStatus SetFocus                 (DisplayElement *pFocus);
	void            SetGraphics              (WebGraphics *pGC);
	void            SetManagerFlag           (MANAGER_FLAGS mask);
	void            SetRoot                  (DisplayElement *pRoot);
	void            SetScrollMode            (SCROLL_MODE m);
	void            SetViewport              (WebRect *pViewRect);
	void            StopRefresh              (void);
	void            ResetDrawDone            (void);
	WEBC_BOOL       WasDrawDone              (void);

	void            SetCapture (DisplayElement *pCaptureRoot); // sets the root element for events (for when JavaScript calls .setCapture)

#ifdef WEBC_DEBUG
	void            DebugStopRefresh         (const char* file, int line);
	void            DebugResumeRefresh       (const char* file, int line);
#endif

	DisplayElement *mpFocus;
	DisplayElement *mpPointerOwner;
	PresetWebRect   mViewRect;
	HTMLBrowser		*mBrowser;
};


#endif // __DMANAGER_HPP__
