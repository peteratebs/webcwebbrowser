//
// WTLIST.CPP -
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
#include "wtlist.hpp"
#include "webc.h"
#include "util.h"
#include "wgcolors.hpp"
#include "wgkeys.hpp"
#include "ref.hpp"
#include "webcstr.h"
#include "objref.cpp"
#include "webcmem.h"


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

WebListBox::WebListBox ()
{
	miMouseOver = 0;
	miSelected = 0;
	mppOption = WEBC_NULL;
	miNumOptions = 0;
	mpVScroll = WEBC_NULL;
	mpHScroll = WEBC_NULL;
    miSliderWidth = webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_WIDTH);
	miTextHeight = miTotalTextHeight = miTotalTextWidth = -1;
	miLongestOption = 0;
	mpListener = WEBC_NULL;
	mStyle = 0;


	SetFlag(DISPLAY_FLAG_ACCEPTS_FOCUS);
	SetMargin(LISTBOX_MARGIN);
	SetPadding(LISTBOX_PADDING);
	SetBorder(LISTBOX_BORDER);
	SetSpacing(LISTBOX_SPACING);

}

WebListBox::~WebListBox ()
{
	for (int t = 0; t < miNumOptions; t++)
	{
		WEBC_DELETE(mppOption[t]);
	}

	WEBC_DELETE(mppOption);

	if (mpVScroll != WEBC_NULL)
	{
		WEBC_DELETE(mpVScroll);
	}

	if (mpHScroll != WEBC_NULL)
	{
		WEBC_DELETE(mpHScroll);
	}
}

DISPLAY_INT WebListBox::GetMargin(void)
{
	return miMargin;
}

DISPLAY_INT WebListBox::GetSpacing(void)
{
	return miSpacing;
}

DISPLAY_INT WebListBox::GetBorder(void)
{
	return miBorder;
}

DISPLAY_INT WebListBox::GetPadding(void)
{
	return miPadding;
}

void WebListBox::SetMargin(DISPLAY_INT l)
{
	miMargin = l;
}

void WebListBox::SetPadding(DISPLAY_INT l)
{
	miPadding = l;
}

void WebListBox::SetSpacing(DISPLAY_INT l)
{
	miSpacing = l;
}

void WebListBox::SetBorder(DISPLAY_INT l)
{
	miBorder = l;
}

void WebListBox::SetSliderWidth(DISPLAY_INT NewWidth)
{
    miSliderWidth = NewWidth;
}

DISPLAY_INT  WebListBox::GetSliderWidth(void)
{
    return miSliderWidth;
}

void WebListBox::AddListener(Listener * l)
{
	mpListener = l;
}

void WebListBox::SetStyle(WEBC_UINT8 f)
{
	mStyle = f;
}

long WebListBox::OptionAt(DISPLAY_INT x, DISPLAY_INT y)
{
	DISPLAY_INT offsetX, offsetY;
	DISPLAY_INT offX = 0, offY = 0;

	if (mpParent)
	{
		mpParent->GetDisplayPosition(this, &offX, &offY);
	}

	WebRect box;
	GetOptionsRect(&box);
	y -= offY + box.top;
	GetScrollOffset(&offsetX, &offsetY);
	y += offsetY;
	return (y / (GetSpacing() + miTextHeight));
}

