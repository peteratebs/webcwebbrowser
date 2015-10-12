/*
|  FB_GRAPHICS.CPP -
|
|	This is the graphics class for the WebC Graphics Layer. What this does 
|   is it creates a surface based on the constructor to be drawn on.  It also 
|   offers various methods to draw onto the surface.
|
|  EBS -
|
|   Author: Peter
|   Date: July 29, 2008
|
|  Copyright EBS Inc. , 2008
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
|
|	Description: This file is meant to represent a graphics object for a frame buffer.  
|   This file will use structures and function pointers to attempt to mimic OO style programming.
|
*/

extern "C" unsigned long getVideoBaseAddr(int which);

#include "webcmem.h"
#include "FB_Window_Manager.h"
#include "FBAbstraction.h"

// AK_HACK
#define IAR_LPC_2478_STK
#include "board.h"

#define BYTES_PER_PIXEL 4

typedef struct tagWEBC_RGBQUAD {
        WEBC_UCHAR8    rgbBlue;
        WEBC_UCHAR8    rgbGreen;
        WEBC_UCHAR8    rgbRed;
        WEBC_UCHAR8    rgbReserved;
} WEBC_RGBQUAD;
typedef WEBC_RGBQUAD *LPWEBC_RGBQUAD;

/**
*	This constructor is used when creating a graphics class that represents a window.
*	Initialization of all the graphic variables are done.
*
*	Params: windowHandle - the window the graphics will be based on	
*/
FB_Graphics::FB_Graphics(WEBC_UINT8* windowHandle){
	HTMLRect windowDim;
	getClientDimensions(windowHandle,&clientDim);
	getWindowDimensions(windowHandle,&windowDim);
	windowHdl = windowHandle;
	
	//Bitmap used to display the interior of the client portion of the window
	clientDisplayBuffer = (WebGraphBitmapInfo*)WEBC_MALLOC(sizeof(WebGraphBitmapInfo));
	clientDisplayBuffer->bHasTransparentColor = false;
	clientDisplayBuffer->pixelFormat = WB_PIXEL_FORMAT_RGB_24;//this will need to be changed to accomadate different color depths
	clientDisplayBuffer->iHeight =  (clientDim.bottom - clientDim.top + 1);
	clientDisplayBuffer->iWidth = (clientDim.right - clientDim.left + 1);
	clientDisplayBuffer->iBytesPerRow = clientDisplayBuffer->iWidth * BYTES_PER_PIXEL;
//	clientDisplayBuffer->pData = (WEBC_UINT8*)WEBC_MALLOC((clientDisplayBuffer->iBytesPerRow) * (clientDisplayBuffer->iHeight));
	//AK_HACK	
        clientDisplayBuffer->pData = (WEBC_UINT8*)getVideoBaseAddr(0);
		m_currentGBuffer = 0;
//END AK_HACK	
	rtp_memset((void*)clientDisplayBuffer->pData, 0xFF ,(clientDisplayBuffer->iBytesPerRow ) * (clientDisplayBuffer->iHeight));
	
	//Bitmap used to display the interior of the window frame portion of the window
#if(BROWSER_FRAME)
	windowDisplayBuffer = (WebGraphBitmapInfo*)WEBC_MALLOC(sizeof(WebGraphBitmapInfo));
	windowDisplayBuffer->bHasTransparentColor = false;
	windowDisplayBuffer->pixelFormat = WB_PIXEL_FORMAT_RGB_24;//this will need to be changed to accomadate different color depths
	windowDisplayBuffer->iHeight =  (windowDim.bottom - windowDim.top + 1);
	windowDisplayBuffer->iWidth = (windowDim.right - windowDim.left + 1);
	windowDisplayBuffer->iBytesPerRow = windowDisplayBuffer->iWidth * BYTES_PER_PIXEL;
	windowDisplayBuffer->pData = (WEBC_UINT8*)WEBC_MALLOC((windowDisplayBuffer->iBytesPerRow) * (windowDisplayBuffer->iHeight));
	memset((void*)windowDisplayBuffer->pData, 0xFF ,(windowDisplayBuffer->iBytesPerRow ) * (windowDisplayBuffer->iHeight));
#else
	windowDisplayBuffer = WEBC_NULL;
#endif
//	clipBounds.bottom = 4000000;
//	clipBounds.top = -4000000;
	//clipBounds.left = -4000000;
	//clipBounds.right = 4000000;
// AK_HACK
	clipBounds.bottom = 240;
	clipBounds.top = 0;
	clipBounds.left = 0;
	clipBounds.right = 320;
	
	invalidRect.bottom = clientDim.bottom;
	invalidRect.top = clientDim.top;
	invalidRect.left = clientDim.left;
	invalidRect.right = clientDim.right;

	drawingWindow = false;
	transparentPen = false;
#if(BROWSER_FRAME)
	loadWindowFrame();
#endif
}

