//
// WEBMNG.CPP - Interface to LibMNG for MNG image support
//
// EBS - WEBC
//
// Copyright EBS Inc. , 2003
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

#if (WEBC_SUPPORT_MNG)

#include "webc.h"
#include "webmng.hpp"
#include "imgconv.hpp"
#include "webcbmp.hpp"
#include "webcassert.h"
#include "rtptime.h"
#include "webcmem.h"


/*****************************************************************************/
// Local Constants 
/*****************************************************************************/

/*****************************************************************************/
// Local type definitions
/*****************************************************************************/

struct WebcMNGFrameData
{
	WEBC_UCHAR8 *frameBuffer;
	size_t       bytesPerRow;
	int          timerSet;
	long         nextFrameTime;
};

class WebcMNGFrame : public WebcImageFrame
{
public:
	int          miFrameIndex;
	long         miNextFrameTime;
	int          mbNextFrame;
	WebBitmap    mBitmap;
	int          miBitmapIndex;
	WebGraphics *mpGC;

	WebcMNGFrame (void);
	~WebcMNGFrame (void);

	virtual void Advance (void);
	virtual long DelayMsec (void);
	virtual int IsLastFrame (void);

	void SetBitmap (WebBitmap bitmap, int bitmapIndex, WebGraphics *gc);
};

class WebcMNGImage : public WebcImage
{
protected:
	int mbFirstDisplay;
	mng_handle mhMNGHandle;
	DISPLAY_INT miWidth;
	DISPLAY_INT miHeight;
	int miNextFrame;
	WebcMNGFrameData mData;
	
	WebBitmap GenerateBitmap (int index, WebGraphics *gc);

public:
	WebcMNGImage (mng_handle hMng, const WEBC_CHAR *pKey);
	virtual ~WebcMNGImage (void);

	virtual size_t Size    (void);

	virtual DISPLAY_INT Width  (void);
	virtual DISPLAY_INT Height (void);

	virtual WebcImageFrame * DrawFrame    (WebcImageFrame *frame, WebRect *rect, WebGraphics *gc);
	virtual WebcImageFrame * DrawTiled    (WebcImageFrame *frame, WebRect *rect, 
	                                       DISPLAY_INT xOffset, DISPLAY_INT yOffset, WebGraphics *gc);
	virtual void    	     ReleaseFrame (WebcImageFrame *frame);
};


/*****************************************************************************/
// Local Function Prototypes
/*****************************************************************************/

static mng_ptr    webc_mng_memalloc      (mng_size_t iLen);
static void       webc_mng_memfree       (mng_ptr iPtr, mng_size_t iLen);
static mng_bool   webc_mng_openstream    (mng_handle hHandle);
static mng_bool   webc_mng_readdata      (mng_handle hHandle, 
                                          mng_ptr pBuf,
                                          mng_uint32 iBuflen, 
                                          mng_uint32p pRead);
static mng_bool   webc_mng_closestream   (mng_handle  hHandle);
static mng_ptr    webc_mng_getcanvasline (mng_handle  hHandle, 
                                          mng_uint32  iLinenr);
static mng_bool   webc_mng_refresh       (mng_handle  hHandle,
                                          mng_uint32  iX,
                                          mng_uint32  iY,
                                          mng_uint32  iWidth,
                                          mng_uint32  iHeight);
static mng_uint32 webc_mng_gettickcount  (mng_handle  hHandle);
static mng_bool   webc_mng_settimer      (mng_handle  hHandle,
                                          mng_uint32  iMsecs);


/*****************************************************************************/
// Global Data
/*****************************************************************************/

/*****************************************************************************/
// WebcMNGDecoder Constructors
/*****************************************************************************/

