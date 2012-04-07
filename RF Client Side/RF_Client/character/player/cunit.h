////////////////////////////////////////////////////////////////////////////////
//
// CRenderUnit Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __CHAR_RENDER_UNIT_H__
#define __CHAR_RENDER_UNIT_H__

#include "CBasePlayer.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CUnitProc : public CBaseProc
{
	friend class CPlayer;
	friend class CAvatar;

	enum { MAX_EQUIPMENT = 7 };

protected :

	CUnitItem *		m_pUnitItem;						// 유닛은 CNormalItem 대신 CUnitItem 으로 대처함.
	CUnitItem 		m_clUnitItem;						// 아바타가 아닐 경우에는 m_pUnitItem = &m_clUnitItem,
														// 아바타의 경우에는  m_pUnitItem = CUnitStorehouseBoard()->GetCurUnit()을 사용한다.

	CEquipItem 		m_listEquipItem[MAX_EQUIPMENT];		// 유닛 파츠의 리스트 ( 메시 )
	CEquipItem *	m_pAttackParts;						// 공격 파츠 - arms or shoulder

public :

	CUnitProc();
	virtual ~CUnitProc();

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

	void			SetEffectInfo_Combat( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID );
	void			SetEffectInfo_Damaged( EffectInfo & po_EffectInfo, DWORD pi_dwActionID );
	void			SetEffectInfo_Item( EffectInfo & po_EffectInfo, DWORD pi_dwActionID );
	void			SetEffectInfo_Create( EffectInfo & po_EffectInfo, DWORD pi_dwActionID );
	void			SetEffectInfo_Death( EffectInfo & po_EffectInfo, DWORD pi_dwActionID );
	void			SetEffectInfo_Gesture( EffectInfo & po_EffectInfo, DWORD pi_dwActionID );
	DWORD			GetUnitEffectData( DWORD pi_dwActionID, DWORD pi_dwEffectTypeID );

	CAniData *		GetAnimationData( DWORD pi_dwActionID, DWORD pi_dwAniTypeID );

	void			SetEquipItemAnimation( BYTE pi_byPartType, BYTE pi_byAniTypeID );

public :

	void			SetBone( DWORD pi_dwBoneID );
	BOOL			PutOnEquipment( BYTE pi_byPartType, DWORD pi_dwDTIndex );
	BOOL			PutOnEquipment( CBoardItem * pi_pBoardItem );
	BOOL			PutOnEquipment( CEquipItem * pi_pEquipItem );
	BOOL			PutOnEquipment( CUnitPartsItem * pi_pUnitPartsItem );
	BOOL			PutOnEquipment( CUnitItem * pi_pUnitItem );
	BOOL			PutOffEquipment( BYTE pi_byPartType );
	CEquipItem *	GetEquipItem( BYTE pi_byPartType );
	CNormalItem *	GetNormalItem( BYTE pi_byPartType );
	CUnitItem *		GetUnitItem( void )						{ return m_pUnitItem; }

protected :

	BOOL			CreateEquipment( BYTE pi_byPartType, DWORD pi_dwDTIndex );
	BYTE			ConvertPartTypeToOrderNo( BYTE pi_byPartType );

	CUnitPartsItem*	GetBooster( void );

public :

	void			SetMaxHP( DWORD pi_dwMaxHP );
	DWORD			GetMaxHP( void );
	void			SetHP( DWORD pi_dwHP );
	DWORD			GetHP( void );
	float			GetHPRate( void );

	void			SetMaxSP( DWORD pi_dwMaxSP );
	DWORD			GetMaxSP( void );
	void			SetSP( DWORD pi_dwSP );
	DWORD			GetSP( void );

public :

	void			MountUnit( void );
	void			SetAttackParts( BYTE pi_byItemKind );

	void			ChangeMoveMode( BYTE pi_byMoveMode );
	float			GetAttackRange( BYTE pi_byAttackType );
	float			GetBoosterSpeed( void );
	void			SetBulletNumThatCurAttackParts( DWORD pi_dwBulletNum );
	DWORD			GetBulletNumThatCurAttackParts( void );

	void			SetLightColor( DWORD pi_dwColor, BYTE pi_bFlag );
	void			SetLightColorFromMap( void );
	void			SetAlphaDensity( float pi_fAlpha );
	void			Enable( DWORD pi_dwFlag );
	void			Disable( DWORD pi_dwFlag );

	void			SetData( void * pi_pData )	{ ; }

	void			LoadAnimaitonData( void );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////


#endif // __CHAR_RENDER_UNIT_H__
