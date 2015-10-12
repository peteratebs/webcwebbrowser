#ifndef __HTMLFIND_HPP__
#define __HTMLFIND_HPP__

#include "webc.h"
#include "helement.hpp"
#include "hlist.hpp"
#include "hli.hpp"

/*************************************************************************
* class HTMLElementFinder
*
*  This interface is used in conjunction with the HTMLElement::FindElement
* method to provide a custom depth-first search capability.
*
*************************************************************************/

class HTMLElement;

class HTMLElementFinder
{
public:
	virtual WEBC_BOOL Check(HTMLElement *pElement) = 0;
};


/*************************************************************************
* class HTMLLastFinder
*
*  Find the last element
*
*************************************************************************/

class HTMLLastFinder : public HTMLElementFinder
{
private:
	HTMLElementFinder* mFilter;
	HTMLElement* mpElement;

public:
	HTMLLastFinder (HTMLElementFinder* filter = 0) { mFilter = filter; mpElement = 0; }
	virtual ~HTMLLastFinder() {}

	// HTMLElementFinder interface
	virtual WEBC_BOOL Check(HTMLElement *pElement)
	{
		if (!mFilter || mFilter->Check(pElement))
		{
			mpElement = pElement;
		}
		return WEBC_FALSE;
	}

	HTMLElement *Found() {return mpElement;}
};


/*************************************************************************
* class HTMLLastOfTypeFinder
*
*  Find the last element of a given type
*
*************************************************************************/

class HTMLLastOfTypeFinder : public HTMLElementFinder
{
public:
	HTMLLastOfTypeFinder(HTMLElementType eType);
	virtual ~HTMLLastOfTypeFinder();

	// HTMLElementFinder interface
	virtual WEBC_BOOL Check(HTMLElement *pElement);

	HTMLElement *Found();

private:
	HTMLElementType meType;
	HTMLElement *mpElement;
};

/*************************************************************************
* class HTMLParentListFinder
*
*  Find the last element of a given type
*
*************************************************************************/

class HTMLParentListFinder : public HTMLElementFinder
{
public:
	HTMLParentListFinder(void);
	virtual ~HTMLParentListFinder(void);

	// HTMLElementFinder interface
	virtual WEBC_BOOL Check(HTMLElement *pElement);

	HTMLList* Found();

private:
	HTMLList* mParentList;
};

/*************************************************************************
* class HTMLLastListItemFinder
*
*  Find the last element of a given type
*
*************************************************************************/

class HTMLLastListItemFinder : public HTMLElementFinder
{
public:
	HTMLLastListItemFinder(HTMLList* parentList);
	virtual ~HTMLLastListItemFinder(void);

	// HTMLElementFinder interface
	virtual WEBC_BOOL Check(HTMLElement *pElement);

	HTMLListItem *Found();

private:
	HTMLList*     mParentList;
	HTMLListItem* mListItem;
};

/*************************************************************************
* class HTMLNextTabFinder
*
*  Find the next element in the tabbing order
*
*************************************************************************/

// Helper functions for HTMLNextTabFinder and HTMLPrevTabFinder

class HTMLNextTabFinder : public HTMLElementFinder
{
public:
	HTMLNextTabFinder(HTMLElement *pLast);
	virtual ~HTMLNextTabFinder();

	// HTMLElementFinder interface
	virtual WEBC_BOOL Check(HTMLElement *pElement);

	HTMLElement *Found();

private:
	HTMLElement *mpFirstOfHigherIndex;
	int          miNextHigherIndex; // the tab index of mpFirstOfHigherIndex
	HTMLElement *mpCurrent;
	int          miCurrentIndex;
	HTMLElement *mpFound;
	WEBC_BOOL         mbCurrentSeen;
};


/*************************************************************************
* class HTMLPrevTabFinder
*
*  Find the previous element in the tabbing order
*
*************************************************************************/

class HTMLPrevTabFinder : public HTMLElementFinder
{
public:
	HTMLPrevTabFinder(HTMLElement *pLast);
	virtual ~HTMLPrevTabFinder();

