// Landscape1.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Landscape1.h"
#include "../../common/dxfunc.h"
#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;								// current instance
TCHAR szTitle[MAX_LOADSTRING];					// The title bar text
TCHAR szWindowClass[MAX_LOADSTRING];			// the main window class name

// Direct3D objects
IDirect3D9       *pD3D       = NULL;
IDirect3DDevice9 *pD3DDevice = NULL;

int iWidth=800;
int iHeight=600; 

#define NUM_TREES 500
float fViewAngle=0.0f;

D3DXVECTOR3 g_vDir;

LPDIRECT3DTEXTURE9 *pMeshTextures1;
D3DMATERIAL9 *pMeshMaterials1;
LPDIRECT3DTEXTURE9 *pMeshTextures2;
D3DMATERIAL9 *pMeshMaterials2;
DWORD skynum, seanum;

inline FLOAT HeightField( FLOAT x, FLOAT y )
{
    return 9*(cosf(x/20+0.2f)*cosf(y/15-0.2f)+1.0f);
}

// Vertex type for the trees
struct TREEVERTEX
{
  D3DXVECTOR3 p;      // Vertex position
  DWORD color;  // Vertex color
  FLOAT tu, tv; // Vertex texture coordinates

  static const DWORD FVF;
};

const DWORD TREEVERTEX::FVF = D3DFVF_XYZ | D3DFVF_DIFFUSE | D3DFVF_TEX1;

// Tree textures
TCHAR* g_strTreeTextures[] =
{
    _T("Tree02S.dds"),
    _T("Tree35S.dds"),
    _T("Tree01S.dds"),
};

#define NUMTREETEXTURES 3

struct Tree
{
    TREEVERTEX  v[4];           // Four corners of billboard quad
    D3DXVECTOR3 vPos;           // Origin of tree
    DWORD       dwTreeTexture;  // Which texture map to use
    DWORD       dwOffset;       // Offset into vertex buffer of tree's vertices
};

Tree          m_Trees[NUM_TREES];   // Array of tree info
ID3DXMesh*     m_pTerrain;           // Terrain object
ID3DXMesh*     m_pSkyBox;            // Skybox background object

IDirect3DVertexBuffer9 * VBTree;
IDirect3DTexture9 * TreeTextures[NUMTREETEXTURES];
D3DXMATRIXA16    m_matBillboardMatrix; // Used for billboard orientation

D3DXVECTOR3   mEyePt;

// Forward declarations of functions included in this code module:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK	About(HWND, UINT, WPARAM, LPARAM);
void DrawScene();

BOOL IsTreePositionValid( DWORD iTree )
{
    FLOAT x = m_Trees[iTree].vPos.x;
    FLOAT z = m_Trees[iTree].vPos.z;

    for( DWORD i=0; i < iTree; i++ )
    {
        double fDeltaX = fabs( x - m_Trees[i].vPos.x );
        double fDeltaZ = fabs( z - m_Trees[i].vPos.z );

        if( 3.0 > pow( fDeltaX, 2 ) + pow( fDeltaZ, 2 ) )
            return FALSE;
    }

    return TRUE;
}

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
 	// TODO: Place code here.
	MSG msg;
	HACCEL hAccelTable;

	// Initialize global strings
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_LANDSCAPE1, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance (hInstance, nCmdShow)) 
	{
		return FALSE;
	}

	hAccelTable = LoadAccelerators(hInstance, (LPCTSTR)IDC_LANDSCAPE1);

	// Main message loop:
	while (GetMessage(&msg, NULL, 0, 0)) 
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) 
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (int) msg.wParam;
}



//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX); 

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= (WNDPROC)WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, (LPCTSTR)IDI_LANDSCAPE1);
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= (LPCTSTR)IDC_LANDSCAPE1;
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, (LPCTSTR)IDI_SMALL);

	return RegisterClassEx(&wcex);
}

