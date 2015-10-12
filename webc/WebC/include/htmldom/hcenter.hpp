#ifndef __HCENTER_HPP__
#define __HCENTER_HPP__

#include "hflow.hpp"

/*************************************************************************
* class HTMLCenter
* 
*************************************************************************/

class HTMLCenter : public HTMLFlow
{
public:
	HTMLCenter(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLCenter();

	virtual HTMLElementType Type() { return HTML_CENTER_ELEMENT; }
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
};

#endif // __HCENTER_HPP__
