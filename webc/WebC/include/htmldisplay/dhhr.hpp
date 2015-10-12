#ifndef __DHHR_HPP__
#define __DHHR_HPP__

#include "dhelem.hpp"
#include "delement.hpp"

class HTMLHorizontalRule;

class HTMLHRDisplay : public HTMLElementDisplay<DisplayElement>
{
public:
	HTMLHRDisplay (HTMLHorizontalRule *pElem);
	virtual ~HTMLHRDisplay ();

	// DisplayElement methods
	virtual void Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC);
	virtual DISPLAYElementType Type (void) { return DISPLAY_ELEMENT_HRDISPLAY; }

	// TextUnit methods
	virtual DISPLAY_INT GetMinWidth(void);
	virtual DISPLAY_INT GetMaxWidth(void);
	virtual void SetParentWidth(DISPLAY_INT iWidth, DISPLAY_INT iAvail);

protected:
	virtual WEBC_BOOL SetElementStyle (void);
};

#endif // __DHHR_HPP__
