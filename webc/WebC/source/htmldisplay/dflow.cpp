//
// DFLOW.CPP - Methods for class HTMLFlowDisplay
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

#include "hflow.hpp"
#include "dflow.hpp"
#include "margins.hpp"
#include "tuenum.hpp"
#include "dhframeset.hpp"
#include "zdelem.hpp"
#include "tulist.hpp"
#include "dhelem.cpp"
#include "format.hpp"
#include "dmanager.hpp"
#include "dbgtimer.h"
#include "webcassert.h"
#include "webcmem.h"

#include "webgraph.hpp"

/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/

#define FLOW_ELEMENT   ((HTMLFlow *) mpHtmlElement)

#if (WEBC_SUPPORT_STYLE_SHEETS)

#define TOTAL_LEFT_PAD       (mBorder.GetPaddingWidth(CSS_LEFT)   + mBorder.GetBorderWidth(CSS_LEFT))
#define TOTAL_TOP_PAD        (mBorder.GetPaddingWidth(CSS_TOP)    + mBorder.GetBorderWidth(CSS_TOP))
#define TOTAL_RIGHT_PAD      (mBorder.GetPaddingWidth(CSS_RIGHT)  + mBorder.GetBorderWidth(CSS_RIGHT))
#define TOTAL_BOTTOM_PAD     (mBorder.GetPaddingWidth(CSS_BOTTOM) + mBorder.GetBorderWidth(CSS_BOTTOM))

#else

#define TOTAL_LEFT_PAD       0
#define TOTAL_TOP_PAD        0
#define TOTAL_RIGHT_PAD      0
#define TOTAL_BOTTOM_PAD     0

#endif //(WEBC_SUPPORT_STYLE_SHEETS)

#define TOTAL_HORIZONTAL_PAD (TOTAL_LEFT_PAD + TOTAL_RIGHT_PAD)
#define TOTAL_VERTICAL_PAD   (TOTAL_TOP_PAD + TOTAL_BOTTOM_PAD)


/*****************************************************************************/
// Local Types
/*****************************************************************************/


/*****************************************************************************/
// Global Data
/*****************************************************************************/


/*****************************************************************************/
// Local functions
/*****************************************************************************/

/*===========================================================================*/
// HTMLFlowDisplay constructors/destructor
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
HTMLFlowDisplay::HTMLFlowDisplay(HTMLFlow* f)
	: HTMLElementDisplay<DisplayElement> (f), mBorder(f)
{
	miParentWidth = 0;
	miParentHeight = 0;
	miContentHeight = 0;
	miMaxWidth = 0;
	miMinWidth = 0;
	mFlowFlags = FD_FLAG_LOCAL_STYLE_MODIFIED;
	mParentFormatContext = 0;
	mPositionedChildFormatContext = 0;
	mpListener = 0;	// HEREHERE
	SetFlowElement(f);

#if (INCLUDE_EXPERIMENTAL_DIV_SCROLLBARS)
	SetScrollMode (SCROLL_MODE_AUTO);
	mpVScroll=0;
	mpHScroll=0;
#endif
}

/*---------------------------------------------------------------------------*/
HTMLFlowDisplay::~HTMLFlowDisplay()
{
	if (mParentFormatContext)
	{
		mParentFormatContext->release();
	}

	if (mPositionedChildFormatContext)
	{
		mPositionedChildFormatContext->release();
	}
}

/*---------------------------------------------------------------------------*/
void HTMLFlowDisplay::SetFlowElement(HTMLFlow * f)
{
	SetHtmlElement(f);
}


/*===========================================================================*/
// Listener methods
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
void HTMLFlowDisplay::Notify(Notifier *who, NotifyEvent *what)
{
	if (what)
	{
        WEBC_DEBUG_ReceiveNotify(this, who, what);
		switch (what->type)
		{
#if (INCLUDE_EXPERIMENTAL_DIV_SCROLLBARS)
		case  NOTIFIER_TYPE_SLIDER:
				switch (what->event)
				{
					case NOTIFY_SLIDER_MOVE:
						Invalidate();
						break;
					default:
						break;
				}
				break;
#endif
		/* Used by SetBackgroundImage for objects plus
		   NotifyDisplay() is called with this argument when
		   HTMLElementContainer::NotifyChildRemoved()
		   et al are called */
			case NOTIFIER_TYPE_HTML_ELEMENT:
				switch (what->event)
				{
					case NOTIFY_CONTENT_ADDED:        // HTMLElementContainer::InsertFirst et al
						// intentional fall-through
					case NOTIFY_CONTENT_REMOVED:	  // HTMLElementContainer::NotifyChildRemoved only
						SetFlowChildStyleModified();  // NOTIFY_FLOW_CHILD_STYLE_CHANGE
													  // Controls FormatPositioned
						SetPosChildStyleModified();	  // NOTIFY_POSITIONED_CHILD_STYLE_CHANGE
													  // Controls FormatPositioned
						SetMinWidthInvalid();
						SetMaxWidthInvalid();
						break;

					case NOTIFY_PARENT_STYLE_CHANGE: // Only used by jhelem set style (see: gTinyIdToChildEventType)
						// intentional fall-through
					case NOTIFY_FORMAT_STYLE_CHANGE: // Not used
						// intentional fall-through
					case NOTIFY_STYLE_CHANGE:		 // Used everywhere when an htnl element is changed
						SetLocalStyleModified();	 // Sets mFlowFlags & FD_FLAG_LOCAL_STYLE_MODIFIED
													 // Invalidates UnitType and Position
													 // propogates NOTIFY_STYLE_CHANGE upward through listener chains
						break;

					default:
						break;
				}
				break;
         /* Only used in NotifyDisplay() if the element it's notifying does not
            have a display element and the nearest parent with a display element
            is caaled and notified */
			case NOTIFIER_TYPE_NODISPLAY_HTML_CHILD:
			{
				HTMLElement* childHtmlElement = (HTMLElement*) who;

				switch (what->event)
				{
					case NOTIFY_STYLE_CHANGE:
						if (childHtmlElement->GetUnitType() != TU_TYPE_INLINE &&
						    childHtmlElement->GetUnitType() != TU_TYPE_NODISPLAY)
						{
							switch (childHtmlElement->GetPosition())
							{
								case TU_POSITION_STATIC:			 // Place element automatically
									SetFlowChildStyleModified();
									break;

								case TU_POSITION_FIXED: 			// Positioned relative to the browser window. Won;t move even if scrolled
									// intentional fall-through
								case TU_POSITION_ABSOLUTE:			// Positioned relative to the first parent element that has a position other than static. If no such element is found, the containing block is <html>:
									SetPosChildStyleModified();
									break;

								case TU_POSITION_RELATIVE:			// Positioned relative to its normal position.
									SetFlowChildStyleModified();
									SetPosChildStyleModified();
									break;
							}
						}
						break;
						// intentional fall-through
					case NOTIFY_PARENT_STYLE_CHANGE:
						if (childHtmlElement->GetUnitType() != TU_TYPE_INLINE)
						{
							SetLocalStyleModified();
							SetMinWidthInvalid();
							SetMaxWidthInvalid();
						}
						break;

					default:
						break;
				}
				break;
			}
			/* Set in HTMLFlowDisplay::Setxxx() methods and dhelem templates
			   for display element changes */
			case NOTIFIER_TYPE_TEXT_UNIT:
			{
				TextUnit* tu = (TextUnit*) who;

				switch (what->event)
				{
					case NOTIFY_MAX_WIDTH_CHANGE: 				// Set when width of a display element changes
						SetMaxWidthInvalid();
						break;

					case NOTIFY_MIN_WIDTH_CHANGE:				// Set when width of a display element changes
						SetMinWidthInvalid();
						break;

					case NOTIFY_FLOW_CHILD_STYLE_CHANGE:       // Other states above call SetFlowChildStyleModified().
						// intentional fall-through
					case NOTIFY_POSITIONED_CHILD_STYLE_CHANGE: // Other states above call SetPosChildStyleModified
															   // Also called by FormatForViewport
						// intentional fall-through
					case NOTIFY_SIZE_CHANGE:				   // Display element Move, resizes et al
						if (tu->GetUnitType() != TU_TYPE_NODISPLAY)
						{
							switch (tu->GetPosition())
							{
								case TU_POSITION_STATIC:
									SetFlowChildStyleModified();
									SetMinWidthInvalid();
									SetMaxWidthInvalid();
									break;

								case TU_POSITION_ABSOLUTE:
								case TU_POSITION_FIXED:
									SetPosChildStyleModified();
									break;

								case TU_POSITION_RELATIVE:
									SetFlowChildStyleModified();
									SetPosChildStyleModified();
									SetMinWidthInvalid();
									SetMaxWidthInvalid();
									break;
							}
						}
						break;

					case NOTIFY_FORMAT_STYLE_CHANGE: 	 // Not used
						// intentional fall-through
					case NOTIFY_STYLE_CHANGE:
						if (!(what->data.oldStyle.display == TU_TYPE_NODISPLAY &&
						      tu->GetUnitType() == TU_TYPE_NODISPLAY))
						{
							switch (what->data.oldStyle.position)
							{
								case TU_POSITION_STATIC:
									SetFlowChildStyleModified();
									break;

								case TU_POSITION_FIXED: // intentional fall-through
								case TU_POSITION_ABSOLUTE:
									SetPosChildStyleModified();
									break;

								case TU_POSITION_RELATIVE:
									SetFlowChildStyleModified();
									SetPosChildStyleModified();
									break;
							}

							switch (tu->GetPosition())
							{
								case TU_POSITION_STATIC:
									SetFlowChildStyleModified();
									break;

								case TU_POSITION_FIXED: // intentional fall-through
								case TU_POSITION_ABSOLUTE:
									SetPosChildStyleModified();
									break;

								case TU_POSITION_RELATIVE:
									SetFlowChildStyleModified();
									SetPosChildStyleModified();
									break;
							}
						}
						break;
				}
				break;
			}

			default:
				break;
		}
	}

	HTMLElementDisplay<DisplayElement>::Notify(who, what);
}


