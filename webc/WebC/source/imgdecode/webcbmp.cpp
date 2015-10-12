//
// WEBCBMP.CPP -
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

#include "webload.hpp"
#include "webcbmp.hpp"
#include "webc.h"
#include "webcstr.h"
#include "webcassert.h"
#include "webcmem.h"


/*****************************************************************************/
// Macros
/*****************************************************************************/


/*****************************************************************************/
// Data types
/*****************************************************************************/


/*****************************************************************************/
// Local Functions
/*****************************************************************************/


/*****************************************************************************/
// WebcImage methods
/*****************************************************************************/

WebcImage::WebcImage (const WEBC_CHAR *pKey, int iUseCount)
{
	mpKey = webc_malloc_string_copy(pKey, __FILE__, __LINE__);
	miUseCount = iUseCount;
	mpNext = 0;
	mpPrev = 0;
	mpCollection = 0;
	mState = WEBIMAGE_PARTIAL_ERROR;
}

WebcImage::~WebcImage (void)
{
	WEBC_ASSERT(miUseCount == 0);

	if (mpCollection)
	{
		mpCollection->Remove(this);
	}

	webc_free_string_copy(mpKey, __FILE__, __LINE__);
}

void WebcImage::Claim (void)
{
	miUseCount++;
}

int WebcImage::InUse (void)
{
	return (miUseCount);
}

WEBC_CHAR * WebcImage::Key (void)
{
	return (mpKey);
}

int WebcImage::Matches (const WEBC_CHAR *pKey)
{
	return (mpKey && pKey && !webc_stricmp(pKey, mpKey));
}

void WebcImage::Release (void)
{
	if (miUseCount > 0)
	{
		miUseCount--;
	}

	if (miUseCount == 0)
	{
		if (mpCollection)
		{
			mpCollection->Release(this);
		}
		else
		{
			WEBC_DELETE(this);
		}
	}
}

DISPLAY_INT WebcImage::Width (void)
{
	return (0);
}

DISPLAY_INT WebcImage::Height (void)
{
	return (0);
}

void WebcImage::SetState (WebcImageState newState)
{
	mState = newState;
}

WebcImageState WebcImage::State (void)
{
	return (mState);
}

WebcImageFrame * WebcImage::DrawFrame (WebcImageFrame *frame, WebRect *rect, WebGraphics *gc)
{
	return (0);
}

WebcImageFrame * WebcImage::DrawTiled (WebcImageFrame *frame, WebRect *rect,
		DISPLAY_INT xOffset, DISPLAY_INT yOffset, WebGraphics *gc)
{
	return (0);
}
void WebcImage::ReleaseFrame (WebcImageFrame *frame)
{
}


/*****************************************************************************/
// WebcBitmapImage methods
/*****************************************************************************/

WebcBitmapImage::WebcBitmapImage (WebGraphics *gc, WebBitmap bitmap, const WEBC_CHAR *key, int useCount)
	: WebcImage(key, useCount)
{
	mpGC = gc;
	mBitmap  = bitmap;
	miWidth  = WEB_BITMAP_WIDTH(bitmap);
	miHeight = WEB_BITMAP_HEIGHT(bitmap);

	mpGC->Claim();

	mScaledBitmap  = WEB_BITMAP_NULL;
	miScaledWidth  = 0;
	miScaledHeight = 0;
}

WebcBitmapImage::~WebcBitmapImage (void)
{
	if (mBitmap != WEB_BITMAP_NULL)
	{
		mpGC->DestroyWebBitmap(mBitmap);
	}

	if (mScaledBitmap != WEB_BITMAP_NULL)
	{
		mpGC->DestroyStretchedBitmap(mScaledBitmap, __FILE__, __LINE__);
	}

	mpGC->Release();
}

size_t WebcBitmapImage::Size (void)
{
	return (miWidth * miHeight * 4); // hack (worst case)
}

DISPLAY_INT WebcBitmapImage::Width (void)
{
	return (miWidth);
}

DISPLAY_INT WebcBitmapImage::Height (void)
{
	return (miHeight);
}

WebcImageFrame * WebcBitmapImage::DrawFrame (WebcImageFrame *frame, WebRect *rect, WebGraphics *gc)
{
	if ((rect->Width() <= 0) || (rect->Height() <= 0))
	{
		return (0);
	}

	DISPLAY_INT w = rect->Width();
	DISPLAY_INT h = rect->Height();

	if ((w != miWidth) || (h != miHeight))
	{
		if ((w != miScaledWidth) || (h != miScaledHeight))
    	{
    		if (mScaledBitmap != WEB_BITMAP_NULL)
    		{
				gc->DestroyStretchedBitmap(mScaledBitmap, __FILE__, __LINE__);
			}

    		mScaledBitmap = gc->CreateStretchedBitmap(mBitmap, w, h, __FILE__, __LINE__);
			if (mScaledBitmap != WEB_BITMAP_NULL)
			{
				miScaledWidth = w;
				miScaledHeight = h;
			}
    	}

		if (mScaledBitmap != WEB_BITMAP_NULL)
		{
			gc->Bitmap(rect->left, rect->top, mScaledBitmap);
			{
				// fix for leak
      			gc->DestroyStretchedBitmap(mScaledBitmap, __FILE__, __LINE__);
     			mScaledBitmap = WEB_BITMAP_NULL;
				// invalidate our scaled image
				miScaledWidth = 0;
				miScaledHeight = 0;

     		}
		}
	}
	else
	{
		gc->Bitmap(rect->left, rect->top, mBitmap);
	}

	return (0);
}

