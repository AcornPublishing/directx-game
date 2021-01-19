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
float ViewAngle1=0;
float ViewAngle1Offset=0.01;

HWND hWnd;

struct sVERTEX
{
    D3DXVECTOR3 p;
    DWORD       color;
};

ID3DXMesh *pObject;
LPDIRECT3DTEXTURE9 *pMeshTextures;
D3DMATERIAL9 *pMeshMaterials;
DWORD dwNumMaterials;

D3DMATERIAL9 mat1;
D3DMATERIAL9 mat2;

D3DXVECTOR3 pt1(-3.0f, 3.0f, 3.0f);
D3DXVECTOR3 pt2( 3.0f, 3.0f, 3.0f);
D3DXVECTOR3 pt3(-3.0f,-3.0f, 3.0f);
D3DXVECTOR3 pt4( 3.0f,-3.0f, 3.0f);

D3DXMATRIXA16 ObjectMatrix;

IDirect3DVertexBuffer9 *pMirror;

D3DXVECTOR3  vEyePt;
D3DXVECTOR3  vLookatPt;
D3DXVECTOR3  vUpVec;

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

   vEyePt    = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
   vLookatPt = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
   vUpVec    = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

   dwNumMaterials = LoadMesh("mirror.x", pD3DDevice,
			   &pObject, &pMeshTextures,
			   "texture.bmp", &pMeshMaterials
			   );

   if( FAILED( pD3DDevice->CreateVertexBuffer( 4*sizeof(sVERTEX),
                                                  D3DUSAGE_WRITEONLY,
                                                  D3DFVF_XYZ | D3DFVF_DIFFUSE,
                                                  D3DPOOL_MANAGED, &pMirror, NULL ) ) )
        return E_FAIL;

   sVERTEX* v;
   pMirror->Lock( 0, 0, (void**)&v, 0 );
   v[0].p = pt1;
   v[1].p = pt2;
   v[2].p = pt3;
   v[3].p = pt4;
   v[0].color=v[1].color=v[2].color=v[3].color=0x80ffffff;
   pMirror->Unlock();

   D3DLIGHT9 light;
   D3DXVECTOR3 vecLightDirUnnormalized(0.0f, -10.0f, 10.0f);
   ZeroMemory(&light, sizeof(D3DLIGHT9));
   light.Type        = D3DLIGHT_DIRECTIONAL;
   light.Diffuse.r   = 1.0f;
   light.Diffuse.g   = 1.0f;
   light.Diffuse.b   = 1.0f;
   D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecLightDirUnnormalized );
   light.Position.x   = 0.0f;
   light.Position.y   = -10.0f;
   light.Position.z   = 10.0f;
   light.Range        = 1000.0f;   
   pD3DDevice->SetLight(0, &light);
   pD3DDevice->LightEnable(0, TRUE);

   ZeroMemory(&mat1, sizeof(D3DMATERIAL9));
   mat1.Diffuse.r=0;
   mat1.Diffuse.g=0;
   mat1.Diffuse.b=255;
   mat1.Diffuse.a=0;
   mat1.Ambient=mat1.Diffuse;
   
   ZeroMemory(&mat2, sizeof(D3DMATERIAL9));
   mat2.Diffuse.r=200;
   mat2.Diffuse.g=200;
   mat2.Diffuse.b=200;
   mat2.Diffuse.a=0;
   mat2.Ambient=mat2.Diffuse;

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

void RenderObject()
{
    D3DXMatrixRotationY(&ObjectMatrix, ViewAngle);

    D3DXMATRIXA16 matLocal, mWorld;
    pD3DDevice->GetTransform(D3DTS_WORLD, &mWorld);

    D3DXMatrixMultiply(&matLocal, &ObjectMatrix, &mWorld);
    pD3DDevice->SetTransform(D3DTS_WORLD, &matLocal);

	pD3DDevice->SetMaterial(&mat1);

	// Draw Object
	for (DWORD i=0; i<dwNumMaterials; i++)
 		pObject->DrawSubset(i);

	pD3DDevice->SetMaterial(&mat2);

    pD3DDevice->SetTransform(D3DTS_WORLD, &mWorld);
}

void DrawScene()
{
	pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	ViewAngle1+=ViewAngle1Offset;
	if (ViewAngle1>1)
		ViewAngle1Offset=-0.01;
	if (ViewAngle1<-1)
		ViewAngle1Offset=0.01;
    
	vEyePt.x = 10*ViewAngle1;
	vEyePt.y = 10*ViewAngle1;
	vEyePt.z = -10;
	
	D3DXMATRIXA16 mView;
	D3DXMatrixLookAtLH(&mView, &vEyePt, &vLookatPt, &vUpVec);
	pD3DDevice->SetTransform(D3DTS_VIEW, &mView);

	ViewAngle += 0.1;
	RenderObject();

	D3DXMATRIXA16 mWorld;
    D3DXMATRIXA16 mReflectInMirror;
    D3DXPLANE  plane;

    pD3DDevice->GetTransform(D3DTS_WORLD, &mWorld);

    D3DXPlaneFromPoints(&plane, &pt1, &pt2, &pt3);
    D3DXMatrixReflect(&mReflectInMirror, &plane);
    pD3DDevice->SetTransform(D3DTS_WORLD, &mReflectInMirror);

    pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CW);

    pD3DDevice->SetClipPlane(0, *D3DXPlaneFromPoints(&plane, &pt2, &pt1, &vEyePt));
    pD3DDevice->SetClipPlane(1, *D3DXPlaneFromPoints(&plane, &pt4, &pt2, &vEyePt));
    pD3DDevice->SetClipPlane(2, *D3DXPlaneFromPoints(&plane, &pt3, &pt4, &vEyePt));
    pD3DDevice->SetClipPlane(3, *D3DXPlaneFromPoints(&plane, &pt1, &pt3, &vEyePt));
    pD3DDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, 
		D3DCLIPPLANE0 | D3DCLIPPLANE1 | D3DCLIPPLANE2 | D3DCLIPPLANE3);

    RenderObject();

    pD3DDevice->SetTransform(D3DTS_WORLD, &mWorld);
    pD3DDevice->SetRenderState(D3DRS_CLIPPLANEENABLE, 0x00);
    pD3DDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

    pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
    pD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
    pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

    pD3DDevice->SetStreamSource(0, pMirror, 0, sizeof(sVERTEX));
    pD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
    pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

    pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
}