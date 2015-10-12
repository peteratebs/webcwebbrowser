//
// DHLI.CPP - Methods for class HTMLListItemDisplay
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

#include "dhli.hpp"
#include "hli.hpp"
#include "wgcolors.hpp"
#include "wgbmp.hpp"
#include "dhelem.cpp"
#include "strbld.hpp"
#include "dmanager.hpp"
#include "wcconst.hpp"
#include "webcstr.h"
#include "webcmem.h"
#include "format.hpp"


/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/

#define FLOW_ELEMENT   ((HTMLFlow *) mpHtmlElement)

#define LI_ENUMERATE_BULLET      0
#define LI_ENUMERATE_REMAINING   1


/*****************************************************************************/
// Local Types
/*****************************************************************************/

class LILineContext : public LineContext
{
 private:
	WEBC_BOOL mLIFirstLineFormatted;
	HTMLListItemBulletDisplay* mBullet;
	HTMLListItemDisplay* mListItemDisplay;

 public:
	LILineContext (
			WEBC_UINT8 textAlign,
			HTMLListItemBulletDisplay* bullet,
			HTMLListItemDisplay* listItem
		);
	virtual void Finalize (void);
};

class NonBreakingLILineContext : public NonBreakingLineContext
{
 private:
	WEBC_BOOL mLIFirstLineFormatted;
	HTMLListItemBulletDisplay* mBullet;
	HTMLListItemDisplay* mListItemDisplay;

 public:
	NonBreakingLILineContext (
			WEBC_UINT8 textAlign,
			HTMLListItemBulletDisplay* bullet,
			HTMLListItemDisplay* listItem
		);
	virtual void Finalize (void);
};

/*****************************************************************************/
// Global Data
/*****************************************************************************/


/*****************************************************************************/
// Local functions
/*****************************************************************************/


/*****************************************************************************/
// HTMLListItemBulletDisplay methods
/*****************************************************************************/

HTMLListItemBulletDisplay::HTMLListItemBulletDisplay (HTMLListItem *pLI)
{
	mpListItem = pLI;
	mpDisplayParent = 0;
	mLabel[0] = WEBC_CHAR_NULL;
}

HTMLListItemBulletDisplay::~HTMLListItemBulletDisplay ()
{
}

void HTMLListItemBulletDisplay::SetParentListItemDisplay (HTMLListItemDisplay *pDisplayParent)
{
	mpDisplayParent = pDisplayParent;
}

void HTMLListItemBulletDisplay::Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC)
{
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

	WebFont font = mpListItem->GetWebFont();
	WebColor color;

	color = pGC->RGBToColor(mpListItem->GetColor());

	CSSListStylePosition position = mpListItem->GetListStylePosition();

	WebBitmap bitmap;

	switch (mpListItem->GetListStyleType())
	{
		case CSS_LIST_STYLE_LOWER_ALPHA:
		case CSS_LIST_STYLE_UPPER_ALPHA:
		case CSS_LIST_STYLE_LOWER_ROMAN:
		case CSS_LIST_STYLE_UPPER_ROMAN:
		case CSS_LIST_STYLE_DECIMAL:
			pGC->Text(x+3, y, mLabel, color, pGC->GetColorByIndex(WGC_WHITE), WEBC_FALSE, font);
			return;

		case CSS_LIST_STYLE_CIRCLE:
			bitmap = pGC->GetBitmapByIndex(WGB_CIRCLE_BULLET);
			break;

		case CSS_LIST_STYLE_SQUARE:
			bitmap = pGC->GetBitmapByIndex(WGB_SQUARE_BULLET);
			break;

		case CSS_LIST_STYLE_DISC:
			bitmap = pGC->GetBitmapByIndex(WGB_DISC_BULLET);
			break;

		default:
			return;
	}

	pGC->Bitmap(x + 6, y + 6, bitmap);
}

