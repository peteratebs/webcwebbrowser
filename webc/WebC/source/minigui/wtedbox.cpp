//
// WTEDBOX.CPP -
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
#include "wtedbox.hpp"
#include "util.h"
#include "wgkeys.hpp"
#include "objref.cpp"
#include "webcmem.h"


//MA
#include "rtpprint.h"

/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/

#define EDITBOX_MARGIN         0
#define EDITBOX_BORDER         WG_CFG_DEFAULT_FRAME_WIDTH
#define EDITBOX_PADDING        2

#define TOTAL_LEFT_PAD         (EDITBOX_MARGIN+EDITBOX_BORDER+EDITBOX_PADDING)
#define TOTAL_RIGHT_PAD        TOTAL_LEFT_PAD
#define TOTAL_TOP_PAD          TOTAL_LEFT_PAD
#define TOTAL_BOTTOM_PAD       TOTAL_LEFT_PAD


//MA
#define TEXT_BOX_SCROLL_DISABLED 0

/*****************************************************************************/
// Local Types
/*****************************************************************************/


/*****************************************************************************/
// Global Data
/*****************************************************************************/


/*****************************************************************************/
// Local functions
/*****************************************************************************/

WebEditBox::WebEditBox ()
{
	miOffsetArraySize = 0;
	miNumLines = 0;
	miCurrentLine = 0;
	mpLineOffsets = 0;
	miYOffset = 0;
	mpVScroll = 0;
	mpHScroll = 0;
	mScrollMode = SCROLL_MODE_AUTO;
	mEditFlags = EDIT_FLAG_WRAP;
	SetLineOffset(0,0);
}

WebEditBox::~WebEditBox ()
{
	if (mpLineOffsets)
	{
		WEBC_FREE(mpLineOffsets);
	}

	if (mpVScroll)
	{
		Remove(mpVScroll);
		WEBC_DELETE(mpVScroll);
	}

	if (mpHScroll)
	{
		Remove(mpHScroll);
		WEBC_DELETE(mpHScroll);
	}
}
//HTMLEventStatus WebEditBox::PopUpSoftKeyPad(void)
//{
//	return HTML_EVENT_STATUS_CONTINUE;
//	return HTML_EVENT_STATUS_CONTINUE;
//}
#if (WEBC_SUPPORT_TOUCHSCREEN)
void PopUpKeyPad(HTMLBrowser *pBrowser, DISPLAY_INT numlines, void *pEl, WEBC_CHAR *pValue, void (*closeFn)(void *El, WEBC_CHAR *p));

void EditBoxKeyboardcloseFn(void *El, WEBC_CHAR *p)
{
	 WebEditBox *wEB = ( WebEditBox *) El;
	// Copy the contents to the application window from the softkey input window
	if (wEB && p)
		wEB->SetText(p);
}
#endif

