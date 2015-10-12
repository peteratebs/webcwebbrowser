/*
|  FB_Window.h -
|
|	This class represents all the information needed to maintain a window.
|   This class is only used by the Window manager, though is passed to other parts of the system
|   via a WEB_UINT8 pointer.
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
#include "webc.h"
#include "webrect.hpp"
#include "FB_Graphics.h"


#define FRAME_TOP_HEIGHT 19//height of the top frame graphic for the representation of a window
#define FRAME_BOTTOM_HEIGHT 3//height of the bottom frame graphic for the representation of a window
#define FRAME_RIGHT_WIDTH 3 //width of the right frame graphics for the representaion of a window
#define FRAME_LEFT_WIDTH 2 //width of the left frame graphics for the representaion of a window
#define DRAW_OFFSET_Y FRAME_TOP_HEIGHT //offset caused by the addition of a frame graphic
#define DRAW_OFFSET_X FRAME_LEFT_WIDTH //offset caused by the addtion of a frame graphic


class FB_Window{
	
	WEBC_CHAR* name;//name of the window, possibly to be displayed in top bar
	WebRect* clientDimensions;//dimensions given to the client to operate within
	WebRect* windowDimensions;//dimensions of the total window area in relation to the overall area
	WEBC_UINT8* parentHandle; //handle of this windows parent if it has a parent(owner)
	bool draw; //whether to draw this window and client
	bool redrawFrame; //whether the frame needs to be redrawn
	bool repaintQueued;
	WEBC_UINT16 timerId; //id of it's timer that ensures it's not dead
	DISPLAY_INT zOrder;  //zOrder of the window, whether it is in front or behind other windows, 1 means front
	FB_Graphics* graphicsContext;
	public:
		//constructor
		FB_Window(DISPLAY_INT x,DISPLAY_INT y,DISPLAY_INT width,DISPLAY_INT height,
				  WEBC_UINT8* parent, WEBC_CHAR* name);
		
		//destructor
		~FB_Window();
		
		/**
		*	Returns a WebRect containing the dimensions of the client area for a window.
		*   The client region is the area within a window controlled by a client.  The 
		*	x, y coordintas of the client are always 0,0;
		* 
		*	Return returns a webrect containing the dimensions
		*/
		WebRect* getClientDimensions();
		
		/**
		*	Returns a WebRect containing the dimensions of the window.
		*   This is the window in it's entirety.  The 
		*	x, y coordintas of the window are always are relative to the area in which it is being
		*   displayed.
		* 
		*	Return returns a webrect containing the dimensions
		*/
		WebRect* getWindowDimensions();

		/**
		*	Moves a window to the desired location.  Adjusts the x, y position of the windowDimensions
		*   variable
		*
		*	Params: x - new x coordinate, y - new y coordinate
		*/
		void moveWindow(DISPLAY_INT x, DISPLAY_INT y);

		/**
		*	Resizes a window to the desired size.  Adjusts the height and width of the windowDimensions
		*	variable and the clientDimensions variable.
		*
		*	Params: width - new width for the window, height - new height for the window
		*/  
		void resizeWindow(DISPLAY_INT width, DISPLAY_INT height);

		/**
		*	Determines whether an x y position is within the client area of a window.
		*	This is used to determine whether an event should be given to a client.
		*
		*	Params: x - x position, y - y position
		*	Return: returns if it was in the client area
		*/
		bool inClientArea(DISPLAY_INT x, DISPLAY_INT y);

		/** 
		*	Determines whether an x y position is within the top bar of a given window, 
		*   this may be used to if a user needs to move the window and clicks in the top bar.
		*
		*	Params: x - x position, y - y position
		*   Return: returns if it was in the window bar
		*/
		bool inWindowBar(DISPLAY_INT x, DISPLAY_INT y);

		/**
		*	This gets the amount of offset needed to put a set of positions
		*   into terms of the client positioning, ie this method gets the x position of the window and adds to it
		*   the width of the left graphic.
		*
		*	Return: the amount of the offset
		*/
		DISPLAY_INT getClientOffsetX();
		
		
		/**
		*	This gets the amount of offset needed to put a set of positions
		*   into terms of the client positioning, ie this method gets the y position of the window and adds to it
		*   the height of the top graphic.
		*
		*	Return: the amount of the offset
		*/
		DISPLAY_INT getClientOffsetY();
		
		/**
		*	This method returns the z order of this window.  If it is on top the return value would be 1.
		*
		*	Return: the z order of the window
		*/
		DISPLAY_INT getZOrder();
		
		/**
		*	This sets the zOrder of the window.
		*
		*	Params: order - the new order of the window
		*/
		void setZOrder(DISPLAY_INT order);
		
		/**
		*	Gives the window an id for the timer that sends it periodic messages.
		*
		*	Params: time - the new id
		*/
		void setTimerId(WEBC_UINT16 time);
		
		
		/**
		*	Gets the id for the timer that sends it periodic messages.
		*
		*	Params: time - the new id
		*/
		WEBC_UINT16 getTimerId();
		
		/**
		*	Sets whether the window should be drawn.
		*
		*	Param: set - whether window should be drawn
		*/
		void setDraw(bool set);

		/**
		*	Gets whether the window should be drawn.
		*
		*	Return: returns whether the window should be drawn
		*/
		bool getDraw();

		/**
		*	Sets whether the window frame graphics should be reDrawn.
		*
		*	Param: newVal - whether the frame graphic of the window should be redrawn
		*/
		void setFrameRedraw(bool newVal);

		/**
		*	Returns whether the window frame graphics should be reDrawn.
		*
		*	Return: returns whether the window frame graphics should be re drawn
		*/
		bool getFrameRedraw();

		/**
		*	Sets a graphics context to this window. 
		*
		*	Params: gcontext - the new graphics context for the window	
		*/
		void setGraphics(FB_Graphics* gcontext);

		
		/**
		*	Gets the graphics context for the window. 
		*
		*	return: this function will return	
		*/
		FB_Graphics* getGraphics();

};