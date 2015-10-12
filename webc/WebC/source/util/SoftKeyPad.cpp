//
// SoftKeyPad.CPP - Methods for Class SoftKeyPad
//
// EBS - WebC
//
// Copyright EBS Inc. , 2011
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//  [tbd]
//

/*****************************************************************************/
// Header files
/*****************************************************************************/

#include "SoftKeyPad.hpp"
#include "wgkeys.hpp"
#include <stdio.h>
#include "delement.hpp"
#include "helement.hpp"
#include "htmlbrow.hpp"

#include "rtpprint.h"
#if (0)
static HTMLEventStatus kbBodyCallback(wcCtx* Ctx, wcEL element,HTMLEvent* event,	char* param);

/*****************************************************************************/
// Local Constants
/*****************************************************************************/

/*****************************************************************************/
// Local type definitions
/*****************************************************************************/

/*****************************************************************************/
// Local Functions
/*****************************************************************************/

/*****************************************************************************/
// Global Data
/*****************************************************************************/

/*****************************************************************************/
// SoftKeyPad Constructors
/*****************************************************************************/


SoftKeyPad *gSoftKeyPad;
WEBC_BOOL gSoftKeyPadEnabled;

#define TEXTFONTHEIGHT 20

SoftKeyPad::SoftKeyPad(const int Height, const int Width, const int *KeyIndexMap, const Softkey *KeyPadMap, const char *pImgUrl0,const char *pImgUrl1,const char *pImgUrl2,const char *pImgUrl3)
{
    SoftKeyPad::Height=Height;
    SoftKeyPad::Width=Width;
    SoftKeyPad::KeyIndexMap=KeyIndexMap;
    SoftKeyPad::KeyPadMap=KeyPadMap;
    SoftKeyPad::pImgUrls[0]=pImgUrl0;
    SoftKeyPad::pImgUrls[1]=pImgUrl1;
    SoftKeyPad::pImgUrls[2]=pImgUrl2;
    SoftKeyPad::pImgUrls[3]=pImgUrl3;
    SoftKeyPad::ScreenHeight=WEBC_SCREEN_HEIGHT;
	KeypadOffsetX = 0;
	KeypadOffsetY = TEXTFONTHEIGHT;
	FocusBrowser = 0;
	FocusElement = 0;
	ProcessingTextArea = wcFALSE;

	nKeyIndexMapPairs = 0;
	// Calculate the number of entries in our -1 terminated lists
	while (*KeyIndexMap != -1)
	{
		nKeyIndexMapPairs += 1;
		KeyIndexMap += 2;
	}
	nKeyPadMapEntries=0;
	while (KeyPadMap->x != -1)
	{
		nKeyPadMapEntries += 1;
		KeyPadMap++;
	}

	struct HTMLBrowserWindowConfig conf;
	
	rtp_memset(&conf, 0, sizeof(conf));
	conf.top=0;
	conf.left=0;
	conf.width = Width+KeypadOffsetX;
	conf.height= Height+KeypadOffsetY;
	conf.BorderThickness = 0;
	conf.BorderColor = 0x0;
	conf.SetBodyBackGroundColor    = WEBC_FALSE;
	conf.BackGroundColor =0x0000; ;
	conf.WindowFlags = WF_SPRITE|WF_DISABLESCROLLBARS|WF_STARTINBG;

	if (wcWinCreateEx(
		&WindowCtx,
		"KeyPad",
		kbBodyCallback,
		(char *) 0, // pImgUrl0,
		0,
		&conf) == 0)
	{
		wcPushData(&WindowCtx, 0, (void *) this);	// Save private data so we can get our object from callbacks
	}
	SoftKeyPad::pImageObj = 0;	/* So ReSizeTextWindow doesn't move us */
	pTextWinObj = (HELEMENT_HANDLE) wcElNew(&WindowCtx, "textarea"); // Text window
	wcDocInsertLast(&WindowCtx,pTextWinObj);
	wcElSetValue(pTextWinObj, " ");
	wcElSetStyle(pTextWinObj, "overflow=hidden", wcTRUE);

	wcEL ImageObj 	= wcElNew(&WindowCtx, "img");
	wcDocInsertLast(&WindowCtx,ImageObj);
	SoftKeyPad::pImageObj = (HELEMENT_HANDLE) ImageObj;
	currentKeyMap=0;
	webc_ElementSetSrcASCII((HELEMENT_HANDLE) ImageObj, (char *)pImgUrl0, WEBC_TRUE);
	ReSizeTextWindow(KeypadOffsetY, wcTRUE);
	gSoftKeyPad = this;
	// Hack, set focus so when we unset the other guys gets raised, need a zorder interface to window.
//	wcWinSetFocus(&WindowCtx,0);
}
void SoftKeyPad::ReSizeTextWindow(int height, wcBOOL doUpdate)
{
char stylebuff[80];

	if ((Height + height) > ScreenHeight)
		height = ScreenHeight-Height;
	KeypadOffsetY = height;
	// Resize and reposition the window */
	wcWinSetPos(&WindowCtx, 0, ScreenHeight-(Height + height), Width, Height + height);

	rtp_sprintf(stylebuff, "height:%dpx;width:%dpx;", height, Width);
	if (pTextWinObj)
		wcElSetStyle(pTextWinObj, stylebuff , doUpdate);
	if (pImageObj)
	{
		rtp_sprintf(stylebuff, "position:absolute;left:%dpx;top:%dpx;", 0, height);
		wcElSetStyle(pImageObj, stylebuff , doUpdate);
	}
}

