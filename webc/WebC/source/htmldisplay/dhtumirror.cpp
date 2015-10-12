//
// DHTUMIRROR.CPP - Methods for class HTMLTextUnitMirror
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

#include "dhtumirror.hpp"
#include "helement.hpp"


/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/


/*****************************************************************************/
// Local Types
/*****************************************************************************/


/*****************************************************************************/
// Global Data
/*****************************************************************************/

static CSSPropertyType borderWidthProperty[4] =
{
	CSS_PROPERTY_BORDER_TOP_WIDTH,
	CSS_PROPERTY_BORDER_RIGHT_WIDTH,
	CSS_PROPERTY_BORDER_BOTTOM_WIDTH,
	CSS_PROPERTY_BORDER_LEFT_WIDTH
};

static CSSPropertyType borderStyleProperty[4] =
{
	CSS_PROPERTY_BORDER_TOP_STYLE,
	CSS_PROPERTY_BORDER_RIGHT_STYLE,
	CSS_PROPERTY_BORDER_BOTTOM_STYLE,
	CSS_PROPERTY_BORDER_LEFT_STYLE
};

static DISPLAY_INT borderWidthThickness[4] =
{
	1,
	3,
	6,
	8
};

/*****************************************************************************/
// Local functions
/*****************************************************************************/

HTMLTextUnitMirror::HTMLTextUnitMirror (HTMLElement *pElem)
{
	mpHtmlElement = pElem;
	mData = 0;
	mTUMirrorValid = 0;
	miTUMirrorZIndex = 0;
}

HTMLTextUnitMirror::~HTMLTextUnitMirror ()
{
}

WEBC_UINT16 HTMLTextUnitMirror::GetUnitType(void)
{
	if (!(mData & TUM_TYPE_VALID))
	{
		mData |= TUM_TYPE_VALID;
		TUM_SET_TYPE(mData, mpHtmlElement->GetUnitType());
	}
	return ((WEBC_UINT16) TUM_GET_TYPE(mData));
}

WEBC_UINT16 HTMLTextUnitMirror::GetBreak(void)
{
	if (!(mData & TUM_BREAK_VALID))
	{
		mData |= TUM_BREAK_VALID;
		TUM_SET_BREAK(mData, mpHtmlElement->GetBreak());
	}
	return ((WEBC_UINT16) TUM_GET_BREAK(mData));
}

WEBC_UINT16 HTMLTextUnitMirror::GetClear(void)
{
	if (!(mData & TUM_CLEAR_VALID))
	{
		mData |= TUM_CLEAR_VALID;
		TUM_SET_CLEAR(mData, mpHtmlElement->GetClear());
	}
	return ((WEBC_UINT16) TUM_GET_CLEAR(mData));
}

WEBC_UINT16 HTMLTextUnitMirror::GetDir(void)
{
	if (!(mData & TUM_DIR_VALID))
	{
		mData |= TUM_DIR_VALID;
		TUM_SET_DIR(mData, mpHtmlElement->GetDir());
	}
	return ((WEBC_UINT16) TUM_GET_DIR(mData));
}

WEBC_UINT16 HTMLTextUnitMirror::GetWrap(void)
{
	if (!(mData & TUM_WRAP_VALID))
	{
		mData |= TUM_WRAP_VALID;
		TUM_SET_WRAP(mData, mpHtmlElement->GetWrap());
	}
	return ((WEBC_UINT16) TUM_GET_WRAP(mData));
}

WEBC_UINT16 HTMLTextUnitMirror::GetVisibility(void)
{
	if (!(mData & TUM_VISIBILITY_VALID))
	{
		// Disable this optimization because it doesn't propigate through the element's children
		// mData |= TUM_VISIBILITY_VALID;
		TUM_SET_VISIBILITY(mData, mpHtmlElement->GetVisibility());
	}
	return ((WEBC_UINT16) TUM_GET_VISIBILITY(mData));
}

DISPLAY_INT HTMLTextUnitMirror::GetMinWidth(void)
{
	return (mpHtmlElement->GetMinWidth());
}

DISPLAY_INT HTMLTextUnitMirror::GetMaxWidth(void)
{
	return (mpHtmlElement->GetMaxWidth());
}

DISPLAY_INT HTMLTextUnitMirror::GetPixelWidth(void)
{
	return (mpHtmlElement->GetPixelWidth());
}

DISPLAY_INT HTMLTextUnitMirror::GetPixelHeight(void)
{
	return (mpHtmlElement->GetPixelHeight());
}

WEBC_UINT16 HTMLTextUnitMirror::GetPosition(void)
{
	if (!(mData & TUM_POSITION_VALID))
	{
		mData |= TUM_POSITION_VALID;
		TUM_SET_POSITION(mData, mpHtmlElement->GetPosition());
	}
	return ((WEBC_UINT16) TUM_GET_POSITION(mData));
}

