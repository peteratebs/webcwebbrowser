//
// WEBGIF.CPP - Methods for class WebcGIFDecoder
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//  [tbd]
//

/*****************************************************************************/
// Header files
/*****************************************************************************/

#include "webcfg.h"

#if (WEBC_SUPPORT_GIFS)

#include "webgif.hpp"
#include "stdio.h"
#include "webgraph.hpp"
#include "webcbmp.hpp"
#include "webcassert.h"
#include "rtptime.h"
#include "webcmem.h"


/*****************************************************************************/
// Defines
/*****************************************************************************/

#define GIF_CODE_ERROR   0xffff
#define WEBC_GIF_MAX_FRAME_DEPTH    50
#if (WEBC_GIF_MAX_FRAME_DEPTH > 127)
 #error WEBC_GIF_MAX_FRAME_DEPTH is too large for char depth
#endif

/*****************************************************************************/
// Data types/structures
/*****************************************************************************/

struct WebcGIFImageLayers
{
	char depth;
	short frame[WEBC_GIF_MAX_FRAME_DEPTH];
};

class WebcGIFFrame : public WebcImageFrame
{
public:
	int  miFrameIndex;
	int  miLoopCount;
	int  miNumFrames;
	long miNextFrameTime;
	int  mbIsLastFrame;
	WebGIFInfo *mpGifInfo;

	WebcGIFFrame (WebGIFInfo *pGifInfo, int iNumFrames);
	~WebcGIFFrame (void);

	virtual void Advance (void);
	virtual long DelayMsec (void);
	virtual int IsLastFrame (void);
};

class WebcGIFImage : public WebcImage
{
protected:
	WebGraphics *mpGC;
	WebBitmap *mpBmpList;
	WebBitmap *mpScaledBmps;
	int miBmpCount;
	unsigned short miLoopCount;
	WebGIFInfo *mpGifInfo;
	DISPLAY_INT miWidth;
	DISPLAY_INT miHeight;

	void AddLayer(WebcGIFImageLayers *layers, short frame);
	void ResetLayers(WebcGIFImageLayers *layers);
	void RemoveTopLayer(WebcGIFImageLayers *layers);
	void DrawBitmap(DISPLAY_INT x, DISPLAY_INT y, DISPLAY_INT w, DISPLAY_INT h, int index, WebGraphics *gc);

public:
	WebcGIFImage (WebGraphics *gc, const WEBC_CHAR *pKey, WebBitmap *pBmpList, int iBmpCount, 
	              int iLoopCount, WebGIFInfo *pGifInfo);
	virtual ~WebcGIFImage (void);

	virtual size_t Size    (void);

	virtual DISPLAY_INT Width  (void);
	virtual DISPLAY_INT Height (void);

	virtual WebcImageFrame * DrawFrame    (WebcImageFrame *frame, WebRect *rect, WebGraphics *gc);
	virtual WebcImageFrame * DrawTiled    (WebcImageFrame *frame, WebRect *rect, 
	                                       DISPLAY_INT xOffset, DISPLAY_INT yOffset, WebGraphics *gc);
	virtual void    	     ReleaseFrame (WebcImageFrame *frame);
};



/*****************************************************************************/
// Data
/*****************************************************************************/

const WEBC_UINT16 codeMask[] = 
	{
		0x0, 0x1, 0x3, 0x7, 0xF, 0x1F, 0x3F, 0x7F, 0xFF,
		0x1FF, 0x3FF, 0x7FF, 0xFFF, 0x1FFF, 0x3FFF, 0x7FFF, 0xFFFF 
	};
	
	
/*****************************************************************************/
// WebGIF methods
/*****************************************************************************/

WebcGIFDecoder::WebcGIFDecoder()
{
	miBitsInSuffix = 0;
	muSuffix = 0;
	mpFirstImage = WEBC_NULL;
	mpFrameInProgress = WEBC_NULL;
	mpPalette = WEBC_NULL;
	mpAppendStuff = 0;
	mpiPrefixStuff = 0;
	mpStack = 0;	
	miLoopCount = 1;
}