/**
*	This constructor is used when a surface needs to be created to draw on without a window
*	
*	Params: width - the width of the surface, height - the height of the surface, mode - the color depth mode of 
*           the surface
*/
FB_Graphics::FB_Graphics(DISPLAY_INT width, DISPLAY_INT height, int mode){
	windowHdl = WEBC_NULL;
	clientDisplayBuffer = (WebGraphBitmapInfo*)WEBC_MALLOC(sizeof(WebGraphBitmapInfo));
	clientDisplayBuffer->bHasTransparentColor = false;
	clientDisplayBuffer->pixelFormat = WB_PIXEL_FORMAT_RGB_24;//this will need to be changed to accomadate different color depths
	clientDisplayBuffer->iHeight =  height;
	clientDisplayBuffer->iWidth = width;
	clientDisplayBuffer->iBytesPerRow = clientDisplayBuffer->iWidth * BYTES_PER_PIXEL;
//AK_HACK	
        clientDisplayBuffer->pData = (WEBC_UINT8*)getVideoBaseAddr(0);
		m_currentGBuffer = 0;
//END AK_HACK	
        rtp_memset((void*)clientDisplayBuffer->pData,0xFF,(clientDisplayBuffer->iBytesPerRow * clientDisplayBuffer->iHeight));
	windowDisplayBuffer = WEBC_NULL;
	clipBounds.bottom = height - 1;
	clipBounds.top = 0;
	clipBounds.left = 0;
	clipBounds.right = width - 1;
	
	invalidRect.bottom = 0;
	invalidRect.top = 0;
	invalidRect.left = 0;
	invalidRect.right = 0;
	
	drawingWindow = false;
	transparentPen = false;
}

/**
*	Destructor, frees the memory used by the graphics class
*/	
FB_Graphics::~FB_Graphics(){
	WEBC_FREE((void*)clientDisplayBuffer->pData);	
	WEBC_FREE((void*)clientDisplayBuffer);
	if(windowDisplayBuffer){
		if(windowDisplayBuffer->pData){
			WEBC_FREE((void*)windowDisplayBuffer->pData);
		}
		WEBC_FREE((void*)windowDisplayBuffer);
	}
	
}

/**
*	Sets the color for the next element to be drawn. 
*
*   Param: color - a 32 bit color where the color values are as such transparentRedGreenBlue
*/
void FB_Graphics::setColor(
	WEBC_UINT32 color
){
	WEBC_UINT32 redmask,greenmask,bluemask;
	redmask = 0x00ff0000;
	greenmask = 0x0000ff00;
	bluemask = 0x000000ff;
	redPen = (WEBC_UINT8)((color&redmask)>> (16));//shift over 16 bits to remove excess
	greenPen = (WEBC_UINT8)((color&greenmask)>>(8));//shift over 8 bits to remove excess
	bluePen = (WEBC_UINT8)(color&bluemask);
}

/**
*	Sets the color for the next element to be drawn.
*
*	Param: red - amount of red, green - amount of green, blue - amount of blue
*/
void FB_Graphics::setColor(
	WEBC_UINT8 red,
	WEBC_UINT8 green,
	WEBC_UINT8 blue
){
	redPen = red;
	greenPen = green;
	bluePen = blue;
}

