/*
|  WEBBMP.HPP - 
| 
|  EBS - WebC
| 
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/ 

#ifndef __WEBBMP_HPP__
#define __WEBBMP_HPP__


#include "webimage.hpp"
#include "bmpdec.h"

class WebcBMPDecoder : public WebcImageDecoder
{
protected:
	WEBC_UINT8* mImageData;
	BMPImageDecoder mDecoder;

public:
	WebcBMPDecoder();
	~WebcBMPDecoder();

	virtual WebcImage* go (WEBC_CHAR* key);
};

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __WEBBMP_HPP__ */
