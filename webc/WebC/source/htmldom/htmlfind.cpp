//
// HTMLFIND.CPP - Methods for HTML Element Finder classes
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
// Include files
/*****************************************************************************/

#include "htmlfind.hpp"
#include "helement.hpp"
#include "hmap.hpp"
#include "hanchor.hpp"
#include "harea.hpp"
#include "himage.hpp"
#include "hoption.hpp"
#include "hframe.hpp"
#include "webcstr.h"


/*****************************************************************************/
// Macro definitions
/*****************************************************************************/


////////////////////////////////////////////////////////////////////////////////////
// class HTMLLastOfTypeFinder
//
////////////////////////////////////////////////////////////////////////////////////

HTMLLastOfTypeFinder::HTMLLastOfTypeFinder(HTMLElementType eType)
{
	meType = eType;
	mpElement = 0;
}

HTMLLastOfTypeFinder::~HTMLLastOfTypeFinder()
{
}

WEBC_BOOL HTMLLastOfTypeFinder::Check(HTMLElement *pElement)
{
	if (pElement->Type() == meType)
	{
		mpElement = pElement;
	}
	
	return (0);
}

HTMLElement *HTMLLastOfTypeFinder::Found(void)
{
	return (mpElement);
}



////////////////////////////////////////////////////////////////////////////////////
// class HTMLNextTabFinder
//
////////////////////////////////////////////////////////////////////////////////////

HTMLNextTabFinder::HTMLNextTabFinder(HTMLElement *pLast)
{
	mpFirstOfHigherIndex = 0;
	miNextHigherIndex = -1; // the tab index of mpFirstOfHigherIndex
	mpCurrent = pLast;
	if (pLast)
	{
		miCurrentIndex = pLast->TabIndex();
	}
	else
	{
		miCurrentIndex = -1;
	}
	mpFound = 0;
	mbCurrentSeen = 0;
}

HTMLNextTabFinder::~HTMLNextTabFinder()
{
}

WEBC_BOOL HTMLNextTabFinder::Check(HTMLElement *pElement)
{
	int iThisIndex = pElement->TabIndex();
	
	if (iThisIndex == INVALID_TABINDEX)
	{
		return 0;
	}

	if (mpCurrent && (pElement == mpCurrent))
	{
		mbCurrentSeen = 1;
		return 0;
	}
	
	if ((iThisIndex == miCurrentIndex) && mbCurrentSeen)
	{
		mpFound = pElement;
		return 1;
	}
	
	if ( (TABINDEX_CMP(iThisIndex, miCurrentIndex) > 0) && 
		 (!mpFirstOfHigherIndex || (mpFirstOfHigherIndex && (TABINDEX_CMP(iThisIndex,miNextHigherIndex) < 0))))
	{
		mpFirstOfHigherIndex = pElement;
		mpFound = pElement;
		miNextHigherIndex = iThisIndex;
	}

	return 0;
}

HTMLElement *HTMLNextTabFinder::Found(void)
{
	return (mpFound);
}


////////////////////////////////////////////////////////////////////////////////////
// class HTMLPrevTabFinder
//
////////////////////////////////////////////////////////////////////////////////////

HTMLPrevTabFinder::HTMLPrevTabFinder(HTMLElement *pLast)
{
	mpCurrent = pLast;	
	mpFound = 0;
	miHighest = INVALID_TABINDEX;
	if (pLast)
	{
		miCurrent = pLast->TabIndex();
	}
	else
	{
		miCurrent = INVALID_TABINDEX;
	}
	
	mbCurrentSeen = 0;
}

HTMLPrevTabFinder::~HTMLPrevTabFinder()
{
}

