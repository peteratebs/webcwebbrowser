//
// WTSCROLL.CPP -
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

#include "webgraph.hpp"
#include "wtscroll.hpp"
#include "util.h"
#include "wgbmp.hpp"
#include "wgcolors.hpp"
#include "wgkeys.hpp"
#include "webcmem.h"
#include "webcdefault.h"


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

WebScroll::WebScroll()
{
#if (INCLUDE_EXPERIMENTAL_DIV_SCROLLBARS)
	mIncludeInFlow=WEBC_FALSE;
#endif
	mpListener = 0;
	InsertLast(&mUpButton);
	InsertLast(&mDownButton);
	mUpButton.SetListener(this);
	mDownButton.SetListener(this);
	mUpButton.ClearFlag(DISPLAY_FLAG_ACCEPTS_FOCUS);
	mDownButton.ClearFlag(DISPLAY_FLAG_ACCEPTS_FOCUS);
	ClearFlag(DISPLAY_FLAG_ACCEPTS_FOCUS);
}

WebScroll::~WebScroll()
{
}

void WebScroll::SetPosition(long i)
{
	if (mpSlider)
	{
		mpSlider->SetPosition(i);
	}
}

void WebScroll::SetRange(long r)
{
	if (mpSlider)
	{
		mpSlider->SetRange(r);
	}
}

void WebScroll::SetWindow(long w)
{
	if (mpSlider)
	{
		mpSlider->SetWindow(w);
	}
}

void WebScroll::SetStep(long s)
{
	if (mpSlider)
	{
		mpSlider->SetStep(s);
	}
}

long WebScroll::GetPosition(void)
{
	return mpSlider? mpSlider->GetPosition() : 0;
}

long WebScroll::GetRange(void)
{
	return mpSlider? mpSlider->GetRange() : 0;
}

long WebScroll::GetWindow(void)
{
	return mpSlider? mpSlider->GetWindow() : 0;
}

long WebScroll::GetStep(void)
{
	return mpSlider? mpSlider->GetStep() : 0;
}

void WebScroll::Up(void)
{
	if (mpSlider)
	{
		mpSlider->Up();
	}
}

void WebScroll::Down(void)
{
	if (mpSlider)
	{
		mpSlider->Down();
	}
}

void WebScroll::PageUp(void)
{
	if (mpSlider)
	{
		mpSlider->PageUp();
	}
}

void WebScroll::PageDown(void)
{
	if (mpSlider)
	{
		mpSlider->PageDown();
	}
}

HTMLEventStatus WebScroll::Event(HTMLEvent * e)
{
    WEBC_DEBUG_ReceiveEvent(this, e);
	return mpSlider? mpSlider->Event(e) : HTML_EVENT_STATUS_CONTINUE;
}

WEBC_UINT32 WebScroll::GetScrollbarFlags ()
{
	WEBC_UINT32 flags = 0;

	if (mpSlider)
	{
		switch (mpSlider->GetSliderState())
		{
		case STATE_PULL_UP:
			flags |= HTML_SCROLL_FLAG_BEFORE_SLIDER_ACTIVE;
			break;

		case STATE_PULL_DOWN:
			flags |= HTML_SCROLL_FLAG_AFTER_SLIDER_ACTIVE;
			break;

		case STATE_DRAGGING:
			flags |= HTML_SCROLL_FLAG_SLIDER_ACTIVE;
			break;
		}
	}

	if (GetFlags() & DISPLAY_FLAG_FOCUS)
	{
		flags |= HTML_SCROLL_FLAG_HAS_FOCUS;
	}

	if (GetFlags() & DISPLAY_FLAG_DISABLED)
	{
		flags |= HTML_SCROLL_FLAG_DISABLED;
	}

	if (GetFlags() & DISPLAY_FLAG_READONLY)
	{
		flags |= HTML_SCROLL_FLAG_READONLY;
	}

	if (mpParent)
	{
		if (mRect.left == 0)
		{
			flags |= HTML_SCROLL_FLAG_LEFT_FLUSH;
		}

		if (mRect.top == 0)
		{
			flags |= HTML_SCROLL_FLAG_TOP_FLUSH;
		}

		if (mRect.right == mpParent->Width() - 1)
		{
			flags |= HTML_SCROLL_FLAG_RIGHT_FLUSH;
		}

		if (mRect.bottom == mpParent->Height() - 1)
		{
			flags |= HTML_SCROLL_FLAG_BOTTOM_FLUSH;
		}
	}

	if (mUpButton.GetButtonFlags() & BUTTON_FLAG_DOWN)
	{
		flags |= HTML_SCROLL_FLAG_DECREASE_ACTIVE;
	}

	if (mDownButton.GetButtonFlags() & BUTTON_FLAG_DOWN)
	{
		flags |= HTML_SCROLL_FLAG_INCREASE_ACTIVE;
	}

	return flags;
}

