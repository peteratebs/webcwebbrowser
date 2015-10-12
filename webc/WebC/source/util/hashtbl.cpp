//
// HASHTBL.CPP - Methods for class HashTable
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

#include "hashtbl.hpp"
#include "string.h"
#include "webc.h"
#include "webcstr.h"
#include "webcmem.h"


/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/

// Initialize X as head of list
#define LIST_INIT(X,N,P)      {X->N=X; X->P=X;}

// Insert Y after X
#define LIST_INSERT(X,Y,N,P)  {X->N->P=Y; Y->N=X->N; X->N=Y; Y->P=X;}

// Remove X
#define LIST_REMOVE(X,N,P)    {X->N->P=X->P; X->P->N=X->N;}

// Replace X with Y
#define LIST_REPLACE(X,Y,N,P) {Y->N=X->N; Y->P=X->P; X->P->N=Y; X->N->P=Y;}


/*****************************************************************************/
// Local Types
/*****************************************************************************/


/*****************************************************************************/
// Global Data
/*****************************************************************************/


/*****************************************************************************/
// Static Data
/*****************************************************************************/
/* we may yet have a use for these...
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

HashTable::HashTable(void)
{
	mpEntries = 0;
	LIST_INIT((&mFreeList),next,prev);
}

HashTable::HashTable(HashIndex_t iSize)
{
	mpEntries = 0;
	LIST_INIT((&mFreeList),next,prev);
	
	Init(iSize);
}

HashTable::~HashTable()
{
	Clear();
	WEBC_FREE(mpEntries);
}

void HashTable::Init(HashIndex_t iSize)
{
	if (mpEntries)
	{
		Clear();
		WEBC_FREE(mpEntries);	
	}
	
	if (iSize > 0)
	{
		miTableSize = iSize;
		mpEntries = (HashEntry *) WEBC_MALLOC_VERBOSE(sizeof(HashEntry) * iSize,"HashTable");
		if (mpEntries)
		{
			tc_memset(mpEntries, 0, sizeof(HashEntry) * iSize);
			for (HashIndex_t n=0; n<miTableSize; n++)
			{
				LIST_INSERT((&mFreeList),(&mpEntries[n]),next,prev);
			}
		}
		else
		{
			miTableSize = 0;
		}
	}
}

void HashTable::Clear(void)
{
	for (HashIndex_t n=0; n<miTableSize; n++)
	{
		if (mpEntries[n].key)
		{
			FreeEntry(&mpEntries[n]);
		}
	}
}

int HashTable::Insert(HashKey *key)
{
	if (key && miTableSize)
	{
		HashCode_t hashCode = (key->HashCode() % miTableSize);
		HashEntry *head = mpEntries + hashCode;
		if (head->key == 0)
		{
			/* head is not in use; remove it from the free chain and use it */
			LIST_REMOVE(head,next,prev);
			LIST_INIT(head,next,prev);
			head->key = key;
			return (HASH_SUCCESS);
		}
		else
		{
			if (hashCode == (head->key->HashCode() % miTableSize))
			{
				/* we found an entry chain with the right hashCode */
				
				/* first check to make sure an equivalent key is not present */
				HashEntry *entry = head;
				do
				{
					if (key->Equals(entry->key))
					{
						return (HASH_KEY_ALREADY_PRESENT);
					}
					entry = entry->next;
				} 
				while (entry != head);
		
				/* key is not yet in the table; grab a new entry from the free chain
				    and link it into the head chain */
				entry = AllocEntry();
				if (entry)
				{
					entry->key = key;
					/* reversing prev and next in LIST_INSERT does an insert before,
					    as opposed to an insert after (which is the default) */
					LIST_INSERT(head,entry,prev,next);
					return (HASH_SUCCESS);
				}
				else
				{
					return (HASH_TABLE_FULL);
				}
			}
			else
			{
				/* head is in use as part of another chain */
				HashEntry *replacement = AllocEntry();
				if (!replacement)
				{
					return (HASH_TABLE_FULL);
				}
				
				LIST_REPLACE(head,replacement,next,prev);
				replacement->key = head->key;
				head->key = key;
				LIST_INIT(head,next,prev);

				return (HASH_SUCCESS);
			}
		}
	}
	else
	{
		return (HASH_BAD_KEY);
	}
}

HashKey * HashTable::Find (HashKey * key)
{	
	if (key && miTableSize)
	{
		HashCode_t hashCode = (key->HashCode() % miTableSize);
		HashEntry *head = mpEntries + hashCode;
		if ((head->key != 0) && (hashCode == (head->key->HashCode() % miTableSize)))
		{
			HashEntry *entry = head;
			do
			{
				if (key->Equals(entry->key))
				{
					return (entry->key);
				}
				entry = entry->next;
			} 
			while (entry != head);
		}
	}
		
	return (0);
}

HashKey * HashTable::Remove (HashKey * key)
{	
	if (key && miTableSize)
	{
		HashCode_t hashCode = (key->HashCode() % miTableSize);
		HashEntry *head = mpEntries + hashCode;
		if ((head->key != 0) && (hashCode == (head->key->HashCode() % miTableSize)))
		{					
			HashEntry *entry = head;
			do
			{
				if (key->Equals(entry->key))
				{
					HashKey *removed = entry->key;
					entry->key = 0;

					if (entry == head)
					{
						if (entry->next == entry)
						{
							/* no need to do a LIST_REMOVE because entry is
							    the only one in this chain */
							FreeEntry(entry);
						}
						else
						{
							HashEntry *doomed = entry->next;
							entry->key = doomed->key;
							doomed->key = 0;
							LIST_REMOVE(doomed,next,prev);
							FreeEntry(doomed);
						}
					}
					else
					{
						LIST_REMOVE(entry,next,prev);
						FreeEntry(entry);
					}
					return (removed);
				}
				entry = entry->next;
			} 
			while (entry != head);
		}
	}
	
	return (0);
}

HashTable::HashEntry * HashTable::AllocEntry(void)
{
	if (mFreeList.next == &mFreeList)
	{
		return (WEBC_NULL);
	}
	
	HashEntry *e = mFreeList.next;
	LIST_REMOVE(e,next,prev);
	return (e);	
}

/* entry must be removed from any chain it is currently in BEFORE
    calling FreeEntry */

void HashTable::FreeEntry(HashEntry *entry)
{
	if (entry->key)
	{
		entry->key->Dispose();
		entry->key = 0;
	}
	LIST_INSERT((&mFreeList),entry,next,prev);
}