WebcMNGDecoder::WebcMNGDecoder()
{
	// initialize our MNG context
	mhMNGHandle = mng_initialize ((mng_ptr) this, 
	                              webc_mng_memalloc, 
	                              webc_mng_memfree, 
	                              MNG_NULL);
	                              
	if (mhMNGHandle != MNG_NULL)
	{
		mng_retcode result; 
		
		// set callbacks for reading the image
		result = mng_setcb_openstream (mhMNGHandle, webc_mng_openstream);
		if (result == MNG_NOERROR)
		{
			result = mng_setcb_closestream (mhMNGHandle, webc_mng_closestream);
			if (result == MNG_NOERROR)
			{
				result = mng_setcb_readdata (mhMNGHandle, webc_mng_readdata);
				if (result == MNG_NOERROR)
				{
					result = mng_setcb_gettickcount(mhMNGHandle, webc_mng_gettickcount);
				}
			}
		}
		
		if (result != MNG_NOERROR)
		{
			// if problems registering the callbacks, then release our handle so
			//  we don't try to load the image
			mng_cleanup(&mhMNGHandle);
			mhMNGHandle = MNG_NULL;			
		}
	}
}

WebcMNGDecoder::~WebcMNGDecoder()
{
	if (mhMNGHandle != MNG_NULL)
	{
		mng_cleanup(&mhMNGHandle);
	}
}

WebcImage * WebcMNGDecoder::go(WEBC_CHAR *bitmapKey)
{
	if (!mpDataBuffer)
	{
		meState = WEBIMAGE_MALLOC_ERROR;
		return (0);
	}

	if ((mhMNGHandle == MNG_NULL) || 
	    (mng_set_suspensionmode(mhMNGHandle, MNG_FALSE) != MNG_NOERROR))
	{
		meState = WEBIMAGE_MALLOC_ERROR;
		return (0);
	}
	
	if (mng_read(mhMNGHandle) != MNG_NOERROR)
	{
		// handle the error and return
		meState = WEBIMAGE_READ_ERROR;
		return (0);
	}

	WebcImage *ret;
	WEBC_NEW_VERBOSE(ret, WebcMNGImage(mhMNGHandle, bitmapKey),"WebcMNGImage");
	if (ret)
	{
		ret->SetState(getState());
		mhMNGHandle = MNG_NULL;
	}	
		
	return ret;
}

long WebcMNGDecoder::readData (unsigned char *buffer, long size)
{
	// read between 1 and size bytes into the buffer
	return (readAtLeast(buffer, size, 1));
}


/*****************************************************************************/
// WebcMNGImage methods
/*****************************************************************************/

WebcMNGImage::WebcMNGImage (mng_handle hMng, const WEBC_CHAR *pKey)
	: WebcImage(pKey)
{
	mhMNGHandle = hMng;
	miWidth = mng_get_imagewidth(hMng);
	miHeight = mng_get_imageheight(hMng);
	mbFirstDisplay = 1;
	miNextFrame = 0;

	mData.frameBuffer = (WEBC_UCHAR8 *) WEBC_MALLOC(miWidth * miHeight * 3);
	mData.bytesPerRow = miWidth * 3;

	mng_set_userdata(mhMNGHandle, (mng_ptr) &mData);
	mng_setcb_getcanvasline(mhMNGHandle, webc_mng_getcanvasline);
	mng_setcb_refresh(mhMNGHandle, webc_mng_refresh);
	mng_setcb_settimer(mhMNGHandle, webc_mng_settimer);
	mng_set_canvasstyle(mhMNGHandle, MNG_CANVAS_RGB8);
}

WebcMNGImage::~WebcMNGImage (void)
{
	if (mhMNGHandle != MNG_NULL)
	{
		mng_cleanup(&mhMNGHandle);
	}

	if (mData.frameBuffer)
	{
		WEBC_FREE(mData.frameBuffer);
	}
}

size_t WebcMNGImage::Size (void)
{
	return (0);
}

DISPLAY_INT WebcMNGImage::Width (void)
{
	return (miWidth);
}

DISPLAY_INT WebcMNGImage::Height (void)
{
	return (miHeight);
}

