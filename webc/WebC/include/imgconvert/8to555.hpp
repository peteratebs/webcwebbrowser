#ifndef __8TO555_HPP__
#define __8TO555_HPP__

#include "imgconv.hpp"

class indexedTo555 : public WebcBitmapConverter
{
	protected:
		virtual void noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		virtual void orderedDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		virtual void fsDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		
		const unsigned char * mpPalette;
		unsigned short *      mpTranslationTable;
		int                   miTransparentIndex;
		unsigned short        muTransparentRGB;
		int                   mbTransparentDefined;
		
	public:
		indexedTo555();
		indexedTo555(const unsigned char * palette);
		indexedTo555(const unsigned char * palette, int transparentIndex);
		virtual ~indexedTo555();
		
		void setPalette(const unsigned char * p) { mpPalette = p; }
		void setTranslate(unsigned short * t) { mpTranslationTable = t; }
		void setTransparentIndex(int t) { miTransparentIndex = t; mbTransparentDefined = 1; }
		virtual unsigned long getTransparentColor (void) { return muTransparentRGB; }
};

#endif // __8TO555_HPP__