void SoftKeyPad::PopDownKeypad(wcBOOL KeepInput)
{
	HTMLElement *pTextWin;
	wcCtx Ctx;
	if (FocusElement&&pTextWinObj&&KeepInput&&closeFn)
	{

		// Store the focus elements when we get key touches
		pTextWin=(HTMLElement *)pTextWinObj;
		WEBC_CHAR *p = pTextWin->Value();
		closeFn((void *)FocusElement, p);
	}


	// Set the focus, to the soft application window
	wcCtxtInit(&Ctx, (wcBROW) FocusBrowser, (wcDOC) FocusBrowser->GetDocument());
	wcWinBringToTop(&Ctx);
	// wcWinSetFocus(&Ctx,(wcEL) FocusElement);
	FocusElement=0;
	closeFn=0;
	// Release our grab on the pointer
	wcWinReleaseFocus(&WindowCtx);
}

void SoftKeyPad::PopUpKeypad(HTMLBrowser *pBrowser, DISPLAY_INT numlines, void *pEl, WEBC_CHAR *pValue, void (*pcloseFn)(void *El, WEBC_CHAR *p))
{
	 HTMLElement *pTextWin;
	if (FocusElement)		// Already popped
		return;
	// Store the focus elements when we get key touches
	FocusBrowser = pBrowser;
	FocusElement = pEl;
	// Copy the contents of the application window to the softkey input window
	pTextWin=(HTMLElement *)pTextWinObj;
	pTextWin->SetValue(pValue);
	closeFn = pcloseFn;
	if ((numlines*TEXTFONTHEIGHT) != KeypadOffsetY)
	{
		ReSizeTextWindow(numlines*TEXTFONTHEIGHT, wcTRUE);
	}
	// Set the focus, to the soft keypad window
	wcWinBringToTop(&WindowCtx);
	// wcWinSetFocus(&WindowCtx,pTextWinObj);
	wcWinClaimFocus(&WindowCtx);
}


/*****************************************************************************/
// SoftKeyPad Destructor
/*****************************************************************************/

SoftKeyPad::~SoftKeyPad(void)
{

}

/*****************************************************************************/
// SoftKeyPad-Specific Methods
/*****************************************************************************/

