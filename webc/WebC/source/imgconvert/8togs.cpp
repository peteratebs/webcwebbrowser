//
// 8togs.CPP - 
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

#include "8togs.hpp"
#include "webimage.hpp"
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

indexedToGreyscale::indexedToGreyscale()
{
	transparentIndex = -1;
}

indexedToGreyscale::~indexedToGreyscale()
{
}

unsigned int indexedToGreyscale::getBytesPerLine (unsigned int width)
{
	WEBC_ASSERT(width > 0);
	
	switch (bits)
	{
		case 1: // 1 bpp, 2 colors
			return (width + 7) >> 3;
			
		case 2: // 2 bpp, 4 colors
			return (width + 3) >> 2;
			
		case 4: // 4 bpp, 16 colors
			return (width + 1) >> 1;
			
		case 8: // 8 bpp, 256 colors
			break;
	}
	
	return width;
}

void generatePaletteMap (WEBC_UINT8* dstMap, WEBC_UINT8* srcPalette, int numColors)
{
	int i;

	for (i=0; i<numColors; i++)
	{
		WEBC_UINT8 r = *(srcPalette++);
		WEBC_UINT8 g = *(srcPalette++);
		WEBC_UINT8 b = *(srcPalette++);
		
		*(dstMap++) = (WEBC_UINT8) RGBTOGREY(r, g, b);
	}	
}

void indexedToGreyscale::fsDither(unsigned char * _dest, unsigned char * source, unsigned int width, unsigned int height)
{
	WEBC_ASSERT(pal);
	
	unsigned int bytesPerLine = getBytesPerLine(width);
	unsigned char* dest;
	int length;
	int pixelsPerByte = 8 / bits;
	int numShades = 1 << bits;
	int floor;
	int c;
	int act;
	FSDitherChannel fs;
	fs.init(width);
	unsigned char color;
	//int offset;
	WEBC_UINT8 colorMap[256];
	int bitsInverse = 8 - bits;
	unsigned char nativeTransparent = (unsigned char) this->getTransparentColor();
	
	generatePaletteMap(colorMap, pal, mNumColors);
	
	WEBC_UINT8 bitsMask = (1 << bits) - 1;
	WEBC_UINT8 errMask = (1 << bitsInverse) - 1;
	
	for (unsigned int y=0; y<height; y++)
	{
		dest = &_dest[y * bytesPerLine];
		length = width;
		floor = length & (pixelsPerByte - 1);
		
		for (; length != floor; length -= pixelsPerByte)
		{
			color = 0;
			for (int t = bitsInverse; t >= 0; t -= bits, source++)
			{
				if (*source == transparentIndex)
				{
					act = nativeTransparent;
				}
				else
				{
					c = colorMap[*source] + fs.getError();

					if (c < 0)
					{
						act = 0;
						fs.addError(c);
					}
					else if (c & ~255) // (c >= 256)
					{
						act = bitsMask;
						fs.addError(c - 255);
					}
					else
					{
						act = (c >> bitsInverse) & bitsMask;
						fs.addError(c & errMask);	
					}

					if (transparentIndex >= 0 && act == nativeTransparent)
					{
						// we assign the darkest non-black shade of gray to be transparent;
						//  if we've mapped this pixel accidentally to the transparent color,
						//  then just make it a little darker
					  #ifdef WEBIMAGE_INVERT_GREYSCALE
						act++; // act = bitsMask;
					  #else
						act--; // act = 0;
					  #endif
					}
				}
				
				color |= act << t;
			}
			*dest++ = color;
		}
		color = 0;
		for (int t = bitsInverse; t > bitsInverse - bits * floor; t -= bits, source++)
		{
			if (*source == transparentIndex)
			{
				act = nativeTransparent;
			}
			else
			{
				c = colorMap[*source] + fs.getError();

				if (c < 0)
				{
					act = 0;
					fs.addError(c);
				}
				else if (c & ~255) // (c >= 255)
				{
					act = bitsMask;
					fs.addError(c - 255);
				}
				else				
				{
					act = (c >> bitsInverse) & bitsMask;
					fs.addError(c & errMask);
				}

				if (transparentIndex >= 0 && act == nativeTransparent)
				{
					// we assign the darkest non-black shade of gray to be transparent;
					//  if we've mapped this pixel accidentally to the transparent color,
					//  then just make it a little darker
				  #ifdef WEBIMAGE_INVERT_GREYSCALE
					act++; // act = bitsMask;
				  #else
					act--; // act = 0;
				  #endif
				}				
			}
			
			color |= act << t;
		}
		if (floor)
		{
			*dest++ = color;
		}
	}
	
}