HTMLEventStatus WebListBox::Event (HTMLEvent *e)
{

    WEBC_DEBUG_ReceiveEvent(this, e);
	ObjectReference<WebListBox> self(this);

	switch (e->type)
	{
/*	case HTML_EVENT_MOUSEDOWN: changed July 1, 2007 make UP the trigger, same as explorer */
	case HTML_EVENT_MOUSEUP:
		if (!(mFlags & DISPLAY_FLAG_DISABLED))
		{
			if (SetSelected(OptionAt(e->data.position.x, e->data.position.y)))
			{
				NotifySelectChange();
				if (!self.Get())
				{
					return HTML_EVENT_STATUS_HALT;
				}
			}
		}
		break;

	case HTML_EVENT_MOUSEMOVE:
		if ((mStyle & LISTBOX_STYLE_FOLLOW_MOUSE) && !(mFlags & DISPLAY_FLAG_READONLY))
		{
			miMouseOver = OptionAt(e->data.position.x, e->data.position.y);
			Invalidate();
		}
		break;

	case HTML_EVENT_KEYDOWN:
		if (!(mFlags & DISPLAY_FLAG_DISABLED))
		{
			switch (e->data.key)
			{
			case WGK_LNDN:
				if (SetSelected(GetSelected() + 1) &&
				    !(mStyle & LISTBOX_STYLE_LIMITED_NOTIFY))
				{
					NotifySelectChange();
					if (!self.Get())
					{
						return HTML_EVENT_STATUS_HALT;
					}
				}
				break;

			case WGK_LNUP:
				if (SetSelected(GetSelected() - 1) &&
				    !(mStyle & LISTBOX_STYLE_LIMITED_NOTIFY))
				{
					NotifySelectChange();
					if (!self.Get())
					{
						return HTML_EVENT_STATUS_HALT;
					}
				}
				break;

			case WGK_CR:
				NotifySelectChange();
				if (!self.Get())
				{
					return HTML_EVENT_STATUS_HALT;
				}
				break;

			default:
				if (!(mFlags & DISPLAY_FLAG_READONLY))
				{
					long index = NextIndexStartingWith((WebChar) e->data.key);
					if (index >= 0)
					{
						SetSelected(index);
						Invalidate();
					}
					break;
				}
			}
		}
		break;

	case HTML_EVENT_FOCUS:
		Invalidate();
		break;

	case HTML_EVENT_UNFOCUS:
		if (mpListener)
		{
			NotifyEvent what(NOTIFIER_TYPE_LIST_BOX, NOTIFY_FOCUS_LOST);
			mpListener->Notify((WebListBox*)this, &what);
			if (!self.Get())
			{
				return HTML_EVENT_STATUS_HALT;
			}
		}
		Invalidate();
		break;
	default:
		break;
	}

	return HTML_EVENT_STATUS_DONE;
}

void WebListBox::FitSelection(void)
{
	if (UseVScroll())
	{
		DISPLAY_INT offsetX, offsetY;
		WebRect r;

		GetScrollOffset(&offsetX, &offsetY);
		GetOptionsRect(&r);

		int pos = miSelected * (miTextHeight + GetSpacing());
		if (pos < offsetY)
		{
			mpVScroll->SetPosition(pos);
		}
		else if (pos > offsetY + r.Height() - miTextHeight - GetSpacing())
		{
			mpVScroll->SetPosition(pos - r.Height() + miTextHeight + GetSpacing());
		}
	}
}

int WebListBox::GetTotalTextHeight(void)
{

	return miTotalTextHeight;
}

int WebListBox::GetTotalTextWidth(void)
{
	return miTotalTextWidth;
}

void WebListBox::Move(WebRect * rect)
{
	DisplayElement::Move(rect);
	ResizeScrollBars();
	if (mpVScroll)
	{
		mpVScroll->Invalidate();
		mpVScroll->SetFlag(DISPLAY_FLAG_DIRTY);

	}
	if (mpHScroll)
	{
		mpHScroll->Invalidate();
	}

}

