//
// PWEBWIN.CPP - Methods for class DisplayElement
//
// EBS - WebC
//
// Copyright EBS Inc. , 2006-2009
// All rights reserved.
// This code may not be redistributed in source or linkable object form
// without the consent of its author.
//
// Module description:
//

/*****************************************************************************/
// Header Files
/*****************************************************************************/

#include "pwebwin.hpp"
#include "webcpeg.h"

/*****************************************************************************/
// Defines/Macros
/****************************************************************************/
// define to see an example of custom scroll bar drawing
//   (WARNING: example is extremely ugly)
#define SPAWN_EXAMPLE 0
#define SPAWN_CHROME_BROWSER 0
#define SPAWN_TEST_SUITE 1

HBROWSER_HANDLE webc_start_webc_tests(HTMLBrowserConfig *pconfig, WEBC_BOOL CreateNewWindow, char * pTestsuiteURL);

/*****************************************************************************/
// Local Types
/*****************************************************************************/


/*****************************************************************************/
// Global Data
/*****************************************************************************/
static PEGCHAR title[] = {'E','B','S',' ','W','e','b','C',' ','E',
                          'm','b','e','d','d','e','d',' ','W','e','b',' ','B',
						  'r','o','w','s','e','r',' ','-',' ','2','.','5',
						  ' ','D','e','m','o','\0'};

/*****************************************************************************/
// Local functions
/*****************************************************************************/

HBROWSER_HANDLE webc_start_chrome_browser(HTMLBrowserConfig *pconfig, WEBC_BOOL CreateNewWindow);
HBROWSER_HANDLE webc_start_webc_demo(HTMLBrowserConfig *pconfig);

PegWebWindow::PegWebWindow (PegRect &Rect) : PegDecoratedWindow(Rect), mBrowser(0)
{
	MessageQueue()->SetTimer(this, 0xabcd, 1, 1);
}


PegWebWindow::PegWebWindow (
		PegRect &rect,
		void* privateData, 
		char *pURL
	)
	: PegDecoratedWindow(rect, FF_THICK) //FF_NONE)
{
	/*
	PegTitle* pTitleBar = new PegTitle(title, TT_COPY|TF_CLOSEBUTTON|TF_MINMAXBUTTON);
	if (pTitleBar)
	{
		this->Add(pTitleBar, FALSE);
	}
	*/

	Add(new PegTitle((PEGCHAR *) L"PegWebWindow"));
    SetColor(PCI_NORMAL, BLACK);

	HTMLBrowserConfig config;
	config.WindowHandle = (void *) this;
	webc_InitPegBrowserConfig(&config, this);
	config.privateData = privateData;
	
#if SPAWN_EXAMPLE
	mBrowser = webc_start_webc_demo(&config);
#elif SPAWN_CHROME_BROWSER
	mBrowser = webc_start_chrome_browser(&config, false);
#elif SPAWN_TEST_SUITE
	mBrowser = webc_start_webc_tests(&config, WEBC_FALSE, pURL);
#endif

	MessageQueue()->SetTimer(this, 0xabcd, 1, 1);
}

PegWebWindow::~PegWebWindow()
{
	HTMLBrowserConfig config;

	MessageQueue()->KillTimer(this, 0xabcd);

	webc_BrowserGetConfig(mBrowser, &config);
	webc_DestroyBrowser(mBrowser, 0);
	webc_DestroyPegBrowserConfig(&config);
}