/******************************************************************************
* void SoftKeyPad::XYToSoftkey(int x, int y)
*
* Parameters
*	int x,y
* Description
*    Called to map an x y offset in the keyboard map to a Key record.
*
* Returns (void)
*    A softKey record or zero if none found.
******************************************************************************/
Softkey *SoftKeyPad::XYToSoftkey(int x, int y)
{
Softkey *pMatch = 0;
	//  KeyIndexMap has pairs of Y offset, record number
	//  Outerllop allows us to only look in regions where Y matches, cuts down on compares
	for (int i = 0; i < nKeyIndexMapPairs; i++)
	{
		if (y >= KeyIndexMap[2*i] && (y <= KeyIndexMap[2*(i+1)] || KeyIndexMap[2*(i+1)] < 0))
		{
		int FirstIndex, LastIndex;
			FirstIndex = KeyIndexMap[2*i+1];
			LastIndex  = KeyIndexMap[2*(i+1)+1];
			if (LastIndex < 0)
				LastIndex = nKeyPadMapEntries;

			for (int i = FirstIndex; i < LastIndex; i++)
			{
				if (x>=KeyPadMap[i].x && x < (KeyPadMap[i].x+KeyPadMap[i].w) && y>=KeyPadMap[i].y && y < (KeyPadMap[i].y+KeyPadMap[i].h))
				return (Softkey *) KeyPadMap+i;
			}
		}
	}
	return 0;
}
#define KBREPEATPERIOD 100
static void KeyBoardRepeatCallback(int UserIntParm, void *UserVoidParm);
void SoftKeyPad::KeyBoardRepeatMethod(void)
{

	if (KeyToRepeat)
	{
		DisplayElement *pDisplay = ((HTMLElement *)pTextWinObj)->GetDisplayElement();
		HTMLEvent kEvent;
		if (pDisplay)
		{
			kEvent.type = HTML_EVENT_KEYUP;
			kEvent.data.key = KeyToRepeat;
			pDisplay->Event(&kEvent);
			kEvent.type = HTML_EVENT_KEYDOWN;
			kEvent.data.key = KeyToRepeat;
			pDisplay->Event(&kEvent);
			wcTimedCallback(KeyBoardRepeatCallback,KBREPEATPERIOD, (int) 0, (void *) this);
		}
	}
}

static void KeyBoardRepeatCallback(int UserIntParm, void *UserVoidParm)
{
SoftKeyPad *pSoftKeyPad = (SoftKeyPad *) UserVoidParm;
	if (pSoftKeyPad)
		pSoftKeyPad->KeyBoardRepeatMethod();
}




