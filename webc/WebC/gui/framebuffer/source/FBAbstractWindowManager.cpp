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


//
// To do:
//
//
//  1. In manager layer.
//     When a window is brought to top.
//        Recalculate intersections of all windows
//	1. In draw layer
//
//	   If window is obscured
//	       perform extra clip operation.
//
//
//

WEBC_UINT8 *lGetBaseAddress(HTMLGraphicsContext **ctx, FBHWInterface *dstFrameInfo, WEBC_INT whichBuffer);

void FB_drawRectangle(HTMLGraphicsContext **ctx,
	DISPLAY_INT x,
	DISPLAY_INT y,
	DISPLAY_INT width,
	DISPLAY_INT height,
	DISPLAY_INT BorderThickness,
	WEBC_BOOL  fill,
	WEBC_UINT32 lineColor,
	WEBC_UINT32 fillColor);


extern void * currentActivatedWindow;//current window which is active
static void FBWIN_DrawThisonly(void *_pW);
void FBWM_closeWindow(HTMLGraphicsContext **ctx);


#include "hcbox.hpp"
typedef struct s_FBWMWin {
	HTMLBrowserConfig	Config;
	HTMLGraphicsContext **ctx;
	struct vector WinClipListVector;
	struct s_FBWMWin* pnext;
	WEBC_UINT8 *SaveUnderData;
	WEBC_BOOL ScheduleRedraw;
} FBWMWin;
FBWMWin *pWRoot;
static void FBWM_removeWindow(void *WindowToRemove);
static FBWMWin * _findWindowbyCtx(HTMLGraphicsContext **ctx);
static void FBWM_EraseThisonly(void *_pW);
static void FBWIN_SaveUnder(FBWMWin *pW);
static void FBWIN_RestorSaveUnder(FBWMWin *pW);
static WEBC_BOOL FBWIN_WinaOverlapsWinb(FBWMWin *pWa, FBWMWin *pWb);
static WEBC_BOOL FBWIN_CheckIfOverlapped(FBWMWin *pW);
static void FBWIN_DrawZlist(FBWMWin * pW);
static void FBWIN_ClearPaintList(void);
static void FBWIN_DrawPaintList(void);
static WEBC_BOOL FBWIN_WinaContainsWinb(FBWMWin *pWa, FBWMWin *pWb);
static FBWMWin *FBWIN_FindContainingWindow(FBWMWin *pWtoTest);
static void FBWIN_ClipToRoot(FBWMWin *pW);
static void FBWIN_SaveUnder(FBWMWin *pW);
static FBWMWin *FBWM_WindowStackTop(void);

static void FBWIN_BuildAllWindowClipLists(void);
static void FBWIN_WinToHtmlRect(FBWMWin *pW, HTMLRect *r);
static void FBWIN_BuildWindowClipList(FBWMWin *pWtoProcess);

void *grabbedFocusedWindow;
int  grabbedFocusedCount;

#define CANVASCOLOR 0xffffffff
void * FBWM_createWindow(HTMLGraphicsContext **ctx,	HTMLBrowserConfig *pconfig)
{
FBWMWin *pW;
	pW = (FBWMWin *)WEBC_MALLOC_VERBOSE(sizeof(FBWMWin),"FBWMWin Structure");
	rtp_memset(pW, 0, sizeof(*pW));
	vector_init(&pW->WinClipListVector,sizeof(HTMLRect));
	// Start at the bottom of the z stack
	pW->pnext = pWRoot;
	pWRoot=pW;

	pW->Config = *pconfig;
	pW->ctx = ctx;

	//	pconfig->left
	//	pconfig->top
	//	pconfig->width
	//	pconfig->height
	//	pconfig->BrowserHandle
	if ((pW->Config.WindowConfig.WindowFlags & WF_STARTINBG)==0)
		FBWM_moveWindowToTop((void *)pW, WEBC_FALSE);
	else // Did not call FBWM_moveWindowToTop, so manually call FBWIN_BuildWindowClipList for this window
		 FBWIN_BuildWindowClipList(pW);

	if ((pW->Config.WindowConfig.WindowFlags & WF_STARTINBG)==0)
	{
		if (pW->Config.WindowConfig.WindowFlags & (WF_SAVEUNDER|WF_SPRITE))
			FBWIN_SaveUnder(pW);
		FBWIN_DrawThisonly((void *)pW);
	}
   return (void *)pW;
}

// A pointer to a window is about to be used, first verify that it is still in the window list, return 0 if it is not, otherwise return self.
static FBWMWin * _VerifyWindowInList(FBWMWin *pW)
{
	FBWMWin *_pW = pWRoot;
	while (_pW)
	{
		if (pW==_pW)
			return pW;
		_pW = _pW->pnext;
	}
	return 0;
}

