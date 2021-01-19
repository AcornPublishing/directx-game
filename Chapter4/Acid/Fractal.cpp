// Fractal.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Fractal.h"
#include <math.h>
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
void DrawScene();

struct PARTICLE
{
    WORD x, y;
	double angle;
	int adjust;
	int speed;
	BYTE r,g,b;
	int life;
} particle_ent;

const int iParticleNumber = 10000;
struct PARTICLE ParticleList[iParticleNumber];
WORD ScreenBuffer[800][600];

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
	LoadString(hInstance, IDC_FRACTAL, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_FRACTAL);

	for (int i=0; i<iParticleNumber; i++)
	{
		ParticleList[i].speed = 5+ rand()%5;
		ParticleList[i].angle = rand()%360;
		ParticleList[i].adjust=rand()%5+5;
		ParticleList[i].x = iWidth/2;
		ParticleList[i].y = iHeight/2;
		ParticleList[i].life=rand()%20;
		ParticleList[i].r=255;
		ParticleList[i].g=0;
		ParticleList[i].b=0;
	}


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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_FRACTAL);
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

   bWindowed=FALSE;
   iWidth=800;
   iHeight=600;
   DXDDInit(&ppiDD, &primsurf, &backsurf, hWnd, iWidth, iHeight, 16, &bWindowed);

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

void DrawScene()
{
	for (int i=0; i<iParticleNumber; i++)
	{
		ParticleList[i].angle+=ParticleList[i].adjust;
		ParticleList[i].x += (WORD)(cos(ParticleList[i].angle/360) * ParticleList[i].speed);
		ParticleList[i].y += (WORD)(sin(ParticleList[i].angle/360) * ParticleList[i].speed);

		ParticleList[i].life--;
		if ((ParticleList[i].life<0) || 
			(ParticleList[i].x<0) || 
			(ParticleList[i].x>=iWidth) ||
			(ParticleList[i].y<0) ||
			(ParticleList[i].y>=iHeight))
		{
			ParticleList[i].life=rand()%200;
			ParticleList[i].x=800/2;
			ParticleList[i].y=600/2;
		}

		ParticleList[i].g+=ParticleList[i].adjust;
		ParticleList[i].b-=ParticleList[i].adjust;

		WORD c = (WORD)(ParticleList[i].b/8 | (ParticleList[i].g/4 << 5) | (ParticleList[i].r/8 << 11));
		ScreenBuffer[ParticleList[i].x][ParticleList[i].y]=c;
	}

	
	for (int x=2; x<iWidth-1; x++)
		for (int y=2; y<iHeight-1; y++)
		{
		    ScreenBuffer[x][y] = (ScreenBuffer[x-1][y-1]
                 + ScreenBuffer[x-1][y]
                 + ScreenBuffer[x-1][y+1]
                 + ScreenBuffer[x][y-1]
                 + ScreenBuffer[x][y+1]
                 + ScreenBuffer[x+1][y-1]
                 + ScreenBuffer[x+1][y]
                 + ScreenBuffer[x+1][y+1])/8;
		}
	
  
    ClearSurface(backsurf, 0);
  	DDSURFACEDESC2 desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(desc);
    
	if (backsurf->Lock(0, &desc, DDLOCK_WAIT, 0)==DD_OK)
	{
        BYTE* dst = (BYTE *)desc.lpSurface;

		for (int x=0; x<iWidth; x++)
            for (int y=0; y<iHeight; y++)
				*(WORD *)(dst+y*desc.lPitch+x*2)=
				ScreenBuffer[x][y];

		backsurf->Unlock(0);
		primsurf->Flip(NULL, DDFLIP_WAIT);
	}
}