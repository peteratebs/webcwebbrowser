//
// WTBUTTON.CPP -
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
#include "wtbutton.hpp"
#include "dmanager.hpp"
#include "wgcolors.hpp"
#include "wgkeys.hpp"
#include "css.h"
#include "webcstr.h"
#include "objref.cpp"
#include "webcmem.h"
#include "webcdefault.h"


/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/

#define BUTTON_BORDER   mBorderWidth
#define BUTTON_PADDING  mPadding


/*****************************************************************************/
// Local Types
/*****************************************************************************/


/*****************************************************************************/
// Global Data
/*****************************************************************************/


/*****************************************************************************/
// Local functions
/*****************************************************************************/

WebButton::WebButton ()
{
	mListener = 0;
	mButtonFlags = BUTTON_FLAG_SINK_FACE | BUTTON_FLAG_UP_TRIGGER;
	SetFlag(DISPLAY_FLAG_ACCEPTS_FOCUS);
}

WebButton::~WebButton ()
{
}

HTMLEventStatus WebButton::Event (HTMLEvent *e)
{
    WEBC_DEBUG_ReceiveEvent(this, e);
	switch (e->type)
	{
		case HTML_EVENT_MOUSEDOWN:
			if (!(mFlags & DISPLAY_FLAG_DISABLED))
			{
				SetButtonFlag(BUTTON_FLAG_MOUSEDOWN|BUTTON_FLAG_DOWN);

				Invalidate();

				if (GetButtonFlags() & BUTTON_FLAG_DOWN_TRIGGER)
				{
					Activate();
				}

				if (GetManager())
				{
					GetManager()->ClaimPointer(this);
				}
			}
			break;

		case HTML_EVENT_MOUSEUP:
			if (!(mFlags & DISPLAY_FLAG_DISABLED))
			{
				ClearButtonFlag(BUTTON_FLAG_MOUSEDOWN);

				if (GetManager())
				{
					GetManager()->ReleasePointer(this);
				}

				Invalidate();

				if (GetButtonFlags() & BUTTON_FLAG_DOWN)
				{
					ClearButtonFlag(BUTTON_FLAG_DOWN);
					if (GetButtonFlags() & BUTTON_FLAG_UP_TRIGGER)
					{
						Activate();
					}
				}
			}
			break;

		case HTML_EVENT_MOUSEOUT:
			ClearButtonFlag(BUTTON_FLAG_DOWN|BUTTON_FLAG_OVER);
			Invalidate();
			break;

		case HTML_EVENT_MOUSEOVER:
			SetButtonFlag(BUTTON_FLAG_OVER);
			if (!(mFlags & DISPLAY_FLAG_DISABLED))
			{
				if (GetButtonFlags() & BUTTON_FLAG_MOUSEDOWN)
				{
					SetButtonFlag(BUTTON_FLAG_DOWN);
				}
			}
			Invalidate();
			break;

		case HTML_EVENT_KEYDOWN:
			if (!(mFlags & DISPLAY_FLAG_DISABLED))
			{
				if (e->data.key == WGK_CR)
				{
					SetButtonFlag(BUTTON_FLAG_DOWN);
					if (GetButtonFlags() & BUTTON_FLAG_DOWN_TRIGGER)
					{
						Activate();
					}
					Invalidate();
				}
			}
			break;

		case HTML_EVENT_KEYUP:
			if (!(mFlags & DISPLAY_FLAG_DISABLED))
			{
				if ((e->data.key == WGK_CR) && (GetButtonFlags() & BUTTON_FLAG_DOWN))
				{
					ClearButtonFlag(BUTTON_FLAG_DOWN);
					Invalidate();
					if (GetButtonFlags() & BUTTON_FLAG_UP_TRIGGER)
					{
						Activate();
					}
				}
			}
			break;

		case HTML_EVENT_FOCUS:
			SetFlag(DISPLAY_FLAG_FOCUS);
			Invalidate();
			break;

		case HTML_EVENT_UNFOCUS:
			ClearButtonFlag(BUTTON_FLAG_DOWN);
			ClearFlag(DISPLAY_FLAG_FOCUS);
			Invalidate();
			break;

		default:
			break;
	}
	return HTML_EVENT_STATUS_DONE;
}

