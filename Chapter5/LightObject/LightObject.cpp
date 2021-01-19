// LightObject.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "LightObject.h"
#include "../../common/dxfunc.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Direct3D objects
IDirect3D9 *pD3D = NULL;
IDirect3DDevice9 *pD3DDevice = NULL;

IDirect3DVertexBuffer9 * iVortexBuffer;
IDirect3DIndexBuffer9 * iIndexTexture;
IDirect3DVolumeTexture9 * iTexture;

struct sVertex
{
  float x, y, z;
  DWORD color;
};

#define NUM_SLICES 64
#define SLICE_SIZE 32
#define CENTER 5.0f
#define ROOT_THREE 1.5f
#define NUM_SHELLS 36
#define NUM_CELLS 12
#define NUM_CELLS2 NUM_CELLS*NUM_CELLS
#define NEAR_SHELL (CENTER - ROOT_THREE)

const int iVertsNum = 24;
const int iIdxNum = 36;
const int iPointsNum = 12;

int iWidth=800;
int iHeight=600; 

DWORD iTotalVerts;
WORD  iTotalIndices;
float iFOV = D3DX_PI/4;
float fViewAngle = 0.0f;

 // Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
void DrawScene();

void CreateQuad()
{
	DWORD r, g, b, a;
	HRESULT hr;
	hr = pD3DDevice->CreateVolumeTexture(32, 32, 32, 1, 0, D3DFMT_A8R8G8B8,
		D3DPOOL_MANAGED, &iTexture, 0);

    if (FAILED(hr))
        return;

	D3DLOCKED_BOX lBox;
	hr = iTexture->LockBox(0, &lBox, 0, 0);
	if (FAILED(hr))
		return;

    float du, dv, dw;

	for (UINT i=0; i < 32; i++)
	{
		BYTE* pSliceStart = (BYTE*)lBox.pBits;
		for (UINT j=0; j< 32; j++)
		{
			for (UINT z=0; z<32; z++)
			{
				if ((i==0) || (i==31) || (j==0) || (j==31) || (z==0) || (z==31))
				{
					r = 0;
					g = 0;
					b = 0;
					a = 0;
				}
				else
				{
					r = (BYTE) ((i/31.0f)*255.0f);
					g = (BYTE) ((j/31.0f)*255.0f);
					b = (BYTE) ((z/31.0f)*255.0f);
					a = 0x00;
				}
				((DWORD*)lBox.pBits)[z] = D3DCOLOR_ARGB(a, r, g, b);
			}
			lBox.pBits = (BYTE*)lBox.pBits + lBox.RowPitch;
		}

		lBox.pBits = pSliceStart + lBox.SlicePitch;
		iTexture->UnlockBox( 0 );
	}

	iTotalVerts    = NUM_SHELLS * (NUM_CELLS + 1) * (NUM_CELLS + 1);
	iTotalIndices  = NUM_SHELLS * NUM_CELLS2 * 6;
    
	if (FAILED(pD3DDevice->CreateVertexBuffer(iTotalVerts*sizeof(sVertex), 
		   D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_DIFFUSE, D3DPOOL_MANAGED, 
		   &iVortexBuffer, 0)))
		return;

	if (FAILED(pD3DDevice->CreateIndexBuffer(iTotalIndices*sizeof(WORD), 
		   D3DUSAGE_WRITEONLY, D3DFMT_INDEX16, 
		   D3DPOOL_MANAGED, &iIndexTexture,0)))
 		return;
	
	sVertex* pVertices;
	float len, delta;
	int x, y;
	int Vert = 0;
	
	// Vertex buffer
	iVortexBuffer->Lock( 0, iTotalVerts*sizeof(sVertex), (void**)&pVertices, 0 );
	dw = NEAR_SHELL;
    
	for(int i=0; i<NUM_SHELLS; i++)
	{
		du = -tanf(iFOV/2.0f) * dw;
		delta = ((2.0f * tanf(iFOV/2.0f) * dw) / ((float)NUM_CELLS));

		for(x=0; x<=NUM_CELLS; x++)
		{
			dv = -tanf(iFOV/2.0f) * dw;
			for(y=0; y<=NUM_CELLS; y++)
			{
				len = sqrtf(du*du + dv*dv + dw*dw);

				pVertices[Vert].x = du / len;
				pVertices[Vert].y = dv / len;
				pVertices[Vert].z = dw / len;

				pVertices[Vert].x *= dw;
				pVertices[Vert].y *= dw;
				pVertices[Vert].z *= dw;				
				pVertices[Vert].z *= -1.0f;

				pVertices[Vert].color = D3DCOLOR_RGBA((int)(255*0.5f), (int)(0.5f), (int)(fabs(pVertices[Vert].z)), (int)(1.0f));

				Vert++;
				dv += delta;
			}
			du += delta;
		}
		dw += (2.0f * ROOT_THREE) / ((float)NUM_SHELLS);
	}
	iVortexBuffer->Unlock();

	// Texture buffer
	WORD* pIndex;
	WORD  Index;

	iIndexTexture->Lock(0, iTotalIndices*sizeof(WORD), (void**)&pIndex, 0);

	for (int i=NUM_SHELLS-1; i>=0; i--)
	{
		Index = (NUM_CELLS+1) * (NUM_CELLS+1) * i;

		for(x=0; x<NUM_CELLS; x++)
		{
			for(y=0; y<NUM_CELLS; y++)
			{
				*pIndex++ = (x + 0) * (NUM_CELLS+1) + (y + 0) + Index;
				*pIndex++ = (x + 0) * (NUM_CELLS+1) + (y + 1) + Index;
				*pIndex++ = (x + 1) * (NUM_CELLS+1) + (y + 1) + Index;

				*pIndex++ = (x + 0) * (NUM_CELLS+1) + (y + 0) + Index;
				*pIndex++ = (x + 1) * (NUM_CELLS+1) + (y + 1) + Index;
				*pIndex++ = (x + 1) * (NUM_CELLS+1) + (y + 0) + Index;
			}
		}
	}

   iIndexTexture->Unlock();

   D3DXMATRIX matProjection;
   D3DXMatrixPerspectiveFovRH(&matProjection, iFOV, 1.0f, 2.0f, 1000.0f );
   pD3DDevice->SetTransform(D3DTS_PROJECTION, &matProjection);
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
	LoadString(hInstance, IDC_LIGHTOBJECT, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}
    
	// 화면에 그릴 3D 객체 초기화
	CreateQuad();
    
	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_LIGHTOBJECT);

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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_LIGHTOBJECT);
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
      CW_USEDEFAULT, 0, CW_USEDEFAULT, CW_USEDEFAULT, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   if (DX3DInitZ(&pD3D, &pD3DDevice, hWnd, iWidth, iHeight, FALSE)!=S_OK)
   {
	   MessageBox(hWnd, "Error initializing DirectX", "Error", MB_OK);	   
	   return FALSE;
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
		pD3DDevice->Clear(1, (D3DRECT*)&r, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(0,0,0), 1.0, 0);

		pD3DDevice->BeginScene();
		DrawScene();
		pD3DDevice->EndScene();

		pD3DDevice->Present(NULL, NULL, 0, NULL);
		break;
	case WM_DESTROY:
		if (pD3DDevice) {pD3DDevice= NULL; pD3DDevice=NULL;}
		if (pD3D) {pD3D= NULL; pD3D=NULL;}
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


void DrawScene()
{
	fViewAngle += 0.01f;

	D3DXMATRIX MatTex, MatTexRot;
	D3DXMatrixRotationY(&MatTexRot, fViewAngle);
	
	D3DMATRIX View = {
		0.5, 0, 0, 0,
		0, 0.5, 0, 0, 
		0, 0, 0.5, 0, 
		0.5, 0.5, 0.5*CENTER, 1,
	};
    
	D3DXMatrixIdentity(&MatTex);
	D3DXMatrixMultiply(&MatTex, (D3DXMATRIX*)&View, &MatTexRot);

	pD3DDevice->SetTransform(D3DTS_TEXTURE0,  &MatTex);

	pD3DDevice->SetTexture(0, iTexture);
	
	pD3DDevice->SetStreamSource( 0, iVortexBuffer, 0, sizeof(sVertex) );
	pD3DDevice->SetIndices(iIndexTexture);
	
	pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_SELECTARG1);
	
	pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_TFACTOR);
	pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG2);
	pD3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB(0x22, 0x66, 0x33, 0x99));
    
	pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	pD3DDevice->SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA);
	pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);
    
	pD3DDevice->SetTextureStageState(0, D3DTEXTURESTAGESTATETYPE(13), D3DTADDRESS_CLAMP);
	pD3DDevice->SetTextureStageState(0, D3DTEXTURESTAGESTATETYPE(14), D3DTADDRESS_CLAMP);

	pD3DDevice->SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3);

	pD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_DIFFUSE);
	pD3DDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, 0, iTotalVerts, 0, NUM_CELLS2*2*NUM_SHELLS);

    pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, FALSE);
    pD3DDevice->SetTexture(0, NULL);
}