/*===========================================================================*/
// TextUnit methods
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
DISPLAY_INT HTMLFlowDisplay::GetTopMargin(void)
{
	return mBorder.GetMarginWidth(CSS_TOP);
}

/*---------------------------------------------------------------------------*/
DISPLAY_INT HTMLFlowDisplay::GetLeftMargin(void)
{
	DISPLAY_INT marginLeftValue;
	DISPLAY_INT marginRightValue;

	mBorder.GetHorizontalMargins(&marginLeftValue, &marginRightValue, Width());

	return (marginLeftValue);
}

/*---------------------------------------------------------------------------*/
DISPLAY_INT HTMLFlowDisplay::GetBottomMargin(void)
{
	return mBorder.GetMarginWidth(CSS_BOTTOM);
}

/*---------------------------------------------------------------------------*/
DISPLAY_INT HTMLFlowDisplay::GetRightMargin(void)
{
	DISPLAY_INT marginLeftValue;
	DISPLAY_INT marginRightValue;

	mBorder.GetHorizontalMargins(&marginLeftValue, &marginRightValue, Width());

	return (marginRightValue);
}

/*---------------------------------------------------------------------------*/
DISPLAY_INT HTMLFlowDisplay::GetMinWidth(void)
{
	if (!MinWidthValid())
	{
		miMinWidth = FindMinWidth();
		mFlowFlags |= FD_FLAG_MINWIDTH_VALID;
	}

	return (miMinWidth);
}

/*---------------------------------------------------------------------------*/
DISPLAY_INT HTMLFlowDisplay::GetMaxWidth(void)
{
	return GetMaxWidthSmallerThan(DISPLAY_INT_MAX);
}

/*---------------------------------------------------------------------------*/
DISPLAY_INT HTMLFlowDisplay::GetMaxWidthSmallerThan(DISPLAY_INT limit)
{
	if ((mFlowFlags & FD_FLAG_MAXWIDTH_VALID) &&
	    (mFlowFlags & FD_FLAG_MAXWIDTH_ESTIMATED) &&
	    (miMaxWidth > limit))
	{
		return (miMaxWidth);
	}

	if (!(mFlowFlags & FD_FLAG_MAXWIDTH_VALID) ||
	     (mFlowFlags & FD_FLAG_MAXWIDTH_ESTIMATED))
	{
		CSSLength width;

		GetCSSLength(&width, CSS_PROPERTY_WIDTH);

		if (!CSS_LengthIsRelative(&width))
		{
			DISPLAY_INT minWidth = GetMinWidth();
			DISPLAY_INT widthValue = CSS_LengthToPixels(&width, 0, 0, 0);

			miMaxWidth = EBSMAX(minWidth, widthValue);
			mFlowFlags &= ~FD_FLAG_MAXWIDTH_ESTIMATED;
		}
		else
		{
			miMaxWidth = FindMaxWidth(limit);

			if (miMaxWidth > limit)
			{
				mFlowFlags |= FD_FLAG_MAXWIDTH_ESTIMATED;
			}
			else
			{
				mFlowFlags &= ~FD_FLAG_MAXWIDTH_ESTIMATED;
			}
		}

		mFlowFlags |= FD_FLAG_MAXWIDTH_VALID;
	}

	return (miMaxWidth);
}

/*---------------------------------------------------------------------------*/
DISPLAY_INT HTMLFlowDisplay::GetBaseline(void)
{
	return (GetPixelHeight());
}

/*---------------------------------------------------------------------------*/
DISPLAY_INT HTMLFlowDisplay::GetPixelWidth(void)
{
	if ( (GetUnitType() != TU_TYPE_BLOCK) &&  (GetUnitType() != TU_TYPE_INLINE_BLOCK))
	{
		FormatIfNecessary();
	}
	return (Width());
}

/*---------------------------------------------------------------------------*/
DISPLAY_INT HTMLFlowDisplay::GetPixelHeight(void)
{
	if ( (GetUnitType() != TU_TYPE_BLOCK) &&  (GetUnitType() != TU_TYPE_INLINE_BLOCK))
	{
		FormatIfNecessary();
	}
	return (Height());
}

/*---------------------------------------------------------------------------*/
void HTMLFlowDisplay::SetParentWidth(DISPLAY_INT ignored, DISPLAY_INT newParentWidth)
{
	DISPLAY_INT oldParentWidth = miParentWidth;

	miParentWidth = newParentWidth;
	mBorder.SetParentWidth(newParentWidth);
printf("SetParentWidth >>>>>>>>>>>>>>>>>>>>>>>>\n");
	DISPLAY_INT minWidth = GetMinWidth();
printf("SetParentWidth <<<<<<<<<<<<<<<<<<<<<<<\n");
	DISPLAY_INT pixelWidth = FindPixelWidth(newParentWidth);
	DISPLAY_INT newWidth = EBSMAX(minWidth, pixelWidth);

	if (newWidth != Width() ||
	    (miParentWidth != oldParentWidth && mBorder.HasPercent()))
	{
		SetWidth(newWidth);
		SetParentStyleModified();
	}
}

/*---------------------------------------------------------------------------*/
void HTMLFlowDisplay::SetParentHeight(DISPLAY_INT iHeight)
{
	miParentHeight = iHeight;

	// set this element's height
	DISPLAY_INT topPad = TOTAL_TOP_PAD;
	DISPLAY_INT bottomPad = TOTAL_BOTTOM_PAD;

	UpdateHeight(topPad, bottomPad);
}
// =================
#if (INCLUDE_EXPERIMENTAL_DIV_SCROLLBARS)
SCROLL_MODE HTMLFlowDisplay::GetScrollMode (void)
{
#if (TEXT_BOX_SCROLL_DISABLED)
	return(SCROLL_MODE_NONE);
#else
	return (mScrollMode);
#endif
}

void HTMLFlowDisplay::SetScrollMode (SCROLL_MODE m)
{
	mScrollMode = m;
}

