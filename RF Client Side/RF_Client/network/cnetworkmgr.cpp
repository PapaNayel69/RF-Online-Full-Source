////////////////////////////////////////////////////////////////////////////
//
// CNetworkMgr Class Implementation
//
////////////////////////////////////////////////////////////////////////////

#include "../GameObject/Manager/CCharacterMgr.h"
#include "../Item/CItemMgr.h"

#include "../CLand.h"
#include "../CCamera.h"

#include "../DataTable/CResourceDataMgr.h"
#include "../UserInterface/SpriteMgr.h"
#include "../GameProgress/CGameProgress.h"
#include "../MainApp.h"

#include "./NetMsgProcessor/Combat/CNetMsgProcessor_Combat.h"
#include "./NetMsgProcessor/Item/CNetMsgProcessor_Item.h"
#include "./NetMsgProcessor/Item/CNetMsgProcessor_ItemArrangement.h"
#include "./NetMsgProcessor/Party/CNetMsgProcessor_Party_Dev.h"
#include "./NetMsgProcessor/Map/CNetMsgProcessor_Map.h"
//#include "./NetMsgProcessor/Map/CNetMsgProcessor_Map_Dev.h"
#include "./NetMsgProcessor/Dungeon/CNetMsgProcessor_Dungeon.h"
//#include "./NetMsgProcessor/Dungeon/CNetMsgProcessor_Dungeon_Dev.h"
#include "./NetMsgProcessor/Summon/CNetMsgProcessor_Summon.h"
#include "./NetMsgProcessor/Unit/CNetMsgProcessor_Unit.h"
#include "./NetMsgProcessor/ItemTrade/CNetMsgProcessor_ItemTrade.h"
#include "./NetMsgProcessor/ItemTrade/CNetMsgProcessor_ItemTradePlayer.h"
#include "./NetMsgProcessor/Resource/CNetMsgProcessor_Resource.h"
#include "./NetMsgProcessor/Event/CNetMsgProcessor_Event.h"

#include "../UserInterface/GUIController/GUIMessagebox.h"

BOOL	g_bWriteTestLog			= FALSE;	// 테스트 코드
DWORD	g_bWriteTestLogCount	= 0;		// 테스트 코드

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CNetworkMgr::CNetworkMgr()
{
	Init();
}

CNetworkMgr::~CNetworkMgr()
{
	Destroy();
}

void
CNetworkMgr::Init( void )
{
	for( int i = 0; i < MAX_NET_MSG_PROCESSOR; ++i )
		m_pNetMsgProcessor[i] = NULL;

	//--------------------------------------------------------------------------

	m_byAvatarGrade			= USER_DEGREE_STD;
	m_fServerVersion		= 0.0f;

	ZeroMemory( m_dwServerIP, 4 * MAX_GAME_SERVER );
	ZeroMemory( m_dwServerPort, 4 * MAX_GAME_SERVER );
	ZeroMemory( m_pServerName, 32 * MAX_GAME_SERVER );

	m_byWorldServerNum		= 0;
	ZeroMemory( m_pWorldServerList, sizeof( WORLD_SERVER_INFO ) * MAX_WORLD_SERVER );

	m_dwAccountIndex		= ID_INVALID;
	m_dwAvatarIndex			= ID_INVALID;
	m_dwZoneServerKey		= ID_INVALID;

	ZeroMemory( m_pPassword, 32 );
	ZeroMemory( m_pID, 32 );

	//--------------------------------------------------------------------------

	m_dwLastestAvatarIndex	= ID_INVALID;
	ZeroMemory( m_vecAvatarStartPos, sizeof( float ) * 3 );

	m_bIsSendMsg			= FALSE;
	m_dwSendMsgTime			= 0;
	m_bIsRequestClose		= FALSE;

	/*
	m_bIsRequestUseSF		= FALSE;
	m_bIsRequestChangeMode	= FALSE;*/

	m_ClientLogFile.SetWriteLogFile( ".\\NetLog\\Client-Net.log", true );
}

