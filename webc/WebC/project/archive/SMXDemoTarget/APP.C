/*
* APP.C                                                     Version 3.7.1
*
* ARM Protosystem application module. Contains code to demonstrate
* use of smx. Replace this module with your application modules.
*
* Copyright (c) 1994-2009 Micro Digital Associates, Inc.
*               All Rights Reserved.
*
* Authors: Marty Cochran and David Moore
*
***********************************************************************/
#include    "smx.h"
#include    "core.h"
#ifdef PEG
//#include    "peg.hpp"
#endif
#include    "smxns.h"

#ifdef __cplusplus
 extern "C" {
#endif

#include "htmlsdk.h"
#include "arm_comm.h"

/* variables */

static dword stime_at_startup;


#if (DEVICE_CONSOLE_IN) && !(defined(PEG) && defined(WIN32))
TCB_PTR     opcon;         /* operation control task (for kbd) */
void opcon_main(void);
#endif

#if (DEVICE_CONSOLE_OUT)
TCB_PTR  display_task;
void display_task_main(void);
#endif /* DEVICE_CONSOLE_OUT */

#if !defined(WIN32)
TCB_PTR LED_task;
void LED_task_main(void);
#endif

#ifdef __cplusplus
}
#endif



#include "FBAbstraction.h"

HBROWSER_HANDLE webc_start_webc_tests(HTMLBrowserConfig *config, WEBC_BOOL CreateNewWindow, char * pTestsuiteURL);

HBROWSER_HANDLE doBrowserInit();
void doBrowserEventLoop(HBROWSER_HANDLE hBrowser);
void doBrowserMouseClick(HBROWSER_HANDLE hBrowser, int x, int y, bool bDown=true);
void doBrowserMouseOver(HBROWSER_HANDLE hBrowser, int x, int y);
void doBrowserUpdate(HBROWSER_HANDLE hBrowser);
void doBrowserShutdown(HBROWSER_HANDLE hBrowser, HTMLBrowserConfig config);


extern "C" {
void BSP_Init();
}

/***** APPLICATION INITIALIZATION
* This is a typical application initialization function. Create
* something equivalent.
**********************************************************************/

#define LOCALIP            "192.168.1.10"   /* Set our IP here */
#define LOCALMASK          "255.255.255.0"  /* Set our subnet mask here */


void _cdecl appl_init(void)
{
// int i;
   int j;

  	//BSP_Init();
	
#if 0
  
#if (DEVICE_CONSOLE_IN) && !(defined(PEG) && defined(WIN32))
   opcon = create_task(opcon_main, MAX, 0);
   startx(opcon);
   BUILD_HT(opcon, "opcon");
#endif

#endif
   
   stime_at_startup = get_stime();

  #if (DEVICE_CONSOLE_OUT)
   /* Create a task to periodically write the display[] lines to the screen. */
   display_task = create_task((CODE_PTR)display_task_main, HI, 0xc0000);
   startx(display_task);
   BUILD_HT(display_task, "display_task");
  #endif /* DEVICE_CONSOLE_OUT */

#if 0   
  #if !defined(WIN32)
   /* Create and start LED task. If board has no LED's, it does nothing. */
   LED_task = create_task((CODE_PTR)LED_task_main, HI, 0);
   BUILD_HT(LED_task, "LED_task");
   startx(LED_task);
  #endif
#endif
   
}


/***** APPLICATION EXIT
* Put application cleanup code here.
**********************************************************************/

void _cdecl appl_exit(void)
{
 
}



#if (DEVICE_CONSOLE_OUT)

/***** DISPLAY TASK and LSR
* This task writes the lines in display[] to the screen/terminal.
**********************************************************************/

void display_task_main(void)
{
  HBROWSER_HANDLE hBrowser;
  
   unlockx(self);
 
#if 1
  int error_code;
  error_code = Ninit(); /* Initialize smxNS */
  if( error_code < 0 ) 
	return; 
  
  error_code = Portinit("*"); /* Intialize all connections */ 
  if( error_code < 0 )
	return;
#endif
  
     /* Set the local IP address using strings defined at top of this file */
   //SetLocalIP(LOCALIP, LOCALMASK);

 	BSP_Init();
  
	hBrowser = doBrowserInit();
	
	doBrowserEventLoop(hBrowser);
   
}

#endif /* DEVICE_CONSOLE_OUT */



#if (DEVICE_CONSOLE_IN) && !(defined(PEG) && defined(WIN32))

/***** OPERATION CONTROL
* This task assumes a UART driver puts ASCII chars into op_pipe from
* a terminal keyboard.
**********************************************************************/

