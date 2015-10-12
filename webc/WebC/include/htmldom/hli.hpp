#ifndef __HLI_HPP__
#define __HLI_HPP__

#include "hdiv.hpp"

/******************************************************************************
* class HTMLListItem
*
******************************************************************************/

#define HLI_FLAG_FREE_FLOATING          HDIV_FLAG_UNUSED_0
#define HLI_FLAG_UNUSED_0               HDIV_FLAG_UNUSED_1

class HTMLList;
class HTMLListItemDisplay;

class HTMLListItem : public HTMLDiv
{
public:
	HTMLListItem(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLListItem();
	
	// HTMLElement methods
	virtual HTMLElementType Type() { return HTML_LIST_ITEM_ELEMENT; }
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);

	virtual DisplayElement *GetDisplayElement (WEBC_BOOL create = WEBC_TRUE);
	virtual TextUnit       *GetTextUnit (void);	
	
	// ListItem-specific methods
	short        ListOrder        (void);
	void         SetListOrder     (short iValue);
	void         SetListStyleType (CSSListStyleType styleType);
	HTMLList    *ParentList       (void);
		
protected:
	CSSListStyleType mListStyleType;
	short miValue;
	
	HTMLListItemDisplay *GetLIDisplay (WEBC_BOOL create = WEBC_TRUE);
	HTMLListItemDisplay *mpLIDisplay;
};

#endif // __HLI_HPP__
