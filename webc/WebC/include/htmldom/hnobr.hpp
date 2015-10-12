#ifndef __HNOBR_HPP__
#define __HNOBR_HPP__

#include "hflow.hpp"

/*************************************************************************
* class HTMLNoBr
* 
*************************************************************************/

class HTMLNoBr : public HTMLFlow
{
public:
	HTMLNoBr(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLNoBr();

	virtual HTMLElementType Type() { return HTML_NOBR_ELEMENT; }
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
};

#endif // __HNOBR_HPP__