// Simple enumerator used by timer to enumerate
void *FBWM_FindWindowByWindowNumber(int WindowNumber)
{
	FBWMWin *_pW = pWRoot;
	int n=0;
	while (_pW)
	{
		if (n==WindowNumber)
			return (void *)_pW;
		_pW = _pW->pnext;
		n++;
	}
	return 0;
}

void FBWM_closeWindow(HTMLGraphicsContext **ctx)
{
FBWMWin *pW = _findWindowbyCtx(ctx);
	if (pW)
	{
		FBWMWin *pWStackTop = FBWM_WindowStackTop();
		if (grabbedFocusedWindow == (void *) pW)
		{
			grabbedFocusedWindow = 0;
			grabbedFocusedCount = 0;

		}
		if (pW->Config.WindowConfig.WindowFlags & (WF_SAVEUNDER|WF_SPRITE))
			FBWIN_RestorSaveUnder(pW);
		else
			FBWM_EraseThisonly((void *)pW);
		FBWM_removeWindow((void *)pW);
		vector_delete(&pW->WinClipListVector);
		WEBC_FREE(pW);

		// Reset the top of the window stack if we were it..
	//	if (pWStackTop==pW && FBWM_WindowStackTop())
	//	{
	//		FBWM_moveWindowToTop((void *) FBWM_WindowStackTop(), WEBC_TRUE);
	//	}
	}
	webc_DestroyFBBrowserConfig(ctx);
}

void *FBWM_getReceivingWindow(HTMLEvent* e)
{
long x, y;
FBWMWin *ReceivingWindow = 0;

	x  = e->data.position.x;
	y = e->data.position.y;

	/* Search up the stack and return the top most window to contain the coordinates */
	FBWMWin *_pW = pWRoot;
	while (_pW)
	{
	int Left,Top;
		Left = _pW->Config.WindowConfig.left+_pW->Config.WindowConfig.BorderThickness;
		Top  = _pW->Config.WindowConfig.top+_pW->Config.WindowConfig.BorderThickness;

		if (x >= Left && x <= Left+_pW->Config.WindowConfig.width &&
			y >= Top && y <= Top+_pW->Config.WindowConfig.height)
		{
			e->data.position.x	= x - Left;
			e->data.position.y	= y - Top;
			ReceivingWindow = _pW;
		}
		_pW = _pW->pnext;
	}
	return (void *) ReceivingWindow;
}
void FBWM_setPointerCoordsToFocusWindow(void * FocusWindow, HTMLEvent* e)
{
long x, y;
FBWMWin *_pW = (FBWMWin *)FocusWindow;
	x = e->data.position.x;
	y = e->data.position.y;
	if (_pW)
	{
	int Left,Top;
		Left = _pW->Config.WindowConfig.left+_pW->Config.WindowConfig.BorderThickness;
		Top  = _pW->Config.WindowConfig.top+_pW->Config.WindowConfig.BorderThickness;
		if (x >= Left && x <= Left+_pW->Config.WindowConfig.width &&
			y >= Top && y <= Top+_pW->Config.WindowConfig.height)
		{
			e->data.position.x	= x - Left;
			e->data.position.y	= y - Top;
		}
		else
		{ // Put it off the map.
			e->data.position.x	= e->data.position.y = -1;
		}
	}
}
HBROWSER_HANDLE FBWM_FindBrowserByWindow(void *target)
{
	if (!target)
		return 0;
	FBWMWin *_pW = (FBWMWin *) target;
	if (!_VerifyWindowInList(_pW))
		return 0;
	return _pW->Config.BrowserHandle;
}

static FBWMWin *FBWM_WindowStackTop(void)
{
	FBWMWin *_pW = pWRoot;
	while (_pW)
	{
		if (!_pW->pnext)
			return _pW;
		_pW = _pW->pnext;
	}
	return 0;
}


static void FBWM_DrawWindowAndContent(FBWMWin *WindowToDraw)
{
	FBWIN_DrawThisonly((void *)WindowToDraw);
	webc_BrowserInvalidate(WindowToDraw->Config.BrowserHandle);
	webc_BrowserDraw(WindowToDraw->Config.BrowserHandle);
}