HTMLEventStatus SoftKeyPad::kbBodyCallbackMethod(wcCtx* Ctx, wcEL element,HTMLEvent* event)
{
Softkey *pSk=0;

    if (event->type == HTML_EVENT_MOUSEUP || event->type == HTML_EVENT_MOUSEDOWN)
	{ // experimental
		pSk = XYToSoftkey(event->data.position.x-KeypadOffsetX,  event->data.position.y-KeypadOffsetY);
		if (!pSk)
			return (HTML_EVENT_STATUS_CONTINUE);
		/* Process keyboard control keys here */
		WebKey KeyToSubmit = (WebKey) pSk->keys[currentKeyMap];
//		if (pSk->keys[currentKeyMap] >= WSPECIAL)
//		{
//			printf("Yo Special key == %d\n",KeyToSubmit);
//		}
		switch (pSk->keys[currentKeyMap])
		{
			/* case WXX: Add escape and keep */
			case WLC:
			case WUC:
			case WNO:
			case WSY:
				if(event->type != HTML_EVENT_MOUSEUP)
					return (HTML_EVENT_STATUS_CONTINUE);
			if(pSk->keys[currentKeyMap]==WUC)
				currentKeyMap=0;
			else if(pSk->keys[currentKeyMap]==WLC)
				currentKeyMap=1;
			else if(pSk->keys[currentKeyMap]==WNO)
				currentKeyMap=2;
			else
				currentKeyMap=3;
			webc_ElementSetSrcASCII(pImageObj, 	(char *)pImgUrls[currentKeyMap], WEBC_TRUE);
			return (HTML_EVENT_STATUS_DONE);
			case WCR:       // Carriage return
				if (ProcessingTextArea)
					KeyToSubmit = WGK_CR;
				else
				{
					PopDownKeypad(wcTRUE);
					KeyToSubmit = 0;
				}
				break;
			case WES:	// Esc
				PopDownKeypad(wcFALSE);
				KeyToSubmit = 0;
				break;
			case WAC:	// Accept
				PopDownKeypad(wcTRUE);
				KeyToSubmit = 0;
				break;
			case WCL: // Caps lock
				KeyToSubmit = 0;
			break;
			case WBS:        // Backspace
				KeyToSubmit = WGK_BACKSPACE;
				break;
			case WKR:        // Right arrow
				KeyToSubmit = WGK_RIGHT;
				break;
			case WKL:        // Right arrow
				KeyToSubmit = WGK_LEFT;
				break;

		}
		if (KeyToSubmit)
		{
			// Now do something with the key
			HTMLEvent kEvent;
			if(event->type == HTML_EVENT_MOUSEDOWN)
			{
				kEvent.type = HTML_EVENT_KEYDOWN;
				// Fire a timer off to repeat the keypress if mouseup not found
				KeyToRepeat = KeyToSubmit;
				wcTimedCallback(KeyBoardRepeatCallback,KBREPEATPERIOD, (int) KeyToSubmit, (void *) this);
			}
			else
			{
				KeyToRepeat = 0;
				kEvent.type = HTML_EVENT_KEYUP;
			}
			// Note: Trigger Event should take care of this..
			kEvent.data.key = KeyToSubmit;
			DisplayElement *pDisplay = ((HTMLElement *)pTextWinObj)->GetDisplayElement();
			if (pDisplay)
				pDisplay->Event(&kEvent);
		}
//		printf("Got %c %d\n", pSk->keys[currentKeyMap], pSk->keys[currentKeyMap]);
		return (HTML_EVENT_STATUS_DONE);
	}

	return (HTML_EVENT_STATUS_CONTINUE);
}


//void SoftKeyPad::RestoreDefaults(void)
//{
//}

static HTMLEventStatus kbBodyCallback(wcCtx* Ctx, wcEL element,HTMLEvent* event,	char* param)
{
Softkey *pSk=0;
SoftKeyPad *pMe;

	pMe = (SoftKeyPad *) wcGetData(Ctx, 0);
	if (!pMe)
		return (HTML_EVENT_STATUS_CONTINUE);
	return pMe->kbBodyCallbackMethod(Ctx,element,event);
}

#ifdef _OLDKEYMAP_
int KbGreyYIndexMap[] = {0,0,24,10,48,20,70,30,-1,-1};