	// HTMLElementFinder interface
	virtual WEBC_BOOL Check(HTMLElement *pElement);

	HTMLElement *Found();

private:
	HTMLElement *mpCurrent;
	HTMLElement *mpFound;
	int miHighest;
	int miCurrent;
	WEBC_BOOL mbCurrentSeen;
};

/*************************************************************************
* class HTMLLastTabFinder
*
*  Find the last element in the tabbing order
*
*************************************************************************/

class HTMLLastTabFinder : public HTMLElementFinder
{
public:
	HTMLLastTabFinder();
	virtual ~HTMLLastTabFinder();

	// HTMLElementFinder interface
	virtual WEBC_BOOL Check(HTMLElement *pElement);

	HTMLElement *Found();

private:
	HTMLElement *mpLastTab;
	short miHighestTabIndex;
};

/*************************************************************************
* class HTMLFirstTabFinder
*
*  Find the first element in the tabbing order
*
*************************************************************************/

class HTMLFirstTabFinder : public HTMLElementFinder
{
public:
	HTMLFirstTabFinder();
	virtual ~HTMLFirstTabFinder();

	// HTMLElementFinder interface
	virtual WEBC_BOOL Check(HTMLElement *pElement);

	HTMLElement *Found();

private:
	HTMLElement *mpFirstTab;
	WEBC_BOOL mbFirstNoIndex;
	short miLowestTabIndex;
};

/*************************************************************************
* class HTMLElementByIdFinder
*
* Find the element with a given ID (exclude TU_NULL_ID)
*
*************************************************************************/

class HTMLElementByIdFinder : public HTMLElementFinder
{
public:
	HTMLElementByIdFinder(WEBC_CHAR *pId);
	virtual ~HTMLElementByIdFinder();

	// HTMLElementFinder interface
	virtual WEBC_BOOL Check(HTMLElement *pElement);

private:
	WEBC_CHAR *mpId;
};


/*************************************************************************
* class HTMLElementClassIdFinder
*
* Find the first element in the given class
*
*************************************************************************/

class HTMLElementByClassFinder : public HTMLElementFinder
{
public:
	HTMLElementByClassFinder(const WEBC_CHAR *pClass);
	virtual ~HTMLElementByClassFinder();

	// HTMLElementFinder interface
	virtual WEBC_BOOL Check(HTMLElement *pElement);

private:
	const WEBC_CHAR *mpClassName;
};


/*************************************************************************
* class HTMLNthOfTypeFinder
*
* Find the Nth HTMLElement of a given type
*
*************************************************************************/

class HTMLNthOfTypeFinder : public HTMLElementFinder
{
public:
	HTMLNthOfTypeFinder(HTMLElementType type, int n);
	virtual ~HTMLNthOfTypeFinder();

	// HTMLElementFinder interface
	virtual WEBC_BOOL Check(HTMLElement *pElement);

private:
	HTMLElementType meType;
	int miCount;
};


/*************************************************************************
* class HTMLMapFinder
*
* Find the MAP with a given name
*
*************************************************************************/

class HTMLMapFinder : public HTMLElementFinder
{
public:
	HTMLMapFinder(WEBC_CHAR *name);
	virtual ~HTMLMapFinder();

	// HTMLElementFinder interface
	virtual WEBC_BOOL Check(HTMLElement *pElement);

private:
	WEBC_CHAR *mpName;
};

/*************************************************************************
* class HTMLElementByNameFinder
*
* Find the Element with a given name
*
*************************************************************************/

class HTMLElementByNameFinder : public HTMLElementFinder
{
public:
	HTMLElementByNameFinder(WEBC_CHAR *name);
	virtual ~HTMLElementByNameFinder();

	// HTMLElementFinder interface
	virtual WEBC_BOOL Check(HTMLElement *pElement);

private:
	WEBC_CHAR *mpName;
};

/*************************************************************************
* class HTMLAnchorByNameFinder
*
* Find the Anchor with a given name
*
*************************************************************************/

class HTMLAnchorByNameFinder : public HTMLElementFinder
{
public:
	HTMLAnchorByNameFinder(const WEBC_CHAR *name);
	virtual ~HTMLAnchorByNameFinder();

