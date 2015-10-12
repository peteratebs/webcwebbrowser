//
// WEBSTR.CPP - WebC String Class
//
// EBS -
//
//
// Copyright EBS Inc. , 2004
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//

/*****************************************************************************/
// Header files
/*****************************************************************************/

#include "css.h"
#include "webstr.hpp"
#include "webcstr.h"
#include "html.h"
#include "webcassert.h"
#include "webcmem.h"

/*****************************************************************************/
// Macros
/*****************************************************************************/

#define BUFFER_SIZE_THRESHOLD     0x3fff  // 16k

/*****************************************************************************/
// Types
/*****************************************************************************/

/*****************************************************************************/
// Function Prototypes
/*****************************************************************************/

long _exactAsciiFilter (
		WEBC_CHAR* filteredStr,
		const void* sourceStr,
		long size
	);

long _exactUnicodeFilter (
		WEBC_CHAR* filteredStr,
		const void* sourceStr,
		long size
	);

long _escapeHtmlAsciiFilter (
		WEBC_CHAR* filteredStr,
		const void* sourceStr,
		long size
	);

long _escapeHtmlUnicodeFilter (
		WEBC_CHAR* filteredStr,
		const void* sourceStr,
		long size
	);

/*****************************************************************************/
// Data
/*****************************************************************************/

static WEBC_CHAR gNullStr[1] = {0};

/*****************************************************************************/
// Function Definitions
/*****************************************************************************/

