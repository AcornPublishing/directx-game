// InitDirectX.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "InitDirectX.h"
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
IDirect3DVertexBuffer9 * vBuffer;
IDirect3DIndexBuffer9 * iBuffer;
const int iVertsNum = 24;
const int iIdxNum = 36;
const int iPointsNum = 12;

struct sVertex
{
  float x, y, z;
  DWORD color;
};

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
IDirect3DTexture9 *pTexture;

void CreateQuad()
{
	D3DXCreateTextureFromFileEx(pD3DDevice, "Particle.bmp", 
                D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, 
                D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
                D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 0, NULL, NULL, &pTexture);


	const sVertex svVortexList[] = {
    -0.5f, -0.5f, -0.5f, 0x00ff0000,
    -0.5f,  0.5f, -0.5f, 0x00ff0000,
     0.5f, -0.5f, -0.5f, 0x00ff0000,
     0.5f,  0.5f, -0.5f, 0x00ff0000,

     0.5f, -0.5f, -0.5f, 0x0000ff00,
     0.5f,  0.5f, -0.5f, 0x0000ff00,
     0.5f, -0.5f,  0.5f, 0x0000ff00,
     0.5f,  0.5f,  0.5f, 0x0000ff00,

     0.5f, -0.5f,  0.5f, 0x000000ff,
     0.5f,  0.5f,  0.5f, 0x000000ff,
    -0.5f, -0.5f,  0.5f, 0x000000ff,
    -0.5f,  0.5f,  0.5f, 0x000000ff,

    -0.5f, -0.5f,  0.5f, 0x00ffff00,
    -0.5f,  0.5f,  0.5f, 0x00ffff00,
    -0.5f, -0.5f, -0.5f, 0x00ffff00,
    -0.5f,  0.5f, -0.5f, 0x00ffff00,

    -0.5f,  0.5f, -0.5f, 0x0000ffff,
    -0.5f,  0.5f,  0.5f, 0x0000ffff,
     0.5f,  0.5f, -0.5f, 0x0000ffff,
     0.5f,  0.5f,  0.5f, 0x0000ffff,

     0.5f, -0.5f, -0.5f, 0x00ff00ff,
     0.5f, -0.5f,  0.5f, 0x00ff00ff,
    -0.5f, -0.5f, -0.5f, 0x00ff00ff,
    -0.5f, -0.5f,  0.5f, 0x00ff00ff,
  };
  
  void * pBuf;

  HRESULT hRes = pD3DDevice->CreateVertexBuffer(sizeof(sVertex) * iVertsNum,
       D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &vBuffer, 0);
  if (FAILED(hRes))
    return;

  // 정점 버퍼 복사
  hRes = vBuffer->Lock(0, sizeof(sVertex) * iVertsNum, &pBuf, 0);
  if (FAILED(hRes))
    return;
  memcpy(pBuf, svVortexList, sizeof(sVertex) * iVertsNum);
  vBuffer->Unlock();

  const unsigned short Idxes[]={
    0,1,2,2,1,3,
    4,5,6,6,5,7,
    8,9,10,10,9,11,
    12,13,14,14,13,15,
    16,17,18,18,17,19,
    20,21,22,22,21,23,
  };

  // 인덱스 버퍼 생성
  hRes = pD3DDevice->CreateIndexBuffer( sizeof(short) * iIdxNum, 
       0, D3DFMT_INDEX16, D3DPOOL_DEFAULT,&iBuffer, 0);
  if (FAILED(hRes))
    return;

  // 인덱스 버퍼 복사
  hRes = iBuffer->Lock(0, sizeof(short) * iIdxNum, &pBuf, 0);
  if (FAILED(hRes))
    return;
  memcpy(pBuf, Idxes, sizeof(short) * iIdxNum);
  iBuffer->Unlock();
}

void SetView()
{
  D3DMATRIX View = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 5, 1,
  };
  pD3DDevice->SetTransform(D3DTS_VIEW, &View);
}

///////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////
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
	LoadString(hInstance, IDC_INITDIRECTX, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}
    
	// 화면에 그릴 3D 객체 초기화
	CreateQuad();
	SetView();
    
	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_INITDIRECTX);

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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_INITDIRECTX);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_INITDIRECTX;
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
      CW_USEDEFAULT, 0, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   if (DX3DInit(&pD3D, &pD3DDevice, hWnd, iWidth, iHeight, FALSE)!=S_OK)
   {
	   MessageBox(hWnd, "Error initializing DirectX", "Error", MB_OK);	   
	   return FALSE;
   }

  DWORD ps;
  pD3DDevice->GetRenderState(D3DRS_POINTSIZE, &ps);
  pD3DDevice->SetRenderState(D3DRS_POINTSIZE, ps*1.02);

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

   return TRUE;
}

void DrawScene()
{
	float b = float((ViewAngle++)%0x1111)/8;
	
	D3DMATRIX World = {
		cos(b)*cos(b), cos(b)*sin(b), sin(b), 0,
		-sin(b), cos(b), 0, 0,
		-sin(b)*cos(b), -sin(b)*sin(b), cos(b), 0,
		0, 0,  0, 1,
	};
	
	pD3DDevice->SetTransform(D3DTS_WORLD, &World);
    
	pD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
	pD3DDevice->SetStreamSource(0, vBuffer, 0, sizeof(sVertex));
	pD3DDevice->SetIndices(iBuffer);

    pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
    pD3DDevice->SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );
    pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

	pD3DDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE);
    pD3DDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE);

	pD3DDevice->SetTexture(0, pTexture);
	pD3DDevice->DrawIndexedPrimitive(D3DPT_POINTLIST, 0, 0, iVertsNum, 0, iPointsNum);

	pD3DDevice->SetRenderState( D3DRS_POINTSPRITEENABLE, FALSE );
    pD3DDevice->SetRenderState( D3DRS_POINTSCALEENABLE,  FALSE );
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;

	switch (message) 
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
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
		pD3DDevice->Clear(1, (D3DRECT*)&r, D3DCLEAR_TARGET, 0, 1.0, 0);

		pD3DDevice->BeginScene();
		DrawScene();
		pD3DDevice->EndScene();

		pD3DDevice->Present(NULL, NULL, 0, NULL);
		break;
	case WM_DESTROY:
		if (vBuffer) {vBuffer->Release; vBuffer=NULL;}
		if (iBuffer) {iBuffer->Release; iBuffer=NULL;}
		if (pD3DDevice) {pD3DDevice= NULL; pD3DDevice=NULL;}
		if (pD3D) {pD3D= NULL; pD3D=NULL;}
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}