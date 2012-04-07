////////////////////////////////////////////////////////////////////////////
//
// CGP_Title Class Implementation
//
////////////////////////////////////////////////////////////////////////////

#include "../GameObject/Manager/CCharacterMgr.h"
#include "../CLand.h"
#include "../CCamera.h"

#include "../Input/IMEEdit.h"

#include "../BaseHeader.h"

//--------------------------------------------------------------------------

#include <windows.h>

#include "CGP_Title.h"

////////////////////////////////////////////////////////////////////////////
//

CGP_Title::CGP_Title()
{
	Init();
}

CGP_Title::~CGP_Title()
{
	Destroy();
}

void
CGP_Title::Init( void )
{
}

BOOL
CGP_Title::Create( void )
{
	if( !CGameProgress::Create() ) return FALSE;

	m_bIsCreated = TRUE;

	return TRUE;
}

BOOL
CGP_Title::Destroy( void )
{
	if( !CGameProgress::Destroy() ) return FALSE;

	m_bIsCreated = FALSE;

	return TRUE;
}

HRESULT
CGP_Title::FrameMove( void )
{
	return S_OK;
}

HRESULT
CGP_Title::Render( void )
{
	return S_OK;
}

BOOL
CGP_Title::InputProcess( void )
{
	m_bExit = TRUE;

	return TRUE;
}

LRESULT
CGP_Title::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return 0;
}

BOOL
CGP_Title::LoadData( void )
{
	if( m_bIsLoadedData ) return TRUE;

	m_bIsLoadedData = TRUE;

	return TRUE;
}

BOOL
CGP_Title::UnloadData( void )
{
	if( !m_bIsLoadedData ) return TRUE;

	m_bIsLoadedData = FALSE;

	return TRUE;
}

HRESULT
CGP_Title::RestoreDeviceObjects( void )
{
	return 0;
}

HRESULT
CGP_Title::InvalidateDeviceObjects( void )
{
	return 0;
}