WEBC_BOOL HTMLPrevTabFinder::Check(HTMLElement *pElement)
{
	int index = pElement->TabIndex();

	if (index == INVALID_TABINDEX)
	{
		return 0;
	}
	
	if (mpCurrent && (pElement == mpCurrent))
	{
		mbCurrentSeen = 1;
		return 0;
	}
	
	if ( (!mbCurrentSeen && (TABINDEX_CMP(index, miCurrent) == 0)) ||
		 ((TABINDEX_CMP(index, miCurrent) < 0) && (TABINDEX_CMP(index, miHighest) >= 0)))
	{
		mpFound = pElement;
		miHighest = index;
	}

	return (0);
}

HTMLElement *HTMLPrevTabFinder::Found(void)
{
	return (mpFound);
}

////////////////////////////////////////////////////////////////////////////////////
// class HTMLLastTabFinder
//
////////////////////////////////////////////////////////////////////////////////////

HTMLLastTabFinder::HTMLLastTabFinder()
{
	mpLastTab = 0;
	miHighestTabIndex = INVALID_TABINDEX;
}

HTMLLastTabFinder::~HTMLLastTabFinder()
{
}

WEBC_BOOL HTMLLastTabFinder::Check(HTMLElement *pElement)
{
	if ((pElement->TabIndex() != INVALID_TABINDEX) && 
		((miHighestTabIndex == INVALID_TABINDEX)
		  || (TABINDEX_CMP(pElement->TabIndex(),miHighestTabIndex) >= 0)))
	{
		mpLastTab = pElement;
		miHighestTabIndex = pElement->TabIndex();
	}
	
	return (0);
}

HTMLElement *HTMLLastTabFinder::Found(void)
{
	return (mpLastTab);
}

////////////////////////////////////////////////////////////////////////////////////
// class HTMLFirstTabFinder
//
////////////////////////////////////////////////////////////////////////////////////

HTMLFirstTabFinder::HTMLFirstTabFinder()
{
	mpFirstTab = 0;
	miLowestTabIndex = -1;
}

HTMLFirstTabFinder::~HTMLFirstTabFinder()
{
}

WEBC_BOOL HTMLFirstTabFinder::Check(HTMLElement *pElement)
{
	// if this one is valid and we don't yet have a valid one or this one 
	//  is strictly lower than the one we already have
	if ((pElement->TabIndex() != INVALID_TABINDEX) && 
		((miLowestTabIndex == INVALID_TABINDEX)	|| (TABINDEX_CMP(pElement->TabIndex(),miLowestTabIndex) < 0)))
	{
		mpFirstTab = pElement;
		miLowestTabIndex = pElement->TabIndex();
		if (miLowestTabIndex == 1)
		{
			// if the index of this one is 1, then we know already it is the first one 
			return 1;
		}
	}
	
	// else we keep searching
	return 0;
}

HTMLElement *HTMLFirstTabFinder::Found(void)
{
	return (mpFirstTab);
}

////////////////////////////////////////////////////////////////////////////////////
// class HTMLElementByIdFinder
//
////////////////////////////////////////////////////////////////////////////////////

HTMLElementByIdFinder::HTMLElementByIdFinder(WEBC_CHAR *pId)
{
	mpId = pId;
}

HTMLElementByIdFinder::~HTMLElementByIdFinder()
{
}

WEBC_BOOL HTMLElementByIdFinder::Check(HTMLElement *pElement)
{
WEBC_CHAR *pElementId = pElement->Id();

	return (pElementId && !webc_stricmp(pElementId, mpId));
}


////////////////////////////////////////////////////////////////////////////////////
// class HTMLNthOfTypeFinder
//
////////////////////////////////////////////////////////////////////////////////////

HTMLNthOfTypeFinder::HTMLNthOfTypeFinder(HTMLElementType type, int n)
{
	meType = type;
	miCount = n;
}

HTMLNthOfTypeFinder::~HTMLNthOfTypeFinder()
{
}

WEBC_BOOL HTMLNthOfTypeFinder::Check(HTMLElement *pElement)
{
	if (pElement->Type() != meType)
	{
		return (0);
	}
		
	if (miCount > 0)
	{
		miCount--;
		return (0);
	}
	else
	{
		return (1);
	}
}


