////////////////////////////////////////////////////////////////////////////////
//
// CNetMsgProcessor_Combat Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _NEW_UI_

#include "../../../GameObject/Manager/CCharacterMgr.h"
#include "../../../GameObject/Manager/CTargetMgr.h"
#include "../../../GameObject/Board/Common/CBoardManager.h"

#include "../../../Character/Avatar/CAvatar_Quest.h"

#include "CNetMsgProcessor_Combat.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CCombatResult::CCombatResult()
{
	ZeroMemory( this, sizeof( CCombatResult ) );

	m_byAttackType	= CAT_NORMAL;
	m_byAttackPart	= ID_INVALID;
	m_dwSFIndex		= ID_INVALID;
	m_bySFLevel		= 1;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CCombatResult::ProcessCombatResult( void )
{
	CCharacter * pAttacker	= NULL;
	CCharacter * pVictim	= NULL;

	pAttacker	= _GetCharMgr()->GetCharacter( m_stAttacker.byID, m_stAttacker.dwSerial );

	// process attacker
	if( !ProcessAttacker( pAttacker ) )
		return FALSE;

	// process victim info of attacker
	SetVictimOfAttacker( pAttacker );

	// process attacker info of victim
	SetAttackerOfVictim( pAttacker );

	// etc
	ProcessETC( pAttacker );

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

DWORD	COMBAT_ACTION_ID[4] = { CAI_SKILL_INVOKE, CAI_FORCE_INVOKE, CAI_SKILL_INVOKE, CAI_COMBAT_ATTACK };
char	COMBAT_FAIL_STRING[4][16] = { "스킬 시전 실패", "포스 시전 실패", "스킬 시전 실패", "일반 공격 실패" };

BOOL
CCombatResult::ProcessAttacker( CCharacter * pi_pAttacker )
{
	if( !pi_pAttacker )
		return FALSE;

	pi_pAttacker->SetServerIndex( m_stAttacker.wIndex );
	pi_pAttacker->GetCombatState()->SetReceiveAttackResultMsg( TRUE );
	pi_pAttacker->SetReceiveNetMsg( "Attack/SF" );

	// display message
#ifdef _DEVELOPER_
	if( pi_pAttacker->IsEqualCharType( CTI_AVATAR ) && ( m_crcResult != CRC_SUCCESS ) )
		_SetSystemMsg( "%s ( %d )", COMBAT_FAIL_STRING[m_byAttackType], m_nServerErrorCode );
#endif

	// process by character type
	if( pi_pAttacker->IsEqualCharType( CTI_AVATAR ) )
	{
		if( !ProcessAttacker_ProcessAvatar( pi_pAttacker ) )
			return FALSE;
	}
	else
	{
		if( !ProcessAttacker_ProcessOther( pi_pAttacker ) )
			return FALSE;
	}

	// process bullet
	ProcessAttacker_ProcessBullet( pi_pAttacker );

	// etc
	return ProcessAttacker_ProcessETC( pi_pAttacker );
}

BOOL
CCombatResult::ProcessAttacker_ProcessAvatar( CCharacter * pi_pAttacker )
{
	if( m_byAttackType != CAT_NORMAL )
	{
		if( !_GetAvatar()->GetSFItem() )
			return FALSE;
		if( _GetAvatar()->GetSFItem()->GetDTIndex() != m_dwSFIndex )
			return FALSE;

		if( m_crcResult == CRC_SUCCESS )
			_GetAvatar()->SetSFUsingMsg( TRUE );
		else
			_GetAvatar()->SetSFUsingMsg( FALSE );
	}

	return TRUE;
}

BOOL
CCombatResult::ProcessAttacker_ProcessOther( CCharacter * pi_pAttacker )
{
	// 아바타가 아닐 경우에는 전투 모드로 변경시킨다.
	if( pi_pAttacker->GetCombatMode() != CSI_COMBAT )
		pi_pAttacker->SetCombatMode( CSI_COMBAT );

	// monster
	if( pi_pAttacker->IsEqualCharType( CTI_MONSTER ) && ( m_byAttackType == CAT_FORCE ) )
	{
		if( !static_cast< CMonster * >( pi_pAttacker )->IsHaveForce( m_dwSFIndex ) )
			return FALSE;
	}

	// skill & force
	if( m_byAttackType != CAT_NORMAL )
	{
		// create skill / force item
		CSFItem clSFItem;
		if( !clSFItem.Create( m_byAttackType, m_dwSFIndex ) )
			return FALSE;
		clSFItem.SetIndex( m_dwSFIndex );
		clSFItem.SetLevel( m_bySFLevel );

		pi_pAttacker->SetSFItem( &clSFItem );

		// player
		if( pi_pAttacker->IsEqualCharType( CTI_PLAYER ) )
		{
			m_vecSFPosition[1] = pi_pAttacker->GetPosY();
			static_cast< CPlayer * >( pi_pAttacker )->SetSFTargetPos( m_vecSFPosition );
			pi_pAttacker->SetRotY( pi_pAttacker->GetPosition(), m_vecSFPosition );
		}
	}

	return TRUE;
}

void
CCombatResult::ProcessAttacker_ProcessBullet( CCharacter * pi_pAttacker )
{
	if( m_crcResult != CRC_ERROR )
	{
		if( ( m_byAttackType != CAT_FORCE ) && ( m_byBulletDTIndex != 0xFF ) )
		{
			if( pi_pAttacker->IsEqualCharType( CTI_PLAYER ) )
				static_cast< CPlayer * >( pi_pAttacker )->SetBulletDTIndex( m_byBulletDTIndex );
		}
	}
}

BOOL
CCombatResult::ProcessAttacker_ProcessETC( CCharacter * pi_pAttacker )
{
	if( m_crcResult == CRC_ERROR )
	{
		pi_pAttacker->GetCombatState()->SetAttackError( TRUE );
		return FALSE;
	}
	else
	{
		pi_pAttacker->GetCombatState()->SetAttackError( FALSE );
	}

	if( pi_pAttacker->IsDeath() )
		return FALSE;

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CCombatResult::SetVictimOfAttacker( CCharacter * pi_pAttacker )
{
	CCharacter *	pVictim	= NULL;
	CVictimInfo *	pVictimInfo = NULL;

	for( int i = 0; i < m_byTotalVictim; ++i )
	{
		pVictim = _GetCharMgr()->GetCharacter( m_listVictim[i].idDster.byID, m_listVictim[i].idDster.dwSerial );
		if( !pVictim )
			continue;

		// get victim info
		if( pi_pAttacker->IsEqualCharType( CTI_AVATAR ) )
		{
			pVictimInfo = pi_pAttacker->GetVictimInfo( pVictim );
			if( !pVictimInfo )
				pVictimInfo = pi_pAttacker->AddVictimInfo( pVictim, !i );
		}
		else
		{
			pVictimInfo = pi_pAttacker->AddVictimInfo( pVictim, TRUE );
		}

		// set victim info
		if( pVictimInfo )
		{
			if( m_listVictim[i].wDamage <= 0 )
				m_listVictim[i].wDamage = 0;

			pVictimInfo->SetAttacked( TRUE );
			pVictimInfo->SetAttackPoint( pVictimInfo->GetAttackPoint() + m_listVictim[i].wDamage );
		}

		if( ( !pi_pAttacker->IsEqualCharType( CTI_AVATAR ) || ( i == 0 ) ) && !m_bIsCounterAttack )
			pi_pAttacker->SetRotY( pi_pAttacker->GetPosition(), pVictim->GetPosition() );
	}

	if( !pi_pAttacker->IsEqualCharType( CTI_AVATAR ) || m_bIsCounterAttack )
	{
		// Set Action - 피해자 세탕후에 호출해야 이펙트가 제대로 나감
		pi_pAttacker->SetAction( COMBAT_ACTION_ID[m_byAttackType], FALSE );
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CCombatResult::SetAttackerOfVictim( CCharacter * pi_pAttacker )
{
	CCharacter *	pVictim	= NULL;
	CAttackerInfo * pi_pAttackerInfo = NULL;

	for( int i = 0; i < m_byTotalVictim; ++i )
	{
		// get victim
		pVictim = _GetCharMgr()->GetCharacter( m_listVictim[i].idDster.byID, m_listVictim[i].idDster.dwSerial );
		if( !pVictim )
			continue;
		pVictim->SetServerIndex( m_listVictim[i].idDster.wIndex );
		pVictim->SetReceiveNetMsg( "Damaged" );

		if( pVictim->IsDeath() )
			continue;

		// set attacker info
		pi_pAttackerInfo = pVictim->AddAttackerInfo( pi_pAttacker, TRUE );
		if( pi_pAttackerInfo )
		{
			pi_pAttackerInfo->SetTotalDamagePoint( pi_pAttackerInfo->GetTotalDamagePoint() + m_listVictim[i].wDamage );
			pi_pAttackerInfo->SetDamageType_Dist( pi_pAttacker->GetAttackDistanceType( m_byAttackType ) );
			pi_pAttackerInfo->SetDamageType_AT( m_byAttackType );
			pi_pAttackerInfo->SetDamageType_CT( m_bIsCritical );
			pi_pAttackerInfo->SetEffectFrameNum( pi_pAttacker->GetMaxEffectFrameNum() );

			if( m_byAttackType != CAT_NORMAL )
				pi_pAttackerInfo->SetSFItem( pi_pAttacker->GetSFItem() );
		}

		// HP - fail
		if( m_crcResult != CRC_SUCCESS )
			m_listVictim[i].wLeftHP = pVictim->GetHP();

		// HP - Success - Avatar
		if( pVictim->IsEqualCharType( CTI_AVATAR ) )
		{
			if( _GetAvatar()->IsEqualUnitMountState( UMS_MOUNT ) )
				_GetAvatar()->GetCombatState()->SetRemainHP( _GetAvatar()->GetHP() - m_listVictim[i].wDamage );
			else
				_GetAvatar()->GetCombatState()->SetRemainHP( m_listVictim[i].wLeftHP );

			if( m_crcResult == CRC_SUCCESS )
				_GetAvatar()->SetHP( _GetAvatar()->GetCombatState()->GetRemainHP() );
		}
		// HP - Success - Other
		else
		{
			pVictim->GetCombatState()->SetRemainHP( m_listVictim[i].wLeftHP );
		}
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CCombatResult::ProcessETC( CCharacter * pi_pAttacker )
{
	if( pi_pAttacker->IsEqualCharType( CTI_AVATAR ) )
	{
		_GetAvatar()->GetTutorialInfo()->SetCombatInfo( m_byAttackType, m_dwSFIndex,
														m_listVictim[0].idDster.byID,
														m_listVictim[0].idDster.wIndex );
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CNetMsgProcessor_Combat::CNetMsgProcessor_Combat()
{
}

CNetMsgProcessor_Combat::~CNetMsgProcessor_Combat()
{
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CNetMsgProcessor_Combat::SendNetMessage( BYTE pi_byMsgIndex, void * pi_pMsg, int pi_nSize )
{
	return CNetMsgProcessor_Base::SendNetMessage( fight_msg, pi_byMsgIndex, pi_pMsg, pi_nSize );
}

bool
CNetMsgProcessor_Combat::AnalysisServerPacket( _MSG_HEADER * pMsgHeader, char* pi_pMsg )
{
	if( pMsgHeader->m_byType[0] != fight_msg )
		return false;

	switch( pMsgHeader->m_byType[1] )
	{
		case attack_gen_result_zocl	:
			AttackGenResult_zocl( pi_pMsg );
			break;

		case attack_skill_result_zocl	:
			AttackSkillResult_zocl( pi_pMsg );
			break;

		case attack_force_result_zocl	:
			AttackForceResult_zocl( pi_pMsg );
			break;

		case attack_unit_result_zocl	:
			AttackUnitResult_zocl( pi_pMsg );
			break;

		case attack_test_result_zocl	:
			AttackTestResult_zocl( pi_pMsg );
			break;

		case attack_count_result_zocl	:
			AttackCountResult_zocl( pi_pMsg );
			break;

		case attack_monster_inform_zocl	:
			AttackMonsterInform_zocl( pi_pMsg );
			break;

		case attack_animus_inform_zocl	:
			AttackAnimusInform_zocl( pi_pMsg );
			break;

		case attack_tower_inform_zocl	:
			AttackTowerInform_zocl( pi_pMsg );
			break;

		case mode_change_result_zocl	:
			ModeChangeResult_zocl( pi_pMsg );
			break;

		case breakdown_equip_item_zocl	:
			BreakdownEquipItem_zocl( pi_pMsg );
			break;

		case shield_equip_dur_dec_zocl	:
			ShieldEquipDurDec_zocl( pi_pMsg );
			break;

		case alter_weapon_bullet_inform_zocl	:
			AlterWeaponBulletInform_zocl( pi_pMsg );
			break;

		case alter_unit_bullet_inform_zocl	:
			AlterUnitBulletInform_zocl( pi_pMsg );
			break;

		case alter_unit_hp_inform_zocl	:
			AlterUnitHpInform_zocl( pi_pMsg );
			break;

		case monster_chase_change_zocl	:
			MonsterChaseChange_zocl( pi_pMsg );
			break;

		case attack_error_result_zocl	:
			AttackErrorResult_zocl( pi_pMsg );
			break;

		default :
			m_pNetworkMgr->NetLogWrite( "Unknown Message : CNetMsgProcessor_Combat::AnalysisServerPacket()\n" );
			return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 아바타의 공격 전 세팅 ( 일반/스킬/포스 )
void
CNetMsgProcessor_Combat::SetAvatarAttackState( BYTE pi_byAttackType, CCharacter * pi_pVictim )
{
	// set victim
	if( pi_pVictim )
	{
		if( !pi_pVictim->IsEqualCharType( CTI_AVATAR ) )
		{
			_GetAvatar()->SetRotY( _GetAvatar()->GetPosition(), pi_pVictim->GetPosition() );

			CVictimInfo * pVictimInfo = _GetAvatar()->AddVictimInfo( pi_pVictim, TRUE );
			if( pVictimInfo )
				pVictimInfo->SetAttacked( FALSE );
		}
	}
	else
	{
		if( pi_byAttackType == CAT_FORCE )
			_GetAvatar()->SetRotY( _GetAvatar()->GetPosition(), _GetAvatar()->GetSFTargetPos() );
	}

	// set state
	_GetAvatar()->SetAttackMode( pi_byAttackType );
	_GetAvatar()->GetCombatState()->SetAttackError( FALSE );
	_GetAvatar()->GetCombatState()->SetReceiveAttackResultMsg( FALSE );

	// set action
	if( pi_byAttackType == CAT_NORMAL )
		_GetAvatar()->SetAction( CAI_COMBAT_ATTACK );
	else if( pi_byAttackType == CAT_FORCE )
		_GetAvatar()->SetAction( CAI_FORCE_INVOKE );
	else
		_GetAvatar()->SetAction( CAI_SKILL_INVOKE );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 일반 공격 요청
BOOL
CNetMsgProcessor_Combat::AttackGenRequest_clzo( CCharacter * pi_pVictim )
{
	if( !pi_pVictim )
		return FALSE;

	_attack_gen_request_clzo stSendMsg;

	stSendMsg.byID			= pi_pVictim->GetCharTypeID();
	stSendMsg.wIndex		= pi_pVictim->GetServerIndex();
	stSendMsg.byAttPart		= IEPT_CLIENT_TO_SERVER[_GetAvatar()->GetAttackPart()];	// 공격부위 ( -1이면 랜덤 )
	stSendMsg.wBulletSerial	= _GetEquipBoard()->GetUsingBulletIndex();

	SetAvatarAttackState( CAT_NORMAL, pi_pVictim );
	_GetGGI()->SetRequestProcessState( TRUE );

	return SendNetMessage( attack_gen_request_clzo,
						   &stSendMsg,
						   sizeof( _attack_gen_request_clzo ) );

	/*
	#define attack_gen_request_clzo 1
	struct _attack_gen_request_clzo {
		BYTE byID;			
		WORD wIndex;
		BYTE byAttPart;		
		WORD wBulletSerial;	//	==> bullet과 무관한 공격이면 0xFF
	};*/
}

// 일반 공격 결과 ( 타 플레이어의 공격도 포함되어 있음 )
// 데미지가 1이상이면 성공, 0이면 실패
void
CNetMsgProcessor_Combat::AttackGenResult_zocl( char * pi_pMsg )
{
	_attack_gen_result_zocl * pRecvMsg = reinterpret_cast< _attack_gen_result_zocl * >( pi_pMsg );

	_GetGGI()->SetRequestProcessState( FALSE );

	CCombatResult clsCombatResult;

	clsCombatResult.m_byAttackType			= CAT_NORMAL;
	clsCombatResult.m_byAttackPart			= pRecvMsg->byAttackPart;

	memcpy( &clsCombatResult.m_stAttacker, &pRecvMsg->idAtter, sizeof( CCombatResult::ATTACKER ) );

	clsCombatResult.m_byTotalVictim			= pRecvMsg->byListNum;
	memcpy( clsCombatResult.m_listVictim, pRecvMsg->DamList, sizeof( CCombatResult::VICTIM ) * pRecvMsg->byListNum );

	clsCombatResult.m_bIsCritical			= pRecvMsg->bCritical;
	clsCombatResult.m_bIsCounterAttack		= FALSE;
	clsCombatResult.m_byBulletDTIndex		= pRecvMsg->byBulletIndex;

	clsCombatResult.m_crcResult				= CRC_FAIL;
	for( int i = 0; i < pRecvMsg->byListNum; ++i )
	{
		if( pRecvMsg->DamList[i].wDamage > 0 )
			clsCombatResult.m_crcResult		= CRC_SUCCESS;
	}

	clsCombatResult.ProcessCombatResult();

	/*
	#define attack_gen_result_zocl 254
	struct _attack_gen_result_zocl {	//add(3/17)
		_CHRID idAtter;	
		BYTE byAttackPart;
		BYTE byBulletIndex;
		bool bCritical;		//1이면 크리티컬공격

		BYTE byListNum;

		struct _dam_list{
			_CHRID idDster;	
			WORD wDamage;  //데미지 
			WORD wLeftHP;
		};

		_dam_list DamList[32];
		
		int size(){
			return sizeof(*this)-sizeof(_dam_list)*(32-byListNum);
		}
	};*/
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 공격 스킬 시전 요청
BOOL
CNetMsgProcessor_Combat::AttackSkillRequest_clzo( CCharacter * pi_pVictim )
{
	_attack_skill_request_clzo stSendMsg;

	if( pi_pVictim )
	{
		if( pi_pVictim->IsEqualCharType( CTI_AVATAR ) )
			stSendMsg.byID		= CTI_PLAYER;
		else
			stSendMsg.byID		= pi_pVictim->GetCharTypeID();
		stSendMsg.wIndex		= pi_pVictim->GetServerIndex();
	}
	else
	{
		stSendMsg.byID			= 0xFF;
		stSendMsg.wIndex		= 0xFFFF;
	}
	stSendMsg.byEffectCode		= _GetAvatar()->GetSFItem()->GetSFType();
	stSendMsg.wSkillIndex		= _GetAvatar()->GetSFItem()->GetIndex();
	stSendMsg.wBulletSerial		= _GetEquipBoard()->GetUsingBulletIndex();

	SetAvatarAttackState( stSendMsg.byEffectCode, pi_pVictim );
	_GetGGI()->SetRequestProcessState( TRUE );

	return SendNetMessage( attack_skill_request_clzo,
						   &stSendMsg,
						   sizeof( _attack_skill_request_clzo ) );

	/*
	#define attack_skill_request_clzo 5
	struct _attack_skill_request_clzo {	
		BYTE byID;				
		WORD wIndex;
		BYTE byEffectCode;	//	effect_code_skill, effect_code_class ..		add(4/18)
		WORD wSkillIndex;
		WORD wBulletSerial;		//bullet 무관 : 0xFFFF
	};*/
}

// 공격 스킬 시전 결과 ( 타 플레이어의 공격도 포함되어 있음 )
// 데미지가 1이상이면 성공, 0이면 실패
void
CNetMsgProcessor_Combat::AttackSkillResult_zocl( char * pi_pMsg )
{
	_attack_skill_result_zocl * pRecvMsg	= reinterpret_cast< _attack_skill_result_zocl * >( pi_pMsg );
	_GetGGI()->SetRequestProcessState( FALSE );

	CCombatResult clsCombatResult;

	clsCombatResult.m_byAttackType		= pRecvMsg->byEffectCode;
	clsCombatResult.m_byAttackPart		= pRecvMsg->byAttackPart;
	clsCombatResult.m_dwSFIndex			= pRecvMsg->wSkillIndex;
	clsCombatResult.m_bySFLevel			= pRecvMsg->bySkillLv;

	memcpy( &clsCombatResult.m_stAttacker, &pRecvMsg->idAtter, sizeof( CCombatResult::ATTACKER ) );

	clsCombatResult.m_byTotalVictim		= pRecvMsg->byListNum;
	memcpy( clsCombatResult.m_listVictim, pRecvMsg->DamList, sizeof( CCombatResult::VICTIM ) * pRecvMsg->byListNum );

	clsCombatResult.m_bIsCritical		= pRecvMsg->bCritical;
	clsCombatResult.m_bIsCounterAttack	= FALSE;
	clsCombatResult.m_byBulletDTIndex	= pRecvMsg->byBulletIndex;

	clsCombatResult.m_crcResult			= CRC_FAIL;
	for( int i = 0; i < pRecvMsg->byListNum; ++i )
	{
		if( pRecvMsg->DamList[i].wDamage > 0 )
			clsCombatResult.m_crcResult	= CRC_SUCCESS;
	}

	clsCombatResult.ProcessCombatResult();

	/*
	#define attack_skill_result_zocl 253
	struct _attack_skill_result_zocl {	
		_CHRID idAtter;	
		BYTE byEffectCode;	//	effect_code_skill, effect_code_class ..		add(4/18)
		WORD wSkillIndex;	//	upt(4/19)
		BYTE bySkillLv;	//add
		BYTE byAttackPart;
		BYTE byBulletIndex;
		bool bCritical;		//1이면 크리티컬공격

		BYTE byListNum;

		typedef _attack_gen_result_zocl::_dam_list _dam_list;

		_dam_list DamList[32];
		
		int size(){
			return sizeof(*this)-sizeof(_dam_list)*(32-byListNum);
		}
	};	*/
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 공격 포스 시전 요청
BOOL
CNetMsgProcessor_Combat::AttackForceRequest_clzo( CCharacter * pi_pVictim )
{
	_attack_force_request_clzo stSendMsg;

	if( pi_pVictim )
	{
		if( pi_pVictim->IsEqualCharType( CTI_AVATAR ) )
			stSendMsg.byID	= CTI_PLAYER;
		else
			stSendMsg.byID	= pi_pVictim->GetCharTypeID();
		stSendMsg.wIndex	= pi_pVictim->GetServerIndex();
	}
	else
	{
		stSendMsg.byID		= 0xFF;
		stSendMsg.wIndex	= 0xFFFF;
	}
	stSendMsg.zAreaPos[0]	= _GetAvatar()->GetSFTargetPos()[0];		// 포스를 사용한 위치 X
	stSendMsg.zAreaPos[1]	= _GetAvatar()->GetSFTargetPos()[2];		// 포스를 사용한 위치 Z
	
	stSendMsg.wForceSerial	= _GetAvatar()->GetSFItem()->GetIndex();	// 포스의 인덱스

	SetAvatarAttackState( CAT_FORCE, pi_pVictim );
	_GetGGI()->SetRequestProcessState( TRUE );

	return SendNetMessage( attack_force_request_clzo,
						   &stSendMsg,
						   sizeof( _attack_force_request_clzo ) );

	/*
	#define attack_force_request_clzo 9
	struct _attack_force_request_clzo {
		BYTE byID;
		WORD wIndex;		//지역기라면 0xFFFF	
		short zAreaPos[2];	//x, z
		WORD wForceSerial;
	};*/
}

// 공격 포스 시전 결과 ( 타 플레이어의 공격도 포함되어 있음 )
// 데미지가 1이상이면 성공, 0이면 실패
void
CNetMsgProcessor_Combat::AttackForceResult_zocl( char * pi_pMsg )
{
	_attack_force_result_zocl * pRecvMsg = reinterpret_cast< _attack_force_result_zocl * >( pi_pMsg );
	_GetGGI()->SetRequestProcessState( FALSE );

	CCombatResult clsCombatResult;

	clsCombatResult.m_byAttackType		= CAT_FORCE;
	clsCombatResult.m_dwSFIndex			= pRecvMsg->byForceIndex;
	clsCombatResult.m_bySFLevel			= pRecvMsg->byForceLv;
	clsCombatResult.m_vecSFPosition[0]	= pRecvMsg->zAreaPos[0];
	clsCombatResult.m_vecSFPosition[2]	= pRecvMsg->zAreaPos[1];

	memcpy( &clsCombatResult.m_stAttacker, &pRecvMsg->idAtter, sizeof( CCombatResult::ATTACKER ) );

	clsCombatResult.m_byTotalVictim		= pRecvMsg->byListNum;
	memcpy( clsCombatResult.m_listVictim, pRecvMsg->DamList, sizeof( CCombatResult::VICTIM ) * pRecvMsg->byListNum );

	clsCombatResult.m_bIsCritical		= pRecvMsg->bCritical;
	clsCombatResult.m_bIsCounterAttack	= FALSE;

	clsCombatResult.m_crcResult			= CRC_FAIL;
	for( int i = 0; i < pRecvMsg->byListNum; ++i )
	{
		if( pRecvMsg->DamList[i].wDamage > 0 )
			clsCombatResult.m_crcResult	= CRC_SUCCESS;
	}

	clsCombatResult.ProcessCombatResult();

	/*
	#define attack_force_result_zocl 252
	struct _attack_force_result_zocl {	//add(3/17)
		_CHRID idAtter;	
		BYTE byForceIndex;
		BYTE byForceLv;		
		short zAreaPos[2];	//지역기일경우 타겟점
		BYTE byAttackPart;
		bool bCritical;		//1이면 크리티컬공격

		BYTE byListNum;

		typedef _attack_gen_result_zocl::_dam_list _dam_list;

		_dam_list DamList[32];
		
		int size(){
			return sizeof(*this)-sizeof(_dam_list)*(32-byListNum);
		}
	};*/
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 기갑 장비 공격 요청
BOOL
CNetMsgProcessor_Combat::AttackUnitRequest_clzo( CCharacter * pi_pVictim )
{
	if( !pi_pVictim )
		return FALSE;

	_attack_unit_request_clzo stSendMsg;

	stSendMsg.byID			= pi_pVictim->GetCharTypeID();
	stSendMsg.wIndex		= pi_pVictim->GetServerIndex();

	BYTE byAttackPartType	= _GetAvatar()->GetUnitAttackPartType() - IEPT_UNIT_ARMS;
	if( byAttackPartType > UBI_SHOULDER_TYPE )
		return FALSE;

	stSendMsg.byWeaponPart	= byAttackPartType;

	SetAvatarAttackState( CAT_NORMAL, pi_pVictim );
	_GetGGI()->SetRequestProcessState( FALSE );

	return SendNetMessage( attack_unit_request_clzo,
						   &stSendMsg,
						   sizeof( _attack_unit_request_clzo ) );

	/*
	#define attack_unit_request_clzo 240
	struct _attack_unit_request_clzo {		//add(3/17)
		BYTE byID;
		WORD wIndex;		//지역기라면 0xFFFF	

		//#define unit_bullet_arms			0
		//#define unit_bullet_shoulder		1
		BYTE byWeaponPart;	
	};*/
}

// 기갑 장비 공격 결과 ( 타 플레이어의 공격도 포함되어 있음 )
// 데미지가 1이상이면 성공, 0이면 실패
void
CNetMsgProcessor_Combat::AttackUnitResult_zocl( char * pi_pMsg )
{
	_attack_unit_result_zocl * pRecvMsg = reinterpret_cast< _attack_unit_result_zocl * >( pi_pMsg );
	_GetGGI()->SetRequestProcessState( FALSE );

	CCombatResult clsCombatResult;

	clsCombatResult.m_byAttackType		= CAT_NORMAL;
	clsCombatResult.m_byAttackPart		= pRecvMsg->byAttackPart;

	memcpy( &clsCombatResult.m_stAttacker, &pRecvMsg->idAtter, sizeof( CCombatResult::ATTACKER ) );

	clsCombatResult.m_byTotalVictim		= pRecvMsg->byListNum;
	memcpy( clsCombatResult.m_listVictim, pRecvMsg->DamList, sizeof( CCombatResult::VICTIM ) * pRecvMsg->byListNum );

	clsCombatResult.m_bIsCritical		= pRecvMsg->bCritical;
	clsCombatResult.m_bIsCounterAttack	= FALSE;
	clsCombatResult.m_byBulletDTIndex	= pRecvMsg->byBulletIndex;

	clsCombatResult.m_crcResult			= CRC_FAIL;
	for( int i = 0; i < pRecvMsg->byListNum; ++i )
	{
		if( pRecvMsg->DamList[i].wDamage > 0 )
			clsCombatResult.m_crcResult	= CRC_SUCCESS;
	}

	clsCombatResult.ProcessCombatResult();

	/*
	#define attack_unit_result_zocl 251
	struct _attack_unit_result_zocl {	//add(3/17)
		_CHRID idAtter;	
		BYTE byWeaponPart;
		WORD wWeaponIndex;
		BYTE byBulletIndex;			
		BYTE byAttackPart;
		bool bCritical;		//1이면 크리티컬공격

		BYTE byListNum;

		typedef _attack_gen_result_zocl::_dam_list _dam_list;

		_dam_list DamList[32];
		
		int size(){
			return sizeof(*this)-sizeof(_dam_list)*(32-byListNum);
		}
	};*/
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 화살 등이 빗맞았을 경우...
BOOL
CNetMsgProcessor_Combat::AttackTestRequest_clzo( BYTE pi_byAttackType, CCharacter * pi_pVictim )
{
	if( !pi_pVictim )
		return FALSE;

	_attack_test_request_clzo stSendMsg;

	if( pi_byAttackType == CAT_NORMAL )
	{
		stSendMsg.byEffectCode	= 0xFF;
		stSendMsg.byEffectIndex	= 0xFF;
	}
	else
	{
		stSendMsg.byEffectCode	= pi_byAttackType;
		stSendMsg.byEffectIndex	= _GetAvatar()->GetSFItem()->GetDTIndex();
	}

	stSendMsg.wBulletSerial		= _GetEquipBoard()->GetUsingBulletIndex();

	stSendMsg.zTar[0]			= pi_pVictim->GetPosX();
	stSendMsg.zTar[1]			= pi_pVictim->GetPosZ();

	SetAvatarAttackState( pi_byAttackType, pi_pVictim );
	_GetGGI()->SetRequestProcessState( TRUE );

	return SendNetMessage( attack_test_request_clzo,
						   &stSendMsg,
						   sizeof( _attack_test_request_clzo ) );

	/*
	#define attack_test_request_clzo 105
	struct _attack_test_request_clzo{
		BYTE	byEffectCode;
		BYTE	byEffectIndex;
		WORD	wBulletSerial;		//bullet 무관 : 0xFFFF
		BYTE	byWeaponPart;		//유닛일경우.. 유닛탑승상태일경우반대시 세팅.. 위에꺼는 싸그리무시.
		short	zTar[2];

		_attack_test_request_clzo()	{
			byEffectCode = 0xFF;
			byEffectIndex = 0xFF;
			wBulletSerial = 0xFFFF;
			byWeaponPart = 0xFF;
		}
	};*/
}

void
CNetMsgProcessor_Combat::AttackTestResult_zocl( char * pi_pMsg )
{
	_attack_test_result_zocl * pRecvMsg = reinterpret_cast< _attack_test_result_zocl * >( pi_pMsg );
	_GetGGI()->SetRequestProcessState( FALSE );

	/*
	#define attack_test_result_zocl 104
	struct _attack_test_result_zocl{
		_CHRID	idAtter;	
		BYTE	byBulletIndex;	// => 0xFF 무관..	
	//	WORD	wLeftBullet;	//del(3/17)
		BYTE	byEffectCode;	// 효과코드.. 0xFF면 일반공격
		BYTE	byEffectIndex;	//일반이면 무시..
		BYTE	byEffectLv;
		BYTE	byWeaponPart;	//unit의 경우..	//add(3/17)
		short	zTar[2];
	};*/
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CNetMsgProcessor_Combat::AttackCountResult_zocl( char * pi_pMsg )
{
	_attack_count_result_zocl * pRecvMsg = reinterpret_cast< _attack_count_result_zocl * >( pi_pMsg );

	CCombatResult clsCombatResult;

	clsCombatResult.m_byAttackType			= CAT_NORMAL;
	clsCombatResult.m_byAttackPart			= pRecvMsg->byAttackPart;

	memcpy( &clsCombatResult.m_stAttacker, &pRecvMsg->idAtter, sizeof( CCombatResult::ATTACKER ) );

	clsCombatResult.m_byTotalVictim			= pRecvMsg->byListNum;
	memcpy( clsCombatResult.m_listVictim, pRecvMsg->DamList, sizeof( CCombatResult::VICTIM ) * pRecvMsg->byListNum );

	clsCombatResult.m_bIsCritical			= pRecvMsg->bCritical;
	clsCombatResult.m_bIsCounterAttack		= TRUE;
	clsCombatResult.m_byBulletDTIndex		= _GetEquipBoard()->GetUsingBulletIndex();

	clsCombatResult.m_crcResult				= CRC_FAIL;
	for( int i = 0; i < pRecvMsg->byListNum; ++i )
	{
		if( pRecvMsg->DamList[i].wDamage > 0 )
			clsCombatResult.m_crcResult		= CRC_SUCCESS;
	}

	clsCombatResult.ProcessCombatResult();

	/*
	// 반격기
	#define attack_count_result_zocl 250
	struct _attack_count_result_zocl {	//add(3/17)
		_CHRID idAtter;	
		BYTE byAttackPart;
		bool bCritical;		//1이면 크리티컬공격

		BYTE byListNum;

		typedef _attack_gen_result_zocl::_dam_list _dam_list;

		_dam_list DamList[32];
		
		int size(){
			return sizeof(*this)-sizeof(_dam_list)*(32-byListNum);
		}
	};*/
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 공격 에러
void
CNetMsgProcessor_Combat::AttackErrorResult_zocl( char * pi_pMsg )
{
	_attack_error_result_zocl * pRecvMsg = reinterpret_cast< _attack_error_result_zocl * >( pi_pMsg );

	_GetGGI()->SetRequestProcessState( FALSE );

#ifdef _DEVELOPER_

	switch( pRecvMsg->sFailureCode )
	{
		case error_attack_hit	: _SetSystemMsg( "공격 실패" ); break;
		default					: _SetSystemMsg( "공격 관련 에러 : 알 수 없는 에러 ( %d )", pRecvMsg->sFailureCode ); break;
	}

#endif

	/*
	#define attack_error_result_zocl 255
	struct _attack_error_result_zocl {	//add(3/17)
		char sFailureCode;
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 몬스터 공격
void
CNetMsgProcessor_Combat::AttackMonsterInform_zocl( char * pi_pMsg )
{
	_attack_monster_inform_zocl * pRecvMsg = reinterpret_cast< _attack_monster_inform_zocl * >( pi_pMsg );

	CCombatResult clsCombatResult;

	clsCombatResult.m_byAttackType	= CAT_NORMAL;
	clsCombatResult.m_byAttackPart	= pRecvMsg->byAttackPart;

	memcpy( &clsCombatResult.m_stAttacker, &pRecvMsg->idAtter, sizeof( CCombatResult::ATTACKER ) );

	clsCombatResult.m_byTotalVictim	= 1;
	memcpy( &clsCombatResult.m_listVictim[0], &pRecvMsg->idDst, sizeof( CCombatResult::VICTIM ) );

	clsCombatResult.m_bIsCritical	= pRecvMsg->bCritical;

	if( pRecvMsg->wDamage > 0 )
		clsCombatResult.m_crcResult	= CRC_SUCCESS;
	else
		clsCombatResult.m_crcResult	= CRC_FAIL;

	clsCombatResult.ProcessCombatResult();

	/*
	#define attack_monster_inform_zocl 249
	struct _attack_monster_inform_zocl{		//add(3/17)
		_CHRID	idAtter;		
		BYTE	byAttackPart;	
		bool	bCritical;			//1이면 크리티컬공격

		_CHRID	idDst;		
		WORD	wDamage;
		WORD	wLeftHP;
	};*/
}

// 애니머스 공격
void
CNetMsgProcessor_Combat::AttackAnimusInform_zocl( char * pi_pMsg )
{
	_attack_animus_inform_zocl * pRecvMsg = reinterpret_cast< _attack_animus_inform_zocl * >( pi_pMsg );

	CCombatResult clsCombatResult;

	clsCombatResult.m_byAttackType	= CAT_NORMAL;
	clsCombatResult.m_byAttackPart	= pRecvMsg->byAttackPart;

	memcpy( &clsCombatResult.m_stAttacker, &pRecvMsg->idAtter, sizeof( CCombatResult::ATTACKER ) );

	clsCombatResult.m_byTotalVictim	= 1;
	memcpy( &clsCombatResult.m_listVictim[0], &pRecvMsg->idDst, sizeof( CCombatResult::VICTIM ) );

	clsCombatResult.m_bIsCritical	= pRecvMsg->bCritical;

	if( pRecvMsg->wDamage > 0 )
		clsCombatResult.m_crcResult	= CRC_SUCCESS;
	else
		clsCombatResult.m_crcResult	= CRC_FAIL;

	clsCombatResult.ProcessCombatResult();

	/*
	#define attack_animus_inform_zocl 248
	struct _attack_animus_inform_zocl{		//add(3/29)		
		_CHRID	idAtter;		
		BYTE	byAttackPart;	
		bool	bCritical;			//1이면 크리티컬공격

		_CHRID	idDst;		
		WORD	wDamage;
		WORD	wLeftHP;
	};*/
}

// 가드 타워 공격
void
CNetMsgProcessor_Combat::AttackTowerInform_zocl( char * pi_pMsg )
{
	_attack_tower_inform_zocl * pRecvMsg = reinterpret_cast< _attack_tower_inform_zocl * >( pi_pMsg );

	CCombatResult clsCombatResult;

	clsCombatResult.m_byAttackType	= CAT_NORMAL;
	clsCombatResult.m_byAttackPart	= pRecvMsg->byAttackPart;

	memcpy( &clsCombatResult.m_stAttacker, &pRecvMsg->idAtter, sizeof( CCombatResult::ATTACKER ) );

	clsCombatResult.m_byTotalVictim	= 1;
	memcpy( &clsCombatResult.m_listVictim[0], &pRecvMsg->idDst, sizeof( CCombatResult::VICTIM ) );

	clsCombatResult.m_bIsCritical	= pRecvMsg->bCritical;

	if( pRecvMsg->wDamage > 0 )
		clsCombatResult.m_crcResult	= CRC_SUCCESS;
	else
		clsCombatResult.m_crcResult	= CRC_FAIL;

	clsCombatResult.ProcessCombatResult();

	/*
	#define attack_tower_inform_zocl 136
	struct _attack_tower_inform_zocl{		//add(3/17)
		_CHRID	idAtter;		
		BYTE	byAttackPart;	
		bool	bCritical;			//1이면 크리티컬공격

		_CHRID	idDst;		
		WORD	wDamage;
		WORD	wLeftHP;	
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 전투 모드 변경 요청
BOOL
CNetMsgProcessor_Combat::ModeChangeRequest_clzo( BYTE pi_byCombatMode )
{
	if( _GetGBM()->IsRequestProcess() )
		return FALSE;

	_mode_change_request_clzo stSendMsg;

	stSendMsg.byModeCode = pi_byCombatMode;

	if( SendNetMessage( mode_change_request_clzo,
						&stSendMsg,
						sizeof( _mode_change_request_clzo ) ) )
	{
		_GetAvatar()->SetCombatMode( pi_byCombatMode );

		// 자동 공격이 설정되어 있을 경우라면, 공격을 시작한다.
		if( _GetAvatar()->IsRequestChangeToCombatMode() )
		{
			_GetAvatar()->SetRequestChangeToCombatMode( FALSE );
			if( pi_byCombatMode == CSI_COMBAT )
				_GetAvatar()->Attack( CAT_NORMAL, _GetTargetMgr()->GetTarget() );
		}

		_GetGBM()->SetRequestProcessState( TRUE );
	}

	return FALSE;

	/*
	#define mode_change_request_clzo 5		//add(8/30)
	struct _mode_change_request_clzo{
		BYTE byModeCode;	//	평화모드:1, 전투모드:2 
	};*/
}

// 전투 모드 변경 요청 결과
void
CNetMsgProcessor_Combat::ModeChangeResult_zocl( char * pi_pMsg )
{
	_mode_change_result_zocl * pRecvMsg = reinterpret_cast< _mode_change_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	/*
	#define mode_change_result_zocl 15
	struct _mode_change_result_zocl{	//upt(8/12)
	//	WORD wIndex;
	//	DWORD dwSerial;
	//	WORD dwEquipVer;

	//	DWORD dwStateFlag;
		BYTE byModeCode;	//	평화모드:0, 전투모드:1 
	};*/
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 전투 중 장착 아이템이 파괴되었음
void
CNetMsgProcessor_Combat::BreakdownEquipItem_zocl( char * pi_pMsg )
{
	_breakdown_equip_item_zocl * pRecvMsg = reinterpret_cast< _breakdown_equip_item_zocl * >( pi_pMsg );

	CPlayer * pPlayer = _GetCharMgr()->GetPlayer( pRecvMsg->dwPlayerSerial );
	if( !pPlayer )
		return;

	// Avatar라면 UI 처리
	if( pPlayer->IsEqualCharType( CTI_AVATAR ) )
	{
		CBoardItem * pBoardItem = _GetEquipBoard()->GetItem( CEPT_CLIENT_TO_UI[ IEPT_SERVER_TO_CLIENT[pRecvMsg->byPartIndex] ] );
		if( pBoardItem )
			pBoardItem->SetDurability( 0 );
	}
	// Avatar가 아니면 외형 요청
	else
	{
		pPlayer->PutOffEquipment( IEPT_SERVER_TO_CLIENT[pRecvMsg->byPartIndex] );

		if( pPlayer->GetServerIndex() != pRecvMsg->wPlayerIndex )
		{
			pPlayer->SetServerIndex( pRecvMsg->wPlayerIndex );
			_GetNetworkMgr()->InitMsg_OtherShapeRequest_clzo( pPlayer, TRUE );
		}
		else if( pPlayer->GetEquipmentVersion() != pRecvMsg->dwEquipVer )
		{
			_GetNetworkMgr()->InitMsg_OtherShapeRequest_clzo( pPlayer, FALSE );
		}
	}

	/*
	#define breakdown_equip_item_zocl 16
	struct _breakdown_equip_item_zocl{
		WORD wPlayerIndex;	
		DWORD dwPlayerSerial;//add(12/23)
		WORD dwEquipVer;//add(12/23)	

		BYTE byPartIndex;
		WORD wItemSerial;	//update(12/23)
	};*/
}

// 전투중 장착 아이템의 내구성이 떨어졌음 ( 아바타만 해당 )
void
CNetMsgProcessor_Combat::ShieldEquipDurDec_zocl( char * pi_pMsg )
{
	_shield_equip_dur_dec_zocl * pRecvMsg = reinterpret_cast< _shield_equip_dur_dec_zocl * >( pi_pMsg );

	CBoardItem * pBoardItem = _GetEquipBoard()->GetItem( CEPT_CLIENT_TO_UI[ IEPT_SERVER_TO_CLIENT[pRecvMsg->byPartIndex] ] );
	if( pBoardItem )
		pBoardItem->SetDurability( pRecvMsg->wLeftDurPoint );

	/*
	#define shield_equip_dur_dec_zocl 8
	struct _shield_equip_dur_dec_zocl{	//add(9/26)
		BYTE byPartIndex;
		WORD wSerial;
		WORD wLeftDurPoint;//add(9/26)
	};*/
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 탄환의 수량
void
CNetMsgProcessor_Combat::AlterWeaponBulletInform_zocl( char * pi_pMsg )
{
	_alter_weapon_bullet_inform_zocl * pRecvMsg = reinterpret_cast< _alter_weapon_bullet_inform_zocl * >( pi_pMsg );

	CBoardItem * pBoardItem = _GetEquipBoard()->GetItemByIndex( pRecvMsg->wItemSerial );
	if( pBoardItem )
		pBoardItem->SetDurability( pRecvMsg->wLeftNum );

	/*
	#define alter_weapon_bullet_inform_zocl 247	
	struct _alter_weapon_bullet_inform_zocl {	//add(4/19)
		WORD	wItemSerial;
		WORD	wLeftNum;
	};*/
}

// 유닛 탄환의 수량
void
CNetMsgProcessor_Combat::AlterUnitBulletInform_zocl( char * pi_pMsg )
{
	_alter_unit_bullet_inform_zocl * pRecvMsg = reinterpret_cast< _alter_unit_bullet_inform_zocl * >( pi_pMsg );

	_GetAvatar()->SetUnitBulletNum( IEPT_UNIT_ARMS + pRecvMsg->byPart, pRecvMsg->wLeftNum );

	/*
	#define alter_unit_bullet_inform_zocl 246	
	struct _alter_unit_bullet_inform_zocl {	//add(4/19)
		BYTE	byPart;
		WORD	wLeftNum;
	};*/
}

// 유닛의 내구도 ( 유닛을 탑승했을 때만 옴 )
void
CNetMsgProcessor_Combat::AlterUnitHpInform_zocl( char * pi_pMsg )
{
	_alter_unit_hp_inform_zocl * pRecvMsg = reinterpret_cast< _alter_unit_hp_inform_zocl * >( pi_pMsg );

	if( !_GetAvatar()->IsEqualUnitMountState( UMS_DISMOUNT ) )
		_GetAvatar()->SetHP( pRecvMsg->dwLeftHP );

	/*
	#define alter_unit_hp_inform_zocl 245
	struct _alter_unit_hp_inform_zocl {		//add(5/3)
		BYTE	bySlotIndex;
		DWORD	dwLeftHP;
	};*/
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 주인공을 쫓아오던 몬스터가 다른 캐릭터를 쫓아갈 때...
void
CNetMsgProcessor_Combat::MonsterChaseChange_zocl( char * pi_pMsg )
{
	_monster_chase_change_zocl * pRecvMsg = reinterpret_cast< _monster_chase_change_zocl * >( pi_pMsg );

	/*
	#define monster_chase_change_zocl 13	//all
	struct _monster_chase_change_zocl{
		WORD wMonIndex;
		short zDstIndex; //-1은 NULL
	};
	*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CNetMsgProcessor_Combat::ShowServerErrorMessage( DWORD pi_dwErrorCode )
{
	switch( pi_dwErrorCode )
	{
		case 0 :
		default	: _SetSystemMsg( "전투 관련 에러 : 알 수 없는 에러 ( %d )", pi_dwErrorCode ); break;
	}
}

#endif // _NEW_UI_