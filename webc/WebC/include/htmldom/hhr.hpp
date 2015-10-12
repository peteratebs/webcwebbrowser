#ifndef __HHR_HPP__
#define __HHR_HPP__

#include "helement.hpp"
#include "dhhr.hpp"

/*************************************************************************
* class HTMLHorizontalRule
* 
*************************************************************************/

class HTMLHorizontalRule : public HTMLElement
{
public:
	HTMLHorizontalRule(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLHorizontalRule();

	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);

	virtual WEBC_UINT16 GetBreak(void);
	virtual WEBC_UINT8 GetTextAlign(void);
	virtual DISPLAY_INT GetMinWidth(void);
	virtual DISPLAY_INT GetMaxWidth(void);
	virtual DISPLAY_INT GetTopMargin(void);
	virtual DISPLAY_INT GetBottomMargin(void);	

	virtual DisplayElement *GetDisplayElement (WEBC_BOOL create = WEBC_TRUE);
	virtual TextUnit       *GetTextUnit (void);	

	// HTMLHorizontalRule specific methods
	void SetTextAlign(WEBC_UINT8 bAlign);
	WEBC_UINT16 RuleSize(void);
	HTMLLength RuleWidth(void);
	
private:
	HTMLLength mWidth;
	WEBC_UINT16       miSize;
	WEBC_UINT8       mbAlign;

	HTMLHRDisplay *GetHRDisplay (WEBC_BOOL create = WEBC_TRUE);
	HTMLHRDisplay *mpHRDisplay;
};

#endif // __HHR_HPP__
