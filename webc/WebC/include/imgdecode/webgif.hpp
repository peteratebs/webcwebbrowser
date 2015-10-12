#ifndef _WEBGIF_
#define _WEBGIF_

#include "webimage.hpp"

#define GIF_HEADER_SIZE            13

#define GIF_FLAG_GLOBAL_PALETTE    0x80
#define GIF_FLAG_SORT              0x08
#define GIF_FLAG_LOCAL_PALETTE     0x80
#define GIF_FLAG_INTERLACE         0x40

#define GIF_NO_DISPOSAL            0
#define GIF_DO_NOT_DISPOSE         1	
#define GIF_RESTORE_TO_BACKGROUND  2
#define GIF_RESTORE_TO_PREVIOUS    3

class WebGraphics;
class WebcImage;

struct WebGIFInfo
{
	short wOffsetX, wOffsetY;
	short wWidth, wHeight;
	short wDelay;
	char bDisposal;
};

struct WebGIFFrame
{
	short offsetX, offsetY;
	short width, height;
	short delay;
	char flags;
	WEBC_UCHAR8 * data;
	WEBC_UCHAR8 * palette;
	WebGIFFrame * next;
	short transparent;
	char disposal;
};

enum GIFExtensionType
{
	GIF_EXT_COMMENT,
	GIF_EXT_CONTROL,
	GIF_EXT_TEXT,
	GIF_EXT_APPLICATION,
	GIF_EXT_NETSCAPE_2_0,
	GIF_EXT_UNKNOWN
};

struct WebGIFExtension
{
	GIFExtensionType type;
	char disposal;
	short transparent;
	short delay;
};


class WebcGIFDecoder : public WebcImageDecoder
{
protected:
	WEBC_UCHAR8 muCodeSize;
	WEBC_UCHAR8 muBitsPerCode;
	unsigned int muSuffix;
	int miBitsInSuffix;
	WEBC_UINT16 muBytesLeftInBlock;

	WEBC_UINT16 muWidth, muHeight;
	char mcHeaderFlags;
	char mcBackgroundColorIndex;
	char mcAspectRatio;
	WEBC_UINT32 muPaletteSize;
	WEBC_UCHAR8 * mpPalette;                // global palette

	int miNumGIF;
	unsigned short miLoopCount;

	WebGIFFrame * mpFirstImage;
	WebGIFFrame * mpFrameInProgress;

	void readHeader(void);
	void readPalette(WEBC_UCHAR8 **ppal, WEBC_UINT32 size);
	WebGIFFrame * readImage(void);
	void readImageData(WEBC_UCHAR8 * d, WEBC_UINT16 width, WEBC_UINT16 height, char interlaced, WEBC_UINT32 size);
	WebGIFExtension readExtension(void);

	WEBC_UINT16 getNextCode(void);

	void eatRemaining(void);
	void cleanUp(void);

	WEBC_UCHAR8 * mpStack, *mpAppendStuff;
	short *mpiPrefixStuff;

public:
	WebcGIFDecoder();
	virtual ~WebcGIFDecoder();

	virtual WebcImage * go(WEBC_CHAR *bitmapKey);
	WebGIFInfo * getGIFInfo(void);
	virtual int numBitmaps(void) { return miNumGIF; }
	virtual unsigned short loopCount(void) { return miLoopCount; }

};


#endif