	// HTMLElementFinder interface
	virtual WEBC_BOOL Check(HTMLElement *pElement);

private:
	const WEBC_CHAR *mpName;
};

/*************************************************************************
* class HTMLAreaByNameFinder
*
* Find the Area with a given name
*
*************************************************************************/

class HTMLAreaByNameFinder : public HTMLElementFinder
{
public:
	HTMLAreaByNameFinder(WEBC_CHAR *name);
	virtual ~HTMLAreaByNameFinder();

	// HTMLElementFinder interface
	virtual WEBC_BOOL Check(HTMLElement *pElement);

private:
	WEBC_CHAR *mpName;
};

/*************************************************************************
* class HTMLFrameByNameFinder
*
* Find the Anchor with a given name
*
*************************************************************************/
class HTMLFrameByNameFinder : public HTMLElementFinder
{
public:
	HTMLFrameByNameFinder(WEBC_CHAR *name);
	virtual ~HTMLFrameByNameFinder();

	// HTMLElementFinder interface
	virtual WEBC_BOOL Check(HTMLElement *pElement);

private:
	WEBC_CHAR *mpName;
};

class HTMLAllLoadEventFinder : public HTMLElementFinder
{
public:
	HTMLAllLoadEventFinder(HTMLEvent *e);
	WEBC_BOOL Check(HTMLElement *pElement);
private:
	HTMLEvent *mpEvent;
};

/*************************************************************************
* class HTMLCountByTypeFinder
*
* Find the Type with the given name and keep track of total number of
* elements of this type
*
*************************************************************************/
class HTMLCountByTypeFinder : public HTMLElementFinder
{
public:
	HTMLCountByTypeFinder(HTMLElementType type);
	WEBC_BOOL Check(HTMLElement *);
	int Length() {return (miLen);}
private:
	HTMLElementType mType;
	int miLen;
};

/*************************************************************************
* class HTMLImageByNameFinder
*
* Find the Image with the given name
*
*************************************************************************/
class HTMLImageByNameFinder : public HTMLElementFinder
{
public:
	HTMLImageByNameFinder(WEBC_CHAR *name);
	WEBC_BOOL Check(HTMLElement *);
private:
	WEBC_CHAR *mpName;
};

class HTMLCountByNameFinder : public HTMLElementFinder
{
public:
	HTMLCountByNameFinder(WEBC_CHAR *name);
	WEBC_BOOL Check(HTMLElement *);
	int Length() { return(miLen); }
private:
	WEBC_CHAR *mpName;
	int miLen;
};

class HTMLNthByNameFinder : public HTMLElementFinder
{
public:
	HTMLNthByNameFinder(WEBC_CHAR *name, int index);
	WEBC_BOOL Check(HTMLElement *);
private:
	WEBC_CHAR *mpName;
	int miCount;
};

/*************************************************************************
* class HTMLReverseVideoFinder
*
* Reverses the foreground and background colors of every element
*
*************************************************************************/

class HTMLReverseVideoFinder : public HTMLElementFinder
{
public:
	HTMLReverseVideoFinder(void);
	WEBC_BOOL Check(HTMLElement *);
};

/*************************************************************************
* class HTMLPrevOfTypeFinder
*
* Find the previous element with the same type as the given element
*
*************************************************************************/

class HTMLPrevOfTypeFinder : public HTMLElementFinder
{
public:
	HTMLPrevOfTypeFinder(HTMLElement *pCurrent);
	WEBC_BOOL Check(HTMLElement *);

	HTMLElement *Found(void) { return mpPrev; }

private:
	HTMLElement *mpCurrent;
	HTMLElement *mpPrev;
};

/*************************************************************************
* class HTMLPrevFinder
*
* Find the previous element with the same type as the given element
*
*************************************************************************/

class HTMLPrevFinder : public HTMLElementFinder
{
private:
	HTMLElementFinder* mFilter;
	HTMLElementFinder* mMatch;
	HTMLElement* mpPrev;

public:
	HTMLPrevFinder (HTMLElementFinder* filter, HTMLElementFinder* match)
	{
		mFilter = filter;
		mMatch = match;
		mpPrev = 0;
	}

