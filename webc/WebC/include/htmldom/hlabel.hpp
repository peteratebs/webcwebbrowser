#ifndef __HLABEL_HPP__
#define __HLABEL_HPP__

#include "hspan.hpp"

/*************************************************************************
* class HTMLLabel
* 
*************************************************************************/

class HTMLLabel : public HTMLSpan
{
protected:
	ObjectReference<HTMLElement> mForElement;
	WEBC_CHAR*                   mFor;
	
public:
	HTMLLabel (HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLLabel (void);

	virtual HTMLElementType Type (void) { return HTML_LABEL_ELEMENT; }
	virtual int SetProperty (HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);	
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
	virtual HTMLElement* EventParent (void);
	virtual HTMLElement* ForElement (void);
};

#endif // __HLABEL_HPP__