/*---------------------------------------------------------------------------

  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
WebString::WebString (
		long initSize
	)
{
	mStringStart = gNullStr;
	mStringLength = 0;
	mBufferStart = 0;
	mBufferSize = initSize;
}

/*---------------------------------------------------------------------------

  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
WebString::~WebString (
		void
	)
{
	freeBuffer();
}

/*---------------------------------------------------------------------------

  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
int WebString::appendFiltered (
		const void* data,
		long size,
		WebStringFilter filter
	)
{
	long numChars = filter(0, data, size);

	if (numChars >= 0)
	{
		if (allocAtLeast(numChars + mStringLength, 0, numChars) < 0)
		{
			return (-1);
		}

		WEBC_ASSERT(mBufferSize > numChars + mStringLength);
		WEBC_ASSERT((mStringStart - mBufferStart) + mStringLength + numChars + 1 <= mBufferSize);

		mStringLength += filter(mStringStart + mStringLength, data, size);
		mStringStart[mStringLength] = 0;

		return (0);
	}

	return (-1);
}

/*---------------------------------------------------------------------------

  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
int WebString::append (
		const char* data,
		long numChars
	)
{
	return appendFiltered(data, numChars, _exactAsciiFilter);
}

/*---------------------------------------------------------------------------

  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
int WebString::append (
		const unsigned short* data,
		long numChars
	)
{
	return appendFiltered(data, numChars, _exactUnicodeFilter);
}

/*---------------------------------------------------------------------------

  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
int WebString::append (
		WebString* str
	)
{
	return (append((const WEBC_CHAR*) str->getChars(), (long) str->getLength()));
}

/*---------------------------------------------------------------------------

  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
int WebString::prepend (
		const char* data,
		long numChars
	)
{
	return prependFiltered(data, numChars, _exactAsciiFilter);
}


/*---------------------------------------------------------------------------

  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
int WebString::prepend (
		const unsigned short* data,
		long numChars
	)
{
	return prependFiltered(data, numChars, _exactUnicodeFilter);
}

/*---------------------------------------------------------------------------

  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
int WebString::prependFiltered (
		const void* data,
		long size,
		WebStringFilter filter
	)
{
	long numChars = filter(0, data, size);

	if (numChars >= 0)
	{
		if (allocAtLeast(numChars + mStringLength, numChars, 0) < 0)
		{
			return (-1);
		}

		WEBC_ASSERT(mBufferSize > numChars + mStringLength);
		WEBC_ASSERT(mStringStart - mBufferStart >= numChars);

		mStringLength += numChars;
		mStringStart -= numChars;
		filter(mStringStart, data, size);

		return (0);
	}

	return (-1);
}

/*---------------------------------------------------------------------------

  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
int WebString::prepend (
		WebString* str
	)
{
	return (prepend((const WEBC_CHAR*)str->getChars(), (long) str->getLength()));
}

/*---------------------------------------------------------------------------

  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
int WebString::copy (
		const char* str
	)
{
	return (copy(str, tc_strlen(str)));
}

/*---------------------------------------------------------------------------

  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
int WebString::copy (
		const unsigned short* str
	)
{
	return (copy(str, tc_wcslen(str)));
}

/*---------------------------------------------------------------------------

  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
int WebString::copy (
		const char* str,
		long numChars
	)
{
	if (numChars >= 0)
	{
		discardAll();
		return (append(str, numChars));
	}

	return (-1);
}

/*---------------------------------------------------------------------------

  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
int WebString::copy (
		const unsigned short* str,
		long numChars
	)
{
	if (numChars >= 0)
	{
		discardAll();
		return (append(str, numChars));
	}

	return (-1);
}

/*---------------------------------------------------------------------------

  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
int WebString::copy (
		WebString* str
	)
{
	return (copy(str->getChars(), str->getLength()));
}

/*---------------------------------------------------------------------------

  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
WEBC_CHAR* WebString::getChars (
		void
	)
{
	return (mStringStart);
}

/*---------------------------------------------------------------------------

  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
long WebString::getLength (
		void
	)
{
	return (mStringLength);
}

/*---------------------------------------------------------------------------

  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
void WebString::discardPrefix (
		long numChars
	)
{
	if (numChars >= 0)
	{
		if (numChars >= mStringLength)
		{
			discardAll();
		}
		else
		{
			mStringStart += numChars;
			mStringLength -= numChars;
		}
	}
}

/*---------------------------------------------------------------------------

  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
void WebString::discardPostfix (
		long numChars
	)
{
	if (numChars >= 0)
	{
		if (numChars >= mStringLength)
		{
			discardAll();
		}
		else
		{
			mStringLength -= numChars;
			mStringStart[mStringLength] = 0;
		}
	}
}

/*---------------------------------------------------------------------------

  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
void WebString::discardAll (
		void
	)
{
	mStringLength = 0;

	if (mBufferStart)
	{
		mStringStart = mBufferStart;
		mStringStart[0] = 0;
	}
	else
	{
		mStringStart = gNullStr;
	}
}

/*---------------------------------------------------------------------------

  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
int WebString::allocAtLeast (
		long numChars,
		long spaceBeforeString,
		long spaceAfterString
	)
{
	/* make sure all values are legal and that everything will fit */
	WEBC_ASSERT(numChars >= 0);
	WEBC_ASSERT(spaceBeforeString >= 0);
	WEBC_ASSERT(spaceAfterString >= 0);
	WEBC_ASSERT(spaceBeforeString + mStringLength + spaceAfterString <= numChars);

	if (numChars >= 0)
	{
		if (!mBufferStart)
		{
			if (mBufferSize < BUFFER_SIZE_THRESHOLD)
			{
				numChars <<= 1;
			}
			else
			{
				// round up to the nearest multiple of BUFFER_SIZE_THRESHOLD
				numChars = (numChars + BUFFER_SIZE_THRESHOLD - 1) - (numChars % BUFFER_SIZE_THRESHOLD);
			}

			// this is the first time; alloc a buffer
			if (numChars < mBufferSize)
			{
				numChars = mBufferSize;
			}

			mBufferStart = (WEBC_CHAR*) WEBC_MALLOC(sizeof(WEBC_CHAR) * (numChars + 1));
			if (mBufferStart)
			{
				mBufferSize = (numChars + 1);
				mStringLength = 0;
				mStringStart = mBufferStart + spaceBeforeString;
				mStringStart[0] = 0;
				return (0);
			}
		}
		else if (mBufferSize <= numChars)
		{
			// realloc the buffer to be larger

			if (mBufferSize < BUFFER_SIZE_THRESHOLD)
			{
				numChars <<= 1;
			}
			else
			{
				// round up to the nearest multiple of BUFFER_SIZE_THRESHOLD
				numChars = (numChars + BUFFER_SIZE_THRESHOLD - 1) - (numChars % BUFFER_SIZE_THRESHOLD);
			}

			WEBC_CHAR* p = (WEBC_CHAR*) WEBC_MALLOC(sizeof(WEBC_CHAR) * (numChars + 1));
			if (p)
			{
				WEBC_CHAR* s = p;

				mBufferSize = (numChars + 1);

				if (spaceBeforeString > 0)
				{
					s = p + spaceBeforeString;
				}
				else if (spaceAfterString > 0)
				{
					s = p + (mBufferSize - (spaceAfterString + mStringLength + 1));
				}

				/* copy string data if there is any */
				if (mStringLength > 0)
				{
					tc_memcpy(s, mStringStart, sizeof(WEBC_CHAR) * (mStringLength + 1));
				}
				else
				{
					s[0] = 0;
				}

				WEBC_FREE(mBufferStart);

				mBufferStart = p;
				mStringStart = s;

				return (0);
			}
		}
		else
		{
			if (mStringLength > 0)
			{
				long currentOffset = mStringStart - mBufferStart;

				WEBC_ASSERT(mBufferStart);

				if (currentOffset < spaceBeforeString)
				{
					rtp_memmove(mBufferStart + spaceBeforeString, mStringStart, sizeof(WEBC_CHAR) * (mStringLength + 1));
					mStringStart = mBufferStart + spaceBeforeString;
				}
				else
				{
					long newStartOffset = mBufferSize - (spaceAfterString + mStringLength + 1);

					if (currentOffset > newStartOffset)
					{
						rtp_memmove(mBufferStart + newStartOffset, mStringStart, sizeof(WEBC_CHAR) * (mStringLength + 1));
						mStringStart = mBufferStart + newStartOffset;
					}
				}
			}
			else
			{
				mStringStart = mBufferStart + spaceBeforeString;
				mStringStart[0] = 0;
			}

			return (0);
		}
	}

	return (-1);
}

