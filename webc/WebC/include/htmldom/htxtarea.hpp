#ifndef __HTXTAREA_HPP__
#define __HTXTAREA_HPP__

#include "hinput.hpp"
#include "dhedbox.hpp"

/*************************************************************************
* class HTMLTextArea
* 
*************************************************************************/

#define TEXTAREA_WRAP_PHYSICAL 0
#define TEXTAREA_WRAP_VIRTUAL  1
#define TEXTAREA_WRAP_OFF      2

class HTMLTextArea : public HTMLInput
{
public:
	HTMLTextArea(HTMLTagParse *td, HTMLDocument *pDocument); 
	virtual ~HTMLTextArea();

	virtual HTMLElementType Type() { return HTML_EDITBOX_ELEMENT; }
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);

	virtual HTMLEventStatus DefaultEvent(HTMLEvent *pEvent);
	virtual void Reset(void);
	virtual void Select(void);
	virtual WEBC_CHAR *Value(void);
	virtual WEBC_CHAR *DefValue(void);
	virtual void SetValue(const WEBC_CHAR *pValue);
	virtual void SetDefValue(const WEBC_CHAR *pValue);

	void SetCols(int c);
	void SetRows(int r);
	void SetColsRows(int c, int r);
	int Cols() {return (miCols);}
	int Rows() {return (miRows);}
	WEBC_UINT8 Wrap();
	void SetWrap(WEBC_UINT8 w);

	virtual DisplayElement *GetDisplayElement (WEBC_BOOL create = WEBC_TRUE);
	virtual TextUnit       *GetTextUnit (void);	
	
protected:
	int miRows;
	int miCols;
	WEBC_CHAR *mpDefValue;
	WEBC_UINT8 mWrap;

	HTMLEditBoxDisplay *GetEditBoxDisplay (WEBC_BOOL create = WEBC_TRUE);
	HTMLEditBoxDisplay *mpEditBoxDisplay;
};


#endif // __HTXTAREA_HPP__
