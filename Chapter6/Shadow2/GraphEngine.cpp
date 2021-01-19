#include "GraphEngine.h"

CGraphEngine::CGraphEngine(IDirect3DDevice9 *pD3DDevice)
{
	pDevice=pD3DDevice;

	ViewAngle=0;

	InitGraphObjects();
	
	D3DVERTEXELEMENT9 decl[]=
	{
		{0,0, D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},  
		{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},  
		D3DDECL_END()
	};
	pDevice->CreateVertexDeclaration(decl, &VertDecl);
}

void CGraphEngine::InitGraphObjects()
{
	// Load effects
	LoadEffect("fx\\room.fx", 0);
	LoadEffect("fx\\obj.fx", 1);
	LoadEffect("fx\\floor.fx", 2);

    // Init room
	InitObject(SO_DEFAULT_ROOM, 0);
	InitObject(SO_DX_MAN, 1);
	
	// Positioning room
    D3DMATRIX Pos = {
      1, 0, 0, 0,
	  0, 1, 0, 0,
	  0, 0, 1, 0,
	  0, 2, 0, 1,
	};
    objects[0]->SetWorldPos(&Pos);

	float b=1;
    D3DMATRIX Pos1 = {
		0.1f, 0, 0, 0,
		0, 0.1f, 0, 0,
		0, 0, 0.1f, 0,
		0, 0, 0, 1,
	};
	D3DXMATRIX Rot;
	D3DXMatrixRotationX(&Rot, -1.5f);
	D3DXMatrixMultiply((D3DXMATRIX*)&Pos1, (D3DXMATRIX*)&Pos1, &Rot);

	objects[1]->SetWorldPos(&Pos1);

	D3DXVECTOR4 *lp = new D3DXVECTOR4(5, -5, 0, 1);
	objects[0]->SetLightPos(lp, 0);
	objects[1]->SetLightPos(lp, 0);

	D3DXVECTOR4 *lp1 = new D3DXVECTOR4(-5, 5, 0, 1);
	objects[0]->SetLightPos(lp1, 1);
	objects[1]->SetLightPos(lp1, 1);
}

bool CGraphEngine::InitObject(int type, int index)
{
	objects[index] = new CDXGObject(pDevice);

	switch(type)
	{
		case SO_DEFAULT_ROOM:
			objects[index]->LoadMeshFromFile("Media\\room.x", NULL);
			objects[index]->SetEffect(pEffect[0], 0);
			objects[index]->SetEffect(pEffect[2], 1);
			objects[index]->SetTechnique("PixelLight", 0);
			objects[index]->SetTechnique("PixelLight", 1);
			break;
		case SO_DX_MAN:
			objects[index]->LoadMeshFromFile("Media\\tiny.x", NULL);
			objects[index]->SetEffect(pEffect[0], 0);
			objects[index]->SetEffect(pEffect[1], 1);
			objects[index]->SetTechnique("PixelLight",  0);
			objects[index]->SetTechnique("PixelLight",  1);
			break;
	}

	return true;
}

void CGraphEngine::LoadEffect(const char* filename, int index)
{
	if (filename==NULL)
		return;

	HRESULT hr = D3DXCreateEffectFromFile(pDevice, filename, 0, 0, 0, 0, &pEffect[index], 0);
	if (FAILED(hr))
		return;
}

void CGraphEngine::RenderScene()
{
    pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	ViewAngle+=0.01f;
	float b=ViewAngle;
	D3DMATRIX World = {
		cos(b), 0, sin(b), 0,
		0, 1, 0, 0,
		-sin(b), 0, cos(b), 0,
		20, -10, 90, 1,
	};
    pDevice->SetTransform(D3DTS_VIEW, &World);

	pDevice->SetVertexDeclaration(VertDecl);

	objects[0]->Render(0);

	pDevice->Clear(0, NULL, D3DCLEAR_STENCIL, D3DCOLOR_XRGB(255,255,255), 1.0f, 0x40);
	for (int i=1; i<2; i++)
	{
		objects[i]->Render(0);
		objects[i]->Render(1, 0);
		objects[i]->Render(1, 1);
	}

	objects[0]->Render(1);
}
