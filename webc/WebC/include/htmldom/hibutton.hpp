#ifndef __HIBUTTON_HPP__
#define __HIBUTTON_HPP__

#include "hinput.hpp"
#include "dhibutton.hpp"
#include "listener.hpp"

#define HIBUTTON_FLAG_UNUSED_0   HINPUT_FLAG_UNUSED_0

/*************************************************************************
* class HTMLInputButton
* 
*************************************************************************/

class HTMLInputButton : public HTMLInput
{
public:
	HTMLInputButton(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLInputButton();

	virtual HTMLElementType Type() { return HTML_INPUT_BUTTON_ELEMENT; }
	virtual HTMLEventStatus DefaultEvent(HTMLEvent *pEvent);
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);

	virtual void SetValue (const WEBC_CHAR *pValue);
	virtual WEBC_CHAR *Value (void);

	virtual DisplayElement *GetDisplayElement (WEBC_BOOL create = WEBC_TRUE);
	virtual TextUnit       *GetTextUnit (void);

	virtual WEBC_BOOL Active (void);
	virtual void Notify (Notifier *who, NotifyEvent *what);

protected:
	HTMLInputButtonDisplay *GetInputButtonDisplay (WEBC_BOOL create = WEBC_TRUE);

	WEBC_CHAR* mValue;
	HTMLInputButtonDisplay *mpDisplayButton;
};

#endif // __HIBUTTON_HPP__
