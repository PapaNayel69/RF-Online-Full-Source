////////////////////////////////////////////////////////////////////////////
//
// CGP_LogIn Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GP_LOGIN_H__
#define __GP_LOGIN_H__

#include "CGameProgress.h"

//--------------------------------------------------------------------------
// Game Progress LogIn ( GPL_ )
#define GPL_MAX_INPUT_FOCUS			2

#define GPL_INPUT_ID				0
#define GPL_INPUT_PASSWORD			1
#define GPL_INPUT_END				0xFF

#define GPL_MAX_ID_LENGTH			32
#define GPL_MAX_PASSWORD_LENGTH		16

/*//////////////////////////////////////////////////////////////////////////

[ CGP_LogIn ]

//////////////////////////////////////////////////////////////////////////*/
class CGP_LogIn : public CGameProgress
{
// < Data Member > ---------------------------------------------------------
private :
	char		m_pInputString[GPL_MAX_INPUT_FOCUS][32];		// [0] == ID, [1] == Password
	BYTE		m_nMaxStringLength[GPL_MAX_INPUT_FOCUS];		// [0] == ID, [1] == Password

	CIMEEdit	m_cIME;
	CSprite		m_cMouse;

	DWORD		m_dwInputFocus;									// 어디에서 입력을 받고 있는가?
	POINT		m_ptInputFocusPosition[GPL_MAX_INPUT_FOCUS];	// 실제 화면상의 위치

// < Member Function > -----------------------------------------------------
public :
	CGP_LogIn();
	~CGP_LogIn();

	void Init( void );
	BOOL Create( void );
	BOOL Destroy( void );

protected :
	HRESULT	FrameMove( void );
	HRESULT	Render( void );
	BOOL	InputProcess( void );

	BOOL	LoadData( void );
	BOOL	UnloadData( void );

	void	UI_LoadSprite( void );
	void	UI_UnloadSprite( void );

public :
	virtual	HRESULT RestoreDeviceObjects( void );
	virtual	HRESULT InvalidateDeviceObjects( void );

	LRESULT	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};

#endif // __GP_LOGIN_H__