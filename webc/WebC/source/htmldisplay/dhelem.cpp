//
// DHELEM.CPP - Methods for class HTMLElementDisplay
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

#include "dhelem.hpp"
#include "helement.hpp"
#include "dbgtimer.h"
#include "tuenum.hpp"
#include "formatctx.hpp"
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

template<class T> HTMLElementDisplay<T>::HTMLElementDisplay (HTMLElement *pElem)
	: HTMLTextUnitMirror (pElem)
{
	mHtmlElementDisplayFlags = 0;
}

template<class T> HTMLElementDisplay<T>::~HTMLElementDisplay ()
{
}

// DisplayElement methods
template<class T> void HTMLElementDisplay<T>::Move (WebRect * pRect)
{
	if (T::mpParent && ((pRect->Width() != T::Width()) || (pRect->Height() != T::Height())))
	{
		T::Move(pRect);
		NotifyEvent what(NOTIFIER_TYPE_TEXT_UNIT, NOTIFY_SIZE_CHANGE);
		T::mpParent->Notify((TextUnit *) this, &what);
	}
	else
	{
		T::Move(pRect);
	}
}

template<class T> void HTMLElementDisplay<T>::SizeTo (DISPLAY_INT w, DISPLAY_INT h)
{
	if (T::mpParent && ((w != T::Width()) || (h != T::Height())))
	{
		T::SizeTo(w, h);
		NotifyEvent what(NOTIFIER_TYPE_TEXT_UNIT, NOTIFY_SIZE_CHANGE);
		T::mpParent->Notify((TextUnit *) this, &what);
	}
	else
	{
		T::SizeTo(w, h);
	}
}

template<class T> void HTMLElementDisplay<T>::SetHeight (DISPLAY_INT h)
{
	if (T::mpParent && (h != T::Height()))
	{
		T::SetHeight(h);
		NotifyEvent what(NOTIFIER_TYPE_TEXT_UNIT, NOTIFY_SIZE_CHANGE);
		T::mpParent->Notify((TextUnit *) this, &what);
	}
	else
	{
		T::SetHeight(h);
	}
}

template<class T> void HTMLElementDisplay<T>::SetWidth (DISPLAY_INT w)
{
    if (T::mpParent && (w != T::Width()))
	{
		T::SetWidth(w);
		NotifyEvent what(NOTIFIER_TYPE_TEXT_UNIT, NOTIFY_SIZE_CHANGE);
        T::mpParent->Notify((TextUnit *) this, &what);
	}
	else
	{
		T::SetWidth(w);
	}
}

template<class T> void HTMLElementDisplay<T>::DrawThisOnly (DISPLAY_INT x, DISPLAY_INT y, WebGraphics *gc)
{
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLELEMENTDISPLAY_DRAWTHISONLY);

	if (GetVisibility() != TU_VISIBILITY_HIDE)
	{
		T::DrawThisOnly(x,y,gc);
	}

	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLELEMENTDISPLAY_DRAWTHISONLY);
}

template<class T> HTMLEventStatus HTMLElementDisplay<T>::Event (HTMLEvent *e)
{
	if (!mpHtmlElement)
		return HTML_EVENT_STATUS_CONTINUE;

	HTMLEventStatus status = mpHtmlElement->Event(e);
	if (status == HTML_EVENT_STATUS_CONTINUE)
	{
		return T::Event(e);
	}

	return (status);
}

