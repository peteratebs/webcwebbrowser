/*
|  WCFILTER.CPP -
| 
|  EBS - WebC
| 
| 
|  Copyright EBS Inc. , 2005
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

/*****************************************************************************/
/* Header files
 *****************************************************************************/

#include "wcfilter.hpp"
#include "webctypes.h"
#include "html.h"
#include "webcstr.h"
#include "webcutf.h"

/*****************************************************************************/
/* Macros
 *****************************************************************************/

/*****************************************************************************/
/* Types
 *****************************************************************************/

/*****************************************************************************/
/* Function Prototypes
 *****************************************************************************/

/*****************************************************************************/
/* Data
 *****************************************************************************/

/*****************************************************************************/
/* Function Definitions
 *****************************************************************************/

/*---------------------------------------------------------------------------*/
WebcCharsetDecodeFilter::WebcCharsetDecodeFilter (enum WebcCharset encoding)
{
	inputEncoding = encoding;
}

/*---------------------------------------------------------------------------*/
WebcDataFilterResult WebcCharsetDecodeFilter::go (
		unsigned char* dst, long dstLen, 
		const unsigned char* src, long srcLen)
{
	WebcDataFilterResult r;
	WebcCharDecodeStream decoder;
	
	decoder.pOutBuffer = (WEBC_CHAR*) dst;
	decoder.iOutBufferLen = dstLen / sizeof(WEBC_CHAR);
	decoder.iCharsWritten = 0;	
	decoder.pInBuffer =  src;
	decoder.iInBufferSize = srcLen;
	decoder.iBytesRead = 0;	
	decoder.charset = inputEncoding;
	
	webc_decode_string(&decoder);
	
	r.bytesRead = decoder.iBytesRead;
	r.bytesWritten = decoder.iCharsWritten * sizeof(WEBC_CHAR);
	
	return r;	
}

/*---------------------------------------------------------------------------*/
WebcDataFilterResult WebcTextToHTMLFilter::go (
		unsigned char* dst, long dstLen, 
		const unsigned char* src, long srcLen)
{
	WebcDataFilterResult r = {0, 0};

	WEBC_CHAR* dstChar = (WEBC_CHAR*) dst;
	const WEBC_CHAR* srcChar = (const WEBC_CHAR*) src;
	const char* replacement;
	long len;
	
	while (r.bytesRead < srcLen && r.bytesWritten < dstLen)
	{
		switch (*srcChar)
		{
			case '\"':
				replacement = "&quot;";
				len = 6;
				break;
				
			case '&':
				replacement = "&amp;";
				len = 5;
				break;
				
			case '<':
				replacement = "&lt;";
				len = 4;
				break;
				
			case '>':
				replacement = "&gt;";	
				len = 4;
				break;
				
			case (char) HTML_NBSP_CHAR:
				replacement = "&nbsp;";
				len = 6;
				break;
				
			case '\n':
				replacement = "<br>";
				len = 4;
				break;
			
			case '\r':
				replacement = "";
				len = 0;
				break;
				
			default:
				replacement = 0;
				len = 1;
				break;				
		}
		
		if (dstLen - r.bytesWritten < len * (long) sizeof(WEBC_CHAR))
		{
			break;
		}

		if (replacement)
		{
			webc_c_strncpy(dstChar, replacement, len);
		}
		else
		{
			*dstChar = *srcChar;		
		}

		dstChar += len;
		srcChar++;
		r.bytesWritten += len * sizeof(WEBC_CHAR);
		r.bytesRead += sizeof(WEBC_CHAR);
	}
	
	return r;	
}