void indexedToGreyscale::orderedDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
	WEBC_ASSERT(pal);
	int pixelsPerByte = 8 / bits;
	int numShades = 1 << bits;
	int c;
	unsigned char r,g,b;
	int pos = 8 - bits;
	unsigned char color = 0;
	int offset;
	for (unsigned int y = 0; y < height; y++)
	{
		for (unsigned int x = 0; x < width; x++)
		{
			offset = *source++ * 3;
			
			r=pal[offset]; g = pal[offset+1]; b = pal[offset+2];
			c = RGBTOGREY(r,g,b);
			int threshold = giDitherMatrix[x & 3][y & 3];
			int inc = (x & 1) ? 8 : 16;
			inc *= (y & 1) ? 1 : -1;		
			if (c < threshold)
			{
				c = EBSCLIP(c+inc,0,255);
			}
			c >>= 8 - bits;
			color |= c << pos;
			pos -= bits;
			if (pos < 0)
			{
				*dest++ = color;
				color = 0;
				pos = 8 - bits;
			}
		}
	}


}

void indexedToGreyscale::noDither(unsigned char * _dest, unsigned char * source, unsigned int width, unsigned int height)
{
	unsigned int bytesPerLine = getBytesPerLine(width);
	unsigned char* dest;
	int length;
	int pixelsPerByte = 8 / bits;
	int numShades = 1 << bits;
	int floor;
	int c;
	int act;
	unsigned char color;
	int offset;
	int bitsInverse = 8 - bits;
	int t;
	unsigned char nativeTransparent = (unsigned char) this->getTransparentColor();

	unsigned char* palRed   = pal + 0;
	unsigned char* palGreen = pal + 1;
	unsigned char* palBlue  = pal + 2;
	
	for (unsigned int y=0; y<height; y++)
	{
		dest = _dest;
		length = width;
		floor = length & (pixelsPerByte - 1);
		
		for (; length != floor; length -= pixelsPerByte)
		{
			color = 0;
			for (t = bitsInverse; t >= 0; t -= bits, source++)
			{			
				if (*source == transparentIndex)
				{
					act = nativeTransparent;
				}
				else
				{
					offset = (*source) + ((*source) << 1);
					c = RGBTOGREY(palRed[offset], palGreen[offset], palBlue[offset]);	
					act = (c & 0xff) >> bitsInverse;

					if (transparentIndex >= 0 && act == nativeTransparent)
					{
						// we assign the darkest non-black shade of gray to be transparent;
						//  if we've mapped this pixel accidentally to the transparent color,
						//  then just make it a little darker
					  #ifdef WEBIMAGE_INVERT_GREYSCALE
						act++;
					  #else
						act--;
					  #endif
					}
				}
					
				color |= act << t;
			}
			*dest = color;
			dest++;
		}
		
		color = 0;
		for (t = bitsInverse; t > bitsInverse - bits * floor; t -= bits, source++)
		{
			if (*source == transparentIndex)
			{
				act = nativeTransparent;
			}
			else
			{
				offset = (*source) + ((*source) << 1);
				c = RGBTOGREY(palRed[offset], palGreen[offset], palBlue[offset]);
				act = (c & 0xff) >> bitsInverse;

				if (transparentIndex >= 0 && act == nativeTransparent)
				{
					// we assign the darkest non-black shade of gray to be transparent;
					//  if we've mapped this pixel accidentally to the transparent color,
					//  then just make it a little darker
				  #ifdef WEBIMAGE_INVERT_GREYSCALE
					act++;
				  #else
					act--;
				  #endif
				}
			}
			
			color |= act << t;
		}
		
		if (floor)
		{
			*dest = color;
		}
		
		_dest += bytesPerLine;
	}
}

unsigned long indexedToGreyscale::getTransparentColor (void)
{
  #ifdef WEBIMAGE_INVERT_GREYSCALE
  	// 0 is white
	return (1 << bits) - 2;
  #else
  	// 0 is black
	return 1;
  #endif
}






	
indexedToGrayscale8bppTransparent::indexedToGrayscale8bppTransparent()
{
	grayLevels = 1;
	grayLevelsLog2 = 0;
	transparentIndex = -1;
	palette = 0;
	paletteSize = 0;
	levelsInverted = WEBC_FALSE;
}

indexedToGrayscale8bppTransparent::~indexedToGrayscale8bppTransparent()
{
}

