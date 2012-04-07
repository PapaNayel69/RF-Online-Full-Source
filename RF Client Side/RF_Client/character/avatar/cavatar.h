////////////////////////////////////////////////////////////////////////////////
//
// CAvatar Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __CHAR_AVATAR_H__
#define __CHAR_AVATAR_H__

#include "../Player/CPlayer.h"
#include "CAvatar_RelationInfo.h"
#include "CAvatar_RelationInfo2.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Character Move Key
#define CMK_UP				0x00000048	//0x00000011
#define CMK_DOWN			0x00000050	//0x0000001F
#define CMK_LEFT			0x0000004b	//0x0000001E
#define CMK_RIGHT			0x0000004d	//0x00000020
//#define CMK_LEFT_ROT			0x00000010
//#define CMK_RIGHT_ROT			0x00000012

#define	CMK_SHIFT			0x0000002A

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

typedef int RESULT;

enum	// ATTACK_RESULT_CODE
{
	ARC_FAIL_MOVE_TO_TARGET = 0, ARC_FAIL_STOP = 1, ARC_SUCCESS = 2
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

class	CGUIObject;
class	CMonster;

struct	QUEST_INFO;
struct	QBI_BASE_DATA;

class	CBoardObject;

class	CNetMsgProcessor_Combat;
class	CNetMsgProcessor_Unit;

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define MAX_AVATAR_CLASS_NUM	( 4 )						// 아바타가 전직할 수 있는 최대 클래스 개수

class CAvatar : public CPlayer
{
protected :

	CEquipItem		m_pDefaultItem[MAX_DEFAULT_PART];

	float			m_fExp;									// 플레이어 경험치 ( 백분율 )
	DWORD			m_dwPrevMaxHP;
	DWORD			m_dwPrevHP;
	DWORD			m_dwPrevMaxFP;
	DWORD			m_dwPrevFP;
	DWORD			m_dwPrevMaxSP;
	DWORD			m_dwPrevSP;
	DWORD			m_dwGold;
	DWORD			m_dwDalant;

	float			m_fTexRate;								// 세금 %
	DWORD			m_dwExchangeRate;						// 환율 ( 1골드 당 달란트 )
	DWORD			m_dwExchangeManageValue;				// 환율 관리값

	DWORD			m_dwClassDTIndex[MAX_AVATAR_CLASS_NUM];	// 아바타의 클래스의 DT Index ( [0] 현재 클래스, [1,2,3] 이전 클래스 )
	char			m_pClassName[MAX_NAME_LENGTH];			// 아바타의 현재 클래스명

	//--------------------------------------------------------------------------

	DWORD			m_dwExpertness[MAX_EXPERTNESS_VALUE_TYPE];// 숙련도의 성공 횟수
	BYTE			m_byUsingSFCount;						// 스킬/포스 사용 횟수 ( 서버에서 BYTE 형으로 사용함 )( 보조형 스킬/포스 시리얼로 사용한다. )

	//--------------------------------------------------------------------------

	BYTE			m_byAttackMode;							// 아바타의 공격 모드 ( 노멀, 스킬, 포스 )
	BOOL			m_bEnableAutoAttack;					// 자동 공격
	BOOL			m_bEnableAutoChaseTarget;				// 자동 추적
	BOOL			m_bEnableAutoCounterAttack;				// 자동 반격
	DWORD			m_dwMoveStartTime;						// 아바타가 움직이기 시작한 시간
	DWORD			m_dwPrevAttackTime;						// 아바타의 이전 공격 타임
	DWORD			m_dwPrevAttackDelay;					// 아바타의 이전 공격 딜레이
	BYTE			m_byPrevAttackType[2];					// 아바타의 이전 공격 형태 ( [0] = 노멀/스킬/포스, [1] = 스킬/포스의 DT Index )
	BOOL			m_bIsInstantAction;						// 현재 아바타가 하는 행동이 일시적인 것인가?

	//--------------------------------------------------------------------------

	float			m_fProfitOfTrade;						// 상점과의 거래시 이득 수치
	BOOL			m_bEnable_SendMsgToRace;				// 같은 종족 전체에 메세지를 보낼 수 있는가?
	short			m_nItemEquipLevel;						// 아이템 장착 레벨 ( + )

	DWORD			m_dwPotionEffectEndTime;				// 포션 사용시 지속 효과가 끝날 시간
	POTION_DATA *	m_pUsedPotionData;						// 사용한 포션의 데이터

