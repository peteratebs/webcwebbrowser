//
// WTCHECK.CPP -
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
#include "wtcheck.hpp"
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

WebCheckBox::WebCheckBox ()
{
	PresetWebRect r(0, 0, WGB_CHECKBOX_ON_WIDTH + 6, WGB_CHECKBOX_ON_HEIGHT + 6);
	Move(&r);
	SetFlag(DISPLAY_FLAG_ACCEPTS_FOCUS);
}

WebCheckBox::~WebCheckBox ()
{
}


HTMLEventStatus WebCheckBox::Event (HTMLEvent *e)
{
    WEBC_DEBUG_ReceiveEvent(this, e);
	switch (e->type)
	{
		case HTML_EVENT_MOUSEDOWN:
			if (!(mFlags & DISPLAY_FLAG_DISABLED))
			{
				Toggle();
			}
			break;

		case HTML_EVENT_KEYDOWN:
			if (!(mFlags & DISPLAY_FLAG_DISABLED))
			{
				if (e->data.key == WGK_CR)
				{
					Toggle();
				}
			}
			break;

		case HTML_EVENT_FOCUS:
			SetFlag(DISPLAY_FLAG_FOCUS);
			Invalidate();
			break;

		case HTML_EVENT_UNFOCUS:
			ClearFlag(DISPLAY_FLAG_FOCUS);
			Invalidate();
			break;

		default:
			break;
	}
	return HTML_EVENT_STATUS_DONE;
}

void WebCheckBox::DrawThisOnly (DISPLAY_INT x, DISPLAY_INT y, WebGraphics *gc)
{
WebBitmap bmp;

	if (GetChecked())
	{
		if (mFlags & DISPLAY_FLAG_DISABLED)
		{
			bmp = gc->GetBitmapByIndex(WGB_CHECKBOX_ON_DISABLED);
		}
		else
		{
			bmp = gc->GetBitmapByIndex(WGB_CHECKBOX_ON);
		}
	}
	else
	{
		if (mFlags & DISPLAY_FLAG_DISABLED)
		{
			bmp = gc->GetBitmapByIndex(WGB_CHECKBOX_OFF_DISABLED);
		}
		else
		{
			bmp = gc->GetBitmapByIndex(WGB_CHECKBOX_OFF);
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

void WebCheckBox::Toggle (void)
{
	if (GetChecked())
	{
		selected = WEBC_FALSE;
	}
	else
	{
		selected = WEBC_TRUE;
	}
	Invalidate();
}

WEBC_BOOL WebCheckBox::GetChecked (void)
{
	return selected;
}

void WebCheckBox::SetChecked (WEBC_BOOL on)
{
	if (selected != on)
	{
		selected = on;
		Invalidate();
	}
}