WEBC_BOOL HTMLFlowDisplay::UpdateScrollBars(DISPLAY_INT widthValue,	DISPLAY_INT heightValue, DISPLAY_INT ContentHeightValue, DISPLAY_INT ContentWidthValue)
{
	WebRect box;
	WEBC_BOOL done = WEBC_FALSE;
	WebGraphics *gc = GetGraphics();


	int i = 0;

	if (!gc)
	{
		return WEBC_FALSE;
	}
	DISPLAY_INT totalHeight = ContentHeightValue;
	DISPLAY_INT MaxWidth = ContentWidthValue;
	box.Set(0,0,widthValue,heightValue);
	do
	{
		done = WEBC_TRUE;
		SCROLL_MODE ScrollMode;
		ScrollMode = GetScrollMode();
		// Add/Remove scroll bars
		if (((totalHeight > box.Height()) && (ScrollMode & SCROLL_MODE_AUTO)) ||
		    (ScrollMode & SCROLL_MODE_VSCROLL))
		{
			if (!mpVScroll)
			{
				WEBC_NEW_VERBOSE(mpVScroll, WebVScroll(),"VerticalSrollbar");
				if (mpVScroll)
				{
				Listener *l = reinterpret_cast<Listener *>(this);
					InsertLast(mpVScroll);
					mpVScroll->SetListener(l);
					mpVScroll->SetWidth(webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_WIDTH));
					mpVScroll->SetHeight(heightValue);
					mpVScroll->SetPosition(0);
					long Step = ContentHeightValue/10;
					if (Step > heightValue)
						Step=heightValue;
					mpVScroll->SetStep(Step);
					if (mFlags & DISPLAY_FLAG_DISABLED)
						mpVScroll->Disable();
				}
				return WEBC_TRUE;
			}
		}
		else
		{
			if (mpVScroll) // and we don't need it
			{
				Remove(mpVScroll);
				WEBC_DELETE(mpVScroll);
				mpVScroll = 0;
				return WEBC_TRUE;
			}
		}
		// Test to see if we need a horizontal scroll
		if (((MaxWidth > box.Width()) && (ScrollMode & SCROLL_MODE_AUTO)) ||
		    (ScrollMode & SCROLL_MODE_HSCROLL))
		{
			if (!mpHScroll)
			{
				WEBC_NEW_VERBOSE(mpHScroll, WebHScroll(),"HorizontalalSrollbar");
				if (mpHScroll)
				{
				Listener *l = reinterpret_cast<Listener *>(this);
					InsertLast(mpHScroll);
					mpHScroll->SetListener(l);
					mpHScroll->SetHeight(webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_WIDTH));
					mpHScroll->SetPosition(0);
					long Step = ContentWidthValue/10;
					if (Step > widthValue)
						Step=widthValue;
					mpHScroll->SetStep(Step);
					if (mFlags & DISPLAY_FLAG_DISABLED)
						mpHScroll->Disable();
					done = WEBC_FALSE;
				}
			}
		}
		else
		{
			if (mpHScroll)
			{
				Remove(mpHScroll);
				WEBC_DELETE(mpHScroll);
				mpHScroll = 0;
				done = WEBC_FALSE;
			}
		}

		i++;
	} while (!done && (i<4));
	int xOff = mBorder.GetBorderWidth(CSS_LEFT);
	int yOff = mBorder.GetBorderWidth(CSS_TOP);
	if (mpVScroll)
	{
		// To do - If box width < slider width ??
		int hOff = mpHScroll ? webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_WIDTH): 0; // Reduce the height if there is a horizontal bar
		PresetWebRect VRect (
				box.right+xOff+1-webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_WIDTH),
				box.top+yOff,
				box.right+xOff+1,
				box.bottom+yOff-hOff);

			mpVScroll->Move(&VRect);
			mpVScroll->SetRange(totalHeight);
			mpVScroll->SetWindow(box.Height());
	}

	if (mpHScroll)
	{
		int wOff = mpVScroll ? webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_WIDTH): 0; // Reduce the width if there is a vertical bar
		PresetWebRect HRect (
				box.left+xOff,
				box.bottom+yOff+1-webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_WIDTH),
				box.right+xOff-wOff,
				box.bottom+yOff+1);

		mpHScroll->Move(&HRect);
		mpHScroll->SetRange(MaxWidth);
		mpHScroll->SetWindow(box.Width());
	}

	return (WEBC_FALSE);
}
#endif
// =================
/*---------------------------------------------------------------------------*/
void HTMLFlowDisplay::Format(void)
{
	DISPLAY_INT topPad = TOTAL_TOP_PAD;
	DISPLAY_INT bottomPad = TOTAL_BOTTOM_PAD;
	DISPLAY_INT leftPad = TOTAL_LEFT_PAD;
	DISPLAY_INT rightPad = TOTAL_RIGHT_PAD;
	DISPLAY_INT iContentWidth = 0;
	// Width()'s return value is the width of mRect; this is set inside
	//  SetParentWidth
	MarginContext margin(leftPad, Width() - rightPad - 1, this);

	// Format the flow content
	miContentHeight = FormatFlowContent(&margin, leftPad, topPad,&iContentWidth);

	// update miContentHeight to include anything in the margins
	DISPLAY_INT clearedHeight = margin.ClearAll(topPad + miContentHeight) - topPad;
	if (clearedHeight > miContentHeight)
	{
		miContentHeight = clearedHeight;
	}

	// set this element's height
	UpdateHeight(topPad, bottomPad);
	UpdateWidth(iContentWidth);

	// clear all flags that necessitate re-formatting
	mFlowFlags &= ~(FD_FLAG_FLOW_CHILD_STYLE_MODIFIED |
	                FD_FLAG_LOCAL_STYLE_MODIFIED |
	                FD_FLAG_PARENT_STYLE_MODIFIED);

}

/*---------------------------------------------------------------------------*/
void HTMLFlowDisplay::FormatBlock(MarginContext *margin)
{
	DISPLAY_INT topPad = TOTAL_TOP_PAD;
	DISPLAY_INT bottomPad = TOTAL_BOTTOM_PAD;
	DISPLAY_INT leftPad = TOTAL_LEFT_PAD;
	DISPLAY_INT rightPad = TOTAL_RIGHT_PAD;
	DISPLAY_INT iContentWidth = 0;

	HTMLFlowDisplayFormatContext formatCtx;
	// Format the flow content
	margin->AddPadding(leftPad, rightPad);
printf("FormatBlock >>>>>>>>>>>>>>>>>>>>>>>>\n");
	miContentHeight = FormatFlowContent(margin, leftPad, topPad,&iContentWidth);
printf("FormatBlock <<<<<<<<<<<<<<<<<<<<<<<<\n");
	margin->RemovePadding(leftPad, rightPad);

	// set this element's height
	UpdateHeight(topPad, bottomPad);
	UpdateWidth(iContentWidth);
	mFlowFlags &= ~(FD_FLAG_FLOW_CHILD_STYLE_MODIFIED|
	                FD_FLAG_LOCAL_STYLE_MODIFIED|
	                FD_FLAG_PARENT_STYLE_MODIFIED);
}

/*---------------------------------------------------------------------------*/
IPositionedFormatContext* HTMLFlowDisplay::FormatPositioned (
		DisplayElement* displayParent,
		IPositionedFormatContext* rootContext,
		IPositionedFormatContext* parentContext,
		FormatContextFactory* childContextFactory
	)
{
	HTMLFlowDisplayFormatContext formatCtx;
	IPositionedFormatContext* containingContext = (GetPosition() == TU_POSITION_FIXED)? rootContext : parentContext;
	IPositionedFormatContext* childContext = 0;

	if ((mFlowFlags & (FD_FLAG_FLOW_CHILD_STYLE_MODIFIED|FD_FLAG_LOCAL_STYLE_MODIFIED)) ||
	    !mParentFormatContext ||
	    !mParentFormatContext->equals(containingContext))
	{
		BeginFormatPositionedChildren(&formatCtx);

		childContext = HTMLElementDisplay<DisplayElement>::FormatPositioned (
				displayParent,
				rootContext,
				parentContext,
				childContextFactory
			);

		EndFormatPositionedChildren(&formatCtx);

		if (childContext && childContext != mPositionedChildFormatContext)
		{
			if (mPositionedChildFormatContext)
			{
				mPositionedChildFormatContext->release();
			}

			mPositionedChildFormatContext = childContext;
			mPositionedChildFormatContext->claim();
		}

		if (mParentFormatContext)
		{
			mParentFormatContext->release();
		}

		mParentFormatContext = containingContext;
		mParentFormatContext->claim();
	}
	else if (mFlowFlags & FD_FLAG_POS_CHILD_STYLE_MODIFIED)
	{
		if (GetPosition() != TU_POSITION_RELATIVE)
		{
			if (displayParent)
			{
				displayParent->InsertOrdered(this);
			}
		}

		childContext = mPositionedChildFormatContext;
		if (childContext)
		{
			childContext->claim();

			BeginFormatPositionedChildren(&formatCtx);
			FormatPositionedChildren(childContext, rootContext, childContextFactory);
			EndFormatPositionedChildren(&formatCtx);
		}
	}
	else
	{
		if (GetPosition() != TU_POSITION_RELATIVE)
		{
			if (displayParent)
			{
				displayParent->InsertOrdered(this);
			}
		}
	}
	mFlowFlags &= ~(FD_FLAG_POS_CHILD_STYLE_MODIFIED |
	                FD_FLAG_FLOW_CHILD_STYLE_MODIFIED |
	                FD_FLAG_LOCAL_STYLE_MODIFIED |
	                FD_FLAG_PARENT_STYLE_MODIFIED);
	return (childContext);
}

/*---------------------------------------------------------------------------*/
// returns the formatted content height
DISPLAY_INT HTMLFlowDisplay::FormatContentPositioned (
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
	)
{
	DISPLAY_INT leftPad = borderLeftValue + paddingLeftValue;
	DISPLAY_INT topPad = paddingTopValue + borderTopValue;
	DISPLAY_INT iContentWidth = 0;
	MarginContext margin (leftPad, leftPad + widthValue - 1, this);
	miContentHeight = FormatFlowContent(&margin, leftPad, topPad,&iContentWidth);


	DISPLAY_INT clearedHeight = margin.ClearAll(topPad + miContentHeight) - topPad;
	if (clearedHeight > miContentHeight)
	{
		miContentHeight = clearedHeight;
	}
	return (miContentHeight);
}


/*===========================================================================*/
// HTMLTextUnitMirror methods
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
DISPLAY_INT HTMLFlowDisplay::GetParentWidth(void)
{
	return (miParentWidth);
}


/*===========================================================================*/
// DisplayElement methods
/*===========================================================================*/

