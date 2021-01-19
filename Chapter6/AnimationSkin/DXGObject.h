#include <string>
#include "d3d9.h"
#include "d3dx9.h"

const int SO_DEFAULT_ROOM = 1;
const int SO_DX_MAN = 2;
const int MAX_OBJECT_EFFECTS = 10;

struct D3DXFRAME_DERIVED: public D3DXFRAME
{
    D3DXMATRIXA16 CombinedMatrix;
};

class CDXGObject
{
private:
	// 3D Device
	IDirect3DDevice9 *pDevice;

	// Mesh
	DWORD dwNumMaterials;
	ID3DXMesh *pObjMesh;
	LPDIRECT3DTEXTURE9 *pMeshTextures;
	D3DMATERIAL9 *pMeshMaterials;

	// Skin
    D3DXMESHDATA MeshData;
    LPD3DXSKININFO pSkinInfo;

	D3DXMATRIX **ppFrameMatrix;
	D3DXMATRIX *pBoneMatrix;

	// Effect
	ID3DXEffect *pEffect[MAX_OBJECT_EFFECTS];
	TCHAR *pTechniqueName[MAX_OBJECT_EFFECTS];
	IDirect3DTexture9 *pTexture[MAX_OBJECT_EFFECTS];
	D3DXVECTOR4 Light[MAX_OBJECT_EFFECTS];

	// World matrix
	D3DXMATRIX matWorld;
protected:
	HRESULT LoadMeshWithSkin(IDirect3DDevice9 *pDevice,	IDirectXFileData *pDataObj, DWORD LoadFlags);
	bool ParseObject(IDirectXFileData *pDataObj, IDirectXFileData *pParentDataObj, D3DXFRAME_DERIVED **frd, bool ref);
    bool ParseChild(IDirectXFileData *pDataObj, D3DXFRAME_DERIVED **frd, bool ref);

	void Positioning();
	void SetShaderParam(int ef_index);
public:
	D3DXFRAME_DERIVED *SkinFrame;

	CDXGObject(IDirect3DDevice9 *pD3DDevice);
	void LoadMeshFromFile(char* filename, char* texturename);
	bool LoadSkinMeshFromFile(char* filename);
	void LoadTexture(char* filename, char* name_in_effect, int index);
	void SetEffect(ID3DXEffect *effect, int index);
	void SetTechnique(char* tech, int index);
	const D3DXMATRIX& GetWorldPos() const { return matWorld; };
	void SetWorldPos(D3DMATRIX *matrix);
	void SetLightPos(D3DXVECTOR4 *Pos, int index);
	bool Render(DWORD ef_index=1, int light=-1);
	void SetMatrix(D3DXMATRIX *matTransformation, D3DXFRAME_DERIVED *Frame);
	D3DXFRAME_DERIVED * FindDXFrame(const char *FrameName, D3DXFRAME_DERIVED *Frame);
	HRESULT UpdateMesh();
};