void FBWM_moveWindowToTop(void *WindowToActivate, WEBC_BOOL redraw)
{
FBWMWin *_pWactivate = (FBWMWin *) WindowToActivate;
	if (redraw && !FBWIN_CheckIfOverlapped(_pWactivate))
		redraw = WEBC_FALSE;
	FBWM_removeWindow((void *)_pWactivate);
	if (!pWRoot)
		pWRoot = _pWactivate;
	else
		FBWM_WindowStackTop()->pnext = _pWactivate;
	_pWactivate->pnext= 0;

	// Rebuild the window clip lists based on new zorder
	FBWIN_BuildAllWindowClipLists();

	if (redraw)
		FBWM_DrawWindowAndContent(_pWactivate);
}
static void FBWM_removeWindow(void *WindowToRemove)
{
FBWMWin *pW = (FBWMWin *) WindowToRemove;
	if (!pW)
		return;
	if (pW==pWRoot)
	{
		pWRoot=pW->pnext;
		return;
	}
	FBWMWin *_pW = pWRoot;
	while (_pW->pnext)
	{
		if (pW==_pW->pnext)
		{
			_pW->pnext = pW->pnext;
			break;
		}
		_pW = _pW->pnext;
	}
}


static FBWMWin * _findWindowbyCtx(HTMLGraphicsContext **ctx)
{
	FBWMWin *pW = (FBWMWin *) (*((FBGraphicsContext *)ctx)).windowHandle;
	return _VerifyWindowInList(pW);
}


void FBWIN_GetCanvasRect(
	HTMLGraphicsContext **ctx,
	HTMLRect				*rectangle
	)
{
FBWMWin * pW = _findWindowbyCtx(ctx);
	if (!pW)
	{ /* Should not happen, buty just in case */
		rectangle->left		= 0;
		rectangle->top		= 0;
		rectangle->right	= WEBC_SCREEN_WIDTH;
		rectangle->bottom	= WEBC_SCREEN_HEIGHT;
		return;
	}
	rectangle->left		= pW->Config.WindowConfig.left+pW->Config.WindowConfig.BorderThickness;
	rectangle->top		= pW->Config.WindowConfig.top+pW->Config.WindowConfig.BorderThickness;
	rectangle->right	= pW->Config.WindowConfig.left+pW->Config.WindowConfig.width-pW->Config.WindowConfig.BorderThickness-1;
	rectangle->bottom	= pW->Config.WindowConfig.top+pW->Config.WindowConfig.height-pW->Config.WindowConfig.BorderThickness-1;

}

static void FBWIN_DrawBorder(void *_pW);
static void FBWIN_DrawCanvas(void *_pW);


static void FBWIN_DrawThisonly(void *_pW)
{
	HTMLRect				cliprect,cliprectSave;
	FBWMWin *pW = (FBWMWin *)_pW;

	FB_BeginDrawing(pW->ctx);

	FB_GetClipRect(pW->ctx, &cliprect);
	cliprectSave=cliprect;

//	pW->Config.BorderStyle;

	cliprect.left = pW->Config.WindowConfig.left;
	cliprect.top = pW->Config.WindowConfig.top;
	cliprect.right  = pW->Config.WindowConfig.left+pW->Config.WindowConfig.width-1;
	cliprect.bottom = pW->Config.WindowConfig.top+pW->Config.WindowConfig.height-1;

	FB_SetClipRect(pW->ctx, &cliprect);
	FBWIN_DrawCanvas(_pW);
	FBWIN_DrawBorder(_pW);
	FB_SetClipRect(pW->ctx, &cliprectSave);
	FB_EndDrawing(pW->ctx);
}
static void FBWM_EraseThisonly(void *_pW)
{
	HTMLRect				cliprect,cliprectSave;
	FBWMWin *pW = (FBWMWin *)_pW;

	FB_GetClipRect(pW->ctx, &cliprect);
	cliprectSave=cliprect;

	cliprect.left = pW->Config.WindowConfig.left;
	cliprect.top = pW->Config.WindowConfig.top;
	cliprect.right  = pW->Config.WindowConfig.left+pW->Config.WindowConfig.width-1;
	cliprect.bottom = pW->Config.WindowConfig.top+pW->Config.WindowConfig.height-1;
	FB_SetClipRect(pW->ctx, &cliprect);
	FB_drawRectangle(pW->ctx,
		(DISPLAY_INT) pW->Config.WindowConfig.left,
		(DISPLAY_INT) pW->Config.WindowConfig.top,
		(DISPLAY_INT) pW->Config.WindowConfig.width,
		(DISPLAY_INT) pW->Config.WindowConfig.height,
		(DISPLAY_INT) pW->Config.WindowConfig.BorderThickness,
		true,
		CANVASCOLOR, // outlineColor,
		CANVASCOLOR);  // fillColor,
	FB_SetClipRect(pW->ctx, &cliprectSave);
//	FB_EndDrawing(pW->ctx);
}



