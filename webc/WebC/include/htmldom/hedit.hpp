#ifndef __HEDIT_HPP__
#define __HEDIT_HPP__

#include "hinput.hpp"
#include "dhedstr.hpp"

#define HEDIT_FLAG_MAXLENGTH     HINPUT_FLAG_UNUSED_0     
#define HEDIT_FLAG_SIZE          HINPUT_FLAG_UNUSED_1     
#define HEDIT_FLAG_UNUSED_0      HINPUT_FLAG_UNUSED_2     
#define HEDIT_FLAG_UNUSED_1      HINPUT_FLAG_UNUSED_3     
#define HEDIT_FLAG_UNUSED_2      HINPUT_FLAG_UNUSED_4     
#define HEDIT_FLAG_UNUSED_3      HINPUT_FLAG_UNUSED_5     
#define HEDIT_FLAG_UNUSED_4      HINPUT_FLAG_UNUSED_6     
#define HEDIT_FLAG_UNUSED_5      HINPUT_FLAG_UNUSED_7     

/*************************************************************************
* class HTMLEditString
* 
*************************************************************************/

class HTMLEditString : public HTMLInput
{
 protected:
	WEBC_SIZE              mSize;
	WEBC_CHAR*             mDefaultValue;
	HTMLEditStringDisplay* mEditStringDisplay;

	HTMLEditStringDisplay* GetEditStringDisplay (WEBC_BOOL create = WEBC_TRUE);

 public:
	HTMLEditString(HTMLTagParse *td, HTMLDocument *pDocument); 
	virtual ~HTMLEditString();
	
	virtual HTMLElementType Type() { return HTML_EDIT_STR_ELEMENT; }
	virtual WEBC_BOOL IsBasedOnType (HTMLElementType type) { return (type == HTML_EDIT_STR_ELEMENT || HTMLInput::IsBasedOnType(type)); }

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
	virtual void SetDisabled  (WEBC_BOOL disabled);    
	virtual void SetMaxLength (WEBC_SIZE maxLength);   
	virtual void SetSize      (WEBC_SIZE size);        
	virtual void SetValue     (const WEBC_CHAR* value);
	
	virtual DisplayElement *GetDisplayElement (WEBC_BOOL create = WEBC_TRUE);
	virtual TextUnit       *GetTextUnit (void);

	virtual void Notify (Notifier *who, NotifyEvent *what);
};

#endif // __HEDIT_HPP__