void HTMLFlowDisplay::SetListener (Listener *listener)	{ // HEREHERE
	mpListener = listener;
}
void HTMLFlowDisplay::NotifyListener (NotifierType t, NotifyEventType e)	{ // HEREHERE
	if (mpListener)
	{
		NotifyEvent what(t, e);
		mpListener->Notify((DisplayElement *)this, &what);
	}
}

/*---------------------------------------------------------------------------*/

void HTMLFlowDisplay::Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC)
{
void *oldctxt = 0;
DisplayElement *d = (DisplayElement *) this;
CSSPropertyValue opacityValue;
int opacity = 255;
	FormatIfNecessary();
	NotifyListener(NOTIFIER_TYPE_TEXT_UNIT, NOTIFY_RENDER_DRAW_START);

	if (mpHtmlElement->GetStyleFromCSS(CSS_PROPERTY_OPACITY, &opacityValue) == CSS_VALUE_SPECIFIED)
	{
		unsigned long ocalc;
		WebRect screenRect(mRect);
		screenRect.MoveTo(x,y);

		// Convert frac to a value from 0 to 255.. used for the alpha channel value
		// Frac is scaled to 0 to 10000 is 0 to 1. so (frac*255)/10000; is scaled to 255
		ocalc = (opacityValue.length.val.frac * 255);
		ocalc /= 10000;
		opacity = (int) ocalc;
		oldctxt = d->GetGraphics()->PushCanvas(&screenRect);
	}
	DisplayElement::Draw(x, y, pViewport, pGC);
	if (oldctxt)
	{
		d->GetGraphics()->PopCanvas(oldctxt, opacity);
	}
	NotifyListener(NOTIFIER_TYPE_TEXT_UNIT, NOTIFY_RENDER_DRAW_DONE);
}

/*---------------------------------------------------------------------------*/
void HTMLFlowDisplay::DrawThisOnly(DISPLAY_INT x, DISPLAY_INT y, WebGraphics * gc)
{
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLFLOWDISPLAY_DRAWTHISONLY);

	WEBC_ASSERT(FLOW_ELEMENT);

	if (GetVisibility() == TU_VISIBILITY_HIDE)
	{
		WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLFLOWDISPLAY_DRAWTHISONLY);
		return;
	}

	// commented out to optimize...
	//if (FLOW_ELEMENT->GetUnitType() != TU_TYPE_INLINE)
	// ...take this property from this instead, since it will be cached
	//WEBC_ASSERT(this->GetUnitType() != TU_TYPE_INLINE);
	if (this->GetUnitType() != TU_TYPE_INLINE) // tbd - is this check really necessary?
	{
		WebRect screenRect(mRect);
		DISPLAY_INT border[4];


		border[CSS_TOP] = mBorder.GetBorderWidth(CSS_TOP);
		border[CSS_RIGHT] = mBorder.GetBorderWidth(CSS_RIGHT);
		border[CSS_LEFT] = mBorder.GetBorderWidth(CSS_LEFT);
		border[CSS_BOTTOM] = mBorder.GetBorderWidth(CSS_BOTTOM);

		screenRect.MoveTo(x+border[CSS_RIGHT],y+border[CSS_TOP]);
		screenRect.SetWidth(screenRect.Width()-border[CSS_LEFT]-border[CSS_RIGHT]);
		screenRect.SetHeight(screenRect.Height()-border[CSS_TOP]-border[CSS_BOTTOM]);
		{
			DrawBackground(&screenRect, &screenRect, gc);
	  #if (WEBC_SUPPORT_STYLE_SHEETS)
			DrawBorder(x, y, gc);
	  #endif // (WEBC_SUPPORT_STYLE_SHEETS)
		}
	}
	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLFLOWDISPLAY_DRAWTHISONLY);
}
void HTMLFlowDisplay::GetBounds (WebRect *rect)
{
	CSSPropertyValue overflowValue;
	// If overflow: hidden; and the size is set, use the assigned size. Otherwise descend
	// though the children.

	if (mpHtmlElement->GetStyleFromCSS(CSS_PROPERTY_OVERFLOW, &overflowValue) == CSS_VALUE_SPECIFIED)
	{
		if (CSS_OVERFLOW_HIDDEN == overflowValue.overflow)
		{
			rect->Set(&mRect);
			return;
		}
	}
	else if (mpHtmlElement->GetStyleFromCSS(CSS_PROPERTY_OVERFLOW_X, &overflowValue) == CSS_VALUE_SPECIFIED)
	{
		if (CSS_OVERFLOW_HIDDEN == overflowValue.overflow)
		{
			DisplayElement::GetBounds(rect);
			rect->left = mRect.left;
			rect->right = mRect.right;
			return;
		}
	}
	else if (mpHtmlElement->GetStyleFromCSS(CSS_PROPERTY_OVERFLOW_Y, &overflowValue) == CSS_VALUE_SPECIFIED)
	{
		if (CSS_OVERFLOW_HIDDEN == overflowValue.overflow)
		{
			DisplayElement::GetBounds(rect);
			rect->top = mRect.top;
			rect->bottom = mRect.bottom;
			return;
		}
	}
	else
	{
		DisplayElement::GetBounds(rect);
	}
}
/*---------------------------------------------------------------------------*/
void HTMLFlowDisplay::GetClientRect (WebRect *rect)
{
	*rect = mRect;
}

/*---------------------------------------------------------------------------*/
DisplayOverflow HTMLFlowDisplay::GetOverflow(void)
{
	return (DisplayElement::GetOverflow());
}

/*---------------------------------------------------------------------------*/
void HTMLFlowDisplay::Move(WebRect* rect)
{
	HTMLElementDisplay<DisplayElement>::Move(rect);
	UpdateContentVAlign();
}

/*---------------------------------------------------------------------------*/
void HTMLFlowDisplay::SetHeight(DISPLAY_INT h)
{
	HTMLElementDisplay<DisplayElement>::SetHeight(h);
	UpdateContentVAlign();
}

/*---------------------------------------------------------------------------*/
void HTMLFlowDisplay::Update (void)
{
	FormatIfNecessary();
}


/*===========================================================================*/
// HTMLElementDisplay methods
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
WEBC_BOOL HTMLFlowDisplay::SetElementStyle(void)
{
	mFlowFlags &= ~FD_FLAG_BGCOLOR_VALID;
	return (mBorder.ResetProperties());
}


/*===========================================================================*/
// Helper methods; these are where the real work is (mostly) done
/*===========================================================================*/

/*---------------------------------------------------------------------------*/
void HTMLFlowDisplay::BeginFormatPositionedChildren (
		HTMLFlowDisplayFormatContext* ctx
	)
{
	// note: the order here is VERY important (also note specific use of InsertLast,
	//  as opposed to InsertOrdered)
	ctx->baseMarker.SetZIndex(DISPLAY_MIN_Z_INDEX);

	InsertLast(&ctx->baseMarker);
	InsertLast(&mInlineContentHolder);
}

/*---------------------------------------------------------------------------*/
void HTMLFlowDisplay::DrawBackground(WebRect *fillRect, WebRect *viewRect, WebGraphics *pGC)
{
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLFLOWDISPLAY_DRAWBACKGROUND);

	// Draw the background color
	WebColor Color = GetBackgroundColor(pGC);


	if (mFlowFlags & FD_FLAG_BGCOLOR_SOLID)
	{
		pGC->Rectangle(viewRect, Color, Color, WEBC_TRUE);
	}

  #if (WEBC_SUPPORT_BACKGROUND_IMAGES)

	WebcImage * image = FLOW_ELEMENT->GetBackgroundImage();
	if (image && (image->State() == WEBIMAGE_OKAY))
	{
		WebRect imageRect;
		CSSLength xPos, yPos;
		DISPLAY_INT offX, offY;

		GetBackgroundXPos(&xPos);
		GetBackgroundYPos(&yPos);

		WebFont font = mpHtmlElement->GetWebFont();
		DISPLAY_INT emHeight = (font)? WEB_FONT_HEIGHT(font) : WEBC_CFG_DEFAULT_TEXT_HEIGHT;
		DISPLAY_INT exHeight = emHeight / 2;

		offX = CSS_LengthToPixels(&xPos, Width(), emHeight, exHeight);
		if (xPos.type == CSS_LENGTH_UNIT_PERCENT)
		{
			offX -= CSS_LengthToPixels(&xPos, image->Width(), 0, 0);
		}

		offY = CSS_LengthToPixels(&yPos, Height(), emHeight, exHeight);
		if (yPos.type == CSS_LENGTH_UNIT_PERCENT)
		{
			offY -= CSS_LengthToPixels(&yPos, image->Height(), 0, 0);
		}

		if (GetBackgroundAttachment() == CSS_BACKGROUND_ATTACHMENT_FIXED)
		{
			offX += viewRect->left - fillRect->left;
			offY += viewRect->top - fillRect->top;
		}

		// if repeat isn't set, the box needs to be clipped to the image's width or height

		switch (GetBackgroundRepeat())
		{
			case CSS_BACKGROUND_REPEAT_NO_REPEAT:
				imageRect.left   = fillRect->left + offX;
				imageRect.top    = fillRect->top + offY;
				imageRect.right  = imageRect.left + image->Width() - 1;
				imageRect.bottom = imageRect.top + image->Height() - 1;
				offX = 0;
				offY = 0;
				break;

			case CSS_BACKGROUND_REPEAT_REPEAT_X:
				imageRect.Set(fillRect->left, fillRect->top + offY, fillRect->right, fillRect->top + offY + image->Height() - 1);
				offY = 0;
				break;

			case CSS_BACKGROUND_REPEAT_REPEAT_Y:
				imageRect.Set(fillRect->left + offX, fillRect->top, fillRect->left + offX + image->Width() - 1, fillRect->bottom);
				offX = 0;
				break;

			case CSS_BACKGROUND_REPEAT_REPEAT: // intentional fall-through
			default:
				imageRect.Set(fillRect);
				break;
		}

		// Draw the image fill
		WebcImageFrame *f = image->DrawTiled(0, &imageRect, offX, offY, pGC);
		if (f)
		{
			// tbd - implement animated background images here
			image->ReleaseFrame(f);
		}
	}

  #endif // WEBC_SUPPORT_BACKGROUND_IMAGES

	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLFLOWDISPLAY_DRAWBACKGROUND);
}

