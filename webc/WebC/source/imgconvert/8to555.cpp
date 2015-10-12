//
// 8to555.CPP - 
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
#include "8to555.hpp"
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

indexedTo555::indexedTo555()
{
	mpPalette = 0;
	mpTranslationTable = 0;
	miTransparentIndex = -1;
	muTransparentRGB = 0;
	mbTransparentDefined = 0;
	mpTranslationTable = 0;
}

indexedTo555::indexedTo555(const unsigned char * palette)
{
	mpPalette = palette;
	mpTranslationTable = 0;
	miTransparentIndex = -1;
	muTransparentRGB = 0;
	mbTransparentDefined = 0;
	mpTranslationTable = 0;
}

indexedTo555::indexedTo555(const unsigned char * palette, int transparentIndex)
{
	mpPalette = palette;
	mpTranslationTable = 0;
	miTransparentIndex = transparentIndex;
	muTransparentRGB = 0;
	mbTransparentDefined = 1;
	mpTranslationTable = 0;
}

indexedTo555::~indexedTo555()
{
}

#define RGBTO555(R,G,B)  (((short)(R) & 0xf8) << 7) | (((short)(G) & 0xf8) << 2) | (((short)(B) & 0xf8) >> 3) 

void indexedTo555::noDither(unsigned char * _dest, unsigned char * source, unsigned int width, unsigned int height)
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

				rgb = RGBTO555(mpPalette[offset],mpPalette[offset+1],mpPalette[offset+2]);

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

				rgb = RGBTO555(mpPalette[offset],mpPalette[offset+1],mpPalette[offset+2]);

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

void indexedTo555::fsDither(unsigned char * _dest, unsigned char * source, unsigned int width, unsigned int height)
{
	unsigned short * dest = (unsigned short *) _dest;
	long offset;
	long length = width * height;
	long r, g, b;
	FSDitherChannel fsRed;
	FSDitherChannel fsGreen;
	FSDitherChannel fsBlue;
	const unsigned char* palRed = mpPalette;
	const unsigned char* palGreen = mpPalette + 1;
	const unsigned char* palBlue = mpPalette + 2;

	fsRed.init(width); 
	fsGreen.init(width); 
	fsBlue.init(width);

	for (; length != 0; length--)
	{
		if (*source == miTransparentIndex)
		{
			*dest = miTransparentIndex;
			dest++;
			fsRed.addError(0);
			fsGreen.addError(0);
			fsBlue.addError(0);
			source++;
			continue;
		}
		
		offset = *source;
		source++;

		offset += offset << 1;
		
		r = palRed[offset]   - fsRed.getError();
		g = palGreen[offset] - fsGreen.getError();
		b = palBlue[offset]  - fsBlue.getError();

		r = EBSCLIP(r, 0, 255);
		g = EBSCLIP(g, 0, 255);
		b = EBSCLIP(b, 0, 255);

		*dest = RGBTO555(r,g,b);
		dest++;

		fsRed.addError   ((char)(r & 0x7));
		fsGreen.addError ((char)(g & 0x7));
		fsBlue.addError  ((char)(b & 0x7));
	}
}

void indexedTo555::orderedDither(unsigned char * _dest, unsigned char * source, unsigned int width, unsigned int height)
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
			*dest++ = RGBTO555(r,g,b);	
		}
	}
	
	
}
