#ifndef _IMGCONV_
#define _IMGCONV_

class WebGraphics;

#include "webcinline.h"

extern const int giDitherMatrix[4][4];

enum WebcImageDitherMode
{
	WEBIMAGE_NO_DITHERING,
	WEBIMAGE_ORDERED,
	WEBIMAGE_ERROR_DIFFUSION
};

class WebcBitmapConverter
{
	protected:
		virtual void noDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height) = 0;
		virtual void orderedDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height) = 0;
		virtual void fsDither(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height) = 0;
		
	public:
		WebcBitmapConverter();
		virtual ~WebcBitmapConverter();

		virtual void go(unsigned char * dest, unsigned char * source, unsigned int width, unsigned int height, WebcImageDitherMode dm);			
		virtual unsigned long getTransparentColor (void) { return 0; }
};

class FSDitherChannel 
{
	private:
		int * error;
		int * nextLine;
		int width;
		int pos;
	public:
		FSDitherChannel();
		~FSDitherChannel();
		void init(int w);
		char getError(void);
		void addError(char e);
		//WEBC_INLINE void addErrorFirstColumn(char e);
		//WEBC_INLINE void addErrorLastColumn(char e);
};

#endif
