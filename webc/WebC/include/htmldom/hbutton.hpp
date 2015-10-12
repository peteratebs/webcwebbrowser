#ifndef __HBUTTON_HPP__
#define __HBUTTON_HPP__

#include "hflow.hpp"
#include "webcutf.h"

/*************************************************************************
* class HTMLButton
*
*************************************************************************/
class JScriptFunction;

typedef WEBC_UINT8 HBUTTON_STATE;

#define HBUTTON_FLAG_DISABLED     HELEM_FLAG_UNUSED_13
#define HBUTTON_FLAG_NOTAB        HELEM_FLAG_UNUSED_14
#define HBUTTON_FLAG_ACTIVE       HELEM_FLAG_UNUSED_15


#define HBUTTON_STATE_NORMAL        0x00
#define HBUTTON_STATE_ACTIVE        0x01
#define HBUTTON_STATE_HOVER         0x04
#define HBUTTON_STATE_FOCUS         0x08


// hanchor.cpp
class HTMLButton : public HTMLFlow
{
public:
	HTMLButton(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLButton();
    virtual WEBC_BOOL Active (void);

	// Listener interface methods

	// TextUnit Interface
	virtual HTMLElementType Type() { return HTML_BUTTON_ELEMENT; }
	virtual WEBC_UINT16 GetUnitType(void) {	return (TU_TYPE_DEFAULT); } // seems right
	virtual WEBC_UINT16 GetBreak(void) {	return (TU_BREAK_NONE); }   // seems right
	virtual WEBC_UINT8  GetVAlign(void)         { return TU_VALIGN_TOP; }// Seems right

	// Overridden from HTMLElement
	virtual HTMLEventStatus DefaultEvent(HTMLEvent *pEvent);
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);

	// Button attribute methods
	virtual WEBC_CHAR *Name(void);
	virtual void  SetName(const WEBC_CHAR *pName);

    virtual WEBC_CHAR *GetType(void);
	virtual void  SetType(const WEBC_CHAR *pType);

    virtual WEBC_CHAR *Value(void);
	virtual void  SetValue(const WEBC_CHAR *pValue);

	virtual short TabIndex(void);
	virtual void  SetTabIndex(short iIndex);

	virtual void SetForm(HTMLForm *pForm);
	virtual HTMLEventType GetFormEventType(void);

	virtual WebColor GetBackgroundColor (void);
	virtual WEBC_BOOL HasBackgroundColor (void);

	virtual ITextDisplayStyle* GetTextDisplayStyle (WEBC_BOOL create);
	virtual void InheritedStyleUpdate (void);

	void GetWidth (CSSLength *width);
	void GetHeight (CSSLength *height);

	virtual HTMLInputType GetInputType() { return mInputType; }

	virtual void SetDisabled  (WEBC_BOOL disabled);
	virtual WEBC_BOOL   Disabled  (void);

	virtual WEBC_UINT16 AccessKey (void) { return mAccessKey; }
	virtual void SetAccessKey (WEBC_UINT16 key) { mAccessKey = key; }

#if (WEBC_SUPPORT_STYLE_SHEETS)
	 int InPseudoClass(CSSPseudoClass ps);
#endif

protected:
	WEBC_CHAR *mpName;
	WEBC_CHAR *mpValue;
	WEBC_CHAR *mpType;
	short miTabIndex;
	HBUTTON_STATE mButtonState;
	WebcCharset mCharset;
	HTMLInputType mInputType; // always HTML_INPUT_TYPE_BUTTON
	WEBC_UINT16 mAccessKey;

	virtual HTMLFlowDisplay *GetFlowDisplay (WEBC_BOOL create = WEBC_TRUE);

};

#endif // __HBUTTON_HPP__