	//--------------------------------------------------------------------------

	BOOL			m_bIsRequestChangeToCombatMode;			// 전투 모드로 변경을 요청했는가?
	BOOL			m_bIsArrivedTargetPosition;				// 타겟 포지션에 도착했는가?
	Vector3f		m_vecLastPath;							// 아바타가 이동해야 할 최종 위치

	BOOL			m_bMoveByKeyboard;						// 키보드로 이동중인가?
	Vector3f		m_vecStartPos_MoveByKeyboard;			// 키보드로 이동을 시작한 처음 위치

	BOOL			m_bShowSFDelayMsg;						// 자동 공격시 스킬/포스 딜레이 메세지를 표시했는가?

	//========================================================================//
	//                                Boundary                                //
	//========================================================================//

	BOOL			m_bIsEnableTeleport;					// 아무 곳에서나 맵간 이동을 할 수 있는가?
	BOOL			m_bShowRebirthMessageBox;				// 부활창을 보여주고 있는가?

	char			m_pSystemMsg[256];
	DWORD			m_dwReciveSystemMsgTime;

	char			m_pSFUsingMsg[256];
	DWORD			m_dwReciveSFUsingMsgTime;

	//========================================================================//
	//                                Boundary                                //
	//========================================================================//

	NPC_TRADE_INFO *			m_pNpcTradeInfo;			// NPC와의 거래 정보
	MOVEMAP_INFO *				m_pMoveMapInfo;				// 맵간 이동에 쓰이는 정보
	QUEST_INFO *				m_pTutorialInfo;			// 튜토리얼 정보
	AFTER_IMAGE_INFO *			m_pAfterImageInfo;			// 잔상 정보
	CHAT_INFO *					m_pChatInfo;				// 채팅 정보
	OBSERVER_INFO *				m_pObserverInfo;			// 옵져버 정보

	CNetMsgProcessor_Combat *	m_pNMP_Combat;
	CNetMsgProcessor_Unit *		m_pNMP_Unit;

	//========================================================================//
	//                                Boundary                                //
	//========================================================================//

public :

	CAvatar();
	virtual ~CAvatar();

			void		CreateBoard( void );

	virtual	CCharacter*	New( void );
	virtual	CCharacter*	Clone( void );

	//--------------------------------------------------------------------------

			BOOL		Render( void );									// 렌더링
			BOOL		Render_IsFPSCameraMode( void );
			void		Render_ProcessAlpha( void );
			void		RenderText( void );
			void		RenderInformation( void );
			BOOL		Animation( DWORD pi_dwAniFrame = -1 );			// 애니메이션 ( 본 or 메시 )
			void		FrameMove( void );								// 캐릭터의 움직임
			LRESULT		MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

protected :

			void		FrameMove_Move( void );

			void		FrameMove_Move_Stop( void );
			BOOL		FrameMove_ProcessNextAction( void );
			void		FrameMove_ProcessNextAction_Move( void );
			void		FrameMove_ProcessNextAction_NormalAttack( void );
			BOOL		FrameMove_ProcessNextAction_SkillForce( void );
			void		FrameMove_ProcessNextAction_Mining( void );
			void		FrameMove_ProcessNextAction_SelectCharacter( void );
			void		FrameMove_ProcessPursuit( void );
			void		FrameMove_ProcessPursuit_Pursuit( CCharacter * pi_pTarget );
			void		FrameMove_ProcessPursuit_AutoAttack( CCharacter * pi_pTarge );

			void		FrameMove_Move_Moving( void );
			void		FrameMove_Move_Moving_CloseWindow( void );
			void		FrameMove_Move_Moving_ProcessAfterShadow( void );
			void		FrameMove_Move_Moving_PursuitEnemy( void );
			//void		FrameMove_Move_Moving_PursuitEnemy_Pursue( CCharacter * pi_pTarget );
			void		FrameMove_Move_Moving_PursuitMerchant( void );
			void		FrameMove_Move_Moving_PursuitItem( void );
			void		FrameMove_Move_Moving_PursuitOre( void );
			void		FrameMove_Combat( void );
			void		FrameMove_SkillForce( void );
			void		FrameMove_Death( void );
			void		FrameMove_Item( void );

			void		FrameMove_EveryFrame( void );

