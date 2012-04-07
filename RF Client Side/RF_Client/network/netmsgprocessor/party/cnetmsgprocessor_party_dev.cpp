////////////////////////////////////////////////////////////////////////////////
//
// CNetMsgProcessor_Party_Dev Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _DEVELOPER_

#include "../../../GameObject/Manager/CCharacterMgr.h"
#include "../../../GameObject/Board/General/CGameEventBoard.h"
#include "../../../GameObject/Board/General/CPartyBoard.h"

#include "CNetMsgProcessor_Party_Dev.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CNetMsgProcessor_Party_Dev::CNetMsgProcessor_Party_Dev()
{
	const char HEADER[16] = "Party";

	memcpy( m_pLogHeader, HEADER, strlen( HEADER ) );
}

CNetMsgProcessor_Party_Dev::~CNetMsgProcessor_Party_Dev()
{
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 다른 플레이어를 자신의 파티원으로 초대함
BOOL
CNetMsgProcessor_Party_Dev::PartyJoinInvitation_clzo( CPlayer * pi_pPlayer )
{
	if( !pi_pPlayer )
		return FALSE;

	NetLogWrite( "Aavatar( %s : %d ) => Request for Invite => Player( %s : %d )",
				 _GetAvatar()->GetName(), _GetAvatar()->GetIndex(),
				 pi_pPlayer->GetName(), pi_pPlayer->GetIndex() );

	return CNetMsgProcessor_Party::PartyJoinInvitation_clzo( pi_pPlayer );
}

// 다른 플레이어가 아바타를 파티에 초대 했음
BOOL
CNetMsgProcessor_Party_Dev::PartyJoinInvitationQuestion_zocl( char * pi_pMsg )
{
	_party_join_invitation_question_zocl * l_pRecvData =
		reinterpret_cast< _party_join_invitation_question_zocl * >( pi_pMsg );

	char l_pPlayerName[64];
	CPlayer * l_pPlayer = _GetCharMgr()->GetPlayer( l_pRecvData->idBoss.dwSerial );
	if( l_pPlayer )	_strcpy( l_pPlayerName, l_pPlayer->GetName() );
	else			_strcpy( l_pPlayerName, "None" );

	NetLogWrite( "Player( %s : %d ) => Request for Invite => Aavatar( %s : %d )",
				 l_pPlayerName, l_pRecvData->idBoss.dwSerial,
				 _GetAvatar()->GetName(), _GetAvatar()->GetIndex() );

	return CNetMsgProcessor_Party::PartyJoinInvitationQuestion_zocl( pi_pMsg );
}

// 다른 플레이어가 아바타를 파티에 초대에 했음 => 에 대한 응답을 서버로 보낸다.
BOOL
CNetMsgProcessor_Party_Dev::PartyJoinInvitationAnswer_clzo( CPlayer * pi_pPlayer )
{
	if( !pi_pPlayer )
		return FALSE;

	NetLogWrite( "Player( %s : %d ) => Request for Invite => Aavatar( %s : %d ) => Accept",
				 pi_pPlayer->GetName(), pi_pPlayer->GetIndex(),
				 _GetAvatar()->GetName(), _GetAvatar()->GetIndex() );

	return CNetMsgProcessor_Party::PartyJoinInvitationAnswer_clzo( pi_pPlayer );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 아바타가 다른 파티장에게 파티원으로 지원함
BOOL
CNetMsgProcessor_Party_Dev::PartyJoinApplication_clzo( CPlayer * pi_pPlayer )
{
	if( !pi_pPlayer )
		return FALSE;

	NetLogWrite( "Aavatar( %s : %d ) => Request for Join => Party Leader( %s : %d )",
				 _GetAvatar()->GetName(), _GetAvatar()->GetIndex(),
				 pi_pPlayer->GetName(), pi_pPlayer->GetIndex() );

	return CNetMsgProcessor_Party::PartyJoinApplication_clzo( pi_pPlayer );
}

// 다른 플레이어가 아바타에게 파티원으로 지원함
BOOL
CNetMsgProcessor_Party_Dev::PartyJoinApplicationQuestion_zocl( char * pi_pMsg )
{
	_party_join_application_question_zocl * l_pRecvData =
		reinterpret_cast< _party_join_application_question_zocl * >( pi_pMsg );

	CPlayer * l_pPlayer = _GetCharMgr()->GetPlayer( l_pRecvData->idApplicant.dwSerial );
	if( l_pPlayer )	_strcpy( m_pLogBuffer, l_pPlayer->GetName() );
	else			_strcpy( m_pLogBuffer, "None" );

	NetLogWrite( "Player ( %s : %d ) => Request for Join => Aavatar<Party Leader> ( %s : %d )",
				 m_pLogBuffer, l_pRecvData->idApplicant.dwSerial,
				 _GetAvatar()->GetName(), _GetAvatar()->GetIndex() );

	return CNetMsgProcessor_Party::PartyJoinApplicationQuestion_zocl( pi_pMsg );
}

// 다른 플레이어가 아바타에게 파티원으로 지원함 => 에 대한 답변
BOOL
CNetMsgProcessor_Party_Dev::PartyJoinApplicationAnswer_clzo( CPlayer * pi_pPlayer )
{
	if( !pi_pPlayer )
		return FALSE;

	NetLogWrite( "Player ( %s : %d ) => Request for Join => Aavatar<Party Leader> ( %s : %d ) => Accept",
				 pi_pPlayer->GetName(), pi_pPlayer->GetIndex(),
				 _GetAvatar()->GetName(), _GetAvatar()->GetIndex() );

	return CNetMsgProcessor_Party::PartyJoinApplicationAnswer_clzo( pi_pPlayer );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 아바타가 파티에 가입했을때 전달되는 파티 멤버의 리스트
BOOL
CNetMsgProcessor_Party_Dev::PartyJoinJoinerResult_zocl( char * pi_pMsg )
{
	_party_join_joiner_result_zocl * l_pRecvData =
		reinterpret_cast< _party_join_joiner_result_zocl * >( pi_pMsg );

	NetLogWrite( "< Party Member List : Total Player %d >", l_pRecvData->byListNum );
	for( int i = 0; i < l_pRecvData->byListNum; ++i )
	{
		NetLogWrite( "			Player %d : %s ( %d )",
					 i, l_pRecvData->List[i].szAvatorName, l_pRecvData->List[i].dwSerial );
	}

	return CNetMsgProcessor_Party::PartyJoinJoinerResult_zocl( pi_pMsg );
}

// 다른 플레이어가 아바타가 속해 있는 파티에 가입했을때 전달되는 정보
BOOL
CNetMsgProcessor_Party_Dev::PartyJoinMemberResult_zocl( char * pi_pMsg )
{
	_party_join_member_result_zocl * l_pRecvData =
		reinterpret_cast< _party_join_member_result_zocl * >( pi_pMsg );

	NetLogWrite( "< Add Member > Player ( %s : %d )",
				 l_pRecvData->szJoinerName, l_pRecvData->dwJoinerSerial );

	return CNetMsgProcessor_Party::PartyJoinMemberResult_zocl( pi_pMsg );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 파티 탈퇴 요청
BOOL
CNetMsgProcessor_Party_Dev::PartyLeaveSelfRequest_clzo( void )
{
	NetLogWrite( "Avatar ( %s : %d ) => Request for Quit Party",
				 _GetAvatar()->GetName(), _GetAvatar()->GetIndex() );

	return CNetMsgProcessor_Party::PartyLeaveSelfRequest_clzo();
}

// 파티 탈퇴 요청에 대한 응답
BOOL
CNetMsgProcessor_Party_Dev::PartyLeaveSelfResult_zocl( char * pi_pMsg )
{
	_party_leave_self_result_zocl * l_pRecvData =
		reinterpret_cast< _party_leave_self_result_zocl * >( pi_pMsg );

	CPlayer * l_pPlayer = _GetPartyBoard()->GetMember( l_pRecvData->dwExiterSerial );
	if( l_pPlayer )	sprintf( m_pLogBuffer, "Player ( %s : %d )", l_pPlayer->GetName(), l_pRecvData->dwExiterSerial );
	else			sprintf( m_pLogBuffer, "Player ( None : %d )", l_pRecvData->dwExiterSerial );
	
	NetLogWrite( "< Quit from Party > =>", m_pLogBuffer );

	return CNetMsgProcessor_Party::PartyLeaveSelfResult_zocl( pi_pMsg );
}

// 파티 강퇴 요청
BOOL
CNetMsgProcessor_Party_Dev::PartyLeaveCompulsionRequest_clzo( DWORD pi_dwIndex )
{
	CPlayer * l_pPlayer = _GetPartyBoard()->GetMember( pi_dwIndex );
	if( l_pPlayer )	_strcpy( m_pLogBuffer, l_pPlayer->GetName() );
	else			_strcpy( m_pLogBuffer, "None" );

	NetLogWrite( "Avatar ( %s : %d ) => Request for Banish => Player( %s : %d )",
				 _GetAvatar()->GetName(), _GetAvatar()->GetIndex(),
				 m_pLogBuffer, pi_dwIndex );

	return CNetMsgProcessor_Party::PartyLeaveCompulsionRequest_clzo( pi_dwIndex );
}

// 파티 강퇴 요청에 대한 응답
BOOL
CNetMsgProcessor_Party_Dev::PartyLeaveCompulsionResult_zocl( char * pi_pMsg )
{
	_party_leave_compulsion_result_zocl * l_pRecvData =
		reinterpret_cast< _party_leave_compulsion_result_zocl * >( pi_pMsg );

	CPlayer * l_pPlayer = _GetPartyBoard()->GetMember( l_pRecvData->dwExiterSerial );
	if( l_pPlayer )	sprintf( m_pLogBuffer, "Player ( %s : %d )", l_pPlayer->GetName(), l_pRecvData->dwExiterSerial );
	else			sprintf( m_pLogBuffer, "Player ( None : %d )", l_pRecvData->dwExiterSerial );
	
	NetLogWrite( "< Banished from Party > => %s", m_pLogBuffer );

	return CNetMsgProcessor_Party::PartyLeaveCompulsionResult_zocl( pi_pMsg );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 파티 해체 요청
BOOL
CNetMsgProcessor_Party_Dev::PartyDisjointRequest_clzo( void )
{
	NetLogWrite( "Avatar ( %s : %d ) => Request for Disjoint Party",
				 _GetAvatar()->GetName(), _GetAvatar()->GetIndex() );

	return CNetMsgProcessor_Party::PartyDisjointRequest_clzo();
}

// 파티 해체 요청에 대한 응답
BOOL
CNetMsgProcessor_Party_Dev::PartyDisjointResult_zocl( char * pi_pMsg )
{
	_party_disjoint_result_zocl * l_pRecvData =
		reinterpret_cast< _party_disjoint_result_zocl * >( pi_pMsg );

	NetLogWrite( "< Disjoint Party >" );

	return CNetMsgProcessor_Party::PartyDisjointResult_zocl( pi_pMsg );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 파티리더 변경 요청
BOOL
CNetMsgProcessor_Party_Dev::PartySuccessionRequest_clzo( DWORD pi_dwLeaderIndex )
{
	CPlayer * l_pPlayer = _GetPartyBoard()->GetMember( pi_dwLeaderIndex );
	if( l_pPlayer )	_strcpy( m_pLogBuffer, l_pPlayer->GetName() );
	else			_strcpy( m_pLogBuffer, "None" );

	NetLogWrite( "Avatar ( %s : %d ) => Request for Change Leader => Player( %s : %d )",
				 _GetAvatar()->GetName(), _GetAvatar()->GetIndex(),
				 m_pLogBuffer, pi_dwLeaderIndex );

	return CNetMsgProcessor_Party::PartySuccessionRequest_clzo( pi_dwLeaderIndex );
}

// 파티리더 변경 요청에 대한 응답
BOOL
CNetMsgProcessor_Party_Dev::PartySuccessionResult_zocl( char * pi_pMsg )
{
	_party_succession_result_zocl * l_pRecvData =
		reinterpret_cast< _party_succession_result_zocl * >( pi_pMsg );

	CPlayer * l_pPlayer = _GetPartyBoard()->GetMember( l_pRecvData->dwSuccessorSerial );
	if( l_pPlayer )	sprintf( m_pLogBuffer, "Player ( %s : %d )", l_pPlayer->GetName(), l_pRecvData->dwSuccessorSerial );
	else			sprintf( m_pLogBuffer, "Player ( None : %d )", l_pRecvData->dwSuccessorSerial );
	
	NetLogWrite( "< Change Leader > => %s", m_pLogBuffer );

	return CNetMsgProcessor_Party::PartySuccessionResult_zocl( pi_pMsg );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 파티 잠금 요청
BOOL
CNetMsgProcessor_Party_Dev::PartyLockRequest_clzo( BOOL pi_bIsLock )
{
	return CNetMsgProcessor_Party::PartyLockRequest_clzo( pi_bIsLock );
}

// 파티 잠금 요청에 대한 응답
BOOL
CNetMsgProcessor_Party_Dev::PartyLockResult_zocl( char * pi_pMsg )
{
	_party_lock_result_zocl * l_pRecvData =
		reinterpret_cast< _party_lock_result_zocl * >( pi_pMsg );

	return CNetMsgProcessor_Party::PartyLockResult_zocl( pi_pMsg );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 파티원 정보 수신
BOOL
CNetMsgProcessor_Party_Dev::PartyMemberInfo_upd( char * pi_pMsg )
{
	_party_member_info_upd * l_pRecvData =
		reinterpret_cast< _party_member_info_upd * >( pi_pMsg );

	CPlayer * l_pPlayer = _GetPartyBoard()->GetMember( l_pRecvData->dwMemSerial );
	if( l_pPlayer )	sprintf( m_pLogBuffer, "Player ( %s : %d )", l_pPlayer->GetName(), l_pRecvData->dwMemSerial );
	else			sprintf( m_pLogBuffer, "Player ( None : %d )", l_pRecvData->dwMemSerial );

	sprintf( m_pLogBuffer, "%s => HP : %d/%d, FP : %d/%d, SP : %d/%d", m_pLogBuffer,
			 l_pRecvData->wHP, l_pRecvData->wMaxHP,
			 l_pRecvData->wFP, l_pRecvData->wMaxFP,
			 l_pRecvData->wSP, l_pRecvData->wMaxSP );
	NetLogWrite( "< Receive Member Info > => %s", m_pLogBuffer );

	return CNetMsgProcessor_Party::PartyMemberInfo_upd( pi_pMsg );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 아바타의 Exp 세팅
BOOL
CNetMsgProcessor_Party_Dev::PartyExpDivision_zocl( char * pi_pMsg )
{
	//_party_exp_division_zocl * l_pRecvData =
	//	reinterpret_cast< _party_exp_division_zocl * >( pi_pMsg );

	return CNetMsgProcessor_Party::PartyExpDivision_zocl( pi_pMsg );
}

// 파티원 HP 수신
BOOL
CNetMsgProcessor_Party_Dev::PartyMemberHp_upd( char * pi_pMsg )
{
	/*
	_party_member_hp_upd * l_pRecvData =
		reinterpret_cast< _party_member_hp_upd * >( pi_pMsg );

	CPlayer * l_pPlayer = _GetPartyBoard()->GetMember( l_pRecvData->dwMemSerial );
	if( l_pPlayer )	sprintf( m_pLogBuffer, "Player ( %s : %d )", l_pPlayer->GetName(), l_pRecvData->dwMemSerial );
	else			sprintf( m_pLogBuffer, "Player ( None : %d )", l_pRecvData->dwMemSerial );

	sprintf( m_pLogBuffer, "%s => HP : %d", m_pLogBuffer, l_pRecvData->wHP );
	NetLogWrite( "< Receive Member HP > => %s", m_pLogBuffer );*/

	return CNetMsgProcessor_Party::PartyMemberHp_upd( pi_pMsg );
}

// 파티원 FP 수신
BOOL
CNetMsgProcessor_Party_Dev::PartyMemberFp_upd( char * pi_pMsg )
{
	/*
	_party_member_fp_upd * l_pRecvData =
		reinterpret_cast< _party_member_fp_upd * >( pi_pMsg );

	CPlayer * l_pPlayer = _GetPartyBoard()->GetMember( l_pRecvData->dwMemSerial );
	if( l_pPlayer )	sprintf( m_pLogBuffer, "Player ( %s : %d )", l_pPlayer->GetName(), l_pRecvData->dwMemSerial );
	else			sprintf( m_pLogBuffer, "Player ( None : %d )", l_pRecvData->dwMemSerial );

	sprintf( m_pLogBuffer, "%s => FP : %d", m_pLogBuffer, l_pRecvData->wFP );
	NetLogWrite( "< Receive Member FP > => %s", m_pLogBuffer );*/

	return CNetMsgProcessor_Party::PartyMemberFp_upd( pi_pMsg );
}

// 파티원 SP 수신
BOOL
CNetMsgProcessor_Party_Dev::PartyMemberSp_upd( char * pi_pMsg )
{
	/*
	_party_member_sp_upd * l_pRecvData =
		reinterpret_cast< _party_member_sp_upd * >( pi_pMsg );

	CPlayer * l_pPlayer = _GetPartyBoard()->GetMember( l_pRecvData->dwMemSerial );
	if( l_pPlayer )	sprintf( m_pLogBuffer, "Player ( %s : %d )", l_pPlayer->GetName(), l_pRecvData->dwMemSerial );
	else			sprintf( m_pLogBuffer, "Player ( None : %d )", l_pRecvData->dwMemSerial );

	sprintf( m_pLogBuffer, "%s => SP : %d", m_pLogBuffer, l_pRecvData->wSP );
	NetLogWrite( "< Receive Member SP > => %s", m_pLogBuffer );*/

	return CNetMsgProcessor_Party::PartyMemberSp_upd( pi_pMsg );
}

// 파티원 레벨업 수신
BOOL
CNetMsgProcessor_Party_Dev::PartyMemberLv_upd( char * pi_pMsg )
{
	/*
	_party_member_lv_upd * l_pRecvData =
		reinterpret_cast< _party_member_lv_upd * >( pi_pMsg );

	CPlayer * l_pPlayer = _GetPartyBoard()->GetMember( l_pRecvData->dwMemSerial );
	if( l_pPlayer )	sprintf( m_pLogBuffer, "Player ( %s : %d )", l_pPlayer->GetName(), l_pRecvData->dwMemSerial );
	else			sprintf( m_pLogBuffer, "Player ( None : %d )", l_pRecvData->dwMemSerial );

	sprintf( m_pLogBuffer, "%s => Level : %d", m_pLogBuffer, l_pRecvData->byLv );
	NetLogWrite( "< Receive Level Up > => %s", m_pLogBuffer );*/

	return CNetMsgProcessor_Party::PartyMemberLv_upd( pi_pMsg );
}

// 파티원 위치 수신
BOOL
CNetMsgProcessor_Party_Dev::PartyMemberPos_upd( char * pi_pMsg )
{
	/*
	_party_member_pos_upd * l_pRecvData =
		reinterpret_cast< _party_member_pos_upd * >( pi_pMsg );

	CPlayer * l_pPlayer = _GetPartyBoard()->GetMember( l_pRecvData->dwMemSerial );
	if( l_pPlayer )	sprintf( m_pLogBuffer, "Player ( %s : %d )", l_pPlayer->GetName(), l_pRecvData->dwMemSerial );
	else			sprintf( m_pLogBuffer, "Player ( None : %d )", l_pRecvData->dwMemSerial );

	sprintf( m_pLogBuffer, "%s => Map : %d, X : %d, Z : %d", m_pLogBuffer,
			 l_pRecvData->byMapCode, l_pRecvData->zPos[0], l_pRecvData->zPos[1] );
	NetLogWrite( "< Receive Member Position > => %s", m_pLogBuffer );*/

	return CNetMsgProcessor_Party::PartyMemberPos_upd( pi_pMsg );
}

BOOL
CNetMsgProcessor_Party_Dev::PartyMemberMaxHfsp_upd( char * pi_pMsg )
{
	/*
	_party_member_max_hfsp_upd * l_pRecvData =
		reinterpret_cast< _party_member_max_hfsp_upd * >( pi_pMsg );

	CPlayer * l_pPlayer = _GetPartyBoard()->GetMember( l_pRecvData->dwMemSerial );
	if( l_pPlayer )	sprintf( m_pLogBuffer, "Player ( %s : %d )", l_pPlayer->GetName(), l_pRecvData->dwMemSerial );
	else			sprintf( m_pLogBuffer, "Player ( None : %d )", l_pRecvData->dwMemSerial );

	sprintf( m_pLogBuffer, "%s => Max HP : %d, Max FP : %d", m_pLogBuffer, l_pRecvData->wMaxHP, l_pRecvData->wMaxFP );
	NetLogWrite( "< Receive Member Max HP/FP > => %s", m_pLogBuffer );*/

	return CNetMsgProcessor_Party::PartyMemberMaxHfsp_upd( pi_pMsg );
}

// 파티원 이펙트 수신
BOOL
CNetMsgProcessor_Party_Dev::PartyMemberEffect_upd( char * pi_pMsg )
{
	//_party_member_effect_upd * l_pRecvData =
	//	reinterpret_cast< _party_member_effect_upd * >( pi_pMsg );

	return CNetMsgProcessor_Party::PartyMemberEffect_upd( pi_pMsg );
}

#endif // _DEVELOPER_