/*
|  FB_GRAPHICS.H -
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
|   
|
*/
#include "webc.h"



class FB_Graphics{
	//data members
	WEBC_UINT8 redPen,bluePen,greenPen;//the values of teh drawing color
	bool transparentPen;//whether the drawing color is set to tranparent
	WebGraphBitmapInfo* clientDisplayBuffer;//the buffer for the client drawing surface
	WebGraphBitmapInfo* windowDisplayBuffer;//the buffer for window graphics
	
	WEBC_UINT8* windowHdl; //the window handle of a window that this graphics is based on
	HTMLRect invalidRect; //what needs to get redrawn
	HTMLRect clipBounds; //clipRegions for drawing
	HTMLRect clientDim; //dimensions of the client area
	
	bool drawingWindow;
	int m_currentGBuffer;

private: 
	/**
	*	This method is used within the graphics class to load the graphics of the window frame into the
	*   windowDisplayBuffer the graphics are stored in a folder under the framebuffer folder.
	*/
	void loadWindowFrame();

	/**
	*	BMP data is stored upside down, internal format is right side up.  This
	*   method is used to flip the image data of a BMP data into the internal format.
	*/
	void flipImage(WebGraphBitmapInfo* org);
	
public:
	/**
	*	This constructor is used when creating a graphics class that represents a window.
	*	Initialization of all the graphic variables are done.
	*
	*	Params: windowHandle - the window the graphics will be based on	
	*/
	FB_Graphics(WEBC_UINT8* windowHandle);

	/**
	*	This constructor is used when a surface needs to be created to draw on without a window
	*	
	*	Params: width - the width of the surface, height - the height of the surface, mode - the color depth mode of 
	*           the surface
	*/
	FB_Graphics(DISPLAY_INT width, DISPLAY_INT height, int mode);
	
	/**
	*	Destructor, frees the memory used by the graphics class
	*/	
	~FB_Graphics();
	
	


	/**
	*	Sets the color for the next element to be drawn. 
	*
	*   Param: color - a 32 bit color where the color values are as such transparentRedGreenBlue
	*/
	void setColor(
		WEBC_UINT32 color
	);
	
	/**
	*	Sets the color for the next element to be drawn.
	*
	*	Param: red - amount of red, green - amount of green, blue - amount of blue
	*/
	void setColor(
		WEBC_UINT8 red,
		WEBC_UINT8 green,
		WEBC_UINT8 blue
	);

	/**
	*	Indicates that the next element to be drawn will be transparent or just not drawn.
	*/
	void setTransparency();
	
	/**
	*	Used to draw a rectangle on the graphics surface.
	*
	*	Params: x - left side position, y - top position, width - width of the rectangle
	*			height - height of the rectangle, fill - whether the rectangle will be filled, 
	*			lineColor - the color of the line that will border the rectangle,
	*			fillColor - the color of the interior of the rectangle if it is filled
	*/
	void drawRectangle(
		DISPLAY_INT x,
		DISPLAY_INT y,
		DISPLAY_INT width,
		DISPLAY_INT height,
		bool fill,
		WEBC_UINT32 lineColor,
		WEBC_UINT32 fillColor
	);

	/**
	*	Used to draw a bitmap on the graphics surface. This method is used when there
	*   does not need to be a restriction on the width and height of a bitmap
	*
	*	Params: bitmapPtr - bitmap to be drawn, dispX - the x position of where
	*           the bitmap must be drawn, dispY - the y position of where the 
	*			bitmap must be drawn.
	*/
	void drawBitmap(
		WebGraphBitmapInfo* bitmapPtr, 
		DISPLAY_INT dispX, 
		DISPLAY_INT dispY
	);
	
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
	);

	/**
	*	This method is used to draw a horizontal line on the graphics surface
	*
	*	Params: xPos - x position of the line, yPos - y position of the line, 
	*			length - the length of the line
	*/
	void drawHorizontalLine( 
		DISPLAY_INT xPos,
		DISPLAY_INT yPos,
		DISPLAY_INT length
	);
	
	/**
	*	Sets the clipping region for the graphics surface.  
	*	The clipping region is the bounding region where can not be drawn.
	*
	*	Params: rect - the new rect for the clipping region
	*/
	void setClipRect(
		HTMLRect rect
	);
	
	/**
	*	Gets the clipping region for the graphics surface. Returns the
	*	bounding region for the non-drawing area.
	*
	*	Return: returns the rectangle representing where the class is 
	*			limited in drawing.
	*/
	HTMLRect getClipRect();

	/**
	*	This method adds this rectangle to the overall region that
	*   needs to be redrawn or redisplayed.
	*
	*	Params: rect - the rectangle to add to the overall region to be redrawn
	*/
	void addDirtyRect(
		HTMLRect rect
	);

	/**
	*	Returns the area that needs to be redrawn if beign displayed
	*
	*	Return: the area that needs to be redrawn
	*/
	HTMLRect getDirtyRect();

	/**
	*	Gets the dimensions of the surface that is being drawn on.
	*
	*	Return: returns the dimensions of the graphics surface.
	*/
	HTMLRect getDimensions();

	/**
	*	Resets the invalid region indicating that all displays are up to date for 
	*   this graphics class.
	*/
	void resetDirtyRect();

	/**
	*	Returns the displayBuffer for this graphic, this is the client area display
	*	so it does not include the bordering window.
	*
	*	Return: the display buffer that is being drawn on
	*/
	WebGraphBitmapInfo* getDisplayBuffer();

	/**
	*	Returns the window buffer , meaning the bordering window graphic.
	*
	*	Return: returns the graphic representing the window to be drawn around a client
	*/
	WebGraphBitmapInfo* getWindowDisplayBuffer();

	/**
	*	Gets the window that is associated with this graphics if a window is not associated
	*	with this graphic NULL will be returned
	*/
	WEBC_UINT8* getWindowHandle();

	void getBGColor(int posX, int posY, WEBC_UINT8* bgRed, WEBC_UINT8* bgGreen,WEBC_UINT8* bgBlue);
	
	void swapDisplayBuffer();
	void swapDrawBuffer();

};
//This struct is used by the graphics layer of Webc to contain information needed by browser and graphics layer
typedef struct{
	HTMLGraphicsContext* graphicsLayerCalls;
	WEBC_UINT8* browserHandle;//handle to the window for the graphics
	WEBC_UINT8* windowHandle;
	FB_Graphics* displayGraphics;
	int drawNesting; //amount of draws needed to perform
} FBGraphicsContext;