/*---------------------------------------------------------------------------*/
void HTMLFlowDisplay::DrawBorder(DISPLAY_INT iX, DISPLAY_INT iY, WebGraphics *gc)
{
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLFLOWDISPLAY_DRAWBORDER);

	WebRect box(mRect);
	box.MoveTo(iX, iY);
	mBorder.DrawBorder(&box, gc);

	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLFLOWDISPLAY_DRAWBORDER);
}

/*---------------------------------------------------------------------------*/
void HTMLFlowDisplay::EndFormatPositionedChildren (
		HTMLFlowDisplayFormatContext* ctx
	)
{
	while (mpFirstChild != &ctx->baseMarker)
	{
		WEBC_ASSERT(mpFirstChild != &mInlineContentHolder);
		Remove(mpFirstChild);
	}

	Remove(&ctx->baseMarker);
}

/*---------------------------------------------------------------------------*/
DISPLAY_INT HTMLFlowDisplay::FindMaxWidth (DISPLAY_INT bailOutLimit)
{
	int iUnits = 0;
	DISPLAY_INT maxWidth = 0;
	DISPLAY_INT lineWidth = 0;
	DISPLAY_INT iFloatLeft = 0;
	DISPLAY_INT iFloatRight = 0;
	DISPLAY_INT iNextLineFloatLeft = 0;
	DISPLAY_INT iNextLineFloatRight = 0;
	DISPLAY_INT tuMaxWidth;
	TextUnit *tu;
	WEBC_UINT16 tuBreak, tuType, tuFloat, tuClear;

	MaxTUEnumerator e(FLOW_ELEMENT);

	tu = e.GetNextUnit();
	while (tu)
	{
		tuType = tu->GetUnitType();

		switch (tuType)
		{
		case TU_TYPE_FRAMESET:
		case TU_TYPE_INLINE:    // intentional fall-through
		case TU_TYPE_NODISPLAY: // intentional fall-through
			break;

		default:
			// We need to set the parent display here so that elements needing to get at the graphics
			//  context can report the correct dimensions
			tu->SetDisplayContext(this);

			tuBreak = tu->GetBreak();
			tuFloat = tu->GetFloat();
			tuMaxWidth = tu->GetMaxWidth() + tu->GetLeftMargin() + tu->GetRightMargin();

			switch (tuFloat)
			{
			case TU_FLOAT_LEFT:
				if (iUnits == 0)
				{
					iFloatLeft += tuMaxWidth;
					lineWidth = iFloatLeft + iFloatRight;
				}
				else
				{
					iNextLineFloatLeft += tuMaxWidth;
				}
				break;

			case TU_FLOAT_RIGHT:
				if (iUnits == 0)
				{
					iFloatRight += tuMaxWidth;
					lineWidth = iFloatLeft + iFloatRight;
				}
				else
				{
					iNextLineFloatRight += tuMaxWidth;
				}
				break;

			case TU_FLOAT_NONE:
				if (tuBreak & TU_BREAK_BEFORE)
				{
					maxWidth = EBSMAX(maxWidth, lineWidth);
					if (maxWidth > bailOutLimit)
					{
						return (maxWidth);
					}
					iFloatLeft += iNextLineFloatLeft;
					iFloatRight += iNextLineFloatRight;
					iNextLineFloatLeft = 0;
					iNextLineFloatRight = 0;
					lineWidth = iFloatLeft + iFloatRight;
					iUnits = 0;
				}

				tuClear = tu->GetClear();

				if (tuClear != TU_CLEAR_NONE && tuType != TU_TYPE_BREAK)
				{
					maxWidth = EBSMAX(maxWidth, lineWidth);
					if (maxWidth > bailOutLimit)
					{
						return (maxWidth);
					}

					switch (tuClear)
					{
						case TU_CLEAR_LEFT:
							iFloatLeft = 0;
							iFloatRight += iNextLineFloatRight;
							break;

						case TU_CLEAR_RIGHT:
							iFloatLeft += iNextLineFloatLeft;
							iFloatRight = 0;
							break;

						case TU_CLEAR_ALL:
							iFloatLeft = 0;
							iFloatRight = 0;
							break;

						default:
							break;
					}

					iNextLineFloatLeft = 0;
					iNextLineFloatRight = 0;

					lineWidth = iFloatLeft + iFloatRight;
					iUnits = 0;
				}

				lineWidth += tuMaxWidth;
				iUnits++;

				if (tuClear != TU_CLEAR_NONE && tuType == TU_TYPE_BREAK)
				{
					// break the line, position the queued floating elements,
					//  and then clear the left, right, or both margins

					maxWidth = EBSMAX(maxWidth, lineWidth);
					if (maxWidth > bailOutLimit)
					{
						return (maxWidth);
					}

					iFloatLeft += iNextLineFloatLeft;
					iFloatRight += iNextLineFloatRight;
					iNextLineFloatLeft = 0;
					iNextLineFloatRight = 0;

					lineWidth = iFloatLeft + iFloatRight;
					maxWidth = EBSMAX(maxWidth, lineWidth);
					if (maxWidth > bailOutLimit)
					{
						return (maxWidth);
					}

					switch (tuClear)
					{
						case TU_CLEAR_LEFT:
							iFloatLeft = 0;
							break;

						case TU_CLEAR_RIGHT:
							iFloatRight = 0;
							break;

						case TU_CLEAR_ALL:
							iFloatLeft = 0;
							iFloatRight = 0;
							break;

						default:
							break;
					}

					lineWidth = iFloatLeft + iFloatRight;
					iUnits = 0;
				}
				else
				{
					if (tuBreak & TU_BREAK_AFTER)
					{
						maxWidth = EBSMAX(maxWidth, lineWidth);
						if (maxWidth > bailOutLimit)
						{
							return (maxWidth);
						}
						iFloatLeft += iNextLineFloatLeft;
						iFloatRight += iNextLineFloatRight;
						iNextLineFloatLeft = 0;
						iNextLineFloatRight = 0;
						lineWidth = iFloatLeft + iFloatRight;
						iUnits = 0;
					}
				}
				break;

			default:
				break;
			}
		}

		tu = e.GetNextUnit();
	}

	maxWidth = EBSMAX(maxWidth, lineWidth);
	iFloatLeft += iNextLineFloatLeft;
	iFloatRight += iNextLineFloatRight;
	lineWidth = iFloatLeft + iFloatRight;
	maxWidth = EBSMAX(maxWidth, lineWidth);

	maxWidth += (TOTAL_LEFT_PAD + TOTAL_RIGHT_PAD);
	return (maxWidth);
}

