//
// WEBCUTF.CPP - Charset conversion routines
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//  [tbd]
//

/*****************************************************************************/
// Header files
/*****************************************************************************/

#include "webcutf.h"
#include "webc.h"
#include "webcstr.h"
#include "webcsjis.h"
#include "webcbig5.h"
#include "webcGb2312.h"
#include "webceuckr.h"


/*****************************************************************************/
// Macros
/*****************************************************************************/

#define UTF8_LEN(C)  ((((C) & 0x80) == 0)? 1 : ((((C) & 0xE0) == 0xC0)? 2 : ((((C) & 0xF0) == 0xE0)? 3 : 4)))
#define UC16_LEN(C)  (((C) & 0xf800)? 3 : ((((C) & 0xff80)? 2 : 1)))

#define UTF8_TO_UC16_1(A)       ((unsigned short) ((A) & 0x7f))

#define UTF8_TO_UC16_2(A,B)   ((((unsigned short) ((A) & 0x1f)) << 6)  | \
                                ((unsigned short) ((B) & 0x3f)))

#define UTF8_TO_UC16_3(A,B,C) ((((unsigned short) ((A) & 0x0f)) << 12) | \
                               (((unsigned short) ((B) & 0x3f)) << 6) | \
                                ((unsigned short) ((C) & 0x3f)))

#define UC16_TO_UTF8_1(U,A)     {(A) = (char) ((U) & 0x007f);}

#define UC16_TO_UTF8_2(U,A,B)   { \
                                 (A) = (char) (0xC0 | (((U) & 0x07C0) >> 6)); \
                                 (B) = (char) (0x80 |  ((U) & 0x003F)); \
                                }

#define UC16_TO_UTF8_3(U,A,B,C) { \
                                 (A) = (char) (0xE0 | (((U) & 0xF000) >> 12)); \
                                 (B) = (char) (0x80 | (((U) & 0x0FC0) >> 6)); \
                                 (C) = (char) (0x80 |  ((U) & 0x003F)); \
                                }


/*****************************************************************************/
// Type definitions
/*****************************************************************************/

typedef long (*CharEncFnc)(WEBC_CHAR *outStr, const WEBC_CHAR *inStr, long n);
typedef long (*CharDecFnc)(WebcCharDecodeStream *stream);

struct EncTypeEntry
{
	char *name;
	enum WebcCharset encoding;
};


/*****************************************************************************/
// Function prototypes
/*****************************************************************************/

//////////////////////////////////////////////////////////////////
// Character Encoding/Decoding Conversion Function Declarations //
//////////////////////////////////////////////////////////////////

static long decode_ascii   (WebcCharDecodeStream *stream);
static long decode_utf8    (WebcCharDecodeStream *stream);
static long decode_unicode (WebcCharDecodeStream *stream);
static long decode_sjis    (WebcCharDecodeStream *stream);
static long decode_big5    (WebcCharDecodeStream *stream);
static long decode_gb2312    (WebcCharDecodeStream *stream);
static long decode_euckr   (WebcCharDecodeStream *stream);
static long decode_eucjp   (WebcCharDecodeStream *stream);

static long encode_ascii   (WEBC_CHAR *outStr, const WEBC_CHAR *inStr, long n);
static long encode_utf8    (WEBC_CHAR *outStr, const WEBC_CHAR *inStr, long n);
static long encode_unicode (WEBC_CHAR *outStr, const WEBC_CHAR *inStr, long n);
static long encode_sjis    (WEBC_CHAR *outStr, const WEBC_CHAR *inStr, long n);
static long encode_big5    (WEBC_CHAR *outStr, const WEBC_CHAR *inStr, long n);
static long encode_gb2312    (WEBC_CHAR *outStr, const WEBC_CHAR *inStr, long n);
static long encode_euckr   (WEBC_CHAR *outStr, const WEBC_CHAR *inStr, long n);
static long encode_eucjp   (WEBC_CHAR *outStr, const WEBC_CHAR *inStr, long n);


