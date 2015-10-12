//
// HCBOX.CPP - Methods for class HTMLContentBox
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

#include "hcbox.hpp"
#include "helement.hpp"
#include "css.h"
#include "dbgtimer.h"


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

HTMLContentBox::HTMLContentBox (HTMLElement *pElem, DISPLAY_INT parentWidth)
{
	mFlags = 0;
	mBorderStyle = 0;
	miEmHeight = 0;
	miExHeight = 0;

	SetElement(pElem);
	SetParentWidth(parentWidth);
}

HTMLContentBox::~HTMLContentBox ()
{

}

void HTMLContentBox::SetElement (HTMLElement *pElement)
{
	mpElement = pElement;
}

void HTMLContentBox::SetParentWidth (DISPLAY_INT w)
{
	miParentWidth = w;
}

DISPLAY_INT HTMLContentBox::GetParentWidth (void)
{
	return miParentWidth;
}

HTMLColor HTMLContentBox::GetBorderColor (CSSSide side)
{
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLCONTENTBOX_GETBORDERCOLOR);

	static WEBC_UINT32 mask[4] = {
			HCBOX_FLAG_TOP_COLOR_SET,
			HCBOX_FLAG_RIGHT_COLOR_SET,
			HCBOX_FLAG_BOTTOM_COLOR_SET,
			HCBOX_FLAG_LEFT_COLOR_SET
		};

	if (!(mFlags & mask[side]))
	{
		mBorderColor[side] = mpElement->GetBorderColor(side);
		mFlags |= mask[side];
	}

	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLCONTENTBOX_GETBORDERCOLOR);

	return (mBorderColor[side]);
}


static int _css_border_style_to_hcbox [] =
{
	HCBOX_STYLE_SOLID,
	HCBOX_STYLE_SOLID,
	HCBOX_STYLE_SOLID,
	HCBOX_STYLE_GROOVE,
	HCBOX_STYLE_INSET,
	HCBOX_STYLE_NONE,
	HCBOX_STYLE_OUTSET,
	HCBOX_STYLE_RIDGE,
	HCBOX_STYLE_SOLID,
};

WEBC_UINT16 HTMLContentBox::GetBorderStyle (CSSSide side)
{
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLCONTENTBOX_GETBORDERSTYLE);

	WEBC_UINT16 style = 0;

	switch (side)
	{
	case CSS_TOP:
		if (!(mBorderStyle & HCBOX_FLAG_TOP_STYLE_SET))
		{
			mBorderStyle &= ~HCBOX_STYLE_TOP_MASK;
			mBorderStyle |= _css_border_style_to_hcbox[mpElement->GetBorderStyle(side)] << HCBOX_STYLE_TOP_SHIFT;
			mBorderStyle |= HCBOX_FLAG_TOP_STYLE_SET;
		}
		style = (WEBC_UINT16) ((mBorderStyle & HCBOX_STYLE_TOP_MASK) >> HCBOX_STYLE_TOP_SHIFT);
		break;

	case CSS_RIGHT:
		if (!(mBorderStyle & HCBOX_FLAG_RIGHT_STYLE_SET))
		{
			mBorderStyle &= ~HCBOX_STYLE_RIGHT_MASK;
			mBorderStyle |= _css_border_style_to_hcbox[mpElement->GetBorderStyle(side)] << HCBOX_STYLE_RIGHT_SHIFT;
			mBorderStyle |= HCBOX_FLAG_RIGHT_STYLE_SET;
		}
		style = (WEBC_UINT16) ((mBorderStyle & HCBOX_STYLE_RIGHT_MASK) >> HCBOX_STYLE_RIGHT_SHIFT);
		break;

	case CSS_BOTTOM:
		if (!(mBorderStyle & HCBOX_FLAG_BOTTOM_STYLE_SET))
		{
			mBorderStyle &= ~HCBOX_STYLE_BOTTOM_MASK;
			mBorderStyle |= _css_border_style_to_hcbox[mpElement->GetBorderStyle(side)] << HCBOX_STYLE_BOTTOM_SHIFT;
			mBorderStyle |= HCBOX_FLAG_BOTTOM_STYLE_SET;
		}
		style = (WEBC_UINT16) ((mBorderStyle & HCBOX_STYLE_BOTTOM_MASK) >> HCBOX_STYLE_BOTTOM_SHIFT);
		break;

	case CSS_LEFT:
		if (!(mBorderStyle & HCBOX_FLAG_LEFT_STYLE_SET))
		{
			mBorderStyle &= ~HCBOX_STYLE_LEFT_MASK;
			mBorderStyle |= _css_border_style_to_hcbox[mpElement->GetBorderStyle(side)] << HCBOX_STYLE_LEFT_SHIFT;
			mBorderStyle |= HCBOX_FLAG_LEFT_STYLE_SET;
		}
		style = (WEBC_UINT16) ((mBorderStyle & HCBOX_STYLE_LEFT_MASK) >> HCBOX_STYLE_LEFT_SHIFT);
		break;
	}

	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLCONTENTBOX_GETBORDERSTYLE);

	return (style);
}

