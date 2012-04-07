#include <dinput.h>
#include "R3dinput.h"
#include "R3input.h"
#include "core.h"



#define SAMPLE_BUFFER_SIZE  16      // arbitrary number of buffer elements

//-----------------------------------------------------------------------------
// Defines, constants, and global variables
//-----------------------------------------------------------------------------
#define SAFE_DELETE(p)  { if(p) { delete (p);     (p)=NULL; } }
#define SAFE_RELEASE(p) { if(p) { (p)->Release(); (p)=NULL; } }

LPDIRECTINPUT8       g_pDI       = NULL;         
LPDIRECTINPUTDEVICE8 g_pKeyboard = NULL;     
LPDIRECTINPUTDEVICE8 g_pMouse = NULL;     

void FreeDirectInput();

static HRESULT CreateDirectInputKeybord( HWND hDlg,DWORD flag )
{
    HRESULT hr;
    BOOL    bExclusive=FALSE;
    BOOL    bForeground=FALSE;
    BOOL    bImmediate=FALSE;
    BOOL    bDisableWindowsKey=FALSE;
    DWORD   dwCoopFlags;
#ifdef _WIN64
    HINSTANCE hInst = (HINSTANCE) GetWindowLongPtr( hDlg, GWLP_HINSTANCE );
#else
    HINSTANCE hInst = (HINSTANCE) GetWindowLong( hDlg, GWL_HINSTANCE );
#endif

    // Cleanup any previous call first
    KillTimer( hDlg, 0 );    
    FreeDirectInput();

    // Detrimine where the buffer would like to be allocated 
	if( _KEYBOARD_EXCLUSIVE & flag )
		bExclusive = TRUE;
	if( _KEYBOARD_FOREGROUND & flag )
		bForeground = TRUE;
	if( _KEYBOARD_IMMEDIATE & flag )
		bImmediate = TRUE;
	if( _KEYBOARD_DISABLE_WINDOWSKEY & flag )
		bDisableWindowsKey = TRUE;

    if( bExclusive )
        dwCoopFlags = DISCL_EXCLUSIVE;
    else
        dwCoopFlags = DISCL_NONEXCLUSIVE;

    if( bForeground )
        dwCoopFlags |= DISCL_FOREGROUND;
    else
        dwCoopFlags |= DISCL_BACKGROUND;

    // Disabling the windows key is only allowed only if we are in foreground nonexclusive
    if( bDisableWindowsKey && !bExclusive && bForeground )
        dwCoopFlags |= DISCL_NOWINKEY;

    // Create a DInput object
    if( FAILED( hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
                                         IID_IDirectInput8, (VOID**)&g_pDI, NULL ) ) )
        return hr;
    
    // Obtain an interface to the system keyboard device.
    if( FAILED( hr = g_pDI->CreateDevice( GUID_SysKeyboard, &g_pKeyboard, NULL ) ) )
        return hr;
    
    // Set the data format to "keyboard format" - a predefined data format 
    //
    // A data format specifies which controls on a device we
    // are interested in, and how they should be reported.
    //
    // This tells DirectInput that we will be passing an array
    // of 256 bytes to IDirectInputDevice::GetDeviceState.
    if( FAILED( hr = g_pKeyboard->SetDataFormat( &c_dfDIKeyboard ) ) )
        return hr;
    
    // Set the cooperativity level to let DirectInput know how
    // this device should interact with the system and with other
    // DirectInput applications.
    hr = g_pKeyboard->SetCooperativeLevel( hDlg, dwCoopFlags );
    if( hr == DIERR_UNSUPPORTED && !bForeground && bExclusive )
    {
        FreeDirectInput();
        return S_OK;
    }

    if( FAILED(hr) )
        return hr;
