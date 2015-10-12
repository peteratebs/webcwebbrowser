#ifndef __DHRADIO_HPP__
#define __DHRADIO_HPP__

#include "dhelem.hpp"
#include "wtradio.hpp"

class HTMLRadioButtonDisplay : public HTMLElementDisplay<WebRadioButton>
{
public:
	HTMLRadioButtonDisplay (HTMLElement *pElem);
	virtual ~HTMLRadioButtonDisplay (void);

	// from WebRadioButton
	virtual void SetSelected (WEBC_BOOL status);

	virtual DISPLAY_INT GetBaseline (void);
	virtual void SetParentDisplay(DisplayElement *parent, DisplayElement* insertBefore);
	virtual WEBC_BOOL SetElementStyle(void);
};

#endif // __DHRADIO_HPP__
