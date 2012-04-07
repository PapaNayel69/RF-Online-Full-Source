////////////////////////////////////////////////////////////////////////////////
//
// CPlayerProc Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __CHAR_RENDER_PLAYER_H__
#define __CHAR_RENDER_PLAYER_H__

#include "../Character/CCharacter.h"

#include "../../Item/CItemData.h"

class CBaseProc
{
	friend class CPlayer;
	friend class CAvatar;

protected :

	CPlayer *		m_pPlayer;

	CBone *			m_pBone;
	BBOX *			m_pBBox;

public :

	CBaseProc();
	virtual ~CBaseProc();

	inline CPlayer *		GetPlayer(){	return m_pPlayer; };

	virtual	void			Render( void ) = 0;
	virtual	void			ProcessEffectAfterRender( void ) = 0;
	virtual	void			CreateShadow( void ) = 0;
	virtual	void			Animation( void ) = 0;
	virtual	void			ProcessEffectBeforeAnimation( void ) = 0;
	virtual	void			FrameMove( void ) = 0;

	virtual	void			SetEffectInfo( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID ) = 0;
	virtual	void			ProcessActionID( DWORD pi_dwActionID ) = 0;
	virtual	void			ProcessEffectAfterSetAction( void ) = 0;

	virtual	CAniData *		GetAnimationByAction( DWORD pi_dwActionID ) = 0;

public :

	virtual	void			SetBone( DWORD pi_dwBoneID ) = 0;
	virtual	BOOL			PutOnEquipment( BYTE pi_byPartType, DWORD pi_dwDTIndex ) = 0;
	virtual	BOOL			PutOnEquipment( CBoardItem * pi_pBoardItem ) = 0;
	virtual	BOOL			PutOnEquipment( CEquipItem * pi_pEquipItem ) = 0;
	virtual	BOOL			PutOffEquipment( BYTE pi_byPartType ) = 0;
	virtual	CEquipItem *	GetEquipItem( BYTE pi_byPartType ) = 0;
	virtual	CNormalItem *	GetNormalItem( BYTE pi_byPartType ) = 0;

public :

	virtual	void			ChangeMoveMode( BYTE pi_byMode ) = 0;
	virtual	float			GetAttackRange( BYTE pi_byAttackType ) = 0;

	virtual	void			SetLightColor( DWORD pi_dwColor, BYTE pi_bFlag ) = 0;
	virtual	void			SetLightColorFromMap( void ) = 0;
	virtual	void			SetAlphaDensity( float pi_fAlpha ) = 0;
	virtual	void			Enable( DWORD pi_dwFlag ) = 0;
	virtual	void			Disable( DWORD pi_dwFlag ) = 0;
	virtual	void			SetData( void * pi_pData ) = 0;