DISPLAY_INT HTMLTextUnitMirror::GetLeft(void)
{
	DISPLAY_INT x = 0, fontHeight = 0, iParentWidth = 0;
	CSSPropertyValue value;
	CSSPropertyType type = CSS_PROPERTY_LEFT;

	if ((mpHtmlElement->GetStyleFromCSS(type, &value) != CSS_VALUE_SPECIFIED) ||
	    (value.length.type == CSS_LENGTH_UNIT_AUTO))
	{
		if ((mpHtmlElement->GetStyleFromHTML(type, &value) != CSS_VALUE_SPECIFIED) ||
		    (value.length.type == CSS_LENGTH_UNIT_AUTO))
		{
			type = CSS_PROPERTY_RIGHT;
			if (mpHtmlElement->GetStyleFromCSS(type, &value) != CSS_VALUE_SPECIFIED)
			{
				return (0);
			}
		}
	}

	switch (value.length.type)
	{
		case CSS_LENGTH_UNIT_EM:
		case CSS_LENGTH_UNIT_EX:
		{
			WebFont font = mpHtmlElement->GetWebFont();
			fontHeight = WEB_FONT_HEIGHT(font);
			break;
		}
        default:
            break;
	}

	if ((value.length.type == CSS_LENGTH_UNIT_PERCENT) || (type == CSS_PROPERTY_RIGHT))
	{
		iParentWidth = GetParentWidth();
	}

	switch (type)
	{
		case CSS_PROPERTY_LEFT:
			x = CSS_LengthToPixels(&value.length, iParentWidth, fontHeight, fontHeight/2);
			break;

		case CSS_PROPERTY_RIGHT:
			x = iParentWidth - GetPixelWidth();
			x -= CSS_LengthToPixels(&value.length, iParentWidth, fontHeight, fontHeight/2);
			break;
        default:
            break;
	}

	return (x);
}

DISPLAY_INT HTMLTextUnitMirror::GetTop(void)
{
	return (mpHtmlElement->GetTop());
}

DISPLAY_INT HTMLTextUnitMirror::GetTopMargin(void)
{
	return (mpHtmlElement->GetTopMargin());
}

DISPLAY_INT HTMLTextUnitMirror::GetLeftMargin(void)
{
	return (mpHtmlElement->GetLeftMargin());
}

DISPLAY_INT HTMLTextUnitMirror::GetBottomMargin(void)
{
	return (mpHtmlElement->GetBottomMargin());
}

DISPLAY_INT HTMLTextUnitMirror::GetRightMargin(void)
{
	return (mpHtmlElement->GetRightMargin());
}

DISPLAY_INT HTMLTextUnitMirror::GetBaseline(void)
{
	return (mpHtmlElement->GetBaseline());
}

WEBC_UINT8 HTMLTextUnitMirror::GetFloat(void)
{
	if (!(mData & TUM_FLOAT_VALID))
	{
		mData |= TUM_FLOAT_VALID;
		TUM_SET_FLOAT(mData, mpHtmlElement->GetFloat());
	}
	return ((WEBC_UINT8) TUM_GET_FLOAT(mData));
}

DISPLAY_INT HTMLTextUnitMirror::GetParagraphSpace(void)
{
	return (mpHtmlElement->GetParagraphSpace());
}

void HTMLTextUnitMirror::GetXYPosition(DISPLAY_INT *piXPos, DISPLAY_INT *piYPos)
{
}

WEBC_UINT8 HTMLTextUnitMirror::GetTextAlign(void)
{
	if (!(mData & TUM_TEXT_ALIGN_VALID))
	{
		mData |= TUM_TEXT_ALIGN_VALID;
		TUM_SET_TEXT_ALIGN(mData, mpHtmlElement->GetTextAlign());
	}
	return ((WEBC_UINT8) TUM_GET_TEXT_ALIGN(mData));
}

WEBC_UINT8 HTMLTextUnitMirror::GetVAlign(void)
{
	if (!(mData & TUM_VALIGN_VALID))
	{
		mData |= TUM_VALIGN_VALID;
		TUM_SET_VALIGN(mData, mpHtmlElement->GetVAlign());
	}
	return ((WEBC_UINT8) TUM_GET_VALIGN(mData));
}

void HTMLTextUnitMirror::SetXYPosition(DISPLAY_INT iXPos, DISPLAY_INT iYPos)
{
}

void HTMLTextUnitMirror::SetParentWidth(DISPLAY_INT iWidth, DISPLAY_INT iAvail)
{
}

void HTMLTextUnitMirror::SetParentHeight(DISPLAY_INT iHeight)
{
}

void HTMLTextUnitMirror::SetParentDisplay(DisplayElement* parent, DisplayElement* insertBefore)
{
}

