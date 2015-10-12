#ifndef __HDD_HPP__
#define __HDD_HPP__

#include "hflow.hpp"

/*************************************************************************
* class HTMLDefinition
* 
*************************************************************************/

class HTMLDefinition : public HTMLFlow
{
public:
	HTMLDefinition(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLDefinition();

	virtual HTMLElementType Type() { return HTML_DEFINITION_ELEMENT; }
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
};

#endif // __HDD_HPP__
