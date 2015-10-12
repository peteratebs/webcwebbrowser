#ifndef __DHBODY_HPP__
#define __DHBODY_HPP__

#include "dflow.hpp"
#include "keycatch.hpp"

#if (WEBC_SUPPORT_FRAMES)
class HTMLFrameSetDisplay;
#endif // WEBC_SUPPORT_FRAMES

class HTMLBody;

class HTMLBodyDisplay : public HTMLFlowDisplay
{
public:
	HTMLBodyDisplay(HTMLBody * f);
	virtual ~HTMLBodyDisplay ();

	virtual void GetBounds (WebRect* rect);
	virtual void Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC);
	virtual void Format(void);
	virtual void SetParentWidth(DISPLAY_INT iWidth, DISPLAY_INT iAvail);
	virtual void SetParentHeight(DISPLAY_INT iHeight);
	virtual FormatResult FormatForViewport (WebRect* viewportRect, WEBC_BOOL hscrollPresent, WEBC_BOOL vscrollPresent);

	void SetHtmlFocus (HTMLElement *pElement);
	void RemoveHtmlFocus (HTMLElement *pElement);
	void RemoveHtmlElementReferences (HTMLElement *pElement);

// <EDIT 20080201 - Add GetBackgroundColor (void);
// <EDIT GetBackgroundColor et al
//    WebColor HTMLBodyDisplay::GetBackgroundColor (void);
//    WEBC_BOOL HTMLBodyDisplay::HasBackgroundColor (void);


	virtual WEBC_BOOL NeedsFormat (void);

protected:
	virtual WEBC_BOOL IgnoreInFindMinWidth(TextUnit *pTU);

	virtual void GetSpecifiedHeight (
			DISPLAY_INT* heightValue,
			WEBC_BOOL* boolIsAuto
		);

	virtual void UpdateHeight (
		DISPLAY_INT topPad,
		DISPLAY_INT bottomPad
	);

	virtual void SetLocalStyleModified (void);
	virtual void SetPosChildStyleModified (void);
	virtual void SetFlowChildStyleModified (void);

  #if (WEBC_SUPPORT_FRAMES)

  HTMLFrameSetDisplay * mpFrameSet;
  #endif // WEBC_SUPPORT_FRAMES
	DisplayKeyCatcher    mKeyCatcher1;
	DisplayKeyCatcher    mKeyCatcher2;
};

#endif // __DHBODY_HPP__
