/*
| FORMATCTX.HPP - Format Context 
| 
|  EBS - 
| 
| 
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/ 

#ifndef __FORMATCTX_HPP__
#define __FORMATCTX_HPP__

#include "webctypes.h"
#include "webrect.hpp"
#include "refcount.hpp"

typedef enum e_FormatMethod
{
	FORMAT_METHOD_BLOCK,
	FORMAT_METHOD_FIXED,
	FORMAT_METHOD_FLOAT,
	FORMAT_METHOD_INLINE,
	FORMAT_METHOD_POSITIONED,
	FORMAT_METHOD_TABLE
}
FormatMethod;

class FormatContext : public ReferenceCounted
{
 public:
 	FormatContext (void);
 	virtual ~FormatContext (void);
 
 	virtual WEBC_BOOL    equals  (FormatContext* other) = 0;
 	virtual FormatMethod method  (void) = 0;
};

class IPositionedFormatContext : public FormatContext
{
 public:
 	virtual FormatMethod    method (void) { return FORMAT_METHOD_POSITIONED; }

	virtual void            getContainingBlock    (WebRect* rect) = 0;
	virtual WEBC_BOOL       parentHeightIsAuto    (void) = 0;
 	virtual int             getTextDirection      (void) = 0;
 	virtual DISPLAY_INT	    getParentBorderLeft   (void) = 0;
 	virtual DISPLAY_INT	    getParentBorderTop    (void) = 0;
 	virtual DISPLAY_INT	    getParentBorderRight  (void) = 0;
 	virtual DISPLAY_INT	    getParentBorderBottom (void) = 0;
 	virtual DISPLAY_INT	    getAutoIndentLeft     (void) = 0;
	virtual DISPLAY_INT	    getAutoIndentTop      (void) = 0;
	virtual DISPLAY_INT	    getAutoIndentRight    (void) = 0;	
};

class PositionedFormatContext : public IPositionedFormatContext
{
 private:
	PresetWebRect    mContainingBlock; 
	WEBC_BOOL        mParentHeightIsAuto;
	int              mTextDirection;
	DISPLAY_INT      mParentBorderLeft;
	DISPLAY_INT      mParentBorderTop;
	DISPLAY_INT      mParentBorderRight;
	DISPLAY_INT      mParentBorderBottom;
	DISPLAY_INT      mAutoIndentLeft;
	DISPLAY_INT      mAutoIndentTop;
	DISPLAY_INT      mAutoIndentRight;

 public:
 	PositionedFormatContext (
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
	 	);
	 	
 	virtual ~PositionedFormatContext (void);
 
 	virtual WEBC_BOOL       equals  (FormatContext* other);
 	virtual void            recycle (void);

	virtual void            getContainingBlock    (WebRect* rect);
	virtual WEBC_BOOL       parentHeightIsAuto    (void);
 	virtual int             getTextDirection      (void);
 	virtual DISPLAY_INT	    getParentBorderLeft   (void);
 	virtual DISPLAY_INT	    getParentBorderTop    (void);
 	virtual DISPLAY_INT	    getParentBorderRight  (void);
 	virtual DISPLAY_INT	    getParentBorderBottom (void);
 	virtual DISPLAY_INT	    getAutoIndentLeft     (void);
	virtual DISPLAY_INT	    getAutoIndentTop      (void);
	virtual DISPLAY_INT	    getAutoIndentRight    (void);	
};

class AutoTopPositionedFormatContext : public IPositionedFormatContext
{
 protected:
 	DISPLAY_INT               mAutoIndentTop;
 	IPositionedFormatContext* mBaseContext;

 public:
 	AutoTopPositionedFormatContext (
 			IPositionedFormatContext* baseContext,
 			DISPLAY_INT autoIndentTop
 		);
 		
 	virtual ~AutoTopPositionedFormatContext (void);

 	virtual WEBC_BOOL       equals  (FormatContext* other);
 	virtual void            recycle (void);
 	
	virtual void            getContainingBlock    (WebRect* rect);
	virtual WEBC_BOOL       parentHeightIsAuto    (void);
 	virtual int             getTextDirection      (void);
 	virtual DISPLAY_INT	    getParentBorderLeft   (void);
 	virtual DISPLAY_INT	    getParentBorderTop    (void);
 	virtual DISPLAY_INT	    getParentBorderRight  (void);
 	virtual DISPLAY_INT	    getParentBorderBottom (void);
 	virtual DISPLAY_INT	    getAutoIndentLeft     (void);
	virtual DISPLAY_INT	    getAutoIndentTop      (void);
	virtual DISPLAY_INT	    getAutoIndentRight    (void);	
};

class FormatContextFactory
{
 public: 
 	FormatContextFactory (void);
 	virtual ~FormatContextFactory (void);
 
	IPositionedFormatContext* newPositioned (
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
		);
};

#endif /* __FORMATCTX_HPP__ */
