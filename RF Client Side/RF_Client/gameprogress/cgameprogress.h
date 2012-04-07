////////////////////////////////////////////////////////////////////////////
//
// CGameProgress Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GAME_PROGRESS_H__
#define __GAME_PROGRESS_H__

/*//////////////////////////////////////////////////////////////////////////

[ CGameProgress ]

//////////////////////////////////////////////////////////////////////////*/
class CGameProgress
{
// < Data Member > ---------------------------------------------------------
protected :
	BOOL		m_bExit;				// 현재의 게임 진행 상태를 벗어날 것인가?
	BOOL		m_bRequestExit;			// m_bExit 요청
	BOOL		m_bIsCreated;			// 객체 생성이 되었는가?
	BOOL		m_bIsLoadedData;		// 데이터를 읽어 들였는가?

// < Member Function > -----------------------------------------------------
public :
	CGameProgress();
	virtual ~CGameProgress();

	virtual void Init( void );
	virtual BOOL Create( void );
	virtual BOOL Destroy( void );

	//----------------------------------------------------------------------

	virtual BOOL	MainLoop( BOOL pi_bIsActive );

	virtual	HRESULT RestoreDeviceObjects( void )	= 0;
	virtual	HRESULT InvalidateDeviceObjects( void )	= 0;

protected :

	virtual HRESULT	FrameMove( void )				= 0;
	virtual HRESULT	Render( void )					= 0;
	virtual BOOL	InputProcess( void )			= 0;

	virtual BOOL	LoadData( void )				= 0;
	virtual BOOL	UnloadData( void )				= 0;

public :
	virtual	LRESULT	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam ) = 0;

	//----------------------------------------------------------------------

	BOOL	IsCreated( void )	{ return m_bIsCreated; }
};

#endif // __GAME_PROGRESS_H__