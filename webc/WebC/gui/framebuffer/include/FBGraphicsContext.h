#pragma once

#include "webc.h"
#include "FB_HWInterface.h"


//This struct is used by the graphics layer of Webc to contain information needed by browser and graphics layer
class FBGraphicsContext
{
public:
	FBGraphicsContext(int width, int height);
	FBGraphicsContext(HTMLBrowserConfig *config, int width, int height);
	HTMLGraphicsContext *graphicsLayerCalls;
	WEBC_UINT8			 *browserHandle; //handle to the window for the graphics
	WEBC_UINT8			 *windowHandle;
	int					  drawNesting; //amount of draws needed to perform


#if (RENDERRGB24||RENDERRGBA)
	WEBC_UINT8			redPen, bluePen, greenPen, opacity, hasopacity; //the values of teh drawing color
#elif (RENDER8BITINDEX)
	WEBC_UINT8			grayPen; //the values of the drawing color
#endif
	bool					transparentPen;				//whether the drawing color is set to tranparent
	//WebGraphBitmapInfo *clientDisplayBuffer;		//the buffer for the client drawing surface
	FBHWInterface *frameBufferItf;

	WEBC_UINT8 *windowHdl;								//the window handle of a window that this graphics is based on
	HTMLRect	invalidRect;							  //what needs to get redrawn
	HTMLRect	clipBounds;								//clipRegions for drawing
	HTMLRect	clientDim;								 //dimensions of the client area

	bool drawingWindow;
	int  currentGBuffer;

#ifdef FB_FONTS_STORED_IN_FILES
	// variables for maintaining the cache of loaded fonts
	unsigned int fontCacheCurrentMemory, fontCacheMaxMemory, fontCacheCurrentUsageIndex;
#endif
};
