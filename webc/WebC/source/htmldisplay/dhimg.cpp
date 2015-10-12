//
// DHIMG.CPP - Methods for class HTMLImageDisplay
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

#include "dhelem.cpp"
#include "dhstring.hpp"
#include "himage.hpp"
#include "harea.hpp"
#include "hmap.hpp"
#include "webctmo.h"
#include "zdelem.hpp"
#include "wgcolors.hpp"
#include "dmanager.hpp"
#include "htmlbrow.hpp"


/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/

#define IMAGE_ELEMENT ((HTMLImage*)mpHtmlElement)


/*****************************************************************************/
// Local Types
/*****************************************************************************/


/*****************************************************************************/
// Global Data
/*****************************************************************************/


/*****************************************************************************/
// Local functions
/*****************************************************************************/

static int _AnimateImage (void* data, long error);

static int _matchId (TimeoutInfo* timeout, void* data)
{
	WEBC_UINT16 id = *((WEBC_UINT16*)data);
	return (timeout->wId == id);
}

/*****************************************************************************/
// HTMLImageDisplay methods
/*****************************************************************************/

HTMLImageDisplay::HTMLImageDisplay (HTMLImage *pImageElement)
	: HTMLElementDisplay<DisplayElement> (pImageElement), mBorder(pImageElement)
{
	mpImage = 0;
	mwTimerId = WEBC_TIMEOUT_INVALID;
	mpCurrentFrame = 0;
	mOutlineStyle = CSS_BORDER_STYLE_NONE;
	miParentWidth = 0;
	miParentHeight = 0;
	mImageDisplayFlags = 0;

	SetElementStyle();
}

HTMLImageDisplay::~HTMLImageDisplay ()
{
	Stop();
	SetImage(0);
}


void HTMLImageDisplay::Notify (Notifier *who, NotifyEvent *what)
{
	if (what && (who == mpHtmlElement))
	{
        WEBC_DEBUG_ReceiveNotify(this, who, what);
		switch (what->event)
		{
			case NOTIFY_BITMAP_CHANGE:
			{
				HTMLImage *pImageElement = (HTMLImage*) mpHtmlElement;
				WebcImage *pImage = pImageElement->GetImage();
				if (mpImage != pImage)
				{
					SetImage(pImage);

					PresetWebRect dirtyRect(&mRect);
					SetImageDimensions();
					dirtyRect.Or(&mRect);

					InvalidateRect(&dirtyRect);
				}
				break;
			}
			case NOTIFY_STYLE_CHANGE:
				{
					NotifyEvent what(NOTIFIER_TYPE_TEXT_UNIT, NOTIFY_STYLE_CHANGE); // NOTIFY_SIZE_CHANGE);
					if (mpParent)
						mpParent->Notify((TextUnit *) this, &what);
				}

				break;
			default:
				break;
		}
	}

	HTMLElementDisplay<DisplayElement>::Notify(who, what);
}

// DisplayElement methods
void HTMLImageDisplay::Move (WebRect * pRect)
{
	if (mpParent && ((pRect->Width() != Width()) || (pRect->Height() != Height())))
	{
		WEBC_BOOL widthChanged = (pRect->Width() != Width());

		DisplayElement::Move(pRect);
		NotifyEvent what(NOTIFIER_TYPE_TEXT_UNIT, NOTIFY_SIZE_CHANGE);
		mpParent->Notify((TextUnit *) this, &what);

		if (widthChanged)
		{
			what.event = NOTIFY_MAX_WIDTH_CHANGE;
			mpParent->Notify((TextUnit *) this, &what);

			what.event = NOTIFY_MIN_WIDTH_CHANGE;
			mpParent->Notify((TextUnit *) this, &what);
		}
	}
	else
	{
		DisplayElement::Move(pRect);
	}
}

