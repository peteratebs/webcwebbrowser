//
// DHSELECT.CPP - Methods for class HTMLDropBoxDisplay and HTMLListBoxDisplay
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

#include "dhselect.hpp"
#include "hselect.hpp"
#include "hoption.hpp"
#include "hcbox.hpp"
#include "dmanager.hpp"
#include "dhelem.cpp"
#include "htmlfind.hpp"

/* Patches to correct select box formatting errors */
#define SELECT_INCLUDE_MARGIN_INTRINSIC 1
#define SELECT_INCLUDE_MARGIN_FIXED     1


/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/

#define SELECT_ELEMENT  ((HTMLSelect*)mpHtmlElement)


/*****************************************************************************/
// Local Types
/*****************************************************************************/


/*****************************************************************************/
// Global Data
/*****************************************************************************/


/*****************************************************************************/
// Local functions
/*****************************************************************************/


/*****************************************************************************/
// HTMLDropBoxDisplay methods
/*****************************************************************************/

HTMLDropBoxDisplay::HTMLDropBoxDisplay (HTMLSelect *pElem)
	: HTMLElementDisplay<WebDropBox> (pElem)
{
	miParentWidth = 0;
	miParentHeight = 0;

	SetHtmlOptions();
}

HTMLDropBoxDisplay::~HTMLDropBoxDisplay ()
{
}

void HTMLDropBoxDisplay::Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC)
{
	if (GetVisibility() != TU_VISIBILITY_HIDE)
	{
		HTMLElementDisplay<WebDropBox>::Draw(x,y,pViewport,pGC);
	}
}

DisplayElement *HTMLDropBoxDisplay::TrapEventPoint (DISPLAY_INT x, DISPLAY_INT y, DISPLAY_INT viewX, DISPLAY_INT viewY)
{
	if (GetVisibility() == TU_VISIBILITY_HIDE)
	{
		return (0);
	}

	return (HTMLElementDisplay<WebDropBox>::TrapEventPoint(x,y,viewX,viewY));
}

void HTMLDropBoxDisplay::Notify(Notifier *who, NotifyEvent *what)
{
	if (what && what->event == NOTIFY_SELECT_CHANGE && mpHtmlElement)
	{
		HTMLEvent e;
		rtp_memset(&e,0,sizeof(e));
		e.type = HTML_EVENT_CHANGE;
		e.target = (HELEMENT_HANDLE)mpHtmlElement; 
		if (mpHtmlElement->Event(&e) != HTML_EVENT_STATUS_CONTINUE)
			return;
	}
	if (what && (who == (Notifier *) mpHtmlElement))
	{
        WEBC_DEBUG_ReceiveNotify(this, who, what);
		switch (what->event)
		{
		case NOTIFY_CONTENT_ADDED:        // intentional fall-through
		case NOTIFY_CONTENT_REMOVED:
			Invalidate();
			if (SetElementStyle())
			{
				Invalidate();
			}
			break;

		default:
			break;
		}
	}

	HTMLElementDisplay<WebDropBox>::Notify(who, what);
}

DISPLAY_INT HTMLDropBoxDisplay::GetMinWidth(void)
{
CSSLength cssWidth;

	SELECT_ELEMENT->GetWidth(&cssWidth);
	if (cssWidth.type == CSS_LENGTH_UNIT_PERCENT)
	{
		return (0);
	}

	return GetPixelWidth();
}

DISPLAY_INT HTMLDropBoxDisplay::GetMaxWidth(void)
{
	return GetMinWidth();
}


WEBC_BOOL HTMLDropBoxDisplay::GetIntrinsicWidth (
		DISPLAY_INT* intrinsicWidth,
		DISPLAY_INT  height,
		WEBC_BOOL    heightIsAuto
	)
{
    SetElementStyle();
   	HTMLContentBox b(SELECT_ELEMENT, miParentWidth);
#if (SELECT_INCLUDE_MARGIN_INTRINSIC)
    *intrinsicWidth = mContentRect.Width() + (2*DROPBOX_MARGIN);
#else
    *intrinsicWidth = mContentRect.Width();
#endif
	return (WEBC_TRUE);
}

