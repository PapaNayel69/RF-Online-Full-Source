////////////////////////////////////////////////////////////////////////////
//
// CBaseNpc Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __CHAR_BASE_NPC_H__
#define __CHAR_BASE_NPC_H__

#include "../Character/CCharacter.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CBaseNpc : public CCharacter
{

protected :

	CMesh *		m_pMesh;
	BASE_DATA *	m_pData;

	BYTE		m_byForceNo;				// 현재 NPC가 시전하고 있는 포스의 순번

public :

	CBaseNpc();
	virtual ~CBaseNpc();

	//--------------------------------------------------------------------------

			BOOL		Render( void );
			void		RenderShadow( void );
			void		CreateShadow( void );
			BOOL		Animation( DWORD pi_dwAniFrame = -1 );
			void		FrameMove( void );

			void		SetAction( DWORD pi_dwActionID, BOOL pi_bCheckActionDuplication = TRUE );

	virtual	BOOL		SetEffectInfo( DWORD pi_dwActionID );
			DWORD		GetNpcForceEffectID( DWORD pi_dwActionID, CSFItem * pi_pSFItem );

protected :

			void		Render_DrawMesh( void );
			void		Render_ProcessEffectAfterRender( void );

			BOOL		Animation_CheckBoneAndMesh( void );
			void		Animation_ProcessEffectBeforeAnimation( void );

			void		FrameMove_Force( void );

			DWORD		GetNpcEffectID( DWORD pi_dwEffectType );

			BOOL		SetAction_IsDuplicateAction( DWORD pi_dwActionID, BOOL pi_bCheckActionDuplication );
	virtual	void		SetAction_ProcessActionID( DWORD pi_dwActionID ) = 0;
	virtual	BOOL		SetAction_WillSetAnimation( DWORD pi_dwActionID );
	virtual	void		SetAction_SetEffectAtferSetAnimation( DWORD pi_dwActionID );
			void		SetAction_SetAnimationInterpolation( DWORD pi_dwActionID, CAniData * pi_pNewAniData );

	virtual	DWORD		GetAnimationTypeID( DWORD pi_dwActionID ) = 0;
			CAniData *	GetAnimationData( DWORD pi_dwActionID, DWORD pi_dwAniTypeID );
	virtual	CAniData *	GetAnimationData_Normal( DWORD pi_dwAniTypeID ) = 0;
	virtual	CAniData *	GetAnimationData_Force( DWORD pi_dwAniTypeID ) = 0;

	virtual	DWORD		GetEffectType( void ) = 0;
	virtual	DWORD		GetEffectTypeID( DWORD pi_dwActionID ) = 0;
			void		SetEffectInfo_Create( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID );
			void		SetEffectInfo_Death( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID );
			void		SetEffectInfo_Combat( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID );
			void		SetEffectInfo_Force( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID );
			void		SetEffectInfo_Force_SetTarget( EffectInfo & po_stEffectInfo );
			void		SetEffectInfo_Force_SetRangeAndAngle( EffectInfo & po_stEffectInfo );
			void		SetEffectInfo_Damaged( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID );
			void		SetEffectInfo_Gesture( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID );
			BOOL		PushEffectInfoToQueue( EffectInfo & pi_stEffectInfo );

	//--------------------------------------------------------------------------

public :

	virtual	void		SetAttribute( BASE_DATA * pi_pData ) = 0;
	virtual	void		SetData( BASE_DATA * pi_pData );
	virtual	BASE_DATA *	GetData( void )						{ return m_pData ; }
	virtual	void		SetMesh( DWORD pi_dwIndex ) = 0;
	virtual	DWORD		GetMeshID( void );
	inline	CMesh *		GetMesh( void )						{ return m_pMesh; }

	//--------------------------------------------------------------------------

			float		GetAttackRange( BYTE pi_byAttackType, BOOL pi_bCheckTarget = TRUE );
			BOOL		IsHaveForce( DWORD pi_dwForceDTIndex );

			void		Enable( DWORD pi_dwFlag );
			void		Disable( DWORD pi_dwFlag );

			BOOL		IsPassedLimitTimeFromReceiveNetworkMsg( void );
			BOOL		IsPassedLimitTimeForDelete( void );

			void		UI_SetRadar( void );
	virtual	BYTE		UI_GetRadarCharType( void );

			void		LoadAnimaitonData( void );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __CHAR_BASE_NPC_H__