PEGINT PegWebWindow::Message(const PegMessage &Mesg)
{
	//ObjectReference<PegWebWindow> self(this);

	// translate PegMessage to HTMLEvent and pass to our Widget

	HTMLEvent e;

	if(!mBrowser)
		return PegDecoratedWindow::Message(Mesg);

	e.type = HTML_EVENT_NONE;
	e.flags = 0;

	switch (Mesg.wType)
	{
		case PM_KEY:
		case PM_KEY_RELEASE:
		case PM_NONCURRENT:
		case PM_CURRENT:
			break;

		default:
			e.data.position.x = Mesg.Point.x - mClient.iLeft;
			e.data.position.y = Mesg.Point.y - mClient.iTop;
			break;
	};

	switch(Mesg.wType)
	{
		case PM_TIMER:
			webc_BrowserSuspendDraw(mBrowser);
			webc_BrowserProcessTimers(mBrowser);
			webc_BrowserResumeDraw(mBrowser);
			webc_BrowserDraw(mBrowser);

			webc_BrowserProcessLoadQueueAll(mBrowser);
			webc_BrowserDraw(mBrowser);

			return (0);

		case PM_SHOW:
			return (PegWindow::Message(Mesg));

		case PM_PARENTSIZED:
			Resize(Parent()->mClient);
			return (0);

		case PM_CURRENT:
			e.type = HTML_EVENT_FOCUS;
			break;

		case PM_LBUTTONDOWN:
			e.type = HTML_EVENT_MOUSEDOWN;
			break;

		case PM_LBUTTONUP:
			e.type = HTML_EVENT_MOUSEUP;
			break;

		case PM_RBUTTONDOWN:
			e.type = HTML_EVENT_MOUSEDOWN;
			break;

		case PM_RBUTTONUP:
			e.type = HTML_EVENT_MOUSEUP;
			break;

		case PM_POINTER_ENTER:
			e.type = HTML_EVENT_MOUSEOVER;
			break;

		case PM_POINTER_EXIT:
			e.type = HTML_EVENT_MOUSEOUT;
			break;

		case PM_POINTER_MOVE:
			e.type = HTML_EVENT_MOUSEMOVE;
			break;

		case PM_NONCURRENT:
			break;

#ifdef PEG_KEYBOARD_SUPPORT

		case PM_KEY:
			e.type = HTML_EVENT_KEYDOWN;
			e.data.key = webc_TranslatePegKey(Mesg.iData);
			break;

		case PM_KEY_RELEASE:
			e.type = HTML_EVENT_KEYUP;
			e.data.key = webc_TranslatePegKey(Mesg.iData);
			break;

#endif // PEG_KEYBOARD_SUPPORT

		default:
			break;
	}//end switch

	switch (Mesg.wType)
	{
		case PM_KEY:
		case PM_KEY_RELEASE:
			if (Mesg.lData & KF_SHIFT)
			{
				e.flags |= HTML_EVENT_FLAG_SHIFT_DOWN;
			}

			if (Mesg.lData & KF_CTRL)
			{
				e.flags |= HTML_EVENT_FLAG_CTRL_DOWN;
			}

			if (Mesg.lData & KF_ALT)
			{
				e.flags |= HTML_EVENT_FLAG_ALT_DOWN;
			}
			break;

		case PM_LBUTTONDOWN:
		case PM_LBUTTONUP:
		case PM_RBUTTONDOWN:
		case PM_RBUTTONUP:
			if (Mesg.iData & KF_SHIFT)
			{
				e.flags |= HTML_EVENT_FLAG_SHIFT_DOWN;
			}

			if (Mesg.iData & KF_CTRL)
			{
				e.flags |= HTML_EVENT_FLAG_CTRL_DOWN;
			}
			break;

		default:
			break;
	};

	if (mBrowser && (e.type != HTML_EVENT_NONE))
	{
		webc_BrowserSuspendDraw(mBrowser);
		webc_BrowserDispatchEvent(mBrowser, &e);
		webc_BrowserResumeDraw(mBrowser);
		webc_BrowserDraw(mBrowser);

		webc_BrowserProcessLoadQueueAll(mBrowser);
		webc_BrowserDraw(mBrowser);
	}
	//else
	{
		return PegDecoratedWindow::Message(Mesg);
	}

	return (0);
}

void PegWebWindow::Draw(void)
{
	BeginDraw();
	PegWindow::Draw();

	if (mBrowser)
	{
		webc_BrowserInvalidate(mBrowser);
		webc_BrowserDraw(mBrowser);
	}

	EndDraw();
}
