// Zoom.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Zoom.h"
#include <math.h>
#include "..\..\common\ddfunc.h"
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

HWND hWnd;

// DirectDraw Variables:
IDirectDraw7 *ppiDD;
IDirectDrawSurface7 *primsurf;
IDirectDrawSurface7 *backsurf=NULL;
IDirectDrawSurface7 *imagesurf=NULL;
IDirectDrawSurface7 *zoomsurf=NULL;
BOOL bWindowed;

int iWidth, iHeight;
int xPos=200, yPos=200;
int xInc=2, yInc=1;
const int rad=100;
const int circ=50;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_ZOOM, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_ZOOM);

	// Main message loop:
	while (true) 
	{
		if (PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE)) 
		{
			if (!GetMessage(&msg, NULL, NULL, NULL)) break;
			if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
			{
				TranslateMessage(&msg);
				DispatchMessage(&msg);
			}
		}

		if (GetActiveWindow()!=hWnd)
			continue;

		backsurf->BltFast(0, 0, imagesurf, 0, 0);

		DDSURFACEDESC2 desc;
		ZeroMemory(&desc, sizeof(desc));  
		desc.dwSize = sizeof(desc);  

		if (imagesurf->Lock(0, &desc, DDLOCK_WAIT, 0)==DD_OK)
		{
			BYTE* src = (BYTE *)desc.lpSurface;
			int lPitch=desc.lPitch;

			ZeroMemory(&desc, sizeof(desc));  
			desc.dwSize = sizeof(desc);  
            
			if (backsurf->Lock(0, &desc, DDLOCK_WAIT, 0)==DD_OK)
			{
				BYTE* dst = (BYTE *)desc.lpSurface;
				int s=(rad*rad-circ*circ);
				
				for (int y=-rad; y<=rad; y++)
				{
					int yy=y*y;
					for (int x=-rad; x<=rad; x++)
					{
						if (x*x+yy<s)
						{
							double mz = circ / sqrt(rad*rad-double(x*x+yy));
							*(WORD *)(dst+(y+yPos)*desc.lPitch+(xPos+x)*2)=
								*(WORD *)(src+(WORD)(mz*y+yPos)*lPitch+
								            (WORD)(mz*x+xPos)*2);
						}
					}
				}
				backsurf->Unlock(0);
			}
			imagesurf->Unlock(0);
		}

		xPos=xPos+xInc;
		yPos=yPos+yInc;
		if (xPos>iWidth-100)
			xInc=-2; 
		if (yPos>iHeight-100)
			yInc=-1;
		if (xPos<100)
			xInc=2; 
		if (yPos<100)
			yInc=1;

		POINT p;
		p.x=p.y=0;
		ClientToScreen(hWnd, &p);
		primsurf->BltFast(p.x, p.y, backsurf, 0, 0);
	}

	return (int) msg.wParam;
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_ZOOM);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= 0;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   bWindowed=FALSE;
   iWidth=800;
   iHeight=600;
   DXDDInit(&ppiDD, &primsurf, &backsurf, hWnd, iWidth, iHeight, 16, &bWindowed);
   imagesurf = LoadBMPToSurfaceFast(&imagesurf, "desktop.bmp", ppiDD);

   DDSURFACEDESC2 desc;
   ZeroMemory(&desc, sizeof(desc));
   desc.dwSize = sizeof(desc);
   desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;     
   desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;     
   desc.dwWidth = 200;     
   desc.dwHeight = 200;

   ppiDD->CreateSurface(&desc, &zoomsurf, 0);

   DDCOLORKEY cData;
   cData.dwColorSpaceLowValue = 0;
   cData.dwColorSpaceHighValue = 0;
   zoomsurf->SetColorKey(DDCKEY_SRCBLT, &cData);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
