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
  float nx, ny, nz;
};

   
sVertex Triangle[7];
IDirect3DVertexBuffer9 * vTriangleBuffer;
D3DMATERIAL9 mat, mat2;

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

   if (DX3DInitZ(&pD3D, &pD3DDevice, hWnd, iWidth, iHeight, FALSE)!=S_OK)
   {
	   MessageBox(hWnd, "Error Initializing DirectX", "Error", MB_OK);	   
	   return FALSE;
   }

   Triangle[0].x=0.7f;
   Triangle[0].y=-0.3f;
   Triangle[0].z=0.5f;
   Triangle[0].nx=0.0f;
   Triangle[0].ny=0.0f;
   Triangle[0].nz=1.0f;

   Triangle[1].x=-0.7f;
   Triangle[1].y=-0.3f;
   Triangle[1].z=-0.2f;
   Triangle[1].nx=0.0f;
   Triangle[1].ny=0.0f;
   Triangle[1].nz=-1.0f;

   Triangle[2].x=0.0f;
   Triangle[2].y=0.7f;
   Triangle[2].z=-1.5f;
   Triangle[2].nx=0.0f;
   Triangle[2].ny=0.0f;
   Triangle[2].nz=-1.0f;
   
   Triangle[3].x=-0.5f;
   Triangle[3].y=-0.5f;
   Triangle[3].z=0.0f;
   Triangle[3].nx=0.0f;
   Triangle[3].ny=0.0f;
   Triangle[3].nz=-1.0f;

   Triangle[4].x=-0.5f;
   Triangle[4].y=0.5f;
   Triangle[4].z=0.0f;
   Triangle[4].nx=0.0f;
   Triangle[4].ny=0.0f;
   Triangle[4].nz=-1.0f;

   Triangle[5].x=0.5f;
   Triangle[5].y=0.5f;
   Triangle[5].z=0.5f;
   Triangle[5].nx=0.0f;
   Triangle[5].ny=0.0f;
   Triangle[5].nz=-1.0f;

   Triangle[6].x=0.5f;
   Triangle[6].y=-0.5f;
   Triangle[6].z=-0.7f;
   Triangle[6].nx=1.0f;
   Triangle[6].ny=0.0f;
   Triangle[6].nz=-1.0f;

   void * pBuf;
   if (FAILED(pD3DDevice->CreateVertexBuffer(sizeof(sVertex) * 7,
	   D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_NORMAL, D3DPOOL_DEFAULT, &vTriangleBuffer, 0)))
	   return FALSE;

   // 정점 버퍼 복사
   if (FAILED(vTriangleBuffer->Lock(0, sizeof(sVertex) * 7, &pBuf, 0)))
	   return FALSE;
   memcpy(pBuf, Triangle, sizeof(sVertex) * 7);
   vTriangleBuffer->Unlock();
   
   D3DMATRIX View = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 4, 1,
   };
   pD3DDevice->SetTransform(D3DTS_VIEW, &View);
   
   ZeroMemory(&mat, sizeof(D3DMATERIAL9));
   mat.Diffuse.r=0;
   mat.Diffuse.g=255;
   mat.Diffuse.b=0;
   mat.Diffuse.a=255;
   mat.Ambient=mat.Diffuse;
 
   ZeroMemory(&mat2, sizeof(D3DMATERIAL9));
   mat2.Diffuse.r=0;
   mat2.Diffuse.g=255;
   mat2.Diffuse.b=255;
   mat2.Diffuse.a=255;
   mat2.Ambient=mat2.Diffuse;

   pD3DDevice->SetRenderState(D3DRS_ZENABLE, D3DZB_TRUE);

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
  D3DXMATRIX matView;

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
		pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0x00FFFFFF, 1.0, 0);

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

void DrawScene()
{
  float b = float((ViewAngle++)%0x1111)/24;

  D3DLIGHT9 Light;
  ZeroMemory(&Light, sizeof(D3DLIGHT9));
  Light.Type = D3DLIGHT_DIRECTIONAL;
  Light.Diffuse.r = Light.Diffuse.g = Light.Diffuse.b = Light.Diffuse.a = 1.0f;
  Light.Direction = D3DXVECTOR3(0.0f, cos(b)*1000, 0.5f);
  Light.Position = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
  pD3DDevice->SetLight(0, &Light);
  pD3DDevice->LightEnable(0, TRUE);

  pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
  
  D3DMATRIX World = {
    cos(b), -sin(b), 0, 0,
    sin(b), cos(b), 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1,
  };
  pD3DDevice->SetTransform(D3DTS_WORLD, &World);
  pD3DDevice->SetStreamSource(0, vTriangleBuffer, 0, sizeof(sVertex));
  pD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_NORMAL);
  pD3DDevice->SetMaterial(&mat);
  pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
 
  D3DMATRIX World1 = {
    cos(-b), -sin(-b), 0, 0,
    sin(-b), cos(-b), 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1,
  };
  pD3DDevice->SetTransform(D3DTS_WORLD, &World1);
  pD3DDevice->SetMaterial(&mat2);
  pD3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 3, 2);
  
  pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
}