WebcGIFDecoder::~WebcGIFDecoder()
{
	cleanUp();
	
	WebGIFFrame *nextImage = 0;
	for (WebGIFFrame * i = mpFirstImage; i != WEBC_NULL; i=nextImage)
	{
		if (i->data)
		{
			WEBC_DELETE(i->data);
		}
		
		if (i->palette)
		{
			WEBC_DELETE(i->palette);
		}
		
		nextImage = i->next;
		WEBC_DELETE(i);
	}
	WEBC_DELETE(mpPalette);	
	
	if (mpFrameInProgress)
	{
		if (mpFrameInProgress->data)
		{
			WEBC_DELETE(mpFrameInProgress->data);
		}

		if (mpFrameInProgress->palette)
		{
			WEBC_DELETE(mpFrameInProgress->palette);
		}
		
		WEBC_DELETE(mpFrameInProgress);
	}
}


WebcImage * WebcGIFDecoder::go(WEBC_CHAR *bitmapKey)
{
	if (!mpDataBuffer)
	{
		meState = WEBIMAGE_MALLOC_ERROR;
		return (0);
	}
	
	WebcImage * ret = WEBC_NULL;
	WebBitmap * bmpList = WEBC_NULL;
	miNumGIF = 0;
	if (setjmp(mLongJump))
	{
		return ret;
	}
	mcLongJumpValid = 1;

	readHeader();
	if (mcHeaderFlags & GIF_FLAG_GLOBAL_PALETTE)
	{
		readPalette(&mpPalette, muPaletteSize);
	}
	WebGIFExtension extensionData;
	WebGIFExtension controlData;
	controlData.delay = -1;
	controlData.transparent = -1;

	WebGIFFrame * pImage, *last;
	last = WEBC_NULL;
	while (meState == WEBIMAGE_OKAY)
	{
			
		switch (getNext())
		{
		case 0x2C:         // image data follows
			pImage = readImage();
			pImage->next = WEBC_NULL;
			if (last)
			{
				last->next = pImage;
			}
			if (!mpFirstImage)
			{
				mpFirstImage = pImage;
			}

			pImage->transparent = controlData.transparent;
			pImage->delay = controlData.delay;
			pImage->disposal = controlData.disposal;

			controlData.transparent = -1;
			controlData.delay = -1;
			controlData.disposal = 0;

			last = pImage;
			miNumGIF++;				
			break;

		case 0x21:         // graphics extension follows
			extensionData = readExtension();
			if (extensionData.type == GIF_EXT_CONTROL)
			{
				controlData = extensionData;
			}
			break;

		case 0x3B:
			meState = WEBIMAGE_DONE;
			break;

		case 0x0:
			break;        // dumb end of block terminator?

		default:
			break;
			//error(WEBIMAGE_FORMAT_ERROR);
		}
	}
	
	WEBC_NEW_SIZED(bmpList, WebBitmap[miNumGIF],(sizeof(WebBitmap)*miNumGIF));
	if (!bmpList)
	{
		error(WEBIMAGE_MALLOC_ERROR);
	}
	
	pImage = mpFirstImage;
	WEBC_UINT8 *pal = mpPalette;
	WEBC_UINT32 palSize = muPaletteSize;

	for (int t = 0; t < miNumGIF; t++)
	{		
		if (pImage->flags & GIF_FLAG_LOCAL_PALETTE)
		{
			pal = pImage->palette;
			palSize = 1 << ((pImage->flags & 0x7) + 1);
		}
		else if (mcHeaderFlags & GIF_FLAG_GLOBAL_PALETTE)
		{			
			pal = mpPalette;
			palSize = muPaletteSize;
		}

		if (pal)
		{
			WebGraphBitmapInfo info = {
					pImage->width,               // iWidth
					pImage->height,              // iHeight;
					pImage->width,               // iBytesPerRow;
					WB_PIXEL_FORMAT_INDEX_8,     // pixelFormat;
					(pImage->transparent >= 0),  // bHasTransparentColor;
					pImage->transparent,         // iTransparentIndex;
					0,                           // uTransparentRed;
					0,                           // uTransparentGreen;
					0,                           // uTransparentBlue;
					palSize,                     // iPaletteSize;
					pal,                         // pPalette;
					pImage->data                 // pData;
				};			
			
			bmpList[t] = mpGraphicsContext->CreateWebBitmap(&info);
		}
		else
		{
			bmpList[t] = 0;
		}

		// if bitmap creation fails, make sure to clean up all preceeding
		//  bitmaps before returning
		if (!bmpList[t])
		{
			for (t = t-1; t >= 0; t--)
			{
				mpGraphicsContext->DestroyWebBitmap(bmpList[t]);
			}
			
			WEBC_DELETE(bmpList);
			bmpList = WEBC_NULL;
			
			error(WEBIMAGE_MALLOC_ERROR);
		}

		WEBC_DELETE(pImage->data);
		pImage->data = 0;
					
		pImage = pImage->next;
	}

	WEBC_NEW_VERBOSE(ret, WebcGIFImage(mpGraphicsContext, bitmapKey, bmpList, miNumGIF, miLoopCount, getGIFInfo()),"WebcGIFImage");

	if (!ret)
	{
		int t;
		for (t = 0; t < miNumGIF; t++)
		{
			mpGraphicsContext->DestroyWebBitmap(bmpList[t]);
		}
		
		WEBC_DELETE(bmpList);
		bmpList = WEBC_NULL;
	}
	else
	{
		WebcImageState s = getState();
		
		ret->SetState((s == WEBIMAGE_DONE)? WEBIMAGE_OKAY : s);
	}
					
	return ret;
}