DISPLAY_INT HTMLDropBoxDisplay::GetPixelWidth(void)
{
	if (mSizeInvalid)
	{
		DisplayManager *m = GetManager();
		WebGraphics *gc = m? m->GetGraphics() : 0;
		if (gc)
		{
			SetDimensions(gc);
		}
	}

	return Width();
}

DISPLAY_INT HTMLDropBoxDisplay::GetPixelHeight(void)
{
	if (mSizeInvalid)
	{
		DisplayManager *m = GetManager();
		WebGraphics *gc = m? m->GetGraphics() : 0;
		if (gc)
		{
			SetDimensions(gc);
		}
	}

	return Height();
}

DISPLAY_INT HTMLDropBoxDisplay::GetBaseline(void)
{
	WebGraphics *gc = GetGraphics();

	if (gc)
	{
		if (mSizeInvalid)
		{
			SetDimensions(gc);
		}

		WebRect box;
		WebChar* text = mpText;
		WebFont font = mFont.GetFont();

		GetContentRect(&box);

		static WebChar emStr[2] = {'M',0};
		if (!text || !text[0])
		{
			text = emStr;
		}

		if (font && text)
		{
			DISPLAY_INT textHeight = gc->TextHeight(text, font);
			DISPLAY_INT textTop    = ((box.Height() - textHeight) >> 1) + box.top + DROPBOX_PADDING;
			DISPLAY_INT textBase   = WEB_FONT_BASELINE(font);

			return (textTop + textBase);
		}
	}

	return Height();
}

void HTMLDropBoxDisplay::SetParentWidth(DISPLAY_INT parentWidth, DISPLAY_INT ignored)
{
	if (parentWidth != miParentWidth)
	{
		mSizeInvalid = WEBC_TRUE;
		miParentWidth = parentWidth;
	}
}

void HTMLDropBoxDisplay::SetParentHeight(DISPLAY_INT parentHeight)
{
	if (parentHeight != miParentHeight)
	{
		mSizeInvalid = WEBC_TRUE;
		miParentHeight = parentHeight;
	}
}

void HTMLDropBoxDisplay::SetParentDisplay(DisplayElement *parent, DisplayElement* insertBefore)
{
	if (parent != mpParent)
	{
		HTMLElementDisplay<WebDropBox>::SetParentDisplay(parent, insertBefore);
		SetElementStyle();
	}
	else
	{
		HTMLElementDisplay<WebDropBox>::SetParentDisplay(parent, insertBefore);
	}
}

void HTMLDropBoxDisplay::SetDisplayContext(DisplayElement *parent)
{
	if (parent->GetManager() != GetManager())
	{
		HTMLElementDisplay<WebDropBox>::SetDisplayContext(parent);
		SetElementStyle();
	}
	else
	{
		HTMLElementDisplay<WebDropBox>::SetDisplayContext(parent);
	}
}

// ****************************************************************
class DropBoxOptionSetter : public HTMLElementFinder
{
private:
	HTMLDropBoxDisplay *mpDropBox;
	int miIndex;

public:
	DropBoxOptionSetter (HTMLDropBoxDisplay *box)
	{
		mpDropBox = box;
		miIndex = 0;
	}

	virtual ~DropBoxOptionSetter ()
	{
	}

	virtual WEBC_BOOL Check (HTMLElement *pElement)
	{
		if (pElement->Type() == HTML_OPTION_ELEMENT)
		{
			HTMLOption *pOption = (HTMLOption *) pElement;
			mpDropBox->SetOptionText(miIndex, pOption->Text());
			miIndex++;
		}

		return (0);
	}
};
// ****************************************************************

void HTMLDropBoxDisplay::SetHtmlOptions (void)
{
	SetNumOptions(SELECT_ELEMENT->NumOptions());

	DropBoxOptionSetter f(this);
	SELECT_ELEMENT->FindElement(&f);
}

