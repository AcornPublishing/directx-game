#include "dxfunc.h"


// Direct3D �ʱ�ȭ�Լ�
HRESULT DX3DInit(IDirect3D9 **ppiD3D9, 
                IDirect3DDevice9 **ppiD3DDevice9,
                HWND hWnd, 
				DWORD iWidth,
				DWORD iHeight,
				BOOL bFullScreen)
{
  // �ʱ�ȭ
  if((*ppiD3D9 = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
    return E_FAIL;

  // ���� �Ű����� ä���
  D3DPRESENT_PARAMETERS d3dpp;
  ZeroMemory(&d3dpp, sizeof(d3dpp));

  d3dpp.BackBufferWidth = iWidth;
  d3dpp.BackBufferHeight = iHeight;

  // ��üȭ�� ��� ��û�ϱ�
  int  iRes;
  if (!bFullScreen)
      iRes=MessageBox(hWnd, "Use fullscreen mode?", "Screen", MB_YESNO | MB_ICONQUESTION);
  else
	  iRes = IDYES;


  if(iRes == IDYES)
  {
      //////////////////////////////////////////////////////////
	  // ��üȭ�� ���
      //////////////////////////////////////////////////////////
	  // ��üȭ�� ��� �Ű����� ����
      d3dpp.BackBufferFormat = D3DFMT_R5G6B5;
	  d3dpp.SwapEffect       = D3DSWAPEFFECT_FLIP;
	  d3dpp.Windowed         = FALSE;
      d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
      d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
  } 
  else 
  {
      //////////////////////////////////////////////////////////
	  // â���
      //////////////////////////////////////////////////////////
	  RECT wndRect;
      RECT clientRect;
	  GetWindowRect(hWnd, &wndRect);
      GetClientRect(hWnd, &clientRect);	  

	  iWidth  = iWidth + (wndRect.right-wndRect.left)  - (clientRect.right-clientRect.left);
	  iHeight = iHeight + (wndRect.bottom-wndRect.top) - (clientRect.bottom-clientRect.top);

	  MoveWindow(hWnd, wndRect.left, wndRect.top, iWidth, iHeight, TRUE);

	  // �ȼ����� ���ϱ�
	  D3DDISPLAYMODE d3ddm;
	  (*ppiD3D9)->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);

	  // �Ű����� ����
	  d3dpp.BackBufferFormat = d3ddm.Format;
	  d3dpp.SwapEffect       = D3DSWAPEFFECT_DISCARD;
	  d3dpp.Windowed         = TRUE;
  }
  
  DWORD Flags= D3DCREATE_MIXED_VERTEXPROCESSING;

  // 3D ��ġ �����
  HRESULT hRes;
  if(FAILED(hRes = (*ppiD3D9)->CreateDevice(
	  D3DADAPTER_DEFAULT, 
	  D3DDEVTYPE_HAL, hWnd, Flags, 
	  &d3dpp, ppiD3DDevice9)))
	  return hRes;

  // ��������
  float Aspect = (float)d3dpp.BackBufferWidth / (float)d3dpp.BackBufferHeight;
  D3DXMATRIX matProjection;
  D3DXMatrixPerspectiveFovLH(&matProjection, D3DX_PI/4.0f, Aspect, 0.1f, 2000.0f);
  (*ppiD3DDevice9)->SetTransform(D3DTS_PROJECTION, &matProjection);

  (*ppiD3DDevice9)->SetRenderState(D3DRS_LIGHTING, FALSE);

  return S_OK;
}

HRESULT DX3DInitZ(IDirect3D9 **ppiD3D9, 
                IDirect3DDevice9 **ppiD3DDevice9,
                HWND hWnd, 
				DWORD iWidth,
				DWORD iHeight,
				BOOL bFullScreen)
{
  // �ʱ�ȭ 
  if((*ppiD3D9 = Direct3DCreate9(D3D_SDK_VERSION)) == NULL)
    return E_FAIL;

  // ���� �Ű����� ä���
  D3DPRESENT_PARAMETERS d3dpp;
  ZeroMemory(&d3dpp, sizeof(d3dpp));

  d3dpp.BackBufferWidth = iWidth;
  d3dpp.BackBufferHeight = iHeight;
  d3dpp.AutoDepthStencilFormat = D3DFMT_D16;
  d3dpp.EnableAutoDepthStencil = TRUE;
 
  // ��üȭ�� ��� ��û�ϱ�
  int  iRes;
  if (!bFullScreen)
      iRes=MessageBox(hWnd, "Use fullscreen mode?", "Screen", MB_YESNO | MB_ICONQUESTION);
  else
	  iRes = IDYES;

  if(iRes == IDYES)
  {
      //////////////////////////////////////////////////////////
	  // ��üȭ�� ���
      //////////////////////////////////////////////////////////
	  // ��üȭ�� ��� �Ű����� ����
      d3dpp.BackBufferFormat = D3DFMT_R5G6B5;
	  d3dpp.SwapEffect       = D3DSWAPEFFECT_FLIP;
	  d3dpp.Windowed         = FALSE;
      d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
      d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;
  } 
  else 
  {
      //////////////////////////////////////////////////////////
	  // â���
      //////////////////////////////////////////////////////////
	  RECT wndRect;
      RECT clientRect;
	  GetWindowRect(hWnd, &wndRect);
      GetClientRect(hWnd, &clientRect);	  

	  iWidth  = iWidth + (wndRect.right-wndRect.left)  - (clientRect.right-clientRect.left);
	  iHeight = iHeight + (wndRect.bottom-wndRect.top) - (clientRect.bottom-clientRect.top);

	  MoveWindow(hWnd, wndRect.left, wndRect.top, iWidth, iHeight, TRUE);

	  // �ȼ����� ���ϱ�
	  D3DDISPLAYMODE d3ddm;
	  (*ppiD3D9)->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddm);

	  // �Ű����� ����
	  d3dpp.BackBufferFormat = d3ddm.Format;
	  d3dpp.SwapEffect       = D3DSWAPEFFECT_DISCARD;
	  d3dpp.Windowed         = TRUE;
  }
  
  DWORD Flags= D3DCREATE_MIXED_VERTEXPROCESSING;

  // 3D ��ġ �����
  HRESULT hRes;
  if(FAILED(hRes = (*ppiD3D9)->CreateDevice(
	  D3DADAPTER_DEFAULT, 
	  D3DDEVTYPE_HAL, hWnd, Flags, 
	  &d3dpp, ppiD3DDevice9)))
	  return hRes;

  // ��������
  float Aspect = (float)d3dpp.BackBufferWidth / (float)d3dpp.BackBufferHeight;
  D3DXMATRIX matProjection;
  D3DXMatrixPerspectiveFovLH(&matProjection, D3DX_PI/4.0f, Aspect, 2.0f, 1000.0f);
  (*ppiD3DDevice9)->SetTransform(D3DTS_PROJECTION, &matProjection);

  (*ppiD3DDevice9)->SetRenderState(D3DRS_LIGHTING, FALSE);

  return S_OK;
}

