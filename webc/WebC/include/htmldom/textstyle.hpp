/*
|  TEXTSTYLE.HPP - 
| 
|  EBS - 
| 
| 
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/ 

#ifndef __TEXTSTYLE_HPP__
#define __TEXTSTYLE_HPP__

#include "webctypes.h"
#include "css.h"
#include "webgraph.hpp"
#include "html.h"
#include "webfonts.hpp"
#include "refcount.hpp"

class HTMLDocument;

class ITextDisplayStyle : public ReferenceCounted
{
 public:
 	ITextDisplayStyle (void);
 	~ITextDisplayStyle (void);
 
 	virtual WEBC_BOOL         equals             (ITextDisplayStyle* other);
 	virtual WebColor          getBackgroundColor (void) = 0;
 	virtual WEBC_BOOL         hasBackgroundColor (void) = 0;
 	virtual WebColor          getColor           (void) = 0;
 	virtual CSSTextDecoration getDecoration      (void) = 0;
 	virtual WebFont           getFont            (void) = 0;
 	virtual CSSBorderStyle    getOutlineStyle    (void) = 0; 	

	virtual void renderText (DISPLAY_INT x, DISPLAY_INT y, WebChar* text, WEBC_SIZE textLen, WebGraphics* gc) = 0;
 	virtual void renderText (WebRect* where, WebChar* text, WEBC_SIZE textLen, WebGraphics* gc) = 0;
};

class TextDisplayStyle : public ITextDisplayStyle
{
 private:
	WebColor          mBackgroundColor;
	WebColor          mColor;
	CSSTextDecoration mDecoration;
	WebFontRef        mFont;
	WEBC_BOOL         mHasBackgroundColor;
	CSSBorderStyle    mOutlineStyle;
	
 public:
 	TextDisplayStyle (
			WebGraphFont* font,
			WebColor color,
			WebColor backgroundColor,
			WEBC_BOOL hasBackgroundColor,
			CSSTextDecoration decoration,
			CSSBorderStyle outlineStyle
		);
		
 	~TextDisplayStyle (void);
 
 	virtual WebColor          getBackgroundColor (void);
 	virtual WebColor          getColor           (void);
 	virtual CSSTextDecoration getDecoration      (void);
 	virtual WebFont           getFont            (void);
 	virtual CSSBorderStyle    getOutlineStyle    (void); 	
 	virtual WEBC_BOOL         hasBackgroundColor (void);
 	virtual void              recycle            (void);
 	
	virtual void renderText (DISPLAY_INT x, DISPLAY_INT y, WebChar* text, WEBC_SIZE textLen, WebGraphics* gc);
 	virtual void renderText (WebRect* where, WebChar* text, WEBC_SIZE textLen, WebGraphics* gc);
};

class ITextDisplayStyleFactory
{
 public:
 	virtual ITextDisplayStyle* newTextStyle (
			WebGraphFont* font,
			HTMLColor color,
			HTMLColor backgroundColor,
			CSSTextDecoration decoration,
			CSSBorderStyle outlineStyle
		) = 0;
};

class NewTextDisplayStyleFactory
{
 protected:
 	WebGraphics* mGC;
 	
 public:
 	NewTextDisplayStyleFactory (WebGraphics* gc);
 	~NewTextDisplayStyleFactory (void);
 
 	virtual ITextDisplayStyle* newTextStyle (
			WebGraphFont* font,
			HTMLColor color,
			HTMLColor backgroundColor,
			CSSTextDecoration decoration,
			CSSBorderStyle outlineStyle
		);
};

class HTMLDocumentTextDisplayStyleFactory : public NewTextDisplayStyleFactory
{
 public:
 	HTMLDocumentTextDisplayStyleFactory (HTMLDocument* doc);
 	~HTMLDocumentTextDisplayStyleFactory (void);	
};

#endif /* __TEXTSTYLE_HPP__ */
