/*
|  FORMATCTX.CPP -
| 
|  EBS -
| 
| 
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

/*****************************************************************************/
/* Header files
 *****************************************************************************/

#include "formatctx.hpp"
#include "webcmem.h"
#include "webcassert.h"

/*****************************************************************************/
/* Macros
 *****************************************************************************/

/*****************************************************************************/
/* Types
 *****************************************************************************/

/*****************************************************************************/
/* Function Prototypes
 *****************************************************************************/

/*****************************************************************************/
/* Data
 *****************************************************************************/

/*****************************************************************************/
/* Function Definitions
 *****************************************************************************/

//=============================================================================
// FormatContext methods
//=============================================================================

//-----------------------------------------------------------------------------
FormatContext::FormatContext (void)
{
}

//-----------------------------------------------------------------------------
FormatContext::~FormatContext (void)
{
}

//=============================================================================
// FormatContext methods
//=============================================================================

//-----------------------------------------------------------------------------
FormatContextFactory::FormatContextFactory (void)
{
}

//-----------------------------------------------------------------------------
FormatContextFactory::~FormatContextFactory (void)
{
}

//-----------------------------------------------------------------------------
IPositionedFormatContext* FormatContextFactory::newPositioned (
		WebRect*         containingBlock, 
		WEBC_BOOL        parentHeightIsAuto,
		int              textDirection,
		DISPLAY_INT      parentBorderLeft,
		DISPLAY_INT      parentBorderTop,
		DISPLAY_INT      parentBorderRight,
		DISPLAY_INT      parentBorderBottom,
		DISPLAY_INT      autoIndentLeft,
		DISPLAY_INT      autoIndentTop,
		DISPLAY_INT      autoIndentRight
	)
{
	PositionedFormatContext* context;
	
	WEBC_NEW_VERBOSE(context, 
	         PositionedFormatContext (
					containingBlock, 
					parentHeightIsAuto,
					textDirection,
					parentBorderLeft,
					parentBorderTop,
					parentBorderRight,
					parentBorderBottom,
					autoIndentLeft,
					autoIndentTop,
					autoIndentRight
				),"FormatContextFactory");						
				
	return (context);
}


//=============================================================================
// FormatContext methods
//=============================================================================

//-----------------------------------------------------------------------------
PositionedFormatContext::PositionedFormatContext (
		WebRect*         containingBlock, 
		WEBC_BOOL        parentHeightIsAuto,
		int              textDirection,
		DISPLAY_INT      parentBorderLeft,
		DISPLAY_INT      parentBorderTop,
		DISPLAY_INT      parentBorderRight,
		DISPLAY_INT      parentBorderBottom,
		DISPLAY_INT      autoIndentLeft,
		DISPLAY_INT      autoIndentTop,
		DISPLAY_INT      autoIndentRight
 	)
{
	mContainingBlock.Set(containingBlock); 
	mParentHeightIsAuto = parentHeightIsAuto;
	mTextDirection = textDirection;
	mParentBorderLeft = parentBorderLeft;
	mParentBorderTop = parentBorderTop;
	mParentBorderRight = parentBorderRight;
	mParentBorderBottom = parentBorderBottom;
	mAutoIndentLeft = autoIndentLeft;
	mAutoIndentTop = autoIndentTop;
	mAutoIndentRight = autoIndentRight;
}
	 	
//-----------------------------------------------------------------------------
PositionedFormatContext::~PositionedFormatContext (void)
{
}

//-----------------------------------------------------------------------------
WEBC_BOOL PositionedFormatContext::equals  (FormatContext* other)
{	
	return (other == this);
}

//-----------------------------------------------------------------------------
void PositionedFormatContext::recycle (void)
{
	WEBC_DELETE(this);
}

//-----------------------------------------------------------------------------
void PositionedFormatContext::getContainingBlock (WebRect* rect)
{
	rect->Set(mContainingBlock);
}

//-----------------------------------------------------------------------------
WEBC_BOOL PositionedFormatContext::parentHeightIsAuto (void)
{
	return (mParentHeightIsAuto);
}