int __cdecl TreeSortCB( const VOID* arg1, const VOID* arg2 )
{
    Tree* p1 = (Tree*)arg1;
    Tree* p2 = (Tree*)arg2;
    
    FLOAT d1 = p1->vPos.x * g_vDir.x + p1->vPos.z * g_vDir.z;
    FLOAT d2 = p2->vPos.x * g_vDir.x + p2->vPos.z * g_vDir.z;
    if (d1 < d2)
        return +1;

    return -1;
}

//
//   FUNCTION: InitInstance(HANDLE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   HWND hWnd;

   hInst = hInstance; // Store instance handle in our global variable

   hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   if (DX3DInitZ(&pD3D, &pD3DDevice, hWnd, iWidth, iHeight, FALSE)!=S_OK)
   {
	   MessageBox(hWnd, "Error Initializing DirectX", "Error", MB_OK);	   
	   return FALSE;
   }



    for( WORD i=0; i<NUM_TREES; i++ )
    {
        do
        {
            // Position the trees randomly
            FLOAT fTheta  = 2.0f*D3DX_PI*(FLOAT)rand()/RAND_MAX;
            FLOAT fRadius = 25.0f + 55.0f * (FLOAT)rand()/RAND_MAX;
            m_Trees[i].vPos.x  = fRadius * sinf(fTheta);
            m_Trees[i].vPos.z  = fRadius * cosf(fTheta);
            m_Trees[i].vPos.y  = HeightField( m_Trees[i].vPos.x, m_Trees[i].vPos.z );
        }
        while( !IsTreePositionValid( i ) );

        // Size the trees randomly
        float fWidth  = 1.0f + 0.2f * (FLOAT)(rand()-rand())/RAND_MAX;
        float fHeight = 1.4f + 0.4f * (FLOAT)(rand()-rand())/RAND_MAX;

        // Each tree is a random color between red and green
        DWORD r = (255-190) + (DWORD)(190*(FLOAT)(rand())/RAND_MAX);
        DWORD g = (255-190) + (DWORD)(190*(FLOAT)(rand())/RAND_MAX);
        DWORD b = 0;
        DWORD dwColor = 0xff000000 + (r<<16) + (g<<8) + (b<<0);

        m_Trees[i].v[0].p     = D3DXVECTOR3(-fWidth, 0*fHeight, 0.0f );
        m_Trees[i].v[0].color = dwColor;
        m_Trees[i].v[0].tu    = 0.0f;   m_Trees[i].v[0].tv = 1.0f;
        m_Trees[i].v[1].p     = D3DXVECTOR3(-fWidth, 2*fHeight, 0.0f  );
        m_Trees[i].v[1].color = dwColor;
        m_Trees[i].v[1].tu    = 0.0f;   m_Trees[i].v[1].tv = 0.0f;
        m_Trees[i].v[2].p     = D3DXVECTOR3( fWidth, 0*fHeight, 0.0f  );
        m_Trees[i].v[2].color = dwColor;
        m_Trees[i].v[2].tu    = 1.0f;   m_Trees[i].v[2].tv = 1.0f;
        m_Trees[i].v[3].p     = D3DXVECTOR3( fWidth, 2*fHeight, 0.0f  );
        m_Trees[i].v[3].color = dwColor;
        m_Trees[i].v[3].tu    = 1.0f;   m_Trees[i].v[3].tv = 0.0f;

        // Pick a random texture for the tree
        m_Trees[i].dwTreeTexture = (DWORD)( ( NUMTREETEXTURES * rand() ) / (FLOAT)RAND_MAX );
    }

	
	
	
	HRESULT hr;
    // Create the tree textures
    for( DWORD i=0; i<NUMTREETEXTURES; i++ )
		D3DXCreateTextureFromFileEx(pD3DDevice, g_strTreeTextures[i], 
             D3DX_DEFAULT, D3DX_DEFAULT, D3DX_DEFAULT, 0, D3DFMT_UNKNOWN, 
             D3DPOOL_MANAGED, D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 
             D3DX_FILTER_TRIANGLE|D3DX_FILTER_MIRROR, 0, NULL, NULL, &TreeTextures[i]);

    // Create a quad for rendering each tree
    if( FAILED( hr = pD3DDevice->CreateVertexBuffer( NUM_TREES*4*sizeof(TREEVERTEX),
                                                       D3DUSAGE_WRITEONLY, TREEVERTEX::FVF,
                                                       D3DPOOL_MANAGED, &VBTree, NULL ) ) )
    {
        return hr;
    }

    // Copy tree mesh data into vertexbuffer
    TREEVERTEX* v;
    VBTree->Lock( 0, 0, (void**)&v, 0 );

    DWORD dwOffset = 0;
    for(int iTree = 0; iTree < NUM_TREES; iTree++ )
    {
        memcpy( &v[dwOffset], m_Trees[iTree].v, 4*sizeof(TREEVERTEX) );
        m_Trees[iTree].dwOffset = dwOffset;
        dwOffset += 4;
    }

    VBTree->Unlock();

    // Load the skybox
   skynum=LoadMesh("SkyBox2.x", pD3DDevice,
			   &m_pSkyBox, &pMeshTextures1,
			   "sky.bmp", &pMeshMaterials1
			   );

   seanum=LoadMesh("SeaFloor.x", pD3DDevice,
			   &m_pTerrain, &pMeshTextures2,
			   "texture.bmp", &pMeshMaterials2
			   );

    // Add some "hilliness" to the terrain
    LPDIRECT3DVERTEXBUFFER9 pVB;
    if( SUCCEEDED( m_pTerrain->GetVertexBuffer( &pVB ) ) )
    {
        struct VERTEX { FLOAT x,y,z,tu,tv; };
        VERTEX* pVertices;
        DWORD   dwNumVertices = m_pTerrain->GetNumVertices();

        pVB->Lock( 0, 0, (void**)&pVertices, 0 );

        for( DWORD i=0; i<dwNumVertices; i++ )
            pVertices[i].y = HeightField( pVertices[i].x, pVertices[i].z );

        pVB->Unlock();
        pVB->Release();
    }

    pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

   return TRUE;
}

