#include "d3d9.h"
#include "d3dx9.h"
#include <dmusici.h>
#include "DXGObject.h"
#include "InputCl.h"

const int NUM_OBJECTS = 20;
const int MAX_EFFECTS = 50;

class CGraphEngine
{
private: 
	// Input variable
	CInputEngine *ieEngine;
	D3DMATRIX worldMat;
	D3DMATRIX xrMat, yrMat, mvMat;
	D3DXVECTOR3 pos, vecLR, vecFB, up;

	// Effect
	ID3DXEffect *pEffect[MAX_EFFECTS];
	TCHAR *pTechniqueName[MAX_EFFECTS];

	// Objects
	CDXGObject *objects[NUM_OBJECTS];	
	IDirect3DVertexDeclaration9 *VertDecl;

	// Direct Music
	IDirectMusicLoader8* pLoader;
	IDirectMusicPerformance8* pPerformance;
	IDirectMusicSegment8* pSegment;
	IDirectMusicAudioPath * pAudioPath;
	IDirectSound3DBuffer* p3DBuffer;
	IDirectSound3DListener* pListener;
	
	HWND hWnd;
	IDirect3DDevice9 *pDevice;

	// Functions
	void InitGraphObjects();
	bool InitObject(int type, int index);
	void LoadEffect(const char* filename, int index);
	void InitDirectInput();
	void KeyControl();
	void MoveCamera(float fOffset, D3DXVECTOR3 *vecModifier);
	void RotateCamera(float fOffset, int Axis);
public:
	CGraphEngine(IDirect3DDevice9 *pD3DDevice, HWND hw);
	~CGraphEngine();
	void RenderScene();
};