Softkey KbGreyKeyMap[] = {
/* X,Y,H,W, TF0, TF1, TF2  */
{0,0,22,24,  {KC('Q'),KC('q'),KC('7'),0}},
{27,0,22,24, {KC('W'),KC('w'),KC('8'),0}},
{52,0,22,24, {KC('E'),KC('e'),KC('9'),0}},
{78,0,22,24, {KC('R'),KC('r'),KC('/'),0}},
{104,0,22,24,{KC('T'),KC('t'),KC('*'),0}},
{131,0,22,24,{KC('Y'),KC('y'), 0,0}},
{157,0,22,24,{KC('U'),KC('u'), 0,0}},
{182,0,22,24,{KC('I'),KC('i'), 0,0}},
{209,0,22,24,{KC('O'),KC('o'), 0,0}},
{235,0,22,24,{KC('P'),KC('p'), 0,0}},


{0 , 24,22,24, {KC('A'),KC('a'),KC('4'),0}},
{27, 24,22,24, {KC('S'),KC('s'),KC('5'),0}},
{52, 24,22,24, {KC('D'),KC('d'),KC('6'),0}},
{78, 24,22,24, {KC('F'),KC('f'),KC('-'),0}},
{104,24,22,24, {KC('G'),KC('g'),WBS,0}},
{131,24,22,24, {KC('H'),KC('h'),  0,0}},
{157,24,22,24, {KC('J'),KC('j'),  0,0}},
{182,24,22,24, {KC('K'),KC('k'),  0,0}},
{209,24,22,24, {KC('L'),KC('l'),  0,0}},
{235,24,22,24, {WBS,WBS, 0,0}},

{0 , 48,22,24, {KC('Z'),KC('z'),KC('1'),0}},
{27, 48,22,24, {KC('X'),KC('x'),KC('2'),0}},
{52, 48,22,24, {KC('C'),KC('c'),KC('3'),0}},
{78, 48,22,24, {KC('V'),KC('v'),WCR,0}},
{104,48,22,24, {KC('B'),KC('b'),WCR,0}},
{131,48,22,24, {KC('N'),KC('n'),WUC,0}},
{157,48,22,24, {KC('M'),KC('m'),WUC,0}},
{182,48,22,24, {KC('*'),KC('*'),  0,0}},
{209,48,22,24, {WCR, WCR,  0,0}},
{235,48,22,24, {WCR, WCR,  0,0}},


{0 , 70,22,24, {WLC,WUC,KC('.'),0}},
{27, 70,22,24, {WLC,WUC,KC('0'),0}},
{52, 70,22,24, {KC(' '),KC(' '),KC('0'),0}},
{78, 70,22,24, {KC(' '),KC(' '),KC('+'),0}},
{104,70,22,24, {KC(' '),KC(' '),KC('+'),0}},
{131,70,22,24, {KC(' '),KC(' '),WLC,0}},
{157,70,22,24, {KC(' '),KC(' '),WLC,0}},
{182,70,22,24, {KC(' '),KC(' '),  0,0}},
{209,70,22,24, {WNO, WNO,  0,0}},
{235,70,22,24, {WNO, WNO,  0,0}},
{-1,-1,0,0, {0,0,0,0}},
};


#define KBYINDEXMAP KbGreyYIndexMap
#define KBKEYMAP    KbGreyKeyMap
#define KBHEIGHT 130
#define KKBWIDTH 300
#define KBURL0 (char *)"file://\\SoftKeyPad\\KbGreyUpper.jpg"
#define KBURL1 (char *)"file://\\SoftKeyPad\\KbGreyLower.jpg"
#define KBURL2 (char *)"file://\\SoftKeyPad\\KbGreyNumeric.jpg"
#define KBURL3 (char *)0
#else