/*---------------------------------------------------------------------------

  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
void WebString::freeBuffer (
		void
	)
{
	if (mBufferStart)
	{
		WEBC_FREE(mBufferStart);
		mBufferStart = 0;
		mStringStart = gNullStr;
		mStringLength = 0;
	}
}

/*---------------------------------------------------------------------------

  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
void WebString::truncate (
		long length
	)
{
	if (length >= 0 && length < mStringLength)
	{
		mStringLength = length;
		mStringStart[mStringLength] = 0;
	}
}

/*---------------------------------------------------------------------------
  WebString::collapseWhitespace
  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
void WebString::collapseWhitespace (
		void
	)
{
	if (mStringLength > 0)
	{
		mStringLength = CollapseWhitespace(mStringStart, mStringStart);
	}
}


/*---------------------------------------------------------------------------
  WebString::unescapeHtmlChars
  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
void WebString::unescapeHtmlChars (
		void
	)
{
	if (mStringLength > 0)
	{
		mStringLength = HTML_UnescapeString(mStringStart, mStringStart);
	}
}


/*---------------------------------------------------------------------------
  WebString::crLfToLf
  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
void WebString::crLfToLf (
		void
	)
{
	if (mStringLength > 0)
	{
		WEBC_CHAR* from = mStringStart;
		WEBC_CHAR *to = mStringStart;
		while (*from)
		{
			if ((from[0] == '\r') && (from[1] == '\n'))
			{
				*to = '\n';
				from += 2;
				to++;
				mStringLength--;
			}
			else
			{
				*to = *from;
				from++;
				to++;
			}
		}
		*to = 0;
	}
}

/*---------------------------------------------------------------------------
  WebString::removeLeadingCrLfs
  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
void WebString::removeLeadingCrLfs (
		void
	)
{
	WEBC_CHAR* src = mStringStart;
	WEBC_CHAR* dest = mStringStart;
	WEBC_BOOL bLeading = WEBC_TRUE;

	mStringLength = 0;

	while (*src)
	{
		if(bLeading)
		{
			if ((*src == '\r') || (*src == '\n'))
			{
				src++;
				continue;
			}
			else
			{
				bLeading = WEBC_FALSE;
			}
		}
		*dest++ = *src++;
		mStringLength++;
	}
	*dest = '\0';
}
/*---------------------------------------------------------------------------
  WebString::removeChars
  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
void WebString::removeChars (
		WEBC_CHAR* badChars
	)
{
	long n;
	WEBC_CHAR* src = mStringStart;
	WEBC_CHAR* dest = mStringStart;

	while (*src)
	{
		for (n=0; badChars[n] != '\0'; n++)
		{
			if (badChars[n] == *src)
			{
				break;
			}
		}
		if (badChars[n] == '\0')
		{
			*dest = *src;
			dest++;
		}
		else
		{
			mStringLength--;
		}
		src++;
	}
	*dest = '\0';
}

/*---------------------------------------------------------------------------
    WebString::removeTrailingNulls
  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
void WebString::removeTrailingNulls (
		void
	)
{
	if (mBufferStart && mStringLength > 0)
	{
		while (mStringLength > 0 && mStringStart[mStringLength-1] == 0)
		{
//            printf("Changing mStringLength from %d to %d\n", mStringLength, mStringLength-1);
			mStringLength--;
		}
	}
}

/*---------------------------------------------------------------------------
  WebString::replaceChars
  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
void WebString::replaceChars (
		WEBC_CHAR replaceThisChar,
		WEBC_CHAR withThisChar
	)
{
	for (long n=0; n < mStringLength; n++)
	{
		if (mStringStart[n] == replaceThisChar)
		{
			mStringStart[n] = withThisChar;
		}
	}
}

/*---------------------------------------------------------------------------
  WebString::removeLeadingWhitespace
  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
void WebString::removeLeadingWhitespace (
		void
	)
{
	if (mBufferStart)
	{
		while (mStringLength > 0 && IS_WHITESPACE(*mStringStart))
		{
			mStringStart++;
			mStringLength--;
		}
	}
}

/*---------------------------------------------------------------------------
  WebString::removeTrailingWhitespace
  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
void WebString::removeTrailingWhitespace (
		void
	)
{
	if (mBufferStart)
	{
		while (mStringLength > 0 && IS_WHITESPACE(mStringStart[mStringLength]))
		{
			mStringLength--;
		}
		mStringStart[mStringLength] = 0;
	}
}

/*---------------------------------------------------------------------------
  WebString::appendHtmlEscaped
  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
int WebString::appendHtmlEscaped (
		const char* asciiData,
		long size
	)
{
	return appendFiltered(asciiData, size, _escapeHtmlAsciiFilter);
}

/*---------------------------------------------------------------------------
  WebString::appendHtmlEscaped
  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
int WebString::appendHtmlEscaped (
		const unsigned short* asciiData,
		long size
	)
{
	return appendFiltered(asciiData, size, _escapeHtmlUnicodeFilter);
}

/*---------------------------------------------------------------------------*/
const char* _getHtmlEntityName (unsigned short ch)
{
	switch (ch)
	{
		case '\"':
			return ("&quot;");

		case '&':
			return ("&amp;");

		case '<':
			return ("&lt;");

		case '>':
			return ("&gt;");

		case (unsigned short) HTML_NBSP_CHAR:
			return ("&nbsp;");

		default:
			break;
	}

	return (0);
}

