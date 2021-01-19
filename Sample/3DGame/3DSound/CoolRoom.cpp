// CoolRoom.cpp : Room Example
//

#ifndef DIRECTINPUT_VERSION
#define DIRECTINPUT_VERSION 0x0800
#endif

#include <windows.h>
#include "d3d9.h"
#include "d3dx9.h"
#include "..\..\common\dxfunc.h"
#include "GraphEngine.h"

// Global Variables:
char szWindowClass[] = "Room Example";
char szTitle[] = "Room Example";

// Direct3D objects
IDirect3D9 *pD3D = NULL;
IDirect3DDevice9 *pD3DDevice = NULL;

CGraphEngine* ge;

int iWidth=800;
int iHeight=600; 

// Forward declarations of functions included in this code module:
int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

bool Init(HWND hWnd);
void GraphEngine();

int PASCAL WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR szCmdLine, int nCmdShow)
 {
  WNDCLASSEX wcex;
  MSG        msg;
  HWND       hWnd;

  CoInitialize(NULL);

  // Register window class
  wcex.cbSize        = sizeof(wcex);
  wcex.style         = CS_CLASSDC;
  wcex.lpfnWndProc   = (WNDPROC)WndProc;
  wcex.cbClsExtra    = 0;
  wcex.cbWndExtra    = 0;
  wcex.hInstance     = hInst;
  wcex.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
  wcex.hCursor       = LoadCursor(NULL, IDC_ARROW);
  wcex.hbrBackground = NULL;
  wcex.lpszMenuName  = NULL;
  wcex.lpszClassName = szWindowClass;
  wcex.hIconSm       = LoadIcon(NULL, IDI_APPLICATION);
  if(!RegisterClassEx(&wcex))
    return FALSE;

  // Create the main window
  hWnd = CreateWindow(szWindowClass, szTitle, WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX,
              CW_USEDEFAULT, CW_USEDEFAULT, iWidth, iHeight, NULL, NULL, hInst, NULL);
  if(!hWnd)
    return FALSE;
  ShowWindow(hWnd, SW_NORMAL);
  UpdateWindow(hWnd);

  if(Init(hWnd) == TRUE) 
  {
	  while (true) 
	  {
		  if (PeekMessage(&msg, NULL, NULL, NULL, PM_REMOVE)) 
		  {
			  TranslateMessage(&msg);
			  DispatchMessage(&msg);
			  if (msg.message == WM_QUIT) break;
		  }
		  GraphEngine();
	  }
  }

  if (pD3DDevice) {pD3DDevice= NULL; pD3DDevice=NULL;}
  if (pD3D) {pD3D= NULL; pD3D=NULL;}

  CoUninitialize();

  return 0;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
  switch(message) {
    case WM_DESTROY:
      PostQuitMessage(0);
      break;

    default:
      return DefWindowProc(hWnd, message, wParam, lParam);
  }

  return 0;
}

bool Init(HWND hWnd)
{
  if (DX3DInitZ(&pD3D, &pD3DDevice, hWnd, iWidth, iHeight, FALSE)!=S_OK)
  {
	  MessageBox(hWnd, "DirectX Initialize Error", "Error", MB_OK);
	  return FALSE;
  }

  ge = new CGraphEngine(pD3DDevice, hWnd);
  
  return TRUE;
}

void GraphEngine()
{
  if (SUCCEEDED(pD3DDevice->BeginScene())) 
  {	  
      pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER , D3DCOLOR_XRGB(255,255,255), 1.0f, 0);
	  ge->RenderScene();
	  pD3DDevice->EndScene();
  }
  
  pD3DDevice->Present(NULL, NULL, NULL, NULL);
}
