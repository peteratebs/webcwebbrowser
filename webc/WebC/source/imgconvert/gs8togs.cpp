//
// gs8togs.CPP - 
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
#include "gs8togs.hpp"
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

greyscale8ToGreyscale::greyscale8ToGreyscale()
{
}


greyscale8ToGreyscale::~greyscale8ToGreyscale()
{
}

void greyscale8ToGreyscale::noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
	int length = width * height;
	int pixelsPerByte = 8 / bits;
	int floor = length & pixelsPerByte;
	int c;
	unsigned char color;
	for (; length != floor; length -= pixelsPerByte)
	{
		color = 0;
		for (int t = 8 - bits; t >= 0; t -= bits)
		{
			c = *source++;
			c >>= 8 - bits;
			color |= c << t;
		}
		*dest++ = color;
	}
	color = 0;
	for (int t = 8 - bits; t > 8 - bits - bits * floor; t -= bits)
	{
		c = *source++;
		c=EBSCLIP(c,0,255);
		c >>= (8 - bits);
		color |= c << t;
	}
	if (floor)
		*dest++ = color;

}

void greyscale8ToGreyscale::fsDither(unsigned char * _dest, unsigned char * source, unsigned int width, unsigned int height)
{
	int bytesPerLine = 0;
	unsigned char * dest;
	switch (bits)
	{
	case 1: // 1 bpp, 2 colors
		bytesPerLine = (width + 7) >> 3;
		break;
		
	case 2: // 2 bpp, 4 colors
		bytesPerLine = (width + 3) >> 2;
		break;
		
	case 4: // 4 bpp, 16 colors
		bytesPerLine = (width + 1) >> 1;
		break;
		
	case 8: // 8 bpp, 256 colors
		bytesPerLine = width;
		break;
	}
	
	int length;
	int pixelsPerByte = 8 / bits;
	int numShades = 1 << bits;
	int floor;
	int c;
	int act;
	FSDitherChannel fs;
	fs.init(width);
	unsigned char color;
	
	for (unsigned int y=0; y<height; y++)
	{
		dest = &_dest[y * bytesPerLine];
		length = width;
		floor = length & (pixelsPerByte - 1);

		for (; length != floor; length -= pixelsPerByte)
		{
			color = 0;
			for (int t = 8 - bits; t >= 0; t -= bits)
			{
			  #ifdef WEBIMAGE_INVERT_GREYSCALE
				c = (255 - *source) - fs.getError();
			  #else
				c = (*source) - fs.getError();
			  #endif
				source++;
				c = EBSCLIP(c,0,255);
				act = c >> (8 - bits);
				fs.addError(act * (255 / (numShades - 1)) - c);
				color |= act << t;
			}
			*dest++ = color;
		}
		color = 0;
		for (int t = 8 - bits; t > 8 - bits - bits * floor; t -= bits)
		{
		  #ifdef WEBIMAGE_INVERT_GREYSCALE
			c = (255 - *source) - fs.getError();
		  #else
			c = (*source) - fs.getError();
		  #endif
			source++;
			c = EBSCLIP(c,0,255);
			act = c >> (8 - bits);
			fs.addError(act * (255 / (numShades - 1)) - c);
			color |= act << t;
		}
		if (floor)
		{
			*dest++ = color;
		}
	}
}

void greyscale8ToGreyscale::orderedDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
	int c;
	int pos = 8 - bits;
	unsigned char color = 0;
	for (unsigned int y = 0; y < height; y++)
	{
		for (unsigned int x = 0; x < width; x++)
		{
			c = *source++;
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
