//
// WTPASSWD.CPP -
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
#include "wtpasswd.hpp"
#include "webc.h"
#include "dmanager.hpp"
#include "util.h"
#include "objref.cpp"



/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/

#define WEBC_PASSWORD_CHAR    (WebChar) '*'

// define this symbol to show the last char of the password widget
//  (helpful if using a keypad-type character input device)
#define WEBC_PASSWORD_SHOW_LAST_CHAR
#undef WEBC_PASSWORD_SHOW_LAST_CHAR

#undef WEBC_PASSWORD_SHOW_LAST_SELECTED_CHAR
// #define WEBC_PASSWORD_SHOW_LAST_SELECTED_CHAR

#define TOTAL_LEFT_PAD         (EDITSTR_MARGIN+EDITSTR_BORDER+EDITSTR_PADDING)
#define TOTAL_RIGHT_PAD        TOTAL_LEFT_PAD
#define TOTAL_TOP_PAD          TOTAL_LEFT_PAD
#define TOTAL_BOTTOM_PAD       TOTAL_LEFT_PAD


/*****************************************************************************/
// Local Types
/*****************************************************************************/


/*****************************************************************************/
// Global Data
/*****************************************************************************/

static WebChar gpWebcPasswordString[] = {WEBC_PASSWORD_CHAR, 0};


/*****************************************************************************/
// Local functions
/*****************************************************************************/

WebPasswordString::WebPasswordString ()
{
}

WebPasswordString::~WebPasswordString ()
{
}

long WebPasswordString::GetIndexAtXY(DISPLAY_INT x, DISPLAY_INT y)
{
	WebGraphics *gc = GetGraphics();
	if (!gc)
	{
		return (0);
	}

	DISPLAY_INT displayX = mRect.left, displayY = mRect.top;
	DISPLAY_INT charWidth = gc->TextWidth(gpWebcPasswordString, mFont.GetFont());

	WebRect box;
	GetTextRect(&box);

	if (mpParent)
	{
		mpParent->GetDisplayPosition(this, &displayX, &displayY);
	}
	return ((x - displayX - box.left + miXOffset - 1 + (charWidth/2)) / charWidth);
}

void WebPasswordString::DrawText(WebGraphics *gc, DISPLAY_INT x, DISPLAY_INT y, const WebChar *text, WebColor textColor, WebColor fillColor, WEBC_BOOL fill, WebFont font, int len)
{
	DISPLAY_INT charWidth = gc->TextWidth(gpWebcPasswordString, font);

  #ifdef WEBC_PASSWORD_SHOW_LAST_SELECTED_CHAR
	if ((text[0] && !text[1]) || (len == 1))
	{
		if ((mEditFlags & EDIT_FLAG_HAS_SELECTION) &&
		    (mFlags & DISPLAY_FLAG_FOCUS) &&
		   !(mFlags & DISPLAY_FLAG_DISABLED) &&
		    (mpText + EBSMIN(miCursorPos, miSelectBegin) == text) &&
		    (EBSMAX(miCursorPos, miSelectBegin) == miTextLen))
		{
			// draw the character as plaintext if it is the last character and the
			//  sole selected character
			gc->TextLen(x, y, text, textColor, fillColor, fill, font, 1);
			return;
		}
	}
  #endif

	for (int i=0; text[i] && (len<0 || i<len); i++)
	{
	  #if (defined(WEBC_PASSWORD_SHOW_LAST_CHAR) && !defined(WEBC_PASSWORD_SHOW_LAST_SELECTED_CHAR))
		if ((mFlags & DISPLAY_FLAG_FOCUS) && (text[i+1] == '\0'))
		{
			gc->Text(x, y, &text[i], textColor, fillColor, fill, font);
			break;
		}
	  #endif

		gc->Text(x, y, gpWebcPasswordString, textColor, fillColor, fill, font);
		x += charWidth;
	}
}

DISPLAY_INT WebPasswordString::PixelOffset (long index)
{
WebGraphics *gc = GetGraphics();

	if (gc)
	{
		if (index > 0)
		{
		  #if (defined(WEBC_PASSWORD_SHOW_LAST_CHAR) && !defined(WEBC_PASSWORD_SHOW_LAST_SELECTED_CHAR))
			if ((mFlags & DISPLAY_FLAG_FOCUS) && !mpText[index])
			{
				return (gc->TextWidth(gpWebcPasswordString, mFont.GetFont()) * (index - 1) +
						gc->TextWidthLen(&mpText[index - 1], mFont.GetFont(), 1));
			}
		  #endif

			return (gc->TextWidth(gpWebcPasswordString, mFont.GetFont()) * index);
		}
	}

	return (0);
}