void WebListBox::SetupScrollBars(WebGraphics * gc)
{
	WebRect window;
	GetContentRect(&window);
	DISPLAY_INT w = window.Width();
	DISPLAY_INT h = window.Height();

	WEBC_BOOL needH, needV;
	needH = needV = WEBC_FALSE;

	for (int t = 0; t < 2; t++)
	{
		if (w < GetTotalTextWidth())
		{
			needH = WEBC_TRUE;
			h -= miSliderWidth;
		}
		if (h < GetTotalTextHeight())
		{
			needV = WEBC_TRUE;
			w -= miSliderWidth;
		}
	}

	if (needH && !mpHScroll)
	{
		WEBC_NEW_VERBOSE(mpHScroll, WebHScroll,"wtlist:WebVscroll");
		if (mpHScroll)
		{
			mpHScroll->SetListener(this);
			mpHScroll->SetRange(GetTotalTextWidth());
			mpHScroll->SetStep(miTextHeight);
			mpHScroll->SetPosition(0);
			if (mFlags & DISPLAY_FLAG_DISABLED)
			{
				mpHScroll->Disable();
			}
			InsertLast(mpHScroll);
			ResizeScrollBars();
		}
	}
	else if (!needH && mpHScroll)
	{
		Remove(mpHScroll);
		WEBC_DELETE(mpHScroll);
		mpHScroll = WEBC_NULL;
	}

	if (needV && !mpVScroll)
	{
		WEBC_NEW_VERBOSE(mpVScroll, WebVScroll,"Wtlist:WebVscroll");
		if (mpVScroll)
		{
			mpVScroll->SetListener(this);
			mpVScroll->SetRange(GetTotalTextHeight());
			mpVScroll->SetStep(miTextHeight + GetSpacing());
			mpVScroll->SetPosition(0);
			if (mFlags & DISPLAY_FLAG_DISABLED)
			{
				mpVScroll->Disable();
			}
			InsertLast(mpVScroll);
			ResizeScrollBars();
		}
	}
	else if (!needV && mpVScroll)
	{
		Remove(mpVScroll);
		WEBC_DELETE(mpVScroll);
		mpVScroll = WEBC_NULL;
	}

}

void WebListBox::ResizeScrollBars(void)
{
	if (mpHScroll)
	{
		WebRect r;
		r.Set(GetMargin() + GetBorder(),
		      Height() - GetMargin() - GetBorder() - miSliderWidth - 1,
		      Width() - (mpVScroll ? miSliderWidth : 0) - GetMargin() - GetBorder() - 1,
		      Height() - GetMargin() - GetBorder() - 1);
		mpHScroll->Move(&r);
		WebRect vr;
		GetOptionsRect(&vr);
		mpHScroll->SetWindow(vr.Width());
		mpHScroll->Invalidate();
	}
	if (mpVScroll)
	{
		WebRect r;
		r.Set(Width() - 1 - GetMargin() - GetBorder() - miSliderWidth,
		      GetMargin() + GetBorder(),
		      Width() - 1 - GetMargin() - GetBorder(),
		      Height() - 1 - (mpHScroll ? miSliderWidth : 0) - GetMargin() - GetBorder());
		mpVScroll->Move(&r);
		WebRect vr;
		GetOptionsRect(&vr);
		mpVScroll->SetWindow(vr.Height());
		mpVScroll->Invalidate();

	}
}

void WebListBox::CalculateTextMetrics(WebGraphics * gc)
{
	static WebChar _a[] = {'A',0};
	WebFont font = mFont.GetFont();
	if (font)
	{
		miTextHeight = gc->TextHeight(_a, font);
		if (miTotalTextHeight < 0)
		{
			miTotalTextHeight = (miTextHeight + GetSpacing()) * miNumOptions;
		}
		if (miTotalTextWidth < 0)
		{
			miTotalTextWidth = 0;
			for (int t = 0; t < miNumOptions; t++)
			{
				if (mppOption[t])
				{
					DISPLAY_INT w = gc->TextWidth(mppOption[t], font);
					miTotalTextWidth = EBSMAX(w, GetTotalTextWidth());
				}
			}
		}
	}
}