/*---------------------------------------------------------------------------*/
DISPLAY_INT HTMLFlowDisplay::FindMinWidth(void)
{
DISPLAY_INT minWidth = 0;
DISPLAY_INT lineWidth = 0;
DISPLAY_INT tuMinWidth;
TextUnitEnumerator *e;
TextUnit *tu, *prevTU = 0;
WEBC_UINT16 tuBreak, tuType;
WEBC_UINT16 bWrap = GetChildWrap();
int iWrapHold = 0;
WEBC_BOOL firstInNoBr = WEBC_FALSE;

	WEBC_NEW(e, MinTUEnumerator(FLOW_ELEMENT));
	if (!e)
	{
		return -1;
	}

	tu = e->GetNextUnit();
	while (tu)
	{
		if (!IgnoreInFindMinWidth(tu))
		{
			tuType = tu->GetUnitType();

			switch (tuType)
			{
			case TU_TYPE_OPEN_NOBR:
				if (iWrapHold == 0)
				{
					firstInNoBr = WEBC_TRUE;
				}
				iWrapHold++;
				break;

			case TU_TYPE_CLOSE_NOBR:
				if (iWrapHold > 0)
				{
					if (iWrapHold == 1)
					{
						if ((bWrap == TU_WRAP_NORMAL) && (!prevTU || prevTU->BreakAllowedAfter()))
						{
							minWidth = EBSMAX(minWidth, lineWidth);
							lineWidth = 0;
							prevTU = 0;
						}
					}
					iWrapHold--;
				}
				break;

			case TU_TYPE_FRAMESET:
			case TU_TYPE_INLINE:    // intentional fall-through
			case TU_TYPE_NODISPLAY: // intentional fall-through
				break;

			default:
				// We need to set the parent display here so that elements needing to get at the graphics
				//  context can report the correct dimensions
				tu->SetDisplayContext(this);

				tuBreak = tu->GetBreak();
				tuMinWidth = tu->GetMinWidth() + tu->GetLeftMargin() + tu->GetRightMargin();

				switch (tu->GetFloat())
				{
				case TU_FLOAT_LEFT:
				case TU_FLOAT_RIGHT:
					minWidth = EBSMAX(minWidth, tuMinWidth);
					break;

				case TU_FLOAT_NONE:
					if (((bWrap == TU_WRAP_NORMAL) && (firstInNoBr || (iWrapHold == 0)) && tu->BreakAllowedBefore())
					    || (tuBreak & TU_BREAK_BEFORE))
					{
						minWidth = EBSMAX(minWidth, lineWidth);
						lineWidth = 0;
						prevTU = 0;
					}

					lineWidth += tuMinWidth;
					prevTU = tu;
					firstInNoBr = WEBC_FALSE;

					if (((bWrap == TU_WRAP_NORMAL) && (iWrapHold == 0) && tu->BreakAllowedAfter())
						|| (tuBreak & TU_BREAK_AFTER) || (tu->GetClear() != TU_CLEAR_NONE))
					{
						minWidth = EBSMAX(minWidth, lineWidth);
						lineWidth = 0;
						prevTU = 0;
					}

					break;

				default:
					break;
				}
			}
		}

		tu = e->GetNextUnit();
	}

	WEBC_DELETE(e);

	minWidth = EBSMAX(minWidth, lineWidth);

	CSSLength width;
	GetCSSLength(&width, CSS_PROPERTY_WIDTH);
	if (!CSS_LengthIsRelative(&width))
	{
		DISPLAY_INT widthValue = CSS_LengthToPixels(&width, 0, 0, 0);
		minWidth = EBSMAX(minWidth, widthValue);
	}

	return (minWidth + (TOTAL_LEFT_PAD + TOTAL_RIGHT_PAD));
}

/*---------------------------------------------------------------------------*/
DISPLAY_INT HTMLFlowDisplay::FindPixelWidth (DISPLAY_INT parentWidth)
{
	DISPLAY_INT pixelWidth;
	CSSLength width;

	GetCSSLength(&width, CSS_PROPERTY_WIDTH);

	switch (width.type)
	{
		case CSS_LENGTH_UNIT_AUTO:
			switch (GetPosition())
			{
				case TU_POSITION_RELATIVE:
				case TU_POSITION_STATIC:
				{
					DISPLAY_INT marginLeftValue;
					DISPLAY_INT marginRightValue;

					mBorder.GetHorizontalMargins(&marginLeftValue, &marginRightValue, miParentWidth);

					DISPLAY_INT totalMarginWidth = marginLeftValue + marginRightValue;

					switch (GetFloat())
					{
						case TU_FLOAT_NONE:
							pixelWidth = EBSMAX(0, parentWidth - totalMarginWidth);
							break;

						default:
							// in the case of a floating element, don't format to larger than
							//  the max width required by the contents of the element
							//  (tbd - make this faster by bailing out of GetMaxWidth when we
							//  know that the width is greater than the parent - margins)
							if (totalMarginWidth > parentWidth)
							{
								pixelWidth = 0;
							}
							else
							{
								pixelWidth = parentWidth - totalMarginWidth;
								DISPLAY_INT maxWidth = GetMaxWidthSmallerThan(pixelWidth);
								if (maxWidth < pixelWidth)
								{
									pixelWidth = maxWidth;
								}
							}
							break;
					}
					break;
				}

				default:
					break;
			}
			break;

		default:
		{
			// just calculate the width value
			WebFont font = mpHtmlElement->GetWebFont();
			DISPLAY_INT emHeight = (font)? WEB_FONT_HEIGHT(font) : WEBC_CFG_DEFAULT_TEXT_HEIGHT;
			DISPLAY_INT exHeight = emHeight / 2;

			pixelWidth = CSS_LengthToPixels(&width, parentWidth, emHeight, exHeight);
			pixelWidth += mBorder.GetBorderWidth(CSS_LEFT) +
				          mBorder.GetBorderWidth(CSS_RIGHT) +
						  mBorder.GetPaddingWidth(CSS_LEFT) +
						  mBorder.GetPaddingWidth(CSS_RIGHT);
			break;
		}
	}

	return (pixelWidth);
}

/*---------------------------------------------------------------------------*/
LineContext* HTMLFlowDisplay::CreateLineContext (void)
{
	LineContext *currentLine;

	if (GetChildWrap() == TU_WRAP_NONE)
	{
		WEBC_NEW_VERBOSE(currentLine, NonBreakingLineContext(FLOW_ELEMENT->GetChildTextAlign()),"LineContext");
	}
	else
	{
		WEBC_NEW_VERBOSE(currentLine, LineContext(FLOW_ELEMENT->GetChildTextAlign()),"LineContext");
	}

	return (currentLine);
}

