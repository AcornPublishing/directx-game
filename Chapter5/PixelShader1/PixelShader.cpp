// PixelShader.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "PixelShader.h"
#include "../../common/dxfunc.h"
#define MAX_LOADSTRING 100

IDirect3D9 *pD3D = NULL;
IDirect3DDevice9 *pD3DDevice = NULL;

int iWidth=800;
int iHeight=600; 

DWORD dwNumMaterials;
ID3DXMesh *pMesh;
LPDIRECT3DTEXTURE9 *pMeshTextures;
D3DMATERIAL9 *pMeshMaterials;

float ViewAngle=0;
float ViewAngleOffset=0.05;

ID3DXEffect* pEffect;
IDirect3DVertexDeclaration9 *VertDecl;

D3DMATRIX View = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 15, 1,
};
D3DXMATRIX matProjection;

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

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
	LoadString(hInstance, IDC_PIXELSHADER, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_PIXELSHADER);

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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_PIXELSHADER);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_PIXELSHADER;
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

   pD3DDevice->SetTransform(D3DTS_VIEW, &View);

   dwNumMaterials = LoadMesh("nlo.x", pD3DDevice,
			   &pMesh, &pMeshTextures,
			   "texture.bmp", &pMeshMaterials
			   );

   D3DXCreateEffectFromFile(pD3DDevice, "simple.vsh", 0, 0, 0, 0, &pEffect, 0);
   pEffect->SetTechnique("PixelLight");

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
		0, 0, 0, 1,
	};
    
	D3DXMATRIX Full;
    D3DXMatrixMultiply(&Full, (D3DXMATRIX*)&World, (D3DXMATRIX*)&View);
    D3DXMatrixMultiply(&Full, &Full, &matProjection);
 	
    pEffect->SetValue("mat", &Full, D3DX_DEFAULT);
    pEffect->SetValue("worldmat", &World, D3DX_DEFAULT);
    pEffect->SetValue("light", new D3DXVECTOR4(ViewAngle*2, 0, -3, 1), D3DX_DEFAULT);
    pEffect->SetValue("facecolor", new D3DXVECTOR4(0.8f, 0.0f, 0.8f, 1.0f), D3DX_DEFAULT);
 	
	pD3DDevice->SetVertexDeclaration(VertDecl);
    
	UINT uPass;
	pEffect->Begin(&uPass, 0);
	for (int i=0; i<uPass; i++)
	{
		pEffect->Pass(i);
		for (DWORD i=0; i<dwNumMaterials; i++)
		{			
			pMesh->DrawSubset(i);
		}
	}
	pEffect->End();
}