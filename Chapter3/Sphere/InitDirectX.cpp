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
IDirect3D9       *pD3D       = NULL;
IDirect3DDevice9 *pD3DDevice = NULL;

int iWidth=800;
int iHeight=600; 
int ViewAngle=0;

DWORD dwNumMaterials;
ID3DXMesh *pMesh;
LPDIRECT3DTEXTURE9 *pMeshTextures;
D3DMATERIAL9 *pMeshMaterials;

DWORD dwNumMaterials1;
ID3DXMesh *pMesh1;
LPDIRECT3DTEXTURE9 *pMeshTextures1;
D3DMATERIAL9 *pMeshMaterials1;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);


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

   dwNumMaterials = LoadMesh("sphere.x", pD3DDevice,
			   &pMesh, &pMeshTextures,
			   "texture.bmp", &pMeshMaterials
			   );

   dwNumMaterials1 = LoadMesh("sphere1.x", pD3DDevice,
			   &pMesh1, &pMeshTextures1,
			   "texture.bmp", &pMeshMaterials1
			   );

   // Set View matrix
  D3DMATRIX View = {
    1, 0, 0, 0,
    0, 1, 0, 0,
    0, 0, 1, 0,
    0, 0, 409, 1,
  };
  pD3DDevice->SetTransform(D3DTS_VIEW, &View);

  D3DLIGHT9 light;
  ZeroMemory(&light, sizeof(D3DLIGHT9));
  light.Type = D3DLIGHT_DIRECTIONAL;
  light.Direction = D3DXVECTOR3(0.5f, 0.0f, 0.5f);
  light.Diffuse.r = light.Diffuse.g = light.Diffuse.b = light.Diffuse.a = 1.0f;
  pD3DDevice->SetLight(0, &light);
  pD3DDevice->LightEnable(0, TRUE);

  ShowWindow(hWnd, nCmdShow);
  UpdateWindow(hWnd);

   return TRUE;
}

void DrawScene()
{
	float b = float((ViewAngle++)%0x1111)/46;
    
	D3DMATRIX World = {
		cos(b)*cos(b), cos(b)*sin(b), sin(b), 0,
		-sin(b), cos(b), 0, 0,
		-sin(b)*cos(b), -sin(b)*sin(b), cos(b), 0,
		0, 0,  0, 1,
	};

	pD3DDevice->SetTransform(D3DTS_WORLD, &World);
	
//	pD3DDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// Draw sphere.x
	for (DWORD i=0; i<dwNumMaterials; i++)
	{			
		pD3DDevice->SetMaterial(&pMeshMaterials[i]);

		if (pMeshTextures[i])
			pD3DDevice->SetTexture(0, pMeshTextures[i]);

		pMesh->DrawSubset(i);
	}		

	D3DMATRIX World1 = {
		cos(b)*cos(b), cos(b)*sin(b), sin(b), 0,
		-sin(b), cos(b), 0, 0,
		-sin(b)*cos(b), -sin(b)*sin(b), cos(b), 0,
		0, 100,  0, 1,
	};
	pD3DDevice->SetTransform(D3DTS_WORLD, &World1);

	// Draw sphere1.x
	for (DWORD i=0; i<dwNumMaterials1; i++)
	{			
		pD3DDevice->SetMaterial(&pMeshMaterials1[i]);

		if (pMeshTextures1[i])
			pD3DDevice->SetTexture(0, pMeshTextures1[i]);

		pMesh1->DrawSubset(i);
	}		
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
		pD3DDevice->Clear(1, (D3DRECT*)&r, D3DCLEAR_TARGET, D3DCOLOR_XRGB(255,255,255), 1.0, 0);

		pD3DDevice->BeginScene();
		DrawScene();
		pD3DDevice->EndScene();


		pD3DDevice->Present(NULL, NULL, 0, NULL);
		break;
	case WM_DESTROY:
		pD3DDevice->Release();
		pD3DDevice= NULL;
		pD3D->Release();
		pD3D = NULL;
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}