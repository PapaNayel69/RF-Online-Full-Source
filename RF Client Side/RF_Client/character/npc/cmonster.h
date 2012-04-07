////////////////////////////////////////////////////////////////////////////
//
// CMonster Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __CHAR_MONSTER_H__
#define __CHAR_MONSTER_H__

#include "CBaseNpc.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CMonster : public CBaseNpc
{

protected :

	CMonsterData *	m_pMonsterData;

	BYTE			m_byCreationType;			// 생성 형태 - 일반 / 소환
	BYTE			m_byDestroyType;			// 파괴 형태 - 죽음 / 소멸

	DWORD			m_dwPrevMoveTime;			// 이전에 움직인 시간 저장


public :

	CMonster();
	virtual ~CMonster();

	virtual	CCharacter *	New( void );
	virtual	CCharacter *	Clone( void );

	//-------------------------------------------------------------------------

			BOOL			Render( void );
			BOOL			Animation( DWORD pi_dwAniFrame = -1 );
			void			FrameMove( void );

	//-------------------------------------------------------------------------

protected :

	virtual	void			SetAction_ProcessActionID( DWORD pi_dwActionID );
	virtual	BOOL			SetAction_WillSetAnimation( DWORD pi_dwActionID );
	virtual	void			SetAction_SetEffectAtferSetAnimation( DWORD pi_dwActionID );

	virtual	DWORD			GetAnimationTypeID( DWORD pi_dwActionID );
	virtual	CAniData *		GetAnimationData_Normal( DWORD pi_dwAniTypeID );
	virtual	CAniData *		GetAnimationData_Force( DWORD pi_dwAniTypeID );

	virtual	DWORD			GetEffectType( void )					{ return ETI_MONSTER; }
	virtual	DWORD			GetEffectTypeID( DWORD pi_dwActionID );

	//-------------------------------------------------------------------------

public :

	virtual	void			SetAttribute( BASE_DATA * pi_pData );
	virtual	void			SetData( BASE_DATA * pi_pData );
	virtual	void			SetBone( DWORD pi_dwBoneID );
			void			SetMesh( DWORD pi_dwIndex );
	inline	void			SetCreationType( BYTE pi_byType )		{ m_byCreationType = pi_byType; }
	inline	BYTE			GetCreationType( void )					{ return m_byCreationType; }
			void			SetDeath( BYTE pi_byDestroyType );
			BYTE			GetLevel( void );
	virtual	BYTE			GetWidth( void );

	//-------------------------------------------------------------------------

			float			GetAttackRange( BYTE pi_byAttackType, BOOL pi_bCheckTarget = TRUE );

			BOOL			IsPassedLimitTimeFromReceiveNetworkMsg( void );
			BOOL			IsPassedLimitTimeForDelete( void );

	virtual	BYTE			UI_GetRadarCharType( void );

	//-------------------------------------------------------------------------

	inline	DWORD			GetEffectValue( void )					{ if( m_pMonsterData ) return m_pMonsterData->m_dwEffectValue; else return ID_INVALID; }
	inline	DWORD			GetAttackEffectMaterial( void )			{ if( m_pMonsterData ) return m_pMonsterData->m_dwAttackEffectMaterial; else return ID_INVALID; }
	inline	DWORD			GetDefenceEffectMaterial( void )		{ if( m_pMonsterData ) return m_pMonsterData->m_dwDefenceEffectMaterial; else return ID_INVALID; }
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __CHAR_MONSTER_H__