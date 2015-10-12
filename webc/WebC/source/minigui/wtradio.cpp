//
// WTRADIO.CPP -
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
#include "wtradio.hpp"
#include "wgbmp.hpp"
#include "wgcolors.hpp"
#include "wgkeys.hpp"


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

WebRadioButton::WebRadioButton ()
{
	mbSelected = WEBC_FALSE;
	mpListener = 0;

	SetFlag(DISPLAY_FLAG_ACCEPTS_FOCUS);
	SetWidth(WGB_RADIO_ON_WIDTH + 6);
	SetHeight(WGB_RADIO_ON_HEIGHT + 6);
}

WebRadioButton::~WebRadioButton()
{
}

HTMLEventStatus WebRadioButton::Event(HTMLEvent * e)
{
    WEBC_DEBUG_ReceiveEvent(this, e);
	switch (e->type)
	{
	case HTML_EVENT_MOUSEDOWN:
		if (mFlags & DISPLAY_FLAG_DISABLED)
		{
			break;
		}
		SetSelected(WEBC_TRUE);
		break;

	case HTML_EVENT_KEYDOWN:
		if (mFlags & DISPLAY_FLAG_DISABLED)
		{
			break;
		}
		if (e->data.key == WGK_CR)
		{
			SetSelected(WEBC_TRUE);
		}
		break;

	case HTML_EVENT_FOCUS:
		SetFlag(DISPLAY_FLAG_FOCUS|DISPLAY_FLAG_DIRTY);
		Invalidate();
		break;

	case HTML_EVENT_UNFOCUS:
		ClearFlag(DISPLAY_FLAG_FOCUS);
		SetFlag(DISPLAY_FLAG_DIRTY);
		Invalidate();
		break;
	default:
		break;
	}
	return HTML_EVENT_STATUS_DONE;
}

void WebRadioButton::DrawThisOnly(DISPLAY_INT x, DISPLAY_INT y, WebGraphics * gc)
{
	WebBitmap bmp;
	if (GetSelected())
	{
		if (mFlags & DISPLAY_FLAG_DISABLED)
		{
			bmp = gc->GetBitmapByIndex(WGB_RADIO_ON_DISABLED);
		}
		else
		{
			bmp = gc->GetBitmapByIndex(WGB_RADIO_ON);
		}
	}
	else
	{
		if (mFlags & DISPLAY_FLAG_DISABLED)
		{
			bmp = gc->GetBitmapByIndex(WGB_RADIO_OFF_DISABLED);
		}
		else
		{
			bmp = gc->GetBitmapByIndex(WGB_RADIO_OFF);
		}
	}

//	gc->StartBuffer();
	gc->Bitmap(x + 3, y + 3, bmp);

	if (mFlags & DISPLAY_FLAG_FOCUS)
	{
		WebRect Rect;
		Rect.Set(x+1, y+1, x + WEB_BITMAP_WIDTH(bmp)+4 - 1, y + WEB_BITMAP_HEIGHT(bmp)+4 - 1);
		gc->Rectangle(&Rect, gc->GetColorByIndex(WGC_BLACK), gc->GetColorByIndex(WGC_WHITE), 0);
	}

//	gc->EndBuffer();
	ClearFlag(DISPLAY_FLAG_DIRTY);
}

void WebRadioButton::SetSelected(WEBC_BOOL status)
{
	if (status != mbSelected)
	{
		mbSelected = status;
		SetFlag(DISPLAY_FLAG_DIRTY);
		Invalidate();

		if (mpListener)
		{
			NotifyEvent e(NOTIFIER_TYPE_RADIO_BUTTON, NOTIFY_SELECT_CHANGE);
			mpListener->Notify( ((Notifier*)((WebRadioButton*)this)), &e );
		}
	}
}

WEBC_BOOL WebRadioButton::GetSelected(void)
{
	return mbSelected;
}

void WebRadioButton::SetListener (Listener *listener)
{
	mpListener = listener;
}
