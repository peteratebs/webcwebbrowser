#ifndef __HHEADING_HPP__
#define __HHEADING_HPP__

#include "hpara.hpp"

/*************************************************************************
* class HTMLHeading
* 
*************************************************************************/

class HTMLHeading : public HTMLParagraph
{
public:
	HTMLHeading(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLHeading();

	virtual HTMLElementType Type() { return HTML_HEADING_ELEMENT; }
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
};

#endif // __HHEADING_HPP__