/*
	//나중에 버퍼모드를 지원하면 사용한다.
    if( !bImmediate )
    {
        // IMPORTANT STEP TO USE BUFFERED DEVICE DATA!
        //
        // DirectInput uses unbuffered I/O (buffer size = 0) by default.
        // If you want to read buffered data, you need to set a nonzero
        // buffer size.
        //
        // Set the buffer size to DINPUT_BUFFERSIZE (defined above) elements.
        //
        // The buffer size is a DWORD property associated with the device.
        DIPROPDWORD dipdw;

        dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
        dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        dipdw.diph.dwObj        = 0;
        dipdw.diph.dwHow        = DIPH_DEVICE;
        dipdw.dwData            = SAMPLE_BUFFER_SIZE; // Arbitary buffer size

        if( FAILED( hr = g_pKeyboard->SetProperty( DIPROP_BUFFERSIZE, &dipdw.diph ) ) )
            return hr;
    }*/

    // Acquire the newly created device
    g_pKeyboard->Acquire();

    // Set a timer to go off 12 times a second, to read input
    // Note: Typically an application would poll the keyboard
    //       much faster than this, but this slow rate is simply 
    //       for the purposes of demonstration
    SetTimer( hDlg, 0, 1000 / 12, NULL );

    return S_OK;
}

//-----------------------------------------------------------------------------
// Name: ReadImmediateData()
// Desc: Read the input device's state when in immediate mode and display it.
//-----------------------------------------------------------------------------
static HRESULT ReadImmediateDataKeyboard()// HWND hDlg )
{
    HRESULT hr;
    TCHAR   strNewText[256*5 + 1] = TEXT("");
    TCHAR   strElement[10];    
    BYTE    diks[256];   // DirectInput keyboard state buffer 
    int     i;

    if( NULL == g_pKeyboard ) 
        return S_OK;
    
    // Get the input's device state, and put the state in dims
    ZeroMemory( &diks, sizeof(diks) );
    hr = g_pKeyboard->GetDeviceState( sizeof(diks), &diks );
    if( FAILED(hr) ) 
    {
        // DirectInput may be telling us that the input stream has been
        // interrupted.  We aren't tracking any state between polls, so
        // we don't have any special reset that needs to be done.
        // We just re-acquire and try again.
        
        // If input is lost then acquire and keep trying 
        hr = g_pKeyboard->Acquire();
        while( hr == DIERR_INPUTLOST ) 
            hr = g_pKeyboard->Acquire();

        // hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
        // may occur when the app is minimized or in the process of 
        // switching, so just try again later 
        return S_OK; 
    }
    
    // Make a string of the index values of the keys that are down
    for( i = 0; i < 256; i++ ) 
    {
        if( diks[i] & 0x80 ) 
        {
            wsprintf( strElement, "0x%02x ", i );
            strcat( strNewText, strElement );
        }
    }
    return S_OK;
}


//-----------------------------------------------------------------------------
// Name: FreeDirectInput()
// Desc: Initialize the DirectInput variables.
//-----------------------------------------------------------------------------
VOID FreeDirectInput()
{
    // Unacquire the device one last time just in case 
    // the app tried to exit while the device is still acquired.
    if( g_pKeyboard ) 
        g_pKeyboard->Unacquire();
    if( g_pMouse ) 
        g_pMouse->Unacquire();
    
    // Release any DirectInput objects.
    SAFE_RELEASE( g_pKeyboard );
    SAFE_RELEASE( g_pDI );
}