	WEBC_BOOL Check (HTMLElement *elem)
	{
		if (mMatch->Check(elem))
		{
			return WEBC_TRUE;
		}

		if (mFilter->Check(elem))
		{
			mpPrev = elem;
		}

		return WEBC_FALSE;
	}

	HTMLElement *Found(void) { return mpPrev; }

};

/*************************************************************************
* class HTMLNextOfTypeFinder
*
* Find the next element with the same type as the given element
*
*************************************************************************/

class HTMLNextOfTypeFinder : public HTMLElementFinder
{
public:
	HTMLNextOfTypeFinder(HTMLElement *pCurrent);
	WEBC_BOOL Check(HTMLElement *);

private:
	WEBC_BOOL mbTakeNext;
	HTMLElement *mpCurrent;
};

/*************************************************************************
* class HTMLNameTypeFinder
*
*************************************************************************/

class HTMLNameTypeFinder : public HTMLElementFinder
{
public:
	HTMLNameTypeFinder(WEBC_CHAR *name, HTMLElementType type);
	WEBC_BOOL Check(HTMLElement *);
private:
	WEBC_CHAR *mpName;
	HTMLElementType mType;
};

/*************************************************************************
* class HTMLIdTypeFinder
*
*************************************************************************/

class HTMLIdTypeFinder : public HTMLElementFinder
{
public:
	HTMLIdTypeFinder(WEBC_CHAR *id, HTMLElementType type);
	WEBC_BOOL Check(HTMLElement *);
private:
	WEBC_CHAR *mpId;
	HTMLElementType mType;
};

/*************************************************************************
* class HTMLFrameWithDocFinder
*
*************************************************************************/
class HTMLDocument;

class HTMLFrameWithDocFinder : public HTMLElementFinder
{
public:
	HTMLFrameWithDocFinder(HTMLDocument *pDoc);
	WEBC_BOOL Check(HTMLElement *pElem);
private:
	HTMLDocument *mpDocument;
};

/*************************************************************************
* class HTMLCountAllFinder
*
*************************************************************************/

class HTMLCountAllFinder : public HTMLElementFinder
{
public:
	HTMLCountAllFinder() { miCount = 0; }
	WEBC_BOOL Check(HTMLElement *pElem);
	int Length() { return (miCount); }
private:
	int miCount;
};

/*************************************************************************
* class HTMLCountBeforeFinder
*
*************************************************************************/

class HTMLCountBeforeFinder : public HTMLElementFinder
{
public:
	HTMLCountBeforeFinder(HTMLElement *pElement) { miCount = 0; mpElement = pElement; }
	WEBC_BOOL Check(HTMLElement *pElem);
	int Length() { return (miCount); }
private:
	int miCount;
	HTMLElement *mpElement;
};


