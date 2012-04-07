////////////////////////////////////////////////////////////////////////////////
//
// CPartyBoard Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../../Manager/CCharacterMgr.h"
#include "../../Manager/CTargetMgr.h"
#include "../../../CLand.h"

#include "../../../Network/NetMsgProcessor/Party/CNetMsgProcessor_Party_Dev.h"

#include "CGameEventBoard.h"
#include "CPartyBoard.h"

#include "../Common/CBoardManager.h"
#include "../../../UserInterface/GameWindow/EtcWin/GUIRadarWindow.h"

CPartyBoard * g_pPartyBoard = NULL;

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CPartyMember::CPartyMember()
{
	m_dwMemberIndex = ID_INVALID;
}

CPartyMember::CPartyMember( CPlayer * pi_pMember )
{
	SetMember( pi_pMember );
}

void
CPartyMember::SetMember( CPlayer * pi_pMember )
{
	if( pi_pMember )
		m_dwMemberIndex = pi_pMember->GetIndex();
}

CPlayer *
CPartyMember::GetMember( void )
{
	return _GetCharMgr()->GetPlayer( m_dwMemberIndex );
}

BOOL
CPartyMember::IsEqual( CPlayer * pi_pPlayer )
{
	if( pi_pPlayer )
	{
		if( pi_pPlayer->GetIndex() == m_dwMemberIndex )
			return TRUE;
	}

	return FALSE;
}

BOOL
CPartyMember::IsEqual( DWORD pi_dwPlayerIndex )
{
	return ( pi_dwPlayerIndex == m_dwMemberIndex );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CPartyBoard::CPartyBoard()
{
	m_dwApplicantIndex[0]	= ID_INVALID;
	m_dwApplicantIndex[1]	= ID_INVALID;

	m_bIsLockOnAddMember	= FALSE;

	m_pNetMsgProcessor		= static_cast< CNetMsgProcessor_Party * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_PARTY ) );
}

CPartyBoard::~CPartyBoard()
{
}

void
CPartyBoard::Clear( void )
{
	m_dwApplicantIndex[0]	= ID_INVALID;
	m_dwApplicantIndex[1]	= ID_INVALID;

	m_bIsLockOnAddMember	= FALSE;

	for( PARTY_LIST_ITER i = m_listParty.begin(); i != m_listParty.end(); ++i )
	{
		SettingToPlayer( ( *i ).GetMember() );
	}
	m_listParty.clear();
	SettingToPlayer( _GetAvatar() );

	m_uiPartyWindow.RemoveAllMember();
	m_uiPartyWindow.Close();
}

