#ifndef __HSELECT_HPP__
#define __HSELECT_HPP__

#include "hinput.hpp"
#include "dhselect.hpp"
#include "listener.hpp"

/*************************************************************************
* class HTMLSelect
* 
*************************************************************************/

class HTMLOption;

#define HSELECT_FLAG_DEFAULT_EXPLICIT   HINPUT_FLAG_UNUSED_0

class HTMLSelect : public HTMLInput
{
public:
	HTMLSelect(HTMLTagParse *td, HTMLDocument *pDocument);
	HTMLSelect(HTMLSelect &copy);
	virtual ~HTMLSelect();

	// TextUnit interface methods	
	virtual HTMLElementType Type() { return HTML_SELECT_ELEMENT; }
	virtual HTMLEventStatus DefaultEvent(HTMLEvent *pEvent);
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);

	virtual short SelectedIndex(void);
	virtual void SetSelectedIndex(short index);
	virtual void SetDefSelected(short index);
	virtual short DefSelected();

	// Listener interface methods
	virtual void Notify (Notifier *who, NotifyEvent *what);
	
	// HTMLInput interface methods
	virtual void Reset(void);
	virtual WEBC_CHAR *Value(void);
	virtual long   Size(void);
	virtual void   SetSize(long i);
	
	// HTMLSelect-specific methods
	short NumOptions(void);
	void SetNumOptions(short numOptions);
	HTMLOption *GetOptionIndex(int iIndex);
	int GetIndexOfOption(HTMLOption *pOption);
	
	virtual DisplayElement *GetDisplayElement (WEBC_BOOL create = WEBC_TRUE);
	virtual TextUnit *GetTextUnit (void);
	
protected:
	short miNumOptions;
	short miDefSelected;
	int   miSize;
	
	HTMLListBoxDisplay *mpListBoxDisplay;
	HTMLDropBoxDisplay *mpDropBoxDisplay;

	HTMLListBoxDisplay *GetListBoxDisplay (WEBC_BOOL create = WEBC_TRUE);
	HTMLDropBoxDisplay *GetDropBoxDisplay (WEBC_BOOL create = WEBC_TRUE);
};

#endif // __HSELECT_HPP__
