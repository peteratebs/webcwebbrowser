#ifndef __HBIG_HPP__
#define __HBIG_HPP__

#include "hflow.hpp"

/*************************************************************************
* class HTMLBig
* 
*************************************************************************/

class HTMLBig : public HTMLFlow
{
public:
	HTMLBig(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLBig();

	virtual HTMLElementType Type() { return HTML_BIG_ELEMENT; }
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
};

#endif // __HBIG_HPP__
