// bomb.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "bomb.h"
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
  float u, v;
};

struct sQuad
{
  float x, y;
  float xInc, yInc;
  float angleInc;
  float angle;
};

double tStartAnimTime;
const int AnimTime = 4000;

sVertex Triangle[4];
sQuad Quads[16];
IDirect3DVertexBuffer9 * vTriangleBuffer;
IDirect3DTexture9 *tQuadTexture;

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
	LoadString(hInstance, IDC_BOMB, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_BOMB);

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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_BOMB);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_BOMB;
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
   
   tStartAnimTime = GetTickCount();   
   
   if (DX3DInit(&pD3D, &pD3DDevice, hWnd, iWidth, iHeight, FALSE)!=S_OK)
   {
	   MessageBox(hWnd, "Error Initializing DirectX", "Error", MB_OK);	   
	   return FALSE;
   }

   D3DXCreateTextureFromFile(pD3DDevice, "tex.bmp", &tQuadTexture);

   if (FAILED(pD3DDevice->CreateVertexBuffer(sizeof(sVertex) * 4,
	   D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &vTriangleBuffer, 0)))
	   return FALSE;

   D3DMATRIX View = {
	   1, 0, 0, 0,
	   0, 1, 0, 0,
	   0, 0, 1, 0,
	   0, 0, 2, 1,
   };
   
   pD3DDevice->SetTransform(D3DTS_VIEW, &View);
  
   for (int i=0; i<4; i++)
	   for (int j=0; j<4; j++)
	   {
		   Quads[i*4+j].x=j*0.2f;
		   Quads[i*4+j].y=i*0.2f;
		   Quads[i*4+j].xInc=(float)((rand() % 8) / 100.0f-0.05f);
		   Quads[i*4+j].yInc=(float)((rand() % 8) / 100.0f-0.05f);
		   Quads[i*4+j].angleInc=(float)((rand() % 8) / 50.0f);
		   Quads[i*4+j].angle=0;
	   }
  
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
		pD3DDevice->Clear(1, (D3DRECT*)&r, D3DCLEAR_TARGET, D3DCOLOR_XRGB(ViewAngle,0, 0), 1.0, 0);

		pD3DDevice->BeginScene();
		DrawScene();
		pD3DDevice->EndScene();

		pD3DDevice->Present(NULL, NULL, 0, NULL);
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

void DrawScene()
{
   // ��ü ǥ��
   pD3DDevice->SetStreamSource(0, vTriangleBuffer, 0, sizeof(sVertex));
   pD3DDevice->SetVertexShader(NULL);
   pD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);
   pD3DDevice->SetTexture(0, tQuadTexture);
	
   for (int i=0; i<4; i++)
	   for (int j=0; j<4; j++)
	   {
		   Triangle[0].x=-0.4f;
		   Triangle[0].y=-0.4f;
		   Triangle[0].z=0.1f;
		   Triangle[0].u = (j)*0.25f;
		   Triangle[0].v = (i)*0.25f;
		   
		   Triangle[1].x=-0.4f;
		   Triangle[1].y=-0.2f;
		   Triangle[1].z=0.1f;
		   Triangle[1].u = (j)*0.25f;
		   Triangle[1].v = (i+1)*0.25f;
		   
		   Triangle[2].x=-0.2f;
		   Triangle[2].y=-0.4f;
		   Triangle[2].z=0.1f;
		   Triangle[2].u = (j+1)*0.25f;
		   Triangle[2].v = (i)*0.25f;
		   
		   Triangle[3].x=-0.2f;
		   Triangle[3].y=-0.2f;
		   Triangle[3].z=0.1f;
		   Triangle[3].u = (j+1)*0.25f;
		   Triangle[3].v = (i+1)*0.25f;

		   void * pBuf;
		   if (FAILED(vTriangleBuffer->Lock(0, sizeof(sVertex) * 4, &pBuf, 0)))
			   return;
		   memcpy(pBuf, Triangle, sizeof(sVertex) * 4);
		   vTriangleBuffer->Unlock();
		   
		   D3DMATRIX World = {
			   cos(Quads[i*4+j].angle), -sin(Quads[i*4+j].angle), 0, 0,
			   sin(Quads[i*4+j].angle), cos(Quads[i*4+j].angle), 0, 0,
			   0, 0, 1, 0,
			   Quads[i*4+j].x, Quads[i*4+j].y, 0, 1,
	       };			   
		   pD3DDevice->SetTransform(D3DTS_WORLD, &World);

		   pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
		   
		   double tCurrentTime = GetTickCount();
		   if ((tCurrentTime-tStartAnimTime)>AnimTime)
		   {
			   Quads[i*4+j].x+=Quads[i*4+j].xInc;
			   Quads[i*4+j].y+=Quads[i*4+j].yInc;
			   Quads[i*4+j].angle+=Quads[i*4+j].angleInc;
		   }
	   }
}
