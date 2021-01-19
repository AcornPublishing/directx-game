// Transparent.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Transparent.h"
#include "../../common/dxfunc.h"
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
int ViewAngle=0;

struct sVertex
{
  float x, y, z;
  DWORD color;
};

   
sVertex Triangle[12];
IDirect3DVertexBuffer9 * vTriangleBuffer;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void DrawScene();
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

   Triangle[0].x=Triangle[3].x=Triangle[6].x=Triangle[9].x=0.7f;
   Triangle[0].y=Triangle[3].y=Triangle[6].y=Triangle[9].y=-0.3f;
   Triangle[0].z=Triangle[3].z=Triangle[6].z=Triangle[9].z=0.1f;

   Triangle[1].x=Triangle[4].x=Triangle[7].x=Triangle[10].x=-0.7f;
   Triangle[1].y=Triangle[4].y=Triangle[7].y=Triangle[10].y=-0.3f;
   Triangle[1].z=Triangle[4].z=Triangle[7].z=Triangle[10].z=0.1f;

   Triangle[2].x=Triangle[5].x=Triangle[8].x=Triangle[11].x=0.0f;
   Triangle[2].y=Triangle[5].y=Triangle[8].y=Triangle[11].y=0.7f;
   Triangle[2].z=Triangle[5].z=Triangle[8].z=Triangle[11].z=0.1f;
  
   Triangle[0].color = Triangle[1].color = Triangle[2].color = 
	   D3DCOLOR_ARGB(50, 255, 255, 255);
   Triangle[3].color = Triangle[4].color = Triangle[5].color = 
	   D3DCOLOR_ARGB(100, 200, 255, 200);
   Triangle[6].color = Triangle[7].color = Triangle[8].color = 
	   D3DCOLOR_ARGB(200, 100, 255, 100);
   Triangle[9].color = Triangle[10].color = Triangle[11].color = 
	   D3DCOLOR_ARGB(255, 0, 255, 0);

   void * pBuf;
   if (FAILED(pD3DDevice->CreateVertexBuffer(sizeof(sVertex) * 12,
	   D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &vTriangleBuffer, 0)))
	   return FALSE;

  // 정점 버퍼 복사
   if (FAILED(vTriangleBuffer->Lock(0, sizeof(sVertex) * 12, &pBuf, 0)))
	   return FALSE;
   memcpy(pBuf, Triangle, sizeof(sVertex) * 12);
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
		pD3DDevice->Clear(1, (D3DRECT*)&r, D3DCLEAR_TARGET, D3DCOLOR_XRGB(0,0, 0), 1.0, 0);

		pD3DDevice->BeginScene();
		DrawScene();
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

void SetWorldView(double b)
{
  D3DMATRIX World = {
    cos(b), -sin(b), 0, 0,
    sin(b), cos(b), 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1,
  };
  pD3DDevice->SetTransform(D3DTS_WORLD, &World);
}

void DrawScene()
{
	pD3DDevice->SetRenderState(D3DRS_ZENABLE, 1);
	pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
	pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
	
	double b = float((ViewAngle++)%0x1111)/16;
	
	// 객체 표시
	pD3DDevice->SetStreamSource(0, vTriangleBuffer, 0, sizeof(sVertex));
	pD3DDevice->SetVertexShader(NULL);
	pD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
	
	SetWorldView(b);
	pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
	SetWorldView(b+0.05);
	pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 3, 1);
	SetWorldView(b+0.1);
	pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 6, 1);
	SetWorldView(b+0.15);
	pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 9, 1);

	pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, 0);
}
