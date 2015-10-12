//
// STRTABLE.CPP - Methods for class StringTable
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//

/*****************************************************************************/
// Header Files
/*****************************************************************************/

#include "strtable.hpp"
#include "webc.h"
#include "webcstr.h"
#include "webcmem.h"

/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/

#define LEFT_BIT_SHIFT        5
#define RIGHT_BIT_SHIFT       2
#define RANDOM_SEED           24539

// Initialize X as head of list
#define LIST_INIT(X,N,P)      {X->N=X; X->P=X;}

// Insert Y after X
#define LIST_INSERT(X,Y,N,P)  {X->N->P=Y; Y->N=X->N; X->N=Y; Y->P=X;}

// Remove X
#define LIST_REMOVE(X,N,P)    {X->N->P=X->P; X->P->N=X->N;}

// Replace X with Y
#define LIST_REPLACE(X,Y,N,P) {Y->N=X->N; Y->P=X->P; X->P->N=Y; X->N->P=Y;}

#define INVALID_HASH_CODE     miTableSize


/*****************************************************************************/
// Local Types
/*****************************************************************************/


/*****************************************************************************/
// Global Data
/*****************************************************************************/

long giSearchCount = 0;
long giSearchDepth = 0;


/*****************************************************************************/
// Static Data
/*****************************************************************************/
/*
static short primes[65] =
     { 173, 281, 349, 463, 541, 659, 733, 863, 941,1069,1151,1291,1373,
      1511,1583,1733,1811,1987,2053,2213,2287,2423,2531,2687,2741,2903,
      2999,3181,3257,3413,3511,3643,3727,3907,3989,4139,4231,4409,4493,
      4657,4751,4937,5003,5179,5279,5443,5521,5693,5791,5939,6053,6221,
      6301,6473,6571,6761,6833,6997,7103,7297,7411,7561,7643,7829,7919};
*/

/*****************************************************************************/
// class methods
/*****************************************************************************/

StringTable::StringTable(void)
{
	mFreeList.prev = &mFreeList;
	mFreeList.next = &mFreeList;
	miTempStringBufferSize = 0;
	mpTempStringBuffer = 0;
}

StringTable::StringTable(unsigned int iSize)
{
	mFreeList.prev = &mFreeList;
	mFreeList.next = &mFreeList;
	miTempStringBufferSize = 0;
	mpTempStringBuffer = 0;

	Init(iSize);
}

StringTable::~StringTable()
{
	Clear();

	if (mpTempStringBuffer)
	{
		WEBC_FREE(mpTempStringBuffer);
	}

	WEBC_FREE(mpEntries);
}

void StringTable::Init(unsigned int iSize)
{
	mpEntries = (StringEntry *) WEBC_MALLOC_VERBOSE(sizeof(StringEntry) * iSize,"Strtable");
	if (mpEntries)
	{
		miTableSize = iSize;
		tc_memset(mpEntries, 0, sizeof(StringEntry) * iSize);
		Clear();
	}
	else
	{
		miTableSize = 0;
	}
}

void StringTable::Clear(void)
{
	for (unsigned int n=0; n<miTableSize; n++)
	{
		if (mpEntries[n].hashCode != INVALID_HASH_CODE)
		{
			FreeEntry(&mpEntries[n]);
		}
	}
}

WEBC_CHAR * StringTable::GetLowercaseString(const WEBC_CHAR *s)
{
	return (GetString(ToLowerCase(s)));
}

WEBC_CHAR * StringTable::GetStringLen (const WEBC_CHAR *s, long len)
{
	return (GetString(ToNullTerm(s, len)));
}

WEBC_CHAR * StringTable::GetLowercaseStringLen (const WEBC_CHAR *s, long len)
{
	return (GetString(ToLowerCaseLen(s, len)));
}

WEBC_CHAR * StringTable::GetString(const WEBC_CHAR *s)
{
// printf("Getstring(%S)\n", s);

	if (s && mpEntries)
	{
		unsigned int hashCode = HashString(s);
		StringEntry *head = mpEntries + hashCode;
		if (hashCode != head->hashCode)
		{
			/* if the hashCode does not match for this entry, there are two
			    possibilities: the entry is unused or it is in use as part
			    of another entry's chain */
			if (head->hashCode != INVALID_HASH_CODE)
			{
				/* head is in use as part of another chain */
				StringEntry *replacement = AllocEntry();
				if (!replacement)
				{
					return (WEBC_NULL);
				}

				LIST_REPLACE(head,replacement,next,prev);
				MoveEntry(replacement, head);
			}
			else
			{
				/* head is not in use; remove it from the free chain and use
				    it */
				LIST_REMOVE(head,next,prev);
			}
			if (InitEntry(head, s, hashCode) < 0)
			{
				/* failed to alloc a copy of s */
				FreeEntry(head);
				return (WEBC_NULL);
			}
			LIST_INIT(head,next,prev);
			return (head->str);
		}

		/* we found an entry chain with the right hashCode, so
		    search for the right entry */
		StringEntry *entry = head;
		do
		{
			if (!webc_stricmp(s, entry->str))
			{
				if (entry->referenceCount < 0xffff)
				{
					/* string found; add a reference to this entry and return */
					entry->referenceCount++;
					return (entry->str);
				}
				else
				{
					/* too many references to this string */
					return (0);
				}
			}
			entry = entry->next;
		}
		while (entry != head);

		/* string is not yet in the table; grab a new entry from the free chain
		    and link it into the head chain */
		entry = AllocEntry();
		if (entry)
		{
			if (InitEntry(entry, s, hashCode) < 0)
			{
				/* failed to alloc a copy of s */
				FreeEntry(entry);
				return (WEBC_NULL);
			}
			/* reversing prev and next in LIST_INSERT does an insert before,
			    as opposed to an insert after (which is the default) */
			LIST_INSERT(head,entry,prev,next);
			return (entry->str);
		}
	}

	return (WEBC_NULL);
}

