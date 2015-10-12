#ifndef __HADDRESS_HPP__
#define __HADDRESS_HPP__

#include "hdiv.hpp"

/*************************************************************************
* class HTMLAddress
* 
*************************************************************************/

class HTMLAddress : public HTMLDiv
{
public:
	HTMLAddress(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLAddress();

	virtual HTMLElementType Type() { return HTML_ADDRESS_ELEMENT; }
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
};

#endif // __HADDRESS_HPP__
