#include "DXGObject.h"
#include "d3d9.h"
#include "d3dx9.h"
#include "..\..\common\dxfunc.h"
#include "rmxftmpl.h"
#include "rmxfguid.h"

CDXGObject::CDXGObject(IDirect3DDevice9 *pD3DDevice)
{
	for (int i=0; i<MAX_OBJECT_EFFECTS; i++)
		pEffect[i]=NULL;

	SkinFrame = NULL;
	pDevice = pD3DDevice;

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
}

bool CDXGObject::Render(DWORD ef_index, int light)
{
	int y=dwNumMaterials;
	if (pEffect[ef_index]==NULL)
		return false;

	Positioning();	

	SetShaderParam(ef_index);

	if (light==-1)
	{
		pEffect[ef_index]->SetValue("lightPos0", Light[0], D3DX_DEFAULT);
		pEffect[ef_index]->SetValue("lightPos1", Light[1], D3DX_DEFAULT);
		pEffect[ef_index]->SetValue("lightPos2", Light[2], D3DX_DEFAULT);
		pEffect[ef_index]->SetValue("lightPos3", Light[3], D3DX_DEFAULT);
		pEffect[ef_index]->SetValue("lightPos4", Light[4], D3DX_DEFAULT);
	}
	else
	{
		pEffect[ef_index]->SetValue("lightPos0", Light[light], D3DX_DEFAULT);
	}

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
			pObjMesh->DrawSubset(j);
		}
    }
    pEffect[ef_index]->End();

	return true;
}

void CDXGObject::LoadMeshFromFile(char* filename, char* texturename)
{
	dwNumMaterials = LoadMesh(filename, pDevice,
		&pObjMesh, &pMeshTextures, texturename, &pMeshMaterials);
}

bool CDXGObject::LoadSkinMeshFromFile(char* filename)
{
	if(filename == NULL)
		return false;

	D3DXFRAME_DERIVED **frd = NULL;
	SkinFrame = NULL;
    
	IDirectXFile *pxFile;
	IDirectXFileEnumObject *pxEnum;
	IDirectXFileData *pxData;

	if (FAILED(DirectXFileCreate(&pxFile)))
		return false;
    
	if (FAILED(pxFile->RegisterTemplates(D3DRM_XTEMPLATES, D3DRM_XTEMPLATE_BYTES))) 
		return false;
	
	if (FAILED(pxFile->CreateEnumObject(filename, DXFILELOAD_FROMFILE, &pxEnum))) 
		return FALSE;
	
	while (SUCCEEDED(pxEnum->GetNextDataObject(&pxData))) 
	{
		if (ParseObject(pxData, NULL, frd, FALSE) == FALSE)
			break;
		if (pxData){pxData->Release(); pxData=NULL;}
	}
	
	if (pSkinInfo) 
	{
		ppFrameMatrix = new D3DXMATRIX*[pSkinInfo->GetNumBones()];
		pBoneMatrix = new D3DXMATRIX[pSkinInfo->GetNumBones()];
				
		for (int i=0; i< pSkinInfo->GetNumBones(); i++) 
		{
			const char *BoneName = pSkinInfo->GetBoneName(i);
            
			D3DXFRAME_DERIVED *pFrame = FindDXFrame(BoneName, SkinFrame);
            
			if(pFrame)
				ppFrameMatrix[i] = &pFrame->CombinedMatrix;
			else
				ppFrameMatrix[i] = NULL;
		}
	}

	if (pxEnum) {pxEnum->Release(); pxEnum=NULL;}
	if (pxFile) {pxFile->Release(); pxFile=NULL;}
	if (pxData) {pxData->Release(); pxData=NULL;}
	return true;
}

HRESULT CDXGObject::UpdateMesh()
{
	for(DWORD i=0;i<pSkinInfo->GetNumBones();i++) 
	{
		pBoneMatrix[i] = (*pSkinInfo->GetBoneOffsetMatrix(i));
		pBoneMatrix[i] *= (*ppFrameMatrix[i]);
	}
	
	void *SrcPtr, *DestPtr;
	MeshData.pMesh->LockVertexBuffer(D3DLOCK_READONLY, (void**)&SrcPtr);
	pObjMesh->LockVertexBuffer(0, (void**)&DestPtr);
    
	pSkinInfo->UpdateSkinnedMesh(pBoneMatrix, NULL, SrcPtr, DestPtr);
	
	pObjMesh->UnlockVertexBuffer();
	MeshData.pMesh->UnlockVertexBuffer();
  
	return S_OK;
}

void CDXGObject::SetMatrix(D3DXMATRIX *matTransformation, D3DXFRAME_DERIVED *Frame)
{
	Frame->CombinedMatrix = Frame->TransformationMatrix * (*matTransformation);

    if (Frame->pFrameSibling)
		SetMatrix(matTransformation, (D3DXFRAME_DERIVED*)Frame->pFrameSibling);

    if (Frame->pFrameFirstChild)
		SetMatrix(&Frame->CombinedMatrix, (D3DXFRAME_DERIVED*)Frame->pFrameFirstChild);
}


D3DXFRAME_DERIVED * CDXGObject::FindDXFrame(const char *FrameName, D3DXFRAME_DERIVED *Frame)
{
	D3DXFRAME_DERIVED *tempFrame;
	
	if(Frame && Frame->Name && FrameName)
		if (!strcmp(FrameName, Frame->Name))
			return Frame;

    if (Frame->pFrameSibling) 
      if ((tempFrame = FindDXFrame(FrameName, (D3DXFRAME_DERIVED*)Frame->pFrameSibling)))
        return tempFrame;    

    if (Frame->pFrameFirstChild) 
      if((tempFrame = FindDXFrame(FrameName, (D3DXFRAME_DERIVED*)Frame->pFrameFirstChild)))
        return tempFrame;

    return NULL;
}

