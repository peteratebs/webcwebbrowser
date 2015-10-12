#ifndef __8TOGS_HPP__
#define __8TOGS_HPP__

#include "imgconv.hpp"
#include "webctypes.h"

/* RGB24 and indexed to greyscale can only convert to 1,2,4 or 8 bit
   greyscale images.  The leftmost pixel is stored in the most
   significant bits.  No crazy alignment issues are taken care of,
   like making sure lines don't cross byte boundaries. */

class indexedToGreyscale : public WebcBitmapConverter
{
	protected:
		virtual void noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		virtual void orderedDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		virtual void fsDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		unsigned int getBytesPerLine (unsigned int width);

		char bits;
		unsigned char * pal;
		int transparentIndex;
		int mNumColors;

	public:
		indexedToGreyscale();
		~indexedToGreyscale();
		void setBitDepth(char c) { bits = c; };
		void setPalette(unsigned char * p, int numColors) { pal = p; mNumColors = numColors; }
		void setTransparentIndex(int t) { transparentIndex = t; }
		virtual unsigned long getTransparentColor (void);
};

// This converter implements a
//  special mode where output bitmaps are always 8bpp,
//  and 0xFF is transparent, but for example in a 4bpp bitmap,
//  the pixel range is still 0 (black) to 15 (white).  Therefore
//  when converting to a 4bpp grayscale bitmap using 8bpp
//  transparency mode, indices 16 to 254 are not used.

class indexedToGrayscale8bppTransparent : public WebcBitmapConverter
{
 protected:
	virtual void noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
	virtual void orderedDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
	virtual void fsDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);

	WEBC_UINT16 grayLevels;
	WEBC_UINT8  grayLevelsLog2;
	WEBC_UINT8  transparentIndex;
	WEBC_UINT8* palette;
	WEBC_INT16  paletteSize;
	WEBC_BOOL   levelsInverted;

#define _RGB_TO_GS8T_256(r,g,b) (((((unsigned long)(r)) << 6) + (((unsigned long)(r)) << 4) + \
                                  (((unsigned long)(g)) << 7) + \
                                  (((unsigned long)(b)) << 5) + (((unsigned long)(b)) << 3) + (((unsigned long)(b)) << 2) + (((unsigned long)(b)) << 1) + ((unsigned long)(b))) >> 8)
#define _RGB_TO_GS8T_16(r,g,b)  ((((unsigned long)(r)) + (((unsigned long)(r)) << 2) + \
                                 (((unsigned long)(g)) << 3) + \
                                 ((unsigned long)(b)) + (((unsigned long)(b)) << 1)) >> 8)
#define _RGB_TO_GS8T_4(r,g,b)   ((((unsigned long)(r)) + (((unsigned long)(r)) << 2) + \
                                 (((unsigned long)(g)) << 3) + \
                                 ((unsigned long)(b)) + (((unsigned long)(b)) << 1)) >> 10)
#define _RGB_TO_GS8T_2(r,g,b)   ((((unsigned long)(r)) + (((unsigned long)(r)) << 2) + \
                                 (((unsigned long)(g)) << 3) + \
                                 ((unsigned long)(b)) + (((unsigned long)(b)) << 1)) >> 11)

	// 0 to grayLevels-1 maps to black to white (or white to black, if grayscale inverted)
 	// 255 is always the transparent color

 public:
	indexedToGrayscale8bppTransparent();
	~indexedToGrayscale8bppTransparent();
	void setGrayLevelsLog2 (WEBC_UINT8 bits) { grayLevelsLog2 = bits; grayLevels = 1 << bits; };
	void setPalette (unsigned char * p, int numColors) { palette = p; paletteSize = numColors; }
	void setTransparentIndex (int t) { transparentIndex = (WEBC_UINT8) t; }
	void setLevelsInverted (WEBC_BOOL b) { levelsInverted = b; }
	virtual unsigned long getTransparentColor (void) { return 255; }
};

#endif // __8TOGS_HPP__