/*************************************************************************
* class HTMLGenericFinder
*
* This is a generic finder utility that traverses the element tree
* and calls a user supplide callback funtion for each element found.
* The callback function has the form:
*   WEBC_BOOL DemoGenericFinderCallback(HTMLElement *pElem, int chore, void **ppContext)
*
* The constructor function has the form:
*	HTMLGenericFinder(HTMLGenericFinderCallback pCallback, int chore, void **pContext) { mpCallback=pCallback; miChore = chore; mpContext = pContext;}
*
*  Note: chore and pContext are passed from the application to the finder calback
*  and may be used for any purpose.	 FindElement() does not touch them.
*
*  See the example callback and constructor function calls for a reference.
*
***********************************************************************************
* Example of a generic callback function that can count all elements, count just divs
* or stop and return the addrsss of the first div.

#define COUNT_ALL 1
#define COUNT_DIVS 2
#define STOP_FIRST_DIV 3

static WEBC_BOOL DemoGenericFinderCallback(HTMLElement *pElem, int chore, void **ppContext)
{
	if (chore == COUNT_ALL)
		*((int*)ppContext) += 1;	   * update applications counter
	else if (chore == COUNT_DIVS && pElem->GetTag() == HTML_TAG_DIV)
			*((int*)ppContext) += 1;   * update applications counter
	else if (chore == STOP_FIRST_DIV && pElem->GetTag() == HTML_TAG_DIV)
	{
		*ppContext =  (void *)pElem;   * return the pointer through the interface
		return(1);
	}
	return(0);
}

***********************************************************************************
* Example of how to use the generic finder using the above callback
*
static void DemoGenericFinder(HTMLDocument *pdoc)
{
int count = 0;
HTMLElement *pfoundElement;

	* Use the generic finder to count all elements in the document
	HTMLGenericFinder finder(DemoGenericFinderCallback,COUNT_ALL,(void **) &count);
	pdoc->FindElement(&finder);
	printf("Counted (%d) if counting elements\n", count);

	* Use the generic finder to count all divs in the document
    count = 0;
	HTMLGenericFinder  finder1(DemoGenericFinderCallback,COUNT_DIVS,(void **) &count);
	pdoc->FindElement(&finder1);
	printf("Counted (%d) if counting divs\n", count);

	* Use the generic finder to find the first div in the document and return it.
    pfoundElement = 0;
	HTMLGenericFinder  finder2(DemoGenericFinderCallback,STOP_FIRST_DIV,(void **) &pfoundElement);
	pdoc->FindElement(&finder2);
	if (pfoundElement)
		printf("Found a div at (%X) if finding first divs\n", pfoundElement);
}
*
*************************************************************************/

typedef WEBC_BOOL (*HTMLGenericFinderCallback)(HTMLElement *pElem, int chore, void **pContext);

class HTMLGenericFinder : public HTMLElementFinder
{
public:
	HTMLGenericFinder(HTMLGenericFinderCallback pCallback, int chore, void **pContext) { mpCallback=pCallback; miChore = chore; mpContext = pContext;}
	WEBC_BOOL Check(HTMLElement *pElem);
private:
	HTMLGenericFinderCallback mpCallback;
	int miChore;
	void **mpContext;
};

/*************************************************************************
* class HTMLCountAfterFinder
*
*************************************************************************/

class HTMLCountAfterFinder : public HTMLElementFinder
{
public:
	HTMLCountAfterFinder(HTMLElement *pElement) { miCount = 0; mpElement = pElement; }
	WEBC_BOOL Check(HTMLElement *pElem);
	int Length() { return (miCount); }
private:
	int miCount;
	HTMLElement *mpElement;
};

/*************************************************************************
* class HTMLNthOfAnyTypeFinder
*
*************************************************************************/

class HTMLNthOfAnyTypeFinder : public HTMLElementFinder
{
public:
	HTMLNthOfAnyTypeFinder(int n) { miCount = n; }
	WEBC_BOOL Check(HTMLElement *pElement);
private:
	int miCount;
};

/*************************************************************************
* class HTMLIdNameTypeIndexFinder
*
*************************************************************************/

class HTMLIdNameTypeIndexFinder : public HTMLElementFinder
{
public:
	HTMLIdNameTypeIndexFinder(WEBC_CHAR *id, WEBC_CHAR *name, HTMLElementType type, long index)
	{
		mpId = id;
		mpName = name;
		mType = type;
		miIndex = index;
	}

	WEBC_BOOL Check(HTMLElement *pElement);

private:
	long miIndex;
	WEBC_CHAR *mpId;
	WEBC_CHAR *mpName;
	HTMLElementType mType;
};

/*************************************************************************
* class HTMLIdNameTypeIndexFinder8
*
*************************************************************************/

class HTMLIdNameTypeIndexFinder8 : public HTMLElementFinder
{
public:
	HTMLIdNameTypeIndexFinder8(char *id, char *name, HTMLElementType type, long index)
	{
		mpId = id;
		mpName = name;
		mType = type;
		miIndex = index;
	}

	WEBC_BOOL Check(HTMLElement *pElement);

private:
	long miIndex;
	char *mpId;
	char *mpName;
	HTMLElementType mType;
};

/*************************************************************************
* class HTMLNextByIdNameTypeIndexFinder
*
*************************************************************************/

