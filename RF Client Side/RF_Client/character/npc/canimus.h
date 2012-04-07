////////////////////////////////////////////////////////////////////////////
//
// CAnimus Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __CHAR_ANIMUS_H__
#define __CHAR_ANIMUS_H__

#include "CBaseNpc.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CAnimus : public CBaseNpc
{
public :

	enum { AGT_RECRUIT, AGT_VETERAN, AGT_MASTER, AGT_AMI, MAX_AGT_TYPE };	// Animus Grade Type

protected :

	CAnimusData	*	m_pAnimusData;
	ANIMUS_DATA *	m_pItemData;

public :

	CAnimus();
	virtual ~CAnimus();

	virtual	CCharacter *	New( void );
	virtual	CCharacter *	Clone( void );

	//-------------------------------------------------------------------------

			BOOL			Render( void );
			BOOL			Animation( DWORD pi_dwAniFrame = -1 );
			void			FrameMove( void );

	//-------------------------------------------------------------------------

protected :

	virtual	void			SetAction_ProcessActionID( DWORD pi_dwActionID );
	virtual	void			SetAction_SetEffectAtferSetAnimation( DWORD pi_dwActionID );

	virtual	DWORD			GetAnimationTypeID( DWORD pi_dwActionID );
	virtual	CAniData *		GetAnimationData_Normal( DWORD pi_dwAniTypeID );
	virtual	CAniData *		GetAnimationData_Force( DWORD pi_dwAniTypeID );

	virtual	DWORD			GetEffectType( void )						{ return ETI_ANIMUS; }
	virtual	DWORD			GetEffectTypeID( DWORD pi_dwActionID );

	//-------------------------------------------------------------------------

public :

	virtual	void			SetAttribute( BASE_DATA * pi_pData );
	virtual	void			SetData( BASE_DATA * pi_pData );
			void			SetDataByExp( DWORD pi_dwExp );
	inline	void			SetItemData( ANIMUS_DATA * pi_pItemData )	{ m_pItemData = pi_pItemData; }
			void			SetBone( DWORD pi_dwBoneID );
			void			SetMesh( DWORD pi_dwIndex );
			BYTE			GetLevel( void );
			BYTE			GetGrade( void );
	virtual	BYTE			GetWidth( void );
	virtual	DWORD			GetMeshID( void );

	virtual	void			SetHP( DWORD pi_dwHP );
	virtual	void			SetFP( DWORD pi_dwFP );
	virtual	float			GetHPRate( void )							{ return 1.0f; }

public :

			float			GetAttackRange( BYTE pi_byAttackType, BOOL pi_bCheckTarget = TRUE );

			BOOL			IsPassedLimitTimeFromReceiveNetworkMsg( void );
			BOOL			IsPassedLimitTimeForDelete( void );

	virtual	BYTE			UI_GetRadarCharType( void );
};

#endif  //__CHAR_ANIMUS_H__