BOOL
CNetworkMgr::Create( void )
{
	g_pNetworkMgr = this;

#ifdef _DEVELOPER_

	m_pNetMsgProcessor[NMP_COMBAT]				= dynamic_cast< CNetMsgProcessor_Base * >( new CNetMsgProcessor_Combat );
	m_pNetMsgProcessor[NMP_ITEM]				= dynamic_cast< CNetMsgProcessor_Base * >( new CNetMsgProcessor_Item );
	m_pNetMsgProcessor[NMP_ITEM_ARRANGE]		= dynamic_cast< CNetMsgProcessor_Base * >( new CNetMsgProcessor_ItemArrangement );	
	m_pNetMsgProcessor[NMP_PARTY]				= dynamic_cast< CNetMsgProcessor_Base * >( new CNetMsgProcessor_Party_Dev );
	m_pNetMsgProcessor[NMP_MAP]					= dynamic_cast< CNetMsgProcessor_Base * >( new CNetMsgProcessor_Map );
	m_pNetMsgProcessor[NMP_DUNGEON]				= dynamic_cast< CNetMsgProcessor_Base * >( new CNetMsgProcessor_Dungeon );
	m_pNetMsgProcessor[NMP_SUMMON]				= dynamic_cast< CNetMsgProcessor_Base * >( new CNetMsgProcessor_Summon );
	m_pNetMsgProcessor[NMP_UNIT]				= dynamic_cast< CNetMsgProcessor_Base * >( new CNetMsgProcessor_Unit );
	m_pNetMsgProcessor[NMP_ITEM_TRADE]			= dynamic_cast< CNetMsgProcessor_Base * >( new CNetMsgProcessor_ItemTrade );
	m_pNetMsgProcessor[NMP_ITEM_TRADE_PLAYER]	= dynamic_cast< CNetMsgProcessor_Base * >( new CNetMsgProcessor_ItemTradePlayer );
	m_pNetMsgProcessor[NMP_RESOURCE]			= dynamic_cast< CNetMsgProcessor_Base * >( new CNetMsgProcessor_Resource );	
	m_pNetMsgProcessor[NMP_EVENT]				= dynamic_cast< CNetMsgProcessor_Base * >( new CNetMsgProcessor_Event );	

#else

	m_pNetMsgProcessor[NMP_COMBAT]				= dynamic_cast< CNetMsgProcessor_Base * >( new CNetMsgProcessor_Combat );
	m_pNetMsgProcessor[NMP_ITEM]				= dynamic_cast< CNetMsgProcessor_Base * >( new CNetMsgProcessor_Item );
	m_pNetMsgProcessor[NMP_ITEM_ARRANGE]		= dynamic_cast< CNetMsgProcessor_Base * >( new CNetMsgProcessor_ItemArrangement );	
	m_pNetMsgProcessor[NMP_PARTY]				= dynamic_cast< CNetMsgProcessor_Base * >( new CNetMsgProcessor_Party );
	m_pNetMsgProcessor[NMP_MAP]					= dynamic_cast< CNetMsgProcessor_Base * >( new CNetMsgProcessor_Map );
	m_pNetMsgProcessor[NMP_DUNGEON]				= dynamic_cast< CNetMsgProcessor_Base * >( new CNetMsgProcessor_Dungeon );
	m_pNetMsgProcessor[NMP_SUMMON]				= dynamic_cast< CNetMsgProcessor_Base * >( new CNetMsgProcessor_Summon );
	m_pNetMsgProcessor[NMP_UNIT]				= dynamic_cast< CNetMsgProcessor_Base * >( new CNetMsgProcessor_Unit );
	m_pNetMsgProcessor[NMP_ITEM_TRADE]			= dynamic_cast< CNetMsgProcessor_Base * >( new CNetMsgProcessor_ItemTrade );
	m_pNetMsgProcessor[NMP_ITEM_TRADE_PLAYER]	= dynamic_cast< CNetMsgProcessor_Base * >( new CNetMsgProcessor_ItemTradePlayer );
	m_pNetMsgProcessor[NMP_RESOURCE]			= dynamic_cast< CNetMsgProcessor_Base * >( new CNetMsgProcessor_Resource );
	m_pNetMsgProcessor[NMP_EVENT]				= dynamic_cast< CNetMsgProcessor_Base * >( new CNetMsgProcessor_Event );

#endif

	//--------------------------------------------------------------------------

	_NET_TYPE_PARAM typeParam;
	typeParam.m_bSendThread					= TRUE;
	//typeParam.m_bRealClientCheck			= TRUE;
	typeParam.m_bRecvLogFile				= FALSE;
	typeParam.m_bSendLogFile				= FALSE;
	typeParam.m_bServer						= FALSE;
	typeParam.m_bSystemLogFile				= TRUE;
	typeParam.m_byProtocolID				= protocol_tcp;
	typeParam.m_byRecvThreadNum				= 1;
	typeParam.m_wSocketMaxNum				= 1;
	typeParam.m_bSpeedHackCheck 			= TRUE;
	sprintf( typeParam.m_szModuleName, "RF_CLIENT");

	SetNetSystem( 1, &typeParam, "RF_CLIENT");

	//----------------------------------------------------------------------
	
	m_byResultOfUserRegistration			= USER_REG_NOT_RECIVED;
	m_byResultOfLogin						= USER_LOGIN_NOT_RECIVED;
	m_byResultOfRequestForWorldServerList	= WORLD_LIST_NOT_RECIVED;
	m_byResultOfSelectWorldServer			= WORLD_SELECT_NOT_RECIVED;
	m_byResultOfEnterTheWorldServer			= WORLD_ENTER_NOT_RECIVED;
	m_byResultOfRequestForDisconnectAccount	= WORLD_DISCONNECT_ACCOUNT_NOT_RECIVED;
	m_byResultOfUserInfo					= USER_INFO_NOT_RECIVED;
	m_byResultOfCharacterCreate				= CHAR_CREATE_NOT_RECIVED;
	m_byResultOfCharacterSelect				= CHAR_SELECT_NOT_RECIVED;
	m_byResultOfCharacterDelete				= CHAR_DELETE_NOT_RECIVED;
	m_byResultOfMoveOutFromServer			= MOVEOUT_FROM_SERVER_NOT_RECIVED;
	m_byResultOfMoveIntoServer				= MOVEINTO_SERVER_NOT_RECIVED;
	m_byResultOfInventoryUpload				= INVEN_UPLOAD_NOT_RECIVED;

	//----------------------------------------------------------------------

	// etc
	for( int i = 0; i < MAX_GAME_SERVER; ++i )
		m_bIsConnectedToServer[i] = FALSE;

	m_nActiveServerID			= GST_NONE;

	m_ClientLogFile.SetWriteAble( true );

	return TRUE;
}

BOOL
CNetworkMgr::Destroy( void )
{
	for( int i = 0; i < MAX_NET_MSG_PROCESSOR; ++i )
		_DELETE( m_pNetMsgProcessor[i] );

	return TRUE;
}

// 실제 공간은 32지만, 내용은 max_id_len로 받는다.
void
CNetworkMgr::SetID( char * pi_pID )
{
	_strcpy( m_pID, pi_pID, max_id_len + 1 );
}

// 실제 공간은 32지만, 내용은 max_pw_len로 받는다.
void
CNetworkMgr::SetPassword( char * pi_pPassword )
{
	_strcpy( m_pPassword, pi_pPassword, max_pw_len + 1 );
}

////////////////////////////////////////////////////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////        Boundary       /////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////////////////////////////////////////////////////////

void
CNetworkMgr::FrameMove( void )
{
	const DWORD REQUEST_LIMIT_TIME = 10000;

	// 일정 시간이 지나면 false 로 세팅 해 준다.
	if( ( timeGetTime() - m_dwSendMsgTime ) > REQUEST_LIMIT_TIME )
		ReceiveResponseMsg();
}