bool CDXGObject::ParseObject(IDirectXFileData *pDataObj, 
	IDirectXFileData *pParentDataObj,  D3DXFRAME_DERIVED **frd, bool ref)
{
	const GUID *Type;
	if(pDataObj == NULL)
		return NULL;
	
	if (FAILED(pDataObj->GetType(&Type)))
		Type = NULL;

	if(*Type == TID_D3DRMFrameTransformMatrix && ref == FALSE && frd) 
	{
		if(*frd) 
		{
			// Get object data
			if (pDataObj)
			{
				void *TemplateData = NULL;
				DWORD TemplateSize = 0;
				
				pDataObj->GetData(NULL, &TemplateSize, (PVOID*)&TemplateData);
				(*frd)->TransformationMatrix = *(D3DXMATRIX*)TemplateData;
			}
		}
	}

	if(*Type == TID_D3DRMFrame && ref == FALSE) 
	{
		D3DXFRAME_DERIVED *pFrame = new D3DXFRAME_DERIVED();
		pFrame->pFrameSibling = pFrame->pFrameFirstChild = NULL;

		// Get object name
		if (pDataObj)
		{
			pFrame->Name = NULL;
			DWORD  Size = 0;
            
			if (FAILED(pDataObj->GetName(NULL, &Size)))
				return NULL;
			if(Size) 
				if ((pFrame->Name = new char[Size]) != NULL)
					pDataObj->GetName(pFrame->Name, &Size);
		}

		
		if(frd == NULL)
		{
			pFrame->pFrameSibling = SkinFrame;
			pFrame->pFrameFirstChild = NULL;
			SkinFrame = pFrame; 
			frd = &pFrame;
		} 
		else 
		{
			pFrame->pFrameSibling = (*frd)->pFrameFirstChild;
			(*frd)->pFrameFirstChild = pFrame; 
			frd = &pFrame;
		}
	}
	
	if (*Type == TID_D3DRMMesh) 
		if (ref == FALSE)
			LoadMeshWithSkin(pDevice, pDataObj, D3DXMESH_SYSTEMMEM);

    return ParseChild(pDataObj, frd, ref);
}

bool CDXGObject::ParseChild(IDirectXFileData *pDataObj, D3DXFRAME_DERIVED **frd, bool ref)
{
	IDirectXFileObject *pSubObj = NULL;
	IDirectXFileData  *pSubData = NULL;
	IDirectXFileDataReference *pDataRef = NULL;
	
	while (SUCCEEDED(pDataObj->GetNextObject(&pSubObj))) 
	{
		if(SUCCEEDED(pSubObj->QueryInterface(IID_IDirectXFileDataReference,(void**)&pDataRef))) 
		{
			if(SUCCEEDED(pDataRef->Resolve(&pSubData))) 
			{
				if (!ParseObject(pSubData, pDataObj, frd, TRUE))
					return FALSE;
				if (pSubData){pSubData->Release();pSubData=NULL;}
			}
			if (pDataRef){pDataRef->Release();pDataRef=NULL;}
		} 
		else
			if(SUCCEEDED(pSubObj->QueryInterface(IID_IDirectXFileData, (void**)&pSubData))) 
			{
				if (!ParseObject(pSubData, pDataObj, frd, ref))
					return FALSE;

				if (pSubData){pSubData->Release();pSubData=NULL;}
			}
			if (pSubData){pSubData->Release();pSubData=NULL;}
	}
    
	return TRUE;
}

HRESULT CDXGObject::LoadMeshWithSkin(IDirect3DDevice9 *pDevice,
	IDirectXFileData *pDataObj, DWORD LoadFlags)
{
	ID3DXMesh *pLoadMesh = NULL;
	pSkinInfo = NULL;
	HRESULT hr;
	
	ID3DXBuffer *matBuffer = NULL, *abjBuffer = NULL;
	
	if (FAILED(hr=D3DXLoadSkinMeshFromXof(pDataObj, LoadFlags, pDevice, &abjBuffer, 
		&matBuffer, NULL, &dwNumMaterials, &pSkinInfo, &pLoadMesh)))
		return hr;
	
	if (abjBuffer){abjBuffer->Release();abjBuffer=NULL;}

	if (pSkinInfo && !pSkinInfo->GetNumBones())
		{pSkinInfo->Release();pSkinInfo=NULL;}

	MeshData.Type = D3DXMESHTYPE_MESH;
	MeshData.pMesh = pLoadMesh; pLoadMesh = NULL;
	
	if(pSkinInfo)
		MeshData.pMesh->CloneMeshFVF(0, MeshData.pMesh->GetFVF(), pDevice, &pObjMesh);

    D3DXMATERIAL *Materials = (D3DXMATERIAL*)matBuffer->GetBufferPointer();
    pMeshMaterials = new D3DMATERIAL9[dwNumMaterials];
    pMeshTextures  = new IDirect3DTexture9*[dwNumMaterials];

    for(DWORD i=0;i<dwNumMaterials; i++) 
	{
      pMeshMaterials[i] = Materials[i].MatD3D;
      pMeshMaterials[i].Ambient = pMeshMaterials[i].Diffuse;

      if(Materials[i].pTextureFilename)
		  D3DXCreateTextureFromFile(pDevice, Materials[i].pTextureFilename, &pMeshTextures[i]);
	  else
		  pMeshTextures[i] = NULL;
    }
	
	if (matBuffer){matBuffer->Release(); matBuffer=NULL;}
    
	return S_OK;
}

