#ifndef __HSPAN_HPP__
#define __HSPAN_HPP__

#include "hflow.hpp"

/*************************************************************************
* class HTMLSpan
* 
*************************************************************************/

class HTMLSpan : public HTMLFlow
{
public:
	HTMLSpan(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLSpan();

	virtual HTMLElementType Type() { return HTML_SPAN_ELEMENT; }
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
	virtual WEBC_UINT16 GetBreak(void);
};

#endif // __HSPAN_HPP__