HTMLEventStatus WebEditBox::Event (HTMLEvent *e)
{
    WEBC_DEBUG_ReceiveEvent(this, e);
	switch (e->type)
	{
#if (WEBC_SUPPORT_TOUCHSCREEN)
		case HTML_EVENT_MOUSEDOWN:
			if (!(mFlags & DISPLAY_FLAG_DISABLED))
			{
				PopUpKeyPad((HTMLBrowser *)webc_GetBrowserWithFocus(), this->miNumLines, (void *) this, GetText(), EditBoxKeyboardcloseFn);
				return (HTML_EVENT_STATUS_DONE);
			}

//		case HTML_EVENT_MOUSEDOWN:
//		GetText();
#endif
		case HTML_EVENT_KEYDOWN:
			if (!(mFlags & DISPLAY_FLAG_DISABLED))
			{
				switch (e->data.key)
				{
					case WGK_PGUP:
					case WGK_PGDN:
					case WGK_HOME:
					case WGK_END:
					case WGK_LNUP:
					case WGK_LNDN:
					{
						WEBC_BOOL selecting = 0;
						if (e->flags & HTML_EVENT_FLAG_SHIFT_DOWN)
						{
							selecting = 1;
							BeginSelect();
						}
						switch (e->data.key)
						{
							case WGK_PGUP:
								break;

							case WGK_PGDN:
								break;

							case WGK_HOME:
								if (e->flags & HTML_EVENT_FLAG_CTRL_DOWN)
								{
									Home();
								}
								else
								{
									LineHome();
								}
								break;

							case WGK_END:
								if (e->flags & HTML_EVENT_FLAG_CTRL_DOWN)
								{
									End();
								}
								else
								{
									LineEnd();
								}
								break;

							case WGK_LNUP:
								LineUp();
								break;

							case WGK_LNDN:
								LineDown();
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

					case WGK_CR:
						InsertChar((WebChar) '\n');
						break;

					default:
						WebEditString::Event(e);
						break;
				}
			}
			break;

		default:
			WebEditString::Event(e);
			break;
	}

	return (HTML_EVENT_STATUS_CONTINUE);
}

WEBC_BOOL WebEditBox::GetFocusRect (WebRect* rect)
{
	WebGraphics *gc = GetGraphics();
	WebFont font = mFont.GetFont();

	if (gc && font && mpText && (mEditFlags & EDIT_FLAG_ENSURE_CURSOR_VISIBLE))
	{
		EnsureCursorVisible();

		DISPLAY_INT cursorX = gc->TextWidthLen(mpText + GetLineOffset(miCurrentLine), font, miCursorPos - GetLineOffset(miCurrentLine)) - miXOffset;
		DISPLAY_INT cursorY = WEB_FONT_HEIGHT(font)*miCurrentLine - miYOffset;

		GetTextRect(rect);
		rect->Shift(cursorX, cursorY);
		rect->SizeTo(5, WEB_FONT_HEIGHT(font));

		return WEBC_TRUE;
	}

	return WEBC_FALSE;
}

void WebEditBox::DrawThisOnly (DISPLAY_INT x, DISPLAY_INT y, WebGraphics *gc)
{
	WebChar c;
	long line;

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

		miXOffset = (mpHScroll)? mpHScroll->GetPosition() : 0;
		miYOffset = (mpVScroll)? mpVScroll->GetPosition() : 0;

		// render our text
		WebFont font = mFont.GetFont();
		if (mpText && font)
		{
			// this loop draws up to the last line
			for (line=0; line<(miNumLines-1); line++)
			{
				c = mpText[GetLineOffset(line+1)];
				mpText[GetLineOffset(line+1)] = 0;
				DrawText(gc, box.left - miXOffset, box.top - miYOffset + (line * WEB_FONT_HEIGHT(font)),
				         mpText + GetLineOffset(line), GetTextColor(gc), 0, 0, font);
				mpText[GetLineOffset(line+1)] = c;
			}

			// now draw the last line of text.
			DrawText(gc, box.left - miXOffset, box.top - miYOffset + (line*WEB_FONT_HEIGHT(font)),
			         mpText + GetLineOffset(line), GetTextColor(gc), 0, 0, font);

			// if we have the focus, draw the cursor
			if ((mFlags & DISPLAY_FLAG_FOCUS) && !(mFlags & DISPLAY_FLAG_DISABLED))
			{
				// now render the selected portion in reverse video (if we have one)
				if (mEditFlags & EDIT_FLAG_HAS_SELECTION)
				{
					long begin = EBSMIN(miCursorPos, miSelectBegin);
					long end = EBSMAX(miCursorPos, miSelectBegin);
					long beginLine = FindLine(begin), endLine = FindLine(end);
					DISPLAY_INT textLeft;
					long currentBegin, currentEnd;

					for (line=beginLine; line<=endLine; line++)
					{
						currentBegin = EBSMAX(begin, GetLineOffset(line));
						if (line == endLine)
						{
							currentEnd = end;
						}
						else
						{
							currentEnd = EBSMIN(end, GetLineOffset(line+1));
						}
						textLeft = gc->TextWidthLen(mpText + GetLineOffset(line), font, EBSMAX(0, begin - GetLineOffset(line)));

						c = mpText[currentEnd];
						mpText[currentEnd] = 0;
						DrawText(gc, box.left - miXOffset + textLeft,
						             box.top - miYOffset + (line*WEB_FONT_HEIGHT(font)),
						             mpText + currentBegin, GetBgColor(gc), GetSelectColor(gc), 1, font);
						mpText[currentEnd] = c;
					}
				}

				DISPLAY_INT cursorX = box.left - miXOffset +
					gc->TextWidthLen(mpText + GetLineOffset(miCurrentLine), font, miCursorPos - GetLineOffset(miCurrentLine)) ;
				box.Set(cursorX, box.top - miYOffset + WEB_FONT_HEIGHT(font)*miCurrentLine,
				        cursorX, box.top - miYOffset + WEB_FONT_HEIGHT(font)*(miCurrentLine+1));
				gc->Rectangle(&box, GetSelectColor(gc), GetSelectColor(gc), 1);
			}

		}

		gc->SetClip(&clipSave);	 // restore the clip rectangle
	} // if clip overlaps

/*	if (mpVScroll && mpHScroll)
	{
		GetCornerRect(&box);
		box.Shift(x,y);
		gc->Rectangle(&box, LIGHTGRAY, LIGHTGRAY, 1);
	}*/

//	gc->EndBuffer();	     // send all buffered commands to the display

}

void WebEditBox::Move (WebRect *pRect)
{
	WebEditString::Move(pRect);
	Format();
}

void WebEditBox::SizeTo (DISPLAY_INT w, DISPLAY_INT h)
{
	WebEditString::SizeTo(w,h);
	Format();
}

void WebEditBox::SetHeight (DISPLAY_INT h)
{
	WebEditString::SetHeight(h);
	Format();
}

void WebEditBox::SetWidth (DISPLAY_INT w)
{
	WebEditString::SetWidth(w);
	Format();
}


void WebEditBox::DeleteChars(long begin, long end)
{
	if (!(mFlags & DISPLAY_FLAG_READONLY))
	{
		WebEditString::DeleteChars(begin, end);
		Format();
		mEditFlags |= EDIT_FLAG_ENSURE_CURSOR_VISIBLE;
	}
}

void WebEditBox::InsertChar (WebChar c)
{
	if (!(mFlags & DISPLAY_FLAG_READONLY))
	{
		WebEditString::InsertChar(c);
		Format();
		mEditFlags |= EDIT_FLAG_ENSURE_CURSOR_VISIBLE;
	}
}

void WebEditBox::SetCursorPosition (long index)
{
	WebEditString::SetCursorPosition(index);
	miCurrentLine = FindLine(miCursorPos);
	mEditFlags |= EDIT_FLAG_ENSURE_CURSOR_VISIBLE;
}

void WebEditBox::EnsureCursorVisible (void)
{
	WebGraphics *gc = GetGraphics();
	WebFont font = mFont.GetFont();

	if (gc && font && mpText)
	{
		DISPLAY_INT cursorX = gc->TextWidthLen(mpText + GetLineOffset(miCurrentLine), font, miCursorPos - GetLineOffset(miCurrentLine)) - miXOffset;
		DISPLAY_INT cursorY = WEB_FONT_HEIGHT(font)*miCurrentLine - miYOffset;
		WebRect box;

		GetTextRect(&box);

		cursorX += box.left;
		cursorY += box.top;

		if (cursorY < box.top)
		{
			miYOffset -= (box.top - cursorY);
		}
		else if ((cursorY + WEB_FONT_HEIGHT(font)) > box.bottom)
		{
			miYOffset += (cursorY + WEB_FONT_HEIGHT(font)) - box.bottom;
		}

		if (cursorX > box.right)
		{
			// overshoot by a bit
			miXOffset += cursorX - box.right + 10;
		}
		else if (cursorX < box.left)
		{
			miXOffset -= (box.left - cursorX);
		}

		if (mpVScroll)
		{
			mpVScroll->SetPosition(miYOffset);
		}

		if (mpHScroll)
		{
			mpHScroll->SetPosition(miXOffset);
		}

		mEditFlags &= ~EDIT_FLAG_ENSURE_CURSOR_VISIBLE;
	}
}

void WebEditBox::LineUp (void)
{
	if (miCurrentLine == 0)
	{
		return;
	}

	long iColumn = miCursorPos - GetLineOffset(miCurrentLine);
	long iIndex = iColumn + GetLineOffset(miCurrentLine - 1);

	if (iIndex >= GetLineOffset(miCurrentLine))
	{
		iIndex = GetLineOffset(miCurrentLine) - 1;
	}

	SetCursorPosition(iIndex);
}

void WebEditBox::LineDown (void)
{
	if (miCurrentLine == miNumLines - 1)
	{
		return;
	}

	long iColumn = miCursorPos - GetLineOffset(miCurrentLine);
	long iIndex = iColumn + GetLineOffset(miCurrentLine + 1);

	if (miCurrentLine < miNumLines - 2)
	{
		if (iIndex >= GetLineOffset(miCurrentLine + 2))
		{
			iIndex = GetLineOffset(miCurrentLine + 2) - 1;
		}
	}

	SetCursorPosition(iIndex);
}

void WebEditBox::LineHome (void)
{
	SetCursorPosition(GetLineOffset(miCurrentLine));
}

void WebEditBox::LineEnd (void)
{
	if (miCurrentLine == (miNumLines - 1))
	{
		End();
	}
	else
	{
		SetCursorPosition(GetLineOffset(miCurrentLine+1)-1);
	}
}

void WebEditBox::PageUp (void)
{
}

void WebEditBox::PageDown (void)
{
}

void WebEditBox::ScrollUp (void)
{
}

void WebEditBox::ScrollDown (void)
{
}

void WebEditBox::ScrollLeft (void)
{
}

void WebEditBox::ScrollRight (void)
{
}

void WebEditBox::ScrollToX (DISPLAY_INT x)
{
}

void WebEditBox::ScrollToY (DISPLAY_INT y)
{
}

void WebEditBox::Format (void)
{
	WebGraphics *gc = GetGraphics();
	WebFont font = mFont.GetFont();

	if (!gc || !mpText || !font)
	{
		return;
	}

	int i = 0;
	do
	{
		long iIndex = 0;
		long iNextWord;
		DISPLAY_INT iWidthUsed = 0;
		WebRect box;
		DISPLAY_INT iWordWidth;

		GetTextRect(&box);
		DISPLAY_INT iWidthAvailable = box.Width();
//printf("Box w == %d\n",iWidthAvailable);
		miMaxLineWidth = 0;
		miNumLines = 1;
		while (mpText[iIndex])
		{
			switch (mpText[iIndex])
			{
				case (WebChar) '\r':
				case (WebChar) '\n':
					// break the line at CR/LF
					AddLine(++iIndex);
					iWidthUsed = 0;
					break;

				default:
					if (!(mEditFlags & EDIT_FLAG_WRAP))
					{
						iWordWidth = gc->TextWidthLen(&mpText[iIndex], font, 1);
						iWidthUsed += iWordWidth;
						miMaxLineWidth = EBSMAX(miMaxLineWidth, iWidthUsed);
						iIndex++;
					}
					else
					{

#if (TEXT_BOX_SCROLL_DISABLED)
					WebChar mpText1='m';
					DISPLAY_INT charWidth = gc->TextWidthLen(&mpText[iIndex], font, 1);
					if (iWidthUsed + charWidth > (iWidthAvailable) && iWidthUsed > 0)
					{
						AddLine(iIndex);
						iWidthUsed = charWidth;
						miMaxLineWidth = EBSMAX(miMaxLineWidth, charWidth);
						iIndex++;
					}
					else
					{
						iIndex++;
						iWidthUsed += charWidth;
					}
				}//switch statement
#else
						if (IS_WHITESPACE(mpText[iIndex]))
						{

							iWidthUsed += gc->TextWidthLen(&mpText[iIndex++], font, 1);
						}
						else
						{

							iNextWord = iIndex;
							while (!IS_WHITESPACE(mpText[iNextWord]) && mpText[iNextWord])
							{
								iNextWord++;
							}
							iWordWidth = gc->TextWidthLen(&mpText[iIndex], font, iNextWord-iIndex);
							// only do a line break if there is something already on the line
							if (iWidthUsed + iWordWidth > iWidthAvailable && iWidthUsed > 0)
							{
								// the next word_ will not fit; break the line.
								AddLine(iIndex);
								iWidthUsed = iWordWidth;
								miMaxLineWidth = EBSMAX(miMaxLineWidth, iWordWidth);
								iIndex = iNextWord;
							}
							else
							{
								// the next word_ will fit on this line
								iWidthUsed += iWordWidth;
								miMaxLineWidth = EBSMAX(miMaxLineWidth, iWordWidth);
								iIndex = iNextWord;
							}
						}
					}
#endif
					break;
			}
		}

		i++;
	}
	while (UpdateScrollBars() && (i < 4));

	miCurrentLine = FindLine(miCursorPos);
}

