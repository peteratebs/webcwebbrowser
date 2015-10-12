#ifndef __HCHKBOX_HPP__
#define __HCHKBOX_HPP__

#include "hinput.hpp"
#include "dhcheck.hpp"

/*************************************************************************
* class HTMLCheckbox
* 
*************************************************************************/

class HTMLCheckbox : public HTMLInput
{
 protected:
	WEBC_CHAR*           mDefaultValue;
	WEBC_BOOL            mDefaultChecked;
	HTMLCheckBoxDisplay* mCheckBoxDisplay;

	HTMLCheckBoxDisplay* GetCheckBoxDisplay (WEBC_BOOL create = WEBC_TRUE);

 public:
	HTMLCheckbox(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLCheckbox();

	virtual HTMLElementType Type() { return HTML_CHECKBOX_ELEMENT; }
	virtual HTMLEventStatus DefaultEvent(HTMLEvent *pEvent);
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);


	// html attribute accessors
	virtual WEBC_BOOL   Checked   (void);
	virtual WEBC_CHAR*  DefValue  (void) { return mDefaultValue; }
	virtual WEBC_CHAR*  Value     (void);
	
	// html attribute modifiers
	virtual void SetChecked   (WEBC_BOOL checked);
	virtual void SetDefValue  (const WEBC_CHAR* value);
	virtual void SetValue     (const WEBC_CHAR* value);

	virtual void Reset(void);

	virtual DisplayElement *GetDisplayElement (WEBC_BOOL create = WEBC_TRUE);
	virtual TextUnit       *GetTextUnit (void);	
};

#endif // __HCHKBOX_HPP__