WebGIFInfo * WebcGIFDecoder::getGIFInfo(void)
{
	WebGIFInfo * ret;
	WEBC_NEW_VERBOSE(ret, WebGIFInfo[miNumGIF], "WebGIFInfo");
	if (ret)
	{
		WebGIFFrame * i = mpFirstImage;
		
		for (int t = 0; t < miNumGIF; t++)
		{
			ret[t].wOffsetX = i->offsetX;
			ret[t].wOffsetY = i->offsetY;
			ret[t].wWidth = i->width;
			ret[t].wHeight = i->height;
			ret[t].wDelay = i->delay;
			ret[t].bDisposal = i->disposal;
			i = i->next;
		}
	}
	
	return (ret);
}

WebGIFExtension WebcGIFDecoder::readExtension(void)
{
	unsigned char type = getNext();
	WebGIFExtension ret;
	int size;
	WEBC_UINT16 high, low;
	char transValid;
	unsigned char flags;
	switch (type)
	{
	case 0xF9:                 // control
		ret.type = GIF_EXT_CONTROL;
		if (getNext() != 4)
		{
			error(WEBIMAGE_FORMAT_ERROR);
		}
		flags = getNext();
		transValid = flags & 1;    // eat flags
		low = getNext(); high = getNext();
		ret.delay = (high << 8) + low;
		if (ret.delay == 0)
		{
			ret.delay = 1;			// ugh
		}
		ret.disposal = (flags >> 2) & 7;
		if (transValid)
		{
			ret.transparent = getNext(); // eat transparent index
		}
		else
		{
			ret.transparent = -1;
			getNext();
			
		}
		
		if (getNext() != 0x0)
		{
			error(WEBIMAGE_FORMAT_ERROR);
		}
		break;

	case 0xFF: // Application Extension Label
		// initialize type to unknown
		ret.type = GIF_EXT_UNKNOWN;
		
		size = getNext();
		
		if (size == 11)
		{
			char appName[12];
			int n;

			// get the application name
			for (n=0; n<11; n++)
			{
				appName[n] = getNext();
			}
			appName[n] = '\0';
			
			// get the data size
			size = getNext();				
			
			// try to recognize the extension
			if (!tc_strcmp(appName, "NETSCAPE2.0") && (size == 3))
			{
				// this is the Netscape 2.0 looping extension for looping
				//  GIFs

				WEBC_UINT8 reserved, loLoop, hiLoop;
				ret.type = GIF_EXT_NETSCAPE_2_0;
				
				reserved = getNext(); // should be 1
				loLoop   = getNext();
				hiLoop   = getNext();
				size     = getNext(); // should be 0
				
				miLoopCount = (hiLoop << 8) | loLoop;
				if (miLoopCount > 0)
				{
					miLoopCount++;
				}
			}
		}
				
		// read off the remaining data
		while (size)
		{
			while (size)
			{
				getNext();
				size--;
			}
			size = getNext();
		} 
		break;
	
	default:
		ret.type = GIF_EXT_UNKNOWN;
		size = getNext();
		while (size--)
		{
			getNext();
		}
		size = getNext();
		while (size)
		{
			while (size--)
			{
				getNext();
			}
			size = getNext();
		}
	}
	
	return ret;
	
}