/*---------------------------------------------------------------------------*/
// Put the current window on top. Called when the browser focus changes.
void guiWmBringToTop(HTMLGraphicsContext** ctx)
{
//	BringWindowToTop(GET_WINDOW_HANDLE(ctx));
FBWMWin * pW = _findWindowbyCtx(ctx);
	FBWM_moveWindowToTop((void *) pW, WEBC_TRUE);
}

void guiWmGetPos(HTMLGraphicsContext** ctx,int *left, int *top, int *width, int *height)
{
FBWMWin * pW = _findWindowbyCtx(ctx);
	if (pW)
	{
		*left   =pW->Config.WindowConfig.left;
		*top    =pW->Config.WindowConfig.top;
		*width  =pW->Config.WindowConfig.width;
		*height =pW->Config.WindowConfig.height;
	}
}
void guiWmSetPos(HTMLGraphicsContext** ctx, int left, int top, int width, int height)
{
FBWMWin * pW = _findWindowbyCtx(ctx);
FBWMWin oldW;
FBWMWin *surroundingW;
int oldwidth, oldheight;
int SizeChanged = 0;

	if (!pW)
		return;

	oldwidth  =   pW->Config.WindowConfig.width;
	oldheight =   pW->Config.WindowConfig.height;

	if (oldwidth != width || oldheight != height)
		SizeChanged = 1;

	// Use bit blitting instead of painting if WF_SPRITE is set and we're moving but not resizing
	// If pW->SaveUnderData is null we were invisible when created.
	if (SizeChanged == 0 && pW->SaveUnderData && pW->Config.WindowConfig.WindowFlags & WF_SPRITE)
	{
	WEBC_UINT8 *pmyimage;
		pmyimage = (WEBC_UINT8*)WEBC_MALLOC(height*width*BYTES_PER_PIXEL);
		if (!pmyimage)
			return;
		// Read the current image into a buffer
		FB_ReadFrameBuffer(pW->ctx,pmyimage,pW->Config.WindowConfig.left,pW->Config.WindowConfig.top, oldwidth,	oldheight);

		// BeginDraw
		FB_BeginDrawing(pW->ctx);

		// Restore the space under the current image
		FB_WriteFrameBuffer(pW->ctx, pW->SaveUnderData, pW->Config.WindowConfig.left, pW->Config.WindowConfig.top,oldwidth,oldheight);

		// Change coordinates
		pW->Config.WindowConfig.left   = left;
		pW->Config.WindowConfig.top	   = top;

		// Read the space under where the current image will be redisplayed
		FB_ReadFrameBuffer(pW->ctx,pW->SaveUnderData,pW->Config.WindowConfig.left,pW->Config.WindowConfig.top, oldwidth,	oldheight);

		// Write the current image to the new coordinates.
		FB_WriteFrameBuffer(pW->ctx, pmyimage, pW->Config.WindowConfig.left, pW->Config.WindowConfig.top,oldwidth,oldheight);
		WEBC_FREE(pmyimage);
		FB_EndDrawing(pW->ctx);
		// EndDraw
		return;
	}

	// If WF_SAVEUNDER is set, "erase" our window by restoring the previous contents.
	if (pW->Config.WindowConfig.WindowFlags & (WF_SAVEUNDER|WF_SPRITE) && pW->SaveUnderData)
		FBWIN_RestorSaveUnder(pW);
	// Copy the window so we can save the coordinates.
	oldW = *pW;
	// Find the highest z-order window that surrounds us
	// Can't pass oldW cause not in the z-list
	surroundingW = FBWIN_FindContainingWindow(pW);

	// Change coordinates
	pW->Config.WindowConfig.left   = left;
	pW->Config.WindowConfig.top	   = top;
	pW->Config.WindowConfig.width  = width;
	pW->Config.WindowConfig.height = height;
	FBWIN_ClipToRoot(pW);

	// If WF_SAVEUNDER is set save the underlying screen area before we redraw ourselves
	if (pW->Config.WindowConfig.WindowFlags & (WF_SAVEUNDER|WF_SPRITE))
	{
		FBWIN_SaveUnder(pW);
		FBWM_DrawWindowAndContent(pW);
		return;
	}

	if (SizeChanged)
	{ // Size is changed.
		// If the new position completely overlaps the new window just draw because no invalid regios are formed
		if (FBWIN_WinaContainsWinb(pW, &oldW))
		{
			FBWM_DrawWindowAndContent(pW);
			return;
		}
	}
	// Repaint all of the windows that overlap this window and all windows that overlap them
	if (!surroundingW)
		surroundingW = pW;
	FBWIN_ClearPaintList();
	surroundingW->ScheduleRedraw = WEBC_TRUE;
	FBWIN_DrawZlist(surroundingW);
	FBWIN_DrawPaintList();
}