			void		FrameMove_Portal( void );
			void		FrameMove_Quest( void );

			void		FrameMove_FreeTargetMode( void );
			void		FrameMove_ItemBoard( void );
			void		FrameMove_Target( void );
			void		FrameMove_CheckUsedPotion( void );

			float		PositionMove( Vector3f pi_vecTargetPos );

public :

			BOOL		SetEffectInfo( DWORD pi_dwActionID );
			void		SetAction( DWORD pi_dwActionID, BOOL pi_bCheckActionDuplication = TRUE );

protected :

			void		SetAction_Move( DWORD pi_dwActionID );
			void		SetAction_Combat( DWORD pi_dwActionID );
			void		SetAction_Skill( DWORD pi_dwActionID );
			void		SetAction_Force( DWORD pi_dwActionID );
			void		SetAction_Death( DWORD pi_dwActionID );

	//========================================================================//
	//                                Boundary                                //
	//========================================================================//
	// Behavior

public :

			void		RequestMoveByKeyboard( DWORD pi_dwKeyType );
			void		RequestStopByKeyboard( void );

			BOOL		ProcessGroundItemForPickUp( CGroundItem * pi_pGroundItem );
			BOOL		EnterToDungeonPortal( CGroundItem * pi_pGroundItem );

			BOOL		MountUnitRequest( CPlayer * pi_pUnit );
			void		MountUnitResult( void );
			BOOL		DismountUnitRequest( void );
			void		DismountUnitResult( void );

			void		SetUnitBulletNum( BYTE pi_byAttackParts, DWORD pi_dwBulletNum );

	//========================================================================//
	//                                Boundary                                //
	//========================================================================//
	// Combat

			RESULT		Attack( BYTE pi_byAttackType, CCharacter * pi_pTarget );
			RESULT		NormalAttack( CCharacter * pi_pTarget );
			BOOL		UseSkillForce_Request( CCharacter * pi_pTarget );
			BOOL		CheckAvatarConditionForCast( CSFItem * pi_pSFItem );

			BOOL		MoveToTarget( CResObject * pi_pResObject, float pi_fDistance );

protected :

			RESULT		CheckTargetConditionForAttack( CCharacter * pi_pTarget );
			RESULT		CheckAvatarConditionForAttack( void );
			RESULT		CheckDistanceAndDelayForAttack( CCharacter * pi_pTarget );

			BOOL		UseSkillForce_Request_PreCheck( CCharacter * pi_pTarget );
			BOOL		UseSkillForce_Request_CheckResurrectionSF( BYTE pi_bySFType, SF_DATA * pi_pSFData, CCharacter * pi_pTarget );
			void		UseSkillForce_Request_SetTarget( CCharacter * pi_pTarget );
			void		UseSkillForce_Request_SendNetMessage( CSFItem * pi_pSFItem, CCharacter * pi_pTarget );
			void		UseSkillForce_Request_ProcessEtc( CSFItem * pi_pSFItem, CCharacter * pi_pTarget );

			BOOL		CheckTargetConditionForCast( SF_DATA * pi_pSFData, CCharacter * pi_pTarget );
			void		CheckTargetConditionForCast_Anywhere( CCharacter * pi_pTarget );
			void		CheckTargetConditionForCast_Map( CCharacter * pi_pTarget );
			BOOL		CheckTargetConditionForCast_CharacterCommon( CCharacter * pi_pTarget );
public :
			BOOL		CheckTargetConditionForCast_EnemyNpc( CCharacter * pi_pTarget );
			BOOL		CheckTargetConditionForCast_Player( CCharacter * pi_pTarget, BOOL pi_bIsCheckSameRace );
protected :

			BOOL		CheckAvatarConditionForCast_CheckSkillForce( CSFItem * pi_pSFItem );
			BOOL		CheckAvatarConditionForCast_CheckHpFpSp( CSFItem * pi_pSFItem );
			BOOL		CheckAvatarConditionForCast_CheckWeaponAndShield( CSFItem * pi_pSFItem );

			BOOL		CheckDistanceForCast( BYTE pi_bySFType, CSFItem * pi_pSFItem, CCharacter * pi_pTarget );
			BOOL		CheckDelayForCast( CSFItem * pi_pSFItem );
			BOOL		CheckRemainTimeForCast( CSFItem * pi_pSFItem, CCharacter * pi_pTarget );

