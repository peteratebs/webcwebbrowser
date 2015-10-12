/*
|   FB_Window.cpp -
|
|	This class represents all the information needed to maintain a window.
|   This class is only used by the Window manager, though is passed to other parts of the system
|   via a WEB_UINT8 pointer.
|
|
|  EBS -
|
|   $Author: Peter Lavallee $
|   $Date: 2008/08/01 $
|   $Name:  $
|   $Revision: 1.1 $
|
|  Copyright EBS Inc. , 2008
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/
#include "FB_Window_Manager.h"





/**
*	Constructor initializes all variables.
*/
FB_Window::FB_Window(DISPLAY_INT x,DISPLAY_INT y,DISPLAY_INT width,DISPLAY_INT height,
		  WEBC_UINT8* parent, WEBC_CHAR* wname){
#if(BROWSER_FRAME)	
	clientDimensions = new WebRect();
	clientDimensions->top = 0;
	clientDimensions->left = 0;
	clientDimensions->right = width - FRAME_LEFT_WIDTH - FRAME_RIGHT_WIDTH - 1;
	clientDimensions->bottom = height - FRAME_TOP_HEIGHT - FRAME_BOTTOM_HEIGHT - 1;

	windowDimensions = new WebRect();
	windowDimensions->top = y;
	windowDimensions->left = x;
	windowDimensions->right = x + width - 1;
	windowDimensions->bottom = y + height - 1;
#else
	clientDimensions = new WebRect();
	clientDimensions->top = 0;
	clientDimensions->left = 0;
	clientDimensions->right = width - 1;
	clientDimensions->bottom = height - 1;
	
	windowDimensions = clientDimensions;
#endif

	zOrder = 1;
	parentHandle = parent;
	name = wname;
	draw = false;
	timerId = -1;
}

/**
*	Destructor does nothing
*/
FB_Window::~FB_Window(){
	delete(clientDimensions);
#if(BROWSER_FRAME)
	delete(windowDimensions);
#endif
}

/**
*	Returns a WebRect containing the dimensions of the client area for a window.
*   The client region is the area within a window controlled by a client.  The 
*	x, y coordintas of the client are always 0,0;
* 
*	Return returns a webrect containing the dimensions
*/
WebRect* FB_Window::getClientDimensions(){
	return clientDimensions;
}

/**
*	Returns a WebRect containing the dimensions of the window.
*   This is the window in it's entirety.  The 
*	x, y coordintas of the window are always are relative to the area in which it is being
*   displayed.
* 
*	Return returns a webrect containing the dimensions
*/
WebRect* FB_Window::getWindowDimensions(){
	return windowDimensions;
}

/**
*	Moves a window to the desired location.  Adjusts the x, y position of the windowDimensions
*   variable
*
*	Params: x - new x coordinate, y - new y coordinate
*/
void FB_Window::moveWindow(DISPLAY_INT x, DISPLAY_INT y){
	windowDimensions->MoveTo(x, y);
}

/**
*	Resizes a window to the desired size.  Adjusts the height and width of the windowDimensions
*	variable and the clientDimensions variable.
*
*	Params: width - new width for the window, height - new height for the window
*/  
void FB_Window::resizeWindow(DISPLAY_INT width, DISPLAY_INT height){
#if(BROWSER_FRAME)
	clientDimensions->SizeTo(width - FRAME_LEFT_WIDTH - FRAME_RIGHT_WIDTH, 
						     height - FRAME_TOP_HEIGHT - FRAME_BOTTOM_HEIGHT);
#endif
	windowDimensions->SizeTo(width, height);
}

