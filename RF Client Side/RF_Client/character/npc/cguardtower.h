////////////////////////////////////////////////////////////////////////////
//
// CGuardTower Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __CHAR_GUARD_TOWER_H__
#define __CHAR_GUARD_TOWER_H__

#include "CBaseNpc.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CGuardTower : public CBaseNpc
{
protected :

	DWORD				m_dwOwerIndex;					// 가드 타워를 설치한 플레이어의 인덱스
	DWORD				m_dwRemainBuildTime;			// 가드 타워 제작에 필요한 대기 시간

	GUARD_TOWER_DATA *	m_pGuardTowerData;

public :

	CGuardTower();
	virtual ~CGuardTower();

	virtual	CCharacter *	New( void );
	virtual	CCharacter *	Clone( void );

			BOOL			Render( void );
			BOOL			Animation( DWORD pi_dwAniFrame = -1 );
			void			FrameMove( void );

protected :

	virtual	void			SetAction_ProcessActionID( DWORD pi_dwActionID );
	virtual	void			SetAction_SetEffectAtferSetAnimation( DWORD pi_dwActionID );

	virtual	DWORD			GetAnimationTypeID( DWORD pi_dwActionID );
	virtual	CAniData *		GetAnimationData_Normal( DWORD pi_dwAniTypeID );
	virtual	CAniData *		GetAnimationData_Force( DWORD pi_dwAniTypeID );

	virtual	DWORD			GetEffectType( void )						{ return ETI_ANIMUS; }
	virtual	DWORD			GetEffectTypeID( DWORD pi_dwActionID );

public :

	virtual	void			SetAttribute( BASE_DATA * pi_pData );
	virtual	void			SetData( BASE_DATA * pi_pData );
			void			SetBone( DWORD pi_dwBoneID );
			void			SetMesh( DWORD pi_dwIndex );
			BYTE			GetLevel( void );
	virtual	BYTE			GetWidth( void );
	virtual	float			GetHPRate( void )							{ return 1.0f; }

	inline	BOOL			IsEqualOwner( CPlayer * pi_pPlayer )		{ if( !pi_pPlayer ) return NULL; else return ( pi_pPlayer->GetIndex() == m_dwOwerIndex ); }

public :

			float			GetAttackRange( BYTE pi_byAttackType, BOOL pi_bCheckTarget = TRUE );
			BYTE			GetRace( void );
			void			SetRemainBuildTime( DWORD pi_dwTime );

			BOOL			IsPassedLimitTimeFromReceiveNetworkMsg( void );
			BOOL			IsPassedLimitTimeForDelete( void );

	virtual	BYTE			UI_GetRadarCharType( void );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif  //__CHAR_GUARD_TOWER_H__
