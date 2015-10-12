#ifndef _WEBIMAGE_
#define _WEBIMAGE_

typedef int(*ImageStreamRead)(char*,long,void*);

#if defined (__vxworks__)
#include <vxWorks.h>
#endif /* defined (__vxworks__)  */

#include <setjmp.h>
#include "imgconv.hpp"
#include "webctypes.h"
#include "wgtypes.hpp"
#include "imgconv.hpp"
#include "webcstr.h"

#define WEBIMAGE_BUFFER_SIZE      4096
#define WEBIMAGE_IS_GREYSCALE(x)  (x==WEBIMAGE_GREYSCALE1||x==WEBIMAGE_GREYSCALE2||x==WEBIMAGE_GREYSCALE4||x==WEBIMAGE_GREYSCALE8)

// Define this symbol for devices where index 0 is white
//#define WEBIMAGE_INVERT_GREYSCALE

#ifdef WEBIMAGE_INVERT_GREYSCALE
 #define RGBTOGREY(r,g,b) (255 - ((((unsigned long)(r)) + (((unsigned long)(r)) << 2) + \
                                  (((unsigned long)(g)) << 3) + ((unsigned long)(b)) + \
                                  (((unsigned long)(b)) << 1)) >> 4))
#else
 #define RGBTOGREY(r,g,b)        ((((unsigned long)(r)) + (((unsigned long)(r)) << 2) + \
                                  (((unsigned long)(g)) << 3) + ((unsigned long)(b)) + \
                                  (((unsigned long)(b)) << 1)) >> 4)
// grayLevel = (red * 5 + green * 8 + blue * 3) / 16
#endif


class WebGraphics;
class WebcImage;

enum WebcImageState
{
	WEBIMAGE_OKAY,
	WEBIMAGE_READ_ERROR,
	WEBIMAGE_FORMAT_ERROR,
	WEBIMAGE_MALLOC_ERROR,
	WEBIMAGE_PARTIAL_ERROR,
	WEBIMAGE_DONE
};

enum WebcImageColorDepth
{
	WEBIMAGE_INDEXED,
	WEBIMAGE_565,
	WEBIMAGE_555,
	WEBIMAGE_24,
	WEBIMAGE_32,
	WEBIMAGE_GREYSCALE1,
	WEBIMAGE_GREYSCALE2,
	WEBIMAGE_GREYSCALE4,
	WEBIMAGE_GREYSCALE8
};

	
class WebcImageDecoder
{
protected:

	WEBC_UCHAR8 * mpReadPtr;
	WEBC_UCHAR8 * mpDataBuffer;
	WEBC_UINT32   muDataBytesAvailable;

	WebcImageColorDepth meColorDepth;
	WebcImageDitherMode meDitherMode;

	WebcImageState meState;

	ImageStreamRead mfCallback;
	void * mpCallbackContext;

	long readAtLeast(WEBC_UCHAR8 * data, long maxSize, long num);

	WebGraphics * mpGraphicsContext;

	jmp_buf mLongJump;
	char mcLongJumpValid;

public:

	WebcImageDecoder();
	WebcImageDecoder(ImageStreamRead cb, void * context);
	virtual ~WebcImageDecoder();

	WEBC_UCHAR8 * getPointer() { return mpReadPtr; }           // These three methods really should be private
	void movePointer(WEBC_UINT32 s);
	WEBC_UINT32 bytesAvailable (void) { return muDataBytesAvailable; }
	
	WEBC_UCHAR8 getNext(void);
	WEBC_UCHAR8 peekNext(void);

	void fillBuffer(int bytes);

	void setReadCallback(ImageStreamRead cb, void * context);
	void setColorDepth(WebcImageColorDepth d);
	void setDitherMode(WebcImageDitherMode d);
	void setGraphics(WebGraphics * g);

	virtual WebcImage * go(WEBC_CHAR *bitmapKey) = 0;
	virtual int numBitmaps(void) { return 1; }
	virtual unsigned short loopCount(void) { return 1; } // 0 means loop infinitely, 1 means no loop

	WebcImageState getState(void) { return meState; }
	void error(WebcImageState s);	
};

#endif
