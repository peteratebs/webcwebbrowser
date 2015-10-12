/*************************************************************************
 *
*    Used with ICCARM and AARM.
 *
 *    (c) Copyright IAR Systems 2008
 *
 *    File name   : main.c
 *    Description : Main module
 *
 *    History :
 *    1. Date        : 12, August 2008
 *       Author      : Stanimir Bonev
 *       Description : Create
 *
 *  This example project shows how to use the IAR Embedded Workbench for ARM
 * to develop code for the IAR-LPC-2478 board. It shows basic use of I/O,
 * timer, interrupt and LCD controllers and LCD touch screen.
 *
 *  The LCD hardware cursor moves as the LCD touching.
 *
 * Jumpers:
 *  EXT/JLINK  - depending of power source
 *  ISP_E      - unfilled
 *  RST_E      - unfilled
 *  BDS_E      - unfilled
 *  C/SC       - SC
 *
 * Note:
 *  After power-up the controller get clock from internal RC oscillator that
 * is unstable and may fail with J-Link auto detect, therefore adaptive clocking
 * should always be used. The adaptive clock can be select from menu:
 *  Project->Options..., section Debugger->J-Link/J-Trace  JTAG Speed - Adaptive.
 *
 *    $Revision: 26684 $
 **************************************************************************/

#include <includes.h>
#include "htmlsdk.h"

extern "C" {
#include <Global.h>
#include <IP.h>
#include <IP_socket.h>
};

#include "FBAbstraction.h"

HBROWSER_HANDLE webc_start_webc_tests(HTMLBrowserConfig *config, WEBC_BOOL CreateNewWindow, char * pTestsuiteURL);

HBROWSER_HANDLE doBrowserInit();
void doBrowserEventLoop(HBROWSER_HANDLE hBrowser);
void doBrowserMouseClick(HBROWSER_HANDLE hBrowser, int x, int y, bool bDown=true);
void doBrowserMouseOver(HBROWSER_HANDLE hBrowser, int x, int y);
void doBrowserUpdate(HBROWSER_HANDLE hBrowser);
void doBrowserShutdown(HBROWSER_HANDLE hBrowser, HTMLBrowserConfig config);

OS_STACKPTR int StackScreenUpdate[128], StackTick[128], StackTouchGet[128], StackBrowser[0x80000], IPStack[4096], IPRxStack[512];          /* Task stacks */

OS_TASK TCBScreenUpdate, TCBTick, TCBTouchGet, IPTCB, IPRxTCB;                        /* Task-control-blocks */

OS_MAILBOX MailXY;
char MailXYBuffer[6*sizeof(MailXYType_t)];

extern const unsigned int button1[];

extern Bmp_t PowerPac;
extern Bmp_t IAR;


/*************************************************************************
 * Function Name: main
 * Parameters: none
 *
 * Return: none
 *
 * Description: main
 *
 *************************************************************************/
void OS_InitHW(void)
{
  // MAM init
  MAMCR_bit.MODECTRL = 0;
  MAMTIM_bit.CYCLES  = 3;   // FCLK > 40 MHz
  MAMCR_bit.MODECTRL = 2;   // MAM functions fully enabled
  GLCD_Ctrl (FALSE);

  // AK_TODO need to figure out why this causes malfunction
  // Init clock
  InitClock();
 #if 0 
  // Init GPIO
  GpioInit();
#endif
  // Init VIC
  VIC_Init();
 #if 0 
  // SDRAM Init
  SDRAM_Init();
#endif  
  
//  // GLCD init
//  GLCD_Init (NULL, NULL);
//
//  GLCD_Cursor_Dis(0);
//
//  GLCD_Copy_Cursor ((Int32U *)Cursor, 0, sizeof(Cursor)/sizeof(Int32U));
//
//  GLCD_Cursor_Cfg(CRSR_FRAME_SYNC | CRSR_PIX_32);
//
//  GLCD_Move_Cursor(cursor_x, cursor_y);
//
//  GLCD_Cursor_En(0);
//
//  // Init touch screen
//  TouchScrInit();
//
//  // Touched indication LED
//  USB_H_LINK_LED_SEL = 0; // GPIO
//  USB_H_LINK_LED_FSET = USB_H_LINK_LED_MASK;
//  USB_H_LINK_LED_FDIR |= USB_H_LINK_LED_MASK;
//
//  GLCD_Ctrl (TRUE);
//
//
//  GLCD_SetFont(&Terminal_18_24_12,0x0000FF,0x000cd4ff);
//  GLCD_SetWindow(95,10,255,33);
//  GLCD_TextSetPos(0,0);
//  printf("\fIAR Systems");
//
//  GLCD_SetWindow(55,195,268,218);
//  GLCD_TextSetPos(0,0);
//  printf("\fTouch screen demo");
//
//  OS_InitTimer();


  OS_InitTimer();
}



