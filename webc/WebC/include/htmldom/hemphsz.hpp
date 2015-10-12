#ifndef __HEMPHSZ_HPP__
#define __HEMPHSZ_HPP__

#include "hflow.hpp"

/*************************************************************************
* class HTMLEmphasize
* 
*************************************************************************/

class HTMLEmphasize : public HTMLFlow
{
public:
	HTMLEmphasize(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLEmphasize();

	virtual HTMLElementType Type() { return HTML_EMPHASIZE_ELEMENT; }
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
};

#endif // __HEMPHSZ_HPP__
