/////////////////////////////////////////////////////////////////////////////
//
// CPlayerProc Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "CPlayer.h"
#include "CBasePlayer.h"
#include "../../GameObject/Manager/CTargetMgr.h"
#include "../../GameObject/Board/Common/CBoardManager.h"

#include "../../AddCodeByJang/EffectUtil.h"
#include "../../AddCodeByJang/PlayerSpecialEffect.h"
#include "../../AddCodeByYun/ParticleList.h"
#include "../../AddCodeByYun/ShadowList.h"

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CBaseProc::CBaseProc()
{
	m_pPlayer	= NULL;

	m_pBone		= NULL;
	m_pBBox		= NULL;
}

CBaseProc::~CBaseProc()
{
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CPlayerProc::CPlayerProc()
{
	m_dwEquipmentVersion		= 0;
	m_dwEquipmentRequestTime	= 0;

	m_pData						= NULL;
}

CPlayerProc::~CPlayerProc()
{
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CPlayerProc::Render( void )
{
	for( int i = 0; i < MAX_EQUIPMENT; ++i )
	{
		if( m_listEquipItem[i].IsEmpty() )
			continue;

		if( !m_pPlayer->SetVertexBlending( i, m_listEquipItem[i].GetMesh(), FALSE ) )
			continue;

		m_listEquipItem[i].SetPosition( m_pPlayer->m_vecPos );
		m_listEquipItem[i].SetRotY( m_pPlayer->m_vecRot[1] );

		m_listEquipItem[i].Render();
	}
}

// J.U.I < begin > -------------------------------------------------------------
void
CPlayerProc::ProcessEffectAfterRender( void )
{
	if( m_pPlayer->IsEnable( ROSF_RENDER_EFFECT ) )
	{
		m_pPlayer->SetPrevAction( m_pPlayer->GetAction() );		// 액션 변하기전에 넣어준다.
		::CPlayerSpecialEffectRender( (void*)m_pPlayer );		// 플레이어의 효과,효과음 처리등...
	}
}
// J.U.I < end > ---------------------------------------------------------------

// MAYUN < begin > -------------------------------------------------------------
void
CPlayerProc::CreateShadow( void )
{
	CMesh * pMesh = NULL;

	for( int i = 0; i < MAX_EQUIPMENT; i++ )
	{
		if( m_listEquipItem[i].IsEmpty() )
			continue;

		if( ( i == CEPT_WEAPON ) || ( i == CEPT_SHIELD ) )		// 무기나 방패는 전투시에만 보여준다.
		{
			if( m_pPlayer->m_byCombatMode == CSI_PEACE )
				continue;
		}

		pMesh = m_listEquipItem[i].GetMesh();
		m_pPlayer->SetVertexBlending( i, pMesh, FALSE );
		_GetCharIF()->DrawCharacterShadow( pMesh, m_pPlayer->m_vecPos, m_pPlayer->m_vecRot[1],
										   0.8f * m_pPlayer->m_fAlphaDensity, 1.0f );
	}
}
// MAYUN < end > ---------------------------------------------------------------

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CPlayerProc::Animation( void )
{
	for( int i = 0; i < MAX_EQUIPMENT; ++i )
	{
		if( !ApplyDefaultMesh( i, &m_listEquipItem[i] ) )
			continue;

		RematchItemToBone( i );

		m_listEquipItem[i].FrameMove();
		m_listEquipItem[i].Animation();

		m_pPlayer->SetVertexBlending( i, m_listEquipItem[i].GetMesh(), TRUE );
	}

	::CPlayerSpecialEffectFrameMove( ( void * )m_pPlayer );		// by J.U.I
}

// MAYUN < begin > -------------------------------------------------------------
void
CPlayerProc::ProcessEffectBeforeAnimation( void )
{
	#define _DAMAGE_TIME	0.27f
	#define _DELAY_TIME_1	0.20f

	#define _DELAY_TIME	0.1f

 	D3DXMATRIX rot;
	CHARACTEROBJECT *bn;
	float damage_time;
	CMesh * co = NULL;

	EFUTIL_CheckContinuousSkillForce((CCharacter *)m_pPlayer);

	damage_time=m_pPlayer->GetDamageTimeForShake();	//맞을때 허리제끼기...
	bn = g_pBoneMgr->GetBone( RLI_PLAYER, m_pPlayer->GetBoneID() );
	//
	if( bn )
	{
		if( damage_time > 0 )
		{
			if( _DELAY_TIME_1 < damage_time )
			{
				float su=_DAMAGE_TIME-_DELAY_TIME_1;
				float gac = ((damage_time-_DELAY_TIME_1)/su)*50.0f;
				MatrixRotate(rot.m,0,0,-50+gac);
			}
			else			
			{
				float gac = (damage_time/_DELAY_TIME_1)*30.0f;
				MatrixRotate(rot.m,0,0,-gac);
			}
			//_GetCharIF()->SetEyeVec("Bip01 Head",bn,rot);
	 		_GetCharIF()->SetEyeVec("Bip01 Spine1",bn,rot);
			damage_time-=R3GetLoopTime();
			m_pPlayer->SetDamageTimeForShake(damage_time );
			if( m_pPlayer->IsEqualCharType( CTI_AVATAR ) )	
				EFUTIL_SetBattleMusicState();	//아바타라면 배틀 사운드를 위한.
		}
		else
		{
			MatrixIdentity(rot.m);
			_GetCharIF()->SetEyeVec("Bip01 Spine1",bn,rot);
		}
	}
}
// MAYUN < end > ---------------------------------------------------------------

BOOL
CPlayerProc::ApplyDefaultMesh( BYTE pi_byPartType, CEquipItem * pi_pEquipItem )
{
	if( pi_pEquipItem->GetMesh() )
	{
		return TRUE;
	}
	// 아이템이 없고 RENDER_DEFAULT_MESH가 세팅되었을 경우에는 디폴트 아이템으로 적용시킨다.
	else if( m_pPlayer->IsEnable( ROSF_RENDER_DEFAULT_MESH ) )
	{
		if( ( pi_byPartType <= IEPT_SHOES ) || ( pi_byPartType == CEPT_HAIR ) )
			SetEquipItemToDefaultMesh( pi_byPartType, pi_pEquipItem );
	}

	return FALSE;
}

void
CPlayerProc::RematchItemToBone( BYTE pi_byPartType )
{
	// 무기나 방패의 경우에는 bone에 rematch 를 시켜줘야 한다.
	if( ( pi_byPartType == IEPT_WEAPON ) || ( pi_byPartType == IEPT_SHIELD ) )
	{
		if( m_listEquipItem[pi_byPartType].IsEmpty() )
			return;

		CMesh * pMesh = m_listEquipItem[pi_byPartType].GetMesh();
		if( m_pBone && pMesh )
		{
			_GetCharIF()->RematchParent( m_pBone, pMesh );
			_GetCharIF()->AnimationReset( pMesh );
		}
	}
}

// 현재 장착된 아이템의 메시가 없어서, 디폴트 메시로 세팅함.
BOOL
CPlayerProc::SetEquipItemToDefaultMesh( BYTE pi_byPartType, CEquipItem * pi_pEquipItem )
{
	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CPlayerProc::FrameMove( void )
{
	if( m_pPlayer->IsDisable( ROSF_MOVABLE ) )
		return;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CPlayerProc::SetEffectInfo( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID )
{
	switch( pi_dwActionID >> 28 )
	{
		case CAI_COMBAT		: SetEffectInfo_Combat( po_stEffectInfo, pi_dwActionID );		break;
		case CAI_SKILL		:
		case CAI_FORCE		: SetEffectInfo_SkillForce( po_stEffectInfo, pi_dwActionID );	break;
		case CAI_DAMAGED	: SetEffectInfo_Damaged( po_stEffectInfo, pi_dwActionID );		break;
		case CAI_ITEM		: SetEffectInfo_Item( po_stEffectInfo, pi_dwActionID );			break;
		case CAI_CREATE		: SetEffectInfo_Create( po_stEffectInfo, pi_dwActionID );		break;
		case CAI_DEATH		: SetEffectInfo_Death( po_stEffectInfo, pi_dwActionID );		break;
		case CAI_GESTURE	: SetEffectInfo_Gesture( po_stEffectInfo, pi_dwActionID );		break;
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CPlayerProc::SetEffectInfo_Combat( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID )
{
	if( pi_dwActionID == CAI_COMBAT_ATTACK_ERROR )	// CAI_COMBAT_ATTACK_ERROR는 모두 동일하다.
	{
		po_stEffectInfo.dwType	= ETI_PLAYER;
		po_stEffectInfo.dwID	= GetPlayerEffectID( 0, PET_ATTACK_ERROR );
	}
	else if( pi_dwActionID == CAI_COMBAT_ATTACK )
	{
		po_stEffectInfo.dwType	= ETI_ITEM;
		po_stEffectInfo.dwID	= GetItemEffectID( GetNormalItem( IEPT_WEAPON ), IET_ATTACK_NORMAL );
	}

	po_stEffectInfo.SetTarget( m_pPlayer->m_pCombatState->GetCurEnemy( ET_VICTIM ) );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CPlayerProc::SetEffectInfo_SkillForce( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID )
{
	if( !m_pPlayer->GetSFItem() )
		return;

	if( ( pi_dwActionID >> 28 ) == CAI_SKILL )
		po_stEffectInfo.dwType	= ETI_SKILL;
	else
		po_stEffectInfo.dwType	= ETI_FORCE;

	po_stEffectInfo.dwID		= GetSkillForceEffectID( pi_dwActionID, m_pPlayer->GetSFItem() );

	po_stEffectInfo.byRangeType_For_Effect = m_pPlayer->GetSFItem()->GetRangeTypeForEffect();

	SetEffectInfo_SkillForce_SetTarget( po_stEffectInfo );
	SetEffectInfo_SkillForce_SetRangeAndAngle( po_stEffectInfo );
}

//==============================================================================

void
CPlayerProc::SetEffectInfo_SkillForce_SetTarget( EffectInfo & po_stEffectInfo )
{
	// 자신에게만 사용할 수 있는 경우..
	if( m_pPlayer->m_clSFItem.IsEqualTargetType( SFT_ANYWHERE ) )
	{
		/*
		BYTE byRangeType = m_pPlayer->m_clSFItem.GetRangeType();

		// 지역기 등의 기술이라면 NULL
		if( byRangeType == SFRT_SELF_AREA || byRangeType == SFRT_FLASH_DAMAGE ||
			byRangeType == SFRT_SECTOR_AREA || byRangeType == SFRT_EXTENT )
			po_stEffectInfo.SetTarget( NULL );
		else
			po_stEffectInfo.SetTarget( m_pPlayer );
		*/
		po_stEffectInfo.SetTarget( m_pPlayer );
	}
	// 맵에만 사용할 수 있는 경우..
	else if( m_pPlayer->m_clSFItem.IsEqualTargetType( SFT_MAP ) )
	{
		po_stEffectInfo.SetTarget( NULL );
	}
	// 타겟에게 쓸 수 있는 기술일경우..
	else
	{
		po_stEffectInfo.SetTarget( m_pPlayer->m_pCombatState->GetCurEnemy( ET_VICTIM ) );
	}
}

void
CPlayerProc::SetEffectInfo_SkillForce_SetRangeAndAngle( EffectInfo & po_stEffectInfo )
{
	// 스킬/포스의 범위 or 각도
	static const float SELFAREA_EXTENT_RANGE[4] = { 42.0f, 56.0f, 70.0f, 84.0f };
	static const float FLASH_ANGLE[4] = { 15.0f, 30.0f, 45.0f, 60.0f };

	// 스킬/포스의 이펙트 거리 or 각도
	po_stEffectInfo.byRangeType = m_pPlayer->m_clSFItem.GetRangeType();

	if( ( po_stEffectInfo.byRangeType == SFRT_SELF_AREA ) || ( po_stEffectInfo.byRangeType == SFRT_EXTENT ) )
	{
		po_stEffectInfo.fRange = SELFAREA_EXTENT_RANGE[m_pPlayer->m_clSFItem.GetStep()];
	}
	else if( po_stEffectInfo.byRangeType == SFRT_FLASH_DAMAGE )
	{
		po_stEffectInfo.fRange = FLASH_ANGLE[m_pPlayer->m_clSFItem.GetStep()];
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CPlayerProc::SetEffectInfo_Damaged( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID )
{
	po_stEffectInfo.dwType = ETI_PLAYER;

	if( pi_dwActionID == CAI_DAMAGED_NORMAL )
	{
		( m_pPlayer->IsAccretia() ) ?	po_stEffectInfo.dwID = GetPlayerEffectID( 0, PET_DAMAGE_NORMAL2 ) :
										po_stEffectInfo.dwID = GetPlayerEffectID( 0, PET_DAMAGE_NORMAL1 );
	}
	else if( pi_dwActionID == CAI_DAMAGED_CRITICAL )
	{
		po_stEffectInfo.dwID = GetPlayerEffectID( 0, PET_DAMAGE_CRITICAL );
	}
	else if( pi_dwActionID == CAI_DAMAGED_MISS )
	{
		po_stEffectInfo.dwID = GetPlayerEffectID( 0, PET_DAMAGE_MISS );
	}

	CAttackerInfo * pAttackerInfo = m_pPlayer->GetCurAttackerInfo();
	if( !pAttackerInfo )
		return;
	po_stEffectInfo.dwValue = pAttackerInfo->GetCurDamagePoint();
	po_stEffectInfo.SetTarget( pAttackerInfo->GetEnemy() );		// 타겟에 공격자를 넣어서 보낸다.
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CPlayerProc::SetEffectInfo_Item( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID )
{
	po_stEffectInfo.dwType = ETI_ITEM;

	if( pi_dwActionID == CAI_ITEM_USE_MINING )
		po_stEffectInfo.dwID = GetItemEffectID( GetNormalItem( IEPT_WEAPON ), IET_USE );
	else if( pi_dwActionID == CAI_ITEM_USE_ITEM )
		po_stEffectInfo.dwID = GetItemEffectID( &m_pPlayer->m_clsUsingItem, IET_USE );
	else if( pi_dwActionID == CAI_ITEM_USE_STOP )
		po_stEffectInfo.dwID = GetItemEffectID( &m_pPlayer->m_clsUsingItem, IET_USE_STOP );
}

void
CPlayerProc::SetEffectInfo_Create( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID )
{
	po_stEffectInfo.dwType = ETI_PLAYER;

	if( pi_dwActionID == CAI_CREATE_CREATE )
		po_stEffectInfo.dwID = GetPlayerEffectID( 0, PET_CREATE_CREATE );
}

void
CPlayerProc::SetEffectInfo_Death( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID )
{
	po_stEffectInfo.dwType = ETI_PLAYER;

	if( pi_dwActionID == CAI_DEATH_READY )
		po_stEffectInfo.dwID = GetPlayerEffectID( 0, PET_CREATE_DESTROY );
}

void
CPlayerProc::SetEffectInfo_Gesture( EffectInfo & po_stEffectInfo, DWORD pi_dwActionID )
{
	po_stEffectInfo.dwType = ETI_PLAYER;

	switch( pi_dwActionID )
	{
		case CAI_GESTURE_LEVEL_UP		: po_stEffectInfo.dwID = GetPlayerEffectID( 0, PET_GESTURE_LEVELUP );	break;
		case CAI_GESTURE_MOUNT			: po_stEffectInfo.dwID = GetPlayerEffectID( 0, PET_GESTURE_MOUNT );		break;
		case CAI_GESTURE_DISMOUNT		: po_stEffectInfo.dwID = GetPlayerEffectID( 0, PET_GESTURE_DISMOUNT );	break;
		case CAI_GESTURE_SUMMON			: po_stEffectInfo.dwID = GetPlayerEffectID( 0, PET_GESTURE_SUMMON );	break;
		case CAI_GESTURE_REPATRIATE		: po_stEffectInfo.dwID = GetPlayerEffectID( 0, PET_GESTURE_REPATRIATE );break;
		case CAI_GESTURE_MOVEMAP_ENTER	: po_stEffectInfo.dwID = GetPlayerEffectID( 0, PET_MOVEMAP_ENTER );		break;
		case CAI_GESTURE_MOVEMAP_EXIT	: po_stEffectInfo.dwID = GetPlayerEffectID( 0, PET_MOVEMAP_EXIT );		break;
		case CAI_GESTURE_DUNGEON_ENTER	: po_stEffectInfo.dwID = GetPlayerEffectID( 0, PET_DUNGEON_ENTER );		break;
		case CAI_GESTURE_DUNGEON_EXIT	: po_stEffectInfo.dwID = GetPlayerEffectID( 0, PET_DUNGEON_EXIT );		break;
		case CAI_GESTURE_GAME_LOGOUT	: po_stEffectInfo.dwID = GetPlayerEffectID( 0, PET_GESTURE_LOGOUT );		break;
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

DWORD
CPlayerProc::GetPlayerEffectID( DWORD pi_dwEffectType1, DWORD pi_dwEffectType2 )
{
	DWORD dwEffectID = 0;

	dwEffectID	= m_pPlayer->m_byRace << 16;
	dwEffectID |= pi_dwEffectType1 << 12;
	dwEffectID |= pi_dwEffectType2 << 4;
	dwEffectID |= 0x0;						// 이펙트 단계

	return dwEffectID;
}

DWORD
CPlayerProc::GetItemEffectID( CBaseItem * pi_pItem, DWORD pi_dwEffectType )
{
	if( !pi_pItem )
		return ID_INVALID;

	DWORD dwEffectID = 0;

	dwEffectID = pi_pItem->GetMeshID() & 0xFFFFFF00;			// 아이템의 Mesh ID에서 처음 3가지 정보를 가져온다.
	dwEffectID |= ( pi_dwEffectType << 4 ) & 0xFFFFFFF0;		// 이펙트 종류
	dwEffectID |= pi_pItem->GetLevel();							// 업그레이드 레벨

	return dwEffectID;
}

// 스킬/포스 이펙트는 외부에서도 호출될 수 있으므로 CSFItem 을 파라미터로 받아야만 한다.
DWORD
CPlayerProc::GetSkillForceEffectID( DWORD pi_dwActionID, CSFItem * pi_pSFItem )
{
	if( !pi_pSFItem )
		return ID_INVALID;

	if( pi_pSFItem->IsEqualSFType( CAT_FORCE ) )
		return GetForceEffectID( pi_dwActionID, pi_pSFItem );
	else
		return GetSkillEffectID( pi_dwActionID, pi_pSFItem );
}

DWORD
CPlayerProc::GetSkillEffectID( DWORD pi_dwActionID, CSFItem * pi_pSFItem )
{
	DWORD dwEffectID = pi_pSFItem->GetDTCode() << 4;

	switch( pi_dwActionID )
	{
		case CAI_SKILL_CAST			: return dwEffectID |= ( pi_pSFItem->GetLevel() - 1 ) / 3;
		case CAI_SKILL_INVOKE		: return dwEffectID |= ( pi_pSFItem->GetLevel() - 1 ) / 3 + 3;
		case CAI_DAMAGED_BY_SKILL	: return dwEffectID |= ( pi_pSFItem->GetLevel() - 1 ) / 3 + 6;
	}

	return ID_INVALID;
}

DWORD
CPlayerProc::GetForceEffectID( DWORD pi_dwActionID, CSFItem * pi_pSFItem )
{
	DWORD dwEffectID = pi_pSFItem->GetDTCode() << 8;

	switch( pi_dwActionID )
	{
		case CAI_FORCE_CAST			: return dwEffectID |= pi_pSFItem->GetLevel() - 1;
		case CAI_FORCE_INVOKE		: return dwEffectID |= pi_pSFItem->GetLevel() - 1 + 7;
		case CAI_DAMAGED_BY_FORCE	: return dwEffectID |= pi_pSFItem->GetLevel() - 1 + 14;
	}

	return ID_INVALID;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CPlayerProc::ProcessActionID( DWORD pi_dwActionID )
{
	switch( pi_dwActionID )
	{
		case CAI_COMBAT_ATTACK		: SetEquipItemAnimation( IEPT_WEAPON, IAI_ATTACK ); break;
		case CAI_SKILL_INVOKE		: SetEquipItemAnimation( IEPT_WEAPON, IAI_ATTACK ); break;
		case CAI_ITEM_USE_MINING	: SetEquipItemAnimation( IEPT_WEAPON, IAI_USE ); break;
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CPlayerProc::ProcessEffectAfterSetAction( void )
{
	CPlayerSpecialEffectSecAction( m_pPlayer );		// by J.U.I
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CAniData *
CPlayerProc::GetAnimationByAction( DWORD pi_dwActionID )
{
	if( pi_dwActionID == ID_INVALID )
		return FALSE;

	DWORD dwAniTypeID;

	switch( pi_dwActionID )
	{
		case CAI_MOVE_STOP			: dwAniTypeID = PAI_STOP; break;
		case CAI_MOVE_MOVING		: dwAniTypeID = ( m_pPlayer->m_byWalkMode == CMM_MOVE_WALK ) ? PAI_WALK : PAI_RUN; break;
		case CAI_MOVE_IDLE			: dwAniTypeID = PAI_IDLE; break;
		case CAI_COMBAT_ATTACK		: dwAniTypeID = PAI_ATTACK; break;
		case CAI_ITEM_USE			: dwAniTypeID = PAI_USEITEM; break;
		case CAI_ITEM_USE_MINING	: dwAniTypeID = PAI_ATTACK; break;
		case CAI_CREATE_CREATE		: dwAniTypeID = PAI_CREATE_CREATE; break;
		case CAI_DAMAGED_CRITICAL	: dwAniTypeID = PAI_DAMAGE_CRITICAL; break;
		case CAI_DEATH_READY		: dwAniTypeID = PAI_DEATH_READY; break;
		case CAI_DEATH_DIE			: dwAniTypeID = PAI_DEATH_DIE; break;
	}

	return GetAnimationData( pi_dwActionID, dwAniTypeID );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

const int	MAX_RACE_TYPE_NUM = 5;
const int	MAX_COMBAT_MODE_NUM = 2;

// Internal Variable
const BYTE	RACE[MAX_RACE_TYPE_NUM][5] =
{
	{ CRT_ALL, CRT_BELLATOR_CORA, CRT_MALE,     CRT_BELLATOR, CRT_BELLATOR_MALE },
	{ CRT_ALL, CRT_BELLATOR_CORA, CRT_FEMALE,   CRT_BELLATOR, CRT_BELLATOR_FEMALE },
	{ CRT_ALL, CRT_BELLATOR_CORA, CRT_MALE,     CRT_CORA,     CRT_CORA_MALE },
	{ CRT_ALL, CRT_BELLATOR_CORA, CRT_FEMALE,   CRT_CORA,     CRT_CORA_FEMALE },
	{ CRT_ALL, CRT_ACCRETIA,	  CRT_ACCRETIA, CRT_ACCRETIA, CRT_ACCRETIA }
};

const BYTE	COMBAT_MODE[MAX_COMBAT_MODE_NUM][2] =
{
	{ 0x2, 0x0 },		// 비전투 모드
	{ 0x2, 0x1 }		// 전투 모드
};

const BYTE	ATTACK_PART[3][2] =
{
	{ 0x0, 0x1 },		// 상단 공격
	{ 0x1, 0x1 },		// 중단 공격
	{ 0x2, 0x1 }		// 하단 공격
};

BYTE	WEAPON_TYPE[2] =
{
	0xFF, 0xFF
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CAniData *
CPlayerProc::GetAnimationData( DWORD pi_dwActionID, DWORD pi_dwAniTypeID )
{
	CAniData * pNewAnimationData = NULL;

	if( ( pi_dwActionID >> 28 ) == CAI_FORCE )
		pNewAnimationData = GetAnimationData_Force( pi_dwActionID );
	else if( ( pi_dwActionID >> 28 ) == CAI_SKILL )
		pNewAnimationData = GetAnimationData_Skill( pi_dwActionID );
	else
		pNewAnimationData = GetAnimationData_Normal( pi_dwActionID, pi_dwAniTypeID );

	if( !pNewAnimationData )
		m_pPlayer->m_Animation.SetAnimationID( ID_INVALID );

	return pNewAnimationData;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CAniData *
CPlayerProc::GetAnimationData_Normal( DWORD pi_dwActionID, DWORD pi_dwAniTypeID )
{
	if( ( pi_dwActionID == ID_INVALID ) || ( pi_dwAniTypeID == ID_INVALID ) )
		return NULL;

	CEquipItem * pWeapon = GetEquipItem( IEPT_WEAPON );

	if( pWeapon )
		WEAPON_TYPE[0] = ( pWeapon->GetMeshID() & 0x0000FF00 ) >> 8;
	else
		WEAPON_TYPE[0] = 0xFF;

	return GetAnimationData_Normal_RaceTypeLoop( pi_dwActionID, pi_dwAniTypeID );
}

CAniData *
CPlayerProc::GetAnimationData_Normal_RaceTypeLoop( DWORD pi_dwActionID, DWORD pi_dwAniTypeID )
{
	CAniData *	pAnimationData = NULL;
	DWORD		dwAnimationID;

	for( int i = 0; i < MAX_RACE_TYPE_NUM; ++i )
	{
		dwAnimationID = 0;
		dwAnimationID = RACE[m_pPlayer->m_byRace][i] << 28;

		pAnimationData = GetAnimationData_Normal_CombatModeLoop( dwAnimationID, pi_dwActionID, pi_dwAniTypeID );
		if( pAnimationData )
			return pAnimationData;
	}

	return NULL;
}

CAniData *
CPlayerProc::GetAnimationData_Normal_CombatModeLoop( DWORD & pio_AnimationID, DWORD pi_dwActionID, DWORD pi_dwAniTypeID )
{
	CAniData * pAniData = NULL;

	for( int i = 0; i < MAX_COMBAT_MODE_NUM; ++i )
	{
		pio_AnimationID &= 0xF0000000;
		pio_AnimationID |= COMBAT_MODE[m_pPlayer->m_byCombatMode][i] << 24;

		pio_AnimationID |= pi_dwAniTypeID << 16;

		pAniData = GetAnimationData_Normal_WeaponLoop( pio_AnimationID, pi_dwActionID, pi_dwAniTypeID );
		if( pAniData )
			return pAniData;
	}

	return NULL;
}

CAniData *
CPlayerProc::GetAnimationData_Normal_WeaponLoop( DWORD & pio_AnimationID, DWORD pi_dwActionID, DWORD pi_dwAniTypeID )
{
	CAniData * pAniData = NULL;

	for( int i = 0; i < 2; ++i )
	{
		pio_AnimationID &= 0xFFFF0000;
		pio_AnimationID |= WEAPON_TYPE[i] << 8;

		pAniData = GetAnimationData_Normal_AttackPartLoop( pio_AnimationID, pi_dwActionID, pi_dwAniTypeID );
		if( pAniData )
			return pAniData;
	}

	return NULL;
}

CAniData *
CPlayerProc::GetAnimationData_Normal_AttackPartLoop( DWORD & pio_AnimationID, DWORD pi_dwActionID, DWORD pi_dwAniTypeID )
{
	CAniData *	pAniData = NULL;
	BYTE		byAttackPart;

	for( int i = 0; i < 2; ++i )
	{
		pio_AnimationID &= 0xFFFFFF00;

		if( pi_dwActionID == CAI_COMBAT_ATTACK )
		{
			byAttackPart = GetAnimationData_Normal_GetAttackPart( m_pPlayer->GetCurVictim() );
			pio_AnimationID |= ATTACK_PART[byAttackPart][i] << 4;
		}
		else
		{
			pio_AnimationID |= 0xF << 4;	// 공격 모드가 아니라면 무조건 0xF임
		}

		pAniData = m_pPlayer->GetAnimationDataByRandom( RLI_PLAYER, pio_AnimationID );
		if( pAniData )
			return pAniData;
	}

	return NULL;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BYTE
CPlayerProc::GetAnimationData_Normal_GetAttackPart( CCharacter * pi_pVictim )
{
	if( !pi_pVictim )
		return 1;

	float	fYDistance		= GetAnimationData_Normal_GetYDistance( pi_pVictim );
	int		nDistanceRatio	= GetAnimationData_Normal_GetDistanceRatio( pi_pVictim );

	// pi_pAttacker 가 pi_pVictim 보다 높음 ( 하단 모션 )
	if( fYDistance > nDistanceRatio )
	{
		return 2;
	}
	// pi_pAttacker 가 pi_pVictim 보다 낮음 ( 상단 모션 )
	else if( fYDistance < -nDistanceRatio )
	{
		return 0;
	}
	// 비슷한 곳에 있다면 몬스터와 플레이어로 나누어 처리 ( 중단 모션 )
	else
	{
		if( pi_pVictim->IsEqualCharType( CTI_PLAYER ) )
		{
			if( m_pPlayer->m_byAttackPart == CEPT_HELMET )
				return 0;
			else if( ( m_pPlayer->m_byAttackPart == CEPT_UPPER_PART ) || ( m_pPlayer->m_byAttackPart == CEPT_GLOVES ) )
				return 1;
			else if( ( m_pPlayer->m_byAttackPart == CEPT_LOWER_PART ) || ( m_pPlayer->m_byAttackPart == CEPT_SHOES ) )
				return 2;
		}
		else
		{
			return 1;
		}
	}

	return 1;
}

float
CPlayerProc::GetAnimationData_Normal_GetYDistance( CCharacter * pi_pVictim )
{
	float fYDistance = 0.0f;

	fYDistance = m_pPlayer->m_vecPos[1] - pi_pVictim->GetPosY();

	BBOX stAttackerBBox, stVictimBBox;

	m_pPlayer->GetBoundBox( stAttackerBBox, TRUE );
	pi_pVictim->GetBoundBox( stVictimBBox, TRUE );

	fYDistance += ( stAttackerBBox.pMax[1] / 2.0f ) -
				  ( ( stVictimBBox.pMax[1] / 2.0f ) * pi_pVictim->GetScale() );

	return fYDistance;
}

int
CPlayerProc::GetAnimationData_Normal_GetDistanceRatio( CCharacter * pi_pVictim )
{
	// 거리에 따른 Y단위를 계산 ( 거리 30 당 Y좌표는 3씩 증가 )
	return ( ( GetDist( m_pPlayer->m_vecPos, pi_pVictim->GetPosition() ) / 30.0f ) + 1 ) * 3;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CAniData *
CPlayerProc::GetAnimationData_Force( DWORD pi_dwActionID )
{
	CSFItem * pSFItem = m_pPlayer->GetSFItem();
	if( !pSFItem )
		return NULL;

	DWORD dwAniID = 0;

	dwAniID  = m_pPlayer->m_byRace << 8;
	dwAniID |= m_pPlayer->m_byCombatMode << 4;

	// 보조
	if( !pSFItem->IsAttackable() )
	{
		dwAniID |= 8;
	}
	// 공격
	else
	{
		BYTE			byHoldType	= IHT_BOTH_HAND;	// 맨손일 경우
		CNormalItem *	pWeapon		= GetNormalItem( IEPT_WEAPON );
		if( pWeapon )
			byHoldType = pWeapon->GetHoldType();

		// 시전 하는 포스의 거리타입에 따라서 애니메이션이 나뉜다.
		switch( pSFItem->GetRangeType() )
		{
			case 0 :
			case 1 :
			case 2 :
			case 3 :
				if( ( byHoldType == IHT_LEFT_HAND ) || ( byHoldType == IHT_RIGHT_HAND ) )
					dwAniID |= 0;
				else
					dwAniID |= 1;
				break;
			case 4 :
				if( ( byHoldType == IHT_LEFT_HAND ) || ( byHoldType == IHT_RIGHT_HAND ) )
					dwAniID |= 6;
				else
					dwAniID |= 7;
				break;
			case 5 :
			case 7 :
				if( ( byHoldType == IHT_LEFT_HAND ) || ( byHoldType == IHT_RIGHT_HAND ) )
					dwAniID |= 2;
				else
					dwAniID |= 3;
				break;
			case 6 :
				if( ( byHoldType == IHT_LEFT_HAND ) || ( byHoldType == IHT_RIGHT_HAND ) )
					dwAniID |= 4;
				else
					dwAniID |= 5;
				break;
			case 8 :
				dwAniID |= 8;
				break;
		}
	}

	// 스킬 / 포스는 모두 하나의 애니메이션만을 가지고 있다.
	CAniData * pAni = _GetAniMgr()->GetAni( RLI_FORCE, dwAniID );
	if( pAni )
		m_pPlayer->m_Animation.SetAnimationID( dwAniID );

	return pAni;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CAniData *
CPlayerProc::GetAnimationData_Skill( DWORD pi_dwActionID )
{
	CSFItem * pSFItem = m_pPlayer->GetSFItem();
	if( !pSFItem )
		return NULL;

	// [0] => 원래 렌더되어야 할 애니메이션, [1] => 대체되어야 할 애니메이션
	DWORD dwAniID[2] = { 0, 0 };

	dwAniID[0] = m_pPlayer->m_byRace << 28;
	dwAniID[0] |= ( pSFItem->GetDTCode() & 0x0000FFFF ) << 12;

	if( pSFItem->IsAttackable() )
	{
		// 공격형은 무기 유형을 따진다.
		CNormalItem * pWeapon = GetNormalItem( IEPT_WEAPON );
		if( !pWeapon )
			return NULL;

		dwAniID[0] |= ( pWeapon->GetMeshID() & 0x0000FF00 ) >> 4;
	}
	else
	{
		// 보조는 전투 모드를 따진다.
		dwAniID[0] |= m_pPlayer->m_byCombatMode << 4;
	}

	// 스킬의 발동 혹은 피해인지를 파악해야한다.
	if( pi_dwActionID == CAI_SKILL_INVOKE )				// 발동
	{
		dwAniID[0] |= ( ( pSFItem->GetLevel() - 1 ) / 3 );
	}
	else if( pi_dwActionID == CAI_DAMAGED_BY_SKILL )	// 피해
	{
		dwAniID[0] |= ( ( ( pSFItem->GetLevel() - 1 ) / 3 ) + 3 );
	}
	else
	{
		return NULL;
	}

	// 대체 애니메이션 ID => 대체 애니메이션은 스킬이름 부분이 FFF임.
	dwAniID[1] = dwAniID[0] | 0x00FFF000;

	// 스킬 or 포스는 모두 하나의 애니메이션만을 가지고 있다.
	CAniData * pAni = NULL;
	for( int i = 0; i < 2; ++i )
	{
		pAni = _GetAniMgr()->GetAni( RLI_SKILL, dwAniID[i] );
		if( pAni )
			return pAni;
	}

	m_pPlayer->m_Animation.SetAnimationID( ID_INVALID );
	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CPlayerProc::SetEquipItemAnimation( BYTE pi_byPartType, BYTE pi_byAniTypeID )
{
	CEquipItem * pEquipItem = GetEquipItem( pi_byPartType );
	if( pEquipItem )
		pEquipItem->SetAnimation( pi_byAniTypeID );
}

void
CPlayerProc::SetBone( DWORD pi_dwBoneID )
{
	m_pBone = _GetBoneMgr()->GetBone( RLI_PLAYER, pi_dwBoneID );

	m_pBBox = _GetBBoxMgr()->GetBoundBox( RLI_PLAYER, pi_dwBoneID );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CPlayerProc::PutOnEquipment( BYTE pi_byPartType, DWORD pi_dwDTIndex )
{
	if( !CreateEquipment( pi_byPartType, pi_dwDTIndex ) )
		return FALSE;

	PutOnEquipment_SetWeaponAndShield( pi_byPartType );

	m_pPlayer->m_stConditionInfo.ApplyItemEffect( &m_listNormalItem[pi_byPartType], TRUE );

	return TRUE;
}

BOOL
CPlayerProc::PutOnEquipment( CBoardItem * pi_pBoardItem )
{
	if( !CreateEquipment( pi_pBoardItem ) )
		return FALSE;

	PutOnEquipment_SetWeaponAndShield( pi_pBoardItem->GetItemKind() );

	m_pPlayer->m_stConditionInfo.ApplyItemEffect( &m_listNormalItem[pi_pBoardItem->GetItemKind()], TRUE );

	return TRUE;
}

BOOL
CPlayerProc::PutOnEquipment( CEquipItem * pi_pEquipItem )
{
	if( !CreateEquipment( pi_pEquipItem ) )
		return FALSE;

	PutOnEquipment_SetWeaponAndShield( pi_pEquipItem->GetItemKind() );

	m_pPlayer->m_stConditionInfo.ApplyItemEffect( &m_listNormalItem[pi_pEquipItem->GetItemKind()], TRUE );

	return TRUE;
}

void
CPlayerProc::PutOnEquipment_SetWeaponAndShield( BYTE pi_byPartType )
{
	if( ( pi_byPartType == IEPT_WEAPON ) || ( pi_byPartType == IEPT_SHIELD ) )
	{
		CMesh * pMesh = m_listEquipItem[pi_byPartType].GetMesh();
		if( pMesh )
			_GetCharIF()->RematchParent( m_pBone, pMesh );

		// 전투 모드에선 fade-out, 비전투 모드에선 fade-in으로 처리.
		if( m_pPlayer->m_byCombatMode == CSI_PEACE )
			m_listEquipItem[pi_byPartType].SetFadeMode( FRM_IN_END );
		else
			m_listEquipItem[pi_byPartType].SetFadeMode( FRM_OUT_END );

		// 무기를 바꿨을 경우에는 애니메이션을 변경해야 한다.
		if( pi_byPartType == IEPT_WEAPON )
			m_pPlayer->SetAction( m_pPlayer->m_dwActionID, FALSE );
	}
}

BOOL
CPlayerProc::PutOffEquipment( BYTE pi_byPartType )
{
	if( pi_byPartType > MAX_EQUIPMENT )
		return FALSE;

	// delete item
	m_listEquipItem[pi_byPartType].SetEmpty();
	m_listEquipItem[pi_byPartType].SetLinkItem( NULL );

	// remove effect
	m_pPlayer->m_stConditionInfo.ApplyItemEffect( &m_listNormalItem[pi_byPartType], FALSE );

	// set animation
	if( pi_byPartType == IEPT_WEAPON )
		m_pPlayer->SetAction( m_pPlayer->m_dwActionID, FALSE );

	return TRUE;
}
//============================================================================//
//                                  Boundary                                  //
//============================================================================//


CEquipItem *
CPlayerProc::GetEquipItem( BYTE pi_byPartType )
{
	if( pi_byPartType > MAX_EQUIPMENT )
		return NULL;

	if( m_listEquipItem[pi_byPartType].IsEmpty() )
		return NULL;

	return &m_listEquipItem[pi_byPartType];
}

CNormalItem *
CPlayerProc::GetNormalItem( BYTE pi_byPartType )
{
	if( pi_byPartType > MAX_EQUIPMENT )
		return NULL;

	if( m_listEquipItem[pi_byPartType].IsEmpty() )	// check equipment is empty - not normal item
		return NULL;

	return &m_listNormalItem[pi_byPartType];
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CPlayerProc::CreateEquipment( BYTE pi_byPartType, DWORD pi_dwDTIndex )
{
	if( pi_byPartType > MAX_EQUIPMENT )
		return FALSE;

	// is hair?
	if( ( pi_byPartType == IEPT_HELMET ) && ( pi_dwDTIndex < MAX_DEFAULT_ITEM_NUM * 5 ) )
		pi_byPartType = CEPT_HAIR;

	// is empty?
	if( !m_listEquipItem[pi_byPartType].IsEmpty() )
		PutOffEquipment( pi_byPartType );

	// set item kind
	BYTE byItemKind = pi_byPartType;
	if( pi_byPartType == CEPT_HAIR )
		byItemKind = IEPT_HELMET;

	// create item ( pi_dwDTIndex의 최상위 바이트를 플레이어의 종족으로 세팅해야 함. )
	if( !m_listEquipItem[pi_byPartType].Create( byItemKind, pi_dwDTIndex | ( m_pPlayer->m_byRace << 28 ) ) )
	{
		m_listEquipItem[pi_byPartType].SetEmpty();
		return FALSE;
	}
	m_listEquipItem[pi_byPartType].SetAnimation( IAI_EQUIP );

	if( !m_listNormalItem[pi_byPartType].Create( byItemKind, pi_dwDTIndex ) )
	{
		return FALSE;
	}

	m_listEquipItem[pi_byPartType].SetLinkItem( &m_listNormalItem[pi_byPartType] );

	return TRUE;
}

BOOL
CPlayerProc::CreateEquipment( CBoardItem * pi_pBoardItem )
{
	if( !pi_pBoardItem )
		return FALSE;

	// create item
	if( !CreateEquipment( pi_pBoardItem->GetItemKind(), pi_pBoardItem->GetDTIndex() ) )
		return FALSE;

	// set property
	BYTE byPartyType = pi_pBoardItem->GetItemKind();
	m_listEquipItem[byPartyType].SetIndex( pi_pBoardItem->GetIndex() );
	m_listEquipItem[byPartyType].SetServerIndex( pi_pBoardItem->GetServerIndex() );

	pi_pBoardItem->CopyToNormalItem( &m_listNormalItem[byPartyType] );

	return TRUE;
}

BOOL
CPlayerProc::CreateEquipment( CEquipItem * pi_pEquipItem )
{
	if( !pi_pEquipItem )
		return FALSE;

	// create item
	if( !CreateEquipment( pi_pEquipItem->GetItemKind(), pi_pEquipItem->GetDTIndex() ) )
		return FALSE;

	// set property
	BYTE byPartyType = pi_pEquipItem->GetItemKind();
	m_listEquipItem[byPartyType].SetIndex( pi_pEquipItem->GetIndex() );
	m_listEquipItem[byPartyType].SetServerIndex( pi_pEquipItem->GetServerIndex() );

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CPlayerProc::ChangeMoveMode( BYTE pi_byMode )
{
	// 스킬/포스에 의한 뛰기 제한 체크
	if( ( pi_byMode == CMM_MOVE_RUN ) && m_pPlayer->m_stConditionInfo.m_bDisable_Run )
	{
		_SetSystemMsg( "현재 아바타가 뛰기 제한에 걸려있습니다." );
		return;
	}

	// 스테미너 체크
	if( ( pi_byMode == CMM_MOVE_RUN ) && ( m_pPlayer->GetSP() <= 0 ) )
	{
		_SetSystemMsg( "스테미너가 없어서 뛸 수 없습니다." );
		return;
	}

	_GetNetworkMgr()->PositionMsg_MoveTypeChangeRequest_clzo( pi_byMode );
}

float
CPlayerProc::GetAttackRange( BYTE pi_byAttackType )
{
	float fAttackRange = 0.0f;

	if( pi_byAttackType == CAT_FORCE )
	{
		CSFItem * pSFItem = m_pPlayer->GetSFItem();

		// 아바타일 경우에는 CAvatar::UseSkillForce_Request() 에서와 동일한 방법으로 CSFItem 을 가져와야 한다.
		if( m_pPlayer->IsEqualCharType( CTI_AVATAR ) )
		{
			if( _GetGBM()->GetSelectSF() )
				pSFItem = _GetGBM()->GetSelectSF();
			else if( _GetNextActionMgr()->IsEqualNextAction( CAI_SKILL_INVOKE ) ||
					 _GetNextActionMgr()->IsEqualNextAction( CAI_FORCE_INVOKE ) )
				pSFItem = _GetNextActionMgr()->GetNextSF();
		}

		if( pSFItem )
			fAttackRange = pSFItem->GetAttackRange();
	}
	else
	{
		if( !m_pData )
			fAttackRange;

		fAttackRange += m_pData->m_fWidth / 2.0f;		// + ( 아바타의 몸통크기 / 2 )

		CEquipItem * pWeapon = GetEquipItem( IEPT_WEAPON );
		if( pWeapon )									// + 무기의 사정거리
			fAttackRange += pWeapon->GetAttackRange();
		else											// + ( 무기가 없을 경우는 주먹 사정거리 )
			fAttackRange += m_pData->m_fAttackRange;
	}

	return fAttackRange;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CPlayerProc::SetLightColor( DWORD pi_dwColor, BYTE pi_bFlag )
{
	for( int i = 0; i < MAX_EQUIPMENT; ++i )
	{
		if( m_listEquipItem[i].IsEmpty() )
			continue;

		m_listEquipItem[i].SetLightColor( pi_dwColor, pi_bFlag );
	}
}

void
CPlayerProc::SetLightColorFromMap( void )
{
	for( int i = 0; i < MAX_EQUIPMENT; ++i )
	{
		if( m_listEquipItem[i].IsEmpty() )
			continue;

		m_listEquipItem[i].SetMaterial( &m_pPlayer->m_d3dMaterial );
	}
}

void
CPlayerProc::SetAlphaDensity( float pi_fAlpha )
{
	for( int i = 0; i < MAX_EQUIPMENT; ++i )
	{
		if( m_listEquipItem[i].IsEmpty() )
			continue;

		m_listEquipItem[i].SetAlphaDensity( pi_fAlpha );
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CPlayerProc::Enable( DWORD pi_dwFlag )
{
	for( int i = 0; i < MAX_EQUIPMENT; ++i )
	{
		if( m_listEquipItem[i].IsEmpty() )
			continue;

		m_listEquipItem[i].Enable( pi_dwFlag );
	}
}

void
CPlayerProc::Disable( DWORD pi_dwFlag )
{
	for( int i = 0; i < MAX_EQUIPMENT; ++i )
	{
		if( m_listEquipItem[i].IsEmpty() )
			continue;

		m_listEquipItem[i].Disable( pi_dwFlag );
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CPlayerProc::LoadAnimaitonData( void )
{
	CAniData * pAniData = NULL;

	m_pPlayer->SetCombatMode( CSI_COMBAT );
	pAniData = GetAnimationData( m_pPlayer->m_dwActionID, PAI_STOP );
	if( pAniData ) pAniData->LoadRealAnimation( TRUE );
	pAniData = GetAnimationData( m_pPlayer->m_dwActionID, PAI_WALK );
	if( pAniData ) pAniData->LoadRealAnimation( TRUE );
	pAniData = GetAnimationData( m_pPlayer->m_dwActionID, PAI_RUN );
	if( pAniData ) pAniData->LoadRealAnimation( TRUE );

	m_pPlayer->SetCombatMode( CSI_PEACE );
	pAniData = GetAnimationData( m_pPlayer->m_dwActionID, PAI_STOP );
	if( pAniData ) pAniData->LoadRealAnimation( TRUE );
	pAniData = GetAnimationData( m_pPlayer->m_dwActionID, PAI_WALK );
	if( pAniData ) pAniData->LoadRealAnimation( TRUE );
	pAniData = GetAnimationData( m_pPlayer->m_dwActionID, PAI_RUN );
	if( pAniData ) pAniData->LoadRealAnimation( TRUE );
	pAniData = GetAnimationData( m_pPlayer->m_dwActionID, PAI_DEATH_READY );
	if( pAniData ) pAniData->LoadRealAnimation( TRUE );
	pAniData = GetAnimationData( m_pPlayer->m_dwActionID, PAI_DEATH_DIE );
	if( pAniData ) pAniData->LoadRealAnimation( TRUE );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////