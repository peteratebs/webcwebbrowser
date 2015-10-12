//
// WTEDSTR.CPP -
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
#include "wtedstr.hpp"
#include "webc.h"
#include "dmanager.hpp"
#include "util.h"
#include "wgkeys.hpp"
#include "wgcolors.hpp"
#include "webcstr.h"
#include "webcassert.h"
#include "objref.cpp"
#include "webcmem.h"


/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/

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


/*****************************************************************************/
// Local functions
/*****************************************************************************/

WebEditString::WebEditString ()
{
	miXOffset = 0;
	mpText = 0;
	miTextSize = 0;
	miCursorPos = 0;
	miSelectBegin = 0;
	mEditFlags = 0;
	mFont = 0;
	mpListener = 0;
	miTextLen = 0;

	SetFlag(DISPLAY_FLAG_ACCEPTS_FOCUS);
}

WebEditString::~WebEditString ()
{
	if (mpText)
	{
		WEBC_FREE(mpText);
	}
}

#if (WEBC_SUPPORT_TOUCHSCREEN)
void PopUpKeyPad(HTMLBrowser *pBrowser, DISPLAY_INT numlines, void *pEl, WEBC_CHAR *pValue, void (*closeFn)(void *El, WEBC_CHAR *p));
void EditStringKeyboardcloseFn(void *El, WEBC_CHAR *p)
{
	// Copy the contents to the application window from the softkey input window
	if (p)
		((HTMLElement *)El)->SetValue(p);
}
#endif
HTMLEventStatus WebEditString::Event (HTMLEvent *e)
{
    WEBC_DEBUG_ReceiveEvent(this, e);
	switch (e->type)
	{

		case HTML_EVENT_MOUSEDOWN:
			if (!(mFlags & DISPLAY_FLAG_DISABLED))
			{
#if (WEBC_SUPPORT_TOUCHSCREEN)
				if (mpListener)
				{
					PopUpKeyPad((HTMLBrowser *)webc_GetBrowserWithFocus(), 1, (void *) mpListener, ((HTMLElement *)mpListener)->Value(), EditStringKeyboardcloseFn);
					return (HTML_EVENT_STATUS_DONE);
				}
#endif
				if (mFlags & DISPLAY_FLAG_FOCUS)
				{
					long index = GetIndexAtXY(e->data.position.x, e->data.position.y);
					if (e->flags & HTML_EVENT_FLAG_SHIFT_DOWN)
					{
						BeginSelect();
						SetCursorPosition(index);
					}
					else
					{
						SetCursorPosition(index);
						BeginSelect();
					}

					mEditFlags |= EDIT_FLAG_MOUSEDOWN;
				}

				if (GetManager())
				{
					GetManager()->ClaimPointer(this);
				}

			}
			break;

		case HTML_EVENT_MOUSEUP:
			EndSelect();

			if (GetManager())
			{
				GetManager()->ReleasePointer(this);
			}

			mEditFlags &= ~EDIT_FLAG_MOUSEDOWN;
			break;

		case HTML_EVENT_MOUSEOUT:
			break;

		case HTML_EVENT_MOUSEOVER:
			break;

		case HTML_EVENT_MOUSEMOVE:
			if (mEditFlags & EDIT_FLAG_MOUSEDOWN)
			{
				long index = GetIndexAtXY(e->data.position.x, e->data.position.y);
				SetCursorPosition(index);
			}
			break;

		case HTML_EVENT_KEYDOWN:
			switch (e->data.key)
			{
				case WGK_TAB:
				case WGK_PGUP:
				case WGK_PGDN:
				case WGK_LNUP:
				case WGK_LNDN:
				case WGK_ALT:
				case WGK_CTRL:
				case WGK_SHIFT:
				case WGK_CAPS:
				case WGK_NUMLOCK:
				case WGK_SCROLLLOCK:
				case WGK_F1:
				case WGK_F2:
				case WGK_F3:
				case WGK_F4:
				case WGK_F5:
				case WGK_F6:
				case WGK_F7:
				case WGK_F8:
				case WGK_F9:
				case WGK_F10:
				case WGK_F11:
				case WGK_F12:
					break;

				case WGK_CR:
					SelectAll();
					if (mpListener)
					{
						NotifyEvent what(NOTIFIER_TYPE_EDIT, NOTIFY_TEXT_EDIT);
						mpListener->Notify(this, &what);
					}
					break;

				case WGK_INSERT:
					mEditFlags ^= EDIT_FLAG_OVERWRITE;
					break;

				case WGK_DELETE:
					Delete();
					break;

				case WGK_BACKSPACE:
					Backspace();
					break;

				case WGK_LEFT:
				case WGK_RIGHT:
				case WGK_HOME:
				case WGK_END:
				{
					WEBC_BOOL selecting = 0;
					if (e->flags & HTML_EVENT_FLAG_SHIFT_DOWN)
					{
						selecting = 1;
						BeginSelect();
					}
					switch (e->data.key)
					{
						case WGK_LEFT:
							SetCursorPosition(miCursorPos-1);
							break;

						case WGK_RIGHT:
							SetCursorPosition(miCursorPos+1);
							break;

						case WGK_HOME:
							Home();
							break;

						case WGK_END:
							End();
							break;
					}
					if (selecting)
					{
						EndSelect();
					}
					break;
				}

				case WGK_ESC:
					SelectAll();
					break;

				default:
					InsertChar((WebChar) e->data.key);
					break;
			}
			break;

		case HTML_EVENT_KEYUP:
			break;

		case HTML_EVENT_FOCUS:
			SetFlag(DISPLAY_FLAG_FOCUS);
			Invalidate();

/* Select all if WEBC_CFG_STR_FOCUS_SELECT is enabled, otherwise leave select unchanged */
#if (WEBC_CFG_STR_FOCUS_SELECT)
			SelectAll();
#endif
			break;

		case HTML_EVENT_UNFOCUS:
			ClearFlag(DISPLAY_FLAG_FOCUS);
			Invalidate();
/* Kill selection if WEBC_CFG_STR_FOCUS_SELECT is enabled, otherwise leave select unchanged */
#if (WEBC_CFG_STR_FOCUS_SELECT)
			// kill the selection when focus moves on
			mEditFlags &= ~EDIT_FLAG_HAS_SELECTION;
#endif
			break;

		default:
			break;
	}

	return (HTML_EVENT_STATUS_CONTINUE);
}

