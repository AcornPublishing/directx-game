// AnimateGrad.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "AnimateGrad.h"
#include "..\..\common\ddfunc.h"
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// DirectDraw Variables:
IDirectDraw7 *ppiDD;
IDirectDrawSurface7 *primsurf;
IDirectDrawSurface7 *backsurf=NULL;

BOOL bWindowed;
int iWidth, iHeight;
HWND hWnd;

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
	LoadString(hInstance, IDC_ANIMATEGRAD, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_ANIMATEGRAD);

	// Main message loop:
	DDPIXELFORMAT ftPixel;
	ZeroMemory(&ftPixel, sizeof(ftPixel));
	ftPixel.dwSize=sizeof(ftPixel);

	backsurf->GetPixelFormat(&ftPixel);
	
	int c=0;
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

		if (!ppiDD) break;

		// 서피스 잠금을 위한 구조체 설정
        DDSURFACEDESC2 desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.dwSize = sizeof(desc);

		
		if (backsurf->Lock(0, &desc, DDLOCK_WAIT, 0)==DD_OK)
		{
		   BYTE iBlue=0;
		   BYTE iGreen=0;
		   BYTE iRed=255;

		   // 16 비트 색심도 색상으로 채우는 코드
		   if (ftPixel.dwRGBBitCount==16)
		   {
			   WORD* dst = (WORD *)desc.lpSurface;
               
			   for (int i=0; i<iHeight; i++)
			   {
				   for (int j=0; j<desc.lPitch; j++)
					*(WORD *)(dst+j) = (WORD)((iRed+c)/8 | ((iGreen+i)/4 << 5) | 
				           ((iBlue+j)/8 << 11));
				   dst=dst+desc.lPitch;
			   }
		   }
		   
		   // 24 비트 색심도 색상으로 채우는 코드
		   if (ftPixel.dwRGBBitCount==24)
		   {
			   BYTE* dst = (BYTE *)desc.lpSurface;
               
			   for (int i=0; i<iHeight; i++)
			   {
				   for (int j=0; j<desc.lPitch; j++)
				   {
					   *(BYTE *)(dst+j) = iRed+j;
					   j++;
					   *(BYTE *)(dst+j) = iGreen+i;
					   j++;
					   *(BYTE *)(dst+j) = iBlue+c;
				   }
				   dst=dst+desc.lPitch;
			   }
		   }
		   c++;
           backsurf->Unlock(0);
		}

		if (bWindowed)
		{
			RECT clientRect;
			POINT p;
			p.x=p.y=0;
			ClientToScreen(hWnd, &p);
			GetClientRect(hWnd, &clientRect);
			OffsetRect(&clientRect, p.x, p.y);

			primsurf->Blt(&clientRect, backsurf, 0, DDBLT_WAIT, 0);
		}
		else
			primsurf->Flip(NULL, DDFLIP_WAIT);
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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_ANIMATEGRAD);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_ANIMATEGRAD;
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
   DXDDInit(&ppiDD, &primsurf, &backsurf, hWnd, iWidth, iHeight, 24, &bWindowed);

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
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		 if (backsurf) {backsurf->Release();backsurf=0;}
		 if (primsurf) {primsurf->Release();primsurf=0;}
		 if (ppiDD) {ppiDD->Release();ppiDD=0;}
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
