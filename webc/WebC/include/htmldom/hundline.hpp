#ifndef __HUNDLINE_HPP__
#define __HUNDLINE_HPP__

#include "hflow.hpp"

/*************************************************************************
* class HTMLUnderline
* 
*************************************************************************/

class HTMLUnderline : public HTMLFlow
{
public:
	HTMLUnderline(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLUnderline();

	virtual HTMLElementType Type() { return HTML_UNDERLINE_ELEMENT; }
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
};

#endif // __HUNDLINE_HPP__
