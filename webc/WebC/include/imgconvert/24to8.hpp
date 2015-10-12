#ifndef __24TO8_HPP__
#define __24TO8_HPP__

#include "imgconv.hpp"
#include "webc.h"

class RGB24ToIndexed : public WebcBitmapConverter
{
	protected:
		virtual void noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		virtual void orderedDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		virtual void fsDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height);
		HTMLGraphicsContext** ctx;

	public:
		RGB24ToIndexed();
		virtual ~RGB24ToIndexed();

		void setGraphicsContext(HTMLGraphicsContext** c) { ctx = c; }
};

#endif // __24TO8_HPP__