DISPLAY_INT HTMLContentBox::GetMarginWidth (CSSSide side)
{
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLCONTENTBOX_GETMARGINWIDTH);

	CSSLength len;
	mpElement->GetMarginWidth(side, &len);
	DISPLAY_INT width = ConvertCSSLength(&len);

	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLCONTENTBOX_GETMARGINWIDTH);

	return (width);
}

void HTMLContentBox::GetHorizontalMargins (
		DISPLAY_INT* marginLeftValue,
		DISPLAY_INT* marginRightValue,
		DISPLAY_INT totalWidthValue)
{
	CSSLength marginLeft;
	CSSLength marginRight;

	mpElement->GetMarginWidth(CSS_LEFT, &marginLeft);
	mpElement->GetMarginWidth(CSS_RIGHT, &marginRight);

	if (marginLeft.type == CSS_LENGTH_UNIT_AUTO)
	{
		if (marginRight.type == CSS_LENGTH_UNIT_AUTO)
		{
			*marginLeftValue = (miParentWidth - totalWidthValue) >> 1;
			if (*marginLeftValue < 0)
			{
				*marginLeftValue = 0;
			}
			*marginRightValue = miParentWidth - totalWidthValue - *marginLeftValue;
		}
		else
		{
			*marginRightValue = ConvertCSSLength(&marginRight);
			*marginLeftValue = miParentWidth - totalWidthValue - *marginRightValue;
		}
	}
	else
	{
		if (marginRight.type == CSS_LENGTH_UNIT_AUTO)
		{
			*marginLeftValue = ConvertCSSLength(&marginLeft);
			*marginRightValue = miParentWidth - totalWidthValue - *marginLeftValue;
		}
		else
		{
			*marginLeftValue = ConvertCSSLength(&marginLeft);
			*marginRightValue = ConvertCSSLength(&marginRight);
		}
	}
}

DISPLAY_INT HTMLContentBox::GetPaddingWidth (CSSSide side)
{
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLCONTENTBOX_GETPADDINGWIDTH);

	CSSLength len;
	mpElement->GetPaddingWidth(side, &len);
	DISPLAY_INT width = ConvertCSSLength(&len);

	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLCONTENTBOX_GETPADDINGWIDTH);

	return (width);
}

DISPLAY_INT HTMLContentBox::GetBorderWidth (CSSSide side)
{
	if (!(mFlags & (1 << (side+4))))
	{
		WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLCONTENTBOX_GETBORDERWIDTH);

		DISPLAY_INT val = 0;
		CSSBorderThickness width;

		if (GetBorderStyle(side) != HCBOX_STYLE_NONE)
		{
			mpElement->GetBorderWidth(side, &width);
			switch (width.type)
			{
				case CSS_BORDER_THICKNESS_LENGTH:
					val = ConvertCSSLength(&width.val.length);
					break;

				case CSS_BORDER_THICKNESS_ABSOLUTE:
					switch (width.val.absolute)
					{
						case CSS_BORDER_THICKNESS_THIN:
							val = 1;
							break;

						case CSS_BORDER_THICKNESS_MEDIUM:
							val = 3;
							break;

						case CSS_BORDER_THICKNESS_THICK:
							val = 6;
							break;

						default:
							break;
					}
					break;

				default:
					break;
			}
		}

		mBorderWidth[side] = (WEBC_UINT8) EBSCLIP(val, 0, 255);
		mFlags |= (1 << (side+4));

		WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLCONTENTBOX_GETBORDERWIDTH);
	}

	return (mBorderWidth[side]);
}