// Clear "must paint" status for all windows.
static void FBWIN_ClearPaintList(void)
{
FBWMWin * pW;
	pW = pWRoot;
	while (pW)
	{
		pW->ScheduleRedraw = WEBC_FALSE;
		pW = pW->pnext;
	}
}

// Draw all windows on the paint list
static void FBWIN_DrawPaintList(void)
{
FBWMWin * pW;
	pW = pWRoot;
	while (pW)
	{
		if (pW->ScheduleRedraw)
			FBWM_DrawWindowAndContent(pW);
		pW = pW->pnext;
	}
}

// Set ScheduleRedraw status for all of the windows higher in the z list that intersect with it
static void FBWIN_DrawZlist(FBWMWin * pW)
{
FBWMWin * _pW;
	_pW = pW->pnext;
	while (_pW)
	{
		if (FBWIN_WinaOverlapsWinb(pW, _pW))
		{	// Propogate the paint if we haven't already visited this window
			if (!_pW->ScheduleRedraw)
			{
				_pW->ScheduleRedraw = WEBC_TRUE;
				FBWIN_DrawZlist(_pW);
			}
		}
		_pW = _pW->pnext;
	}
}



void _DrawBorder (WebRect *rect, HTMLGraphicsContext** ctx,WEBC_UINT16 style[4],DISPLAY_INT border[4],WebColor color[4], WebColor lightColor[4], WebColor darkColor[4], DISPLAY_INT Radius[4]);

static WebColor FBWIN_RGBToColor(HTMLGraphicsContext **ctx, WEBC_UINT16 red, WEBC_UINT16 green, WEBC_UINT16 blue,WEBC_UINT16 alpha)
{
HTMLColor			  error;
HTMLColor			  color;
	color = HTML_RGBAToColor(red,green,blue,alpha);
	return FB_RGBToColor(ctx, color, &error);
}
static WebColor FBWIN_RGBToLightColor (HTMLGraphicsContext **ctx,WEBC_UINT8 red, WEBC_UINT8 green, WEBC_UINT8 blue,WEBC_UINT8 alpha)
{
	return (FBWIN_RGBToColor(ctx,((WEBC_UINT16)red + 720) >> 2,
	                   ((WEBC_UINT16)green + 720) >> 2,
	                   ((WEBC_UINT16)blue + 720) >> 2,
	                   alpha));
}



static WebColor FBWIN_RGBToLightColor (HTMLGraphicsContext **ctx,HTMLColor rgb)
{
	return (FBWIN_RGBToLightColor (
			ctx,
			HTML_ColorGetRed(rgb),
			HTML_ColorGetGreen(rgb),
			HTML_ColorGetBlue(rgb),
			HTML_ColorGetAlpha(rgb)
		));
}


static WebColor FBWIN_RGBToDarkColor (HTMLGraphicsContext **ctx,WEBC_UINT8 red, WEBC_UINT8 green, WEBC_UINT8 blue,WEBC_UINT8 alpha)
{
	return (FBWIN_RGBToColor(ctx,(WEBC_UINT16)red*4/7, (WEBC_UINT16)green*4/7, (WEBC_UINT16)blue*4/7, alpha));
}


static WebColor FBWIN_RGBToDarkColor (HTMLGraphicsContext **ctx,HTMLColor rgb)
{
	return (FBWIN_RGBToDarkColor (
			ctx,
			HTML_ColorGetRed(rgb),
			HTML_ColorGetGreen(rgb),
			HTML_ColorGetBlue(rgb),
			HTML_ColorGetAlpha(rgb)
		));
}




static void FBWIN_SaveUnder(FBWMWin *pW)
{
WEBC_UINT8 *pdata;
DISPLAY_INT width,height;

	if (pW->SaveUnderData)
	{
		WEBC_FREE(pW->SaveUnderData);
		pW->SaveUnderData=0;
	}

	width = pW->Config.WindowConfig.width;
	height =pW->Config.WindowConfig.height;

	pdata = (WEBC_UINT8*)WEBC_MALLOC_VERBOSE(height*width*BYTES_PER_PIXEL,"SaveUnder");
	if (pdata)
	{
		pW->SaveUnderData = pdata;
		FB_ReadFrameBuffer(pW->ctx,pdata,pW->Config.WindowConfig.left,	pW->Config.WindowConfig.top, width,	height);
	}
}


