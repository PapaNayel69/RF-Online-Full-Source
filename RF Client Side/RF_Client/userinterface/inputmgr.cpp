////////////////////////////////////////////////////////////////////////////
//
// CInputMgr Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "InputMgr.h"

CInputMgr::CInputMgr()
{
	m_hWnd = NULL;	
	m_pInput = NULL;
}

CInputMgr::~CInputMgr()
{

}

void
CInputMgr::Init( HWND pi_hWnd )
{
	m_hWnd = pi_hWnd;
	m_cIME.IMESetEnglish( pi_hWnd );
	m_cIME.IMEEditInit( m_ptInputPos, 128, D3DCOLOR_XRGB( 255, 255, 255 ) );	
	m_cIME.SetCaretColor( D3DCOLOR_XRGB( 95, 78, 252 ) );
}

void
CInputMgr::SetInputObject( CGUIInput * pi_pInput )
{
	m_pInput = pi_pInput;

	// 포커스되었을 때 깜빡이는 캐럿을 처음에는 보이게 한다.
	m_cIME.SetBlinkCaretState( FALSE );
}

void
CInputMgr::ReadyInput( POINT & pi_ptInputPos, char * pi_pInitStr )
{
	m_cIME.SetText( pi_pInitStr );

	SetInputPosition( pi_ptInputPos );
}

void
CInputMgr::SetText( char * pi_pStr )
{
	m_cIME.SetText( pi_pStr );
}

void
CInputMgr::SetInputPosition( POINT & pi_ptInputPos )
{
	m_ptInputPos.x = pi_ptInputPos.x;
	m_ptInputPos.y = pi_ptInputPos.y;

	m_cIME.SetIMEPosition( m_ptInputPos );
}

void
CInputMgr::DrawText( void )
{
	m_cIME.TextOutput( m_hWnd );
}

void
CInputMgr::DrawCaret( void )
{
	m_cIME.ShowCaret( m_hWnd );
}

void
CInputMgr::SetCaretColor( DWORD pi_dwColor )
{
	m_cIME.SetCaretColor( pi_dwColor );
}

void
CInputMgr::SetTextColor( DWORD pi_dwColor )
{
	m_cIME.SetTextColor( pi_dwColor );
}

BOOL
CInputMgr::IsKoreanMode( void )
{
	return m_cIME.IMEIsKoreanMode( m_hWnd );
}

void
CInputMgr::SetKorean( void )
{
	m_cIME.IMESetKorean( m_hWnd );
}

void
CInputMgr::SetEnglish( void )
{
	m_cIME.IMESetEnglish( m_hWnd );
}

void
CInputMgr::SetToggleLang( void )
{
	m_cIME.IMEToggleLanguage( m_hWnd );
}