void HTMLImageDisplay::SizeTo (DISPLAY_INT w, DISPLAY_INT h)
{
	if (mpParent && ((w != Width()) || (h != Height())))
	{
		WEBC_BOOL widthChanged  = (w != Width());
		WEBC_BOOL heightChanged = (h != Height());

		DisplayElement::SizeTo(w, h);
		NotifyEvent what(NOTIFIER_TYPE_TEXT_UNIT, NOTIFY_SIZE_CHANGE);
		mpParent->Notify((TextUnit *) this, &what);

		if (widthChanged)
		{
			what.event = NOTIFY_MAX_WIDTH_CHANGE;
			mpParent->Notify((TextUnit *) this, &what);

			what.event = NOTIFY_MIN_WIDTH_CHANGE;
			mpParent->Notify((TextUnit *) this, &what);
		}
	}
	else
	{
		DisplayElement::SizeTo(w, h);
	}
}

void HTMLImageDisplay::SetHeight (DISPLAY_INT h)
{
	if (mpParent && (h != Height()))
	{
		DisplayElement::SetHeight(h);
		NotifyEvent what(NOTIFIER_TYPE_TEXT_UNIT, NOTIFY_SIZE_CHANGE);
		mpParent->Notify((TextUnit *) this, &what);
	}
	else
	{
		DisplayElement::SetHeight(h);
	}
}

void HTMLImageDisplay::SetWidth (DISPLAY_INT w)
{
	if (mpParent && (w != Width()))
	{
		DisplayElement::SetWidth(w);
		NotifyEvent what(NOTIFIER_TYPE_TEXT_UNIT, NOTIFY_SIZE_CHANGE);
		mpParent->Notify((TextUnit *) this, &what);

		what.event = NOTIFY_MAX_WIDTH_CHANGE;
		mpParent->Notify((TextUnit *) this, &what);

		what.event = NOTIFY_MIN_WIDTH_CHANGE;
		mpParent->Notify((TextUnit *) this, &what);
	}
	else
	{
		DisplayElement::SetWidth(w);
	}
}

void HTMLImageDisplay::Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC)
{
	if (!(mHtmlElementDisplayFlags & HELEM_DISPLAY_FLAG_STYLE_SET))
	{
		SetElementStyle();
	}

	if (GetVisibility() == TU_VISIBILITY_HIDE)
	{
		return;
	}

	WebRect clip, screenRect(mRect);
	pGC->GetClip(&clip);
	screenRect.MoveTo(x,y);
	if (!clip.Overlaps(&screenRect))
	{
		return;
	}

	HTMLImage *pImageElem = (HTMLImage*) mpHtmlElement;
	DISPLAY_INT hspace = pImageElem->GetHSpace();
	DISPLAY_INT vspace = pImageElem->GetVSpace();
	WebcImage *bmp = pImageElem->GetImage();

//	pGC->StartBuffer();

	if (HasBackgroundColor())
	{
		DISPLAY_INT w = screenRect.Width();
		DISPLAY_INT h = screenRect.Height();

		if (w < h)
		{
			pGC->HLine(screenRect.top, screenRect.left, screenRect.right, GetBackgroundColor(), h);
		}
		else
		{
			pGC->VLine(screenRect.left, screenRect.top, screenRect.bottom, GetBackgroundColor(), w);
		}
	}

	if (bmp && (bmp->State() == WEBIMAGE_OKAY))
	{
		PresetWebRect rect(&screenRect);

		rect.left += hspace;
		rect.top += vspace;
		rect.right = EBSMAX(rect.left, rect.right - hspace);
		rect.bottom = EBSMAX(rect.top, rect.bottom - vspace);

		mpCurrentFrame = bmp->DrawFrame(mpCurrentFrame, &rect, pGC);
		if (mpCurrentFrame)
		{
			if (mpCurrentFrame->IsLastFrame())
			{
				Stop();
			}
			else
			{
				Play(mpCurrentFrame->DelayMsec());
			}
		}
	}
	else
	{
	  #if (WEBC_DRAW_IMAGE_BOXES)
		// Draw alt text instead
		pGC->Frame3D(&screenRect, pGC->GetColorByIndex(WGC_DARKGRAY), pGC->GetColorByIndex(WGC_LIGHTGRAY), 1);
	  #endif

	  #if (WEBC_DRAW_ALT_TEXT)
		screenRect.SizeBy(-(4 + hspace*2), -(4 + vspace*2));
		screenRect.Shift(2,2);
		if (screenRect.Overlaps(&clip))
		{
			screenRect.And(&clip);
			pGC->SetClip(&screenRect);

			if (pImageElem->Alt())
			{
				WebFontRef fontRef(mpHtmlElement->GetFont());
				WebFont font = fontRef.GetFont();
				if (font)
				{
					DISPLAY_INT w = pGC->TextWidth(pImageElem->Alt(), font);
					DISPLAY_INT h = WEB_FONT_HEIGHT(font);
					pGC->Text(((screenRect.left + screenRect.right - w) >> 1),
							  ((screenRect.top + screenRect.bottom - h) >> 1), pImageElem->Alt(),
							  pGC->GetColorByIndex(WGC_DARKGRAY), 0 /* fill color */, WEBC_FALSE /* fill */, font);
				}
			}

			pGC->SetClip(&clip);
		}
	  #endif
	}

	// draw the border
	WebRect borderBox(mRect);
	borderBox.MoveTo(x,y);
	mBorder.DrawBorder(&borderBox, pGC);

	switch (mOutlineStyle)
	{
		case CSS_BORDER_STYLE_NONE:
			break;

		default:
			// account for the border which is inside the outline
//			screenRect.Expand(mBorder.GetBorderWidth(CSS_TOP) + mBorder.GetPaddingWidth(CSS_TOP),
//				              mBorder.GetBorderWidth(CSS_BOTTOM) + mBorder.GetPaddingWidth(CSS_BOTTOM),
//							  mBorder.GetBorderWidth(CSS_LEFT) + mBorder.GetPaddingWidth(CSS_LEFT),
//							  mBorder.GetBorderWidth(CSS_RIGHT) + mBorder.GetPaddingWidth(CSS_RIGHT) );

			// draw direct
			pGC->PatternFrame(&screenRect, pGC->GetColorByIndex(WGC_BLACK), 1, mOutlineStyle);
			break;
	}

//	pGC->EndBuffer();
}

