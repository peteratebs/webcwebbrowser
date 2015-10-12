//
// DHBODY.CPP - Methods for class HTMLBodyDisplay
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006	CSSPropertyValue overflowValue;
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//

/*****************************************************************************/
// Header Files
/*****************************************************************************/

#include "dhbody.hpp"
#include "hbody.hpp"
#include "dmanager.hpp"
#include "htmldoc.hpp"
#include "tuenum.hpp"
#include "margins.hpp"
#include "dhframeset.hpp"
#include "formatctx.hpp"
#include "webcassert.h"
#include "dhelem.cpp"

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

// template long HTMLElementDisplay<DisplayElement>::GetZIndex(void);

HTMLBodyDisplay::HTMLBodyDisplay (HTMLBody * f)
	: HTMLFlowDisplay (f)
{
  #if (WEBC_SUPPORT_FRAMES)
	mpFrameSet = 0;
  #endif // WEBC_SUPPORT_FRAMES

	SetFlag(DISPLAY_FLAG_ACCEPTS_FOCUS);
}

HTMLBodyDisplay::~HTMLBodyDisplay ()
{
}

/*---------------------------------------------------------------------------*/
void HTMLBodyDisplay::SetParentWidth(DISPLAY_INT ignored, DISPLAY_INT newParentWidth)
{
	DISPLAY_INT oldParentWidth = miParentWidth;
	DISPLAY_INT newWidth = newParentWidth;

	miParentWidth = newParentWidth;
	mBorder.SetParentWidth(newParentWidth);

	if (newWidth != Width() ||
	    (miParentWidth != oldParentWidth && mBorder.HasPercent()))
	{
		SetWidth(newWidth);
	}
}

/*---------------------------------------------------------------------------*/
void HTMLBodyDisplay::SetParentHeight(DISPLAY_INT iHeight)
{
	miParentHeight = iHeight;
}