/*****************************************************************************/
// Global data
/*****************************************************************************/

WEBC_READONLY EncTypeEntry EncNameTable [] =
{
	{"us-ascii",     WEBC_CHARSET_ASCII},
	{"windows-1250", WEBC_CHARSET_WINDOWS1250},
	{"windows-1251", WEBC_CHARSET_WINDOWS1251},
	{"windows-1252", WEBC_CHARSET_WINDOWS1252},
	{"windows-1253", WEBC_CHARSET_WINDOWS1253},
	{"windows-1254", WEBC_CHARSET_WINDOWS1254},
	{"windows-1255", WEBC_CHARSET_WINDOWS1255},
	{"windows-1256", WEBC_CHARSET_WINDOWS1256},
	{"windows-1257", WEBC_CHARSET_WINDOWS1257},
	{"windows-1258", WEBC_CHARSET_WINDOWS1258},
	{"iso-8859-1",   WEBC_CHARSET_ISO88591},
	{"iso-8859-2",   WEBC_CHARSET_ISO88592},
	{"iso-8859-3",   WEBC_CHARSET_ISO88593},
	{"iso-8859-4",   WEBC_CHARSET_ISO88594},
	{"iso-8859-5",   WEBC_CHARSET_ISO88595},
	{"iso-8859-6",   WEBC_CHARSET_ISO88596},
	{"iso-8859-7",   WEBC_CHARSET_ISO88597},
	{"iso-8859-8",   WEBC_CHARSET_ISO88598I},
	{"iso-8859-8-i", WEBC_CHARSET_ISO88598I},
	{"iso-8859-9",   WEBC_CHARSET_ISO88599},
	{"iso-8859-10",  WEBC_CHARSET_ISO885910},
	{"iso-8859-15",  WEBC_CHARSET_ISO885915},
	{"iso-2022-jp",  WEBC_CHARSET_ISO2022JP},
	{"iso-2022-kr",  WEBC_CHARSET_ISO2022KR},
	{"k0i8",         WEBC_CHARSET_KOI8},
	{"k0i8-r",       WEBC_CHARSET_KOI8R},
	{"gb2312",       WEBC_CHARSET_GB2312},
	{"hz-gb-2312",   WEBC_CHARSET_HZGB2312},
	{"euc-cn",       WEBC_CHARSET_EUCCN},
	{"euc-tw",       WEBC_CHARSET_EUCTW},
	{"big5",         WEBC_CHARSET_BIG5},
	{"euc-kr",       WEBC_CHARSET_EUCKR},
	{"ksc5601",      WEBC_CHARSET_KSC5601},
	{"utf-8",        WEBC_CHARSET_UTF8},
	{"utf8",         WEBC_CHARSET_UTF8},
	{"tis-620",      WEBC_CHARSET_TIS620},
	{"windows-874",  WEBC_CHARSET_TIS620},
	{"euc-jp",       WEBC_CHARSET_EUCJP},
	{"shift_jis",    WEBC_CHARSET_SHIFTJIS},
	{"shift-jis",    WEBC_CHARSET_SHIFTJIS},
	{"x-sjis",       WEBC_CHARSET_SHIFTJIS},
	{"sjis",         WEBC_CHARSET_SHIFTJIS},
	{"s-jis",        WEBC_CHARSET_SHIFTJIS},
	{"unicode",      WEBC_CHARSET_UNICODE}
};

#define ENC_NAME_TABLE_SIZE (sizeof (EncNameTable) / sizeof (EncTypeEntry))

/* As functions are added to convert the various encodings below, uncomment the corresponding
   function names, and delete the corresponding "NULL" value.	*/

