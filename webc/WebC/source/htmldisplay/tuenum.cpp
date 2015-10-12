#include "textunit.hpp"
#include "htmldom.hpp"
#include "tuenum.hpp"
#include "webcutf.h"
#include "hstring.hpp"
#include "htmldoc.hpp"
#include "dbgtimer.h"


TUOpenNoBr  *gpOpenNoBr = 0;
TUCloseNoBr *gpCloseNoBr = 0;

////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////

DisplayTUEnumerator::DisplayTUEnumerator(HTMLElement *pRoot)
{
	miDepth = -1;
	mpCurrent = (pRoot && pRoot->FirstChild())? pRoot : 0;
	mUnitType = TU_TYPE_INLINE;
	mbOpenNoBr = WEBC_FALSE;
	mbCloseNoBr = WEBC_FALSE;
}

DisplayTUEnumerator::~DisplayTUEnumerator(void)
{
}


TextUnit *DisplayTUEnumerator::GetNextUnit(void)
{
	WEBC_BOOL tryAgain = WEBC_FALSE;

	WEBC_DEBUG_TIMER_START(WEBC_DEBUG_DISPLAYTUENUMERATOR_GETNEXTUNIT);
		
	do
	{
		HTMLElement *pNextElement = GetNext();
		
		if (mbOpenNoBr)
		{
			WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_DISPLAYTUENUMERATOR_GETNEXTUNIT);
			return (gpOpenNoBr);
		}
		
		if (mbCloseNoBr)
		{
			WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_DISPLAYTUENUMERATOR_GETNEXTUNIT);
			return (gpCloseNoBr);
		}	

		if (pNextElement)
		{
			TextUnit *tu = pNextElement->GetTextUnit();	
			WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_DISPLAYTUENUMERATOR_GETNEXTUNIT);	
			return (tu);
		}
	} while (tryAgain);

	WEBC_DEBUG_TIMER_STOP(WEBC_DEBUG_DISPLAYTUENUMERATOR_GETNEXTUNIT);		
	return (0);
}


HTMLElement *DisplayTUEnumerator::GetNext(void)
{
	if (mbOpenNoBr)
	{
		mbOpenNoBr = WEBC_FALSE;
		goto continue_after_hold_wrap;
	}

	if (mbCloseNoBr)
	{
		mbCloseNoBr = WEBC_FALSE;
		goto continue_after_release_wrap;
	}

	while (mpCurrent)
	{
		if ((mUnitType == TU_TYPE_INLINE  || mUnitType == TU_TYPE_INLINE_BLOCK) && mpCurrent->FirstChild())
		{
			if (mpCurrent->GetChildWrap() == TU_WRAP_NONE)
			{
				mbOpenNoBr = WEBC_TRUE;
				return (0);
			}
continue_after_hold_wrap:
			mpCurrent = mpCurrent->FirstChild();
			miDepth++;
		}
		else
		{
			while (!mpCurrent->mpNext && miDepth > 0)
			{
				mpCurrent = mpCurrent->mpParent;
				miDepth--;
				if (mpCurrent->GetChildWrap() == TU_WRAP_NONE)
				{
					mbCloseNoBr = WEBC_TRUE;
					return (0);
				}
continue_after_release_wrap:;
			}

			mpCurrent = mpCurrent->mpNext;
		}
		
		mUnitType = (mpCurrent)? mpCurrent->GetUnitType() : TU_TYPE_NODISPLAY;
		if (mUnitType != TU_TYPE_INLINE && mUnitType != TU_TYPE_INLINE_BLOCK)
		{
			break;
		}
	}

	return (mpCurrent);
}

MinTUEnumerator::MinTUEnumerator (HTMLElement *pRoot)
	: DisplayTUEnumerator(pRoot), mStringDisplay(0)
{
	mpLastString = 0;	
	if (pRoot && pRoot->GetDocument() && pRoot->GetDocument()->GetDisplayManager())
	{
		mStringDisplay.SetManager(pRoot->GetDocument()->GetDisplayManager());
	}
}
		
MinTUEnumerator::~MinTUEnumerator(void)
{
}
	
TextUnit *MinTUEnumerator::GetNextUnit(void)
{
HTMLElement *pNode;

	if (mpLastString)
	{
		int len = mpLastString->GetStringLength();
		HTMLString *pString = mpLastString->GetStringElement();
		miOffset += len;
		WebChar *str = pString->GetString();
		if ((str[miOffset] == (WebChar) 0) || (len == 0))
		{
			mpLastString = 0;	
		}
		else
		{
			mStringDisplay.SetStringPosition(miOffset, 0);
			return (mpLastString);			
		}
	}

	pNode = GetNext();

	if (mbOpenNoBr)
	{
		return (gpOpenNoBr);
	}
	else if (mbCloseNoBr)
	{
		return (gpCloseNoBr);
	}	
	
	if (pNode)
	{
		if (pNode->Type() == HTML_STRING_ELEMENT)
		{
			miOffset = 0;
			HTMLString *pString = (HTMLString *) pNode;
			mStringDisplay.SetStringElement(pString);
			mStringDisplay.SetStringPosition(miOffset, 0);
			mpLastString = &mStringDisplay;
			return (mpLastString);
		}
		else
		{
			return (pNode->GetTextUnit());
		}
	}

	return (0);	
}