//
//  FUNCTION: WndProc(HWND, unsigned, WORD, LONG)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_COMMAND:
		wmId    = LOWORD(wParam); 
		wmEvent = HIWORD(wParam); 
		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, (LPCTSTR)IDD_ABOUTBOX, hWnd, (DLGPROC)About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
		break;
	case WM_PAINT:
		pD3DDevice->Clear(0, 0, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_XRGB(255,255,255), 1.0, 0);

		pD3DDevice->BeginScene();
		DrawScene();
		pD3DDevice->EndScene();

		pD3DDevice->Present(NULL, NULL, 0, NULL);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

// Message handler for about box.
LRESULT CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_INITDIALOG:
		return TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL) 
		{
			EndDialog(hDlg, LOWORD(wParam));
			return TRUE;
		}
		break;
	}
	return FALSE;
}

void DrawTrees()
{
    // Set diffuse blending for alpha set in vertices.
    pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   TRUE );
    pD3DDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
    pD3DDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

    // Enable alpha testing (skips pixels with less than a certain alpha.)
    {
        pD3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE, TRUE );
        pD3DDevice->SetRenderState( D3DRS_ALPHAREF,        0x08 );
        pD3DDevice->SetRenderState( D3DRS_ALPHAFUNC, D3DCMP_GREATEREQUAL );
    }

    // Loop through and render all trees
    pD3DDevice->SetStreamSource( 0, VBTree, 0, sizeof(TREEVERTEX) );
    pD3DDevice->SetFVF( TREEVERTEX::FVF );
    for( DWORD i=0; i<NUM_TREES; i++ )
    {
        // Quick culling for trees behind the camera
        // This calculates the tree position relative to the camera, and
        // projects that vector against the camera's direction vector. A
        // negative dot product indicates a non-visible tree.
        if( 0 > ( m_Trees[i].vPos.x - mEyePt.x ) * g_vDir.x + 
                ( m_Trees[i].vPos.z - mEyePt.z ) * g_vDir.z )
        {
            break;
        }
        
        // Set the tree texture
        pD3DDevice->SetTexture( 0, TreeTextures[m_Trees[i].dwTreeTexture] );

        // Translate the billboard into place
        m_matBillboardMatrix._41 = m_Trees[i].vPos.x;
        m_matBillboardMatrix._42 = m_Trees[i].vPos.y;
        m_matBillboardMatrix._43 = m_Trees[i].vPos.z;
        pD3DDevice->SetTransform( D3DTS_WORLD, &m_matBillboardMatrix );

        // Render the billboard
        pD3DDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, m_Trees[i].dwOffset, 2 );
    }

    // Restore state
    D3DXMATRIXA16  matWorld;
    D3DXMatrixIdentity( &matWorld );
    pD3DDevice->SetTransform( D3DTS_WORLD, &matWorld );
    pD3DDevice->SetRenderState( D3DRS_ALPHATESTENABLE,    FALSE );
    pD3DDevice->SetRenderState( D3DRS_ALPHABLENDENABLE,   FALSE );

    return;
}