WEBC_BOOL HTMLDropBoxDisplay::SetElementStyle (void)
{
	// Update the options for this box
	//  tbd - only do this if necessary?
	SetHtmlOptions();

	if (GetManager())
	{
		WebGraphics *gc = GetGraphics();
		if (!gc)
		{
			return (WEBC_FALSE);
		}

		// Set font
		SetFont(SELECT_ELEMENT->GetFont());

		// Set background color
		HTMLColor rgbColor;
		rgbColor = SELECT_ELEMENT->GetBackgroundColor();
		if (HTML_ColorGetAlpha(rgbColor) != 0xff)
		{
			mBackgroundColor = gc->RGBToColor(rgbColor);
		}
		else
		{
			// tbd
		}

		// Set text color
		rgbColor = SELECT_ELEMENT->GetColor();
		if (HTML_ColorGetAlpha(rgbColor) != 0xff)
		{
			mTextColor = gc->RGBToColor(rgbColor);
		}
		else
		{
			// tbd - disable text drawing
		}

		if (mpHtmlElement->mFlags & HINPUT_FLAG_DISABLED)
		{
			Disable();
		}
		else
		{
			Enable();
		}

		if (mpHtmlElement->mFlags & HINPUT_FLAG_READONLY)
		{
			SetFlag(DISPLAY_FLAG_READONLY);
			mListbox.SetFlag(DISPLAY_FLAG_READONLY);
		}
		else
		{
			ClearFlag(DISPLAY_FLAG_READONLY);
			mListbox.ClearFlag(DISPLAY_FLAG_READONLY);
		}

		WebRect oldRect(mRect);
		SetDimensions(gc);
		if (!oldRect.Equals(&mRect))
		{
			return (WEBC_TRUE);
		}
	}

	return (WEBC_FALSE);
}

void HTMLDropBoxDisplay::SetDimensions (WebGraphics *gc)
{
DISPLAY_INT pixelWidth, pixelHeight, fontHeight;
CSSLength cssWidth, cssHeight;
WebRect rect(mRect);
WebFont font = mFont.GetFont();

	mSizeInvalid = WEBC_FALSE;
	fontHeight = font? WEB_FONT_HEIGHT(font) : WEBC_CFG_DEFAULT_TEXT_HEIGHT;
	HTMLContentBox b(SELECT_ELEMENT, miParentWidth);

	SELECT_ELEMENT->GetWidth(&cssWidth);
	SELECT_ELEMENT->GetHeight(&cssHeight);

	mListbox.CalculateTextMetrics(gc);

	switch (cssWidth.type)
	{
	case CSS_LENGTH_UNIT_AUTO:

		pixelWidth = (b.GetBorderWidth(CSS_LEFT)  + b.GetPaddingWidth(CSS_LEFT)  + DROPBOX_MARGIN) +
		             (b.GetBorderWidth(CSS_RIGHT) + b.GetPaddingWidth(CSS_RIGHT) + DROPBOX_MARGIN) +
		             mListbox.GetTotalTextWidth() + webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_WIDTH) + 4;
		break;
	default:
		pixelWidth = CSS_LengthToPixels(&cssWidth, miParentWidth, fontHeight, fontHeight/2);
#if (SELECT_INCLUDE_MARGIN_FIXED)
        pixelWidth += (2*DROPBOX_MARGIN);
#endif
		break;
	}

	switch (cssHeight.type)
	{
	case CSS_LENGTH_UNIT_AUTO:
		pixelHeight = (b.GetBorderWidth(CSS_TOP)    + b.GetPaddingWidth(CSS_TOP)    + DROPBOX_MARGIN) +
		              (b.GetBorderWidth(CSS_BOTTOM) + b.GetPaddingWidth(CSS_BOTTOM) + DROPBOX_MARGIN) +
		              fontHeight;
		break;

	default:
		pixelHeight = CSS_LengthToPixels(&cssHeight, miParentHeight, fontHeight, fontHeight/2);
#if (SELECT_INCLUDE_MARGIN_FIXED)
        pixelHeight += (2*DROPBOX_MARGIN);
#endif
		break;
	}

	rect.SizeTo(pixelWidth,	pixelHeight);
	FindContentRect(&mContentRect, &rect, &b);
	Move(&rect);
}

