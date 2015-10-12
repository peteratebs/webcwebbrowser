/*********************************************************************
*                SEGGER MICROCONTROLLER SYSTEME GmbH                 *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*         (C) 1996-2004 SEGGER Microcontroller Systeme GmbH          *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************
----------------------------------------------------------------------
File        : WinMain.c
Purpose     : Start of the windows application.
              Registers & creates main window, contains message loop
---------------------------END-OF-HEADER------------------------------
*/

#include <windows.h>
#include <stdio.h>

#include "Resource.h"
#include "SIM.h"

/*********************************************************************
*
*       Types
*
**********************************************************************
*/

#define U32 unsigned int

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/

static HINSTANCE  _hInst;
static HBITMAP    _hBmpDevice;
static HMENU      _hMenuPopup;
static int        _rgbTransparent = 0xFF0000;
static const char _sWindowClass[] = {"MainWindow"};

/*********************************************************************
*
*       Forward declarations for callbacks
*
**********************************************************************
*/

void main(void);

/*********************************************************************
*
*       static code, callback about
*
**********************************************************************
*/
/*********************************************************************
*
*       _cbAbout
*/
static LRESULT CALLBACK _cbAbout(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam) {
  switch (message) {
  case WM_INITDIALOG:
    {
      char acBuffer[256];
      strcpy(acBuffer, "Compiled " __TIME__ " on " __DATE__);
      SetWindowText(GetDlgItem(hDlg, ID_DATE), acBuffer);
    }
    return TRUE;
  case WM_COMMAND:
    if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) {
      EndDialog(hDlg, LOWORD(wParam));
      return TRUE;
    }
    break;
  }
  return FALSE;
}

/*********************************************************************
*
*       static code, helper functions
*
**********************************************************************
*/
/*********************************************************************
*
*       _GetWidthOfBitmap
*/
static int _GetWidthOfBitmap(HBITMAP hBmp) {
  BITMAP bmp;
  GetObject(hBmp, sizeof(bmp), &bmp);
  return bmp.bmWidth;
}

/*********************************************************************
*
*       _GetYSizeOfBitmap
*/
static int _GetYSizeOfBitmap(HBITMAP hBmp) {
  BITMAP bmp;
  GetObject(hBmp, sizeof(bmp), &bmp);
  return bmp.bmHeight;
}

/*********************************************************************
*
*       _GetScanLine
*/
static void _GetScanLine(HBITMAP hBmp, int y, DWORD* pRGB) {
  if (hBmp) {
    HDC hdc;
    BITMAPV4HEADER BmpInfo = {0};
    BmpInfo.bV4Size = sizeof(BmpInfo);
    hdc = GetWindowDC(NULL);
    /* Fill in the Bitmap info structure */
    GetDIBits(hdc, hBmp, 0, 0, NULL, (BITMAPINFO*)&BmpInfo, DIB_RGB_COLORS);
    BmpInfo.bV4V4Compression = BI_RGB;
    BmpInfo.bV4BitCount = 32;
    if (BmpInfo.bV4Height > 0) {
      y = BmpInfo.bV4Height - y - 1;
    }
    GetDIBits(hdc, hBmp, y, 1, pRGB, (BITMAPINFO*)&BmpInfo, DIB_RGB_COLORS);
  }
}

/*********************************************************************
*
*       _ComparePixels
*/
static int _ComparePixels(U32 Color0, U32 Color1) {
  return ((Color0 & 0xffffff) ^ (Color1 & 0xffffff));
}

