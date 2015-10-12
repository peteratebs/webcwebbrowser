#ifndef __SOFTKEYPAD_HPP__
#define __SOFTKEYPAD_HPP__

#include "webcmem.h"
#include "wcapi.h"
//#include "webc.h"

/*************************************************************************
* class SoftKeyPad
*
*************************************************************************/

typedef struct s_Softkey {
int x;
int y;
int h;
int w;
unsigned char keys[4];
} Softkey;

#define KC(X) (unsigned char)X

#define WBS 255         // Backspace
#define WCR 254         // Carriage return
#define WUC 253         // Uppercase Keyboard
#define WLC 252         // Lowercase Keyboard
//#define WNC 251         // NumericKeyboard
/*
WKR : Right
WES : Escape
WAC : Accept
WBS : Back Space
WCR : Carrige Return ??
WSY : SYMBOL
WNO : NUMBERS
WCL : CAPS LOCK
WSP : Space
*/

#define WKR 251	// Right
#define WES 250 // Esc
#define WAC 249 // Accept
#define WSY 248 // Symbol keyboard
#define WNO 247 // Numeric keyboard
#define WCL 246 // Caps lock

#define WSPECIAL 246


// SoftKeyPad.cpp
class SoftKeyPad
{
public:
	SoftKeyPad(const int Height, const int Width, const int *KeyIndexMap, const Softkey *KeyPadMap, const char *pImgUrl0,const char *pImgUrl1,const char *pImgUrl2,const char *pImgUrl3);
	~SoftKeyPad(void);
	Softkey *XYToSoftkey(int x, int y);
	HTMLEventStatus SoftKeyPad::kbBodyCallbackMethod(wcCtx* Ctx, wcEL element,HTMLEvent* event);
	void ReSizeTextWindow(int height, wcBOOL doupdate);
	void PopUpKeypad(HTMLBrowser *pBrowser, HTMLElement *pEl);
	void PopDownKeypad(wcBOOL KeepInput);

#if (0)
	void SetTextColor(HTMLColor color);
	HTMLColor TextColor(void);
	void SetLinkColor(HTMLColor color);
	HTMLColor LinkColor(void);
	void SetALinkColor(HTMLColor color);
	HTMLColor ALinkColor(void);
	void SetVLinkColor(HTMLColor color);
	HTMLColor VLinkColor(void);
	void SetBodyBackgroundColor(HTMLColor color);
	HTMLColor BodyBackgroundColor(void);

	int Loaded(void) { return mbLoaded; }
	void SetLoaded(void) { mbLoaded = 1; }

	WEBC_BOOL IsFragment() { return mbIsFragment; }

	virtual DisplayElement *GetDisplayElement (WEBC_BOOL create = WEBC_TRUE);
#endif

protected:
	int Height;		 			// Height and width of keypad image.
	int Width;
    int ScreenHeight;			// Height of the screen (need to call get canvasrect */
	int KeypadOffsetX;			// Offset to the keypad image after data entry field
	int KeypadOffsetY;
	int currentKeyMap;
	const int *KeyIndexMap;		/* Passed from constructor */
	const Softkey *KeyPadMap;
	const char *pImgUrls[4];
	int nKeyIndexMapPairs;		/* Derived during constructor */
	int nKeyPadMapEntries;
	HELEMENT_HANDLE	pTextWinObj;// Text window
	HELEMENT_HANDLE	pImageObj;	// Keypad image
	wcCtx WindowCtx;		    /* Window properties */
	HTMLBrowser * FocusBrowser;	// Browser that popped us up
	HTMLElement * FocusElement; // Text input element that poped us up.
	WEBC_BOOL ProcessingTextArea;// Working for a text area, allow newlines.  
};


#endif // __SOFTKEYPAD_HPP__