// #define USEOLDDRAWBORDER
#ifdef USEOLDDRAWBORDER
void HTMLContentBox::DrawBorder (WebRect *rect, WebGraphics *gc)
{
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLCONTENTBOX_DRAWBORDER);

	DISPLAY_INT left, right, leftStep, rightStep, leftErr, rightErr, leftMod, rightMod;
	DISPLAY_INT x,y,w,h;
	WebColor color = 0, lightColor = 0, darkColor = 0;
	HTMLColor rgb;
	WEBC_UINT16 style;
	DISPLAY_INT border[4];
	WebRect box;

	border[CSS_TOP] = GetBorderWidth(CSS_TOP);
	border[CSS_RIGHT] = GetBorderWidth(CSS_RIGHT);
	border[CSS_LEFT] = GetBorderWidth(CSS_LEFT);
	border[CSS_BOTTOM] = GetBorderWidth(CSS_BOTTOM);

	x = rect->left;
	y = rect->top;
	w = rect->Width();
	h = rect->Height();

	if (border[CSS_LEFT] > 0)
	{
		style = GetBorderStyle(CSS_LEFT);
		rgb = GetBorderColor(CSS_LEFT);

		switch (style)
		{
		case HCBOX_STYLE_RIDGE:
			lightColor = gc->RGBToLightColor(rgb);
			darkColor = gc->RGBToDarkColor(rgb);
			box.Set(x, y, x + border[CSS_LEFT]/2 - 1, y + h - 1);
			gc->Rectangle(&box, lightColor, lightColor, WEBC_TRUE);
			box.Set(x + border[CSS_LEFT]/2, y, x + border[CSS_LEFT] - 1, y + h - 1);
			gc->Rectangle(&box, darkColor, darkColor, WEBC_TRUE);
			break;

		case HCBOX_STYLE_GROOVE:
			darkColor = gc->RGBToLightColor(rgb);
			lightColor = gc->RGBToDarkColor(rgb);
			box.Set(x, y, x + border[CSS_LEFT]/2 - 1, y + h - 1);
			gc->Rectangle(&box, lightColor, lightColor, WEBC_TRUE);
			box.Set(x + border[CSS_LEFT]/2, y, x + border[CSS_LEFT] - 1, y + h - 1);
			gc->Rectangle(&box, darkColor, darkColor, WEBC_TRUE);
			break;

		case HCBOX_STYLE_INSET:
			color = gc->RGBToDarkColor(rgb);
			box.Set(x, y, x + border[CSS_LEFT] - 1, y + h - 1);
			gc->Rectangle(&box, color, color, WEBC_TRUE);
			break;

		case HCBOX_STYLE_OUTSET:
			color = gc->RGBToLightColor(rgb);
			box.Set(x, y, x + border[CSS_LEFT] - 1, y + h - 1);
			gc->Rectangle(&box, color, color, WEBC_TRUE);
			break;

		default:
			color = gc->RGBToColor(rgb);
			box.Set(x, y, x + border[CSS_LEFT] - 1, y + h - 1);
			gc->Rectangle(&box, color, color, WEBC_TRUE);
			break;
		}
	}

	if (border[CSS_RIGHT] > 0)
	{
		style = GetBorderStyle(CSS_RIGHT);
		rgb = GetBorderColor(CSS_RIGHT);

		switch (style)
		{
		case HCBOX_STYLE_RIDGE:
			lightColor = gc->RGBToLightColor(rgb);
			darkColor = gc->RGBToDarkColor(rgb);
			box.Set(x + w - border[CSS_RIGHT], y, x + w - border[CSS_RIGHT]/2 - 1, y + h - 1);
			gc->Rectangle(&box, lightColor, lightColor, WEBC_TRUE);
			box.Set(x + w - border[CSS_RIGHT]/2, y, x + w - 1, y + h - 1);
			gc->Rectangle(&box, darkColor, darkColor, WEBC_TRUE);
			break;

		case HCBOX_STYLE_GROOVE:
			darkColor = gc->RGBToLightColor(rgb);
			lightColor = gc->RGBToDarkColor(rgb);
			box.Set(x + w - border[CSS_RIGHT], y, x + w - border[CSS_RIGHT]/2 - 1, y + h - 1);
			gc->Rectangle(&box, lightColor, lightColor, WEBC_TRUE);
			box.Set(x + w - border[CSS_RIGHT]/2, y, x + w - 1, y + h - 1);
			gc->Rectangle(&box, darkColor, darkColor, WEBC_TRUE);
			break;

		case HCBOX_STYLE_INSET:
			color = gc->RGBToLightColor(rgb);
			box.Set(x + w - border[CSS_RIGHT], y, x + w - 1, y + h - 1);
			gc->Rectangle(&box, color, color, WEBC_TRUE);
			break;

		case HCBOX_STYLE_OUTSET:
			color = gc->RGBToDarkColor(rgb);
			box.Set(x + w - border[CSS_RIGHT], y, x + w - 1, y + h - 1);
			gc->Rectangle(&box, color, color, WEBC_TRUE);
			break;

		default:
			color = gc->RGBToColor(rgb);
			box.Set(x + w - border[CSS_RIGHT], y, x + w - 1, y + h - 1);
			gc->Rectangle(&box, color, color, WEBC_TRUE);
			break;
		}

	}

	if (border[CSS_TOP] > 0)
	{
		style = GetBorderStyle(CSS_TOP);
		rgb = GetBorderColor(CSS_TOP);

		switch (style)
		{
		case HCBOX_STYLE_RIDGE:
			lightColor = gc->RGBToLightColor(rgb);
			darkColor = gc->RGBToDarkColor(rgb);
			break;

		case HCBOX_STYLE_GROOVE:
			darkColor = gc->RGBToLightColor(rgb);
			lightColor = gc->RGBToDarkColor(rgb);
			break;

		case HCBOX_STYLE_INSET:
			color = gc->RGBToDarkColor(rgb);
			break;

		case HCBOX_STYLE_OUTSET:
			color = gc->RGBToLightColor(rgb);
			break;

		default:
			color = gc->RGBToColor(rgb);
			break;
		}

		leftStep = border[CSS_LEFT] / border[CSS_TOP];
		rightStep = border[CSS_RIGHT] / border[CSS_TOP];
		leftMod = border[CSS_LEFT] % border[CSS_TOP];
		rightMod = border[CSS_RIGHT] % border[CSS_TOP];
		left = x + leftStep / 2;
		right = x + w - 1 - rightStep / 2;
		leftErr = leftMod / 2;
		rightErr = rightMod / 2;

		for (DISPLAY_INT yy = 0; yy < border[CSS_TOP]; yy++)
		{
			switch (style)
			{
			case HCBOX_STYLE_GROOVE:
			case HCBOX_STYLE_RIDGE:
				if (yy < border[CSS_TOP] / 2)
				{
					gc->HLine(y + yy, left, right, lightColor);
				}
				else
				{
					gc->HLine(y + yy, left, right, darkColor);
				}
				break;

			default:
				gc->HLine(y + yy, left, right, color);
				break;
			}

			left += leftStep;
			leftErr += leftMod;
			if (leftErr >= border[CSS_TOP])
			{
				left++;
				leftErr -= border[CSS_TOP];
			}

			right -= rightStep;
			rightErr += rightMod;
			if (rightErr >= border[CSS_TOP])
			{
				right--;
				rightErr -= border[CSS_TOP];
			}
		}
	}

	if (border[CSS_BOTTOM] > 0)
	{
		style = GetBorderStyle(CSS_BOTTOM);
		rgb = GetBorderColor(CSS_BOTTOM);

		switch (style)
		{
		case HCBOX_STYLE_RIDGE:
			lightColor = gc->RGBToLightColor(rgb);
			darkColor = gc->RGBToDarkColor(rgb);
			break;

		case HCBOX_STYLE_GROOVE:
			darkColor = gc->RGBToLightColor(rgb);
			lightColor = gc->RGBToDarkColor(rgb);
			break;

		case HCBOX_STYLE_INSET:
			color = gc->RGBToLightColor(rgb);
			break;

		case HCBOX_STYLE_OUTSET:
			color = gc->RGBToDarkColor(rgb);
			break;

		default:
			color = gc->RGBToColor(rgb);
			break;
		}

		leftStep = border[CSS_LEFT] / border[CSS_BOTTOM];
		rightStep = border[CSS_RIGHT] / border[CSS_BOTTOM];
		leftMod = border[CSS_LEFT] % border[CSS_BOTTOM];
		rightMod = border[CSS_RIGHT] % border[CSS_BOTTOM];
		left = x + leftStep / 2;
		right = x + w - 1 - rightStep / 2;
		leftErr = leftMod / 2;
		rightErr = rightMod / 2;

		for (DISPLAY_INT yy = 0; yy < border[CSS_BOTTOM]; yy++)
		{
			switch (style)
			{
			case HCBOX_STYLE_GROOVE:
			case HCBOX_STYLE_RIDGE:
				if (yy >= border[CSS_BOTTOM] / 2)
				{
					gc->HLine(y + h - 1 - yy, left, right, lightColor);
				}
				else
				{
					gc->HLine(y + h - 1 - yy, left, right, darkColor);
				}
				break;

			default:
				gc->HLine(y + h - 1 - yy, left, right, color);
			}

			left += leftStep;
			leftErr += leftMod;
			if (leftErr >= border[CSS_BOTTOM])
			{
				left++;
				leftErr -= border[CSS_BOTTOM];
			}

			right -= rightStep;
			rightErr += rightMod;
			if (rightErr >= border[CSS_BOTTOM])
			{
				right--;
				rightErr -= border[CSS_BOTTOM];
			}
		}
	}

	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLCONTENTBOX_DRAWBORDER);
}