////////////////////////////////////////////////////////////////////////////////////
// class HTMLMapFinder
//
////////////////////////////////////////////////////////////////////////////////////

HTMLMapFinder::HTMLMapFinder(WEBC_CHAR *name)
{
	mpName = name;
}

HTMLMapFinder::~HTMLMapFinder()
{
}

WEBC_BOOL HTMLMapFinder::Check(HTMLElement *pElement)
{
WEBC_CHAR *pMapName;

	if (pElement->Type() != HTML_MAP_ELEMENT)
	{
		return (0);
	}
		
	pMapName = ((HTMLMap *) pElement)->Name();
	if (pMapName && *pMapName)
	{
		return (!webc_stricmp(pMapName, mpName));
	}
		
	return (0);
}


////////////////////////////////////////////////////////////////////////////////////
// class HTMLElementByNameFinder
//
////////////////////////////////////////////////////////////////////////////////////

HTMLElementByNameFinder::HTMLElementByNameFinder(WEBC_CHAR *name)
{
	mpName = name;
}

HTMLElementByNameFinder::~HTMLElementByNameFinder()
{
}

WEBC_BOOL HTMLElementByNameFinder::Check(HTMLElement *pElement)
{
	WEBC_CHAR *elementName = (pElement) ? pElement->Name() : 0;

	if (elementName && mpName)
	{
		return (webc_stricmp(elementName, mpName) == 0);
	}
	
	return (0);
}//end Check


////////////////////////////////////////////////////////////////////////////////////
// class HTMLAnchorByNameFinder
//
////////////////////////////////////////////////////////////////////////////////////

HTMLAnchorByNameFinder::HTMLAnchorByNameFinder(const WEBC_CHAR *name)
{
	mpName = name;
}

HTMLAnchorByNameFinder::~HTMLAnchorByNameFinder()
{
}

WEBC_BOOL HTMLAnchorByNameFinder::Check(HTMLElement *pElement)
{
const WEBC_CHAR *pAnchorName;

	if (pElement->Type() != HTML_ANCHOR_ELEMENT)
	{
		return (0);
	}
		
	pAnchorName = ((HTMLAnchor *) pElement)->Name();
	if (pAnchorName && *pAnchorName)
	{
		return (!webc_stricmp(pAnchorName, mpName));
	}
		
	return (0);
}

////////////////////////////////////////////////////////////////////////////////////
// class HTMLAreaByNameFinder
//
////////////////////////////////////////////////////////////////////////////////////
HTMLAreaByNameFinder::HTMLAreaByNameFinder(WEBC_CHAR *name)
{
	mpName = name;
}

HTMLAreaByNameFinder::~HTMLAreaByNameFinder()
{
}

WEBC_BOOL HTMLAreaByNameFinder::Check(HTMLElement *pElement)
{
	WEBC_CHAR *pAreaName;
	if (pElement->Type() != HTML_AREA_ELEMENT)
	{
		return (0);
	}
		
	pAreaName = ((HTMLArea *) pElement)->Name();
	if (pAreaName && *pAreaName)
	{
		return (!webc_stricmp(pAreaName, mpName));
	}
		
	return (0);
}

////////////////////////////////////////////////////////////////////////////////////
// class HTMLFrameByNameFinder
//
////////////////////////////////////////////////////////////////////////////////////

HTMLFrameByNameFinder::HTMLFrameByNameFinder(WEBC_CHAR *name)
{
	mpName = name;
}

HTMLFrameByNameFinder::~HTMLFrameByNameFinder()
{
}

WEBC_BOOL HTMLFrameByNameFinder::Check(HTMLElement *pElement)
{
  #if (WEBC_SUPPORT_FRAMES)
	WEBC_CHAR *pFrameName;

	if ((pElement->Type() == HTML_FRAME_ELEMENT) || (pElement->Type() == HTML_IFRAME_ELEMENT))
	{
		pFrameName = ((HTMLFrame *) pElement)->Name();
		if (pFrameName && *pFrameName)
		{
			return (!webc_stricmp(pFrameName, mpName));
		}
	}
  #endif // WEBC_SUPPORT_FRAMES
		
	return (0);
}

