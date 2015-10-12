/*
|  WCFILTER.HPP - 
| 
|  EBS - WebC
| 
|  Copyright EBS Inc. , 2005
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/ 

#ifndef __WCFILTER_HPP__
#define __WCFILTER_HPP__

#include "webcutf.h"

#ifdef __cplusplus

typedef struct s_WebcDataFilterResult
{
	long bytesRead;
	long bytesWritten;
}
WebcDataFilterResult;

class WebcDataFilter
{
 public:
	virtual WebcDataFilterResult go (unsigned char* dst, long dstLen, const unsigned char* src, long srcLen) = 0;
	virtual void reset () {};
};

class WebcCharsetDecodeFilter : public WebcDataFilter
{
 private:
	enum WebcCharset inputEncoding;
 
 public:
 	WebcCharsetDecodeFilter (enum WebcCharset encoding);
	virtual WebcDataFilterResult go (unsigned char* dst, long dstLen, const unsigned char* src, long srcLen);

	void setInputEncoding (enum WebcCharset encoding) { inputEncoding = encoding; }
};

class WebcTextToHTMLFilter : public WebcDataFilter
{
 public:
	virtual WebcDataFilterResult go (unsigned char* dst, long dstLen, const unsigned char* src, long srcLen);
};

extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __WCFILTER_HPP__ */
