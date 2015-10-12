#ifndef __STRTABLE_HPP__
#define __STRTABLE_HPP__

#include "webctypes.h"

class StringTable
{
protected:
	struct StringEntry
	{
		unsigned short referenceCount;
		WEBC_CHAR * str;
		unsigned int hashCode;
		StringEntry *prev, *next;
	};
	
	unsigned int miTableSize;
	StringEntry *mpEntries;
	StringEntry  mFreeList;

	long miTempStringBufferSize;
	WEBC_CHAR *mpTempStringBuffer;
	
	WEBC_CHAR *ToLowerCase(const WEBC_CHAR *str);
	WEBC_CHAR *ToLowerCaseLen(const WEBC_CHAR *str, long len);
	WEBC_CHAR *ToNullTerm(const WEBC_CHAR *str, long len);
	void EnlargeTempBuffer(long size);
	
	StringEntry * AllocEntry(void);
	void FreeEntry(StringEntry *entry);
	
	unsigned int HashString (const WEBC_CHAR *s);
	int  InitEntry (StringEntry *entry, const WEBC_CHAR *s, unsigned int hashCode);
	void CopyEntry (StringEntry *dst, StringEntry *src);
	void MoveEntry (StringEntry *dst, StringEntry *src);

public:
	StringTable (void);
	StringTable (unsigned int iSize);
	~StringTable (void);

	void  Init (unsigned int iSize);
	WEBC_CHAR *GetString (const WEBC_CHAR *s);
	WEBC_CHAR *GetStringLen (const WEBC_CHAR *s, long len);
	WEBC_CHAR *GetLowercaseString (const WEBC_CHAR *s);
	WEBC_CHAR *GetLowercaseStringLen (const WEBC_CHAR *s, long len);
	void  ReleaseString (const WEBC_CHAR *s);
	void  Clear (void);
};

#endif // __STRTABLE_HPP__