void WebEditString::DrawThisOnly (DISPLAY_INT x, DISPLAY_INT y, WebGraphics *gc)
{
	WebRect box;
	GetFrameRect(&box);
	box.Shift(x,y);

//	gc->StartBuffer();                      // begin buffering graphics commands
	gc->Rectangle(&box, GetBgColor(gc), GetBgColor(gc), 1);	// draw background
	DrawFrame(&box, gc);

	// save the current graphics context clip rectangle and set clipping to the
	//  text display region of the widget
	GetTextRect(&box);
	box.Shift(x,y);
	WebRect clipSave;
	gc->GetClip(&clipSave);
	if (clipSave.Overlaps(&box))
	{
		WebRect clip(box);
		clip.And(&clipSave);
		gc->SetClip(&clip);

		// miXOffset is set in this method (allows text to scroll in the window)
		EnsureCursorVisible();

		// render our text
		WebColor c;
		c = GetTextColor(gc);

		WebFont font = mFont.GetFont();
		if (mpText && font)
		{
			// if we have the focus, draw the cursor
			if ((mFlags & DISPLAY_FLAG_FOCUS) && !(mFlags & DISPLAY_FLAG_DISABLED))
			{
				// now render the selected portion in reverse video (if we have one)
				if (mEditFlags & EDIT_FLAG_HAS_SELECTION)
				{
					long selStart, selEnd;

					selStart = EBSMIN(miCursorPos, miSelectBegin);
					selEnd   = EBSMAX(miCursorPos, miSelectBegin);

					DISPLAY_INT selectLeft = box.left + PixelOffset(selStart) - miXOffset;
					DISPLAY_INT selectRight = box.left + PixelOffset(selEnd) - miXOffset;

					DrawText(gc, box.left - miXOffset, box.top, mpText, c, 0, 0, font, selStart);
					DrawText(gc, selectLeft, box.top, mpText + selStart, GetBgColor(gc), GetSelectColor(gc), 1, font, selEnd - selStart);
					DrawText(gc, selectRight, box.top, mpText + selEnd, c, 0, 0, font);
				}
				else
				{
					DrawText(gc, box.left - miXOffset, box.top, mpText, c, 0, 0, font);

					// Draw the cursor
					DISPLAY_INT cursorX = box.left + PixelOffset(miCursorPos) - miXOffset;
					box.Set(cursorX, box.top, cursorX+WEBC_CFG_STR_CURSOR_WIDTH-1, box.top + WEB_FONT_HEIGHT(font));
					gc->Rectangle(&box, GetSelectColor(gc), GetSelectColor(gc), 1);
				}
			}
			else
			{
				DrawText(gc, box.left - miXOffset, box.top, mpText, c, 0, 0, font);
			}
		}

		gc->SetClip(&clipSave);	 // restore the clip rectangle
	}
//	gc->EndBuffer();	     // send all buffered commands to the display
}

