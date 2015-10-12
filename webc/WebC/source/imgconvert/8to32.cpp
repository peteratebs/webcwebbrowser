//
// 8to32.CPP - 
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
#include "8to32.hpp"
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

indexedToRGBA32::indexedToRGBA32()
{
	mpPalette = 0;
	mpTranslationTable = 0;
	miTransparentIndex = -1;
	muTransparentRGB = 0;
	mbTransparentDefined = 0;
	mpTranslationTable = 0;
}

indexedToRGBA32::indexedToRGBA32(unsigned char * palette)
{
	mpPalette = palette;
	mpTranslationTable = 0;
	miTransparentIndex = -1;
	muTransparentRGB = 0;
	mbTransparentDefined = 0;
	mpTranslationTable = 0;
}

indexedToRGBA32::indexedToRGBA32(unsigned char * palette, int transparentIndex)
{
	mpPalette = palette;
	mpTranslationTable = 0;
	miTransparentIndex = transparentIndex;
	muTransparentRGB = 0;
	mbTransparentDefined = 1;
	mpTranslationTable = 0;
}

indexedToRGBA32::~indexedToRGBA32()
{
}

void indexedToRGBA32::noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
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
				*dest++ = 0; // alpha
				source++;
				continue;
			}

			int offset = *source++ * 3;

			*dest++ = mpPalette[offset];   // red
			*dest++ = mpPalette[offset+1]; // green
			*dest++ = mpPalette[offset+2]; // blue
			*dest++ = 0;                   // alpha
		}
	}
	else
	{
		for (; length != 0; length --)
		{
			int offset = *source++ * 3;

			*dest++ = mpPalette[offset];   // red
			*dest++ = mpPalette[offset+1]; // green
			*dest++ = mpPalette[offset+2]; // blue
			*dest++ = 0;                   // alpha
		}
	}
}

void indexedToRGBA32::fsDither(unsigned char * _dest, unsigned char * source, unsigned int width, unsigned int height)
{
}

void indexedToRGBA32::orderedDither(unsigned char * _dest, unsigned char * source, unsigned int width, unsigned int height)
{	
}


/*****************************************************************************/
// indexedToBGRA32 methods
/*****************************************************************************/

indexedToBGRA32::indexedToBGRA32()
	: indexedToRGBA32()
{
}

indexedToBGRA32::indexedToBGRA32(unsigned char * palette)
	: indexedToRGBA32(palette)
{
}

indexedToBGRA32::indexedToBGRA32(unsigned char * palette, int transparentIndex)
	: indexedToRGBA32(palette, transparentIndex)
{
}

indexedToBGRA32::~indexedToBGRA32()
{
}

void indexedToBGRA32::noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
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
				*dest++ = 0; // alpha
				source++;
				continue;
			}

			int offset = *source++ * 3;

			*dest++ = mpPalette[offset+2]; // blue
			*dest++ = mpPalette[offset+1]; // green
			*dest++ = mpPalette[offset];   // red
			*dest++ = 0;                   // alpha
		}
	}
	else
	{
		for (; length != 0; length --)
		{
			int offset = *source++ * 3;

			*dest++ = mpPalette[offset+2]; // blue
			*dest++ = mpPalette[offset+1]; // green
			*dest++ = mpPalette[offset];   // red
			*dest++ = 0;                   // alpha
		}
	}
}