void HTMLDropBoxDisplay::FindContentRect(WebRect *rect, WebRect *elemRect, HTMLContentBox *b)
{
	rect->Set((b->GetBorderWidth(CSS_LEFT) + b->GetPaddingWidth(CSS_LEFT) + DROPBOX_MARGIN),
	          (b->GetBorderWidth(CSS_TOP)  + b->GetPaddingWidth(CSS_TOP)  + DROPBOX_MARGIN),
	          elemRect->Width()  - 1 - (b->GetBorderWidth(CSS_RIGHT)  + b->GetPaddingWidth(CSS_RIGHT)  + DROPBOX_MARGIN),
	          elemRect->Height() - 1 - (b->GetBorderWidth(CSS_BOTTOM) + b->GetPaddingWidth(CSS_BOTTOM) + DROPBOX_MARGIN));
}

void HTMLDropBoxDisplay::GetContentRect(WebRect * rect)
{
	if (mSizeInvalid)
	{
		DisplayManager *m = GetManager();
		WebGraphics *gc = m? m->GetGraphics() : 0;
		if (gc)
		{
			SetDimensions(gc);
		}
	}

	rect->Set(&mContentRect);
}

void HTMLDropBoxDisplay::DrawFrame(WebRect *box, WebGraphics *gc)
{
HTMLContentBox b(SELECT_ELEMENT, miParentWidth);

	b.DrawBorder(box, gc);
}

DISPLAY_INT HTMLDropBoxDisplay::GetParentWidth(void)
{
	return (miParentWidth);
}

WebColor HTMLDropBoxDisplay::GetTextColor (WebGraphics* gc)
{
	return mTextColor;
}

WebColor HTMLDropBoxDisplay::GetBgColor (WebGraphics* gc)
{
	return mBackgroundColor;
}


/*****************************************************************************/
// HTMLListBoxDisplay methods
/*****************************************************************************/

HTMLListBoxDisplay::HTMLListBoxDisplay (HTMLSelect *pElem)
	: HTMLElementDisplay<WebListBox> (pElem)
{
	miParentWidth = 0;
	miParentHeight = 0;

	SetHtmlOptions();
}

HTMLListBoxDisplay::~HTMLListBoxDisplay ()
{
}

void HTMLListBoxDisplay::Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC)
{
	if (GetVisibility() != TU_VISIBILITY_HIDE)
	{
		HTMLElementDisplay<WebListBox>::Draw(x,y,pViewport,pGC);
	}
}

DisplayElement *HTMLListBoxDisplay::TrapEventPoint (DISPLAY_INT x, DISPLAY_INT y, DISPLAY_INT viewX, DISPLAY_INT viewY)
{
	if (GetVisibility() == TU_VISIBILITY_HIDE)
	{
		return (0);
	}

	return (HTMLElementDisplay<WebListBox>::TrapEventPoint(x,y,viewX,viewY));
}

DISPLAY_INT HTMLListBoxDisplay::GetMinWidth(void)
{
CSSLength cssWidth;

	SELECT_ELEMENT->GetWidth(&cssWidth);
	if (cssWidth.type == CSS_LENGTH_UNIT_PERCENT)
	{
		return (0);
	}

	return GetPixelWidth();
}

DISPLAY_INT HTMLListBoxDisplay::GetMaxWidth(void)
{
	return GetMinWidth();
}

DISPLAY_INT HTMLListBoxDisplay::GetPixelWidth(void)
{
	if (mStyle & LISTBOX_STYLE_SIZE_INVALID)
	{
		DisplayManager *m = GetManager();
		WebGraphics *gc = m? m->GetGraphics() : 0;
		if (gc)
		{
			SetDimensions(gc);
		}
	}

	return Width();
}

DISPLAY_INT HTMLListBoxDisplay::GetPixelHeight(void)
{
	if (mStyle & LISTBOX_STYLE_SIZE_INVALID)
	{
		DisplayManager *m = GetManager();
		WebGraphics *gc = m? m->GetGraphics() : 0;
		if (gc)
		{
			SetDimensions(gc);
		}
	}

	return Height();
}


void HTMLListBoxDisplay::SetParentWidth(DISPLAY_INT parentWidth, DISPLAY_INT ignored)
{
	if (parentWidth != miParentWidth)
	{
		mStyle |= LISTBOX_STYLE_SIZE_INVALID;
		miParentWidth = parentWidth;
	}
}

