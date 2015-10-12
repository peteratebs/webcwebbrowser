//
// WTSLIDER.CPP -
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
#include "wtslider.hpp"
#include "dmanager.hpp"
#include "util.h"
#include "wgcolors.hpp"
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

WebSlider::WebSlider()
{

	state = STATE_NORMAL;
	position = 0;
	window = range = 1;
	step = WEBC_CFG_DEFAULT_VSCROLL_STEP;
	listener = 0;
}

WebSlider::~WebSlider()
{
}

void WebSlider::SetPosition(long i)
{
	if (position != i)
	{
		position = i;
		ClampPosition();
		if (listener)
		{
			NotifyEvent what(NOTIFIER_TYPE_SLIDER, NOTIFY_SLIDER_MOVE);
			listener->Notify(this, &what);
		}
	}
}

void WebSlider::SetRange(long r)
{
	range = r;
	ClampPosition();
}

void WebSlider::SetWindow(long w)
{
	window = w;
	ClampPosition();
}

void WebSlider::SetStep(long s)
{

	step = s;

}

long WebSlider::GetPosition(void)
{

	return position;

}

long WebSlider::GetRange(void)
{

	return range;

}

long WebSlider::GetWindow(void)
{
	return window;
}

long WebSlider::GetStep(void)
{

	return step;

}

void WebSlider::SetListener(Listener * l)
{
	listener = l;
}


void WebSlider::Up(void)
{

	SetPosition(GetPosition() - step);

}

void WebSlider::Down(void)
{

	SetPosition(GetPosition() + step);

}

void WebSlider::ClampPosition(void)
{

	if (position < 0)
	{
		position = 0;
	}
	if (position + GetWindow() > range)
	{
		position = range - GetWindow();
	}
}


void WebSlider::BeginDrag (DISPLAY_INT init)
{
	DISPLAY_INT s,e;
	GetSliderRange(&s,&e);
	initialDragPosition = init - s;
	state = STATE_DRAGGING;

}

void WebSlider::EndDrag (void)
{

	state = STATE_NORMAL;

}

void WebSlider::BeginPullUp (void)
{

	state = STATE_PULL_UP;

}

void WebSlider::BeginPullDown (void)
{

	state = STATE_PULL_DOWN;

}

void WebSlider::EndPullUp (void)
{

	state = STATE_NORMAL;

}

void WebSlider::EndPullDown (void)
{

	state = STATE_NORMAL;

}

void WebSlider::PageUp(void)
{

	int pos = GetPosition();
	pos -= GetWindow() - GetStep();
	if (pos < 0)
	{
		pos = 0;
	}

	SetPosition(pos);

}

void WebSlider::PageDown(void)
{
	int pos = GetPosition();
	int inc = GetWindow() - GetStep();
	if (inc < GetStep())
	{
		inc = GetStep();
	}

	pos += inc;
	if (pos + GetWindow() > GetRange())
	{
		pos = GetRange() - GetWindow();
	}

	SetPosition(pos);
}


void WebSlider::DrawThisOnly(DISPLAY_INT x, DISPLAY_INT y, WebGraphics * gc)
{
	WebColor backgoundColor = gc->RGBToColor(webc_GetDefaultColor(WEBC_DEFAULT_SLIDER_BACKGROUND_COLOR));

	// Draw the background of the slider
	WebRect rect;
	rect.left = x;
	rect.top = y;
	rect.bottom = y + WebSlider::Height() - 1;
	rect.right = x + WebSlider::Width() - 1;
	gc->Rectangle(&rect, backgoundColor, backgoundColor, WEBC_TRUE);

	if (!(mFlags & DISPLAY_FLAG_DISABLED))
	{
		WebColor hilite = gc->RGBToColor(webc_GetDefaultColor(WEBC_DEFAULT_SLIDER_FRAME_HILITE_COLOR));
		WebColor shadow = gc->RGBToColor(webc_GetDefaultColor(WEBC_DEFAULT_SLIDER_FRAME_SHADOW_COLOR));
		gc->Frame3D(&rect, hilite, shadow, webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_BACKGROUND_FRAME_WIDTH));
	}
}

void WebSlider::DrawSlider(WebRect* sliderRect, WebGraphics* gc)
{
	WebColor sliderHiliteColor = gc->RGBToColor(webc_GetDefaultColor(WEBC_DEFAULT_SLIDER_HILITE_COLOR));
	WebColor sliderShadowColor = gc->RGBToColor(webc_GetDefaultColor(WEBC_DEFAULT_SLIDER_SHADOW_COLOR));
	WebColor sliderColor = gc->RGBToColor(webc_GetDefaultColor(WEBC_DEFAULT_SLIDER_COLOR));

	DISPLAY_INT frameWidth = webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_FRAME_WIDTH);

	if (!(mFlags & DISPLAY_FLAG_DISABLED))
	{
		gc->Frame3D(sliderRect, sliderHiliteColor, sliderShadowColor, frameWidth);
	}
	else
	{
		gc->Frame3D(sliderRect, sliderShadowColor, sliderShadowColor, frameWidth);
	}

	sliderRect->top    += frameWidth;
	sliderRect->bottom -= frameWidth;
	sliderRect->left   += frameWidth;
	sliderRect->right  -= frameWidth;

	gc->Rectangle(sliderRect, sliderColor, sliderColor, WEBC_TRUE);
}

