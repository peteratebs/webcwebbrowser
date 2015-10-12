#ifndef __HLIST_HPP__
#define __HLIST_HPP__

#include "hli.hpp"
#include "hpara.hpp"

/******************************************************************************
* class HTMLList 
*
******************************************************************************/

class HTMLList : public HTMLParagraph
{
public:
	HTMLList(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLList();

	// HTMLElementType
	virtual HTMLElementType Type() { return HTML_LIST_ELEMENT; }
	virtual DISPLAY_INT GetParagraphSpace(void);
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);

	// HTMLList-specific methods
	void SetListStyleType(CSSListStyleType styleType);
	void SetStart(short iValue);
	void SetNesting(short iNesting);
	short Start(void);
	short Nesting(void);
	short Items(void);

protected:
	void FindNesting(void);

	CSSListStyleType mListStyleType;
	short miStart;
	short miNesting;
};

class HTMLOrderedList : public HTMLList
{
public:
	HTMLOrderedList(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLOrderedList();
};

class HTMLUnorderedList : public HTMLList
{
public:
	HTMLUnorderedList(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLUnorderedList();
};

#endif // __HLIST_HPP__