			void		AttackToTarget( BYTE pi_byAttackType, CCharacter * pi_pTarget, BOOL pi_bIsMoveToTarget );	// target을 공격한다.
			
public :

			void		ChangeCombatMode( BYTE pi_byMode );
			void		ToggleCombatMode( void );
			void		ChangeMoveMode( BYTE pi_byMode );
			void		ToggleMoveMode( void );
			BOOL		IsEnableMove( void );
			int			CheckAttackDelay( BYTE pi_byAttackType, CSFItem * pi_pSFItem );
			BOOL		CheckObstacle( CCharacter * pi_pChar );
			float		GetAttackRange( BYTE pi_byAttackType, BOOL pi_bCheckTarget = TRUE );
			DWORD		GetAttackDelay( CSFItem * pi_pSFItem );
			DWORD		CalculateAttackDelayWithAllEffect( BYTE pi_byAttackType, DWORD pi_dwBasicDelay );

			BOOL		IsMyCompanion( CCharacter * pi_pChar );				// 캐릭터가 나와 우호적인 관계인가?
			BOOL		IsSummonableAnimus( CAnimusItem * pi_pAnimusItem );

protected :

			BOOL		CheckCorrespondenceOfWeaponWithBullet( void );		// 원거리 무기와 탄환의 검사
			BOOL		CheckCorrespondenceOfWeaponWithBullet_WhenMount( void );
			BOOL		CheckCorrespondenceOfWeaponWithBullet_WhenDismount( void );

public :

			CDummy *	CreateDummy( float pi_fX = ID_INVALID, float pi_fZ = ID_INVALID );

	//--------------------------------------------------------------------------

	inline	void		SetAttackMode( BYTE pi_byMode )				{ m_byAttackMode = pi_byMode; }
	inline	BYTE		GetAttackMode( void )						{ return m_byAttackMode; }
	inline	void		EnableAutoAttack( BOOL pi_bFlag )			{ m_bEnableAutoAttack = pi_bFlag; }
	inline	BOOL		IsEnableAutoAttack( void )					{ return m_bEnableAutoAttack; }
	inline	void		EnableAutoChaseTarget( BOOL pi_bFlag )		{ m_bEnableAutoChaseTarget = pi_bFlag; }
	inline	BOOL		IsEnableAutoChaseTarget( void )				{ return m_bEnableAutoChaseTarget; }
	inline	void		EnableAutoCounterAttack( BOOL pi_bFlag )	{ m_bEnableAutoCounterAttack = pi_bFlag; }
	inline	BOOL		IsEnableAutoCounterAttack( void )			{ return m_bEnableAutoCounterAttack; }
	inline	void		InstantAction( BOOL pi_bFlag )				{ m_bIsInstantAction = pi_bFlag; }
	inline	BOOL		IsInstantAction( void )						{ return m_bIsInstantAction; }

	//--------------------------------------------------------------------------

	//		void		IncreaseSFSuccessCount( void );
			void		SetSFSuccessCount( BYTE pi_bySFType, DWORD pi_dwIndex, DWORD pi_dwCount );

	inline	void		SetUsingSFCount( BYTE pi_byCount )			{ m_byUsingSFCount = pi_byCount; }
	inline	BYTE		GetUsingSFCount( void )						{ return m_byUsingSFCount; }
	inline	BYTE		IncreaseUsingSFCount( void )				{ return ++m_byUsingSFCount; }

	//========================================================================//
	//                                Boundary                                //
	//========================================================================//

			void		StopTargetPursuit( BOOL pi_bIsSetAction = TRUE );	// target 정보를 리셋한다.

	//========================================================================//
	//                                Boundary                                //
	//========================================================================//
	// ETC
	virtual	BOOL		PutOffEquipment( BYTE pi_byPartType );

			BOOL		SetDefaultItem( BYTE pi_byPartType, DWORD pi_dwIndex );
			BOOL		SetDefaultItem( BYTE pi_byPartType, CEquipItem * pi_pNormalItem );
			BOOL		UnsetDefaultItem( BYTE pi_byPartType );
			CEquipItem*	GetDefaultItem( BYTE pi_byPartType );

			void		SetWalkMode( BYTE pi_byWalkMode );
			void		SetCombatMode( BYTE pi_byCombatMode );

