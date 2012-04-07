////////////////////////////////////////////////////////////////////////////////
//
// CPlayer Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __CHAR_PLAYER2_H__
#define __CHAR_PLAYER2_H__

#include "CBasePlayer.h"
#include "CUnit.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Unit Mount State
// UMS_MOUNTING & UMS_DISMOUNTING => 기갑장비를 타거나 내리는 중임.
enum { UMS_DISMOUNT, UMS_DISMOUNTING, UMS_MOUNT, UMS_MOUNTING };

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

class CPlayer : public CCharacter
{
	friend class CPlayerProc;
	friend class CUnitProc;

protected :

	CPlayer *		m_pLinkPlayer;					// 연관된 플레이어 혹은 유닛

	CBaseProc *		m_pResProc;						// resource procedure
	CPlayerProc		m_clsPlayerProc;
	CUnitProc		m_clsUnitProc;

	BYTE			m_byUserGrade;					// 유저 등급

	BYTE			m_byRace;						// 플레이어의 종족
	BYTE			m_byPrevRace;					// 플레이어의 종족

	DWORD			m_dwLevel;						// 플레이어 레벨
	DWORD			m_dwMaxSP;
	DWORD			m_dwSP;

	BYTE			m_byUnitMountState;				// 유닛 탑승 상태
	BYTE			m_byPrevUnitMountState;			// 유닛 탑승 상태

	BYTE			m_byPartyMemberType;			// 파티원 타입 ( 파티원, 파티장, 일반 )
	BYTE			m_byMapIndex;					// 플레이어가 속한 맵의 인덱스

	//--------------------------------------------------------------------------

	char			m_byAttackPart;					// 플레이어가 공격한 부위
	DWORD			m_dwBulletDTIndex;				// 플레이어가 사용하는 탄환의 DT Index ( 이펙트를 위해 사용됨 )
	Vector3f		m_vecSFTargetPos;				// 스킬/포스사용의 지역기 사용시 지역기를 사용한 위치

	CBoardItem 		m_clsUsingItem;					// 플레이어가 현재 사용중인 아이템

	//--------------------------------------------------------------------------

	BOOL			m_bIsQuitGame;					// 게임을 종료했는가?

public:

	BYTE			m_byToggle7fps;					// by J.U.I - 물효과를 위한 7프래임마다 토글값저장.
	BYTE			m_byIsInWater;					// 물속인가.?
	float			m_fWaterFoot;					// 물 발자국 찍기위한 변수 
	float			m_fMoveMap;						// 맵 이동중이 아니면 0...
	DWORD			m_dwMoveUniqueID;				// 현재 이동중인 이펙트 아이디.
	CSwordTrace		m_cSwordTrace;					// by J.U.I - 무기의 궤적표현을 위한...

public :

	CPlayer();
	virtual ~CPlayer();


			BOOL		Render( void );
			BOOL		RenderName( void );
			void		CreateShadow( void );
			BOOL		Animation( DWORD pi_dwAniFrame = -1 );
			void		FrameMove( void );

	virtual	BOOL		SetEffectInfo( DWORD pi_dwActionID );
			DWORD		GetSkillForceEffectID( DWORD pi_dwActionID, CSFItem * pi_pSFItem );

	virtual	void		SetAction( DWORD pi_dwActionID, BOOL pi_bCheckActionDuplication = TRUE );

protected :

			void		FrameMove_Move( void );
			void		FrameMove_Skill( void );
			void		FrameMove_Force( void );
			void		FrameMove_Gesture( void );
			void		FrameMove_EveryFrame( void );

			BOOL		PushEffectInfoToQueue( EffectInfo & pi_stEffectInfo );

			BOOL		SetAction_IsDuplicateAction( DWORD pi_dwActionID, BOOL pi_bCheckActionDuplication );
			void		SetAction_SwitchingCombatMode( void );
			void		SetAction_SaveCurrentState( void );

			void		ProcessActionID( DWORD pi_dwActionID );
			void		ProcessActionID_Move( DWORD pi_dwActionID );
			void		ProcessActionID_Combat( DWORD pi_dwActionID );
			void		ProcessActionID_Skill( DWORD pi_dwActionID );
			void		ProcessActionID_Force( DWORD pi_dwActionID );
			void		ProcessActionID_Item( DWORD pi_dwActionID );
			void		ProcessActionID_Gesture( DWORD pi_dwActionID );
			void		SetAction_SetAnimationInterpolation( DWORD pi_dwActionID, CAniData * pi_pNewAniData );

public :

