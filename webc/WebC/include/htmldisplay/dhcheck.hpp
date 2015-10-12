#ifndef __DHCHECK_HPP__
#define __DHCHECK_HPP__

#include "dhelem.hpp"
#include "wtcheck.hpp"

class HTMLCheckBoxDisplay : public HTMLElementDisplay<WebCheckBox>
{
public:
	HTMLCheckBoxDisplay (HTMLElement *pElem);
	virtual ~HTMLCheckBoxDisplay (void);

	virtual DISPLAY_INT GetBaseline (void);
	virtual void SetParentDisplay(DisplayElement *parent, DisplayElement* insertBefore);
	virtual WEBC_BOOL SetElementStyle(void);
	
};

#endif // __DHCHECK_HPP__