HTMLEventStatus HTMLImageDisplay::Event (HTMLEvent *e)
{
    WEBC_DEBUG_ReceiveEvent(this, e);
	return (mpHtmlElement->Event(e));
}

DISPLAY_INT HTMLImageDisplay::GetMinWidth(void)
{
	if (mpHtmlElement)
	{
		CSSLength len;

		mpHtmlElement->GetWidth(&len);
		if (len.type == CSS_LENGTH_UNIT_PERCENT)
		{
			return (0);
		}
	}

	return GetPixelWidth();
}

DISPLAY_INT HTMLImageDisplay::GetMaxWidth(void)
{
	if (mpHtmlElement)
	{
		CSSLength len;

		mpHtmlElement->GetWidth(&len);
		if (len.type == CSS_LENGTH_UNIT_PERCENT)
		{
			return (0);
		}
	}

	return GetPixelWidth();
}

DISPLAY_INT HTMLImageDisplay::GetPixelWidth(void)
{
	if (!(mHtmlElementDisplayFlags & HELEM_DISPLAY_FLAG_STYLE_SET))
	{
		SetElementStyle();
	}
	return (Width());
}

DISPLAY_INT HTMLImageDisplay::GetPixelHeight(void)
{
	if (!(mHtmlElementDisplayFlags & HELEM_DISPLAY_FLAG_STYLE_SET))
	{
		SetElementStyle();
	}
	return (Height());
}

DISPLAY_INT HTMLImageDisplay::GetBaseline(void)
{
	return GetPixelHeight();
}


void HTMLImageDisplay::SetParentWidth(DISPLAY_INT iWidth, DISPLAY_INT iAvail)
{
	miParentWidth = iAvail;

	mBorder.SetParentWidth(iWidth);

	if (mpHtmlElement)
	{
		CSSLength width;

		mpHtmlElement->GetWidth(&width);

		if (width.type == CSS_LENGTH_UNIT_PERCENT)
		{
			HTMLImage *pI = (HTMLImage *) mpHtmlElement;
			DISPLAY_INT iPixelWidth = (iAvail * width.val.dec) / 100;
			DISPLAY_INT iPixelHeight;
			CSSLength height;

			mpHtmlElement->GetHeight(&height);

			WebcImage *pBmp = pI->GetImage();
			if (pBmp && (pBmp->State() == WEBIMAGE_OKAY) &&
			    (height.type == CSS_LENGTH_UNIT_AUTO))
			{
				iPixelHeight = ((pBmp->Height() * iPixelWidth) / pBmp->Width()) + pI->GetVSpace()*2;
			}
			else
			{
				iPixelHeight = mRect.Height();
			}

			SizeTo(iPixelWidth, iPixelHeight);
		}
	}
}