	inline	void		SetLinkPlayer( CPlayer * pi_pPlayer )	{ m_pLinkPlayer = pi_pPlayer; }
	inline	CPlayer *	GetLinkPlayer( void )					{ return m_pLinkPlayer; }
			BOOL		IsEqualLinkPlayer( CPlayer * pi_pPlayer );

	inline	void		SetRace( BYTE pi_byRace )				{ m_byRace = pi_byRace; }
	inline	BYTE		GetRace( void )							{ return m_byRace; }
	inline	BOOL		IsEqualRace( BYTE pi_byRace )			{ return ( ( m_byRace / 2 ) == ( pi_byRace / 2 ) ); }
	inline	BOOL		IsEqualRaceAndZender( BYTE pi_byRace )	{ return ( m_byRace == pi_byRace ); }
	inline	BOOL		IsBellato( void )						{ return ( ( m_byRace == CRT_BELLATOR_MALE ) || ( m_byRace == CRT_BELLATOR_FEMALE ) ); }
	inline	BOOL		IsCora( void )							{ return ( ( m_byRace == CRT_CORA_MALE ) || ( m_byRace == CRT_CORA_FEMALE ) ); }
	inline	BOOL		IsAccretia( void )						{ return ( m_byRace == CRT_ACCRETIA ); }

	virtual	void		SetBone( DWORD pi_dwBoneID );
			BOOL		PutOnEquipment( BYTE pi_byPartType, DWORD pi_dwDTIndex );
			BOOL		PutOnEquipment( CBoardItem * pi_pBoardItem );
			BOOL		PutOnEquipment( CEquipItem * pi_pEquipItem );
	virtual	BOOL		PutOffEquipment( BYTE pi_byPartType );
			CEquipItem*	GetEquipItem( BYTE pi_byPartType );
			CNormalItem*GetNormalItem( BYTE pi_byPartType );

			BOOL		CreateUnitEquipment( CUnitItem * pi_pUnitItem );

	inline	void		SetEquipmentVersion( DWORD pi_dwVer )	{ m_clsPlayerProc.m_dwEquipmentVersion = pi_dwVer; }
	inline	DWORD		GetEquipmentVersion( void )				{ return m_clsPlayerProc.m_dwEquipmentVersion; }
	inline	void		SetEquipmentRequestTime( DWORD pi_dwTime ){ m_clsPlayerProc.m_dwEquipmentRequestTime = pi_dwTime; }
	inline	DWORD		GetEquipmentRequestTime( void )			{ return m_clsPlayerProc.m_dwEquipmentRequestTime; }

			BOOL		MountUnit( CUnitItem * pi_pUnitItem, BOOL pi_bIsRiding );
			BOOL		DismountUnit( BOOL pi_bIsRiding );
	inline	void		SetUnitMountState( BYTE pi_byState )	{ m_byUnitMountState = pi_byState; }
	inline	BOOL		IsEqualUnitMountState( BYTE pi_byState ){ return ( m_byUnitMountState == pi_byState ); }
			BYTE		GetUnitAttackPartType( void );
	inline	CUnitItem *	GetUnitItem( void )						{ return m_clsUnitProc.GetUnitItem(); }

public :

	virtual	float		GetRealSpeed( void );
	virtual	BYTE		GetAttackDistanceType( BYTE pi_byAttackType );
	virtual	float		GetAttackRange( BYTE pi_byAttackType, BOOL pi_bCheckTarget = TRUE );
	virtual	BYTE		GetWidth( void );

	virtual	void		SetLightColor( DWORD pi_dwColor, BYTE pi_bFlag = LIGHT_NONE );
	virtual	void		SetLightColorFromMap( void );
	virtual	void		SetAlphaDensity( float pi_fAlpha );
	virtual	void		Enable( DWORD pi_dwFlag );
	virtual	void		Disable( DWORD pi_dwFlag );

public :

			void		SetUserGrade( BYTE pi_byUserGrade );
	inline	BYTE		GetUserGrade( void )					{ return m_byUserGrade; }
	inline	BOOL		IsEqualUserGrade( BYTE pi_byUserGrade )	{ return ( m_byUserGrade == pi_byUserGrade ); }

