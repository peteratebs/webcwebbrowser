/*
|  FB_Window_Manager.h -
|
|	This file is used to manage windows and the appearence of the windows on the screen
|
|  EBS -
|
|   $Author: Peter Lavallee $
|   $Date: 2008/08/01 $
|   $Name:  $
|   $Revision: 1.1 $
|
|  Copyright EBS Inc. , 2004
|  All rights reserved.
|  This code may not be redistributed in source or linkable object form
|  without the consent of its author.
*/

#include "webctmo.h"
#include "webc.h"
#include "webrect.hpp"
#include "FB_Window.h"
#include "FB_List_Iterator.h"

#define UNICODE //using unicode

/**
*	Called to initialize the window Manager.  Starts threads needed to run and initializes variables
*	used by the window manager
*/
void windowManagerInit();


/**
*	Called to stop the window Manager.  Destroys threads and memory needed to by the window manager.
*/
void windowManagerShutdown();

/**
*	Called to create a new window
*
*	Params: x - x position of where the window will appear on screen, y - y position of where the 
*			the window will appear on screen, width - width of the window, height - height of the window
*			parent - parent of the wind, if no parent then NULL, name - the name/title of the window
*	Return: returns a handle to the window that was created
*/
WEBC_UINT8* createWindow(DISPLAY_INT x, DISPLAY_INT y, DISPLAY_INT width, 
								DISPLAY_INT height, WEBC_UINT8* parent, WEBC_CHAR* name);

/**
*	Sets the window given to be drawable, meaning when the manager goes to draw the screen, 
*	This window will appear.
*
*	Params: windowHandle - the window to display
*/
void showWindow(WEBC_UINT8* windowHandle);

/**
*	Destroys a given window freeing up the space it occupied and removing it from the windows list.
*
*	Params: windowHandle - the window to be destroyed
*/
void destroyWindow(WEBC_UINT8* windowHandle);

/**
*	Gets the dimensions of a given window. This means the x y position will be
*   in relation to the screen being displayed.
*
*	Param: windowHandle - the handle of the to get the dimensions, info - the struct to return
*		   the dimensions.
*/
void getWindowDimensions(WEBC_UINT8* windowHandle, HTMLRect* info);

/**
*	Called to move a given window to a specific x y position.
*
*	Params: windowHandle - the window to move, x - the new x position, y - the new y position
*/
void moveWindowTo(WEBC_UINT8* windowHandle, DISPLAY_INT x, DISPLAY_INT y);

/**
*	Called to move a given window by a specific amount.
*
*	Params: windoHandle - the window to move, x - the horizontal amount to move the window,
*			y - the vertical amount to move the window.
*/
void moveWindowBy(WEBC_UINT8* windowHandle, DISPLAY_INT x, DISPLAY_INT y);

/**
*	Gets the dimensions of a given window's client area.  This is the dimensions that the client can draw in.
*	x y position is always 0 0.
*
*	Params: windowHandle - window to get the dimensions of, info - struct to receive the dimensions
*/
void getClientDimensions(WEBC_UINT8* windowHandle, HTMLRect* info);

/**
*	Sets a window to receive all events that occur.
*
*	Params: windowHandle - the window to receive the events
*/
void setCapture(WEBC_UINT8* windowHandle);

/**
*	Releases a window that has been capturing all events. 
*	This way the window manager will now determine who the events will go to.
*
*	Params: windowHandle - the window that will stop receiving events
*/
void releaseCapture(WEBC_UINT8* windowHandle);

/**
*	Sets the given window's timer to the given time, this timer keeps the window moving
*
*	Params: windowHandle - windowHandle for the new timer, time - new timer.
*/
void setTimer(WEBC_UINT8* windowHandle, WEBC_UINT16 time);

/**
*	Schedules the given window for a rePaint.
*
*	Params: handle - the window to be repainted on the screen
*/
void rePaint(WEBC_UINT8* handle);

/**
*	Method used by the timer regulator to issue a new timer message to the window.
*
*	Params: windowHandle - window to be issued the message, err - error in the time the timer is sent
*/
int timerCallback(void* windowHandle, long err);

/**
*	Called to get the information about the screen that is being displayed on.
*
*	Params: info - the struct to return the screen information in.
*/
void getScreenResolution(HTMLScreenInfo* info);

/**
*	This method is used internally by the window manager to determine which
*	window receives messages by coordinates and where the windows exists on screen
*
*	Params: x - the x position , y - the y position
*	Return: the handle of the window to recieve the action 
*/
WEBC_UINT8* getReceivingWindow(DISPLAY_INT x, DISPLAY_INT y);

/**
*	Called by the window manager to recalculate all the z orders of the windows while
*	moving one window to the top
*
*	Params: windowHandle - the handle that will be moved up
*/
void moveWindowToTop(FB_Window *windowHandle);

/**
*	Used to drop the z order of a window, if a window's order is dropped so must
*	the ones below it
*
*	Params: windowHandle - the window to be dropped in z order
*/
void dropZOrder(FB_Window *windowHandle);

/**
*	This method is called to find all the windows that are intersecting with the given windows
*
*	Params: the window that to find the intersected windows of
*	Return: The list of windows that intersect with the other windows
*/
FB_List* getIntersections(FB_Window* windowHandle);

/**
*	This method is called to find all the windows that are intersecting with the given rectangle
*
*	Params: the rectangle to find the intersected windows of
*	Return: The list of windows that intersect with the rectangle
*/
FB_List* getOrderedIntersections(HTMLRect rect);

/**
*	Called to invalidate an area the sceen buffer that will be displayed to the screen.
*	This is called by the window manager when a window has been moved.
*
*	Params: prevX - the previous x position of the window, prevY - the previous y position of the window, 
*			nextX - the next x position of the window, nextY - the next y position of the window, 
*			windowHandle - the actual window that was moved.
*/
void invalidateScreenBufferRegion(DISPLAY_INT prevX, DISPLAY_INT prevY, 
								  DISPLAY_INT nextX, DISPLAY_INT nextY, FB_Window* windowHandle);

/**
*	Called when a window/graphics need to be displayed to the screen.
*
*	Params: igraphics - the graphics of the window that will be displayed
*/
void displayWindow(FB_Graphics* igraphics);

