#ifndef __8TO24_HPP__
#define __8TO24_HPP__

#include "imgconv.hpp"

class indexedToRGB24 : public WebcBitmapConverter
{
	protected:
		virtual void noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		virtual void orderedDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		virtual void fsDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		
		unsigned char  * mpPalette;
		unsigned short * mpTranslationTable;
		int              miTransparentIndex;
		unsigned short   muTransparentRGB;
		int              mbTransparentDefined;
		
	public:
		indexedToRGB24();
		indexedToRGB24(unsigned char * palette);
		indexedToRGB24(unsigned char * palette, int transparentIndex);
		virtual ~indexedToRGB24();
		
		void setPalette(unsigned char * p) { mpPalette = p; }
		void setTranslate(unsigned short * t) { mpTranslationTable = t; }
		void setTransparentIndex(int t) { miTransparentIndex = t; mbTransparentDefined = 1; }
		virtual unsigned long getTransparentColor (void) { return muTransparentRGB; }
};

class indexedToBGR24 : public indexedToRGB24
{
	protected:
		virtual void noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		
	public:
		indexedToBGR24();
		indexedToBGR24(unsigned char * palette);
		indexedToBGR24(unsigned char * palette, int transparentIndex);
		virtual ~indexedToBGR24();
};

#endif // __8TO24_HPP__
