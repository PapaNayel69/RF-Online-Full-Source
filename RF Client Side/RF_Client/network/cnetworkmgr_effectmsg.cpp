////////////////////////////////////////////////////////////////////////////////
//
// CNetworkMgr Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../GameObject/Manager/CCharacterMgr.h"
#include "../Character/Avatar/CAvatar_Quest.h"

#include "../GameObject/Board/General/CPartyBoard.h"
#include "../GameObject/Board/Common/CBoardManager.h"
#include "../GameObject/Manager/CTargetMgr.h"

extern BOOL  g_bWriteTestLog;		// 테스트 코드
extern DWORD g_bWriteTestLogCount;	// 테스트 코드

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 보조 포스 사용 요청
BOOL
CNetworkMgr::EffectMsg_ForceRequest_clzo( CCharacter * pi_pChar )
{
	if( IsSendRequestMsg() )
		return FALSE;

	_force_request_clzo l_sSend;

	l_sSend.wForceSerial	= _GetAvatar()->GetSFItem()->GetIndex();		// 포스의 인덱스
	
	if( pi_pChar )
	{
		if( pi_pChar->IsEqualCharType( CTI_AVATAR ) )
			l_sSend.idDst.byID	= CTI_PLAYER;								// 캐릭터의 타입 아이디
		else
			l_sSend.idDst.byID	= pi_pChar->GetCharTypeID();				// 캐릭터의 타입 아이디
		l_sSend.idDst.dwSerial	= pi_pChar->GetIndex();						// 캐릭터의 인덱스
		l_sSend.idDst.wIndex	= pi_pChar->GetServerIndex();				// 캐릭터의 서버 인덱스
	}
	else
	{
		l_sSend.idDst.byID		= CTI_PLAYER;
		l_sSend.idDst.dwSerial	= _GetAvatar()->GetIndex();
		l_sSend.idDst.wIndex	= _GetAvatar()->GetServerIndex();
	}
					
	// 메세지를 보냄
	BYTE l_nType[] = { effect_msg, force_request_clzo };
	BOOL l_bResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 ( char * )&l_sSend,
									 sizeof( _force_request_clzo ) );

	// 결과 처리
	if( l_bResult )
	{
		// 공격자 세팅
		if( pi_pChar )
		{
			if( !pi_pChar->IsEqualCharType( CTI_AVATAR ) )
				_GetAvatar()->SetRotY( _GetAvatar()->GetPosition(), pi_pChar->GetPosition() );

			CVictimInfo * l_pVictimInfo = _GetAvatar()->AddVictimInfo( pi_pChar, TRUE );
			if( l_pVictimInfo )
				l_pVictimInfo->SetAttacked( FALSE );

			// 피해자 세팅
			CAttackerInfo * l_pAttackerInfo = pi_pChar->AddAttackerInfo( _GetAvatar(), TRUE );
			if( l_pAttackerInfo )
			{
				l_pAttackerInfo->SetDamageType_Dist( ADT_FAR );
				l_pAttackerInfo->SetDamageType_AT( CAT_FORCE );
				l_pAttackerInfo->SetTotalDamagePoint( 0 );
				l_pAttackerInfo->SetSFItem( _GetAvatar()->GetSFItem() );
				l_pAttackerInfo->SetEffectFrameNum( _GetAvatar()->GetMaxEffectFrameNum() );
			}
		}

		// 이 부분은 피해자 세팅후에 와야 한다.
		_GetAvatar()->SetAction( CAI_FORCE_INVOKE );
		_GetAvatar()->SetAttackMode( CAT_FORCE );
		_GetAvatar()->GetCombatState()->SetReceiveAttackResultMsg( FALSE );
		_GetAvatar()->GetCombatState()->SetAttackError( FALSE );

		SendRequestMsg();
		_GetGGI()->SetRequestProcessState( TRUE );
	}
	else
	{
		NetLogWrite( "CNetworkMgr::EffectMsg_ForceRequest_clzo(%d) Failure : %d", NST_GAME_SERVER, l_bResult );
	}

	/*
	#define force_request_clzo 1
	struct _force_request_clzo {
	//	BYTE byPerformSerial;	//시전시리얼..//del(4/14)
		WORD wForceSerial;
		_CHRID idDst;//시전대상..	
	};*/

	return l_bResult;
}

// 보조 포스 사용 요청에 대한 응답 ( 아바타 )
void
CNetworkMgr::EffectMsg_ForceResultOne_zocl( char * pi_pMsg )
{
	_force_result_one_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _force_result_one_zocl ) );

	ReceiveResponseMsg();
	_GetGGI()->SetRequestProcessState( FALSE );

	// 공격자 세팅
	_GetAvatar()->GetCombatState()->SetReceiveAttackResultMsg( TRUE );

	CCharacter * l_pChar = _GetAvatar()->GetCombatState()->GetCurEnemy( ET_VICTIM );
	if( !l_pChar )
		return;
	if( ( l_pChar->GetActionByCategory() == CAI_DEATH ) || ( l_pChar->GetAction() == CAI_INTERNALSTATE_DEATH ) )
		return;

	CVictimInfo * l_pVictimInfo = _GetAvatar()->GetVictimInfo( l_pChar );
	if( l_pVictimInfo )
		l_pVictimInfo->SetAttacked( TRUE );

	// 성공
	if( l_sRecv.byErrCode == 0 )
	{
		// 피해자 세팅
		CAttackerInfo * l_pAttackerInfo = l_pChar->GetAttackerInfo( _GetAvatar() );
		if( l_pAttackerInfo )
		{
			l_pAttackerInfo->SetDamageType_Dist( ADT_FAR );
			l_pAttackerInfo->SetDamageType_AT( CAT_FORCE );
			l_pAttackerInfo->SetTotalDamagePoint( 0 );
			l_pAttackerInfo->SetSFItem( _GetAvatar()->GetSFItem() );
			l_pAttackerInfo->SetEffectFrameNum( _GetAvatar()->GetMaxEffectFrameNum() );
		}

		_GetAvatar()->SetSFUsingMsg( TRUE );
		_GetAvatar()->SetReceiveNetMsg( "Force Invoke - Succ" );	// 테스트코드

		//_WriteLog( "6) assist force => %s : %d", _GetAvatar()->GetSFItem()->GetName(), _GetAvatar()->GetSFItem()->GetDTIndex() );
	}
	// 실패
	else
	{
		switch( l_sRecv.byErrCode )
		{
			case 1 : _SetSystemMsg( "보조 포스 시전 실패 : 존재하지 않는 포스입니다." ); break;
			case 2 : _SetSystemMsg( "보조 포스 시전 실패 : FP가 부족합니다." ); break;
			case 3 : _SetSystemMsg( "보조 포스 시전 실패 : 포스 사용 대상이 없습니다." ); break;
			case 4 : _SetSystemMsg( "보조 포스 시전 실패 : 보조형 포스가 아닙니다." ); break;
			case error_sf_existing_lv_better : _SetSystemMsg( "보조 포스 시전 실패 : 이전에 적용된 스킬/포스보다 레벨이 낮습니다." ); break;
			case error_sf_existing_dur_10pro : _SetSystemMsg( "보조 포스 시전 실패 : 지속형 스킬/포스는 남은 시간이 10%% 이하로 남아있어야만 재시전이 가능합니다." ); break;
			default : _SetSystemMsg( "보조 포스 시전 실패 : 알 수 없는 에러 ( %d )", l_sRecv.byErrCode ); break;
		}

		_GetAvatar()->SetSFUsingMsg( FALSE );
		_GetAvatar()->SetReceiveNetMsg( "Force Invoke - Fail" );	// 테스트코드
	}

	//--------------------------------------------------------------------------

	if( l_sRecv.byErrCode == 0 )
	{
		if( _GetAvatar()->GetSFItem() )
			_GetAvatar()->GetTutorialInfo()->SetCombatInfo( CAT_FORCE, _GetAvatar()->GetSFItem()->GetDTIndex(),
															l_pChar->GetCharTypeID(),
															l_pChar->GetDTIndex() );
	}
	
	/*
	//포스를 자신에게 알림
	#define force_result_one_zocl 2
	struct _force_result_one_zocl {
		BYTE byErrCode;//0:성공, //1:없는포스아이템, //2:FP부족 //3:없는대상 4;//일시보조,지속아님 5;//사용할수없는포스 6;//지속피해보조포스사용제한 7;//지속이득보조포스사용제한 8;//시전실패 100;//플레이어 상황상 사용못하는 포스
	//	BYTE byPerformSerial;	//시전시리얼..//del(4/14)
		WORD wLeftFP;
		DWORD dwItemCum; //포스아이템성공누적치..
	};*/
}

