#ifndef _DXFUNC_H_
#define _DXFUNC_H_

#include "d3d9.h"
#include "d3dx9.h"

// Direct3D 초기화 함수를 선언한다.
HRESULT DX3DInit(IDirect3D9 **ppiD3D9, 
                IDirect3DDevice9 **ppiD3DDevice9,
                HWND hWnd, 
				DWORD iWidth,
				DWORD iHeight,
				BOOL bFullScreen
				);

HRESULT DX3DInitZ(IDirect3D9 **ppiD3D9, 
                IDirect3DDevice9 **ppiD3DDevice9,
                HWND hWnd, 
				DWORD iWidth,
				DWORD iHeight,
				BOOL bFullScreen
				);

DWORD LoadMesh(char *filename, IDirect3DDevice9 *ppiD3DDevice9,
			   ID3DXMesh **ppMesh,
			   LPDIRECT3DTEXTURE9 **pMeshTextures,
			   char *texturefilename, 
			   D3DMATERIAL9 **pMeshMaterials
			   );

bool LoadTexture(DWORD* text_buf, int iWidth, int iHeight, 
				 char* filename);

#endif
