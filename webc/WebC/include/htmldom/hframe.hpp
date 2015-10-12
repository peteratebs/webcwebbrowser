#ifndef __HFRAME_HPP__
#define __HFRAME_HPP__

#include "webcfg.h"

#if (WEBC_SUPPORT_FRAMES)

#include "helement.hpp"
#include "dhframe.hpp"
#include "webcutf.h"

/*************************************************************************
* class HTMLFrame
* 
*************************************************************************/

class HTMLFrameSet;

#define HFRAME_FLAG_SRC_DIRTY       HELEM_FLAG_UNUSED_0
#define HFRAME_FLAG_BORDER          HELEM_FLAG_UNUSED_1
#define HFRAME_FLAG_BORDER_DEFINED  HELEM_FLAG_UNUSED_2
#define HFRAME_FLAG_QUARANTINED     HELEM_FLAG_UNUSED_3
#define HFRAME_FLAG_UNUSED_0        HELEM_FLAG_UNUSED_4

class HTMLFrame : public HTMLElement
{
public:
	HTMLFrame(HTMLTagParse *td, HTMLDocument *pDocument);
	virtual ~HTMLFrame();

	virtual HTMLElementType Type() { return HTML_FRAME_ELEMENT; }
	virtual HTMLElement *FindElement(HTMLElementFinder *pFinder, int iSearchFrames = 1, WEBC_BOOL bIncludeSelf=WEBC_TRUE);
	virtual int SetProperty(HTMLAttributeType Property, WEBC_CHAR *value, HTMLDocument *pDocument = 0, int call_base = 1);
	virtual void Update(WEBC_PFBYTE o, WEBC_PFBYTE data);
	virtual CSSValueType GetStyleFromHTML(CSSPropertyType Property, CSSPropertyValue *pValue);
	
	virtual void SetName(const WEBC_CHAR *pName);
	virtual WEBC_CHAR *Name();
	virtual void SetSrc(const WEBC_CHAR *pSrc);
	virtual WEBC_CHAR *Src();
	virtual void SetScrollMode(WEBC_CHAR *pScroll);
	virtual WEBC_CHAR *ScrollMode();
	virtual void SetFrameborder(WEBC_BOOL yes);
	virtual WEBC_BOOL Frameborder();
	virtual void SetBorder(WEBC_UINT8 b);
	virtual WEBC_UINT8 Border();
	virtual void SetLongDesc(const WEBC_CHAR *pLongDesc);
	virtual WEBC_CHAR *LongDesc();

	void SetFrameMarginWidth(WEBC_CHAR *pValue);
	void SetFrameMarginHeight(WEBC_CHAR *pValue);
	DISPLAY_INT GetFrameMarginWidth();
	DISPLAY_INT GetFrameMarginHeight();
	void SetNoResize(int iOn);
	void SetBorderColor(HTMLColor color);

	HTMLDocument *FrameDocument(void);
	HTMLFrameSet *GetFrameSet(void);
	
	virtual DisplayElement *GetDisplayElement (WEBC_BOOL create = WEBC_TRUE);
	virtual TextUnit       *GetTextUnit (void);

protected:
	HTMLDocument *mpFrameDocument;
	WEBC_CHAR *mpName;
	WEBC_CHAR *mpSrc;
	WEBC_CHAR *mpMode;
	DISPLAY_INT miFrameMarginWidth;
	DISPLAY_INT miFrameMarginHeight;
	WebcCharset mCharset;
	WEBC_CHAR *mpLongDesc;
	
	virtual HTMLFrameDisplay *GetFrameDisplay(WEBC_BOOL create = WEBC_TRUE);
	HTMLFrameDisplay *mpFrameDisplay;
};

#endif // WEBC_SUPPORT_FRAMES
#endif // __HFRAME_HPP__