void HTMLTextUnitMirror::Release(void)
{
}

WEBC_INT32 HTMLTextUnitMirror::FindDisplayZIndex (void)
{
	if (!(mTUMirrorValid & TUM_VALID_ZINDEX))
	{
		WEBC_INT32 z = (WEBC_INT32) mpHtmlElement->GetZIndex();

		z += 0x00007fffL; // largest signed 16-bit positive value
		z <<= 2; // shift left by 2 bits

		switch (GetPosition())
		{
		case TU_POSITION_RELATIVE:
		case TU_POSITION_STATIC:
			switch (GetFloat())
			{
			case TU_FLOAT_NONE:
				break;

			default:
				z |= 0x1;
				break;
			}
			break;

		case TU_POSITION_ABSOLUTE:
			z |= 0x2;
			break;

		case TU_POSITION_FIXED:
			z |= 0x3;
			break;

		default:
			break;
		}

		miTUMirrorZIndex = z;
		mTUMirrorValid |= TUM_VALID_ZINDEX;
	}

	return (miTUMirrorZIndex);
}

// return WEBC_TRUE if something changed
WEBC_BOOL HTMLTextUnitMirror::InvalidateTUMirror(void)
{
WEBC_UINT32 oldData = mData;

	mData &= ~TUM_ANY_VALID;

	if (mTUMirrorValid & TUM_VALID_ZINDEX)
	{
		long oldIndex;
		mTUMirrorValid &= ~TUM_VALID_ZINDEX;
		oldIndex = miTUMirrorZIndex;
		if (oldIndex != FindDisplayZIndex())
		{
			return (WEBC_TRUE);
		}
	}

	if (oldData & TUM_TYPE_VALID)
	{
		if (GetUnitType() != TUM_GET_TYPE(oldData))
		{
			return (WEBC_TRUE);
		}
	}

	if (oldData & TUM_BREAK_VALID)
	{
		if (GetBreak() != TUM_GET_BREAK(oldData))
		{
			return (WEBC_TRUE);
		}
	}

	if (oldData & TUM_CLEAR_VALID)
	{
		if (GetClear() != TUM_GET_CLEAR(oldData))
		{
			return (WEBC_TRUE);
		}
	}

	if (oldData & TUM_DIR_VALID)
	{
		if (GetDir() != TUM_GET_DIR(oldData))
		{
			return (WEBC_TRUE);
		}
	}

	if (oldData & TUM_WRAP_VALID)
	{
		if (GetWrap() != TUM_GET_WRAP(oldData))
		{
			return (WEBC_TRUE);
		}
	}

	/*
	if (oldData & TUM_VISIBILITY_VALID)
	{
		if (GetVisibility() != TUM_GET_VISIBILITY(oldData))
		{
			return (WEBC_TRUE);
		}
	}
	*/

	if (oldData & TUM_POSITION_VALID)
	{
		if (GetPosition() != TUM_GET_POSITION(oldData))
		{
			return (WEBC_TRUE);
		}
	}

	if (oldData & TUM_FLOAT_VALID)
	{
		if (GetFloat() != TUM_GET_FLOAT(oldData))
		{
			return (WEBC_TRUE);
		}
	}

	if (oldData & TUM_TEXT_ALIGN_VALID)
	{
		if (GetTextAlign() != TUM_GET_TEXT_ALIGN(oldData))
		{
			return (WEBC_TRUE);
		}
	}

	if (oldData & TUM_VALIGN_VALID)
	{
		if (GetVAlign() != TUM_GET_VALIGN(oldData))
		{
			return (WEBC_TRUE);
		}
	}

	return (WEBC_FALSE);
}

// return WEBC_TRUE if something changed
WEBC_BOOL HTMLTextUnitMirror::InvalidateZIndex(void)
{
	if (mTUMirrorValid & TUM_VALID_ZINDEX)
	{
		long oldIndex = miTUMirrorZIndex;
		mTUMirrorValid &= ~TUM_VALID_ZINDEX;
		if (oldIndex != FindDisplayZIndex())
		{
			return (WEBC_TRUE);
		}
	}

	return (WEBC_FALSE);
}


WEBC_BOOL HTMLTextUnitMirror::InvalidateVisibility(void)
{
	if (mData & TUM_VISIBILITY_VALID)
	{
		WEBC_UINT32 oldData = mData;
		mData &= ~TUM_VISIBILITY_VALID;
		if (GetVisibility() != TUM_GET_VISIBILITY(oldData))
		{
			return (WEBC_TRUE);
		}
	}

	return (WEBC_FALSE);
}


