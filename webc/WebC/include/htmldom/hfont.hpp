#ifndef __HFONT_HPP__
#define __HFONT_HPP__

#include "hflow.hpp"

/*************************************************************************
* class HTMLFont
* 
*************************************************************************/

class HTMLFont : public HTMLFlow
{
public:
	HTMLFont(HTMLTagParse *td, HTMLDocument *pDocument); 
	virtual ~HTMLFont();

	virtual HTMLElementType Type() { return HTML_FONT_ELEMENT; }
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
	
protected:
	WEBC_CHAR   *mpFontFace;
	CSSFontSize mFontSize;
	HTMLColor   mFontColor;
};

#endif // __HFONT_HPP__
