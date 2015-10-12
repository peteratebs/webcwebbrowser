#ifndef __WEBMNG_HPP_INCLUDED__
#define __WEBMNG_HPP_INCLUDED__

#include "webimage.hpp"
#include "libmng.h"

class WebcMNGDecoder : public WebcImageDecoder
{
protected:
	mng_handle mhMNGHandle;

public:
	WebcMNGDecoder();
	~WebcMNGDecoder();

	virtual WebcImage * go(WEBC_CHAR *bitmapKey);
	
	long readData (unsigned char *buffer, long size);
}; 

#endif // __WEBMNG_HPP_INCLUDED__