WebGIFFrame * WebcGIFDecoder::readImage(void)
{
	WEBC_NEW_VERBOSE(mpFrameInProgress, WebGIFFrame,"WebGIFFrame");
	if (!mpFrameInProgress)
	{
		error(WEBIMAGE_MALLOC_ERROR);
	}

	mpFrameInProgress->palette = WEBC_NULL;
	mpFrameInProgress->data = WEBC_NULL;

	WEBC_UCHAR8 low, high;
	
	low = getNext(); high = getNext();
	mpFrameInProgress->offsetX = low + (high << 8);
	
	low = getNext(); high = getNext();
	mpFrameInProgress->offsetY = low + (high << 8);
	
	low = getNext(); high = getNext();
	mpFrameInProgress->width = low + (high << 8);
	
	low = getNext(); high = getNext();
	mpFrameInProgress->height = low + (high << 8);
	
	mpFrameInProgress->flags = getNext();

	if (mpFrameInProgress->flags & GIF_FLAG_LOCAL_PALETTE)
	{
		WEBC_UINT16 size = 1 << ((mpFrameInProgress->flags & 0x7) + 1);
		readPalette(&mpFrameInProgress->palette, size);
	}

	if (mpFrameInProgress->width == 0 || mpFrameInProgress->height == 0)
	{
		error(WEBIMAGE_FORMAT_ERROR);
	}

	WEBC_NEW_SIZED(mpFrameInProgress->data, WEBC_UCHAR8[mpFrameInProgress->width * mpFrameInProgress->height],(mpFrameInProgress->width * mpFrameInProgress->height));
	if (!mpFrameInProgress->data)
	{
		error(WEBIMAGE_MALLOC_ERROR);
	}

	readImageData(mpFrameInProgress->data, 
	              mpFrameInProgress->width, 
	              mpFrameInProgress->height, 
	              mpFrameInProgress->flags & GIF_FLAG_INTERLACE, 
	              mpFrameInProgress->width * mpFrameInProgress->height);
	
	WebGIFFrame *result = mpFrameInProgress;
	mpFrameInProgress = WEBC_NULL;
	
	return (result);
}