#else // ifdef USEOLDDRAWBORDER

void _DrawBorder (WebRect *rect, HTMLGraphicsContext** ctx,WEBC_UINT16 style[4],DISPLAY_INT border[4],WebColor color[4], WebColor lightColor[4], WebColor darkColor[4], DISPLAY_INT Radius[4]);

void HTMLContentBox::DrawBorder (WebRect *rect, WebGraphics *gc)
{
	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLCONTENTBOX_DRAWBORDER);

	DISPLAY_INT x,y,w,h;
	HTMLColor rgb;
	WEBC_UINT16 style[4];
	DISPLAY_INT border[4];
	DISPLAY_INT Radius[4];
	WebColor color[4], lightColor[4], darkColor[4];

	border[CSS_TOP] = GetBorderWidth(CSS_TOP);
	border[CSS_RIGHT] = GetBorderWidth(CSS_RIGHT);
	border[CSS_LEFT] = GetBorderWidth(CSS_LEFT);
	border[CSS_BOTTOM] = GetBorderWidth(CSS_BOTTOM);

	x = rect->left;
	y = rect->top;
	w = rect->Width();
	h = rect->Height();


	border[CSS_TOP] = GetBorderWidth(CSS_TOP);
	border[CSS_RIGHT] = GetBorderWidth(CSS_RIGHT);
	border[CSS_LEFT] = GetBorderWidth(CSS_LEFT);
	border[CSS_BOTTOM] = GetBorderWidth(CSS_BOTTOM);

	x = rect->left;
	y = rect->top;
	w = rect->Width();
	h = rect->Height();

	for (int i = (int) CSS_TOP; i <= (int) CSS_LEFT; i++)
	{
		color[i]=lightColor[i]=darkColor[i]=0;
		style[i] = GetBorderStyle((CSSSide) i);
		Radius[i] = 0;
		if (border[i] > 0)
		{
			rgb = GetBorderColor((CSSSide)i);
			switch (style[i])
			{
			case HCBOX_STYLE_RIDGE:
				lightColor[i] = gc->RGBToLightColor(rgb);
				darkColor[i] = gc->RGBToDarkColor(rgb);
				break;
			case HCBOX_STYLE_GROOVE:
				darkColor[i] = gc->RGBToLightColor(rgb);
				lightColor[i] = gc->RGBToDarkColor(rgb);
			break;
			case HCBOX_STYLE_INSET:
				color[i] = gc->RGBToDarkColor(rgb);
				break;
			case HCBOX_STYLE_OUTSET:
				color[i] = gc->RGBToLightColor(rgb);
				break;
			default:
				color[i]= gc->RGBToColor(rgb);
				break;
			}
		}
	}

	_DrawBorder (rect, gc->mGraphics,style, border, color, lightColor, darkColor, Radius);
	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_HTMLCONTENTBOX_DRAWBORDER);
}