/*---------------------------------------------------------------------------*/
DISPLAY_INT HTMLFlowDisplay::FormatFlowContent (
		MarginContext* margin,
		DISPLAY_INT leftPad,
		DISPLAY_INT topPad,
		DISPLAY_INT *pcontentWidth
	)
{

	WEBC_ASSERT(!(mFlowFlags & FD_FLAG_FLOW_FORMAT_IN_PROGRESS));
	mFlowFlags |= FD_FLAG_FLOW_FORMAT_IN_PROGRESS;

	DISPLAY_INT contentHeight = 0;
	*pcontentWidth = 0;
	int loops=0;
	do
	{
		contentHeight = 0;
		TextUnitEnumerator* e = GetTextUnits();
		if (e)
		{
			TextUnit* tu = e->GetNextUnit();
			if (tu)
			{
				LineContext* currentLine = CreateLineContext();

				if (!currentLine)
				{
					tu->Release();
				}
				else
				{
					DISPLAY_INT contentWidth;
					WEBC_UINT8 tuFloat;
					WEBC_UINT16 tuType;
					DisplayElement oldContentMarker;
					DisplayElement floatPlaceholder;

					contentWidth = margin->GetRight() - margin->GetLeft() + 1;

					currentLine->miTop = topPad;
					currentLine->mpMargin = margin;
					currentLine->miParentWidth = margin->GetRight() - margin->GetLeft() + 1;
					currentLine->SetLeftAndRight();

					if (mInlineContentHolder.mpParent != this)
					{
						InsertLast(&mInlineContentHolder);
					}

					mInlineContentHolder.InsertLast(&oldContentMarker);
					mInlineContentHolder.InsertLast(&floatPlaceholder);

					while (tu)
					{
						tuType = tu->GetUnitType();

						switch (tuType)
						{
						case TU_TYPE_DEFAULT: // intentional fall-through
						case TU_TYPE_TEXT:    // intentional fall-through
						case TU_TYPE_BREAK:   // intentional fall-through
						case TU_TYPE_BLOCK:   // intentional fall-through
						case TU_TYPE_INLINE_BLOCK:  // intentional fall-through
						case TU_TYPE_TABLE:   // intentional fall-through
						default:

								tuFloat = tu->GetFloat();
						/* If not otherwise specified float inline block, left so we don't line break */

							switch (tuFloat)
							{
							case TU_FLOAT_LEFT:
							case TU_FLOAT_RIGHT:
								// SetParentDisplay will add the DisplayElement of this TU as our last child,
								//  if it has one
								tu->SetParentDisplay(&mInlineContentHolder, 0);
								tu->SetParentHeight(GetManager()? GetManager()->mViewRect.Height() : 0);
								tu->SetParentWidth(contentWidth, contentWidth);

								/* tbd
								if (currentLine->BreaksBeforeAlignedUnit(tu))
								{
									currentLine->Finalize(margin);
								}
								*/

								if (!currentLine->Empty())
								{
									margin->QueueUnit(tu);
								}
								else
								{
									DISPLAY_INT floatElementTop = currentLine->miTop;
									if (currentLine->miPrevLineBottomMargin > 0)
									{
										floatElementTop += currentLine->miPrevLineBottomMargin;
									}

									if (tuFloat == TU_FLOAT_LEFT)
									{
										margin->AlignUnitLeft(tu, floatElementTop);
									}
									else
									{
										margin->AlignUnitRight(tu, floatElementTop);
									}

									currentLine->SetLeftAndRight();
								}
								break;

							case TU_FLOAT_NONE:
								switch (tu->GetPosition())
								{
								case TU_POSITION_ABSOLUTE: // intentional fall-through
								case TU_POSITION_FIXED:
									break;

								case TU_POSITION_RELATIVE: // intentional fall-through
								case TU_POSITION_STATIC:   // intentional fall-through
								default:
									// SetParentDisplay will add the DisplayElement of this TU as our last child,
									//  if it has one
									tu->SetParentDisplay(&mInlineContentHolder, &floatPlaceholder);
									if (tu->GetUnitType() == TU_TYPE_BLOCK)
									{
										switch (tu->GetClear())
										{
											case TU_CLEAR_LEFT:
												currentLine->ClearLeft();
												break;

											case TU_CLEAR_RIGHT:
												currentLine->ClearRight();
												break;

											case TU_CLEAR_ALL:
												currentLine->ClearAll();
												break;

											default:
												break;
										}

										tu->SetParentHeight(GetManager()? GetManager()->mViewRect.Height() : 0);
										tu->SetParentWidth(contentWidth, contentWidth);

										DISPLAY_INT iXPos = leftPad + tu->GetLeftMargin();
										DISPLAY_INT iYPos;
										if (currentLine->miTop == topPad)
										{
											iYPos = currentLine->miTop + tu->GetTopMargin();
										}
										else
										{
											DISPLAY_INT topMargin = tu->GetTopMargin();
											DISPLAY_INT tuParagraphSpace = tu->GetParagraphSpace();

											if (topMargin < currentLine->miPrevLineBottomMargin)
											{
												topMargin = currentLine->miPrevLineBottomMargin;
											}

											iYPos = currentLine->miTop + EBSMAX(tuParagraphSpace, topMargin);
										}

										DISPLAY_INT tuMarginLeft = tu->GetLeftMargin();
										DISPLAY_INT tuWidthValue = tu->GetPixelWidth();

										margin->AddPadding(tuMarginLeft, contentWidth - (tuMarginLeft + tuWidthValue));
										margin->EnterElement(iXPos, iYPos);
								printf("FormatFlowContent >>>>>>>>>>>>>>>>>>>>>>>>\n");
										tu->FormatBlock(margin);
								printf("FormatFlowContent <<<<<<<<<<<<<<<<<<<<<<<<\n");
										margin->ExitElement(iXPos, iYPos);
										margin->RemovePadding(tuMarginLeft, contentWidth - (tuMarginLeft + tuWidthValue));
										tu->SetXYPosition(iXPos, iYPos);

										DISPLAY_INT tuHeight = tu->GetPixelHeight();

										// Check if style display: none is true.
										// If so clear the tuHeight variable so it doesn't effect positioning
										if (tu->DisplayNoneIsTrue())
											tuHeight = 0;

										if (tuHeight > 0)
										{
											currentLine->Clear(iYPos + tuHeight);
											currentLine->miPrevLineBottomMargin = tu->GetBottomMargin();
										}
									}
									else if (tu->GetUnitType() == TU_TYPE_INLINE_BLOCK)
									{
										switch (tu->GetClear())
										{
											case TU_CLEAR_LEFT:
												currentLine->ClearLeft();
												break;

											case TU_CLEAR_RIGHT:
												currentLine->ClearRight();
												break;

											case TU_CLEAR_ALL:
												currentLine->ClearAll();
												break;

											default:
												break;
										}

										tu->SetParentHeight(GetManager()? GetManager()->mViewRect.Height() : 0);
#if(1)
										tu->FormatBlock(margin);
										currentLine->AddUnit(tu, tuType);
#else
										DISPLAY_INT tuMarginLeft = tu->GetLeftMargin();
										DISPLAY_INT tuWidthValue = tu->GetPixelWidth();


										margin->AddPadding(tuMarginLeft, contentWidth - (tuMarginLeft + tuWidthValue));
+										tu->FormatBlock(margin);
										margin->RemovePadding(tuMarginLeft, contentWidth - (tuMarginLeft + tuWidthValue));
										int i = GetMaxWidth();
//										tu->SetXYPosition(currentLine->UsedWidth(), currentLine->miTop);
 										currentLine->AddUnit(tu, tuType);
#endif
									}
									else
									{
										tu->SetParentHeight(GetManager()? GetManager()->mViewRect.Height() : 0);
										currentLine->AddUnit(tu, tuType);
									}
									break;
								}
								break;

							default:
								break;
							}
							break;

						case TU_TYPE_FRAMESET:  // intentional fall-through
						case TU_TYPE_INLINE:    // intentional fall-through
						case TU_TYPE_NODISPLAY:
							// frameset, inline, and nodisplay elements are not formatted here
							tu->Release();
							break;
						}

						tu = e->GetNextUnit();
					}
					*pcontentWidth = currentLine->UsedWidth();
					currentLine->Finalize();

					// Set the final content height
					contentHeight = currentLine->miTop - topPad;


					while (mInlineContentHolder.mpFirstChild != &oldContentMarker)
					{
						mInlineContentHolder.Remove(mInlineContentHolder.mpFirstChild);
					}
					mInlineContentHolder.Remove(&oldContentMarker);
					mInlineContentHolder.Remove(&floatPlaceholder);

					WEBC_DELETE(currentLine);
				}
			}

			WEBC_DELETE(e);
		}
#if (!INCLUDE_EXPERIMENTAL_DIV_SCROLLBARS)
	}
	while (0);
#else

		if (this->mpHtmlElement->GetTag()==HTML_TAG_DIV)
		{ // Display scrollbars if height and with are specified
			DISPLAY_INT heightValue;
			WEBC_BOOL boolIsAuto;
			GetSpecifiedHeight (&heightValue, &boolIsAuto);
			if (boolIsAuto)
				heightValue=0;
			if ((margin->GetRight()-margin->GetLeft()>0)  && heightValue > 0)
			{
				if (loops >= 3)
					break;
				loops+=1;
				DISPLAY_INT MaxWidth = GetMaxWidth();
				if (!UpdateScrollBars(margin->GetRight()-margin->GetLeft(),heightValue, contentHeight,MaxWidth))
					break;
			}
			else
				break;
		}
		else
			break;
	} while(1);
#endif

	mFlowFlags &= ~FD_FLAG_FLOW_FORMAT_IN_PROGRESS;
	return (contentHeight);
}

// PVO Routine to see if display none style is set.
WEBC_BOOL HTMLFlowDisplay::DisplayNoneIsTrue(void)
{
	return (mpHtmlElement->GetVisibleFromDisplay() == 0);
}

/*---------------------------------------------------------------------------*/
WEBC_BOOL HTMLFlowDisplay::NeedsFormat (void)
{
	return (!(mFlowFlags & (FD_FLAG_FLOW_FORMAT_IN_PROGRESS)) &&
	         (mFlowFlags & (FD_FLAG_FLOW_CHILD_STYLE_MODIFIED|
	                        FD_FLAG_LOCAL_STYLE_MODIFIED|
	                        FD_FLAG_PARENT_STYLE_MODIFIED)));
}

/*---------------------------------------------------------------------------*/
void HTMLFlowDisplay::FormatIfNecessary (void)
{
	if (NeedsFormat())
	{
		Format();
	}
}

#if (WEBC_SUPPORT_BACKGROUND_IMAGES)

/*---------------------------------------------------------------------------*/
CSSBackgroundRepeat HTMLFlowDisplay::GetBackgroundRepeat (void)
{
	return FLOW_ELEMENT->GetBackgroundRepeat();
}

/*---------------------------------------------------------------------------*/
CSSBackgroundAttachment HTMLFlowDisplay::GetBackgroundAttachment (void)
{
	return FLOW_ELEMENT->GetBackgroundAttachment();
}

/*---------------------------------------------------------------------------*/
void HTMLFlowDisplay::GetBackgroundXPos (CSSLength *pos)
{
	FLOW_ELEMENT->GetBackgroundXPos(pos);
}

/*---------------------------------------------------------------------------*/
void HTMLFlowDisplay::GetBackgroundYPos (CSSLength *pos)
{
	FLOW_ELEMENT->GetBackgroundYPos(pos);
}

#endif // WEBC_SUPPORT_BACKGROUND_IMAGES

/*---------------------------------------------------------------------------*/
WebColor HTMLFlowDisplay::GetBackgroundColor (WebGraphics *pGC)
{
	if (!(mFlowFlags & FD_FLAG_BGCOLOR_VALID))
	{
		HTMLColor rgbColor = FLOW_ELEMENT->GetBackgroundColor();
		if (HTML_ColorGetAlpha(rgbColor) != 0xff)
		{
			mBackgroundColor = pGC->RGBToColor(rgbColor);
			mFlowFlags |= FD_FLAG_BGCOLOR_SOLID;
		}
		else
		{
			mFlowFlags &= ~FD_FLAG_BGCOLOR_SOLID;
		}

		mFlowFlags |= FD_FLAG_BGCOLOR_VALID;
	}

	return (mBackgroundColor);
}