template<class T> void HTMLElementDisplay<T>::Notify(Notifier *who, NotifyEvent *what)
{
	if (what && (who == mpHtmlElement))
	{
		const WebRect * tmpRect = 	&(((DisplayElement *)(this))->mRect);
		switch (what->event)
		{
		case NOTIFY_PARENT_STYLE_CHANGE:  // intentional fall-through
		case NOTIFY_FORMAT_STYLE_CHANGE:  // intentional fall-through
		case NOTIFY_STYLE_CHANGE:
		{
			WEBC_BOOL invalidated = WEBC_FALSE;
			PresetWebRect oldRect(tmpRect);

			if (SetElementStyle())
			{
				oldRect.Or(tmpRect);
				T::InvalidateRect(&oldRect);
				invalidated = WEBC_TRUE;
			}

			WEBC_UINT16 visible = GetVisibility();
			WEBC_UINT16 display = GetUnitType();
			WEBC_UINT16 position = GetPosition();


			if (InvalidateTUMirror())
			{
				if (!invalidated)
				{
					T::Invalidate();
					invalidated = WEBC_TRUE;
				}

				// GetHTMLParentDisplay will work even if we happen to be
				//  disconnected from the display tree at the moment.
				DisplayElement *parent = GetHTMLParentDisplay();

				if (parent)
				{
					NotifyEvent what(NOTIFIER_TYPE_TEXT_UNIT, NOTIFY_STYLE_CHANGE);
					what.data.oldStyle.display = display;
					what.data.oldStyle.position = position;
					parent->Notify((TextUnit *) this, &what);
				}
			}

			if (!invalidated)
			{
				if (visible != GetVisibility())
				{
					T::Invalidate();
					invalidated = WEBC_TRUE;
				}
			}
			break;
		}

		case NOTIFY_PARENT_DISPLAY_STYLE_CHANGE:  // intentional fall-through
		case NOTIFY_DISPLAY_STYLE_CHANGE:
		{
			WEBC_BOOL invalidated = WEBC_FALSE;

			PresetWebRect oldRect(tmpRect);
			if (SetElementStyle())
			{
				oldRect.Or(tmpRect);
				T::InvalidateRect(&oldRect);
				invalidated = WEBC_TRUE;
			}

			if (InvalidateVisibility())
			{
				if (!invalidated)
				{
					T::Invalidate();
					invalidated = WEBC_TRUE;
				}
			}

			if (InvalidateZIndex())
			{
				DisplayElement *parent = GetHTMLParentDisplay();
				if (parent)
				{
					NotifyEvent what(NOTIFIER_TYPE_TEXT_UNIT, NOTIFY_ZINDEX_CHANGE);
					parent->Notify((TextUnit*) this, &what);
				}

				if (!invalidated)
				{
					T::Invalidate();
					invalidated = WEBC_TRUE;
				}
			}
			break;
		}

		default:
			break;
		}
	}
	T::Notify(who, what);
}

template<class T> WEBC_INT32 HTMLElementDisplay<T>::GetZIndex (void)
{
	return (FindDisplayZIndex());
}

template<class T> WEBC_BOOL HTMLElementDisplay<T>::TrapEventInThis (DISPLAY_INT x, DISPLAY_INT y)
{

	return ((T::mRect.Contains(x,y)) && (GetVisibility() != TU_VISIBILITY_HIDE));
}

// TextUnit methods
template<class T> DISPLAY_INT HTMLElementDisplay<T>::GetMinWidth(void)
{
	return GetPixelWidth();
}

template<class T> DISPLAY_INT HTMLElementDisplay<T>::GetMaxWidth(void)
{
	return GetPixelWidth();
}

template<class T> DISPLAY_INT HTMLElementDisplay<T>::GetPixelWidth(void)
{
	return T::Width();
}

template<class T> DISPLAY_INT HTMLElementDisplay<T>::GetPixelHeight(void)
{
	return T::Height();
}

template<class T> DISPLAY_INT HTMLElementDisplay<T>::GetBaseline(void)
{
	return GetPixelHeight();
}

template<class T> void HTMLElementDisplay<T>::SetXYPosition(DISPLAY_INT iXPos, DISPLAY_INT iYPos)
{
	switch (GetPosition())
	{
	case TU_POSITION_RELATIVE:
		// tbd - this should take into account right and bottom if left or top aren't defined
		iXPos += GetLeft();
		iYPos += GetTop();
		// intentional fall-through
	default:
		T::SetFixedPosition(WEBC_FALSE);
		break;
	}

	T::MoveTo(iXPos, iYPos);
}

template<class T> void HTMLElementDisplay<T>::SetParentDisplay(DisplayElement* parent, DisplayElement* insertBefore)
{
	if (parent != T::mpParent)
	{
		mHtmlElementDisplayFlags &= ~HELEM_DISPLAY_FLAG_STYLE_SET;
	}

	if (insertBefore)
	{
		parent->InsertBefore(insertBefore, this);
	}
	else
	{
		parent->InsertLast(this);
	}
}

// HTMLElementDisplay methods
template<class T> void HTMLElementDisplay<T>::SetHtmlElement(HTMLElement *pElement)
{
	if (mpHtmlElement != pElement)
	{
		mpHtmlElement = pElement;
		if (InvalidateTUMirror() && T::mpParent)
		{
			NotifyEvent what(NOTIFIER_TYPE_TEXT_UNIT, NOTIFY_SIZE_CHANGE);
			T::mpParent->Notify((TextUnit *) this, &what);
		}
	}
}