void HTMLListBoxDisplay::SetParentHeight(DISPLAY_INT parentHeight)
{
	if (parentHeight != miParentHeight)
	{
		mStyle |= LISTBOX_STYLE_SIZE_INVALID;
		miParentHeight = parentHeight;
	}
}

void HTMLListBoxDisplay::SetParentDisplay(DisplayElement *parent, DisplayElement* insertBefore)
{
	if (parent != mpParent)
	{
		HTMLElementDisplay<WebListBox>::SetParentDisplay(parent, insertBefore);
		SetElementStyle();
	}
	else
	{
		HTMLElementDisplay<WebListBox>::SetParentDisplay(parent, insertBefore);
	}
}

void HTMLListBoxDisplay::SetDisplayContext(DisplayElement *parent)
{
	if (parent->GetManager() != GetManager())
	{
		HTMLElementDisplay<WebListBox>::SetDisplayContext(parent);
		SetElementStyle();
	}
	else
	{
		HTMLElementDisplay<WebListBox>::SetDisplayContext(parent);
	}
}

WEBC_BOOL HTMLListBoxDisplay::GetIntrinsicWidth (
		DISPLAY_INT* intrinsicWidth,
		DISPLAY_INT  height,
		WEBC_BOOL    heightIsAuto
	)
{
    SetElementStyle();
   	HTMLContentBox b(SELECT_ELEMENT, miParentWidth);
#if (SELECT_INCLUDE_MARGIN_INTRINSIC)
    *intrinsicWidth = mContentRect.Width() + (2*DROPBOX_MARGIN);
#else
    *intrinsicWidth = mContentRect.Width();
#endif
	return (WEBC_TRUE);
}

// ****************************************************************
class ListBoxOptionSetter : public HTMLElementFinder
{
private:
	HTMLListBoxDisplay *mpListBox;
	int miIndex;

public:
	ListBoxOptionSetter (HTMLListBoxDisplay *box)
	{
		mpListBox = box;
		miIndex = 0;
	}

	virtual ~ListBoxOptionSetter ()
	{
	}

	virtual WEBC_BOOL Check (HTMLElement *pElement)
	{
		if (pElement->Type() == HTML_OPTION_ELEMENT)
		{
			HTMLOption *pOption = (HTMLOption *) pElement;
			mpListBox->SetOptionText(miIndex, pOption->Text());
			miIndex++;
		}

		return (0);
	}
};
// ****************************************************************

void HTMLListBoxDisplay::SetHtmlOptions (void)
{
	SetNumOptions(SELECT_ELEMENT->NumOptions());

	ListBoxOptionSetter f(this);
	SELECT_ELEMENT->FindElement(&f);
}

WEBC_BOOL HTMLListBoxDisplay::SetElementStyle (void)
{
	// Update the options for this box
	//  tbd - only do this if necessary?
	SetHtmlOptions();

	if (GetManager())
	{
		WebGraphics *gc = GetGraphics();
		if (!gc)
		{
			return (WEBC_FALSE);
		}

		// Set font
		SetFont(SELECT_ELEMENT->GetFont());

		// Set background color
		HTMLColor rgbColor;
		rgbColor = SELECT_ELEMENT->GetBackgroundColor();
		if (HTML_ColorGetAlpha(rgbColor) != 0xff)
		{
			mBackgroundColor = gc->RGBToColor(rgbColor);
		}
		else
		{
		}

		// Set text color
		rgbColor = SELECT_ELEMENT->GetColor();
		if (HTML_ColorGetAlpha(rgbColor) != 0xff)
		{
			mTextColor = gc->RGBToColor(rgbColor);
		}
		else
		{
			// tbd - disable text drawing
		}

		if (mpHtmlElement->mFlags & HINPUT_FLAG_DISABLED)
		{
			Disable();
		}
		else
		{
			Enable();
		}

		if (mpHtmlElement->mFlags & HINPUT_FLAG_READONLY)
		{
			SetFlag(DISPLAY_FLAG_READONLY);
		}
		else
		{
			ClearFlag(DISPLAY_FLAG_READONLY);
		}

		// Set element dimensions
		WebRect oldRect(mRect);
		SetDimensions(gc);
		if (!oldRect.Equals(&mRect))
		{
			return (WEBC_TRUE);
		}
	}

	return (WEBC_FALSE);
}