// pi_byServerType => 0 : Master Server, 1 : Zone Server
void
CNetworkMgr::Connect( BYTE pi_byServerType )
{
	if( m_nActiveServerID != ID_INVALID && m_bIsConnectedToServer[m_nActiveServerID] )
		return;

	if( ( pi_byServerType == GST_ZONE ) || _GetMainApp()->IsRequestLogout() )
	{
		m_nActiveServerID = pi_byServerType;
		AcceptClientCheck( 0, 0, 0 );
	}
	else
	{
		BOOL l_nResult = CNetWorking::Connect( 0, NST_GAME_SERVER,
											   GetServerIP( pi_byServerType ),
											   GetServerPort( pi_byServerType ) );
		if( l_nResult )
		{
			m_nActiveServerID = pi_byServerType;
		}
		else
		{
			m_ClientLogFile.Write( "Connect To Server(Type:%d) Failure : %d", pi_byServerType, l_nResult );
			m_bIsConnectedToServer[pi_byServerType] = FALSE;
		}
	}

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "ConnectToMasterServer ( %d )", ++g_bWriteTestLogCount ); // 테스트 코드
}

// 소켓 연결 처리 함수
void
CNetworkMgr::AcceptClientCheck( DWORD dwProID, DWORD dwClientIndex, DWORD dwSerial )
{
	m_bIsConnectedToServer[m_nActiveServerID] = TRUE;

	if( m_nActiveServerID == GST_ZONE )
	{
		//m_bIsConnectedToServer[GST_ZONE] = FALSE;
		SystemMsg_MoveinUserRequest_zone();

#ifdef _DEVELOPER_
		SecureMsg_ResponFreeRequest();
#endif
	}
}

// 소켓 종료
void
CNetworkMgr::Close( BYTE pi_byServerType )
{
	if( m_nActiveServerID != ID_INVALID && !m_bIsConnectedToServer[m_nActiveServerID] )
		return;

	if( ( pi_byServerType == GST_WORLD ) || _GetMainApp()->IsRequestLogout() )
	{
		m_bIsRequestClose = TRUE;
		CloseClientCheck( 0, 0, 0 );
	}
	else
	{
		m_bIsRequestClose = TRUE;
		CNetWorking::Close( 0, NST_GAME_SERVER );
	}
}

