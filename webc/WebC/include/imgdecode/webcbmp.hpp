#ifndef __WEBCBMP_HPP_INCLUDED__
#define __WEBCBMP_HPP_INCLUDED__

#include "webgraph.hpp"
#include "webimage.hpp"
#include "listener.hpp"

class WebcImageCollection;

class WebcImageFrame
{
public:
	virtual ~WebcImageFrame (void) {};
	virtual void Advance (void) = 0;
	virtual long DelayMsec (void) = 0;
	virtual int  IsLastFrame (void) = 0;
};

class WebcImage : public Notifier
{
protected:
	int miUseCount;
	WEBC_CHAR *mpKey;
	WebcImageState mState;

public:
	WebcImageCollection *mpCollection;
	WebcImage *mpNext;
	WebcImage *mpPrev;

	WebcImage (const WEBC_CHAR *pKey, int iUseCount = 1);
	virtual ~WebcImage (void);

	virtual void       Claim   (void);
	virtual int        InUse   (void);
	virtual WEBC_CHAR * Key     (void);
	virtual int        Matches (const WEBC_CHAR *pKey);
	virtual void       Release (void);
	virtual size_t     Size    (void) = 0;

	virtual DISPLAY_INT Width  (void);
	virtual DISPLAY_INT Height (void);

	virtual void           SetState (WebcImageState newState);
	virtual WebcImageState State    (void);

	virtual WebcImageFrame * DrawFrame    (WebcImageFrame *frame, WebRect *rect, WebGraphics *gc);
	virtual WebcImageFrame * DrawTiled    (WebcImageFrame *frame, WebRect *rect,
	                                       DISPLAY_INT xOffset, DISPLAY_INT yOffset, WebGraphics *gc);
	virtual void    	     ReleaseFrame (WebcImageFrame *frame);

};

class WebcBitmapImage : public WebcImage
{
protected:
	WebGraphics *mpGC;
	WebBitmap mBitmap;
	WebBitmap mScaledBitmap;
	DISPLAY_INT miWidth;
	DISPLAY_INT miHeight;
	DISPLAY_INT miScaledWidth;
	DISPLAY_INT miScaledHeight;

public:
	WebcBitmapImage (WebGraphics *gc, WebBitmap bitmap, const WEBC_CHAR *key, int useCount = 1);
	virtual ~WebcBitmapImage (void);

	virtual size_t Size    (void);

	virtual DISPLAY_INT Width  (void);
	virtual DISPLAY_INT Height (void);

	virtual WebcImageFrame * DrawFrame    (WebcImageFrame *frame, WebRect *rect, WebGraphics *gc);
	virtual WebcImageFrame * DrawTiled    (WebcImageFrame *frame, WebRect *rect,
	                                       DISPLAY_INT xOffset, DISPLAY_INT yOffset, WebGraphics *gc);

#if(WEBC_IMAGE_DRAW_EXTENSIONS)
	virtual void GetDrawableBitmap(WebGraphBitmapInfo** genericBitMap);
	virtual void ReplaceBitmap(WebGraphBitmapInfo* genericBitMap);
#endif

};

class WebcImageCollection
{
public:
	WebcImageCollection(WEBC_UINT32 ulMaxBytes = 0xffffffffL, WEBC_UINT32 ulGarbageCollectAt = 1024L*1024L);
	~WebcImageCollection(void);

	int  Add(WebcImage *pImage);
	void Remove(WebcImage *pImage);
	void Release(WebcImage *pImage);
	WebcImage *Find(const WEBC_CHAR *pKey);
	WebcImage *FindAndClaim(const WEBC_CHAR *pKey);
	long GarbageCollect(long lBytes = 0x7fffffffL);

protected:
	WebcImage *mpFirst;
	WebcImage *mpLast;
	WEBC_UINT32 mulMaxBytes;
	WEBC_UINT32 mulCurBytes;
	WEBC_UINT32 mulGarbageCollectAt;
};

#endif// __WEBCBMP_HPP_INCLUDED__
