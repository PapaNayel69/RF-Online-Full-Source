////////////////////////////////////////////////////////////////////////////////
//
// CNetMsgProcessor_Party Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../../../GameObject/Manager/CCharacterMgr.h"
#include "../../../GameObject/Board/General/CGameEventBoard.h"
#include "../../../GameObject/Board/General/CPartyBoard.h"

#include "CNetMsgProcessor_Party.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CNetMsgProcessor_Party::CNetMsgProcessor_Party()
{
}

CNetMsgProcessor_Party::~CNetMsgProcessor_Party()
{
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CNetMsgProcessor_Party::SendNetMessage( BYTE pi_byMsgIndex, void * pi_pMsg, int pi_nSize )
{
	return CNetMsgProcessor_Base::SendNetMessage( party_msg, pi_byMsgIndex, pi_pMsg, pi_nSize );
}

bool
CNetMsgProcessor_Party::AnalysisServerPacket( _MSG_HEADER * pMsgHeader, char* pi_pMsg )
{
	if( pMsgHeader->m_byType[0] != party_msg )
		return false;

	switch( pMsgHeader->m_byType[1] )
	{
		case party_join_invitation_question_zocl :
			PartyJoinInvitationQuestion_zocl( pi_pMsg );
			break;

		case party_join_application_question_zocl :
			PartyJoinApplicationQuestion_zocl( pi_pMsg );
			break;

		case party_join_joiner_result_zocl :
			PartyJoinJoinerResult_zocl( pi_pMsg );
			break;

		case party_join_member_result_zocl :
			PartyJoinMemberResult_zocl( pi_pMsg );
			break;

		case party_leave_self_result_zocl :
			PartyLeaveSelfResult_zocl( pi_pMsg );
			break;

		case party_leave_compulsion_result_zocl :
			PartyLeaveCompulsionResult_zocl( pi_pMsg );
			break;

		case party_disjoint_result_zocl :
			PartyDisjointResult_zocl( pi_pMsg );
			break;

		case party_succession_result_zocl :
			PartySuccessionResult_zocl( pi_pMsg );
			break;

		case party_lock_result_zocl :
			PartyLockResult_zocl( pi_pMsg );
			break;

		case party_exp_division_zocl :
			PartyExpDivision_zocl( pi_pMsg );
			break;

		// udp - begin
		case party_member_info_upd :
			PartyMemberInfo_upd( pi_pMsg );
			break;

		case party_member_hp_upd :
			PartyMemberHp_upd( pi_pMsg );
			break;

		case party_member_fp_upd :
			PartyMemberFp_upd( pi_pMsg );
			break;

		case party_member_sp_upd :
			PartyMemberSp_upd( pi_pMsg );
			break;

		case party_member_lv_upd :
			PartyMemberLv_upd( pi_pMsg );
			break;

		case party_member_pos_upd :
			PartyMemberPos_upd( pi_pMsg );
			break;

		case party_member_max_hfsp_upd :
			PartyMemberMaxHfsp_upd( pi_pMsg );
			break;

		case party_member_effect_upd :
			PartyMemberEffect_upd( pi_pMsg );
			break;
		// udp - end

		default :
			m_pNetworkMgr->NetLogWrite( "Unknown Message : CNetMsgProcessor_Party::AnalysisServerPacket()\n" );
			return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 다른 플레이어를 자신의 파티원으로 초대함
BOOL
CNetMsgProcessor_Party::PartyJoinInvitation_clzo( CPlayer * pi_pPlayer )
{
	if( !pi_pPlayer )
		return FALSE;

	_party_join_invitation_clzo l_stSend;

	l_stSend.wDstIndex = pi_pPlayer->GetServerIndex();

	return SendNetMessage( party_join_invitation_clzo,
						   &l_stSend,
						   sizeof( _party_join_invitation_clzo ) );

	/*
	//특정대상을 파티원으로 모집요청
	//조건사항.. 자신은 무소속또는 보스..상대는 무소속인상태에서만 요청가능
	#define party_join_invitation_clzo 1
	struct _party_join_invitation_clzo {
		WORD wDstIndex;
	};*/
}

// 다른 플레이어가 아바타를 파티에 초대 했음
BOOL
CNetMsgProcessor_Party::PartyJoinInvitationQuestion_zocl( char * pi_pMsg )
{
	_party_join_invitation_question_zocl * l_pRecvData =
		reinterpret_cast< _party_join_invitation_question_zocl * >( pi_pMsg );

	CPlayer * l_pPlayer = _GetCharMgr()->GetPlayer( l_pRecvData->idBoss.dwSerial );
	if( !l_pPlayer )
		return FALSE;

	CPartyEvent l_clsPartyEvent;
	l_clsPartyEvent.SetType( GET_PARTY );
	l_clsPartyEvent.SetPartyEventType( CPartyEvent::INVITE_AVATAR_TO_OTHER_PARTY );
	l_clsPartyEvent.SetApplicant( l_pPlayer );
	_GetGameEventBoard()->PushEvent( l_clsPartyEvent );

	return TRUE;
	
	/*
	//특정인이 자신에게 파티에 모집요청함
	#define party_join_invitation_question_zocl 2
	struct _party_join_invitation_question_zocl {
		_CLID idBoss;
	};*/
}

// 다른 플레이어가 아바타를 파티에 초대에 했음 => 에 대한 응답을 서버로 보낸다.
BOOL
CNetMsgProcessor_Party::PartyJoinInvitationAnswer_clzo( CPlayer * pi_pPlayer )
{
	if( !pi_pPlayer )
		return FALSE;

	_party_join_invitation_answer_clzo l_stSend;

	l_stSend.idBoss.dwSerial	= pi_pPlayer->GetIndex();
	l_stSend.idBoss.wIndex		= pi_pPlayer->GetServerIndex();

	return SendNetMessage( party_join_invitation_answer_clzo,
						   &l_stSend,
						   sizeof( _party_join_invitation_answer_clzo ) );

	/*
	// 승락할 경우에만 서버에 보냄. 승락하지 않을 경우에는 보낼 필요 없음
	//특정인이 자신에게 파티에 모집요청의 답변
	#define party_join_invitation_answer_clzo 3
	struct _party_join_invitation_answer_clzo {
		_CLID idBoss;
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 아바타가 다른 파티장에게 파티원으로 지원함
BOOL
CNetMsgProcessor_Party::PartyJoinApplication_clzo( CPlayer * pi_pPlayer )
{
	if( !pi_pPlayer )
		return FALSE;

	_party_join_application_clzo l_stSend;

	l_stSend.wBossIndex = pi_pPlayer->GetServerIndex();

	return SendNetMessage( party_join_application_clzo,
						   &l_stSend,
						   sizeof( _party_join_application_clzo ) );

	/*
	//특정파티장에게 자신을 파티원으로 지원요청
	//조건사항.. 자신은 무소속.. 상대는 보스인상태에서만 요청가능
	#define party_join_application_clzo 4
	struct _party_join_application_clzo {
		WORD wBossIndex;
	};*/
}

// 다른 플레이어가 아바타에게 파티원으로 지원함
BOOL
CNetMsgProcessor_Party::PartyJoinApplicationQuestion_zocl( char * pi_pMsg )
{
	_party_join_application_question_zocl * l_pRecvData =
		reinterpret_cast< _party_join_application_question_zocl * >( pi_pMsg );

	if( _GetPartyBoard()->IsLockOnAddMember() )
		return FALSE;

	CPlayer * l_pPlayer = _GetCharMgr()->GetPlayer( l_pRecvData->idApplicant.dwSerial );
	if( !l_pPlayer )
		return FALSE;

	CPartyEvent l_clsPartyEvent;
	l_clsPartyEvent.SetType( GET_PARTY );
	l_clsPartyEvent.SetPartyEventType( CPartyEvent::PLAYER_JOIN_MY_PARTY );
	l_clsPartyEvent.SetApplicant( l_pPlayer );
	_GetGameEventBoard()->PushEvent( l_clsPartyEvent );
	
	/*
	//특정인이 자신에게 파티원으로 지원함
	#define party_join_application_question_zocl 5
	struct _party_join_application_question_zocl {
		_CLID idApplicant;
	};*/

	return TRUE;
}

// 다른 플레이어가 아바타에게 파티원으로 지원함 => 에 대한 답변
BOOL
CNetMsgProcessor_Party::PartyJoinApplicationAnswer_clzo( CPlayer * pi_pPlayer )
{
	if( !pi_pPlayer )
		return FALSE;

	_party_join_application_answer_clzo l_stSend;

	l_stSend.idApplicant.dwSerial	= pi_pPlayer->GetIndex();
	l_stSend.idApplicant.wIndex		= pi_pPlayer->GetServerIndex();

	return SendNetMessage( party_join_application_answer_clzo,
						   &l_stSend,
						   sizeof( _party_join_application_answer_clzo ) );

	/*
	//특정인이 자신에게 파티원으로 지원요청의 답변
	#define party_join_application_answer_clzo 6
	struct _party_join_application_answer_clzo {
		_CLID idApplicant;
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 아바타가 파티에 가입했을때 전달되는 파티 멤버의 리스트
BOOL
CNetMsgProcessor_Party::PartyJoinJoinerResult_zocl( char * pi_pMsg )
{
	_party_join_joiner_result_zocl * l_pRecvData =
		reinterpret_cast< _party_join_joiner_result_zocl * >( pi_pMsg );

	for( int i = 0; i < l_pRecvData->byListNum; ++i )
	{
		if( _GetAvatar()->GetIndex() == l_pRecvData->List[i].dwSerial )
			continue;

		_GetPartyBoard()->AddMember( l_pRecvData->List[i].dwSerial, l_pRecvData->List[i].szAvatorName );
	}

	// 제일 끝에 아바타 자신도 포함시킨다. ( UI에는 추가시키지 않는다. )
	_GetPartyBoard()->AddMember( _GetAvatar() );

	/*
	//모집요청승락시 자신에게 조직원리스트를 보냄..
	#define party_join_joiner_result_zocl 7
	struct _party_join_joiner_result_zocl {
		struct _list{
			DWORD dwSerial;
			char szAvatorName[max_name_len+1];
			// DWORD dwIP;	//add(4/4) //del (6/22)
		};

		BYTE byListNum;
		_list List[member_per_party];

		_party_join_joiner_result_zocl(){ byListNum = 0; }

		int size(){ return sizeof(*this)-sizeof(_list)*(member_per_party-byListNum); }
	};*/

	return TRUE;
}

// 다른 플레이어가 아바타가 속해 있는 파티에 가입했을때 전달되는 정보
BOOL
CNetMsgProcessor_Party::PartyJoinMemberResult_zocl( char * pi_pMsg )
{
	_party_join_member_result_zocl * l_pRecvData =
		reinterpret_cast< _party_join_member_result_zocl * >( pi_pMsg );

	if( _GetPartyBoard()->GetTotalMemberNum() == 0 )
		_GetPartyBoard()->AddMember( _GetAvatar() );

	return _GetPartyBoard()->AddMember( l_pRecvData->dwJoinerSerial, l_pRecvData->szJoinerName );

	/*
	//모집요청승락시 기존조직원들에게 요청승락을 알림..
	#define party_join_member_result_zocl 8
	struct _party_join_member_result_zocl {
		DWORD dwJoinerSerial;
		char szJoinerName[max_name_len+1];
		//DWORD dwIP;	//add(4/4) //del(6/22)
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 파티 탈퇴 요청
BOOL
CNetMsgProcessor_Party::PartyLeaveSelfRequest_clzo( void )
{
	return SendNetMessage( party_leave_self_request_clzo,
						   NULL,
						   0 );

	/*
	//스스로 조직에서 탈퇴함을 요청
	#define party_leave_self_request_clzo 9
	struct _party_leave_self_request_clzo {
	};*/
}

// 파티 탈퇴 요청에 대한 응답
BOOL
CNetMsgProcessor_Party::PartyLeaveSelfResult_zocl( char * pi_pMsg )
{
	_party_leave_self_result_zocl * l_pRecvData =
		reinterpret_cast< _party_leave_self_result_zocl * >( pi_pMsg );

	if( l_pRecvData->dwExiterSerial != 0xFF )
	{
		_GetPartyBoard()->LeaveFromMyParty_Result( l_pRecvData->dwExiterSerial );

		if( l_pRecvData->bWorldExit )
			_GetCharMgr()->DeleteCharacter( CTI_PLAYER, l_pRecvData->dwExiterSerial );
	}
	else
	{
		_SetSystemMsg( "아바타의 파티 탈퇴 요청이 실패 했습니다." );
		return FALSE;
	}

	return TRUE;

	/*
	//스스로 조직에서 탈퇴함을 자신포함조직원에게 알림..
	//보스의 탈퇴의경우 다음끗발자가 보스로 승계
	#define party_leave_self_result_zocl 10
	struct _party_leave_self_result_zocl {
		DWORD dwExiterSerial;		//0xFF이면 실패메시지..자신에게만
		bool  bWorldExit;		//add(7/3)..월드에서 나감으로 파티탈퇴
	};*/
}

// 파티 강퇴 요청
BOOL
CNetMsgProcessor_Party::PartyLeaveCompulsionRequest_clzo( DWORD pi_dwIndex )
{
	_party_leave_compulsion_request_clzo l_stSend;

	l_stSend.dwExiterSerial = pi_dwIndex;

	return SendNetMessage( party_leave_compulsion_request_clzo,
						   &l_stSend,
						   sizeof( _party_leave_compulsion_request_clzo ) );

	/*
	//조직에서 강퇴시킴을 요청
	#define party_leave_compulsion_request_clzo 11
	struct _party_leave_compulsion_request_clzo {
		DWORD dwExiterSerial;
	};*/
}

// 파티 강퇴 요청에 대한 응답
BOOL
CNetMsgProcessor_Party::PartyLeaveCompulsionResult_zocl( char * pi_pMsg )
{
	_party_leave_compulsion_result_zocl * l_pRecvData =
		reinterpret_cast< _party_leave_compulsion_result_zocl * >( pi_pMsg );

	if( l_pRecvData->dwExiterSerial != 0xFF )
	{
		_GetPartyBoard()->BanishPlayerFromMyParty_Result( l_pRecvData->dwExiterSerial );
	}
	else
	{
		_SetSystemMsg( "아바타의 파티 강티 요청이 실패 했습니다." );
		return FALSE;
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 파티 해체 요청
BOOL
CNetMsgProcessor_Party::PartyDisjointRequest_clzo( void )
{
	return SendNetMessage( party_disjoint_request_clzo,
						   NULL,
						   0 );

	/*
	//조직을 해체요청..보스만 가능
	#define party_disjoint_request_clzo 13
	struct _party_disjoint_request_clzo {
	};*/
}

// 파티 해체 요청에 대한 응답
BOOL
CNetMsgProcessor_Party::PartyDisjointResult_zocl( char * pi_pMsg )
{
	_party_disjoint_result_zocl * l_pRecvData =
		reinterpret_cast< _party_disjoint_result_zocl * >( pi_pMsg );

	if( l_pRecvData->bySuccess )
	{
		_GetPartyBoard()->Clear();
		_SetSystemMsg( "파티가 해제 되었습니다." );
	}
	else
	{
		_SetSystemMsg( "파티가 해제 요청이 실패하였습니다." );
		return FALSE;
	}

	return TRUE;
	
	/*
	//조직이 해체됬음을 조직원에게 알림..
	#define party_disjoint_result_zocl 14
	struct _party_disjoint_result_zocl {
		BYTE bySuccess;	//(bool) false면 자신에게만 옴.. true면 전 파티원에게..
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 파티리더 변경 요청
BOOL
CNetMsgProcessor_Party::PartySuccessionRequest_clzo( DWORD pi_dwLeaderIndex )
{
	_party_succession_request_clzo l_stSend;

	l_stSend.dwSuccessorSerial = pi_dwLeaderIndex;

	return SendNetMessage( party_succession_request_clzo,
						   &l_stSend,
						   sizeof( _party_succession_request_clzo ) );

	/*
	//보스승계요청
	#define party_succession_request_clzo 15
	struct _party_succession_request_clzo {
		DWORD dwSuccessorSerial;
	};*/
}

// 파티리더 변경 요청에 대한 응답
BOOL
CNetMsgProcessor_Party::PartySuccessionResult_zocl( char * pi_pMsg )
{
	_party_succession_result_zocl * l_pRecvData =
		reinterpret_cast< _party_succession_result_zocl * >( pi_pMsg );

	if( l_pRecvData->dwSuccessorSerial != 0xFF )
	{
		_GetPartyBoard()->ChangeLeader_Result( l_pRecvData->dwSuccessorSerial );
	}
	else
	{
		_SetSystemMsg( "파티리더 변경요청이 실패 했습니다." );
		return FALSE;
	}

	/*
	//보스승계 알림
	#define party_succession_result_zocl 16
	struct _party_succession_result_zocl {
		DWORD dwSuccessorSerial;	//0xff이면 실패.. 자기한테만..
	};*/

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 파티 잠금 요청
BOOL
CNetMsgProcessor_Party::PartyLockRequest_clzo( BOOL pi_bIsLock )
{
	_party_lock_request_clzo l_stSend;

	l_stSend.bLock = static_cast< bool >( pi_bIsLock );

	return SendNetMessage( party_lock_request_clzo,
						   &l_stSend,
						   sizeof( _party_lock_request_clzo ) );
}

// 파티 잠금 요청에 대한 응답
BOOL
CNetMsgProcessor_Party::PartyLockResult_zocl( char * pi_pMsg )
{
	_party_lock_result_zocl * l_pRecvData =
		reinterpret_cast< _party_lock_result_zocl * >( pi_pMsg );

	if( l_pRecvData->byLock != 0xFF )
	{
		_GetPartyBoard()->LockOnMyParty_Result( l_pRecvData->byLock );
	}
	else
	{
		_SetSystemMsg( "파티 잠금 요청 실패" );
		return FALSE;
	}

	return TRUE;
	
	/*
	//파티멤버 잠금요청 알림
	#define party_lock_result_zocl 116
	struct _party_lock_result_zocl {	//add(7/5)
		BYTE byLock;	//1..잠금.. 0.. 품..	0xFF : 에러..
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 파티원 정보 수신
BOOL
CNetMsgProcessor_Party::PartyMemberInfo_upd( char * pi_pMsg )
{
	_party_member_info_upd * pRecvData = reinterpret_cast< _party_member_info_upd * >( pi_pMsg );

	_GetPartyBoard()->SetMemberLevel( pRecvData->dwMemSerial, pRecvData->byLv );
	_GetPartyBoard()->SetMemberMaxHP( pRecvData->dwMemSerial, pRecvData->wMaxHP );
	_GetPartyBoard()->SetMemberHP( pRecvData->dwMemSerial, pRecvData->wHP );
	_GetPartyBoard()->SetMemberMaxFP( pRecvData->dwMemSerial, pRecvData->wMaxFP );
	_GetPartyBoard()->SetMemberFP( pRecvData->dwMemSerial, pRecvData->wFP );
	_GetPartyBoard()->SetMemberMaxSP( pRecvData->dwMemSerial, pRecvData->wMaxSP );
	_GetPartyBoard()->SetMemberSP( pRecvData->dwMemSerial, pRecvData->wSP );

	_GetPartyBoard()->ClearMemberSF( pRecvData->dwMemSerial );
	for( int i = 0; i < pRecvData->byContEffectNum; ++i )
	{
		_GetPartyBoard()->SetMemberSF( pRecvData->dwMemSerial,
									   pRecvData->Effect[i].wEffectCode >> 12,
									   pRecvData->Effect[i].wEffectCode & 0x0FFF,
									   pRecvData->Effect[i].byEffectLv,
									   TRUE );
	}

	Vector3f vecMemberPosition = { pRecvData->zPos[0], 0, pRecvData->zPos[1] };
	_GetPartyBoard()->SetMemberPosition( pRecvData->dwMemSerial, pRecvData->byMapCode, vecMemberPosition );

	return TRUE;

	/*
	#define party_member_info_upd 17
	struct _party_member_info_upd {
		DWORD dwMemSerial; //맴버의 시리얼.
		WORD wHP;
		WORD wFP;
		WORD wSP;	
		WORD wMaxHP;	
		WORD wMaxFP;	
		WORD wMaxSP;
		BYTE byLv;	
		BYTE byMapCode;
		short zPos[2]; //x, z

		BYTE byContEffectNum;

		struct _EFFECT {
			WORD wEffectCode;	//맨왼쪽비트... effect_code.. 그다음부턴 SFIndex..	
			BYTE byEffectLv;	
	//		WORD wLeftSec;//작은것일수록 최근에 걸린거..		//del(5/4)	
		};	

		_EFFECT Effect[CONT_SF_SIZE*2];
		
		int size(){ return sizeof(*this)-sizeof(_EFFECT)*(CONT_SF_SIZE*2-byContEffectNum); }
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 아바타의 Exp 세팅
BOOL
CNetMsgProcessor_Party::PartyExpDivision_zocl( char * pi_pMsg )
{
	_party_exp_division_zocl * l_pRecvData =
		reinterpret_cast< _party_exp_division_zocl * >( pi_pMsg );

	_GetAvatar()->SetExp( static_cast< float >( l_pRecvData->wExpRate ) / 100.0f );

	return TRUE;

	/*
	#define party_exp_division_zocl 21
	struct _party_exp_division_zocl {
	//	double dExp;
		WORD wExpRate; //add(6/17)
	};*/
}

// 파티원 HP 수신
BOOL
CNetMsgProcessor_Party::PartyMemberHp_upd( char * pi_pMsg )
{
	_party_member_hp_upd * l_pRecvData =
		reinterpret_cast< _party_member_hp_upd * >( pi_pMsg );

	_GetPartyBoard()->SetMemberHP( l_pRecvData->dwMemSerial, l_pRecvData->wHP );

	return TRUE;

	/*
	#define party_member_hp_upd 117
	struct _party_member_hp_upd {	//add(4/4)
		DWORD dwMemSerial; //맴버의 시리얼.
		WORD wHP;
	};*/
}

// 파티원 FP 수신
BOOL
CNetMsgProcessor_Party::PartyMemberFp_upd( char * pi_pMsg )
{
	_party_member_fp_upd * l_pRecvData =
		reinterpret_cast< _party_member_fp_upd * >( pi_pMsg );

	_GetPartyBoard()->SetMemberFP( l_pRecvData->dwMemSerial, l_pRecvData->wFP );

	return TRUE;

	/*
	#define party_member_fp_upd 119
	struct _party_member_fp_upd {	//add(4/4)
		DWORD dwMemSerial; //맴버의 시리얼.
		WORD wFP;
	};*/
}

// 파티원 SP 수신
BOOL
CNetMsgProcessor_Party::PartyMemberSp_upd( char * pi_pMsg )
{
	_party_member_sp_upd * l_pRecvData =
		reinterpret_cast< _party_member_sp_upd * >( pi_pMsg );

	_GetPartyBoard()->SetMemberSP( l_pRecvData->dwMemSerial, l_pRecvData->wSP );

	return TRUE;

	/*
	#define party_member_sp_upd 118
	struct _party_member_sp_upd {	//add(10/14)
		DWORD dwMemSerial; //맴버의 시리얼.
		WORD wSP;
	};*/
}

// 파티원 레벨업 수신
BOOL
CNetMsgProcessor_Party::PartyMemberLv_upd( char * pi_pMsg )
{
	_party_member_lv_upd * l_pRecvData =
		reinterpret_cast< _party_member_lv_upd * >( pi_pMsg );

	_GetPartyBoard()->SetMemberLevel( l_pRecvData->dwMemSerial, l_pRecvData->byLv );

	return TRUE;

	/*
	#define party_member_lv_upd 119	//add(7/5)
	struct _party_member_lv_upd {	
		DWORD dwMemSerial; //맴버의 시리얼.
		BYTE byLv;
	};*/
}

// 파티원 위치 수신
BOOL
CNetMsgProcessor_Party::PartyMemberPos_upd( char * pi_pMsg )
{
	_party_member_pos_upd * l_pRecvData =
		reinterpret_cast< _party_member_pos_upd * >( pi_pMsg );

	Vector3f l_vecMemberPosition = { l_pRecvData->zPos[0], 0, l_pRecvData->zPos[1] };
	_GetPartyBoard()->SetMemberPosition( l_pRecvData->dwMemSerial, l_pRecvData->byMapCode, l_vecMemberPosition );

	return TRUE;

	/*
	#define party_member_pos_upd 120	//add(7/5)
	struct _party_member_pos_upd {	
		DWORD dwMemSerial; //맴버의 시리얼.
		BYTE byMapCode;
		short zPos[2];
	};*/
}

BOOL
CNetMsgProcessor_Party::PartyMemberMaxHfsp_upd( char * pi_pMsg )
{
	_party_member_max_hfsp_upd * pRecvData = reinterpret_cast< _party_member_max_hfsp_upd * >( pi_pMsg );

	_GetPartyBoard()->SetMemberMaxHP( pRecvData->dwMemSerial, pRecvData->wMaxHP );
	_GetPartyBoard()->SetMemberMaxFP( pRecvData->dwMemSerial, pRecvData->wMaxFP );
	_GetPartyBoard()->SetMemberMaxSP( pRecvData->dwMemSerial, pRecvData->wMaxSP );

	return TRUE;

	/*
	#define party_member_max_hfsp_upd 121
	struct _party_member_max_hfsp_upd {
		DWORD dwMemSerial; //맴버의 시리얼.
		WORD wMaxHP;
		WORD wMaxFP;
		WORD wMaxSP;	//add(10/14)
	};*/
}

// 파티원 이펙트 수신
BOOL
CNetMsgProcessor_Party::PartyMemberEffect_upd( char * pi_pMsg )
{
	_party_member_effect_upd * pRecvData = reinterpret_cast< _party_member_effect_upd * >( pi_pMsg );

	_GetPartyBoard()->SetMemberSF( pRecvData->dwMemSerial,
								   pRecvData->wEffectCode >> 12, pRecvData->wEffectCode & 0x0FFF,
								   pRecvData->byEffectLv, !pRecvData->byAlterCode );
	return TRUE;

	/*
	#define party_member_effect_upd 20
	struct _party_member_effect_upd {
		DWORD dwMemSerial; //맴버의 시리얼.
		BYTE byAlterCode;	//0: 삽입.. 1: 제거..
		WORD wEffectCode;	//맨왼쪽비트... effect_code.. 그다음부턴 SFIndex..	upt(4/12)
		BYTE byEffectLv;	//add(8/18)	
	};*/
}