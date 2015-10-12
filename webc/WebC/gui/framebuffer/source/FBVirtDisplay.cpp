#ifndef WINDOWSGDI		/* So we can build GDI and Framebuffer from the same project */
#include "webc.h"
#include "webcmem.h"
#include "webrect.hpp"
#include "rtpstr.h"
#include "webcassert.h"
#include "webcstr.h"
#include "webcfg.h"
#include "webcLog.h"
#include "FBAbstraction.h"
#include "FB_HWInterface.h"
#include "FBConfig.h"

#include "FBAbstractionFonts.h"
#include "Filters.h"
#include "FBGraphicsContext.h"
#include <stdlib.h>
#include <math.h>
#include "rtpstr.h"
#include "Remoteterminal.h"
#include "einkterminal.h"

#include "rtptime.h"
#include "rtpprint.h"

#if (MAX_VIRTUAL_DISPLAYS > 0)
HTMLRect FB_getDirtyRect(HTMLGraphicsContext **ctx);
void FB_resetDirtyRect(HTMLGraphicsContext **ctx);
WEBC_UINT8 *lGetBaseAddress(HTMLGraphicsContext **ctx, FBHWInterface *dstFrameInfo, WEBC_INT whichBuffer);


static WEBC_UINT8 *gpVirtualDisplayPages;
static int gForeGroundVirtualDisplayPage;
WEBC_UINT8* gpBrowserHandles[MAX_VIRTUAL_DISPLAYS];
HTMLGraphicsContext **ctxbypagenumber[MAX_VIRTUAL_DISPLAYS];
void lDoDisplay(	HTMLGraphicsContext **ctx );

void VirtualDisplaySelectPageNumber(int newForeGroundPage);
void VirtualDisplaySelectPageNumberAndRender(int newForeGroundPage,WEBC_BOOL bRender);
WEBC_UINT8 * virtualDisplayGetBaseAddressByIndex(int index);
static WEBC_BOOL bDisplayIsDirty = WEBC_FALSE;



static WEBC_BOOL bPartialDraw = WEBC_FALSE;

void VirtualDisplayRotateBrowserNumber(void)
{
	if (gForeGroundVirtualDisplayPage + 1 >= MAX_VIRTUAL_DISPLAYS)
		VirtualDisplaySelectPageNumber(0);
	else
		VirtualDisplaySelectPageNumber(gForeGroundVirtualDisplayPage + 1);
}
void VirtualDisplaySelectPageNumber(int newForeGroundPage)
{
  VirtualDisplaySelectPageNumberAndRender(newForeGroundPage,WEBC_TRUE);
}


extern "C" {
	WEBC_BOOL ClearTheDirtyDisplay()
	{
     WEBC_BOOL
		  bDrawn=bDisplayIsDirty;

	 if(bDisplayIsDirty)
	 {
#if INCLUDE_REMOTE_TERMINAL
#else
		lDoDisplay(ctxbypagenumber[gForeGroundVirtualDisplayPage]);
#endif
	    bDisplayIsDirty = WEBC_FALSE;
	 }
     return bDrawn;
	}

	void webc_forceUpdate()
	{
	}

	int eink_set_1bit();
	int eink_set_4bit();
	int eink_set_bitres(int ibit);

};
void VirtualDisplaySelectPageNumberAndRender(int newForeGroundPage,WEBC_BOOL bRender)
{
	if (newForeGroundPage < 0 || newForeGroundPage >= MAX_VIRTUAL_DISPLAYS)
		return;
	if (gpVirtualDisplayPages)
	{
		if (gForeGroundVirtualDisplayPage != newForeGroundPage && ctxbypagenumber[gForeGroundVirtualDisplayPage])
		{
			FBHWInterface *dstFrameInfo = GET_FBGRAPHICS_CTX(ctxbypagenumber[gForeGroundVirtualDisplayPage])->frameBufferItf;
			// HERE Copy current output buffer to the foreground page's back buffer
			rtp_memcpy(	virtualDisplayGetBaseAddressByIndex(gForeGroundVirtualDisplayPage),
						dstFrameInfo->GetBaseAddress(((FBGraphicsContext *)ctxbypagenumber[gForeGroundVirtualDisplayPage])->currentGBuffer),
						WEBC_SCREEN_HEIGHT * WEBC_SCREEN_WIDTH);
			gForeGroundVirtualDisplayPage = newForeGroundPage;
			if (ctxbypagenumber[gForeGroundVirtualDisplayPage])
			{
				dstFrameInfo = GET_FBGRAPHICS_CTX(ctxbypagenumber[gForeGroundVirtualDisplayPage])->frameBufferItf;
				// HERE Copy new foreground pages back buffer to the current output buffer
				rtp_memcpy(	dstFrameInfo->GetBaseAddress(((FBGraphicsContext *)ctxbypagenumber[gForeGroundVirtualDisplayPage])->currentGBuffer),
						virtualDisplayGetBaseAddressByIndex(newForeGroundPage),
						WEBC_SCREEN_HEIGHT * WEBC_SCREEN_WIDTH * BYTES_PER_PIXEL);
				// HERE - Force a redraw
				{
					HTMLRect				canvas;
					FB_GetCanvasRect(ctxbypagenumber[gForeGroundVirtualDisplayPage],&canvas);
					FB_InvalidateRect(ctxbypagenumber[gForeGroundVirtualDisplayPage],&canvas);
#ifdef WIN32
					  lDoDisplay(ctxbypagenumber[gForeGroundVirtualDisplayPage]);// only done platform for immediate rendering
#else
#if INCLUDE_REMOTE_TERMINAL
					  lDoDisplay(ctxbypagenumber[gForeGroundVirtualDisplayPage]);// only done platform for immediate ren
					  bDisplayIsDirty = WEBC_TRUE;
#else
					  bDisplayIsDirty = WEBC_TRUE;
					  if(bPartialDraw==WEBC_TRUE)
					  {
					    int iBit;
					    // force to 1 bit
					    iBit = eink_set_1bit();
					    lDoDisplay(ctxbypagenumber[gForeGroundVirtualDisplayPage]);// only done platform for immediate ren
					    // back to 4 bit
                        eink_set_bitres(iBit);
					    bDisplayIsDirty = WEBC_FALSE;
					  }
#endif

#endif

					FB_resetDirtyRect(ctxbypagenumber[gForeGroundVirtualDisplayPage]);

				}
			}
		}

	}
}

