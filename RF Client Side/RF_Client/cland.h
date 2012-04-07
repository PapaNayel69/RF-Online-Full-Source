////////////////////////////////////////////////////////////////////////////
//
// CLand Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __LAND_H__
#define __LAND_H__

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define MAX_MAP				( 100 )			// 게임내 맵의 최대 갯수
#define MAX_PORTAL			( 50 )			// 한 맵에서 포탈의 최대 갯수
#define MAX_MINE			( 50 )			// 한 맵에서 광산의 최대 갯수
#define MAX_AREA			( 50 )			// 한 맵에서 영역정보의 최대 갯수
#define MAX_LINKED_PORTAL	MAX_PORTAL		// 하나의 포탈에 연결된 포탈의 최대 갯수

//------------------------------------------------------------------------------

#define PTI_ROAD			0
#define PTI_WAYPOINT		1

//------------------------------------------------------------------------------

struct MINE_INFO
{
	BYTE		byMapIndex;					// 광산이 속한 맵 인덱스
	BYTE		byIndex;					// 광산의 인덱스

	float		vecMin[3], vecMax[3];		// 포탈의 영역
};

struct PORTAL_INFO
{
	BYTE		byMapIndex;					// 포탈이 속한 맵 인덱스
	BYTE		byIndex;					// 포탈의 인덱스
	BYTE		byPortalType;				// 포탈의 타입 ( 길 or 웨이 포인트 )

	BOOL		bIsMovable;					// 이동 가능한 포탈인가?
	BYTE		byUsableRace[5];			// 사용 가능 종족

	char		pName[32];					// 포탈의 이름
	char		pDesc[64];					// 포탈의 설명
	Vector3f	vecMin, vecMax;				// 포탈의 영역

	BYTE		byLinkedPortalNum;			// 현재 포탈과 연결된 포탈의 갯수
	BYTE		byLinkedPortal[MAX_LINKED_PORTAL][2];	// 현재 포탈과 연결된 포탈의 인덱스 [0] : Map Index, [1] : Portal Index

	BOOL		bIsUsable;					// 포탈을 사용할 수 있는가?
};

struct MAP_AREA_INFO
{
	DWORD		dwIndex;
	RECT		recArea;
	char		pName[64];
};

struct MAP_INFO
{
	BYTE			byIndex;					// 맵의 인덱스
	char			pName[32];					// 맵 이름
	char			pName_UI[32];				// 맵 이름 ( UI에 표시될 이름 )
	char			pFileName[128];				// 맵의 파일 이름
	char			pMinimapFileName[128];		// 맵의 미니맵 파일 이름
	WORD			wMinimapAngle;				// 미니맵의 각도

	BYTE			byPortalNum;				// 맵내의 포탈 갯수
	PORTAL_INFO		pPortalInfo[MAX_PORTAL];	// 포탈 정보

	BYTE			byMineNum;					// 맵내의 광산 갯수
	MINE_INFO		pMineInfo[MAX_MINE];		// 광산 정보

	BYTE			byAreaNum;					// 맵내의 영역 정보 갯수
	MAP_AREA_INFO	pAreaInfo[MAX_AREA];		// 영역 정보
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CLand
{
// < Data Member > -------------------------------------------------------------
private :
	CLevel	*	m_pLevel;					// 실제 level 데이터

	MAP_INFO	m_pMapInfo[MAX_MAP];		// 포탈 정보

	BYTE		m_byMaxMapNum;				// 게임내 맵의 최대 갯수
	BYTE		m_byMapIndex;				// 현재 사용중인 맵의 인덱스
	BYTE		m_byPortalIndex;			// 현재 사용중인 포탈 인덱스

// < Member Function > ---------------------------------------------------------
public :
	CLand();
	~CLand();

	void	Init( void );
	BOOL	Create( void );
	BOOL	Destroy( void );

	//--------------------------------------------------------------------------

	BOOL	LoadLevel( char * pi_pLevelName );
	BOOL	LoadLevel( BYTE pi_byMapIndex = 0xFF );
	BOOL	UnloadLevel( void );
	BOOL	Render( Vector3f & pi_vecPos );
	BOOL	RenderAlpha( Vector3f & pi_vecPos );

	void	OpenMinimapWindow( char * pi_pMapName, char * pi_pItemName );
	void	UI_OpenMoveMapWindow( void );
	void	UI_SetRadar( void );

	void	RenderInfo( void );		// 임시 함수 - 맵의 정보를 출력

	BOOL	LoadData( void );

private :

	void	UI_SetMinimap( BYTE pi_byMapIndex = 0xFF );

public :

	//--------------------------------------------------------------------------

	inline	CLevel *		GetLevel( void )						{ return m_pLevel; }

	inline	void			SetMaxMapNum( BYTE pi_byMaxNum  )		{ m_byMaxMapNum = pi_byMaxNum; }
	inline	BYTE			GetMaxMapNum( void )					{ return m_byMaxMapNum; }
	inline	void			SetMapIndex( BYTE pi_byMapIndex  )		{ m_byMapIndex = pi_byMapIndex; }
	inline	BYTE			GetMapIndex( void )						{ return m_byMapIndex; }
	inline	void			SetPortalIndex( BYTE pi_byPortalIndex )	{ m_byPortalIndex = pi_byPortalIndex; }
	inline	BYTE			GetPortalIndex( void )					{ return m_byPortalIndex; }

	inline	BYTE			GetMaxPortalNum( BYTE pi_byMapIndex )	{ return m_pMapInfo[pi_byMapIndex].byPortalNum; }

	inline	MAP_INFO *		GetCurMapInfo( void )					{ return &m_pMapInfo[m_byMapIndex]; }
	inline	MAP_INFO *		GetMapInfo( BYTE pi_byIndex )			{ return &m_pMapInfo[pi_byIndex]; }

	inline	PORTAL_INFO *	GetCurPortalInfo( void )
							{ return &( m_pMapInfo[m_byMapIndex].pPortalInfo[m_byPortalIndex] ); }
	inline	PORTAL_INFO *	GetPortalInfo( BYTE pi_byMapIndex, BYTE pi_byPortalIndex )
							{ return &( m_pMapInfo[pi_byMapIndex].pPortalInfo[pi_byPortalIndex] ); }

	inline	BYTE			GetMaxMineNum( BYTE pi_byMapIndex )		{ return m_pMapInfo[pi_byMapIndex].byMineNum; }
	inline	MINE_INFO *		GetMineInfo( BYTE pi_byMapIndex, BYTE pi_byMineIndex )
							{ return &( m_pMapInfo[pi_byMapIndex].pMineInfo[pi_byMineIndex] ); }
};

#endif // __LAND_H__