	virtual	void			LoadAnimaitonData( void ) = 0;
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CPlayerProc : public CBaseProc
{
	friend class CPlayer;
	friend class CAvatar;
	enum { MAX_EQUIPMENT = MAX_EQUIP_PART+1 };			// +1은 hair

protected :

	CNormalItem		m_listNormalItem[MAX_EQUIPMENT];	// 장착한 아이템의 리스트
	CEquipItem		m_listEquipItem[MAX_EQUIPMENT];		// 장착한 아이템의 리스트 ( 메시 )

	DWORD			m_dwEquipmentVersion;				// 플레이어가 장착하고 있는 아이템의 버젼
	DWORD			m_dwEquipmentRequestTime;			// 플레이어의 장착 아이템 리스트를 요구한 시간 ( 여러번 계속 요구할시 일정 간격으로 보내기 위해 사용 )

	DT_CPInfo *		m_pData;							// 플레이어의 데이터

public :

	CPlayerProc();
	virtual ~CPlayerProc();

protected :

	void			Render( void );
	void			ProcessEffectAfterRender( void );
	void			CreateShadow( void );
	void			Animation( void );
	void			ProcessEffectBeforeAnimation( void );
	void			FrameMove( void );

	void			SetEffectInfo( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID );
	void			ProcessActionID( DWORD pi_dwActionID );
	void			ProcessEffectAfterSetAction( void );

	CAniData *		GetAnimationByAction( DWORD pi_dwActionID );

protected :

	BOOL			ApplyDefaultMesh( BYTE pi_byPartType, CEquipItem * pi_pEquipItem );
	void			RematchItemToBone( BYTE pi_byPartType );
	BOOL			SetEquipItemToDefaultMesh( BYTE pi_byPartType, CEquipItem * pi_pEquipItem );

	void			SetEffectInfo_Combat( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID );
	void			SetEffectInfo_SkillForce( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID );
	void			SetEffectInfo_SkillForce_SetTarget( EffectInfo & po_stEffectInfo );
	void			SetEffectInfo_SkillForce_SetRangeAndAngle( EffectInfo & po_stEffectInfo );
	void			SetEffectInfo_Damaged( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID );
	void			SetEffectInfo_Item( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID );
	void			SetEffectInfo_Create( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID );
	void			SetEffectInfo_Death( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID );
	void			SetEffectInfo_Gesture( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID );

	DWORD			GetPlayerEffectID( DWORD pi_dwEffectType1, DWORD pi_dwEffectType2 );
	DWORD			GetItemEffectID( CBaseItem * pi_pItem, DWORD pi_dwEffectType );
	DWORD			GetSkillForceEffectID( DWORD pi_dwActionID, CSFItem * pi_pSFItem );
	DWORD			GetSkillEffectID( DWORD pi_dwActionID, CSFItem * pi_pSFItem );
	DWORD			GetForceEffectID( DWORD pi_dwActionID, CSFItem * pi_pSFItem );

	CAniData *		GetAnimationData( DWORD pi_dwActionID, DWORD pi_dwAniTypeID );
	CAniData *		GetAnimationData_Normal( DWORD pi_dwActionID, DWORD pi_dwAniTypeID );
	CAniData *		GetAnimationData_Normal_RaceTypeLoop( DWORD pi_dwActionID, DWORD pi_dwAniTypeID );
	CAniData *		GetAnimationData_Normal_CombatModeLoop( DWORD & pio_AnimationID, DWORD pi_dwActionID, DWORD pi_dwAniTypeID );
	CAniData *		GetAnimationData_Normal_WeaponLoop( DWORD & pio_AnimationID, DWORD pi_dwActionID, DWORD pi_dwAniTypeID );
	CAniData *		GetAnimationData_Normal_AttackPartLoop( DWORD & pio_AnimationID, DWORD pi_dwActionID, DWORD pi_dwAniTypeID );
	BYTE			GetAnimationData_Normal_GetAttackPart( CCharacter * pi_pVictim );
	float			GetAnimationData_Normal_GetYDistance( CCharacter * pi_pVictim );
	int				GetAnimationData_Normal_GetDistanceRatio( CCharacter * pi_pVictim );
	CAniData *		GetAnimationData_Force( DWORD pi_dwActionID );
	CAniData *		GetAnimationData_Skill( DWORD pi_dwActionID );

	void			SetEquipItemAnimation( BYTE pi_byPartType, BYTE pi_byAniTypeID );

public :

	void			SetBone( DWORD pi_dwBoneID );
	BOOL			PutOnEquipment( BYTE pi_byPartType, DWORD pi_dwDTIndex );
	BOOL			PutOnEquipment( CBoardItem * pi_pBoardItem );
	BOOL			PutOnEquipment( CEquipItem * pi_pEquipItem);
	BOOL			PutOffEquipment( BYTE pi_byPartType );
	CEquipItem *	GetEquipItem( BYTE pi_byPartType );
	CNormalItem *	GetNormalItem( BYTE pi_byPartType );

protected :

	void			PutOnEquipment_SetWeaponAndShield( BYTE pi_byPartType );
	BOOL			CreateEquipment( BYTE pi_byPartType, DWORD pi_dwDTIndex );
	BOOL			CreateEquipment( CBoardItem * pi_pBoardItem );
	BOOL			CreateEquipment( CEquipItem * pi_pEquipItem );

public :

	void			ChangeMoveMode( BYTE pi_byMode );
	float			GetAttackRange( BYTE pi_byAttackType );

	void			SetLightColor( DWORD pi_dwColor, BYTE pi_bFlag );
	void			SetLightColorFromMap( void );
	void			SetAlphaDensity( float pi_fAlpha );
	void			Enable( DWORD pi_dwFlag );
	void			Disable( DWORD pi_dwFlag );

	void			SetData( void * pi_pData )	{ m_pData = ( DT_CPInfo * )pi_pData; }

	void			LoadAnimaitonData( void );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////


#endif // __CHAR_RENDER_PLAYER_H__