void WebcGIFDecoder::readImageData(WEBC_UCHAR8 * data, WEBC_UINT16 width, WEBC_UINT16 height, char interlaced, WEBC_UINT32 size)
{
	
#define MAX_CODES    4096       // should never exceed 12 bits/code
#define STACK_SIZE   MAX_CODES+10

	const int pitch[] = 
		{
			8, 8, 4, 2
		};
	const int startRow[] = 
		{
			0, 4, 2, 1
		};
	
	WEBC_UCHAR8 * d;

	WEBC_NEW_SIZED(mpStack, WEBC_UCHAR8[STACK_SIZE],STACK_SIZE);
	WEBC_NEW_SIZED(mpAppendStuff, WEBC_UCHAR8[MAX_CODES+1],MAX_CODES+1);
	WEBC_NEW_SIZED(mpiPrefixStuff, short[(MAX_CODES+1)/** sizeof(*prefixStuff)*/],((MAX_CODES+1)*sizeof(short)) );

	if (!mpStack || !mpAppendStuff || !mpiPrefixStuff)
	{
		cleanUp();
		error(WEBIMAGE_MALLOC_ERROR);
	}
	
	muCodeSize = getNext();
	if (muCodeSize > 10)
	{
		cleanUp();
		error(WEBIMAGE_FORMAT_ERROR);
	}
	muBitsPerCode = muCodeSize + 1;
	muSuffix = 0;
	miBitsInSuffix = 0;

	WEBC_UINT16 clearCode = 1 << muCodeSize;
	WEBC_UINT16 nextCode = clearCode + 2;
	
	WEBC_UINT16 code = muSuffix;
	WEBC_UINT16 oldCode;
	WEBC_UINT16 ch;
	WEBC_UINT32 pixelsLeft = width * height;

	oldCode = ch = 0;

	muBytesLeftInBlock = getNext();	
	
	if (!muBytesLeftInBlock)
	{
		cleanUp();
		error(WEBIMAGE_FORMAT_ERROR);
	}
	
	char pass = 0;                    // pass number for interlaced images
	int xPos = 0; 
	int yPos = startRow[pass]; 

	d = data + yPos * width;

	while (1)
	{
		code = getNextCode();
		if (code == GIF_CODE_ERROR)
		{
			break;
		}
		if (code == clearCode)
		{
			// clear the translation table
			muBitsPerCode = muCodeSize + 1;
			nextCode = clearCode + 2;
			while ((code = getNextCode()) == clearCode) {;}
			
			WEBC_ASSERT(code != GIF_CODE_ERROR);
			if (code == clearCode + 1)
			{
				break;
			}
			if (code >= nextCode)
			{
				code = 0;
			}
			oldCode = ch = code;
			WEBC_ASSERT(d < (data + size));
			*d++ = (unsigned char)code;
			pixelsLeft--;
			if (pixelsLeft == 0)
			{
				eatRemaining();
				cleanUp();
				return;
			}
			if (interlaced && ++xPos > width-1)
			{
				yPos += pitch[pass];
				while ((yPos > height-1) && (pass < 3))
				{
					yPos = startRow[++pass];
				}
				xPos = 0;
				d = data + yPos * width;
			}
			

		}
		else if (code == clearCode + 1)
		{
			break;
		}
		else
		{
			WEBC_UCHAR8 *b = mpStack;
			WEBC_UINT16 saveCode = code;
			
			if (code >= nextCode)
			{
				code = oldCode;
				*b++ = (unsigned char)ch;
			}
			while ( code >= clearCode + 2)
			{
				
				*b++ = mpAppendStuff[code];
				code = mpiPrefixStuff[code];
			
				if (code >= MAX_CODES || b > mpStack + STACK_SIZE - 1)
				{
//					eatRemaining();
					meState = WEBIMAGE_PARTIAL_ERROR;
					cleanUp();
					return;
				}
			
			}
			*b++ = (WEBC_UCHAR8)code;
			if (nextCode < (1 << muBitsPerCode))
			{
				ch = code;
				mpAppendStuff[nextCode] = (unsigned char)ch;					
				mpiPrefixStuff[nextCode++] = oldCode;
				oldCode = saveCode;
			}
			if (nextCode >= (1 << muBitsPerCode) && muBitsPerCode < 12)
			{
				muBitsPerCode++;
			}
			
			WEBC_ASSERT((b - mpStack) < STACK_SIZE);
			WEBC_ASSERT(yPos < height);
			WEBC_ASSERT(d < (data + size));

			while (b > mpStack) 
			{
				b--;
				*d++ = *b;
				pixelsLeft--;
				if (pixelsLeft == 0)
				{
					eatRemaining();
					cleanUp();
					return;
				}
				if (interlaced && ++xPos > width-1)
				{
					yPos += pitch[pass];
					while ((yPos > height-1) && (pass < 3))
					{
						yPos = startRow[++pass];
					}
					xPos = 0;
					d = data + yPos * width;
				}
			}
			
			WEBC_ASSERT(d <= (data + size) || yPos >= height);
		}
	}
	eatRemaining();
	cleanUp();
	return;
}

void WebcGIFDecoder::cleanUp(void)
{
	if (mpAppendStuff)
	{
		WEBC_DELETE(mpAppendStuff);
		mpAppendStuff = 0;
	}
	
	if (mpiPrefixStuff)
	{
		WEBC_DELETE(mpiPrefixStuff);
		mpiPrefixStuff = 0;
	}
	
	if (mpStack)
	{
		WEBC_DELETE(mpStack);
		mpStack = 0;
	}
}

void WebcGIFDecoder::eatRemaining(void)
{
	while (muBytesLeftInBlock && meState == WEBIMAGE_OKAY)
	{
		movePointer(muBytesLeftInBlock);
		muBytesLeftInBlock = getNext();
	}
}

