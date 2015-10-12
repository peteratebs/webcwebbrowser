#ifndef __HCBOX_HPP__
#define __HCBOX_HPP__

#include "webgraph.hpp"
#include "css.h"

class HTMLElement;

typedef WEBC_UINT16 HCBOXFLAGS;

#define HCBOX_LEFT    0
#define HCBOX_TOP     1
#define HCBOX_RIGHT   2
#define HCBOX_BOTTOM  3

#define HCBOX_FLAG_LEFT_COLOR_SET      0x0001
#define HCBOX_FLAG_TOP_COLOR_SET       0x0002
#define HCBOX_FLAG_RIGHT_COLOR_SET     0x0004
#define HCBOX_FLAG_BOTTOM_COLOR_SET    0x0008
#define HCBOX_FLAG_LEFT_WIDTH_SET      0x0010
#define HCBOX_FLAG_TOP_WIDTH_SET       0x0020
#define HCBOX_FLAG_RIGHT_WIDTH_SET     0x0040
#define HCBOX_FLAG_BOTTOM_WIDTH_SET    0x0080
#define HCBOX_FLAG_EM_HEIGHT_SET       0x0100
#define HCBOX_FLAG_EX_HEIGHT_SET       0x0200

#define HCBOX_FLAG_LEFT_STYLE_SET      0x1000
#define HCBOX_FLAG_TOP_STYLE_SET       0x2000
#define HCBOX_FLAG_RIGHT_STYLE_SET     0x4000
#define HCBOX_FLAG_BOTTOM_STYLE_SET    0x8000

#define HCBOX_HAS_PERCENT              0x8000

#define HCBOX_STYLE_NONE               0
#define HCBOX_STYLE_SOLID              1
#define HCBOX_STYLE_OUTSET             2
#define HCBOX_STYLE_INSET              3
#define HCBOX_STYLE_GROOVE             4
#define HCBOX_STYLE_RIDGE              5

#define HCBOX_STYLE_MASK               0x0007
#define HCBOX_STYLE_LEFT_SHIFT         0
#define HCBOX_STYLE_TOP_SHIFT          3
#define HCBOX_STYLE_RIGHT_SHIFT        6
#define HCBOX_STYLE_BOTTOM_SHIFT       9
#define HCBOX_STYLE_LEFT_MASK          (HCBOX_STYLE_MASK << HCBOX_STYLE_LEFT_SHIFT)
#define HCBOX_STYLE_TOP_MASK           (HCBOX_STYLE_MASK << HCBOX_STYLE_TOP_SHIFT)
#define HCBOX_STYLE_RIGHT_MASK         (HCBOX_STYLE_MASK << HCBOX_STYLE_RIGHT_SHIFT)
#define HCBOX_STYLE_BOTTOM_MASK        (HCBOX_STYLE_MASK << HCBOX_STYLE_BOTTOM_SHIFT)

class HTMLContentBox
{
protected:
	HCBOXFLAGS   mFlags;
	HTMLElement* mpElement;
	DISPLAY_INT  miParentWidth;
	DISPLAY_INT  miEmHeight;
	DISPLAY_INT  miExHeight;
	WEBC_UINT8   mBorderWidth[4];
	HTMLColor    mBorderColor[4];
	WEBC_UINT16  mBorderStyle;
	
	DISPLAY_INT ConvertCSSLength(CSSLength *len);

public:
	HTMLContentBox (HTMLElement *pElem, DISPLAY_INT parentWidth=0);
	~HTMLContentBox ();
	
	HTMLColor      GetBorderColor   (CSSSide side);
	WEBC_UINT16    GetBorderStyle   (CSSSide side);
	DISPLAY_INT    GetBorderWidth   (CSSSide side);
	DISPLAY_INT    GetEmHeight      (void);
	DISPLAY_INT    GetExHeight      (void);
	DISPLAY_INT    GetHorizontalPad (void);
	DISPLAY_INT    GetMarginWidth   (CSSSide side);
	DISPLAY_INT    GetPaddingWidth  (CSSSide side);
	DISPLAY_INT    GetParentWidth   (void);
	DISPLAY_INT    GetTotalPad      (CSSSide side);
	DISPLAY_INT    GetVerticalPad   (void);
	int            HasPercent       (void);

	void GetHorizontalMargins (
			DISPLAY_INT* marginLeftValue, 
			DISPLAY_INT* marginRightValue, 
			DISPLAY_INT totalWidthValue
		);

	void           DrawBorder       (WebRect *box, WebGraphics *gc);
	WEBC_BOOL      ResetProperties  (void);
	void           SetElement       (HTMLElement *pElement);
	void           SetParentWidth   (DISPLAY_INT w);
};

#endif // __HCBOX_HPP__