class HTMLNextByIdNameTypeIndexFinder : public HTMLElementFinder
{
public:
	HTMLNextByIdNameTypeIndexFinder(HTMLElement *pElement, char *id, char *name, HTMLElementType type, long index)
	{
		mpElement = pElement;
		mpId = id;
		mpName = name;
		mType = type;
		miIndex = index;
		mbTakeNext = 0;
	}

	WEBC_BOOL Check(HTMLElement *pElement);

private:
	HTMLElement *mpElement;
	long miIndex;
	char *mpId;
	char *mpName;
	HTMLElementType mType;
	WEBC_BOOL mbTakeNext;
};


/*************************************************************************
* class HTMLPrevByIdNameTypeIndexFinder
*
*************************************************************************/

class HTMLPrevByIdNameTypeIndexFinder : public HTMLElementFinder
{
public:
	HTMLPrevByIdNameTypeIndexFinder(HTMLElement *pElement, char *id, char *name, HTMLElementType type, long index)
	{
		mpElement = pElement;
		mpId = id;
		mpName = name;
		mType = type;
		miIndex = index;
		mpFound = 0;
	}

	WEBC_BOOL Check(HTMLElement *pElement);
	HTMLElement *Found() { return mpFound; }

private:
	HTMLElement *mpElement;
	long miIndex;
	char *mpId;
	char *mpName;
	HTMLElementType mType;
	HTMLElement *mpFound;
};


/**
 * class HTMLNameOrIdFinder
 */
class HTMLNameOrIdFinder : public HTMLElementFinder
{
public:
	HTMLNameOrIdFinder(WEBC_CHAR *pNameOrId)
	{
		mpNameOrId = pNameOrId;
	}
	WEBC_BOOL Check(HTMLElement *pElement);
private:
	WEBC_CHAR *mpNameOrId;
};

/**
 * class HTMLTypeAndNameOrIdFinder
 */
class HTMLTypeAndNameOrIdFinder : public HTMLElementFinder
{
public:
	HTMLTypeAndNameOrIdFinder(WEBC_CHAR *pNameOrId, HTMLElementType type)
	{
		mpNameOrId = pNameOrId;
		mType = type;
	}

	WEBC_BOOL Check(HTMLElement *pElement);

private:
	WEBC_CHAR *mpNameOrId;
	HTMLElementType mType;
};


/*************************************************************************
* class HTMLLastVisibleTabFinder
*
*  Find the last element in the tabbing order
*
*************************************************************************/

class HTMLLastVisibleTabFinder : public HTMLElementFinder
{
public:
	HTMLLastVisibleTabFinder();
	virtual ~HTMLLastVisibleTabFinder();

	// HTMLElementFinder interface
	virtual WEBC_BOOL Check(HTMLElement *pElement);

	HTMLElement *Found();

private:
	HTMLElement *mpLastTab;
	short miHighestTabIndex;
};

/*************************************************************************
* class HTMLFirstVisibleTabFinder
*
*  Find the first element in the tabbing order
*
*************************************************************************/

class HTMLFirstVisibleTabFinder : public HTMLElementFinder
{
public:
	HTMLFirstVisibleTabFinder();
	virtual ~HTMLFirstVisibleTabFinder();

	// HTMLElementFinder interface
	virtual WEBC_BOOL Check(HTMLElement *pElement);

	HTMLElement *Found();

private:
	HTMLElement *mpFirstTab;
	WEBC_BOOL mbFirstNoIndex;
	short miLowestTabIndex;
};

/*************************************************************************
* class HTMLStyleUpdateFinder
*
*  Calls StyleUpdate(0) on all elements
*
*************************************************************************/

class HTMLStyleUpdateFinder : public HTMLElementFinder
{
public:
	HTMLStyleUpdateFinder();
	virtual ~HTMLStyleUpdateFinder();

	// HTMLElementFinder interface
	virtual WEBC_BOOL Check(HTMLElement *pElement);
};


/*************************************************************************
* class HTMLSelectedOptionFinder
*
*  Finds the first HTMLOption element whose selected flag is set
*
*************************************************************************/

