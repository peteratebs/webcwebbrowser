#ifndef __WTPASSWD_HPP__
#define __WTPASSWD_HPP__

#include "wtedstr.hpp"

class WebPasswordString : public WebEditString
{
public:
	WebPasswordString ();
	virtual ~WebPasswordString ();
	
protected:
	virtual long GetIndexAtXY(DISPLAY_INT x, DISPLAY_INT y);
	virtual void DrawText(WebGraphics *gc, DISPLAY_INT x, DISPLAY_INT y, const WebChar *text, WebColor textColor, WebColor fillColor, WEBC_BOOL fill, WebFont font, int len=-1);
	virtual DISPLAY_INT PixelOffset (long index);
};


#endif // __WTEDSTR_HPP__
