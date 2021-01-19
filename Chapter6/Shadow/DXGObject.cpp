#include "DXGObject.h"
#include "d3d9.h"
#include "d3dx9.h"
#include "..\..\common\dxfunc.h"

CDXGObject::CDXGObject(IDirect3DDevice9 *pD3DDevice)
{
	for (int i=0; i<MAX_OBJECT_EFFECTS; i++)
		pEffect[i]=NULL;
	pDevice=pD3DDevice;
	D3DXMatrixIdentity(&matWorld);
}

void CDXGObject::SetEffect(ID3DXEffect *effect, int index)
{
	pEffect[index] = effect;
}

void CDXGObject::SetTechnique(char* tech, int index)
{
	if (tech!=NULL)
	{
		pTechniqueName[index] = tech;
		pEffect[index]->SetTechnique(tech);
	}
}

void CDXGObject::LoadTexture(char* filename, char* name_in_effect, int index)
{
	D3DXCreateTextureFromFileEx(pDevice, filename, D3DX_DEFAULT, 
		D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, D3DPOOL_MANAGED, 
		D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR, 
		D3DX_FILTER_TRIANGLE | D3DX_FILTER_MIRROR,
		0, NULL, NULL, &pTexture[index]);

	if (name_in_effect)
		for (int i=0; i<MAX_OBJECT_EFFECTS; i++)
			if (pEffect[i])
				if (pEffect[i]->IsParameterUsed(name_in_effect, pTechniqueName[i]))
					pEffect[i]->SetTexture(name_in_effect, pTexture[index]);
}

void CDXGObject::SetLightPos(D3DXVECTOR4 *Pos, int index)
{
	Light[index] = *Pos;
}

void CDXGObject::SetWorldPos(D3DMATRIX* matrix)
{ 
    matWorld = *matrix;
}

void CDXGObject::Positioning()
{
	pDevice->SetTransform(D3DTS_WORLD, &GetWorldPos());
}

void CDXGObject::SetShaderParam(int ef_index)
{
	// Set matrix
	D3DXMATRIX Full, View, matProjection, ident;
	pDevice->GetTransform(D3DTS_VIEW, &View);
	pDevice->GetTransform(D3DTS_PROJECTION, &matProjection);
    D3DXMatrixMultiply(&Full, &matWorld, &View);

	if (pEffect[ef_index]->IsParameterUsed("world_view_matrix", pTechniqueName[ef_index]))
		pEffect[ef_index]->SetValue("world_view_matrix", &Full, D3DX_DEFAULT);

    D3DXMATRIX matWorldViewInv, matWorldViewInvTrans;
    D3DXMatrixInverse( &matWorldViewInv, NULL, &Full);
	D3DXMatrixTranspose( &matWorldViewInvTrans, &matWorldViewInv );
	if (pEffect[ef_index]->IsParameterUsed("WorldViewInvTrans", pTechniqueName[ef_index]))
		pEffect[ef_index]->SetValue("WorldViewInvTrans", &matWorldViewInvTrans, D3DX_DEFAULT);

	D3DXMatrixMultiply(&Full, &Full, &matProjection);

	if (pEffect[ef_index]->IsParameterUsed("world_matrix", pTechniqueName[ef_index]))
		pEffect[ef_index]->SetValue("world_matrix", &matWorld, D3DX_DEFAULT);

	if (pEffect[ef_index]->IsParameterUsed("view_matrix", pTechniqueName[ef_index]))
		pEffect[ef_index]->SetValue("view_matrix", &View, D3DX_DEFAULT);

	if (pEffect[ef_index]->IsParameterUsed("proj_matrix", pTechniqueName[ef_index]))
		pEffect[ef_index]->SetValue("proj_matrix", &matProjection, D3DX_DEFAULT);

	if (pEffect[ef_index]->IsParameterUsed("world_view_proj_matrix", pTechniqueName[ef_index]))
		pEffect[ef_index]->SetValue("world_view_proj_matrix", &Full, D3DX_DEFAULT);

	if (pEffect[ef_index]->IsParameterUsed("mat", pTechniqueName[ef_index]))
		pEffect[ef_index]->SetValue("mat", &Full, D3DX_DEFAULT);

	D3DXMatrixIdentity(&ident);
	if (pEffect[ef_index]->IsParameterUsed("ident_mat", pTechniqueName[ef_index]))
		pEffect[ef_index]->SetValue("ident_mat", &ident, D3DX_DEFAULT);

	pEffect[ef_index]->SetValue("lightPos0", Light[0], D3DX_DEFAULT);
	pEffect[ef_index]->SetValue("lightPos1", Light[1], D3DX_DEFAULT);
    pEffect[ef_index]->SetValue("lightPos2", Light[2], D3DX_DEFAULT);
    pEffect[ef_index]->SetValue("lightPos3", Light[3], D3DX_DEFAULT);
    pEffect[ef_index]->SetValue("lightPos4", Light[4], D3DX_DEFAULT);
}

bool CDXGObject::Render(DWORD ef_index)
{
	int y=dwNumMaterials;
	if (pEffect[ef_index]==NULL)
		return false;

    Positioning();	

	SetShaderParam(ef_index);

	// Render object
	UINT uPass;
    pEffect[ef_index]->Begin(&uPass, NULL);
	
	for(UINT i = 0; i < uPass; i++)
    {
        pEffect[ef_index]->Pass(i);

		for (DWORD j=0; j<dwNumMaterials; j++)
		{			
			pDevice->SetMaterial(&pMeshMaterials[j]);

			if (pMeshTextures[j])
			{
				pDevice->SetTexture(0, pMeshTextures[j]);
				if (pEffect[ef_index]->IsParameterUsed("tText", pTechniqueName[ef_index]))
					pEffect[ef_index]->SetValue("tText", &pMeshTextures[j], D3DX_DEFAULT);
			}
			pMesh->DrawSubset(j);
		}
    }
    pEffect[ef_index]->End();

	return true;
}

void CDXGObject::LoadMeshFromFile(char* filename, char* texturename)
{
   dwNumMaterials = LoadMesh(filename, pDevice,
			   &pMesh, &pMeshTextures,
			   texturename, &pMeshMaterials);
}

