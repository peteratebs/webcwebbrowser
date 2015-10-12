//
// 24to555.CPP - 
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

#include "webgraph.hpp"
#include "24to555.hpp"
#include "webimage.hpp"
#include "util.h"


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

RGB24To555::RGB24To555()
{
}

RGB24To555::~RGB24To555()
{
}

#define RGBTO555(R,G,B)  (((short)(R) & 0xf8) << 7) | (((short)(G) & 0xf8) << 2) | (((short)(B) & 0xf8) >> 3) 

void RGB24To555::noDither(unsigned char * _dest, unsigned char * source, unsigned int width, unsigned int height)
{
	unsigned short *dest = (unsigned short *) _dest;
	
	int length = width * height;
	for (; length != 0; length --)
	{
		*dest = RGBTO555(source[0],source[1],source[2]);
		source += 3;
		dest++;
	}
}

void RGB24To555::fsDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
/*
	int length = width * height;
	FSDitherChannel fs[3];
	fs[0].init(width); fs[1].init(width); fs[2].init(width);
	html_color col, er;
	for (; length != 0; length --)
	{
		if (*source == miTransparentIndex)
		{
			*dest++ = miTransparentIndex;
			fs[0].addError(0);
			fs[1].addError(0);
			fs[2].addError(0);
			source++;
			continue;
		}
		int offset = *source++ * 3;
		col.red = EBSCLIP(mpPalette[offset] - fs[0].getError(), 0, 255);
		col.green = EBSCLIP(mpPalette[offset+1] - fs[1].getError(), 0, 255);
		col.blue = EBSCLIP(mpPalette[offset+2] - fs[2].getError(), 0, 255);
		*dest++ = mpGraphics->RGBToColor(&col, &er);
		fs[0].addError(er.red);
		fs[1].addError(er.green);
		fs[2].addError(er.blue);
	}
	*/
}

void RGB24To555::orderedDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
	/*
	unsigned char r,g,b;
	for (unsigned int x = 0; x < width; x++)
	{
		for (unsigned int y = 0; y < height; y++)
		{
			if (*source == miTransparentIndex)
			{
				*dest++ = miTransparentIndex;
				source++;
				continue;
			}
			int offset = *source++ * 3;
			r = mpPalette[offset];
			g = mpPalette[offset+1];
			b = mpPalette[offset+2];
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
			*dest++ = mpGraphics->RGBToColor(r,g,b);	
		}
	}
*/
}