/**
*	Indicates that the next element to be drawn will be transparent or just not drawn.
*/
void FB_Graphics::setTransparency(){
	transparentPen = true;
}


/**
*	This method is used to draw a horizontal line on the graphics surface
*
*	Params: xPos - x position of the line, yPos - y position of the line, 
*			length - the length of the line
*/
void FB_Graphics::drawHorizontalLine( 
	DISPLAY_INT xPos,
	DISPLAY_INT yPos,
	DISPLAY_INT length
){
	//xPos += DRAW_OFFSET_X;
	//yPos += DRAW_OFFSET_Y;
	WEBC_UINT8* colPtr;
	//Draw info off screen frame buffer to be displayed later
	//xPos * 3 represents that there are 3 bytes to every pixel and there for to get to the right pixel we must multiply by three
	if(!transparentPen){

		if(xPos > clipBounds.right || yPos > clipBounds.bottom || yPos < clipBounds.top)
			return;
		if(xPos < clipBounds.left){
			length = length - (clipBounds.left - xPos + 1);
			xPos = clipBounds.left;
		}
		
		if(length > (clipBounds.right - xPos + 1))
			length = clipBounds.right - xPos + 1;	
		

		if(drawingWindow){
				colPtr = (WEBC_UINT8*) windowDisplayBuffer->pData + ((yPos*windowDisplayBuffer->iBytesPerRow) + (xPos * 4));
		}
		else{
			colPtr = (WEBC_UINT8*) clientDisplayBuffer->pData + ((yPos*clientDisplayBuffer->iBytesPerRow) + (xPos * 4));
		}
		for(int i = length; i > 0 ; i--, colPtr += 4){
			colPtr[0] = redPen;
			colPtr[1] = greenPen;
			colPtr[2] = bluePen;
		}
	}
	else{
		transparentPen = false;
	}
}


/**
*	Used to draw a rectangle on the graphics surface.
*
*	Params: x - left side position, y - top position, width - width of the rectangle
*			height - height of the rectangle, fill - whether the rectangle will be filled, 
*			lineColor - the color of the line that will border the rectangle,
*			fillColor - the color of the interior of the rectangle if it is filled
*/
void FB_Graphics::drawRectangle(
		DISPLAY_INT x,
		DISPLAY_INT y,
		DISPLAY_INT width,
		DISPLAY_INT height,
		bool fill,
		WEBC_UINT32 lineColor,
		WEBC_UINT32 fillColor
){
	if(fill){
		setColor(fillColor);
		for(int j = y; j < y + height; j++){
			drawHorizontalLine(x, j, width);
		}
	}
	if(!fill || lineColor != fillColor){
		setColor(lineColor);
		//top line
		drawHorizontalLine(x,y,width);
		//sides
		for(int k = y + 1; k < y + height- 1; k++){
			drawHorizontalLine(x , k , 1);
			drawHorizontalLine(x + width, k , 1);
		}		
		//bottom line
		drawHorizontalLine(x,y+height-1, width);

	}
}

/**
*	Used to draw a bitmap on the graphics surface. This method is used when there
*   does not need to be a restriction on the width and height of a bitmap
*
*	Params: bitmapPtr - bitmap to be drawn, dispX - the x position of where
*           the bitmap must be drawn, dispY - the y position of where the 
*			bitmap must be drawn.
*/
void FB_Graphics::drawBitmap(
	WebGraphBitmapInfo* bitmapPtr, 
	DISPLAY_INT dispX, 
	DISPLAY_INT dispY
){
	drawBitmap(bitmapPtr, dispX, dispY, bitmapPtr->iWidth, bitmapPtr->iHeight);
}

