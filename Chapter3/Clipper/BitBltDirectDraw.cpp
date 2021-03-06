// BitBltDirectDraw.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "BitBltDirectDraw.h"
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
IDirectDrawSurface7 *plainsurf=NULL;
BOOL bWindowed;
int iWidth, iHeight;
int iPlainLeft=-180;
double iPlainState=0;
double iStateOffset=1;

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
	LoadString(hInstance, IDC_BITBLTDIRECTDRAW, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_BITBLTDIRECTDRAW);

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

		if (imagesurf!=NULL)
		{
			iPlainLeft++;
			if ((iPlainLeft+180)>800)
				iPlainLeft=iPlainLeft;

			// 복사할 목표 영역
			RECT dst;
			dst.left=iPlainLeft>0 ? iPlainLeft : 0;
			dst.top=10;
			dst.right=(iPlainLeft+180)<800 ? iPlainLeft+180 : 800;
			dst.bottom=100;

			RECT src;
			src.left=iPlainLeft>0 ? 0 : abs(iPlainLeft);
			src.top=WORD(iPlainState)*90;
			src.right=(iPlainLeft+180)<800 ? 180 : abs(iPlainLeft-800);
			src.bottom=(WORD(iPlainState)+1)*90;

			iPlainState+=iStateOffset;
			if (iPlainState>7)
				iStateOffset=-0.02;
			if (iPlainState<0)
				iStateOffset=0.02;
			if (iPlainLeft>980)
				iPlainLeft=-180;

			// 서피스 복사
			backsurf->BltFast(0, 0, imagesurf, 0, 0);

            DDBLTFX bltfx;
			ZeroMemory(&bltfx, sizeof(bltfx));
			bltfx.dwSize=sizeof(bltfx);

			backsurf->Blt(&dst, plainsurf, &src, DDBLT_KEYSRC | DDBLT_WAIT, &bltfx);
		}

		if (bWindowed)
		{
			RECT clientRect;
			POINT p;
			p.x=p.y=0;
			ClientToScreen(hWnd, &p);
			primsurf->BltFast(p.x, p.y, backsurf, 0, 0);
		}
		else
			primsurf->BltFast(0, 0, backsurf, 0, 0);
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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_BITBLTDIRECTDRAW);
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
   plainsurf = LoadBMPToSurfaceFast(&plainsurf, "plain.bmp", ppiDD);

   DDCOLORKEY  cData;
   DDSURFACEDESC2 desc;
   ZeroMemory(&desc, sizeof(desc));
   desc.dwSize = sizeof(desc);

   HRESULT hRes=plainsurf->Lock( 0, &desc, DDLOCK_WAIT | DDLOCK_WRITEONLY, 0 );
   if (hRes!=DD_OK)
	   return FALSE;

   WORD* btSurf = (WORD*)desc.lpSurface;

   cData.dwColorSpaceLowValue = *btSurf;
   cData.dwColorSpaceHighValue = *btSurf;

   plainsurf->Unlock(0);

   plainsurf->SetColorKey(DDCKEY_SRCBLT, &cData);

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
