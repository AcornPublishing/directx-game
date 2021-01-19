// MipMapTest.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "MipMapTest.h"
#include "../../common/dxfunc.h"
#include <d3d9types.h>
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

float fTime=0.0f;

struct sVertex
{
    D3DXVECTOR3 p;
    D3DXVECTOR3 n;
    static const DWORD FVF;
};

struct sVertexTeapot
{
    D3DXVECTOR3 p;
    D3DXVECTOR3 n;
    static const DWORD FVF;
};

ID3DXMesh* mTeapot;
D3DMATERIAL9 TeapotMat;
DWORD  dwSphereVerticesNum;

IDirect3DVertexBuffer9 * vbSphere;
IDirect3DCubeTexture9 * cubeTexture;

IDirect3DSurface9 *  pBackBuffer;

LPDIRECT3DTEXTURE9 *pMeshTextures;
D3DMATERIAL9 *pMeshMaterials;



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
	LoadString(hInstance, IDC_MIPMAPTEST, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_MIPMAPTEST);

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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_MIPMAPTEST);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_MIPMAPTEST;
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

   // Load teapot mesh
   DWORD dwNumMaterials = LoadMesh("teapot.x", pD3DDevice,
			   &mTeapot, &pMeshTextures,
			   "texture.bmp", &pMeshMaterials
			   );

   // Init Sphere
   DWORD dwSphereRings    = 20;
   DWORD dwSphereSegments = 20;
   dwSphereVerticesNum = 2 * dwSphereRings * (dwSphereSegments+1);

   // Create the vertex buffers
   if (FAILED (pD3DDevice->CreateVertexBuffer (dwSphereVerticesNum*sizeof(sVertex),
												           D3DUSAGE_WRITEONLY, 0, D3DPOOL_MANAGED, &vbSphere, NULL)))
   {
      return E_FAIL;
   }

    // Fill sphere
    sVertex* vert;
    vbSphere->Lock( 0, 0, (void**)&vert, 0 );

    float fDeltaRingAngle = ( D3DX_PI / dwSphereRings );
    float fDeltaSegAngle  = ( 2.0f * D3DX_PI / dwSphereSegments );

    D3DXVECTOR4 vT;
    for(int rings=0; rings < dwSphereRings; rings++)
    {
        float r0 = sinf( (rings+0) * fDeltaRingAngle );
        float r1 = sinf( (rings+1) * fDeltaRingAngle );
        float y0 = cosf( (rings+0) * fDeltaRingAngle );
        float y1 = cosf( (rings+1) * fDeltaRingAngle );

        for (int segs = 0; segs<dwSphereSegments+1; segs++)
        {
            float x0 = r0 * sinf(segs * fDeltaSegAngle);
            float z0 = r0 * cosf(segs * fDeltaSegAngle);
            float x1 = r1 * sinf(segs * fDeltaSegAngle);
            float z1 = r1 * cosf(segs * fDeltaSegAngle);

            (*vert).p = (*vert).n = D3DXVECTOR3(x0,y0,z0);
            vert++;

            (*vert).p = (*vert).n = D3DXVECTOR3(x1,y1,z1);
            vert++;
        }
    }

    vbSphere->Unlock();


   // Init material
   ZeroMemory( &TeapotMat, sizeof(D3DMATERIAL9) );
   TeapotMat.Diffuse.r = TeapotMat.Ambient.r = 0.5f;
   TeapotMat.Diffuse.g = TeapotMat.Ambient.g = 0.0f;
   TeapotMat.Diffuse.b = TeapotMat.Ambient.b = 0.5f;
   TeapotMat.Diffuse.a = TeapotMat.Ambient.a = 0.5f;

   // Init light
   D3DLIGHT9 light;
   ZeroMemory( &light, sizeof(D3DLIGHT9) );
   light.Type = D3DLIGHT_DIRECTIONAL;
   light.Diffuse.r = 1.0f;
   light.Diffuse.g = 1.0f;
   light.Diffuse.b = 1.0f;
   light.Position.x = -0.4f;
   light.Position.y = -0.4f;
   light.Position.z = 0.0f;
   light.Range = 1000.0f;
   D3DXVECTOR3 vecLightDirUnnormalized(-0.4, -0.4, 0.0);
   D3DXVec3Normalize((D3DXVECTOR3*)&light.Direction, &vecLightDirUnnormalized);
   pD3DDevice->SetLight(0, &light);
   pD3DDevice->LightEnable(0, TRUE);
   pD3DDevice->SetRenderState(D3DRS_AMBIENT, 0xff555555);

   // Set view matrix
   D3DMATRIX View = {
	   1, 0, 0, 0,
	   0, 1, 0, 0,
	   0, 0, 1, 0,
	   0, 0, 4.5, 1,
   };
   
   pD3DDevice->SetTransform(D3DTS_VIEW, &View);

   pD3DDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO, &pBackBuffer);

   /////////////////////////////////////////////////////////////////////////////
   // Init Texture
   HRESULT hr;

   if (FAILED(hr = pD3DDevice->CreateCubeTexture (256, 0, D3DUSAGE_RENDERTARGET|D3DUSAGE_AUTOGENMIPMAP,
             D3DFMT_A8R8G8B8, D3DPOOL_DEFAULT , &cubeTexture, NULL)))
	   return hr;

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
		pD3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0, 0);
        
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
	D3DXMATRIXA16 matTeapotWorld;
	float fNear = 1.1f;
	float fFar  = 100.0f;

	fTime +=0.01;
	D3DXMatrixRotationY(&matTeapotWorld, fmodf(2.0f*fTime, D3DX_PI*2.0f));  
	pD3DDevice->SetTransform(D3DTS_WORLD, &matTeapotWorld);

	IDirect3DSurface9 *pTextureSurfaceLevel;
	
	for (int i=0; i<6; i++)
	{
		cubeTexture->GetCubeMapSurface( (D3DCUBEMAP_FACES)i,0, &pTextureSurfaceLevel);
		cubeTexture->Release();
		pD3DDevice->SetRenderTarget (0, pTextureSurfaceLevel);
        
		pD3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0, 255-40*i, 0), 1.0f, 0L);

        pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
		pD3DDevice->SetMaterial(&TeapotMat);
		
		LPDIRECT3DVERTEXBUFFER9 pVB;
		LPDIRECT3DINDEXBUFFER9 pIB;
		
		mTeapot->GetVertexBuffer(&pVB);
		mTeapot->GetIndexBuffer(&pIB);
		DWORD numVertices = mTeapot->GetNumVertices();
		DWORD numFaces = mTeapot->GetNumFaces();
		
		pD3DDevice->SetFVF(mTeapot->GetFVF());
		pD3DDevice->SetStreamSource(0, pVB, 0, sizeof(sVertexTeapot));
		pD3DDevice->SetIndices(pIB);
		pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, numVertices, 0, numFaces);
		
		if (pVB){pVB->Release; pVB=NULL;}
		if (pIB){pIB->Release; pIB=NULL;}
      }

   pD3DDevice->SetRenderTarget (0, pBackBuffer);
   pD3DDevice->Clear (0L, NULL, D3DCLEAR_TARGET, 0xFFFFFFFF, 1.0f, 0L);
   pD3DDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

   pD3DDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3);   
   pD3DDevice->SetTexture(0, cubeTexture);

   pD3DDevice->SetStreamSource(0, vbSphere, 0, sizeof(sVertex));
   pD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_TEXCOORDSIZE3(1));
   
   pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, dwSphereVerticesNum-2);
   
   pD3DDevice->SetTexture(0, NULL);

}