// 보조 포스 사용 요청에 대한 응답 ( 다른 플레이어 )
void
CNetworkMgr::EffectMsg_ForceResultOther_zocl( char * pi_pMsg )
{
	_force_result_other_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _force_result_other_zocl ) );

	if( l_sRecv.byRetCode || l_sRecv.byForceIndex == 0xFF )
		return;

	CCharacter * l_pChar1 = NULL;		// 시전자
	CCharacter * l_pChar2 = NULL;		// 피시전자

	l_pChar1 = _GetCharMgr()->GetCharacter( l_sRecv.idPerformer.byID, l_sRecv.idPerformer.dwSerial );
	if( !l_pChar1 )
		return;
	l_pChar1->SetServerIndex( l_sRecv.idPerformer.wIndex );
	if( ( l_pChar1->GetActionByCategory() == CAI_DEATH ) || ( l_pChar1->GetAction() == CAI_INTERNALSTATE_DEATH ) )
		return;

	l_pChar2 = _GetCharMgr()->GetCharacter( l_sRecv.idDster.byID, l_sRecv.idDster.dwSerial );
	if( !l_pChar2 )
		return;
	l_pChar2->SetServerIndex( l_sRecv.idDster.wIndex );
	if( ( l_pChar2->GetActionByCategory() == CAI_DEATH ) || ( l_pChar2->GetAction() == CAI_INTERNALSTATE_DEATH ) )
		return;

	//--------------------------------------------------------------------------
	// 시전자 설정
	l_pChar1->GetCombatState()->SetReceiveAttackResultMsg( TRUE );

	CSFItem clSFItem;
	clSFItem.Create( CAT_FORCE, l_sRecv.byForceIndex );
	clSFItem.SetIndex( l_sRecv.byForceIndex );
	clSFItem.SetLevel( l_sRecv.byForceLv );
	l_pChar1->SetSFItem( &clSFItem );

	l_pChar1->SetRotY( l_pChar1->GetPosition(), l_pChar2->GetPosition() );

	CVictimInfo * l_pVictimInfo = l_pChar1->AddVictimInfo( l_pChar2, TRUE );
	if( l_pVictimInfo )
		l_pVictimInfo->SetAttacked( TRUE );
		
	l_pChar1->SetReceiveNetMsg( "Support Force Invoke - Succ" );	// 테스트 코드

	//--------------------------------------------------------------------------
	// 피시전자 설정
	CAttackerInfo * l_pAttackerInfo = l_pChar2->AddAttackerInfo( l_pChar1, TRUE );
	if( l_pAttackerInfo )
	{
		l_pAttackerInfo->SetDamageType_Dist( ADT_FAR );
		l_pAttackerInfo->SetDamageType_AT( CAT_FORCE );
		l_pAttackerInfo->SetTotalDamagePoint( 0 );
		l_pAttackerInfo->SetSFItem( l_pChar1->GetSFItem() );
		l_pAttackerInfo->SetEffectFrameNum( l_pChar1->GetMaxEffectFrameNum() );
	}

	l_pChar2->SetReceiveNetMsg( "Support Force Applied - Succ" );

	l_pChar1->SetAction( CAI_FORCE_INVOKE );

	/*
	//포스의 결과를 주변에 알림
	#define force_result_other_zocl 3
	struct _force_result_other_zocl {		
		BYTE byRetCode;	//0이면 성공..
		BYTE byForceIndex;
		BYTE byForceLv;
		_CHRID idPerformer;	//시전자
		_CHRID idDster;		//시전대상..
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 보조 스킬 사용 요청
BOOL
CNetworkMgr::EffectMsg_SkillRequest_clzo( CCharacter * pi_pChar )
{
	if( IsSendRequestMsg() )
		return FALSE;

	_skill_request_clzo l_sSend;

	l_sSend.bySkillIndex	= _GetAvatar()->GetSFItem()->GetIndex();		// 스킬의 DT 인덱스
	
	if( pi_pChar )
	{
		if( pi_pChar->IsEqualCharType( CTI_AVATAR ) )
			l_sSend.idDst.byID	= CTI_PLAYER;								// 캐릭터의 타입 아이디
		else
			l_sSend.idDst.byID	= pi_pChar->GetCharTypeID();				// 캐릭터의 타입 아이디
		l_sSend.idDst.dwSerial	= pi_pChar->GetIndex();						// 캐릭터의 인덱스
		l_sSend.idDst.wIndex	= pi_pChar->GetServerIndex();				// 캐릭터의 서버 인덱스
	}
	else
	{
		l_sSend.idDst.byID		= CTI_PLAYER;
		l_sSend.idDst.dwSerial	= _GetAvatar()->GetIndex();
		l_sSend.idDst.wIndex	= _GetAvatar()->GetServerIndex();
	}
					
	// 메세지를 보냄
	BYTE l_nType[] = { effect_msg, skill_request_clzo };
	BOOL l_bResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 ( char * )&l_sSend,
									 sizeof( _skill_request_clzo ) );

	// 결과 처리
	if( l_bResult )
	{
		// 공격자 세팅
		if( pi_pChar )
		{
			if( !pi_pChar->IsEqualCharType( CTI_AVATAR ) )
				_GetAvatar()->SetRotY( _GetAvatar()->GetPosition(), pi_pChar->GetPosition() );

			CVictimInfo * l_pVictimInfo = _GetAvatar()->AddVictimInfo( pi_pChar, TRUE );
			if( l_pVictimInfo )
				l_pVictimInfo->SetAttacked( FALSE );

			// 피해자 세팅
			CAttackerInfo * l_pAttackerInfo = pi_pChar->AddAttackerInfo( _GetAvatar(), TRUE );
			if( l_pAttackerInfo )
			{
				l_pAttackerInfo->SetDamageType_Dist( _GetAvatar()->GetAttackDistanceType( CAT_SKILL ) );
				l_pAttackerInfo->SetDamageType_AT( CAT_SKILL );
				l_pAttackerInfo->SetTotalDamagePoint( 0 );
				l_pAttackerInfo->SetSFItem( _GetAvatar()->GetSFItem() );
			}
		}

		// 이 부분은 피해자 세팅후에 와야 한다.
		_GetAvatar()->SetAction( CAI_SKILL_INVOKE );
		_GetAvatar()->SetAttackMode( CAT_SKILL );
		_GetAvatar()->GetCombatState()->SetReceiveAttackResultMsg( FALSE );
		_GetAvatar()->GetCombatState()->SetAttackError( FALSE );

		SendRequestMsg();
		_GetGGI()->SetRequestProcessState( TRUE );
	}
	else
	{
		NetLogWrite( "CNetworkMgr::EffectMsg_ForceRequest_clzo(%d) Failure : %d", NST_GAME_SERVER, l_bResult );
	}

	/*
	//스킬요청..일시보조, 지속..
	#define skill_request_clzo 4	
	struct _skill_request_clzo {
	//	BYTE byPerformSerial;	//시전시리얼.. //del(4/14)
		BYTE bySkillIndex;
		_CHRID idDst;//시전대상..	
	};*/

	return l_bResult;
}

