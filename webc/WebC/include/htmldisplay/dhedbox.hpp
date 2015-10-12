#ifndef __DHEDBOX_HPP__
#define __DHEDBOX_HPP__

#include "dhelem.hpp"
#include "dhedit.hpp"
#include "wtedbox.hpp"

class HTMLEditBoxDisplay : public HTMLEditDisplay<WebEditBox>
{
public:
	HTMLEditBoxDisplay (HTMLElement *pElem);
	virtual ~HTMLEditBoxDisplay ();

protected:
	virtual WEBC_BOOL SizeIsValid (void);
	virtual void SetSizeValid (WEBC_BOOL valid = WEBC_TRUE);
	virtual WEBC_BOOL SetElementStyle (void);
	virtual DISPLAY_INT FindPixelWidth (HTMLContentBox *b, WebGraphics *gc);
	virtual DISPLAY_INT FindPixelHeight (HTMLContentBox *b, DISPLAY_INT parentHeight, WebGraphics *gc);

	// Overridden from WebEditString
	virtual void GetTextRect(WebRect *rect);
};

#endif // __DHEDBOX_HPP__