//-----------------------------------------------------------------------------
// Name: OnCreateDevice()
// Desc: Setups a the mouse device using the flags from the dialog.
//-----------------------------------------------------------------------------
static HRESULT CreateDirectInputMouse( HWND hDlg ,DWORD flag)
{
    HRESULT hr;
    BOOL    bExclusive;
    BOOL    bForeground;
    BOOL    bImmediate;
    DWORD   dwCoopFlags;

    // Cleanup any previous call first
    KillTimer( hDlg, 0 );    
    FreeDirectInput();

    // Detrimine where the buffer would like to be allocated 
	if( _MOUSE_EXCLUSIVE & flag )
		bExclusive =TRUE;
	if( _MOUSE_FOREGROUND & flag )
	    bForeground =TRUE;
	if( _MOUSE_IMMEDIATE & flag )
	    bImmediate =TRUE;

    if( bExclusive )
        dwCoopFlags = DISCL_EXCLUSIVE;
    else
        dwCoopFlags = DISCL_NONEXCLUSIVE;

    if( bForeground )
        dwCoopFlags |= DISCL_FOREGROUND;
    else
        dwCoopFlags |= DISCL_BACKGROUND;

    // Create a DInput object
    if( FAILED( hr = DirectInput8Create( GetModuleHandle(NULL), DIRECTINPUT_VERSION, 
                                         IID_IDirectInput8, (VOID**)&g_pDI, NULL ) ) )
        return hr;
    
    // Obtain an interface to the system mouse device.
    if( FAILED( hr = g_pDI->CreateDevice( GUID_SysMouse, &g_pMouse, NULL ) ) )
        return hr;
    
    // Set the data format to "mouse format" - a predefined data format 
    //
    // A data format specifies which controls on a device we
    // are interested in, and how they should be reported.
    //
    // This tells DirectInput that we will be passing a
    // DIMOUSESTATE2 structure to IDirectInputDevice::GetDeviceState.
    if( FAILED( hr = g_pMouse->SetDataFormat( &c_dfDIMouse2 ) ) )
        return hr;
    
    // Set the cooperativity level to let DirectInput know how
    // this device should interact with the system and with other
    // DirectInput applications.
    hr = g_pMouse->SetCooperativeLevel( hDlg, dwCoopFlags );
    if( hr == DIERR_UNSUPPORTED && !bForeground && bExclusive )
    {
        FreeDirectInput();
//        MessageBox( hDlg, _T("SetCooperativeLevel() returned DIERR_UNSUPPORTED.\n")
//                          _T("For security reasons, background exclusive mouse\n")
  //                        _T("access is not allowed."), 
    //                      _T("Mouse"), MB_OK );
        return S_OK;
    }

    if( FAILED(hr) )
        return hr;
/*
	//나중에 버퍼모드를 지원할 경우.
    if( !bImmediate )
    {
        // IMPORTANT STEP TO USE BUFFERED DEVICE DATA!
        //
        // DirectInput uses unbuffered I/O (buffer size = 0) by default.
        // If you want to read buffered data, you need to set a nonzero
        // buffer size.
        //
        // Set the buffer size to SAMPLE_BUFFER_SIZE (defined above) elements.
        //
        // The buffer size is a DWORD property associated with the device.
        DIPROPDWORD dipdw;
        dipdw.diph.dwSize       = sizeof(DIPROPDWORD);
        dipdw.diph.dwHeaderSize = sizeof(DIPROPHEADER);
        dipdw.diph.dwObj        = 0;
        dipdw.diph.dwHow        = DIPH_DEVICE;
        dipdw.dwData            = SAMPLE_BUFFER_SIZE; // Arbitary buffer size

        if( FAILED( hr = g_pMouse->SetProperty( DIPROP_BUFFERSIZE, &dipdw.diph ) ) )
            return hr;
    }
*/
    // Acquire the newly created device
    g_pMouse->Acquire();

    // Set a timer to go off 12 times a second, to read input
    // Note: Typically an application would poll the mouse
    //       much faster than this, but this slow rate is simply 
    //       for the purposes of demonstration
    SetTimer( hDlg, 0, 1000 / 12, NULL );

    return S_OK;
}




BOOL st_DInputState;

