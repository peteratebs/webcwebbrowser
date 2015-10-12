#ifndef __HFLOW_HPP__
#define __HFLOW_HPP__

#include "helement.hpp"

/*************************************************************************
* class HTMLFlow
* 
*************************************************************************/
 	
class HTMLFlowDisplay;
class ITextDisplayStyle;
 	
#define HFLOW_FLAG_TEXTALIGN                HELEM_FLAG_UNUSED_0
#define HFLOW_FLAG_CHILD_WRAP_CACHED        HELEM_FLAG_UNUSED_1
#define HFLOW_FLAG_CHILD_TEXTALIGN_CACHED   HELEM_FLAG_UNUSED_2
#define HFLOW_FLAG_BGIMGUPDATED             HELEM_FLAG_UNUSED_3
#define HFLOW_FLAG_COLOR_CACHED             HELEM_FLAG_UNUSED_4
#define HFLOW_FLAG_TEXT_DECORATION_CACHED   HELEM_FLAG_UNUSED_5
#define HFLOW_FLAG_BGCOLOR_CACHED           HELEM_FLAG_UNUSED_6
#define HFLOW_FLAG_UNUSED_0                 HELEM_FLAG_UNUSED_7
#define HFLOW_FLAG_UNUSED_1                 HELEM_FLAG_UNUSED_8
#define HFLOW_FLAG_UNUSED_2                 HELEM_FLAG_UNUSED_9
#define HFLOW_FLAG_UNUSED_3                 HELEM_FLAG_UNUSED_10

#define HFLOW_SET_CHILD_TEXTALIGN(X)  {mChildStyle &= ~0x3; mChildStyle |= (X & 0x3);}
#define HFLOW_GET_CHILD_TEXTALIGN     (mChildStyle & 0x3)
#define HFLOW_SET_CHILD_WRAP(X)       {mChildStyle &= ~0xc; mChildStyle |= ((X & 0x3) << 2);}
#define HFLOW_GET_CHILD_WRAP          ((mChildStyle >> 2) & 0x3)
#define HFLOW_SET_TEXT_DECORATION(X)  {mChildStyle &= ~0x70; mChildStyle |= ((X & 0x7) << 4);}
#define HFLOW_GET_TEXT_DECORATION     ((CSSTextDecoration)((mChildStyle >> 4) & 0x7))

// hflow.cpp
class HTMLFlow : public HTMLElementContainer
{
protected:
	virtual HTMLFlowDisplay * GetFlowDisplay (WEBC_BOOL create = WEBC_TRUE);

	WEBC_UINT8 ResolveFlowUnitType (void);

	// Inline Element Data
	WEBC_UINT8 mChildStyle;
	HTMLColor mColor;
	HTMLColor mBackgroundColor;
	ITextDisplayStyle* mTextStyle;

  #if(WEBC_SUPPORT_BACKGROUND_IMAGES)
	WebcImage *mpBackgroundImage;
	WEBC_CHAR *mpBackground;
  #endif
	
	CSSLength mWidth;
	
	HTMLFlowDisplay *mpFlowDisplay;

public:
	HTMLFlow(HTMLTagParse *td, HTMLDocument *pDocument);
	HTMLFlow(HTMLFlow &copy);
	virtual ~HTMLFlow();

	// Listener interface
	virtual void Notify (Notifier *who, NotifyEvent *what);

	// TextUnit interface
	virtual WEBC_UINT16 GetUnitType(void);
	virtual WEBC_UINT16 GetBreak(void);

	// Methods over-ridden from HTMLElement 
	virtual HTMLElementType Type() { return HTML_FLOW_ELEMENT; }
	virtual HTMLEventStatus DefaultEvent(HTMLEvent *pEvent);
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);
	virtual void GetWidth (CSSLength *width);
	virtual void LocalStyleUpdate (void);
	virtual void InheritedStyleUpdate (void);
	
	virtual void SetHtmlAlign(WEBC_UINT8 bAlign);

	// Properties needed for formatting/displaying this element

	// Background Images
  #if (WEBC_SUPPORT_BACKGROUND_IMAGES)	
  
	WEBC_CHAR*                 Background(void) { return mpBackground; }
	void                       SetBackground(const WEBC_CHAR *pUrl);
	WebcImage*                 GetBackgroundImage (void);
	CSSBackgroundRepeat        GetBackgroundRepeat (void);
	CSSBackgroundAttachment    GetBackgroundAttachment (void);
	void                       GetBackgroundXPos (CSSLength *pos);
	void                       GetBackgroundYPos (CSSLength *pos);
	void                       SetBackgroundImage (WebcImage *pBitmap);	
	
  #endif	// WEBC_SUPPORT_BACKGROUND_IMAGES
	
	virtual HTMLColor          GetBackgroundColor (void);
	virtual WEBC_UINT8         GetChildVAlign(void);
	virtual WEBC_UINT8         GetChildTextAlign(void);
	virtual WEBC_UINT16        GetChildWrap(void);	
	virtual CSSTextDecoration  GetTextDecoration (void);
	virtual HTMLColor          GetColor (void);
	virtual CSSBorderStyle     GetOutlineStyle(void);
	virtual ITextDisplayStyle* GetTextDisplayStyle (WEBC_BOOL create = WEBC_TRUE);
	virtual DisplayElement*    GetDisplayElement (WEBC_BOOL create = WEBC_TRUE);
	virtual TextUnit*          GetTextUnit (void);
};

#endif // __HFLOW_HPP__
