#include <tchar.h>
#include "InputCl.h"


const TCHAR *ACTION_NAMES[] =
{
    TEXT("Walk left"),
    TEXT("Walk right"),
    TEXT("Walk forward"),
    TEXT("Walk back"),
    TEXT("Punch"),
    TEXT("Look X Axis"),
    TEXT("Look Y Axis"),
    TEXT("Quit")
};

DIACTION ActionMap[] =
{
    // Keyboard input mappings
    { WALK_LEFT,        DIKEYBOARD_LEFT,              0,  ACTION_NAMES[WALK_LEFT], },
    { WALK_RIGHT,       DIKEYBOARD_RIGHT,             0,  ACTION_NAMES[WALK_RIGHT], },
	{ WALK_FORWARD,     DIKEYBOARD_UP,                0,  ACTION_NAMES[WALK_FORWARD], },
	{ WALK_BACK,        DIKEYBOARD_DOWN,              0,  ACTION_NAMES[WALK_BACK], },

	{ PUNCH,            DIKEYBOARD_P,      DIA_APPFIXED,  ACTION_NAMES[PUNCH], },

	{ QUIT,             DIKEYBOARD_Q,      DIA_APPFIXED,  ACTION_NAMES[QUIT], },

    // Mouse input mappings
    { LOOK_X,             DIMOUSE_XAXIS,                0,  ACTION_NAMES[LOOK_X], },
    { LOOK_Y,             DIMOUSE_YAXIS,                0,  ACTION_NAMES[LOOK_Y], },
    { PUNCH,              DIMOUSE_BUTTON0,              0,  ACTION_NAMES[PUNCH], },
};

IDirectInput8 *diGameInput;
DIACTIONFORMAT diActionFormat;
bool g_bRefChart; 

inline DWORD GetNumOfMappings() { return sizeof(ActionMap)/sizeof(DIACTION); }

BOOL CALLBACK EnumDevicesCallback( LPCDIDEVICEINSTANCE lpddi, 
                                  LPDIRECTINPUTDEVICE8 lpdid, DWORD dwFlags, 
                                  DWORD dwRemaining, LPVOID pvRef )
{ 
    ((CInputEngine*)pvRef)->AddDevice(lpddi, lpdid);

    return DIENUM_CONTINUE;
}

HRESULT CInputEngine::AddDevice(const DIDEVICEINSTANCE* lpddi, 
                                const LPDIRECTINPUTDEVICE8 lpdid)
{ 
    HRESULT hr;

    if( iNumDevices < MAX_DEVICES )
    {
        if( FAILED(hr = lpdid->BuildActionMap( &diActionFormat, NULL, DIDBAM_DEFAULT )) )
            return DIENUM_CONTINUE;

        
        for( UINT i=0; i < diActionFormat.dwNumActions; i++ )
        {
            DIACTION *dia = &(diActionFormat.rgoAction[i]);

            if( dia->dwHow != DIAH_ERROR && dia->dwHow != DIAH_UNMAPPED )
                aDevices[iNumDevices].bMapped[dia->uAppData] = TRUE;
        }

        if( FAILED(hr = lpdid->SetActionMap(&diActionFormat, NULL, DIDSAM_DEFAULT)))
        {
            ZeroMemory( aDevices[iNumDevices].bMapped, 
                 sizeof(aDevices[iNumDevices].bMapped) );
            return DIENUM_CONTINUE;
        }

        aDevices[iNumDevices].pDevice = lpdid;
        lpdid->AddRef();

        DIPROPDWORD dipdw;  
        dipdw.diph.dwSize       = sizeof(DIPROPDWORD); 
        dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER); 
        dipdw.diph.dwObj        = 0; 
        dipdw.diph.dwHow        = DIPH_DEVICE; 
 
        hr = lpdid->GetProperty(DIPROP_AXISMODE, &dipdw.diph); 
        if (SUCCEEDED(hr)) 
            aDevices[iNumDevices].bAxisRelative = (DIPROPAXISMODE_REL == dipdw.dwData);

        g_bRefChart = TRUE; 
        iNumDevices++;    
    }
  
    return DIENUM_CONTINUE;
}

CInputEngine::CInputEngine()
{
	iNumDevices = 0;

	// Init Direct Input
	if (FAILED(DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
		IID_IDirectInput8, (VOID**)&diGameInput, NULL)))
		return;

    ZeroMemory(&diActionFormat, sizeof(DIACTIONFORMAT));
    diActionFormat.dwSize = sizeof(DIACTIONFORMAT);
    diActionFormat.dwActionSize = sizeof(DIACTION);
    diActionFormat.dwDataSize = GetNumOfMappings() * sizeof(DWORD);
    diActionFormat.guidActionMap = g_guidApp;
    diActionFormat.dwGenre = DIVIRTUAL_FIGHTING_HAND2HAND; 
    diActionFormat.dwNumActions = GetNumOfMappings();
    diActionFormat.rgoAction = ActionMap;
    diActionFormat.lAxisMin = -99;
    diActionFormat.lAxisMax = 99;
    diActionFormat.dwBufferSize = 16;
    _tcscpy( diActionFormat.tszActionMap, _T("ActionMap Sample") );

	if (FAILED(diGameInput->EnumDevicesBySemantics(NULL, &diActionFormat,
				EnumDevicesCallback, this, DIEDBSFL_ATTACHEDONLY)))
        return;
}

int CInputEngine::GetActionsState()
{
	int r;
	HRESULT hr;

    for (int iDevice=0; iDevice<iNumDevices; iDevice++)
    {
        LPDIRECTINPUTDEVICE8 pdidDevice = aDevices[iDevice].pDevice;
        DIDEVICEOBJECTDATA rgdod[10];
        DWORD dwItems = 10;

        hr = pdidDevice->Acquire();
        hr = pdidDevice->Poll();
        hr = pdidDevice->GetDeviceData(sizeof(DIDEVICEOBJECTDATA), 
			rgdod, &dwItems, 0);

        if (FAILED(hr))
            continue;

        if (aDevices[iDevice].bAxisRelative)
            aDevices[iDevice].dwInput[LOOK_X] = 0;

        for( DWORD j=0; j<dwItems; j++ )
        {
            UINT_PTR dwAction = rgdod[j].uAppData;
            int dwData = 0;

            switch(dwAction)
            {	
				case LOOK_X:
				case LOOK_Y:
                {
                    dwData = rgdod[j].dwData; 
                    
                    if (aDevices[iDevice].bAxisRelative)
                    {
                        if( (int)dwData < 0 )
                            dwData = max((int)dwData, diActionFormat.lAxisMin);
                        else
                            dwData = min((int)dwData, diActionFormat.lAxisMax);
                    }
                    
                    break;
                }

                default:
                {
                    if (rgdod[j].dwData & BUTTON_DOWN)
						dwData = 1;
					else
						dwData = 0;
                    break;
                }
            }

            aDevices[iDevice].dwInput[dwAction] = (DWORD)dwData;
			r++;
        }
    }
	
	return r;
}