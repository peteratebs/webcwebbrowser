/*
|  FB_Window_Manager.cpp -
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
//defines

#include "webcmem.h"
#include "FB_Window_Manager.h"
#include "FBAbstraction.h"
#include "LMC2478_FB.h"



WebcTimeoutManager* timeoutManager;//timer manager for windows to see if they are dead
FB_List* messageList;//list of messages for windows
FB_List* windowList;//list of current open windows
DISPLAY_INT prevCursorPositionX, prevCursorPositionY;//this is for when moving a window

//boolean variable to control certain behavior of windowmanager
bool timerMessageExists;//eliminates duplicate timer messages
bool mouseMoveMessageExists;//eliminates duplicate mouse movements
bool repaintExists;
//used for key strokes
static int giShift = 0;
static int giCaps = 0;
static int giCtrl = 0;
static int giAlt = 0;
static int giEscapeDown = 0;

bool windowCaptureEvents;//events go to the window as opposed to the client


FB_Graphics* screenBuffer;//buffer to display to screen

FB_Window* currentFocusedWindow;//current window to which events are sent
FB_Window* currentActivatedWindow;//current window which is active


/**
*	This method is call to initialize the window manager.  It initialize needed variables and creates threads needed
*   as well as starting them. Also initializes the final destination for graphics. ie. in simulator a windows window
*/
void windowManagerInit(){
	
	currentFocusedWindow = WEBC_NULL;
	currentActivatedWindow = WEBC_NULL;
	windowCaptureEvents = false;
	timerMessageExists = false;
	mouseMoveMessageExists = false;
	repaintExists = false;

	screenBuffer = new FB_Graphics(HORIZONTAL_RESOLUTION, VERTICAL_RESOLUTION,0);//right now mode don't really do anything
	screenBuffer->resetDirtyRect();
	messageList = new FB_List();
	windowList = new FB_List();
	timeoutManager = new WebcTimeoutManager();

	//portfb_init();
}

/** 
*	This method will shutdown the window manager destroying all things it had
*/
void windowManagerShutdown(){
	//portfb_destroy();
	
	delete(screenBuffer);
	
	FB_List_Iterator* iter = new FB_List_Iterator(windowList);
	FB_Window* win = (FB_Window*)iter->getNext();
	while(win){
		delete(win);
		win = (FB_Window*)iter->getNext();
	}
	delete(windowList);
	delete(iter);
	
	iter = new FB_List_Iterator(messageList);
	HTMLEvent* e = (HTMLEvent*)iter->getNext();
	while(e){
		delete(e);
		e = (HTMLEvent*)iter->getNext();
	}
	delete(messageList);
	delete(iter);

	delete(timeoutManager);

}
/**
*	Creates a new window with specified parameters and adds the window to the window list.
*   Params: x - x position of the window, y - vertical position of the window, width - width of the window,
*           height - height of the window, parent - if the window belongs to another, this is that window,
*           name - title of the window.
*   Return: returns the created window.
*/
WEBC_UINT8* createWindow(DISPLAY_INT x, DISPLAY_INT y, DISPLAY_INT width, 
						 DISPLAY_INT height, WEBC_UINT8* parent, WEBC_CHAR* name){
	
	FB_Window* window = new FB_Window(x,y,width,height,parent,name);
	windowList->push((void*)window);
	currentActivatedWindow = window;
	return (WEBC_UINT8*) window;
}

/**
*	Destroys the window given a window handle
*   Param: windowHandle - the window handle
*/
void destroyWindow(WEBC_UINT8* windowHandle){
	FB_Window* tmp = (FB_Window*)windowHandle;
	if(currentFocusedWindow == tmp){
		releaseCapture(windowHandle);
	}
	if(currentActivatedWindow == tmp){
		currentActivatedWindow = WEBC_NULL;
	}
	windowList->remove((void*)windowHandle);
	
	delete(tmp);

}

/**
*	Allows for a window to be shown, controls the variable that determines whether or not it will be drawn.
*   Params: windowHandle - the window to be shown
*/
void showWindow(WEBC_UINT8* windowHandle){
	FB_Window* window = (FB_Window*) windowHandle;
	window->setDraw(true);
}	

