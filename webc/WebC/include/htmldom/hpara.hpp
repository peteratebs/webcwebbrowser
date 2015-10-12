#ifndef __HPARA_HPP__
#define __HPARA_HPP__

#include "hflow.hpp"

/*************************************************************************
* class HTMLParagraph
* 
*************************************************************************/

class HTMLParagraph : public HTMLFlow
{
public:
	HTMLParagraph(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLParagraph();

	virtual HTMLElementType Type() { return HTML_PARAGRAPH_ELEMENT; }
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);

	virtual void SetHtmlAlign(WEBC_UINT8 bAlign);
	
	// TextUnit methods
	virtual WEBC_UINT16 GetBreak(void);
	virtual DISPLAY_INT GetParagraphSpace(void);

protected:	
	WEBC_UINT8 mPAlign;
};

#endif // __HPARA_HPP__