void HTMLImageDisplay::SetParentHeight(DISPLAY_INT parentHeight)
{
	miParentHeight = parentHeight;
}

void HTMLImageDisplay::SetParentDisplay(DisplayElement *parent, DisplayElement* insertBefore)
{
	HTMLElementDisplay<DisplayElement>::SetParentDisplay(parent, insertBefore);

	if (IMAGE_ELEMENT)
	{
		HTMLMap *map = IMAGE_ELEMENT->GetMap();
		if (map)
		{
			DisplayElement *areaDisplay;
			ZOrderedDisplayElement listMarker(-100000000L);

			InsertLast(&listMarker);

			// now add all HTMLAreaDisplay elements for this image.
			HTMLElement *child = map->FirstChild();
			HTMLArea *area;
			while (child)
			{
				if (child->Type() == HTML_AREA_ELEMENT)
				{
					area = (HTMLArea *) child;
					areaDisplay = area->GetAreaDisplay(IMAGE_ELEMENT, WEBC_TRUE);
					if (areaDisplay)
					{
						InsertOrdered(areaDisplay);
					}
				}
				child = child->mpNext;
			}

			// remove any elements that we didn't add back onto the list
			while (mpFirstChild != &listMarker)
			{
				Remove(mpFirstChild);
			}

			Remove(&listMarker);
		}
	}
}

void HTMLImageDisplay::SetXYPosition(DISPLAY_INT iXPos, DISPLAY_INT iYPos)
{
	HTMLElementDisplay<DisplayElement>::SetXYPosition(iXPos, iYPos);
}

DisplayElement *HTMLImageDisplay::TrapEventPoint(DISPLAY_INT x, DISPLAY_INT y, DISPLAY_INT viewX, DISPLAY_INT viewY)
{
	if (GetVisibility() == TU_VISIBILITY_HIDE)
	{
		return (0);
	}

	if (IMAGE_ELEMENT)
	{
		HTMLMap *map = IMAGE_ELEMENT->GetMap();
		if (map)
		{
			x -= IMAGE_ELEMENT->GetHSpace();
			y -= IMAGE_ELEMENT->GetVSpace();

			HTMLArea *area;
			HTMLElement *child = map->FirstChild();
			while (child)
			{
				if (child->Type() == HTML_AREA_ELEMENT)
				{
					area = (HTMLArea *) child;
					if (area->Contains(x - mRect.left, y - mRect.top))
					{
						return (area->GetAreaDisplay(IMAGE_ELEMENT, WEBC_FALSE));
					}
				}
				child = child->mpNext;
			}

			x += IMAGE_ELEMENT->GetHSpace();
			y += IMAGE_ELEMENT->GetVSpace();
		}
	}

	// if we contain the point, trap it
	if (mRect.Contains(x,y))
	{
		return (this);
	}

	return (0);
}