WEBC_UINT16 WebcGIFDecoder::getNextCode(void)
{
	WEBC_UINT32 code = muSuffix;
	int bitsRead = 0;
	while (bitsRead + miBitsInSuffix < muBitsPerCode)
	{
		if (muBytesLeftInBlock == 0) 
		{
			muBytesLeftInBlock = getNext();
			//fillBuffer(muBytesLeftInBlock * 2);
			if (muBytesLeftInBlock == 0)
			{
				return GIF_CODE_ERROR;
			}
		}
		/*if (pos > lastValid)
		{
			fillBuffer(512); //two blocks worth
			if (pos > lastValid)
			{
				meState = WEBIMAGE_PARTIAL_ERROR;
				return GIF_CODE_ERROR;
			}
		}*/
		code += (WEBC_UINT32)(getNext()) << (bitsRead + miBitsInSuffix);
		muBytesLeftInBlock--;
		bitsRead += 8;
	}
	muSuffix = code >> muBitsPerCode;
	code &= codeMask[muBitsPerCode];
	miBitsInSuffix = bitsRead - muBitsPerCode + miBitsInSuffix;
	
	WEBC_ASSERT(code <= 4096);

	return (WEBC_UINT16) code;
}


void WebcGIFDecoder::readPalette(WEBC_UCHAR8 **ppal, WEBC_UINT32 size)
{
	if (meState != WEBIMAGE_OKAY)
	{
		*ppal = WEBC_NULL;
		return;
	}
	
	WEBC_NEW_SIZED(*ppal, WEBC_UCHAR8[768],768);

	if (!(*ppal))
	{
		error(WEBIMAGE_MALLOC_ERROR);
	}
	
	WEBC_UCHAR8 * temp = *ppal;
	for (WEBC_UINT32 t = 0; t < size * 3; t ++)
	{
		*temp++ = getNext();
	}
}


void WebcGIFDecoder::readHeader(void)
{
	if (meState != WEBIMAGE_OKAY)
	{
		return;
	}
	if ( (getNext() != 'G') || (getNext() != 'I') || (getNext() != 'F'))
	{
		error(WEBIMAGE_FORMAT_ERROR);
		return;
	}
	getNext(); getNext(); getNext(); //version
	
	WEBC_UCHAR8 low, high;
	low = getNext(); high = getNext();
	muWidth = low + (high << 8);
	
	low = getNext(); high = getNext();
	muHeight = low + (high << 8);
	
	mcHeaderFlags = getNext();
	muPaletteSize = 1 << ((mcHeaderFlags & 0x7) + 1);

	mcBackgroundColorIndex = getNext();
	mcAspectRatio = getNext();
	
}


/*****************************************************************************/
// WebcGIFImage methods
/*****************************************************************************/

WebcGIFImage::WebcGIFImage (WebGraphics *gc, const WEBC_CHAR *pKey, WebBitmap *pBmpList, 
		int iBmpCount, int iLoopCount, WebGIFInfo *pGifInfo)
	: WebcImage(pKey)
{
	mpGC = gc;
	miWidth = 0;
	miHeight = 0;
	mpBmpList = pBmpList;
	mpGifInfo = pGifInfo;
	miBmpCount = iBmpCount;
	miLoopCount = iLoopCount;
	mpScaledBmps = 0;

	mpGC->Claim();	
	
	if (mpBmpList && mpBmpList[0])
	{
		DISPLAY_INT x, y, w, h;
		int n;
	
		for (n=0; n<miBmpCount; n++)
		{
			if (mpGifInfo)
			{
				x = mpGifInfo[n].wOffsetX;
				y = mpGifInfo[n].wOffsetY;
			}
			else
			{
				x = 0;
				y = 0;
			}
			
			w = WEB_BITMAP_WIDTH(mpBmpList[n]) + x;
			h = WEB_BITMAP_HEIGHT(mpBmpList[n]) + y;
			
			if (miWidth < w)
			{
				miWidth = w;
			}

			if (miHeight < h)
			{
				miHeight = h;
			}
		}
	}

}

