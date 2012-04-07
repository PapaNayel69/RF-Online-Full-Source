////////////////////////////////////////////////////////////////////////////////
//
// CTargetMgr Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __TARGET_MGR_H__
#define __TARGET_MGR_H__

#include "../../DefineMessage.h"

#include "../../UserInterface/GameWindow/GUISelectedCharWin.h"
#include "../../UserInterface/GameWindow/GUISelectedCharMenu.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

enum // TRAGET_MODE_TYPE
{
	TMT_SELECT_TARGET,
	TMT_FREE_TARGET,
	MAX_TRAGET_MODE_TYPE
};

enum // PURSUIT_TARGET_TYPE
{
	PTT_NONE		= -1,
	PTT_PLAYER		= CTI_PLAYER,
	PTT_ENEMY		= CTI_MONSTER,
	PTT_MERCHANT	= CTI_NPC,
	PTT_GROUND_ITEM	= 4,
	PTT_ORE			= 5,
	MAX_PURSUIT_TARGET_TYPE
};

enum // PURSUIT_PURPOSE_TYPE
{
	PPT_NONE		= -1,
	PPT_PURSUIT		= 0,
	PPT_NORMAL_ATTACK,
	PPT_SKILL_FORCE,
	PPT_TRADE,
	PPT_LOOTING,
	PPT_MINING,
	MAX_PURSUIT_PURPOSE_TYPE
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CTarget
{
	DWORD		m_dwTargetID[2];

public :

	CTarget();

	void		SetTarget( CCharacter * pi_pTarget );
	void		SetTarget( BYTE pi_byCharTypeID, DWORD pi_dwTargetIndex );
	CCharacter*	GetTarget( void );
	BOOL		IsEqual( CCharacter * pi_pCharacter );
	BOOL		IsEmpty( void );
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

class CTargetItem
{
	DWORD		m_dwTargetItemIndex;

public :

	CTargetItem();

	void			SetTargetItem( CGroundItem * pi_pTargetItem );
	CGroundItem *	GetTargetItem( void );
	BOOL			IsEqual( CGroundItem * pi_pGroundItem );
	BOOL			IsEmpty( void );
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

typedef vector< CTarget >			TARGET_LIST;
typedef vector< CTarget >::iterator	TARGET_LIST_ITER;

#define	MAX_TARGET_NUM_IN_LIST		( 10 )

class CGUIScreen;

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CTargetMgr
{
	CTarget			m_Target;					// 캐릭터가 선택한 타겟 ( target 모드에서 사용됨 )
	CTarget			m_InternalTarget;			// 내부 상태로만 유지되는 타겟 ( free target 모드에서 사용됨 )
	CTargetItem		m_TargetItem;				// 타겟 아이템
	CTarget			m_LatestTarget;				// 가장 최근에 아바타가 선택했던 타겟
	BYTE			m_byTargetMode;				// 타겟 모드

	struct PURSUIT_INFO
	{
		int			m_nTargetType;				// 추적하는 타겟의 타입
		int			m_nPurposeType;				// 추적 목적
		DWORD		m_dwPursuitTime;			// 타겟 추적을 시작한 시간
	} PursuitInfo;

	struct TARGET_LIST_INFO
	{
		TARGET_LIST	m_listTarget;				// 아바타가 지정한 타겟 리스트
		int			m_nCurTargetNo;				// 아바타가 선택한 타겟의 소켓 번호
	} TargetList;

	CGUISelectedCharWin		m_uiTargetInfoBar;
	CGUISelectedMobInfoWin	m_uiTargetInfoWindow;
	CGUISelectedCharMenu	m_uiPlayerPopupMenu;

public :

	CTargetMgr();
	~CTargetMgr();

			void			InitTargetInfoBar( DWORD pi_dwID, CGUIScreen * pi_pUiScreen );
			void			InitTargetInfoWindow( DWORD pi_dwID, CGUIWindow * pi_pDockingWindow, CGUIScreen * pi_pUiScreen );
			void			InitPlayerPopupMenu( DWORD pi_dwID, CGUIScreen * pi_pUiScreen );

			void			FrameMove( void );
			void			Render( void );

			void			SetTarget( CCharacter * pi_pTarget );
			CCharacter *	GetTarget( void );
			BOOL			IsEqualTarget( CCharacter * pi_pCharacter );
			BOOL			IsEmptyTarget( void );
			void			ChangeTargetFromLatestTarget( void );
			void			ChangeTargetFromNextTarget( void );

			void			SetTargetItem( CGroundItem * pi_pTargetItem );
			CGroundItem *	GetTargetItem( void );
			BOOL			IsEqualTargetItem( CGroundItem * pi_pGroundItem );

protected :

			void			SetTarget_OpenTargetUI( CCharacter * pi_pTarget );
			void			SetTarget_SetEnemyNpcUI( CCharacter * pi_pTarget );
			void			SetTarget_SetPlayerUI( CPlayer * pi_pTarget );
			void			SetTarget_SetMerchantUI( CNpc * pi_pTarget );

public :

			void			SetTargetTextColor( DWORD pi_dwColor );

			BYTE			GetResultOfCombatPowerComparisonWithAvatar( CCharacter * pi_pTarget );

	//--------------------------------------------------------------------------

			void			SetPursuitInfo( int pi_nTarget, int pi_nPurpose );
			void			ResetPursuitInfo( void );
	inline	void			SetPursuitTargetType( int pi_nTargetType )		{ PursuitInfo.m_nTargetType = pi_nTargetType; }
	inline	void			SetPursuitPurpose( int pi_nPurposeType )		{ PursuitInfo.m_nPurposeType = pi_nPurposeType; }
	inline	int				GetPursuitTargetType( void )					{ return PursuitInfo.m_nTargetType; }
	inline	int				GetPursuitPurpose( void )						{ return PursuitInfo.m_nPurposeType; }
	inline	BOOL			IsEqualPursuitTargetType( int pi_nTargetType )	{ return ( PursuitInfo.m_nTargetType == pi_nTargetType ); }
	inline	BOOL			IsEqualPursuitPurpose( int pi_nPurposeType )	{ return ( PursuitInfo.m_nPurposeType == pi_nPurposeType ); }

	//--------------------------------------------------------------------------

			void			AddTargetToList( BYTE pi_bySocketNo, CCharacter * pi_pTarget );
			void			AddTargetToList( BYTE pi_bySocketNo, BYTE pi_byCharTypeID, DWORD pi_dwTargetIndex );
			CCharacter *	GetTargetInList( BYTE pi_bySocketNo );
			CCharacter *	GetNextTargetInList( BOOL pi_bBackward );
			void			SetCurrentTargetSocketNo( BYTE pi_bySocketNo )	{ TargetList.m_nCurTargetNo = pi_bySocketNo; }

	//--------------------------------------------------------------------------

			void			OpenWindow( DWORD pi_dwWindowID );
			void			CloseWindow( DWORD pi_dwWindowID );
			void			CloseAllWindow( void );
			BOOL			IsOpenableWindow( DWORD pi_dwWindowID );

protected :

			void			SetPopupMenu( void );
			void			SetPopupMenu_Unit( CPlayer * pi_pUnit );
			void			SetPopupMenu_Player( CPlayer * pi_pPlayer );
			void			SetPopupMenu_GuardTower( CGuardTower * pi_pGuardTower );
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

extern CTargetMgr * g_pTargetMgr;
#define _GetTargetMgr() g_pTargetMgr

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CNextActionMgr
{
	DWORD			m_dwNextActionID;				// 다음에 수행될 액션
	CTarget			m_NextTarget;					// m_dwNextActionID 에 대응하는 타겟
	CTargetItem		m_NextTargetItem;				// m_dwNextActionID 에 대응하는 아이템
	Vector3f		m_vecNextTargetPosition;		// m_dwNextActionID 에 대응하는 위치
	CSFItem	*		m_pNextSFItem;					// m_dwNextActionID 에 대응하는 SF
	DWORD			m_dwNextExtraData;				// m_dwNextActionID 에 Extra data ( 특별히 정해진 의미가 없으므로 상황에 알맞게 사용한다. )

	BOOL			m_bClearNextActionID;			// NextActionID를 클리어 할 것인가?

public :

	CNextActionMgr();

	inline	void			SetNextAction( DWORD pi_dwActionID )		{ m_dwNextActionID = pi_dwActionID; m_bClearNextActionID = TRUE; }
	inline	DWORD			GetNextAction( void )						{ return m_dwNextActionID; }
	inline	BOOL			IsEqualNextAction( DWORD pi_dwActionID )	{ return ( m_dwNextActionID == pi_dwActionID ); }
			void			SetEmpty( BOOL pi_bIgnoreClearFlag );
	inline	BOOL			IsEmpty( void )								{ return ( m_dwNextActionID == ID_INVALID ); }

	inline	void			SetNextTarget( CCharacter * pi_pTarget )	{ m_NextTarget.SetTarget( pi_pTarget ); }
	inline	CCharacter *	GetNextTarget( void )						{ return m_NextTarget.GetTarget(); }

	inline	void			SetNextTargetItem( CGroundItem * pi_pItem )	{ m_NextTargetItem.SetTargetItem( pi_pItem ); }
	inline	CGroundItem *	GetNextTargetItem( void )					{ m_NextTargetItem.GetTargetItem(); }

	inline	void			SetNextSF( CSFItem * pi_pSFItem )			{ m_pNextSFItem = pi_pSFItem; }
	inline	CSFItem *		GetNextSF( void )							{ return m_pNextSFItem; }

	inline	void			SetNextExtraData( DWORD pi_dwExtraData )	{ m_dwNextExtraData = pi_dwExtraData; }
	inline	DWORD			GetNextExtraData( void )					{ return m_dwNextExtraData; }

	inline	void			SetNextTargetPosition( Vector3f pi_vecNextTargetPos )
							{ memcpy( m_vecNextTargetPosition, pi_vecNextTargetPos, sizeof( float ) * 3 ); }
	inline	void			GetNextTargetPosition( Vector3f po_vecNextTargetPos )
							{ memcpy( po_vecNextTargetPos, m_vecNextTargetPosition, sizeof( float ) * 3 ); }
	inline	float *			GetNextTargetPosition( void )				{ return m_vecNextTargetPosition; }

	inline	void			ClearNextActionID( BOOL pi_bFlag )			{ m_bClearNextActionID = pi_bFlag; }
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

extern CNextActionMgr * g_pNextActionMgr;
#define _GetNextActionMgr() g_pNextActionMgr

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __TARGET_MGR_H__