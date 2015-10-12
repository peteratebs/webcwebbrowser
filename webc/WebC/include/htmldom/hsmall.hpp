#ifndef __HSMALL_HPP__
#define __HSMALL_HPP__

#include "hflow.hpp"

/*************************************************************************
* class HTMLSmall
* 
*************************************************************************/

class HTMLSmall : public HTMLFlow
{
public:
	HTMLSmall(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLSmall();

	virtual HTMLElementType Type() { return HTML_SMALL_ELEMENT; }
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
};

#endif // __HSMALL_HPP__
