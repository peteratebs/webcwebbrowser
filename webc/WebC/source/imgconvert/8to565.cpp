//
// 8to565.CPP - 
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
#include "8to565.hpp"
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

indexedTo565::indexedTo565()
{
	mpPalette = 0;
	mpTranslationTable = 0;
	miTransparentIndex = -1;
	muTransparentRGB = 0;
	mbTransparentDefined = 0;
	mpTranslationTable = 0;
}

indexedTo565::indexedTo565(const unsigned char * palette)
{
	mpPalette = palette;
	mpTranslationTable = 0;
	miTransparentIndex = -1;
	muTransparentRGB = 0;
	mbTransparentDefined = 0;
	mpTranslationTable = 0;
}

indexedTo565::indexedTo565(const unsigned char * palette, int transparentIndex)
{
	mpPalette = palette;
	mpTranslationTable = 0;
	miTransparentIndex = transparentIndex;
	muTransparentRGB = 0;
	mbTransparentDefined = 1;
	mpTranslationTable = 0;
}

indexedTo565::~indexedTo565()
{
}

#define RGBTO565(R,G,B)  (((short)(R) & 0xf8) << 8) | (((short)(G) & 0xfc) << 3) | (((short)(B) & 0xf8) >> 3) 

void indexedTo565::noDither(unsigned char * _dest, unsigned char * source, unsigned int width, unsigned int height)
{
	unsigned short *dest = (unsigned short *) _dest;
	unsigned short rgb;
	
	if (mbTransparentDefined)
	{
		muTransparentRGB = 1; 
	}
	
	int length = width * height;
	if (!mpTranslationTable)
	{
		if (mbTransparentDefined)
		{
			for (; length != 0; length --)
			{
				if (*source == miTransparentIndex)
				{
					*dest++ = muTransparentRGB;
					source++;
					continue;
				}
				int offset = *source++ * 3;

				rgb = RGBTO565(mpPalette[offset],mpPalette[offset+1],mpPalette[offset+2]);

				if (rgb == 1)
				{
					rgb = 0;
				}

				*dest = rgb;
				dest++;
			}
		}
		else
		{
			for (; length != 0; length --)
			{
				int offset = *source++ * 3;

				rgb = RGBTO565(mpPalette[offset],mpPalette[offset+1],mpPalette[offset+2]);

				if (rgb == 1)
				{
					rgb = 0;
				}

				*dest = rgb;
				dest++;
			}
		}
	}
	else
	{
		for (; length != 0; length --)
		{
			*dest++ = mpTranslationTable[*source++];
		}
	}
}

void indexedTo565::fsDither(unsigned char * _dest, unsigned char * source, unsigned int width, unsigned int height)
{
	unsigned short * dest = (unsigned short *) _dest;
	int length = width * height;
	FSDitherChannel fs[3];
	fs[0].init(width); fs[1].init(width); fs[2].init(width);
	//html_color col, er;
	WEBC_UINT8 r,g,b;
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
		r = EBSCLIP(mpPalette[offset]   - fs[0].getError(), 0, 255);
		g = EBSCLIP(mpPalette[offset+1] - fs[1].getError(), 0, 255);
		b  = EBSCLIP(mpPalette[offset+2] - fs[2].getError(), 0, 255);
		*dest++ = RGBTO565(r,g,b);
		fs[0].addError(r&7);
		fs[1].addError(g&3);
		fs[2].addError(b&7);
	}
}

void indexedTo565::orderedDither(unsigned char * _dest, unsigned char * source, unsigned int width, unsigned int height)
{
	unsigned short * dest = (unsigned short *) _dest;
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
			*dest++ = RGBTO565(r,g,b);	
		}
	}
	
	
}
