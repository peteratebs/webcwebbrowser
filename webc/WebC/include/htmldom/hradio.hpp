#ifndef __HRADIO_HPP__
#define __HRADIO_HPP__

#include "hinput.hpp"
#include "dhradio.hpp"

/*************************************************************************
* class HTMLRadioButton
* 
*************************************************************************/

class HTMLRadioButton : public HTMLInput
{
public:
	HTMLRadioButton(HTMLTagParse *td, HTMLDocument *pDocument); 
	virtual ~HTMLRadioButton();
	virtual HTMLElementType Type() { return HTML_RADIO_BUTTON_ELEMENT; }
	virtual HTMLEventStatus DefaultEvent(HTMLEvent *pEvent);
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);

	// Observer interface
	virtual void Update(WEBC_PFBYTE o, WEBC_PFBYTE data);
	
	virtual void Reset(void);

	// html attribute accessors
	virtual WEBC_BOOL   Checked   (void);
	virtual WEBC_CHAR*  DefValue  (void);
	virtual WEBC_CHAR*  Value     (void);
	
	// html attribute modifiers
	virtual void SetChecked   (WEBC_BOOL checked);
	virtual void SetDefValue  (const WEBC_CHAR* value);
	virtual void SetValue     (const WEBC_CHAR* value);

	virtual DisplayElement *GetDisplayElement (WEBC_BOOL create = WEBC_TRUE);
	virtual TextUnit       *GetTextUnit (void);

	void UnselectSiblings (void);

	// public data - double linked list of mutex radio buttons	
	HTMLRadioButton *mpGroupNext;
	HTMLRadioButton *mpGroupPrev;

protected:
	void GroupInsertBefore(HTMLRadioButton *pRadio);
	void GroupUnlink (void);
	HTMLRadioButtonDisplay *GetRadioButtonDisplay (WEBC_BOOL create = WEBC_TRUE);

	WEBC_CHAR *mpDefValue;
	WEBC_CHAR *mpValue;
	int miDefChecked;

	HTMLRadioButtonDisplay *mpRadioDisplay;
};

#endif // __HRADIO_HPP__