template<class T> WEBC_BOOL HTMLElementDisplay<T>::SetElementStyle(void)
{
	return (WEBC_FALSE);
}

template<class T> void HTMLElementDisplay<T>::SetDisplayContext(DisplayElement *parent)
{
	if (T::GetManager() != parent->GetManager())
	{
		mHtmlElementDisplayFlags &= ~HELEM_DISPLAY_FLAG_STYLE_SET;
		T::SetManager(parent->GetManager());
	}
}

/*---------------------------------------------------------------------------*/
template<class T> WEBC_BOOL HTMLElementDisplay<T>::NeedsAutoIndentTop (
		WEBC_BOOL parentHeightIsAuto
	)
{
	CSSLength top;

	GetCSSLength(&top, CSS_PROPERTY_TOP);

	return (top.type == CSS_LENGTH_UNIT_AUTO ||
	        (top.type == CSS_LENGTH_UNIT_PERCENT && parentHeightIsAuto));
}

/*---------------------------------------------------------------------------*/
template<class T> IPositionedFormatContext* HTMLElementDisplay<T>::FormatSelfPositioned (
		DisplayElement* displayParent,
		IPositionedFormatContext* rootContext,
		IPositionedFormatContext* parentContext,
		FormatContextFactory* childContextFactory,
		WEBC_BOOL generateChildContext
	)
{
	WebRect rect;
	WEBC_UINT16 position = GetPosition();
	IPositionedFormatContext* childContext = 0;

	if (position == TU_POSITION_RELATIVE)
	{
		if (generateChildContext)
		{
			const WebRect * tmpRect = 	&(((DisplayElement *)(this))->mRect);
			rect.Set(tmpRect);
			rect.MoveTo(0,0);

			CSSLength height;
			GetCSSLength(&height, CSS_PROPERTY_HEIGHT);

			childContext = childContextFactory->newPositioned (
					&rect,
					(height.type == CSS_LENGTH_UNIT_AUTO) ||
					(height.type == CSS_LENGTH_UNIT_PERCENT && parentContext->parentHeightIsAuto()), // parent height is auto
					TU_DIR_LTR,
					0, // border-left-width
					0, // border-top-width
					0, // border-right-width
					0, // border-bottom-width
					0, // autoIndentLeft
					0, // autoIndentTop
					0  // autoIndentRight
				);
		}
	}
	else
	{
		WebRect containingBlock;
		IPositionedFormatContext* containingContext;
		DISPLAY_INT borderLeftValue;
		DISPLAY_INT borderTopValue;
		DISPLAY_INT borderRightValue;
		DISPLAY_INT borderBottomValue;

		if (displayParent)
		{
			if (displayParent != T::mpParent)
			{
				mHtmlElementDisplayFlags &= ~HELEM_DISPLAY_FLAG_STYLE_SET;
			}
			displayParent->InsertOrdered(this);
		}

		if (position == TU_POSITION_FIXED)
		{
			T::SetFixedPosition(WEBC_TRUE);
			containingContext = rootContext;
		}
		else
		{
			T::SetFixedPosition(WEBC_FALSE);
			containingContext = parentContext;
		}

		containingContext->getContainingBlock(&containingBlock);

		WEBC_BOOL   parentHeightIsAuto = containingContext->parentHeightIsAuto();
		int         textDirection      = containingContext->getTextDirection();
		DISPLAY_INT parentBorderLeft   = containingContext->getParentBorderLeft();
		DISPLAY_INT parentBorderTop    = containingContext->getParentBorderTop();
		DISPLAY_INT parentBorderRight  = containingContext->getParentBorderRight();
		DISPLAY_INT parentBorderBottom = containingContext->getParentBorderBottom();

		DISPLAY_INT autoIndentLeft     = parentContext->getAutoIndentLeft();
		DISPLAY_INT autoIndentTop      = parentContext->getAutoIndentTop();
		DISPLAY_INT autoIndentRight    = parentContext->getAutoIndentRight();

		if (position == TU_POSITION_FIXED)
		{
			WebRect parentRect;

			parentContext->getContainingBlock(&parentRect);

			autoIndentLeft  += parentRect.left;
			autoIndentTop   += parentRect.top;
			autoIndentRight += (containingBlock.right - parentRect.right);
		}

		DISPLAY_INT parentWidth = containingBlock.Width() - (parentBorderLeft + parentBorderRight);
		DISPLAY_INT parentHeight = containingBlock.Height() - (parentBorderTop + parentBorderBottom);

		WebFont font = mpHtmlElement->GetWebFont();
		DISPLAY_INT emHeight = font? WEB_FONT_HEIGHT(font) : WEBC_CFG_DEFAULT_TEXT_HEIGHT;
		DISPLAY_INT exHeight = emHeight >> 1;

		// First find the left edge and width

		CSSLength left;
		CSSLength right;
		CSSLength width;

		// Find value for left, right, width, marginLeft, marginRight

		GetCSSLength(&left,  CSS_PROPERTY_LEFT);
		GetCSSLength(&right, CSS_PROPERTY_RIGHT);
		GetCSSLength(&width, CSS_PROPERTY_WIDTH);

		DISPLAY_INT leftValue;
		DISPLAY_INT rightValue;
		DISPLAY_INT widthValue;
		DISPLAY_INT marginLeftValue;
		DISPLAY_INT paddingLeftValue;
		DISPLAY_INT paddingTopValue;
		DISPLAY_INT paddingRightValue;
		DISPLAY_INT paddingBottomValue;

		// Find value for padding and border

		paddingLeftValue   = GetPixelLength(CSS_PROPERTY_PADDING_LEFT,   parentWidth, emHeight, exHeight, 0);
		paddingTopValue    = GetPixelLength(CSS_PROPERTY_PADDING_TOP,    parentWidth, emHeight, exHeight, 0);
		paddingRightValue  = GetPixelLength(CSS_PROPERTY_PADDING_RIGHT,  parentWidth, emHeight, exHeight, 0);
		paddingBottomValue = GetPixelLength(CSS_PROPERTY_PADDING_BOTTOM, parentWidth, emHeight, exHeight, 0);

		borderLeftValue    = GetBorderWidth(CSS_LEFT,   parentWidth, emHeight, exHeight, parentBorderLeft);
		borderTopValue     = GetBorderWidth(CSS_TOP,    parentWidth, emHeight, exHeight, parentBorderTop);
		borderRightValue   = GetBorderWidth(CSS_RIGHT,  parentWidth, emHeight, exHeight, parentBorderRight);
		borderBottomValue  = GetBorderWidth(CSS_BOTTOM, parentWidth, emHeight, exHeight, parentBorderBottom);

		CSSLength height;
		CSSLength top;
		CSSLength bottom;
		DISPLAY_INT heightValue=0;
		WEBC_BOOL heightIsAuto=0;

		GetCSSLength(&height, CSS_PROPERTY_HEIGHT);
		GetCSSLength(&top, CSS_PROPERTY_TOP);

#define NOTHING_KNOWN 0
#define HEIGHT_KNOWN  1
#define TOP_KNOWN     2
#define BOTTOM_KNOWN  4

		WEBC_UINT8 verticalCase;

		verticalCase =  (height.type == CSS_LENGTH_UNIT_AUTO ||
		                 (height.type == CSS_LENGTH_UNIT_PERCENT && parentHeightIsAuto))? 0 : HEIGHT_KNOWN;
		verticalCase |= (top.type == CSS_LENGTH_UNIT_AUTO)? 0 : TOP_KNOWN;

		if (verticalCase != (HEIGHT_KNOWN|TOP_KNOWN))
		{
			GetCSSLength(&bottom, CSS_PROPERTY_BOTTOM);
			verticalCase |= (bottom.type == CSS_LENGTH_UNIT_AUTO || parentHeightIsAuto)? 0 : BOTTOM_KNOWN;
		}

		DISPLAY_INT topValue = 0;
		DISPLAY_INT bottomValue = 0;
		DISPLAY_INT marginTopValue = 0;
		DISPLAY_INT marginBottomValue = 0;

		switch (verticalCase)
		{
			case NOTHING_KNOWN:
				// top = auto indent top
				// height = content height
				topValue = autoIndentTop;
				heightValue = 0;
				heightIsAuto = WEBC_TRUE;
				marginTopValue = GetPixelLength(CSS_PROPERTY_MARGIN_TOP, parentWidth, emHeight, exHeight, 0);
				break;

			case HEIGHT_KNOWN:
				// top = auto indent top
				// height = height
				topValue = autoIndentTop;
				heightValue = CSS_LengthToPixels(&height, parentHeight, emHeight, exHeight);
				heightIsAuto = WEBC_FALSE;
				marginTopValue = GetPixelLength(CSS_PROPERTY_MARGIN_TOP, parentWidth, emHeight, exHeight, 0);
				break;

			case TOP_KNOWN:
				// top = top
				// height = content height
				if (top.type == CSS_LENGTH_UNIT_PERCENT && parentHeightIsAuto)
				{
					topValue = autoIndentTop;
				}
				else
				{
					topValue = CSS_LengthToPixels(&top, parentHeight, emHeight, exHeight);
				}
				heightValue = 0;
				heightIsAuto = WEBC_TRUE;
				marginTopValue = GetPixelLength(CSS_PROPERTY_MARGIN_TOP, parentWidth, emHeight, exHeight, 0);
				break;

			case HEIGHT_KNOWN|TOP_KNOWN:
				// top = top
				// height = height
				if (top.type == CSS_LENGTH_UNIT_PERCENT && parentHeightIsAuto)
				{
					topValue = autoIndentTop;
				}
				else
				{
					topValue = CSS_LengthToPixels(&top, parentHeight, emHeight, exHeight);
				}
				heightValue = CSS_LengthToPixels(&height, parentHeight, emHeight, exHeight);
				heightIsAuto = WEBC_FALSE;
				marginTopValue = GetPixelLength(CSS_PROPERTY_MARGIN_TOP, parentWidth, emHeight, exHeight, 0);
				break;

			case BOTTOM_KNOWN:
				// bottom = bottom
				// height = content height
				// top = parentHeight - (height + bottom)
				bottomValue = GetPixelLength(&bottom, parentHeight, emHeight, exHeight, 0);
				marginBottomValue = GetPixelLength(CSS_PROPERTY_MARGIN_BOTTOM, parentWidth, emHeight, exHeight, 0);
				heightValue = 0;
				heightIsAuto = WEBC_TRUE;
				break;

			case HEIGHT_KNOWN|BOTTOM_KNOWN:
				// bottom = bottom
				// height = height
				// top = parentHeight - (height + bottom)
				bottomValue = GetPixelLength(&bottom, parentHeight, emHeight, exHeight, 0);
				heightValue = CSS_LengthToPixels(&height, parentHeight, emHeight, exHeight);
				heightIsAuto = WEBC_FALSE;

				marginBottomValue = GetPixelLength(CSS_PROPERTY_MARGIN_BOTTOM, parentWidth, emHeight, exHeight, 0);

				topValue = parentHeight -
			               (borderTopValue +
			                paddingTopValue +
			                heightValue +
			                paddingBottomValue +
			                borderBottomValue +
			                marginBottomValue +
			                bottomValue);
				break;

			case TOP_KNOWN|BOTTOM_KNOWN:
				// top = top
				// bottom = bottom
				// height = parentHeight - (top + bottom)
				if (top.type == CSS_LENGTH_UNIT_PERCENT && parentHeightIsAuto)
				{
					topValue = autoIndentTop;
				}
				else
				{
					topValue = CSS_LengthToPixels(&top, parentHeight, emHeight, exHeight);
				}
				bottomValue = GetPixelLength(&bottom, parentHeight, emHeight, exHeight, 0);

				marginTopValue = GetPixelLength(CSS_PROPERTY_MARGIN_TOP, parentWidth, emHeight, exHeight, 0);
				marginBottomValue = GetPixelLength(CSS_PROPERTY_MARGIN_BOTTOM, parentWidth, emHeight, exHeight, 0);

				heightValue = parentHeight -
				              (topValue +
							   marginTopValue +
							   borderTopValue +
							   paddingTopValue +
				               paddingBottomValue +
							   borderBottomValue +
							   marginBottomValue +
							   bottomValue);
				heightIsAuto = WEBC_FALSE;
				break;
		}

		if (width.type != CSS_LENGTH_UNIT_AUTO)
		{
			widthValue = CSS_LengthToPixels(&width, parentWidth, emHeight, exHeight);

			WEBC_BOOL leftAligned = (textDirection == TU_DIR_RTL)?
			                            (right.type == CSS_LENGTH_UNIT_AUTO && left.type  != CSS_LENGTH_UNIT_AUTO) :
										(left.type  != CSS_LENGTH_UNIT_AUTO || right.type == CSS_LENGTH_UNIT_AUTO);

			if (leftAligned)
			{
				// ignore right and margin-right
				leftValue = GetPixelLength(&left, parentWidth, emHeight, exHeight, autoIndentLeft);
				marginLeftValue = GetPixelLength(CSS_PROPERTY_MARGIN_LEFT, parentWidth, emHeight, exHeight, 0);
			}
			else
			{
				DISPLAY_INT marginRightValue;

				// ignore left and margin-left
				rightValue = GetPixelLength(&right, parentWidth, emHeight, exHeight, autoIndentRight);
				marginRightValue = GetPixelLength(CSS_PROPERTY_MARGIN_RIGHT, parentWidth, emHeight, exHeight, 0);
				leftValue = parentWidth -
				            (rightValue +
							 marginRightValue +
							 borderRightValue +
							 paddingRightValue +
				             widthValue +
							 paddingLeftValue +
							 borderLeftValue);

				marginLeftValue = 0;
			}
		}
		else
		{
			// width.type == CSS_LENGTH_UNIT_AUTO

			// try to get the intrinsic width of the element.  This might rely on the
			//  calculated height of the element; for example, in the case of an image with
			//  specified height and auto width, the aspect ratio of the source image determines
			//  the resulting intrinsic width.

			DISPLAY_INT marginRightValue;
			WEBC_BOOL hasIntrinsicWidth = GetIntrinsicWidth(&widthValue, heightValue, heightIsAuto);

			if (left.type == CSS_LENGTH_UNIT_AUTO && right.type != CSS_LENGTH_UNIT_AUTO && hasIntrinsicWidth)
			{
				rightValue = GetPixelLength(&right, parentWidth, emHeight, exHeight, autoIndentRight);
				marginRightValue = GetPixelLength(CSS_PROPERTY_MARGIN_RIGHT, parentWidth, emHeight, exHeight, 0);
				marginLeftValue = 0;

				leftValue = parentWidth -
							 (widthValue +
							  borderLeftValue +
							  paddingLeftValue +
							  paddingRightValue +
							  borderRightValue +
							  marginRightValue +
							  rightValue);
			}
			else
			{
				leftValue = GetPixelLength(&left,  parentWidth, emHeight, exHeight, autoIndentLeft);
				marginLeftValue = GetPixelLength(CSS_PROPERTY_MARGIN_LEFT,  parentWidth, emHeight, exHeight, 0);

				if (!hasIntrinsicWidth || (right.type != CSS_LENGTH_UNIT_AUTO && left.type != CSS_LENGTH_UNIT_AUTO))
				{
					// right and left are both non-auto, OR element has no intrinsic width:
					//  calculate left, right, and margins then extrapolate width
					rightValue = GetPixelLength(&right, parentWidth, emHeight, exHeight, autoIndentRight);
					marginRightValue = GetPixelLength(CSS_PROPERTY_MARGIN_RIGHT, parentWidth, emHeight, exHeight, 0);

					widthValue = parentWidth -
								 (leftValue +
								  marginLeftValue +
								  borderLeftValue +
								  paddingLeftValue +
								  paddingRightValue +
								  borderRightValue +
								  marginRightValue +
								  rightValue);

					// we don't want to stretch the width beyond what the maximum content
					//  width; only give the element as much space as it actually needs.
					if (right.type == CSS_LENGTH_UNIT_AUTO || left.type == CSS_LENGTH_UNIT_AUTO)
					{
						// to speed this up, bail out of GetMaxWidth when we exceed widthValue
						DISPLAY_INT maxWidth = GetMaxWidthSmallerThan(widthValue);

						// NOTE: GetMaxWidth and GetMaxWidthSmallerThan return the total width
						//  of the element, including borders and padding.  So we must subtract
						//  borders and padding to make it correspond to the contentWidth

						maxWidth -= (borderLeftValue + paddingLeftValue + paddingRightValue + borderRightValue);

						if (maxWidth < widthValue)
						{
							WEBC_BOOL leftAligned = (textDirection == TU_DIR_RTL)?
														(right.type == CSS_LENGTH_UNIT_AUTO && left.type  != CSS_LENGTH_UNIT_AUTO) :
														(left.type  != CSS_LENGTH_UNIT_AUTO || right.type == CSS_LENGTH_UNIT_AUTO);

							if (!leftAligned)
							{
								// box is right-aligned
								//  adjust leftValue to account for the unused space
								leftValue += (widthValue - maxWidth);
							}
							widthValue = maxWidth;
						}
					}
				}
			}
		}

		// Now format the content of this element

		DISPLAY_INT contentHeight = FormatContentPositioned (
				widthValue,
				heightValue,
				heightIsAuto,
				borderLeftValue,
				borderTopValue,
				borderRightValue,
				borderBottomValue,
				paddingLeftValue,
				paddingTopValue,
				paddingRightValue,
				paddingBottomValue
			);

		// Find the top edge and height

		//  NOTE: top and bottom percentages are relative to parent's HEIGHT, whereas
		//   margin-top and margin-bottom are relative to parent's WIDTH, except in the root
		//   context (tbd - handle this special case correctly)

		switch (verticalCase)
		{
			case NOTHING_KNOWN:
			case TOP_KNOWN:
				heightValue = contentHeight;
				break;

			case BOTTOM_KNOWN:
				// height = content height
				// top = parentHeight - (height + bottom)
				heightValue = contentHeight;
				topValue = parentHeight -
			               (borderTopValue +
			                paddingTopValue +
			                heightValue +
			                paddingBottomValue +
			                borderBottomValue +
			                marginBottomValue +
			                bottomValue);
				break;
		}

		rect.Set (0, 0,
				borderLeftValue + paddingLeftValue + widthValue  + paddingRightValue  + borderRightValue - 1,
				borderTopValue  + paddingTopValue  + heightValue + paddingBottomValue + borderBottomValue - 1
			);

		rect.MoveTo (
				parentBorderLeft + leftValue + marginLeftValue,
				parentBorderTop + topValue  + marginTopValue
			);

		if (position == TU_POSITION_FIXED)
		{
			rect.Shift(containingBlock.left, containingBlock.top);
		}

		Move(&rect);

		if (generateChildContext)
		{
			childContext = childContextFactory->newPositioned (
					&rect,
					heightIsAuto,
					TU_DIR_LTR,
					borderLeftValue,
					borderTopValue,
					borderRightValue,
					borderBottomValue,
					paddingLeftValue,
					paddingTopValue,
					paddingRightValue
				);
		}
	}

	return (childContext);
}

