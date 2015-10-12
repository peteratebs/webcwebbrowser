#ifndef __24TO24_HPP__
#define __24TO24_HPP__

#include "imgconv.hpp"

class RGB24ToRGB24 : public WebcBitmapConverter
{
	protected:
		virtual void noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		virtual void orderedDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		virtual void fsDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		
	public:
		RGB24ToRGB24();
		virtual ~RGB24ToRGB24();
};

class RGB24ToBGR24 : public WebcBitmapConverter
{
	protected:
		virtual void noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		virtual void orderedDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		virtual void fsDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		
	public:
		RGB24ToBGR24();
		virtual ~RGB24ToBGR24();
};

#endif // __24TO24_HPP__