class HTMLSelectedOptionFinder : public HTMLElementFinder
{
public:
	HTMLSelectedOptionFinder();
	virtual ~HTMLSelectedOptionFinder();

	// HTMLElementFinder interface
	virtual WEBC_BOOL Check(HTMLElement *pElement);

	int Index(void);

protected:
	int miIndex;
	int miNum;
};


/*************************************************************************
* class HTMLDisplayElementFinder
*
*  Finds the Nth element with a DisplayElement
*
*************************************************************************/

class HTMLDisplayElementFinder : public HTMLElementFinder
{
public:
	HTMLDisplayElementFinder(HTMLElement *pElem = 0, int n = 0);
	virtual ~HTMLDisplayElementFinder();

	// HTMLElementFinder interface
	virtual WEBC_BOOL Check(HTMLElement *pElement);

private:
	int miNum;
	HTMLElement *mpElement;
};


/*************************************************************************
* class HTMLSetFlagFinder
*
*  sets or clears flags of an HTMLElement and its children
*
*************************************************************************/

class HTMLSetFlagFinder : public HTMLElementFinder
{
public:
	HTMLSetFlagFinder(HELEM_FLAGS mask, int onBool);
	virtual ~HTMLSetFlagFinder();

	// HTMLElementFinder interface
	virtual WEBC_BOOL Check(HTMLElement *pElement);

private:
	HELEM_FLAGS   mMask;
	int           mbOn;
};

/*
************************************************************************
* class HTMLElementFormFinder
*
*************************************************************************/

class HTMLElementFormFinder : public HTMLElementFinder
{
private:
	HTMLForm* mpForm;

public:
	HTMLElementFormFinder(void) { mpForm = 0; }
	virtual ~HTMLElementFormFinder(void) {}

	// HTMLElementFinder interface
	virtual WEBC_BOOL Check(HTMLElement *pElement)
	{
		mpForm = pElement->Form();
		if (mpForm)
		{
			return (1);
		}

		return (0);
	}

	HTMLForm* Found(void) { return mpForm; }
};

/*************************************************************************
* class HTMLElementAndFinder
*
*************************************************************************/

class HTMLElementAndFinder : public HTMLElementFinder
{
private:
	HTMLElementFinder* mFinderOne;
	HTMLElementFinder* mFinderTwo;

public:
	HTMLElementAndFinder (HTMLElementFinder* one, HTMLElementFinder* two)
	{
		mFinderOne = one;
		mFinderTwo = two;
	}

	virtual ~HTMLElementAndFinder(void)
	{
	}

	// HTMLElementFinder interface
	virtual WEBC_BOOL Check (HTMLElement* pElement)
	{
		return (mFinderOne->Check(pElement) && mFinderTwo->Check(pElement));
	}
};


/*************************************************************************
* class HTMLElementOrFinder
*
*************************************************************************/

class HTMLElementOrFinder : public HTMLElementFinder
{
private:
	HTMLElementFinder* mFinderOne;
	HTMLElementFinder* mFinderTwo;

public:
	HTMLElementOrFinder (HTMLElementFinder* one, HTMLElementFinder* two)
	{
		mFinderOne = one;
		mFinderTwo = two;
	}

	virtual ~HTMLElementOrFinder(void)
	{
	}

	virtual WEBC_BOOL Check (HTMLElement* pElement)
	{
		return (mFinderOne->Check(pElement) || mFinderTwo->Check(pElement));
	}
};

/*************************************************************************
* class HTMLElementNotFinder
*
*************************************************************************/

class HTMLElementNotFinder : public HTMLElementFinder
{
private:
	HTMLElementFinder* mFinder;

public:
	HTMLElementNotFinder (HTMLElementFinder* finder)
	{
		mFinder = finder;
	}

	virtual ~HTMLElementNotFinder(void)
	{
	}

	virtual WEBC_BOOL Check (HTMLElement* pElement)
	{
		return (!mFinder->Check(pElement));
	}
};


/*************************************************************************
* class HTMLElementTypeFinder
*
*************************************************************************/