void WebEditString::EnsureCursorVisible (void)
{
	if (mFlags & DISPLAY_FLAG_FOCUS)
	{
		int pos;
		WebRect box;

		if (mEditFlags & EDIT_FLAG_HAS_SELECTION)
		{
			pos = EBSMIN(miCursorPos, miSelectBegin);
		}
		else
		{
			pos = miCursorPos;
		}

		DISPLAY_INT cursorX = PixelOffset(pos) - miXOffset;

		GetTextRect(&box);

		if (cursorX > (box.Width() - 1))
		{
			miXOffset += cursorX - (box.Width() - 1);
		}
		else if (cursorX < 0)
		{
			miXOffset += cursorX;
		}
	}
}

DISPLAY_INT WebEditString::PixelOffset (long index)
{
	WebGraphics *gc = GetGraphics();
	return (gc? gc->TextWidthLen(mpText, mFont.GetFont(), index) : 0);
}

void WebEditString::DrawText(WebGraphics *gc, DISPLAY_INT x, DISPLAY_INT y,
                             const WebChar *text, WebColor textColor,
                             WebColor fillColor, WEBC_BOOL fill, WebFont font, int len)
{
	// Filter out characters that won't draw correctly
	WEBC_BOOL bMalloced = 0;
	if (len < 0) len = webc_strlen(text);
	WebChar pStackStr[200];
	WebChar *pStr, *to;

	if (len > 0)
	{
		if (len < 200)
		{
			pStr = pStackStr;
		}
		else
		{
			bMalloced = 1;
			pStr = (WebChar *) WEBC_MALLOC(sizeof(WebChar) * (len+1));
		}

		if (pStr)
		{
			to = pStr;
			while (*text)
			{
				if ((*text != (WebChar) '\r') && (*text != (WebChar) '\n') && (*text != (WebChar) '\t'))
				{
					*(to++) = *text;
				}
				else
				{
					len--;
				}
				text++;
			}
			*to = (WebChar) 0;

			text = pStr;
		}

		gc->TextLen(x, y, text, textColor, fillColor, fill, font, len);

		if (bMalloced)
		{
			WEBC_FREE(pStr);
		}
	}
}


void WebEditString::SetText (const WebChar *text)
{
WebChar _null[1] = {(WebChar) 0};
long diff = 0, newLen;

	if (!text)
		text = _null;

	long len = webc_strlen(text);

	if (miTextSize <= len)
	{
		SetMaxLength((len + 0x20) & (~0x1f), 0);
	}

	if (mpText)
	{
		webc_strncpy(mpText, text, miTextSize);
		newLen = EBSMIN(len, miTextSize);
		diff = newLen - miTextLen;
		miTextLen = newLen;
		Invalidate();
		WEBC_ASSERT(miTextLen >= 0);
	}

	if (!(mFlags & (DISPLAY_FLAG_DISABLED | DISPLAY_FLAG_READONLY)))
	{
		SetCursorPosition(miCursorPos + diff);
	}
}