// 보조 포스 스킬 요청에 대한 응답 ( 아바타 )
void
CNetworkMgr::EffectMsg_SkillResultOne_zocl( char * pi_pMsg )
{
	_skill_result_one_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _skill_result_one_zocl ) );

	ReceiveResponseMsg();
	_GetGGI()->SetRequestProcessState( FALSE );

	// 공격자 세팅
	_GetAvatar()->GetCombatState()->SetReceiveAttackResultMsg( TRUE );

	CCharacter * l_pChar = _GetAvatar()->GetCombatState()->GetCurEnemy( ET_VICTIM );
	if( !l_pChar )
		return;
	if( ( l_pChar->GetActionByCategory() == CAI_DEATH ) || ( l_pChar->GetAction() == CAI_INTERNALSTATE_DEATH ) )
		return;

	CVictimInfo * l_pVictimInfo = _GetAvatar()->GetVictimInfo( l_pChar );
	if( l_pVictimInfo )
		l_pVictimInfo->SetAttacked( TRUE );

	//--------------------------------------------------------------------------

	if( l_sRecv.byErrCode == 0 )
	{
		// 피해자 세팅
		CAttackerInfo * l_pAttackerInfo = l_pChar->GetAttackerInfo( _GetAvatar() );
		if( l_pAttackerInfo )
		{
			l_pAttackerInfo->SetDamageType_Dist( _GetAvatar()->GetAttackDistanceType( CAT_SKILL ) );
			l_pAttackerInfo->SetDamageType_AT( CAT_SKILL );
			l_pAttackerInfo->SetTotalDamagePoint( 0 );
			l_pAttackerInfo->SetSFItem( _GetAvatar()->GetSFItem() );
			l_pAttackerInfo->SetEffectFrameNum( _GetAvatar()->GetMaxEffectFrameNum() );
		}

		_GetAvatar()->SetSFUsingMsg( TRUE );
		_GetAvatar()->SetReceiveNetMsg( "Skill Invoke - Succ" );	// 테스트코드

#ifdef _DEVELOPER_
		//_WriteLog( "7) assist skill => %s : %d", _GetAvatar()->GetSFItem()->GetName(), _GetAvatar()->GetSFItem()->GetDTIndex() );
#endif
	}
	else
	{
		switch( l_sRecv.byErrCode )
		{
			case error_sf_not_actable_race		: _SetSystemMsg( "보조 스킬 시전 실패 : 사용 가능 종족이 아닙니다." ); break;
			case error_sf_shortage_fp			: _SetSystemMsg( "보조 스킬 시전 실패 : FP가 부족합니다." ); break;
			case error_sf_no_exist_dst			: _SetSystemMsg( "보조 스킬 시전 실패 : 스킬 사용 대상이 없습니다." ); break;
			case error_sf_existing_lv_better	: _SetSystemMsg( "보조 스킬 시전 실패 : 이전에 적용된 스킬/포스보다 레벨이 낮습니다." ); break;
			case error_sf_existing_dur_10pro	: _SetSystemMsg( "보조 스킬 시전 실패 : 지속형 스킬/포스는 남은 시간이 10%% 이하로 남아있어야만 재시전이 가능합니다." ); break;
			default	: _SetSystemMsg( "보조 스킬 시전 실패 : 알 수 없는 에러 ( %d )", l_sRecv.byErrCode ); break;
		}

		_GetAvatar()->SetSFUsingMsg( FALSE );
		_GetAvatar()->SetReceiveNetMsg( "Skill Invoke - Fail" );	// 테스트코드
	}

	//--------------------------------------------------------------------------

	if( l_sRecv.byErrCode == 0 )
	{
		if( _GetAvatar()->GetSFItem() )
			_GetAvatar()->GetTutorialInfo()->SetCombatInfo( CAT_SKILL, _GetAvatar()->GetSFItem()->GetDTIndex(),
															l_pChar->GetCharTypeID(),
															l_pChar->GetDTIndex() );
	}
	
	/*
	//스킬를 자신에게 알림
	#define skill_result_one_zocl 5
	struct _skill_result_one_zocl {
		BYTE byErrCode;//0:성공, //1:할수없는스킬 //2:FP부족 //3:없는대상 4;//일시보조,지속아님 5;//사용할수없는스킬 6;//지속이득보조스킬사용제한 7;//시전실패, 100;//플레이어 상황상 사용못하는 스킬
	//	BYTE byPerformSerial;	//시전시리얼..//del(4/14)
		WORD wLeftFP;
	};*/
}

