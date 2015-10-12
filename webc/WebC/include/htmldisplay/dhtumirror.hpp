#ifndef __DHTUMIRROR_HPP__
#define __DHTUMIRROR_HPP__

#include "textunit.hpp"
#include "delement.hpp"


/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/

#define TUM_VALID_ZINDEX         0x01

#define TUM_TYPE_VALID           0x00000001L
#define TUM_BREAK_VALID          0x00000002L
#define TUM_CLEAR_VALID          0x00000004L
#define TUM_DIR_VALID            0x00000008L
#define TUM_WRAP_VALID           0x00000010L
#define TUM_VISIBILITY_VALID     0x00000020L
#define TUM_POSITION_VALID       0x00000040L
#define TUM_FLOAT_VALID          0x00000080L
#define TUM_TEXT_ALIGN_VALID     0x00000100L
#define TUM_VALIGN_VALID         0x00000200L
#define TUM_ANY_VALID            0x000003ffL

#if (1)

#define TUM_TYPE_MASK            (0xfL << 10)
#define TUM_GET_TYPE(X)          ((X & TUM_TYPE_MASK) >> 10)
#define TUM_SET_TYPE(X,V)        X &= ~(TUM_TYPE_MASK); X |= ((V & 0xf) << 10);

#define TUM_BREAK_MASK           (0x3L << 14)
#define TUM_GET_BREAK(X)         ((X & TUM_BREAK_MASK) >> 14)
#define TUM_SET_BREAK(X,V)       X &= ~(TUM_BREAK_MASK); X |= ((V & 0x3) << 14);

#define TUM_CLEAR_MASK           (0x3L << 16)
#define TUM_GET_CLEAR(X)         ((X & TUM_CLEAR_MASK) >> 16)
#define TUM_SET_CLEAR(X,V)       X &= ~(TUM_CLEAR_MASK); X |= ((V & 0x3) << 16);

#define TUM_DIR_MASK             (0x1L << 18)
#define TUM_GET_DIR(X)           ((X & TUM_DIR_MASK) >> 18)
#define TUM_SET_DIR(X,V)         X &= ~(TUM_DIR_MASK); X |= ((V & 0x1) << 18);

#define TUM_WRAP_MASK            (0x1L << 19)
#define TUM_GET_WRAP(X)          ((X & TUM_WRAP_MASK) >> 19)
#define TUM_SET_WRAP(X,V)        X &= ~(TUM_WRAP_MASK); X |= ((V & 0x1) << 19);

#define TUM_VISIBILITY_MASK      (0x3L << 20)
#define TUM_GET_VISIBILITY(X)    ((X & TUM_VISIBILITY_MASK) >> 20)
#define TUM_SET_VISIBILITY(X,V)  X &= ~(TUM_VISIBILITY_MASK); X |= ((V & 0x3) << 20);

#define TUM_POSITION_MASK        (0x3L << 22)
#define TUM_GET_POSITION(X)      ((X & TUM_POSITION_MASK) >> 22)
#define TUM_SET_POSITION(X,V)    X &= ~(TUM_POSITION_MASK); X |= ((V & 0x3) << 22);

#define TUM_FLOAT_MASK           (0x3L << 24)
#define TUM_GET_FLOAT(X)         ((X & TUM_FLOAT_MASK) >> 24)
#define TUM_SET_FLOAT(X,V)       X &= ~(TUM_FLOAT_MASK); X |= ((V & 0x3) << 24);

#define TUM_TEXT_ALIGN_MASK      (0x3L << 26)
#define TUM_GET_TEXT_ALIGN(X)    ((X & TUM_TEXT_ALIGN_MASK) >> 26)
#define TUM_SET_TEXT_ALIGN(X,V)  X &= ~(TUM_TEXT_ALIGN_MASK); X |= ((V & 0x3) << 26);

#define TUM_VALIGN_MASK          (0x7L << 28)
#define TUM_GET_VALIGN(X)        ((X & TUM_VALIGN_MASK) >> 28)
#define TUM_SET_VALIGN(X,V)      X &= ~(TUM_VALIGN_MASK); X |= ((V & 0x7) << 28);
#else
#define TUM_TYPE_MASK            (0x7L << 10)
#define TUM_GET_TYPE(X)          ((X & TUM_TYPE_MASK) >> 10)
#define TUM_SET_TYPE(X,V)        X &= ~(TUM_TYPE_MASK); X |= ((V & 0x7) << 10);

#define TUM_BREAK_MASK           (0x3L << 13)
#define TUM_GET_BREAK(X)         ((X & TUM_BREAK_MASK) >> 13)
#define TUM_SET_BREAK(X,V)       X &= ~(TUM_BREAK_MASK); X |= ((V & 0x3) << 13);

#define TUM_CLEAR_MASK           (0x3L << 15)
#define TUM_GET_CLEAR(X)         ((X & TUM_CLEAR_MASK) >> 15)
#define TUM_SET_CLEAR(X,V)       X &= ~(TUM_CLEAR_MASK); X |= ((V & 0x3) << 15);

#define TUM_DIR_MASK             (0x1L << 17)
#define TUM_GET_DIR(X)           ((X & TUM_DIR_MASK) >> 17)
#define TUM_SET_DIR(X,V)         X &= ~(TUM_DIR_MASK); X |= ((V & 0x1) << 17);

#define TUM_WRAP_MASK            (0x1L << 18)
#define TUM_GET_WRAP(X)          ((X & TUM_WRAP_MASK) >> 18)
#define TUM_SET_WRAP(X,V)        X &= ~(TUM_WRAP_MASK); X |= ((V & 0x1) << 18);