void
CPartyBoard::InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY )
{
	m_uiWindow = &m_uiPartyWindow;

	CBaseBoard::InitWindow( pi_dwID, pi_nPosX, pi_nPosY );

	m_uiPartyWindow.SetWindowType( UI_PMT_MEMBER );

	_GetGaugeWindow()->AddDockingCustomer( &m_uiPartyWindow );
	m_uiPartyWindow.EnableDockingBorder( CGUIWindow::BOTTOM, TRUE );
	m_uiPartyWindow.SetDockingBorder( CGUIWindow::BOTTOM );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CPartyBoard::AddMember( CPlayer * pi_pPlayer )
{
	if( !AddMember_CheckCondition( pi_pPlayer ) )
		return FALSE;

	AddMember_SetUI( pi_pPlayer );

	if( AddMember_GetMemberType( pi_pPlayer ) == PMT_LEADER )
		SettingToLeader( pi_pPlayer );
	else
		SettingToMember( pi_pPlayer );

	m_listParty.push_back( CPartyMember( pi_pPlayer ) );
	pi_pPlayer->UI_SetRadar();

	SetApplicantIndex( NULL );

	return TRUE;
}

BOOL
CPartyBoard::AddMember( DWORD pi_dwIndex, char * pi_pName )
{
	CPlayer * l_pPlayer = _GetCharMgr()->GetPlayer( pi_dwIndex );
	if( !l_pPlayer )
	{
		l_pPlayer = _GetCharMgr()->AddPlayer( pi_dwIndex );
		if( !l_pPlayer )
			return FALSE;

		l_pPlayer->SetIndex( pi_dwIndex );
		l_pPlayer->SetName( pi_pName );
	}

	return AddMember( l_pPlayer );
}

BOOL
CPartyBoard::DeleteMember( DWORD pi_dwIndex )
{
	for( PARTY_LIST_ITER i = m_listParty.begin(); i != m_listParty.end(); ++i )
	{
		if( ( *i ).IsEqual( pi_dwIndex ) )
		{
			SettingToPlayer( ( *i ).GetMember() );
			m_uiPartyWindow.RemoveMember( pi_dwIndex );
			m_listParty.erase( i );
			break;
		}
	}

	if( m_listParty.size() <= 1 )
		Clear();

	return FALSE;
}

BOOL
CPartyBoard::ChangeLeader( DWORD pi_dwNewLeaderIndex )
{
	DWORD l_dwPrevLeaderIndex = m_listParty.front().GetIndex();
	if( l_dwPrevLeaderIndex == pi_dwNewLeaderIndex )
	{
		SettingToLeader( m_listParty.front().GetMember() );
		return TRUE;
	}

	CPartyMember l_clsNewPartyLeader;
	for( PARTY_LIST_ITER i = m_listParty.begin(); i != m_listParty.end(); ++i )
	{
		if( ( *i ).IsEqual( l_dwPrevLeaderIndex ) )
		{
			SettingToMember( ( *i ).GetMember() );
		}
		else if( ( *i ).IsEqual( pi_dwNewLeaderIndex ) )
		{
			SettingToLeader( ( *i ).GetMember() );
			l_clsNewPartyLeader = ( *i );
			i = m_listParty.erase( i );
			--i;
		}
	}

	if( l_clsNewPartyLeader.GetIndex() == ID_INVALID )
		return FALSE;

	m_listParty.push_front( l_clsNewPartyLeader );
	return TRUE;
}

BOOL
CPartyBoard::ChangeLeaderToNextMember( void )
{
	return TRUE;
}

void
CPartyBoard::SettingToLeader( CPlayer * pi_pPlayer )
{
	if( !pi_pPlayer )
		return;

	pi_pPlayer->SetPartyMemberType( PMT_LEADER );
	m_uiPartyWindow.SetLeader( pi_pPlayer->GetIndex() );

	if( pi_pPlayer->IsEqualCharType( CTI_AVATAR ) )
		m_uiPartyWindow.SetWindowType( PMT_LEADER );
}

void
CPartyBoard::SettingToMember( CPlayer * pi_pPlayer )
{
	if( !pi_pPlayer )
		return;

	pi_pPlayer->SetPartyMemberType( PMT_MEMBER );
	m_uiPartyWindow.SetMember( pi_pPlayer->GetIndex() );

	if( pi_pPlayer->IsEqualCharType( CTI_AVATAR ) )
		m_uiPartyWindow.SetWindowType( PMT_MEMBER );
}

void
CPartyBoard::SettingToPlayer( CPlayer * pi_pMember )
{
	if( !pi_pMember )
		return;

	pi_pMember->SetPartyMemberType( PMT_NONE );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CPartyBoard::AddMember_CheckCondition( CPlayer * pi_pPlayer )
{
	if( !pi_pPlayer )
		return FALSE;

	if( m_listParty.size() >= MAX_PLAYER_IN_PARTY )
		return FALSE;

	for( PARTY_LIST_ITER i = m_listParty.begin(); i != m_listParty.end(); ++i )
	{
		if( ( *i ).IsEqual( pi_pPlayer ) )
			return FALSE;
	}

	return TRUE;
}

PM_TYPE
CPartyBoard::AddMember_GetMemberType( CPlayer * pi_pPlayer )
{
	if( !pi_pPlayer )
		return PMT_NONE;
	else if( pi_pPlayer->GetCharTypeID() == CTI_AVATAR )
		return static_cast< PARTY_MEMBER_TYPE >( m_uiPartyWindow.GetWindowType() );
	else if( m_uiPartyWindow.GetWindowType() == PMT_LEADER )
		return PMT_MEMBER;
	else if( m_dwApplicantIndex[0] == pi_pPlayer->GetIndex() )
		return PMT_LEADER;
	else if( IsPartyLeader( pi_pPlayer->GetIndex() ) )
		return PMT_LEADER;

	return PMT_MEMBER;
}

void
CPartyBoard::AddMember_SetUI( CPlayer * pi_pPlayer )
{
	if( !pi_pPlayer )
		return;

	if( pi_pPlayer->IsEqualCharType( CTI_AVATAR ) )
	{
		// 파티 합류와 관련된 모든 이벤트를 삭제한다.
		_GetGameEventBoard()->ClearAllEvent( GET_PARTY );
		return;
	}

	GROUP_MEMBER_INFO l_guiMemberInfo;

	l_guiMemberInfo.dwMemberID	= pi_pPlayer->GetIndex();
	_strcpy( l_guiMemberInfo.pCharName, pi_pPlayer->GetName(), 20 );

	m_uiPartyWindow.AddMember( &l_guiMemberInfo );

	if( !m_uiPartyWindow.IsShow() )
		m_uiPartyWindow.Open();
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CPlayer *
CPartyBoard::GetLeader( void )
{
	if( m_listParty.empty() )
		return NULL;

	return m_listParty.front().GetMember();
}

CPlayer *
CPartyBoard::GetMember( DWORD pi_dwIndex )
{
	for( PARTY_LIST_ITER i = m_listParty.begin(); i != m_listParty.end(); ++i )
	{
		if( ( *i ).IsEqual( pi_dwIndex ) )
			return ( *i ).GetMember();
	}

	return NULL;
}

BOOL
CPartyBoard::IsPartyLeader( DWORD pi_dwIndex )
{
	if( m_listParty.empty() )
		return FALSE;

	return m_listParty.front().IsEqual( pi_dwIndex );
}

BOOL
CPartyBoard::IsPartyMember( DWORD pi_dwIndex )
{
	for( PARTY_LIST_ITER i = m_listParty.begin(); i != m_listParty.end(); ++i )
	{
		if( ( *i ).IsEqual( pi_dwIndex ) )
			return TRUE;
	}

	return FALSE;
}

void
CPartyBoard::SetApplicantIndex( CPlayer * pi_pPlayer )
{
	if( pi_pPlayer )
	{
		m_dwApplicantIndex[0]	= pi_pPlayer->GetIndex();
		m_dwApplicantIndex[1]	= pi_pPlayer->GetServerIndex();
	}
	else
	{
		m_dwApplicantIndex[0]	= ID_INVALID;
		m_dwApplicantIndex[1]	= ID_INVALID;
	}
}

int
CPartyBoard::GetTotalMemberNum( void )
{
	return m_listParty.size();
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CPartyBoard::FrameMove( void )
{
}

LRESULT
CPartyBoard::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case WM_PARTY_INVITE_OTHER_PLAYER :
			InviteOrJoinToParty();
			break;

		case WM_PARTY_JOIN_OTHER_PARTY :
			InviteOrJoinToParty();
			break;

		case WM_PARTY_ACCEPT_JOIN :
			AcceptJoinToMyParty( wParam );
			break;

		case WM_PARTY_ACCEPT_INVITE :
			AcceptInviteFromOtherParty( wParam );
			break;

		case WM_PARTY_CHANGE_LEADER :
			ChangeLeader_Request( wParam );
			break;

		case WM_PARTY_BANISH_PLAYER :
			BanishPlayerFromMyParty_Request( wParam );
			break;

		case WM_PARTY_LEAVE_PARTY :
			LeaveFromMyParty_Request();
			break;

		case WM_PARTY_LOCKON_ADDMEMBER :
			LockOnMyParty_Request( wParam );
			break;

		case WM_PARTY_SELECT_MEMBER :
			SelectMember( wParam );
			break;
	}

	return 0;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CPartyBoard::InviteOrJoinToParty( void )
{
	CPlayer * l_pPlayer = static_cast< CPlayer * >( _GetTargetMgr()->GetTarget() );
	if( !l_pPlayer )
		return FALSE;

	if( _GetAvatar()->IsEqualPartyMemberType( PMT_NONE ) )
	{
		if( l_pPlayer->IsEqualPartyMemberType( PMT_NONE ) )
			return RequestInvitePlayerToMyParty( l_pPlayer );
		else if( l_pPlayer->IsEqualPartyMemberType( PMT_LEADER ) )
			return RequestJoinToOtherParty( l_pPlayer );
		else
			_SetSystemMsg( "일반 파티 멤버에게는 가입 초대나 가입 요청을 할 수 없습니다." );
	}
	else if( _GetAvatar()->IsEqualPartyMemberType( PMT_LEADER ) )
	{
		if( l_pPlayer->IsEqualPartyMemberType( PMT_NONE ) )
			return RequestInvitePlayerToMyParty( l_pPlayer );
		else
			_SetSystemMsg( "이미 다른 파티에 속한 플레이어 입니다." );
	}
	else
	{
		_SetSystemMsg( "파티 멤버는 다른 플레이어를 초대할 수 없습니다." );
	}

	return FALSE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 다른 플레이어를 파티에 초대하거나 새로운 파티 생성을 요청함
BOOL
CPartyBoard::RequestInvitePlayerToMyParty( CPlayer * pi_pPlayer )
{
	if( m_bIsLockOnAddMember )
	{
		_SetSystemMsg( "파티 잠금 상태에서는 새로운 파티원을 가입시킬 수 없습니다." );
		return FALSE;
	}

	SetApplicantIndex( pi_pPlayer );

	if( m_uiPartyWindow.GetWindowType() != PMT_LEADER )
		m_uiPartyWindow.SetWindowType( PMT_LEADER );

	return m_pNetMsgProcessor->PartyJoinInvitation_clzo( pi_pPlayer );
}

// 파티 초대를 허락
BOOL
CPartyBoard::AcceptInviteFromOtherParty( DWORD pi_dwPartyLeaderIndex )
{
	if( !_GetAvatar()->IsEqualPartyMemberType( PMT_NONE ) )
	{
		_SetSystemMsg( "아바타가 이미 파티에 가입되어 있습니다." );
		return FALSE;
	}

	CPlayer * l_pPartyLeader = _GetCharMgr()->GetPlayer( pi_dwPartyLeaderIndex );
	if( !l_pPartyLeader )
		return FALSE;

	SetApplicantIndex( l_pPartyLeader );

	if( m_uiPartyWindow.GetWindowType() != PMT_MEMBER )
		m_uiPartyWindow.SetWindowType( PMT_MEMBER );

	return m_pNetMsgProcessor->PartyJoinInvitationAnswer_clzo( l_pPartyLeader );
}

// 다른 파티에 가입 요청을 한다.
BOOL
CPartyBoard::RequestJoinToOtherParty( CPlayer * pi_pPlayer )
{
	SetApplicantIndex( pi_pPlayer );

	if( m_uiPartyWindow.GetWindowType() != PMT_MEMBER )
		m_uiPartyWindow.SetWindowType( PMT_MEMBER );

	return m_pNetMsgProcessor->PartyJoinApplication_clzo( pi_pPlayer );
}

// 가입 요청을 허락
BOOL
CPartyBoard::AcceptJoinToMyParty( DWORD pi_dwPlayerIndex )
{
	CPlayer * l_pPlayer = _GetCharMgr()->GetPlayer( pi_dwPlayerIndex );
	if( !l_pPlayer )
		return FALSE;

	SetApplicantIndex( l_pPlayer );

	if( m_uiPartyWindow.GetWindowType() != PMT_LEADER )
		m_uiPartyWindow.SetWindowType( PMT_LEADER );

	return m_pNetMsgProcessor->PartyJoinApplicationAnswer_clzo( l_pPlayer );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 파티 리더 변경 요청
BOOL
CPartyBoard::ChangeLeader_Request( DWORD pi_dwIndex )
{
	if( !IsPartyLeader( _GetAvatar()->GetIndex() ) )
	{
		_SetSystemMsg( "파티 리더만이 변경 요청을 할 수 있습니다." );
		return FALSE;
	}

	if( !IsPartyMember( pi_dwIndex ) )
	{
		_SetSystemMsg( "자신의 파티원만을 리더로 변경할 수 있습니다." );
		return FALSE;
	}

	m_pNetMsgProcessor->PartySuccessionRequest_clzo( pi_dwIndex );

	return TRUE;
}

BOOL
CPartyBoard::ChangeLeader_Result( DWORD pi_dwIndex )
{
	if( !ChangeLeader( pi_dwIndex ) )
		_SetSystemMsg( "파티리더 변경이 실패하였습니다." );

	CPlayer * l_pNewLeader = GetLeader();
	if( l_pNewLeader )
	{
		_SetSystemMsg( "파티리더가 %s님으로 변경 되었습니다.", l_pNewLeader->GetName() );
		return TRUE;
	}
	else
	{
		_SetSystemMsg( "파티리더가 변경 되었습니다." );
	}

	return FALSE;
}

// 플레이어 강퇴 요청
BOOL
CPartyBoard::BanishPlayerFromMyParty_Request( DWORD pi_dwIndex )
{
	if( !IsPartyLeader( _GetAvatar()->GetIndex() ) )
	{
		_SetSystemMsg( "파티 리더만이 파티원을 강제 퇴장 시킬 수 있습니다." );
		return FALSE;
	}

	if( !IsPartyMember( pi_dwIndex ) )
	{
		_SetSystemMsg( "자신의 파티원만을 강제 퇴장 시킬 수 있습니다." );
		return FALSE;
	}

	m_pNetMsgProcessor->PartyLeaveCompulsionRequest_clzo( pi_dwIndex );

	return TRUE;
}

BOOL
CPartyBoard::BanishPlayerFromMyParty_Result( DWORD pi_dwIndex )
{
	if( _GetAvatar()->GetIndex() == pi_dwIndex )
	{
		_SetSystemMsg( "파티에서 강퇴 당하였습니다." );
		Clear();
		return TRUE;
	}

	CPlayer * l_pPlayer = GetMember( pi_dwIndex );
	if( l_pPlayer )
	{
		_SetSystemMsg( "%s님이 파티에서 강퇴 당하였습니다.", l_pPlayer->GetName() );

		DeleteMember( pi_dwIndex );

		return TRUE;
	}
	else
	{
		_SetSystemMsg( "파티원이 강퇴 당하였습니다." );
	}

	return FALSE;
}

// 파티에서 탈퇴 요청
BOOL
CPartyBoard::LeaveFromMyParty_Request( void )
{
	if( IsPartyLeader( _GetAvatar()->GetIndex() ) )
	{
		_SetSystemMsg( "파티 리더는 파티를 탈퇴할 수 없습니다." );
		return FALSE;
	}

	m_pNetMsgProcessor->PartyLeaveSelfRequest_clzo();

	return TRUE;
}

BOOL
CPartyBoard::LeaveFromMyParty_Result( DWORD pi_dwIndex )
{
	if( _GetAvatar()->GetIndex() == pi_dwIndex )
	{
		_SetSystemMsg( "파티를 탈퇴하였습니다." );
		Clear();
		return TRUE;
	}

	CPlayer * l_pPlayer = GetMember( pi_dwIndex );
	if( l_pPlayer )
	{
		_SetSystemMsg( "%s님이 파티를 탈퇴 하였습니다.", l_pPlayer->GetName() );

		if( l_pPlayer->GetPartyMemberType() == PMT_LEADER )
			ChangeLeaderToNextMember();

		DeleteMember( pi_dwIndex );

		return TRUE;
	}
	else
	{
		_SetSystemMsg( "파티원이 탈퇴 하였습니다." );
	}

	return FALSE;
}

// 파티 잠금 요청
BOOL
CPartyBoard::LockOnMyParty_Request( BOOL pi_bIsLock )
{
	if( !IsPartyLeader( _GetAvatar()->GetIndex() ) )
	{
		_SetSystemMsg( "파티 리더만이 잠금 요청을 할 수 있습니다." );
		return FALSE;
	}

	m_pNetMsgProcessor->PartyLockRequest_clzo( pi_bIsLock );

	return TRUE;
}

BOOL
CPartyBoard::LockOnMyParty_Result( BOOL pi_bIsLock )
{
	m_bIsLockOnAddMember = pi_bIsLock;
	m_uiPartyWindow.Lock( pi_bIsLock );

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CPartyBoard::SelectMember( DWORD pi_dwMemberIndex )
{
	if( _GetGBM()->GetSelectSF() )
	{
		CPlayer * pMember = GetMember( pi_dwMemberIndex );
		if( !pMember )
			return;
		if( pMember->IsEnable( ROSF_OUT_OF_VISUAL_FIELD_CLIENT ) )
			return;

		_GetLinkBoard()->UI_UseSkillForceRequest( _GetGBM()->GetSelectSF(), pMember );
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CPartyBoard::SetMemberLevel( DWORD pi_dwIndex, BYTE pi_byLevel )
{
	CPlayer * l_pMember = GetMember( pi_dwIndex );
	if( l_pMember )
	{	
		l_pMember->SetLevel( pi_byLevel );
		m_uiPartyWindow.SetLevel( pi_dwIndex, pi_byLevel );
	}
}

void
CPartyBoard::SetMemberMaxHP( DWORD pi_dwIndex, DWORD pi_dwMaxHP )
{
	CPlayer * l_pMember = GetMember( pi_dwIndex );
	if( l_pMember )
	{	
		l_pMember->SetMaxHP( pi_dwMaxHP );

		if( ( l_pMember->GetMaxHP() == 0 ) || ( l_pMember->GetHP() == 0 ) )
			m_uiPartyWindow.SetHPGauge( pi_dwIndex, 0.0f );
		else
			m_uiPartyWindow.SetHPGauge( pi_dwIndex, ( float )l_pMember->GetHP() / ( float )l_pMember->GetMaxHP() );
	}
}

void
CPartyBoard::SetMemberHP( DWORD pi_dwIndex, DWORD pi_dwHP )
{
	CPlayer * l_pMember = GetMember( pi_dwIndex );
	if( l_pMember )
	{
		l_pMember->SetHP( pi_dwHP );

		if( ( l_pMember->GetMaxHP() == 0 ) || ( l_pMember->GetHP() == 0 ) )
		{
			m_uiPartyWindow.SetHPGauge( pi_dwIndex, 0.0f );
			ClearMemberSF( pi_dwIndex );
		}
		else
		{
			m_uiPartyWindow.SetHPGauge( pi_dwIndex, ( float )l_pMember->GetHP() / ( float )l_pMember->GetMaxHP() );
		}
	}
}

void
CPartyBoard::SetMemberMaxFP( DWORD pi_dwIndex, DWORD pi_dwMaxFP )
{
	CPlayer * l_pMember = GetMember( pi_dwIndex );
	if( l_pMember )
	{	
		l_pMember->SetMaxFP( pi_dwMaxFP );

		if( ( l_pMember->GetMaxFP() == 0 ) || ( l_pMember->GetFP() == 0 ) )
			m_uiPartyWindow.SetFPGauge( pi_dwIndex, 0.0f );
		else
			m_uiPartyWindow.SetFPGauge( pi_dwIndex, ( float )l_pMember->GetFP() / ( float )l_pMember->GetMaxFP() );
	}
}

void
CPartyBoard::SetMemberFP( DWORD pi_dwIndex, DWORD pi_dwFP )
{
	CPlayer * l_pMember = GetMember( pi_dwIndex );
	if( l_pMember )
	{	
		l_pMember->SetFP( pi_dwFP );

		if( ( l_pMember->GetMaxFP() == 0 ) || ( l_pMember->GetFP() == 0 ) )
			m_uiPartyWindow.SetFPGauge( pi_dwIndex, 0.0f );
		else
			m_uiPartyWindow.SetFPGauge( pi_dwIndex, ( float )l_pMember->GetFP() / ( float )l_pMember->GetMaxFP() );
	}
}

void
CPartyBoard::SetMemberMaxSP( DWORD pi_dwIndex, DWORD pi_dwMaxSP )
{
	CPlayer * l_pMember = GetMember( pi_dwIndex );
	if( l_pMember )
	{
		l_pMember->SetMaxSP( pi_dwMaxSP );

		if( ( l_pMember->GetMaxSP() == 0 ) || ( l_pMember->GetSP() == 0 ) )
			m_uiPartyWindow.SetSPGauge( pi_dwIndex, 0.0f );
		else
			m_uiPartyWindow.SetSPGauge( pi_dwIndex, ( float )l_pMember->GetSP() / ( float )l_pMember->GetMaxSP() );
	}
}

void
CPartyBoard::SetMemberSP( DWORD pi_dwIndex, DWORD pi_dwSP )
{
	CPlayer * l_pMember = GetMember( pi_dwIndex );
	if( l_pMember )
	{	
		l_pMember->SetSP( pi_dwSP );

		if( ( l_pMember->GetMaxSP() == 0 ) || ( l_pMember->GetSP() == 0 ) )
			m_uiPartyWindow.SetSPGauge( pi_dwIndex, 0.0f );
		else
			m_uiPartyWindow.SetSPGauge( pi_dwIndex, ( float )l_pMember->GetSP() / ( float )l_pMember->GetMaxSP() );
	}
}

void
CPartyBoard::SetMemberPosition( DWORD pi_dwIndex, BYTE pi_byMapIndex, Vector3f pi_vecMemberPosition )
{
	CPlayer * pMember = GetMember( pi_dwIndex );
	if( pMember )
	{
		pMember->SetMapIndex( pi_byMapIndex );

		if( pMember->IsEnable( ROSF_OUT_OF_VISUAL_FIELD_SERVER ) )
		{
			// pi_vecMemberPosition의 Y좌표가 0이기 때문에, Y좌표는 현재의 좌표로 세팅시켜 준다.
			pMember->SetPosition( pi_vecMemberPosition[0], pMember->GetPosY(), pi_vecMemberPosition[2], TRUE, FALSE );

			if( pi_byMapIndex == _GetLand()->GetMapIndex() )
				pMember->UI_SetRadar();

			//pMember->UI_SetPositionOfRadar();
			//pMember->UI_SetPositionOfMinimap();
		}

		if( pi_byMapIndex == _GetLand()->GetMapIndex() )
			pMember->UI_SetMinimap();
	}
}

void
CPartyBoard::SetMemberSF( DWORD pi_dwIndex, BYTE pi_bySFType, DWORD pi_dwSFIndex,
						  BYTE pi_bySFLevel, BOOL pi_bAdd )
{
	CPlayer * pMember = GetMember( pi_dwIndex );
	if( pMember )
	{
		CSFItem clsSFItem;

		if( !clsSFItem.Create( pi_bySFType, pi_dwSFIndex ) )
			return;

		clsSFItem.SetIndex( pi_dwSFIndex );
		clsSFItem.SetLevel( pi_bySFLevel );

		SetMemberSF_ApplySF( pMember, &clsSFItem, pi_bAdd );
	}
}

void
CPartyBoard::SetMemberSF_ApplySF( CPlayer * pi_pMember, CSFItem * pi_pSFItem, BOOL pi_bAdd )
{
	if( !pi_pMember || !pi_pSFItem )
		return;

	if( pi_bAdd )
	{
		DWORD dwResult = pi_pMember->AddAppliedSF( pi_pSFItem );
		if( dwResult == TRUE )
		{
			m_uiPartyWindow.AddSkillForce( pi_pMember->GetIndex(),
										   pi_pSFItem->GetSFType(), pi_pSFItem->GetContinuanceUsingType(),
										   pi_pSFItem->GetDTCode(), pi_pSFItem->GetIconID() );
		}
		else if( dwResult == TRUE + 1 )
		{
		}
	}
	else
	{
		pi_pMember->DeleteAppliedSF( pi_pSFItem );
		m_uiPartyWindow.RemoveSkillForce( pi_pMember->GetIndex(),
										  pi_pSFItem->GetSFType(), pi_pSFItem->GetContinuanceUsingType(),
										  pi_pSFItem->GetDTCode() );
	}
}

void
CPartyBoard::ClearMemberSF( DWORD pi_dwIndex )
{
	CPlayer * l_pMember = GetMember( pi_dwIndex );
	if( l_pMember )
	{
		l_pMember->ClearAppliedSF();
		m_uiPartyWindow.RemoveAllSkillForce( pi_dwIndex );
	}
}

// 아바타가 맵 이동 후, 파티 멤버의 UI를 새로 세팅해줘야 한다.
void
CPartyBoard::ResetPartyMemberAfterMoveMap( void )
{
	CPlayer * pMember;
	for( PARTY_LIST_ITER i = m_listParty.begin(); i != m_listParty.end(); ++i )
	{
		pMember = ( *i ).GetMember();
		if( pMember )
		{
			pMember->UI_SetRadar();
			pMember->UI_SetMinimap();
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CPartyBoard::OpenWindow( void )
{
	CBaseBoard::OpenWindow();

#ifdef _NEW_UI_

	if( _GetAnimusControlBoard()->IsOpen() )
	{
		RECT rtAnimusBoardRect;
		_GetAnimusControlBoard()->GetWindowRect( rtAnimusBoardRect );
		SetWindowPos( 0, rtAnimusBoardRect.bottom );
	}

#endif // _NEW_UI_
}