const CharEncFnc encode_table[] = {
                                encode_ascii,
                                encode_utf8,
                                encode_unicode,
                                0 /* encode_utf7 */,
                                0 /* encode_windows1250 */,
                                0 /* encode_windows1251 */,
                                encode_ascii /* TEST encode_windows1252 */,
                                0 /* encode_windows1253 */,
                                0 /* encode_windows1254 */,
                                0 /* encode_windows1255 */,
                                0 /* encode_windows1256 */,
                                0 /* encode_windows1257 */,
                                0 /* encode_windows1258 */,
                                encode_ascii /* encode_iso88591 */,
                                encode_ascii /* encode_iso88592 */,
                                encode_ascii /* encode_iso88593 */,
                                encode_ascii /* encode_iso88594 */,
                                encode_ascii /* encode_iso88595 */,
                                encode_ascii /* encode_iso88596 */,
                                encode_ascii /* encode_iso88597 */,
                                0 /* encode_iso88598I */,
                                0 /* encode_iso88599 */,
                                0 /* encode_iso885910 */,
                                0 /* encode_iso885915 */,
                                0 /* encode_iso2022jp */,
                                0 /* encode_iso2022kr */,
                                0 /* encode_koi8 */,
                                0 /* encode_koi8r */,
                                encode_gb2312,
                                0 /* encode_hzgb2312 */,
                                0 /* encode_euccn */,
                                0 /* encode_euctw */,
                                encode_big5,
                                encode_euckr,
                                0 /* encode_ksc5601 */,
                                0 /* encode_tis620 */,
                                encode_eucjp,
                                encode_sjis
};

const CharDecFnc decode_table[] = {
                                decode_ascii,
                                decode_utf8,
                                decode_unicode,
                                0 /* decode_utf7 */,
                                0 /* decode_windows1250 */,
                                0 /* decode_windows1251 */,
                                0 /* decode_windows1252 */,
                                0 /* decode_windows1253 */,
                                0 /* decode_windows1254 */,
                                0 /* decode_windows1255 */,
                                0 /* decode_windows1256 */,
                                0 /* decode_windows1257 */,
                                0 /* decode_windows1258 */,
                                decode_ascii /* decode_iso88591 */,
                                decode_ascii /* decode_iso88592 */,
                                decode_ascii /* decode_iso88593 */,
                                decode_ascii /* decode_iso88594 */,
                                decode_ascii /* decode_iso88595 */,
                                decode_ascii /* decode_iso88596 */,
                                decode_ascii /* decode_iso88597 */,
                                0 /* decode_iso88598I */,
                                0 /* decode_iso88599 */,
                                0 /* decode_iso885910 */,
                                0 /* decode_iso885915 */,
                                0 /* decode_iso2022jp */,
                                0 /* decode_iso2022kr */,
                                0 /* decode_koi8 */,
                                0 /* decode_koi8r */,
                                decode_gb2312,
                                0 /* decode_hzgb2312 */,
                                0 /* decode_euccn */,
                                0 /* decode_euctw */,
                                decode_big5,
                                decode_euckr,
                                0 /* decode_ksc5601 */,
                                0 /* decode_tis620 */,
                                decode_eucjp,
                                decode_sjis, // decode_shiftjis
};


/*****************************************************************************/
// Character Encoding/Decoding Utility Function Definitions
/*****************************************************************************/

long webc_encode_string (WEBC_CHAR *outStr, const WEBC_CHAR *inStr, long num, WebcCharset charset)
{
	int n = charset;

	n = EBSCLIP(n, 0, WEBC_NUM_CHARSETS-1);

	if (encode_table[n] != 0)
	{
		if (num == 0)
		{
			num = 0x7fffffff;
		}
		return (encode_table[n](outStr, inStr, num));
	}

	return 0;
}

long webc_decode_string (WebcCharDecodeStream *stream)
{
	int n = stream->charset;

	n = EBSCLIP(n, 0, WEBC_NUM_CHARSETS-1);

	if (decode_table[n] != 0)
	{
		return (decode_table[n](stream));
	}

	return (decode_ascii(stream));
}