/**
*	Determines whether an x y position is within the client area of a window.
*	This is used to determine whether an event should be given to a client.
*
*	Params: x - x position, y - y position
*	Return: returns if it was in the client area
*/
bool FB_Window::inClientArea(DISPLAY_INT x, DISPLAY_INT y){
	bool retVal = false;
#if(BROWSER_FRAME)
	if( (x >= windowDimensions->left + FRAME_LEFT_WIDTH ) && (x <= windowDimensions->right - FRAME_RIGHT_WIDTH ) &&
		(y >= windowDimensions->top + FRAME_TOP_HEIGHT ) && (y <= windowDimensions->bottom - FRAME_BOTTOM_HEIGHT )){
		retVal = true;
	}
#else
	if( (x >= windowDimensions->left) && (x <= windowDimensions->right) &&
		(y >= windowDimensions->top) && (y <= windowDimensions->bottom)){
		retVal = true;
	}
#endif
	return retVal;
}

/** 
*	Determines whether an x y position is within the top bar of a given window, 
*   this may be used to if a user needs to move the window and clicks in the top bar.
*
*	Params: x - x position, y - y position
*   Return: returns if it was in the window bar
*/
bool FB_Window::inWindowBar(DISPLAY_INT x, DISPLAY_INT y){
	bool retVal = false;
#if(BROWSER_FRAME)
	if( (x >= windowDimensions->left ) && (x <= windowDimensions->right ) &&
		(y >= windowDimensions->top ) && (y <= windowDimensions->top + FRAME_TOP_HEIGHT )){
		retVal = true;
	}
#endif
	return retVal;
}

/**
*	Gets the id for the timer that sends it periodic messages.
*
*	Params: time - the new id
*/
WEBC_UINT16 FB_Window::getTimerId(){
	return timerId;
}

/**
*	Gives the window an id for the timer that sends it periodic messages.
*
*	Params: time - the new id
*/
void FB_Window::setTimerId(WEBC_UINT16 time){
	timerId = time; 
}

/**
*	Sets whether the window should be drawn.
*
*	Param: set - whether window should be drawn
*/
void FB_Window::setDraw(bool set){
	draw = set;
}

/**
*	Gets whether the window should be drawn.
*
*	Return: returns whether the window should be drawn
*/
bool FB_Window::getDraw(){
	return draw;
}

/**
*	This gets the amount of offset needed to put a set of positions
*   into terms of the client positioning, ie this method gets the x position of the window and adds to it
*   the width of the left graphic.
*
*	Return: the amount of the offset
*/
DISPLAY_INT FB_Window::getClientOffsetX(){
#if(BROWSER_FRAME)
	return windowDimensions->left + FRAME_LEFT_WIDTH;
#else
	return windowDimensions->left;
#endif
}

/**
*	This gets the amount of offset needed to put a set of positions
*   into terms of the client positioning, ie this method gets the y position of the window and adds to it
*   the height of the top graphic.
*
*	Return: the amount of the offset
*/
DISPLAY_INT FB_Window::getClientOffsetY(){
#if(BROWSER_FRAME)
	return windowDimensions->top + FRAME_TOP_HEIGHT;
#else
	return windowDimensions->top;
#endif
}

/**
*	Sets whether the window frame graphics should be reDrawn.
*
*	Param: newVal - whether the frame graphic of the window should be redrawn
*/
void FB_Window::setFrameRedraw(bool newVal){
	redrawFrame = newVal;
}

/**
*	Returns whether the window frame graphics should be reDrawn.
*
*	Return: returns whether the window frame graphics should be re drawn
*/
bool FB_Window::getFrameRedraw(){
#if(BROWSER_FRAME)
	return redrawFrame;
#else
	return false;
#endif
}

/**
*	This method returns the z order of this window.  If it is on top the return value would be 1.
*
*	Return: the z order of the window
*/
DISPLAY_INT FB_Window::getZOrder(){
	return zOrder;
}

/**
*	This sets the zOrder of the window.
*
*	Params: order - the new order of the window
*/
void FB_Window::setZOrder(DISPLAY_INT order){
	zOrder = order;
}


/**
*	Sets a graphics context to this window. 
*
*	Params: gcontext - the new graphics context for the window	
*/
void FB_Window::setGraphics(FB_Graphics* gcontext){
	graphicsContext = gcontext;
}
	
/**
*	Gets the graphics context for the window. 
*
*	return: this function will return	
*/
FB_Graphics* FB_Window::getGraphics(){
	return graphicsContext;
}