MaxTUEnumerator::MaxTUEnumerator (HTMLElement *pRoot)
	: DisplayTUEnumerator(pRoot), mStringDisplay(0)
{
	mpLastString = 0;	
	if (pRoot && pRoot->GetDocument() && pRoot->GetDocument()->GetDisplayManager())
	{
		mStringDisplay.SetManager(pRoot->GetDocument()->GetDisplayManager());
	}
}
		
MaxTUEnumerator::~MaxTUEnumerator(void)
{
}
	
TextUnit *MaxTUEnumerator::GetNextUnit(void)
{
HTMLElement *pNode;

	if (mpLastString)
	{
		int len = mpLastString->GetStringLength();
		HTMLString *pString = mpLastString->GetStringElement();
		miOffset += len;
		WebChar *str = pString->GetString();
		if ((str[miOffset] == (WebChar) 0) || (len == 0))
		{
			mpLastString = 0;	
		}
		else
		{
			mStringDisplay.SetStringPosition(miOffset, 0);
			return (mpLastString);			
		}
	}

	pNode = GetNext();

	if (mbOpenNoBr)
	{
		return (gpOpenNoBr);
	}
	else if (mbCloseNoBr)
	{
		return (gpCloseNoBr);
	}	
	
	if (pNode)
	{
		if  (pNode->Type() == HTML_STRING_ELEMENT)
		{
			miOffset = 0;
			HTMLString *pString = (HTMLString *) pNode;
			mStringDisplay.SetStringElement(pString);
			mStringDisplay.SetStringPosition(miOffset, 0);
			mpLastString = &mStringDisplay;
			return (mpLastString);
		}
		else
		{
			return (pNode->GetTextUnit());
		}
	}
	
	return (0);
}

PositionedTUEnumerator::PositionedTUEnumerator (
		HTMLElement* root, 
		DisplayElement* displayRoot, 
		DISPLAY_INT topPadding
	)
{
	mDisplayRoot = displayRoot;
	mAutoIndentTop = topPadding;
	mpCurrent = root->FirstChild();
	miDepth = 0;
}

PositionedTUEnumerator::~PositionedTUEnumerator(void)
{
}

TextUnit *PositionedTUEnumerator::GetNextUnit(void)
{
	if (mpCurrent)
	{
		TextUnit *tu = (mpCurrent->Type() != HTML_STRING_ELEMENT)? mpCurrent->GetTextUnit() : 0;
		
		// mpCurrent is the next element that MIGHT be absolutely positioned
		//  we must traverse the tree until we find the next actual one
		while (mpCurrent && 
		       ((mpCurrent->Type() == HTML_STRING_ELEMENT) || 
		        (tu && 
		         (tu->GetUnitType() == TU_TYPE_NODISPLAY || tu->GetPosition() == TU_POSITION_STATIC))))
		{			
			if (mpCurrent->FirstChild())
			{
				miDepth++;
				mpCurrent = mpCurrent->FirstChild();
			}
			else
			{
				while (mpCurrent && (!mpCurrent->mpNext && miDepth > 0))
				{
					miDepth--;
					updateAutoIndent(mpCurrent);
					mpCurrent = mpCurrent->mpParent;
				}

				if(mpCurrent)
				{
				updateAutoIndent(mpCurrent);
				mpCurrent = mpCurrent->mpNext;
				}
			}
			
			if (mpCurrent) 
			{
				tu = (mpCurrent->Type() != HTML_STRING_ELEMENT)? mpCurrent->GetTextUnit() : 0;
			}
		}

		if (mpCurrent)
		{
			// if we find one, step to the side in the tree so
			//  we'll start our search there next time round
			while (!mpCurrent->mpNext && miDepth > 0)
			{
				miDepth--;
				updateAutoIndent(mpCurrent);
				mpCurrent = mpCurrent->mpParent;
			}
	
			updateAutoIndent(mpCurrent);
			mpCurrent = mpCurrent->mpNext;
			
			return (tu);
		}		
	}
	
	return (0);
}

void PositionedTUEnumerator::updateAutoIndent (HTMLElement* elem)
{
	if (elem->GetUnitType() != TU_TYPE_NODISPLAY &&
		elem->GetUnitType() != TU_TYPE_INLINE &&
		elem->GetUnitType() != TU_TYPE_INLINE_BLOCK &&
		elem->GetPosition() == TU_POSITION_STATIC &&
		elem->GetFloat() == TU_FLOAT_NONE)
	{
		DisplayElement* display = elem->GetLastDisplayElement();
		if (display && display != mDisplayRoot)
		{
			DISPLAY_INT autoIndentTop = display->mRect.Height();
			
			do
			{
				autoIndentTop += display->mRect.top;

				display = display->mpParent;

				if (display == mDisplayRoot)
				{
					mAutoIndentTop = autoIndentTop;
					break;
				}
			}
			while (display);
		}
	}
}

DISPLAY_INT PositionedTUEnumerator::getAutoIndentTop (void)
{
	return (mAutoIndentTop);
}