WEBC_BOOL HTMLListItemBulletDisplay::SetElementStyle(void)
{
	DISPLAY_INT h = 0, w = 0;
	WebGraphics *gc = GetManager()? GetManager()->GetGraphics() : 0;

	if (!gc || !mpListItem)
	{
		return (WEBC_FALSE);
	}

	WebBitmap bitmap;

	switch (mpListItem->GetListStyleType())
	{
		case CSS_LIST_STYLE_LOWER_ALPHA:
		case CSS_LIST_STYLE_UPPER_ALPHA:
		case CSS_LIST_STYLE_LOWER_ROMAN:
		case CSS_LIST_STYLE_UPPER_ROMAN:
		case CSS_LIST_STYLE_DECIMAL:
		{
			WebFont f = mpListItem->GetWebFont();
			if (f)
			{
				GetLabel(mLabel);
				w = gc->TextWidth(mLabel, f);
				h = WEB_FONT_HEIGHT(f);
				SizeTo(w+3,h);
			}
			return (WEBC_TRUE);
		}

		case CSS_LIST_STYLE_CIRCLE:
			bitmap = gc->GetBitmapByIndex(WGB_CIRCLE_BULLET);
			break;

		case CSS_LIST_STYLE_SQUARE:
			bitmap = gc->GetBitmapByIndex(WGB_SQUARE_BULLET);
			break;

		case CSS_LIST_STYLE_DISC:
		case CSS_LIST_STYLE_NONE:
		default:
			bitmap = gc->GetBitmapByIndex(WGB_DISC_BULLET);
			break;
	}

	if (bitmap)
	{
		SizeTo(WEB_BITMAP_WIDTH(bitmap) + 12, WEB_BITMAP_HEIGHT(bitmap) + 8);
	}

	return (WEBC_TRUE);
}

void HTMLListItemBulletDisplay::GetLabel(WebChar *pWGLabel)
{
WEBC_CHAR *pLabel;

	switch (mpListItem->GetListStyleType())
	{
		case CSS_LIST_STYLE_LOWER_ROMAN:
			{
				RomanNumeralBuilder bld(mpListItem->ListOrder());
				pLabel = bld.MallocString(__FILE__, __LINE__);
				if (pLabel)
				{
					webc_strcpy(pWGLabel, pLabel);
					webc_strcat(pWGLabel, WEBC_STR_DOT);
					bld.FreeString(pLabel, __FILE__, __LINE__);
				}
			}
			break;

		case CSS_LIST_STYLE_UPPER_ROMAN:
			{
				RomanNumeralBuilder bld(mpListItem->ListOrder());
				ToUpperCaseBuilder filt;
				FilteredStringBuilder fbld(&bld, &filt);
				pLabel = fbld.MallocString(__FILE__, __LINE__);
				if (pLabel)
				{
					webc_strcpy(pWGLabel, pLabel);
					webc_strcat(pWGLabel, WEBC_STR_DOT);
					fbld.FreeString(pLabel, __FILE__, __LINE__);
				}
			}
			break;

		case CSS_LIST_STYLE_LOWER_ALPHA:
			{
				char label[4];
				label[3]='\0';
				label[2]=' ';
				label[1]='.';
				label[0]=((mpListItem->ListOrder()-1) % 26) + 'a';
				webc_c_strcpy(pWGLabel, label);
			}
			break;

		case CSS_LIST_STYLE_UPPER_ALPHA:
			{
				char label[4];
				label[3]='\0';
				label[2]=' ';
				label[1]='.';
				label[0]=((mpListItem->ListOrder()-1) % 26) + 'A';
				webc_c_strcpy(pWGLabel, label);
			}
			break;

		case CSS_LIST_STYLE_DECIMAL:
			{
				DecimalStringBuilder bld(mpListItem->ListOrder());
				pLabel = bld.MallocString(__FILE__, __LINE__);
				if (pLabel)
				{
					webc_strcpy(pWGLabel, pLabel);
					webc_strcat(pWGLabel, WEBC_STR_DOT);
					bld.FreeString(pLabel, __FILE__, __LINE__);
				}
			}
			break;

		default:
			pWGLabel[0] = WEBC_CHAR_NULL;
			break;
	}
}

