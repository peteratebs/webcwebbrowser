/*
|  WEBCGUI.H - WebC/GUI integration layer template
|
|  EBS -
|
|   $Author: vmalaiya $
|   $Date: 2006/10/10 20:32:14 $
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

#ifdef __cplusplus

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xos.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "webc.h"
#include "webrect.hpp"

typedef struct
{
    HTMLGraphicsContext* ctx;
    Display *           display;
    int                 screen_num;
    Window              windowHandle;
    GC                  gc;
    int                 xlib_width;
    int                 xlib_height;
    XFontStruct *       xlib_font_info;
  #if defined(WEBC_BUFFER_SCROLLING)
    Bitmap*             bufferImage;
    Graphics*           bufferGraphics;
  #endif // GDIPLUS_DOUBLE_BUFFER
  #ifdef WEBC_BUFFER_SCROLLING
    Bitmap*             scratchBitmap;
    Graphics*           scratchGraphics;
    WebRect             scratchContents;
  #endif
    WebRect             invalidRect;
    HTMLRect            current_cliprect;
    int                 drawNesting;
    WEBC_UINT8          blendLevelInt;
    //REAL              blendLevelReal;
    int                 blendLevelReal;
	bool				isTransparent;
} WebcXLIBGraphics;

extern "C" {
int    webc_InitXLIBBrowserConfig    (HTMLBrowserConfig* config, WEBC_BOOL bCreateWindow);
void   webc_DestroyXLIBBrowserConfig (HTMLBrowserConfig* config);
WebKey webc_TranslateXLIBKey         (WEBC_UINT32 rawKey);
void xlib_execute_webc(HTMLGraphicsContext** ctx,
					   HBROWSER_HANDLE gbrowser);
}

#endif /* __cplusplus */

#endif /* __WEBCGUI_H__ */
