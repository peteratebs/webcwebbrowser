//
// 24togs.CPP - 
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
#include "24togs.hpp"
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

RGB24ToGreyscale::RGB24ToGreyscale()
{
	miBits = 8; // default to 256 shades of gray
}

RGB24ToGreyscale::~RGB24ToGreyscale()
{
}

void RGB24ToGreyscale::fsDither(unsigned char * _dest, unsigned char * source, unsigned int width, unsigned int height)
{
	int bytesPerLine;
	unsigned char * dest;
	switch (miBits)
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
			
		default: // unsupported bit depth
			// return error ?!
			return;
	}
	
	int length;
	int pixelsPerByte = 8 / miBits;
	int numShades = 1 << miBits;
	int floor;
	int c;
	int act;
	int bitsInverse = 8 - miBits;
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
			for (int t = bitsInverse; t >= 0; t -= miBits)
			{
				c = RGBTOGREY(source[0], source[1], source[2]) - fs.getError();
				c = EBSCLIP(c,0,255);
				act = c >> bitsInverse;
				fs.addError(act * (255 / (numShades - 1)) - c);
				color |= act << t;
				source += 3;
			}
			*dest++ = color;
		}
		
		color = 0;
		for (int t = bitsInverse; t > bitsInverse - miBits * floor; t -= miBits)
		{
			c = RGBTOGREY(source[0], source[1], source[2]) - fs.getError();
			c = EBSCLIP(c,0,255);
			act = c >> bitsInverse;
			fs.addError(act * (255 / (numShades - 1)) - c);
			color |= act << t;
			source += 3;
		}
		
		if (floor)
		{
			*dest++ = color;
		}
	}
}

void RGB24ToGreyscale::orderedDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height)
{
	int pixelsPerByte = 8 / miBits;
	int numShades = 1 << miBits;
	int c;
	unsigned char r,g,b;
	int bitsInverse = 8 - miBits;
	int pos = bitsInverse;
	unsigned char color = 0;
	
	for (unsigned int y = 0; y < height; y++)
	{
		for (unsigned int x = 0; x < width; x++)
		{
			r=*source++; g = *source++; b = *source++;
			c = RGBTOGREY(r,g,b);
			int threshold = giDitherMatrix[x & 3][y & 3];
			int inc = (x & 1) ? 8 : 16;
			inc *= (y & 1) ? 1 : -1;		
			if (c < threshold)
			{
				c = EBSCLIP(c+inc,0,255);
			}
			c >>= bitsInverse;
			color |= c << pos;
			pos -= miBits;
			if (pos < 0)
			{
				*dest++ = color;
				color = 0;
				pos = bitsInverse;
			}
		}
	}


}

void RGB24ToGreyscale::noDither(unsigned char * _dest, unsigned char * source, unsigned int width, unsigned int height)
{
	int length;
	int pixelsPerByte = 8 / miBits;
	int floor;
	int c;
	unsigned char color;
	int bitsInverse = 8 - miBits;
	long bytesPerLine;
	unsigned char * dest;

	switch (miBits)
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
			
		default: // unsupported bit depth
			// return error ?!
			return;
	}
	
	for (unsigned int y=0; y<height; y++)
	{
		dest = &_dest[y * bytesPerLine];
		length = width;
		floor = length & (pixelsPerByte - 1);

		for (; length != floor; length -= pixelsPerByte)
		{
			color = 0;
			for (int t = bitsInverse; t >= 0; t -= miBits)
			{
				c = RGBTOGREY(source[0], source[1], source[2]);
				c >>= bitsInverse;
				color |= c << t;
				source += 3;
			}
			*dest++ = color;
		}
		
		color = 0;
		for (int t = bitsInverse; t > bitsInverse - miBits * floor; t -= miBits)
		{
			c = RGBTOGREY(source[0], source[1], source[2]);
			c >>= bitsInverse;
			color |= c << t;
			source += 3;
		}
		
		if (floor)
		{
			*dest++ = color;
		}
	}
}
