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

D3DMATERIAL9 mat;

struct sVertex
{
  float x, y, z;
  DWORD color;
};

   
sVertex Triangle[3];
IDirect3DVertexBuffer9 * vTriangleBuffer;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
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

   if (DX3DInitZ(&pD3D, &pD3DDevice, hWnd, iWidth, iHeight, FALSE)!=S_OK)
   {
	   MessageBox(hWnd, "Error Initializing DirectX", "Error", MB_OK);	   
	   return FALSE;
   }

   dwNumMaterials = LoadMesh("nlo.x", pD3DDevice,
			   &pMesh, &pMeshTextures,
			   "texture.bmp", &pMeshMaterials
			   );

   ZeroMemory(&mat, sizeof(D3DMATERIAL9));
   mat.Diffuse.r=255;
   mat.Diffuse.g=255;
   mat.Diffuse.b=255;
   mat.Diffuse.a=0;
   mat.Ambient=mat.Diffuse;

   Triangle[0].x=0.5f;
   Triangle[0].y=-0.5f;
   Triangle[0].z=-0.1f;

   Triangle[1].x=-0.5f;
   Triangle[1].y=-0.5f;
   Triangle[1].z=0.2f;

   Triangle[2].x=0.0f;
   Triangle[2].y=0.5f;
   Triangle[2].z=0.1f;

   Triangle[0].color = D3DCOLOR_XRGB(255, 255, 0);
   Triangle[1].color = D3DCOLOR_XRGB(255, 255, 0);
   Triangle[2].color = D3DCOLOR_XRGB(255, 255, 0);
   
   void * pBuf;
   if (FAILED(pD3DDevice->CreateVertexBuffer(sizeof(sVertex) * 3,
	   D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_DEFAULT, &vTriangleBuffer, 0)))
	   return FALSE;

   if (FAILED(vTriangleBuffer->Lock(0, sizeof(sVertex) * 3, &pBuf, 0)))
	   return FALSE;
   memcpy(pBuf, Triangle, sizeof(sVertex) * 3);
   vTriangleBuffer->Unlock();

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
		pD3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255,255,255), 1.0, 0);

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

void DrawScene()
{
    float b = float((ViewAngle++)%0x1111)/24;
    float b1 = float((ViewAngle++)%0x1111)/260;

	D3DLIGHT9 light;
    ZeroMemory(&light, sizeof(D3DLIGHT9));
	light.Type = D3DLIGHT_POINT;
	light.Position = D3DXVECTOR3(cos(b1), cos(b1)*30, 20);
	light.Diffuse.r = light.Diffuse.g = light.Diffuse.b = 1.0f;
	light.Ambient=light.Diffuse;
	light.Specular=light.Diffuse;
	light.Range=25.0f;
	light.Attenuation0=0.0f;
	light.Attenuation1=0.0f;
	light.Attenuation2=1.0f;
	pD3DDevice->SetLight(0, &light);
	pD3DDevice->LightEnable(0, TRUE);
	
	pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	
	pD3DDevice->SetMaterial(&mat);
  
	D3DMATRIX World = {
		cos(b)*cos(b), cos(b)*sin(b), sin(b), 0,
		-sin(b), cos(b), 0, 0,
		-sin(b)*cos(b), -sin(b)*sin(b), cos(b), 0,
		0, 0, 40, 1,
	};

	pD3DDevice->SetTransform(D3DTS_WORLD, &World);

	for (DWORD i=0; i<dwNumMaterials; i++)
	{			
		if (pMeshTextures[i])
			pD3DDevice->SetTexture(0, pMeshTextures[i]);

		pMesh->DrawSubset(i);
	}		
	
	pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);
	pD3DDevice->SetTexture(0, NULL);
	
	// Draw sun
	D3DMATRIX World1 = {
		1, 0, 0, 0,
	    0, 1, 0, 0,
		0, 0, 1, 0,
	    cos(b1), cos(b1)*30, 20, 1,
	};
	pD3DDevice->SetTransform(D3DTS_WORLD, &World1);
    
	pD3DDevice->SetStreamSource(0, vTriangleBuffer, 0, sizeof(sVertex));
	pD3DDevice->SetVertexShader(NULL);
	pD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
    
	pD3DDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 1);
}