HTMLAllLoadEventFinder::HTMLAllLoadEventFinder(HTMLEvent *e)
{
	mpEvent = e;
}

WEBC_BOOL HTMLAllLoadEventFinder::Check(HTMLElement *pElem)
{
	pElem->Event(mpEvent);
	return (0);
}

/*************************************************************************
* class HTMLCountTypeWithNameFinder
* 
* Keep track of total number of elements of this type
*
*************************************************************************/
HTMLCountByTypeFinder::HTMLCountByTypeFinder(HTMLElementType type)
{	
	mType = type;
	miLen = 0;
}
WEBC_BOOL HTMLCountByTypeFinder::Check(HTMLElement *pElem)
{
	if (pElem->Type() == mType)
	{
		miLen++;
	}
	return (0);
}

/*************************************************************************
* class HTMLImageByNameFinder
* 
* Find the Image with the given name 
*
*************************************************************************/
HTMLImageByNameFinder::HTMLImageByNameFinder(WEBC_CHAR *name)
{
	mpName = name;
}

WEBC_BOOL HTMLImageByNameFinder::Check(HTMLElement *pElement)
{
WEBC_CHAR *pImageName;

	if (pElement->Type() != HTML_IMAGE_ELEMENT)
	{
		return (0);
	}
		
	pImageName = pElement->Name();
	if (pImageName && *pImageName)
	{
		return (!webc_stricmp(pImageName, mpName));
	}
		
	return (0);
}

/*************************************************************************
* class HTMLCountByNameFinder
* 
* Count the Elements with the given name 
*
*************************************************************************/
HTMLCountByNameFinder::HTMLCountByNameFinder(WEBC_CHAR *name)
{
	mpName = name;
	miLen = 0;
}

WEBC_BOOL HTMLCountByNameFinder::Check(HTMLElement *pElement)
{
	WEBC_CHAR *name = pElement->Name();
	if (name && *name && mpName)
	{
		if (!webc_stricmp(name, mpName))
		{
			miLen++;
		}
	}
	return (0);
}

/*************************************************************************
* class HTMLNthByName 
* 
* Get the Element with the given name at the given index
*
*************************************************************************/
HTMLNthByNameFinder::HTMLNthByNameFinder(WEBC_CHAR *name, int index)
{
	mpName = name;
	miCount = index;
}

WEBC_BOOL HTMLNthByNameFinder::Check(HTMLElement *pElement)
{
	WEBC_CHAR *name = pElement->Name();
	if (name && *name && mpName)
	{
		if (!webc_stricmp(name, mpName))
		{
			if (miCount > 0)
			{		
				miCount--;
				return (0);
			}			
			return 1;
		}
	}
	return 0;
}//end Check
	

/*************************************************************************
* class HTMLPrevOfTypeFinder
* 
* Find the previous element with the same type as the given element
*
*************************************************************************/

HTMLPrevOfTypeFinder::HTMLPrevOfTypeFinder(HTMLElement *pCurrent)
{
	mpCurrent = pCurrent;
	mpPrev = 0;
}

WEBC_BOOL HTMLPrevOfTypeFinder::Check(HTMLElement *pElement)
{
	if (pElement == mpCurrent)
	{
		return (1);
	}
	
	if (pElement->Type() == mpCurrent->Type())
	{
		mpPrev = pElement;
	}
	
	return (0);
}
	

/*************************************************************************
* class HTMLNextOfTypeFinder
* 
* Find the next element with the same type as the given element
*
*************************************************************************/

HTMLNextOfTypeFinder::HTMLNextOfTypeFinder(HTMLElement *pCurrent)
{
	mbTakeNext = 0;
	mpCurrent = pCurrent;
}

