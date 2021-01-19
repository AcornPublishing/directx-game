// Meta.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Meta.h"
#include "../../common/dxfunc.h"
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

IDirect3D9 *pD3D = NULL;
IDirect3DDevice9 *pD3DDevice = NULL;

int iWidth=800;
int iHeight=600;

DWORD dwNumMatSphere;
ID3DXMesh *pMeshSphere;
LPDIRECT3DTEXTURE9 *pMeshTexturesSphere;
D3DMATERIAL9 *pMeshMaterialsSphere;

IDirect3DCubeTexture9 * TextureSphere;
IDirect3DTexture9 * Texture1;

float ViewAngle=0;
float ViewAngleOffset=0.05;

ID3DXEffect* pEffectSphere;
ID3DXEffect* pEffectWall;
IDirect3DVertexDeclaration9 *VertDeclSphere;

D3DMATRIX View = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 55, 1,
};

D3DMATRIX View1 = {
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 5, 1,
};

D3DXMATRIX matProjection;

ID3DXMesh *pRoom;
LPDIRECT3DTEXTURE9 *pRoomTextures;
D3DMATERIAL9 *pRoomMaterials;
DWORD dwRoomMaterials;

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
	LoadString(hInstance, IDC_META, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_META);

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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_META);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_META;
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

   dwNumMatSphere = LoadMesh("sphere.x", pD3DDevice,
			   &pMeshSphere, &pMeshTexturesSphere,
			   "texture.bmp", &pMeshMaterialsSphere
			   );

   D3DXCreateCubeTextureFromFile(pD3DDevice, "LightCube.dds", &TextureSphere);
   D3DXCreateTextureFromFile(pD3DDevice, "wall.jpg", &Texture1);

   D3DXCreateEffectFromFile(pD3DDevice, "sphere.vsh", 0, 0, 0, 0, &pEffectSphere, 0);
   pEffectSphere->SetTechnique("PixelLight");
   pEffectSphere->SetValue("sphereTexture", &TextureSphere, D3DX_DEFAULT);

   D3DXCreateEffectFromFile(pD3DDevice, "wall.vsh", 0, 0, 0, 0, &pEffectWall, 0);
   pEffectWall->SetTechnique("PixelLight");
   pEffectWall->SetValue("sphereTexture", &TextureSphere, D3DX_DEFAULT);
   pEffectWall->SetValue("wallTexture", &Texture1, D3DX_DEFAULT);

   D3DVERTEXELEMENT9 decl[]=
   {
	   {0,0, D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,0},
	   {0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},  
	   D3DDECL_END()
   };

   pD3DDevice->CreateVertexDeclaration(decl, &VertDeclSphere);
   
   dwRoomMaterials = LoadMesh("room.x", pD3DDevice,
			   &pRoom, &pRoomTextures,
			   "Room.bmp", &pRoomMaterials);

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
	
	float b=ViewAngle;
	D3DMATRIX World = {
		cos(b)*cos(b), cos(b)*sin(b), sin(b), 0,
		-sin(b), cos(b), 0, 0,
		-sin(b)*cos(b), -sin(b)*sin(b), cos(b), 0,
		0, 0, 0, 1,
	};
    
	pD3DDevice->SetVertexDeclaration(VertDeclSphere);

	D3DXMATRIXA16 matWorld;
	D3DXMatrixScaling(&matWorld, 10.0f, 10.0f, 10.0f);

	D3DMATRIX matWorld1 = {
		cos(b)*cos(b), cos(b)*sin(b), sin(b),
		-sin(b), cos(b), 0,
		-sin(b)*cos(b), -sin(b)*sin(b), cos(b),
	};

	D3DXMATRIX Full1;
    D3DXMatrixMultiply(&Full1, (D3DXMATRIX*)&World, (D3DXMATRIX*)&View1);
    D3DXMatrixMultiply(&Full1, (D3DXMATRIX*)&View1, &matProjection);

	pEffectWall->SetValue("mat", &Full1, D3DX_DEFAULT);
	pEffectWall->SetValue("mat1", &matWorld1, D3DX_DEFAULT);
    pEffectWall->SetValue("view_position", &matWorld, D3DX_DEFAULT);
    pEffectWall->SetValue("ViewAngle", &ViewAngle, D3DX_DEFAULT);

	pD3DDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );
	UINT uPass;
	pEffectWall->Begin(&uPass, 0);
	for (int i=0; i<uPass; i++)
	{
		pEffectWall->Pass(i);
		for (DWORD i=0; i<dwRoomMaterials; i++)
		{
			pRoom->DrawSubset(i);
		}
	}
	pEffectWall->End();

	D3DXMATRIX Full;
    D3DXMatrixMultiply(&Full, (D3DXMATRIX*)&World, (D3DXMATRIX*)&View);
    D3DXMatrixMultiply(&Full, &Full, &matProjection);

	pEffectSphere->SetValue("mat", &Full, D3DX_DEFAULT);
    pEffectSphere->SetValue("view_position", &View, D3DX_DEFAULT);
    pEffectSphere->SetValue("ViewAngle", &ViewAngle, D3DX_DEFAULT);
  	
	pEffectSphere->Begin(&uPass, 0);
	for (int i=0; i<uPass; i++)
	{
		pEffectSphere->Pass(i);
		for (DWORD i=0; i<dwNumMatSphere; i++)
		{			
			pMeshSphere->DrawSubset(i);
		}
	}
	pEffectSphere->End();
}