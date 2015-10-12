#ifndef __24TO555_HPP__
#define __24TO555_HPP__

#include "imgconv.hpp"

class RGB24To555 : public WebcBitmapConverter
{
	protected:
		virtual void noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		virtual void orderedDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		virtual void fsDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		
	public:
		RGB24To555();
		virtual ~RGB24To555();
};

#endif // __24TO555_HPP__