// 보조 스킬 사용 요청에 대한 응답 ( 다른 플레이어 )
void
CNetworkMgr::EffectMsg_SkillResultOther_zocl( char * pi_pMsg )
{
	_skill_result_other_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _skill_result_other_zocl ) );

	if( l_sRecv.byRetCode || l_sRecv.bySkillIndex == 0xFF )
		return;

	CCharacter * l_pChar1 = NULL;		// 시전자
	CCharacter * l_pChar2 = NULL;		// 피시전자

	l_pChar1 = _GetCharMgr()->GetCharacter( l_sRecv.idPerformer.byID, l_sRecv.idPerformer.dwSerial );
	if( !l_pChar1 )
		return;
	l_pChar1->SetServerIndex( l_sRecv.idPerformer.wIndex );
	if( ( l_pChar1->GetActionByCategory() == CAI_DEATH ) || ( l_pChar1->GetAction() == CAI_INTERNALSTATE_DEATH ) )
		return;

	l_pChar2 = _GetCharMgr()->GetCharacter( l_sRecv.idDster.byID, l_sRecv.idDster.dwSerial );
	if( !l_pChar2 )
		return;
	l_pChar2->SetServerIndex( l_sRecv.idDster.wIndex );
	if( ( l_pChar2->GetActionByCategory() == CAI_DEATH ) || ( l_pChar2->GetAction() == CAI_INTERNALSTATE_DEATH ) )
		return;

	//--------------------------------------------------------------------------
	// 시전자 설정
	l_pChar1->GetCombatState()->SetReceiveAttackResultMsg( TRUE );

	CSFItem clSFItem;
	clSFItem.Create( CAT_SKILL, l_sRecv.bySkillIndex );
	clSFItem.SetIndex( l_sRecv.bySkillIndex );
	clSFItem.SetLevel( l_sRecv.bySkillLv );
	l_pChar1->SetSFItem( &clSFItem );

	l_pChar1->SetRotY( l_pChar1->GetPosition(), l_pChar2->GetPosition() );

	CVictimInfo * l_pVictimInfo = l_pChar1->AddVictimInfo( l_pChar2, TRUE );
	if( l_pVictimInfo )
		l_pVictimInfo->SetAttacked( TRUE );
		
	l_pChar1->SetReceiveNetMsg( "Support Skill Invoke - Succ" );	// 테스트 코드

	//--------------------------------------------------------------------------
	// 피시전자 설정
	CAttackerInfo * l_pAttackerInfo = l_pChar2->AddAttackerInfo( l_pChar1, TRUE );
	if( l_pAttackerInfo )
	{
		l_pAttackerInfo->SetDamageType_Dist( l_pChar1->GetAttackDistanceType( CAT_SKILL ) );
		l_pAttackerInfo->SetDamageType_AT( CAT_SKILL );
		l_pAttackerInfo->SetTotalDamagePoint( 0 );
		l_pAttackerInfo->SetSFItem( l_pChar1->GetSFItem() );
		l_pAttackerInfo->SetEffectFrameNum( l_pChar1->GetMaxEffectFrameNum() );
	}

	l_pChar2->SetReceiveNetMsg( "Support Skill Applied - Succ" );

	l_pChar1->SetAction( CAI_SKILL_INVOKE );

	/*
	//스킬의 결과를 주변에 알림
	#define skill_result_other_zocl 6
	struct _skill_result_other_zocl {		
		BYTE byRetCode;	//0이면 성공..
		_CHRID idPerformer;	//시전자
		_CHRID idDster;		//시전대상..
		BYTE bySkillIndex;
		BYTE bySkillLv;
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 클래스 스킬 사용 요청
BOOL
CNetworkMgr::EffectMsg_ClassSkillRequest_clzo( CCharacter * pi_pChar )
{
	if( IsSendRequestMsg() )
		return FALSE;

	_class_skill_request_clzo l_sSend;

	l_sSend.wSkillIndex		= _GetAvatar()->GetSFItem()->GetDTIndex();	// 스킬의 DT 인덱스
	
	if( pi_pChar )
	{
		if( pi_pChar->IsEqualCharType( CTI_AVATAR ) )
			l_sSend.idDst.byID	= CTI_PLAYER;								// 캐릭터의 타입 아이디
		else
			l_sSend.idDst.byID	= pi_pChar->GetCharTypeID();				// 캐릭터의 타입 아이디
		l_sSend.idDst.dwSerial	= pi_pChar->GetIndex();						// 캐릭터의 인덱스
		l_sSend.idDst.wIndex	= pi_pChar->GetServerIndex();				// 캐릭터의 서버 인덱스
	}
	else
	{
		l_sSend.idDst.byID		= CTI_PLAYER;
		l_sSend.idDst.dwSerial	= _GetAvatar()->GetIndex();
		l_sSend.idDst.wIndex	= _GetAvatar()->GetServerIndex();
	}

	// 메세지를 보냄
	BYTE l_nType[] = { effect_msg, class_skill_request_clzo };
	BOOL l_bResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 ( char * )&l_sSend,
									 sizeof( _class_skill_request_clzo ) );

	// 결과 처리
	if( l_bResult )
	{
		// 공격자 세팅
		if( pi_pChar )
		{
			if( !pi_pChar->IsEqualCharType( CTI_AVATAR ) )
				_GetAvatar()->SetRotY( _GetAvatar()->GetPosition(), pi_pChar->GetPosition() );

			CVictimInfo * l_pVictimInfo = _GetAvatar()->AddVictimInfo( pi_pChar, TRUE );
			if( l_pVictimInfo )
				l_pVictimInfo->SetAttacked( FALSE );

			// 피해자 세팅
			CAttackerInfo * l_pAttackerInfo = pi_pChar->AddAttackerInfo( _GetAvatar(), TRUE );
			if( l_pAttackerInfo )
			{
				l_pAttackerInfo->SetDamageType_Dist( _GetAvatar()->GetAttackDistanceType( CAT_SKILL ) );
				l_pAttackerInfo->SetDamageType_AT( CAT_SKILL );
				l_pAttackerInfo->SetTotalDamagePoint( 0 );
				l_pAttackerInfo->SetSFItem( _GetAvatar()->GetSFItem() );
			}
		}

		// 이 부분은 피해자 세팅후에 와야 한다.
		_GetAvatar()->SetAction( CAI_SKILL_INVOKE );
		_GetAvatar()->SetAttackMode( CAT_SKILL );
		_GetAvatar()->GetCombatState()->SetReceiveAttackResultMsg( FALSE );
		_GetAvatar()->GetCombatState()->SetAttackError( FALSE );

		SendRequestMsg();
		_GetGGI()->SetRequestProcessState( TRUE );
	}
	else
	{
		NetLogWrite( "CNetworkMgr::EffectMsg_ClassSkillRequest_clzo(%d) Failure : %d", NST_GAME_SERVER, l_bResult );
	}

	/*
	//클래스스킬요청..일시보조, 지속..
	#define class_skill_request_clzo 44	
	struct _class_skill_request_clzo {	
		WORD wSkillIndex;
		_CHRID idDst;//시전대상..	
	};*/

	return l_bResult;
}