void indexedToGrayscale8bppTransparent::noDither (
		unsigned char * dest, 
		unsigned char * source, 
		unsigned int width, 
		unsigned int height)
{
	WEBC_ASSERT(palette && grayLevels > 1 && grayLevels <= 256);
	WEBC_ASSERT(grayLevelsLog2 == 8 || grayLevelsLog2 == 4 || grayLevelsLog2 == 2 || grayLevelsLog2 == 1);
	
	unsigned int x,y;
	unsigned char srcPixel, dstPixel;
	unsigned char* pal_color;
	
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			srcPixel = *source;
			source++;
			if (srcPixel == transparentIndex)
			{
				dstPixel = 0xff;
			}
			else
			{
				pal_color = palette + (srcPixel + (srcPixel<<1));
				unsigned char r = *(pal_color++);
				unsigned char g = *(pal_color++);
				unsigned char b = *(pal_color);
				switch (grayLevelsLog2)
				{
					case 8:
						dstPixel = (WEBC_UINT8) (levelsInverted? (254 - _RGB_TO_GS8T_256(r,g,b)) : _RGB_TO_GS8T_256(r,g,b));
						break;
						
					case 4:
						dstPixel = (WEBC_UINT8) (levelsInverted? (15 - _RGB_TO_GS8T_16(r,g,b)) : _RGB_TO_GS8T_256(r,g,b));
						break;
						
					case 2:
						dstPixel = (WEBC_UINT8) (levelsInverted? (3 - _RGB_TO_GS8T_4(r,g,b)) : _RGB_TO_GS8T_256(r,g,b));
						break;
						
					case 1:
						dstPixel = (WEBC_UINT8) (levelsInverted? (1 - _RGB_TO_GS8T_2(r,g,b)) : _RGB_TO_GS8T_256(r,g,b));
						break;
				}
			}
			*dest = dstPixel;
			dest++;
		}
	}
	
}

void indexedToGrayscale8bppTransparent::orderedDither (
		unsigned char * dest, 
		unsigned char * source, 
		unsigned int width, 
		unsigned int height)
{
	WEBC_ASSERT(palette && grayLevels > 1 && grayLevels <= 256);
}

void indexedToGrayscale8bppTransparent::fsDither (
		unsigned char * dest, 
		unsigned char * source, 
		unsigned int width, 
		unsigned int height)
{
	WEBC_ASSERT(palette && grayLevels > 1 && grayLevels <= 256);
	FSDitherChannel fs;
	fs.init(width);

#define RGB_TO_GRAYLEVEL(r,g,b)  ((((unsigned long)(r)) + (((unsigned long)(r)) << 2) + \
                                 (((unsigned long)(g)) << 3) + ((unsigned long)(b)) + \
                                 (((unsigned long)(b)) << 1)) >> 4)


	WEBC_ASSERT(grayLevelsLog2 == 8 || grayLevelsLog2 == 4 || grayLevelsLog2 == 2 || grayLevelsLog2 == 1);
	
	unsigned int x,y;
	unsigned char srcPixel, dstPixel, dstGrayLevel;
	unsigned char* pal_color;
	
	for (y = 0; y < height; y++)
	{
		for (x = 0; x < width; x++)
		{
			srcPixel = *source;
			source++;
			if (srcPixel == transparentIndex)
			{
				dstPixel = 0xff;
			}
			else
			{
				pal_color = palette + (srcPixel + (srcPixel<<1));
				unsigned char r = *(pal_color++);
				unsigned char g = *(pal_color++);
				unsigned char b = *(pal_color);
				
				dstGrayLevel = (WEBC_UINT8) (levelsInverted? (255 - RGB_TO_GRAYLEVEL(r,g,b)) : RGB_TO_GRAYLEVEL(r,g,b));
				dstGrayLevel += fs.getError();				
				 
				switch (grayLevelsLog2)
				{
					case 8:
						dstPixel = dstGrayLevel;
						if (dstPixel != 0)
						{
							dstPixel--;
						}
						break;
						
					case 4:
						dstPixel = dstGrayLevel >> 4;
						fs.addError(dstPixel & 0x0f);
						break;
						
					case 2:
						dstPixel = dstGrayLevel >> 6;
						fs.addError(dstPixel & 0x3f);
						break;
						
					case 1:
						dstPixel = dstGrayLevel >> 7;
						fs.addError(dstPixel & 0x7f);
						break;
				}
			}
			*dest = dstPixel;
			dest++;
		}
	}	
}
