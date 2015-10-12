//
// 24to8.CPP - 
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

#include "webimage.hpp"
#include "24to8.hpp"
#include "webgraph.hpp"
#include "util.h"
#include "webcassert.h"


/*****************************************************************************/
// Defines/Macros
/*****************************************************************************/

/*****************************************************************************/
// Local Types
/*****************************************************************************/

/*****************************************************************************/
// Global Data
/*****************************************************************************/

/*****************************************************************************/
// Local functions
/*****************************************************************************/

RGB24ToIndexed::RGB24ToIndexed()
{
	ctx = WEBC_NULL;
}

RGB24ToIndexed::~RGB24ToIndexed()
{
}

void RGB24ToIndexed::noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
	WEBC_ASSERT(ctx && *ctx);
	int length = width * height;
	unsigned char r,g,b;
	HTMLColor error;

	for (; length != 0; length --)
	{
		r = *source++;
		g = *source++;
		b = *source++;

		*dest++ = (WEBC_UINT8) (*ctx)->RGBAToColor(ctx, HTML_RGBAToColor(r,g,b,0), &error);
	}
}

void RGB24ToIndexed::orderedDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
	WEBC_ASSERT(ctx && *ctx);
	int length = width * height;
	unsigned char r,g,b;
	HTMLColor error;

	for (unsigned int x = 0; x < width; x++)
	{
		for (unsigned int y = 0; y < height; y++)
		{
			r = *source++; g = *source++; b = *source++;
			int threshold = giDitherMatrix[x & 3][y & 3];
			int inc = (x & 1) ? 16 : 24;
			inc *= (y & 1) ? 1 : -1;
			if (r < threshold)
			{	
				r = EBSCLIP(r + inc,0,255);
			}
			if (g < threshold)
			{	
				g = EBSCLIP(g + inc,0,255);
			}
			if (b < threshold)
			{	
				b = EBSCLIP(b + inc,0,255);
			}
			*dest++ = (WEBC_UINT8) (*ctx)->RGBAToColor(ctx, HTML_RGBAToColor(r,g,b,0), &error);
		}
	}
}

void RGB24ToIndexed::fsDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
	WEBC_ASSERT(ctx && *ctx);
	int length = width * height;
	FSDitherChannel fsRed, fsGreen, fsBlue;
	fsRed.init(width); fsGreen.init(width); fsBlue.init(width);
	long r,g,b;
	HTMLColor e;
	for (; length != 0; length--)
	{
		r = *source++;
		r -= fsRed.getError();
		r = EBSCLIP(r, 0, 255);

		g = *source++;
		g -= fsGreen.getError();
		g = EBSCLIP(g, 0, 255);

		b = *source++;
		b -= fsBlue.getError();
		b = EBSCLIP(b, 0, 255);

		*dest++ = (WEBC_UINT8) (*ctx)->RGBAToColor(ctx, HTML_RGBAToColor(r,g,b,0), &e);
		
		fsRed.addError(HTML_ColorGetRed(e));
		fsGreen.addError(HTML_ColorGetGreen(e));
		fsBlue.addError(HTML_ColorGetBlue(e));
	}
}