WEBC_BOOL WebEditBox::UpdateScrollBars(void)
{
	WebRect box;
	WEBC_BOOL done = WEBC_FALSE;
	WebGraphics *gc = GetGraphics();
	WebFont font = mFont.GetFont();
	int i = 0;

	if (!gc || !font)
	{
		return WEBC_FALSE;
	}

	DISPLAY_INT totalHeight = miNumLines * WEB_FONT_HEIGHT(font);

	do
	{
		done = WEBC_TRUE;
		GetTextRect(&box);
		SCROLL_MODE ScrollMode;
		ScrollMode = GetScrollMode();

		// Add/Remove scroll bars
		if (((totalHeight > box.Height()) && (ScrollMode & SCROLL_MODE_AUTO)) ||
		    (ScrollMode & SCROLL_MODE_VSCROLL))
		{
			if (!mpVScroll)
			{
				WEBC_NEW_VERBOSE(mpVScroll, WebVScroll(),"WebEditBox:WebVscroll");
				if (mpVScroll)
				{
					InsertLast(mpVScroll);
					mpVScroll->SetListener(this);
					mpVScroll->SetWidth(webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_WIDTH));
					mpVScroll->SetPosition(0);
					mpVScroll->SetStep(WEB_FONT_HEIGHT(font));
					if (mFlags & DISPLAY_FLAG_DISABLED)
					{
						mpVScroll->Disable();
					}
				}
				return WEBC_TRUE;
			}
		}
		else
		{
			if (mpVScroll) // and we don't need it
			{
				Remove(mpVScroll);
				WEBC_DELETE(mpVScroll);
				mpVScroll = 0;
				return WEBC_TRUE;
			}
		}


		// Test to see if we need a horizontal scroll
		if (((miMaxLineWidth > box.Width()) && (ScrollMode & SCROLL_MODE_AUTO)) ||
		    (ScrollMode & SCROLL_MODE_HSCROLL))
		{
			if (!mpHScroll)
			{
				WEBC_NEW_VERBOSE(mpHScroll, WebHScroll(),"WebEditBox:WebHscroll");
				if (mpHScroll)
				{
					InsertLast(mpHScroll);
					mpHScroll->SetListener(this);
					mpHScroll->SetHeight(webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_WIDTH));
					mpHScroll->SetPosition(0);
					mpHScroll->SetStep(32);
					if (mFlags & DISPLAY_FLAG_DISABLED)
					{
						mpHScroll->Disable();
					}
					done = WEBC_FALSE;
				}
			}
		}
		else
		{
			if (mpHScroll)
			{
				Remove(mpHScroll);
				WEBC_DELETE(mpHScroll);
				mpHScroll = 0;
				done = WEBC_FALSE;
			}
		}

		i++;
	} while (!done && (i<4));

	GetTextRect(&box);

	if (mpVScroll)
	{
		PresetWebRect VRect (
				box.right + 1,
				box.top,
				box.right + webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_WIDTH),
				box.bottom);

		mpVScroll->Move(&VRect);
		mpVScroll->SetRange(totalHeight);
		mpVScroll->SetWindow(box.Height());
	}

	if (mpHScroll)
	{
		PresetWebRect HRect (
				box.left,
				box.bottom + 1,
				box.right,
				box.bottom + webc_GetDefaultDisplayInt(WEBC_DEFAULT_SLIDER_WIDTH));

		mpHScroll->Move(&HRect);
		mpHScroll->SetRange(miMaxLineWidth + 10);
		mpHScroll->SetWindow(box.Width());
	}

	return (WEBC_FALSE);
}