static void FBWIN_RestorSaveUnder(FBWMWin *pW)
{
	if (pW->SaveUnderData)
	{
		FB_BeginDrawing(pW->ctx);
		FB_WriteFrameBuffer(pW->ctx, pW->SaveUnderData, pW->Config.WindowConfig.left, pW->Config.WindowConfig.top,
	 		pW->Config.WindowConfig.width,
	 		pW->Config.WindowConfig.height);
		FB_EndDrawing(pW->ctx);
		WEBC_FREE(pW->SaveUnderData);
		pW->SaveUnderData=0;
	}
}


static void FBWIN_DrawCanvas(void *_pW)
{
	HTMLRect				cliprect,cliprectSave;
	FBWMWin *pW = (FBWMWin *)_pW;

	FB_GetClipRect(pW->ctx, &cliprect);
	cliprectSave=cliprect;
	// Clear the canvas area, don't overwrite the borders
	cliprect.left = pW->Config.WindowConfig.left+pW->Config.WindowConfig.BorderThickness;
	cliprect.top = pW->Config.WindowConfig.top+pW->Config.WindowConfig.BorderThickness;
	cliprect.right  = cliprect.left+pW->Config.WindowConfig.width-(2*pW->Config.WindowConfig.BorderThickness)-1;
	cliprect.bottom = cliprect.top+pW->Config.WindowConfig.height-(2*pW->Config.WindowConfig.BorderThickness)-1;
	FB_SetClipRect(pW->ctx, &cliprect);

	FB_drawRectangle(pW->ctx,
		(DISPLAY_INT) pW->Config.WindowConfig.left+pW->Config.WindowConfig.BorderThickness,
		(DISPLAY_INT) pW->Config.WindowConfig.top+pW->Config.WindowConfig.BorderThickness,
		(DISPLAY_INT) pW->Config.WindowConfig.width-(2*pW->Config.WindowConfig.BorderThickness),
		(DISPLAY_INT) pW->Config.WindowConfig.height-(2*pW->Config.WindowConfig.BorderThickness),
		0,
		true,
		pW->Config.WindowConfig.BackGroundColor, // outlineColor,
		pW->Config.WindowConfig.BackGroundColor);  // fillColor,
	FB_SetClipRect(pW->ctx, &cliprectSave);
//	FB_EndDrawing(pW->ctx);
}

static void FBWIN_DrawBorder(void *_pW)
{
WEBC_UINT16 style[4];
DISPLAY_INT border[4];
DISPLAY_INT radius[4];
WebColor color[4], lightcolor[4], darkcolor[4];
WebRect rect;

	HTMLRect				cliprect,cliprectSave;
	FBWMWin *pW = (FBWMWin *)_pW;
	if (pW->Config.WindowConfig.width < 1 || pW->Config.WindowConfig.height<1)
		return;

//	FB_BeginDrawing(pW->ctx);

	FB_GetClipRect(pW->ctx, &cliprect);
	cliprectSave=cliprect;

//	pW->Config.BorderStyle;

	rect.left   = pW->Config.WindowConfig.left;
	rect.top    = pW->Config.WindowConfig.top;
	rect.right  = pW->Config.WindowConfig.left+pW->Config.WindowConfig.width-1;
	rect.bottom = pW->Config.WindowConfig.top+pW->Config.WindowConfig.height-1;

	lightcolor[0]=lightcolor[1]=lightcolor[2]=lightcolor[3]=0;
	darkcolor[0]=darkcolor[1]=darkcolor[2]=darkcolor[3]=0;
	if (pW->Config.WindowConfig.BorderStyle & BS_RAISED)
		style[0]=style[1]=style[2]=style[3]=HCBOX_STYLE_OUTSET;
	else if (pW->Config.WindowConfig.BorderStyle & BS_LOWERED)
		style[0]=style[1]=style[2]=style[3]=HCBOX_STYLE_INSET;
	else if (pW->Config.WindowConfig.BorderStyle & BS_RIDGE)
	{
		style[0]=style[1]=style[2]=style[3]=HCBOX_STYLE_RIDGE;
		lightcolor[0]=lightcolor[1]=lightcolor[2]=lightcolor[3]=FBWIN_RGBToLightColor (pW->ctx,(HTMLColor)(pW->Config.WindowConfig.BorderColor&0xffffff));
		darkcolor[0]=darkcolor[1]=darkcolor[2]=darkcolor[3]=FBWIN_RGBToDarkColor (pW->ctx,(HTMLColor)(pW->Config.WindowConfig.BorderColor&0xffffff));
	}
	else if (pW->Config.WindowConfig.BorderStyle & BS_GROOVE)
	{
		style[0]=style[1]=style[2]=style[3]=HCBOX_STYLE_GROOVE;
		lightcolor[0]=lightcolor[1]=lightcolor[2]=lightcolor[3]=FBWIN_RGBToDarkColor (pW->ctx,(HTMLColor)(pW->Config.WindowConfig.BorderColor&0xffffff));
		darkcolor[0]=darkcolor[1]=darkcolor[2]=darkcolor[3]=FBWIN_RGBToLightColor (pW->ctx,(HTMLColor)(pW->Config.WindowConfig.BorderColor&0xffffff));
	}
	else /* Default to something */
		style[0]=style[1]=style[2]=style[3]=HCBOX_STYLE_OUTSET;

	color[0] = color[1] = color[2] = color[3] = pW->Config.WindowConfig.BorderColor;
	radius[0] = radius[1] = radius[2] = radius[3] = 0;
	border[0] = border[1] = border[2] = border[3] = pW->Config.WindowConfig.BorderThickness;
	_DrawBorder (&rect, pW->ctx, style, border, color, lightcolor, darkcolor, radius);
}