WEBC_BOOL HTMLNextOfTypeFinder::Check(HTMLElement *pElement)
{
	if (mbTakeNext && (pElement->Type() == mpCurrent->Type()))
	{
		return (1);
	}
	
	if (pElement == mpCurrent)
	{
		mbTakeNext = 1;
	}
	
	return (0);
}

/*************************************************************************
* class HTMLNameTypeFinder
* 
*************************************************************************/

HTMLNameTypeFinder::HTMLNameTypeFinder (WEBC_CHAR *name, HTMLElementType type)
{
	mpName = name;
	mType = type;
}

WEBC_BOOL HTMLNameTypeFinder::Check(HTMLElement *pElement)
{
WEBC_CHAR *pName;

	if (pElement->Type() != mType)
	{
		return (0);
	}
		
	pName = pElement->Name();
	if (pName && *pName)
	{
		return (!webc_stricmp(pName, mpName));
	}
		
	return (0);
}

/*************************************************************************
* class HTMLIdTypeFinder
* 
*************************************************************************/

HTMLIdTypeFinder::HTMLIdTypeFinder (WEBC_CHAR *id, HTMLElementType type)
{
	mpId = id;
	mType = type;
}

WEBC_BOOL HTMLIdTypeFinder::Check(HTMLElement *pElement)
{
WEBC_CHAR *pId;

	if (pElement->Type() != mType)
	{
		return (0);
	}
		
	pId = pElement->Id();
	if (pId && *pId && mpId)
	{
		return (!webc_stricmp(pId, mpId));
	}
		
	return (0);
}

/*************************************************************************
* class HTMLNameTypeFinder
* 
*************************************************************************/

HTMLFrameWithDocFinder::HTMLFrameWithDocFinder(HTMLDocument *pDoc)
{
	mpDocument = pDoc;
}

WEBC_BOOL HTMLFrameWithDocFinder::Check(HTMLElement *pElem)
{
#if (WEBC_SUPPORT_FRAMES)
	if ((pElem->Type() == HTML_FRAME_ELEMENT) || (pElem->Type() == HTML_IFRAME_ELEMENT))
	{
		if (mpDocument == ((HTMLFrame *)pElem)->FrameDocument())
		{
			return 1;
		}
	}
#endif // WEBC_SUPPORT_FRAMES	
	return 0;
}

/*************************************************************************
* class HTMLCountAllFinder
* 
*************************************************************************/

WEBC_BOOL HTMLCountAllFinder::Check(HTMLElement *pElem)
{
	miCount++;
	return 0;
}

/*************************************************************************
* class HTMLCountBeforeFinder
* 
*************************************************************************/

WEBC_BOOL HTMLCountBeforeFinder::Check(HTMLElement *pElem)
{
	if (pElem == mpElement)
	{
		return (1);
	}
		
	miCount++;
	return 0;
}

/*************************************************************************
* class HTMLGenericFinder
*
*************************************************************************/


WEBC_BOOL HTMLGenericFinder::Check(HTMLElement *pElem)
{

	if (mpCallback)
	{
		return(mpCallback(pElem,miChore,mpContext));
	}
	return(0);
}


/*************************************************************************
* class HTMLCountAfterFinder
* 
*************************************************************************/

WEBC_BOOL HTMLCountAfterFinder::Check(HTMLElement *pElem)
{
	if (!mpElement)
	{
		miCount++;
	}

	if (pElem == mpElement)
	{
		mpElement = 0;
	}

	return 0;
}

/*************************************************************************
* class HTMLNameTypeFinder
* 
*************************************************************************/

WEBC_BOOL HTMLNthOfAnyTypeFinder::Check(HTMLElement *pElem)
{
	if (miCount > 0)
	{
		miCount--;
		return 0;
	}
	return 1;
}

/*************************************************************************
* class HTMLIdNameTypeIndexFinder
* 
*************************************************************************/

