#ifndef __HSTRIKE_HPP__
#define __HSTRIKE_HPP__

#include "hflow.hpp"

/*************************************************************************
* class HTMLStrike
* 
*************************************************************************/

class HTMLStrike : public HTMLFlow
{
public:
	HTMLStrike(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLStrike();

	virtual HTMLElementType Type() { return HTML_STRIKE_ELEMENT; }
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
};

#endif // __HSTRIKE_HPP__
