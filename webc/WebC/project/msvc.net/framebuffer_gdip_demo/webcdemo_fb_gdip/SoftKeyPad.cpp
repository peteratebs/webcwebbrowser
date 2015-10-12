//
// SoftKeyPad.cpp - WebC GUI Example
//
//
//  C code needed for the popup soft keyboard used in page of the Spectrometer Demo.
//	See the NewClient page for an example of how it is used.
//
// Public functions:
//
// 		SetSoftKeyPadElementHandler - Finds "SoftKeyPadImage" in the document and attaches evant handlers.
// 		SetSoftKeyPadVisible		- Pop up the keyboard.
// 		SetSoftKeyPadInvisible		- Hide the keyboard.
// 		SetSoftKeyTargetElement		- Tell the keyboard control who has the focus.

//
// Internal functions:
//
//		SetSoftKeyPadTypeFace		- Helper to change between 3 keyboard faces, CAPS, NUM, and Lower.
//		SoftKeyPadHandler			- Implements the soft keyboard logic.
//		SoftKeyPadPtrToKey			- Implements image map for the soft keyboard.
//
// Calls webC api functions:
// 	webc_ElementSetTagEventHandler,webc_ElementSetStyleASCII,webc_ElementSetAttributeASCII,
// 	webc_ElementFocus,webc_ElementGetValueASCII,webc_BrowserDispatchEvent,webc_FreeASCIIString

#include "htmlsdk.h"
#include "rtpstr.h"


static HTMLEventStatus SoftKeyPadHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param);

static int KeyMapNumber;
static int KeyXorigin;
static int KeyYorigin;
static HELEMENT_HANDLE hTarget;

// 	Page startup code calls this to associate the keyboard
int SetSoftKeyPadElementHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc)
{
HELEMENT_HANDLE h = webc_DocFindElement (hdoc, "SoftKeyPadImage", 0, HTML_ELEMENT_ANY, 0);
	KeyMapNumber = 0;
	if (h)
	{
		KeyMapNumber = 0;
		webc_ElementSetTagEventHandler (h, SoftKeyPadHandler);
		return 0;
	}
	return -1;
}
// 	Focus code calls this to pop up a keyboard
int SetSoftKeyPadVisible(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc,int xorigin, int yorigin)
{
HELEMENT_HANDLE h = webc_DocFindElement (hdoc, "SoftKeyPadImage", 0, HTML_ELEMENT_ANY, 0);
	KeyXorigin = xorigin + 56;
	KeyYorigin = yorigin + 30;
	if (h)
	{
		webc_ElementSetStyleASCII(h, "visibility:visible;", true);
		return 0;
	}
	return -1;
}
// 	Carriage return keypress handler calls this to hide the keyboard
void SetSoftKeyPadInvisible(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc)
{
HELEMENT_HANDLE h = webc_DocFindElement (hdoc, "SoftKeyPadImage", 0, HTML_ELEMENT_ANY, 0);
	if (h)
		webc_ElementSetStyleASCII(h, "visibility:hidden;", true);
}

// 	Focus code calls this to tell the keyboard where to send data.
void SetSoftKeyTargetElement(HELEMENT_HANDLE h)
{
	hTarget = h;
}

