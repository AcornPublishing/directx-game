#include <dinput.h>
#include "GraphEngine.h"

CGraphEngine::CGraphEngine(IDirect3DDevice9 *pD3DDevice)
{
	pDevice=pD3DDevice;

	// Init vectors
	pos  = D3DXVECTOR3( 0.0f,  0.0f,  0.0f);
	vecLR  = D3DXVECTOR3( 10.0f,  0.0f,  0.0f);
	vecFB = D3DXVECTOR3 ( 0.0f,  0.0f, -10.0f);
	up   = D3DXVECTOR3 ( 0.0f, 10.0f,  0.0f);
	D3DXMatrixIdentity((D3DXMATRIX *)&xrMat);
	D3DXMatrixIdentity((D3DXMATRIX *)&yrMat);
	D3DXMatrixIdentity((D3DXMATRIX *)&mvMat);

	// Init view matrix
	D3DMATRIX w = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		20, -10, 90, 1
	};
	worldMat = w;

	// Vertix declaration
	D3DVERTEXELEMENT9 decl[]=
	{
		{0,0, D3DDECLTYPE_FLOAT3,D3DDECLMETHOD_DEFAULT,D3DDECLUSAGE_POSITION,0},
		{0, 12, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},  
		{0, 24, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},  
		D3DDECL_END()
	};
	pDevice->CreateVertexDeclaration(decl, &VertDecl);

	// Call init functions
	InitGraphObjects();
	InitDirectInput();
}

CGraphEngine::~CGraphEngine()
{
	delete ieEngine;
	for (int i=0; i<2; i++)
		delete objects[i];
}


void CGraphEngine::InitGraphObjects()
{
	/////////////////////////////////////////
	// LOADING
	/////////////////////////////////////////

	// Load effects
	LoadEffect("fx\\room.fx", 0);
	LoadEffect("fx\\obj.fx", 1);
	LoadEffect("fx\\floor.fx", 2);

    // Init room
	InitObject(SO_DX_MAN, 0);
	InitObject(SO_DEFAULT_ROOM, 10);
	
	/////////////////////////////////////////
	// POSITIONING
	/////////////////////////////////////////

	// Positioning room
    D3DMATRIX Pos = {
      1, 0, 0, 0,
	  0, 1, 0, 0,
	  0, 0, 1, 0,
	  0, 40, 0, 1,
	};
    objects[10]->SetWorldPos(&Pos);

	// Positioning man
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

	objects[0]->SetWorldPos(&Pos1);

	D3DXVECTOR4 *lp = new D3DXVECTOR4(5, -5, 0, 1);
	objects[10]->SetLightPos(lp, 0);
	objects[0]->SetLightPos(lp, 0);
}