void DrawScene()
{
	fViewAngle+=0.02f;
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXVECTOR3 vEyePt;
    D3DXVECTOR3 vLookatPt;

    vEyePt.x = 30.0f*cosf( 0.8f * ( fViewAngle ) );
    vEyePt.z = 30.0f*sinf( 0.8f * ( fViewAngle ) );
    vEyePt.y = 4 + HeightField( vEyePt.x, vEyePt.z );

    vLookatPt.x = 30.0f*cosf( 0.8f * ( fViewAngle + 0.5f ) );
    vLookatPt.z = 30.0f*sinf( 0.8f * ( fViewAngle + 0.5f ) );
    vLookatPt.y = vEyePt.y - 1.0f;

    // Set the app view matrix for normal viewing
    D3DXMATRIXA16 matView;
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );
    pD3DDevice->SetTransform( D3DTS_VIEW, &matView );

    // Set up a rotation matrix to orient the billboard towards the camera.
    D3DXVECTOR3 vDir = vLookatPt - vEyePt;
    if( vDir.x > 0.0f )
        D3DXMatrixRotationY( &m_matBillboardMatrix, -atanf(vDir.z/vDir.x)+D3DX_PI/2 );
    else
        D3DXMatrixRotationY( &m_matBillboardMatrix, -atanf(vDir.z/vDir.x)-D3DX_PI/2 );
    g_vDir   = vDir;

    // Sort trees in back-to-front order
    qsort(m_Trees, NUM_TREES, sizeof(Tree), TreeSortCB);

    // Store vectors used elsewhere
    mEyePt = vEyePt;

	// Center view matrix for skybox and disable zbuffer
	D3DXMATRIXA16 matView1, matViewSave;
	pD3DDevice->GetTransform( D3DTS_VIEW, &matViewSave );
	matView1 = matViewSave;
	matView1._41 = 0.0f; matView1._42 = -0.3f; matView1._43 = 0.0f;
	pD3DDevice->SetTransform( D3DTS_VIEW,      &matView1 );
	pD3DDevice->SetRenderState( D3DRS_ZENABLE, FALSE );
	pD3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, FALSE );

     // Render the skybox
	for (DWORD i=0; i<skynum; i++)
	{			
		pD3DDevice->SetMaterial(&pMeshMaterials1[i]);

		if (pMeshTextures1[i])
			pD3DDevice->SetTexture(0, pMeshTextures1[i]);

		m_pSkyBox->DrawSubset(i);
	}

	// Restore the render states
	pD3DDevice->SetTransform( D3DTS_VIEW,      &matViewSave );
	pD3DDevice->SetRenderState( D3DRS_ZENABLE, TRUE );
	pD3DDevice->SetRenderState( D3DRS_ZWRITEENABLE, TRUE);

    // Draw the terrain
	for (DWORD i=0; i<seanum; i++)
	{			
		pD3DDevice->SetMaterial(&pMeshMaterials2[i]);

		if (pMeshTextures2[i])
			pD3DDevice->SetTexture(0, pMeshTextures2[i]);

		m_pTerrain->DrawSubset(i);
	}

	// Draw the trees
	DrawTrees();
 }