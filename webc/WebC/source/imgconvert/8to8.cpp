//
// 8to8.CPP - 
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
#include "webimage.hpp"
#include "8to8.hpp"
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

indexedToIndexed::indexedToIndexed()
{
	mpGraphics = 0;
	mpPalette = 0;
	mpTranslationTable = 0;
	mSrcTransparentIndex = -1;
	mDstTransparentIndex = 0;
}

indexedToIndexed::~indexedToIndexed()
{
}

void indexedToIndexed::noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
	int length = width * height;
	if (!mpTranslationTable)
	{
		int offset;
		HTMLColor error;

		if (mSrcTransparentIndex >= 0)
		{			
			for (; length != 0; length --)
			{
				if (*source == mSrcTransparentIndex)
				{
					source++;
					*dest = mDstTransparentIndex;
					dest++;
				}
				else
				{
					offset = *source;
					source++;
					offset += (offset << 1); // offset *= 3;
					*dest = (WEBC_UINT8) (*mpGraphics)->RGBAToColor (
							mpGraphics, 
							HTML_RGBAToColor(mpPalette[offset],mpPalette[offset+1],mpPalette[offset+2],0), 
							&error
						);
					dest++;
				}
			}
		}
		else
		{
			for (; length != 0; length --)
			{
				offset = *source;
				source++;
				offset += (offset << 1); // offset *= 3;
				*dest = (WEBC_UINT8) (*mpGraphics)->RGBAToColor (
						mpGraphics, 
						HTML_RGBAToColor(mpPalette[offset],mpPalette[offset+1],mpPalette[offset+2],0), 
						&error
					);
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

void indexedToIndexed::fsDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
	long offset;
	long length = width * height;
	long r, g, b;
	FSDitherChannel fsRed;
	FSDitherChannel fsGreen;
	FSDitherChannel fsBlue;
	HTMLColor er;
	unsigned char* palRed = mpPalette;
	unsigned char* palGreen = mpPalette + 1;
	unsigned char* palBlue = mpPalette + 2;

	fsRed.init(width); 
	fsGreen.init(width); 
	fsBlue.init(width);

	for (; length != 0; length --)
	{
		if (*source == mSrcTransparentIndex)
		{
			*dest++ = mDstTransparentIndex;
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

		*dest = (WEBC_UINT8) (*mpGraphics)->RGBAToColor (
				mpGraphics, 
				HTML_RGBAToColor(r,g,b,0), 
				&er
			);
		dest++;

		fsRed.addError(HTML_ColorGetRed(er));
		fsGreen.addError(HTML_ColorGetGreen(er));
		fsBlue.addError(HTML_ColorGetBlue(er));
	}
}

void indexedToIndexed::orderedDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
	unsigned char r,g,b;
	HTMLColor er;
	for (unsigned int x = 0; x < width; x++)
	{
		for (unsigned int y = 0; y < height; y++)
		{
			if (*source == mSrcTransparentIndex)
			{
				*dest++ = mDstTransparentIndex;
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

			*dest = (WEBC_UINT8) (*mpGraphics)->RGBAToColor (
					mpGraphics, 
					HTML_RGBAToColor(r,g,b,0), 
					&er
				);
			dest++;
		}
	}
	
	
}
