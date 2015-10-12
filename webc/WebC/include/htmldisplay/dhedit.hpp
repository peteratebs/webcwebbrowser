#ifndef __DHEDIT_HPP__
#define __DHEDIT_HPP__

#include "dhelem.hpp"
#include "wtedstr.hpp"
#include "hcbox.hpp"

/******************************************************************************
* template class HTMLEditDisplay 
*
*    Class template for HTMLEditStringDisplay, HTMLEditBoxDisplay, and
*    HTMLPasswordDisplay
*
******************************************************************************/

template<class T> class HTMLEditDisplay : public HTMLElementDisplay<T>
{
protected:
	PresetWebRect mTextRect;
	DISPLAY_INT   miParentWidth;
	DISPLAY_INT   miParentHeight;
	WebColor      mBackgroundColor;
	WebColor      mTextColor;

	// MUST be implemented to instantiate this class
	virtual WEBC_BOOL SizeIsValid (void) = 0;
	virtual void SetSizeValid (WEBC_BOOL valid = WEBC_TRUE) = 0;	
	virtual DISPLAY_INT FindPixelWidth (HTMLContentBox *b, WebGraphics *gc) = 0;

	// Overridden from WebEditString
	virtual void GetFrameRect(WebRect *rect);
	virtual void GetTextRect(WebRect *rect);
	virtual void DrawFrame(WebRect *box, WebGraphics *gc);
	virtual DISPLAY_INT FindPixelHeight (HTMLContentBox *b, DISPLAY_INT parentHeight, WebGraphics *gc);
	void FindTextRect(WebRect *textRect, WebRect *elemRect, HTMLContentBox *b);
	void SetDimensions(WebGraphics *gc);
	virtual WEBC_BOOL SetElementStyle (void);
	
public:

	HTMLEditDisplay (HTMLElement *pElem);
	virtual ~HTMLEditDisplay (void);

	// DisplayElement methods
	virtual void Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC);
	virtual DisplayElement *TrapEventPoint (DISPLAY_INT x, DISPLAY_INT y, DISPLAY_INT viewX, DISPLAY_INT viewY);
	
	// TextUnit methods
	virtual DISPLAY_INT GetMinWidth(void);
	virtual DISPLAY_INT GetMaxWidth(void);
	virtual DISPLAY_INT GetPixelWidth(void);
	virtual DISPLAY_INT GetPixelHeight(void);
	virtual DISPLAY_INT GetParentWidth(void);
	virtual void SetParentDisplay(DisplayElement *parent, DisplayElement* insertBefore);
	virtual void SetParentHeight(DISPLAY_INT parentHeight);
	virtual void SetParentWidth(DISPLAY_INT parentWidth, DISPLAY_INT iAvail);

	virtual WebColor GetTextColor   (WebGraphics* gc);
	virtual WebColor GetBgColor     (WebGraphics* gc);
};

#endif // __DHEDIT_HPP__
