#ifndef __DHIBUTTON_HPP__
#define __DHIBUTTON_HPP__

#include "dhelem.hpp"
#include "wtbutton.hpp"
#include "hcbox.hpp"

class ITextDisplayStyle;

class HTMLInputButtonDisplay : public HTMLElementDisplay<WebButton>
{
protected:
	virtual WEBC_BOOL SetElementStyle(void);

	void SetButtonDimensions (
			DISPLAY_INT parentWidth, 
			DISPLAY_INT parentHeight, 
			WEBC_BOOL parentHeightAuto
		);

	ITextDisplayStyle* mTextStyle;
	HTMLContentBox     mBorder;

public:
	HTMLInputButtonDisplay (HTMLElement *pElem);
	virtual ~HTMLInputButtonDisplay ();

	virtual DisplayElement *TrapEventPoint (DISPLAY_INT x, DISPLAY_INT y, DISPLAY_INT viewX, DISPLAY_INT viewY);
	virtual void Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect* viewPortRect, WebGraphics* gc);
	
	// TextUnit methods
	virtual DISPLAY_INT GetBaseline(void);
	virtual DISPLAY_INT GetTopMargin(void);
	virtual DISPLAY_INT GetLeftMargin(void);
	virtual DISPLAY_INT GetBottomMargin(void);
	virtual DISPLAY_INT GetRightMargin(void);
	virtual DISPLAY_INT GetMinWidth(void);
	virtual DISPLAY_INT GetMaxWidth(void);
	virtual DISPLAY_INT GetPixelWidth(void);
	virtual DISPLAY_INT GetPixelHeight(void);
	virtual void SetParentDisplay(DisplayElement *parent, DisplayElement* insertBefore);
	virtual void SetParentHeight(DISPLAY_INT height);
	virtual void SetParentWidth(DISPLAY_INT width, DISPLAY_INT iAvail);
};

#endif // __DHIBUTTON_HPP__