void WebEditString::SetFont (WebGraphFont *font)
{
	mFont.Set(font);
}

WebChar *WebEditString::GetText (void)
{
	return (mpText);
}

void WebEditString::SetMaxLength (long newLen, WEBC_BOOL saveText)
{
	WebChar *pNew = (WebChar *) WEBC_MALLOC((newLen + 1) * sizeof(WebChar));
	if (!pNew)
	{
		return;
	}
	miTextSize = newLen + 1;
	pNew[0] = 0;
	if (mpText)
	{
		if (saveText)
		{
			webc_strncpy(pNew, mpText, newLen);
			pNew[newLen] = 0;
		}
		WEBC_FREE(mpText);
	}
	mpText = pNew;
}

long WebEditString::GetMaxLength (void)
{
	return (miTextSize - 1);
}

void WebEditString::SelectAll (void)
{
	EndSelect();
	Home();
	BeginSelect();
	End();
	EndSelect();
}

void WebEditString::Home (void)
{
	SetCursorPosition(0);
}

void WebEditString::End (void)
{
	SetCursorPosition(miTextLen);
}

void WebEditString::Delete (void)
{
	if (!(mFlags & DISPLAY_FLAG_READONLY))
	{
		if (mEditFlags & EDIT_FLAG_HAS_SELECTION)
		{
			DeleteChars(EBSMIN(miCursorPos, miSelectBegin), EBSMAX(miCursorPos, miSelectBegin));
			miCursorPos = EBSMIN(miSelectBegin, miCursorPos);
			miSelectBegin = miCursorPos;
			mEditFlags &= ~(EDIT_FLAG_HAS_SELECTION|EDIT_FLAG_SELECTING);
		}
		else
		{
			DeleteChars(miCursorPos, miCursorPos + 1);
		}
	}
}

void WebEditString::Backspace (void)
{
	if (mEditFlags & EDIT_FLAG_HAS_SELECTION)
	{
		Delete();
	}
	else
	{
		if (miCursorPos > 0)
		{
			miCursorPos--;
			Delete();
		}
	}
}

void WebEditString::WriteModeToggle (void)
{
	mEditFlags ^= EDIT_FLAG_OVERWRITE;
}

void WebEditString::Done (void)
{
}

void WebEditString::SetCursorPosition (long index)
{
	Invalidate();
	miCursorPos = EBSCLIP(index, 0, miTextLen);
	if ((mEditFlags & EDIT_FLAG_SELECTING) && (miSelectBegin != miCursorPos))
	{
		mEditFlags |= EDIT_FLAG_HAS_SELECTION;
	}
	else
	{
		mEditFlags &= ~EDIT_FLAG_HAS_SELECTION;
	}
}

void WebEditString::BeginSelect (void)
{
	if (!(mEditFlags & EDIT_FLAG_SELECTING))
	{
		mEditFlags |= EDIT_FLAG_SELECTING;
		if (!(mEditFlags & EDIT_FLAG_HAS_SELECTION))
		{
			miSelectBegin = miCursorPos;
		}
	}
}

void WebEditString::EndSelect (void)
{
	mEditFlags &= ~EDIT_FLAG_SELECTING;
}

void WebEditString::Cut (void)
{
	if (mEditFlags & EDIT_FLAG_HAS_SELECTION)
	{
		Copy();
		Delete();
	}
}

void WebEditString::Copy (void)
{
}

void WebEditString::Paste (void)
{
}

