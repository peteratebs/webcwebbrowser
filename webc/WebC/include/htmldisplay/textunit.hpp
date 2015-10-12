#ifndef __TEXT_UNIT_INCLUDED__
#define __TEXT_UNIT_INCLUDED__

#include "listener.hpp"
#include "wgtypes.hpp"

enum TUFormatStatus
{
	TU_FORMAT_INVALID = 0,  // needs to be formatted
	TU_FORMAT_VALID,        // formatting up to date
	TU_FORMAT_MAYBE_VALID,  // a child's formatting is invalid; it may invalid our formatting
	TU_FORMAT_IN_PROGRESS   // format is in progress
};

class DisplayElement;
class WebRect;
class FormatContext;
class IPositionedFormatContext;
class FormatContextFactory;

// ***********************************************************************
/* class TextUnit is an interface to all of the graphical objects in the HTML document;
    these include blocks of text, images, tables, and various input widgets
*/

// Basic unit types (3 bits)
#define TU_TYPE_DEFAULT          0x0000   // Behaves like a normal box
#define TU_TYPE_TEXT             0x0001   // Same as default, but affects text-relative alignment
#define TU_TYPE_FRAMESET         0x0002   // Behaves like a frameset
#define TU_TYPE_INLINE           0x0003   // Transparent to format; "looks" like its children
#define TU_TYPE_NODISPLAY        0x0004   // Don't format, don't display
#define TU_TYPE_BLOCK            0x0005   // Block-level element; like a DIV or P
#define TU_TYPE_TABLE            0x0006   // Like default, but relative width ignores margins
#define TU_TYPE_BREAK            0x0007   // Line break - don't clear margins before adding to line
#define TU_TYPE_OPEN_NOBR        0x0008   // Open NOBR
#define TU_TYPE_CLOSE_NOBR       0x0009   // Close NOBR
#define TU_TYPE_INLINE_BLOCK	 0x000A   // Block-level element; like a DIV or P but with no breaks

// Explicit line break (2 bits)
#define TU_BREAK_NONE            0x0000   // No explicit line break associated with this element
#define TU_BREAK_AFTER           0x0001   // Line should break after this element (BR)
#define TU_BREAK_BEFORE          0x0002   // Line should break before this element (CR)
#define TU_BREAK_BOTH            0x0003   // Line should break both before and after this element

// Aligned unit clearing (2 bits)
#define TU_CLEAR_NONE            0x0000   // No clear
#define TU_CLEAR_LEFT            0x0001   // Place element below lowest left-aligned unit
#define TU_CLEAR_RIGHT           0x0002   // Place element below lowest right-aligned unit
#define TU_CLEAR_ALL             0x0003   // Place element below the bottoms of all aligned units

// Implicit line break (1 bit)
#define TU_WRAP_NONE             0x0000   // Don't allow implicit line break before this element
#define TU_WRAP_NORMAL           0x0001   // Allow implicit line breaks where whitespace occurs

// Format direction (1 bit)
#define TU_DIR_LTR               0x0000   // Left to right direction; like English text
#define TU_DIR_RTL               0x0001   // Right to Left direction; like Hebrew text

// Visibility (2 bits)
#define TU_VISIBILITY_SHOW       0x0000   // Format and display as normal
#define TU_VISIBILITY_HIDE       0x0001   // Format but don't display
#define TU_VISIBILITY_INHERIT    0x0002   // Inherit visibility from parent

// Horizontal Alignment (2 bits)
#define TU_ALIGN_NONE            0x00
#define TU_ALIGN_LEFT            0x01
#define TU_ALIGN_CENTER          0x02
#define TU_ALIGN_RIGHT           0x03

// Vertical Alignment (3 bits)
#define TU_VALIGN_BASELINE       0x00
#define TU_VALIGN_BOTTOM         0x01
#define TU_VALIGN_MIDDLE         0x02
#define TU_VALIGN_SUB            0x03
#define TU_VALIGN_SUPER          0x04
#define TU_VALIGN_TEXTBOTTOM     0x05
#define TU_VALIGN_TEXTTOP        0x06
#define TU_VALIGN_TOP            0x07
#define TU_VALIGN_CENTER         TU_VALIGN_MIDDLE

// Float (2 bits)
#define TU_FLOAT_NONE            0x00
#define TU_FLOAT_LEFT            0x01
#define TU_FLOAT_RIGHT           0x02

// Position (2 bits)
#define TU_POSITION_STATIC       0x00
#define TU_POSITION_ABSOLUTE     0x01
#define TU_POSITION_FIXED        0x02
#define TU_POSITION_RELATIVE     0x03

#define TU_NULL_ID -1

class MarginContext;

// Abstract base class... defines interface to all TextUnit implementations
class TextUnit : public Notifier, public Listener
{
public:
	virtual ~TextUnit() { }
	