WebBitmap WebcMNGImage::GenerateBitmap (int index, WebGraphics *gc)
{
	if (mData.frameBuffer)
	{				
		mng_retcode result;

		if (index == miNextFrame)
		{
			if (miNextFrame == 0)
			{
				mData.timerSet = 0;
				result = mng_display(mhMNGHandle);
				miNextFrame++;
			}
			else
			{
				mData.timerSet = 0;
				result = mng_display_resume(mhMNGHandle);
				miNextFrame++;
			}
		}
		else
		{
			if (miNextFrame > index)
			{
				result = mng_display_reset(mhMNGHandle);
				miNextFrame = 0;
			}

			if (miNextFrame == 0)
			{
				mData.timerSet = 0;
				result = mng_display(mhMNGHandle);
				miNextFrame++;
			}
			
			while (miNextFrame <= index)
			{
				mData.timerSet = 0;
				result = mng_display_resume(mhMNGHandle);
				miNextFrame++;
			}
		}

		switch (result)
		{
			case MNG_NOERROR:
			case MNG_NEEDTIMERWAIT:
			{
				WEBC_ASSERT(!(result == MNG_NEEDTIMERWAIT && !mData.timerSet));
				WebGraphBitmapInfo info = {
						miWidth,                     // iWidth;
						miHeight,                    // iHeight;
						mData.bytesPerRow,           // iBytesPerRow;
						WB_PIXEL_FORMAT_RGB_24,      // pixelFormat;
						0,                           // bHasTransparentColor;
						0,                           // iTransparentIndex;
						0,                           // uTransparentRed;
						0,                           // uTransparentGreen;
						0,                           // uTransparentBlue;
						0,                           // iPaletteSize;
						(WEBC_UCHAR8 *) 0,           // pPalette;
						mData.frameBuffer            // pData;
					};			
				
				return (gc->CreateWebBitmap(&info));
			}
				
			default:
				break;
		}
	}	
	
	return (WEB_BITMAP_NULL);
}

WebcImageFrame * WebcMNGImage::DrawFrame (WebcImageFrame *frame, WebRect *rect, WebGraphics *gc)
{
	WebBitmap bmp = WEB_BITMAP_NULL;
	WebcMNGFrame *mngFrame = (WebcMNGFrame *) frame;
	
	if (!mngFrame)
	{
		WEBC_NEW_VERBOSE(mngFrame, WebcMNGFrame(),"WebcMNGFrame");
	}
	
	if (mngFrame && (mngFrame->mBitmap != WEB_BITMAP_NULL) && (mngFrame->miBitmapIndex == mngFrame->miFrameIndex))
	{
		bmp = mngFrame->mBitmap;
	}
	else
	{
		int index = mngFrame? mngFrame->miFrameIndex : 0;
		
		bmp = GenerateBitmap(index, gc);

		if (mngFrame)
		{
			mngFrame->SetBitmap(bmp, index, gc);
			if (mData.timerSet)
			{
				mngFrame->miNextFrameTime = mData.nextFrameTime;
				mngFrame->mbNextFrame = 1;
			}
			else
			{
				mngFrame->mbNextFrame = 0;
			}
		}
	}

	if (bmp != WEB_BITMAP_NULL)
	{
		if ((rect->Width() == miWidth) && (rect->Height() == miHeight))
		{
			gc->Bitmap(rect->left, rect->top, bmp);		
		}
		else
		{
			gc->StretchBitmap(rect->left, rect->top, rect->Width(), rect->Height(), bmp);
		}

		if (!mngFrame)
		{
			gc->DestroyWebBitmap(bmp);
		}
	}
	
	return (mngFrame);
}

WebcImageFrame * WebcMNGImage::DrawTiled (WebcImageFrame *frame, WebRect *rect, 
                                DISPLAY_INT xOffset, DISPLAY_INT yOffset, WebGraphics *gc)
{
	return (0);
}
                                
void WebcMNGImage::ReleaseFrame (WebcImageFrame *frame)
{
	WEBC_DELETE(frame);
}


