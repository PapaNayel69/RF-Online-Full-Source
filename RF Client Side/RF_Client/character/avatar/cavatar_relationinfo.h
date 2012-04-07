////////////////////////////////////////////////////////////////////////////////
//
// CAvatar Relation Infomation Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __AVATAR_INFO1_H__
#define __AVATAR_INFO1_H__


////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Structure          //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct NPC_TRADE_INFO
{
	DWORD		m_dwCurrentIntroduceNpc;				// 현재 아바타에게 안내중인 상인의 인덱스

	DWORD		m_dwTradeCharType;						// 거래할 사람의 Type ( Player, NPC )
	DWORD		m_dwTradeCharIndex;						// 거래할 사람의 Index
	DWORD		m_dwTradeButtonType;					// 거래할 사람의 Button Type

	DWORD		m_dwPrevTradeCharType;					// 이전에 거래한 사람의 Type ( Player, NPC )
	DWORD		m_dwPrevTradeCharIndex;					// 이전에 거래한 사람의 Index
	DWORD		m_dwPrevTradeButtonType;				// 이전에 거래한 사람의 Button Type	
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Structure          //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 맵간 이동에 쓰임
struct PORTAL_INFO;
struct MOVEMENT_BETWEEN_MAP_INFO
{
	enum { MMS_NONE, MMS_PREV_LOAD_DATA, MMS_LOAD_DATA, MMS_AFTER_LOAD_DATA, MMS_BEGIN_NEW_MAP };

	BYTE			m_byMoveType;						// 맵 이동 타입 ( GST_PORTAL or GST_REBIRTH 등 )
	BYTE			m_byMoveStep;						// 맵 이동 단계
	DWORD			m_dwMoveStepStartTime;
	DWORD			m_dwMoveDelay;						// 맵 이동간에 쓰일 딜레이

	//BYTE			m_byServerIP[4];					// 이동할 서버의 IP
	//DWORD			m_dwServerPort;						// 이동할 서버의 Port
	//BYTE			m_byMapIndex;						// 이동할 맵의 Index
	Vector3f		m_vecPosition;						// 이동할 위치

	BOOL			m_bIsMoved;							// 포탈을 이동했는가?

	PORTAL_INFO *	m_pEnterPortalInfo;					// 아바타가 들어가는 포탈의 데이터
	PORTAL_INFO *	m_pExitPortalInfo;					// 아바타가 나오는 포탈의 데이터

	BYTE			m_byEnterMapIndex;					// 아바타가 현재 있는 맵의 인덱스
	BYTE			m_byExitMapIndex;					// 아바타가 이동할 맵의 인덱스

	//--------------------------------------------------------------------------

	MOVEMENT_BETWEEN_MAP_INFO() { Init(); }
	void	Init( void );

	void	FrameMove( void );

	BOOL	MoveToPortal_Request( BYTE pi_byPortalIndex );
	void	MoveToPortal_Result( BOOL pi_byMoveType );	// 맵 이동 ( 포탈 or 부활 양쪽 모두 사용 )
	BOOL	MoveToPortal_Result_ToOtherServer( void );

	BOOL	MoveToDungeon_Request( CGroundItem * pi_pDungeonItem );	// 던젼 이동

private :

	void	MoveToPortal_Result_PrevLoadData( void );
	void	MoveToPortal_Result_LoadData( void );
	void	MoveToPortal_Result_AfterLoadData( void );
	void	MoveToPortal_Result_BeginNewMap( void );

	void	SetEffectInfo( void );
};

typedef	MOVEMENT_BETWEEN_MAP_INFO	MOVEMAP_INFO;

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Structure          //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "../../UserInterface/GameWindow/Dungeon/GUIDungeon.h"

enum { DET_CLEAR, DET_TIMEOVER, DET_DEATH, DET_PORTAL, DET_CHEAT, MAX_DET_TYPE };	// dungeon exit type

class CDungeonInfo
{
	BOOL	m_bIsInDungeon;

	DWORD	m_dwEnterTime;
	DWORD	m_dwExitTime;
	BYTE	m_byDungeonExitType;

	DWORD	m_dwMonsterKillNum;
	DWORD	m_dwTargetMonsterTotalNum;
	DWORD	m_dwTargetMonsterKillNum;

	BOOL	m_bIsEnableHint;

	DUNGEON_ITEM_DATA *	m_pDungeonData;

	CGUIKillingCounter	m_uiKillCounterWindow;
	CGUIMissionStateWin	m_uiMissionDescWindow;

public :

	CDungeonInfo();

			void	InitKillCounterWindow( DWORD pi_dwWindowID, int pi_nPosX, int pi_nPosY );
			void	InitMissionDescWindow( DWORD pi_dwWindowID );
			void	OpenMissionDescWindow( BYTE pi_byType );
			void	Open( DWORD pi_dwWindowID );
			void	Close( DWORD pi_dwWindowID );
			BOOL	IsOpen( DWORD pi_dwWindowID );

			DWORD	OpenPortal( BYTE pi_byMapMoveType );
			void	ClosePortalOrExitDungeon( BYTE pi_byMapMoveType );
			void	EnterDungeon( BYTE pi_byMapMoveType );
			void	ExitDungeon( void );

			void	SetEnterDungeonTime( DWORD pi_dwRemainTime );
			DWORD	GetRemainTime( void );
			void	SetDungeonData( CGroundItem * pi_pDungeonItem );

	inline	void	SetInDungeon( BOOL pi_bFlag )				{ m_bIsInDungeon = pi_bFlag; }
	inline	BOOL	IsInDungeon( void )							{ return m_bIsInDungeon; }
	inline	void	SetDungeonExitType( BYTE pi_byType )		{ m_byDungeonExitType = pi_byType; }
	inline	BYTE	GetDungeonExitType( void )					{ return m_byDungeonExitType; }
	inline	void	SetMonsterKillNum( DWORD pi_dwNum )			{ m_dwMonsterKillNum = pi_dwNum; }
	inline	DWORD	GetMonsterKillNum( void )					{ return m_dwMonsterKillNum; }
	inline	DWORD	GetTargetMonsterTotalNum( void )			{ return m_dwTargetMonsterTotalNum; }
			void	SetTargetMonsterKillNum( DWORD pi_dwNum );
	inline	DWORD	GetTargetMonsterKillNum( void )				{ return m_dwTargetMonsterKillNum; }
};

//------------------------------------------------------------------------------

extern CDungeonInfo g_stDungeonInfo;
#define _GetDungeonInfo() ( &g_stDungeonInfo )

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __AVATAR_INFO1_H__