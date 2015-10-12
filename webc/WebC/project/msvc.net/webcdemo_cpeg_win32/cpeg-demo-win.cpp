extern "C" {
#include "peg.h"
}

#include "webc.h"
#include "webcc-peg.h"
//#include "webc_ctrl_demo.h"


#ifdef SMALL_VIEWER
int giPegScreenWidth = 480;
int giPegScreenHeight = 450;
#else
int giPegScreenWidth = 800;
int giPegScreenHeight = 570;
#endif

#define SPAWN_EXAMPLE 0
#define SPAWN_CHROME_BROWSER 0
#define SPAWN_TEST_SUITE 1

HBROWSER_HANDLE webc_start_webc_demo(HTMLBrowserConfig* cfg, WEBC_BOOL bCreateNewWindow);
HBROWSER_HANDLE webc_start_webc_tests(HTMLBrowserConfig* cfg, WEBC_BOOL bCreateNewWindow, char * pTestsuiteURL);
HBROWSER_HANDLE webc_start_chrome_browser(HTMLBrowserConfig *pconfig, WEBC_BOOL bCreateNewWindow);


extern "C" {


HBROWSER_HANDLE	gbrowser=0;

PEGINT WebCPanelNotify(void *pThing, const PegMessage *pMesg);
void WebCPanelDestroy(void *pThing);


// C-PEG's main entrypoint
void PegAppInitialize(PegPresentation *pPresent)
{
    PegRect r;
    PegPanel *pp;
	HTMLBrowserConfig config[3];
	RECT topRect;

    //PegRectSet(&r,  0, 0, PegXResGet() - 1, PegYResGet() - 1);
    PegRectSet(&r,  0, 0, giPegScreenWidth, giPegScreenHeight);
	


    pp = PegPanelCreate(&r, 1, FF_THICK);
    PegFuncPtrSet(pp, PFP_DESTROY, WebCPanelDestroy);
    PegFuncPtrSet(pp, PFP_NOTIFY, WebCPanelNotify);

	webc_Init();

	rtp_memset(&config[0], 0, sizeof(HTMLBrowserConfig));
	rtp_memset(&config[1], 0, sizeof(HTMLBrowserConfig));
	rtp_memset(&config[2], 0, sizeof(HTMLBrowserConfig));

#if (SPAWN_CHROME_BROWSER)
	webc_InitCPegBrowserConfig (&config[0], (PegThing *)pp);
    gbrowser = webc_start_chrome_browser(&config[0], WEBC_TRUE);
//	cpeg_execute_webc(config[0].graphics, gbrowser);
#endif
	
#if (SPAWN_TEST_SUITE)
	webc_InitCPegBrowserConfig (&config[1], (PegThing *)pp);
    gbrowser = webc_start_webc_tests(&config[1], WEBC_TRUE, NULL);
//	cpeg_execute_webc(config[1].graphics, gbrowser);
#endif
	
#if (SPAWN_EXAMPLE)
	webc_InitCPegBrowserConfig (&config[2], (PegThing *)pp);
    gbrowser = webc_start_webc_demo(&config[2], WEBC_TRUE);
//	cpeg_execute_webc(config[2].graphics, gbrowser);
#endif

	// add the panel to the presentation
	PegAdd(pPresent, pp, TRUE);

	// resize the owner Win32 window 
	HWND hTopWindow = ::FindWindow("C/PEG Application", "C/PEG Development Window");

	if(hTopWindow)
	{
		::GetWindowRect(hTopWindow, &topRect);
		::MoveWindow(hTopWindow, topRect.left, topRect.top, giPegScreenWidth+23, giPegScreenHeight+43, TRUE ); 
	}
}


PEGINT WebCPanelNotify(void *pThing, const PegMessage *pMesg)
{
	HTMLEvent e;
	e.type = HTML_EVENT_NONE;
	e.flags = 0;
	WEBC_BOOL bDraw = WEBC_TRUE;
	WEBC_BOOL bNotify = WEBC_FALSE;
	PEGINT retVal = 0;

	PEGUSHORT msgNum = PegMessageTypeGet(pMesg);

    switch(msgNum)
    {
	case PM_PARENTSIZED:
	case PM_SHOW:
		PegTimerStart(pThing, msgNum, 5, 0); // one -shot
		bNotify = WEBC_TRUE;
		break;
	case PM_TIMER:
		webc_BrowserInvalidate(gbrowser);
		e.type = HTML_EVENT_FOCUS;
		break;

	case PM_LBUTTONDOWN:            /*  Fall through  */
	case PM_LBUTTONUP:            /*  Fall through  */
    {
		e.data.position.x = pMesg->u.Point.x;
		e.data.position.y = pMesg->u.Point.y;
        if (msgNum == PM_LBUTTONDOWN)
            e.type = HTML_EVENT_MOUSEDOWN;
        else
            e.type = HTML_EVENT_MOUSEUP;
	    break;
	}

	case PM_POINTER_MOVE:
	{
        e.data.position.x = pMesg->u.Point.x;
		e.data.position.y = pMesg->u.Point.y;
        e.type = HTML_EVENT_MOUSEMOVE;
        bDraw = WEBC_FALSE;
	    break;
	}

	case PM_KEY:
    case PM_KEY_RELEASE:
    {
	    if (msgNum == PM_KEY)
			e.type = HTML_EVENT_KEYDOWN;
        else
			e.type = HTML_EVENT_KEYUP;

		e.data.key = pMesg->sData;
	    break;
	}

    default:
		bNotify = WEBC_TRUE;
		break;
    }

	if(bNotify)
		retVal = PegPanelNotify(pThing, pMesg);

	if (gbrowser && (e.type != HTML_EVENT_NONE))
	{
		webc_BrowserSuspendDraw(gbrowser);
		webc_BrowserDispatchEvent(gbrowser, &e);
		webc_BrowserResumeDraw(gbrowser);
		if(bDraw)
			webc_BrowserDraw(gbrowser);
		webc_BrowserProcessLoadQueueAll(gbrowser);
		if(bDraw)
			webc_BrowserDraw(gbrowser);
	}

	return(retVal);
}

void WebCPanelDestroy(void *pThing)
{
	// Add shutdown code here

    PegPanelDestroy(pThing);
}


}



