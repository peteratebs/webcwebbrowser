#ifndef __HBR_HPP__
#define __HBR_HPP__

#include "helement.hpp"

/*************************************************************************
* class HTMLBr
* 
*************************************************************************/

class HTMLBr : public HTMLElement
{
public:
	HTMLBr(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLBr();

	virtual WEBC_UINT16 GetUnitType(void);
	virtual WEBC_UINT16 GetBreak (void);
	virtual DISPLAY_INT GetPixelHeight(void);
	virtual DISPLAY_INT GetBaseline(void);
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);

protected:
	WEBC_UINT8 mClear;
};

#endif // __HBR_HPP__