// 클래스 스킬 요청에 대한 응답 ( 아바타 )
void
CNetworkMgr::EffectMsg_ClassSkillResultOne_zocl( char * pi_pMsg )
{
	_class_skill_result_one_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _class_skill_result_one_zocl ) );

	ReceiveResponseMsg();
	_GetGGI()->SetRequestProcessState( FALSE );

	// 공격자 세팅
	_GetAvatar()->GetCombatState()->SetReceiveAttackResultMsg( TRUE );

	CCharacter * l_pChar = _GetAvatar()->GetCombatState()->GetCurEnemy( ET_VICTIM );
	if( !l_pChar )
		return;
	if( ( l_pChar->GetActionByCategory() == CAI_DEATH ) || ( l_pChar->GetAction() == CAI_INTERNALSTATE_DEATH ) )
		return;

	CVictimInfo * l_pVictimInfo = _GetAvatar()->GetVictimInfo( l_pChar );
	if( l_pVictimInfo )
		l_pVictimInfo->SetAttacked( TRUE );

	//--------------------------------------------------------------------------

	if( l_sRecv.byErrCode == 0 )
	{
		// 피해자 세팅
		CAttackerInfo * l_pAttackerInfo = l_pChar->GetAttackerInfo( _GetAvatar() );
		if( l_pAttackerInfo )
		{
			l_pAttackerInfo->SetDamageType_Dist( _GetAvatar()->GetAttackDistanceType( CAT_SKILL ) );
			l_pAttackerInfo->SetDamageType_AT( CAT_SKILL );
			l_pAttackerInfo->SetTotalDamagePoint( 0 );
			l_pAttackerInfo->SetSFItem( _GetAvatar()->GetSFItem() );
			l_pAttackerInfo->SetEffectFrameNum( _GetAvatar()->GetMaxEffectFrameNum() );
		}

		_GetAvatar()->SetSFUsingMsg( TRUE );
		_GetAvatar()->SetReceiveNetMsg( "Class Skill Invoke - Succ" );	// 테스트코드

		//_WriteLog( "7) assist skill => %s : %d", _GetAvatar()->GetSFItem()->GetName(), _GetAvatar()->GetSFItem()->GetDTIndex() );
	}
	else
	{
		switch( l_sRecv.byErrCode )
		{
			case error_sf_not_actable_race		: _SetSystemMsg( "보조 스킬 시전 실패 : 사용 가능 종족이 아닙니다." ); break;
			case error_sf_shortage_fp			: _SetSystemMsg( "보조 스킬 시전 실패 : FP가 부족합니다." ); break;
			case error_sf_no_exist_dst			: _SetSystemMsg( "보조 스킬 시전 실패 : 스킬 사용 대상이 없습니다." ); break;
			case error_sf_existing_lv_better	: _SetSystemMsg( "보조 스킬 시전 실패 : 이전에 적용된 스킬/포스보다 레벨이 낮습니다." ); break;
			case error_sf_existing_dur_10pro	: _SetSystemMsg( "보조 스킬 시전 실패 : 지속형 스킬/포스는 남은 시간이 10%% 이하로 남아있어야만 재시전이 가능합니다." ); break;
			default	: _SetSystemMsg( "보조 스킬 시전 실패 : 알 수 없는 에러 ( %d )", l_sRecv.byErrCode ); break;
		}

		_GetAvatar()->SetSFUsingMsg( FALSE );
		_GetAvatar()->SetReceiveNetMsg( "Class Skill Invoke - Fail" );	// 테스트코드
	}

	//--------------------------------------------------------------------------

	if( l_sRecv.byErrCode == 0 )
	{
		if( _GetAvatar()->GetSFItem() )
			_GetAvatar()->GetTutorialInfo()->SetCombatInfo( CAT_SKILL, _GetAvatar()->GetSFItem()->GetDTIndex(),
															l_pChar->GetCharTypeID(),
															l_pChar->GetDTIndex() );
	}
	
	/*
	//스킬를 자신에게 알림
	#define class_skill_result_one_zocl 45
	struct _class_skill_result_one_zocl {
		BYTE byErrCode;//0:성공, //1:할수없는스킬 //2:FP부족 //3:없는대상 4;//일시보조,지속아님 5;//사용할수없는스킬 6;//지속이득보조스킬사용제한 7;//시전실패, 100;//플레이어 상황상 사용못하는 스킬
		WORD wLeftFP;
	};*/
}