WEBC_BOOL WebButton::Activated (void)
{
	return ((GetButtonFlags() & BUTTON_FLAG_ACTIVATED) != 0);
}

void WebButton::Deactivate (void)
{
	ClearButtonFlag(BUTTON_FLAG_ACTIVATED);
}

void WebButton::ClearButtonFlag(WEB_BUTTON_FLAGS f)
{
	if ((mButtonFlags & BUTTON_FLAG_DOWN) && (f & BUTTON_FLAG_DOWN))
	{
		mButtonFlags &= ~f;
		if (mListener)
		{
			NotifyEvent what(NOTIFIER_TYPE_BUTTON, NOTIFY_BUTTON_UP);
			mListener->Notify(this, &what);
		}
	}

	mButtonFlags &= ~f;
}

void WebButton::SetButtonFlag(WEB_BUTTON_FLAGS f)
{
	if ((f & BUTTON_FLAG_DOWN) && !(mButtonFlags & BUTTON_FLAG_DOWN))
	{
		mButtonFlags |= f;
		if (mListener)
		{
			NotifyEvent what(NOTIFIER_TYPE_BUTTON, NOTIFY_BUTTON_DOWN);
			mListener->Notify(this, &what);
		}
	}

	mButtonFlags |= f;
}

WEB_BUTTON_FLAGS WebButton::GetButtonFlags(void)
{
	return mButtonFlags;
}

void WebButton::SetListener (Listener *pWho)
{
	mListener = pWho;
}

void WebButton::Activate (void)
{
	SetButtonFlag(BUTTON_FLAG_ACTIVATED);
	if (mListener)
	{
		NotifyEvent what(NOTIFIER_TYPE_BUTTON, NOTIFY_BUTTON_PRESS);
		mListener->Notify(this, &what);
	}
}


//=============================================================================
// Web3DButton methods
//=============================================================================

//-----------------------------------------------------------------------------
Web3DButton::Web3DButton ()
{
	mBorderWidth = webc_GetDefaultDisplayInt(WEBC_DEFAULT_BUTTON_FRAME_WIDTH);
	mPadding = webc_GetDefaultDisplayInt(WEBC_DEFAULT_BUTTON_PADDING);
}

//-----------------------------------------------------------------------------
Web3DButton::~Web3DButton ()
{
}

//-----------------------------------------------------------------------------
void Web3DButton::DrawThisOnly (DISPLAY_INT x, DISPLAY_INT y, WebGraphics *gc)
{
	WebRect box(mRect);
	box.MoveTo(x, y);
	gc->Rectangle(&box, GetBackgroundColor(gc), GetBackgroundColor(gc), 1);
	DrawBorder(x, y, gc);

	DISPLAY_INT borderWidth = webc_GetDefaultDisplayInt(WEBC_DEFAULT_BUTTON_FRAME_WIDTH);

	box.Shift(borderWidth, borderWidth);
	box.SizeBy(-(borderWidth * 2), -(borderWidth * 2));

	WebRect clip;
	gc->GetClip(&clip);
	if (clip.Overlaps(&box))
	{
		box.And(&clip);
		gc->SetClip(&box);
		DrawButtonFace(x,y,gc);
		gc->SetClip(&clip);
	}
}

//-----------------------------------------------------------------------------
void Web3DButton::DrawBorder(DISPLAY_INT x, DISPLAY_INT y, WebGraphics *gc)
{
	WebColor hilite = GetHiliteColor(gc), lolite = GetShadowColor(gc);
	WebRect box(mRect);
	box.MoveTo(x, y);

	// invert the frame when the button is down.
	if (GetButtonFlags() & BUTTON_FLAG_DOWN)
	{
		gc->Frame3D(&box, lolite, hilite, BUTTON_BORDER);
	}
	else if (!(GetButtonFlags() & BUTTON_FLAG_MOUSEDOWN))
	{
		gc->Frame3D(&box, hilite, lolite, BUTTON_BORDER);
	}
}