void WebListBox::DrawThisOnly (DISPLAY_INT x, DISPLAY_INT y, WebGraphics *gc)
{
	WebRect visibleRegion, oldClip, frame(mRect);

	CalculateTextMetrics(gc);
	SetupScrollBars(gc);

	WebColor hilite, lolite, black, blue, white;
	hilite = white = gc->RGBToColor(0xff, 0xff, 0xff);
	lolite = black = gc->RGBToColor(0, 0, 0);
	blue = gc->RGBToColor(0, 0, 0xff);

	frame.MoveTo(x,y);

	if (mFlags & DISPLAY_FLAG_FOCUS && !(mStyle & LISTBOX_STYLE_NOFOCUSFRAME))
	{
		gc->Rectangle(&frame, black, black, WEBC_FALSE);
	}
	if (mStyle & LISTBOX_STYLE_NOT3D)
	{
		hilite = lolite = black;
	}

	frame.top += GetMargin();
	frame.left += GetMargin();
	frame.bottom -= GetMargin();
	frame.right -= GetMargin();

//	gc->StartBuffer();

	gc->Rectangle(&frame, GetBgColor(gc), GetBgColor(gc), WEBC_TRUE);
	DrawFrame(&frame, gc);

	DISPLAY_INT windowX, windowY;
	GetScrollOffset(&windowX, &windowY);
	long firstVisible = windowY / (miTextHeight + GetSpacing()) - 1;
	if (firstVisible < 0)
		firstVisible = 0;

	gc->GetClip(&oldClip);

	GetOptionsRect(&visibleRegion);
	visibleRegion.Shift(x,y);

	WebRect clip(visibleRegion);
	clip.And(&oldClip);
	gc->SetClip(&clip);

	WEBC_BOOL invert;
	int offset = firstVisible * (miTextHeight + GetSpacing());
	for (int t = firstVisible; t < miNumOptions; t++)
	{
		invert = ( ((t == GetSelected()) && !(mStyle & LISTBOX_STYLE_FOLLOW_MOUSE)) ||
		           ((t == miMouseOver)   &&  (mStyle & LISTBOX_STYLE_FOLLOW_MOUSE)) );

		if (invert)
		{
			PresetWebRect r (visibleRegion.left - windowX,  visibleRegion.top + offset - windowY,
			                 visibleRegion.right - windowX, visibleRegion.top + offset - windowY + miTextHeight - 1);
			gc->Rectangle(&r, GetSelectColor(gc), GetSelectColor(gc), WEBC_TRUE);
		}
		gc->Text(visibleRegion.left - windowX, visibleRegion.top + offset - windowY,
			 mppOption[t], (invert? GetBgColor(gc) : GetTextColor(gc)),
			 blue, WEBC_FALSE, mFont.GetFont());

		offset += miTextHeight + GetSpacing();
	}

	gc->SetClip(&oldClip);
//	gc->EndBuffer();
}

WEBC_BOOL WebListBox::UseVScroll(void)
{
	return (mpVScroll != WEBC_NULL);
}

WEBC_BOOL WebListBox::UseHScroll(void)
{
	return (mpHScroll != WEBC_NULL);
}

void WebListBox::GetScrollOffset(DISPLAY_INT * x, DISPLAY_INT * y)
{
	if (!UseVScroll())
	{
		*y = 0;
	}
	else
	{
		*y = mpVScroll->GetPosition();
	}

	if (!UseHScroll())
	{
		*x = 0;
	}
	else
	{
		*x = mpHScroll->GetPosition();
	}
}

void WebListBox::SetNumOptions (long i)
{
	WebChar **ppNewOptions;
	WEBC_NEW_SIZED(ppNewOptions, WebChar*[i],(sizeof(WebChar)*i));
	if (ppNewOptions)
	{
		int t;

		for (t = 0; t < EBSMIN(i, miNumOptions); t++)
		{
			ppNewOptions[t] = mppOption[t];
		}

		if (i < miNumOptions)
		{
			// downsizing; delete extra options
			for (; t < miNumOptions; t++)
			{
				WEBC_DELETE(mppOption[t]);
			}
		}
		else
		{
			// upsizing; initialize new options to WEBC_NULL
			for (; t < i; t++)
			{
				ppNewOptions[t] = WEBC_NULL;
			}
		}

		if (mppOption)
		{
			WEBC_DELETE(mppOption);
		}

		miNumOptions = i;
		mppOption = ppNewOptions;
	}
}

void WebListBox::SetOptionText (long n, const WebChar *text)
{
	if ((n >= 0) && (n < miNumOptions))
	{
		if (!text)
		{
			if (mppOption[n])
			{
				mppOption[n][0] = (WebChar) '\0';
			}
			return;
		}

		WebChar *pNewOption;
		int _s=webc_strlen(text)+1;
		WEBC_NEW_SIZED(pNewOption, WebChar[_s], (_s*sizeof(WebChar)));
		if (pNewOption)
		{
			webc_strcpy(pNewOption, text);
			if (webc_strlen(text) > (unsigned long) miLongestOption)
			{
				miLongestOption = webc_strlen(text);
			}
			WEBC_DELETE(mppOption[n]);
			mppOption[n] = pNewOption;
		}
	}
}


