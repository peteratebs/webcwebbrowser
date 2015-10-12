#ifndef __HSPACER_HPP__
#define __HSPACER_HPP__

#include "helement.hpp"

/*************************************************************************
* class HTMLSpacer
* 
*  Extends HTMLElement by providing a way of setting and retrieving 
* position information.  As noted in the description of class TextUnit,
* this position is given in pixels relative to the object's parent element
*
*************************************************************************/

// hspacer.cpp
class HTMLSpacer : public HTMLElement
{
public:
	HTMLSpacer(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLSpacer(void);

	// TextUnit interface
	virtual HTMLElementType Type() { return HTML_SPACER_ELEMENT; }
	virtual DISPLAY_INT GetPixelWidth(void);
	virtual DISPLAY_INT GetPixelHeight(void);

	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);

	void SetWidth(int iWidth);
	void SetHeight(int iHeight);
	
protected:
	WEBC_UINT16 mwWidth;
	WEBC_UINT16 mwHeight;
};

#endif // __HSPACER_HPP__
