#ifndef __24TO32_HPP__
#define __24TO32_HPP__

#include "imgconv.hpp"

class RGB24ToRGBA32 : public WebcBitmapConverter
{
	protected:
		virtual void noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		virtual void orderedDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		virtual void fsDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		
	public:
		RGB24ToRGBA32();
		virtual ~RGB24ToRGBA32();
};

class RGB24ToBGRA32 : public WebcBitmapConverter
{
	protected:
		virtual void noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		virtual void orderedDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		virtual void fsDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		
	public:
		RGB24ToBGRA32();
		virtual ~RGB24ToBGRA32();
};

#endif // __24TO32_HPP__
