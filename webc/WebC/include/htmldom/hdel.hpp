#ifndef __HDEL_HPP__
#define __HDEL_HPP__

#include "hflow.hpp"

/*************************************************************************
* class HTMLDel
* 
*************************************************************************/

class HTMLDel : public HTMLFlow
{
public:
	HTMLDel(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLDel();

	virtual HTMLElementType Type() { return HTML_DEL_ELEMENT; }
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
};

#endif // __HDEL_HPP__
