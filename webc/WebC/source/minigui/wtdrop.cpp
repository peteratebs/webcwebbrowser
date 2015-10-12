//
// WTDROP.CPP -
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
#include "wtdrop.hpp"
#include "util.h"
#include "dmanager.hpp"
#include "wgbmp.hpp"
#include "wgcolors.hpp"
#include "wgkeys.hpp"
#include "webcassert.h"


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

WebDropBox::WebDropBox ()
{
	mState = DROPBOX_STATE_NORMAL;
	mpText = WEBC_NULL;
	mSizeInvalid = WEBC_FALSE;
	miSelected = 0;
	miLastSelected = 0;
	mpListener = 0;

	SetFlag(DISPLAY_FLAG_ACCEPTS_FOCUS);
	InsertLast(&mButton);
	mListbox.AddListener(this);
	mButton.SetListener(this);
	mButton.ClearFlag(DISPLAY_FLAG_ACCEPTS_FOCUS);
    /* Switch triggering on the button to trigger on down, so the dropbox appears on click, not on button up */
    mButton.ClearButtonFlag(BUTTON_FLAG_UP_TRIGGER);
    mButton.SetButtonFlag(BUTTON_FLAG_DOWN_TRIGGER);
	mListbox.SetStyle(LISTBOX_STYLE_NOT3D
	                | LISTBOX_STYLE_NOFOCUSFRAME
	                | LISTBOX_STYLE_FOLLOW_MOUSE
	                | LISTBOX_STYLE_LIMITED_NOTIFY);
	mListbox.SetMargin(0);
	mListbox.SetBorder(1);
	mListbox.ClearFlag(DISPLAY_FLAG_ACCEPTS_FOCUS);
}

WebDropBox::~WebDropBox ()
{
}

void WebDropBox::SetManager(DisplayManager* manager)
{
	DisplayElement::SetManager(manager);

	WebGraphics* gc = GetGraphics();
	if (gc)
	{
		WebBitmap downArrow = gc->GetBitmapByIndex(WGB_DOWN_ARROW);
		mButton.SetDefaultBitmap(downArrow);
		mButton.SetDownBitmap(downArrow);
		mButton.SetOverBitmap(downArrow);
		if (downArrow)
		{
			mButton.SetWidth(10 + WEB_BITMAP_WIDTH(downArrow));
            /* Now set the slider width to be the same as the button width, so they line up */
			mListbox.SetSliderWidth(mButton.mRect.Width());
		}
	}
}

void WebDropBox::SetNumOptions(long i)
{
	mListbox.SetNumOptions(i);
}

void WebDropBox::SetOptionText(long n, const WebChar * text)
{
	mListbox.SetOptionText(n, text);
	if (n == GetSelected())
	{
		mpText = mListbox.GetOptionText(mListbox.GetSelected());
	}
}

WebChar * WebDropBox::GetOptionText(long n)
{
	return mListbox.GetOptionText(n);
}


void WebDropBox::Move(WebRect * rect)
{
	DisplayElement::Move(rect);
	WebRect r;
	GetContentRect(&r);
	r.left = r.right - mListbox.GetSliderWidth() + 1;
	mButton.Move(&r);
}

void WebDropBox::SizeTo (DISPLAY_INT w, DISPLAY_INT h)
{
	DisplayElement::SizeTo(w,h);
	WebRect r;
	GetContentRect(&r);
	r.left = r.right - mListbox.GetSliderWidth() + 1;
	mButton.Move(&r);
}

void WebDropBox::SetHeight (DISPLAY_INT h)
{
	DisplayElement::SetHeight(h);
	WebRect r;
	GetContentRect(&r);
	r.left = r.right - mListbox.GetSliderWidth() + 1;
	mButton.Move(&r);
}

void WebDropBox::SetWidth (DISPLAY_INT w)
{
	DisplayElement::SetWidth(w);
	WebRect r;
	GetContentRect(&r);
	r.left = r.right -  mListbox.GetSliderWidth() + 1;
	mButton.Move(&r);
}