/**
*	Used to draw a bitmap on the graphics surface. This method is used when there
*   needs to be a restriction on the width and height of a bitmap
*
*	Params: bitmapPtr - bitmap to be drawn, dispX - the x position of where
*           the bitmap must be drawn, dispY - the y position of where the 
*			bitmap must be drawn, dispWidth - the width of the bitmap, 
*			dispHeight - the height of the bitmap
*/
void FB_Graphics::drawBitmap(
	WebGraphBitmapInfo* bitmapPtr, 
	DISPLAY_INT dispX, 
	DISPLAY_INT dispY,
	DISPLAY_INT dispWidth,
	DISPLAY_INT dispHeight
){
	DISPLAY_INT width,height;
	width = dispWidth;
	height = dispHeight;

	WEBC_UINT8 transRed,transBlue,transGreen,transIndex;
	if(bitmapPtr->bHasTransparentColor){
		transRed = bitmapPtr->uTransparentRed;
		transGreen = bitmapPtr->uTransparentGreen;
		transBlue = bitmapPtr->uTransparentBlue;
		transIndex = bitmapPtr->iTransparentIndex;
	}
	const WEBC_UINT8* rowDataPtr = (bitmapPtr->pData);
	int paletteSizeNeeded = (bitmapPtr->pixelFormat == WB_PIXEL_FORMAT_INDEX_8)? bitmapPtr->iPaletteSize : 0;
	
	int length = 0;
	int startx = 0;
	const WEBC_UINT8 *current,*next;
	if (bitmapPtr->pixelFormat == WB_PIXEL_FORMAT_RGB_24)
	{	
	  //if(bitmapPtr->bHasTransparentColor)
	  if(1)
	  {
		for (int y = 0; y < height; y++, rowDataPtr += (bitmapPtr->iBytesPerRow))
		{
			current = rowDataPtr;
			next = current + 3;
			length = 1;
			startx = 0;
			for (int x = 0; x < width; x++)
			{
				if(next[0] != current[0] ||
					next[1] != current[1] ||
					next[2] != current[2] ||
					x == width - 1){
						if(bitmapPtr->bHasTransparentColor &&
							current[0] == transRed &&
							current[1] == transGreen &&
							current[2] == transBlue){
								setTransparency();
						}
						else{
							setColor(current[0],current[1],current[2]);
						}
						
						drawHorizontalLine(dispX + startx, dispY + y,length);
						length = 1;
						startx = x + 1;
				}
				else{
					length += 1;
				}
				current += 3;
				if(x != width - 1){
					next += 3;
				}
			}
		}
	  }
	  else // faster bit-blit loop
	  {
		WEBC_SIZE srcRowBytes = bitmapPtr->iBytesPerRow;
		WEBC_SIZE dstRowBytes = clientDisplayBuffer->iBytesPerRow;
		WEBC_UINT8 *srcDataPtr = (WEBC_UINT8*)rowDataPtr;
		WEBC_UINT8 *dstDataPtr = ((WEBC_UINT8*)clientDisplayBuffer->pData) + (dstRowBytes * dispY) + (4 * dispX);
		for (int y = 0; y < height; y++, srcDataPtr += srcRowBytes, dstDataPtr += dstRowBytes)
		{
			WEBC_UINT8 *inPtr = (WEBC_UINT8*)srcDataPtr;
			WEBC_UINT8 *outPtr = (WEBC_UINT8*)dstDataPtr;
		  	for (int x = 0; x < width; x++)
			{
			  *outPtr++ = *inPtr++;
			  *outPtr++ = *inPtr++;
			  *outPtr++ = *inPtr++;
			  outPtr++; // skip alpha
			}
		}
	  }
	}
	else if(bitmapPtr->pixelFormat == WB_PIXEL_FORMAT_INDEX_8)
	{
		int temp = sizeof(WEBC_RGBQUAD);
		WEBC_UINT8 index;
		WEBC_RGBQUAD* bmpColors = (WEBC_RGBQUAD*) WEBC_MALLOC(temp * bitmapPtr->iPaletteSize);
		if(!bmpColors){
			return;
		}
		
		int n;
		//printf("BITMAP ADDRESS: %X:\n", bitmapPtr->pData);
		for (n = 0; n < bitmapPtr->iPaletteSize; n++)
		{
			bmpColors[n].rgbRed   = bitmapPtr->pPalette[(n*3) + 0];
			bmpColors[n].rgbGreen = bitmapPtr->pPalette[(n*3) + 1];
			bmpColors[n].rgbBlue  = bitmapPtr->pPalette[(n*3)  + 2];
			bmpColors[n].rgbReserved = 0;
			//printf("INDEX: %d, RED: %d, GREEN: %d, BLUE: %d\n",n,bmpColors[n].rgbRed,bmpColors[n].rgbGreen,bmpColors[n].rgbBlue);
		}
		
		
		for (int y = 0; y < height; y++, rowDataPtr += (bitmapPtr->iBytesPerRow))
		{
			current = rowDataPtr;
			next = current + 1;
			length = 1;
			startx = 0;
			for (int x = 0; x < width; x++)
			{
				if(next[0] != current[0] ||	x == width - 1){
					index = current[0];
					if(bitmapPtr->bHasTransparentColor && current[0] == bitmapPtr->iTransparentIndex){
								setTransparency();
					}
					else{
						setColor(bmpColors[index].rgbRed,bmpColors[index].rgbGreen,bmpColors[index].rgbBlue);
					}
					drawHorizontalLine(dispX + startx, dispY + y,length);
					length = 1;
					startx = x + 1;
				}
				else{
					length += 1;
				}
				current += 1;
				if(x != width - 1){
					next += 1;
				}
			}
		}
		WEBC_FREE((void*)bmpColors);
		bmpColors = WEBC_NULL;
	}

}