// 클래스 스킬 사용 요청에 대한 응답 ( 다른 플레이어 )
void
CNetworkMgr::EffectMsg_ClassSkillResultOther_zocl( char * pi_pMsg )
{
	_class_skill_result_other_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _class_skill_result_other_zocl ) );

	if( l_sRecv.byRetCode || l_sRecv.wSkillIndex == 0xFF )
		return;

	CCharacter * l_pChar1 = NULL;		// 시전자
	CCharacter * l_pChar2 = NULL;		// 피시전자

	l_pChar1 = _GetCharMgr()->GetCharacter( l_sRecv.idPerformer.byID, l_sRecv.idPerformer.dwSerial );
	if( !l_pChar1 )
		return;
	l_pChar1->SetServerIndex( l_sRecv.idPerformer.wIndex );
	if( ( l_pChar1->GetActionByCategory() == CAI_DEATH ) || ( l_pChar1->GetAction() == CAI_INTERNALSTATE_DEATH ) )
		return;

	l_pChar2 = _GetCharMgr()->GetCharacter( l_sRecv.idDster.byID, l_sRecv.idDster.dwSerial );
	if( !l_pChar2 )
		return;
	l_pChar2->SetServerIndex( l_sRecv.idDster.wIndex );
	if( ( l_pChar2->GetActionByCategory() == CAI_DEATH ) || ( l_pChar2->GetAction() == CAI_INTERNALSTATE_DEATH ) )
		return;

	//--------------------------------------------------------------------------
	// 시전자 설정
	l_pChar1->GetCombatState()->SetReceiveAttackResultMsg( TRUE );

	CSFItem clSFItem;
	clSFItem.Create( CAT_SKILL, l_sRecv.wSkillIndex );
	clSFItem.SetIndex( l_sRecv.wSkillIndex );
	clSFItem.SetLevel( 1 );
	l_pChar1->SetSFItem( &clSFItem );

	l_pChar1->SetRotY( l_pChar1->GetPosition(), l_pChar2->GetPosition() );

	CVictimInfo * l_pVictimInfo = l_pChar1->AddVictimInfo( l_pChar2, TRUE );
	if( l_pVictimInfo )
		l_pVictimInfo->SetAttacked( TRUE );

	l_pChar1->SetReceiveNetMsg( "Class Support Skill Invoke - Succ" );	// 테스트 코드

	//--------------------------------------------------------------------------
	// 피시전자 설정
	CAttackerInfo * l_pAttackerInfo = l_pChar2->AddAttackerInfo( l_pChar1, TRUE );
	if( l_pAttackerInfo )
	{
		l_pAttackerInfo->SetDamageType_Dist( l_pChar1->GetAttackDistanceType( CAT_SKILL ) );
		l_pAttackerInfo->SetDamageType_AT( CAT_SKILL );
		l_pAttackerInfo->SetTotalDamagePoint( 0 );
		l_pAttackerInfo->SetSFItem( l_pChar1->GetSFItem() );
		l_pAttackerInfo->SetEffectFrameNum( l_pChar1->GetMaxEffectFrameNum() );
	}

	l_pChar2->SetReceiveNetMsg( "Class Support Skill Applied - Succ" );

	l_pChar1->SetAction( CAI_SKILL_INVOKE );

	/*
	//스킬의 결과를 주변에 알림
	#define class_skill_result_other_zocl 46
	struct _class_skill_result_other_zocl {
		BYTE byRetCode;	//0이면 성공..
		_CHRID idPerformer;	//시전자
		_CHRID idDster;		//시전대상..
		WORD wSkillIndex;
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 보조 지속형 스킬/포스의 추가
void
CNetworkMgr::EffectMsg_EffectAddInform_zocl( char * pi_pMsg )
{
	_effect_add_inform_zocl * pRecvMsg = reinterpret_cast< _effect_add_inform_zocl * >( pi_pMsg );

	_GetAvatar()->SetServerSFEffectCode( pRecvMsg->wEffectCode, pRecvMsg->byLv, TRUE );
	_GetAvatar()->UpdateAppliedSFTime( pRecvMsg->wEffectCode, pRecvMsg->wDurSec );

	/*
	#define effect_add_inform_zocl 7
	struct _effect_add_inform_zocl {
		BYTE byLv;	
		WORD wEffectCode;	//맨왼쪽비트... effect_code.. 그다음부턴 SFIndex..
		WORD wDurSec;		//add(5/5)
	};*/
}

// 보조 지속형 스킬/포스의 제거
void
CNetworkMgr::EffectMsg_EffectRemoveInform_zocl( char * pi_pMsg )
{
	_effect_remove_inform_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _effect_remove_inform_zocl ) );

	_GetAvatar()->SetServerSFEffectCode( l_sRecv.wEffectCode, 1, FALSE );
	
	/*
	#define effect_remove_inform_zocl 8
	struct _effect_remove_inform_zocl {//add(3/24)
		BYTE wEffectCode; //upt(4/12)
	};*/
}

// 아바타의 FP 세팅
void
CNetworkMgr::EffectMsg_SetFpInform_zocl( char * pi_pMsg )
{
	_set_fp_inform_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _set_fp_inform_zocl ) );

	_GetAvatar()->SetFP( l_sRecv.wFP );

	/*
	#define set_fp_inform_zocl 9
	struct _set_fp_inform_zocl {	// add(3/27)
		WORD wFP;
	};*/
}

// 아바타의 HP 세팅
void
CNetworkMgr::EffectMsg_SetHpInform_zocl( char * pi_pMsg )
{
	_set_hp_inform_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _set_hp_inform_zocl ) );

	_GetAvatar()->SetHP( l_sRecv.wHP );

	/*
	#define set_hp_inform_zocl 10
	struct _set_hp_inform_zocl {	// add(3/27)
		WORD wHP;
	};*/
}

// 아바타의 SP 세팅
void
CNetworkMgr::EffectMsg_SetSpInform_zocl( char * pi_pMsg )
{
	_set_sp_inform_zocl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _set_sp_inform_zocl ) );

	_GetAvatar()->SetSP( l_sRecv.wSP );

	/*
	#define set_sp_inform_zocl 11
	struct _set_sp_inform_zocl {	// add(3/27)
		WORD wSP;
	};*/
}

// 아이템 제작 스킬로 만들어진 아이템 생성등에 쓰임
void
CNetworkMgr::EffectMsg_InsertItemInform_zocl( char * pi_pMsg )
{
	_insert_item_inform_zocl * pRecvMag = reinterpret_cast< _insert_item_inform_zocl * >( pi_pMsg );

	// 일단은 인벤토리쪽만 처리
	if( pRecvMag->byStorageCode == IBT_INVENTORY )
	{
		// 새로운 아이템 설정
		CBoardItem clsNewItem;
		if( !clsNewItem.Create( IEPT_SERVER_TO_CLIENT[ pRecvMag->byTableCode ], pRecvMag->wItemIndex ) )
			return;
		clsNewItem.SetIndex( pRecvMag->wSerial );
		clsNewItem.SetServerIndex( pRecvMag->wSerial );

		if( clsNewItem.IsSplitable() )
			clsNewItem.SetQuantity( pRecvMag->dwDurPoint );
		else
			clsNewItem.SetDurability( pRecvMag->dwDurPoint );
		

		_GetItemWorkBoard()->SetNewItem( &clsNewItem );		
		//memcpy( &_GetAvatar()->GetItemWorkInfo()->m_clsNewItem, &clsNewItem, sizeof( CBoardItem ) );

		//_WriteLog( "9) working => %s, iept : %d", clsNewItem.GetName(), clsNewItem.GetItemKind() );
	}

	/*
	#define insert_item_inform_zocl 12
	struct _insert_item_inform_zocl {	// add(3/27)
		BYTE byStorageCode;	//인벤토리: 0, 장비: 1 장식: 2, 벨트:3, 포스:4 링크:5
		BYTE byTableCode;	
		WORD wItemIndex;
		DWORD dwDurPoint;	//중첩아이템은 수량.. 내구아이템은 상위워드(한계내구성) 하위워드(현재내구성)
		WORD wSerial;
	};*/
}

