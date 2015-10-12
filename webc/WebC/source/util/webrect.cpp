//
// WEBRECT.CPP - Methods for class WebRect
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//

/*****************************************************************************/
// Header Files
/*****************************************************************************/

#include "webrect.hpp"
#include "util.h"


/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/


/*****************************************************************************/
// Local Types
/*****************************************************************************/

typedef int (*CaseOpFn) (WebRect *rect0, WebRect *rect1, WebRect *rect2);

enum WebRectCompareCase 
{
	CLIP_NORTHWEST = 0x0,
	CLIP_WEST      = 0x1,
	CLIP_NORTH     = 0x2,
	SURROUND       = 0x3,
	EAST_CLIPPED   = 0x4,
	CLIP_SOUTHWEST = 0x5,
	HCROSS         = 0x6,
	CLIP_SOUTH     = 0x7,
	SOUTH_CLIPPED  = 0x8,
	VCROSS         = 0x9,
	CLIP_NORTHEAST = 0xa,
	CLIP_EAST      = 0xb,
	SURROUNDED     = 0xc,
	NORTH_CLIPPED  = 0xd,
	WEST_CLIPPED   = 0xe,
	CLIP_SOUTHEAST = 0xf
};


/*****************************************************************************/
// Local function prototypes
/*****************************************************************************/

static int _ClipNorthWestMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _ClipWestMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _ClipNorthMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _SurroundMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _EastClippedMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _ClipSouthWestMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _HCrossMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _ClipSouthMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _SouthClippedMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _VCrossMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _ClipNorthEastMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _ClipEastMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _SurroundedMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _NorthClippedMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _WestClippedMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _ClipSouthEastMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);

static int _ClipNorthWestSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _ClipWestSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _ClipNorthSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _SurroundSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _EastClippedSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _ClipSouthWestSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _HCrossSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _ClipSouthSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _SouthClippedSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _VCrossSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _ClipNorthEastSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _ClipEastSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _SurroundedSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _NorthClippedSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _WestClippedSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);
static int _ClipSouthEastSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2);


/*****************************************************************************/
// Global Data
/*****************************************************************************/

static WEBC_READONLY CaseOpFn _MergeOps[16] = 
{
	_ClipNorthWestMergeOp,
	_ClipWestMergeOp,
	_ClipNorthMergeOp,
	_SurroundMergeOp,
	_EastClippedMergeOp,
	_ClipSouthWestMergeOp,
	_HCrossMergeOp,
	_ClipSouthMergeOp,
	_SouthClippedMergeOp,
	_VCrossMergeOp,
	_ClipNorthEastMergeOp,
	_ClipEastMergeOp,
	_SurroundedMergeOp,
	_NorthClippedMergeOp,
	_WestClippedMergeOp,
	_ClipSouthEastMergeOp
};

static WEBC_READONLY CaseOpFn _SplitOps[16] = 
{
	_ClipNorthWestSplitOp,
	_ClipWestSplitOp,
	_ClipNorthSplitOp,
	_SurroundSplitOp,
	_EastClippedSplitOp,
	_ClipSouthWestSplitOp,
	_HCrossSplitOp,
	_ClipSouthSplitOp,
	_SouthClippedSplitOp,
	_VCrossSplitOp,
	_ClipNorthEastSplitOp,
	_ClipEastSplitOp,
	_SurroundedSplitOp,
	_NorthClippedSplitOp,
	_WestClippedSplitOp,
	_ClipSouthEastSplitOp
};



/*****************************************************************************/
// Function bodies
/*****************************************************************************/

PresetWebRect::PresetWebRect()
{
	left = 0;
	top = 0;
	right = 0;
	bottom = 0;
}

PresetWebRect::PresetWebRect(const WebRect * r)
{
	Set(r->left, r->top, r->right, r->bottom);
}

PresetWebRect::PresetWebRect(const WebRect & r)
{
	Set(r.left, r.top, r.right, r.bottom);
}