// 소켓 종료 처리 함수
void
CNetworkMgr::CloseClientCheck( DWORD dwProID, DWORD dwClientIndex, DWORD dwSerial )
{
	if( !m_bIsRequestClose )
	{
		_GUIMessageBox( NULL, "서버와의 연결이 끊어졌습니다. 프로그램을 종료합니다.", 0xbcffffff,
						MSGBOX_ID_DISCONNECT_SERVER, DLG_STYLE_OK );
	}

	m_bIsConnectedToServer[m_nActiveServerID] = FALSE;
	m_nActiveServerID = GST_NONE;

	m_bIsRequestClose = FALSE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 클라이언트 종료 요청
void
CNetworkMgr::RequestQuitProgram( void )
{
	_GetMainApp()->RequestQuitProgram();
}

// 서버에 요청 메세지를 보냈음
void
CNetworkMgr::SendRequestMsg( void )
{
	m_bIsSendMsg	= TRUE;
	m_dwSendMsgTime	= timeGetTime();

	if( m_nActiveServerID != GST_ZONE )
	{
		if( !_IsOpenedDialogBox( NULL, MSGBOX_ID_GAMEPROGRESS_LOGIN ) )
			_GUIMessageBox( NULL, "서버의 응답을 대기중입니다. 잠시만 기다려 주십시오.",
							0xb0ffffff, MSGBOX_ID_GAMEPROGRESS_LOGIN, DLG_STYLE_NONE );
	}
}

// 요청 메세지에 대한 응답이 도착했음
void
CNetworkMgr::ReceiveResponseMsg( void )
{
	m_bIsSendMsg	= FALSE;
	m_dwSendMsgTime	= 0;

	if( m_nActiveServerID != GST_ZONE )
	{
		if( _IsOpenedDialogBox( NULL, MSGBOX_ID_GAMEPROGRESS_LOGIN ) )
			_CloseDialogBox( NULL, MSGBOX_ID_GAMEPROGRESS_LOGIN );
	}
}

// 서버에 요청 메세지를 보낸후, 한계 시간이 지났는지를 체크
BOOL
CNetworkMgr::IsPassed_RequestLimitTime( void )
{
	const DWORD REQUEST_LIMIT_TIME = 40000;		// 서버 제한 => 최소 30 초

	if( ( timeGetTime() - m_dwSendMsgTime ) > REQUEST_LIMIT_TIME )
		return TRUE;

	return FALSE;
}

BOOL
CNetworkMgr::SendNetMessage( DWORD pi_dwClientIndex, BYTE * pi_pType, void * pi_pMsg, int pi_nSize )
{
	BOOL l_bResult = m_pProcess[0]->LoadSendMsg( pi_dwClientIndex, pi_pType,
												 static_cast< char * >( pi_pMsg ), pi_nSize );
	if( !l_bResult )
		NetLogWrite( "CNetworkMgr::SendNetMessage : Fail ( Type : %d, Index : %d )", pi_pType[0], pi_pType[1] );

	return l_bResult;
}

void
CNetworkMgr::NetLogWrite( const char * pi_pFormat, ... )
{
	static char l_pNetLogWirteBuffer[1024];

	va_list l_pArgumentList;

	va_start( l_pArgumentList, pi_pFormat );
	vsprintf( l_pNetLogWirteBuffer, pi_pFormat, l_pArgumentList );
	va_end( l_pArgumentList );

	m_ClientLogFile.Write( l_pNetLogWirteBuffer );
}

CNetMsgProcessor_Base *
CNetworkMgr::GetNetMsgProcessor( NET_MSG_PROCESSOR pi_nmtType )
{
	if( pi_nmtType >= MAX_NET_MSG_PROCESSOR )
		return NULL;

	return m_pNetMsgProcessor[pi_nmtType];
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 서버로부터 넘어온 패킷을 분석한다.
// pi_dwProID => 클라이언트에서는 프로세서가 하나만 존재하므로, 프로세서 ID는 사용할 필요가 없음
bool
CNetworkMgr::DataAnalysis( DWORD pi_dwProID, DWORD pi_dwClientIndex, _MSG_HEADER* pMsgHeader, char* pMsg )
{
	// 클라이언트 인덱스 ID 에 따라서 알맞는 함수 호출
	switch( pi_dwClientIndex )
	{
		// Game Server
		case NST_GAME_SERVER :
			return AnalysisServerPacket( pMsgHeader, pMsg );
			
		// local chat server
		case NST_LOCAL_CHAT :
			return AnalysisLocalChatServerPacket( pMsgHeader, pMsg );
	}

	return false;
}

bool
CNetworkMgr::AnalysisServerPacket( _MSG_HEADER * pMsgHeader, char* pi_pMsg )
{
	//m_ClientLogFile.Write( "( %d ) AnalysisServerPacket", ++g_bWriteTestLogCount ); // 테스트 코드

	switch( pMsgHeader->m_byType[0] )
	{
		// 어카운트 메세지
		case account_msg :
			switch( pMsgHeader->m_byType[1] )
			{
				case join_account_result_locl :
					AccountMsg_JoinAccountResult_locl( pi_pMsg );
					break;

				case login_account_result_locl :
					AccountMsg_LoginAccountResult_locl( pi_pMsg );
					break;

				case world_list_result_locl :
					AccountMsg_WorldListResult_locl( pi_pMsg );
					break;

				case select_world_result_locl :
					AccountMsg_SelectWorldResult_locl( pi_pMsg );
					break;

				case push_close_result_locl :
					AccountMsg_PushCloseResult_locl( pi_pMsg );
					break;

				case server_notify_inform_locl :
					AccountMsg_ServerNotifyInform_locl( pi_pMsg );
					break;

				default :
					m_ClientLogFile.Write( "Client Error - CNetworkMgr::AnalysisZoneServerPacket() - account_msg - Unknown Message\n" );
					return false;
			}
			break;

		// 시스템 메세지
		case system_msg :
			switch( pMsgHeader->m_byType[1] )
			{
				case enter_world_result_zone :
					SystemMsg_EnterWorldResult_zone( pi_pMsg );
					break;

				case movein_user_result_zone :
					SystemMsg_MoveinUserResult_zone( pi_pMsg );
					break;

				case moveout_user_result_zone :
					SystemMsg_MoveoutUserResult_zone( pi_pMsg );
					break;

				case reged_char_result_zone :
					SystemMsg_RegedCharResult_zone( pi_pMsg );
					break;

				case add_char_result_zone :
					SystemMsg_AddCharResult_zone( pi_pMsg );
					break;

				case del_char_result_zone :
					SystemMsg_DelCharResult_zone( pi_pMsg );
					break;

				case sel_char_result_zone :
					SystemMsg_SelCharResult_zone( pi_pMsg );
					break;

				case server_notify_inform_zone :
					SystemMsg_ServerNotifyInform_zone( pi_pMsg );
					break;

				default :
					m_ClientLogFile.Write( "Client Error - CNetworkMgr::AnalysisZoneServerPacket() - system_msg - Unknown Message\n" );
					return false;
			}
			break;

		////////////////////////////////////////////////////////////////////////
		// 임시코드

		case chat_msg :
			/*
			switch( pMsgHeader->m_byType[1] )
			{
				//case chat_message_receipt_zone :
				//	ChatMsg_ChatMessageReceipt_zone( pi_pMsg );
				//	break;

				case chat_far_failure_zocl :
					ChatMsg_ChatFarFailure_zocl( pi_pMsg );
					break;

				default :
					m_ClientLogFile.Write( "Client Error - CNetworkMgr::AnalysisZoneServerPacket() - chat_msg - Unknown Message\n" );
					return false;
			}
			break;
			*/
			// udp - begin
			switch( pMsgHeader->m_byType[1] )
			{
				case chat_far_failure_zocl :
					ChatMsg_ChatFarFailure_zocl( pi_pMsg );
					break;

				case chat_message_receipt_udp :
					ChatMsg_ChatMessageReceipt_udp( pi_pMsg );
					break;

				case announ_message_receipt_udp :
					ChatMsg_AnnounMessageReceipt_udp( pi_pMsg );
					break;

				default :
					m_ClientLogFile.Write( "Client Error - CNetworkMgr::AnalysisLocalChatServerData() - chat_msg (udp) - Unknown Message\n" );
					return false;
			}
			break;
			// udp - end

		// 임시코드
		////////////////////////////////////////////////////////////////////////

		// 초기화 메세지
		case init_msg :
			switch( pMsgHeader->m_byType[1] )
			{
				case new_pos_start_result_zocl :
					InitMsg_NewPosStartResult_zocl( pi_pMsg );
					break;

				case base_download_result_zocl :
					InitMsg_BaseDownloadResult_zocl( pi_pMsg );
					break;

				case inven_download_result_zocl :
					InitMsg_InvenDownloadResult_zocl( pi_pMsg );
					break;

				case exit_save_data_result_zocl :
					InitMsg_ExitSaveDataResult_zocl( pi_pMsg );
					break;

				case stat_inform_zocl :
					InitMsg_StatInform_zocl( pi_pMsg );
					break;

				case fcitem_inform_zocl :
					InitMsg_FcitemInform_zocl( pi_pMsg );
					break;

				case cum_download_result_zocl :
					InitMsg_CumDownloadResult_zocl( pi_pMsg );
					break;

				case force_download_result_zocl :
					InitMsg_ForceDownloadResult_zocl( pi_pMsg );
					break;

				case unit_download_result_zocl :
					InitMsg_UnitDownloadResult_zocl( pi_pMsg );
					break;

				case animus_download_result_zocl :
					InitMsg_AnimusDownloadResult_zocl( pi_pMsg );
					break;

				case monster_create_zocl :
					InitMsg_MonsterCreate_zocl( pi_pMsg );
					break;

				case recallmon_create_zocl :
					InitMsg_RecallMonCreate_zocl( pi_pMsg );
					break;

				case animus_create_zocl :
					InitMsg_AnimusCreate_zocl( pi_pMsg );
					break;

				case tower_create_zocl :
					InitMsg_TowerCreate_zocl( pi_pMsg );
					break;

				case itembox_create_zocl :
					InitMsg_ItemboxCreate_zocl( pi_pMsg );
					break;

				case parkingunit_create_zocl :
					InitMsg_ParkingUnitCreate_zocl( pi_pMsg );
					break;

				case player_destroy_zocl :
					InitMsg_PlayerDestroy_zocl( pi_pMsg );
					break;

				case player_die_zocl :
					InitMsg_PlayerDie_zocl( pi_pMsg );
					break;

				case monster_destroy_zocl :
					InitMsg_MonsterDestroy_zocl( pi_pMsg );
					break;

				case animus_destroy_zocl :
					InitMsg_AnimusDestroy_zocl( pi_pMsg );
					break;

				case tower_destroy_zocl :
					InitMsg_TowerDestroy_zocl( pi_pMsg );
					break;

				case itembox_destroy_zocl :
					InitMsg_ItemboxDestroy_zocl( pi_pMsg );
					break;

				case parkingunit_destroy_zocl :
					InitMsg_ParkingUnitDestroy_zocl( pi_pMsg );
					break;

				case other_shape_all_zocl :
					InitMsg_OtherShapeAll_zocl( pi_pMsg );
					break;

				case other_shape_part_zocl :
					InitMsg_OtherShapePart_zocl( pi_pMsg );
					break;

				case other_shape_failure_zocl :
					InitMsg_OtherShapeFailure_zocl( pi_pMsg );
					break;

				case other_shape_change_zocl :
					InitMsg_OtherShapeChange_zocl( pi_pMsg );
					break;

				case other_unit_ride_change_zocl :
					InitMsg_OtherUnitRideChange_zocl( pi_pMsg );
					break;

				case other_new_view_zocl :
					InitMsg_OtherNewView_zocl( pi_pMsg );
					break;

				case player_revival_zocl :
					InitMsg_PlayerRevival_zocl( pi_pMsg );
					break;

				case other_map_out_zocl :
					InitMsg_OtherMapOut_zocl( pi_pMsg );
					break;

				case player_resurrect_zocl :
					InitMsg_PlayerResurrect_zocl( pi_pMsg );
					break;

				default :
					m_ClientLogFile.Write( "Client Error - CNetworkMgr::AnalysisZoneServerPacket() - init_msg - Unknown Message\n" );
					return false;
			}
			break;

		// 포지션 메세지
		case position_msg :
			switch( pMsgHeader->m_byType[1] )
			{
				case move_error_result_zocl :
					PositionMsg_MoveErrorResult_zocl( pi_pMsg );
					break;

				case player_move_zocl :
					PositionMsg_PlayerMove_zocl( pi_pMsg );
					break;

				case monster_move_zocl :
					PositionMsg_MonsterMove_zocl( pi_pMsg );
					break;

				case animus_move_zocl :
					PositionMsg_AnimusMove_zocl( pi_pMsg );
					break;

				case monster_chase_zocl :
					PositionMsg_MonsterChase_zocl( pi_pMsg );
					break;

				case player_fixpositon_zocl :
					PositionMsg_PlayerFixPosition_zocl( pi_pMsg );
					break;

				case monster_fixpositon_zocl :
					PositionMsg_MonsterFixPosition_zocl( pi_pMsg );
					break;

				case animus_fixpositon_zocl :
					PositionMsg_AnimusFixpositon_zocl( pi_pMsg );
					break;

				case tower_fixpositon_zocl :
					PositionMsg_TowerFixpositon_zocl( pi_pMsg );
					break;

				case itembox_fixpositon_zocl :
					PositionMsg_ItemboxFixPosition_zocl( pi_pMsg );
					break;

				case parkingunit_fixpositon_zocl :
					PositionMsg_ParkingunitFixpositon_zocl( pi_pMsg );
					break;

				case correct_position_zocl :
					break;

				case monster_stop_zocl :
					PositionMsg_MonsterStop_zocl( pi_pMsg );
					break;

				case player_stop_zocl :
					PositionMsg_PlayerStop_zocl( pi_pMsg );
					break;

				case player_real_move_zocl :
					PositionMsg_PlayerRealMove_zocl( pi_pMsg );
					break;

				case monster_real_move_zocl :
					PositionMsg_MonsterRealMove_zocl( pi_pMsg );
					break;

				case animus_real_move_zocl :
					PositionMsg_AnimusRealMove_zocl( pi_pMsg );
					break;

				case state_inform_zocl :
					PositionMsg_StateInform_zocl( pi_pMsg );
					break;

				case goto_baseportal_result_zocl :
					PositionMsg_GotoBaseportalResult_zocl( pi_pMsg );
					break;

				case goto_avator_result_zocl :
					PositionMsg_GotoAvatorResult_zocl( pi_pMsg );
					break;

				case break_stop_result_zocl :
					PositionMsg_BreakStopResult_zocl( pi_pMsg );
					break;

				case object_real_fixpositon_zocl :
					PositionMsg_ObjectRealFixpositon_zocl( pi_pMsg );
					break;

				default :
					m_ClientLogFile.Write( "Client Error - CNetworkMgr::AnalysisZoneServerPacket() - position_msg - Unknown Message\n" );
					return false;
			}
			break;

		// 파이트 메세지
		case fight_msg :
			if( m_pNetMsgProcessor[NMP_COMBAT] )
				return m_pNetMsgProcessor[NMP_COMBAT]->AnalysisServerPacket( pMsgHeader, pi_pMsg );
			break;

		// 아이템 메세지
		case item_msg :
#ifdef _NEW_UI_

			if( m_pNetMsgProcessor[NMP_ITEM] )
				return m_pNetMsgProcessor[NMP_ITEM]->AnalysisServerPacket( pMsgHeader, pi_pMsg );
			break;
#endif
			switch( pMsgHeader->m_byType[1] )
			{
				case itembox_state_change_zocl :
					ItemMsg_ItemboxStateChange_zocl( pi_pMsg );
					break;

				case itembox_take_new_result_zocl :
					ItemMsg_ItemboxTakeNewResult_zocl( pi_pMsg );
					break;

				case itembox_take_add_result_zocl :
					ItemMsg_ItemboxTakeAddResult_zocl( pi_pMsg );
					break;

				case throw_storage_result_zocl :
					ItemMsg_ThrowStorageResult_zocl( pi_pMsg );
					break;

				case use_potion_result_zocl :
					ItemMsg_UsePotionResult_zocl( pi_pMsg );
					break;

				case equip_part_result_zocl :
					ItemMsg_EquipPartResult_zocl( pi_pMsg );
					break;

				case embellish_result_zocl :
					ItemMsg_EmbellishResult_zocl( pi_pMsg );
					break;

				case off_part_result_zocl :
					ItemMsg_OffPartResult_zocl( pi_pMsg );
					break;

				case make_item_result_zocl :
					ItemMsg_MakeItemResult_zocl( pi_pMsg );
					break;

				case upgrade_item_result_zocl :
					ItemMsg_UpgradeItemResult_zocl( pi_pMsg );
					break;

				case downgrade_item_result_zocl :
					ItemMsg_DowngradeItemResult_zocl( pi_pMsg );
					break;

				case add_bag_result_zocl :
					ItemMsg_AddBagResult_zocl( pi_pMsg );
					break;

				case delete_storage_inform_zocl :
					ItemMsg_DeleteStorageInform_zocl( pi_pMsg );
					break;

				case storage_refresh_inform_zocl :
					ItemMsg_StorageRefreshInform_zocl( pi_pMsg );
					break;

				default :
					m_ClientLogFile.Write( "Client Error - CNetworkMgr::AnalysisZoneServerPacket() - item_msg - Unknown Message\n" );
					return false;
			}
			break;

		// 맵 메세지
		case map_msg :
			if( m_pNetMsgProcessor[NMP_MAP] )
				return m_pNetMsgProcessor[NMP_MAP]->AnalysisServerPacket( pMsgHeader, pi_pMsg );
			break;

		// 던젼 메세지
		case dungeon_msg :
			if( m_pNetMsgProcessor[NMP_DUNGEON] )
				return m_pNetMsgProcessor[NMP_DUNGEON]->AnalysisServerPacket( pMsgHeader, pi_pMsg );
			break;

		// 옵져버 메세지
		case observer_msg :
			switch( pMsgHeader->m_byType[1] )
			{
				case observer_move_result_zocl :
					ObserverMsg_MoveResult_zocl( pi_pMsg );
					break;

				default :
					m_ClientLogFile.Write( "Client Error - CNetworkMgr::AnalysisZoneServerPacket() - observer_msg - Unknown Message\n" );
					return false;
			}
			break;

		// 체크 메세지
		case check_msg :
			return true;

		// 이벤트 메세지
		case event_msg :
			if( m_pNetMsgProcessor[NMP_EVENT] )
				return m_pNetMsgProcessor[NMP_EVENT]->AnalysisServerPacket( pMsgHeader, pi_pMsg );
			break;

		// 아이템 거래 메세지
		case trade_msg :

			if( m_pNetMsgProcessor[NMP_ITEM_TRADE] )
				return m_pNetMsgProcessor[NMP_ITEM_TRADE]->AnalysisServerPacket( pMsgHeader, pi_pMsg );
			break;

/*
			switch( pMsgHeader->m_byType[1] )
			{
				//case used_storage_result_zocl :
				//	ItemTradeMsg_UsedStorageResult_zocl( pi_pMsg );
				//	break;

				case buy_store_success_zocl :
					ItemTradeMsg_BuyStoreSuccess_zocl( pi_pMsg );
					break;

				case buy_store_failure_zocl :
					ItemTradeMsg_BuyStoreFailure_zocl( pi_pMsg );
					break;

				case sell_store_result_zocl :
					ItemTradeMsg_SellStoreResult_zocl( pi_pMsg );
					break;

				case repair_result_zocl :
					ItemTradeMsg_RepairResult_zocl( pi_pMsg );
					break;

				case store_list_result_zocl :
					ItemTradeMsg_StoreListResult_zocl( pi_pMsg );
					break;

				case exchange_money_result_zocl :
					ItemTradeMsg_ExchangeMoneyResult_zocl( pi_pMsg );
					break;

				default :
					m_ClientLogFile.Write( "Client Error - CNetworkMgr::AnalysisZoneServerPacket() - trade_msg - Unknown Message\n" );
					return false;
			}
			break;
*/

		// UI 메세지
		case ui_msg :
#ifdef _NEW_UI_

			if( m_pNetMsgProcessor[NMP_ITEM_ARRANGE] )
				return m_pNetMsgProcessor[NMP_ITEM_ARRANGE]->AnalysisServerPacket( pMsgHeader, pi_pMsg );
			break;
#else
			switch( pMsgHeader->m_byType[1] )
			{
				case sf_link_change_result_zocl :
					UIMsg_SFLinkChangeResult_zocl( pi_pMsg );
					break;

				case force_inven_change_result_zocl :
					UIMsg_ForceInvenChangeResult_zocl( pi_pMsg );
					break;

				case res_separation_result_zocl :
					UIMsg_ResSeparationResult_zocl( pi_pMsg );
					break;

				case res_division_result_zocl :
					UIMsg_ResDivisionResult_zocl( pi_pMsg );
					break;

				case potionsocket_change_result_zocl :
					UIMsg_PotionsocketChangeResult_zocl( pi_pMsg );
					break;

				case potionsocket_separation_result_zocl :
					UIMsg_PotionsocketSeparationResult_zocl( pi_pMsg );
					break;

				case potionsocket_division_result_zocl :
					UIMsg_PotionsocketDivisionResult_zocl( pi_pMsg );
					break;

				/*case unit_get_on_result_zocl :
					UIMsg_UnitGetOnResult_zocl( pi_pMsg );
					break;

				case unit_get_on_other_zocl :
					UIMsg_UnitGetOnOther_zocl( pi_pMsg );
					break;

				case unit_get_off_result_zocl :
					UIMsg_UnitGetOffResult_zocl( pi_pMsg );
					break;

				case unit_get_off_other_zocl :
					UIMsg_UnitGetOffOther_zocl( pi_pMsg );
					break;*/

				default :
					m_ClientLogFile.Write( "Client Error - CNetworkMgr::AnalysisZoneServerPacket() - ui_msg - Unknown Message\n" );
					return false;
			}
			break;
#endif

		// 리소스 메세지
		case resource_msg :

			if( m_pNetMsgProcessor[NMP_RESOURCE] )
				return m_pNetMsgProcessor[NMP_RESOURCE]->AnalysisServerPacket( pMsgHeader, pi_pMsg );
			break;

/*
			switch( pMsgHeader->m_byType[1] )
			{
				case mine_start_result_zocl :
					ResMsg_MineStartResult_zocl( pi_pMsg );
					break;

				case mine_cancle_result_zocl :
					ResMsg_MineCancleResult_zocl( pi_pMsg );
					break;

				case mine_complete_result_zocl :
					ResMsg_MineCompleteResult_zocl( pi_pMsg );
					break;

				case mine_start_result_other_zocl :
					ResMsg_MineStartResultOther_zocl( pi_pMsg );
					break;

				case mine_end_result_other_zocl :
					ResMsg_MineEndResultOther_zocl( pi_pMsg );
					break;

				case ore_cutting_result_zocl :
					ResMsg_OreCuttingResult_zocl( pi_pMsg );
					break;

				case ore_into_bag_result_zocl :
					ResMsg_OreIntoBagResult_zocl( pi_pMsg );
					break;

				case cutting_complete_result_zocl :
					ResMsg_CuttingCompleteResult_zocl( pi_pMsg );
					break;

				default :
					m_ClientLogFile.Write( "Client Error - CNetworkMgr::AnalysisZoneServerPacket() - resource_msg - Unknown Message\n" );
					return false;
			}
			break;
*/

		// 파티 메세지
		case party_msg :
			//if( m_pProcessNetworkMsg_Party )
			//	return m_pProcessNetworkMsg_Party->AnalysisServerPacket( pMsgHeader, pi_pMsg );
			if( m_pNetMsgProcessor[NMP_PARTY] )
				return m_pNetMsgProcessor[NMP_PARTY]->AnalysisServerPacket( pMsgHeader, pi_pMsg );
			/*
			switch( pMsgHeader->m_byType[1] )
			{
				case party_join_invitation_question_zocl :
					PartyMsg_PartyJoinInvitationQuestion_zocl( pi_pMsg );
					break;

				case party_join_application_question_zocl :
					PartyMsg_PartyJoinApplicationQuestion_zocl( pi_pMsg );
					break;

				case party_join_joiner_result_zocl :
					PartyMsg_PartyJoinJoinerResult_zocl( pi_pMsg );
					break;

				case party_join_member_result_zocl :
					PartyMsg_PartyJoinMemberResult_zocl( pi_pMsg );
					break;

				case party_leave_self_result_zocl :
					PartyMsg_PartyLeaveSelfResult_zocl( pi_pMsg );
					break;

				case party_leave_compulsion_result_zocl :
					PartyMsg_PartyLeaveCompulsionResult_zocl( pi_pMsg );
					break;

				case party_disjoint_result_zocl :
					PartyMsg_PartyDisjointResult_zocl( pi_pMsg );
					break;

				case party_succession_result_zocl :
					PartyMsg_PartySuccessionResult_zocl( pi_pMsg );
					break;

				case party_lock_result_zocl :
					PartyMsg_PartyLockResult_zocl( pi_pMsg );
					break;

				case party_exp_division_zocl :
					PartyMsg_PartyExpDivision_zocl( pi_pMsg );
					break;

				// udp - begin
				case party_member_info_upd :
					PartyMsg_PartyMemberInfo_upd( pi_pMsg );
					break;

				case party_member_hp_upd :
					PartyMsg_PartyMemberHp_upd( pi_pMsg );
					break;

				case party_member_fp_upd :
					PartyMsg_PartyMemberFp_upd( pi_pMsg );
					break;

				case party_member_sp_upd :
					PartyMsg_PartyMemberSp_upd( pi_pMsg );
					break;

				case party_member_lv_upd :
					PartyMsg_PartyMemberLv_upd( pi_pMsg );
					break;

				case party_member_pos_upd :
					PartyMsg_PartyMemberPos_upd( pi_pMsg );
					break;

				case party_member_max_hfsp_upd :
					PartyMsg_PartyMemberMaxHfsp_upd( pi_pMsg );
					break;

				case party_member_effect_upd :
					PartyMsg_PartyMemberEffect_upd( pi_pMsg );
					break;
				// udp - end

				default :
					m_ClientLogFile.Write( "Client Error - CNetworkMgr::AnalysisZoneServerPacket() - party_msg - Unknown Message\n" );
					return false;
			}*/
			break;

		// 이펙트 메세지
		case effect_msg :
			switch( pMsgHeader->m_byType[1] )
			{
				case force_result_one_zocl :
					EffectMsg_ForceResultOne_zocl( pi_pMsg );
					break;

				case force_result_other_zocl :
					EffectMsg_ForceResultOther_zocl( pi_pMsg );
					break;

				case skill_result_one_zocl :
					EffectMsg_SkillResultOne_zocl( pi_pMsg );
					break;

				case skill_result_other_zocl :
					EffectMsg_SkillResultOther_zocl( pi_pMsg );
					break;

				case class_skill_result_one_zocl :
					EffectMsg_ClassSkillResultOne_zocl( pi_pMsg );
					break;

				case class_skill_result_other_zocl :
					EffectMsg_ClassSkillResultOther_zocl( pi_pMsg );
					break;

				case effect_add_inform_zocl :
					EffectMsg_EffectAddInform_zocl( pi_pMsg );
					break;

				case effect_remove_inform_zocl :
					EffectMsg_EffectRemoveInform_zocl( pi_pMsg );
					break;

				case set_fp_inform_zocl :
					EffectMsg_SetFpInform_zocl( pi_pMsg );
					break;

				case set_hp_inform_zocl :
					EffectMsg_SetHpInform_zocl( pi_pMsg );
					break;

				case set_sp_inform_zocl :
					EffectMsg_SetSpInform_zocl( pi_pMsg );
					break;

				case insert_item_inform_zocl :
					EffectMsg_InsertItemInform_zocl( pi_pMsg );
					break;

				case stun_inform_zocl :
					EffectMsg_StunInform_zocl( pi_pMsg );
					break;

				case last_effect_change_inform_zocl :
					EffectMsg_LastEffectChangeInform_zocl( pi_pMsg );
					break;

				case make_tower_result_zocl :
					EffectMsg_MakeTowerResult_zocl( pi_pMsg );
					break;

				case back_tower_result_zocl :
					EffectMsg_BackTowerResult_zocl( pi_pMsg );
					break;

				case alter_tower_hp_zocl :
					EffectMsg_AlterTowerHp_zocl( pi_pMsg );
					break;

				case tower_complete_inform_zocl :
					EffectMsg_TowerCompleteInform_zocl( pi_pMsg );
					break;

				case alter_cont_effect_time_zocl :
					EffectMsg_AlterContEffectTime_zocl( pi_pMsg );
					break;

				default :
					m_ClientLogFile.Write( "Client Error - CNetworkMgr::AnalysisZoneServerPacket() - effect_msg - Unknown Message\n" );
					return false;
			}
			break;

		// 아이템 트레이드 메세지 ( 플레이어 )
		case dtrade_msg :
			if( m_pNetMsgProcessor[NMP_ITEM_TRADE_PLAYER] )
				return m_pNetMsgProcessor[NMP_ITEM_TRADE_PLAYER]->AnalysisServerPacket( pMsgHeader, pi_pMsg );
			break;

		// 소환 메세지
		case recall_msg :
			if( m_pNetMsgProcessor[NMP_SUMMON] )
				return m_pNetMsgProcessor[NMP_SUMMON]->AnalysisServerPacket( pMsgHeader, pi_pMsg );
			break;

		// 유닛 메세지
		case unit_msg :
			if( m_pNetMsgProcessor[NMP_UNIT] )
				return m_pNetMsgProcessor[NMP_UNIT]->AnalysisServerPacket( pMsgHeader, pi_pMsg );
			break;

		// Sync
		case sync_msg :
			switch( pMsgHeader->m_byType[1] )
			{
				case object_server_pos_result_zocl :
					SyncMsg_ObjectServerPosResult_zocl( pi_pMsg );
					break;

				case adjust_amount_inform_zocl :
					SyncMsg_AdjustAmountInform_zocl( pi_pMsg );
					break;

				default :
					m_ClientLogFile.Write( "Client Error - CNetworkMgr::AnalysisZoneServerPacket() - sync_msg - Unknown Message\n" );
					return false;
			}
			break;

		// 에러시는 로그를 남긴다.
		default :
			m_ClientLogFile.Write( "Client Error - CNetworkMgr::AnalysisZoneServerPacket() - Unknown Message\n" );
			return false;
		}

	return true;
}

bool
CNetworkMgr::AnalysisLocalChatServerPacket( _MSG_HEADER* pMsgHeader, char* pMsg )
{
	switch( pMsgHeader->m_byType[0] )
	{
		// 챗 메세지
		case chat_msg :
			/*
			switch( pMsgHeader->m_byType[1] )
			{
				//case chat_message_receipt_zone :
				//	ChatMsg_ChatMessageReceipt_zone( pi_pMsg );

				default :
					m_ClientLogFile.Write( "Client Error - CNetworkMgr::AnalysisLocalChatServerData() - chat_msg - Unknown Message\n" );
					return false;
			}*/
			break;

		// 에러시는 로그를 남긴다.
		default :
			m_ClientLogFile.Write( "Client Error - CNetworkMgr::AnalysisLocalChatServerData() - Unknown Message\n" );
			return false;
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////        Boundary       /////////////////////////////
////////////////////////                       /////////////////////////////
////////////////////////////////////////////////////////////////////////////

/*
CUDPNetworkMgr::CUDPNetworkMgr()
{
	Init();
}

CUDPNetworkMgr::~CUDPNetworkMgr()
{
	Destroy();
}

void
CUDPNetworkMgr::Init( void )
{
	m_ClientLogFile.SetWriteLogFile( ".\\NetLog\\Client-Net.log", true );
}

BOOL
CUDPNetworkMgr::Create( void )
{
	_UDP_PROCESS_TYPE UDPType;

	UDPType.bBind	= true;
	UDPType.wPort	= PORT_UDP_PARTY;
	sprintf( UDPType.szModuleName, "ServerUDP_Party");

	// 연결에 실패하면 애플리케이션을 종료한다.
	if( !SetNetSystem( &UDPType ) )
		PostMessage( _GetHWND(), WM_CLOSE, 0, 0 );

	return TRUE;
}

BOOL
CUDPNetworkMgr::Destroy ( void )
{
	return TRUE;
}

bool
CUDPNetworkMgr::DataAnalysis( UDPMsg * pi_pMsg, sockaddr_in * pi_pOutFrom )
{
	switch( pi_pMsg->m_byType[0] )
	{
		// 파티 메세지
		case party_msg :
			switch( pi_pMsg->m_byType[1] )
			{
				case party_member_info_upd :
					PartyMsg_PartyMemberInfo_upd( pi_pMsg->m_szMsg );
					break;

				//case party_member_chat_upd :
				//	PartyMsg_PartyMemberChat_upd( pi_pMsg->m_szMsg );
				//	break;

				case party_member_hp_upd :
					PartyMsg_PartyMemberHp_upd( pi_pMsg->m_szMsg );
					break;

				case party_member_fp_upd :
					PartyMsg_PartyMemberFp_upd( pi_pMsg->m_szMsg );
					break;

				case party_member_lv_upd :
					PartyMsg_PartyMemberLv_upd( pi_pMsg->m_szMsg );
					break;

				case party_member_pos_upd :
					PartyMsg_PartyMemberPos_upd( pi_pMsg->m_szMsg );
					break;

				case party_member_effect_upd :
					PartyMsg_PartyMemberEffect_upd( pi_pMsg->m_szMsg );
					break;

				default :
					m_ClientLogFile.Write( "Client Error - CNetworkMgr::AnalysisLocalChatServerData() - party_msg (udp) - Unknown Message\n" );
					return false;
			}
			break;

		// 채팅 메세지
		case chat_msg :
			switch( pi_pMsg->m_byType[1] )
			{
				case chat_message_receipt_udp :
					ChatMsg_ChatMessageReceipt_udp( pi_pMsg->m_szMsg );
					break;

				case announ_message_receipt_udp :
					ChatMsg_AnnounMessageReceipt_udp( pi_pMsg->m_szMsg );
					break;

				default :
					m_ClientLogFile.Write( "Client Error - CNetworkMgr::AnalysisLocalChatServerData() - chat_msg (udp) - Unknown Message\n" );
					return false;
			}
			break;

		// 에러시는 로그를 남긴다.
		default :
			m_ClientLogFile.Write( "Client Error - CUDPNetworkMgr::DataAnalysis() - Unknown Message\n" );
			return false;
	}

	return true;
}
*/