/**
*	Get the dimensions of a window, this means the top left corner relative to the screen and the width of the 
*   window including the window graphics.
*   Params: windowHandle - the window to get the dimensions, info - the structure the information will be returned
*							in.
*/
void getWindowDimensions(WEBC_UINT8* windowHandle, HTMLRect* info){
	FB_Window *window = (FB_Window*)windowHandle;
	WebRect* tmp = window->getWindowDimensions();
	info->bottom = tmp->bottom;
	info->top = tmp->top;
	info->left = tmp->left;
	info->right = tmp->right;
}

/**
*	Get the dimensions of the client of a window, this means the top left corner will be 0,0 and the width and height
*   will only be that of the client the window conatains, not including window graphics. meaning if a window is created
*   500 x 500 then the client area and thus the width and height returned by this method are going to be slightly
*   less than 500.  This amount is determined by the defines located in FB_Window.h.  FRAME_LEFT_WIDTH, FRAME_RIGHT_WIDTH...so on
*	
*   Params: windowHandle - the window to get the dimensions, info - the structure the information will be returned
*							in.
*/
void getClientDimensions(WEBC_UINT8* windowHandle, HTMLRect* info){
	FB_Window *window = (FB_Window*)windowHandle;
	WebRect* tmp;
	if(window){
		tmp = window->getClientDimensions();
		info->bottom = tmp->bottom;
		info->top = tmp->top;
		info->left = tmp->left;
		info->right = tmp->right;
	}
}

/**
*	Handles the moving of a window on the screen, calls for a repaint of the window moved
*   Coordinates passed in are the new x, y corrdinates of the window
*  
*	Params: windowHandle - handle for the window, x - new x position of the window, y - new y position of the window
*/
void moveWindowTo(WEBC_UINT8* windowHandle, DISPLAY_INT x, DISPLAY_INT y){
	invalidateScreenBufferRegion(((FB_Window*)windowHandle)->getWindowDimensions()->left,
								 ((FB_Window*)windowHandle)->getWindowDimensions()->top,
								 x,y,
								 ((FB_Window*)windowHandle));
	((FB_Window*)windowHandle)->moveWindow(x, y);
	((FB_Window*)windowHandle)->setFrameRedraw(true);

	FB_List* list = getIntersections((FB_Window*) windowHandle);
	FB_List_Iterator* iter = new FB_List_Iterator(list);
	FB_Window* tmpWin = (FB_Window*)iter->getNext();
	while(tmpWin != WEBC_NULL && tmpWin->getZOrder() >= ((FB_Window*)windowHandle)->getZOrder()){
		dropZOrder(tmpWin);
		tmpWin = (FB_Window*)iter->getNext();
	}

	delete(list);
	delete(iter);

	rePaint(windowHandle);
}

/**
*	Handles the moving of a window on the screen, calls for a repaint of the window moved.
*  
*	Params: windowHandle - handle for the window, x - amount to move a window horizontally, 
*			y - amount to move a window verticaly
*/
void moveWindowBy(WEBC_UINT8* windowHandle, DISPLAY_INT x, DISPLAY_INT y){
	WebRect* rect = ((FB_Window*)windowHandle)->getWindowDimensions();
	invalidateScreenBufferRegion(rect->left,
								 rect->top,
								 x,y,
								 ((FB_Window*)windowHandle));
	((FB_Window*)windowHandle)->moveWindow(rect->left + x, rect->top + y);
	((FB_Window*)windowHandle)->setFrameRedraw(true);

	FB_List* list = getIntersections((FB_Window*) windowHandle);
	FB_List_Iterator* iter = new FB_List_Iterator(list);
	FB_Window* tmpWin = (FB_Window*)iter->getNext();
	while(tmpWin != WEBC_NULL && tmpWin->getZOrder() >= ((FB_Window*)windowHandle)->getZOrder()){
		dropZOrder(tmpWin);
		tmpWin = (FB_Window*)iter->getNext();
	}
	if(list){
		delete(list);
	}
	if(iter){
		delete(iter);
	}

	rePaint(windowHandle);
}