	// External Property methods
	virtual WEBC_UINT16 GetUnitType(void) = 0;
	virtual WEBC_UINT16 GetBreak(void) = 0;
	virtual WEBC_UINT16 GetClear(void) = 0;
	virtual WEBC_UINT16 GetDir(void) = 0;
	virtual WEBC_UINT16 GetWrap(void) = 0;
	virtual WEBC_UINT16 GetChildWrap(void) { return TU_WRAP_NORMAL; }
	virtual WEBC_UINT16 GetVisibility(void) = 0;
	virtual WEBC_UINT16 GetPosition(void) = 0;
	virtual WEBC_UINT8  GetFloat(void) = 0;
	virtual WEBC_UINT8  GetTextAlign(void) = 0;
	virtual WEBC_UINT8  GetVAlign(void) = 0;
	virtual DISPLAY_INT GetLeft(void) = 0;
	virtual DISPLAY_INT GetTop(void) = 0;	
	virtual DISPLAY_INT GetMinWidth(void) = 0;
	virtual DISPLAY_INT GetMaxWidth(void) = 0;
	virtual DISPLAY_INT GetPixelWidth(void) = 0;
	virtual DISPLAY_INT GetPixelHeight(void) = 0;
	virtual DISPLAY_INT GetTopMargin(void) = 0;
	virtual DISPLAY_INT GetLeftMargin(void) = 0;
	virtual DISPLAY_INT GetBottomMargin(void) = 0;
	virtual DISPLAY_INT GetRightMargin(void) = 0;	
	virtual DISPLAY_INT GetBaseline(void) = 0;
	virtual DISPLAY_INT GetParagraphSpace(void) = 0;
  
	virtual WEBC_BOOL BreakAtOrBefore(DISPLAY_INT iPosition, TextUnit **firstHalf, TextUnit **secondHalf) { return WEBC_FALSE; }
	virtual WEBC_BOOL BreakAtFirstPoint(TextUnit **firstHalf, TextUnit **secondHalf)                      { return WEBC_FALSE; }
	virtual WEBC_BOOL BreakAtLastPoint(TextUnit **firstHalf, TextUnit **secondHalf)                       { return WEBC_FALSE; }
	virtual WEBC_BOOL BreakAllowedBefore(void) { return WEBC_TRUE; }
	virtual WEBC_BOOL BreakAllowedAfter(void)  { return WEBC_TRUE; }
	virtual WEBC_BOOL BreakAllowedInside(void) { return WEBC_FALSE; }
	virtual WEBC_BOOL DisplayNoneIsTrue(void)  { return WEBC_FALSE; }
	virtual void BreakAfter(void) {}
	virtual void BreakBefore(void) {}
	virtual DISPLAY_INT MinBreakWidth (void) { return GetMinWidth(); }
	
	// Action methods
	virtual void SetParentWidth(DISPLAY_INT iWidth, DISPLAY_INT iAvail) = 0;
	virtual void SetParentHeight(DISPLAY_INT iHeight) = 0;
	virtual void SetXYPosition(DISPLAY_INT iXPos, DISPLAY_INT iYPos) = 0;
	virtual void FormatBlock(MarginContext *pMargin) {};
	virtual void Release(void) = 0;
	virtual void SetParentDisplay(DisplayElement* parent, DisplayElement* insertBefore) = 0;
	virtual void SetDisplayContext(DisplayElement *parent) = 0;
	
	// returns the format context for children of this element,
	//  built from the context factory passed in (if any)
	virtual IPositionedFormatContext* FormatPositioned (
			DisplayElement* displayParent,
			IPositionedFormatContext* rootContext,
			IPositionedFormatContext* parentContext,
			FormatContextFactory* childContextFactory
		) { return 0; }

	virtual WEBC_BOOL NeedsAutoIndentTop (WEBC_BOOL parentHeightIsAuto) { return WEBC_FALSE; }	
		
	virtual DISPLAY_INT GetMaxWidthSmallerThan (DISPLAY_INT value)
		{ return GetMaxWidth(); }
		
	virtual WEBC_BOOL GetIntrinsicWidth (
			DISPLAY_INT* intrinsicWidth,
			DISPLAY_INT  height, 
			WEBC_BOOL    heightIsAuto
		) { return WEBC_FALSE; }
	
	// Listener interface
	virtual void Notify (Notifier *who, NotifyEvent *what) {}	
};


// Abstract class to enumerate through a list of TextUnits; classes that implement this interface
//   correspond to different implementations of TextUnit lists (i.e. linked list, array, binary tree, etc.)
class TextUnitEnumerator 
{
public:
	virtual ~TextUnitEnumerator() { }
	virtual TextUnit *GetNextUnit(void) = 0;
};

#endif