bool CGraphEngine::InitObject(int type, int index)
{
	objects[index] = new CDXGObject(pDevice);

	switch(type)
	{
		case SO_DEFAULT_ROOM:
			objects[index]->LoadMeshFromFile("Media\\room.x", "Media\\wall.bmp");
			objects[index]->SetEffect(pEffect[0], 0);
			objects[index]->SetEffect(pEffect[2], 1);
			objects[index]->SetTechnique("PixelLight", 0);
			objects[index]->SetTechnique("PixelLight", 1);
			break;
		case SO_DX_MAN:
			objects[index]->LoadSkinMeshFromFile("Media\\tiny.x");
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

void CGraphEngine::RotateCamera(float fOffset, int Axis)
{
    D3DXMATRIX tView;

	switch (Axis)
	{
	case 0:
		D3DXMatrixRotationY(&tView, fOffset);
		D3DXMatrixMultiply((D3DXMATRIX*)&yrMat, (D3DXMATRIX*)&yrMat, (D3DXMATRIX*)&tView);
		break;
	case 1:
		D3DXMatrixRotationX(&tView, fOffset);
		D3DXMatrixMultiply((D3DXMATRIX*)&xrMat, (D3DXMATRIX*)&xrMat, (D3DXMATRIX*)&tView);
		break;
	}
}

void CGraphEngine::MoveCamera(float fOffset, D3DXVECTOR3 *vecModifier)
{
    D3DXMATRIX tView, tMat;
	D3DXVECTOR3 vect;

	D3DXVec3Subtract(&vect, vecModifier, &pos);
	vect *= fOffset;

	D3DXMatrixTranslation(&tView, vect.x, vect.y, vect.z);
	tMat = mvMat;
	D3DXMatrixMultiply((D3DXMATRIX*)&tMat, (D3DXMATRIX*)&tMat, (D3DXMATRIX*)&tView);

	int i=0;
	while(i<11)
	{
		objects[i]->getMeshBoundBox();
		D3DXVECTOR3 BoundingBoxMin, BoundingBoxMax;
		D3DXVec3TransformCoord(&BoundingBoxMin, &objects[i]->vecBoundingBoxMin, objects[i]->GetWorldPos());
		D3DXVec3TransformCoord(&BoundingBoxMax, &objects[i]->vecBoundingBoxMax, objects[i]->GetWorldPos());
		
		if (BoundingBoxMin.z > BoundingBoxMax.z)
		{
			double mx = BoundingBoxMax.z;
			BoundingBoxMax.z = BoundingBoxMin.z;
			BoundingBoxMin.z = mx;
		}

		if (tMat._41 > BoundingBoxMin.x && 
			tMat._41 < BoundingBoxMax.x &&
			tMat._42 > BoundingBoxMin.y && 
			tMat._42 < BoundingBoxMax.y &&
			tMat._43 > BoundingBoxMin.z && 
			tMat._43 < BoundingBoxMax.z)
		{
			D3DXVec3Subtract(&vect, vecModifier, &pos);
			if (fOffset<0)
				vect *= 0.5;
			else
				vect *= -0.5;
			
			D3DXMatrixTranslation(&tView, vect.x, vect.y, vect.z);
			tMat = mvMat;
			D3DXMatrixMultiply((D3DXMATRIX*)&tMat, (D3DXMATRIX*)&tMat, (D3DXMATRIX*)&tView);
			mvMat = tMat;
			return;
		}
		i+=10;
	}
	mvMat = tMat;
}

void CGraphEngine::KeyControl()
{
	for (int iDevice=0; iDevice<ieEngine->iNumDevices; iDevice++)
	{
		if (ieEngine->aDevices[iDevice].dwInput[QUIT]==1)
			PostQuitMessage(1);

		if (ieEngine->aDevices[iDevice].dwInput[WALK_FORWARD]==1)
			MoveCamera(.2f, &vecFB);

		if (ieEngine->aDevices[iDevice].dwInput[WALK_BACK]==1)
			MoveCamera(-.2f, &vecFB);

		if (ieEngine->aDevices[iDevice].dwInput[WALK_LEFT]==1)
			MoveCamera(.2f, &vecLR);

		if (ieEngine->aDevices[iDevice].dwInput[WALK_RIGHT]==1)
			MoveCamera(-.2f, &vecLR);

		if (ieEngine->aDevices[iDevice].dwInput[LOOK_X]!=0xCDCDCDCD)
		{
			if ((int)ieEngine->aDevices[iDevice].dwInput[LOOK_X]>0)
				RotateCamera(0.05f, 0);
			
			if ((int)ieEngine->aDevices[iDevice].dwInput[LOOK_X]<0)
				RotateCamera(-0.05f, 0);
			ieEngine->aDevices[iDevice].dwInput[LOOK_X]=0;
		}

		if (ieEngine->aDevices[iDevice].dwInput[LOOK_Y]!=0xCDCDCDCD)
		{
			if ((int)ieEngine->aDevices[iDevice].dwInput[LOOK_Y]>0)
				RotateCamera(-0.05f, 1);
			
			if ((int)ieEngine->aDevices[iDevice].dwInput[LOOK_Y]<0)
				RotateCamera(0.05f, 1);
			ieEngine->aDevices[iDevice].dwInput[LOOK_Y]=0;
		}
	}

	// Positioning
	pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	vecLR = D3DXVECTOR3(10.0f, 0.0f, 0.0f);
	vecFB = D3DXVECTOR3(0.0f, 0.0f, -10.0f);
	up = D3DXVECTOR3(0.0f, 10.0f, 0.0f);

	D3DMATRIX View=worldMat;
	D3DXMatrixMultiply((D3DXMATRIX*)&View, (D3DXMATRIX*)&xrMat, (D3DXMATRIX*)&yrMat);
	D3DXMatrixMultiply((D3DXMATRIX*)&View, (D3DXMATRIX*)&View, (D3DXMATRIX*)&mvMat);

	D3DXVec3TransformCoord(&pos, &pos, (D3DXMATRIX*)&View);
	D3DXVec3TransformCoord(&vecFB, &vecFB, (D3DXMATRIX*)&View);
	D3DXVec3TransformCoord(&vecLR, &vecLR, (D3DXMATRIX*)&View);
	D3DXVec3TransformNormal(&up, &up, (D3DXMATRIX*)&View);
                                              
	D3DXMatrixLookAtLH((D3DXMATRIX*)&View, &pos, &vecFB, &up);
	pDevice->SetTransform(D3DTS_VIEW, &View);
}

void CGraphEngine::RenderScene()
{
	if (ieEngine->GetActionsState()!=0)
		KeyControl();

    pDevice->SetRenderState(D3DRS_FILLMODE, D3DFILL_SOLID);
    pDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	pDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);

	pDevice->SetVertexDeclaration(VertDecl);

	for (int i=0; i<1; i++)
	{
		objects[i]->Render(0);
	}

	pDevice->Clear(0, NULL, D3DCLEAR_STENCIL, D3DCOLOR_XRGB(255,255,255), 1.0f, 0x40);
	for (int i=10; i<11; i++)
	{
		objects[i]->Render(0);
		objects[i]->Render(1, 0);
		objects[i]->Render(1, 1);
	}

	for (int i=0; i<1; i++)
	{
		objects[i]->Render(1);
	}
}

void CGraphEngine::InitDirectInput()
{
	ieEngine = new CInputEngine();
}
