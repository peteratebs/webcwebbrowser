//
// TULIST.CPP - Methods for class TextUnitList
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
// Not used

/*****************************************************************************/
// Header files
/*****************************************************************************/

#include "tulist.hpp"
#include "webc.h"
#include "webcmem.h"


/*****************************************************************************/
// Local Constants
/*****************************************************************************/

/*****************************************************************************/
// Local type definitions
/*****************************************************************************/

/*****************************************************************************/
// Local Functions
/*****************************************************************************/

/*****************************************************************************/
// Global Data
/*****************************************************************************/

TUListNode *TextUnitList::mpNodePool = 0;
int TextUnitList::miPoolSize = 0;


/*****************************************************************************/
// TextUnitList instance methods
/*****************************************************************************/

TextUnitList::TextUnitList(void)
{
	mpFirst = 0;
	mpLast = 0;
}

TextUnitList::~TextUnitList(void)
{
	while (mpFirst)
	{
		RemoveFirst();
	}
}

TextUnit *TextUnitList::GetFirstUnit (TUListIterator &i)
{
	i = mpFirst;
	return (i? i->pTU : 0);
}

TextUnit *TextUnitList::GetNextUnit (TUListIterator &i)
{
	i = i->pNext;
	return (i? i->pTU : 0);
}

TextUnit *TextUnitList::GetLastUnit (TUListIterator &i)
{
	i = mpLast;
	return (i? i->pTU : 0);
}

TextUnit *TextUnitList::GetPrevUnit (TUListIterator &i)
{
	i = i->pPrev;
	return (i? i->pTU : 0);
}


void TextUnitList::InsertFirst (TextUnit *pTU)
{
TUListNode *node = AllocNode();

	if (node)
	{
		node->pTU = pTU;
		node->pNext = mpFirst;
		node->pPrev = 0;
		if (!mpLast)
		{
			mpLast = node;
		}
		if (mpFirst)
		{
			mpFirst->pPrev = node;
		}
		mpFirst = node;
	}
}

void TextUnitList::InsertLast  (TextUnit *pTU)
{
TUListNode *node = AllocNode();

	if (node)
	{
		node->pTU = pTU;
		node->pNext = 0;
		node->pPrev = mpLast;
		if (!mpFirst)
		{
			mpFirst = node;
		}
		if (mpLast)
		{
			mpLast->pNext = node;
		}
		mpLast = node;
	}
}

TextUnit *TextUnitList::RemoveFirst (void)
{
TUListNode *node = mpFirst;
TextUnit *pTU = 0;

	if (node)
	{
		mpFirst = node->pNext;

		if (mpFirst)
		{
			mpFirst->pPrev = 0;
		}
		else
		{
			mpLast = 0;
		}

		pTU = node->pTU;
		FreeNode(node);
	}
	return (pTU);
}

TextUnit *TextUnitList::RemoveLast  (void)
{
TUListNode *node = mpLast;
TextUnit *pTU = 0;

	if (node)
	{
		mpLast = node->pPrev;

		if (mpLast)
		{
			mpLast->pNext = 0;
		}
		else
		{
			mpFirst = 0;
		}

		pTU = node->pTU;
		FreeNode(node);
	}
	return (pTU);

}


/*****************************************************************************/
// TextUnitList static methods
/*****************************************************************************/

void TextUnitList::Exit(void)
{
TUListNode *pNext;

	while (mpNodePool)
	{
		pNext = mpNodePool->pNext;
		WEBC_FREE(mpNodePool);
		miPoolSize--;
		mpNodePool = pNext;
	}
}

TUListNode *TextUnitList::AllocNode (void)
{
	if (mpNodePool)
	{
		TUListNode *pNew = mpNodePool;
		mpNodePool = mpNodePool->pNext;
		miPoolSize--;
		return (pNew);
	}
	else
	{
		return (TUListNode *) WEBC_MALLOC(sizeof(TUListNode));
	}
}

void TextUnitList::FreeNode (TUListNode *node)
{
	if (miPoolSize < MAX_TU_LIST_NODE_POOL_SIZE)
	{
		node->pNext = mpNodePool;
		mpNodePool = node;
		miPoolSize++;
	}
	else
	{
		WEBC_FREE(mpNodePool);
	}
}
