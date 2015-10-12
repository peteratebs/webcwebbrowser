#ifndef __WTEDSTR_HPP__
#define __WTEDSTR_HPP__

#include "delement.hpp"

#define EDIT_FLAG_OVERWRITE              0x01
#define EDIT_FLAG_SELECTING              0x02
#define EDIT_FLAG_HAS_SELECTION          0x04
#define EDIT_FLAG_MOUSEDOWN              0x08
#define EDIT_FLAG_WRAP                   0x10
#define EDIT_FLAG_NO_SIZE_LIMIT          0x20
#define EDIT_FLAG_SIZE_INVALID           0x40
#define EDIT_FLAG_ENSURE_CURSOR_VISIBLE  0x80

#define EDITSTR_MARGIN         0
#define EDITSTR_BORDER         WG_CFG_DEFAULT_FRAME_WIDTH
#define EDITSTR_PADDING        2

#define EDITSTRING_INFO_IS_PROTECTED 1

class WebEditString : public DisplayElement
{
public:
	WebEditString ();
	virtual ~WebEditString ();

	virtual HTMLEventStatus Event (HTMLEvent *e);
	virtual void DrawThisOnly (DISPLAY_INT x, DISPLAY_INT y, WebGraphics *gc);
	virtual DISPLAYElementType Type (void) { return DISPLAY_ELEMENT_WEBEDITSTRING; }

	virtual void SetText (const WebChar *label);
	virtual void SetFont (WebGraphFont *font);
	WebChar *GetText (void);

	void SetMaxLength (long l, WEBC_BOOL saveText = WEBC_TRUE);
	long GetMaxLength (void);

	virtual void Home (void);
	virtual void End (void);
	virtual void Delete (void);
	virtual void Backspace (void);
	virtual void WriteModeToggle (void);
	virtual void Done (void);
	virtual void SetCursorPosition (long index);
	virtual void BeginSelect (void);
	virtual void EndSelect (void);
	virtual void Cut (void);
	virtual void Copy (void);
	virtual void Paste (void);
	virtual void InsertChar (WebChar c);
	virtual void SelectAll (void);

	virtual WebColor GetTextColor   (WebGraphics* gc);
	virtual WebColor GetBgColor     (WebGraphics* gc);
	virtual WebColor GetSelectColor (WebGraphics* gc);

	void SetListener(Listener *l);
	void SetWrap(WEBC_BOOL on);
	void EnableSizeLimit(void);
	void DisableSizeLimit(void);
#if (!EDITSTRING_INFO_IS_PROTECTED)
	DISPLAY_INT     miXOffset;
	WebChar        *mpText;
	long            miTextSize;
	long            miTextLen;
	long            miCursorPos;
	long            miSelectBegin;
	WEBC_UINT8      mEditFlags; // see wtedstr.cpp
#endif

protected:
	virtual void DeleteChars(long begin, long end);
	virtual long GetIndexAtXY(DISPLAY_INT x, DISPLAY_INT y);
	virtual void EnsureCursorVisible (void);
	virtual void DrawText(WebGraphics *gc, DISPLAY_INT x, DISPLAY_INT y, const WebChar *text, WebColor textColor, WebColor fillColor, WEBC_BOOL fill, WebFont font, int len=-1);
	virtual DISPLAY_INT PixelOffset (long index);
	virtual void GetFrameRect(WebRect *rect);
	virtual void GetTextRect(WebRect *rect);
	virtual void DrawFrame(WebRect *rect, WebGraphics *gc);
#if (EDITSTRING_INFO_IS_PROTECTED)
	DISPLAY_INT     miXOffset;
	WebChar        *mpText;
	long            miTextSize;
	long            miTextLen;
	long            miCursorPos;
	long            miSelectBegin;
	WEBC_UINT8      mEditFlags; // see wtedstr.cpp
#endif
	WebFontRef      mFont;
	Listener       *mpListener;
};


#endif // __WTEDSTR_HPP__