/*---------------------------------------------------------------------------*/
void HTMLBodyDisplay::Format(void)
{
	DisplayManager *manager = GetManager();

	if (manager)
	{
		PresetWebRect rect(&manager->mViewRect);
		DISPLAY_INT viewportWidth  = rect.Width();
		DISPLAY_INT viewportHeight = rect.Height();
		WEBC_DEBUG_LOG_TIME_HANDLE(TimeInside)

		WEBC_DEBUG_LOG_TIME_ENTER(TimeInside,"HTMLBodyDisplay::Format", " ")
		rect.MoveTo(0, 0);


		SetParentWidth(viewportWidth, viewportWidth);
		SetParentHeight(viewportHeight);

#if (WEBC_SUPPORT_FRAMES)
		HTMLBody* bodyElement = (HTMLBody*) mpHtmlElement;
		HTMLFrameSet* framesetElement = (bodyElement)? bodyElement->GetFrameSet() : 0;

		mpFrameSet = (HTMLFrameSetDisplay*) ((framesetElement)? framesetElement->GetDisplayElement() : 0);

		if (mpFrameSet)
		{
			// remove all old children
			while (mpFirstChild)
			{
				Remove(mpFirstChild);
			}

			// add the frameset
			mpFrameSet->SetParentWidth(viewportWidth, viewportWidth);
			mpFrameSet->SetParentHeight(viewportHeight);
			mpFrameSet->SetXYPosition(0,0);
			mpFrameSet->SetParentDisplay(this, 0);
			miContentHeight = viewportHeight;
			miMaxWidth = viewportWidth;
			miMinWidth = viewportWidth;

			mFlowFlags &= ~(FD_FLAG_POS_CHILD_STYLE_MODIFIED|
			                FD_FLAG_FLOW_CHILD_STYLE_MODIFIED|
			                FD_FLAG_LOCAL_STYLE_MODIFIED|
							FD_FLAG_PARENT_STYLE_MODIFIED);
		}
		else
#endif // WEBC_SUPPORT_FRAMES
		{
			DISPLAY_INT marginLeftValue   = GetLeftMargin();
			DISPLAY_INT marginRightValue  = GetRightMargin();
			DISPLAY_INT marginTopValue    = GetTopMargin();

			DISPLAY_INT paddingLeftValue  = mBorder.GetPaddingWidth(CSS_LEFT);
			DISPLAY_INT paddingRightValue = mBorder.GetPaddingWidth(CSS_RIGHT);
			DISPLAY_INT paddingTopValue   = mBorder.GetPaddingWidth(CSS_TOP);

			// check to see if we just need to update positioned child formatting
			if (mFlowFlags & (FD_FLAG_FLOW_CHILD_STYLE_MODIFIED|
			                  FD_FLAG_LOCAL_STYLE_MODIFIED|
			                  FD_FLAG_PARENT_STYLE_MODIFIED))
			{
				DISPLAY_INT iContentWidth;
				MarginContext margin (marginLeftValue + paddingLeftValue,
				                      viewportWidth - (marginRightValue + paddingRightValue) - 1, this);

				// Format the flow content
				miContentHeight = FormatFlowContent (&margin,
				                                     marginLeftValue + paddingLeftValue,
				                                     marginTopValue + paddingTopValue, &iContentWidth);

				// update miContentHeight to include anything in the margins
				DISPLAY_INT topPad = marginTopValue + paddingTopValue;
				DISPLAY_INT clearedHeight = margin.ClearAll(topPad + miContentHeight) - topPad;
				if (clearedHeight > miContentHeight)
				{
					miContentHeight = clearedHeight;
				}

				// set this element's height
				UpdateHeight(0, 0);
			}

			// Format positioned elements
			FormatContextFactory childContextFactory;

			if (mPositionedChildFormatContext)
			{
				WebRect oldRect;

				mPositionedChildFormatContext->getContainingBlock(&oldRect);

				if (!oldRect.Equals(&rect) ||
				    miContentHeight != mPositionedChildFormatContext->getAutoIndentTop())
				{
					mPositionedChildFormatContext->release();
					mPositionedChildFormatContext = 0;
				}
			}

			WEBC_BOOL reformatPositionedChildren = (mFlowFlags & FD_FLAG_POS_CHILD_STYLE_MODIFIED) != 0;

			if (!mPositionedChildFormatContext)
			{
				mPositionedChildFormatContext = childContextFactory.newPositioned (
						&rect,            // containingBlock,
						WEBC_FALSE,       // parentHeightIsAuto,
						TU_DIR_LTR,       // textDirection,
						0,                // parentBorderLeft,
						0,                // parentBorderTop,
						0,                // parentBorderRight,
						0,                // parentBorderBottom,
						marginLeftValue,  // autoIndentLeft,
						miContentHeight,  // autoIndentTop,
						marginRightValue  // autoIndentRight
					);

				reformatPositionedChildren = (mPositionedChildFormatContext != WEBC_NULL);
			}

			if (reformatPositionedChildren)
			{
				HTMLFlowDisplayFormatContext formatCtx;

				BeginFormatPositionedChildren(&formatCtx);

				FormatPositionedChildren (
						mPositionedChildFormatContext,
						mPositionedChildFormatContext,
						&childContextFactory
					);

				EndFormatPositionedChildren(&formatCtx);
			}

			mFlowFlags &= ~(FD_FLAG_POS_CHILD_STYLE_MODIFIED|
			                FD_FLAG_FLOW_CHILD_STYLE_MODIFIED|
			                FD_FLAG_LOCAL_STYLE_MODIFIED|
							FD_FLAG_PARENT_STYLE_MODIFIED);
		}

		WEBC_ASSERT(!NeedsFormat());

		// handles the case where HTMLDocument::ScrollToAnchor is called before the
		//  document's content has been formatted.
		if (mpHtmlElement)
		{
			HTMLDocument* document = mpHtmlElement->GetDocument();
			if (document)
			{
				document->ScrollToDefaultAnchor();
				document->SetQueuedFocus();
			}
		}
		WEBC_DEBUG_LOG_TIME_EXIT(TimeInside,"HTMLBodyDisplay::Format", " ")
	}
}