/**
*	Fills an info struct with information about the display screen. This information is defined in FB_Window_Manager.h.
*   
*   Params: info - struct to put screen info in
*/
void getScreenResolution(HTMLScreenInfo* info){
	info->availHeight = VERTICAL_RESOLUTION;
	info->availWidth = HORIZONTAL_RESOLUTION;
	info->height = VERTICAL_RESOLUTION;
	info->width = HORIZONTAL_RESOLUTION;
	info->colorDepth = 24;//this will probably need to be changed
}

/**
*	Sets the capture of a window. Meanin while this window is the focused window all events will go to it.
*
*	Params: windowHandle - window to set the capture of
*/
void setCapture(WEBC_UINT8* windowHandle){
	currentFocusedWindow = (FB_Window*)windowHandle;
}


/**
*	releases the capture of a window. Events will now go to whom ever the windowManager deems is appropriate.
*
*	Params: windowHandle - window to release the capture of
*/
void releaseCapture(WEBC_UINT8* windowHandle){
	if(currentFocusedWindow == (FB_Window*)windowHandle){
		currentFocusedWindow = WEBC_NULL;
	}
}

/**
*	This function is is used by the window manager to determine if a set of coordinates falls within a window
*	the window whom is returned is also the foremost window on screen who fits the criteria.
*
*	Params: x - the x coordinate to check, y - the y coordinate to check
*/
WEBC_UINT8* getReceivingWindow(DISPLAY_INT x, DISPLAY_INT y){
	WEBC_UINT8* retVal = WEBC_NULL;
	FB_Window* tmpWin = WEBC_NULL;
	FB_List_Iterator* iter = new FB_List_Iterator(windowList);
	DISPLAY_INT order = 10000;//just a number to get it going
	tmpWin = (FB_Window*)iter->getNext();
	while(tmpWin != WEBC_NULL){
		if((tmpWin->inClientArea(x,y) || tmpWin->inWindowBar(x,y)) && tmpWin->getZOrder() < order){
			retVal = (WEBC_UINT8*)tmpWin;
			order = tmpWin->getZOrder();
			tmpWin = (FB_Window*)iter->getNext();
		}
		else{
			tmpWin = (FB_Window*)iter->getNext();
		}
	}
	delete(iter);
	return retVal;
}

/**
*	This method informs the window manager that a certain window needs to be repainted.  A repaint message is created
*   and pushed into the message list
*
*	Params: handle - the window that must be repainted
*/
void rePaint(WEBC_UINT8* handle){
	if(!repaintExists){
		HTMLEvent* e = (HTMLEvent*)WEBC_MALLOC(sizeof(HTMLEvent));
		e->type = (HTMLEventType) FBWM_PAINT;
		e->target = (HELEMENT_HANDLE)handle;
		messageList->push((void*)e);
		repaintExists = true;
	}
	
}

/**
*	Used to math a timeout id, this is a method required by the timeout manager.
*/
int matchId (TimeoutInfo* timeout, void* data)
{
	WEBC_UINT16 id = *((WEBC_UINT16*)data);
	return (timeout->wId == id);
}

/**
*	Sets the timer for a window and registers the timer with the timeout manager
*
*	Params: windowHandle: the window that the timer will be attached to, time - time of the timer
*/
void setTimer(WEBC_UINT8* windowHandle, WEBC_UINT16 time){
	FB_Window* wind = (FB_Window*)windowHandle;
	if(wind->getTimerId() != (WEBC_UINT16)(-1)){
		timeoutManager->CancelTimeout(matchId,(void*)wind->getTimerId(),false);
	}
	wind->setTimerId(
		timeoutManager->SetTimeout(timerCallback,0,(void*)windowHandle,time,-100)
	);
	wind->setTimerId(time);
}

/**
*	Call back method called by the timeout manager when a specific timeout has expired.
*   This method creates a timer message for the specific window handle and places it on the message queue
*  
*	Params: windowHandle - the window that needs the timer, err - parameter required by timeout manager
*/
int timerCallback(void* windowHandle, long err){
	if(!timerMessageExists){
		FB_Window* wHnd = (FB_Window*)windowHandle;
		HTMLEvent* e =(HTMLEvent*) WEBC_MALLOC(sizeof(HTMLEvent));
		e->type = (HTMLEventType) FBWM_TIMER;
		e->target = (WEBC_UINT8*) windowHandle;
	
		messageList->push((void*)e);
		timerMessageExists = true;
	}

	return 0;
}

