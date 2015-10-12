#ifndef __HCITE_HPP__
#define __HCITE_HPP__

#include "hflow.hpp"

/*************************************************************************
* class HTMLCite
* 
*************************************************************************/

class HTMLCite : public HTMLFlow
{
public:
	HTMLCite(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLCite();

	virtual HTMLElementType Type() { return HTML_CITE_ELEMENT; }
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
};

#endif // __HCITE_HPP__
