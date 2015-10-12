#ifndef __ZDELEM_HPP__
#define __ZDELEM_HPP__

#include "delement.hpp"

class ZOrderedDisplayElement : public DisplayElement
{
public:
	ZOrderedDisplayElement (WEBC_INT32 zIndex = 0);

	// methods overridden from DisplayElement
	virtual WEBC_INT32 GetZIndex (void);

	// ZOrderedDisplayElement-specific methods
	void SetZIndex(WEBC_INT32 zIndex);
	virtual DISPLAYElementType Type (void) { return DISPLAY_ELEMENT_ZORDERED; }

private:
	WEBC_INT32 miZIndexValue;
};

#endif // __ZDELEM_HPP__
