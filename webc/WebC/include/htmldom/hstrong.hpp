#ifndef __HSTRONG_HPP__
#define __HSTRONG_HPP__

#include "hflow.hpp"

/*************************************************************************
* class HTMLStrong
* 
*************************************************************************/

class HTMLStrong : public HTMLFlow
{
public:
	HTMLStrong(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLStrong();

	virtual HTMLElementType Type() { return HTML_STRONG_ELEMENT; }
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
};

#endif // __HSTRONG_HPP__
