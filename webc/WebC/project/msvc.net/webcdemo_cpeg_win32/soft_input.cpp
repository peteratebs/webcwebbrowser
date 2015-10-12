
#include <windows.h>
#include <tchar.h>
#include "resource.h"
#include "htmlsdk.h"
#include "webcstr.h"
#include "webcfg.h"


static HINSTANCE g_hInst=0;
HBROWSER_HANDLE g_hBrowser=0;

void DoHome( HBROWSER_HANDLE hBrowser);


INT_PTR CALLBACK SoftInputMessageHandler(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		{
		// forward
		HWND hButton = GetDlgItem(hDlg,IDC_FORWARD_BUTTON);
		HICON hIcon = ::LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_FORWARD_ICON));
		LRESULT lResult = SendMessage(hButton, (UINT) BM_SETIMAGE, (WPARAM) IMAGE_ICON, (LPARAM) hIcon );  

		// back
		hButton = GetDlgItem(hDlg,IDC_BACK_BUTTON);
		hIcon = ::LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_BACK_ICON));
		lResult = SendMessage(hButton, (UINT) BM_SETIMAGE, (WPARAM) IMAGE_ICON, (LPARAM) hIcon );  

		// select
		hButton = GetDlgItem(hDlg,IDC_SELECT_BUTTON);
		hIcon = ::LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_SELECT_ICON));
		lResult = SendMessage(hButton, (UINT) BM_SETIMAGE, (WPARAM) IMAGE_ICON, (LPARAM) hIcon );  

		// up
		hButton = GetDlgItem(hDlg,IDC_UP_BUTTON);
		hIcon = ::LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_UP_ICON));
		lResult = SendMessage(hButton, (UINT) BM_SETIMAGE, (WPARAM) IMAGE_ICON, (LPARAM) hIcon );  

		// down
		hButton = GetDlgItem(hDlg,IDC_DOWN_BUTTON);
		hIcon = ::LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_DOWN_ICON));
		lResult = SendMessage(hButton, (UINT) BM_SETIMAGE, (WPARAM) IMAGE_ICON, (LPARAM) hIcon );  

		// home
		hButton = GetDlgItem(hDlg,IDC_HOME_BUTTON);
		hIcon = ::LoadIcon(g_hInst, MAKEINTRESOURCE(IDI_HOME_ICON));
		lResult = SendMessage(hButton, (UINT) BM_SETIMAGE, (WPARAM) IMAGE_ICON, (LPARAM) hIcon );  

//		HBITMAP hBitmap = ::LoadBitmap(g_hInst, MAKEINTRESOURCE(IDB_BITMAP1));
//		LRESULT lResult = SendMessage(hButton, (UINT) BM_SETIMAGE, (WPARAM) IMAGE_BITMAP, (LPARAM) hBitmap );  

		return (INT_PTR)TRUE;
		}
		break;
	case WM_COMMAND:
		//if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			int which = LOWORD(wParam);
			HTMLEvent e;

			e.type = HTML_EVENT_NONE;
			e.data.key = WGK_F1;
			e.flags = 0;

			switch(which)
			{

			case IDC_FORWARD_BUTTON:
				//e.type = HTML_EVENT_KEYDOWN;
				//e.data.key = WGK_F1;
				//e.flags = 0;
				webc_BrowserForward(g_hBrowser);
				break;
			case IDC_BACK_BUTTON:
				//e.type = HTML_EVENT_KEYDOWN;
				//e.data.key = WGK_F1;
				//e.flags = 0;
				webc_BrowserBack(g_hBrowser);
				break;
			case IDC_SELECT_BUTTON:
				e.type = HTML_EVENT_KEYDOWN;
				e.data.key = WGK_CR;
				e.flags = 0;
				break;
			case IDC_UP_BUTTON:
				e.type = HTML_EVENT_KEYDOWN;
				e.data.key = WGK_LNUP;
				e.flags = HTML_EVENT_FLAG_CTRL_DOWN;
				break;
			case IDC_DOWN_BUTTON:
				e.type = HTML_EVENT_KEYDOWN;
				e.data.key = WGK_LNDN;
				e.flags = HTML_EVENT_FLAG_CTRL_DOWN;
				break;
			case IDC_HOME_BUTTON:
				//e.type = HTML_EVENT_KEYDOWN;
				//e.data.key = WGK_HOME;
				//e.flags = 0;
				DoHome(g_hBrowser);
				break;
			}

			if(e.type != HTML_EVENT_NONE)
			{
				HTMLEventStatus stat = webc_BrowserDispatchEvent (g_hBrowser, &e);
			}

			//EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}


