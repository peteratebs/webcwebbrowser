/*
|  TEXTSTYLE.CPP -
|
|  EBS -
|
|
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

/*****************************************************************************/
/* Header files
 *****************************************************************************/

#include "textstyle.hpp"
#include "webcmem.h"
#include "webcassert.h"
#include "dmanager.hpp"
#include "htmldoc.hpp"

/*****************************************************************************/
/* Macros
 *****************************************************************************/

/*****************************************************************************/
/* Types
 *****************************************************************************/

/*****************************************************************************/
/* Function Prototypes
 *****************************************************************************/

/*****************************************************************************/
/* Data
 *****************************************************************************/

#define TEXT_OUTLINE_PADDING 1


/*****************************************************************************/
/* Function Definitions
 *****************************************************************************/

//=============================================================================
// ITextDisplayStyle methods
//=============================================================================

//-----------------------------------------------------------------------------
ITextDisplayStyle::ITextDisplayStyle (void)
{
}

//-----------------------------------------------------------------------------
ITextDisplayStyle::~ITextDisplayStyle (void)
{
}

//-----------------------------------------------------------------------------
WEBC_BOOL ITextDisplayStyle::equals (ITextDisplayStyle* other)
{
	return (other &&
	        this->getBackgroundColor() == other->getBackgroundColor() &&
	        this->hasBackgroundColor() == other->hasBackgroundColor() &&
	        this->getColor()           == other->getColor()           &&
	        this->getDecoration()      == other->getDecoration()      &&
	        this->getFont()            == other->getFont()            &&
	        this->getOutlineStyle()    == other->getOutlineStyle());
}

//=============================================================================
// TextDisplayStyle methods
//=============================================================================

//-----------------------------------------------------------------------------
TextDisplayStyle::TextDisplayStyle (
		WebGraphFont* font,
		WebColor color,
		WebColor backgroundColor,
		WEBC_BOOL hasBackgroundColor,
		CSSTextDecoration decoration,
		CSSBorderStyle outlineStyle
	)
{
	mBackgroundColor = backgroundColor;
	mColor = color;
	mDecoration = decoration;
	mFont.Set(font);
	mHasBackgroundColor = hasBackgroundColor;
	mOutlineStyle = outlineStyle;
}

//-----------------------------------------------------------------------------
TextDisplayStyle::~TextDisplayStyle (void)
{
}

//-----------------------------------------------------------------------------
WebColor TextDisplayStyle::getBackgroundColor (void)
{
	return (mBackgroundColor);
}

//-----------------------------------------------------------------------------
WebColor TextDisplayStyle::getColor (void)
{
	return (mColor);
}

//-----------------------------------------------------------------------------
CSSTextDecoration TextDisplayStyle::getDecoration (void)
{
	return (mDecoration);
}

//-----------------------------------------------------------------------------
WebFont TextDisplayStyle::getFont (void)
{
	return (mFont.GetFont());
}

//-----------------------------------------------------------------------------
CSSBorderStyle TextDisplayStyle::getOutlineStyle (void)
{
	return (mOutlineStyle);
}

//-----------------------------------------------------------------------------
WEBC_BOOL TextDisplayStyle::hasBackgroundColor (void)
{
	return (mHasBackgroundColor);
}

//-----------------------------------------------------------------------------
void TextDisplayStyle::recycle (void)
{
	WEBC_DELETE(this);
}

//-----------------------------------------------------------------------------
void TextDisplayStyle::renderText (DISPLAY_INT x, DISPLAY_INT y, WebChar* text, WEBC_SIZE textLen, WebGraphics* gc)
{
	WebRect where;
	WebFont font = mFont.GetFont();

	if (font)
	{
		where.SetSizeAndPosition(x, y, gc->TextWidthLen(text, font, textLen), gc->TextHeightLen(text, font, textLen));
		renderText(&where, text, textLen, gc);
	}
}

