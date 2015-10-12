#ifndef __WEBCUTF_H_INCLUDED__
#define __WEBCUTF_H_INCLUDED__

#include "webctypes.h"

enum WebcCharset
{
	WEBC_CHARSET_UNKNOWN = -1,
	WEBC_CHARSET_ASCII = 0,     // ASCII
	WEBC_CHARSET_UTF8,			// Unicode
	WEBC_CHARSET_UNICODE,		// Unicode
	WEBC_CHARSET_UTF7,			// Unicode

	WEBC_CHARSET_WINDOWS1250,	// Central European	
	WEBC_CHARSET_WINDOWS1251,	// Cyrillic
	WEBC_CHARSET_WINDOWS1252,	// Western European	
	WEBC_CHARSET_WINDOWS1253,	// Greek
	WEBC_CHARSET_WINDOWS1254,	// Turkish		
	WEBC_CHARSET_WINDOWS1255,	// Hebrew
	WEBC_CHARSET_WINDOWS1256,	// Arabic
	WEBC_CHARSET_WINDOWS1257,	// Baltic						
	WEBC_CHARSET_WINDOWS1258,	// Vietnamese

	WEBC_CHARSET_ISO88591,		// Latin1 (Western European)
	WEBC_CHARSET_ISO88592,		// Latin2 (Central European)
	WEBC_CHARSET_ISO88593,		// Latin3 (Southern European)	
	WEBC_CHARSET_ISO88594,		// Latin4 (Baltic)	
	WEBC_CHARSET_ISO88595,		// Cyrillic	
	WEBC_CHARSET_ISO88596,		// Arabic
	WEBC_CHARSET_ISO88597,		// Greek
	WEBC_CHARSET_ISO88598I,		// Hebrew				
	WEBC_CHARSET_ISO88599,		// Latin5 (Turkish)
	WEBC_CHARSET_ISO885910,		// Latin6 (Nordic)
	WEBC_CHARSET_ISO885915,		// Latin9
	WEBC_CHARSET_ISO2022JP,		// Japanese
	WEBC_CHARSET_ISO2022KR,		// Korean	

	WEBC_CHARSET_KOI8,			// Cyrillic
	WEBC_CHARSET_KOI8R,			// Cyrillic
	WEBC_CHARSET_GB2312,		// Chinese (Simplified)
	WEBC_CHARSET_HZGB2312,		// Chinese (Simplified)	
	WEBC_CHARSET_EUCCN,			// Chinese (Simplified)
	WEBC_CHARSET_EUCTW,			// Chinese (Simplified)
	WEBC_CHARSET_BIG5,			// Chinese (Traditional)
	WEBC_CHARSET_EUCKR,			// Korean
	WEBC_CHARSET_KSC5601,		// Korean
	WEBC_CHARSET_TIS620,		// Thai	
	WEBC_CHARSET_EUCJP,			// Japanese	
	WEBC_CHARSET_SHIFTJIS,		// Japanese
	WEBC_NUM_CHARSETS
};

typedef struct _WebcCharDecodeStream
{
	WEBC_CHAR      *pOutBuffer;
	long           iOutBufferLen;
	long           iCharsWritten;
	
	const unsigned char *pInBuffer;
	long           iInBufferSize;
	long           iBytesRead;
	
	enum WebcCharset charset;
} WebcCharDecodeStream;

#if (WEBC_SUPPORT_UNICODE)
#define WEBC_DEFAULT_CHARSET  WEBC_CHARSET_UNICODE
#else
#define WEBC_DEFAULT_CHARSET  WEBC_CHARSET_ASCII
#endif


#ifdef __cplusplus
extern "C" {
#endif

/*****************************************************************************/
// Character Encoding/Decoding Utility Function Declarations 
/*****************************************************************************/

long webc_encode_string (WEBC_CHAR *outStr, const WEBC_CHAR *inStr, long n, enum WebcCharset charset);
long webc_decode_string (WebcCharDecodeStream *stream);
enum WebcCharset webc_charset_lookup(const WEBC_CHAR *str);
#if (WEBC_SUPPORT_UNICODE)
enum WebcCharset webc_charset_lookup_8(const char *str);
#else
#define webc_charset_lookup_8    webc_charset_lookup
#endif

const char *webc_charset_string(enum WebcCharset charSet);

#ifdef __cplusplus
}
#endif

#endif //__WEBCUTF_H_INCLUDED__
