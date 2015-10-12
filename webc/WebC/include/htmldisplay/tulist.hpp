#ifndef __TULIST_HPP__
#define __TULIST_HPP__

// Not used
class TextUnit;

struct TUListNode
{
	TextUnit *pTU;
	TUListNode *pNext;
	TUListNode *pPrev;
};

#define MAX_TU_LIST_NODE_POOL_SIZE      100

typedef TUListNode *TUListIterator;

class TextUnitList
{
public:
	TextUnitList(void);
	~TextUnitList(void);

	TextUnit *GetFirstUnit (TUListIterator &i);
	TextUnit *GetNextUnit (TUListIterator &i);
	TextUnit *GetLastUnit (TUListIterator &i);
	TextUnit *GetPrevUnit (TUListIterator &i);

	void InsertFirst (TextUnit *pTU);
	void InsertLast  (TextUnit *pTU);
	TextUnit *RemoveFirst (void);
	TextUnit *RemoveLast  (void);

	static void Exit(void);

private:
	static TUListNode *AllocNode (void);
	static void FreeNode (TUListNode *node);
	static TUListNode *mpNodePool;
	static int miPoolSize;

	TUListNode *mpFirst;
	TUListNode *mpLast;
};

#endif // __TULIST_HPP__