HTMLEventStatus WebDropBox::Event (HTMLEvent *e)
{
    WEBC_DEBUG_ReceiveEvent(this, e);
	switch (e->type)
	{
	case HTML_EVENT_MOUSEDOWN:
		Drop();
		break;

	case HTML_EVENT_FOCUS:
		miLastSelected = miSelected;
		Invalidate();
		break;

	case HTML_EVENT_KEYDOWN:
		if (!(mFlags & DISPLAY_FLAG_DISABLED))
		{
			if (e->data.key == WGK_ESC)
			{
				Invalidate();
				Undrop();
			}
			else if (mState == DROPBOX_STATE_DROPPED)
			{
				return (mListbox.Event(e));
			}
			else
			{
				switch (e->data.key)
				{
				case ' ':
					switch (mState)
					{
					case DROPBOX_STATE_DROPPED:
						Invalidate();
						Undrop();
						break;

					case DROPBOX_STATE_NORMAL:
						Invalidate();
						Drop();
						break;
					}
					break;

				case WGK_CR:
					if (miLastSelected != miSelected)
					{
						miLastSelected = miSelected;
						NotifySelectChange();
					}
					break;

				case WGK_LNDN:
					SetSelected(GetSelected() + 1);
					Invalidate();
					break;

				case WGK_LNUP:
					SetSelected(GetSelected() - 1);
					Invalidate();
					break;

				default:
					{
						long index = mListbox.NextIndexStartingWith((WebChar) e->data.key);
						if (index >= 0)
						{
							SetSelected(index);
							Invalidate();
						}
						break;
					}
				}
			}
		}
		break;

	case HTML_EVENT_UNFOCUS:
		Invalidate();
		Undrop();
		if (miLastSelected != miSelected)
		{
			miLastSelected = miSelected;
			NotifySelectChange();
		}
		break;

	default:
		break;
	}
	return HTML_EVENT_STATUS_DONE;
}
void WebDropBox::DrawThisOnly (DISPLAY_INT x, DISPLAY_INT y, WebGraphics *gc)
{
	WEBC_BOOL focused = (mFlags & DISPLAY_FLAG_FOCUS) || (mState == DROPBOX_STATE_DROPPED);

	mListbox.CalculateTextMetrics(gc);

	WebRect oldClip;
	gc->GetClip(&oldClip);
	WebRect clip;
	clip.top = y;
	clip.left = x;
	clip.bottom = y + Height() - 1;
	clip.right = x + Width() - 1;
	clip.And(&oldClip);

	gc->SetClip(&clip);

	WebRect rect;
	rect.top = y;
	rect.left = x;
	rect.bottom = y + Height() - 1;
	rect.right = x + Width() - 1;

//	gc->StartBuffer();

	WebColor hilite, lolite, blue, white, black;
	HTMLColor col = HTML_RGBAToColor(0,0,0,0);
	black = hilite = gc->RGBToColor(col);

	col = HTML_RGBAToColor(0xff,0xff,0xff,0);
	white = lolite = gc->RGBToColor(col);

	col = HTML_RGBAToColor(0,0,0xff,0);
	blue = gc->RGBToColor(col);

	if (focused)
	{
		gc->Rectangle(&rect, black, black, WEBC_FALSE);
	}

	GetFrameRect(&rect);
	rect.Shift(x,y);
	DrawFrame(&rect, gc);

	GetContentRect(&rect);
	rect.Shift(x,y);

	if (focused)
	{
		gc->Rectangle(&rect, GetSelectColor(gc), GetSelectColor(gc), WEBC_TRUE);
	}
	else
	{
		gc->Rectangle(&rect, GetBgColor(gc), GetBgColor(gc), WEBC_TRUE);
	}

	if (mpText && mFont.GetFont())
	{
		DISPLAY_INT offset = (rect.Height() - gc->TextHeight(mpText, mFont.GetFont())) / 2;

		clip.Set(&rect);
		clip.And(&oldClip);
		gc->SetClip(&clip);
		gc->Text(rect.left + DROPBOX_PADDING,
			 rect.top + DROPBOX_PADDING + offset,
			 mpText, (focused) ? GetBgColor(gc) : GetTextColor(gc),
			 0, WEBC_FALSE, mFont.GetFont());
	}

//	gc->EndBuffer();
	gc->SetClip(&oldClip);

}

void WebDropBox::SetSelected(long l)
{
	if (!(mFlags & DISPLAY_FLAG_READONLY))
	{
		mListbox.SetSelected(l);
		miSelected = mListbox.GetSelected();
		mpText = mListbox.GetOptionText(miSelected);
		Invalidate();
	}
}

long WebDropBox::GetSelected(void)
{
	return (miSelected);
}

void WebDropBox::SetFont(WebGraphFont *f)
{
	mListbox.SetFont(f);
	mFont.Set(f);
}

void WebDropBox::Notify(Notifier* p, NotifyEvent *what)
{
    WEBC_DEBUG_ReceiveNotify(this, 0, what);
	if ((WebButton*)p == &mButton)
	{
		if (mButton.Activated())
		{
			if (mState == DROPBOX_STATE_NORMAL)
			{
				Drop();
			}
			else if (mState == DROPBOX_STATE_DROPPED)
			{
				Undrop();
				mpText = mListbox.GetOptionText(mListbox.GetSelected());
			}
			mButton.Deactivate();
			mButton.ClearFlag(DISPLAY_FLAG_FOCUS);
		}
	}
	else if ((WebListBox*)p == &mListbox)
	{
		switch (what->event)
		{
		case NOTIFY_SELECT_CHANGE:
			Undrop();
			if (miSelected != mListbox.GetSelected())
			{
				miSelected = mListbox.GetSelected();
				mpText = mListbox.GetOptionText(mListbox.GetSelected());
				miLastSelected = miSelected;
				NotifySelectChange();
			}
			break;

		default:
			break;
		}
	}

	DisplayElement::Notify(p, what);
}