int OpenSoftInput(HINSTANCE hInst, HWND hWnd, HBROWSER_HANDLE hBrowser)
{
	g_hInst = hInst;
	g_hBrowser = hBrowser;

	HWND hDialog = CreateDialog(hInst, MAKEINTRESOURCE(IDD_NAVIGATION_DIALOG), hWnd, SoftInputMessageHandler);

	RECT parentRect, dialogRect;
	GetWindowRect(hWnd, &parentRect);
	GetWindowRect(hDialog, &dialogRect);

	::MoveWindow(hDialog, dialogRect.left, parentRect.bottom, dialogRect.right - dialogRect.left, dialogRect.bottom - dialogRect.top, TRUE);
	ShowWindow(hDialog, SW_SHOW);

	DoHome(g_hBrowser);
	return 0;
}


int CloseSoftInput()
{
	return 0;
}

#ifdef SMALL_VIEWER
const char *g_testSuffix="helpViewer/index2.htm";
#else
const char *g_testSuffix="helpViewer/index.htm";
#endif
const char *g_testPrefix="file://";

char *GetSamplePath()
{
	LPSTR pFullPath= new CHAR[4096];
	char *retVal= (char *)0;
	BOOL bExists=FALSE;

	if(GetModuleFileNameA(NULL, pFullPath, 4096))
	{
		int len = _tcslen(pFullPath);
		int cur=len;
		while(cur >= 0)
		{
			if(pFullPath[cur] == _T('\\'))
			{
				pFullPath[cur+1] = _T('\0'); // trunc
				break;
			}
			cur--;
		}

		// now change all the backslashes to slashes
		cur=len;
		while(cur >= 0)
		{
			if(pFullPath[cur] == _T('\\'))
			{
				pFullPath[cur] = _T('/');
			}
			cur--;
		}

		retVal = new char[len + 1 + strlen(g_testPrefix) + strlen(g_testSuffix)];
		strcpy(retVal, g_testPrefix);
		strcat(retVal, pFullPath);
		strcat(retVal, g_testSuffix);
	
		// test to see that the file exists
		HANDLE hTestExists = ::CreateFileA(retVal + strlen(g_testPrefix),GENERIC_READ, FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_EXISTING, 0, NULL);
		if(INVALID_HANDLE_VALUE != hTestExists)
		{
			::CloseHandle(hTestExists);
			bExists = TRUE;
		}

	}
	
	delete[] pFullPath;

	return bExists ? retVal : 0;
}

bool bRegistered=false;

HTMLEventStatus smallViewerLoadHandler (
        HBROWSER_HANDLE hbrowser,
        HDOC_HANDLE hdoc,
        HELEMENT_HANDLE helem,
        HTMLEvent* event,
        char* param
    )
{
	if (event->type == HTML_EVENT_LOAD)
	{
		DoHome( hbrowser);
	}

	return (HTML_EVENT_STATUS_CONTINUE);
}

void DoHome( HBROWSER_HANDLE hBrowser)
{

	HDOC_HANDLE hDoc = webc_BrowserGetDocument(hBrowser);
	
	if(hDoc)
	{
		HELEMENT_HANDLE hFrame = webc_DocFindElement (hDoc, WEBC_NULL, WEBC_NULL, HTML_FRAME_ELEMENT, 0);
		if(hFrame)
		{
			HDOC_HANDLE hFrameDoc = webc_ElementGetFrameDocument(hFrame);
			if(hFrameDoc)
			{
				HELEMENT_HANDLE hFirstAnchor = webc_DocFindElement (hFrameDoc, WEBC_NULL, WEBC_NULL, HTML_ANCHOR_ELEMENT, 0);	
				if(hFirstAnchor)
				{
					webc_ElementFocus (hFirstAnchor,WEBC_TRUE);
				}
			}
		}
		else // we're doing the small scale demo
		{
			HELEMENT_HANDLE hFirstAnchor = webc_DocFindElement (hDoc, WEBC_NULL, WEBC_NULL, HTML_ANCHOR_ELEMENT, 0);	
			if(hFirstAnchor)
			{
				webc_ElementFocus (hFirstAnchor,WEBC_TRUE);

				if(!bRegistered)
				{
					webc_BrowserSetTagEventHandler (hBrowser, HTML_TAG_BODY, smallViewerLoadHandler);
					bRegistered = true;
				}
			}

		}
	}
}