// BitBltDirectDraw.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include <stdio.h>
#include "BitBltDirectDraw.h"
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
IDirectDrawSurface7 *imagesurf=NULL;
IDirectDrawSurface7 *imagesurf2=NULL;
BOOL bWindowed;
int iWidth, iHeight;

// Modes variables
struct dsModes
{
	DWORD width, height, depth;
};
dsModes dmList[100];
int dsModesTotal=0;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	DisplayModes(HWND, UINT, WPARAM, LPARAM);

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
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
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
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }
		
   HRESULT hr; 
   hr = DirectDrawCreateEx(NULL, (VOID**)&ppiDD, IID_IDirectDraw7, NULL);
   if(FAILED(hr))
	   return 0;
   
   int nItemSelected=DialogBox(hInst, (LPCTSTR)IDD_DISPLAYMODES, hWnd, (DLGPROC)DisplayModes);
   if (nItemSelected==65535)
	   return FALSE;

   bWindowed=TRUE;
   iWidth=dmList[nItemSelected].width;
   iHeight=dmList[nItemSelected].height;

   if (ppiDD)
   {
	   ppiDD->Release;
	   ppiDD=NULL;
   }
   DXDDInit(&ppiDD, &primsurf, &backsurf, hWnd, iWidth, iHeight, dmList[nItemSelected].depth, &bWindowed);
   imagesurf = LoadBMPToSurface("desktop.bmp", ppiDD);
   imagesurf2 = LoadBMPToSurface("vertol.bmp", ppiDD);

   DDCOLORKEY  cData;
   cData.dwColorSpaceLowValue = 0;
   cData.dwColorSpaceHighValue = 0;
   imagesurf2->SetColorKey(DDCKEY_SRCBLT, &cData);

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

	if (imagesurf->IsLost())
	{
		imagesurf->Release();
		imagesurf = LoadBMPToSurface("desktop.bmp", ppiDD);
	}

	if (imagesurf2->IsLost())
	{
		imagesurf2->Restore();
		imagesurf2 = LoadBMPToSurface(&imagesurf2, "vertol.bmp", ppiDD);
	}
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

		if ((imagesurf!=NULL) && (!bWindowed))
		{
			// 복사할 목표 영역
			RECT dst;
			dst.left=dst.top=0;
			dst.right=iWidth;
			dst.bottom=iHeight;

			// 첫번째 이미지 표시
			if (backsurf->Blt(&dst, imagesurf, 0, DDBLT_WAIT, 0)==DDERR_SURFACELOST)
			{
				RestoreSurfaces();
				backsurf->Blt(&dst, imagesurf, 0, DDBLT_WAIT, 0);
			}

			// 두번째 이미지 표시
			backsurf->BltFast(100, 100, imagesurf2, 0, DDBLTFAST_SRCCOLORKEY | DDBLTFAST_WAIT);

			primsurf->Flip(NULL, DDFLIP_WAIT);
		}
		
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

HRESULT WINAPI dmFound(LPDDSURFACEDESC2 desc, LPVOID p)
{
	if (desc->ddpfPixelFormat.dwRGBBitCount==16)
	{
		TCHAR szTemp[MAX_PATH];
		_sntprintf(szTemp, MAX_PATH-1, TEXT("%d x %d - %d"), 
			   desc->dwWidth, 
			   desc->dwHeight, 
			   desc->ddpfPixelFormat.dwRGBBitCount);
		
		SendMessage((HWND)p, LB_ADDSTRING, 0,
                     (LPARAM) TEXT(szTemp));

		dmList[dsModesTotal].width  = desc->dwWidth;
		dmList[dsModesTotal].height = desc->dwHeight;
		dmList[dsModesTotal].depth  = desc->ddpfPixelFormat.dwRGBBitCount;
		dsModesTotal++;

	}

	return DDENUMRET_OK;
}


// Message handler for display modes box.
LRESULT CALLBACK DisplayModes(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	HWND hWndListBox;
	switch (message)
	{
	case WM_INITDIALOG:
        hWndListBox = GetDlgItem(hDlg, IDC_LIST1);
 		ppiDD->EnumDisplayModes(0, 0, hWndListBox, dmFound);
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			if (LOWORD(wParam) == IDOK)
			{
				HWND hWndListBox = GetDlgItem(hDlg, IDC_LIST1);
				wParam = (int)SendMessage( hWndListBox, LB_GETCURSEL, 0, 0 );
			}
			else 
				wParam = 65535;
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}
