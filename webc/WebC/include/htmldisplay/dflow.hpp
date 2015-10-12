#ifndef __DFLOW_HPP__
#define __DFLOW_HPP__

#include "dhelem.hpp"
#include "dcontent.hpp"
#include "css.h"
#include "hcbox.hpp"
#include "zdelem.hpp"

#if (INCLUDE_EXPERIMENTAL_DIV_SCROLLBARS)
#include "wtscroll.hpp"
#endif

class HTMLFlow;
class LineContext;

typedef WEBC_UINT16 FLOWDISPLAY_FLAGS;

#define FD_FLAG_MAXWIDTH_VALID              0x0001
#define FD_FLAG_MINWIDTH_VALID              0x0002
#define FD_FLAG_BGCOLOR_VALID               0x0004
#define FD_FLAG_BGCOLOR_SOLID               0x0008
#define FD_FLAG_PARENT_HEIGHT_AUTO          0x0010
#define FD_FLAG_MAXWIDTH_ESTIMATED          0x0020
#define FD_FLAG_FLOW_CHILD_STYLE_MODIFIED   0x0040
#define FD_FLAG_POS_CHILD_STYLE_MODIFIED    0x0080
#define FD_FLAG_LOCAL_STYLE_MODIFIED        0x0100
#define FD_FLAG_FLOW_FORMAT_IN_PROGRESS     0x0200
#define FD_FLAG_PARENT_STYLE_MODIFIED       0x0400

struct HTMLFlowDisplayFormatContext
{
	ZOrderedDisplayElement baseMarker;
};


class HTMLFlowDisplay : public HTMLElementDisplay<DisplayElement>
{
public:
	HTMLFlowDisplay(HTMLFlow* f);

	virtual ~HTMLFlowDisplay();

	// methods overridden from DisplayElement
	virtual void Draw(DISPLAY_INT x, DISPLAY_INT y, WebRect* viewport, WebGraphics* gc);
	virtual void DrawThisOnly(DISPLAY_INT x, DISPLAY_INT y, WebGraphics* gc);
	virtual void GetBounds (WebRect *rect);
	virtual void GetClientRect(WebRect* rect);
	virtual DisplayOverflow GetOverflow(void);
// HEREHERE	virtual void Notify(Notifier* who, NotifyEvent* what);
	virtual void Move(WebRect* rect);
	virtual void SetHeight(DISPLAY_INT h);
	virtual void Update(void);
	virtual DISPLAYElementType Type (void) { return DISPLAY_ELEMENT_HTMLFLOWDISPLAY; }

	// methods overriden from TextUnit
	virtual DISPLAY_INT GetTopMargin(void);
	virtual DISPLAY_INT GetLeftMargin(void);
	virtual DISPLAY_INT GetBottomMargin(void);
	virtual DISPLAY_INT GetRightMargin(void);
	virtual DISPLAY_INT GetMinWidth(void);
	virtual DISPLAY_INT GetMaxWidth(void);
	virtual DISPLAY_INT GetPixelWidth(void);
	virtual DISPLAY_INT GetPixelHeight(void);
	virtual DISPLAY_INT GetBaseline(void);
	virtual DISPLAY_INT GetParentWidth(void);
	virtual void SetParentWidth(DISPLAY_INT iWidth, DISPLAY_INT iAvail);
	virtual void SetParentHeight(DISPLAY_INT iHeight);
	virtual void Format(void);
	virtual void FormatBlock(MarginContext* margin);
	virtual WEBC_BOOL DisplayNoneIsTrue(void);

	virtual IPositionedFormatContext* FormatPositioned (
			DisplayElement* displayParent,
			IPositionedFormatContext* rootContext,
			IPositionedFormatContext* parentContext,
			FormatContextFactory* childContextFactory
		);

	virtual DISPLAY_INT GetMaxWidthSmallerThan (DISPLAY_INT value);

	void SetFlowElement(HTMLFlow* f);
	virtual WEBC_BOOL NeedsFormat (void);