WebVScroll::WebVScroll()
{
	WEBC_NEW_VERBOSE(mpSlider, WebVSlider,"WebVScroll()");
	if (mpSlider)
	{
		mpSlider->SetListener(this);
		InsertLast(mpSlider);
	}
	InsertLast(&mUpButton);
	InsertLast(&mDownButton);
}

WebVScroll::~WebVScroll()
{
	if (mpSlider)
	{
		WEBC_DELETE(mpSlider);
	}
}

void WebVScroll::SetManager(DisplayManager* manager)
{
	WebScroll::SetManager(manager);

	WebGraphics* gc = GetGraphics();
	if (gc)
	{
		WebBitmap upArrow = gc->GetBitmapByIndex(WGB_UP_ARROW);
		WebBitmap downArrow = gc->GetBitmapByIndex(WGB_DOWN_ARROW);

		mUpButton.SetDefaultBitmap(upArrow);
		mUpButton.SetDownBitmap(upArrow);
		mUpButton.SetOverBitmap(upArrow);

		mDownButton.SetDefaultBitmap(downArrow);
		mDownButton.SetDownBitmap(downArrow);
		mDownButton.SetOverBitmap(downArrow);
	}
}

void WebVScroll::Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect* view, WebGraphics* gc)
{
	PresetWebRect r(mRect);
	r.MoveTo(x,y);
	if (gc->HasNativeVScrollDraw())
	{
		DISPLAY_INT sliderBegin = 0, sliderEnd, sliderLength = 0;
		if (mpSlider)
		{
			mpSlider->GetSliderRange(&sliderBegin, &sliderEnd);
			sliderLength = sliderEnd - sliderBegin + 1;
		}

		WEBC_BOOL drawn = gc->DrawVScroll(
				&r,
				webc_GetDefaultDisplayInt(WEBC_DEFAULT_SCROLL_BAR_BUTTON_LENGTH),
				sliderBegin, /* sliderBegin */
				sliderLength+1, /* sliderLength */
				GetScrollbarFlags() /* flags */ );

		if (drawn)
		{
			return;
		}
	}

	WebScroll::Draw(x, y, view, gc);
}

void WebVScroll::UpdateRect(void)
{
	WebRect rect;
	DISPLAY_INT midPoint = (mRect.bottom - mRect.top) >> 1;

	rect.Set(0, 0, mRect.Width() - 1, EBSMIN(midPoint,webc_GetDefaultDisplayInt(WEBC_DEFAULT_SCROLL_BAR_BUTTON_LENGTH) - 1));
	mUpButton.Move(&rect);

	rect.Set(0, EBSMAX(midPoint + 1, mRect.Height() - 1 - webc_GetDefaultDisplayInt(WEBC_DEFAULT_SCROLL_BAR_BUTTON_LENGTH)), mRect.Width() - 1, mRect.Height() - 1);
	mDownButton.Move(&rect);

	if (mpSlider)
	{
		rect.Set (
				0,
				mUpButton.mRect.bottom + (1 - webc_GetDefaultDisplayInt(WEBC_DEFAULT_SCROLL_BAR_BUTTON_OVERLAP)),
				mRect.Width() - 1,
				mDownButton.mRect.top - (1 - webc_GetDefaultDisplayInt(WEBC_DEFAULT_SCROLL_BAR_BUTTON_OVERLAP))
			);
		mpSlider->Move(&rect);
	}
}

WebHScroll::WebHScroll()
{
	WEBC_NEW_VERBOSE(mpSlider, WebHSlider,"WebHScroll()");
	if (mpSlider)
	{
		mpSlider->SetListener(this);
		InsertLast(mpSlider);
	}
	InsertLast(&mUpButton);
	InsertLast(&mDownButton);
}

WebHScroll::~WebHScroll()
{
	if (mpSlider)
	{
		WEBC_DELETE(mpSlider);
	}
}


void WebHScroll::SetManager(DisplayManager* manager)
{
	WebScroll::SetManager(manager);

	WebGraphics* gc = GetGraphics();
	if (gc)
	{
		WebBitmap upArrow = gc->GetBitmapByIndex(WGB_LEFT_ARROW);
		WebBitmap downArrow = gc->GetBitmapByIndex(WGB_RIGHT_ARROW);

		mUpButton.SetDefaultBitmap(upArrow);
		mUpButton.SetDownBitmap(upArrow);
		mUpButton.SetOverBitmap(upArrow);

		mDownButton.SetDefaultBitmap(downArrow);
		mDownButton.SetDownBitmap(downArrow);
		mDownButton.SetOverBitmap(downArrow);
	}
}

