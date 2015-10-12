#ifndef __DHEDSTR_HPP__
#define __DHEDSTR_HPP__

#include "dhelem.hpp"
#include "dhedit.hpp"
#include "wtedstr.hpp"

class HTMLContentBox;

class HTMLEditStringDisplay : public HTMLEditDisplay<WebEditString>
{
public:
	HTMLEditStringDisplay (HTMLElement *pElem);
	virtual ~HTMLEditStringDisplay ();

	virtual DISPLAY_INT GetBaseline (void);

protected:
	virtual WEBC_BOOL SizeIsValid (void);
	virtual void SetSizeValid (WEBC_BOOL valid = WEBC_TRUE);
	virtual DISPLAY_INT FindPixelWidth (HTMLContentBox *b, WebGraphics *gc);
};

#endif // __DHEDSTR_HPP__
