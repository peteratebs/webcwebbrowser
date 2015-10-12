#ifndef __WEBRECT_H__
#define __WEBRECT_H__

#include "wgtypes.hpp"
#include "util.h"
#include "webcinline.h"

#define WEBRECT_INLINE WEBC_INLINE

// has no constructor to speed up stack instance
class WebRect
{
 public:
	DISPLAY_INT left, top, right, bottom;

	WEBRECT_INLINE WEBC_BOOL Empty (void)
	{
		return ((right < left) || (bottom < top));		
	}

	WEBRECT_INLINE WEBC_BOOL Equals(const WebRect * r)
	{
		return ((left == r->left)   &&
		        (right == r->right) &&
		        (top == r->top)     &&
		        (bottom == r->bottom));
		
	}

	WEBRECT_INLINE WEBC_BOOL Contains(const WebRect * r)
	{
		return ((r->top >= top)   && (r->top < bottom)     &&
		        (r->bottom > top) && (r->bottom <= bottom) &&
		        (r->left >= left) && (r->left < right)     &&
		        (r->right > left) && (r->right <= right));
	}
	
	WEBRECT_INLINE WEBC_BOOL Contains(DISPLAY_INT x, DISPLAY_INT y)
	{
		return ((y >= top) && (y <= bottom) && (x >= left) && (x <= right));
	}
	
	WEBRECT_INLINE WEBC_BOOL Overlaps(const WebRect * r)
	{
		// test vertical overlap first because it is more likely to fail when clipping
		//  web pages (which are typically much taller than they are wide
		return ((top <= r->bottom) && (bottom >= r->top) && (left <= r->right) && (right >= r->left));
	}
	
	WEBRECT_INLINE void Or(const WebRect * r)
	{
		if (r->top < top)
		{
			top = r->top;
		}
		if (r->left < left)
		{
			left = r->left;
		}
		if (r->right > right)
		{
			right = r->right;
		}
		if (r->bottom > bottom)
		{
			bottom = r->bottom;
		}
	}
	
	WEBRECT_INLINE void And(const WebRect * r)
	{
		if (r->top > top)
		{
			top = r->top;
		}
		if (r->left > left)
		{
			left = r->left;
		}
		if (r->right < right)
		{
			right = r->right;
		}
		if (r->bottom < bottom)
		{
			bottom = r->bottom;
		}
	}
	
	WEBRECT_INLINE void MoveTo(DISPLAY_INT x, DISPLAY_INT y)
	{
		right += x - left;
		left = x;
		bottom += y - top;
		top = y;
	}
	
	WEBRECT_INLINE void SizeTo(DISPLAY_INT w, DISPLAY_INT h)
	{
		right = left + w - 1;
		bottom = top + h - 1;
	}

	WEBRECT_INLINE void SetWidth(DISPLAY_INT w)
	{
		right = left + w - 1;
	}

	WEBRECT_INLINE void SetHeight(DISPLAY_INT h)
	{
		bottom = top + h - 1;
	}
		
	WEBRECT_INLINE void Shift(DISPLAY_INT x, DISPLAY_INT y)
	{
		top += y;
		bottom += y;
		left += x;
		right += x;
	}

	WEBRECT_INLINE void Expand(DISPLAY_INT topSize, DISPLAY_INT bottomSize, DISPLAY_INT leftSize, DISPLAY_INT rightSize)
	{
		top -= topSize;
		if(top < 0)
			top = 0;
		bottom += bottomSize;
		left -= leftSize;
		if(left < 0)
			left = 0;
		right += rightSize;
	}

	WEBRECT_INLINE void ShiftOver(DISPLAY_INT x)
	{
		left += x;
		right += x;
	}

	WEBRECT_INLINE void ShiftDown(DISPLAY_INT y)
	{
		top += y;
		bottom += y;
	}
	
	WEBRECT_INLINE void Set(DISPLAY_INT l, DISPLAY_INT t, DISPLAY_INT r, DISPLAY_INT b)
	{
		top = t;
		left = l;
		right = r;
		bottom = b;
	}
	
	WEBRECT_INLINE void Set(const WebRect *Rect)
	{
		Set(Rect->left, Rect->top, Rect->right, Rect->bottom);
	}
	
	WEBRECT_INLINE void Set(const WebRect &Rect)
	{
		Set(Rect.left, Rect.top, Rect.right, Rect.bottom);
	}
	
	WEBRECT_INLINE DISPLAY_INT Width(void)
	{
		return right - left + 1;
	}
	
	WEBRECT_INLINE DISPLAY_INT Height(void)
	{
		return bottom - top + 1;
	}
		
	WEBRECT_INLINE void SizeBy(DISPLAY_INT x, DISPLAY_INT y)
	{
		right += x;
		bottom += y;
	}
	
	WEBRECT_INLINE DISPLAY_INT Area(void)
	{
		return (Width() * Height());
	}
	
	WEBRECT_INLINE void SetSizeAndPosition (DISPLAY_INT x, DISPLAY_INT y, DISPLAY_INT w, DISPLAY_INT h)
	{
		left = x;
		top = y;
		right = left + w - 1;
		bottom = top + h - 1;
	}

	unsigned char Compare(WebRect *pOther);
	
	// Calculate the smallest set of non-overlapping rects equivalent to
	//  the region (this + r1); output 1-3 rects: this, r1, r2, return num output rects
	int Merge(WebRect *r1, WebRect *r2);
	int MergeCase(WebRect *r1, WebRect *r2, unsigned char compareCase);

	// Calculate the smallest set of non-overlapping rects equivalent to
	//  the region (this - r1); output 0-3 rects: this, r1, r2, return num output rects
	int Split(WebRect *r1, WebRect *r2);
	int SplitCase(WebRect *r1, WebRect *r2, unsigned char compareCase);
};

class PresetWebRect : public WebRect
{
 public:
	PresetWebRect();
	PresetWebRect(const WebRect * r);
	PresetWebRect(const WebRect & r);
	PresetWebRect(DISPLAY_INT l, DISPLAY_INT t, DISPLAY_INT r, DISPLAY_INT b);
};

#endif // __WEBRECT_H__