void _cdecl opcon_main(void)
{
   char  key;
  #ifdef WIN32
   dword key_dword;
  #endif

   while(TRUE)
   {
     #ifdef WIN32
      PGET_DWORD(key_dword, op_pipe, INF);
      key = (byte)key_dword;           /* for smxSim, a dword is put in pipe but only low byte is used (it is ASCII char) */
     #elif DEVICE_CONSOLE_IN_USES_INT
      key = pget_char(op_pipe, INF);   /* get from op_pipe, which is filled by lsr due to isr */
     #else
      key = get_kbd_char_polled();     /* poll; don't have interrupt-driven UART driver */
     #endif

      if (key == '\0')
         continue;

    #if defined(PEG) && defined(PEG_KEYBOARD_SUPPORT)
      // Key goes to PEG and nobody else.
      //
      // USER: If you want shift flags, you need to get them from your
      // device and pass them in the upper byte(s) of the dword passed.
      // See the comment above SendKeyToPeg() for more discussion.
      //
      SendKeyToPeg((dword)key);        /* has no shift flags */

    #else
      if(key == Esc)
         start_par(exitx,0);

     #if defined(SMXNET_DEMO)
      else if (key == Ctrl_A)
         start_server = 1;
      else if (key == Ctrl_B)
         start_client = 1;

     #elif defined(SMXUSBO_DEMO)
      else                             /* send chars to OTG task */
         pput_char(key, usbo_demo_pipe, INF);
     #endif

    #endif // ifdef PEG else
   }
}
#endif /* DEVICE_CONSOLE_IN && !(PEG && WIN32) */



#if defined(WIN32) && DEVICE_CONSOLE_OUT
void _cdecl show_etime(void)
{
}
#endif


#if !defined(WIN32)

/***** LED Task
* Display count or pattern on LED's. A board could have one or more
* of the different types of LED's, or none.
**********************************************************************/

#include "led.h"

/* select 1 pattern */

#if BOARD_LED_NUM_7SEG
#define LED_7SEG_COUNT      1
#define LED_7SEG_SNAKE      0
#define LED_7SEG_CIRCLE     0
#endif

#if BOARD_LED_NUM_IN_ROW
#define LED_ROW_COUNT       1
#define LED_ROW_ALTERNATE   0
#define LED_ROW_FLASH       0
#define LED_ROW_SCAN        0
#endif

#if BOARD_LED_NUM_7SEG
#if LED_7SEG_SNAKE
const dword LED_SnakeSegs[8] =
   {LED_SEG_A, LED_SEG_B, LED_SEG_G, LED_SEG_E,
    LED_SEG_D, LED_SEG_C, LED_SEG_G, LED_SEG_F};
#elif LED_7SEG_CIRCLE
const dword LED_CircleSegs[6] =
   {LED_SEG_A, LED_SEG_B, LED_SEG_C, LED_SEG_D,
    LED_SEG_E, LED_SEG_F};
#endif
#endif


void LED_task_main(void)
{
   u32 ctr = 0;

   unlockx(self);

   while(1)
   {
    #if BOARD_LED_NUM_IN_ROW
     #if LED_ROW_COUNT
      WriteLEDs(ctr);

     #elif LED_ROW_ALTERNATE
      WriteLEDs((ctr & 1) ? LED_PATTERN_ODD : LED_PATTERN_EVEN);

     #elif LED_ROW_FLASH
      WriteLEDs((ctr & 1) ? LED_PATTERN_ALL : LED_PATTERN_NONE);

     #elif LED_ROW_SCAN
      WriteLEDs(1 << (ctr % BOARD_LED_NUM_IN_ROW));
     #endif
    #endif /* BOARD_LED_NUM_IN_ROW */

    #if BOARD_LED_NUM_7SEG
     #if LED_7SEG_COUNT
      #if (BOARD_LED_NUM_7SEG == 2)
//    Write7SegNum(ctr % 100);  /* 0 to 99 */
      Write7SegNum(ctr % 256);  /* 0 to FF */
      #else
//    Write7SegNum(ctr % 10);   /* 0 to 9 */
      Write7SegNum(ctr % 16);   /* 0 to F */
      #endif
      count(cf.sec, ticks, INF);

     #elif LED_7SEG_SNAKE
      Write7Seg(LED_SnakeSegs[ctr % 8]);
      count(cf.sec/10, ticks, INF);

     #elif LED_7SEG_CIRCLE
      Write7Seg(LED_CircleSegs[ctr % 6]);
      count(cf.sec/10, ticks, INF);
     #endif

    #else
     #if (defined(SMXNS_DEMO) && SNS_PROTO_WEBS)
      count(cf.sec * nsdemo_count / 1000, ticks, INF);
     #else
      count(cf.sec, ticks, INF);
     #endif
    #endif /* BOARD_LED_NUM_7SEG */

      ctr++;
   }
}
#endif  /* WIN32 */




