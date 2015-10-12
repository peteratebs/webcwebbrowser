#ifndef __HITALIC_HPP__
#define __HITALIC_HPP__

#include "hflow.hpp"

/*************************************************************************
* class HTMLItalic
* 
*************************************************************************/

class HTMLItalic : public HTMLFlow
{
public:
	HTMLItalic(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLItalic();

	virtual HTMLElementType Type() { return HTML_ITALIC_ELEMENT; }
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
};

#endif // __HITALIC_HPP__