//-----------------------------------------------------------------------------
void Web3DButton::DrawButtonFace(DISPLAY_INT x, DISPLAY_INT y, WebGraphics *gc)
{
}

//-----------------------------------------------------------------------------
void Web3DButton::SetBorderWidth(DISPLAY_INT width)
{
	mBorderWidth = EBSMAX(0,width);
}

//-----------------------------------------------------------------------------
void Web3DButton::SetPadding(DISPLAY_INT width)
{
	mPadding = EBSMAX(0,width);
}

//-----------------------------------------------------------------------------
WebColor Web3DButton::GetBackgroundColor(WebGraphics* gc)
{
	return gc->RGBToColor(webc_GetDefaultColor(WEBC_DEFAULT_BUTTON_BACKGROUND_COLOR));
}

//-----------------------------------------------------------------------------
WebColor Web3DButton::GetHiliteColor (WebGraphics* gc)
{
	return gc->RGBToColor(webc_GetDefaultColor(WEBC_DEFAULT_BUTTON_HILITE_COLOR));
}

//-----------------------------------------------------------------------------
WebColor Web3DButton::GetShadowColor (WebGraphics* gc)
{
	return gc->RGBToColor(webc_GetDefaultColor(WEBC_DEFAULT_BUTTON_SHADOW_COLOR));
}


//=============================================================================
// Web3DTextButton methods
//=============================================================================

//-----------------------------------------------------------------------------
Web3DTextButton::Web3DTextButton ()
{
	mText = 0;
	mFont = 0;
	mTextSize = 0;
}

//-----------------------------------------------------------------------------
Web3DTextButton::~Web3DTextButton ()
{
	if (mText)
	{
		WEBC_FREE(mText);
	}
}

//-----------------------------------------------------------------------------
void Web3DTextButton::DrawButtonFace(DISPLAY_INT x, DISPLAY_INT y, WebGraphics *gc)
{
	DISPLAY_INT textX, textY, textW, textH;
	WebRect box(mRect);
	box.MoveTo(x, y);

	// center the text
	WebFont font = mFont.GetFont();
	if (font && mText)
	{
		textW = gc->TextWidth(mText, font);
		textH = gc->TextHeight(mText, font);
		textX = x + (mRect.Width()>>1) - (textW>>1);
		textY = y + (mRect.Height()>>1) - (textH>>1);

		// when the button is down, draw the text slighly lower and to the right
		if ((GetButtonFlags() & BUTTON_FLAG_DOWN) && (GetButtonFlags() & BUTTON_FLAG_SINK_FACE))
		{
			textX++;
			textY++;
		}

		if (mFlags & DISPLAY_FLAG_DISABLED)
		{
			//WebColor c = gc->LightColor(GetTextColor(gc));
			WebColor c = GetTextColor(gc);

			gc->Text(textX, textY, mText, c, gc->GetColorByIndex(WGC_LIGHTGRAY), 0, font);
		}
		else
		{
			gc->Text(textX, textY, mText, GetTextColor(gc), gc->GetColorByIndex(WGC_LIGHTGRAY), 0, font);

			// now draw the focus indicator
			if (mFlags & DISPLAY_FLAG_FOCUS)
			{
				box.Set(box.left + 3, box.top + 3, box.right - 3, box.bottom - 3);
				gc->PatternFrame(&box, gc->GetColorByIndex(WGC_BLACK), 1, CSS_BORDER_STYLE_DOTTED);
			}
		}
	}
}