// 캐릭터가 스턴상태가 됐음 => 1초후에 자동으로 풀림
void
CNetworkMgr::EffectMsg_StunInform_zocl( char * pi_pMsg )
{
	_stun_inform_zocl l_Recv;
	memcpy( &l_Recv, pi_pMsg, sizeof( _stun_inform_zocl ) );

	CCharacter * l_pChar = _GetCharMgr()->GetCharacter( l_Recv.idStun.byID, l_Recv.idStun.dwSerial );
	if( l_pChar )
		l_pChar->EnableStun();

	/*
	#define stun_inform_zocl 13
	struct _stun_inform_zocl {			// add(3/27)
		_CHRID idStun;	
	};*/
}

// 플레이어에 마지막으로 걸린 이펙트
void
CNetworkMgr::EffectMsg_LastEffectChangeInform_zocl( char * pi_pMsg )
{
	_last_effect_change_inform_zocl * pRecvMsg = reinterpret_cast< _last_effect_change_inform_zocl * >( pi_pMsg );

	CCharacter * pChar = _GetCharMgr()->GetCharacter( pRecvMsg->byObjID, pRecvMsg->dwSerial );
	if( !pChar )
		return;

	// 파티 멤버일 경우에는 처리하지 않는다. ( PartyMsg_PartyMemberEffect_upd()에서 처리 )
	if( pChar->IsEqualCharType( CTI_PLAYER ) )
	{
		if( _GetPartyBoard()->IsPartyMember( pChar->GetIndex() ) )
			return;
	}

	// 효과는 클리어 시킨 후 적용
	pChar->GetAppliedSF()->Clear();

	if( pRecvMsg->wLastContEffect != 0xFFFF )
	{
		pChar->SetServerSFEffectCode( pRecvMsg->wLastContEffect, 1, TRUE );
	}

	/*
	#define last_effect_change_inform_zocl 14
	struct _last_effect_change_inform_zocl {	//add(8/12)
		BYTE	byObjID;	//obj_id_player.. obj_id_monter..	//add(8/20)
		DWORD	dwSerial;
		WORD	wLastContEffect;	//upt(4/12)	
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 가드 타워 설치
BOOL
CNetworkMgr::EffectMsg_MakeTowerRequest_clzo( BUILD_GUARD_TOWER	& pi_stGuardTowerInfo )
{
	_make_tower_request_clzo stSendMsg;
	ZeroMemory( &stSendMsg, sizeof( _make_tower_request_clzo ) );

	// set guard tower item
	stSendMsg.wSkillIndex		= pi_stGuardTowerInfo.m_dwSkillDTIndex;
	stSendMsg.zPos[0]			= _GetAvatar()->GetPosX();
	stSendMsg.zPos[1]			= _GetAvatar()->GetPosY();
	stSendMsg.zPos[2]			= _GetAvatar()->GetPosZ();
	stSendMsg.wTowerItemSerial	= pi_stGuardTowerInfo.m_dwGuardTowerIndex;

	// set material
	stSendMsg.byMaterialNum		= 0;
	for( int i = 0; i < MAX_ITEM_MATERIAL_NUM; ++i )
	{
		if( pi_stGuardTowerInfo.m_dwMaterialIndex[i] == ID_INVALID )
			continue;

		stSendMsg.Material[stSendMsg.byMaterialNum].wItemSerial			= pi_stGuardTowerInfo.m_dwMaterialIndex[i];
		stSendMsg.Material[stSendMsg.byMaterialNum].byMaterSlotIndex	= i;
		stSendMsg.Material[stSendMsg.byMaterialNum].byAmount			= pi_stGuardTowerInfo.m_dwMaterialNum[i];

		++stSendMsg.byMaterialNum;
	}

	// send
	BYTE nType[] = { effect_msg, make_tower_request_clzo };
	BOOL bResult = SendNetMessage( NST_GAME_SERVER,
								   nType,
								   ( char * )&stSendMsg,
								   sizeof( _make_tower_request_clzo ) );

	if( bResult )
		_GetGBM()->SetRequestProcessState( TRUE );
	else
		NetLogWrite( "CNetworkMgr::EffectMsg_MakeTowerRequest_clzo(%d) Failure : %d", NST_GAME_SERVER, bResult );

	/*
	#define make_tower_request_clzo 15
	struct _make_tower_request_clzo {	//add(4/2)
		enum	{	max_material = 30	};	

		WORD	wSkillIndex;	//upt(4/14)
		short	zPos[3];
		WORD	wTowerItemSerial;
		
		struct __material{
			WORD wItemSerial;
			BYTE byMaterSlotIndex;	//가드타워데이터 필드중 재료정의 필드의 순서 0 ~ 2
			BYTE byAmount;
		};

		BYTE	byMaterialNum;
		__material Material[max_material];

		_make_tower_request_clzo()	{	byMaterialNum = 0;	}
		int size()	{ if(byMaterialNum > max_material) byMaterialNum = 0;	return sizeof(*this) - sizeof(__material) * (max_material - byMaterialNum);	}
	};*/

	return bResult;
}