//-----------------------------------------------------------------------------
int PositionedFormatContext::getTextDirection (void)
{
	return (mTextDirection);
}

//-----------------------------------------------------------------------------
DISPLAY_INT	PositionedFormatContext::getParentBorderLeft (void)
{
	return (mParentBorderLeft);
}

//-----------------------------------------------------------------------------
DISPLAY_INT	PositionedFormatContext::getParentBorderTop (void)
{
	return (mParentBorderTop);
}

//-----------------------------------------------------------------------------
DISPLAY_INT	PositionedFormatContext::getParentBorderRight (void)
{
	return (mParentBorderRight);
}

//-----------------------------------------------------------------------------
DISPLAY_INT	PositionedFormatContext::getParentBorderBottom (void)
{
	return (mParentBorderBottom);
}

//-----------------------------------------------------------------------------
DISPLAY_INT	PositionedFormatContext::getAutoIndentLeft (void)
{
	return (mAutoIndentLeft);
}

//-----------------------------------------------------------------------------
DISPLAY_INT	PositionedFormatContext::getAutoIndentTop (void)
{
	return (mAutoIndentTop);
}

//-----------------------------------------------------------------------------
DISPLAY_INT	PositionedFormatContext::getAutoIndentRight (void)
{
	return (mAutoIndentRight);
}


//=============================================================================
// AutoTopPositionedFormatContext methods
//=============================================================================

//-----------------------------------------------------------------------------
AutoTopPositionedFormatContext::AutoTopPositionedFormatContext (
		IPositionedFormatContext* baseContext,
		DISPLAY_INT autoIndentTop
	)
{
	WEBC_ASSERT(baseContext);
	
	mAutoIndentTop = autoIndentTop;
	mBaseContext = baseContext;	
	mBaseContext->claim();	
}
	
//-----------------------------------------------------------------------------
AutoTopPositionedFormatContext::~AutoTopPositionedFormatContext (void)
{
}

//-----------------------------------------------------------------------------
WEBC_BOOL AutoTopPositionedFormatContext::equals  (FormatContext* other)
{	
	return (other == this);
}

//-----------------------------------------------------------------------------
void AutoTopPositionedFormatContext::recycle (void)
{
	mBaseContext->release();
	WEBC_DELETE(this);	
}

//-----------------------------------------------------------------------------
void AutoTopPositionedFormatContext::getContainingBlock (WebRect* rect)
{
	mBaseContext->getContainingBlock(rect);
}

//-----------------------------------------------------------------------------
WEBC_BOOL AutoTopPositionedFormatContext::parentHeightIsAuto (void)
{
	return mBaseContext->parentHeightIsAuto();
}

//-----------------------------------------------------------------------------
int AutoTopPositionedFormatContext::getTextDirection (void)
{
	return mBaseContext->getTextDirection();
}

//-----------------------------------------------------------------------------
DISPLAY_INT	AutoTopPositionedFormatContext::getParentBorderLeft (void)
{
	return mBaseContext->getParentBorderLeft();
}

//-----------------------------------------------------------------------------
DISPLAY_INT	AutoTopPositionedFormatContext::getParentBorderTop (void)
{
	return mBaseContext->getParentBorderTop();
}

//-----------------------------------------------------------------------------
DISPLAY_INT	AutoTopPositionedFormatContext::getParentBorderRight (void)
{
	return mBaseContext->getParentBorderRight();
}

//-----------------------------------------------------------------------------
DISPLAY_INT	AutoTopPositionedFormatContext::getParentBorderBottom (void)
{
	return mBaseContext->getParentBorderBottom();
}

//-----------------------------------------------------------------------------
DISPLAY_INT	AutoTopPositionedFormatContext::getAutoIndentLeft (void)
{
	return mBaseContext->getAutoIndentLeft();
}

//-----------------------------------------------------------------------------
DISPLAY_INT	AutoTopPositionedFormatContext::getAutoIndentTop (void)
{
	return (mAutoIndentTop);
}

//-----------------------------------------------------------------------------
DISPLAY_INT	AutoTopPositionedFormatContext::getAutoIndentRight (void)
{
	return mBaseContext->getAutoIndentRight();
}