			BOOL		IsUsableItem( CBoardObject * pi_pBoardObject, BOOL pi_bIsRenderErrMsg = FALSE );	// 종족체크 & 레벨제한체크 & etc 체크
private :
			BOOL		IsUsableItem_CheckRace( CBoardItem * pi_pItem );
			BOOL		IsUsableItem_CheckLevel( CBoardItem * pi_pItem );
			BOOL		IsUsableItem_CheckExpertness( CBoardItem * pi_pItem );
			BOOL		IsUsableItem_CheckPotion( CBoardItem * pi_pItem );
			BOOL		IsUsableSF( CSFItem * pi_pSFItem, BOOL pi_bIsRenderErrMsg );
			BOOL		IsUsableAnimus( CAnimusItem * pi_pAnimusItem );
public :

			BOOL		IsEquipable( CBaseItem * pi_pBaseItem );							// 아바타가 주어진 아이템을 장착할 수 있는 상태인지를 체크

			BOOL		IsEnableDetect( CCharacter * pi_pCharacter );						// 주어진 캐릭터가 디텍트가 가능한가?

			DWORD		AddAppliedSF( CSFItem * pi_pSFItem );
			DWORD		DeleteAppliedSF( CSFItem * pi_pSFItem );
			void		ClearAppliedSF( void );
			void		UpdateAppliedSFTime( WORD pi_wServerEffectCode, DWORD pi_dwTime );

			void		UI_DeselectSF( void );
			void		UI_DisplaySFInfoString( BYTE pi_bySocketNo, CGUIObject * pi_pGuiItem );

	//========================================================================//
	//                                Boundary                                //
	//========================================================================//
	// ETC - Set & Get

			void		SetExp( float pi_fExp );
			void		SetExpByCount( double pi_fExp );
	inline	float		GetExp( void )								{ return m_fExp; }

			void		SetLevel( DWORD pi_dwLevel );

			void		SetMaxHP( DWORD pi_dwMaxHP );
			void		SetHP( DWORD pi_dwMaxHP );
			void		SetMaxFP( DWORD pi_dwMaxFP );
			void		SetFP( DWORD pi_dwFP );
			void		SetMaxSP( DWORD pi_dwMaxSP );
			void		SetSP( DWORD pi_dwSP );

			void		SetPartyMemberType( BYTE pi_byType );

			void		SetGold( DWORD pi_dwGold );
	inline	DWORD		GetGold( void )								{ return m_dwGold; }
			void		SetDalant( DWORD pi_dwDalant );
	inline	DWORD		GetDalant( void )							{ return m_dwDalant; }

			void		SetOrgTexRate( float pi_fTexRate );
	inline	float		GetOrgTexRate( void )						{ return m_fTexRate; }
	inline	float		GetBuyTexRate( void )						{ return ( 1.0f + m_fTexRate ); }
	inline	float		GetSellTexRate( void )						{ return ( 1.0f - m_fTexRate ); }
			void		SetExchangeRate( DWORD pi_dwExchangeRate );
	inline	DWORD		GetExchangeRate( void )						{ return m_dwExchangeRate; }
	inline	float		GetExchangeCommission( void )				{ return 0.05f; }
	inline	void		SetExchangeManageValue( DWORD pi_dwValue )	{ m_dwExchangeManageValue = pi_dwValue; }
	inline	DWORD		GetExchangeManageValue( void )				{ return m_dwExchangeManageValue; }


			void		SetClassDTIndex( BYTE pi_byOrderNo, DWORD pi_dwDTIndex );
			void		SetClassDTIndexByStrDTCode( BYTE pi_byOrderNo, char * pi_strDTCode );
			DWORD		GetClassDTIndex( BYTE pi_byOrderNo );
			BYTE		GetClassLevel( void );
	inline	char *		GetClassName( void )						{ return m_pClassName; }
			BOOL		IsChangableClass( void );
			void		ChangeClass( DWORD pi_dwNewClassDTIndex );
			void		RefreshClassInfo( void );

protected :

			void		SetClassName( char * pi_pClassName );

public :

			void		SetExpertness( BYTE pi_byType, DWORD pi_dwCount );
	inline	DWORD		GetSuccessCountOfExpertness( BYTE pi_byType ){ return m_dwExpertness[pi_byType]; }
			BYTE		GetExpertness( BYTE pi_byType );
			BYTE		CalculateExpertness( BYTE pi_byType, DWORD pi_dwCount );

	//--------------------------------------------------------------------------