void WebHScroll::Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect* view, WebGraphics* gc)
{
	PresetWebRect r(mRect);
	r.MoveTo(x,y);
	if (gc->HasNativeHScrollDraw())
	{
		DISPLAY_INT sliderBegin = 0, sliderEnd, sliderLength = 0;
		if (mpSlider)
		{
			mpSlider->GetSliderRange(&sliderBegin, &sliderEnd);
			sliderLength = sliderEnd - sliderBegin + 1;
		}

		WEBC_BOOL drawn = gc->DrawHScroll(
				&r,
				webc_GetDefaultDisplayInt(WEBC_DEFAULT_SCROLL_BAR_BUTTON_LENGTH),
				sliderBegin, /* sliderBegin */
				sliderLength+2, /* sliderLength */
				GetScrollbarFlags() /* flags */ );

		if (drawn)
		{
			return;
		}
	}

	WebScroll::Draw(x, y, view, gc);
}

void WebHScroll::UpdateRect(void)
{
	WebRect rect;
	DISPLAY_INT midPoint = (mRect.right - mRect.left) >> 1;

	rect.Set(0, 0, EBSMIN(midPoint, webc_GetDefaultDisplayInt(WEBC_DEFAULT_SCROLL_BAR_BUTTON_LENGTH)), mRect.Height() - 1);
	mUpButton.Move(&rect);

	rect.Set(EBSMAX(midPoint + 1, mRect.Width() - 1 - webc_GetDefaultDisplayInt(WEBC_DEFAULT_SCROLL_BAR_BUTTON_LENGTH)), 0, mRect.Width() - 1, mRect.Height() - 1);
	mDownButton.Move(&rect);

	if (mpSlider)
	{
		rect.Set (
				mUpButton.mRect.right + (1 - webc_GetDefaultDisplayInt(WEBC_DEFAULT_SCROLL_BAR_BUTTON_OVERLAP)),
				0,
				mDownButton.mRect.left - (1 - webc_GetDefaultDisplayInt(WEBC_DEFAULT_SCROLL_BAR_BUTTON_OVERLAP)),
				Height() - 1
			);
		mpSlider->Move(&rect);
	}
}

void WebScroll::SetListener(Listener * l)
{
	mpListener = l;
}


void WebScroll::Move(WebRect * p)
{
	DisplayElement::Move(p);
	UpdateRect();
}


void WebScroll::Notify(Notifier * who, NotifyEvent *what)
{
    WEBC_DEBUG_ReceiveNotify(this, who, what);
	if (who == &mUpButton)
	{
		if (what && what->event == NOTIFY_BUTTON_PRESS)
		{
			if (mpSlider)
			{
				mpSlider->Up();
			}
			mUpButton.Deactivate();
		}
	}
	if (who == &mDownButton)
	{
		if (what && what->event == NOTIFY_BUTTON_PRESS)
		{
			if (mpSlider)
			{
				mpSlider->Down();
			}
			mDownButton.Deactivate();
		}
	}
	if (who == mpSlider && mpListener)
	{
		if (what && what->event == NOTIFY_SLIDER_MOVE)
		{
			mpListener->Notify(this, what);
		}
	}

}

void WebScroll::Disable (void)
{
	DisplayElement::Disable();

	if (mpSlider)
	{
		mpSlider->Disable();
	}

	mUpButton.Disable();
	mDownButton.Disable();
}

void WebScroll::Enable (void)
{
	DisplayElement::Enable();

	if (mpSlider)
	{
		mpSlider->Enable();
	}

	mUpButton.Enable();
	mDownButton.Enable();
}

void WebScroll::Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics* gc)
{
	DisplayElement::Draw(x, y, pViewport, gc);

	DISPLAY_INT frameWidth = webc_GetDefaultDisplayInt(WEBC_DEFAULT_SCROLL_BAR_FRAME_WIDTH);
	if (frameWidth > 0)
	{
		WebRect rect(mRect);
		rect.MoveTo(x,y);
		WebColor hilite = gc->RGBToColor(webc_GetDefaultColor(WEBC_DEFAULT_SCROLL_BAR_HILITE_COLOR));
		WebColor shadow = gc->RGBToColor(webc_GetDefaultColor(WEBC_DEFAULT_SCROLL_BAR_SHADOW_COLOR));
		gc->Frame3D(&rect, hilite, shadow, frameWidth);
	}
}