WEBC_BOOL HTMLIdNameTypeIndexFinder::Check(HTMLElement *pElement)
{
	if (mpId && (!pElement->Id() || webc_stricmp(pElement->Id(), mpId)))
	{
		return 0;
	}
	
	if (mpName && (!pElement->Name() || webc_stricmp(pElement->Name(), mpName)))
	{
		return 0;
	}
	
	if ((mType != HTML_ELEMENT_ANY) && (pElement->Type() != mType))
	{
		return 0;
	}
	
	if (miIndex == 0)
	{
		return 1;
	}
	
	miIndex--;
	return 0;
}

/*************************************************************************
* class HTMLIdNameTypeIndexFinder8
* 
*************************************************************************/

WEBC_BOOL HTMLIdNameTypeIndexFinder8::Check(HTMLElement *pElement)
{
	if (mpId && (!pElement->Id() || webc_c_stricmp(pElement->Id(), mpId)))
	{
		return 0;
	}
	
	if (mpName && (!pElement->Name() || webc_c_stricmp(pElement->Name(), mpName)))
	{
		return 0;
	}
	
	if ((mType != HTML_ELEMENT_ANY) && (pElement->Type() != mType))
	{
		return 0;
	}
	
	if (miIndex == 0)
	{
		return 1;
	}
	
	miIndex--;
	return 0;
}

/*************************************************************************
* class HTMLNextByIdNameTypeIndexFinder
* 
*************************************************************************/

WEBC_BOOL HTMLNextByIdNameTypeIndexFinder::Check(HTMLElement *pElement)
{	
	if (mpElement == pElement)
	{
		mbTakeNext = 1;
		return 0;
	}

	if (mpId && (!pElement->Id() || webc_c_stricmp(pElement->Id(), mpId)))
	{
		return 0;
	}
	
	if (mpName && (!pElement->Name() || webc_c_stricmp(pElement->Name(), mpName)))
	{
		return 0;
	}
	
	if ((mType != HTML_ELEMENT_ANY) && (pElement->Type() != mType))
	{
		return 0;
	}
	
	if ((miIndex == 0) && mbTakeNext)
	{
		return 1;
	}
	
	if (mbTakeNext)
	{
		miIndex--;
	}

	return 0;
}

/*************************************************************************
* class HTMLPrevByIdNameTypeIndexFinder
* 
*************************************************************************/

WEBC_BOOL HTMLPrevByIdNameTypeIndexFinder::Check(HTMLElement *pElement)
{
	if (mpElement == pElement)
	{
		return 1;
	}
	
	if (mpId && (!pElement->Id() || webc_c_stricmp(pElement->Id(), mpId)))
	{
		return 0;
	}
	
	if (mpName && (!pElement->Name() || webc_c_stricmp(pElement->Name(), mpName)))
	{
		return 0;
	}
	
	if ((mType != HTML_ELEMENT_ANY) && (pElement->Type() != mType))
	{
		return 0;
	}
	
	if (miIndex == 0)
	{
		mpFound = pElement;
	}
	else
	{
		miIndex--;
	}

	return 0;
}

/**
 * HTMLTypeAndNameOrIdFinder
 */
WEBC_BOOL HTMLTypeAndNameOrIdFinder::Check(HTMLElement *pElement)
{
	if (pElement->Type() == mType &&
		pElement->IsNameOrId(mpNameOrId))
		return 1;
	return 0;
}

/**
 * HTMLNameOrIdFinder
 */
WEBC_BOOL HTMLNameOrIdFinder::Check(HTMLElement *pElement)
{
	 return (pElement->IsNameOrId(mpNameOrId));
}


////////////////////////////////////////////////////////////////////////////////////
// class HTMLLastVisibleTabFinder
//
////////////////////////////////////////////////////////////////////////////////////

HTMLLastVisibleTabFinder::HTMLLastVisibleTabFinder()
{
	mpLastTab = 0;
	miHighestTabIndex = INVALID_TABINDEX;
}

HTMLLastVisibleTabFinder::~HTMLLastVisibleTabFinder()
{
}