int KbGreyYIndexMap[] = {1,0,32,10,64,20,96,30,-1,-1};
/*
WKR : Right
WES : Escape
WAC : Accept
WBS : Back Space
WCR : Carrige Return ??
WSY : SYMBOL
WNO : NUMBERS
WCL : CAPS LOCK
WSP : Space
*/
Softkey KbGreyKeyMap[] = {
/* X,Y,H,W, TF0, TF1, TF2  */
{1,1,30,30,  {KC('Q'),KC('q'),KC('1'),KC('!')}},
{33,1,30,30, {KC('W'),KC('w'),KC('2'),KC('@')}},
{65,1,30,30, {KC('E'),KC('e'),KC('3'),KC('#')}},
{97,1,30,30, {KC('R'),KC('r'),KC('4'),KC('$')}},
{129,1,30,30,{KC('T'),KC('t'),KC('5'),KC('%')}},
{161,1,30,30,{KC('Y'),KC('y'),KC('6'),KC('^')}},
{193,1,30,30,{KC('U'),KC('u'),KC('7'),KC('&')}},
{225,1,20,30,{KC('I'),KC('i'),KC('8'),KC('*')}},
{257,1,30,30,{KC('O'),KC('o'),KC('9'),KC('(')}},
{289,1,30,30,{KC('P'),KC('p'),KC('0'),KC(')')}},


{1 , 32,30,30, {KC('A'),KC('a'),KC('`'),KC('~')}},
{33, 32,30,30, {KC('S'),KC('s'),KC(';'),KC(':')}},
{65, 32,30,30, {KC('D'),KC('d'),KC('\''),KC('\"')}},
{97, 32,30,30, {KC('F'),KC('f'),KC(','),KC('<')}},
{129,32,30,30, {KC('G'),KC('g'),KC('.'),KC('>')}},
{161,32,30,30, {KC('H'),KC('h'),KC('/'),KC('?')}},
{193,32,30,30, {KC('J'),KC('j'),KC('-'),KC('_')}},
{225,32,30,30, {KC('K'),KC('k'),KC('='),KC('+')}},
{257,32,30,30, {KC('L'),KC('l'),KC('['),KC('{')}},
{289,32,30,30, {WBS,	WBS, 	WBS,	WBS}},

{1 , 64,30,30, {KC('Z'),KC('z'),KC(']'),KC('}')}},
{33, 64,30,30, {KC('X'),KC('x'),KC('\\'),KC('|')}},
{65, 64,30,30, {KC('C'),KC('c'),KC('@'),0}},
{97, 64,30,30, {KC('V'),KC('v'),KC(':'),0}},
{129,64,30,30, {KC('B'),KC('b'),0,0}},
{161,64,30,30, {KC('N'),KC('n'),0,0}},
{193,64,30,30, {KC('M'),KC('m'),0,0}},
{225,64,30,30, {WCR,	WCR,	WCR,WCR}},
{257,64,30,30, {WAC, 	WAC,  	WAC,WAC}},
{289,64,30,30, {WKR, 	WKR,  	WKR,WKR}},


/*
{0 , 96,30,30, {WLC,WUC,KC('.'),0}},
{27, 96,30,30, {WLC,WUC,KC('0'),0}},
{52, 96,30,30, {KC(' '),KC(' '),KC('0'),0}},
{78, 96,30,30, {KC(' '),KC(' '),KC('+'),0}},
{104,96,30,30, {KC(' '),KC(' '),KC('+'),0}},
{131,96,30,30, {KC(' '),KC(' '),WLC,0}},
{157,96,30,30, {KC(' '),KC(' '),WLC,0}},
{182,96,30,30, {KC(' '),KC(' '),  0,0}},
{209,96,30,30, {WNO, WNO,  0,0}},
{235,96,30,30, {WNO, WNO,  0,0}},
{-1,-1,0,0, {0,0,0,0}},
*/
#define WSP (unsigned char) ' '
{0 , 96,30,56, {WCL,WCL,0,0}},
{58 , 96,30,43, {WNO,WNO,WSY,WNO}},  //Switch to number
{103 , 96,30,114, {WSP,WSP,WSP,WSP}}, //Space
{219 , 96,30,59, {WLC,WUC,WUC,WUC}}, // abc key
// {219 , 96,30,59, {WCL,WCL,WCL,WCL}}, //CAPS LOCK
{280 , 96,30,39, {WES,WES,WES,WES}}, //ESCAPE

{-1,-1,0,0, {0,0,0,0}},
{-1,-1,0,0, {0,0,0,0}},
{-1,-1,0,0, {0,0,0,0}},
{-1,-1,0,0, {0,0,0,0}},
{-1,-1,0,0, {0,0,0,0}},
{-1,-1,0,0, {0,0,0,0}},
};

#define KBYINDEXMAP KbGreyYIndexMap
#define KBKEYMAP    KbGreyKeyMap
#define KBHEIGHT 130
#define KKBWIDTH 320
#define KBURL0 (char *)"webc://EBS_SOFTKEY_CAPITALS.bmp"
#define KBURL1 (char *)"webc://EBS_SOFTKEY_SMALLSS.bmp"
#define KBURL2 (char *)"webc://EBS_SOFTKEY_NUMBERS.bmp"
#define KBURL3 (char *)"webc://EBS_SOFTKEY_SYMBOLS.bmp"
#endif
SoftKeyPad *pKeyPad;