/**
*	Sets the clipping region for the graphics surface.  
*	The clipping region is the bounding region where can not be drawn.
*
*	Params: rect - the new rect for the clipping region
*/
void FB_Graphics::setClipRect(
	HTMLRect rect
){
  return; // AK_HACK
	clipBounds.bottom = rect.bottom;
	clipBounds.top = rect.top;
	clipBounds.left = rect.left;
	clipBounds.right = rect.right;
}

/**
*	Gets the clipping region for the graphics surface. Returns the
*	bounding region for the non-drawing area.
*
*	Return: returns the rectangle representing where the class is 
*			limited in drawing.
*/
HTMLRect FB_Graphics::getClipRect(){
	return clipBounds;
}

/**
*	This method adds this rectangle to the overall region that
*   needs to be redrawn or redisplayed.
*
*	Params: rect - the rectangle to add to the overall region to be redrawn
*/
void FB_Graphics::addDirtyRect(
   HTMLRect passedRect
){  
  HTMLRect rect = passedRect;
	if(rect.bottom > invalidRect.bottom)
	{
		invalidRect.bottom = rect.bottom;
	}
	if(rect.right > invalidRect.right)
	{
		invalidRect.right = rect.right;
	}
	if(rect.left < invalidRect.left)
	{
		invalidRect.left = rect.left;
	}
	if(rect.top < invalidRect.top)
	{
		invalidRect.top = rect.top;
	}
	return;
}


/**
*	Returns the area that needs to be redrawn if beign displayed
*
*	Return: the area that needs to be redrawn
*/
HTMLRect FB_Graphics::getDirtyRect(){
	return invalidRect;
}

/**
*	Resets the invalid region indicating that all displays are up to date for 
*   this graphics class.
*/
void FB_Graphics::resetDirtyRect(){
	invalidRect.bottom = 0;
	invalidRect.left = 0;
	invalidRect.right = 0;
	invalidRect.top = 0;
}

/**
*	Returns the displayBuffer for this graphic, this is the client area display
*	so it does not include the bordering window.
*
*	Return: the display buffer that is being drawn on
*/
WebGraphBitmapInfo* FB_Graphics::getDisplayBuffer(){
		return clientDisplayBuffer;
}

/**
*	Returns the window buffer , meaning the bordering window graphic.
*
*	Return: returns the graphic representing the window to be drawn around a client
*/
WebGraphBitmapInfo* FB_Graphics::getWindowDisplayBuffer(){
		return windowDisplayBuffer;
}

