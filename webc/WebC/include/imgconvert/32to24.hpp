#ifndef __32TO24_HPP__
#define __32TO24_HPP__

#include "imgconv.hpp"

class RGB32AToRGB24 : public WebcBitmapConverter
{
	protected:
		virtual void noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		virtual void orderedDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		virtual void fsDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		
	public:
		RGB32AToRGB24();
		virtual ~RGB32AToRGB24();
};

class RGBA32ToBGR24 : public WebcBitmapConverter
{
	protected:
		virtual void noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		virtual void orderedDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		virtual void fsDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		
	public:
		RGBA32ToBGR24();
		virtual ~RGBA32ToBGR24();
};

#endif // __32TO24_HPP__