DISPLAY_INT HTMLListItemBulletDisplay::GetBaseline(void)
{
	switch (mpListItem->GetListStyleType())
	{
		case CSS_LIST_STYLE_LOWER_ALPHA:
		case CSS_LIST_STYLE_UPPER_ALPHA:
		case CSS_LIST_STYLE_LOWER_ROMAN:
		case CSS_LIST_STYLE_UPPER_ROMAN:
		case CSS_LIST_STYLE_DECIMAL:
			if (mpListItem)
			{
				WebFont font = mpListItem->GetWebFont();
				if (font)
				{
					return WEB_FONT_BASELINE(font);
				}
			}
			return ((Height() << 2) / 5 + 1);
        default:
            break;
	}

	return (Height());
}

DISPLAY_INT HTMLListItemBulletDisplay::GetPixelWidth(void)
{
	return (Width());
}

DISPLAY_INT HTMLListItemBulletDisplay::GetPixelHeight(void)
{
	return (Height());
}

void HTMLListItemBulletDisplay::SetXYPosition(DISPLAY_INT x, DISPLAY_INT y)
{
	SetFixedPosition(WEBC_FALSE);
	MoveTo(x,y);
}

WEBC_UINT16 HTMLListItemBulletDisplay::GetVisibility(void)
{
	return (mpDisplayParent->GetVisibility());
}

void HTMLListItemBulletDisplay::SetParentDisplay(DisplayElement* parent, DisplayElement* insertBefore)
{
	DisplayManager* oldDisplayManager = GetManager();

	if (insertBefore)
	{
		parent->InsertBefore(insertBefore, this);
	}
	else
	{
		parent->InsertLast(this);
	}

	if (GetManager() != oldDisplayManager)
	{
		SetElementStyle();
	}
}

void HTMLListItemBulletDisplay::SetDisplayContext(DisplayElement *parent)
{
	if (GetManager() != parent->GetManager())
	{
		SetManager(parent->GetManager());
		SetElementStyle();
	}
}

CSSListStylePosition HTMLListItemBulletDisplay::GetListStylePosition (void)
{
	return mpListItem? mpListItem->GetListStylePosition() : CSS_LIST_STYLE_POSITION_OUTSIDE;
}

/*****************************************************************************/
// HTMLListItemDisplay methods
/*****************************************************************************/

HTMLListItemDisplay::HTMLListItemDisplay(HTMLListItem *pLI)
	: HTMLFlowDisplay(pLI), mBullet(pLI)
{
	mBullet.SetParentListItemDisplay(this);
}

TextUnitEnumerator* HTMLListItemDisplay::GetTextUnits(void)
{
HTMLListItemContentEnumerator *e;

	WEBC_NEW_VERBOSE(e, HTMLListItemContentEnumerator(mpHtmlElement, &mBullet), "HTMLListItemContentEnumerator");
	return (e);
}

LineContext* HTMLListItemDisplay::CreateLineContext(void)
{
	LineContext *currentLine;

	if (GetChildWrap() == TU_WRAP_NONE)
	{
		WEBC_NEW_VERBOSE(currentLine, NonBreakingLILineContext(FLOW_ELEMENT->GetChildTextAlign(), &mBullet, this),"CreateLineContext");
	}
	else
	{
		WEBC_NEW_VERBOSE(currentLine, LILineContext(FLOW_ELEMENT->GetChildTextAlign(), &mBullet, this),"CreateLineContext");
	}

	return (currentLine);
}

DISPLAY_INT HTMLListItemDisplay::GetPixelHeight(void)
{
	DISPLAY_INT blockHeight = HTMLFlowDisplay::GetPixelHeight();
	DISPLAY_INT bulletHeight = mBullet.Height();

	return (EBSMAX(blockHeight, bulletHeight));
}

/*****************************************************************************/
// HTMLListItemContentEnumerator methods
/*****************************************************************************/