/*---------------------------------------------------------------------------*/
void HTMLBodyDisplay::GetSpecifiedHeight (DISPLAY_INT *heightValue, WEBC_BOOL *boolHeightIsAuto)
{
	DisplayManager *manager = GetManager();
	if (manager)
	{
		*heightValue = manager->mViewRect.Height();
		*boolHeightIsAuto = 0;
	}
	else
	{
		*heightValue = 0;
		*boolHeightIsAuto = 1;
	}
}

/*---------------------------------------------------------------------------*/
void HTMLBodyDisplay::UpdateHeight (
		DISPLAY_INT topPad,
		DISPLAY_INT bottomPad
	)
{
	DISPLAY_INT heightValue;
	WEBC_BOOL heightIsAuto;

	GetSpecifiedHeight(&heightValue, &heightIsAuto);

	if (heightIsAuto)
	{
		heightValue = miContentHeight;
	}
	else
	{
		if (heightValue < miContentHeight)
		{
			heightValue = miContentHeight;
		}
	}

	SetHeight(topPad + heightValue + bottomPad);
}

/*---------------------------------------------------------------------------*/
void HTMLBodyDisplay::GetBounds (WebRect* rect)
{
	DisplayElement::GetBounds(rect);

	if (rect->left < 0)
	{
		rect->left = 0;
	}

	if (rect->top < 0)
	{
		rect->top = 0;
	}
}

/*---------------------------------------------------------------------------*/
void HTMLBodyDisplay::Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC)
{
	FormatIfNecessary();

  #if (WEBC_SUPPORT_FRAMES)
	if (mpFrameSet)
	{
		mpFrameSet->Draw(x, y, pViewport, pGC);
	}
	else
  #endif //	WEBC_SUPPORT_FRAMES
	{
		if (!(mpHtmlElement->mFlags & HBODY_FLAG_NOBACKGROUND))
		{
			WebRect screenRect;
			screenRect.Set(x, y, pViewport->right, pViewport->bottom);
			DrawBackground(&screenRect, pViewport, pGC);
		}

		// Draw all children
		DisplayElement *pChild = mpFirstChild;
		while (pChild)
		{
			if (pChild->mFlags & DISPLAY_FLAG_FIXED_POS)
			{
				pChild->Draw(pViewport->left + pChild->mRect.left, pViewport->top + pChild->mRect.top, pViewport, pGC);
			}
			else
			{
				pChild->Draw(x + pChild->mRect.left, y + pChild->mRect.top, pViewport, pGC);
			}
			pChild = pChild->mpNext;
		}
	}
}

FormatResult HTMLBodyDisplay::FormatForViewport (
		WebRect* viewportRect,
		WEBC_BOOL hscrollPresent,
		WEBC_BOOL vscrollPresent
	)
{
	if (Width() != viewportRect->Width())
	{
		SizeTo(viewportRect->Width(), viewportRect->Height());
		SetParentStyleModified();
	}
	else if (Height() != viewportRect->Height())
	{
		SizeTo(viewportRect->Width(), viewportRect->Height());
		SetPosChildStyleModified();
	}

	FormatIfNecessary();
	CSSPropertyValue overflowValue;
	if (mpHtmlElement->GetStyleFromCSS(CSS_PROPERTY_OVERFLOW, &overflowValue) == CSS_VALUE_SPECIFIED)
	{
		if (CSS_OVERFLOW_HIDDEN == overflowValue.overflow)
		{
			return (DISPLAY_FORMAT_SUCCESS);
		}
	}
#if (WEBC_SUPPORT_SCROLLBARS)
	WebRect bounds;
	GetBounds(&bounds);

	if (bounds.Height() > viewportRect->Height() && !vscrollPresent)
	{
		return (DISPLAY_FORMAT_NEEDS_VSCROLL);
	}

	if (bounds.Width() > viewportRect->Width() && !hscrollPresent)
	{
		return (DISPLAY_FORMAT_NEEDS_HSCROLL);
	}
#endif
	return (DISPLAY_FORMAT_SUCCESS);
}

