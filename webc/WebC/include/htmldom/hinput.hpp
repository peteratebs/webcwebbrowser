#ifndef __HINPUT_HPP__
#define __HINPUT_HPP__

#include "helement.hpp"

/*************************************************************************
* class HTMLInput
* 
*************************************************************************/

#define HINPUT_FLAG_DISABLED     HELEM_FLAG_UNUSED_0
#define HINPUT_FLAG_READONLY     HELEM_FLAG_UNUSED_1
#define HINPUT_FLAG_NOTAB        HELEM_FLAG_UNUSED_2
#define HINPUT_FLAG_ACTIVE       HELEM_FLAG_UNUSED_3
#define HINPUT_FLAG_HOVER        HELEM_FLAG_UNUSED_4
#define HINPUT_FLAG_UNUSED_0     HELEM_FLAG_UNUSED_5
#define HINPUT_FLAG_UNUSED_1     HELEM_FLAG_UNUSED_6
#define HINPUT_FLAG_UNUSED_2     HELEM_FLAG_UNUSED_7
#define HINPUT_FLAG_UNUSED_3     HELEM_FLAG_UNUSED_8
#define HINPUT_FLAG_UNUSED_4     HELEM_FLAG_UNUSED_9
#define HINPUT_FLAG_UNUSED_5     HELEM_FLAG_UNUSED_10
#define HINPUT_FLAG_UNUSED_6     HELEM_FLAG_UNUSED_11
#define HINPUT_FLAG_UNUSED_7     HELEM_FLAG_UNUSED_12

#define HTML_INPUT_STATE_ACTIVE  0x01
#define HTML_INPUT_STATE_HOVER   0x02

#define WEBC_CFG_TEXT_INPUT_PADDING_LEFT          3
#define WEBC_CFG_TEXT_INPUT_PADDING_TOP           2
#define WEBC_CFG_TEXT_INPUT_PADDING_RIGHT         3
#define WEBC_CFG_TEXT_INPUT_PADDING_BOTTOM        1

class HTMLInput : public HTMLElementContainer
{
 protected:
	WEBC_UINT16        mAccessKey;
	WEBC_CHAR*         mName;
	WEBC_INT16         mTabIndex;
	ITextDisplayStyle* mTextStyle;
	WEBC_CHAR*         mInitialValue;
	WEBC_CHAR*     mAlt;
	HTMLInputType mInputType; // reflects the "type attribute" but more importantly, specifies the subClass
	WEBC_CHAR*         mAccept; // list of mime content types that the server will handle correctly
	WEBC_UINT8 mAlign;

 public:
  #if (WEBC_SUPPORT_JSCRIPT)
	HTMLInput(HTMLTagParse *td, HTMLDocument *pDocument, JSObject *pJSObj = 0);
  #else
	HTMLInput(HTMLTagParse *td, HTMLDocument *pDocument);
  #endif // WEBC_SUPPORT_JSCRIPT	
	HTMLInput(HTMLInput &copy);
	virtual ~HTMLInput();

	// Overridden from HTMLElement
	virtual HTMLEventStatus DefaultEvent(HTMLEvent *pEvent);
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);
  #if (WEBC_SUPPORT_STYLE_SHEETS)
	virtual int InPseudoClass(CSSPseudoClass ps);
  #endif 	

	virtual void SetForm(HTMLForm *pForm);

	virtual ITextDisplayStyle* GetTextDisplayStyle (WEBC_BOOL create = WEBC_TRUE);
	virtual void InheritedStyleUpdate (void);
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
	virtual HTMLEventStatus JSEvent(HTMLEvent *pEvent);
	
	// Property management

	// html attribute accessors
	virtual WEBC_UINT16 AccessKey (void);
	virtual WEBC_BOOL   Disabled  (void) { return (mFlags & HINPUT_FLAG_DISABLED)? WEBC_TRUE : WEBC_FALSE; }
	virtual WEBC_CHAR*  Name      (void);
	virtual WEBC_BOOL   ReadOnly  (void);
	virtual WEBC_INT16  TabIndex  (void);
	virtual WEBC_CHAR*  Alt      (void) { return mAlt; }
	virtual HTMLInputType GetInputType() { return mInputType; }
	virtual WEBC_CHAR *Accept(void);
	virtual WEBC_UINT8 HtmlAlign() { return mAlign; }
	
	// html attribute modifiers
	virtual void SetAccessKey (WEBC_UINT16 key);
	virtual void SetDisabled  (WEBC_BOOL disabled);
	virtual void SetName      (const WEBC_CHAR* name);
	virtual void SetReadOnly  (WEBC_BOOL readonly);
	virtual void SetTabIndex  (WEBC_INT16 tabIndex);
    virtual void SetAlt      (const WEBC_CHAR *alt);
	virtual void SetAccept(const WEBC_CHAR *pAccept);
	virtual void SetHtmlAlign(WEBC_UINT8 bAlign);

};

#endif // __HINPUT_HPP__