#define WINDOW_RIGHT(W) W->Config.WindowConfig.left + W->Config.WindowConfig.width-1
#define WINDOW_BOTTOM(W) W->Config.WindowConfig.top + W->Config.WindowConfig.height-1

/* Test if two windows overlap */
static WEBC_BOOL FBWIN_WinaOverlapsWinb(FBWMWin *pWa, FBWMWin *pWb)
{
	if (WINDOW_RIGHT(pWa) < pWb->Config.WindowConfig.left)
		goto noOverlap;
	if (WINDOW_BOTTOM(pWa)< pWb->Config.WindowConfig.top)
		goto noOverlap;
	if (WINDOW_RIGHT(pWb) < pWa->Config.WindowConfig.left)
		goto noOverlap;
	if (WINDOW_BOTTOM(pWb) < pWa->Config.WindowConfig.top)
		goto noOverlap;
	return WEBC_TRUE;

noOverlap:
	return WEBC_FALSE;
}

// Create an HtmlRect from a windows position, h and w.
static void FBWIN_WinToHtmlRect(FBWMWin *pW, HTMLRect *r)
{
	r->right = WINDOW_RIGHT(pW);
	r->bottom = WINDOW_BOTTOM(pW);
	r->left = pW->Config.WindowConfig.left;
	r->top  = pW->Config.WindowConfig.top;
}

/* Test if any higer z-order windows overlap this window */
static WEBC_BOOL FBWIN_CheckIfOverlapped(FBWMWin *pW)
{
	while (pW && pW->pnext)
	{
		if (FBWIN_WinaOverlapsWinb(pW->pnext,pW))
			return WEBC_TRUE;
		pW = pW->pnext;
	}
	return WEBC_FALSE;
}


/* Test if one Windows fully surounds another Window */
static WEBC_BOOL FBWIN_WinaContainsWinb(FBWMWin *pWa, FBWMWin *pWb)
{
	if (
		pWb->Config.WindowConfig.left >= pWa->Config.WindowConfig.left &&
		WINDOW_RIGHT(pWb) <= WINDOW_RIGHT(pWa)						   &&
		pWb->Config.WindowConfig.top >= pWa->Config.WindowConfig.top   &&
		WINDOW_BOTTOM(pWb) <= WINDOW_BOTTOM(pWa)					   )
	{
		return WEBC_TRUE;
	}
	return WEBC_FALSE;
}


/* Find the highest z-order window to fully contain this window */
static FBWMWin *FBWIN_FindContainingWindow(FBWMWin *pWtoTest)
{
FBWMWin *pW;
FBWMWin *pWresult=0;

	pW = pWRoot;
	while (pW)
	{
		if (pW==pWtoTest)
			break;
		if (FBWIN_WinaContainsWinb(pW, pWtoTest))
			pWresult = pW;
		pW = pW->pnext;
	}
	return pWresult;
}



#define HTMLRECT_OVERLAP(A,B) ((A->top <= B->bottom) && (A->bottom >= B->top) && (A->left <= B->right) && (A->right >= B->left))
inline static void HTMLRECT_AND(HTMLRect *to, HTMLRect *fr)
{
	if (fr->top > to->top)
		to->top = fr->top;
	if (fr->left > to->left)
		to->left = fr->left;
	if (fr->right < to->right)
		to->right = fr->right;
	if (fr->bottom < to->bottom)
		to->bottom = fr->bottom;
}

