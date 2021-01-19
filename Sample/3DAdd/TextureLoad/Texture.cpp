// Texture.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Texture.h"
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
int TextureHeight=0;

struct sVertex
{
  float x, y, z;
  float u, v;
};

struct sVertexRHW
{
  float x, y, z, RHW;
  float u, v;
};

IDirect3DVertexBuffer9 * vQuadBuffer;
IDirect3DTexture9 *tQuadTexture;

IDirect3DTexture9 *tBackTexture;
IDirect3DVertexBuffer9 * vBackBuffer;
DWORD text[65536];

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
	LoadString(hInstance, IDC_TEXTURE, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_TEXTURE);

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
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_TEXTURE);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_TEXTURE;
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

   if (DX3DInit(&pD3D, &pD3DDevice, hWnd, iWidth, iHeight, FALSE)!=S_OK)
   {
	   MessageBox(hWnd, "Error Initializing DirectX", "Error", MB_OK);	   
	   return FALSE;
   }
   
   sVertex Quad[4];
   
   Quad[0].x=-0.5f;
   Quad[0].y=-0.5f;
   Quad[0].z=0.1f;
   Quad[0].u=0.0f;
   Quad[0].v=1.0f;

   Quad[1].x=-0.5f;
   Quad[1].y=0.5f;
   Quad[1].z=0.1f;
   Quad[1].u=0.0f;
   Quad[1].v=0.0f;

   Quad[2].x=0.5f;
   Quad[2].y=-0.5f;
   Quad[2].z=0.1f;
   Quad[2].u=1.0f;
   Quad[2].v=1.0f;

   Quad[3].x=0.5f;
   Quad[3].y=0.5f;
   Quad[3].z=0.1f;
   Quad[3].u=1.0f;
   Quad[3].v=0.0f;

   void * pBuf;
   if (FAILED(pD3DDevice->CreateVertexBuffer(sizeof(sVertex) * 4,
	   D3DUSAGE_WRITEONLY, D3DFVF_XYZ | D3DFVF_TEX1, D3DPOOL_DEFAULT, &vQuadBuffer, 0)))
	   return FALSE;
   if (FAILED(vQuadBuffer->Lock(0, sizeof(sVertex) * 4, &pBuf, 0)))
	   return FALSE;
   memcpy(pBuf, Quad, sizeof(sVertex) * 4);
   vQuadBuffer->Unlock();


   sVertexRHW BackQuad[4];

   BackQuad[0].x=0.0f;
   BackQuad[0].y=0.0f;
   BackQuad[0].z=0.2f;
   BackQuad[0].u=0.0f;
   BackQuad[0].v=0.0f;

   BackQuad[1].x=800.0f;
   BackQuad[1].y=0.0f;
   BackQuad[1].z=0.2f;
   BackQuad[1].u=1.0f;
   BackQuad[1].v=0.0f;

   BackQuad[2].x=800.0f;
   BackQuad[2].y=600.0f;
   BackQuad[2].z=0.2f;
   BackQuad[2].u=1.0f;
   BackQuad[2].v=1.0f;

   BackQuad[3].x=0.0f;
   BackQuad[3].y=600.0f;
   BackQuad[3].z=0.1f;
   BackQuad[3].u=0.0f;
   BackQuad[3].v=1.0f;

   // Create back quad buffer
   if (FAILED(pD3DDevice->CreateVertexBuffer(sizeof(sVertexRHW) * 4,
	   D3DUSAGE_WRITEONLY, D3DFVF_XYZRHW | D3DFVF_TEX1, D3DPOOL_DEFAULT, &vBackBuffer, 0)))
	   return FALSE;
   if (FAILED(vBackBuffer->Lock(0, sizeof(sVertexRHW) * 4, &pBuf, 0)))
	   return FALSE;
   memcpy(pBuf, BackQuad, sizeof(sVertexRHW) * 4);
   vBackBuffer->Unlock();

   D3DXCreateTextureFromFile(pD3DDevice, "back.bmp", &tBackTexture);

   pD3DDevice->CreateTexture(256, 256, 0, 0,
                                  D3DFMT_A8R8G8B8,
                                  D3DPOOL_MANAGED, 
								  &tQuadTexture, NULL);
   LoadTexture((DWORD*)&text, 255, 255, "tex.bmp");

/*   // Fill transparent color
   D3DLOCKED_RECT text_rect;
   if (FAILED(tQuadTexture->LockRect(0, &text_rect, 0, 0)))
	   return FALSE;

   BYTE* dst=(BYTE*)text_rect.pBits;
   for (int i=0; i<256; i++)
	   for (int j=0; j<256; j++)
	   {
		   *(DWORD *)(dst+i*text_rect.Pitch+j*4)=text[i*255+j];
	   }
   tQuadTexture->UnlockRect(0);
*/

   // Set view matrix
   D3DMATRIX View = {
	   1, 0, 0, 0,
	   0, 1, 0, 0,
	   0, 0, 1, 0,
	   0, 0, 2, 1,
   };
   pD3DDevice->SetTransform(D3DTS_VIEW, &View);
   
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
		pD3DDevice->Clear(1, (D3DRECT*)&r, D3DCLEAR_TARGET, 0x00FFFFFF, 0.0, 0);

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
  pD3DDevice->SetStreamSource(0, vBackBuffer, 0, sizeof(sVertexRHW));
  pD3DDevice->SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
  pD3DDevice->SetTexture(0, tBackTexture);

  pD3DDevice->DrawPrimitive(D3DPT_TRIANGLEFAN, 0, 2);

  float b = float((ViewAngle++)%0x1111)/24;

  D3DMATRIX World = {
    cos(b), -sin(b), 0, 0,
    sin(b), cos(b), 0, 0,
    0, 0, 1, 0,
    0, 0, 0, 1,
  };
  pD3DDevice->SetTransform(D3DTS_WORLD, &World);

  pD3DDevice->SetStreamSource(0, vQuadBuffer, 0, sizeof(sVertex));
  pD3DDevice->SetFVF(D3DFVF_XYZ | D3DFVF_TEX1);

  // Fill texture
  D3DLOCKED_RECT text_rect;
  if (FAILED(tQuadTexture->LockRect(0, &text_rect, 0, 0)))
   return;

  BYTE* dst=(BYTE*)text_rect.pBits;
  for (int i=0; i<TextureHeight; i++)
	  for (int j=0; j<256; j++)
		  if (text[i*255+j]!=0xFFFF00FF)
			  *(DWORD *)(dst+i*text_rect.Pitch+j*4)=text[i*255+j];

  for (int i=TextureHeight; i<256; i++)
	  for (int j=0; j<256; j++)
		  *(DWORD *)(dst+i*text_rect.Pitch+j*4)=0x00000000;
  tQuadTexture->UnlockRect(0);

  TextureHeight++;
  if (TextureHeight>255)
	  TextureHeight=0;

  pD3DDevice->SetTexture(0, tQuadTexture);

  pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
  pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
  pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);
 
  pD3DDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

  pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, 0);
}
