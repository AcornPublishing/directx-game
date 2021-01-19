// Shader.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Shader.h"
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

float ViewAngle=0;
float ViewAngleOffset=0.05;
IDirect3DVertexBuffer9 * vBuffer;
IDirect3DIndexBuffer9 * iBuffer;

ID3DXEffect* pEffect;
IDirect3DVertexDeclaration9 *VertDecl;

const int iVertsNum = 24;
const int iIdxNum = 36;
const int iPointsNum = 12;

D3DMATRIX View = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 5, 1,
};
D3DXMATRIX matProjection;

struct sVertex
{
    D3DXVECTOR3 p;
    D3DXVECTOR3 n;
};


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
	LoadString(hInstance, IDC_SHADER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_SHADER);

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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_SHADER);
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
	   MessageBox(hWnd, "Error initializing DirectX", "Error", MB_OK);	   
	   return FALSE;
   }
   
  sVertex svVortexList[iVertsNum];
  svVortexList[0].p = D3DXVECTOR3(-0.5f, -0.5f, -0.5f);
  svVortexList[1].p = D3DXVECTOR3(-0.5f,  0.5f, -0.5f);
  svVortexList[2].p = D3DXVECTOR3(0.5f, -0.5f, -0.5f);
  svVortexList[3].p = D3DXVECTOR3(0.5f,  0.5f, -0.5f);
  svVortexList[4].p = D3DXVECTOR3(0.5f, -0.5f, -0.5f);
  svVortexList[5].p = D3DXVECTOR3(0.5f,  0.5f, -0.5f);
  svVortexList[6].p = D3DXVECTOR3(0.5f, -0.5f,  0.5f);
  svVortexList[7].p = D3DXVECTOR3(0.5f,  0.5f,  0.5f);
  svVortexList[8].p = D3DXVECTOR3(0.5f, -0.5f,  0.5f);
  svVortexList[9].p = D3DXVECTOR3(0.5f,  0.5f,  0.5f);
  svVortexList[10].p = D3DXVECTOR3(-0.5f, -0.5f,  0.5f);
  svVortexList[11].p = D3DXVECTOR3(-0.5f,  0.5f,  0.5f);
  svVortexList[12].p = D3DXVECTOR3(-0.5f, -0.5f,  0.5f);
  svVortexList[13].p = D3DXVECTOR3(-0.5f,  0.5f,  0.5f);
  svVortexList[14].p = D3DXVECTOR3(-0.5f, -0.5f, -0.5f);
  svVortexList[15].p = D3DXVECTOR3(-0.5f,  0.5f, -0.5f);
  svVortexList[16].p = D3DXVECTOR3(-0.5f,  0.5f, -0.5f);
  svVortexList[17].p = D3DXVECTOR3(-0.5f,  0.5f,  0.5f);
  svVortexList[18].p = D3DXVECTOR3(0.5f,  0.5f, -0.5f);
  svVortexList[19].p = D3DXVECTOR3(0.5f,  0.5f,  0.5f);
  svVortexList[20].p = D3DXVECTOR3(0.5f, -0.5f, -0.5f);
  svVortexList[21].p = D3DXVECTOR3(0.5f, -0.5f,  0.5f);
  svVortexList[22].p = D3DXVECTOR3(-0.5f, -0.5f, -0.5f);
  svVortexList[23].p = D3DXVECTOR3(-0.5f, -0.5f,  0.5f);
  D3DXVec3Normalize(&svVortexList[0].n, &svVortexList[0].p);
  D3DXVec3Normalize(&svVortexList[1].n, &svVortexList[1].p);
  D3DXVec3Normalize(&svVortexList[2].n, &svVortexList[2].p);
  D3DXVec3Normalize(&svVortexList[3].n, &svVortexList[3].p);
  D3DXVec3Normalize(&svVortexList[4].n, &svVortexList[4].p);
  D3DXVec3Normalize(&svVortexList[5].n, &svVortexList[5].p);
  D3DXVec3Normalize(&svVortexList[6].n, &svVortexList[6].p);
  D3DXVec3Normalize(&svVortexList[7].n, &svVortexList[7].p);
  D3DXVec3Normalize(&svVortexList[8].n, &svVortexList[8].p);
  D3DXVec3Normalize(&svVortexList[9].n, &svVortexList[9].p);
  D3DXVec3Normalize(&svVortexList[10].n, &svVortexList[10].p);
  D3DXVec3Normalize(&svVortexList[11].n, &svVortexList[11].p);
  D3DXVec3Normalize(&svVortexList[12].n, &svVortexList[12].p);
  D3DXVec3Normalize(&svVortexList[13].n, &svVortexList[13].p);
  D3DXVec3Normalize(&svVortexList[14].n, &svVortexList[14].p);
  D3DXVec3Normalize(&svVortexList[15].n, &svVortexList[15].p);
  D3DXVec3Normalize(&svVortexList[16].n, &svVortexList[16].p);
  D3DXVec3Normalize(&svVortexList[17].n, &svVortexList[17].p);
  D3DXVec3Normalize(&svVortexList[18].n, &svVortexList[18].p);
  D3DXVec3Normalize(&svVortexList[19].n, &svVortexList[19].p);
  D3DXVec3Normalize(&svVortexList[20].n, &svVortexList[20].p);
  D3DXVec3Normalize(&svVortexList[21].n, &svVortexList[21].p);
  D3DXVec3Normalize(&svVortexList[22].n, &svVortexList[22].p);
  D3DXVec3Normalize(&svVortexList[23].n, &svVortexList[23].p);
   
   void * pBuf;
      
   if (FAILED(pD3DDevice->CreateVertexBuffer(sizeof(sVertex) * iVertsNum,
	   D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &vBuffer, 0)))
	   return FALSE;

   if (FAILED(vBuffer->Lock(0, sizeof(sVertex) * iVertsNum, &pBuf, 0)))
	   return FALSE;
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
  if (FAILED(pD3DDevice->CreateIndexBuffer( sizeof(short) * iIdxNum, 
       0, D3DFMT_INDEX16, D3DPOOL_DEFAULT,&iBuffer, 0)))
    return FALSE;

  // 인덱스 버퍼 복사
  if (FAILED(iBuffer->Lock(0, sizeof(short) * iIdxNum, &pBuf, 0)))
    return FALSE;
  memcpy(pBuf, Idxes, sizeof(short) * iIdxNum);
  iBuffer->Unlock();

   D3DXCreateEffectFromFile(pD3DDevice, "simple.vsh", 0, 0, 0, 0, &pEffect, 0);
   pEffect->SetTechnique("Transform");
   
   D3DVERTEXELEMENT9 decl[]=
   {
	   {0,0, D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,0},
	   {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},  
	   D3DDECL_END()
   };

   pD3DDevice->CreateVertexDeclaration(decl, &VertDecl);
   
   float Aspect = (float)iWidth / (float)iHeight;
   D3DXMatrixPerspectiveFovLH(&matProjection, D3DX_PI/4.0f, Aspect, 0.1f, 1000.0f);
   
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
		pD3DDevice->Clear(1, (D3DRECT*)&r, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, 0, 1.0, 0);

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
	ViewAngle+=ViewAngleOffset;
	if (ViewAngle>10)
		ViewAngleOffset=-0.05f;
	if (ViewAngle<0)
		ViewAngleOffset=0.05f;
	
	float b=ViewAngle;
	D3DMATRIX World = {
		cos(b)*cos(b), cos(b)*sin(b), sin(b), 0,
		-sin(b), cos(b), 0, 0,
		-sin(b)*cos(b), -sin(b)*sin(b), cos(b), 0,
		0, 0,  0, 1,
	};
    
	D3DXMATRIX Full;
    D3DXMatrixMultiply(&Full, (D3DXMATRIX*)&World, (D3DXMATRIX*)&View);
    D3DXMatrixMultiply(&Full, &Full, &matProjection);
 	
	pEffect->SetValue("TimeFactor", &ViewAngle, D3DX_DEFAULT);
	pEffect->SetValue("mat", &Full, D3DX_DEFAULT);
	
	pD3DDevice->SetVertexDeclaration(VertDecl);
    
	pD3DDevice->SetStreamSource(0, vBuffer, 0, sizeof(sVertex));
    pD3DDevice->SetIndices(iBuffer);

	UINT uPass;
	pEffect->Begin(&uPass, 0);
	for (int i=0; i<uPass; i++)
	{
		pEffect->Pass(i);
		pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, iVertsNum, 0, iPointsNum);
	}
	pEffect->End();
}
