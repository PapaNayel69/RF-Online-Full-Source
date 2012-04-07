////////////////////////////////////////////////////////////////////////////////
//
// CCharacter Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../../GameObject/Manager/CCharacterMgr.h"
#include "../../GameObject/Manager/CTargetMgr.h"
#include "../../GameObject/Manager/CGroundItemManager.h"

#include "../../UserInterface/GameWindow/EtcWin/GUIRadarWindow.h"

#include "../../AddCodeByJang/EffectUtil.h"

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

//------------------------------------------------------------------------------
// < Function >
//   o 캐릭터를 주어진 타겟 지점으로 이동시킨다.
// < Parameter >
//   o pi_vecTargetPos - 캐릭터가 이동할 최종 목표 지점.
// < Return Value >
//   o BOOL - !( -1 ): 계속 움직일 수 있음.
//			- ( -1 ) : 더이상 움직일 수 없음.
//------------------------------------------------------------------------------
float
CCharacter::PositionMove( Vector3f pi_vecTargetPos )
{
	float l_fSpeed	= GetRealSpeed();
	float l_fDist	= GetDist( m_vecPos, pi_vecTargetPos );

	if( l_fDist <= ( l_fSpeed + 1.0f ) )
		return -1;

	float l_fYangle;
	if( !GetYAngle( &l_fYangle, m_vecPos, pi_vecTargetPos ) )
		l_fYangle = m_vecRot[1]; // l_fYangle = m_stRotYIPInfo.m_fTarY;

	m_vecPos[0] += ( (float)sin( AngleToPi( l_fYangle ) ) * l_fSpeed );
	m_vecPos[2] += ( (float)cos( AngleToPi( l_fYangle ) ) * l_fSpeed );

	return l_fSpeed;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CCharacter::SetAction( DWORD pi_dwActionID, BOOL pi_bCheckActionDuplication )
{
	switch( pi_dwActionID >> 28 )
	{
		case CAI_MOVE			: SetAction_Move( pi_dwActionID ); break;
		case CAI_COMBAT			: SetAction_Combat( pi_dwActionID ); break;
		case CAI_DAMAGED		: SetAction_Damaged( pi_dwActionID ); break;
		case CAI_DEATH			: SetAction_Death( pi_dwActionID ); break;
		case CAI_INTERNALSTATE	: SetAction_InternalState( pi_dwActionID ); break;
	}

	if( ( IsEqualAction( CAI_COMBAT ) || IsEqualAction( CAI_SKILL ) || IsEqualAction( CAI_FORCE ) ) && ( m_dwActionID != pi_dwActionID ) )
		m_pCombatState->ClearMyself();

	m_dwActionID		= pi_dwActionID;

	m_dwOldBoneFrame	= 0;
	m_dwBoneFrame		= 160;
	m_Animation.Animating( TRUE );

	m_dwIdleTime		= IDLE_TIME1;
	m_dwActionTime		= timeGetTime();

	SetAnimationSpeedByAction();

	return;
}

void
CCharacter::SetAction_Move( DWORD pi_dwActionID )
{
	if( pi_dwActionID == CAI_MOVE_STOP )
	{
		m_bIsFirstMoving		= TRUE;
		m_fMagnificationOfSpeed = 1.0f;
	}
	else if( pi_dwActionID == CAI_MOVE_MOVING )
	{
		m_pCharAniInfo->SetPushOutCount( 0 );
	}

	m_Animation.SetAnimationSpeed( 1.0f );
}

void
CCharacter::SetAction_Combat( DWORD pi_dwActionID )
{
}

void
CCharacter::SetAction_Damaged( DWORD pi_dwActionID )
{
}

void
CCharacter::SetAction_Death( DWORD pi_dwActionID )
{
	if( pi_dwActionID == CAI_DEATH_READY )
	{
		m_stAppliedSF.Clear();
		m_Animation.SetAnimationSpeed( 1.0f );
		m_dwHP = 0;
		if( _GetRadarWindow() )
			_GetRadarWindow()->RemoveCharacter( m_dwCharTypeID, m_dwIndex );

		Disable( ROSF_ANIMATION_LOOP );
		Disable( ROSF_ANIMATION_INTERPOLATION );
		SetLightColor( ID_INVALID );

		m_pCombatState->NotifyDeathToEnemy( ET_ATTACKER );
		m_pCombatState->NotifyDeathToEnemy( ET_VICTIM );

		_GetGroundItemMgr()->CheckGroundItemForCreating( this );
	}
	else if( pi_dwActionID == CAI_DEATH_DIE )
	{
		m_dwDeadTime = timeGetTime();
	}
}

void
CCharacter::SetAction_InternalState( DWORD pi_dwActionID )
{
	if( pi_dwActionID == CAI_INTERNALSTATE_DEATH )
	{
		m_pCombatState->SetDeadTime();
	}
}

void
CCharacter::SetAnimationSpeedByAction( void )
{
	if( !( IsEqualCharType( CTI_PLAYER ) || IsEqualCharType( CTI_AVATAR ) ) )
		return;

	float	fAniSpeed		= 1.0f;
	BOOL	bSetAniSpeed	= FALSE;

	if( m_dwActionID == CAI_COMBAT_ATTACK )
	{
		bSetAniSpeed = TRUE;
	}
	else if( m_dwActionID == CAI_SKILL_INVOKE )
	{
		if( m_clSFItem.IsAttackable() )
			bSetAniSpeed = TRUE;
	}

	if( bSetAniSpeed )
	{
		if( IsPassedLastEffectFrame() )
			fAniSpeed	= 1.7f;
		else
			fAniSpeed	= 1.3f;
	}

	SetAnimationSpeed( fAniSpeed );
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 현재 동작에 알맞은 이펙트로 설정한다.
// TRUE - 이펙트를 발생됐음. FALSE - 이펙트가 발생되지 않았음
BOOL
CCharacter::SetEffectInfo( DWORD pi_dwActionID )
{
	EffectInfo stEffectInfo;

	if( ( pi_dwActionID >> 28 ) == CAI_DAMAGED )
		SetEffectInfo_Damage( stEffectInfo, pi_dwActionID );
	else
		return FALSE;

	return PushEffectInfoToQueue( stEffectInfo );
}

void
CCharacter::SetEffectInfo_Damage( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID )
{
	if( ( pi_dwActionID == CAI_DAMAGED_BY_FORCE ) || ( pi_dwActionID == CAI_DAMAGED_BY_SKILL ) )
	{
		// get data
		CAttackerInfo * pAttackerInfo	= GetCurAttackerInfo();
		if( !pAttackerInfo )
			return;

		CCharacter *	pAttacker		= pAttackerInfo->GetEnemy();
		if( !pAttacker )
			return;

		CSFItem *		pSFItem			= pAttackerInfo->GetSFItem();	// attacker에 있는 GetSFItem()은 NULL일 수 있으므로, 반드시 CAttackerInfo에 있는 GetSFItem()을 사용해야한다.
		if( !pSFItem )
			return;

		// set effect type
		( pi_dwActionID == CAI_DAMAGED_BY_SKILL ) ?	po_stEffectInfo.dwType	= ETI_SKILL :
													po_stEffectInfo.dwType	= ETI_FORCE;

		// set effect ID
		if( pAttacker->IsEqualCharType( CTI_AVATAR ) || pAttacker->IsEqualCharType( CTI_PLAYER ) )
		{
			po_stEffectInfo.dwID = static_cast< CPlayer * >( pAttacker )->
								   GetSkillForceEffectID( pi_dwActionID, pSFItem );
		}
		else if( pAttacker->IsEqualCharType( CTI_MONSTER ) ||
				 pAttacker->IsEqualCharType( CTI_ANIMUS ) ||
				 pAttacker->IsEqualCharType( CTI_GUARD_TOWER ) )
		{
			po_stEffectInfo.dwID = static_cast< CBaseNpc * >( pAttacker )->
								   GetNpcForceEffectID( pi_dwActionID, pSFItem );
		}

		// set damage
		po_stEffectInfo.dwValue		= pAttackerInfo->GetCurDamagePoint();
		po_stEffectInfo.dwTarget[0]	= pAttacker->GetCharTypeID();	// 타겟에 공격자를 넣어서 보낸다.
		po_stEffectInfo.dwTarget[1]	= pAttacker->GetIndex();

		// set ETC
		po_stEffectInfo.byRangeType_For_Effect	= pSFItem->GetRangeTypeForEffect();
	}
}
/*
void
CCharacter::SetEffectInfo_Damage_ByPlayerSkillForce( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID )
{
	CAttackerInfo *	pAttackerInfo	= GetCurAttackerInfo();
	CPlayer *		pAttacker		= static_cast< CPlayer * >( pAttackerInfo->GetEnemy() );
	CSFItem *		pSFItem			= pAttackerInfo->GetSFItem();	// attacker에 있는 GetSFItem()은 NULL일 수 있으므로, 반드시 CAttackerInfo에 있는 GetSFItem()을 사용해야한다.
	if( !pSFItem )
		return;

	( pi_dwActionID == CAI_DAMAGED_BY_SKILL ) ?	po_stEffectInfo.dwType	= ETI_SKILL :
												po_stEffectInfo.dwType	= ETI_FORCE;

	po_stEffectInfo.dwID					= pAttacker->GetSkillForceEffectData( pi_dwActionID, pSFItem );
	po_stEffectInfo.byRangeType_For_Effect	= pSFItem->GetRangeTypeForEffect();
}*/

BOOL
CCharacter::PushEffectInfoToQueue( EffectInfo & pi_EffectInfo )
{
	if( pi_EffectInfo.dwID == ID_INVALID )
		return FALSE;

	pi_EffectInfo.SetMyself( this );

	_GetEffectInfoList()->push_back( &pi_EffectInfo );

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

// MAYUN < begin > -------------------------------------------------------------
void
CCharacter::SetAnimationInterpolation( CAniData * pi_pAni, BOOL pi_bIsEnable )
{
	if( pi_pAni && !m_Animation.IsEqual( pi_pAni ) )
	{
		ResetAnimation( m_Animation.GetAniData() );

		if( !m_pBone )
			return;

		m_pBone->m_Frame = m_dwBoneFrame;
		_GetCharIF()->FrameMove( m_pBone );

		// enable
		if( pi_bIsEnable && IsEnable( ROSF_ANIMATION_INTERPOLATION ) )
		{
			//l_pBone->m_TransitionStart	= true;

			for ( int mo = 0; mo < m_pBone->m_MaxObj; mo++ )
			{
				memcpy( &m_pAniIPInfo->matInterpolation[mo], m_pBone->m_pMesh[mo].m_MatResult, sizeof(D3DMATRIX) );
				if( m_pBone->m_pMesh[mo].m_HaveParent)
				{
					D3DXMATRIX rinv;
					D3DXMatrixInverse( &rinv,NULL, &m_pBone->m_pMesh[mo].m_pParent->m_MatResult);
					D3DXMatrixMultiply( &m_pAniIPInfo->matLInterpolation[mo], &m_pBone->m_pMesh[mo].m_MatResult,&rinv );
				}
				else
				{
					memcpy(	&m_pAniIPInfo->matLInterpolation[mo], m_pBone->m_pMesh[mo].m_MatResult, sizeof(D3DXMATRIX) );
				}
			}

			m_bIsAniInterpolation					= TRUE;
			m_pAniIPInfo->dwAniInterpolationTime	= 1000;
			m_pAniIPInfo->dwAniInterpolationFrame	= 0;
		}
		// disable
		else
		{
			m_pBone->m_TransitionStart				= false;

			m_bIsAniInterpolation					= FALSE;
			m_pAniIPInfo->dwAniInterpolationFrame	= 0;
			m_pAniIPInfo->dwAniInterpolationTime	= 0;

			for (int mo = 0; mo < m_pBone->m_MaxObj; mo++ )
			{
				memset( &m_pAniIPInfo->matInterpolation[mo], 0, sizeof(D3DMATRIX) );
			}
		}
	}
}

BOOL
CCharacter::SetVertexBlending( int pi_nIndex, CMesh * pi_pMesh, BOOL pi_bForAnimation )
{
	assert( pi_nIndex < MAX_PARTS_RESULT );

	if( pi_pMesh )
	{
		if( pi_bForAnimation )
		{
			if(m_dwMaxResult[pi_nIndex]<pi_pMesh->m_MaxObj)
			{
				if (m_pMatResMat[pi_nIndex][0]) delete[] m_pMatResMat[pi_nIndex][0];
				m_pMatResMat[pi_nIndex][0] = new D3DXMATRIX[pi_pMesh->m_MaxObj];

				if (m_pMatResMat[pi_nIndex][1]) delete[] m_pMatResMat[pi_nIndex][1];
				m_pMatResMat[pi_nIndex][1] = new D3DXMATRIX[pi_pMesh->m_MaxObj];

				m_dwMaxResult[pi_nIndex] = pi_pMesh->m_MaxObj;
			}
		}

		for (int j = 0; j < pi_pMesh->m_MaxObj; j++ )
		{
			if( ( !pi_bForAnimation ) && ( j >= m_dwMaxResult[pi_nIndex] ) )
				return FALSE;

			ObjectMesh *mesh = &pi_pMesh->m_pMesh[j];
			if( !mesh->m_BlendNum )		//Blend Vertex Object
			{
				( pi_bForAnimation ) ? 
					memcpy(&m_pMatResMat[pi_nIndex][0][j],&mesh->m_MatResult,sizeof(D3DXMATRIX)) :
					memcpy(&mesh->m_MatResult,&m_pMatResMat[pi_nIndex][0][j],sizeof(D3DXMATRIX));
			}
			else
			{
				for( int b = 0; b < mesh->m_BlendNum && b < 2; b++ )
				{
					( pi_bForAnimation ) ?
						memcpy(&m_pMatResMat[pi_nIndex][b][j],&mesh->m_pMatBlend[b],sizeof(D3DXMATRIX)) :
						memcpy(&mesh->m_pMatBlend[b],&m_pMatResMat[pi_nIndex][b][j],sizeof(D3DXMATRIX));
				}
			}
		}

		return TRUE;
	}

	return FALSE;
}
// MAYUN < end > ---------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CCharacter::SetCombatMode( BYTE pi_byCombatMode )
{
	m_byCombatMode = pi_byCombatMode;

	if( !IsDeath() )
		SetAction( m_dwActionID );
}

BYTE
CCharacter::GetAttackDistanceType( BYTE pi_byAttackType )
{
	if( pi_byAttackType == CAT_FORCE )
		return ADT_FAR;

	return ADT_NEAR;
}

float
CCharacter::GetAttackRange( BYTE pi_byAttackType, BOOL pi_bCheckTarget )
{
	return 0;
}

void
CCharacter::SetWalkMode( BYTE pi_byWalkMode )
{
	m_byWalkMode = pi_byWalkMode;

	// 현재 캐릭터가 움직이는 상태라면 액션 변경
	// 정지 상태에서는 애니메이션의 변경이 없으므로 바꾸지 않는다.
	if( IsEqualAction( CAI_MOVE_MOVING ) )
	{
		SetAction( CAI_MOVE_MOVING );
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CCharacter::SetSpeed( BYTE pi_bMoveMode, float pi_fSpeed )
{
	if( pi_bMoveMode == CMM_MOVE_WALK )
		m_fSpeed[0] = pi_fSpeed;
	else if( pi_bMoveMode == CMM_MOVE_RUN )
		m_fSpeed[1] = pi_fSpeed;
}

void
CCharacter::SetSpeed( float pi_fSpeed )
{
	if( GetWalkMode() == CMM_MOVE_WALK )
		m_fSpeed[0] = pi_fSpeed;
	else if( GetWalkMode() == CMM_MOVE_RUN )
		m_fSpeed[1] = pi_fSpeed;
}

float
CCharacter::GetSpeed( BYTE pi_bMoveMode )
{
	if( pi_bMoveMode == CMM_MOVE_WALK )
		return m_fSpeed[0];
	else if( pi_bMoveMode == CMM_MOVE_RUN )
		return m_fSpeed[1];

	return 0.0f;
}

float
CCharacter::GetSpeed( void )
{
	if( m_byWalkMode == CMM_MOVE_WALK )
		return m_fSpeed[0];
	else if( m_byWalkMode == CMM_MOVE_RUN )
		return m_fSpeed[1];

	return 0.0f;
}

float g_fSpeed = 0.0f;	// 임시 코드
float
CCharacter::GetRealSpeed( void )
{
	g_fSpeed = ( GetSpeed() + ( m_stConditionInfo.m_fMoveSpeed * CHAR_SPEED_DEFAULT ) );	// 테스트코드

	return ( R3API::R3GetLoopTime() * g_fSpeed * m_fMagnificationOfSpeed );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CCharacter::SetMaxHP( DWORD pi_dwMaxHP )
{
	m_dwMaxHP = pi_dwMaxHP;
}

DWORD
CCharacter::GetMaxHP( void )
{
	return m_dwMaxHP;
}

DWORD
CCharacter::GetHP( void )
{
	return m_dwHP;
}

float
CCharacter::GetHPRate( void )
{
	if( m_dwMaxHP == 0 || m_dwHP == 0 )
		return 0.0f;
	else
		return ( ( float )m_dwHP / ( float )m_dwMaxHP );
}

void
CCharacter::SetMaxFP( DWORD pi_dwMaxFP )
{
	//m_dwMaxFP = pi_dwMaxFP * m_stConditionInfo.m_fMaxHPnFP;
	m_dwMaxFP = pi_dwMaxFP;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CCharacter::SetRotY( float pi_fRotY, BOOL pi_bEnableRotYInterpolation )
{
	CResObject::SetRotY( pi_fRotY, pi_bEnableRotYInterpolation );
}

void
CCharacter::SetRotY( Vector3f pi_vecOrgPos, Vector3f pi_vecTarPos, BOOL pi_bEnableRotYInterpolation )
{
	CResObject::SetRotY( pi_vecOrgPos, pi_vecTarPos, pi_bEnableRotYInterpolation );
}

void
CCharacter::SetPosY( float pi_fPosY, BOOL pi_bEnablePosYInterpolation )
{
	if( IsEqualAction( CAI_MOVE_MOVING ) )
		CResObject::SetPosY( pi_fPosY, pi_bEnablePosYInterpolation );
	else
		CResObject::SetPosY( pi_fPosY, FALSE );
}

void
CCharacter::SetPosition( float pi_fPosX, float pi_fPosY, float pi_fPosZ, BOOL pi_bSetPrevPosition, BOOL pi_bEnablePosYInterpolation )
{
	if( IsEqualAction( CAI_MOVE_MOVING ) )
		CResObject::SetPosition( pi_fPosX, pi_fPosY, pi_fPosZ, pi_bSetPrevPosition, pi_bEnablePosYInterpolation );
	else
		CResObject::SetPosition( pi_fPosX, pi_fPosY, pi_fPosZ, pi_bSetPrevPosition, FALSE );
}

void
CCharacter::SetPosition( Vector3f pi_vecPos, BOOL pi_bSetPrevPosition, BOOL pi_bEnablePosYInterpolation )
{
	if( IsEqualAction( CAI_MOVE_MOVING ) )
		CResObject::SetPosition( pi_vecPos, pi_bSetPrevPosition, pi_bEnablePosYInterpolation );
	else
		CResObject::SetPosition( pi_vecPos, pi_bSetPrevPosition, FALSE );
}

BOOL
CCharacter::CheckYPos( void )
{
	if( !CResObject::CheckYPos() )
	{
		if( IsEqualAction( CAI_MOVE_MOVING ) )
			SetAction( CAI_MOVE_STOP );

		return FALSE;
	}

	return TRUE;
}

void
CCharacter::UI_SetPositionOfRadar( void )
{
	if( _GetRadarWindow() )
	{
		_GetRadarWindow()->UpdateCharacterPos( m_dwCharTypeID, m_dwIndex,
											   m_vecPos[0], m_vecPos[1], m_vecPos[2] );
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CCharacter::SetSFItem( CSFItem * pi_pSFItem )
{
	if( pi_pSFItem )
		memcpy( &m_clSFItem, pi_pSFItem, sizeof( CSFItem ) );
	else
		m_clSFItem.SetEmpty();
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CCharacter::Enable( DWORD pi_dwFlag )
{
	DWORD l_dwPrevStatusFlag = m_dwStatusFlag;

	CResObject::Enable( pi_dwFlag );

	if( IsEnable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT ) )
		EFUtil_DeleteSF( &m_stAppliedSF );

	//if( _GetTargetMgr() && _GetTargetMgr()->IsEqualTarget( this ) )
	//{
	//	if( IsEnable( ROSF_OUT_OF_VISUAL_FIELD_SERVER ) )
	//		int a = 0;
	//}
}

void
CCharacter::Disable( DWORD pi_dwFlag )
{
	DWORD l_dwPrevStatusFlag = m_dwStatusFlag;

	CResObject::Disable( pi_dwFlag );

	if( IsDisable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT | ROSF_AFTER_IMAGE_OF_OBJECT ) )
		UI_SetRadar();
}

BOOL
CCharacter::IsEqual( CCharacter * pi_pCharacter )
{
	if( pi_pCharacter )
	{
		if( ( m_dwCharTypeID == pi_pCharacter->GetCharTypeID() ) &&
			( m_dwIndex == pi_pCharacter->GetIndex() ) )
			return TRUE;
	}

	return FALSE;
}

BOOL
CCharacter::IsEqual( DWORD pi_dwCharTypeID, DWORD pi_dwIndex )
{
	return( ( m_dwCharTypeID == pi_dwCharTypeID ) && ( m_dwIndex == pi_dwIndex ) );
}

BOOL
CCharacter::IsDeath( void )
{
	if( IsEqualActionCategory( CAI_DEATH ) || IsEqualAction( CAI_INTERNALSTATE_DEATH ) )
		return TRUE;

	return FALSE;
}

BOOL
CCharacter::IsCombatAction( void )
{
	if( IsEqualAction( CAI_COMBAT_ATTACK ) || IsEqualAction( CAI_SKILL_INVOKE ) ||
		IsEqualAction( CAI_FORCE_INVOKE ) )
		return TRUE;

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

DWORD
CCharacter::AddAppliedSF( CSFItem * pi_pSFItem )
{
	return m_stAppliedSF.AddSFItem( pi_pSFItem );
}

DWORD
CCharacter::DeleteAppliedSF( CSFItem * pi_pSFItem )
{
	return m_stAppliedSF.DeleteSFItem( pi_pSFItem );
}

void
CCharacter::ClearAppliedSF( void )
{
	m_stAppliedSF.Clear();
}

void
CCharacter::SetServerSFEffectCode( WORD pi_wServerEffectCode, BYTE pi_bySFLevel, BOOL pi_bIsAdd )
{
	if( pi_wServerEffectCode != 0xFFFF )
	{
		CSFItem clSFItem;
		BYTE	bySFType	= pi_wServerEffectCode >> 12;
		DWORD	dwDTIndex	= pi_wServerEffectCode & 0x0FFF;

		if( !clSFItem.Create( bySFType, dwDTIndex ) )
			return;

		clSFItem.SetLevel( pi_bySFLevel );

		if( pi_bIsAdd )
			AddAppliedSF( &clSFItem );
		else
			DeleteAppliedSF( &clSFItem );
	}
	else
	{
		m_stAppliedSF.DeleteLastestAppliedSFItem();
	}
}

CAttackerInfo *
CCharacter::AddAttackerInfo( CCharacter * pi_pEnemy, BOOL pi_bIsSettingToCurEnemy )
{
	return static_cast< CAttackerInfo * >( m_pCombatState->AddEnemyInfo( ET_ATTACKER, pi_pEnemy, pi_bIsSettingToCurEnemy ) );
}

CVictimInfo *
CCharacter::AddVictimInfo( CCharacter * pi_pEnemy, BOOL pi_bIsSettingToCurEnemy )
{
	return static_cast< CVictimInfo * >( m_pCombatState->AddEnemyInfo( ET_VICTIM, pi_pEnemy, pi_bIsSettingToCurEnemy ) );
}

CAttackerInfo *
CCharacter::GetAttackerInfo( CCharacter * pi_pEnemy )
{
	if( pi_pEnemy )
		return static_cast< CAttackerInfo * >( m_pCombatState->GetEnemyInfo( ET_ATTACKER, pi_pEnemy->GetCharTypeID(), pi_pEnemy->GetIndex() ) );

	return NULL;
}

CVictimInfo *
CCharacter::GetVictimInfo( CCharacter * pi_pEnemy )
{
	if( pi_pEnemy )
		return static_cast< CVictimInfo * >( m_pCombatState->GetEnemyInfo( ET_VICTIM, pi_pEnemy->GetCharTypeID(), pi_pEnemy->GetIndex() ) );

	return NULL;
}

CAttackerInfo *
CCharacter::GetCurAttackerInfo( void )
{
	return static_cast< CAttackerInfo * >( m_pCombatState->GetCurEnemyInfo( ET_ATTACKER ) );
}

CVictimInfo *
CCharacter::GetCurVictimInfo( void )
{
	return static_cast< CVictimInfo * >( m_pCombatState->GetCurEnemyInfo( ET_VICTIM ) );
}

CCharacter *
CCharacter::GetCurAttacker( void )
{
	return ( m_pCombatState->GetCurEnemy( ET_ATTACKER ) );
}

CCharacter *
CCharacter::GetCurVictim( void )
{
	return ( m_pCombatState->GetCurEnemy( ET_VICTIM ) );
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CCharacter::EnableStun( void )
{
	m_stConditionInfo.SetStun( TRUE );

	m_stETCInfo.SetReceiveNetMsg( "Stun" );
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CAniData *
CCharacter::GetAnimationDataByRandom( BYTE pi_byAniMgrType, DWORD & pio_dwAniID )
{
	CAniData * l_pAni = NULL;

	// 먼저 0번의 애니메이션을 가져온다.
	l_pAni = _GetAniMgr()->GetAni( pi_byAniMgrType, pio_dwAniID );
	if( l_pAni )
	{
		ANI_EXT_INFO * l_pExtraInfo = ( ANI_EXT_INFO * )l_pAni->m_pExtra;

		if( l_pExtraInfo->nAniNumOfSameAction > 1 )	// random 하게 가져온다.
		{
			pio_dwAniID |= ( rand() % l_pExtraInfo->nAniNumOfSameAction );

			l_pAni = _GetAniMgr()->GetAni( pi_byAniMgrType, pio_dwAniID );
			if( !l_pAni )
			{
				m_Animation.SetAnimationID( ID_INVALID );
				return NULL;
			}
		}
		
		m_Animation.SetAnimationID( pio_dwAniID );
		return l_pAni;
	}

	m_Animation.SetAnimationID( ID_INVALID );
	return NULL;
}

void
CCharacter::RefreshConditionInfo( void )
{
	m_stConditionInfo.Init();
	m_stConditionInfo.m_pCharacter = this;

	if( ( m_dwCharTypeID == CTI_PLAYER ) || ( m_dwCharTypeID == CTI_AVATAR ) )
	{
		CPlayer * pPlayer = static_cast< CPlayer * >( this );

		for( int i = 0; i < MAX_EQUIP_PART + 1; ++i )
			m_stConditionInfo.ApplyItemEffect( pPlayer->GetNormalItem( i ), TRUE );
	}
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CCharacter::CreatePath( Vector3f pi_vecTargetPos )
{
	m_cPathStack.Create( m_vecPos, pi_vecTargetPos );
}

int
CCharacter::GetNextPath( Vector3f po_vecPos )
{
	if( m_cPathStack.GetNextPath( po_vecPos ) )
	{
		m_cPathStack.IncPath();
		/*if( GetCharTypeID() != CTI_AVATAR && m_cPathStack.GetPathResult() == _PATH_TOO_LONG_CLIP )
		//{
		//	return FALSE;	// 아바타가 아닌경우는 중간에 끊어가는경우가 없구 한방에 가는거다...
		} => 주의 : 이 루틴이 동작하면 안 됨! */
		return TRUE;
	}

	return FALSE;
}

// by J.U.I
void
CCharacter::CheckLongPoint( Vector3f pi_vecPos, Vector3f pi_vecTargetPos )	//길게 피킹할경우. 다시 패스를 얻어낸다.
{
	Vector3f l_vecTemp;

	if( m_cPathStack.GetPathResult() == _PATH_TOO_LONG_CLIP )	//멀리찍었던거...
	{
		m_cPathStack.GetPickPoint(l_vecTemp);					//이전에 피킹한거를 백업한다.
		m_cPathStack.Create(pi_vecPos,l_vecTemp);
		m_cPathStack.GetNextPath(pi_vecTargetPos);
	}
}

void
CCharacter::GetCurPath( Vector3f po_vecPos )
{
	m_cPathStack.GetCurPath( po_vecPos );
}

BOOL
CCharacter::GetLastPath( Vector3f po_vecPath )
{
	return m_cPathStack.GetPath( po_vecPath, m_cPathStack.GetTotalPathNo() - 1 );
}

BOOL
CCharacter::GetPath( Vector3f po_vecPath, int pi_nPathNo )
{
	return m_cPathStack.GetPath( po_vecPath, pi_nPathNo );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// by MAYUN
D3DXMATRIX *
CCharacter::GetResultMat( DWORD pi_dwPart, DWORD pi_dwMeshNum )
{
	if( m_dwMaxResult[pi_dwPart]<=pi_dwMeshNum )
		return NULL;

	return &m_pMatResMat[pi_dwPart][0][pi_dwMeshNum]; 
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CCharacter::IsRemovableWhenWasNotReciveNetworkMsg( void )
{
	if( IsEqualAction( CAI_MOVE_STOP ) || IsEqualAction( CAI_MOVE_IDLE ) || IsDeath() )
		return TRUE;
	else
		return FALSE;
}

BOOL
CCharacter::IsPassedLimitTimeFromReceiveNetworkMsg( void )
{
	return FALSE;
}

BOOL
CCharacter::IsPassedLimitTimeForDelete( void )
{
	return FALSE;
}

//extern float g_fAverageFPS;

DWORD
CCharacter::GetTotalFrameSkipTime( void )
{
	//if( g_fAverageFPS >= 50.0f )
	//	return 0;

	if( _GetTargetMgr()->IsEqualTarget( this ) )
		return 0;

	if( IsEqualAction( CAI_MOVE_MOVING ) )
		m_fDistanceBetweenAvatar = GetDist( m_vecPos, _GetAvatar()->GetPosition() );

	if( m_fDistanceBetweenAvatar <= 50.0f )
		return 10;
	else if( m_fDistanceBetweenAvatar <= 100.0f )
		return 20;
	else if( m_fDistanceBetweenAvatar <= 150.0f )
		return 30;
	else if( m_fDistanceBetweenAvatar <= 200.0f )
		return 50;
	else if( m_fDistanceBetweenAvatar <= 250.0f )
		return 70;

	return 100;
}

/*
extern float g_fAverageFPS;

#define MAX_SKIP_LEVEL			( 6 )
#define MAX_SKIP_DISTANCE_TYPE	( 5 )

static const float	SKIP_DISTANCE[MAX_SKIP_DISTANCE_TYPE]	= { 50.0f, 100.0f, 150.0f, 200.0f, 250.0f };
static const float	SKIP_FPS[MAX_SKIP_DISTANCE_TYPE]		= { 10.0f, 20.0f, 30.0f, 40.0f, 50.0f };
static const DWORD	SKIP_TIME_BY_FPS[MAX_SKIP_LEVEL][MAX_SKIP_LEVEL] =
{
	{ 50.0f, 80.0f, 100.0f, 150.0f, 200.0f, 300.0f },
	{ 20.0f, 50.0f, 80.0f, 120.0f, 170.0f, 250.0f },
	{ 10.0f, 20.0f, 50.0f, 90.0f, 140.0f, 200.0f },
	{ 10.0f, 20.0f, 30.0f, 60.0f, 80.0f, 110.0f },
	{ 10.0f, 20.0f, 30.0f, 50.0f, 70.0f, 100.0f },
};

DWORD
CCharacter::GetTotalFrameSkipTime( void )
{
	if( g_fAverageFPS >= 50.0f )
		return 0;

	if( _GetTargetMgr()->IsEqualTarget( this ) )
		return 0;

	if( IsEqualAction( CAI_MOVE_MOVING ) )
		m_fDistanceBetweenAvatar = GetDist( m_vecPos, _GetAvatar()->GetPosition() );

	BYTE l_byFPSType = ( BYTE )( g_fAverageFPS / 10.0f );
	if( m_fDistanceBetweenAvatar <= SKIP_DISTANCE[0] )
		return SKIP_TIME_BY_FPS[l_byFPSType][0];
	else if( m_fDistanceBetweenAvatar <= SKIP_DISTANCE[1] )
		return SKIP_TIME_BY_FPS[l_byFPSType][1];
	else if( m_fDistanceBetweenAvatar <= SKIP_DISTANCE[2] )
		return SKIP_TIME_BY_FPS[l_byFPSType][2];
	else if( m_fDistanceBetweenAvatar <= SKIP_DISTANCE[3] )
		return SKIP_TIME_BY_FPS[l_byFPSType][3];
	else if( m_fDistanceBetweenAvatar <= SKIP_DISTANCE[4] )
		return SKIP_TIME_BY_FPS[l_byFPSType][4];

	return SKIP_TIME_BY_FPS[l_byFPSType][5];
}*/