WebcImageFrame * WebcBitmapImage::DrawTiled (WebcImageFrame *frame, WebRect *rect,
		DISPLAY_INT xOffset, DISPLAY_INT yOffset, WebGraphics *gc)
{
	gc->TileBitmap(rect, xOffset, yOffset, mBitmap);
	return (0);
}

#if(WEBC_IMAGE_DRAW_EXTENSIONS)

void WebcBitmapImage::GetDrawableBitmap(WebGraphBitmapInfo** genericBitMap)
{
	mpGC->CreateGenericBitmap(mBitmap->bitmap, genericBitMap);
}

void WebcBitmapImage::ReplaceBitmap(WebGraphBitmapInfo* genericBitMap)
{
	if (mBitmap != WEB_BITMAP_NULL)
	{
		mpGC->DestroyWebBitmap(mBitmap);
	}
	mBitmap = mpGC->CreateWebBitmap(genericBitMap);

}

#endif
/*****************************************************************************/
// WebcImageCollection methods
/*****************************************************************************/

WebcImageCollection::WebcImageCollection(WEBC_UINT32 ulMaxBytes, WEBC_UINT32 ulGarbageCollectAt)
{
	mulCurBytes = 0;
	mulMaxBytes = ulMaxBytes;
	mulGarbageCollectAt = ulGarbageCollectAt;
	mpFirst = 0;
	mpLast = 0;
}

WebcImageCollection::~WebcImageCollection(void)
{
WebcImage *pToDelete = 0;

	while (mpFirst)
	{
		pToDelete = mpFirst;
		Remove(mpFirst);

		// only delete this bitmap if no one has it claimed
		if (!pToDelete->InUse())
		{
			WEBC_DELETE(pToDelete);
		}
	}
}

int WebcImageCollection::Add(WebcImage *pBitmap)
{
	if (!pBitmap)
	{
		return (-1);
	}

	if ((mulCurBytes + pBitmap->Size()) > mulMaxBytes)
	{
		GarbageCollect(mulMaxBytes - mulCurBytes - pBitmap->Size());
	}

	if ((mulCurBytes + pBitmap->Size()) > mulMaxBytes)
	{
		return (-1);
	}

	pBitmap->mpCollection = this;
	pBitmap->mpPrev = 0;
	pBitmap->mpNext = mpFirst;
	if (mpFirst)
	{
		mpFirst->mpPrev = pBitmap;
	}
	else
	{
		mpLast = pBitmap;
	}
	mpFirst = pBitmap;

	mulCurBytes += pBitmap->Size();

	return (0);
}

void WebcImageCollection::Remove(WebcImage *pBitmap)
{
	if (pBitmap->mpCollection != this)
	{
		return;
	}

	if (pBitmap->mpNext)
	{
		pBitmap->mpNext->mpPrev = pBitmap->mpPrev;
	}
	else
	{
		mpLast = pBitmap->mpPrev;
	}

	if (pBitmap->mpPrev)
	{
		pBitmap->mpPrev->mpNext = pBitmap->mpNext;
	}
	else
	{
		mpFirst = pBitmap->mpNext;
	}

	pBitmap->mpNext = 0;
	pBitmap->mpPrev = 0;
	pBitmap->mpCollection = 0;
	mulCurBytes -= pBitmap->Size();
}

void WebcImageCollection::Release(WebcImage *pBitmap)
{
	if (!pBitmap->InUse())
	{
		switch (pBitmap->State())
		{
			case WEBIMAGE_DONE:
			case WEBIMAGE_OKAY:
				if (mulCurBytes < mulGarbageCollectAt)
				{
					break;
				}

				// fall through
			default:
				Remove(pBitmap);
				WEBC_DELETE(pBitmap);
		}
	}
}

WebcImage *WebcImageCollection::Find(const WEBC_CHAR * pKey)
{
WebcImage *pBmp;

	pBmp = mpFirst;
	while (pBmp)
	{
		if (pBmp->Matches(pKey))
		{
			// Move to the front of the list
			Remove(pBmp);
			Add(pBmp);
			return (pBmp);
		}
		pBmp = pBmp->mpNext;
	}

	return (0);
}

WebcImage *WebcImageCollection::FindAndClaim(const WEBC_CHAR * pKey)
{
WebcImage *pBmp;

	pBmp = mpFirst;
	while (pBmp)
	{
		if (pBmp->Matches(pKey))
		{
			// Move to the front of the list
			Remove(pBmp);
			Add(pBmp);
			pBmp->Claim();
			return (pBmp);
		}
		pBmp = pBmp->mpNext;
	}

	return (0);
}

long WebcImageCollection::GarbageCollect(long lBytesToFree)
{
long lBytesFreed = 0;
WebcImage *pBmp, *pPrevBmp;

	pBmp = mpLast;
	while (pBmp && (lBytesFreed < lBytesToFree))
	{
		pPrevBmp = pBmp->mpPrev;
		if (!pBmp->InUse())
		{
			lBytesFreed += pBmp->Size();
			Remove(pBmp);
			WEBC_DELETE(pBmp);
		}
		pBmp = pPrevBmp;
	}

	return (lBytesFreed);
}