WebcGIFImage::~WebcGIFImage (void)
{
int n;

	if (mpGifInfo)
	{
		WEBC_DELETE(mpGifInfo);
		mpGifInfo = 0;
	}
	
	if (mpBmpList)
	{
		for (n=0; n<miBmpCount; n++)
		{
			if (mpBmpList[n] != WEB_BITMAP_NULL)
			{
				mpGC->DestroyWebBitmap(mpBmpList[n]);
			}
		}
		WEBC_DELETE(mpBmpList);
		mpBmpList = 0;
	}
	
	if (mpScaledBmps)
	{
		for (n=0; n<miBmpCount; n++)
		{
			if (mpScaledBmps[n] != WEB_BITMAP_NULL)
			{
				mpGC->DestroyStretchedBitmap(mpScaledBmps[n], __FILE__, __LINE__);
			}
		}
		WEBC_FREE(mpScaledBmps);
		mpScaledBmps = 0;
	}

	mpGC->Release();
}

size_t WebcGIFImage::Size (void)
{
	return (miWidth * miHeight * miBmpCount * 4); // hack (worst case)
}

DISPLAY_INT WebcGIFImage::Width (void)
{
	return (miWidth);
}

DISPLAY_INT WebcGIFImage::Height (void)
{
	return (miHeight);
}

WebcImageFrame * WebcGIFImage::DrawFrame (WebcImageFrame *bmpFrame, WebRect *rect, WebGraphics *gc)
{
	DISPLAY_INT w = rect->Width(), h = rect->Height();
	WebcGIFFrame *frame = (WebcGIFFrame *) bmpFrame;

	if (mpGifInfo && (miBmpCount > 1))
	{
		WebcGIFImageLayers layers;
		int lastFrame, currentFrame;
		int n;
	
		if (frame)
		{
			currentFrame = frame->miFrameIndex;
		}
		else
		{
			currentFrame = 0;

			WEBC_NEW_VERBOSE(frame, WebcGIFFrame(mpGifInfo, miBmpCount),"WebcGIFFrame");
		}

		ResetLayers(&layers);
	
		for (n=0; n <= currentFrame; n++)
		{
			// calculate the last frame
			lastFrame = (n > 0) ? (n - 1) : (miBmpCount - 1);
		
			// dispose of the last frame	
			switch (mpGifInfo[lastFrame].bDisposal)
			{					
				case GIF_RESTORE_TO_BACKGROUND:
					ResetLayers(&layers);
					break;
					
				case GIF_RESTORE_TO_PREVIOUS:
					RemoveTopLayer(&layers);
					break;

				case GIF_NO_DISPOSAL:
				case GIF_DO_NOT_DISPOSE:
				default:
					break;
			}
	
			// add the current frame
			AddLayer(&layers, n);
		}
	
		DISPLAY_INT imageW = Width(), imageH = Height();
		for (n = 0; n < layers.depth; n++)
		{
			currentFrame = layers.frame[n];
			DrawBitmap (
				(mpGifInfo[currentFrame].wOffsetX * w) / imageW + rect->left, 
				(mpGifInfo[currentFrame].wOffsetY * h) / imageH + rect->top,
				(mpGifInfo[currentFrame].wWidth   * w) / imageW,
				(mpGifInfo[currentFrame].wHeight  * h) / imageH,
				currentFrame, gc );
		}
		
		if (frame)
		{
			if ((miLoopCount != 0) && 
			    (frame->miLoopCount == miLoopCount-1) && 
				(frame->miFrameIndex == miBmpCount-1))
			{
				frame->mbIsLastFrame = 1;
			}
			else
			{
				frame->mbIsLastFrame = 0;
			}
		}
	}
	else
	{
		int currentFrame = (frame)? frame->miFrameIndex : 0;
		DrawBitmap(rect->left, rect->top, w, h, EBSCLIP(currentFrame, 0, miBmpCount-1), gc);
	}	

	return (frame);
}

