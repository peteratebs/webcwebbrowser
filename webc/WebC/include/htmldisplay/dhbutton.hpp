#ifndef __DHBUTTON_HPP__
#define __DHBUTTON_HPP__

#include "hbutton.hpp"
#include "dflow.hpp"


class HTMLButtonDisplay : public HTMLFlowDisplay
{
protected:

public:
	HTMLButtonDisplay(HTMLButton * td);
	virtual ~HTMLButtonDisplay();

	// methods overridden from DisplayElement
    // These methods we are passing along immediately, they demonstrate spying on the hierarchy
    virtual DISPLAY_INT Width(void);
	virtual void Draw(DISPLAY_INT x, DISPLAY_INT y, WebRect* viewport, WebGraphics* gc);
	virtual void DrawThisOnly(DISPLAY_INT x, DISPLAY_INT y, WebGraphics* gc);

    // Trapevent point we actually use
	virtual DisplayElement *TrapEventPoint (DISPLAY_INT x, DISPLAY_INT y, DISPLAY_INT viewX, DISPLAY_INT viewY);

    // Format we set the elements width before proceeding to format
	virtual void Format(void);

	// methods overriden from TextUnit
	virtual void SetParentWidth(DISPLAY_INT ignored, DISPLAY_INT newParentWidth);
    // These methods we are passing along immediately, they demonstrate spying on the hierarchy
	virtual DISPLAY_INT GetMinWidth(void);
    // GetMaxWidth() point we actually use
	virtual DISPLAY_INT GetMaxWidth(void);
    // GetPixelWidth() point we actually use
	virtual DISPLAY_INT GetPixelWidth(void);
	virtual DISPLAY_INT GetPixelHeight(void);

};


#endif //  __DHBUTTON_HPP__
