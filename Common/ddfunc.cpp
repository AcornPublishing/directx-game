#include "ddraw.h"
#include <math.h>


////////////////////////////////
//   ���ǽ��� �Ұ��ϴ� �Լ�   //
////////////////////////////////
BOOL ClearSurface(LPDIRECTDRAWSURFACE7 surf, int iColor)
{
	if (surf==0)
		return FALSE;

	DDBLTFX bltfx;
	ZeroMemory(&bltfx, sizeof(bltfx));
	bltfx.dwSize = sizeof(bltfx);
	bltfx.dwFillColor = iColor;

	HRESULT r;
	r=surf->Blt(0, 0, 0, DDBLT_COLORFILL | DDBLT_WAIT, &bltfx );
	if (r==DD_OK)
		return TRUE;
	else
		return FALSE;
}

////////////////////////////////////
//   Direct3D�� �ʱ�ȭ�ϴ� �Լ�   //
////////////////////////////////////
HRESULT DXDDInit(IDirectDraw7 **ppiDD, 
				IDirectDrawSurface7 **primsurf,
				IDirectDrawSurface7 **backsurf,
                HWND hWnd, 
				DWORD iWidth,
				DWORD iHeight,
				WORD iColors,
				BOOL *bFullScreen)
{   
	// ��üȭ�� ��带������� ���´�.
    int  iRes;
	if ((*bFullScreen)==FALSE)
		iRes=MessageBox(hWnd, "Use fullscreen mode?", "Screen", MB_YESNO | MB_ICONQUESTION);
	else
		iRes = IDYES;

	HRESULT hr; 
	hr = DirectDrawCreateEx(NULL, (VOID**)ppiDD, IID_IDirectDraw7, NULL);
    if(FAILED(hr))
        return 0;

	if(iRes == IDYES)
	{      
		//////////////////////////////////////////////////////////
		// ��üȭ����
		//////////////////////////////////////////////////////////
		*bFullScreen=FALSE;

		SetWindowLong(hWnd, GWL_STYLE, WS_POPUP);
		(*ppiDD)->SetCooperativeLevel(hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN);
		(*ppiDD)->SetDisplayMode(iWidth, iHeight, iColors, 0, 0);
	}
	else
	{
        //////////////////////////////////////////////////////////
		// â���
		////////////////////////////////////////////////////////// 
		*bFullScreen=TRUE;

		RECT wndRect;
		RECT clientRect;
		GetWindowRect(hWnd, &wndRect);
		GetClientRect(hWnd, &clientRect);	  
		int iWinWidth  = iWidth + (wndRect.right-wndRect.left)  - (clientRect.right-clientRect.left);
		int iWinHeight = iHeight + (wndRect.bottom-wndRect.top) - (clientRect.bottom-clientRect.top);
		
		MoveWindow(hWnd, wndRect.left, wndRect.top, iWinWidth, iWinHeight, TRUE);

		(*ppiDD)->SetCooperativeLevel(hWnd, DDSCL_NORMAL);
	}

	DDSURFACEDESC2 desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(desc);

	if(iRes == IDYES)
	{      
       desc.dwFlags =  DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
       desc.dwBackBufferCount = 1;
       desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
	}
	else
	{
       desc.dwFlags =  DDSD_CAPS;
       desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	}

	hr=(*ppiDD)->CreateSurface(&desc, primsurf, 0);
	if (hr!=DD_OK)
		return 0;

	if(iRes == IDYES)
	{      
        DDSCAPS2 surfcaps;
        ZeroMemory(&surfcaps, sizeof(surfcaps));
		surfcaps.dwCaps = DDSCAPS_BACKBUFFER;
		hr=(*primsurf)->GetAttachedSurface(&surfcaps, backsurf);
		if (hr!=DD_OK)
			return 0;
		
        ClearSurface(*primsurf, 0);
		ClearSurface(*backsurf, 0);
	}
	else
	{
		ZeroMemory(&desc, sizeof(desc));
		desc.dwSize = sizeof(desc);
		desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		desc.dwWidth = iWidth;
		desc.dwHeight = iHeight;
		desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		
		// ���ǽ�����
		HRESULT r=(*ppiDD)->CreateSurface(&desc, backsurf, 0);
		if (r!=DD_OK)		
			return 0; // ���ǽ� ���� �� ���� �߻�
	}


	return S_OK;
}