void WebcGIFImage::DrawBitmap(DISPLAY_INT x, DISPLAY_INT y, DISPLAY_INT w, DISPLAY_INT h, int index, WebGraphics *gc)
{
	if ((w <= 0) || (h <= 0))
	{
		return;
	}
	
	if ((w != WEB_BITMAP_WIDTH(mpBmpList[index])) ||
	    (h != WEB_BITMAP_HEIGHT(mpBmpList[index])))
	{	
		if (!mpScaledBmps)
		{
			mpScaledBmps = (WebBitmap *) WEBC_MALLOC_VERBOSE(sizeof(WebBitmap) * miBmpCount,"ScaledBitmap");
			if (mpScaledBmps)
			{
				for (int n=0; n<miBmpCount; n++)
				{
					mpScaledBmps[n] = WEB_BITMAP_NULL;
				}
			}
		}
		
		if (mpScaledBmps)
		{
			if ((mpScaledBmps[index] == WEB_BITMAP_NULL) || 
			    (w != WEB_BITMAP_WIDTH(mpScaledBmps[index])) ||
	    	    (h != WEB_BITMAP_HEIGHT(mpScaledBmps[index])))
	    	{
	    		if (mpScaledBmps[index])
	    		{
					gc->DestroyStretchedBitmap(mpScaledBmps[index], __FILE__, __LINE__);
	    		}
	    		
	    		mpScaledBmps[index] = gc->CreateStretchedBitmap(mpBmpList[index], w, h, __FILE__, __LINE__);
	    	}

			if (mpScaledBmps[index] != WEB_BITMAP_NULL)
			{
    			gc->Bitmap(x,y,mpScaledBmps[index]);
			}
		}
		else
		{
			gc->StretchBitmap(x,y,w,h,mpBmpList[index]);
		}
	}
	else
	{
		gc->Bitmap(x,y,mpBmpList[index]);
	}	
}

WebcImageFrame * WebcGIFImage::DrawTiled (WebcImageFrame *frame, WebRect *rect, 
		DISPLAY_INT xOffset, DISPLAY_INT yOffset, WebGraphics *gc)
{
	if (mpBmpList && mpBmpList[0])
	{
		gc->TileBitmap(rect, xOffset, yOffset, mpBmpList[0]);
	}
	return (0);
}

void WebcGIFImage::ReleaseFrame (WebcImageFrame *frame)
{
	WEBC_DELETE(frame);
}

void WebcGIFImage::AddLayer(WebcGIFImageLayers *layers, short frame)
{
	if (layers->depth == WEBC_GIF_MAX_FRAME_DEPTH)
	{
		for (int n=0; n<WEBC_GIF_MAX_FRAME_DEPTH-1; n++)
		{
			layers->frame[n] = layers->frame[n+1];
		}
		layers->depth--;
	}
	
	layers->frame[layers->depth++] = frame;
}

void WebcGIFImage::ResetLayers(WebcGIFImageLayers *layers)
{
	layers->depth = 0;
}

void WebcGIFImage::RemoveTopLayer(WebcGIFImageLayers *layers)
{
	layers->depth = (layers->depth > 0) ? (layers->depth - 1) : 0;
}


/*****************************************************************************/
// WebcGIFFrame methods
/*****************************************************************************/

WebcGIFFrame::WebcGIFFrame (WebGIFInfo *pGifInfo, int iNumFrames)
{
	miFrameIndex = 0;
	miLoopCount = 0;
	miNumFrames = iNumFrames;
	mbIsLastFrame = 0;
	mpGifInfo = pGifInfo;
	miNextFrameTime = rtp_get_system_msec() + mpGifInfo[miFrameIndex].wDelay * 10;
}

WebcGIFFrame::~WebcGIFFrame (void)
{
}

void WebcGIFFrame::Advance (void)
{
	miFrameIndex++;

	if (miFrameIndex >= miNumFrames)
	{
		miFrameIndex = 0;
		miLoopCount++;
	}

	if (mpGifInfo)
	{
		miNextFrameTime = rtp_get_system_msec() + mpGifInfo[miFrameIndex].wDelay * 10;
	}
}

long WebcGIFFrame::DelayMsec (void)
{
	long delay = (miNextFrameTime - rtp_get_system_msec());
	if (delay < 1)
	{
		delay = 1;
	}
	return (delay);
}

int WebcGIFFrame::IsLastFrame (void)
{
	return (mbIsLastFrame);
}

#endif // WEBC_SUPPORT_GIFS