#if (WEBC_SUPPORT_UNICODE)
extern "C" {
extern WEBC_READONLY WEBC_CHAR16 rHTMLExtendedCharEntityToUC[32];
}
#endif

// ascii to WEBC_CHAR
long decode_ascii (WebcCharDecodeStream *stream)
{
	long outPos, inPos;

	outPos = stream->iCharsWritten;
	inPos = stream->iBytesRead;

	while ((outPos < stream->iOutBufferLen) && (inPos < stream->iInBufferSize))
	{
	  #if (WEBC_SUPPORT_UNICODE)
		unsigned char i = stream->pInBuffer[inPos++];
		stream->pOutBuffer[outPos++] = (0x80 <= i && i <= 0x9f)? rHTMLExtendedCharEntityToUC[i-0x80] : (WEBC_CHAR) i;
	  #else
		stream->pOutBuffer[outPos++] = (WEBC_CHAR) stream->pInBuffer[inPos++];
	  #endif
	}

	stream->iCharsWritten = outPos;
	stream->iBytesRead = inPos;

	return (0);
}

// UTF-8 to WEBC_CHAR
long decode_utf8 (WebcCharDecodeStream *stream)
{
	unsigned char a;
  #if (WEBC_SUPPORT_UNICODE)
	unsigned char b, c;
  #endif
	int charLen;
	long outPos, inPos;

	outPos = stream->iCharsWritten;
	inPos = stream->iBytesRead;

	while (outPos < stream->iOutBufferLen)
	{
		charLen = UTF8_LEN(stream->pInBuffer[inPos]);
		if (charLen > stream->iInBufferSize - inPos)
		{
			break;
		}

		switch (charLen)
		{
			case 1:
				a = stream->pInBuffer[inPos];
				stream->pOutBuffer[outPos++] = (WEBC_CHAR) UTF8_TO_UC16_1(a);
				break;

		  #if (WEBC_SUPPORT_UNICODE)
			case 2:
				a = stream->pInBuffer[inPos];
				b = stream->pInBuffer[inPos+1];
				stream->pOutBuffer[outPos++] = UTF8_TO_UC16_2(a,b);
				break;

			case 3:
				a = stream->pInBuffer[inPos];
				b = stream->pInBuffer[inPos+1];
				c = stream->pInBuffer[inPos+2];
				stream->pOutBuffer[outPos++] = UTF8_TO_UC16_3(a,b,c);
				break;
            case 4:
                stream->pOutBuffer[outPos++] = 0x3f; // square
                break;
		  #endif // WEBC_SUPPORT_UNICODE
		}

		inPos += charLen;
	}

	stream->iCharsWritten = outPos;
	stream->iBytesRead = inPos;

	return (0);
}

// unicode to WEBC_CHAR
long decode_unicode (WebcCharDecodeStream *stream)
{
	unsigned short *inBuffer = (unsigned short *) stream->pInBuffer;
	long outPos, inPos, inBufferSize;

	outPos = stream->iCharsWritten;
	inPos = stream->iBytesRead / sizeof(unsigned short);
	inBufferSize = stream->iInBufferSize / sizeof(unsigned short);

	while ((outPos < stream->iOutBufferLen) && (inPos < inBufferSize))
	{
	  #if (WEBC_SUPPORT_UNICODE)
		stream->pOutBuffer[outPos++] = (WEBC_CHAR) inBuffer[inPos++];
	  #else
		if (inBuffer[inPos] < 128)
		{
			stream->pOutBuffer[outPos++] = (WEBC_CHAR) inBuffer[inPos];
		}
		inPos++;
	  #endif
	}

	stream->iCharsWritten = outPos;
	stream->iBytesRead = inPos * sizeof(unsigned short);

	return (0);
}

