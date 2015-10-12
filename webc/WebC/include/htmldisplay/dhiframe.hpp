#ifndef __DHIFRAME_HPP__
#define __DHIFRAME_HPP__

#include "dhframe.hpp"

class HTMLInlineFrame;

class HTMLIFrameDisplay : public HTMLFrameDisplay
{	
 protected:
	virtual WEBC_BOOL SetElementStyle(void);
	void SetDimensions (void);

	// returns the formatted content height
	virtual DISPLAY_INT FormatContentPositioned (
			DISPLAY_INT widthValue, 
			DISPLAY_INT heightValue,
			WEBC_BOOL   heightIsAuto,
			DISPLAY_INT borderLeftValue,
			DISPLAY_INT borderTopValue,
			DISPLAY_INT borderRightValue,
			DISPLAY_INT borderBottomValue,
			DISPLAY_INT paddingLeftValue,
			DISPLAY_INT paddingTopValue,
			DISPLAY_INT paddingRightValue,
			DISPLAY_INT paddingBottomValue				
		);

 public:
	HTMLIFrameDisplay (HTMLInlineFrame *pElem);
	virtual ~HTMLIFrameDisplay ();

	// DisplayElement methods
	virtual DisplayElement *FocusDefault (void);

	// TextUnit methods
	virtual DISPLAY_INT GetTopMargin(void);
	virtual DISPLAY_INT GetLeftMargin(void);
	virtual DISPLAY_INT GetBottomMargin(void);
	virtual DISPLAY_INT GetRightMargin(void);
	virtual DISPLAY_INT GetMinWidth(void);
	virtual DISPLAY_INT GetMaxWidth(void);
	virtual void SetParentWidth(DISPLAY_INT iWidth, DISPLAY_INT iAvail);
	virtual void SetParentHeight(DISPLAY_INT iHeight);

	virtual WEBC_BOOL GetIntrinsicWidth (
			DISPLAY_INT* intrinsicWidth,
			DISPLAY_INT  height, 
			WEBC_BOOL    heightIsAuto
		);
};

#endif // __DHIFRAME_HPP__