/**
*	Gets the window that is associated with this graphics if a window is not associated
*	with this graphic NULL will be returned
*/
WEBC_UINT8* FB_Graphics::getWindowHandle(){
	return windowHdl;
}

/**
*	BMP data is stored upside down, internal format is right side up.  This
*   method is used to flip the image data of a BMP data into the internal format.
*/
void FB_Graphics::flipImage(WebGraphBitmapInfo* org){
	WEBC_UINT8* flppd = (WEBC_UINT8*)WEBC_MALLOC(org->iHeight * org->iBytesPerRow);
	WEBC_UINT8 *orgColPtr, *flpColPtr,*orgRowPtr, *flpRowPtr;
	
	for(int i = 0; i < org->iHeight ; i++){
		orgColPtr = orgRowPtr = (WEBC_UINT8*)(org->pData + (i * org->iBytesPerRow));
		flpColPtr = flpRowPtr = (WEBC_UINT8*)(flppd + ((org->iHeight - i -1) * org->iBytesPerRow));
		for(int j = 0; j < org->iWidth ; j++, flpColPtr += 3, orgColPtr += 3){
			flpColPtr[0] = orgColPtr[2];
			flpColPtr[1] = orgColPtr[1];
			flpColPtr[2] = orgColPtr[0];
		}
	}
	rtp_memcpy((void*)org->pData,(const void*)flppd, org->iHeight * org->iBytesPerRow);
	WEBC_FREE((void*)flppd);
}

/**
*	This method is used within the graphics class to load the graphics of the window frame into the
*   windowDisplayBuffer the graphics are stored in a folder under the framebuffer folder.
*/
void FB_Graphics::loadWindowFrame(){
	char leftline[100] = "..\\..\\..\\gui\\framebuffer\\FB_Window_Frame\\leftline.bmp";
	char titlebar[100] = "..\\..\\..\\gui\\framebuffer\\FB_Window_Frame\\titlebar.bmp";
	char rightline[100] = "..\\..\\..\\gui\\framebuffer\\FB_Window_Frame\\rightline.bmp";
	char bottomline[100] = "..\\..\\..\\gui\\framebuffer\\FB_Window_Frame\\bottomline.bmp";
	char corner[100] = "..\\..\\..\\gui\\framebuffer\\FB_Window_Frame\\corner.bmp";
	char *path;

	drawingWindow = true;

	DISPLAY_INT partWidth,partHeight;
	DISPLAY_INT x,y,width,height;
	int part;
	x = clientDim.left;
	y = clientDim.top;
	width = clientDim.right - clientDim.left + 1;
	height = clientDim.bottom - clientDim.top + 1;

	WebGraphBitmapInfo* bitmap;
	WebGraphBitmapInfo* stretchedBitmap;
	FBGraphicsContext* ctx = WEBC_NULL;

	int file;
	int offset;
	for(part = 0; part < 5; part++){
		switch(part){
			case 0:
				path = titlebar;
				partHeight = FRAME_TOP_HEIGHT;
				partWidth = width;
				x = FRAME_LEFT_WIDTH;
				y = 0;
				break;
			case 1: 
				path = leftline;
				partHeight = height + FRAME_TOP_HEIGHT;
				partWidth = FRAME_LEFT_WIDTH;
				x = y = 0;
				break;
			case 2:
				path = rightline;
				partHeight = height + FRAME_TOP_HEIGHT;
				partWidth = FRAME_RIGHT_WIDTH;
				x = width + FRAME_LEFT_WIDTH;
				y = 0;
				break;
			case 3:
				path = bottomline;
				partHeight = FRAME_BOTTOM_HEIGHT;
				partWidth = width + FRAME_LEFT_WIDTH;
				x = 0;
				y = FRAME_TOP_HEIGHT + height;
				break;
			case 4:
				path = corner;
				partHeight = FRAME_BOTTOM_HEIGHT;
				partWidth = FRAME_RIGHT_WIDTH;
				x = width + FRAME_LEFT_WIDTH;
				y = FRAME_TOP_HEIGHT + height;
				break;
			default:
				path = titlebar;
				partHeight = FRAME_TOP_HEIGHT;
				partWidth = width;
				x = FRAME_LEFT_WIDTH;
				y = 0;
				break;
		}
#if 0
// TODO Need to apply bintoc or something here
                
		file = _open(path,_O_BINARY, _O_RDWR);
		bitmap = (WebGraphBitmapInfo*)WEBC_MALLOC(sizeof(WebGraphBitmapInfo));
		lseek(file,10,SEEK_SET);
		read(file,&offset,4);
		lseek(file,4,SEEK_CUR);
		read(file,&bitmap->iWidth,4);
		read(file,&bitmap->iHeight,4);		

		bitmap->iBytesPerRow = ((bitmap->iWidth * 3) + 3) & ~3;
		bitmap->pixelFormat = WB_PIXEL_FORMAT_RGB_24;
		bitmap->bHasTransparentColor = false;
		bitmap->iPaletteSize = 0;
		bitmap->pData = (WEBC_UINT8*)WEBC_MALLOC(bitmap->iHeight * bitmap->iBytesPerRow );		

		lseek(file,offset,SEEK_SET);
		read(file,(void*)bitmap->pData,(bitmap->iHeight * bitmap->iBytesPerRow));
		flipImage(bitmap);	

		stretchedBitmap = (WebGraphBitmapInfo*)FB_CreateStretchedBitmap((HTMLGraphicsContext**)ctx, (void*)bitmap,partWidth,partHeight);
		drawBitmap(stretchedBitmap,x,y);
		
		WEBC_FREE((void*)stretchedBitmap->pData);
		WEBC_FREE((void*)stretchedBitmap);
		WEBC_FREE((void*)bitmap->pData);
		WEBC_FREE((void*)bitmap);
		close(file);
#endif	
	}
	drawingWindow = false;
	
}


