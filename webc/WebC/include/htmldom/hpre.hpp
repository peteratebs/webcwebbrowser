#ifndef __HPRE_HPP__
#define __HPRE_HPP__

#include "hflow.hpp"

/*************************************************************************
* class HTMLPreformatted
* 
*************************************************************************/

class HTMLPreformatted : public HTMLFlow
{
public:
	HTMLPreformatted(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLPreformatted();

	virtual HTMLElementType Type() { return HTML_PRE_ELEMENT; }
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
};

#endif // __HPRE_HPP__