int StringTable::InitEntry (StringEntry *entry, const WEBC_CHAR *s, unsigned int hashCode)
{
	int len = webc_strlen(s);
	entry->str = (WEBC_CHAR *) WEBC_MALLOC_VERBOSE((len + 1)*sizeof(WEBC_CHAR),"StringTableInitEntry");
	if (entry->str)
	{
		tc_movebytes(entry->str, s, (len + 1)*sizeof(WEBC_CHAR));
		entry->hashCode = hashCode;
		entry->referenceCount = 1;
		return (0);
	}
	else
	{
		return (-1);
	}
}


void StringTable::ReleaseString (const WEBC_CHAR *s)
{
	if (!s)
	{
		return;
	}

	unsigned int hashCode = HashString(s);
	StringEntry *head = mpEntries + hashCode;
	if (hashCode != head->hashCode)
	{
		return;
	}

	StringEntry *entry = head;
	do
	{
		if (!webc_stricmp(s, entry->str))
		{
			entry->referenceCount--;
			if (entry->referenceCount == 0)
			{
				if (entry == head)
				{
					if (entry->next == entry)
					{
						FreeEntry(entry);
					}
					else
					{
						StringEntry *doomed = entry->next;
						if (entry->str)
						{
							WEBC_FREE(entry->str);
							entry->str = 0;
						}
						MoveEntry(entry, doomed);
						LIST_REMOVE(doomed,next,prev);
						FreeEntry(doomed);
					}
				}
				else
				{
					LIST_REMOVE(entry,next,prev);
					FreeEntry(entry);
				}
			}
			return;
		}
		entry = entry->next;
	}
	while (entry != head);
}

unsigned int StringTable::HashString (const WEBC_CHAR *s)
{
	unsigned int h = RANDOM_SEED;
	for (; *s; s++)
	{
		WEBC_CHAR S;
		S = LOWERCASE(*s);
		h = h ^ ( (h << LEFT_BIT_SHIFT) + (h >> RIGHT_BIT_SHIFT) + S);
	}

	return (h % miTableSize);
}

StringTable::StringEntry * StringTable::AllocEntry(void)
{
	if (mFreeList.next == &mFreeList)
	{
		return (WEBC_NULL);
	}

	StringEntry *e = mFreeList.next;
	LIST_REMOVE(e,next,prev);
	return (e);
}

void StringTable::MoveEntry(StringEntry *dst, StringEntry *src)
{
	CopyEntry(dst,src);
	src->str = WEBC_NULL;  // prevent FreeEntry from freeing string data
}


/* entry must be removed from any chain it is currently in BEFORE
    calling FreeEntry */

void StringTable::FreeEntry(StringEntry *entry)
{
	if (entry->str)
	{
		WEBC_FREE(entry->str);
		entry->str = 0;
	}
	entry->hashCode = INVALID_HASH_CODE;
	entry->referenceCount = 0;
	LIST_INSERT((&mFreeList),entry,next,prev);
}

void StringTable::CopyEntry (StringEntry *dst, StringEntry *src)
{
	dst->str = src->str;
	dst->hashCode = src->hashCode;
	dst->referenceCount = src->referenceCount;
}

WEBC_CHAR *StringTable::ToLowerCase(const WEBC_CHAR *str)
{
unsigned long size = webc_strlen(str) + 1;

	EnlargeTempBuffer(size);

	if (mpTempStringBuffer)
	{
		WEBC_CHAR *s = mpTempStringBuffer;
		while (size--)
		{
			*s = LOWERCASE(*str);
			s++;
			str++;
		}
	}

	return (mpTempStringBuffer);
}

WEBC_CHAR *StringTable::ToNullTerm(const WEBC_CHAR *str, long n)
{
	EnlargeTempBuffer(n+1);

	if (mpTempStringBuffer)
	{
		tc_movebytes(mpTempStringBuffer, str, n*sizeof(WEBC_CHAR));
		mpTempStringBuffer[n] = 0;
	}

	return (mpTempStringBuffer);
}

WEBC_CHAR *StringTable::ToLowerCaseLen(const WEBC_CHAR *str, long len)
{
	EnlargeTempBuffer(len + 1);

	if (mpTempStringBuffer)
	{
		WEBC_CHAR *s = mpTempStringBuffer;
		while (len--)
		{
			*s = LOWERCASE(*str);
			s++;
			str++;
		}
		*s = 0;
	}

	return (mpTempStringBuffer);
}

void StringTable::EnlargeTempBuffer(long size)
{
	if (size > miTempStringBufferSize)
	{
		if (mpTempStringBuffer)
		{
			WEBC_FREE(mpTempStringBuffer);
		}
		miTempStringBufferSize = (size + 0xff) & ~(0xff);
		mpTempStringBuffer = (WEBC_CHAR *) WEBC_MALLOC_VERBOSE(miTempStringBufferSize * sizeof(WEBC_CHAR),"StringTableTempBuffer");
	}
}
