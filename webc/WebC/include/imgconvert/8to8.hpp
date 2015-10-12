#ifndef __8TO8_HPP__
#define __8TO8_HPP__

#include "imgconv.hpp"

class indexedToIndexed : public WebcBitmapConverter
{
	protected:
		virtual void noDither(unsigned char* dest, unsigned char* source, unsigned int width, unsigned int height);
		virtual void orderedDither(unsigned char* dest, unsigned char* source, unsigned int width, unsigned int height);
		virtual void fsDither(unsigned char* dest, unsigned char* source, unsigned int width, unsigned int height);

		HTMLGraphicsContext** mpGraphics;
		WEBC_UINT8*           mpPalette;
		WEBC_UINT8*           mpTranslationTable;
		int                   mSrcTransparentIndex;
		int                   mDstTransparentIndex;

	public:
		indexedToIndexed(void);
		virtual ~indexedToIndexed(void);

		void setPalette(unsigned char* p) { mpPalette = p; }
		void setGraphicsContext(HTMLGraphicsContext** ctx) { mpGraphics = ctx; }
		void setTranslate(unsigned char* t) { mpTranslationTable = t; }
		void setTransparentIndex(int dst, int src) { mSrcTransparentIndex = src; mDstTransparentIndex = dst; }
		virtual unsigned long TransparentColor (void) { return mDstTransparentIndex; }
};

#endif // __8TO8_HPP__