void HTMLListBoxDisplay::SetDimensions (WebGraphics *gc)
{
DISPLAY_INT pixelWidth, pixelHeight;
CSSLength cssWidth, cssHeight;
WebRect rect(mRect);

	mStyle &= ~LISTBOX_STYLE_SIZE_INVALID;

	CalculateTextMetrics(gc);

	WebFont font = mFont.GetFont();
	DISPLAY_INT fontHeight = font? WEB_FONT_HEIGHT(font) : WEBC_CFG_DEFAULT_TEXT_HEIGHT;

	HTMLContentBox b(SELECT_ELEMENT, miParentWidth);

	SELECT_ELEMENT->GetWidth(&cssWidth);
	SELECT_ELEMENT->GetHeight(&cssHeight);

	switch (cssWidth.type)
	{
	case CSS_LENGTH_UNIT_AUTO:
		pixelWidth = b.GetBorderWidth(CSS_LEFT)  + b.GetPaddingWidth(CSS_LEFT) +
		             b.GetBorderWidth(CSS_RIGHT) + b.GetPaddingWidth(CSS_RIGHT) +
		             GetMargin()*2 + GetSpacing() + GetTotalTextWidth() + webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_WIDTH) +1;
		break;

	default:
		pixelWidth = CSS_LengthToPixels(&cssWidth, miParentWidth, fontHeight, fontHeight/2);
#if (SELECT_INCLUDE_MARGIN_FIXED)
        pixelWidth += (2*DROPBOX_MARGIN);
#endif
		break;
	}

	switch (cssHeight.type)
	{
	case CSS_LENGTH_UNIT_AUTO:
		pixelHeight = b.GetBorderWidth(CSS_TOP)    + b.GetPaddingWidth(CSS_TOP) +
		              b.GetBorderWidth(CSS_BOTTOM) + b.GetPaddingWidth(CSS_BOTTOM) +
		              GetMargin()*2 + (fontHeight + GetSpacing()) * SELECT_ELEMENT->Size();
		break;

	default:
		pixelHeight = CSS_LengthToPixels(&cssHeight, miParentHeight, fontHeight, fontHeight/2);
#if (SELECT_INCLUDE_MARGIN_FIXED)
        pixelHeight += (2*DROPBOX_MARGIN);
#endif
		break;
	}

	rect.SizeTo(pixelWidth,	pixelHeight);
	FindContentRect(&mContentRect, &rect, &b);
	Move(&rect);
}

void HTMLListBoxDisplay::FindContentRect(WebRect *rect, WebRect *elemRect, HTMLContentBox *b)
{
	rect->Set((b->GetBorderWidth(CSS_LEFT) + b->GetPaddingWidth(CSS_LEFT) + GetMargin()),
	          (b->GetBorderWidth(CSS_TOP)  + b->GetPaddingWidth(CSS_TOP) + GetMargin()),
	          elemRect->Width()  - 1 - (b->GetBorderWidth(CSS_RIGHT)  + b->GetPaddingWidth(CSS_RIGHT) + GetMargin()),
	          elemRect->Height() - 1 - (b->GetBorderWidth(CSS_BOTTOM) + b->GetPaddingWidth(CSS_BOTTOM) + GetMargin()));
}

void HTMLListBoxDisplay::GetContentRect(WebRect * rect)
{
	if (mStyle & LISTBOX_STYLE_SIZE_INVALID)
	{
		DisplayManager *m = GetManager();
		WebGraphics *gc = m? m->GetGraphics() : 0;
		if (gc)
		{
			SetDimensions(gc);
		}
	}

	rect->Set(&mContentRect);
}

void HTMLListBoxDisplay::DrawFrame(WebRect *box, WebGraphics *gc)
{
HTMLContentBox b(SELECT_ELEMENT, miParentWidth);

	b.DrawBorder(box, gc);
}

DISPLAY_INT HTMLListBoxDisplay::GetParentWidth(void)
{
	return (miParentWidth);
}

WebColor HTMLListBoxDisplay::GetTextColor (WebGraphics* gc)
{
	return mTextColor;
}

WebColor HTMLListBoxDisplay::GetBgColor (WebGraphics* gc)
{
	return mBackgroundColor;
}