/**
*	When a window is moved then the areas it was occupying must be invalidated
*	This means that a rectangle of the screenBuffer graphics will be cleared
*
*	Params: prevX - x before move, prevY - y before move, newX - x after move, newY - y after move,
*			windowHandle - window that is being moved
*/
void invalidateScreenBufferRegion(DISPLAY_INT prevX, DISPLAY_INT prevY, DISPLAY_INT nextX, DISPLAY_INT nextY, FB_Window* windowHandle){
		 WebRect* dim = windowHandle->getWindowDimensions();
		 HTMLRect rect;

		 if(nextX < prevX){
			 rect.left = nextX;
			 rect.right = prevX + dim->Width() ;
		 }
		 else{
			rect.left = prevX;
			rect.right = nextX + dim->Width();
		 }
		 if(nextY < prevY){
			rect.top = nextY;
			rect.bottom = prevY + dim->Height();
		 }
		 else{
			rect.top = prevY;
			rect.bottom = nextY + dim->Height();
		 }
		 
		 screenBuffer->addDirtyRect(rect);
}

/**
*	Used to recalculate the windows' z orders when a window is moved, created or made active and brought to the
*   front.
*	
*	Params: windowHandle - window that was altered to call for the change
*/
void moveWindowToTop(FB_Window* windowHandle){
	FB_List* intersectionList = getIntersections(windowHandle);
	FB_List_Iterator* iter = new FB_List_Iterator(intersectionList);
	FB_Window* win =(FB_Window*) iter->getNext();
	DISPLAY_INT prevOrder = windowHandle->getZOrder();
	windowHandle->setZOrder(1);

	while(win != WEBC_NULL){
		if(win->getZOrder() <= prevOrder){
			dropZOrder(win);
		}
		win = (FB_Window*)iter->getNext();
	}
	rePaint((WEBC_UINT8*)windowHandle);
	delete(iter);
	delete(intersectionList);
}

/** 
*	Used to drop the z orders of windows because of another window moving to the top.
*   Each window that intersects with the passed in window has to go through
*   this process.  if no intersections are found that have below the current window then it's z-order is dropped
*   one and the function returns.
*	
*	Params: windowHandle - the window that is to be reduced
*/
void dropZOrder(FB_Window* windowHandle){
	FB_List* list = getIntersections(windowHandle);
	FB_List_Iterator* iter = new FB_List_Iterator(list);
	FB_Window* win = (FB_Window*)iter->getNext();
	while(win != WEBC_NULL){
		if(win->getZOrder() > windowHandle->getZOrder()){
			dropZOrder(win);
		}
		win = (FB_Window*)iter->getNext();
	}
	windowHandle->setZOrder(windowHandle->getZOrder()+1);
	delete(iter);
}

/** 
*	Used to get the interactions of a given window with the other windows
*
*	Params: windowHandle - the window that intersects
*	Return: returns the list of windows that intersect
*/ 
FB_List* getIntersections(FB_Window* windowHandle){
	FB_List* retList = new FB_List();
	FB_List_Iterator* iter = new FB_List_Iterator(windowList);
	FB_Window* tempWin = (FB_Window*)iter->getNext();
	WebRect *dim1, *dim2;
	dim1 = windowHandle->getWindowDimensions();
	while(tempWin != WEBC_NULL){
		dim2 = tempWin->getWindowDimensions();
		if(dim1->Overlaps(dim2) && tempWin != windowHandle){
			retList->push(tempWin);
		}
		tempWin = (FB_Window*)iter->getNext();
	}

	return retList;
}

