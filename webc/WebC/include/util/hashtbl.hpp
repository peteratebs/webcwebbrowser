#ifndef __HASHTBL_HPP__
#define __HASHTBL_HPP__

typedef unsigned long HashCode_t;
typedef unsigned short HashIndex_t;

#include "webctypes.h"

class HashKey 
{
public:
	virtual void       Dispose  (void) = 0;
	virtual WEBC_BOOL  Equals   (HashKey *key) = 0;
	virtual HashCode_t HashCode (void) = 0;
};

// Return codes for HashTable::Insert
#define HASH_SUCCESS                0
#define HASH_KEY_ALREADY_PRESENT   -1
#define HASH_TABLE_FULL            -2
#define HASH_BAD_KEY               -3

class HashTable
{
protected:
	struct HashEntry
	{
		HashKey *key;
		HashEntry *prev, *next;
	};
	
	HashIndex_t  miTableSize;
	HashEntry   *mpEntries;
	HashEntry    mFreeList;
	
	HashEntry * AllocEntry(void);
	void FreeEntry(HashEntry *entry);
	
public:
	HashTable (void);
	HashTable (HashIndex_t iSize);
	~HashTable (void);

	void      Init   (HashIndex_t iSize);
	void      Clear  (void);
	int       Insert (HashKey *key);
	HashKey * Find   (HashKey *key);
	HashKey * Remove (HashKey *key);
};

#endif // __HASHTBL_HPP__
