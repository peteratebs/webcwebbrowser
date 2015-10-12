//
// DHFRAME.CPP - Methods for class HTMLFrameDisplay
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

#include "hframe.hpp"
#include "dhframe.hpp"
#include "htmldoc.hpp"
#include "dhelem.cpp"
#include "webcstr.h"


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
#if (WEBC_SUPPORT_FRAMES)

HTMLFrameDisplay::HTMLFrameDisplay (HTMLFrame *pElem)
	: HTMLElementDisplay<DisplayManager> (pElem), mBorder(pElem)
{
	SetElementStyle();
}

HTMLFrameDisplay::~HTMLFrameDisplay ()
{
}

void HTMLFrameDisplay::Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *view, WebGraphics *gc)
{
	if (GetVisibility() != TU_VISIBILITY_HIDE)
	{
		DisplayManager::Draw(x,y,view,gc);

		WebRect screenRect(mRect);
		screenRect.MoveTo(x,y);
		mBorder.DrawBorder(&screenRect, gc);
	}
}

void HTMLFrameDisplay::GetPaddingWidth (WebRect* padding)
{
	padding->left   = mBorder.GetPaddingWidth(CSS_LEFT)   + mBorder.GetBorderWidth(CSS_LEFT);
	padding->top    = mBorder.GetPaddingWidth(CSS_TOP)    + mBorder.GetBorderWidth(CSS_TOP);
	padding->right  = mBorder.GetPaddingWidth(CSS_RIGHT)  + mBorder.GetBorderWidth(CSS_RIGHT);
	padding->bottom = mBorder.GetPaddingWidth(CSS_BOTTOM) + mBorder.GetBorderWidth(CSS_BOTTOM);
}

HTMLEventStatus HTMLFrameDisplay::Event (HTMLEvent* event)
{
    WEBC_DEBUG_ReceiveEvent(this, event);
	switch (event->type)
	{
		case HTML_EVENT_UNFOCUS:
			if (RemoveFocus(mpFocus) == HTML_EVENT_STATUS_HALT)
			{
				return (HTML_EVENT_STATUS_HALT);
			}
			break;

		default:
			if (DisplayManager::Event(event) == HTML_EVENT_STATUS_HALT)
			{
				return (HTML_EVENT_STATUS_HALT);
			}
			break;
	}
	return (HTML_EVENT_STATUS_DONE);
}

DisplayElement *HTMLFrameDisplay::TrapEventPoint (DISPLAY_INT x, DISPLAY_INT y, DISPLAY_INT viewX, DISPLAY_INT viewY)
{
	if (GetVisibility() == TU_VISIBILITY_HIDE)
	{
		return (0);
	}

	return (DisplayManager::TrapEventPoint(x,y,viewX,viewY));
}

DisplayElement *HTMLFrameDisplay::FocusDefault (void)
{
	return (GetManager());
}

void HTMLFrameDisplay::SetParentWidth(DISPLAY_INT ignored, DISPLAY_INT iWidth)
{
	mBorder.SetParentWidth(iWidth);
	SetWidth(iWidth);
}

void HTMLFrameDisplay::SetParentHeight(DISPLAY_INT iHeight)
{
	SetHeight(iHeight);
}

WEBC_BOOL HTMLFrameDisplay::SetElementStyle(void)
{
HTMLFrame *pElem = (HTMLFrame *) mpHtmlElement;

	if (!pElem->ScrollMode())
	{
		SetScrollMode(SCROLL_MODE_AUTO);
	}
	else
	{
		if (!webc_c_stricmp(pElem->ScrollMode(), "yes"))
		{
			SetScrollMode(SCROLL_MODE_BOTH);
		}
		else if (!webc_c_stricmp(pElem->ScrollMode(), "no"))
		{
			SetScrollMode(SCROLL_MODE_NONE);
		}
		else
		{
			SetScrollMode(SCROLL_MODE_AUTO);
		}
	}

	if (pElem->Border() == 0)
	{
		ClearManagerFlag(MANAGER_FLAG_BORDER);
	}
	else
	{
		SetManagerFlag(MANAGER_FLAG_BORDER);
	}

	return (WEBC_TRUE);
}

#endif // WEBC_SUPPORT_FRAMES