/*---------------------------------------------------------------------------*/
long _exactAsciiFilter (
		WEBC_CHAR* filteredStr,
		const void* data,
		long size
	)
{
	const char* sourceStr = (const char*) data;

	if (filteredStr)
	{
	  #if (WEBC_SUPPORT_UNICODE)
		webc_c_strncpy(filteredStr, sourceStr, size);
	  #else
		tc_memcpy(filteredStr, sourceStr, size * sizeof(WEBC_CHAR));
	  #endif
	}

	return (size);
}

/*---------------------------------------------------------------------------*/
long _exactUnicodeFilter (
		WEBC_CHAR* filteredStr,
		const void* data,
		long size
	)
{
	const unsigned short* sourceStr = (const unsigned short*) data;

	if (filteredStr)
	{
	  #if (WEBC_SUPPORT_UNICODE)
		tc_memcpy(filteredStr, sourceStr, size * sizeof(WEBC_CHAR));
	  #else
		int n;

		for (n=0; n<size; n++)
		{
			if (sourceStr[n] < 256)
			{
				filteredStr[n] = (WEBC_CHAR) sourceStr[n];
			}
			else
			{
				// tbd - use codepages here; use code from EBS's CIFS/SMB product
				filteredStr[n] = (WEBC_CHAR) '?';
			}
		}
	  #endif
	}

	return (size);
}