//-----------------------------------------------------------------------------
// Name: ReadImmediateData()
// Desc: Read the input device's state when in immediate mode and display it.
//-----------------------------------------------------------------------------
static HRESULT ReadImmediateDataMouse()
{
    HRESULT       hr;
    TCHAR         strNewText[256] = TEXT("");
    DIMOUSESTATE2 dims2;      // DirectInput mouse state structure
	st_DInputState = FALSE;

    if( NULL == g_pMouse ) 
        return S_OK;

    // Get the input's device state, and put the state in dims
    ZeroMemory( &dims2, sizeof(dims2) );
    hr = g_pMouse->GetDeviceState( sizeof(DIMOUSESTATE2), &dims2 );
    if( FAILED(hr) ) 
    {
        // DirectInput may be telling us that the input stream has been
        // interrupted.  We aren't tracking any state between polls, so
        // we don't have any special reset that needs to be done.
        // We just re-acquire and try again.
        
        // If input is lost then acquire and keep trying 
        hr = g_pMouse->Acquire();
        while( hr == DIERR_INPUTLOST ) 
            hr = g_pMouse->Acquire();

        // hr may be DIERR_OTHERAPPHASPRIO or other errors.  This
        // may occur when the app is minimized or in the process of 
        // switching, so just try again later 
        return S_OK; 
    }
	gMouse.x += dims2.lX;
	gMouse.y += dims2.lY;
	gMouse.z = dims2.lZ;
    if( dims2.rgbButtons[0] & 0x80)
		gMouse.button |= _LEFT_BUTTON_DOWN;
	else
		gMouse.button &= (~_LEFT_BUTTON_DOWN);
    if( dims2.rgbButtons[1] & 0x80)
		gMouse.button |= _RIGHT_BUTTON_DOWN;
	else
		gMouse.button &= (~_RIGHT_BUTTON_DOWN);
    if( dims2.rgbButtons[2] & 0x80)
		gMouse.button |= _MIDDLE_BUTTON_DOWN;
	else
		gMouse.button &= (~_MIDDLE_BUTTON_DOWN);

  /*  
    // The dims structure now has the state of the mouse, so 
    // display mouse coordinates (x, y, z) and buttons.
    sprintf( strNewText, "(X=% 3.3d, Y=% 3.3d, Z=% 3.3d) B0=%c B1=%c B2=%c B3=%c B4=%c B5=%c B6=%c B7=%c",
                         dims2.lX, dims2.lY, dims2.lZ,
                        (dims2.rgbButtons[0] & 0x80) ? '1' : '0',
                        (dims2.rgbButtons[1] & 0x80) ? '1' : '0',
                        (dims2.rgbButtons[2] & 0x80) ? '1' : '0',
                        (dims2.rgbButtons[3] & 0x80) ? '1' : '0',
                        (dims2.rgbButtons[4] & 0x80) ? '1' : '0',
                        (dims2.rgbButtons[5] & 0x80) ? '1' : '0',
                        (dims2.rgbButtons[6] & 0x80) ? '1' : '0',
                        (dims2.rgbButtons[7] & 0x80) ? '1' : '0');
    // Get the old text in the text box
    TCHAR strOldText[128];
    GetDlgItemText( hDlg, IDC_DATA, strOldText, 127 );
    
    // If nothing changed then don't repaint - avoid flicker
    if( 0 != lstrcmp( strOldText, strNewText ) ) 
        SetDlgItemText( hDlg, IDC_DATA, strNewText );
*/
	st_DInputState = TRUE;
    
    return S_OK;
}

BOOL IsUseDirectInput()	//현재 다이렉트 인풋을 쓰고있는가.
{
	return st_DInputState;
}

HRESULT InitR3DInput(HWND hWnd,DWORD flag)
{
	HRESULT hresult;
	hresult = CreateDirectInputKeybord( hWnd,flag );
	if( hresult != S_OK )
		return hresult;
	hresult = CreateDirectInputMouse( hWnd,flag);
	if( hresult != S_OK )
		return hresult;
    return S_OK;
}

void ReleaseR3DInput()
{
	FreeDirectInput();
}
HRESULT ReadR3ImmediateDataMouse()
{
	HRESULT hr = ReadImmediateDataMouse();
	_R3ENGINE_STATE *state=GetR3State();
	
	if( gMouse.x > state->mScreenXsize )
		gMouse.x = (int)state->mScreenXsize;
	if( gMouse.x < 0 )
		gMouse.x = 0;
	if( gMouse.y > state->mScreenYsize )
		gMouse.y = (int)state->mScreenYsize;
	if( gMouse.y < 0 )
		gMouse.y = 0;

	return hr;
}
HRESULT ReadR3ImmediateDataKeyboard()
{
	return ReadImmediateDataKeyboard();
}
