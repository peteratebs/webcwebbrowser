#ifndef __HBASE_HPP__
#define __HBASE_HPP__

#include "helement.hpp"

/*************************************************************************
* class HTMLBase
* 
*************************************************************************/

class HTMLBase : public HTMLElement
{
public:
	HTMLBase(HTMLTagParse *td, HTMLDocument *pDocument);
	~HTMLBase();
	
	virtual HTMLElementType Type() { return HTML_BASE_ELEMENT; }	
	virtual WEBC_UINT16 GetUnitType(void) { return TU_TYPE_NODISPLAY; }
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);

	void SetHRef(WEBC_CHAR *pHRef);
	void SetTarget(WEBC_CHAR *pName);
	WEBC_CHAR *HRef(void);
	WEBC_CHAR *Target(void);
	
protected:
	WEBC_CHAR *mpHRef;
	WEBC_CHAR *mpTarget;
};

#endif // __HBASE_HPP__
