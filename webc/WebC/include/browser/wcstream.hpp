/*
|  WCSTREAM.HPP - 
| 
|  EBS - WebC
| 
|  Copyright EBS Inc. , 2005
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/ 

#ifndef __WCSTREAM_HPP__
#define __WCSTREAM_HPP__

#include "webctypes.h"
#include "webload.hpp"
#include "wcfilter.hpp"

#ifdef __cplusplus

static const int DEFAULT_BUFFER_SIZE = 512; 

static const int SUCCESS = 0;
static const int ERROR_END_OF_FILE = 0;
static const int ERROR_BUFFER_ALLOC = -1;	
static const int ERROR_BAD_ARGS = -2;
static const int ERROR_BUFFER_TOO_SMALL = -3;

class WebcStream
{
 public:
};

class WebcInputStream : public WebcStream
{
 public:
	virtual long read (unsigned char* buffer, long maxBytes) = 0; 
};

class WebcBufferedInputStream : public WebcInputStream
{
 protected:
 	WebcInputStream* inputSource;
  	unsigned char* inputBuffer;
 	long inputBufferSize;
 	long inputBufferUsed;
 	
 	// reads as much data from the input stream as will fit
 	int fillBuffer (void);
 	
 	// throws out the given number of bytes at the beginning of
 	//  the buffer and moves remaining data to the beginning.
 	int compactBytes (long byteCount);	

 public:
	WebcBufferedInputStream (WebcInputStream* source, long bufferSize = DEFAULT_BUFFER_SIZE);
	~WebcBufferedInputStream ();
		
	virtual long read (unsigned char* buffer, long maxBytes) = 0;
	
	int resizeBuffer (long newSize);
	unsigned char* getBufferData () { return inputBuffer; } 
	long getBufferLength () { return inputBufferUsed; }
	void discardBufferedData () { inputBufferUsed = 0; }
};

class WebcSeededInputStream : public WebcInputStream
{
 private:
 	WebcInputStream* fallbackInputStream;
 	const unsigned char* seedData;
 	long seedLength;
 
 public: 
 	WebcSeededInputStream (WebcInputStream* fallback, const unsigned char* data, long length);
	virtual long read (unsigned char* buffer, long maxBytes);
	
	void setSeedData (const unsigned char* data, long length) { seedData = data; seedLength = length; }
	WEBC_BOOL hasMoreSeedData () { return seedLength > 0; }
};

class WebcURLInputStream : public WebcInputStream
{
 private:
 	UrlStreamCtx* urlContext;
 	
 public:
 	WebcURLInputStream (UrlStreamCtx* source);
 	
 	virtual long read (unsigned char* buffer, long maxBytes);
};

class WebcFilteredInputStream : public WebcBufferedInputStream
{
 protected:
 	WebcDataFilter* inputFilter;
 	
 public:
 	WebcFilteredInputStream (WebcDataFilter* filter, WebcInputStream* source, long bufferSize = DEFAULT_BUFFER_SIZE);

 	virtual long read (unsigned char* buffer, long maxBytes);
};

class WebcOutputStream : public WebcStream
{
 public:
	virtual long write (const unsigned char* data, long numBytes) = 0;
	virtual int flush (void) = 0;
};

class HTMLDocument;

class WebcHTMLDocumentOutputStream : public WebcOutputStream
{
 protected:
	HTMLDocument* document;

 public:
 	WebcHTMLDocumentOutputStream (HTMLDocument* doc) { document = doc; }
	virtual long write (const unsigned char* data, long numBytes);
	virtual int flush (void) { return SUCCESS; }
};

class WebcBufferedOutputStream : public WebcOutputStream
{
 protected:
	WebcOutputStream* outputStream;
	unsigned char* outputBuffer;
	long outputBufferSize;
	long outputBufferUsed;
 	
 public:
 	WebcBufferedOutputStream (WebcOutputStream* sink, long bufferSize = DEFAULT_BUFFER_SIZE);
 	~WebcBufferedOutputStream ();
	virtual long write (const unsigned char* data, long numBytes);
	virtual int flush (void);
};

class WebcFilteredOutputStream : public WebcBufferedOutputStream
{
 protected:
 	WebcDataFilter* outputFilter;
 	 
 public:
	WebcFilteredOutputStream (WebcDataFilter* filter, WebcOutputStream* sink, long bufferSize = DEFAULT_BUFFER_SIZE);
	virtual long write (const unsigned char* data, long numBytes);
};

extern "C" {
#endif

#ifdef __cplusplus
}
#endif

#endif /* __WCSTREAM_HPP__ */