#include "webc.h"

#define RECT_SET(RECT, X,Y,R,B) RECT.left=(X);RECT.top=(Y);RECT.right=(R);RECT.bottom=(B);


static void _DrawCtxBorderLine (HTMLGraphicsContext** ctx, DISPLAY_INT y, DISPLAY_INT left, DISPLAY_INT right, WebColor color, DISPLAY_INT thickness)
{
HTMLRect hrect;
	RECT_SET(hrect, left, y, right, y+thickness-1)
	(*ctx)->drawRectangle(ctx,&hrect, color, color, WEBC_TRUE);
}

#if (WEBC_SUPPORT_RADIUS)
void GuiDrawCorners(HTMLGraphicsContext** ctx, int top, int left, int height, int width, int thickness, int radius, WebColor color);
#endif
void _DrawBorder (WebRect *rect, HTMLGraphicsContext** ctx,WEBC_UINT16 style[4],DISPLAY_INT border[4],WebColor color[4], WebColor lightColor[4], WebColor darkColor[4], DISPLAY_INT Radius[4])
{
	HTMLRect hrect;
	DISPLAY_INT thickness = 1;

	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_HTMLCONTENTBOX_DRAWBORDER);

	DISPLAY_INT left, right, leftStep, rightStep, leftErr, rightErr, leftMod, rightMod;
	DISPLAY_INT x,y,w,h;
	DISPLAY_INT vtop,vbottom;


	x = rect->left;
	y = rect->top;
	w = rect->Width();
	h = rect->Height();