void WebSlider::GetSliderRange(DISPLAY_INT* s, DISPLAY_INT* e)
{
	DISPLAY_INT slideMin;
	DISPLAY_INT slideMax;

	GetTotalRange(&slideMin, &slideMax);

	if (GetRange() == GetWindow() || GetRange() == 0)
	{
		*s = slideMin;
		*e = slideMax;
	}
	else
	{
		DISPLAY_INT totalRange = (slideMax - slideMin + 1);
		DISPLAY_INT sliderSize = (totalRange * GetWindow()) / GetRange();
		sliderSize = EBSMAX(sliderSize, MINIMUM_SLIDER_SIZE);
		*s = slideMin + ((GetPosition() * (totalRange - sliderSize)) / (GetRange() - GetWindow()));
		sliderSize = EBSMIN(sliderSize, MAXIMUM_SLIDER_SIZE);
		*e = *s + sliderSize - 1;
	}
}

void WebVSlider::GetTotalRange(DISPLAY_INT* slideMin, DISPLAY_INT* slideMax)
{
	*slideMin = webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_PADDING);
	*slideMax = Height() - 1 - *slideMin;
}

void WebVSlider::DragTo (DISPLAY_INT pos)
{
	int ss = Height() * GetWindow() / GetRange();
	int np = (pos - initialDragPosition) * GetRange() /
		(Height() - (ss <= MINIMUM_SLIDER_SIZE ? MINIMUM_SLIDER_SIZE : 0));
	SetPosition(np);
}

long WebVSlider::GetWindow(void)
{
	return window;
}


void WebHSlider::GetTotalRange(DISPLAY_INT* slideMin, DISPLAY_INT* slideMax)
{
	*slideMin = webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_PADDING);
	*slideMax = Width() - 1 - *slideMin;
}

long WebHSlider::GetWindow(void)
{
	return window;
}


void WebHSlider::DragTo (DISPLAY_INT pos)
{
	int ss = Width() * GetWindow() / GetRange();
	int np = (pos - initialDragPosition) * GetRange() /
		(Width() - (ss <= MINIMUM_SLIDER_SIZE ? MINIMUM_SLIDER_SIZE : 0));
	SetPosition(np);
}

WebVSlider::WebVSlider()
{
}

WebVSlider::~WebVSlider()
{
}

SliderPointType WebVSlider::ClassifyPoint(DISPLAY_INT x, DISPLAY_INT y)
{
	DISPLAY_INT start, end;
	GetSliderRange(&start, &end);
	DISPLAY_INT offsetX, offsetY;
	offsetX = offsetY = 0;
	if (mpParent)
	{
		mpParent->GetDisplayPosition(this, &offsetX, &offsetY);
	}
	x -= offsetX;
	y -= offsetY;
	if (x > 0 && x < WebSlider::Width())
	{
		if (y >= start && y <= end)
		{
			return POINT_INSIDE;
		}
		else if (y < start && y > 0)
		{
			return POINT_BEFORE;
		}
		else if (y > end && y < WebSlider::Height())
		{
			return POINT_AFTER;
		}
		return POINT_NONE;
	}
	if (y > 0 && y < WebSlider::Height())
	{
		return POINT_ALONGSIDE;
	}
	return POINT_NONE;

}

HTMLEventStatus WebVSlider::Event(HTMLEvent * e)
{
	SliderPointType where;
    WEBC_DEBUG_ReceiveEvent(this, e);
	if (mFlags & DISPLAY_FLAG_DISABLED)
	{
		return HTML_EVENT_STATUS_DONE;
	}
	switch (e->type)
	{
	case HTML_EVENT_KEYDOWN:
		if (e->data.key == '8')
		{
			Up();
		}
		else if (e->data.key == '2')
		{
			Down();
		}
		else if (e->data.key == 'a')
		{
			PageDown();
		}
		else if (e->data.key == 'q')
		{
			PageUp();
		}
		SetFlag(DISPLAY_FLAG_DIRTY);
		Invalidate();
		break;

	case HTML_EVENT_MOUSEMOVE:
		if (state == STATE_DRAGGING)
		{
			if (ClassifyPoint(e->data.position.x, e->data.position.y) != POINT_NONE)
			{
				DragTo(e->data.position.y);
				SetFlag(DISPLAY_FLAG_DIRTY);
				Invalidate();
			}
		}
		break;

	case HTML_EVENT_MOUSEDOWN:
		SetFlag(DISPLAY_FLAG_FOCUS|DISPLAY_FLAG_DIRTY);
		Invalidate();
		where = ClassifyPoint(e->data.position.x, e->data.position.y);
		if (where == POINT_BEFORE)
		{
			PageUp();
			BeginPullUp();
		}
		else if (where == POINT_AFTER)
		{
			PageDown();
			BeginPullDown();
		}
		else if (where == POINT_INSIDE)
		{
			if (state == STATE_NORMAL)
			{
				BeginDrag(e->data.position.y);
				if (GetManager())
				{
					GetManager()->ClaimPointer(this);
				}
			}
		}
		break;

	case HTML_EVENT_MOUSEUP:
		if (state == STATE_DRAGGING && GetManager())
		{
			GetManager()->ReleasePointer(this);
		}

		state = STATE_NORMAL;         // should be okay to do this
		Invalidate();
		break;
	default:
		break;
	}
	return HTML_EVENT_STATUS_DONE;
}

