#ifndef __HOPTION_HPP__
#define __HOPTION_HPP__

#include "hinput.hpp"

/*************************************************************************
* class HTMLOption
* 
*************************************************************************/

class HTMLSelect;

class HTMLOption : public HTMLInput
{
public:
#if (WEBC_SUPPORT_JSCRIPT)
	HTMLOption(HTMLTagParse *td, HTMLDocument *pDocument, JSObject *pJSObj = 0);
#else
	HTMLOption(HTMLTagParse *td, HTMLDocument *pDocument);
#endif // WEBC_SUPPORT_JSCRIPT	
	HTMLOption(HTMLOption &copy);
	~HTMLOption(void);
	
	// TextUnit interface methods	
	virtual HTMLElementType Type() { return HTML_OPTION_ELEMENT; }

	// HTMLInput interface methods
	virtual WEBC_CHAR *Value(void);
	virtual void SetValue(const WEBC_CHAR *pValue);
	virtual short TabIndex() { return (-1); }
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);

	// HTMLOption-specific methods	
	void SetText(WEBC_CHAR *pText);
	WEBC_CHAR *Text();
	WEBC_UINT8 DefSelected() { return mbDefaultSelected; }
	int GetIndex(void);
	HTMLSelect *GetSelectElement (void);
	virtual void SetForm(HTMLForm* pForm);

protected:
	WEBC_CHAR *mpValue;
	WEBC_CHAR *mpText;
	WEBC_UINT8 mbDefaultSelected;
};

#endif // __HOPTION_HPP__
