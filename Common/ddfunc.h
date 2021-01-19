#ifndef _DDFUNC_H_
#define _DDFUNC_H_

#include "ddraw.h"

// DirectDraw 초기화 함수
HRESULT DXDDInit(IDirectDraw7 **ppiDD, 
				IDirectDrawSurface7 **primsurf,
				IDirectDrawSurface7 **backsurf,
                HWND hWnd, 
				DWORD iWidth,
				DWORD iHeight,
				WORD iColors,
				BOOL *bFullScreen
				);

BOOL ClearSurface(LPDIRECTDRAWSURFACE7 surf, int iColor);
IDirectDrawSurface7 *LoadBMPToSurface(LPCTSTR filename, IDirectDraw7 *ppiDD);
IDirectDrawSurface7 *LoadBMPToSurface(IDirectDrawSurface7 **ddsurf, LPCTSTR filename,
		IDirectDraw7 *ppiDD);
IDirectDrawSurface7 *LoadBMPToSurfaceFast(LPCTSTR filename, IDirectDraw7 *ppiDD);
IDirectDrawSurface7 *LoadBMPToSurfaceFast(IDirectDrawSurface7 **ddsurf, LPCTSTR filename, 
		IDirectDraw7 *ppiDD);
void DrawLine(IDirectDrawSurface7* surf, WORD X1, WORD Y1, 
			  WORD X2, WORD Y2, BYTE r, BYTE g, BYTE b);
void Circle(IDirectDrawSurface7* surf, WORD X, WORD Y, 
			  WORD R, WORD lines, BYTE r, BYTE g, BYTE b);

#endif