long decode_sjis (WebcCharDecodeStream *stream)
{
#if (WEBC_SUPPORT_SHIFTJIS)
	while ((stream->iCharsWritten < stream->iOutBufferLen) && (stream->iBytesRead < stream->iInBufferSize) &&
	       (stream->iInBufferSize - stream->iBytesRead >= JIS_CHAR_LEN(stream->pInBuffer[stream->iBytesRead])))
	{
		stream->iBytesRead += webc_jis_to_unicode(stream->pOutBuffer + stream->iCharsWritten,
		                                          stream->pInBuffer + stream->iBytesRead);
		stream->iCharsWritten++;
	}
#endif // WEBC_SUPPORT_SHIFTJIS

	return (0);
}

long decode_big5 (WebcCharDecodeStream *stream)
{
#if (WEBC_SUPPORT_BIG5)
	while ((stream->iCharsWritten < stream->iOutBufferLen) && (stream->iBytesRead < stream->iInBufferSize) &&
	       (stream->iInBufferSize - stream->iBytesRead >= BIG5_CHAR_LEN(stream->pInBuffer[stream->iBytesRead])))
	{
		stream->iBytesRead += webc_big5_to_unicode(stream->pOutBuffer + stream->iCharsWritten,
		                                           stream->pInBuffer + stream->iBytesRead);
		stream->iCharsWritten++;
	}
#endif // WEBC_SUPPORT_BIG5

	return (0);
}

long decode_gb2312 (WebcCharDecodeStream *stream)
{
#if (WEBC_SUPPORT_GB2312)
	while ((stream->iCharsWritten < stream->iOutBufferLen) && (stream->iBytesRead < stream->iInBufferSize) &&
	       (stream->iInBufferSize - stream->iBytesRead >= GB2312_CHAR_LEN(stream->pInBuffer[stream->iBytesRead])))
	{
		stream->iBytesRead += webc_gb2312_to_unicode(stream->pOutBuffer + stream->iCharsWritten,
		                                           stream->pInBuffer + stream->iBytesRead);
		stream->iCharsWritten++;
	}
#endif // WEBC_SUPPORT_GB2312

	return (0);
}

long decode_euckr (WebcCharDecodeStream *stream)
{
#if (WEBC_SUPPORT_EUC_KR)
	while ((stream->iCharsWritten < stream->iOutBufferLen) && (stream->iBytesRead < stream->iInBufferSize) &&
	       (stream->iInBufferSize - stream->iBytesRead >= EUC_KR_CHAR_LEN(stream->pInBuffer[stream->iBytesRead])))
	{
		stream->iBytesRead += webc_euc_kr_to_unicode(stream->pOutBuffer + stream->iCharsWritten,
		                                             stream->pInBuffer + stream->iBytesRead);
		stream->iCharsWritten++;
	}
#endif // WEBC_SUPPORT_SHIFTJIS

	return (0);
}

long decode_eucjp (WebcCharDecodeStream *stream)
{
#if (WEBC_SUPPORT_EUC_JP)
	while ((stream->iCharsWritten < stream->iOutBufferLen) && (stream->iBytesRead < stream->iInBufferSize) &&
	       (stream->iInBufferSize - stream->iBytesRead >= EUC_JP_CHAR_LEN(stream->pInBuffer[stream->iBytesRead])))
	{
		stream->iBytesRead += webc_euc_jp_to_unicode(stream->pOutBuffer + stream->iCharsWritten,
		                                             stream->pInBuffer + stream->iBytesRead);
		stream->iCharsWritten++;
	}
#endif // WEBC_SUPPORT_SHIFTJIS

	return (0);
}

long encode_ascii (WEBC_CHAR *outStr, const WEBC_CHAR *inStr, long n)
{
	long retval = EBSMIN(n, (long) webc_strlen(inStr));

	if (outStr)
	{
		webc_strncpy(outStr, inStr, retval);
	}

	return (retval);
}