static void TickTask(void) {
  while (1) {
    Int32U t = OS_GetTime32();
#if 0
    OS_EnterRegion();
    GLCD_SetWindow(55,195,268,218);
    GLCD_TextSetPos(0,0);
    printf("\fTicks: %d", t);
    OS_LeaveRegion();
#endif
    OS_Delay (200);
  }
}

void BSP_Init()
{
  Int32U cursor_x = (C_GLCD_H_SIZE - CURSOR_H_SIZE)/2, cursor_y = (C_GLCD_V_SIZE - CURSOR_V_SIZE)/2;
  
  // GLCD init
  GLCD_Init (NULL, NULL);

  GLCD_Cursor_Dis(0);

  GLCD_Copy_Cursor ((Int32U *)Cursor, 0, sizeof(Cursor)/sizeof(Int32U));

  GLCD_Cursor_Cfg(CRSR_FRAME_SYNC | CRSR_PIX_32);

  GLCD_Move_Cursor(cursor_x, cursor_y);

  GLCD_Cursor_En(0);

  // Init touch screen
  TouchScrInit();

  // Touched indication LED
  USB_H_LINK_LED_SEL = 0; // GPIO
  USB_H_LINK_LED_FSET = USB_H_LINK_LED_MASK;
  USB_H_LINK_LED_FDIR |= USB_H_LINK_LED_MASK;

  GLCD_Ctrl (TRUE);

#if 0
  GLCD_SetFont(&Terminal_18_24_12,0x0000FF,0x000cd4ff);
  GLCD_SetWindow(95,10,255,33);

  GLCD_SetWindow(55,195,268,218);
  GLCD_TextSetPos(0,0);
  printf("\fTouch screen demo");
  
  GLCD_LoadPicTransparent ((C_GLCD_H_SIZE-IAR.H_Size)/2, 10, &IAR, 0x00ffffff);
  GLCD_LoadPicTransparent ((C_GLCD_H_SIZE-PowerPac.H_Size)/2, IAR.V_Size+20, &PowerPac, 0x00ffffff);
#endif
}

bool ReadUserButton(int whichButton)
{
  bool retVal = false;
  int regVal;
 
  OS_EnterRegion();

  if(whichButton)
  {
	regVal = BUT2_FIO;
	retVal = !(regVal & BUT2_MASK);
  }
  else
  {
	regVal = BUT1_FIO;
	retVal = !(regVal & BUT1_MASK);
  }

  OS_LeaveRegion();
  
  return retVal;
}

static void BrowserTask(void) 
{
#if 1
	IP_Init();
	OS_CREATETASK(&IPTCB, "IP_Task", IP_Task, 150, IPStack);
  
//	OS_CREATETASK(&IPRxTCB, "IP_RxTask", IP_RxTask, 149, IPRxStack);   // Start the IP_RxTask, optional.

	//
	// Wait until interface is ready.
	//
	while (IP_IFaceIsReady() == 0) {
	OS_Delay(100);
	}
#endif
	HBROWSER_HANDLE hBrowser = doBrowserInit();
	
	doBrowserEventLoop(hBrowser);

	//OS_Delay (200);
}
  
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
	//hBrowser = webc_start_webc_tests(&config, WEBC_TRUE, "http://192.168.1.199/testsuite/Content/test.html");
	hBrowser = webc_start_webc_tests(&config, WEBC_TRUE, "http://192.168.1.199/testsuite/Content/demos/mapsdemo.html");
	//hBrowser = webc_start_webc_tests(&config, WEBC_TRUE, "http://www.regulant.com/docs/Content/test.html");
	//hBrowser = webc_start_webc_tests(&config, WEBC_TRUE, "http://192.168.1.199/testsuite/index.htm");
	// hBrowser = webc_start_webc_tests(&config, WEBC_TRUE, "http://www.regulant.com/docs/Content/welcome.html");
	// hBrowser = webc_start_webc_tests(&config, WEBC_TRUE, "http://www.regulant.com/docs/helpViewer/index2.htm");
	//hBrowser = webc_start_webc_tests(&config, WEBC_TRUE, "http://www.google.com/");
	// hBrowser = webc_start_webc_tests(&config, WEBC_TRUE, "webc://test.html");
	
	doBrowserUpdate(hBrowser);
	
	return hBrowser;
}