void WebDropBox::CalculateAvailableSpace(int * up, int * down)
{
	WEBC_ASSERT(GetManager());
	DISPLAY_INT x, y;
	x = mRect.left;
	y = mRect.top;
	if (mpParent)
	{
		mpParent->GetDisplayPosition(this, &x, &y);
	}

	*up = y - GetManager()->mViewRect.top;
	*down = GetManager()->mViewRect.bottom - y - Height();
}


void WebDropBox::Drop(void)
{
	WEBC_ASSERT(GetManager());
	if (mState == DROPBOX_STATE_DROPPED)
	{
		return;
	}

	int spaceUp, spaceDown;
	CalculateAvailableSpace(&spaceUp, &spaceDown);

	int neededSpace = mListbox.GetTotalTextHeight() +
		mListbox.GetMargin() + mListbox.GetPadding() * 2 +
		mListbox.GetBorder() * 2;

	WebRect rect;
	if (spaceDown >= neededSpace || spaceDown > spaceUp)
	{
		DISPLAY_INT space = EBSMIN(spaceDown, neededSpace);
		rect.top = Height() - DROPBOX_MARGIN;
		rect.left = DROPBOX_MARGIN;
		rect.right = Width() - DROPBOX_MARGIN - 1;
		rect.bottom = rect.top + space - 1;
	}
	else
	{
		DISPLAY_INT space = EBSMIN(spaceUp, neededSpace);
		rect.top = -space;
		rect.left = DROPBOX_MARGIN;
		rect.right = Width() - DROPBOX_MARGIN - 1;
		rect.bottom = DROPBOX_MARGIN - 1;
	}


	SetFlag(DISPLAY_FLAG_DIRTY);
	Invalidate();
	mState = DROPBOX_STATE_DROPPED;

	if (mpParent)
	{
		DISPLAY_INT left,top;
		mpParent->GetDisplayPosition(this, &left, &top);
		rect.Shift(left,top);
	}

	mListbox.Move(&rect);
	DisplayManager *m = GetManager();
	WebGraphics *gc = m? m->GetGraphics() : 0;
	if (gc)
	{
		mListbox.SetupScrollBars(gc);
	}

	// we need to make sure the list box appears on top of everything.
	DisplayElement *parent = this;
	while (parent->mpParent)
	{
		parent = parent->mpParent;
	}
	parent->InsertLast(&mListbox);

	mListbox.Invalidate();
}

void WebDropBox::Undrop(void)
{
	if (GetManager())
	{

		if (mState == DROPBOX_STATE_NORMAL)
		{
			return;
		}
		SetFlag(DISPLAY_FLAG_DIRTY);
		mState = DROPBOX_STATE_NORMAL;
		mListbox.Invalidate();
		if (mListbox.mpParent)
		{
			mListbox.mpParent->Remove(&mListbox);
		}
		Invalidate();
	}
}

void WebDropBox::GetContentRect(WebRect * rect)
{
}

void WebDropBox::GetFrameRect(WebRect * r)
{
	r->Set(DROPBOX_MARGIN, DROPBOX_MARGIN, Width()  - (DROPBOX_MARGIN + 1), Height() - (DROPBOX_MARGIN + 1));
}

void WebDropBox::DrawFrame(WebRect *rect, WebGraphics *gc)
{
WebColor hilite = gc->LightColor(gc->DarkColor(GetBgColor(gc)));
WebColor lolite = gc->DarkColor(hilite);

	gc->Frame3D(rect, lolite, hilite, DROPBOX_FRAME_WIDTH);
}

void WebDropBox::Enable (void)
{
	DisplayElement::Enable();
	mButton.Enable();
	mListbox.Enable();
}

void WebDropBox::Disable (void)
{
	DisplayElement::Disable();
	mButton.Disable();
	mListbox.Disable();
}

void WebDropBox::NotifySelectChange (void)
{
	if (mpListener)
	{
		NotifyEvent what(NOTIFIER_TYPE_SELECT, NOTIFY_SELECT_CHANGE);
		mpListener->Notify((WebDropBox *) this, &what);
	}
}

void WebDropBox::AddListener(Listener * l)
{
	mpListener = l;
}

WebColor WebDropBox::GetTextColor   (WebGraphics* gc)
{
	return gc->GetColorByIndex(WGC_BLACK);
}

WebColor WebDropBox::GetBgColor     (WebGraphics* gc)
{
	return gc->GetColorByIndex(WGC_WHITE);
}

WebColor WebDropBox::GetSelectColor (WebGraphics* gc)
{
	return gc->GetColorByIndex(WGC_LIGHTBLUE);
}

WebColor WebDropList::GetTextColor (WebGraphics* gc)
{
	return mpListener? ((WebDropBox*)mpListener)->GetTextColor(gc) : WebListBox::GetTextColor(gc);
}

WebColor WebDropList::GetBgColor (WebGraphics* gc)
{
	return mpListener? ((WebDropBox*)mpListener)->GetBgColor(gc) : WebListBox::GetBgColor(gc);
}

WebColor WebDropList::GetSelectColor (WebGraphics* gc)
{
	return mpListener? ((WebDropBox*)mpListener)->GetSelectColor(gc) : WebListBox::GetSelectColor(gc);
}