class HTMLElementTypeFinder : public HTMLElementFinder
{
private:
	HTMLElementType mType;

public:
	HTMLElementTypeFinder (HTMLElementType t)
	{
		mType = t;
	}

	virtual ~HTMLElementTypeFinder(void)
	{
	}

	virtual WEBC_BOOL Check (HTMLElement* pElement)
	{
		return (pElement->Type() == mType);
	}
};

/*************************************************************************
* class HTMLSequenceFinder
*
*************************************************************************/

class HTMLSequenceFinder : public HTMLElementFinder
{
private:
	HTMLElementFinder* mFirstFinder;
	HTMLElementFinder* mSecondFinder;
	HTMLElement* mFirstElement;
	HTMLElement* mSecondElement;

public:
	HTMLSequenceFinder(HTMLElementFinder* firstCriteria, HTMLElementFinder* secondCriteria)
	{
		mFirstElement = 0;
		mSecondElement = 0;
		mFirstFinder = firstCriteria;
		mSecondFinder = secondCriteria;
	}

	WEBC_BOOL Check(HTMLElement* test)
	{
		if (!mFirstElement)
		{
			if (mFirstFinder->Check(test))
			{
				mFirstElement = test;
			}
		}
		else
		{
			if (mSecondFinder->Check(test))
			{
				mSecondElement = test;
				return (WEBC_TRUE);
			}
		}

		return (WEBC_FALSE);
	}

	HTMLElement* FirstFound(void) {return mFirstElement; }
	HTMLElement* SecondFound(void) {return mSecondElement; }

};

class HTMLExactElementFinder : public HTMLElementFinder
{
private:
	HTMLElement* mElement;

public:
	HTMLExactElementFinder(HTMLElement* element)
	{
		mElement = element;
	}

	WEBC_BOOL Check(HTMLElement* test)
	{
		return (mElement == test);
	}
};

/*************************************************************************
* class HTMLElementCounter
*
*************************************************************************/

class HTMLElementCounter : public HTMLElementFinder
{
private:
	HTMLElementFinder* mFilter;
	unsigned int mCount;

public:
	HTMLElementCounter (HTMLElementFinder* filter)
	{
		mFilter = filter;
		mCount = 0;
	}

	virtual ~HTMLElementCounter(void)
	{
	}

	virtual WEBC_BOOL Check (HTMLElement* pElement)
	{
		if (mFilter->Check(pElement))
		{
			mCount++;
		}
		return (WEBC_FALSE);
	}

	unsigned int Count(void) { return mCount; }
};

/*************************************************************************
* class HTMLNthElementFinder
*
*************************************************************************/

class HTMLNthElementFinder : public HTMLElementFinder
{
private:
	HTMLElementFinder* mFilter;
	unsigned int mCount;

public:
	HTMLNthElementFinder (HTMLElementFinder* filter, unsigned int n)
	{
		mFilter = filter;
		mCount = n;
	}

	virtual ~HTMLNthElementFinder(void)
	{
	}

	virtual WEBC_BOOL Check (HTMLElement* pElement)
	{
		if (mFilter->Check(pElement))
		{
			if (mCount == 0)
			{
				return (WEBC_TRUE);
			}

			mCount--;
		}

		return (WEBC_FALSE);
	}
};




/*************************************************************************
* class HTMLFocusElementFinder
*
*  Finds the element with focus
*
*************************************************************************/
// Helper functions for HTMLFocusElementFinder
#if (WEBC_INCLUDE_FOCUS_FINDER)

class HTMLFocusElementFinder : public HTMLElementFinder
{
public:
	HTMLFocusElementFinder(void)
	{
		mbFound = 0;
	}
	~HTMLFocusElementFinder()
	{
	}
	virtual WEBC_BOOL Check(HTMLElement *pElement);


	HTMLElement *Found()
	{
		return mbFound;
	}

private:
	HTMLElement *         mbFound;  // Not used. But don;t want pure virtual
};

#endif /* (WEBC_INCLUDE_FOCUS_FINDER) */


#endif //__HTMLFIND_HPP__