			void		SetRequestChangeToCombatMode( BOOL pi_bFlag );
			BOOL		IsRequestChangeToCombatMode( void )			{ return m_bIsRequestChangeToCombatMode; }

	inline	void		ArrivedTargetPosition( void )				{ m_bIsArrivedTargetPosition = TRUE; }

	inline	void		SetCurrentTargetPos( Vector3f pi_vecTargetPos )
						{ memcpy( m_vecNextPath, pi_vecTargetPos, sizeof( float ) * 3 ); }
	inline	void		GetCurrentTargetPos( Vector3f po_vecTargetPos )
						{ memcpy( po_vecTargetPos, m_vecNextPath, sizeof( float ) * 3 ); }
	inline	float *		GetCurrentTargetPos( void )					{ return m_vecNextPath; }

	inline	void		SetLastTargetPos( Vector3f pi_vecLastTargetPos )
						{ memcpy( m_vecLastPath, pi_vecLastTargetPos, sizeof( float ) * 3 ); }
	inline	void		GetLastTargetPos( Vector3f po_vecLastTargetPos )
						{ memcpy( po_vecLastTargetPos, m_vecLastPath, sizeof( float ) * 3 ); }
	inline	float *		GetLastTargetPos( void )					{ return m_vecLastPath; }

	//--------------------------------------------------------------------------

			void		ApplyItemEffect_Jade( CBoardItem * pi_pItem, BOOL pi_bIsAdd );
	inline	float		GetOrgTradeProfitRate( void )				{ return m_fProfitOfTrade; }
	inline	float		GetBuyTradeProfitRate( void )				{ return ( 1.0f - m_fProfitOfTrade ); }
	inline	float		GetSellTradeProfitRate( void )				{ return ( 1.0f + m_fProfitOfTrade ); }
	inline	BOOL		IsEnable_SendMsgToRace( void )				{ return m_bEnable_SendMsgToRace; }

			void		UseItem( CBoardItem * pi_pItem );

	//--------------------------------------------------------------------------

			BOOL		IsPassedLimitTimeFromReceiveNetworkMsg( void );
			BOOL		IsPassedLimitTimeForDelete( void );

	inline	void		SetShowRebirthMessageBox( BOOL pi_bFlag )	{ m_bShowRebirthMessageBox = pi_bFlag; }
	inline	BOOL		IsShowRebirthMessageBox( void )				{ return m_bShowRebirthMessageBox; }

	//========================================================================//
	//                                Boundary                                //
	//========================================================================//

			void				RefreshConditionInfo( void );
			void				UI_RefreshCharInfoWindow( BOOL pi_bCallForMountUnit = FALSE );

protected :

			void				UI_RefreshCharInfoWindow_WhenMountUnit( BOOL pi_bCallForMountUnit );
			void				UI_RefreshCharInfoWindow_WhenDismountUnit( BOOL pi_bCallForMountUnit );

public :
			QBI_BASE_DATA *		GetCurrentProcessingQBI( int & pio_nStartIndex );

	inline	NPC_TRADE_INFO *	GetNpcTradeInfo( void )				{ return m_pNpcTradeInfo; }

	inline	MOVEMAP_INFO *		GetMoveMapInfo( void )				{ return m_pMoveMapInfo; }
			BOOL				IsMovingMap( void );
	inline	void				SetEnableTeleport( BOOL pi_bFlag )	{ m_bIsEnableTeleport = pi_bFlag; }
	inline	BOOL				IsEnableTeleport( void )			{ return m_bIsEnableTeleport; }

	inline	QUEST_INFO *		GetTutorialInfo( void )				{ return m_pTutorialInfo; }
	inline	AFTER_IMAGE_INFO *	GetAfterImageInfo( void )			{ return m_pAfterImageInfo; }
	inline	CHAT_INFO *			GetChatInfo( void )					{ return m_pChatInfo; }
	inline	OBSERVER_INFO *		GetObserverInfo( void )				{ return m_pObserverInfo; }

	//========================================================================//
	//                                Boundary                                //
	//========================================================================//

			void		SetSFUsingMsg( BOOL pi_bIsSuccessed );
#ifdef _DEVELOPER_
protected :
			void		RenderModeInfo( void );
			void		RenderSFUsingMsg( void );
			void		RenderAppliedSFMsg( void );
public :
#endif
};

#endif // __CHAR_AVATAR_H__