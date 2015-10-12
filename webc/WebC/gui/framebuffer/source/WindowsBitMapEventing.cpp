#ifdef WIN32
#ifndef WINDOWSGDI		/* So we can build GDI and Framebuffer from the same project */

#include "WindowsBitMapFrameBuffer.h"
#include "webcmem.h"
#include "assert.h"
#include "FBConfig.h"
#include "Remoteterminal.h"
#include "FBAbstraction.h"


using namespace Gdiplus;


#define IDM_EXIT				105


extern void * currentActivatedWindow; //current window which is active

/**
*	This method's job is to receive messages from a driver and queue them up.
*	If, at the end of the method, a message does not have a destination or has no specific type it is
*   discarded.  This method should be called from the main messaging loop.
*
*	To change this method to your specific target it must be made to receive messages from keyboard and mouse drivers.
*
*
*
*	Params: OS specific, whatever is required by thread mechanism/drivers
*	Return: OS specific, whatever is required by thread mechanism/drivers
*/

static WebKey webc_TranslateFBKey(WEBC_UINT32 rawKey);
static WebKey pcKbToWebKey(short keyValue);
static void webc_check_clipboard(HWND hWnd, char *pastebuffer);
static char pastebuffer[1024];
//used for key strokes
static int giShift = 0;
static int giCaps = 0;
static int giCtrl = 0;
static int giAlt = 0;
static int giEscapeDown = 0;
LRESULT CALLBACK messageReceive(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
	int send_to_webc = 0;
	int wmId; //variable for windows exit function
	WebKey keyPressed;
	HTMLEvent e;
	e.target = NULL;
	e.type = HTML_EVENT_NONE;
	e.flags = 0;


	DISPLAY_INT x,y;
//	short keyValue;
	pastebuffer[0]=0;

	switch(message){
		case WM_RBUTTONDOWN:     // right button down
			x = LOWORD(lParam);  // extract the x position related with event
			y = HIWORD(lParam);	 //  extract the y position related with event
			send_to_webc = mouse_button_event(RBUTTON_DOWN,x,y,&e); // // Format a MOUSE or TOUCH event for webC
			break;
		case WM_LBUTTONDOWN:    // left button down
			x = LOWORD(lParam); // extract the x position related with event
			y = HIWORD(lParam); // extract the y position related with event
			send_to_webc = mouse_button_event(LBUTTON_DOWN,x,y,&e);
			SetCapture(hWnd);   // Make sure we get an UP, even if the user moves out on hWnd
			break;
		case WM_LBUTTONUP:      // left button up
			x = LOWORD(lParam); // the x position related with event
			y = HIWORD(lParam); //  y position related with event
			send_to_webc = mouse_button_event(LBUTTON_UP,x,y,&e);
			ReleaseCapture();
			break;
		case WM_RBUTTONUP:      // right button up 
			webc_check_clipboard(hWnd, pastebuffer); // Under windows we support pasting to webC
			if (pastebuffer[0])
				e.type = HTML_EVENT_KEYDOWN;
			else
			{
				x = LOWORD(lParam); //change to extract the x position related with event
				y = HIWORD(lParam); //change to extract the y position related with event
				send_to_webc = mouse_button_event(RBUTTON_UP,x,y,&e);
			}
			break;
		case WM_MOUSEMOVE:      // the mouse moves
			x = LOWORD(lParam); // extract the x position related with the event
			y = HIWORD(lParam); // extract the y position related with the event
			send_to_webc = mouse_move_event(x,y,&e);
			break;
		case WM_KEYDOWN:        // key down
			/*
			*  keyValue is the driver value of the key where keyValue must be set to either an ascii value from the keyBoard
			*  i.e. 'a' or 'A' or keyValue is one of the values defined in keyData i.e. WGK_TAB or WGK_SHIFT
			*  The assignment of keyValue must be changed to extract the keypressed from the drivers sent value
			*/
			keyPressed = pcKbToWebKey((short) (HIWORD(lParam) & 0xff));

			//set modifier variables
			if(keyPressed == WGK_SHIFT){
				giShift = 1;
			}
			else if(keyPressed == WGK_CTRL){
				giCtrl = 1;
			}
			else if(keyPressed == WGK_CAPS){
				giCaps = ~giCaps;
			}
			else if(keyPressed == WGK_ALT){
				giAlt = 1;
			}
			else if(keyPressed == WGK_ESC){
				giEscapeDown = 1; // ESC is both a modifier and a keypress
				send_to_webc = keyboard_event(KEY_DOWN, keyPressed, &e);
			}
			else
			{
				//set modifier flags
				if (giShift)
					e.flags |= HTML_EVENT_FLAG_SHIFT_DOWN;
				if (giCtrl)
					e.flags |= HTML_EVENT_FLAG_CTRL_DOWN;
				if (giAlt)
					e.flags |= HTML_EVENT_FLAG_ALT_DOWN;
				send_to_webc = keyboard_event(KEY_DOWN, keyPressed, &e); // Format a KEY DOWN event for webC
			}			
			break;


		case WM_KEYUP: //change this case to be the keyboard drivers key up
			/*
			*  keyValue is the driver value of the key where keyValue must be set to either an ascii value from the keyBoard
			*  i.e. 'a' or 'A' or keyValue is one of the values defined in keyData i.e. WGK_TAB or WGK_SHIFT
			*  The assignment of keyValue must be changed to extract the key pressed from the drivers sent value
			*/
			keyPressed = pcKbToWebKey((short) (HIWORD(lParam) & 0xff));
			if(keyPressed == WGK_SHIFT)
				giShift = 0;
			else if(keyPressed == WGK_CTRL)
				giCtrl = 0;
			else if(keyPressed == WGK_ALT)
				giAlt = 0;
			else 
			{
				if(keyPressed == WGK_ESC)
					giEscapeDown = 0;
				else
					send_to_webc = keyboard_event(KEY_UP, keyPressed, &e); // Format a KEY UP event for webC
			}
			break;
// ===
		case WM_TIMER:
		{
			PAINTSTRUCT ps;        // Not sure if we need BeginPaint EndPaint but it's windows specific anyway.
			BeginPaint(hWnd, &ps);
			EndPaint(hWnd, &ps);
			send_to_webc = webc_timer_event(&e); // Format a timer event for webC
		}
		break;
// ====
		case WM_DESTROY:
			PostQuitMessage(0);
			break;
		case WM_COMMAND:   //windows specific case, close the simulator and free stuff
			wmId  = LOWORD(wParam);
			switch (wmId)
			{
				case IDM_EXIT:
					DestroyWindow(hWnd);
				default:
					break;
			}
		default:
			send_to_webc = 0;
			break;
	}

	if (send_to_webc)
	{
		if (e.type == HTML_EVENT_KEYDOWN && pastebuffer[0]) // Windows specific. Handle paste
		{
		int i;
			for (i = 0; pastebuffer[i]; i++)
			{
				e.data.key = pastebuffer[i];
				e.type = HTML_EVENT_KEYDOWN;
				webc_MessageReceive(&e);
				e.data.key = pastebuffer[i];
				e.type = HTML_EVENT_KEYUP;
				webc_MessageReceive(&e);
			}
		}
		else
		{
			if (webc_MessageReceive(&e)!=0) // Will return non zero if not in our client area (this check is porbably not needed.
				return DefWindowProc(hWnd, message, wParam, lParam);
		}
		return 0;
	}
	else
	{
		return DefWindowProc(hWnd, message, wParam, lParam); //change this to execute default process not related to the keyboard and mouse drivers
	}
}

