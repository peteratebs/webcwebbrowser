#ifndef __HHIDDEN_HPP__
#define __HHIDDEN_HPP__

#include "hinput.hpp"

/*************************************************************************
* class HTMLInputHidden
* 
*************************************************************************/

class HTMLInputHidden : public HTMLInput
{
protected:
	WEBC_CHAR *mValue;

public:
	HTMLInputHidden(HTMLTagParse *td, HTMLDocument *pDocument); 
	virtual ~HTMLInputHidden();

	virtual HTMLElementType Type() { return HTML_HIDDEN_INPUT_ELEMENT; }
	virtual WEBC_UINT16 GetUnitType(void);
	virtual DISPLAY_INT GetMinWidth(void)    { return 0; }
	virtual DISPLAY_INT GetMaxWidth(void)    { return 0; }
	virtual DISPLAY_INT GetPixelWidth(void)  { return 0; }
	virtual DISPLAY_INT GetPixelHeight(void) { return 0; }
	virtual DISPLAY_INT GetBaseline(void)    { return 0; }

	virtual WEBC_CHAR *Value(void);
	virtual void SetValue(const WEBC_CHAR *pValue);	
};

#endif // __HHIDDEN_HPP__
