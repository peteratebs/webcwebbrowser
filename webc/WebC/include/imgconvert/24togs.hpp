#ifndef __24TOGS_HPP__
#define __24TOGS_HPP__

#include "imgconv.hpp"

/* RGB24 and indexed to greyscale can only convert to 1,2,4 or 8 bit
   greyscale images.  The leftmost pixel is stored in the most 
   significant bits.  No crazy alignment issues are taken care of,
   like making sure lines don't cross byte boundaries. */

class RGB24ToGreyscale : public WebcBitmapConverter
{
	protected:
		virtual void noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		virtual void orderedDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		virtual void fsDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		char miBits;
		
	public:
		RGB24ToGreyscale();
		~RGB24ToGreyscale();
		void setBitDepth(char c) { miBits = c; };
};

#endif // __24TOGS_HPP__