// Windows specific, on right click we paste from the clipboard into the webC queue
static void webc_check_clipboard(HWND hWnd, char *pastebuffer)
{
    HGLOBAL   hglb;
    LPTSTR    lptstr;
	pastebuffer[0] = 0;
    {
        if (!IsClipboardFormatAvailable(CF_TEXT))
            return;
        if (!OpenClipboard(hWnd))
            return;

        hglb = GetClipboardData(CF_TEXT);
        if (hglb != NULL)
        {
            lptstr = (LPTSTR) GlobalLock(hglb);
            if (lptstr != NULL)
            {
			char *ptext;
			ptext = (char *) lptstr;
				strcpy(pastebuffer, ptext);
                GlobalUnlock(hglb);
            }
        }
        CloseClipboard();

        return;
    }
}

// Called from FB_ProcessInputQueue() which is called when webC is in a loop that does not return immediately to the top event loop
// Gives the Windows event loop a chance to run
int Target_ProcessInputQueue(
	void		 *fn,
	WEBC_BOOL  block,
	WEBC_INT32 *exitCode
	)
{
	MSG msg;
	if (block || PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE|PM_NOYIELD))
	{
		int status = GetMessage(&msg, NULL, 0, 0);
		if (status)
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			return (-1);
		}
	}
	return 0;
}


// PC-AT keyboard translation. Not Windows specific per-se. Borrow this code if you need AT key translation
/**
*	Translates an unreadable key into a readable one via the mappings created here in portfb.cpp
*
*	Params: rawKey - the unreadable key
*	Return: WebKey - the readable key
*/

//this is platform specific
#define WG_FB_NUM_KEYS 32//define for keys