long encode_utf8 (WEBC_CHAR *outStr, const WEBC_CHAR *inStr, long n)
{
	if (outStr)
	{
		long outPos = 0, inPos = 0;

		while (inStr[inPos] && (inPos < n))
		{
			switch (UC16_LEN(inStr[inPos]))
			{
				case 1:
					UC16_TO_UTF8_1(inStr[inPos],outStr[outPos]);
					outPos += 1;
					break;

				case 2:
					UC16_TO_UTF8_2(inStr[inPos],outStr[outPos],outStr[outPos+1]);
					outPos += 2;
					break;

				case 3:
					UC16_TO_UTF8_3(inStr[inPos],outStr[outPos],outStr[outPos+1],outStr[outPos+2]);
					outPos += 3;
					break;
			}
			inPos++;
		}

		return (outPos);
	}
	else
	{
		long outLen = 0, inPos = 0;

		while (inStr[inPos] && (inPos < n))
		{
			outLen += UC16_LEN(inStr[inPos]);
			inPos++;
		}

		return (outLen);
	}
}

long encode_unicode (WEBC_CHAR *outStr, const WEBC_CHAR *inStr, long n)
{
	long retval = EBSMIN(n,(long) webc_strlen(inStr));

	if (outStr)
	{
		webc_strncpy(outStr, inStr, retval);
	}

	return (retval);
}

long encode_sjis (WEBC_CHAR *outStr, const WEBC_CHAR *inStr, long n)
{
  #if (WEBC_SUPPORT_SHIFTJIS)
	if (outStr)
	{
		WEBC_UINT8 temp[2];
		long outPos = 0, inPos = 0, len;

		while (inStr[inPos] && (inPos < n))
		{
			len = webc_unicode_to_jis (temp, inStr[inPos]);
			switch (len)
			{
				case 1:
					outStr[outPos++] = temp[0];
					break;

				case 2:
					outStr[outPos++] = temp[0];
					outStr[outPos++] = temp[1];
					break;
			}
			inPos++;
		}

		return (outPos);
	}
	else
	{
		WEBC_UINT8 temp[2];
		long outLen = 0, inPos = 0;

		while (inStr[inPos] && (inPos < n))
		{
			outLen += webc_unicode_to_jis (temp, inStr[inPos]);
			inPos++;
		}

		return (outLen);
	}
  #else
	return (0);
  #endif
}

long encode_big5 (WEBC_CHAR *outStr, const WEBC_CHAR *inStr, long n)
{
  #if (WEBC_SUPPORT_BIG5)
	if (outStr)
	{
		WEBC_UINT8 temp[2];
		long outPos = 0, inPos = 0, len;

		while (inStr[inPos] && (inPos < n))
		{
			len = webc_unicode_to_big5 (temp, inStr[inPos]);
			switch (len)
			{
				case 1:
					outStr[outPos++] = temp[0];
					break;

				case 2:
					outStr[outPos++] = temp[0];
					outStr[outPos++] = temp[1];
					break;
			}
			inPos++;
		}

		return (outPos);
	}
	else
	{
		WEBC_UINT8 temp[2];
		long outLen = 0, inPos = 0;

		while (inStr[inPos] && (inPos < n))
		{
			outLen += webc_unicode_to_big5 (temp, inStr[inPos]);
			inPos++;
		}

		return (outLen);
	}
  #else
	return (0);
  #endif
}

long encode_gb2312 (WEBC_CHAR *outStr, const WEBC_CHAR *inStr, long n)
{
  #if (WEBC_SUPPORT_GB2312)
	if (outStr)
	{
		WEBC_UINT8 temp[2];
		long outPos = 0, inPos = 0, len;

		while (inStr[inPos] && (inPos < n))
		{
			len = webc_unicode_to_gb2312 (temp, inStr[inPos]);
			switch (len)
			{
				case 1:
					outStr[outPos++] = temp[0];
					break;

				case 2:
					outStr[outPos++] = temp[0];
					outStr[outPos++] = temp[1];
					break;
			}
			inPos++;
		}

		return (outPos);
	}
	else
	{
		WEBC_UINT8 temp[2];
		long outLen = 0, inPos = 0;

		while (inStr[inPos] && (inPos < n))
		{
			outLen += webc_unicode_to_gb2312 (temp, inStr[inPos]);
			inPos++;
		}

		return (outLen);
	}
  #else
	return (0);
  #endif
}