DisplayElement *HTMLTextUnitMirror::GetHTMLParentDisplay (void)
{
HTMLElement *parent;
DisplayElement *parentDisplay;

	if (!mpHtmlElement)
	{
		return (0);
	}

	// find the closest parent of this HTMLElement that has a
	//  DisplayElement already and return that DisplayElement
	parent = mpHtmlElement->mpParent;
	while (parent)
	{
		parentDisplay = parent->GetDisplayElement(WEBC_FALSE);
		if (parentDisplay)
		{
			return (parentDisplay);
		}
		parent = parent->mpParent;
	}

	return (0);
}

DISPLAY_INT HTMLTextUnitMirror::GetParentWidth(void)
{
	return (0);
}

DISPLAY_INT HTMLTextUnitMirror::GetPixelLength (
		CSSPropertyType property,
		DISPLAY_INT relativeTo,
		DISPLAY_INT emHeight,
		DISPLAY_INT exHeight,
		DISPLAY_INT defaultValue
	)
{
	CSSLength length;

	GetCSSLength(&length, property);

	return (GetPixelLength(&length, relativeTo, emHeight, exHeight, defaultValue));
}

DISPLAY_INT HTMLTextUnitMirror::GetPixelLength (
		CSSLength* length,
		DISPLAY_INT relativeTo,
		DISPLAY_INT emHeight,
		DISPLAY_INT exHeight,
		DISPLAY_INT defaultValue
	)
{
	if (length->type == CSS_LENGTH_UNIT_AUTO)
	{
		return (defaultValue);
	}

	return (CSS_LengthToPixels(length, relativeTo, emHeight, exHeight));
}

// sets the length type to auto if property can not be retrieved;
//  converts to pixels if possible (leaves auto as auto)
void HTMLTextUnitMirror::GetCSSLength (
		CSSLength* length,
		CSSPropertyType property
	)
{
	CSSPropertyValue value;

	if (mpHtmlElement->GetStyleFromCSS(property, &value) != CSS_VALUE_SPECIFIED)
	{
		if (mpHtmlElement->GetStyleFromHTML(property, &value) != CSS_VALUE_SPECIFIED)
		{
			value.length.type = CSS_LENGTH_UNIT_AUTO;
		}
	}

	*length = value.length;
}

DISPLAY_INT HTMLTextUnitMirror::GetBorderWidth (
		CSSSide side,
		DISPLAY_INT relativeTo,
		DISPLAY_INT emHeight,
		DISPLAY_INT exHeight,
		DISPLAY_INT parentBorderWidth
	)
{
	DISPLAY_INT borderWidth = 0;
	CSSPropertyType property;
	CSSPropertyValue value;
	CSSValueType result;
	side = (CSSSide) (((int) side) & 3);

	property = borderStyleProperty[side];

	if ((result = mpHtmlElement->GetStyleFromCSS(property, &value)) != CSS_VALUE_UNKNOWN ||
	    (result = mpHtmlElement->GetStyleFromHTML(property, &value)) != CSS_VALUE_UNKNOWN)
	{
		if ((result == CSS_VALUE_INHERIT && parentBorderWidth > 0) ||
		    value.borderStyle != CSS_BORDER_STYLE_NONE)
		{
			property = borderWidthProperty[side];

			if ((result = mpHtmlElement->GetStyleFromCSS(property, &value)) != CSS_VALUE_UNKNOWN ||
			    (result = mpHtmlElement->GetStyleFromHTML(property, &value)) != CSS_VALUE_UNKNOWN)
			{
				if (result == CSS_VALUE_INHERIT)
				{
					return (parentBorderWidth);
				}

				switch (value.borderThickness.type)
				{
					case CSS_BORDER_THICKNESS_LENGTH:
						borderWidth = GetPixelLength (
								&value.borderThickness.val.length,
								relativeTo,
								emHeight,
								exHeight,
								0 // defaultValue
							);
						break;

					case CSS_BORDER_THICKNESS_ABSOLUTE:
						borderWidth = borderWidthThickness[value.borderThickness.val.absolute & 0x3];
						break;

					default:
						break;
				}
			}
		}
	}

	return (borderWidth);
}

WEBC_BOOL HTMLTextUnitMirror::InvalidateUnitType (void)
{
WEBC_UINT32 oldData = mData;

	mData &= ~TUM_TYPE_VALID;
	if (oldData & TUM_TYPE_VALID)
	{
		if (GetUnitType() != TUM_GET_TYPE(oldData))
		{
			return (WEBC_TRUE);
		}
	}

	return (WEBC_FALSE);
}

WEBC_BOOL HTMLTextUnitMirror::InvalidatePosition (void)
{
WEBC_UINT32 oldData = mData;

	mData &= ~TUM_POSITION_VALID;
	if (oldData & TUM_POSITION_VALID)
	{
		if (GetPosition() != TUM_GET_POSITION(oldData))
		{
			return (WEBC_TRUE);
		}
	}

	return (WEBC_FALSE);
}