extern "C" void wcSoftKeyEnable(HBROWSER_HANDLE hBrowser);
extern "C" void wcSoftKeyDisable(void);

/*****************************************************************************/
// wcSoftKeyEnable
/**
 @memo          Enable webc's soft keyboard.
 @doc           This function should typically be called from startup code of touchscreen based applications to enable touchscreen support. On touch screen devices clicks on a text edit control pop up a softkeyboard control.
				This behavior must be enabled both at compile time by setting the constant \Ref{WEBC_SUPPORT_TOUCHSCREEN} in webcfg.h and at run time by calling wcSoftKeyEnable.
				This function is typically called once at startup on touchscreen based applications. On the desktop it is used for demonstrating touchscreen support 
				where \Ref{wcSoftKeyEnable} enables the keyboard for a demonstration (\Ref{ExSoftKeyPad})and then disables it when the example applet finishes. 
				<br><b>
				Note: The softkeypad control is written in C++ using the webC 'C' so it should serve as a valuable example of a fairly complex application written with the webC API.
				The source code for the soft keyboard control resides in source/util/SoftKeyPad.cpp.
				</b><br>


 @param HBROWSER_HANDLE hBrowser - An initialized browser handle that popup keyboard window control will create itself in.
 @return        Nothing.
 @see           ExSoftKeyPad wcSoftKeyDisable PopUpKeyPad
 */
/*****************************************************************************/
void wcSoftKeyEnable(HBROWSER_HANDLE hBrowser)
{
wcCtx mCtx;
	if (!gSoftKeyPad)
	{
		mCtx.hbrowser = hBrowser;
		WEBC_NEW_VERBOSE(pKeyPad,SoftKeyPad(KBHEIGHT, KKBWIDTH, KBYINDEXMAP, KBKEYMAP, KBURL0, KBURL1, KBURL2, KBURL3),"SoftKeyPad");
	}
	gSoftKeyPadEnabled=WEBC_TRUE;
//	wcWinSetFocus(&mCtx,0);	// Set the focus back to the parent so it displays
}

/*****************************************************************************/
// wcSoftKeyDisable
/**
@memo          Disable webc's soft keyboard.
@doc           This function is typically not used for touchscreen based applications. It is used for demonstrating the touchscreen support on desktop systems where wcSoftKeyDisable 
			   \Ref{wcSoftKeyEnable} enables the keyboard for a demonstration (\Ref{ExSoftKeyPad})and then disables it when the example applet finishes. 

 @return        Nothing.
 @see           ExSoftKeyPad wcSoftKeyDisable PopUpKeyPad
 */
/*****************************************************************************/
void wcSoftKeyDisable(void)
{
	gSoftKeyPadEnabled=WEBC_FALSE;
//	wcWinSetFocus(&mCtx,0);	// Set the focus back to the parent so it displays
}

/*****************************************************************************/
// PopUpKeyPad
/**
 @memo          Pop open a keypad.
 @doc           Opens a keypad control. This routine is called when text edit control receives a mouse click and \Ref{WEBC_SUPPORT_TOUCHSCREEN} is enabled. 
 This function is not explicitly part of the API but it is listed here in case the need for a wcPoPUpKeyPad() API entry arrises. Study the source code for usage examples.
 
 @return        Nothing
 @see           ExSoftKeyPad wcSoftKeyDisable PopUpKeyPad
 */
/*****************************************************************************/
#endif
extern "C" void EMGLSysSoftKeyBoardShow(void);
void PopUpKeyPad(HTMLBrowser *pBrowser, DISPLAY_INT numlines, void *pEl, WEBC_CHAR *pValue, void (*closeFn)(void *El, WEBC_CHAR *p))
{
//    EMGLSysSoftKeyBoardShow();
//	if (gSoftKeyPad && gSoftKeyPadEnabled)
//		gSoftKeyPad->PopUpKeypad(pBrowser,numlines,pEl,pValue, closeFn);
}