// ok, this is a silly thing to do but it will work.
void HTMLBodyDisplay::SetHtmlFocus (HTMLElement *pElement)
{
	if (!GetManager())
	{
		return;
	}

	if (!(mKeyCatcher1.mFlags & DISPLAY_FLAG_FOCUS) && (mKeyCatcher2.GetElement() != pElement))
	{
		// this is the default case
		mKeyCatcher1.SetElement(pElement);
		GetManager()->SetFocus(&mKeyCatcher1);
		mKeyCatcher2.SetElement(0);
	}
	else if (!(mKeyCatcher2.mFlags & DISPLAY_FLAG_FOCUS) && (mKeyCatcher1.GetElement() != pElement))
	{
		// give it to key catcher number 2
		mKeyCatcher2.SetElement(pElement);
		GetManager()->SetFocus(&mKeyCatcher2);
		mKeyCatcher1.SetElement(0);
	}
}

void HTMLBodyDisplay::RemoveHtmlFocus (HTMLElement *pElement)
{
	if (mKeyCatcher1.GetElement() == pElement)
	{
		if (GetManager())
		{
			GetManager()->RemoveFocus(&mKeyCatcher1);
		}
		mKeyCatcher1.SetElement(0);
	}

	if (mKeyCatcher2.GetElement() == pElement)
	{
		if (GetManager())
		{
			GetManager()->RemoveFocus(&mKeyCatcher2);
		}
		mKeyCatcher2.SetElement(0);
	}
}

void HTMLBodyDisplay::RemoveHtmlElementReferences (HTMLElement *pElement)
{
	if (mKeyCatcher1.GetElement() == pElement)
	{
		if (GetManager())
		{
			GetManager()->RemoveElementReferences(&mKeyCatcher1);
		}
		mKeyCatcher1.SetElement(0);
	}

	if (mKeyCatcher2.GetElement() == pElement)
	{
		if (GetManager())
		{
			GetManager()->RemoveElementReferences(&mKeyCatcher2);
		}
		mKeyCatcher2.SetElement(0);
	}
}

void HTMLBodyDisplay::SetLocalStyleModified (void)
{
	DisplayManager* manager = GetManager();

	if (manager)
	{
		manager->InvalidateViewport();
	}

	HTMLFlowDisplay::SetLocalStyleModified();
}

void HTMLBodyDisplay::SetPosChildStyleModified (void)
{
	DisplayManager* manager = GetManager();

	if (manager)
	{
		manager->InvalidateViewport();
	}

	HTMLFlowDisplay::SetPosChildStyleModified();
}

void HTMLBodyDisplay::SetFlowChildStyleModified (void)
{
	DisplayManager* manager = GetManager();

	if (manager)
	{
		manager->InvalidateViewport();
	}

	HTMLFlowDisplay::SetFlowChildStyleModified();
}

/*---------------------------------------------------------------------------*/
WEBC_BOOL HTMLBodyDisplay::NeedsFormat (void)
{
	return (!(mFlowFlags & FD_FLAG_FLOW_FORMAT_IN_PROGRESS) &&
	        (mFlowFlags & (FD_FLAG_FLOW_CHILD_STYLE_MODIFIED|
	                       FD_FLAG_POS_CHILD_STYLE_MODIFIED|
	                       FD_FLAG_LOCAL_STYLE_MODIFIED|
	                       FD_FLAG_PARENT_STYLE_MODIFIED)));
}

/*---------------------------------------------------------------------------*/
WEBC_BOOL HTMLBodyDisplay::IgnoreInFindMinWidth(TextUnit *pTU)
{
	return (WEBC_FALSE);
}
