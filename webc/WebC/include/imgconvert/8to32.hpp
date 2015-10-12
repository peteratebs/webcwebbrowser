#ifndef __8TO32_HPP__
#define __8TO32_HPP__

#include "imgconv.hpp"

class indexedToRGBA32 : public WebcBitmapConverter
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
		indexedToRGBA32();
		indexedToRGBA32(unsigned char * palette);
		indexedToRGBA32(unsigned char * palette, int transparentIndex);
		virtual ~indexedToRGBA32();
		
		void setPalette(unsigned char * p) { mpPalette = p; }
		void setTranslate(unsigned short * t) { mpTranslationTable = t; }
		void setTransparentIndex(int t) { miTransparentIndex = t; mbTransparentDefined = 1; }
		virtual unsigned long getTransparentColor (void) { return muTransparentRGB; }
};

class indexedToBGRA32 : public indexedToRGBA32
{
	protected:
		virtual void noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		
	public:
		indexedToBGRA32();
		indexedToBGRA32(unsigned char * palette);
		indexedToBGRA32(unsigned char * palette, int transparentIndex);
		virtual ~indexedToBGRA32();
};

#endif // __8TO32_HPP__