	virtual	void		SetLevel( DWORD pi_dwLevel );
	inline	DWORD		GetLevel( void )						{ return m_dwLevel; }

	virtual	void		SetMaxHP( DWORD pi_dwMaxHP = ID_INVALID );
	virtual	DWORD		GetMaxHP( void );
	virtual	void		SetHP( DWORD pi_dwHP );
	virtual	DWORD		GetHP( void );
	virtual	float		GetHPRate( void );

	virtual	void		SetMaxFP( DWORD pi_dwMaxFP = ID_INVALID );
	virtual	DWORD		GetMaxFP( void );
	virtual	void		SetFP( DWORD pi_dwFP );
	virtual	DWORD		GetFP( void );

	virtual	void		SetMaxSP( DWORD pi_dwMaxSP );
			DWORD		GetMaxSP( void );
	virtual	void		SetSP( DWORD pi_dwSP );
			DWORD		GetSP( void );

			void		SetTextColor( DWORD pi_dwColor );

	virtual	void		SetPartyMemberType( BYTE pi_byType );
	inline	BYTE		GetPartyMemberType( void )				{ return m_byPartyMemberType; }
	inline	BOOL		IsEqualPartyMemberType( BYTE pi_byType ){ return ( m_byPartyMemberType == pi_byType ); }
	inline	void		SetMapIndex( BYTE pi_byMapIndex )		{ m_byMapIndex = pi_byMapIndex; }
	inline	BYTE		GetMapIndex( void )						{ return m_byMapIndex; }	

			void		SetServerStatusFlag( DWORD pi_dwFlag );

public :

	virtual	void		SetWalkMode( BYTE pi_byWalkMode );

	inline	void		SetAttackPart( char pi_byPart )			{ m_byAttackPart = pi_byPart; }
	inline	char		GetAttackPart( void )					{ return m_byAttackPart; }
	inline	void		SetBulletDTIndex( DWORD pi_dwDTIndex )	{ m_dwBulletDTIndex = pi_dwDTIndex; }
	inline	DWORD		GetBulletDTIndex( void )				{ return m_dwBulletDTIndex; }

	inline	void		ClearSFTargetPos( void )
						{ ZeroMemory( m_vecSFTargetPos, sizeof( float ) * 3 ); }
	inline	void		SetSFTargetPos( Vector3f pi_vecTargetPos )
						{ memcpy( m_vecSFTargetPos, pi_vecTargetPos, sizeof( float ) * 3 ); }
	inline	void		GetSFTargetPos( Vector3f po_vecTargetPos )
						{ memcpy( po_vecTargetPos, m_vecSFTargetPos, sizeof( float ) * 3 ); }
	inline	float *		GetSFTargetPos( void )					{ return m_vecSFTargetPos; }

	inline	void		SetUsingItem( CBoardItem * pi_pItem )	{ if( pi_pItem ) memcpy( &m_clsUsingItem, pi_pItem, sizeof( CBoardItem ) ); }
	inline	CBoardItem* GetUsingItem( void )					{ if( m_clsUsingItem.IsEmpty() ) return NULL; else return ( &m_clsUsingItem ); }
	inline	void		RemoveUsingItem( void )					{ m_clsUsingItem.SetEmpty(); }

public :

	virtual	CDummy *	CreateDummy( float pi_fX = ID_INVALID, float pi_fZ = ID_INVALID );

	virtual	BOOL		IsPassedLimitTimeFromReceiveNetworkMsg( void );
	virtual	BOOL		IsPassedLimitTimeForDelete( void );

			void		SetTraceOfWeapon( BOOL pi_bFlag );

public :

	virtual	void		UI_SetRadar( void );
	virtual	void		UI_SetPositionOfRadar( void );
			BOOL		UI_SetRadar_CheckMe( void );
	virtual	void		UI_SetMinimap( void );
			void		UI_SetPositionOfMinimap( void );

	inline	void		SetData( void * pi_pData )				{ m_pResProc->SetData( pi_pData ); }

			void		LoadAnimaitonData( void );

	inline	void		SetQuitGame( BOOL pi_bFlag )			{ m_bIsQuitGame = pi_bFlag; }
	inline	BOOL		IsQuitGame( void )						{ return m_bIsQuitGame; }
};

#endif // __CHAR_PLAYER2_H__