// the draw routines should be cleaned up to not use constant colors
void WebVSlider::DrawThisOnly(DISPLAY_INT x, DISPLAY_INT y, WebGraphics * gc)
{
	WebSlider::DrawThisOnly(x, y, gc);

	DISPLAY_INT start, end;
	GetSliderRange(&start, &end);

	WebRect sliderRect;
	sliderRect.top = y + start;
	sliderRect.bottom = y + end;
	sliderRect.left = x + webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_PADDING);
	sliderRect.right = x + WebSlider::Width() - 1 - webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_PADDING);

	DrawSlider(&sliderRect, gc);
}

WebHSlider::WebHSlider()
{
	step = WEBC_CFG_DEFAULT_HSCROLL_STEP;
}

WebHSlider::~WebHSlider()
{
}

SliderPointType WebHSlider::ClassifyPoint(DISPLAY_INT x, DISPLAY_INT y)
{
	DISPLAY_INT start, end;
	DISPLAY_INT offsetX, offsetY;
	offsetX = offsetY = 0;
	if (mpParent)
	{
		mpParent->GetDisplayPosition(this, &offsetX, &offsetY);
	}
	x -= offsetX;
	y -= offsetY;

	GetSliderRange(&start, &end);
	if (y > 0 && y < WebSlider::Height())
	{
		if (x >= start && x <= end)
		{
			return POINT_INSIDE;
		}
		else if (x < start && x > 0)
		{
			return POINT_BEFORE;
		}
		else if (x > end && x < WebSlider::Width())
		{
			return POINT_AFTER;
		}
		return POINT_NONE;
	}

	if (x > 0 && x < WebSlider::Width())
	{
		return POINT_ALONGSIDE;
	}
	return POINT_NONE;



}


HTMLEventStatus WebHSlider::Event(HTMLEvent * e)
{
    WEBC_DEBUG_ReceiveEvent(this, e);
	SliderPointType where;
	if (mFlags & DISPLAY_FLAG_DISABLED)
	{
		return HTML_EVENT_STATUS_DONE;
	}

	switch (e->type)
	{
	case HTML_EVENT_KEYDOWN:
		if (e->data.key == '6')
		{
			Down();
		}
		else if (e->data.key == '4')
		{
			Up();
		}
		else if (e->data.key == 'a')
		{
			PageDown();
		}
		else if (e->data.key == 'q')
		{
			PageUp();
		}
		break;

	case HTML_EVENT_MOUSEMOVE:
		if (state == STATE_DRAGGING)
		{
			if (ClassifyPoint(e->data.position.x, e->data.position.y) != POINT_NONE)
			{
				DragTo(e->data.position.x);
			}
		}
		break;

	case HTML_EVENT_MOUSEDOWN:
		SetFlag(DISPLAY_FLAG_FOCUS|DISPLAY_FLAG_DIRTY);
		Invalidate();
		where = ClassifyPoint(e->data.position.x, e->data.position.y);
		if (where == POINT_BEFORE)
		{
			BeginPullUp();
			PageUp();
		}
		else if (where == POINT_AFTER)
		{
			BeginPullDown();
			PageDown();

		}
		else if (where == POINT_INSIDE)
		{

			BeginDrag(e->data.position.x);
			if (GetManager())
			{
				GetManager()->ClaimPointer(this);
			}

		}
		break;

	case HTML_EVENT_MOUSEUP:
		if (state == STATE_DRAGGING)
		{
			if (GetManager())
			{
				GetManager()->ReleasePointer(this);
			}
		}
		state = STATE_NORMAL;
		Invalidate();
		break;

	case HTML_EVENT_FOCUS:
		SetFlag(DISPLAY_FLAG_FOCUS|DISPLAY_FLAG_DIRTY);
		break;

	case HTML_EVENT_UNFOCUS:
		ClearFlag(DISPLAY_FLAG_FOCUS);
		SetFlag(DISPLAY_FLAG_DIRTY);
		break;
	}

	return HTML_EVENT_STATUS_DONE;
}

void WebHSlider::DrawThisOnly(DISPLAY_INT x, DISPLAY_INT y, WebGraphics * gc)
{
	WebSlider::DrawThisOnly(x, y, gc);
	DISPLAY_INT start, end;
	GetSliderRange(&start, &end);

	WebRect sliderRect;
	sliderRect.top = y + webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_PADDING);
	sliderRect.bottom = y + WebSlider::Height() - 1 - webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_PADDING);
	sliderRect.left = x + start;
	sliderRect.right = x + end;
	DrawSlider(&sliderRect, gc);
}
