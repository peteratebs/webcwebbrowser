#ifndef __HIFRAME_HPP__
#define __HIFRAME_HPP__

#include "webcfg.h"

#if (WEBC_SUPPORT_FRAMES && WEBC_SUPPORT_IFRAMES)

#include "hframe.hpp"

/*************************************************************************
* class HTMLInlineFrame
* 
*************************************************************************/

class HTMLInlineFrame : public HTMLFrame
{
public:
	HTMLInlineFrame(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLInlineFrame();

	virtual HTMLElementType Type() { return HTML_IFRAME_ELEMENT; }
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
	
	// Tag Attributes
	void SetWidth(WEBC_CHAR *pWidth);
	void SetHeight(WEBC_CHAR *pHeight);
	void SetAlign(WEBC_CHAR *pAlign);
	int  GetAlign(void);

	virtual WEBC_UINT8 Border();
	
	HTMLLength FrameWidth(void);
	HTMLLength FrameHeight(void);

protected:
	HTMLLength mFrameWidth;
	HTMLLength mFrameHeight;
	WEBC_UINT8 mFrameAlign;
	WEBC_UINT8 mBorderWidth;
	
	virtual HTMLFrameDisplay *GetFrameDisplay(WEBC_BOOL create = WEBC_TRUE);
};

#endif // WEBC_SUPPORT_FRAMES && WEBC_SUPPORT_IFRAMES
#endif // __HIFRAME_HPP__