/*---------------------------------------------------------------------------*/
long _escapeHtmlAsciiFilter (
		WEBC_CHAR* filteredStr,
		const void* data,
		long size
	)
{
	const char* sourceStr = (const char*) data;
	long pos, epos;
	const char* htmlEntity;

	for (pos = 0, epos = 0; pos < size; pos++)
	{
		htmlEntity = _getHtmlEntityName((unsigned short) sourceStr[pos]);

		if (htmlEntity)
		{
			if (filteredStr)
			{
				webc_c_strcpy(filteredStr + epos, htmlEntity);
			}
			epos += tc_strlen(htmlEntity);
		}
		else
		{
			if (filteredStr)
			{
				filteredStr[epos] = (WEBC_CHAR) sourceStr[pos];
			}
			epos++;
		}
	}

	return epos;
}

/*---------------------------------------------------------------------------*/
long _escapeHtmlUnicodeFilter (
		WEBC_CHAR* filteredStr,
		const void* data,
		long size
	)
{
	const unsigned short* sourceStr = (const unsigned short*) data;
	long pos, epos;
	const char* htmlEntity;

	for (pos = 0, epos = 0; pos < size; pos++)
	{
		htmlEntity = _getHtmlEntityName((unsigned short) sourceStr[pos]);

		if (htmlEntity)
		{
			if (filteredStr)
			{
				// webc_c_strcpy(filteredStr + epos, htmlEntity);
				//webc_strcpy(filteredStr + epos, htmlEntity);
				webc_c_strcpy(filteredStr + epos, htmlEntity);

			}
			epos += tc_strlen(htmlEntity);
		}
		else
		{
			if (filteredStr)
			{
				filteredStr[epos] = (WEBC_CHAR) sourceStr[pos];
			}
			epos++;
		}
	}

	return epos;
}

/*---------------------------------------------------------------------------
  CollapseWhitespace
  ---------------------------------------------------------------------------*/
/** @memo

    @doc

    @see

    @return
 */
long CollapseWhitespace (
		WEBC_CHAR *dest,
		WEBC_CHAR *src
	)
{
long in_whitespace, dest_len;

	// Ignore leading whitespace
/*
	while (IS_WHITESPCE(*src))
	{
		src++;
	}
*/

	in_whitespace = 0;
	dest_len = 0;
	while (src && dest && *src)
	{
		if (IS_WHITESPACE(*src))
		{
			if (!in_whitespace)
			{
				*(dest++) = ' ';
				dest_len++;
				in_whitespace = 1;
			}
		}
		else
		{
			in_whitespace = 0;
			*(dest++) = *(src);
			dest_len++;
		}

		src++;
	}

	// Ignore trailing whitespace
/*
	while (IS_WHITESPACE(*(dest-1)))
	{
		dest--;
	}
*/

	*dest = '\0';
	return (dest_len);
}