//-----------------------------------------------------------------------------
void Web3DTextButton::SetText (const WebChar *label)
{
	long len = (label) ? webc_strlen(label) : 1;

	if (mTextSize <= len)
	{
		long iNewSize = ((len + 1) + 0x1f) & (~0x1f);
		WebChar *pNew = (WebChar *) WEBC_MALLOC(iNewSize * sizeof(WebChar));
		if (pNew)
		{
			if (mText)
			{
				WEBC_FREE(mText);
			}
			mText = pNew;
			mTextSize = iNewSize;
		}
	}

	if (mText)
	{
		if (label)
		{
			webc_strncpy(mText, label, mTextSize);
		}
		else
		{
			mText[0] = (WebChar) '\0';
		}
	}
}

//-----------------------------------------------------------------------------
void Web3DTextButton::SetFont (WebGraphFont *font)
{
	mFont.Set(font);
}

//-----------------------------------------------------------------------------
WebChar *Web3DTextButton::GetText (void)
{
	return (mText);
}

//-----------------------------------------------------------------------------
WebColor Web3DTextButton::GetTextColor(WebGraphics* gc)
{
	return gc->GetColorByIndex(WGC_BLACK);
}


//=============================================================================
// Web3DBitmapButton methods
//=============================================================================

//-----------------------------------------------------------------------------
Web3DBitmapButton::Web3DBitmapButton ()
{
	mDefaultBitmap = 0;
	mOverBitmap = 0;
	mDownBitmap = 0;
	mDisabledBitmap = 0;
}

//-----------------------------------------------------------------------------
Web3DBitmapButton::~Web3DBitmapButton ()
{
}

//-----------------------------------------------------------------------------
void Web3DBitmapButton::DrawButtonFace(DISPLAY_INT x, DISPLAY_INT y, WebGraphics *gc)
{
	DISPLAY_INT bmpX, bmpY, bmpW, bmpH;
	WebRect box(mRect);
	WebBitmap bmp;
	box.MoveTo(x, y);

	if (mFlags & DISPLAY_FLAG_DISABLED)
	{
		bmp = mDisabledBitmap;
	}
	if (GetButtonFlags() & BUTTON_FLAG_DOWN)
	{
		bmp = mDownBitmap;
	}
	else if (GetButtonFlags() & BUTTON_FLAG_OVER)
	{
		bmp = mOverBitmap;
	}
	else
	{
		bmp = mDefaultBitmap;
	}

	// center the text
	if (bmp)
	{
		bmpW = WEB_BITMAP_WIDTH(bmp);
		bmpH = WEB_BITMAP_HEIGHT(bmp);
		bmpX = x + (mRect.Width()>>1) - (bmpW>>1);
		bmpY = y + (mRect.Height()>>1) - (bmpH>>1);

		// when the button is down, draw the text slighly lower and to the right
		if ((GetButtonFlags() & BUTTON_FLAG_DOWN) && (GetButtonFlags() & BUTTON_FLAG_SINK_FACE))
		{
			bmpX++;
			bmpY++;
		}

		gc->Bitmap(bmpX, bmpY, bmp);

		// now draw the focus indicator
		if (mFlags & DISPLAY_FLAG_FOCUS)
		{
			box.Set(box.left + 3, box.top + 3, box.right - 3, box.bottom - 3);
			gc->Rectangle(&box, gc->GetColorByIndex(WGC_DARKGRAY), 0, 0);
		}
	}
}

//-----------------------------------------------------------------------------
void Web3DBitmapButton::SetDefaultBitmap (WebBitmap pBitmap)
{
	mDefaultBitmap = pBitmap;
}

//-----------------------------------------------------------------------------
void Web3DBitmapButton::SetOverBitmap (WebBitmap pBitmap)
{
	mOverBitmap = pBitmap;
}

//-----------------------------------------------------------------------------
void Web3DBitmapButton::SetDownBitmap (WebBitmap pBitmap)
{
	mDownBitmap = pBitmap;
}

//-----------------------------------------------------------------------------
void Web3DBitmapButton::SetDisabledBitmap (WebBitmap pBitmap)
{
	mDisabledBitmap = pBitmap;
}
