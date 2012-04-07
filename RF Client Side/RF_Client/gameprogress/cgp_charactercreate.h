////////////////////////////////////////////////////////////////////////////
//
// CGP_CharacterCreate Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GP_CHARACTER_CREATE_H__
#define __GP_CHARACTER_CREATE_H__

#include "CGameProgress.h"

//--------------------------------------------------------------------------
// Game Progress Character Create ( GPCC_ )
#define GPCC_MAX_INPUT_FOCUS		0

#define GPCC_INPUT_END				0xFF

/*//////////////////////////////////////////////////////////////////////////

[ CGP_CharacterCreate ]

//////////////////////////////////////////////////////////////////////////*/
class CGP_CharacterCreate : public CGameProgress
{
// < Data Member > ---------------------------------------------------------
private :
	DWORD	m_dwInputFocus;

// < Member Function > -----------------------------------------------------
public :
	CGP_CharacterCreate();
	~CGP_CharacterCreate();

	void Init( void );
	BOOL Create( void );
	BOOL Destroy( void );

protected :
	HRESULT	FrameMove( void );
	HRESULT	Render( void );
	BOOL	InputProcess( void );

	BOOL	LoadData( void );
	BOOL	UnloadData( void );

public :
	LRESULT	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	HRESULT	RestoreDeviceObjects( void );
	HRESULT	InvalidateDeviceObjects( void );
};

#endif // __GP_CHARACTER_CREATE_H__