#ifndef __HBODY_HPP__
#define __HBODY_HPP__

#include "hflow.hpp"
#include "hframset.hpp"
#include "dhbody.hpp"

/*************************************************************************
* class HTMLBody
*
*************************************************************************/

#define HBODY_FLAG_BGCOLOR       HFLOW_FLAG_UNUSED_0
#define HBODY_FLAG_NOBACKGROUND  HFLOW_FLAG_UNUSED_1

// hbody.cpp
class HTMLBody : public HTMLFlow
{
public:
	HTMLBody(HTMLDocument *pDocument = 0, WEBC_BOOL bIsFragment = WEBC_FALSE);
	HTMLBody(HTMLTagParse *td, HTMLDocument *pDocument);
	~HTMLBody();

	virtual HTMLElementType Type() { return HTML_BODY_ELEMENT; }
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);
	virtual HTMLEventStatus DefaultEvent(HTMLEvent *pEvent);
	virtual void RestoreDefaults(void);
	virtual WEBC_UINT16 GetUnitType(void);
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);

	void SetTextColor(HTMLColor color);
	HTMLColor TextColor(void);
	void SetLinkColor(HTMLColor color);
	HTMLColor LinkColor(void);
	void SetALinkColor(HTMLColor color);
	HTMLColor ALinkColor(void);
	void SetVLinkColor(HTMLColor color);
	HTMLColor VLinkColor(void);
	void SetBodyBackgroundColor(HTMLColor color);
	HTMLColor BodyBackgroundColor(void);

	int Loaded(void) { return mbLoaded; }
	void SetLoaded(void) { mbLoaded = 1; }

	WEBC_BOOL IsFragment() { return mbIsFragment; }

	virtual DisplayElement *GetDisplayElement (WEBC_BOOL create = WEBC_TRUE);

  #if (WEBC_SUPPORT_FRAMES)
	HTMLFrameSet* GetFrameSet (void);
	void          SetFrameSet (HTMLFrameSet*);
  #endif // WEBC_SUPPORT_FRAMES

protected:
	HTMLColor mTextColor;
	HTMLColor mLinkColor;
	HTMLColor mALinkColor;
	HTMLColor mVLinkColor;
	HTMLColor mBodyBackgroundColor;
	WEBC_UINT8 mbLoaded;
	WEBC_BOOL mbIsFragment; // this is a "fake" body for implementing createDocumentFragment

  #if (WEBC_SUPPORT_FRAMES)
	ObjectReference<HTMLElement> mFrameSet;
  #endif
};


#endif // __HBODY_HPP__
