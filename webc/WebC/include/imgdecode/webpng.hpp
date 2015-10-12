#ifndef __WEBPNG_HPP_INCLUDED__
#define __WEBPNG_HPP_INCLUDED__

#include "webimage.hpp"
#include "png.h"

class WebcPNGDecoder : public WebcImageDecoder
{
protected:
	png_structp mpPngData;
	png_infop   mpPngInfo;

	WEBC_INT32 miWidth;
	WEBC_INT32 miHeight;
	WEBC_INT32 miNumChannel;
	int miDepth;
	int miColorType;

	WEBC_UCHAR8* mpImageData;

public:
	WebcPNGDecoder();
	~WebcPNGDecoder();

	virtual WebcImage * go(WEBC_CHAR *key);


};

#endif // __WEBPNG_HPP_INCLUDED__