///////////////////////////////////////////
//    ���ǽ��� �̹����� �ε��ϴ� �Լ�    //
///////////////////////////////////////////
IDirectDrawSurface7 *LoadBMPToSurface(LPCTSTR filename,
		IDirectDraw7 *ppiDD)
{
	IDirectDrawSurface7 *surf;
	HANDLE hBmp;
	BITMAP bm;

	// ���Ͽ��� �̹����� �ε��Ѵ�.
	hBmp = LoadImage(0, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	if (hBmp==0)
		return 0;
	GetObject(hBmp, sizeof(bm), &bm);

	// ���ǽ� ������ ���� ����ü ����
	DDSURFACEDESC2 desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(desc);
	desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
	desc.dwWidth = bm.bmWidth;
	desc.dwHeight = bm.bmHeight;
	desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;

	// ���ǽ� ����
	HRESULT r=ppiDD->CreateSurface( &desc, &surf, 0 );
	if (r!=DD_OK)		
		return 0; // ���ǽ������� ���� �߻�

	// �̹��� ������ ���ǽ��� ����
	HDC hdcImage;  
	hdcImage = CreateCompatibleDC(0);
	if (hdcImage == 0)
		return 0;
	SelectObject(hdcImage, hBmp);

	HDC hdc;

	surf->GetDC(&hdc);
	StretchBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcImage, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
    surf->ReleaseDC(hdc);

	DeleteDC(hdcImage);
	DeleteObject(hBmp);
	return surf;
}

/////////////////////////////////////////
//    ���ǽ��� �̹����� �ε��ϴ� �Լ�  //
/////////////////////////////////////////
IDirectDrawSurface7 *LoadBMPToSurface(IDirectDrawSurface7 **ddsurf, LPCTSTR filename,
		IDirectDraw7 *ppiDD)
{
	IDirectDrawSurface7 *surf;
	surf=*ddsurf;
	HANDLE hBmp;
	BITMAP bm;

	// ���Ͽ��� �̹����� �ε��Ѵ�.
	hBmp = LoadImage(0, filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_CREATEDIBSECTION);
	if (hBmp==0)
		return 0;
	GetObject(hBmp, sizeof(bm), &bm);

	if (surf==NULL)
	{
		// ���ǽ� ������ ���� ����ü ����
		DDSURFACEDESC2 desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.dwSize = sizeof(desc);
		desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		desc.dwWidth = bm.bmWidth;
		desc.dwHeight = bm.bmHeight;
		desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;

        // ���ǽ� ����
		HRESULT r=ppiDD->CreateSurface( &desc, &surf, 0 );
		if (r!=DD_OK)		
			return 0; // ���ǽ� ������ ���� �߻�
	}
        
	// �̹��� ������ ���ǽ��� ����
	HDC hdcImage;  
	hdcImage = CreateCompatibleDC(0);
	if (hdcImage == 0)
		return 0;
	SelectObject(hdcImage, hBmp);
        
	HDC hdc;
        
	surf->GetDC(&hdc);
	StretchBlt(hdc, 0, 0, bm.bmWidth, bm.bmHeight, hdcImage, 0, 0, bm.bmWidth, bm.bmHeight, SRCCOPY);
	surf->ReleaseDC(hdc);
        
	DeleteDC(hdcImage);
	DeleteObject(hBmp);
	return surf;
}

///////////////////////////////
//      ���� �׸��� �Լ�     //
///////////////////////////////
void DrawLine(IDirectDrawSurface7* surf, WORD X1, WORD Y1, 
			  WORD X2, WORD Y2, BYTE r, BYTE g, BYTE b)
{
	if (!surf)
		return;

	WORD c = (WORD)(b/8 | (g/4 << 5) | (r/8 << 11));


    int NS;
	if (Y2<Y1)
	{
		NS=Y1;
		Y1=Y2;
		Y2=NS;

		NS=X1;
		X1=X2;
		X2=NS;
	}

	// ���ǽ� ���
	DDSURFACEDESC2 desc;
	ZeroMemory(&desc, sizeof(desc));
	desc.dwSize = sizeof(desc);
    
	if (surf->Lock(0, &desc, DDLOCK_WAIT, 0)==DD_OK)
	{
		BYTE* dst = (BYTE *)desc.lpSurface;
		int DX=abs(X2-X1);
		int DY=abs(Y2-Y1);
		int iOffset=Y1*desc.lPitch+X1*2;

		if (X1<=X2) 
		{
			// X�� �� ���� ���� ��
			if (DX>DY)
			{
				NS=DX % 2;
				for (int i=0; i<=DX; i++)
				{
					*(WORD *)(dst+iOffset)=c;
					iOffset=iOffset+2;
					NS=NS-DY;
					if (NS<0)
					{
						iOffset=iOffset+desc.lPitch;
						NS=NS+DX;
					}
				}
			}
			else
			{
				// Y�� �� ���� ���� ��
				NS=DY % 2;
				for (int i=0; i<DY; i++)
				{
					*(WORD *)(dst+iOffset)=c;
					iOffset=iOffset+desc.lPitch;
					NS=NS-DX;
					if (NS<0)
					{
						iOffset=iOffset+2;
						NS=NS+DY;
					}
				}
			}
		}
		else
		{
			// X�� �� ���� ���� ��
			if (DX>DY)
			{
				NS=DX % 2;
				for (int i=0; i<=DX; i++)
				{
					*(WORD *)(dst+iOffset)=c;
					iOffset=iOffset-2;
					NS=NS-DY;
					if (NS<0)
					{
						iOffset=iOffset+desc.lPitch;
						NS=NS+DX;
					}
				}
			}
			else
			{
				// Y�� �� �������Ҷ�
				NS=DY % 2;
				for (int i=0; i<DY; i++)
				{
					*(WORD *)(dst+iOffset)=c;
					iOffset=iOffset+desc.lPitch;
					NS=NS-abs(DX);
					if (NS<0)
					{
						iOffset=iOffset-2;
						NS=NS+DY;
					}
				}
			}
		}
		surf->Unlock(0);
	}
}

///////////////////////////////////////////////////
// ��Ʈ�� �̹����� ���ǽ��� ������ �ε��ϴ� �Լ� //
///////////////////////////////////////////////////
IDirectDrawSurface7 *LoadBMPToSurfaceFast(IDirectDrawSurface7 **ddsurf, LPCTSTR filename,
		IDirectDraw7 *ppiDD)
{
	int iImageWidth, iImageHeight;

    HANDLE hFile = CreateFile(filename, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);        
	if(hFile == INVALID_HANDLE_VALUE) 
		return FALSE;

	BITMAPFILEHEADER bmpfilehdr;  
	DWORD dwRead;
	if(!ReadFile(hFile, &bmpfilehdr, sizeof(bmpfilehdr), &dwRead, NULL))
		return FALSE;
    
	char* ptr=(char*)&bmpfilehdr.bfType;
	if (*ptr!='B' || *++ptr!='M')
		return FALSE;

	BITMAPINFOHEADER bmpinfohdr;
	if(!ReadFile(hFile, &bmpinfohdr, sizeof(bmpinfohdr), &dwRead, NULL))
		return FALSE;

	iImageWidth=bmpinfohdr.biWidth;
	iImageHeight=bmpinfohdr.biHeight;

	if (bmpinfohdr.biCompression!=BI_RGB)
		return 0;

	// ���ǽ� ó���� ����
	IDirectDrawSurface7 *surf=*ddsurf;
	DDSURFACEDESC2 desc;
	HRESULT hRes;

	if (surf==NULL)
	{
		// ���ǽ� ������ ����ü����
		ZeroMemory(&desc, sizeof(desc));
		desc.dwSize = sizeof(desc);
		desc.dwFlags = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		desc.dwWidth = iImageWidth;
		desc.dwHeight = iImageHeight;
		desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;

		// ���ǽ� ����
		hRes=ppiDD->CreateSurface( &desc, &surf, 0 );
		if (hRes!=DD_OK)
			return FALSE;
	}

	int iImageSize=bmpinfohdr.biSizeImage;
	if (iImageSize==0)
		iImageSize=((iImageWidth*(bmpinfohdr.biBitCount/8)+3) & ~3)*iImageHeight;

	BYTE* buf=new BYTE[iImageSize];
	if(!ReadFile(hFile, buf, iImageSize, &dwRead, NULL))
		return FALSE;

    if (bmpinfohdr.biBitCount!=24)
		return FALSE;

	// 24 ��Ʈ ��Ʈ�� �̹����� ���ǽ��� ����
	ZeroMemory( &desc, sizeof(desc) );
	desc.dwSize = sizeof(desc);
	hRes=surf->Lock( 0, &desc, DDLOCK_WAIT | DDLOCK_WRITEONLY, 0 );
	if (hRes!=DD_OK)
		return FALSE;

	int bytesrequired=iImageWidth*3;
	int bytesgiven=(bytesrequired+3) & ~3;
	BYTE* btSurf = (BYTE*)desc.lpSurface;
	BYTE* imagebits = (BYTE*)(&buf[(iImageHeight-1)*bytesgiven]);

	for (int i=0; i<iImageHeight; i++)
	{
		WORD* wdSurf=(WORD*)btSurf;
		RGBTRIPLE* tlImage=(RGBTRIPLE*)imagebits;
		for (int p=0; p<iImageWidth; p++)
		{
			*wdSurf = (WORD)((tlImage->rgbtBlue / 8) | ((tlImage->rgbtGreen / 4) << 5) | 
				           ((tlImage->rgbtRed / 8) << 11));
			tlImage++;
			wdSurf++;
		}
		btSurf += desc.lPitch;
		imagebits -= bytesgiven;
	}
	surf->Unlock(0);

	delete [] buf;
	return surf;
}