/**
*	This method is called to find all the windows that are intersecting with the given rectangle 
*   The list is ordered from low ordered windows to high order, i.e. 5..2..1
*
*	Params: the rectangle to find the intersected windows of
*	Return: The list of windows that intersect with the rectangle
*/
FB_List* getOrderedIntersections(HTMLRect rect){
	FB_List* retList = new FB_List();
	FB_List_Iterator* iter = new FB_List_Iterator(windowList);
	FB_Window* tempWin = (FB_Window*)iter->getNext();
	DISPLAY_INT prevOrder;
	WebRect *dim1, *dim2;
	dim1 = new WebRect();
	
	dim1->SetSizeAndPosition(rect.left, rect.top, rect.right - rect.left + 1, rect.bottom - rect.top + 1);
	while(tempWin != WEBC_NULL){
		prevOrder = tempWin->getZOrder();
		dim2 = tempWin->getWindowDimensions();
		if(dim2->Overlaps(dim1)){
			if(tempWin->getZOrder() <= prevOrder){
				retList->push(tempWin);
			}
			else{
				retList->addToFront(tempWin);
			}

		}
		tempWin = (FB_Window*)iter->getNext();
	}

	return retList;
}


/**
*	This method displays a graphics that must be displayed. The process is draw the neccessary parts into
*   the screenBuffer, then display the screen buffer the the final display.
* 
*	Params: igrahics - the container for a bitmap that represents the graphics of a client and window
*/
void displayWindow(FB_Graphics* igraphics){
	WEBC_UINT32 white = 0x00FFFFFF;
	
	FB_Window* win = (FB_Window*)igraphics->getWindowHandle();
	//First find what parts of the screen buffer have been invalidated, then clear those parts out with background
	HTMLRect invalid = screenBuffer->getDirtyRect();
	if(invalid.left != invalid.right && invalid.top != invalid.bottom){
		screenBuffer->drawRectangle(invalid.left, invalid.top, invalid.right - invalid.left + 1, invalid.bottom - invalid.top + 1,
								true, white, white);
		screenBuffer->resetDirtyRect();
	}
	//have cleared out screen buffer

	//find the windows that were part of the invalidated region of the screen buffer
	FB_List* dirtyIntersectionList = getOrderedIntersections(invalid);
	FB_List_Iterator* iter = new FB_List_Iterator(dirtyIntersectionList);
	FB_Window* dirtWin = (FB_Window*) iter->getNext();
	//create a webrect to take advantage of useful methods
	WebRect* tempInvalid = new WebRect();
	tempInvalid->SetSizeAndPosition(invalid.left, invalid.top, invalid.right - invalid.left + 1, invalid.bottom - invalid.top + 1);
	
	//redraw the portions of the windows that were part of the dirty rect of the screen buffer
	//the windows are in order from lowest z-order to highest, so a bit like painters algorithm
	WebRect temp;
	while(dirtWin != WEBC_NULL){
		if(dirtWin->getDraw()){
#if(BROWSER_FRAME)
			temp = (*dirtWin->getWindowDimensions());
			temp.And(tempInvalid);
			screenBuffer->drawBitmap(dirtWin->getGraphics()->getWindowDisplayBuffer(),temp.left, temp.top, temp.Width(), temp.Height());
#endif			
			temp = (*dirtWin->getClientDimensions());
			temp.MoveTo(dirtWin->getClientOffsetX(), dirtWin->getClientOffsetY());
			temp.And(tempInvalid);
			screenBuffer->drawBitmap(dirtWin->getGraphics()->getDisplayBuffer(), temp.left, temp.top, temp.Width(), temp.Height());
		}
		dirtWin = (FB_Window*) iter->getNext();
	}
	
	if(win->getDraw()){
		
#if(BROWSER_FRAME)
		WebRect* winRect = win->getWindowDimensions();
		if(win->getFrameRedraw()){
			//drawFrame
			screenBuffer->drawBitmap(igraphics->getWindowDisplayBuffer(),winRect->left, winRect->top);
			win->setFrameRedraw(false);
		}
#endif
		//draw browser
		invalid = igraphics->getDirtyRect();
	
		screenBuffer->drawBitmap(igraphics->getDisplayBuffer(), invalid.left + win->getClientOffsetX(), invalid.top + win->getClientOffsetY(), 
			invalid.right - invalid.left + 1, invalid.bottom - invalid.top + 1);
		
		//portfb_display(screenBuffer->getDisplayBuffer());
	}
	
}