static Int16U  lastX=0, lastY=0;

static void doBrowserEventLoop(HBROWSER_HANDLE hBrowser) 
{
	bool bDoRedaw=false;
	bool bClickState=false;
	
	while(1)
	{
		bDoRedaw = false;
		// turn off the "thinking" light
		  USB_H_LINK_LED_FSET = USB_H_LINK_LED_MASK;

		OS_U8 event = OS_WaitEventTimed(EVENT_KEY_TUCHED | EVENT_KEY_NOT_TUCHED, 200);
		if( event & EVENT_KEY_TUCHED )
		{
			MailXYType_t xy;
			OS_GetMail(&MailXY, (char*)&xy);
			OS_EnterRegion();
			lastX = xy.X;
			lastY = xy.Y;
			GLCD_Move_Cursor(xy.X, xy.Y);
			USB_H_LINK_LED_FCLR = USB_H_LINK_LED_MASK;
			OS_LeaveRegion();

			doBrowserMouseOver(hBrowser, lastX, lastY);
			bDoRedaw = true;
		}
		
		if(ReadUserButton(0))
		{
			webc_BrowserRefresh(hBrowser);
		}
		else if(ReadUserButton(1) && !bClickState)
		{
			// send a mouse down event
			doBrowserMouseClick(hBrowser, lastX, lastY, true);
			bDoRedaw = true;
			bClickState = true;
		}
		else if(!ReadUserButton(1) && bClickState)
		{
			// send a mouse up event
			doBrowserMouseClick(hBrowser, lastX, lastY, false);
			bDoRedaw = true;
			bClickState = false;
		}
		else
		{
			bDoRedaw = true; // idle draw request
		}
		
		if(bDoRedaw)
		{
		// turn on the "thinking" light
		  USB_H_LINK_LED_FCLR = USB_H_LINK_LED_MASK;
		  doBrowserUpdate(hBrowser);
		// turn off the "thinking" light
		  USB_H_LINK_LED_FSET = USB_H_LINK_LED_MASK;
		}

		
		//OS_Delay (50);
	}

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
		OS_EnterRegion();
		webc_BrowserProcessTimers(hBrowser);
		webc_BrowserDraw(hBrowser);
		webc_BrowserProcessLoadQueueAll(hBrowser);
		webc_BrowserDraw(hBrowser);
		OS_LeaveRegion();
  }
}

void doBrowserShutdown(HBROWSER_HANDLE hBrowser, HTMLBrowserConfig config)
{
  webc_DestroyFBBrowserConfig(config.graphics);
  webc_Exit();
}


/*********************************************************************
*
*       main
*
*********************************************************************/

int main(void) {  
  OS_IncDI();                      /* Initially disable interrupts  */
  OS_InitKern();                   /* initialize OS                 */
  OS_InitHW();                     /* initialize Hardware for OS    */
  BSP_Init();                      /* initialize LCD ports          */
  
  //OS_CREATETASK(&TCBTick, "Tick Task", TickTask,  51, StackTick);
  
  OS_CREATETASK(&TCBScreenUpdate, "Browser Task", BrowserTask,  51, StackBrowser);
  
  OS_CREATETASK(&TCBTouchGet, "Get touch Task", TouchGetTask,  50, StackTouchGet);
    
  OS_CREATEMB(&MailXY, sizeof(MailXYType_t), 6, &MailXYBuffer);
    
  OS_Start();                      /* Start multitasking            */
  
  return 0;
}


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