/*---------------------------------------------------------------------------*/
WEBC_UINT16 HTMLFlowDisplay::GetChildWrap(void)
{
	return ((FLOW_ELEMENT)? FLOW_ELEMENT->GetChildWrap() : TU_WRAP_NORMAL);
}

/*---------------------------------------------------------------------------*/
void HTMLFlowDisplay::GetSpecifiedHeight (DISPLAY_INT* heightValue, WEBC_BOOL* boolIsAuto)
{
	CSSLength height;

	GetCSSLength(&height, CSS_PROPERTY_HEIGHT);

	if ((height.type == CSS_LENGTH_UNIT_AUTO) ||
	    ((height.type == CSS_LENGTH_UNIT_PERCENT) && (mFlowFlags & FD_FLAG_PARENT_HEIGHT_AUTO)))
	{
		*boolIsAuto = 1;
		*heightValue = 0;
	}
	else
	{
		*boolIsAuto = 0;

		WebFont font = mpHtmlElement->GetWebFont();
		DISPLAY_INT emHeight = (font)? WEB_FONT_HEIGHT(font) : WEBC_CFG_DEFAULT_TEXT_HEIGHT;
		DISPLAY_INT exHeight = emHeight / 2;

		*heightValue = CSS_LengthToPixels(&height, miParentHeight, emHeight, exHeight);
	}
}

/*---------------------------------------------------------------------------*/
TextUnitEnumerator *HTMLFlowDisplay::GetTextUnits(void)
{
DisplayTUEnumerator *e;

	WEBC_NEW_VERBOSE(e, DisplayTUEnumerator(FLOW_ELEMENT),"TextUnitEnumeratore");
	return (e);
}

/*---------------------------------------------------------------------------*/
WEBC_BOOL HTMLFlowDisplay::IgnoreInFindMinWidth(TextUnit *tu)
{
	int f = tu->GetFloat();
	int p = tu->GetPosition();
	return ((f == TU_FLOAT_NONE) &&
	        ((p == TU_POSITION_ABSOLUTE) || (p == TU_POSITION_FIXED)));
}

/*---------------------------------------------------------------------------*/
WEBC_BOOL HTMLFlowDisplay::MinWidthValid (void)
{
	return ((mFlowFlags & FD_FLAG_MINWIDTH_VALID) != 0);
}

/*---------------------------------------------------------------------------*/
WEBC_BOOL HTMLFlowDisplay::MaxWidthValid (void)
{
	return ((mFlowFlags & FD_FLAG_MAXWIDTH_VALID) != 0);
}

/*---------------------------------------------------------------------------*/
void HTMLFlowDisplay::SetLocalStyleModified (void)
{
	if (!(mFlowFlags & FD_FLAG_LOCAL_STYLE_MODIFIED))
	{
		mFlowFlags |= FD_FLAG_LOCAL_STYLE_MODIFIED;

		DisplayElement *parent = mpHtmlElement? mpHtmlElement->GetDisplayParent() : 0;
		if (parent)
		{
			NotifyEvent e(NOTIFIER_TYPE_TEXT_UNIT, NOTIFY_STYLE_CHANGE);

			e.data.oldStyle.display = GetUnitType();
			e.data.oldStyle.position = GetPosition();

			InvalidateUnitType();
			InvalidatePosition();

			parent->Notify((TextUnit *) this, &e);
		}
	}
}

/*---------------------------------------------------------------------------*/
void HTMLFlowDisplay::SetParentStyleModified (void)
{
	mFlowFlags |= FD_FLAG_PARENT_STYLE_MODIFIED;
}

/*---------------------------------------------------------------------------*/
void HTMLFlowDisplay::SetPosChildStyleModified (void)
{
	if (!(mFlowFlags & FD_FLAG_POS_CHILD_STYLE_MODIFIED))
	{
		mFlowFlags |= FD_FLAG_POS_CHILD_STYLE_MODIFIED;

		DisplayElement *parent = mpHtmlElement? mpHtmlElement->GetDisplayParent() : 0;
		if (parent)
		{
			// instead of
			NotifyEvent e(NOTIFIER_TYPE_TEXT_UNIT, NOTIFY_POSITIONED_CHILD_STYLE_CHANGE);
			parent->Notify((TextUnit *) this, &e);
		}
	}
}

/*---------------------------------------------------------------------------*/
void HTMLFlowDisplay::SetFlowChildStyleModified (void)
{
	if (!(mFlowFlags & FD_FLAG_FLOW_CHILD_STYLE_MODIFIED))
	{
		mFlowFlags |= FD_FLAG_FLOW_CHILD_STYLE_MODIFIED;

		DisplayElement *parent = mpHtmlElement? mpHtmlElement->GetDisplayParent() : 0;

		if (parent)
		{
			NotifyEvent e(NOTIFIER_TYPE_TEXT_UNIT, NOTIFY_FLOW_CHILD_STYLE_CHANGE);
			enum DisplayOverflow d = this->GetOverflow();
			if (d != DISPLAY_OVERFLOW_HIDDEN)
				parent->Notify((TextUnit *) this, &e);
		}
	}
}

/*---------------------------------------------------------------------------*/
void HTMLFlowDisplay::SetMinWidthInvalid (void)
{
	if (mFlowFlags & FD_FLAG_MINWIDTH_VALID)
	{
		mFlowFlags &= ~FD_FLAG_MINWIDTH_VALID;

		DisplayElement *parent = mpHtmlElement? mpHtmlElement->GetDisplayParent() : 0;
		if (parent)
		{
			NotifyEvent e(NOTIFIER_TYPE_TEXT_UNIT, NOTIFY_MIN_WIDTH_CHANGE);
			enum DisplayOverflow d = this->GetOverflow();
			if (d != DISPLAY_OVERFLOW_HIDDEN)
				parent->Notify((TextUnit *) this, &e);
		}
	}
}

/*---------------------------------------------------------------------------*/
void HTMLFlowDisplay::SetMaxWidthInvalid (void)
{
	if (mFlowFlags & FD_FLAG_MAXWIDTH_VALID)
	{
		mFlowFlags &= ~(FD_FLAG_MAXWIDTH_VALID|FD_FLAG_MAXWIDTH_ESTIMATED);

		DisplayElement *parent = mpHtmlElement? mpHtmlElement->GetDisplayParent() : 0;
		if (parent)
		{
			NotifyEvent e(NOTIFIER_TYPE_TEXT_UNIT, NOTIFY_MAX_WIDTH_CHANGE);
			enum DisplayOverflow d = this->GetOverflow();
			if (d != DISPLAY_OVERFLOW_HIDDEN)
				parent->Notify((TextUnit *) this, &e);
		}
	}
}

/*---------------------------------------------------------------------------*/
void HTMLFlowDisplay::UpdateContentVAlign (void)
{
	// Adjust our InlineContentHolder rectangle to account for vertical alignment
	if (mInlineContentHolder.mpParent == this)
	{
		DISPLAY_INT topPad = TOTAL_TOP_PAD;
		DISPLAY_INT bottomPad = TOTAL_BOTTOM_PAD;
		DISPLAY_INT fullHeight = Height();
		DISPLAY_INT emptySpace = fullHeight - (topPad + miContentHeight + bottomPad);

		PresetWebRect box(&mInlineContentHolder.mRect);

		DISPLAY_INT vOffset = 0;

		if (emptySpace > 0)
		{
			switch (FLOW_ELEMENT->GetChildVAlign())
			{
				case TU_VALIGN_BOTTOM:
					vOffset = emptySpace;
					break;

				case TU_VALIGN_CENTER:
					vOffset = (emptySpace >> 1); // divide by two
					break;

				case TU_VALIGN_TOP: // intentional fall-through
				default:
					break;
			}
		}

		box.Set(0, vOffset, Width() - 1, vOffset + miContentHeight - 1);
		mInlineContentHolder.Move(&box);
	}
}

/*---------------------------------------------------------------------------*/
void HTMLFlowDisplay::UpdateHeight (
		DISPLAY_INT topPad,
		DISPLAY_INT bottomPad
	)
{
	DISPLAY_INT heightValue;
	WEBC_BOOL heightIsAuto;

	GetSpecifiedHeight(&heightValue, &heightIsAuto);

	if (heightIsAuto)
	{
		heightValue = miContentHeight;
	}

	SetHeight(topPad + heightValue + bottomPad);
}
/*---------------------------------------------------------------------------*/
void HTMLFlowDisplay::UpdateWidth (DISPLAY_INT CalculatedContentWidth)
{
	CSSLength width;

	GetCSSLength(&width, CSS_PROPERTY_WIDTH);

	if (width.type == CSS_LENGTH_UNIT_AUTO)
	{
		SetWidth(CalculatedContentWidth);
	}
}
