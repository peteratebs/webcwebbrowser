#ifndef __WEBJPEG_HPP__
#define __WEBJPEG_HPP__

#include "webimage.hpp"
#include "setjmp.h"

extern "C" 
{
#include "jpeglib.h"
}

class WebcJPEGDecoder;

struct WebJPEGSourceStruct
{
	jpeg_source_mgr pub;
	WebcJPEGDecoder * jpeg;
};

struct WebJPEGErrorHandlerStruct
{
	jpeg_error_mgr pub;           // standard libjpeg stuff
	jmp_buf jumpBuffer;           // the thing for longjmp
};


class WebcJPEGDecoder : public WebcImageDecoder
{
protected:
	int  InitSource(j_decompress_ptr decom);    
	void FreeSource(j_decompress_ptr decom);
	WEBC_UCHAR8 * mpImageData;
	WEBC_UINT16 muWidth, muHeight;

public:
	WebcJPEGDecoder();
	~WebcJPEGDecoder();

	virtual WebcImage * go(WEBC_CHAR *bitmapKey);

};
 
#endif //__WEBJPEG_HPP__