void HTMLImageDisplay::SetImageDimensions (void)
{
	if (!mpHtmlElement)
	{
		return;
	}

	HTMLImage *pImageElement = (HTMLImage *) mpHtmlElement;
	DISPLAY_INT iPixelWidth, iPixelHeight;
	CSSLength width, height;

	DISPLAY_INT iBitmapWidth = mpImage? mpImage->Width() : WEBC_BROKEN_IMG_LINK_WIDTH;
	DISPLAY_INT iBitmapHeight = mpImage? mpImage->Height() : WEBC_BROKEN_IMG_LINK_HEIGHT;

	pImageElement->GetWidth(&width);
	pImageElement->GetHeight(&height);

	if (width.type == CSS_LENGTH_UNIT_AUTO)
	{
		if (height.type == CSS_LENGTH_UNIT_AUTO)
		{
			// width and height are both auto, so size to the bitmap dimensions
			iPixelWidth = iBitmapWidth;
			iPixelHeight = iBitmapHeight;
		}
		else
		{
			// height is specified but width is not; set height explicitly and
			//  set width so as to maintain aspect ratio
			iPixelHeight = CSS_LengthToPixels(&height, miParentHeight, 0, 0);
			iPixelWidth = (iBitmapHeight == 0)? 0 : (iBitmapWidth * iPixelHeight) / iBitmapHeight;
		}
	}
	else
	{
		if (height.type == CSS_LENGTH_UNIT_AUTO)
		{
			// width is specified but height is not; set width explicitly and
			//  set height so as to maintain aspect ratio
			iPixelWidth = CSS_LengthToPixels(&width, miParentWidth, 0, 0);
			iPixelHeight = (iBitmapWidth == 0)? 0 : (iBitmapHeight * iPixelWidth) / iBitmapWidth;
		}
		else
		{
			// width and height are both specified; set both explicitly
			iPixelWidth = CSS_LengthToPixels(&width, miParentWidth, 0, 0);
			iPixelHeight = CSS_LengthToPixels(&height, miParentHeight, 0, 0);
		}
	}

#if (WEBC_SUPPORT_SMALL_SCREEN)
	if (iPixelWidth > WEBC_SCREEN_WIDTH)
	{
		DISPLAY_INT smallHeight = (iPixelHeight * WEBC_SCREEN_WIDTH) / iPixelWidth;
		DISPLAY_INT smallWidth  = WEBC_SCREEN_WIDTH;
		if (iPixelHeight >= 14 && iPixelHeight <= 72 && iPixelWidth/iPixelHeight >= 4)
		{
			// assume this is some kind of text label; don't scale down to less than 14 pixels high
			if (smallHeight < 14)
			{
				smallWidth = (iPixelWidth * 14) / iPixelHeight;
				smallHeight = 14;
			}
		}
		iPixelWidth  = smallWidth;
		iPixelHeight = smallHeight;
	}
#endif // WEBC_SUPPORT_SMALL_SCREEN

#if 0
	// this conditional breaks stuff; why is it here?!
	//if ((GetPosition() != TU_POSITION_ABSOLUTE && GetPosition() != TU_POSITION_FIXED) ||
	//    (width.type != CSS_LENGTH_UNIT_AUTO && height.type != CSS_LENGTH_UNIT_AUTO))
	{
		SizeTo(iPixelWidth  + pImageElement->GetHSpace() * 2,
		       iPixelHeight + pImageElement->GetVSpace() * 2);
	}
#else
	SizeTo(iPixelWidth , iPixelHeight);
#endif
}

WEBC_BOOL HTMLImageDisplay::SetElementStyle (void)
{
	WEBC_BOOL changed = WEBC_FALSE;

	mBorder.ResetProperties();

	HTMLImage *pImageElement = (HTMLImage*) mpHtmlElement;
	WebcImage *pImage = pImageElement->GetImage();
	if (mpImage != pImage)
	{
		SetImage(pImage);

		changed = WEBC_TRUE;
	}

	CSSBorderStyle outlineStyle = mpHtmlElement->GetOutlineStyle();
	if (outlineStyle != mOutlineStyle)
	{
		mOutlineStyle = outlineStyle;
		changed = WEBC_TRUE;
	}

	WebGraphics *gc = GetGraphics();
	if (gc)
	{
		HTMLColor rgbColor;
		WebColor oldColor = mBackgroundColor;
		rgbColor = mpHtmlElement->GetBackgroundColor();
		if (HTML_ColorGetAlpha(rgbColor) == 255)
		{
			if (mImageDisplayFlags & DHIMG_FLAG_HAS_BGCOLOR)
			{
				changed = WEBC_TRUE;
			}
			mImageDisplayFlags &= ~DHIMG_FLAG_HAS_BGCOLOR;
		}
		else
		{
			mBackgroundColor = gc->RGBToColor(rgbColor);
			if (mBackgroundColor != oldColor || !(mImageDisplayFlags & DHIMG_FLAG_HAS_BGCOLOR))
			{
				changed = WEBC_TRUE;
			}
			mImageDisplayFlags |= DHIMG_FLAG_HAS_BGCOLOR;
		}

		mHtmlElementDisplayFlags |= HELEM_DISPLAY_FLAG_STYLE_SET;
	}

	if (mpHtmlElement->mFlags & HINPUT_FLAG_DISABLED)
	{
		Disable();
	}
	else
	{
		Enable();
	}

	WebRect oldRect(mRect);
	SetImageDimensions();
	if (!oldRect.Equals(&mRect))
	{
		changed = WEBC_TRUE;
	}

	return (changed);
}

