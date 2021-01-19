#ifndef INPUTCL_H
#define INPUTCL_H

#include <dinput.h>

// {50EB7CDA-FB22-464c-8F96-EED3B5683E9F}
const GUID g_guidApp = { 0x50eb7cda, 0xfb22, 0x464c, 
   {0x8f, 0x96, 0xee, 0xd3, 0xb5, 0x68, 0x3e, 0x9f}};

enum ActionsEnum{
 WALK_LEFT,
 WALK_RIGHT,
 WALK_FORWARD,
 WALK_BACK,
 PUNCH, 
 LOOK_X,    
 LOOK_Y, 
 QUIT
};

// Constants
const int MAX_DEVICES     = 8;    
const int BUTTON_DOWN     = 0x80; 
const int NUM_OF_ACTIONS  = 8;    

struct DeviceState
{
    LPDIRECTINPUTDEVICE8 pDevice;   
    bool  bAxisRelative;            
    DWORD dwInput[NUM_OF_ACTIONS];  
    bool  bMapped[NUM_OF_ACTIONS];  
};  

class CInputEngine
{
private: 
public:
 DeviceState aDevices[MAX_DEVICES];  
 int iNumDevices;  

 CInputEngine();
 int GetActionsState();
 HRESULT AddDevice(const DIDEVICEINSTANCE* lpddi, 
        const LPDIRECTINPUTDEVICE8 lpdid);
};

#endif