/*****************************************************************************/
// WebcMNGFrame methods
/*****************************************************************************/

WebcMNGFrame::WebcMNGFrame (void)
{
	miFrameIndex = 0;
	miNextFrameTime = 0;
	mbNextFrame = 0;
	mBitmap = WEB_BITMAP_NULL;
	miBitmapIndex = 0;
	mpGC = 0;
}

WebcMNGFrame::~WebcMNGFrame (void)
{
	if (mpGC)
	{
		if (mBitmap != WEB_BITMAP_NULL)
		{
			mpGC->DestroyWebBitmap(mBitmap);
		}
		mpGC->Release();
	}
}

void WebcMNGFrame::Advance (void)
{
	miFrameIndex++;
	mbNextFrame = 0;
}

long WebcMNGFrame::DelayMsec (void)
{
	long delay = (miNextFrameTime - rtp_get_system_msec());
	if (delay < 1)
	{
		delay = 1;
	}
	return (delay);
}

int WebcMNGFrame::IsLastFrame (void)
{
	return (!mbNextFrame);
}

void WebcMNGFrame::SetBitmap (WebBitmap bitmap, int bitmapIndex, WebGraphics *gc)
{
	if (gc)
	{
		gc->Claim();
	}	

	if (mpGC)
	{
		if (mBitmap != WEB_BITMAP_NULL)
		{
			mpGC->DestroyWebBitmap(mBitmap);
		}
		mpGC->Release();
	}

	mpGC = gc;
	mBitmap = bitmap;
	miBitmapIndex = bitmapIndex;
}


/*****************************************************************************/
// LibMNG callback functions
/*****************************************************************************/

mng_ptr webc_mng_memalloc (mng_size_t iLen)
{
	unsigned char *mem = (unsigned char *) WEBC_MALLOC(iLen);
	if (mem)
	{
		tc_memset(mem, 0, iLen);
	}
	return ((mng_ptr) mem);
}

void webc_mng_memfree (mng_ptr iPtr, mng_size_t iLen)
{
	WEBC_FREE(iPtr);
}

mng_bool webc_mng_openstream (mng_handle hHandle)
{
	return (MNG_TRUE);
}

mng_bool webc_mng_readdata (mng_handle hHandle, mng_ptr pBuf, mng_uint32 iBuflen, mng_uint32p pRead)
{
	long result;
	WebcMNGDecoder *webMng = (WebcMNGDecoder *) mng_get_userdata(hHandle);
	
	result = webMng->readData((unsigned char *) pBuf, iBuflen);
	if (result < 0)
	{
		*pRead = 0;
		return (MNG_FALSE);
	}
	
	*pRead = result;
	
	return (MNG_TRUE);
}

mng_bool webc_mng_closestream (mng_handle  hHandle)
{
	return (MNG_TRUE);
}

mng_ptr webc_mng_getcanvasline (mng_handle hHandle, mng_uint32 iLinenr)
{
	WebcMNGFrameData *data = (WebcMNGFrameData *) mng_get_userdata(hHandle);

	return (&data->frameBuffer[iLinenr * data->bytesPerRow]);
}

mng_bool webc_mng_refresh (mng_handle hHandle, mng_uint32 iX, mng_uint32 iY,
                           mng_uint32 iWidth, mng_uint32 iHeight)
{
	return (MNG_TRUE);
}

mng_uint32 webc_mng_gettickcount (mng_handle hHandle)
{
	return (rtp_get_system_msec());
}

mng_bool webc_mng_settimer (mng_handle hHandle, mng_uint32 iMsecs)
{
	WebcMNGFrameData *data = (WebcMNGFrameData *) mng_get_userdata(hHandle);

	data->timerSet = 1;
	data->nextFrameTime = rtp_get_system_msec() + iMsecs;
	
	return (MNG_TRUE);
}

#endif // WEBC_SUPPORT_MNG