WebColor HTMLImageDisplay::GetBackgroundColor (void)
{
	return (mBackgroundColor);
}

WEBC_BOOL HTMLImageDisplay::HasBackgroundColor (void)
{
	return (mImageDisplayFlags & DHIMG_FLAG_HAS_BGCOLOR);
}

CSSBorderStyle HTMLImageDisplay::GetOutlineStyle (void)
{
	return (mOutlineStyle);
}

void HTMLImageDisplay::StepFrame (void)
{
	if (mpCurrentFrame)
	{
		mpCurrentFrame->Advance();
	}
}

void HTMLImageDisplay::Play (long msecTime)
{
  #if (WEBC_ANIMATE_IMAGES)

	HTMLBrowser* browser = mpHtmlElement->GetBrowser();

	if (browser)
	{
		Pause();

		mwTimerId = browser->TimeoutManager()->SetTimeout (
				(webcTimeoutFn) _AnimateImage,
				0, // destructor
				this,
				EBSMAX(1, msecTime),
				0 // loop
			);
	}

  #endif // WEBC_ANIMATE_IMAGES
}

void HTMLImageDisplay::Pause(void)
{
  #if (WEBC_ANIMATE_IMAGES)
	HTMLBrowser* browser = mpHtmlElement->GetBrowser();

	if (browser)
	{
		if (mwTimerId != WEBC_TIMEOUT_INVALID)
		{
			browser->TimeoutManager()->CancelTimeout(_matchId, &mwTimerId, WEBC_TRUE);
			mwTimerId = WEBC_TIMEOUT_INVALID;
		}
	}
  #endif
}

void HTMLImageDisplay::Stop(void)
{
	Pause();
}

void HTMLImageDisplay::SetImage (WebcImage *image)
{
  #if (WEBC_ANIMATE_IMAGES)
	Stop();
  #endif

	if (image)
	{
		image->Claim();
	}

	if (mpCurrentFrame)
	{
		mpImage->ReleaseFrame(mpCurrentFrame);
		mpCurrentFrame = 0;
	}

	if (mpImage)
	{
		mpImage->Release();
	}

	mpImage = image;
}

#if (WEBC_ANIMATE_IMAGES)
int _AnimateImage (void* data, long err)
{
HTMLImageDisplay *img = (HTMLImageDisplay *) data;

	if (img)
	{
		img->StepFrame();
		img->Invalidate();
	}

	return (0);
}
#endif // WEBC_ANIMATE_IMAGES

// returns the formatted content height
DISPLAY_INT HTMLImageDisplay::FormatContentPositioned (
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
	DISPLAY_INT iBitmapWidth = mpImage? mpImage->Width() : WEBC_BROKEN_IMG_LINK_WIDTH;
	DISPLAY_INT iBitmapHeight = mpImage? mpImage->Height() : WEBC_BROKEN_IMG_LINK_HEIGHT;

	return ((widthValue * iBitmapHeight) / iBitmapWidth);
}

WEBC_BOOL HTMLImageDisplay::GetIntrinsicWidth (
		DISPLAY_INT* intrinsicWidth,
		DISPLAY_INT  height,
		WEBC_BOOL    heightIsAuto
	)
{
	DISPLAY_INT iBitmapWidth = mpImage? mpImage->Width() : WEBC_BROKEN_IMG_LINK_WIDTH;

	if (heightIsAuto)
	{
		*intrinsicWidth = iBitmapWidth;
	}
	else
	{
		DISPLAY_INT iBitmapHeight = mpImage? mpImage->Height() : WEBC_BROKEN_IMG_LINK_HEIGHT;

		*intrinsicWidth = (height * iBitmapWidth) / iBitmapHeight;
	}

	return (WEBC_TRUE);
}