WEBC_BOOL HTMLLastVisibleTabFinder::Check(HTMLElement *pElement)
{
short iTabIndex = pElement->TabIndex();

	if ((iTabIndex != INVALID_TABINDEX) && !pElement->IsVisible())
	{
		iTabIndex = INVALID_TABINDEX;
	}
	
	if ((iTabIndex != INVALID_TABINDEX) && (TABINDEX_CMP(iTabIndex, miHighestTabIndex) >= 0))
	{
		mpLastTab = pElement;
		miHighestTabIndex = pElement->TabIndex();
	}
	
	return (0);
}

HTMLElement *HTMLLastVisibleTabFinder::Found(void)
{
	return (mpLastTab);
}

////////////////////////////////////////////////////////////////////////////////////
// class HTMLFirstVisibleTabFinder
//
////////////////////////////////////////////////////////////////////////////////////

HTMLFirstVisibleTabFinder::HTMLFirstVisibleTabFinder()
{
	mpFirstTab = 0;
	miLowestTabIndex = -1;
}

HTMLFirstVisibleTabFinder::~HTMLFirstVisibleTabFinder()
{
}

WEBC_BOOL HTMLFirstVisibleTabFinder::Check(HTMLElement *pElement)
{
short iTabIndex = pElement->TabIndex();

	if ((iTabIndex != INVALID_TABINDEX) && !pElement->IsVisible())
	{
		iTabIndex = INVALID_TABINDEX;
	}
	
	// if this one is valid and we don't yet have a valid one or this one 
	//  is strictly lower than the one we already have
	if ((iTabIndex != INVALID_TABINDEX) && 
		((miLowestTabIndex == INVALID_TABINDEX)	|| (TABINDEX_CMP(iTabIndex, miLowestTabIndex) < 0)))
	{
		mpFirstTab = pElement;
		miLowestTabIndex = iTabIndex;
		if (miLowestTabIndex == 1)
		{
			// if the index of this one is 1, then we know already it is the first one 
			return 1;
		}
	}
	
	// else we keep searching
	return 0;
}

HTMLElement *HTMLFirstVisibleTabFinder::Found(void)
{
	return (mpFirstTab);
}


/*************************************************************************
* class HTMLStyleUpdateFinder
* 
*  Calls StyleUpdate(0) on all elements
*
*************************************************************************/
HTMLStyleUpdateFinder::HTMLStyleUpdateFinder()
{
}

HTMLStyleUpdateFinder::~HTMLStyleUpdateFinder()
{
}

WEBC_BOOL HTMLStyleUpdateFinder::Check(HTMLElement *pElement) 
{
	pElement->StyleUpdate();
	
	return (0);
}	


/*************************************************************************
* class HTMLSelectedOptionFinder
* 
*************************************************************************/
HTMLSelectedOptionFinder::HTMLSelectedOptionFinder()
{
	miNum = 0;
	miIndex = 0;
}

HTMLSelectedOptionFinder::~HTMLSelectedOptionFinder()
{
}

// HTMLElementFinder interface
WEBC_BOOL HTMLSelectedOptionFinder::Check(HTMLElement *pElement)
{
	if (pElement->Type() == HTML_OPTION_ELEMENT)
	{		
		HTMLOption *pOption = (HTMLOption *) pElement;
		if (pOption->DefSelected())
		{
			miIndex = miNum;
		}

		miNum++;
	}
	
	return (0);
}

int HTMLSelectedOptionFinder::Index(void)
{
	return (miIndex);
}


/*************************************************************************
* class HTMLDisplayElementFinder
* 
*  Finds the Nth element with a DisplayElement
*
*************************************************************************/

HTMLDisplayElementFinder::HTMLDisplayElementFinder(HTMLElement *pElem, int n) 
{ 
	miNum = n; 
	mpElement = pElem;
}

HTMLDisplayElementFinder::~HTMLDisplayElementFinder()
{
}

