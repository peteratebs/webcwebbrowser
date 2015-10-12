#ifndef __HINS_HPP__
#define __HINS_HPP__

#include "hflow.hpp"

/*************************************************************************
* class HTMLIns
* 
*************************************************************************/

class HTMLIns : public HTMLFlow
{
public:
	HTMLIns(HTMLTagParse* td, HTMLDocument *pDocument);
	virtual ~HTMLIns();

	virtual HTMLElementType Type() { return HTML_INS_ELEMENT; }
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
};

#endif // __HINS_HPP__
