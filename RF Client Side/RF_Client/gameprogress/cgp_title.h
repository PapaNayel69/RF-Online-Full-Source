////////////////////////////////////////////////////////////////////////////
//
// CGP_Title Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GP_TITLE_H__
#define __GP_TITLE_H__

#include "CGameProgress.h"

/*//////////////////////////////////////////////////////////////////////////

[ CGP_Title ]

//////////////////////////////////////////////////////////////////////////*/
class CGP_Title : public CGameProgress
{
// < Data Member > ---------------------------------------------------------
private :

// < Member Function > -----------------------------------------------------
public :
	CGP_Title();
	~CGP_Title();

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

#endif // __GP_TITLE_H__