// HTMLElementFinder interface
WEBC_BOOL HTMLDisplayElementFinder::Check(HTMLElement *pElement) 
{
	// first we need to find the element specified (if non-null)
	if (mpElement)
	{
		if (pElement != mpElement)
		{
			return 0;
		}
		
		mpElement = 0;
	}
	
	if (pElement->GetDisplayElement(WEBC_FALSE))
	{
		if (miNum > 0)
		{
			miNum--;
		}
		else
		{
			return (1);
		}
	}
	
	return (0);
}

////////////////////////////////////////////////////////////////////////////////////
// class HTMLElementByClassFinder
//
////////////////////////////////////////////////////////////////////////////////////

HTMLElementByClassFinder::HTMLElementByClassFinder(const WEBC_CHAR *pClass)
{
	mpClassName = pClass;
}

HTMLElementByClassFinder::~HTMLElementByClassFinder()
{
}

WEBC_BOOL HTMLElementByClassFinder::Check(HTMLElement *pElement)
{
	WEBC_CHAR *elemClass;
	Enumerator *e = pElement->GetClasses();
	
	if (e)
	{
		elemClass = (WEBC_CHAR *) e->Next();
		while (elemClass && webc_stricmp(elemClass, mpClassName))
		{
			elemClass = (WEBC_CHAR *) e->Next();
		}
		e->Dispose();
		
		if (elemClass)
		{
			return (1);
		}
	}
	
	return (0);
}

/*************************************************************************
* class HTMLSetFlagFinder
* 
*  sets or clears flags of an HTMLElement and its children
*
*************************************************************************/

HTMLSetFlagFinder::HTMLSetFlagFinder(HELEM_FLAGS mask, int onBool)
{
	mMask = mask;
	mbOn = onBool;
}

HTMLSetFlagFinder::~HTMLSetFlagFinder()
{
}

WEBC_BOOL HTMLSetFlagFinder::Check(HTMLElement *pElement)
{
	if (mbOn)
	{
		pElement->SetFlag(mMask);
	}
	else
	{
		pElement->ClearFlag(mMask);
	}
	
	return (0);
}
	

/*************************************************************************
* class HTMLLastListItemFinder
* 
*  Find the last list item in the given list
*
*************************************************************************/

HTMLLastListItemFinder::HTMLLastListItemFinder(HTMLList* parentList)
{
	mParentList = parentList;
	mListItem = 0;
}

HTMLLastListItemFinder::~HTMLLastListItemFinder(void)
{
}

WEBC_BOOL HTMLLastListItemFinder::Check(HTMLElement *pElement)
{
	if (pElement->Type() == HTML_LIST_ITEM_ELEMENT)
	{
		if (((HTMLListItem*)pElement)->ParentList() == mParentList)
		{
			mListItem = (HTMLListItem*) pElement;
		}
	}
	
	return (0);
}

HTMLListItem *HTMLLastListItemFinder::Found(void)
{
	return mListItem;
}

/*************************************************************************
* class HTMLParentListFinder
* 
*  Find the last element of a given type
*
*************************************************************************/

HTMLParentListFinder::HTMLParentListFinder(void)
{
	mParentList = 0;
}

HTMLParentListFinder::~HTMLParentListFinder(void)
{
}

WEBC_BOOL HTMLParentListFinder::Check(HTMLElement *pElement)
{
	if (pElement->Type() == HTML_LIST_ELEMENT)
	{
		mParentList = (HTMLList*) pElement;
		return (1);
	}
	else if (pElement->Type() == HTML_TABLE_ELEMENT)
	{
		return (1);
	}

	return (0);
}

HTMLList* HTMLParentListFinder::Found(void)
{
	return (mParentList);
}
WEBC_BOOL HTMLFocusElementFinder::Check(HTMLElement *pElement)
{
		if (pElement->GetDisplayElement(WEBC_FALSE))
		{
			if (pElement->GetDisplayElement(WEBC_FALSE)->GetFlags() & DISPLAY_FLAG_FOCUS)
			{
				mbFound = pElement;
				return (1);
			}
		}
		return (0);
}