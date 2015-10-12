#ifndef __HAREA_HPP__
#define __HAREA_HPP__

#include "helement.hpp"
#include "dharea.hpp"

/*************************************************************************
* class HTMLArea
* 
*************************************************************************/

class HTMLArea : public HTMLElement
{
public:
	HTMLArea(HTMLTagParse *td = 0, HTMLDocument *pDocument = 0);
	virtual ~HTMLArea();

	virtual HTMLElementType Type() { return HTML_AREA_ELEMENT; }
	virtual WEBC_UINT16 GetUnitType(void) { return TU_TYPE_NODISPLAY; }
	virtual HTMLEventStatus DefaultEvent(HTMLEvent *pEvent);
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);

	virtual WEBC_CHAR *Name(void) { return (mpName); }
	virtual void  SetName(const WEBC_CHAR *pName);

	virtual WEBC_CHAR *HRef(void) { return mpHRef; }
	virtual void  SetHRef(WEBC_CHAR *pHRef);
	
	virtual WEBC_BOOL NoHRef(void) { return mbNoHRef; }
	virtual void  SetNoHRef(WEBC_BOOL bNoHRef) { mbNoHRef = bNoHRef; }

	virtual WEBC_CHAR *Target(void) { return mpTarget; }
	virtual void  SetTarget(WEBC_CHAR *pTarget);	

	virtual WEBC_CHAR*  Alt      (void) { return mAlt; }
    virtual void SetAlt      (const WEBC_CHAR *alt);

	virtual HTMLShapeType Shape (void) { return mShape; }
	virtual WEBC_CHAR *ShapeStr(void);
	virtual void SetShape (HTMLShapeType shape) { mShape = shape; }
	virtual void SetShape (const WEBC_CHAR *pStrShape);

	WEBC_UINT8 Contains(DISPLAY_INT x, DISPLAY_INT y);
	DisplayElement *GetAreaDisplay (HTMLElement *parent, WEBC_BOOL create = WEBC_TRUE);

	virtual long CoordinateCount(void) { return mCoordinateCount; }
	virtual HTMLLength *Coordinates(void) { return mCoordinateList; }
	virtual void SetCoordinates(WEBC_CHAR *pStrCoordValues);

	virtual WEBC_UINT16 AccessKey (void);
	virtual void SetAccessKey (WEBC_UINT16 key);

	virtual short TabIndex(void);	
	virtual void  SetTabIndex(short iIndex);	

protected:
	long             mCoordinateCount; // number of coordinates
	HTMLLength*      mCoordinateList;  // coordinate points
	HTMLShapeType    mShape;	        
	WEBC_CHAR *mpStrShape;
	HTMLAreaDisplay* mpAreaDisplayList;	
	WEBC_CHAR*        mpHRef;
	WEBC_CHAR*        mpName;		
	WEBC_CHAR*        mpTarget;
	WEBC_CHAR*     mAlt;
	WEBC_BOOL	mbNoHRef;
	WEBC_UINT16        mAccessKey;
	WEBC_INT16         mTabIndex;
};

#endif // __HAREA_HPP__