DWORD LoadMesh (char *filename, IDirect3DDevice9 *ppiD3DDevice9,
			   ID3DXMesh **ppMesh,
			   LPDIRECT3DTEXTURE9 **pMeshTextures,
			   char *texturefilename, 
			   D3DMATERIAL9 **pMeshMaterials
			   )
{
	LPD3DXBUFFER pD3DXMtrlBuffer;
	DWORD dwNumMaterials;

	D3DXLoadMeshFromX(filename, D3DXMESH_SYSTEMMEM,
		ppiD3DDevice9, NULL, &pD3DXMtrlBuffer, NULL, &dwNumMaterials, ppMesh);

	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();

	// ������ �ؽ�ó �ʱ�ȭ
	(*pMeshTextures) = new LPDIRECT3DTEXTURE9[dwNumMaterials];
	(*pMeshMaterials) = new D3DMATERIAL9[dwNumMaterials];

	for( DWORD i=0; i<dwNumMaterials; i++ )
	{
		// ��������
		(*pMeshMaterials)[i] = d3dxMaterials[i].MatD3D;

		// �ؽ�ó ����
		if( FAILED(D3DXCreateTextureFromFile(ppiD3DDevice9, 
			d3dxMaterials[i].pTextureFilename, &(*pMeshTextures)[i])))
			if( FAILED(D3DXCreateTextureFromFile(ppiD3DDevice9, 
				texturefilename, &(*pMeshTextures)[i])))
				(*pMeshTextures)[i] = NULL;
	}
	return dwNumMaterials;
}

bool LoadTexture(DWORD* text_buf, int iWidth, int iHeight, char* filename)
{
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

	if (bmpinfohdr.biCompression!=BI_RGB)
		return 0;

	int iImageSize=bmpinfohdr.biSizeImage;
	if (iImageSize==0)
		iImageSize=((iWidth*4+3) & ~3)*iHeight;

	BYTE* buf=new BYTE[iImageSize];
	if(!ReadFile(hFile, buf, iImageSize, &dwRead, NULL))
		return FALSE;

    if (bmpinfohdr.biBitCount!=24)
		return FALSE;

	int bytesgiven=(iWidth*3+3) & ~3;
	DWORD* btSurf = text_buf;
	BYTE* imagebits = (BYTE*)(&buf[(iHeight-1)*bytesgiven]);

	for (int i=0; i<iHeight; i++)
	{
		RGBTRIPLE* tlImage=(RGBTRIPLE*)imagebits;
		for (int p=0; p<iWidth; p++)
		{
			*btSurf = (DWORD)((tlImage->rgbtBlue) | (tlImage->rgbtGreen << 8) | 
				           (tlImage->rgbtRed << 16) | (0xFF << 24));
			tlImage++;
			btSurf++;
		}
		imagebits -= bytesgiven;
	}

	delete [] buf;
	return TRUE;
}