//-----------------------------------------------------------------------------
void TextDisplayStyle::renderText (WebRect* where, WebChar* text, WEBC_SIZE textLen, WebGraphics* gc)
{
	WebFont font = mFont.GetFont();
	DISPLAY_INT x = where->left;
	DISPLAY_INT y = where->top;

	if (textLen > 0)
	{
//		printf("[%d,%d] %S \n", x, y, text);
		gc->TextLen(x, y, text, mColor, mBackgroundColor, mHasBackgroundColor, font, textLen);
	}

	switch (mDecoration)
	{
		case CSS_TEXT_DECORATION_UNDERLINE:
		{
			WebRect decorationRect;
			DISPLAY_INT widthValue = where->Width();
			DISPLAY_INT baselineValue = WEB_FONT_BASELINE(font) + 2;

			decorationRect.Set(x, y + baselineValue - 1, x + widthValue - 1, y + baselineValue - 1);
			gc->Rectangle(&decorationRect, mColor, mColor, WEBC_TRUE);
			break;
		}

		case CSS_TEXT_DECORATION_LINETHRU:
		{
			WebRect decorationRect;
			DISPLAY_INT widthValue = where->Width();
			DISPLAY_INT baselineValue = WEB_FONT_BASELINE(font) + 2;

			decorationRect.Set(x, y + (baselineValue>>1) + 1, x + widthValue - 1, y + (baselineValue>>1) + 1);
			gc->Rectangle(&decorationRect, mColor, mColor, WEBC_TRUE);
			break;
		}

		default:
			break;
	}

	switch (mOutlineStyle)
	{
		case CSS_BORDER_STYLE_NONE:
			break;

		default:
		{
			WebColor blackColor = gc->RGBToColor(0,0,0);
			PresetWebRect frameRect (
					where->left - TEXT_OUTLINE_PADDING, 
					where->top - TEXT_OUTLINE_PADDING, 
					where->right + TEXT_OUTLINE_PADDING,
					where->bottom + TEXT_OUTLINE_PADDING);

			gc->PatternFrame(&frameRect, blackColor, 1, mOutlineStyle);
			break;
		}
	}
}

//=============================================================================
// TextDisplayStyle methods
//=============================================================================

//-----------------------------------------------------------------------------
NewTextDisplayStyleFactory::NewTextDisplayStyleFactory (WebGraphics* gc)
{
	mGC = gc;
}

//-----------------------------------------------------------------------------
NewTextDisplayStyleFactory::~NewTextDisplayStyleFactory (void)
{
}

//-----------------------------------------------------------------------------
ITextDisplayStyle* NewTextDisplayStyleFactory::newTextStyle (
		WebGraphFont* font,
		HTMLColor color,
		HTMLColor backgroundColor,
		CSSTextDecoration decoration,
		CSSBorderStyle outlineStyle
	)
{
	ITextDisplayStyle* textStyle = 0;

	if (mGC)
	{
		WEBC_BOOL hasBackgroundColor = (HTML_ColorGetAlpha(backgroundColor) == 0);

		WEBC_NEW_VERBOSE(textStyle, TextDisplayStyle (
				font,
				mGC->RGBToColor(color),
				hasBackgroundColor? mGC->RGBToColor(backgroundColor) : 0,
				hasBackgroundColor,
				decoration,
				outlineStyle),"newTextStyle");
	}

	return (textStyle);
}

//=============================================================================
// TextDisplayStyle methods
//=============================================================================
//-----------------------------------------------------------------------------
HTMLDocumentTextDisplayStyleFactory::HTMLDocumentTextDisplayStyleFactory (HTMLDocument* doc)
	: NewTextDisplayStyleFactory(0)
{
	DisplayManager* manager = (doc)? doc->GetDisplayManager() : 0;
	mGC = (manager)? manager->GetGraphics() : 0;
}

//-----------------------------------------------------------------------------
HTMLDocumentTextDisplayStyleFactory::~HTMLDocumentTextDisplayStyleFactory (void)
{
}