PresetWebRect::PresetWebRect(DISPLAY_INT l, DISPLAY_INT t, DISPLAY_INT r, DISPLAY_INT b)
{
	Set(l,t,r,b);
}

int WebRect::Merge(WebRect *pRect1, WebRect *pRect2)
{
	return (MergeCase(pRect1, pRect2, Compare(pRect1)));
}

int WebRect::MergeCase(WebRect *r1, WebRect *r2, unsigned char rectCase)
{
	return (_MergeOps[rectCase & 0xf](this, r1, r2));
}

int WebRect::Split(WebRect *pRect1, WebRect *pRect2)
{
	return (SplitCase(pRect1, pRect2, Compare(pRect1)));
}

int WebRect::SplitCase(WebRect *r1, WebRect *r2, unsigned char rectCase)
{
	return (_SplitOps[rectCase & 0xf](this, r1, r2));
}
unsigned char WebRect::Compare(WebRect *pOther)
{
	unsigned char compareCase = 0;
	
	if (left > pOther->left)
	{
		compareCase |= 0x08;
	}
	
	if (top > pOther->top)
	{
		compareCase |= 0x04;
	}

	if (right > pOther->right)
	{
		compareCase |= 0x02;
	}

	if (bottom > pOther->bottom)
	{
		compareCase |= 0x01;
	}
	
	return (compareCase);
}


/*
  Clip NorthWest:

    +---+
    |   |
    |   |-+
    |   | |
    +---+ |
      |   |
      +---+
*/

int _ClipNorthWestMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	rect2->left   = rect0->right+1;
	rect2->top    = rect1->top;
	rect2->right  = rect1->right;
	rect2->bottom = rect1->bottom;

	rect1->right  = rect0->right;
	rect1->top    = rect0->bottom+1;	
	
	return (3);
}


/*
  Clip West:

    +---+
    |   |-+
    |   | |
    |   |-+
    +---+ 
*/

int _ClipWestMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	rect1->left = rect0->right+1;
	
	return (2);
}


/*
  Clip North:

    +---+
    |   |
    |   |
    |   |
    +---+
     | |
     +-+ 
*/

int _ClipNorthMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	rect1->top = rect0->bottom+1;
	
	return (2);
}


/*
  Surround:

    +---+
    |   |
    |   |
    |   |
    +---+
*/

int _SurroundMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	return (1);
}


/*
  East Clipped:

      +---+
    +---+ |
    |   | |
    +---+ |
      +---+
*/

int _EastClippedMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	rect0->right = rect1->left-1;
	
	return (2);
}


/*
  Clip SouthWest:

        +---+
        |   |
      +---+ |
      |   | |
      |   |-+
      |   |
      +---+
*/

int _ClipSouthWestMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	rect2->left   = rect0->right+1;
	rect2->top    = rect1->top;
	rect2->right  = rect1->right;
	rect2->bottom = rect1->bottom;
	
	rect1->right  = rect0->right;
	rect1->bottom = rect0->top-1;
	
	return (3);
}


/*
  Horizontal Cross:
  
       +-+
       | |
      +---+
      |   | 
      +---+
       | |
       +-+
*/

int _HCrossMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	rect2->left   = rect1->left;
	rect2->top    = rect1->top;
	rect2->right  = rect1->right;
	rect2->bottom = rect0->top-1;
	
	rect1->top    = rect0->bottom+1; 
	
	return (3);
}


/*
  Clip South:
  
     +-+ 
     | |
    +---+
    |   |
    |   |
    |   |
    +---+
*/

int _ClipSouthMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	rect1->bottom = rect0->top-1;
	
	return (2);
}

/*
  South Clipped:

     +-+ 
     | |
    +| |+
    || ||
    |+-+|
    |   |
    +---+
*/

int _SouthClippedMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	rect0->bottom = rect1->top-1;
	
	return (2);
}


/*
  Vertical Cross:
  
       +-+
     +-| |-+
     | | | |
     +-| |-+
       +-+
*/

int _VCrossMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	rect2->left   = rect1->left;
	rect2->top    = rect1->top;
	rect2->right  = rect0->left-1;
	rect2->bottom = rect1->bottom;
	
	rect1->left   = rect0->right+1;
	
	return (3);
}