/**
*	Gets the dimensions of the surface that is being drawn on.
*
*	Return: returns the dimensions of the graphics surface.
*/
HTMLRect FB_Graphics::getDimensions(){
	return FB_Graphics::clientDim;
}

/**
*  When the frame buffer is being written into this is used to get the color at a specific position
*/
void FB_Graphics::getBGColor(int posX, int posY, WEBC_UINT8* bgRed, WEBC_UINT8* bgGreen,WEBC_UINT8* bgBlue){

	if(posX > clipBounds.right){
		posX = clipBounds.right;
	}
	else if(posX < clipBounds.left){
		posX = clipBounds.left;
	}
	if(posY > clipBounds.bottom){
		posY = clipBounds.bottom;
	}
	else if(posY < clipBounds.top){
		posY = clipBounds.top;
	}
	WEBC_UINT8* tempPtr = (WEBC_UINT8*)clientDisplayBuffer->pData + (posY * clientDisplayBuffer->iBytesPerRow) + (posX*4);
	*bgRed = tempPtr[0];
	*bgGreen = tempPtr[1];
	*bgBlue = tempPtr[2];
	
	
}


void FB_Graphics::swapDisplayBuffer()
{
  return;
	if(m_currentGBuffer)
	{
		m_currentGBuffer = 0;
		LCD_UPBASE         =  (unsigned long) getVideoBaseAddr(0) & ~7UL ;
		LCD_LPBASE         =  (unsigned long) getVideoBaseAddr(0) & ~7UL ;
	}
	else
	{
		m_currentGBuffer = 1;
		LCD_UPBASE         =  (unsigned long) getVideoBaseAddr(1) & ~7UL ;
		LCD_LPBASE         =  (unsigned long) getVideoBaseAddr(1) & ~7UL ;
	}
        
}

void FB_Graphics::swapDrawBuffer()
{
  return;
	if(m_currentGBuffer)
	{
		// we're displaying the 2 buffer, so use the 1 buffer
	  clientDisplayBuffer->pData = (WEBC_UINT8*)getVideoBaseAddr(0);
	}
	else
	{
	  clientDisplayBuffer->pData = (WEBC_UINT8*)getVideoBaseAddr(1);
	}

}