HTMLListItemContentEnumerator::HTMLListItemContentEnumerator (HTMLElement *pRoot, HTMLListItemBulletDisplay *pBullet)
	: DisplayTUEnumerator(pRoot)
{
	mpBullet = pBullet;
	if (mpBullet && mpBullet->GetListStylePosition() == CSS_LIST_STYLE_POSITION_INSIDE)
	{
		mState = LI_ENUMERATE_BULLET;
	}
	else
	{
		mState = LI_ENUMERATE_REMAINING;
	}
}

TextUnit *HTMLListItemContentEnumerator::GetNextUnit (void)
{
	switch (mState)
	{
		case LI_ENUMERATE_BULLET:
			mState = LI_ENUMERATE_REMAINING;
			return (mpBullet);

		case LI_ENUMERATE_REMAINING:
			break;
	}

	return (DisplayTUEnumerator::GetNextUnit());
}

/*****************************************************************************/
// HTMLListItemContentEnumerator methods
/*****************************************************************************/
LILineContext::LILineContext (
		WEBC_UINT8 textAlign,
		HTMLListItemBulletDisplay* bullet,
		HTMLListItemDisplay* listItem
	)
	: LineContext (textAlign)
{
	mLIFirstLineFormatted = WEBC_FALSE;
	mBullet = bullet;
	mListItemDisplay = listItem;
}

void LILineContext::Finalize (void)
{
	if (!mpFirstUnit)
	{
		return;
	}

	DISPLAY_INT baselineHeight;
	DISPLAY_INT lineHeight;
	DISPLAY_INT textTopPosition;
	DISPLAY_INT textBottomPosition;

	CalculateLineHeight (
			&baselineHeight,
			&lineHeight,
			&textTopPosition,
			&textBottomPosition
		);

	if (!mLIFirstLineFormatted)
	{
		if (mBullet && mBullet->GetListStylePosition() == CSS_LIST_STYLE_POSITION_OUTSIDE)
		{
			mBullet->SetParentDisplay(mListItemDisplay, 0);

			DISPLAY_INT x = miLeft - mBullet->Width();
			DISPLAY_INT y = miTop + baselineHeight - mBullet->GetBaseline();

			mBullet->SetXYPosition(x,y);
		}

		mLIFirstLineFormatted = WEBC_TRUE;
	}

	LineContext::Finalize (
			baselineHeight,
			lineHeight,
			textTopPosition,
			textBottomPosition
		);
}

/*****************************************************************************/
// HTMLListItemContentEnumerator methods
/*****************************************************************************/
NonBreakingLILineContext::NonBreakingLILineContext (
		WEBC_UINT8 textAlign,
		HTMLListItemBulletDisplay* bullet,
		HTMLListItemDisplay* listItem
	)
	: NonBreakingLineContext (textAlign)
{
	mLIFirstLineFormatted = WEBC_FALSE;
	mBullet = bullet;
	mListItemDisplay = listItem;
}

void NonBreakingLILineContext::Finalize (void)
{
	if (!mpFirstUnit)
	{
		return;
	}

	DISPLAY_INT baselineHeight;
	DISPLAY_INT lineHeight;
	DISPLAY_INT textTopPosition;
	DISPLAY_INT textBottomPosition;

	CalculateLineHeight (
			&baselineHeight,
			&lineHeight,
			&textTopPosition,
			&textBottomPosition
		);

	if (!mLIFirstLineFormatted)
	{
		if (mBullet && mBullet->GetListStylePosition() == CSS_LIST_STYLE_POSITION_OUTSIDE)
		{
			mBullet->SetParentDisplay(mListItemDisplay, 0);

			DISPLAY_INT x = miLeft - mBullet->Width();
			DISPLAY_INT y = miTop + baselineHeight - mBullet->GetBaseline();

			mBullet->SetXYPosition(x,y);
		}

		mLIFirstLineFormatted = WEBC_TRUE;
	}

	NonBreakingLineContext::Finalize (
			baselineHeight,
			lineHeight,
			textTopPosition,
			textBottomPosition
		);
}