//assignes key strokes to numbers, this is platform specific
static WEBC_UINT32 keyData [WG_FB_NUM_KEYS*2] =
{
	WGK_TAB         , 0x09 ,
	WGK_CR          , 0x0d ,
	WGK_PGUP        , 604  ,
	WGK_PGDN        , 605  ,
	WGK_LNUP        , 606  ,
	WGK_LNDN        , 607  ,
	WGK_ALT         , 610  ,
	WGK_CTRL        , 611  ,
	WGK_SHIFT       , 612  ,
	WGK_CAPS        , 613  ,
	WGK_NUMLOCK     , 614  ,
	WGK_SCROLLLOCK  , 615  ,
	WGK_F1          , 650  ,
	WGK_F2          , 651  ,
	WGK_F3          , 652  ,
	WGK_F4          , 653  ,
	WGK_F5          , 654  ,
	WGK_F6          , 655  ,
	WGK_F7          , 656  ,
	WGK_F8          , 657  ,
	WGK_F9          , 658  ,
	WGK_F10         , 659  ,
	WGK_F11         , 660  ,
	WGK_F12         , 661  ,
	WGK_INSERT      , 601  ,
	WGK_DELETE      , 600  ,
	WGK_BACKSPACE   , 0x08 ,
	WGK_LEFT        , 608  ,
	WGK_RIGHT       , 609  ,
	WGK_HOME        , 602  ,
	WGK_END         , 603  ,
	WGK_ESC         , 0x1b
};


//key map variables
static WebcKeyMap keyMap;
static WEBC_BOOL  keyMapInitialized = WEBC_FALSE;
#define AT_KEYTABLE_LEN 0x59
// each key in non-shifted and shifted state
static int keyboardCharacters[AT_KEYTABLE_LEN][2] = {
	{0x00, 0x00},
	{0x1b, 0x1b},
	{'1', '!'},
	{'2', '@'},
	{'3', '#'},
	{'4', '$'},
	{'5', '%'},
	{'6', '^'},
	{'7', '&'},
	{'8', '*'},
	{'9', '('},
	{'0', ')'},
	{'-', '_'},
	{'=', '+'},
	{0x08, 0x08},
	{0x09, 0x09},
	{'q', 'Q'},
	{'w', 'W'},
	{'e', 'E'},
	{'r', 'R'},
	{'t', 'T'},
	{'y', 'Y'},
	{'u', 'U'},
	{'i', 'I'},
	{'o', 'O'},
	{'p', 'P'},
	{'[', '{'},
	{']', '}'},
	{0x0d, 0x0d},
	{611, 611},
	{'a', 'A'},
	{'s', 'S'},
	{'d', 'D'},
	{'f', 'F'},
	{'g', 'G'},
	{'h', 'H'},
	{'j', 'J'},
	{'k', 'K'},
	{'l', 'L'},
	{';', ':'},
	{'\'','\"'},
	{'`', '`'},
	{612, 612},
	{'\\','|'},
	{'z', 'Z'},
	{'x', 'X'},
	{'c', 'C'},
	{'v', 'V'},
	{'b', 'B'},
	{'n', 'N'},
	{'m', 'M'},
	{',', '<'},
	{'.', '>'},
	{'/', '?'},
	{612, 612},
	{'*', '*'},
	{610, 610},
	{' ', ' '},
	{613, 613},
	{650, 650},
	{651, 651},
	{652, 652},
	{653, 653},
	{654, 654},
	{655, 655},
	{656, 656},
	{657, 657},
	{658, 658},
	{659, 659},
	{614, 614},
	{615, 615},
	{602, 602},
	{606, 606},
	{604, 604},
	{'-', '_'},
	{608, 608},
	{0, 0},
	{609, 609},
	{'+', '+'},
	{603,603},
	{607, 607},
	{605, 605},
	{601, 601},
	{600, 600},
	{0, 0},
	{0, 0},
	{0, 0},
	{660,660},
	{661,661}
};


static WebKey webc_TranslateFBKey(WEBC_UINT32 rawKey)
{
	if (!keyMapInitialized)
	{
		webc_CreateKeyMap(&keyMap, keyData, WG_FB_NUM_KEYS);
		keyMapInitialized = WEBC_TRUE;
	}

	return webc_TranslateKey(&keyMap, rawKey);
}
static WebKey pcKbToWebKey(short keyValue)
{
	short oldkeyValue=keyValue;
		keyValue = keyboardCharacters[keyValue][(giShift || giCaps)? 1 : 0]; //turn keyValue into ascii or keyData value
//			keyValue = HIWORD(lParam) & 0xff;
//			keyValue = keyboardCharacters[keyValue][(giShift || giCaps)? 1 : 0]; //turn keyValue into ascii or keyData value
//			keyboard_event(KEY_DOWN, webc_TranslateFBKey(keyValue), e);

		return(webc_TranslateFBKey(keyValue));
}

#endif /* WINDOWSGDI	*/
#endif // WIN32