/*********************************************************************
*
*       _SetBitmapRegion
*
* Purpose:
*   This is a key function for the display of the bitmap. It assigns
*   a clipping region to the window identical to the outline of the
*   bitmap.
*/
static void _SetBitmapRegion(HWND hWnd, HBITMAP hBmp, DWORD rgbTrans, char IsHardkey) {
  if (hBmp) {
    HRGN hRgn = 0;
    HRGN hRgn0;
    POINT Point = {0};
    DWORD acBits[4096];
    int y;
    int yOff = 0;
    int xOff = 0;
    int XSize = _GetWidthOfBitmap(hBmp);
    int YSize = _GetYSizeOfBitmap(hBmp);
    RECT WinRect;
    if (IsHardkey) {
      HWND hWndParent = GetParent(hWnd);
      ClientToScreen(hWndParent, &Point);
      GetWindowRect(hWnd, &WinRect);
      YSize = WinRect.bottom - WinRect.top;
      yOff = WinRect.top  - Point.y;
      xOff = WinRect.left - Point.x;
    }
    for (y = 0; y < YSize; y++) {
      int i, i0, i1;
      _GetScanLine(hBmp, y + yOff, &acBits[0]);
      for (i = 0; i < XSize;) {
        while (_ComparePixels(acBits[i], rgbTrans) == 0 && i < XSize) {
          i++;
        }
        i0 = i;
        while (_ComparePixels(acBits[i], rgbTrans) != 0 && i < XSize) {
          i++;
        }
        i1 = i;
        if (i0 <XSize) {
          hRgn0 = CreateRectRgn(i0 - xOff, y, i1 - xOff, y + 1);
          if (hRgn0) {
            if (hRgn) {
              CombineRgn(hRgn, hRgn, hRgn0, RGN_OR);
              DeleteObject(hRgn0);
            } else {
              hRgn = hRgn0;
            }
          }
        }
      }
    }
    SetWindowRgn(hWnd, hRgn,0);  // Note: Region is now owned by the system, do not delete
  }
}

/*********************************************************************
*
*       _OnCommand
*
*       --- GENERIC - No modification required for application ---
*
* Function description
*   Called when a command is sent to the main window.
*   Commands are menu commands or system commands.
*   We only handle menu commands here, since system commands (such as
*   Maximize) are handle in the default procedure
*/
static void _OnCommand(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
	int wmId = LOWORD(wParam);
  /* Standard Non-application specific menu items */
	switch (wmId) {
  case IDM_ABOUT: DialogBox(_hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)_cbAbout); break;
  case IDM_EXIT:
    DestroyWindow(hWnd);
    return;
  }
  DefWindowProc(hWnd, Msg, wParam, lParam);
}

/*********************************************************************
*
*       Window procedure for Device
*/
static LRESULT CALLBACK _WndProc(HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam) {
  static int CaptureOn = 0;
  static int xPosOld, yPosOld;
  int xPos = (signed short)LOWORD(lParam);  // horizontal position of cursor
  int yPos = (signed short)HIWORD(lParam);  // vertical position of cursor
  switch (Msg) {
  case WM_CREATE:
    _SetBitmapRegion(hWnd, _hBmpDevice, _rgbTransparent, 0);
    break;
  case WM_PAINT:
    {
      HDC hDC, hDCImage;
      PAINTSTRUCT ps;
      //
      // Preparations.
      //
      hDC = BeginPaint(hWnd, &ps);
      hDCImage = CreateCompatibleDC(hDC);
      SelectObject(hDCImage, _hBmpDevice);
      //
      // Give application a chance to draw
      //
      SIM_Paint(hDCImage);
      //
      // Copy generated image into display
      //
      BitBlt(hDC, 0, 0, 1000, 1000, hDCImage, 0, 0, SRCCOPY);
      //
      // Cleanup
      //
      DeleteDC(hDCImage);
      EndPaint(hWnd, &ps);
    }
    break;
  case WM_DESTROY:
    PostQuitMessage(0);
    break;
  case WM_COMMAND:
    _OnCommand(hWnd, Msg, wParam, lParam);
    break;
  case WM_RBUTTONDOWN:
    { 
      POINT Point;
      Point.x = xPos;
      Point.y = yPos;
      ClientToScreen(hWnd, &Point);
      TrackPopupMenu(_hMenuPopup, TPM_RIGHTBUTTON, Point.x, Point.y, 0, hWnd, NULL);
    }
    break;
  case WM_LBUTTONDOWN:
    SetCapture(hWnd);
    CaptureOn = 1;
    xPosOld = xPos;
    yPosOld = yPos;
    break;
  case WM_LBUTTONUP:
    ReleaseCapture();
    CaptureOn = 0;
    break;
  case WM_MOUSEMOVE:
    if (CaptureOn) {
      int xDiff = xPos - xPosOld;
      int yDiff = yPos - yPosOld;
      if (xDiff | yDiff) {
        RECT r;
        GetWindowRect(hWnd, &r);
        MoveWindow(hWnd, r.left + xDiff, r.top + yDiff, r.right - r.left, r.bottom - r.top, 1);
      }
    }
    break;
  default:
    return DefWindowProc(hWnd, Msg, wParam, lParam);
  }
  return 0;
}