inline static void HTMLRECT_NOT_UNION(HTMLRect *to, HTMLRect *fr)
{
	if ((to->left <= fr->right) && (to->right >= fr->left))
	{
		if (to->left >= fr->left)
			to->left = fr->right+1;
		else
			to->right = fr->left-1;
	}
	if ( (to->top <= fr->bottom) && (to->bottom >= fr->top))
	{
		if (to->top >= fr->top)
			to->top = fr->bottom+1;
		else
			to->bottom = fr->top-1;
	}
}



// Check if a clipping region is overlapped by any regions in the window's clip list
// If so, modify inClipRect, reducing it by any overlapping regions in the window's ClipListVector.
// Return -1 if inClipRect becomes totally obscured
static int _FBWIN_ClipByWindowClipList(FBWMWin *pW, HTMLRect *inClipRect)
{
vector_iterator vi;
WEBC_PFBYTE p;
int returnvalue = 0;

	p = vector_get_first(&pW->WinClipListVector, &vi);
	while (p)
	{
	HTMLRect *pClipRect;
		pClipRect = (HTMLRect *) p;
		if (HTMLRECT_OVERLAP(inClipRect, pClipRect))
		{
			HTMLRECT_NOT_UNION(inClipRect, pClipRect);
			if (inClipRect->top > inClipRect->bottom || inClipRect->left > inClipRect->right)
			{
				return -1;
			}
		}
		p = vector_get_next(&vi);
	}
	return 0;
}

int FBWIN_ClipByWindowClipList(HTMLGraphicsContext **ctx, HTMLRect *inClipRect)
{
	FBWMWin *pW = (FBWMWin *) (*((FBGraphicsContext *)ctx)).windowHandle;
	if (pW)
		return _FBWIN_ClipByWindowClipList(pW, inClipRect);
	else
		return 0;
}

// For a given window build a list of retangles from other windows that obscure any of it.
static void FBWIN_BuildWindowClipList(FBWMWin *pWtoProcess)
{
FBWMWin *pW;
HTMLRect rectToProcess;

	vector_delete(&pWtoProcess->WinClipListVector);
	pW=pWtoProcess;
	FBWIN_WinToHtmlRect(pWtoProcess, &rectToProcess);
	while (pW->pnext)
	{
	HTMLRect OverLap;
		FBWIN_WinToHtmlRect(pW->pnext, &OverLap);
		if (HTMLRECT_OVERLAP((&rectToProcess), (&OverLap)))
		{
			HTMLRECT_AND(&OverLap, &rectToProcess);
			vector_add_node_to_rear(&pWtoProcess->WinClipListVector, (WEBC_PFBYTE) &OverLap);
		}
		pW = pW->pnext;
	}
}

// Build a list of of obscuring retangles from other windows that obscure them for each window in the z-order stack
// Called when the order is changed.
static void FBWIN_BuildAllWindowClipLists(void)
{
FBWMWin *_pW = pWRoot;
	while (_pW)
	{
		FBWIN_BuildWindowClipList(_pW);
		_pW = _pW->pnext;
	}
}



static void FBWIN_ClipToRoot(FBWMWin *pW)
{
	// Change coordinates
	if (pW->Config.WindowConfig.left < pWRoot->Config.WindowConfig.left)
		pW->Config.WindowConfig.left = pWRoot->Config.WindowConfig.left;
	if (pW->Config.WindowConfig.top < pWRoot->Config.WindowConfig.top)
		pW->Config.WindowConfig.top = pWRoot->Config.WindowConfig.top;
	if (WINDOW_RIGHT(pW) > WINDOW_RIGHT(pWRoot))
	{
		pW->Config.WindowConfig.width=WINDOW_RIGHT(pWRoot)-pW->Config.WindowConfig.left+1;
		if (pW->Config.WindowConfig.width > (2*pW->Config.WindowConfig.BorderThickness))
			pW->Config.WindowConfig.width -= (2*pW->Config.WindowConfig.BorderThickness);
	}
	if (WINDOW_BOTTOM(pW) > WINDOW_BOTTOM(pWRoot))
	{
		pW->Config.WindowConfig.height=WINDOW_BOTTOM(pWRoot)-pW->Config.WindowConfig.top+1;
		if (pW->Config.WindowConfig.height > (2*pW->Config.WindowConfig.BorderThickness))
			pW->Config.WindowConfig.height -= (2*pW->Config.WindowConfig.BorderThickness);
	}
}

#endif /* WINDOWSGDI	*/