int WebEditBox::AddLine(long iBegin)
{
	if (miNumLines >= miOffsetArraySize)
	{
		ReallocLines((miNumLines + 0x20) & ~0x1f);
		if (miNumLines >= miOffsetArraySize)
		{
			// malloc error
			return (-1);
		}
	}
	SetLineOffset(miNumLines++, iBegin);

	return (0);
}

void WebEditBox::ReallocLines (long iNewSize)
{
long *pNew = (long *) WEBC_MALLOC(iNewSize*sizeof(long));

	if (pNew)
	{
		if (mpLineOffsets)
		{
			tc_movebytes(pNew, mpLineOffsets, EBSMIN(iNewSize, miOffsetArraySize)*sizeof(long));
			WEBC_FREE(mpLineOffsets);
		}
		miOffsetArraySize = iNewSize;
		mpLineOffsets = pNew;
	}
}

long WebEditBox::FindLine (long index)
{
long line;

	for (line=1; line<miNumLines; line++)
	{
		if (GetLineOffset(line) > index)
		{
			break;
		}
	}

	return (line - 1);
}

long WebEditBox::GetIndexAtXY(DISPLAY_INT x, DISPLAY_INT y)
{
	WebGraphics *gc = GetGraphics();
	WebFont font = mFont.GetFont();
	if (gc && font && mpText)
	{
		DISPLAY_INT displayX = mRect.left, displayY = mRect.top;
		if (mpParent)
		{
			mpParent->GetDisplayPosition(this, &displayX, &displayY);
		}

		WebRect box;
		GetTextRect(&box);

		x -= displayX + box.left - miXOffset;
		y -= displayY + box.top - miYOffset;

		long line = EBSCLIP(y / WEB_FONT_HEIGHT(mFont.GetFont()), 0, miNumLines-1);
		long column = gc->GetCharOffset(x, mpText + GetLineOffset(line), mFont.GetFont());

		if (line < miNumLines-1)
		{
			if ((column + GetLineOffset(line)) >= GetLineOffset(line+1))
			{
				return (GetLineOffset(line+1) - 1);
			}
		}

		return (EBSMIN(column + GetLineOffset(line), miTextLen));
	}

	return (0);
}