long encode_euckr (WEBC_CHAR *outStr, const WEBC_CHAR *inStr, long n)
{
  #if (WEBC_SUPPORT_EUC_KR)
	if (outStr)
	{
		WEBC_UINT8 temp[2];
		long outPos = 0, inPos = 0, len;

		while (inStr[inPos] && (inPos < n))
		{
			len = webc_unicode_to_euc_kr (temp, inStr[inPos]);
			switch (len)
			{
				case 1:
					outStr[outPos++] = temp[0];
					break;

				case 2:
					outStr[outPos++] = temp[0];
					outStr[outPos++] = temp[1];
					break;
			}
			inPos++;
		}

		return (outPos);
	}
	else
	{
		WEBC_UINT8 temp[2];
		long outLen = 0, inPos = 0;

		while (inStr[inPos] && (inPos < n))
		{
			outLen += webc_unicode_to_euc_kr (temp, inStr[inPos]);
			inPos++;
		}

		return (outLen);
	}
  #else
	return (0);
  #endif
}

long encode_eucjp (WEBC_CHAR *outStr, const WEBC_CHAR *inStr, long n)
{
  #if (WEBC_SUPPORT_EUC_JP)
	if (outStr)
	{
		WEBC_UINT8 temp[2];
		long outPos = 0, inPos = 0, len;

		while (inStr[inPos] && (inPos < n))
		{
			len = webc_unicode_to_euc_jp (temp, inStr[inPos]);
			switch (len)
			{
				case 1:
					outStr[outPos++] = temp[0];
					break;

				case 2:
					outStr[outPos++] = temp[0];
					outStr[outPos++] = temp[1];
					break;
			}
			inPos++;
		}

		return (outPos);
	}
	else
	{
		WEBC_UINT8 temp[2];
		long outLen = 0, inPos = 0;

		while (inStr[inPos] && (inPos < n))
		{
			outLen += webc_unicode_to_euc_jp (temp, inStr[inPos]);
			inPos++;
		}

		return (outLen);
	}
  #else
	return (0);
  #endif
}

enum WebcCharset webc_charset_lookup(const WEBC_CHAR *str)
{
int i;

	for (i=0; i<ENC_NAME_TABLE_SIZE; i++)
	{
		if (!webc_c_strnicmp(str, EncNameTable[i].name, tc_strlen(EncNameTable[i].name)))
		{
			return (EncNameTable[i].encoding);
		}
	}

	return (WEBC_CHARSET_UNKNOWN); // tbd - maybe this should default to UTF8 instead?
}

#if (WEBC_SUPPORT_UNICODE)
enum WebcCharset webc_charset_lookup_8(const char *str)
{
int i;

	for (i=0; i<ENC_NAME_TABLE_SIZE; i++)
	{
		if (!tc_strnicmp(str, EncNameTable[i].name, tc_strlen(EncNameTable[i].name)))
		{
			return (EncNameTable[i].encoding);
		}
	}

	return (WEBC_CHARSET_UNKNOWN); // tbd - maybe this should default to UTF8 instead?
}
#endif // WEBC_SUPPORT_UNICODE

const char *webc_charset_string(WebcCharset charSet)
{
	int i;

	for (i=0; i<ENC_NAME_TABLE_SIZE; i++)
	{
		if(EncNameTable[i].encoding == charSet)
			return (EncNameTable[i].name);
	}
	return WEBC_NULL;
}
