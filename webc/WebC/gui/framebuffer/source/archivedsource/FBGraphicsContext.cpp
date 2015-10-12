#include "webc.h"
#include "webcmem.h"
#include "FBConfig.h"
#include "FBGraphicsContext.h"

FBGraphicsContext::FBGraphicsContext(int width, int height)
{
	clientDim.left	= (DISPLAY_INT)0;
	clientDim.top	 = (DISPLAY_INT)0;
	clientDim.right  = (DISPLAY_INT)width;
	clientDim.bottom = (DISPLAY_INT)height;

	windowHdl = 0;

	frameBufferItf = WEBC_NULL;
	currentGBuffer = 0;
	clipBounds.bottom	= (DISPLAY_INT) (VERTICAL_RESOLUTION -1); //239;
	clipBounds.top		= (DISPLAY_INT)0;
	clipBounds.left		= (DISPLAY_INT)0;
	clipBounds.right	= (DISPLAY_INT)(HORIZONTAL_RESOLUTION -1); //319;

	invalidRect.bottom = clientDim.bottom;
	invalidRect.top	 = clientDim.top;
	invalidRect.left	= clientDim.left;
	invalidRect.right  = clientDim.right;

	drawingWindow  = false;
	transparentPen = false;
}
FBGraphicsContext::FBGraphicsContext(HTMLBrowserConfig *config, int width, int height)
{
	clientDim.left	= config->WindowConfig.left;
	clientDim.top	 = config->WindowConfig.top;
	clientDim.right  = config->WindowConfig.left+config->WindowConfig.width;
	clientDim.bottom = config->WindowConfig.top+config->WindowConfig.height;
	windowHdl = 0;

	frameBufferItf = WEBC_NULL;
	currentGBuffer = 0;

	clipBounds.bottom	= (DISPLAY_INT) (VERTICAL_RESOLUTION -1); //239;
	clipBounds.top		= (DISPLAY_INT)0;
	clipBounds.left		= (DISPLAY_INT)0;
	clipBounds.right	= (DISPLAY_INT)(HORIZONTAL_RESOLUTION -1); //319;

	invalidRect.bottom = clientDim.bottom;
	invalidRect.top	 = clientDim.top;
	invalidRect.left	= clientDim.left;
	invalidRect.right  = clientDim.right;

	drawingWindow  = false;
	transparentPen = false;
}
