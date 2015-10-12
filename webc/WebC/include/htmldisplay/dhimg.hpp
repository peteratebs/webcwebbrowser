#ifndef __DHIMG_HPP__
#define __DHIMG_HPP__

#include "dhelem.hpp"
#include "delement.hpp"
#include "hcbox.hpp"

#ifdef _EFI_
#undef Pause
#endif

class HTMLImage;

typedef enum e_HTMLImageDisplayFlags
{
	DHIMG_FLAG_HAS_BGCOLOR        = 0x0001,
	DHIMG_FLAG_PARENT_HEIGHT_AUTO = 0x0002
}
HTMLImageDisplayFlags;

class HTMLImageDisplay : public HTMLElementDisplay<DisplayElement>
{
protected:
	virtual WEBC_BOOL SetElementStyle (void);

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

	void SetImageDimensions (void);
	void SetImage (WebcImage *image);

	WEBC_UINT16     mwTimerId;
	CSSBorderStyle  mOutlineStyle;
	WebColor        mBackgroundColor;
	WEBC_UINT16     mImageDisplayFlags;
	WebcImage*      mpImage;
	WebcImageFrame* mpCurrentFrame;
	DISPLAY_INT     miParentWidth;
	DISPLAY_INT     miParentHeight;
	HTMLContentBox     mBorder;

public:
	HTMLImageDisplay (HTMLImage *pElem);
	virtual ~HTMLImageDisplay (void);

	// Listener methods
	virtual void Notify (Notifier *who, NotifyEvent *what);

	// DisplayElement methods
	virtual void Move (WebRect * pRect);
	virtual void SizeTo (DISPLAY_INT w, DISPLAY_INT h);
	virtual void SetHeight (DISPLAY_INT h);
	virtual void SetWidth (DISPLAY_INT w);
	virtual void Draw (DISPLAY_INT x, DISPLAY_INT y, WebRect *pViewport, WebGraphics *pGC);
	virtual HTMLEventStatus Event (HTMLEvent *e);
	virtual DisplayElement *TrapEventPoint(DISPLAY_INT x, DISPLAY_INT y, DISPLAY_INT viewX, DISPLAY_INT viewY);
	virtual DISPLAYElementType Type (void) { return DISPLAY_ELEMENT_IMAGEDISPLAY; }

	// TextUnit methods
	virtual DISPLAY_INT GetMinWidth(void);
	virtual DISPLAY_INT GetMaxWidth(void);
	virtual DISPLAY_INT GetPixelWidth(void);
	virtual DISPLAY_INT GetPixelHeight(void);
	virtual DISPLAY_INT GetBaseline(void);
	virtual void SetParentWidth(DISPLAY_INT iWidth, DISPLAY_INT iAvail);
	virtual void SetParentHeight(DISPLAY_INT parentHeight);
	virtual void SetParentDisplay(DisplayElement *parent, DisplayElement* insertBefore);
	virtual void SetXYPosition(DISPLAY_INT iXPos, DISPLAY_INT iYPos);

	// returns the formatted content height

	virtual WEBC_BOOL GetIntrinsicWidth (
			DISPLAY_INT* intrinsicWidth,
			DISPLAY_INT  height,
			WEBC_BOOL    heightIsAuto
		);

	// HTMLImageDisplay specific methods
	WebColor GetBackgroundColor (void);
	WEBC_BOOL HasBackgroundColor (void);
	CSSBorderStyle GetOutlineStyle (void);
	void StepFrame (void);

	// These three methods are for animated images
	void Play  (long msecError = 0);
	void Pause (void);
	void Stop  (void);
};

#endif // __DHIMG_HPP__