/*
  Clip NorthEast:

        +---+
        |   |
      +-|   |
      | |   |
      | +---+
      |   |
      +---+
*/

int _ClipNorthEastMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	rect2->left   = rect1->left;
	rect2->top    = rect1->top;
	rect2->right  = rect0->left-1;
	rect2->bottom = rect1->bottom;
	
	rect1->left   = rect0->left;
	rect1->top    = rect0->bottom+1;
	
	return (3);
}


/*
  Clip East:

      +---+
    +-|   |
    | |   |
    +-|   |
      +---+
*/

int _ClipEastMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	rect1->right = rect0->left-1;
	
	return (2);
}


/*
  Surrounded:

    +---+
    |+-+|
    || ||
    |+-+|
    +---+
*/

int _SurroundedMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	rect0->Set(rect1);
	
	return (1);
}


/*
  North Clipped:

    +---+
    |   |
    |+-+|
    || ||
    +| |+
     | |
     +-+ 
*/

int _NorthClippedMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	rect0->top = rect1->bottom+1;
	
	return (2);
}


/*
  West Clipped:

    +---+
    | +---+
    | |   |
    | +---+
    +---+
*/

int _WestClippedMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	rect0->left = rect1->right+1;
	
	return (2);
}


/*
  Clip SouthEast:

      +---+
      |   |
      | +---+
      | |   |
      +-|   |
        |   |
        +---+
*/

int _ClipSouthEastMergeOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	rect2->left   = rect1->left;
	rect2->top    = rect1->top;
	rect2->right  = rect0->left-1;
	rect2->bottom = rect1->bottom;
	
	rect1->left   = rect0->left;
	rect1->bottom = rect0->top-1;
	
	return (3);
}

/*
  Clip NorthWest:

    +---+
    |   |
    |   |-+
    |   | |
    +---+ |
      |   |
      +---+

(rect0 is foreground, rect1 background) */

int _ClipNorthWestSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	rect1->right  = rect1->left - 1;
	rect1->left   = rect0->left;
	rect1->bottom = rect0->bottom;
	
	rect0->bottom = rect1->top - 1;

	return (2);
}


/*
  Clip West:

    +---+
    |   |-+
    |   | |
    |   |-+
    +---+ 

(rect0 is foreground, rect1 background) */

int _ClipWestSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	rect2->top    = rect1->bottom + 1;
	rect2->bottom = rect0->bottom;
	rect2->left   = rect0->left;
	rect2->right  = rect0->right;

	rect1->right  = rect1->left - 1;
	rect1->left   = rect0->left;
	
	rect0->bottom = rect1->top - 1;
	
	return (3);
}


/*
  Clip North:

    +---+
    |   |
    |   |
    |   |
    +---+
     | |
     +-+ 

(rect0 is foreground, rect1 background) */

int _ClipNorthSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	rect2->left   = rect1->right + 1;
	rect2->right  = rect0->right;
	rect2->top    = rect0->top;
	rect2->bottom = rect0->bottom;
	
	rect1->bottom = rect1->top - 1;
	rect1->top    = rect0->top;
	
	rect0->right  = rect1->left - 1;
		
	return (3);
}


/*
  Surround:

    +---+
    |   |
    |   |
    |   |
    +---+

(rect0 is foreground, rect1 background) */

int _SurroundSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	// tbd - this should really return 4 rects, but we'll just do one for now
	return (1);
}


/*
  East Clipped:

      +---+
    +---+ |
    |   | |
    +---+ |
      +---+

(rect0 is foreground, rect1 background) */

int _EastClippedSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	rect0->right = rect1->left-1;
	
	return (1);
}


/*
  Clip SouthWest:

        +---+
        |   |
      +---+ |
      |   | |
      |   |-+
      |   |
      +---+

(rect0 is foreground, rect1 background) */

