#ifndef __HBOLD_HPP__
#define __HBOLD_HPP__

#include "hflow.hpp"

/*************************************************************************
* class HTMLBold
* 
*************************************************************************/

class HTMLBold : public HTMLFlow
{
public:
	HTMLBold(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLBold();

	virtual HTMLElementType Type() { return HTML_BOLD_ELEMENT; }
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
};

#endif // __HBOLD_HPP__