/*---------------------------------------------------------------------------*/
template<class T> IPositionedFormatContext* HTMLElementDisplay<T>::FormatPositioned (
		DisplayElement* displayParent,
		IPositionedFormatContext* rootContext,
		IPositionedFormatContext* parentContext,
		FormatContextFactory* childContextFactory
	)
{
	WEBC_BOOL generateChildContext = WEBC_FALSE;
	IPositionedFormatContext* childContext = 0;

	{
		PositionedTUEnumerator e(mpHtmlElement, this, 0);
		if (e.GetNextUnit())
		{
			generateChildContext = WEBC_TRUE;
		}
	}

	childContext = FormatSelfPositioned (
			displayParent,
			rootContext,
			parentContext,
			childContextFactory,
			generateChildContext
		);

	if (childContext)
	{
		FormatPositionedChildren(childContext, rootContext, childContextFactory);
	}

	return (childContext);
}

/*---------------------------------------------------------------------------*/
template<class T> void HTMLElementDisplay<T>::FormatPositionedChildren (
		IPositionedFormatContext* childContext,
		IPositionedFormatContext* rootContext,
		FormatContextFactory* childContextFactory
	)
{
	IPositionedFormatContext* customContext = 0;

	PositionedTUEnumerator e(mpHtmlElement, this, childContext->getAutoIndentTop());
	TextUnit* textUnit = e.GetNextUnit();

	// Format positioned children
	while (textUnit)
	{
		if (textUnit->NeedsAutoIndentTop(childContext->parentHeightIsAuto()))
		{
			WEBC_NEW_VERBOSE(customContext,
			         AutoTopPositionedFormatContext (
			                 childContext,
			                 e.getAutoIndentTop()
			             ),"AutoTopPositionedFormatContext");
		}

		if (!customContext)
		{
			customContext = childContext;
		}

		IPositionedFormatContext* ctx = textUnit->FormatPositioned (
				this,
				rootContext,
				customContext,
				childContextFactory
			);

		if (ctx)
		{
			ctx->release();
		}

		if (customContext != childContext)
		{
			customContext->release();
			customContext = 0;
		}

		textUnit = e.GetNextUnit();
	}
}
