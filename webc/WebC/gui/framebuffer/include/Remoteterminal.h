#pragma once


#define INCLUDE_REMOTE_TERMINAL   0  /* Set to 1 if Terminal device */

#ifdef __TIARM__
#define FORCE_HEADLESS_DEVICE     0 /* Set to 1 to force the device into headless mode */
#endif

#if (INCLUDE_REMOTE_TERMINAL)
//void call_TerminalTimout(HBROWSER_HANDLE which_Browser);   // Needed by both terminal and headless
void set_TerminalCanvas(DISPLAY_INT height, int BytesPerRow, int BytesPerPix);	 // Both
WEBC_UINT8 *get_TerminalDisplayBuffer(void);  // Headless only	  // need to clean up windows
int send_TerminalDisplayBuffer(WEBC_UINT8 *srcPtr,HTMLRect rect);
void start_HeadlessTerminal(void);		  // Headless only


//void call_TerminalCheckKey(WebKey keyPressed);
WEBC_UINT8 *get_TerminalDisplayLine(DISPLAY_INT x, DISPLAY_INT y);	// Terminal only
void send_TerminalEvent(HBROWSER_HANDLE hbrowser,	HTMLEvent* event); // Terminal
void call_TerminalCheckKey(WebKey keyPressed);  // Terminal
#else
#ifdef __TIARM__
// tbd
#else
#define call_TerminalTimout(X)
#endif
#define call_TerminalCheckKey(X)
#define get_TerminalDisplayLine(X,Y) 0
#define set_TerminalCanvas(X,Y,Z)
#define get_TerminalDisplayBuffer() 0
#define send_TerminalDisplayBuffer() 0
#define send_TerminalEvent(X, Y)
#define start_HeadlessTerminal()
#endif

#define webcport 23
#define webctype 4

#define MAXTCPSENDSIZE 8192
#define MAXTCPRECVSIZE 8192
extern "C" {
	bool IsPartialDraw(void);
//	void call_TerminalTimout(HBROWSER_HANDLE which_Browser);
}
