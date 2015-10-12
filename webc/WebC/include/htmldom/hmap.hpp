#ifndef __HMAP_HPP__
#define __HMAP_HPP__

#include "helement.hpp"
#include "harea.hpp"

/*************************************************************************
* class HTMLMap
* 
*************************************************************************/

class HTMLMap : public HTMLElementContainer
{
public:
	HTMLMap(HTMLTagParse *td = 0, HTMLDocument *pDocument = 0);
	virtual ~HTMLMap();

	virtual HTMLElementType Type() { return HTML_MAP_ELEMENT; }
	virtual WEBC_UINT16 GetUnitType(void) { return TU_TYPE_NODISPLAY; }
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);

	virtual WEBC_CHAR *Name(void);
	virtual void  SetName(const WEBC_CHAR *pName);
	
	HTMLArea *GetArea(int x, int y);	
	
protected:
	WEBC_CHAR *mpName;
};

#endif // __HMAP_HPP__
