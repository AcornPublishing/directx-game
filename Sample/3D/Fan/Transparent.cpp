// Transparent.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Transparent.h"
#include "../../../common/dxfunc.h"
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Direct3D objects
IDirect3D9 *pD3D = NULL;
IDirect3DDevice9 *pD3DDevice = NULL;

int iWidth=800;
int iHeight=600; 

struct sVertex
{
  float x, y, z;
  DWORD color;
};

   
sVertex Triangle[5];
IDirect3DVertexBuffer9 * vTriangleBuffer;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void SetView();

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
	LoadString(hInstance, IDC_TRANSPARENT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_TRANSPARENT);

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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_TRANSPARENT);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_TRANSPARENT;
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

   if (DX3DInit(&pD3D, &pD3DDevice, hWnd, iWidth, iHeight, FALSE)!=S_OK)
   {
	   MessageBox(hWnd, "Error Initializing DirectX", "Error", MB_OK);	   
	   return FALSE;
   }

   Triangle[0].x=-0.7f;
   Triangle[0].y=-0.7f;
   Triangle[0].z=0.1f;

   Triangle[1].x=-0.9f;
   Triangle[1].y=-0.3f;
   Triangle[1].z=0.1f;

   Triangle[2].x=0.0f;
   Triangle[2].y=0.4f;
   Triangle[2].z=0.1f;
   
   Triangle[3].x=0.3f;
   Triangle[3].y=0.0f;
   Triangle[3].z=0.1f;

   Triangle[4].x=0.3f;
   Triangle[4].y=-0.4f;
   Triangle[4].z=0.1f;

   Triangle[0].color = D3DCOLOR_XRGB(0, 0, 255);
   Triangle[1].color = D3DCOLOR_XRGB(0, 255, 0);
   Triangle[2].color = D3DCOLOR_XRGB(255, 0, 0);
   Triangle[3].color = D3DCOLOR_XRGB(255, 0, 255);
   Triangle[4].color = D3DCOLOR_XRGB(0, 255, 255);

   void * pBuf;
   if (FAILED(pD3DDevice->CreateVertexBuffer(sizeof(sVertex) * 5,
	   D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &vTriangleBuffer, 0)))
	   return FALSE;

  // 정점 버퍼 복사
   if (FAILED(vTriangleBuffer->Lock(0, sizeof(sVertex) * 5, &pBuf, 0)))
	   return FALSE;
   memcpy(pBuf, Triangle, sizeof(sVertex) * 5);
   vTriangleBuffer->Unlock();
   
   SetView();

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
		RECT  r;
		SetRect(&r, 0, 0, iWidth, iHeight);
		pD3DDevice->Clear(1, (D3DRECT*)&r, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0, 0, 0), 1.0, 0);

		pD3DDevice->BeginScene();
        
		pD3DDevice->SetStreamSource(0, vTriangleBuffer, 0, sizeof(sVertex));
		pD3DDevice->SetVertexShader(NULL);
		pD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
        
		pD3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 3);
		pD3DDevice->EndScene();

		pD3DDevice->Present(NULL, NULL, 0, NULL);
		break;
	case WM_DESTROY:
		if (pD3DDevice) {pD3DDevice= NULL; pD3DDevice=NULL;}
		if (pD3D) {pD3D= NULL; pD3D=NULL;}
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

void SetView()
{
  D3DMATRIX View = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 2, 1,
  };
  pD3DDevice->SetTransform(D3DTS_VIEW, &View);
}
