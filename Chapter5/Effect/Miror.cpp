// Miror.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Miror.h"
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

ID3DXMesh *pObject;
LPDIRECT3DTEXTURE9 *pMeshTextures;
D3DMATERIAL9 *pMeshMaterials;
DWORD dwNumMaterials;

ID3DXMesh *pTube;
LPDIRECT3DTEXTURE9 *pTubeTextures;
D3DMATERIAL9 *pTubeMaterials;
DWORD dwTubeNumMaterials;

ID3DXMesh *pRoom;
LPDIRECT3DTEXTURE9 *pRoomTextures;
D3DMATERIAL9 *pRoomMaterials;
DWORD dwRoomMaterials;

D3DXMATRIXA16 matProj;

D3DXMATRIXA16 mProject;
D3DXMATRIXA16 mView;
D3DXMATRIXA16 mWorld;
D3DXMATRIXA16 mFlyer;

ID3DXEffect* pEffect;
ID3DXRenderToEnvMap* pRenderToEnvMap;
IDirect3DCubeTexture9* pCubeTexture;


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
	LoadString(hInstance, IDC_MIROR, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_MIROR);

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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_MIROR);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_MIROR;
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

   D3DXMATRIXA16 m;
   D3DXMatrixIdentity(&mWorld);
   D3DXMatrixRotationX(&mWorld, -2.0f);
   D3DXMatrixScaling(&m, 0.5f, 0.2f, 0.3f);
   D3DXMatrixMultiply(&mWorld, &mWorld, &m);
   
   D3DXMatrixTranslation(&mView, 0.0f, -1.0f, 7.0f);

   dwTubeNumMaterials = LoadMesh("mirror.x", pD3DDevice,
			   &pTube, &pTubeTextures,
			   "texture.bmp", &pTubeMaterials);

   dwNumMaterials = LoadMesh("nlo.x", pD3DDevice,
			   &pObject, &pMeshTextures,
			   "texture.bmp", &pMeshMaterials);

   dwRoomMaterials = LoadMesh("room.x", pD3DDevice,
			   &pRoom, &pRoomTextures,
			   "Room.bmp", &pRoomMaterials);

   if (FAILED(D3DXCreateEffect(pD3DDevice, cEffect, sizeof(cEffect)-1, NULL, NULL, 0, NULL, &pEffect, NULL)))
	   return E_FAIL;
   
   if (FAILED(D3DXCreateRenderToEnvMap(pD3DDevice, 256, 1,
	   D3DFMT_A8R8G8B8, TRUE, D3DFMT_D16, &pRenderToEnvMap)))
	   return E_FAIL;

   if (FAILED(D3DXCreateCubeTexture( pD3DDevice, 256, 1,
	   D3DUSAGE_RENDERTARGET, D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT, &pCubeTexture)))
	   return E_FAIL;
    
   pEffect->SetTexture("CubeMapTex", pCubeTexture);

   pD3DDevice->GetTransform(D3DTS_PROJECTION, &mProject);
   D3DXMatrixPerspectiveFovLH(&matProj, D3DX_PI * 0.5f, 1.0f, 0.5f, 1000.0f);

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
		pD3DDevice->Clear(1, (D3DRECT*)&r, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(100, 100, 100), 1.0, 0);

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

HRESULT RenderWorld(CONST D3DXMATRIXA16 *pView, CONST D3DXMATRIXA16 *pProject)
{
    return S_OK;
}

void DrawScene()
{
	ViewAngle+=0.05f;
    D3DXMATRIXA16  mat;
    D3DXMatrixScaling(&mFlyer, 0.1f, 0.1f, 0.1f);
    D3DXMatrixTranslation(&mat, 0.0f, 0.0f, 3.0f);
    D3DXMatrixMultiply(&mFlyer, &mFlyer, &mat);
    D3DXMatrixRotationY(&mat, ViewAngle);
    D3DXMatrixMultiply(&mFlyer, &mFlyer, &mat);

	HRESULT hr;
	
    // Render to map
    D3DXMATRIXA16 mViewDir( mView );
    mViewDir._41 = 0.0f; mViewDir._42 = 0.0f; mViewDir._43 = 0.0f;

    hr = pRenderToEnvMap->BeginCube( pCubeTexture );

    for (int i=0; i<6; i++)
    {
        pRenderToEnvMap->Face((D3DCUBEMAP_FACES) i, 0);

        D3DXVECTOR3 vLookDir;
        D3DXVECTOR3 vUpDir;
		
		switch(i)
		{
		case D3DCUBEMAP_FACE_POSITIVE_X:
            vLookDir = D3DXVECTOR3( 1.0f, 0.0f, 0.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
        case D3DCUBEMAP_FACE_NEGATIVE_X:
            vLookDir = D3DXVECTOR3(-1.0f, 0.0f, 0.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
        case D3DCUBEMAP_FACE_POSITIVE_Y:
            vLookDir = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 0.0f,-1.0f );
            break;
        case D3DCUBEMAP_FACE_NEGATIVE_Y:
            vLookDir = D3DXVECTOR3( 0.0f,-1.0f, 0.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
            break;
        case D3DCUBEMAP_FACE_POSITIVE_Z:
            vLookDir = D3DXVECTOR3( 0.0f, 0.0f, 1.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
        case D3DCUBEMAP_FACE_NEGATIVE_Z:
            vLookDir = D3DXVECTOR3( 0.0f, 0.0f,-1.0f );
            vUpDir   = D3DXVECTOR3( 0.0f, 1.0f, 0.0f );
            break;
		}
		
        D3DXMATRIXA16 mView;
		D3DXVECTOR3 vEyePt   = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		D3DXMatrixLookAtLH(&mView, &vEyePt, &vLookDir, &vUpDir);
        D3DXMatrixMultiply(&mView, &mViewDir, &mView);

		D3DXMATRIXA16 matWorld;
		D3DXMatrixScaling(&matWorld, 10.0f, 10.0f, 10.0f);
	
		pD3DDevice->SetTransform(D3DTS_WORLD, &matWorld);
		pD3DDevice->SetTransform(D3DTS_VIEW, &mView);
		pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProj);
	
		pD3DDevice->SetRenderState( D3DRS_ZFUNC, D3DCMP_ALWAYS );
		for (DWORD i=0; i<dwRoomMaterials; i++)
		{
			if (pRoomTextures[i])
				pD3DDevice->SetTexture(0, pRoomTextures[i]);
			pRoom->DrawSubset(i);
		}

		pD3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
    
		pD3DDevice->SetTransform(D3DTS_WORLD, &mFlyer);
	
		for (DWORD i=0; i<dwNumMaterials; i++)
		{			
			if (pMeshTextures[i])
				pD3DDevice->SetTexture(0, pMeshTextures[i]);
			pObject->DrawSubset(i);
		}		
	}
    pRenderToEnvMap->End(0);

	// Render scene
	pD3DDevice->SetTransform(D3DTS_VIEW, &mView);
	pD3DDevice->SetTransform(D3DTS_PROJECTION, &mProject);
	
	// Render mirror
	pD3DDevice->SetTransform( D3DTS_WORLD, &mWorld);
	UINT uPass;
	pEffect->Begin(&uPass, 0);
	for (int i=0; i<uPass; i++)
	{
		pEffect->Pass(i);
		for (DWORD i=0; i<dwTubeNumMaterials; i++)
		{
			pTube->DrawSubset(i);
		}
	}

	pEffect->End();
}