void WebEditBox::Notify (Notifier *pWho, NotifyEvent *what)
{
    WEBC_DEBUG_ReceiveNotify(this, pWho, what);
	if ((mpVScroll && (pWho == mpVScroll)) || (mpHScroll && (pWho == mpHScroll)))
	{
		SetFlag(DISPLAY_FLAG_DIRTY);
		Invalidate();
	}
}

void WebEditBox::GetFrameRect(WebRect *rect)
{
	rect->Set(EDITBOX_MARGIN,
	          EDITBOX_MARGIN,
	          mRect.Width() - 1  - EDITBOX_MARGIN,
	          mRect.Height() - 1 - EDITBOX_MARGIN);
}

void WebEditBox::GetTextRect(WebRect *rect)
{
	rect->Set((EDITBOX_MARGIN+EDITBOX_BORDER+EDITBOX_PADDING),
	          (EDITBOX_MARGIN+EDITBOX_BORDER+EDITBOX_PADDING),
	          mRect.Width() - 1  - (EDITBOX_MARGIN+EDITBOX_BORDER+EDITBOX_PADDING)
	              - ((mpVScroll)? mpVScroll->Width() : 0),
	          mRect.Height() - 1 - (EDITBOX_MARGIN+EDITBOX_BORDER+EDITBOX_PADDING)
	              - ((mpHScroll)? mpHScroll->Height() : 0));
}

