#ifndef __HANCHOR_HPP__
#define __HANCHOR_HPP__

#include "hflow.hpp"
#include "webcutf.h"

/*************************************************************************
* class HTMLAnchor
* 
*************************************************************************/
class JScriptFunction;

typedef WEBC_UINT8 HANCHOR_STATE;

#define HANCHOR_STATE_NORMAL        0x00
#define HANCHOR_STATE_ACTIVE        0x01
#define HANCHOR_STATE_VISITED       0x02
#define HANCHOR_STATE_HOVER         0x04
#define HANCHOR_STATE_FOCUS         0x08

#define HTMLANCHOR_FLAG_COLORVALID	0x1
#define HTMLANCHOR_FLAG_ACOLORVALID 0x2

// hanchor.cpp
class HTMLAnchor : public HTMLFlow
{
public:
	HTMLAnchor(HTMLTagParse *td, HTMLDocument *pDocument);
	HTMLAnchor(HTMLAnchor &copy);
	virtual ~HTMLAnchor(); 

	// TextUnit Interface
	virtual HTMLElementType Type() { return HTML_ANCHOR_ELEMENT; }
	
	// Overridden from HTMLElement
	virtual HTMLEventStatus DefaultEvent(HTMLEvent *pEvent);
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);
	
	// Anchor attribute methods
	virtual WEBC_CHAR *HRef(void);
	virtual void  SetHRef(WEBC_CHAR *pHRef);

	virtual WEBC_CHAR *Name(void);
	virtual void  SetName(const WEBC_CHAR *pName);

	virtual short TabIndex(void);	
	virtual void  SetTabIndex(short iIndex);	

	virtual WEBC_CHAR *Target(void);
	virtual void  SetTarget(WEBC_CHAR *pTarget);	

	virtual WEBC_UINT16 AccessKey (void) { return mAccessKey; }
	virtual void SetAccessKey (WEBC_UINT16 key) { mAccessKey = key; }

	virtual WEBC_CHAR *CharSet(void);
	virtual WEBC_BOOL SetCharSet(const WEBC_CHAR *pCharSet);

	virtual long CoordinateCount(void);
	virtual HTMLLength *Coordinates(void);
	virtual void SetCoordinates(WEBC_CHAR *pStrCoordValues);

	virtual WEBC_CHAR *Rel(void);
	virtual void  SetRel(WEBC_CHAR *pRel);	

	virtual WEBC_CHAR *Rev(void);
	virtual void  SetRev(WEBC_CHAR *pRev);	

	virtual WEBC_CHAR *HRefLang(void);
	virtual void  SetHRefLang(WEBC_CHAR *pHRefLang);	

	virtual HTMLShapeType Shape (void) { return mShape; }
	virtual void SetShape (HTMLShapeType shape) { mShape = shape; }
	virtual WEBC_CHAR *ShapeStr(void);
	virtual void SetShape (const WEBC_CHAR *pStrShape);

	virtual WEBC_CHAR *MimeType(void);
	virtual void  SetMimeType(WEBC_CHAR *pMimeType);	

#if (WEBC_SUPPORT_STYLE_SHEETS)
	 int InPseudoClass(CSSPseudoClass ps);
#endif

protected:	
	WEBC_CHAR *mpHRef;
	WEBC_CHAR *mpName;
	WEBC_CHAR *mpTarget;
	short miTabIndex;
	HANCHOR_STATE mAnchorState;
	WebcCharset mCharset;
	WEBC_CHAR *mpStrCharset;
	WEBC_UINT16 mAccessKey;
	long mCoordinateCount; // number of coordinates
	HTMLLength *mCoordinateList;  // coordinate points
	WEBC_CHAR *mpRel;
	WEBC_CHAR *mpRev;
	WEBC_CHAR *mpHRefLang;
	HTMLShapeType    mShape;
	WEBC_CHAR *mpStrShape;
	WEBC_CHAR *mpMimeType;

	void PopulateCoordinates();
};

#endif // __HANCHOR_HPP__
