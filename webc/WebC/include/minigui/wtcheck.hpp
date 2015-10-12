#ifndef __WTCHECK_HPP__
#define __WTCHECK_HPP__

#include "delement.hpp"

class WebCheckBox : public DisplayElement
{
protected:
	WEBC_BOOL selected;
public:
	WebCheckBox ();
	virtual ~WebCheckBox ();

	virtual HTMLEventStatus Event (HTMLEvent *e);
	virtual void DrawThisOnly (DISPLAY_INT x, DISPLAY_INT y, WebGraphics *gc);
	virtual DISPLAYElementType Type (void) { return DISPLAY_ELEMENT_WEBCHECKBOX; }

	void Toggle (void);
	WEBC_BOOL GetChecked (void);
	void SetChecked (WEBC_BOOL on);
};


#endif // __WTCHECK_HPP__
