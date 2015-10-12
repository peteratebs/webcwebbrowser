//
// 8to24.CPP - 
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
#include "8to24.hpp"
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
// indexedToRGB24 methods
/*****************************************************************************/

indexedToRGB24::indexedToRGB24()
{
	mpPalette = 0;
	mpTranslationTable = 0;
	miTransparentIndex = -1;
	muTransparentRGB = 0;
	mbTransparentDefined = 0;
	mpTranslationTable = 0;
}

indexedToRGB24::indexedToRGB24(unsigned char * palette)
{
	mpPalette = palette;
	mpTranslationTable = 0;
	miTransparentIndex = -1;
	muTransparentRGB = 0;
	mbTransparentDefined = 0;
	mpTranslationTable = 0;
}

indexedToRGB24::indexedToRGB24(unsigned char * palette, int transparentIndex)
{
	mpPalette = palette;
	mpTranslationTable = 0;
	miTransparentIndex = transparentIndex;
	muTransparentRGB = 0;
	mbTransparentDefined = 1;
	mpTranslationTable = 0;
}

indexedToRGB24::~indexedToRGB24()
{
}

void indexedToRGB24::noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{	
	if (mbTransparentDefined)
	{
		muTransparentRGB = 1; // very dark blue
	}
	
	long length = width * height;
	
	if (mbTransparentDefined)
	{
		for (; length != 0; length --)
		{
			if (*source == miTransparentIndex)
			{
				*dest++ = 0; // red
				*dest++ = 0; // green
				*dest++ = 1; // blue
				source++;
				continue;
			}

			int offset = *source++ * 3;

			*dest++ = mpPalette[offset];
			*dest++ = mpPalette[offset+1];
			*dest++ = mpPalette[offset+2];
		}
	}
	else
	{
		for (; length != 0; length --)
		{
			int offset = *source++ * 3;

			*dest++ = mpPalette[offset];
			*dest++ = mpPalette[offset+1];
			*dest++ = mpPalette[offset+2];
		}
	}
}

void indexedToRGB24::fsDither(unsigned char * _dest, unsigned char * source, unsigned int width, unsigned int height)
{
}

void indexedToRGB24::orderedDither(unsigned char * _dest, unsigned char * source, unsigned int width, unsigned int height)
{	
}


/*****************************************************************************/
// indexedToBGR24 methods
/*****************************************************************************/

indexedToBGR24::indexedToBGR24()
	: indexedToRGB24()
{
}

indexedToBGR24::indexedToBGR24(unsigned char * palette)
	: indexedToRGB24(palette)
{
}

indexedToBGR24::indexedToBGR24(unsigned char * palette, int transparentIndex)
	: indexedToRGB24(palette, transparentIndex)
{
}

indexedToBGR24::~indexedToBGR24()
{
}

void indexedToBGR24::noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{	
	if (mbTransparentDefined)
	{
		muTransparentRGB = 1; // very dark blue
	}
	
	long length = width * height;
	
	if (mbTransparentDefined)
	{
		for (; length != 0; length --)
		{
			if (*source == miTransparentIndex)
			{
				*dest++ = 1; // blue 
				*dest++ = 0; // green
				*dest++ = 0; // red
				source++;
				continue;
			}

			int offset = *source++ * 3;

			*dest++ = mpPalette[offset+2];
			*dest++ = mpPalette[offset+1];
			*dest++ = mpPalette[offset];
		}
	}
	else
	{
		for (; length != 0; length --)
		{
			int offset = *source++ * 3;

			*dest++ = mpPalette[offset+2];
			*dest++ = mpPalette[offset+1];
			*dest++ = mpPalette[offset];
		}
	}
}