int _ClipSouthWestSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{	
	rect1->right  = rect0->right;
	rect1->top    = rect1->bottom + 1;
	rect1->bottom = rect0->bottom;

	rect0->right  = rect1->left - 1;
	
	return (2);
}


/*
  Horizontal Cross:
  
       +-+
       | |
      +---+
      |   | 
      +---+
       | |
       +-+

(rect0 is foreground, rect1 background) */

int _HCrossSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	DISPLAY_INT temp = rect1->left;
	
	rect1->left   = rect1->right + 1;
	rect1->right  = rect0->right;
	rect1->top    = rect0->top;
	rect1->bottom = rect0->bottom;
	
	rect0->right  = temp - 1;
	
	return (2);
}


/*
  Clip South:
  
     +-+ 
     | |
    +---+
    |   |
    |   |
    |   |
    +---+

(rect0 is foreground, rect1 background) */

int _ClipSouthSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	rect2->right  = rect0->right;
	rect2->top    = rect0->top;
	rect2->left   = rect1->right + 1;
	rect2->bottom = rect0->bottom;
	
	rect1->top    = rect1->bottom + 1;
	rect1->bottom = rect0->bottom;
	
	rect0->right  = rect1->left - 1;
	
	return (3);
}

/*
  South Clipped:

     +-+ 
     | |
    +| |+
    || ||
    |+-+|
    |   |
    +---+

(rect0 is foreground, rect1 background) */

int _SouthClippedSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	rect0->bottom = rect1->top-1;
	
	return (1);
}


/*
  Vertical Cross:
  
       +-+
     +-| |-+
     | | | |
     +-| |-+
       +-+

(rect0 is foreground, rect1 background) */

int _VCrossSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	DISPLAY_INT temp = rect1->top;
	
	rect1->left   = rect0->left;
	rect1->right  = rect0->right;
	rect1->top    = rect1->bottom + 1;
	rect1->bottom = rect0->bottom;
	
	rect0->bottom = temp - 1;
	
	return (2);
}

/*
  Clip NorthEast:

        +---+
        |   |
      +-|   |
      | |   |
      | +---+
      |   |
      +---+

(rect0 is foreground, rect1 background) */

int _ClipNorthEastSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	rect1->bottom = rect0->bottom;
	rect1->left   = rect1->right + 1;
	rect1->right  = rect0->right;
	
	rect0->bottom = rect0->top - 1;
	
	return (2);
}


/*
  Clip East:

      +---+
    +-|   |
    | |   |
    +-|   |
      +---+

(rect0 is foreground, rect1 background) */

int _ClipEastSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	rect2->top    = rect1->bottom + 1;
	rect2->bottom = rect0->bottom;
	rect2->left   = rect0->left;
	rect2->right  = rect0->right;
	
	rect1->left   = rect1->right + 1;
	rect1->right  = rect0->right;
	
	rect0->bottom = rect1->top - 1;

	return (3);
}


/*
  Surrounded:

    +---+
    |+-+|
    || ||
    |+-+|
    +---+

(rect0 is foreground, rect1 background) */

int _SurroundedSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{	
	return (0);
}


/*
  North Clipped:

    +---+
    |   |
    |+-+|
    || ||
    +| |+
     | |
     +-+ 

(rect0 is foreground, rect1 background) */

int _NorthClippedSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	rect0->top = rect1->bottom+1;
	
	return (1);
}


/*
  West Clipped:

    +---+
    | +---+
    | |   |
    | +---+
    +---+

(rect0 is foreground, rect1 background) */

int _WestClippedSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	rect0->left = rect1->right + 1;
	
	return (1);
}


/*
  Clip SouthEast:

      +---+
      |   |
      | +---+
      | |   |
      +-|   |
        |   |
        +---+

(rect0 is foreground, rect1 background) */

int _ClipSouthEastSplitOp (WebRect *rect0, WebRect *rect1, WebRect *rect2)
{
	rect1->top    = rect0->top;
	rect1->left   = rect1->right + 1;
	rect1->right  = rect0->right;
	
	rect0->top    = rect1->bottom + 1;
	
	return (2);
}
