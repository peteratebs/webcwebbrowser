#ifndef __HBLQUOTE_HPP__
#define __HBLQUOTE_HPP__

#include "hpara.hpp"

/*************************************************************************
* class HTMLBlockquote
* 
*************************************************************************/

class HTMLBlockquote : public HTMLParagraph
{
public:
	HTMLBlockquote(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLBlockquote();

	virtual HTMLElementType Type() { return HTML_BLOCKQUOTE_ELEMENT; }
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);	
};

#endif // __HBLQUOTE_HPP__
