//
// HDSTREAM.HPP - Structs, prototypes, and defines for HDSTREAM.CPP
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//

#ifndef __HDSTREAM_HPP_INCLUDED__
#define __HDSTREAM_HPP_INCLUDED__

#include "html.h"
#include "htmldom.hpp"
#include "htmlbld.hpp"
#include "webstr.hpp"

class HTMLDocument;

#define HDOC_STREAM_IN_WRITE 0x01
#define HDOC_STREAM_DELETED  0x02

const int MAX_WRITE_NESTING = 10;

class HTMLDocumentStream
{
public:
	HTMLDocumentStream(HTMLElement* rootElement);
	~HTMLDocumentStream();

	long Write(const WEBC_CHAR* pString);
	long WriteLine(const WEBC_CHAR* pString);
	long Write(const WEBC_CHAR* pString, long lStrLen);
	long WriteLine(const WEBC_CHAR* pString, long lStrLen);
	void Close(void);

	int InWrite(void) {return mbFlags & HDOC_STREAM_IN_WRITE;}

protected:
	HtmlDocumentBuilder  mBuilder;
	HTMLDocument*        mpDocument;
	WEBC_UINT8           mbFlags;
	int                  mInsideWrite;
	HTMLParseContext     mParseContext;
	WebString            mWriteBuffer[MAX_WRITE_NESTING];
#if(WEBC_DEBUG_PARSER)
	WebString            mParseInDataBuffer;    /* Raw data presented the Write */
	WebString            mParseOutDataBuffer;   /* Raw data after Parse */
#endif
};

#endif // __HDSTREAM_HPP_INCLUDED__
