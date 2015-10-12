#ifndef __24TO565_HPP__
#define __24TO565_HPP__

#include "imgconv.hpp"

class RGB24To565 : public WebcBitmapConverter
{
	protected:
		virtual void noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		virtual void orderedDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		virtual void fsDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		
	public:
		RGB24To565();
		virtual ~RGB24To565();
};

#endif // __24TO565_HPP__