void WebEditBox::GetCornerRect(WebRect *box)
{
	box->Set(Width()  - EDITBOX_BORDER - EDITBOX_MARGIN - (mpVScroll? mpVScroll->Width() : 0),
	         Height() - EDITBOX_BORDER - EDITBOX_MARGIN - (mpHScroll? mpHScroll->Height() : 0),
	         Width()  - EDITBOX_BORDER - EDITBOX_MARGIN - 1,
	         Height() - EDITBOX_BORDER - 1);

}

void WebEditBox::DrawFrame(WebRect *box, WebGraphics *gc)
{
WebColor hilite = gc->LightColor(gc->DarkColor(GetBgColor(gc)));
WebColor lolite = gc->DarkColor(hilite);

	gc->Frame3D(box, lolite, hilite, EDITBOX_BORDER);     // draw a recessed frame
}

WebChar *WebEditBox::GetLineText (long line)
{
	return (mpText + GetLineOffset(EBSCLIP(line, 0, miNumLines-1)));
}

SCROLL_MODE WebEditBox::GetScrollMode (void)
{
//printf("Force no scroll\n");
	//return (mScrollMode);
#if (TEXT_BOX_SCROLL_DISABLED)
	return(SCROLL_MODE_NONE);
#else
	return (mScrollMode);
#endif
}

void WebEditBox::SetScrollMode (SCROLL_MODE m)
{
	mScrollMode = m;
}

void WebEditBox::Disable (void)
{
	DisplayElement::Disable();

	if (mpVScroll)
	{
		mpVScroll->Disable();
	}

	if (mpHScroll)
	{
		mpHScroll->Disable();
	}
}

void WebEditBox::Enable (void)
{
	DisplayElement::Enable();

	if (mpVScroll)
	{
		mpVScroll->Enable();
	}

	if (mpHScroll)
	{
		mpHScroll->Enable();
	}
}

long WebEditBox::GetLineOffset(long iLine)
{
	if ((iLine >= 0) && (iLine < miOffsetArraySize))
	{
		return (mpLineOffsets[iLine]);
	}

	return (0);
}

void WebEditBox::SetLineOffset(long iLine, long iOffset)
{
	if (iLine < 0)
	{
		return;
	}

	if (iLine >= miOffsetArraySize)
	{
		ReallocLines((iLine + 0x20) & ~0x1f);
	}

	if (iLine < miOffsetArraySize)
	{
		mpLineOffsets[iLine] = iOffset;
	}
}

void WebEditBox::SetText (const WebChar *label)
{
	WebEditString::SetText(label);
	Format();
}

void WebEditBox::SetFont (WebGraphFont *font)
{
	WebEditString::SetFont(font);
	Format();
}
