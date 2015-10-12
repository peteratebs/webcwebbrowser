/*
|  WEBCGUI.H - WebC/GUI integration layer template
| 
|  EBS - 
| 
|   $Author: vmalaiya $
|   $Date: 2006/08/31 22:49:20 $
|   $Name:  $
|   $Revision: 1.1 $
| 
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/ 

#ifndef __WEBCGUI_H__
#define __WEBCGUI_H__

#include "webc.h"
#include "webrect.hpp"

/*****************************************************************************/
/* Types
 *****************************************************************************/

typedef struct
{
	HTMLGraphicsContext* ctx;
	HDC                  outputDC;
	HWND                 window;
	HDC                  windowDC;
	int                  windowWidth;
	int                  windowHeight;
	HDC                  bufferDC;
	HBITMAP              bufferBitmap;
	HBITMAP              bufferSave;
	HDC                  bitmapDC;
	HDC                  maskDC;
	HDC                  fontDC;
	int                  bufferDepth;
	HBITMAP              scratchBitmap;
	HDC                  scratchDC;
	DISPLAY_INT          scratchWidth;
	DISPLAY_INT          scratchHeight;
	DISPLAY_INT          bufferWidth;
	DISPLAY_INT          bufferHeight;
	WEBC_BOOL            invalidRectSet;
	WebRect              clipRect;
	WebRect              invalidRect;
}
WebcGDIWCEGraphics;

#define GET_OUTPUT_DC(C)      (((WebcGDIWCEGraphics*)(C))->outputDC)
#define GET_BITMAP_DC(C)      (((WebcGDIWCEGraphics*)(C))->bitmapDC)
#define GET_MASK_DC(C)        (((WebcGDIWCEGraphics*)(C))->maskDC)
#define GET_FONT_DC(C)        (((WebcGDIWCEGraphics*)(C))->fontDC)
#define GET_CLIP_RECT(C)      (((WebcGDIWCEGraphics*)(C))->clipRect)
#define GET_INVALID_RECT(C)   (((WebcGDIWCEGraphics*)(C))->invalidRect)
#define INVALID_RECT_SET(C)   (((WebcGDIWCEGraphics*)(C))->invalidRectSet)
#define GET_WINDOW(C)         (((WebcGDIWCEGraphics*)(C))->window)
#define GET_BUFFER_DEPTH(C)   (((WebcGDIWCEGraphics*)(C))->bufferDepth)
#define GET_BUFFER_BITMAP(C)  (((WebcGDIWCEGraphics*)(C))->bufferBitmap)
#define GET_BUFFER_SAVED(C)   (((WebcGDIWCEGraphics*)(C))->bufferSave)

#ifdef __cplusplus
 
extern "C" {

int    webc_InitGDIWCEBrowserConfig    (HTMLBrowserConfig* config, HWND windowHandle);
void   webc_DestroytGDIWCEBrowserConfig (HTMLBrowserConfig* config);
void   webc_DestroyGDIWCEBrowser       (HBROWSER_HANDLE hbrowser);
WebKey webc_TranslateGDIWCEKey         (WEBC_UINT32 rawKey);

}

#endif /* __cplusplus */

#endif /* __WEBCGUI_H__ */
