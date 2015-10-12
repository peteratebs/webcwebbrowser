#ifndef __HPASSWD_HPP__
#define __HPASSWD_HPP__

#include "hinput.hpp"
#include "dhpasswd.hpp"

/*************************************************************************
* class HTMLInputPassword
* 
*************************************************************************/

class HTMLInputPassword : public HTMLInput
{
public:
	HTMLInputPassword(HTMLTagParse *td, HTMLDocument *pDocument); 
	virtual ~HTMLInputPassword();
	
	virtual HTMLElementType Type() { return HTML_INPUT_PASSWORD_ELEMENT; }

	virtual HTMLEventStatus DefaultEvent(HTMLEvent *pEvent);
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);

	virtual void Reset(void);
	virtual void Select(void);

	// html attribute accessors
	virtual WEBC_CHAR*  DefValue  (void);
	virtual WEBC_SIZE   MaxLength (void);
	virtual WEBC_SIZE   Size      (void);
	virtual WEBC_CHAR*  Value     (void);
	
	// html attribute modifiers
	virtual void SetDefValue  (const WEBC_CHAR* value);
	virtual void SetMaxLength (WEBC_SIZE maxLength);   
	virtual void SetSize      (WEBC_SIZE size);        
	virtual void SetValue     (const WEBC_CHAR* value);

	virtual DisplayElement *GetDisplayElement (WEBC_BOOL create = WEBC_TRUE);
	virtual TextUnit       *GetTextUnit (void);

protected:
	HTMLPasswordDisplay *GetPasswordDisplay (WEBC_BOOL create = WEBC_TRUE);

	int miSize;
	WEBC_CHAR *mpDefValue;

	HTMLPasswordDisplay *mpPasswordDisplay;
};

#endif // __HPASSWD_HPP__