	CSSBackgroundRepeat     GetBackgroundRepeat (void);
	CSSBackgroundAttachment GetBackgroundAttachment (void);
	void                    GetBackgroundXPos (CSSLength* pos);
	void                    GetBackgroundYPos (CSSLength* pos);
	WEBC_UINT16             GetChildWrap (void);
// HEREHERE
	void SetListener(Listener *l);
// HEREHERE
	void Notify(Notifier * n, NotifyEvent *what);
#if (INCLUDE_EXPERIMENTAL_DIV_SCROLLBARS)
	SCROLL_MODE  GetScrollMode (void);
	void         SetScrollMode (SCROLL_MODE m);
	DISPLAY_INT             vScrollWidth       (void) {return 16;}
	DISPLAY_INT             hScrollWidth       (void) {return 16;}
	void					*GetHScroll			(void) {return (void *)mpHScroll;}
	void					*GetVScroll			(void) {return (void *)mpVScroll;}
	DisplayElement			*GetInlineContentHolder (void) {return &mInlineContentHolder;}
#endif
	void HTMLFlowDisplay::UpdateWidth (DISPLAY_INT CalculatedContentWidth);

protected:
	DISPLAY_INT              miContentHeight;
	DISPLAY_INT              miParentWidth;
	DISPLAY_INT              miParentHeight;
	DISPLAY_INT              miMaxWidth;
	DISPLAY_INT              miMinWidth;
	FLOWDISPLAY_FLAGS        mFlowFlags;
	FlowDisplayContentHolder mInlineContentHolder;
	HTMLContentBox           mBorder;
	WebColor                 mBackgroundColor;
	Listener * mpListener;

#if (INCLUDE_EXPERIMENTAL_DIV_SCROLLBARS)
	WebVScroll *mpVScroll;
	WebHScroll *mpHScroll;
	SCROLL_MODE mScrollMode;
	WEBC_BOOL UpdateScrollBars(DISPLAY_INT widthValue, DISPLAY_INT heightValue,DISPLAY_INT ContentHeightValue,DISPLAY_INT ContentWidthValue);

#endif

	void NotifyListener(NotifierType t, NotifyEventType e);


	// Overridden from HTMLElementDisplay
	virtual WEBC_BOOL SetElementStyle(void);

	// HTMLFlowDisplay protected methods
	void DrawBackground(WebRect *fillRect, WebRect *viewRect, WebGraphics *pGC);
	void DrawBorder(DISPLAY_INT x, DISPLAY_INT y, WebGraphics *gc);
	DISPLAY_INT FindPixelWidth (DISPLAY_INT iParentWidth);
	WEBC_BOOL MinWidthValid (void);
	WEBC_BOOL MaxWidthValid (void);
	DISPLAY_INT FindMinWidth(void);
	DISPLAY_INT FindMaxWidth(DISPLAY_INT bailOutLimit);
	HTMLFlow *FlowElement (void) { return ((HTMLFlow *) mpHtmlElement); }
	WebColor GetBackgroundColor (WebGraphics *pGC);

	FormatContext*            mParentFormatContext;
	IPositionedFormatContext* mPositionedChildFormatContext;

	// returns the formatted content height, NOT including padding or
	//  borders; margin is adjusted to inside the border
	//  and padding widths BEFORE this method is called.
	virtual DISPLAY_INT FormatFlowContent (
			MarginContext* margin,
			DISPLAY_INT leftPad,
			DISPLAY_INT topPad,
			DISPLAY_INT *pcontentWidth
		);

	// returns the formatted content height
	virtual DISPLAY_INT FormatContentPositioned (
			DISPLAY_INT widthValue,
			DISPLAY_INT heightValue,
			WEBC_BOOL   heightIsAuto,
			DISPLAY_INT borderLeftValue,
			DISPLAY_INT borderTopValue,
			DISPLAY_INT borderRightValue,
			DISPLAY_INT borderBottomValue,
			DISPLAY_INT paddingLeftValue,
			DISPLAY_INT paddingTopValue,
			DISPLAY_INT paddingRightValue,
			DISPLAY_INT paddingBottomValue
		);

	virtual void GetSpecifiedHeight (
			DISPLAY_INT* heightValue,
			WEBC_BOOL* boolIsAuto
		);

	virtual void UpdateHeight (
			DISPLAY_INT topPad,
			DISPLAY_INT bottomPad
		);

	void UpdateContentVAlign (void);

	void BeginFormatPositionedChildren (
			HTMLFlowDisplayFormatContext* ctx
		);

	void EndFormatPositionedChildren (
			HTMLFlowDisplayFormatContext* ctx
		);

	void FormatIfNecessary (void);

	virtual void SetLocalStyleModified (void);
	virtual void SetParentStyleModified (void);
	virtual void SetPosChildStyleModified (void);
	virtual void SetFlowChildStyleModified (void);
	virtual void SetMinWidthInvalid (void);
	virtual void SetMaxWidthInvalid (void);

	virtual TextUnitEnumerator *GetTextUnits(void);
	virtual LineContext* CreateLineContext(void);
	virtual WEBC_BOOL IgnoreInFindMinWidth(TextUnit *pTU);
};

#endif // __DFLOW_HPP__
