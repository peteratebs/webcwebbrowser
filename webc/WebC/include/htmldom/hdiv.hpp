#ifndef __HDIV_HPP__
#define __HDIV_HPP__

#include "hflow.hpp"

/*************************************************************************
* class HTMLDiv
* 
*************************************************************************/

#define HDIV_FLAG_ALIGN_SET                 HFLOW_FLAG_UNUSED_0
#define HDIV_FLAG_UNUSED_0                  HFLOW_FLAG_UNUSED_1

class HTMLDiv : public HTMLFlow
{
public:
	HTMLDiv(HTMLTagParse *td, HTMLDocument *pDocument);
	HTMLDiv(HTMLDiv &copy);

	virtual ~HTMLDiv();

	virtual HTMLElementType Type() { return HTML_DIV_ELEMENT; }
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);

	virtual void SetHtmlAlign(WEBC_UINT8 bAlign);
	
protected:
	WEBC_UINT8 mDivAlign;
};

#endif // __HDIV_HPP__
