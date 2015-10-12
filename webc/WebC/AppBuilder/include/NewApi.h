#include "htmlsdk.h"
#include "listener.hpp"
#include "rtpprint.h"
#include "rtpstr.h"
#include "helement.hpp"
#include "delement.hpp"
#include "dmanager.hpp"
#include "webctmo.h"

#define HELEMENT_OBJ(X)           ((HTMLElement *)(X))
#define DELEMENT_HANDLE void *
#define HDOC_OBJ(X)               ((HTMLDocument *)(X))



HELEMENT_HANDLE webc_GetControlSprite(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc);

enum e_DraggingCallbackType
{
	DRAGGING=0,
	DONEDRAGGING
};
typedef enum e_DraggingCallbackType DraggingCallbackType;

enum e_DraggingCallbackResponse
{
	KEEPDRAGGING=0,
	STOPDRAGGING,
	RESETDRAGGING
};
typedef enum e_DraggingCallbackResponse DraggingCallbackResponse;

///
typedef DraggingCallbackResponse (*DraggingCallbackFunction) (
		HBROWSER_HANDLE hbrowser,
		HDOC_HANDLE hdoc,
		DraggingCallbackType CallType,
		struct _webcDraghandle *DragInfo
	);


#define DRAGOBJECTMAGICNUMBER 0x12344321
struct _webcDraghandle	{
	HELEMENT_HANDLE DraggingElement;						/* Element we are dragging */
	unsigned long Signature;
	int DraggingElementStartX;
	int DraggingElementStartY;
	int DraggingElementCurrentX;
	int DraggingElementCurrentY;
	int DraggingPointerStartX;
	int DraggingPointerStartY;
#define HASXMIN 1
#define HASXMAX 2
#define HASYMIN 4
#define HASYMAX 8
	unsigned long DraggingFlags;

	int DraggingElementminX;
	int DraggingElementmaxX;
	int DraggingElementminY;
	int DraggingElementmaxY;


	DraggingCallbackFunction cb;									/* Function to call when dragging occurs */
};


enum e_SlidingCallbackType
{
	SLIDDING=0,
	DONESLIDDING
};
typedef enum e_SlidingCallbackType SlidingCallbackType;

enum e_SlidingCallbackResponse
{
	KEEPSLIDDING=0,
	STOPSLIDDING,
	RESETSLIDDING
};
typedef enum e_SlidingCallbackResponse SlidingCallbackResponse;

///
typedef SlidingCallbackResponse (*SlidingCallbackFunction) (
		HBROWSER_HANDLE hbrowser,
		HDOC_HANDLE hdoc,
		SlidingCallbackType CallType,
		struct _webcDraghandle *DragInfo
	);

#define SLIDINGWINDOW_VERTICAL 		1
#define SLIDINGWINDOW_HORIZONTAL 	2

int  webc_SlidingWindowOpen(
	HBROWSER_HANDLE hbrowser,
	HDOC_HANDLE hdoc,
	int WindowTop,
	int WindowLeft,
	int WindowHeight,
	int WindowWidth,
	HELEMENT_HANDLE SlidingElement,
	int SlidingElementStartTop,
	int SlidingElementStartLeft,
	unsigned long Controlflags,
	SlidingCallbackFunction cb
);

int webc_ElementGetXYWHPosition(HELEMENT_HANDLE element, int *X, int *Y, int *W, int *H);
int webc_SetControlSpritePosition(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, int X, int Y);
int webc_ElementSetXYPosition(HELEMENT_HANDLE element, int X, int Y);
int webc_ElementSetXYWHPosition(HELEMENT_HANDLE element, int X, int Y,int W, int H);
int webc_ElementSetZindex(HELEMENT_HANDLE helem, int z);
int webc_ElementHide(HELEMENT_HANDLE helem, WEBC_BOOL doHide);
HELEMENT_HANDLE webc_GetControlSprite(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc);
struct _webcDraghandle * webc_DragStart(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, int elemStartX, int elemStartY, DraggingCallbackFunction cb, int x, int y);
HELEMENT_HANDLE webc_ElementAppendInnerHtmlASCII(HELEMENT_HANDLE element, char * newhtml);
HELEMENT_HANDLE webc_ElementFixedGetLastChild (HELEMENT_HANDLE element);
char *webc_Strmalloc(char *str);

int webc_ElementRemoveChild (HELEMENT_HANDLE element,HELEMENT_HANDLE child);
int webc_ElementDeleteChildren (HELEMENT_HANDLE element);
int webc_ElementInsertFirst(HELEMENT_HANDLE element,HELEMENT_HANDLE child);
int webc_ElementInsertLast(HELEMENT_HANDLE element,HELEMENT_HANDLE child);
int webc_ElementInsertAfter(HELEMENT_HANDLE element,HELEMENT_HANDLE insertthischild,HELEMENT_HANDLE afterthichild);
int webc_ElementInsertBefore(HELEMENT_HANDLE element,HELEMENT_HANDLE insertthischild,HELEMENT_HANDLE beforehichild);
HELEMENT_HANDLE webc_CreateElementFromHtml(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, char *Html);
DELEMENT_HANDLE webc_GetDisplayElement(HELEMENT_HANDLE element);
void webc_ElementEnableDisplayEvents(HELEMENT_HANDLE hElem);
HELEMENT_HANDLE webc_GetRootContainer(HDOC_HANDLE hdoc);
int webc_ElementDelete(HELEMENT_HANDLE element);
void webc_ElementRedraw(HELEMENT_HANDLE element);


