//
// WEBSTR.HPP - WebC String Class
//
// EBS -
//
//
// Copyright EBS Inc. , 2004
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//

#ifndef __WEBSTR_HPP__
#define __WEBSTR_HPP__

#ifdef __cplusplus

#include "webc.h"

long const WEB_STR_DEFAULT_SIZE = 32;

typedef long (*WebStringFilter) (WEBC_CHAR* filteredStr, const void* sourceStr, long size);

class WebString
{
protected:
	WEBC_CHAR* mStringStart;
	WEBC_CHAR* mBufferStart;
	WEBC_SIZE  mBufferSize;
	WEBC_SIZE  mStringLength;

	int appendFiltered (
			const void* data,
			long size,
			WebStringFilter filter
		);

	int prependFiltered (
			const void* data,
			long size,
			WebStringFilter filter
		);

	int allocAtLeast (
			long size,
			long spaceBeforeString,
			long spaceAfterString
		);

public:
	WebString (
			long initSize = WEB_STR_DEFAULT_SIZE
		);

	~WebString (
			void
		);

	int append (
			const char* asciiData,
			long size
		);

	int append (
			const unsigned short* ucData,
			long size
		);

	int append (
			WebString* str
		);

	int appendHtmlEscaped (
			const char* asciiData,
			long size
		);

	int appendHtmlEscaped (
			const unsigned short* ucData,
			long size
		);

	int prepend (
			const char* asciiData,
			long size
		);

	int prepend (
			const unsigned short* ucData,
			long size
		);

	int prepend (
			WebString* str
		);

	int copy (
			const char* asciiData
		);

	int copy (
			const char* asciiData,
			long length
		);

	int copy (
			const unsigned short* ucData
		);

	int copy (
			const unsigned short* ucData,
			long length
		);

	int copy (
			WebString* str
		);

	WEBC_CHAR* getChars (
			void
		);

	long getLength (
			void
		);

	void discardPrefix (
			long size
		);

	void discardPostfix (
			long size
		);

	void discardAll (
			void
		);

	void freeBuffer (
			void
		);

	void truncate (
			long length
		);

	// filter/conversion methods

	void collapseWhitespace (
			void
		);

	void unescapeHtmlChars (
			void
		);

	void crLfToLf (
			void
		);

	void removeLeadingCrLfs (
			void
		);

	void removeChars (
			WEBC_CHAR* badChars
		);

	void removeTrailingNulls (
		void
	);

	void replaceChars (
			WEBC_CHAR replaceThisChar,
			WEBC_CHAR withThisChar
		);

	void removeLeadingWhitespace (
			void
		);

	void removeTrailingWhitespace (
			void
		);
};

extern "C" {
#endif

// return length of dest
long CollapseWhitespace (
		WEBC_CHAR *dest,
		WEBC_CHAR *src
	);

#ifdef __cplusplus
}
#endif

#endif /* __WEBSTR_HPP__ */