// 가드 타워 설치 결과 
void
CNetworkMgr::EffectMsg_MakeTowerResult_zocl( char * pi_pMsg )
{
	_make_tower_result_zocl * pRecvMsg = reinterpret_cast< _make_tower_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );
	_GetAvatar()->SetFP( pRecvMsg->wLeftFP );

	if( pRecvMsg->byErrCode == 0 )
	{
		// 인벤토리의 가드 타워 아이템에 가드 타워 캐릭터의 인덱스를 연결한다.
		CBoardItem * pBoardItem = static_cast< CBoardItem * >( _GetGBM()->GetOrgObject() );
		if( pBoardItem )
			pBoardItem->SetExtraData( pRecvMsg->dwTowerObjSerial );

		_GetInventoryBoard()->UI_UseResult( TRUE );

		_SetSystemMsg( "가드 타워를 설치 하였습니다." );
	}
	else
	{
		_GetInventoryBoard()->UI_UseResult( FALSE );

		switch( pRecvMsg->byErrCode )
		{
			case error_tower_full_tower				: _SetSystemMsg( "더 이상 가드 타워를 설치할 수 없습니다." ); break;
			case error_tower_no_have_item			: _SetSystemMsg( "인벤토리에 가드 타워 아이템이 없습니다." ); break;
			case error_tower_no_tower_item			: _SetSystemMsg( "가드 타워 아이템이 아닙니다." ); break;
			case error_tower_overlap_material		:
			case error_tower_no_have_material		:
			case error_tower_no_match_material		: _SetSystemMsg( "가드 타워 설치에 필요한 재료의 조합이 틀립니다." ); break;
			case error_tower_lack_material			:
			case error_tower_no_match_amount		: _SetSystemMsg( "가드 타워 설치에 필요한 재료의 수량이 틀립니다." ); break;
			case error_tower_no_act_skill			: _SetSystemMsg( "가드 타워 설치 스킬을 사용할 수 없습니다." ); break;
			case error_tower_data_fail				:
			case error_tower_loge_distance			:
			default									: _SetSystemMsg( "가드 타워 관련 에러 : 알 수 없는 에러 (%d)", pRecvMsg->byErrCode ); break;
		}
	}

	/*
	#define make_tower_result_zocl 16
	struct _make_tower_result_zocl {
		BYTE	byErrCode;
		DWORD	dwTowerObjSerial;
		WORD	wLeftFP;	//add(4/25)
	};*/
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 가드 타워 회수
BOOL
CNetworkMgr::EffectMsg_BackTowerRequest_clzo( void )
{
	CCharacter * pTarget = _GetTargetMgr()->GetTarget();
	if( !pTarget )
		return FALSE;

	_back_tower_request_clzo stSendMsg;

	stSendMsg.dwTowerObjSerial	= pTarget->GetIndex();

	// send
	BYTE nType[] = { effect_msg, back_tower_request_clzo };
	BOOL bResult = SendNetMessage( NST_GAME_SERVER,
								   nType,
								   ( char * )&stSendMsg,
								   sizeof( _back_tower_request_clzo ) );

	if( bResult )
		_GetGBM()->SetRequestProcessState( TRUE );
	else
		NetLogWrite( "CNetworkMgr::EffectMsg_BackTowerRequest_clzo(%d) Failure : %d", NST_GAME_SERVER, bResult );

	/*
	#define back_tower_request_clzo 17	//타워해체
	struct _back_tower_request_clzo {	
		DWORD	dwTowerObjSerial;	//upt(4/9)
	}; 
	*/

	return bResult;
}

// 가드 타워 회수 결과 
void
CNetworkMgr::EffectMsg_BackTowerResult_zocl( char * pi_pMsg )
{
	_back_tower_result_zocl * pRecvMsg = reinterpret_cast< _back_tower_result_zocl * >( pi_pMsg );

	_GetGBM()->SetRequestProcessState( FALSE );

	if( pRecvMsg->byErrCode == 0 )
	{
		_SetSystemMsg( "가드 타워를 회수 하였습니다." );
	}
	else
	{
		switch( pRecvMsg->byErrCode )
		{
			case error_tower_not_operating_tower	: _SetSystemMsg( "가드 타워 관련 에러 : 현재 작동중인 가드 타워가 아닙니다." );
			default									: _SetSystemMsg( "가드 타워 관련 에러 : 알 수 없는 에러 (%d)", pRecvMsg->byErrCode ); break;
		}
	}

	/*
	#define back_tower_result_zocl 18	
	struct _back_tower_result_zocl {	//add(4/4)
		BYTE	byErrCode;
		WORD	wItemSerial;
		WORD	wLeftHP;
	};*/
}

// 가드 타워의 내구도
void
CNetworkMgr::EffectMsg_AlterTowerHp_zocl( char * pi_pMsg )
{
	_alter_tower_hp_zocl * pRecvMsg = reinterpret_cast< _alter_tower_hp_zocl * >( pi_pMsg );

	CBoardItem * pBoardItem = _GetInventoryBoard()->GetItemByIndex( pRecvMsg->wItemSerial );
	if( !pBoardItem )
		return;

	pBoardItem->SetDurability( pRecvMsg->wLeftHP );

	CGuardTower * pGuardTower = _GetCharMgr()->GetGuardTower( pBoardItem->GetExtraData() );
	if( !pGuardTower )
		return;

	pGuardTower->SetHP( pRecvMsg->wLeftHP );

	/*
	#define alter_tower_hp_zocl 19
	struct _alter_tower_hp_zocl {	//add(4/4)
		WORD	wItemSerial;
		WORD	wLeftHP;
	};*/
}

// 가드 타워의 완성
void
CNetworkMgr::EffectMsg_TowerCompleteInform_zocl( char * pi_pMsg )
{
	_tower_complete_inform_zocl * pRecvMsg = reinterpret_cast< _tower_complete_inform_zocl * >( pi_pMsg );

	CGuardTower * pGuardTower = _GetCharMgr()->GetGuardTower( pRecvMsg->dwTowerObjSerial );
	if( !pGuardTower )
		return;

	pGuardTower->SetAction( CAI_CREATE_CREATE );

	/*
	#define tower_complete_inform_zocl 20	//가드타워완성..
	struct _tower_complete_inform_zocl {		//add(4/24)
		DWORD	dwTowerObjSerial;
	};*/
}

// 아바타의 걸려있는 지속 효과의 남은 시간이 바뀜.
void
CNetworkMgr::EffectMsg_AlterContEffectTime_zocl( char * pi_pMsg )
{
	_alter_cont_effect_time_zocl * pRecvMsg = reinterpret_cast< _alter_cont_effect_time_zocl * >( pi_pMsg );

	for( int i = 0; i < pRecvMsg->byEffectNum; ++i )
	{
		if( pRecvMsg->List[i].zLeftSec < 0 )
			pRecvMsg->List[i].zLeftSec = 0;

		_GetAvatar()->UpdateAppliedSFTime( pRecvMsg->List[i].wEffectCode, pRecvMsg->List[i].zLeftSec );
	}

	/*
	#define alter_cont_effect_time_zocl 21	//걸려있는 지속효과의 남은 시간이 바뀜
	struct _alter_cont_effect_time_zocl {
		struct __list
		{
			WORD	wEffectCode;
			short	zLeftSec;
		};

		BYTE	byEffectNum;
		__list	List[CONT_SF_SIZE];

		_alter_cont_effect_time_zocl()	{	byEffectNum = 0;	}
		int size()	{	if(byEffectNum > CONT_SF_SIZE)	byEffectNum = 0; return sizeof(*this)-sizeof(__list)*(CONT_SF_SIZE-byEffectNum);	}
	};*/
}