HBROWSER_HANDLE doBrowserInit()
{  
	HTMLBrowserConfig config;
	HBROWSER_HANDLE hBrowser=0;
	
	webc_Init();
	
	config.graphics = 0;
	config.BrowserHandle = 0;
	config.processInputQueue = 0;
	config.browserEventHandler = 0;
	config.privateData = 0;
	config.WindowName = "windowName";
	config.WindowTitle = "WindowTitle";
	config.WindowHandle = 0;
	config.top = 0;
	config.left = 0;
	config.height = 240;
	config.width = 320;
	config.is_resizable = WEBC_FALSE;
	config.is_modal = WEBC_FALSE;
	config.has_status = WEBC_FALSE;
	config.has_menubar = WEBC_FALSE;
	config.has_location = WEBC_FALSE;
	config.has_toolbar = WEBC_FALSE;
	config.has_scrollbars_enabled = WEBC_TRUE;
	
	if (webc_InitFBBrowserConfig(&config, 0) < 0)
	{
		return; // error
	}
	
	 //hBrowser = webc_start_webc_tests(&config, WEBC_TRUE, "webc://welcome.html");
	
	//hBrowser = webc_start_webc_tests(&config, WEBC_TRUE, "http://www.regulant.com/docs/Content/jscriptTest.htm");
	//hBrowser = webc_start_webc_tests(&config, WEBC_TRUE, "http://192.168.1.199/testsuite/Content/welcome.html");
	hBrowser = webc_start_webc_tests(&config, WEBC_TRUE, "http://192.168.1.199/testsuite/demos/mapsdemo/mapsdemo.html");
	//hBrowser = webc_start_webc_tests(&config, WEBC_TRUE, "http://www.regulant.com/docs/Content/test.html");
	//hBrowser = webc_start_webc_tests(&config, WEBC_TRUE, "http://192.168.1.199/testsuite/index.htm");
	// hBrowser = webc_start_webc_tests(&config, WEBC_TRUE, "http://www.regulant.com/docs/Content/welcome.html");
	// hBrowser = webc_start_webc_tests(&config, WEBC_TRUE, "http://www.regulant.com/docs/helpViewer/index2.htm");
	//  hBrowser = webc_start_webc_tests(&config, WEBC_TRUE, "http://m.yahoo.com/");
	
	doBrowserUpdate(hBrowser);
	
	return hBrowser;
}

void doBrowserEventLoop(HBROWSER_HANDLE hBrowser)
{
  do { ; } while(1);
	sleepx(get_stime() + 3600); // an hour?
}


void doBrowserMouseClick(HBROWSER_HANDLE hBrowser, int x, int y, bool bDown)
{
  	HTMLEvent e;
	e.flags = 0;
	e.data.position.x = x;
	e.data.position.y = y;

	webc_BrowserSuspendDraw(hBrowser);
	
	if(bDown)
	{
		// send a mouse down event
		e.type = HTML_EVENT_MOUSEDOWN;
		webc_BrowserDispatchEvent(hBrowser, &e);
	}
	else
	{
		// send a mouse up event
		e.type = HTML_EVENT_MOUSEUP;
		webc_BrowserDispatchEvent(hBrowser, &e);
	}

	if (webc_BrowserGetDrawState (hBrowser) > 0)
		webc_BrowserResumeDraw(hBrowser);
}


void doBrowserMouseOver(HBROWSER_HANDLE hBrowser, int x, int y)
{
  	HTMLEvent e;
	e.flags = 0;
	e.data.position.x = x;
	e.data.position.y = y;

	webc_BrowserSuspendDraw(hBrowser);

	// send a move event
	e.type = HTML_EVENT_MOUSEMOVE;
	webc_BrowserDispatchEvent(hBrowser, &e);
	
	if (webc_BrowserGetDrawState (hBrowser) > 0)
		webc_BrowserResumeDraw(hBrowser);

}

void doBrowserUpdate(HBROWSER_HANDLE hBrowser)
{
  if(hBrowser)
  {
		webc_BrowserProcessTimers(hBrowser);
		webc_BrowserDraw(hBrowser);
		webc_BrowserProcessLoadQueueAll(hBrowser);
		webc_BrowserDraw(hBrowser);
  }
}

void doBrowserShutdown(HBROWSER_HANDLE hBrowser, HTMLBrowserConfig config)
{
  webc_DestroyFBBrowserConfig(config.graphics);
  webc_Exit();
}



// hack to resolve function called by mozilla spidermonkey

extern "C" {

double copysign(double x, double y)
{
  double retVal = x;
  
  if((y < 0.0) && (x > 0.0))
	retVal = 0.0 - x;
  else if((y > 0.0) && (x < 0.0))
	retVal = 0.0 - x;
  
  return retVal;
}

  
}
