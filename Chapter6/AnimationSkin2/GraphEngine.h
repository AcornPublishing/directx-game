#include "d3d9.h"
#include "d3dx9.h"
#include "DXGObject.h"

const int NUM_OBJECTS = 20;
const int MAX_EFFECTS = 50;

class CGraphEngine
{
private: 
	DWORD tStartAnimTime;
	D3DXMATRIX matAnimStart, matAnimEnd;

	float ViewAngle;
	ID3DXEffect *pEffect[MAX_EFFECTS];
	TCHAR *pTechniqueName[MAX_EFFECTS];
	IDirect3DDevice9 *pDevice;
	CDXGObject *objects[NUM_OBJECTS];	
	IDirect3DVertexDeclaration9 *VertDecl;
public:
	CGraphEngine(IDirect3DDevice9 *pD3DDevice);
	void InitGraphObjects();
	bool InitObject(int type, int index);
	void LoadEffect(const char* filename, int index);
	void RenderScene();
};
