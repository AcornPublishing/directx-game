// DrawLine.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "DrawLine.h"
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
double off=0;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void DrawScene();

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
	LoadString(hInstance, IDC_DRAWLINE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_DRAWLINE);

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
		if (!primsurf)
			continue;
		DrawScene();
		off+=0.01;
		if (off>1)
			off=0;
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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_DRAWLINE);
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
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   bWindowed=TRUE;
   iWidth=800;
   iHeight=600;
   DXDDInit(&ppiDD, &primsurf, &backsurf, hWnd, iWidth, iHeight, 16, &bWindowed);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

void RestoreSurfaces()
{
	if (primsurf->IsLost())
		primsurf->Restore();

	if (backsurf->IsLost())
		backsurf->Restore();
}

void DrawScene()
{
    double x1, y1, x2, y2, x3, y3, x4, y4,
		tx1, ty1;

    double p=0.9;
	double q=0.1;
	x1=0;	x2=iWidth; x3=iWidth; x4=0;
	y1=0; y2=0; y3=iHeight; y4=iHeight;
	
	ClearSurface(backsurf, 0);

		tx1=off*x1+(1-off)*x2;
		ty1=off*y1+(1-off)*y2;
		x2=off*x2+(1-off)*x3;
		y2=off*y2+(1-off)*y3;
		x3=off*x3+(1-off)*x4;
		y3=off*y3+(1-off)*y4;
		x4=off*x4+(1-off)*x1;
		y4=off*y4+(1-off)*y1;
		x1=tx1;
		y1=ty1;

    for (int i=0; i<100; i++)
	{
		DrawLine(backsurf, x1, y1, x2, y2, 2,i*2,i*2);
		DrawLine(backsurf, x2, y2, x3, y3, 2,i*2,i*2);
		DrawLine(backsurf, x3, y3, x4, y4, 2,i*2,i*2);
		DrawLine(backsurf, x4, y4, x1, y1, 2,i*2,i*2);
		tx1=p*x1+q*x2;
		ty1=p*y1+q*y2;
		x2=p*x2+q*x3;
		y2=p*y2+q*y3;
		x3=p*x3+q*x4;
		y3=p*y3+q*y4;
		x4=p*x4+q*x1;
		y4=p*y4+q*y1;
		x1=tx1;
		y1=ty1;
	}
	
	primsurf->Flip(NULL, DDFLIP_WAIT);
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
		DrawScene();

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