void WebEditString::InsertChar (WebChar c)
{
	if (!(mFlags & DISPLAY_FLAG_READONLY))
	{
		if (mEditFlags & EDIT_FLAG_HAS_SELECTION)
		{
			Delete();
		}

		if (mEditFlags & EDIT_FLAG_NO_SIZE_LIMIT)
		{
			if ((miTextLen + 1) >= miTextSize)
			{
				// re-alloc the buffer to accomodate the new character
				SetMaxLength((miTextLen + 0x21) & (~0x1f), 1);
			}
		}

		if ((miTextLen + 1) < miTextSize)
		{
			if (!(mEditFlags & EDIT_FLAG_OVERWRITE))
			{
				// shift it right one char
				rtp_memmove(mpText + miCursorPos + 1, mpText + miCursorPos, (miTextLen + 1 - miCursorPos)*sizeof(WebChar));
			}
			mpText[miCursorPos++] = c;
			miTextLen++;
		}

		WEBC_ASSERT(miTextLen >= 0);

		Invalidate();
	}
}

void WebEditString::DeleteChars(long begin, long end)
{
	if (!(mFlags & DISPLAY_FLAG_READONLY))
	{
		begin = EBSMAX(0, begin);
		end = EBSMIN(miTextLen, end);

		if (end > begin)
		{
			Invalidate();
			rtp_memmove(mpText + begin, mpText + end, (miTextLen + 1 - end) * sizeof(WebChar));
			miTextLen -= (end-begin);
			WEBC_ASSERT(miTextLen >= 0);
		}
	}
}

long WebEditString::GetIndexAtXY(DISPLAY_INT x, DISPLAY_INT y)
{
	WebGraphics *gc = GetGraphics();

	if (!gc)
	{
		return 0;
	}

	DISPLAY_INT displayX = mRect.left, displayY = mRect.top;
	WebRect box;

	GetTextRect(&box);

	if (mpParent)
	{
		mpParent->GetDisplayPosition(this, &displayX, &displayY);
	}
	return (gc? gc->GetCharOffset(x - displayX - box.left + miXOffset, mpText, mFont.GetFont()) : 0);
}

void WebEditString::SetListener(Listener *l)
{
	mpListener = l;
}

void WebEditString::GetFrameRect(WebRect *rect)
{
	rect->Set(EDITSTR_MARGIN,
	          EDITSTR_MARGIN,
	          mRect.Width() - 1  - EDITSTR_MARGIN,
	          mRect.Height() - 1 - EDITSTR_MARGIN);
}

void WebEditString::GetTextRect(WebRect *rect)
{
	rect->Set((EDITSTR_MARGIN+EDITSTR_BORDER+EDITSTR_PADDING),
	          (EDITSTR_MARGIN+EDITSTR_BORDER+EDITSTR_PADDING),
	          mRect.Width() - 1  - (EDITSTR_MARGIN+EDITSTR_BORDER+EDITSTR_PADDING),
	          mRect.Height() - 1 - (EDITSTR_MARGIN+EDITSTR_BORDER+EDITSTR_PADDING));
}

void WebEditString::DrawFrame(WebRect *box, WebGraphics *gc)
{
WebColor hilite = gc->LightColor(gc->DarkColor(GetBgColor(gc)));
WebColor lolite = gc->DarkColor(hilite);

	gc->Frame3D(box, lolite, hilite, EDITSTR_BORDER);     // draw a recessed frame
}

void WebEditString::EnableSizeLimit(void)
{
	mEditFlags &= ~EDIT_FLAG_NO_SIZE_LIMIT;
}

void WebEditString::DisableSizeLimit(void)
{
	mEditFlags |= EDIT_FLAG_NO_SIZE_LIMIT;
}

void WebEditString::SetWrap(WEBC_BOOL on)
{
	if (on)
	{
		mEditFlags |= EDIT_FLAG_WRAP;
	}
	else
	{
		mEditFlags &= ~EDIT_FLAG_WRAP;
	}
}


WebColor WebEditString::GetTextColor   (WebGraphics* gc)
{
	return gc->GetColorByIndex(WGC_BLACK);
}

WebColor WebEditString::GetBgColor     (WebGraphics* gc)
{
	return gc->GetColorByIndex(WGC_WHITE);
}

WebColor WebEditString::GetSelectColor (WebGraphics* gc)
{
	return gc->GetColorByIndex(WGC_LIGHTGRAY);
}
