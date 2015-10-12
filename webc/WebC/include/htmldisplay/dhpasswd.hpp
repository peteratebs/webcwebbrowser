#ifndef __DHPASSWD_HPP__
#define __DHPASSWD_HPP__

#include "dhelem.hpp"
#include "dhedit.hpp"
#include "wtpasswd.hpp"

class HTMLPasswordDisplay : public HTMLEditDisplay<WebPasswordString>
{
public:
	HTMLPasswordDisplay (HTMLElement *pElem);
	virtual ~HTMLPasswordDisplay ();

	virtual DISPLAY_INT GetBaseline(void);

protected:
	virtual WEBC_BOOL SizeIsValid (void);
	virtual void SetSizeValid (WEBC_BOOL valid = WEBC_TRUE);
	virtual DISPLAY_INT FindPixelWidth (HTMLContentBox *b, WebGraphics *gc);
};

#endif // __DHPASSWD_HPP__
