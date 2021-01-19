#include <string>
#include "d3d9.h"
#include "d3dx9.h"

const int SO_DEFAULT_ROOM = 1;
const int SO_DX_MAN = 2;
const int MAX_OBJECT_EFFECTS = 10;

class CDXGObject
{
public:
	// 3D Device
	IDirect3DDevice9 *pDevice;

	// Mesh
	DWORD dwNumMaterials;
	ID3DXMesh *pMesh;
	LPDIRECT3DTEXTURE9 *pMeshTextures;
	D3DMATERIAL9 *pMeshMaterials;

	// Effect
	ID3DXEffect *pEffect[MAX_OBJECT_EFFECTS];
	TCHAR *pTechniqueName[MAX_OBJECT_EFFECTS];
	IDirect3DTexture9 *pTexture[MAX_OBJECT_EFFECTS];
	D3DXVECTOR4 Light[MAX_OBJECT_EFFECTS];

	// World matrix
	D3DXMATRIX matWorld;
public:
	CDXGObject(IDirect3DDevice9 *pD3DDevice);
	void LoadMeshFromFile(char* filename, char* texturename);
	void LoadTexture(char* filename, char* name_in_effect, int index);
	void SetEffect(ID3DXEffect *effect, int index);
	void SetTechnique(char* tech, int index);
	const D3DXMATRIX& GetWorldPos() const { return matWorld; };
	void SetWorldPos(D3DMATRIX *matrix);
	void SetLightPos(D3DXVECTOR4 *Pos, int index);
	void Positioning();
	bool Render(DWORD ef_index=1);
	void SetShaderParam(int ef_index);
};