#if (!WEBC_SUPPORT_RADIUS)
Radius[0]=Radius[1]=Radius[2]=Radius[3]=0;
#endif
	vtop    = y + Radius[CSS_TOP]/2;
	vbottom = y + h - 1 - Radius[CSS_BOTTOM]/2;

	if (w < 1) w=1;
	if (h < 1) h=1;

	if (border[CSS_LEFT] > 0)
	{
		switch (style[CSS_LEFT])
		{
		case HCBOX_STYLE_RIDGE:
			RECT_SET(hrect, x, vtop, x + border[CSS_LEFT]/2 - 1, vbottom);
			(*ctx)->drawRectangle(ctx,&hrect,lightColor[CSS_LEFT], lightColor[CSS_LEFT], WEBC_TRUE);
			RECT_SET(hrect,x + border[CSS_LEFT]/2, vtop, x + border[CSS_LEFT] - 1, vbottom);
			(*ctx)->drawRectangle(ctx,&hrect, darkColor[CSS_LEFT], darkColor[CSS_LEFT], WEBC_TRUE);
			break;

		case HCBOX_STYLE_GROOVE:
			RECT_SET(hrect, x, vtop, x + border[CSS_LEFT]/2 - 1, vbottom);
			(*ctx)->drawRectangle(ctx,&hrect, lightColor[CSS_LEFT], lightColor[CSS_LEFT], WEBC_TRUE);
			RECT_SET(hrect, x + border[CSS_LEFT]/2, y, x + border[CSS_LEFT] - 1, vbottom);
			(*ctx)->drawRectangle(ctx,&hrect, darkColor[CSS_LEFT], darkColor[CSS_LEFT], WEBC_TRUE);

			break;

		case HCBOX_STYLE_INSET:
			RECT_SET(hrect, x, vtop, x + border[CSS_LEFT] - 1, vbottom);
			(*ctx)->drawRectangle(ctx,&hrect, color[CSS_LEFT], color[CSS_LEFT], WEBC_TRUE);
			break;

		case HCBOX_STYLE_OUTSET:
			RECT_SET(hrect, x, vtop, x + border[CSS_LEFT] - 1, vbottom);
			(*ctx)->drawRectangle(ctx,&hrect, color[CSS_LEFT], color[CSS_LEFT], WEBC_TRUE);
			break;

		default:
			RECT_SET(hrect, x, vtop, x + border[CSS_LEFT] - 1, vbottom);
			(*ctx)->drawRectangle(ctx,&hrect, color[CSS_LEFT], color[CSS_LEFT], WEBC_TRUE);
			break;
		}
	}

	if (border[CSS_RIGHT] > 0)
	{
		switch (style[CSS_RIGHT])
		{
		case HCBOX_STYLE_RIDGE:
			RECT_SET(hrect, x + w - border[CSS_RIGHT]-1, vtop, x + w - border[CSS_RIGHT]/2 - 1, vbottom);
			(*ctx)->drawRectangle(ctx,&hrect, lightColor[CSS_RIGHT], lightColor[CSS_RIGHT], WEBC_TRUE);
			RECT_SET(hrect, x + w - border[CSS_RIGHT]/2, vtop, x + w - 1, vbottom);
			(*ctx)->drawRectangle(ctx,&hrect, darkColor[CSS_RIGHT], darkColor[CSS_RIGHT], WEBC_TRUE);
			break;

		case HCBOX_STYLE_GROOVE:
			RECT_SET(hrect, x + w - border[CSS_RIGHT]-1, vtop, x + w - border[CSS_RIGHT]/2 - 1, vbottom);
			(*ctx)->drawRectangle(ctx,&hrect, lightColor[CSS_RIGHT], lightColor[CSS_RIGHT], WEBC_TRUE);
			RECT_SET(hrect, x + w - border[CSS_RIGHT]/2, vtop, x + w - 1, vbottom);
			(*ctx)->drawRectangle(ctx,&hrect, darkColor[CSS_RIGHT], darkColor[CSS_RIGHT], WEBC_TRUE);
			break;

		case HCBOX_STYLE_INSET:
			RECT_SET(hrect, x + w - border[CSS_RIGHT]-1, vtop, x + w - 1, vbottom);
			(*ctx)->drawRectangle(ctx,&hrect, color[CSS_RIGHT], color[CSS_RIGHT], WEBC_TRUE);
			break;

		case HCBOX_STYLE_OUTSET:
			RECT_SET(hrect, x + w - border[CSS_RIGHT]-1, vtop, x + w - 1, vbottom);
			(*ctx)->drawRectangle(ctx,&hrect, color[CSS_RIGHT], color[CSS_RIGHT], WEBC_TRUE);
			break;

		default:
			RECT_SET(hrect, x + w - border[CSS_RIGHT]-1, vtop, x + w - 1, vbottom);
			(*ctx)->drawRectangle(ctx,&hrect, color[CSS_RIGHT], color[CSS_RIGHT], WEBC_TRUE);
			break;
		}

	}

	if (border[CSS_TOP] > 0)
	{
		leftStep = border[CSS_LEFT] / border[CSS_TOP];
		rightStep = border[CSS_RIGHT] / border[CSS_TOP];
		leftMod = border[CSS_LEFT] % border[CSS_TOP];
		rightMod = border[CSS_RIGHT] % border[CSS_TOP];
		left = x +  + Radius[CSS_LEFT]/2 + leftStep / 2;
		right = x + w - 1 - rightStep / 2 - Radius[CSS_RIGHT]/2;
		leftErr = leftMod / 2;
		rightErr = rightMod / 2;

		for (DISPLAY_INT yy = 0; yy < border[CSS_TOP]; yy++)
		{
			switch (style[CSS_TOP])
			{
			case HCBOX_STYLE_GROOVE:
			case HCBOX_STYLE_RIDGE:
				if (yy < border[CSS_TOP] / 2)
				{
					_DrawCtxBorderLine(ctx, y + yy, left, right, lightColor[CSS_TOP],thickness);
				}
				else
				{
					_DrawCtxBorderLine(ctx,y + yy, left, right, darkColor[CSS_TOP],thickness);
				}
				break;

			default:
				_DrawCtxBorderLine(ctx,y + yy, left, right, color[CSS_TOP],thickness);
				break;
			}
			if (Radius[CSS_LEFT]== 0)
			{
				left += leftStep;
				leftErr += leftMod;
				if (leftErr >= border[CSS_TOP])
				{
					left++;
					leftErr -= border[CSS_TOP];
				}
			}
			if (Radius[CSS_RIGHT]== 0)
			{
				right -= rightStep;
				rightErr += rightMod;
				if (rightErr >= border[CSS_TOP])
				{
					right--;
					rightErr -= border[CSS_TOP];
				}
			}
		}
	}
	if (border[CSS_BOTTOM] > 0)
	{

		leftStep = border[CSS_LEFT] / border[CSS_BOTTOM];
		rightStep = border[CSS_RIGHT] / border[CSS_BOTTOM];
		leftMod = border[CSS_LEFT] % border[CSS_BOTTOM];
		rightMod = border[CSS_RIGHT] % border[CSS_BOTTOM];
		left = x + Radius[CSS_LEFT]/2 + leftStep / 2;
		right = x + w - 1 - rightStep / 2 - Radius[CSS_RIGHT]/2;

		leftErr = leftMod / 2;
		rightErr = rightMod / 2;

		for (DISPLAY_INT yy = 0; yy < border[CSS_BOTTOM]; yy++)
		{
			switch (style[CSS_BOTTOM])
			{
			case HCBOX_STYLE_GROOVE:
			case HCBOX_STYLE_RIDGE:
				if (yy >= border[CSS_BOTTOM] / 2)
				{
					_DrawCtxBorderLine(ctx,y + h - 1 - yy, left, right, lightColor[CSS_BOTTOM],thickness);
				}
				else
				{
					_DrawCtxBorderLine(ctx,y + h - 1 - yy, left, right, darkColor[CSS_BOTTOM],thickness);
				}
				break;

			default:
				_DrawCtxBorderLine(ctx,y + h - 1 - yy, left, right, color[CSS_BOTTOM],thickness);
			}

			if (Radius[CSS_LEFT]== 0)
			{
				left += leftStep;
				leftErr += leftMod;
				if (leftErr >= border[CSS_BOTTOM])
				{
					left++;
					leftErr -= border[CSS_BOTTOM];
					}
			}
			if (Radius[CSS_RIGHT]== 0)
			{
				right -= rightStep;
				rightErr += rightMod;
				if (rightErr >= border[CSS_BOTTOM])
				{
					right--;
					rightErr -= border[CSS_BOTTOM];
				}
			}
		}
	}