extern "C" {

HBROWSER_HANDLE VirtualDisplayGetCurrentBrowser(void)
{
 	return(gpBrowserHandles[gForeGroundVirtualDisplayPage]);
}
}


int VirtualDisplayInit(WEBC_UINT8 *windowHandle)
{
	if (windowHandle)	/* Used as a flag to initialize or not */
		return 0;

	if (!gpVirtualDisplayPages)
	{
		gpVirtualDisplayPages = (WEBC_UINT8*)WEBC_MALLOC(MAX_VIRTUAL_DISPLAYS * WEBC_SCREEN_HEIGHT * WEBC_SCREEN_WIDTH * BYTES_PER_PIXEL);
		if (!gpVirtualDisplayPages)
			return(-1);
		gForeGroundVirtualDisplayPage = 0;
	}
	return 0;
}
static int virtualDisplayCtxToPageNumber(HTMLGraphicsContext **ctx)
{
int i;
	int thisPagenumber = 0;
	for(i = 0; i < MAX_VIRTUAL_DISPLAYS;i++)
	{
		if (ctx == ctxbypagenumber[i])
			return(i);
	}
	return(0);
}

WEBC_UINT8 * virtualDisplayGetBaseAddressByIndex(int index)
{
	WEBC_UINT8 *p = gpVirtualDisplayPages + (index * (WEBC_SCREEN_HEIGHT * WEBC_SCREEN_WIDTH * BYTES_PER_PIXEL));
	return(p);
}

WEBC_UINT8 * virtualDisplayGetBaseAddress(HTMLGraphicsContext **ctx)
{
	return(virtualDisplayGetBaseAddressByIndex(virtualDisplayCtxToPageNumber(ctx)));
}
void virtualMapBrowserHandleToCurrentFrame(HTMLGraphicsContext **ctx)
{
	gpBrowserHandles[gForeGroundVirtualDisplayPage] = ((FBGraphicsContext *) ctx)->browserHandle;
	ctxbypagenumber[gForeGroundVirtualDisplayPage] = ctx;
}
WEBC_BOOL virtualDisplayBackGroundDraw(HTMLGraphicsContext **ctx)
{
//	if (dstFrameInfo->VirtualPagenumber == 	gForeGroundVirtualDisplayPage)
//		return WEBC_TRUE;
	if (((FBGraphicsContext *) ctx)->browserHandle == 0 || ((FBGraphicsContext *) ctx)->browserHandle == gpBrowserHandles[gForeGroundVirtualDisplayPage])
		return WEBC_FALSE;

	return WEBC_TRUE;
}

void lVirtualDoDisplay(	HTMLGraphicsContext **ctx )
{
#ifdef __TIARM__
FBHWInterface * dstFrameInfo = GET_FBGRAPHICS_CTX(ctx)->frameBufferItf;
WEBC_UINT8 *srcPtr = lGetBaseAddress(ctx, dstFrameInfo,((FBGraphicsContext *)ctx)->currentGBuffer);
	if (!virtualDisplayBackGroundDraw(ctx))
	{	/* Don't send the background for update */
 		if(send_EinkDisplayBuffer(srcPtr,FB_getDirtyRect(ctx),IsPartialDraw()) )
		{
		 rtp_printf("lDoDisplay:MF:send_EinkDisplayBuffer:ok\r\n");
		}
		else
		{
          rtp_printf("lDoDisplay:MF:send_EinkDisplayBuffer:failed\r\n");
		}
	}
#endif
}
#endif




#endif /* WINDOWSGDI	*/
