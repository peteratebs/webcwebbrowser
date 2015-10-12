#ifndef __HKBD_HPP__
#define __HKBD_HPP__

#include "hflow.hpp"

/*************************************************************************
* class HTMLKbd
* 
*************************************************************************/

class HTMLKbd : public HTMLFlow
{
public:
	HTMLKbd(HTMLTagParse* td, HTMLDocument *pDocument);
	virtual ~HTMLKbd();

	virtual HTMLElementType Type() { return HTML_KBD_ELEMENT; }
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
};

#endif // __HKBD_HPP__