WebChar *WebListBox::GetOptionText (long n)
{
	return (((n >= 0) && (n < miNumOptions))? mppOption[n] : 0);
}

void WebListBox::SetFont (WebGraphFont *fnt)
{
	mFont.Set(fnt);
	miTextHeight = -1;
	miTotalTextWidth = miTotalTextHeight = -1;
}

WEBC_BOOL WebListBox::SetSelected (long i)
{
	WEBC_INT32 oldValue = miSelected;

	if (mFlags & DISPLAY_FLAG_READONLY)
	{
		return 0;
	}

	miSelected = EBSCLIP(i, 0, miNumOptions - 1);

	if (oldValue != miSelected || miMouseOver != miSelected)
	{
		miMouseOver = miSelected;
		FitSelection();
		Invalidate();
	}
	/* July 1, 2007 .. Return one from SetSelected, even if a change of select
	   may not have happened.. this allows drop boxes to Undrop() */
	/* was return ((oldValue == miSelected)? 0 : 1); */
	return (1);
}

void WebListBox::NotifySelectChange(void)
{
	if (mpListener)
	{
		NotifyEvent what(NOTIFIER_TYPE_LIST_BOX, NOTIFY_SELECT_CHANGE);
		mpListener->Notify((WebListBox*)this, &what);
	}
}


long WebListBox::GetSelected (void)
{
	return miSelected;
}

void WebListBox::Notify(Notifier * who, NotifyEvent *what)
{
    WEBC_DEBUG_ReceiveNotify(this, who, what);
	Invalidate();
}

void WebListBox::GetFrameRect(WebRect *rect)
{
	rect->Set(0,0,Width()-1,Height()-1);
}

void WebListBox::GetContentRect(WebRect *rect)
{
	DISPLAY_INT pad = GetMargin() + GetPadding() + GetBorder();
	rect->Set(pad, pad, Width() - pad - 1, Height() - pad - 1);
}

void WebListBox::GetOptionsRect(WebRect * rect)
{
	GetContentRect(rect);
	if (UseVScroll())
	{
		rect->right -= miSliderWidth;
	}
	if (UseHScroll())
	{
		rect->bottom -= miSliderWidth;
	}
}

void WebListBox::DrawFrame(WebRect *box, WebGraphics *gc)
{
WebColor hilite = gc->LightColor(gc->DarkColor(GetBgColor(gc)));
WebColor lolite = gc->DarkColor(hilite);

	gc->Frame3D(box, lolite, hilite, LISTBOX_BORDER);     // draw a recessed frame
}

void WebListBox::Enable (void)
{
	DisplayElement::Enable();

	if (mpVScroll)
	{
		mpVScroll->Enable();
	}

	if (mpHScroll)
	{
		mpHScroll->Enable();
	}
}

void WebListBox::Disable (void)
{
	DisplayElement::Disable();

	if (mpVScroll)
	{
		mpVScroll->Disable();
	}

	if (mpHScroll)
	{
		mpHScroll->Disable();
	}
}

long WebListBox::NextIndexStartingWith (WebChar ch)
{
	if (miNumOptions > 0)
	{
		long newSelection = miSelected;
		newSelection = (newSelection + 1) % miNumOptions;
		while (newSelection != miSelected)
		{
			WebChar *str = GetOptionText(newSelection);
			if (str)
			{
				while (IS_WHITESPACE(str[0])) str++;
				if (UPPERCASE(str[0]) == UPPERCASE(ch))
				{
					return newSelection;
				}
			}
			newSelection = (newSelection + 1) % miNumOptions;
		}
	}

	return -1;
}

WebColor WebListBox::GetTextColor   (WebGraphics* gc)
{
	return gc->GetColorByIndex(WGC_BLACK);
}

WebColor WebListBox::GetBgColor     (WebGraphics* gc)
{
	return gc->GetColorByIndex(WGC_WHITE);
}

WebColor WebListBox::GetSelectColor (WebGraphics* gc)
{
	return gc->GetColorByIndex(WGC_BLUE);
}