#if (WEBC_SUPPORT_RADIUS)
	if (Radius[0])
		GuiDrawCorners(ctx, y, x, h-border[CSS_BOTTOM], w - (border[CSS_LEFT]+border[CSS_RIGHT]), border[0], Radius[0], color[0]);
#endif
}


#endif

WEBC_BOOL HTMLContentBox::ResetProperties (void)
{
	mFlags = 0;
	mBorderStyle = 0;

	// tbd - only return WEBC_TRUE if something changes

	return (WEBC_TRUE);
}

DISPLAY_INT HTMLContentBox::GetEmHeight (void)
{
	if (!(mFlags & HCBOX_FLAG_EM_HEIGHT_SET))
	{
		WebFont font = mpElement->GetWebFont();
		miEmHeight = font? WEB_FONT_HEIGHT(font) : WEBC_CFG_DEFAULT_TEXT_HEIGHT;
		miExHeight = miEmHeight/2;
		mFlags |= HCBOX_FLAG_EM_HEIGHT_SET;
	}

	return (miEmHeight);
}

DISPLAY_INT HTMLContentBox::GetExHeight (void)
{
	if (!(mFlags & HCBOX_FLAG_EM_HEIGHT_SET))
	{
		WebFont font = mpElement->GetWebFont();
		miEmHeight = font? WEB_FONT_HEIGHT(font) : WEBC_CFG_DEFAULT_TEXT_HEIGHT;
		miExHeight = miEmHeight/2;
		mFlags |= HCBOX_FLAG_EM_HEIGHT_SET;
	}

	return (miExHeight);
}

DISPLAY_INT HTMLContentBox::ConvertCSSLength(CSSLength *len)
{
	switch (len->type)
	{
		case CSS_LENGTH_UNIT_EM:
		case CSS_LENGTH_UNIT_EX:
			if (!(mFlags & HCBOX_FLAG_EM_HEIGHT_SET))
			{
				WebFont font = mpElement->GetWebFont();
				miEmHeight = font? WEB_FONT_HEIGHT(font) : WEBC_CFG_DEFAULT_TEXT_HEIGHT;
				miExHeight = miEmHeight/2;
				mFlags |= HCBOX_FLAG_EM_HEIGHT_SET;
			}
			break;

		case CSS_LENGTH_UNIT_PERCENT:
			mBorderStyle |= HCBOX_HAS_PERCENT;
			break;

		default:
			break;
	}

	return (CSS_LengthToPixels(len, miParentWidth, miEmHeight, miExHeight));
}

int HTMLContentBox::HasPercent(void)
{
	return ((mBorderStyle & HCBOX_HAS_PERCENT) != 0);
}
