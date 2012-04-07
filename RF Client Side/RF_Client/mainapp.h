////////////////////////////////////////////////////////////////////////////
//
// CMainApp Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __MAINAPP_H__
#define __MAINAPP_H__

////////////////////////////////////////////////////////////////////////////
// Game Progress ID ( GPI_ )
#define MAX_GAME_PROGRESS	3
#define GPI_INVALID			0xFF
#define GPI_TITLE			0x0
#define GPI_LOGIN			0x1
#define GPI_MAIN_GAME		0x2
//#define GPI_CHAR_CREATE		0x3

////////////////////////////////////////////////////////////////////////////
//
class CMainApp : public R3D3dWrapper
{
// < Data Member > ---------------------------------------------------------
private :
	CLand					m_cLand;
	CCamera					m_cCamera;
	CCharacterMgr			m_cCharMgr;
	CNetworkMgr				m_cNetworkMgr;

	CCharResDataMgr			m_cCharResDataMgr;

	//----------------------------------------------------------------------

	BOOL					m_bIsActive;
	float					m_fVersion;					// 클라이언트 버젼

	CGameProgress *			m_pGameProgress;			// 현재의 게임 진행 객체를 가리키는 포인터
	BYTE					m_byGameProgressID;			// 게임 진행 단계 ID ( title, login, game, etc... )

	//----------------------------------------------------------------------

	BOOL					m_bCreatedAllObject;		// 모든 오브젝트를 생성했는가?
	BOOL					m_bLoadedAllData;			// 모든 데이터를 로드했는가?
	BOOL					m_bRequestQuitProgram;		// 프로그램 종료를 요청했는가?
	BOOL					m_bRequestUploadInventory;	// 인벤토리 업로드를 요청했는가?
	DWORD					m_dwRequestQuitTime;		// 프로그램 종료를 요청한 시간
	DWORD					m_dwQuitWaitingTime;		// 프로그램 종료 대기 시간
	BOOL					m_bIsQuit;					// 프로그램 종료 요청을 처리해서 프로그램 종료 실행

	BOOL					m_bRequestLogout;			// 캐릭터 선택 화면으로 이동 요청

	//----------------------------------------------------------------------

	BOOL					m_bIsObserver;				// 현재 클라이언트가 옵져버인가?

	//----------------------------------------------------------------------
	// User Interface
	// ui sprite manager
	CSpriteMgr 				m_cSpriteMgr;
	CInputMgr				m_cInputMgr;
	CMouseDev 				m_cMouseDev;

	CGUIScreen 				m_cScreen;			
	// User Interface
	//----------------------------------------------------------------------

// < Member Function > -----------------------------------------------------
protected :
	HRESULT RestoreDeviceObjects();
	HRESULT InvalidateDeviceObjects();

	BOOL	LoadData();
	BOOL	UnloadData();

public :
	CMainApp();
	~CMainApp();

	BOOL	Create( void );
	BOOL	Destroy( void );

protected :
	void 	Create_GameProgress( BYTE pi_byGameProgressID );

public :
	BOOL	GameMainLoop( void );
	BOOL	IsReadyToStart( void );

	BOOL	InputProcess( void );

	LRESULT	MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	BOOL	RequestQuitProgram( DWORD pi_dwQuitWaitingTime = ID_INVALID, char * pi_pMsg = NULL );

	BOOL	GetDataFromLauncher( void );

	//--------------------------------------------------------------------------

	inline	BOOL	IsActive( void )							{ return m_bIsActive; }

	inline	void	SetVersion( float pi_fVersion )				{ m_fVersion = pi_fVersion; }
	inline	float	GetVersion( void )							{ return m_fVersion; }

	inline	void	SetGameProgressID( BYTE pi_byID )			{ m_byGameProgressID = pi_byID; }
	inline	BYTE	GetGameProgressID( void )					{ return m_byGameProgressID; }

	inline	void	SetRequestLogout( BOOL pi_bFlag )			{ m_bRequestLogout = pi_bFlag; }
	inline	BOOL	IsRequestLogout( void )						{ return m_bRequestLogout; }

			void	SetSystemMousePos( int pi_nX, int pi_nY );
};

#endif // __MAINAPP_H__