/*********************************************************************
*
*       _RegisterClass
*
*       --- GENERIC - No modification required for application ---
*
* Function description
*   Register window class for main window
*/
static void _RegisterClass(void) {
  WNDCLASSEX wcex    = {0};
  wcex.cbSize        = sizeof(WNDCLASSEX); 
  wcex.style         = CS_HREDRAW | CS_VREDRAW;
  wcex.lpfnWndProc   = _WndProc;
  wcex.lpszClassName = _sWindowClass;
  wcex.hInstance     = _hInst;
  wcex.hIcon         = LoadImage(_hInst, (LPCTSTR)IDI_ICON, IMAGE_ICON, 0, 0, 0);
  wcex.hIconSm       = LoadImage(_hInst, (LPCTSTR)IDI_ICON, IMAGE_ICON, 16, 16, 0);
  wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
  wcex.lpszMenuName  = NULL;
  RegisterClassEx(&wcex);
}

/*********************************************************************
*
*       Thread
*/
static UINT Thread(LPVOID lpParameter) {
  main();
  return 0;
}

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       WinMain
*
*       --- GENERIC - No modification required for application ---
*
* Function description
*   Start of the windows program.
*   Does the following:
*   - Creates the main window
*   - Loads accelerator table
*   - Performs application specific initialization (--> _InitApp)
*   - Executes the message loop.
*/
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	MSG    Msg;
	HACCEL hAccelTable;
  HWND   hWndMain;
  BITMAP BmpDevice;
  DWORD  ThreadID;
  /* Init global data */
  _hInst = hInstance;
  /* Register main window class */
	_RegisterClass();
  /* Load bitmap */
  _hBmpDevice = (HBITMAP)LoadImage(_hInst, (LPCTSTR) IDB_DEVICE, IMAGE_BITMAP,  0, 0, 0);
  _hMenuPopup = LoadMenu(_hInst, (LPCSTR)IDC_CONTEXTMENU);
  _hMenuPopup = GetSubMenu(_hMenuPopup, 0);
  /* Create main window */
  GetObject(_hBmpDevice, sizeof(BmpDevice), &BmpDevice);
  hWndMain = CreateWindowEx(WS_EX_TOPMOST, _sWindowClass, "embOS Simulation", WS_SYSMENU | WS_CLIPCHILDREN | WS_POPUP | WS_VISIBLE,
                                  10, 20, BmpDevice.bmWidth, BmpDevice.bmHeight, NULL, NULL, _hInst, NULL);
  if (!hWndMain) {
    return 1;   /* Error */
  }
  /* Show main window */
  ShowWindow(hWndMain, nCmdShow);
  /* Load accelerator table */
	hAccelTable = LoadAccelerators(_hInst, (LPCTSTR)IDC_WINMAIN);
	/* application initialization: */
  CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Thread, NULL, 0, &ThreadID);
  /* main message loop */
  if (SIM_Init(hWndMain) == 0) {
    while (GetMessage(&Msg, NULL, 0, 0)) {
      if (!TranslateAccelerator(Msg.hwnd, hAccelTable, &Msg)) {
        TranslateMessage(&Msg);
        DispatchMessage(&Msg);
      }
    }
  }
	return 0;
}

/*************************** end of file ****************************/
