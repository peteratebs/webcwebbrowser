#ifndef __WTBUTTON_HPP__
#define __WTBUTTON_HPP__

#include "delement.hpp"
#include "listener.hpp"

typedef WEBC_UINT8 WEB_BUTTON_FLAGS;

#define BUTTON_FLAG_DOWN            0x01
#define BUTTON_FLAG_OVER            0x02
#define BUTTON_FLAG_MOUSEDOWN       0x04
#define BUTTON_FLAG_ACTIVATED       0x08
#define BUTTON_FLAG_SINK_FACE       0x10
#define BUTTON_FLAG_DOWN_TRIGGER    0x20
#define BUTTON_FLAG_UP_TRIGGER      0x40

class WebButton : public DisplayElement
{
 private:
	WEB_BUTTON_FLAGS mButtonFlags;
	Listener*        mListener;

	void Activate (void);

 public:
	WebButton ();
	virtual ~WebButton ();

	virtual HTMLEventStatus Event (HTMLEvent *e);
	virtual DISPLAYElementType Type (void) { return DISPLAY_ELEMENT_WEBBUTTON; }

	void             SetButtonFlag    (WEB_BUTTON_FLAGS f);
	void             ClearButtonFlag  (WEB_BUTTON_FLAGS f);
	WEB_BUTTON_FLAGS GetButtonFlags   (void);
	WEBC_BOOL        Activated        (void);
	void             Deactivate       (void);
	void             SetListener      (Listener* who);
};


class Web3DButton : public WebButton
{
 protected:
	virtual void DrawButtonFace(DISPLAY_INT x, DISPLAY_INT y, WebGraphics *gc);
	virtual void DrawBorder(DISPLAY_INT x, DISPLAY_INT y, WebGraphics *gc);

	DISPLAY_INT  mBorderWidth;
	DISPLAY_INT  mPadding;

 public:
	Web3DButton ();
	virtual ~Web3DButton ();

	virtual void DrawThisOnly (DISPLAY_INT x, DISPLAY_INT y, WebGraphics *gc);

	void SetBorderWidth(DISPLAY_INT width);
	void SetPadding(DISPLAY_INT width);
	virtual WebColor GetBackgroundColor (WebGraphics* gc);
	virtual WebColor GetHiliteColor (WebGraphics* gc);
	virtual WebColor GetShadowColor (WebGraphics* gc);
};

class Web3DTextButton : public Web3DButton
{
public:
	Web3DTextButton ();
	virtual ~Web3DTextButton ();

	void SetText (const WebChar *label);
	void SetFont (WebGraphFont *font);
	WebChar* GetText (void);

	virtual WebColor GetTextColor (WebGraphics* gc);

protected:
	virtual void DrawButtonFace(DISPLAY_INT x, DISPLAY_INT y, WebGraphics *gc);

	WebChar*    mText;
	WebFontRef  mFont;
	long        mTextSize;
};

class Web3DBitmapButton : public Web3DButton
{
public:
	Web3DBitmapButton ();
	virtual ~Web3DBitmapButton ();

	void SetDefaultBitmap (WebBitmap pBitmap);
	void SetOverBitmap (WebBitmap pBitmap);
	void SetDownBitmap (WebBitmap pBitmap);
	void SetDisabledBitmap (WebBitmap pBitmap);

protected:
	virtual void DrawButtonFace(DISPLAY_INT x, DISPLAY_INT y, WebGraphics *gc);

	WebBitmap mDefaultBitmap;
	WebBitmap mOverBitmap;
	WebBitmap mDownBitmap;
	WebBitmap mDisabledBitmap;
};


#endif // __WTBUTTON_HPP__