void SetSoftKeyPadTypeFace(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, int KeyPadNumber);
static unsigned char SoftKeyPadPtrToKey(HTMLEvent* event);
static HTMLEventStatus SoftKeyPadHandler(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, HELEMENT_HANDLE helem, HTMLEvent* event,char * param)
{
	HELEMENT_HANDLE h;
	char c;

//  if (event->type == HTML_EVENT_MOUSEMOVE)
//	{ // Could easilly add hover effects
//  }
//   else
    if (event->type == HTML_EVENT_CLICK)
	{
		if (hTarget) // If we are active
		{
		int newKeyMapNumber = KeyMapNumber;

			c = SoftKeyPadPtrToKey(event);

			if (c == 1)		 newKeyMapNumber = 0;	// Process special key values to swap
			else if (c == 2) newKeyMapNumber = 1;	// Keyboard faces
			else if (c == 3) newKeyMapNumber = 2;
			if (newKeyMapNumber!=KeyMapNumber)
				SetSoftKeyPadTypeFace(hbrowser, hdoc, newKeyMapNumber);
			KeyMapNumber = newKeyMapNumber;
			if (c>3)								// .. else
			{
				HTMLEvent my_event;
				rtp_memset(&my_event, 0, sizeof(my_event));
            	// Set focus to the control we want
            	webc_ElementFocus(hTarget, WEBC_TRUE);
            	{   // right shift to the end because setfocus puts us at the beginning
            		char *s =  webc_ElementGetValueASCII(hTarget);
					if (s)
					{
						for (int i = 0; i < rtp_strlen(s); i++)
						{
                			my_event.type = HTML_EVENT_KEYDOWN;
                			my_event.data.key = WGK_RIGHT;
                			webc_BrowserDispatchEvent(hbrowser, &my_event);
						}
						webc_FreeASCIIString(s);
					}

				}
				// Send the key
				my_event.type = HTML_EVENT_KEYDOWN;
				if (c == 10)
					my_event.data.key = WGK_CR;
				else if (c == 8)
					my_event.data.key = WGK_BACKSPACE;
				else
					my_event.data.key = c;
				webc_BrowserDispatchEvent(hbrowser, &my_event);
			}
		}
    }
	return (HTML_EVENT_STATUS_DONE);
}

// Simple hardwired soft keyboard
static char KeyMap0[4][10] = {
	{'Q','W','E','R','T','Y','U','I','O','P',},
	{'A','S','D','F','G','H','J','K','L',8,},
	{'Z','X','C','V','B','N','M',0,10,10,},
	{2,2,32,32,32,32,32,32,3,3,}
};

static char KeyMap1[4][10]=
{
	{'q','w','e','r','t','y','u','i','o','p'},
	{'a','s','d','f','g','h','j','k','l',8},
	{'z','x','c','v','b','n','m',0,10,10},
	{1,1,32,32,32,32,32,32,3,3,}
};

static char KeyMap2[4][10]=
{
	{1,1,1,1,1,'7','8','9','/','*'},
	{2,2,2,2,2,'4','5','6','-',8},
	{0,0,0,0,0,'1','2','3',10,10},
	{0,0,0,0,0,'.','0','0','+','+'}
};

static unsigned char SoftKeyPadPtrToKey(HTMLEvent* event)
{
int x, y;
int xorigin=0,yorigin=0;
int  kwidth = 26, kheight = 20;
int  maxkeysw = 10, maxkeysh = 4;

	xorigin=KeyXorigin;
	yorigin=KeyYorigin;
	char c = 0;
	x = (event->data.position.x-xorigin)/kwidth;
	y = (event->data.position.y-yorigin)/kheight;
	if (x >= 0 && x <= maxkeysw && y >= 0 && y <= maxkeysh)
	{
		if (KeyMapNumber == 1)
			c = KeyMap1[y][x];
		else if (KeyMapNumber == 2)
			c = KeyMap2[y][x];
		else
			c = KeyMap0[y][x];
	}
	return (c);
}

static char *kbs[] = {"images\\SoftKeyPadImage0.bmp","images\\SoftKeyPadImage1.bmp","images\\SoftKeyPadImage2.bmp"};
void SetSoftKeyPadTypeFace(HBROWSER_HANDLE hbrowser, HDOC_HANDLE hdoc, int KeyPadNumber)
{
	HELEMENT_HANDLE h = webc_DocFindElement (hdoc, "SoftKeyPadImage", 0, HTML_ELEMENT_ANY, 0);
	if (h)
		webc_ElementSetAttributeASCII(h, "src", kbs[KeyPadNumber]);
}
