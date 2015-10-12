#ifndef __HCODE_HPP__
#define __HCODE_HPP__

#include "hflow.hpp"

/*************************************************************************
* class HTMLCode
* 
*************************************************************************/

class HTMLCode : public HTMLFlow
{
public:
	HTMLCode(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLCode();

	virtual HTMLElementType Type() { return HTML_CODE_ELEMENT; }
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
};

#endif // __HCODE_HPP__