#define TUM_VISIBILITY_MASK      (0x3L << 19)
#define TUM_GET_VISIBILITY(X)    ((X & TUM_VISIBILITY_MASK) >> 19)
#define TUM_SET_VISIBILITY(X,V)  X &= ~(TUM_VISIBILITY_MASK); X |= ((V & 0x3) << 19);

#define TUM_POSITION_MASK        (0x3L << 21)
#define TUM_GET_POSITION(X)      ((X & TUM_POSITION_MASK) >> 21)
#define TUM_SET_POSITION(X,V)    X &= ~(TUM_POSITION_MASK); X |= ((V & 0x3) << 21);

#define TUM_FLOAT_MASK           (0x3L << 23)
#define TUM_GET_FLOAT(X)         ((X & TUM_FLOAT_MASK) >> 23)
#define TUM_SET_FLOAT(X,V)       X &= ~(TUM_FLOAT_MASK); X |= ((V & 0x3) << 23);

#define TUM_TEXT_ALIGN_MASK      (0x3L << 25)
#define TUM_GET_TEXT_ALIGN(X)    ((X & TUM_TEXT_ALIGN_MASK) >> 25)
#define TUM_SET_TEXT_ALIGN(X,V)  X &= ~(TUM_TEXT_ALIGN_MASK); X |= ((V & 0x3) << 25);

#define TUM_VALIGN_MASK          (0x7L << 27)
#define TUM_GET_VALIGN(X)        ((X & TUM_VALIGN_MASK) >> 27)
#define TUM_SET_VALIGN(X,V)      X &= ~(TUM_VALIGN_MASK); X |= ((V & 0x7) << 27);

#endif

/*****************************************************************************/
// HTMLTextUnitMirror
/*****************************************************************************/

class HTMLElement;

class HTMLTextUnitMirror : public TextUnit
{
public:
	HTMLTextUnitMirror (HTMLElement *pElem);
	virtual ~HTMLTextUnitMirror ();

	// TextUnit interface
	virtual WEBC_UINT16 GetBreak(void);
	virtual WEBC_UINT16 GetClear(void);
	virtual WEBC_UINT16 GetDir(void);
	virtual WEBC_UINT8  GetFloat(void);
	virtual WEBC_UINT16 GetPosition(void);
	virtual WEBC_UINT8  GetTextAlign(void);
	virtual WEBC_UINT16 GetUnitType(void);
	virtual WEBC_UINT16 GetVisibility(void);
	virtual WEBC_UINT8  GetVAlign(void);
	virtual WEBC_UINT16 GetWrap(void);

	virtual DISPLAY_INT GetBaseline(void);
	virtual DISPLAY_INT GetBottomMargin(void);
	virtual DISPLAY_INT GetLeft(void);
	virtual DISPLAY_INT GetLeftMargin(void);
	virtual DISPLAY_INT GetPixelHeight(void);
	virtual DISPLAY_INT GetMaxWidth(void);
	virtual DISPLAY_INT GetMinWidth(void);
	virtual DISPLAY_INT GetParagraphSpace(void);
	virtual DISPLAY_INT GetRightMargin(void);
	virtual DISPLAY_INT GetTop(void);
	virtual DISPLAY_INT GetTopMargin(void);
	virtual DISPLAY_INT GetPixelWidth(void);

	virtual void GetXYPosition(DISPLAY_INT *piXPos, DISPLAY_INT *piYPos);
	virtual void SetParentDisplay(DisplayElement *parent, DisplayElement* insertBefore);
	virtual void SetParentHeight(DISPLAY_INT iHeight);
	virtual void SetParentWidth(DISPLAY_INT iWidth, DISPLAY_INT iAvail);
	virtual void SetXYPosition(DISPLAY_INT iXPos, DISPLAY_INT iYPos);
	virtual void Release(void);

	virtual WEBC_INT32 FindDisplayZIndex (void);

	virtual WEBC_BOOL InvalidateTUMirror(void);
	virtual WEBC_BOOL InvalidateZIndex(void);
	virtual WEBC_BOOL InvalidateVisibility(void);

	virtual DISPLAY_INT GetParentWidth(void);

	DisplayElement *GetHTMLParentDisplay (void);

protected:  
	HTMLElement* mpHtmlElement;
	long         miTUMirrorZIndex;
	WEBC_UINT8   mTUMirrorValid;
	WEBC_UINT32  mData;

	DISPLAY_INT GetPixelLength (
			CSSPropertyType property, 
			DISPLAY_INT relativeTo,
			DISPLAY_INT emHeight,
			DISPLAY_INT exHeight,
			DISPLAY_INT defaultValue
		);

	DISPLAY_INT GetPixelLength (
			CSSLength* length, 
			DISPLAY_INT relativeTo,
			DISPLAY_INT emHeight,
			DISPLAY_INT exHeight,
			DISPLAY_INT defaultValue
		);
		
	// sets the length type to auto if property can not be retrieved
	void GetCSSLength (
			CSSLength* length,
			CSSPropertyType property
		);
	
	DISPLAY_INT GetBorderWidth (
			CSSSide side,
			DISPLAY_INT relativeTo,
			DISPLAY_INT emHeight,
			DISPLAY_INT exHeight,
			DISPLAY_INT parentBorderWidth
		);
		
	WEBC_BOOL InvalidateUnitType (void);
	WEBC_BOOL InvalidatePosition (void);
};

#endif // __DHTUMIRROR_HPP__
