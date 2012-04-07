////////////////////////////////////////////////////////////////////////////
//
// CNetworkMgr Class Implementation
//
////////////////////////////////////////////////////////////////////////////

#include "../GameObject/Manager/CCharacterMgr.h"

extern BOOL  g_bWriteTestLog;		// 테스트 코드
extern DWORD g_bWriteTestLogCount;	// 테스트 코드


extern char  m_gClassTypeString[8];
////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////


// 사용자 가입 요청
BOOL
CNetworkMgr::AccountMsg_JoinAccountRequest_cllo( void )
{
	_join_account_request_cllo l_sSend;
	ZeroMemory( &l_sSend, sizeof( _join_account_request_cllo ) );

	memcpy( l_sSend.szAccountID, m_pID, max_id_len + 1 );
	memcpy( l_sSend.szPassword, m_pPassword, max_pw_len + 1 );

	// 메세지를 보냄
	BYTE l_nType[] = { account_msg, join_account_request_cllo };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 (char *)&l_sSend,
									 sizeof( _join_account_request_cllo ) );

	// 결과 처리
	if( l_nResult )
	{
		SendRequestMsg();
		ZeroMemory( m_pPassword, 32 );
	}
	else
	{
		NetLogWrite( "CNetworkMgr::AccountMsg_JoinAccountRequest_cllo(%d) Failure : %d", NST_GAME_SERVER, l_nResult );
	}

	/*
	#define join_account_request_cllo 1
	struct _join_account_request_cllo {
		char szAccountID[max_id_len+1];
		char szPassword[max_pw_len+1];

		int size()	{	return sizeof(*this);	}
	};*/

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) AccountMsg_JoinAccountRequest_cllo", ++g_bWriteTestLogCount ); // 테스트 코드

	return l_nResult;
}

// 사용자 가입 요청에 대한 결과
void
CNetworkMgr::AccountMsg_JoinAccountResult_locl( char * pi_pMsg )
{
	_join_account_result_locl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _join_account_result_locl ) );

	m_byResultOfUserRegistration = l_sRecv.byRetCode;

	ReceiveResponseMsg();

	/*
	#define join_account_result_locl 2
	struct _join_account_result_locl {
		BYTE	byRetCode;
		int size()	{	return sizeof(*this);	}
	};*/

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) AccountMsg_JoinAccountResult_locl", ++g_bWriteTestLogCount ); // 테스트 코드
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 사용자 인증 요청
BOOL
CNetworkMgr::AccountMsg_LoginAccountRequest_cllo( void )
{
	_login_account_request_cllo l_sSend;
	ZeroMemory( &l_sSend, sizeof( _login_account_request_cllo ) );

	memcpy( l_sSend.szAccountID, m_pID, max_id_len + 1 );
	memcpy( l_sSend.szPassword, m_pPassword, max_pw_len + 1 );

	// 메세지를 보냄
	BYTE l_nType[] = { account_msg, login_account_request_cllo };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 (char *)&l_sSend,
									 sizeof( _login_account_request_cllo ) );

	// 결과 처리
	if( l_nResult )
	{
		SendRequestMsg();
	}
	else
	{
		NetLogWrite( "CNetworkMgr::AccountMsg_LoginAccountRequest_cllo(%d) Failure : %d", NST_GAME_SERVER, l_nResult );
	}

	/*
	#define login_account_request_cllo 3
	struct _login_account_request_cllo {
		char szAccountID[max_id_len+1];
		char szPassword[max_pw_len+1];

		int size()	{	return sizeof(*this);	}
	};*/

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) MasterMsg_LoginUserRequest_clma", ++g_bWriteTestLogCount ); // 테스트 코드

	return l_nResult;
}

// 사용자 인증 요청에 대한 결과
void
CNetworkMgr::AccountMsg_LoginAccountResult_locl( char * pi_pMsg )
{
	_login_account_result_locl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _login_account_result_locl ) );

	m_byResultOfLogin	= l_sRecv.byRetCode;
	m_dwAccountIndex	= l_sRecv.dwAccountSerial;

	if( m_byResultOfLogin != RET_CODE_ALREADY_LOGIN )
		ZeroMemory( m_pPassword, 32 );

	ReceiveResponseMsg();

	/*
	#define login_account_result_locl 4
	struct _login_account_result_locl {

		BYTE	byRetCode;
		DWORD	dwAccountSerial;	//add(9/15)
		DWORD	dwPatchServerIP;
		WORD	wPatchServerPort;

		int size()	{	return sizeof(*this);	}
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 월드 서버 리스트 요청
BOOL
CNetworkMgr::AccountMsg_WorldListRequest_cllo( void )
{
	_world_list_request_cllo l_sSend;

	l_sSend.dwClientVersion = 0;

	// 메세지를 보냄
	BYTE l_nType[] = { account_msg, world_list_request_cllo };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 (char *)&l_sSend,
									 sizeof( _world_list_request_cllo ) );

	// 결과 처리
	if( l_nResult )
		SendRequestMsg();
	else
		NetLogWrite( "CNetworkMgr::AccountMsg_WorldListRequest_cllo(%d) Failure : %d", NST_GAME_SERVER, l_nResult );

	/*
	#define world_list_request_cllo 5
	struct _world_list_request_cllo {
		DWORD	dwClientVersion;

		int size()	{	return sizeof(*this);	}
	};*/

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) AccountMsg_WorldListRequest_cllo", ++g_bWriteTestLogCount ); // 테스트 코드

	return l_nResult;
}

// 월드 서버 리스트 요청에 대한 결과
void
CNetworkMgr::AccountMsg_WorldListResult_locl( char * pi_pMsg )
{
	_world_list_result_locl l_sRecv;

	_world_list_result_locl * l_pRecv = ( _world_list_result_locl * )pi_pMsg;
	memcpy( &l_sRecv, l_pRecv, l_pRecv->size() );

	/*
	l_sRecv.byRetCode = pi_pMsg[0];
	memcpy( &l_sRecv.wDataSize, &pi_pMsg[1], 2 );
	memcpy( l_sRecv.sListData, &pi_pMsg[3], l_sRecv.wDataSize );*/

	m_byResultOfRequestForWorldServerList	= l_sRecv.byRetCode;

	m_byWorldServerNum = l_sRecv.sListData[0];
	BYTE l_bySizeOfName;
	for( int i = 0, cnt = 1; i < m_byWorldServerNum; ++i )
	{
		m_pWorldServerList[i].m_bIsOpen = l_sRecv.sListData[cnt++];
		l_bySizeOfName = l_sRecv.sListData[cnt++];
		memcpy( m_pWorldServerList[i].m_pName, &l_sRecv.sListData[cnt], l_bySizeOfName );
		cnt += l_bySizeOfName;
	}

	ReceiveResponseMsg();

	/*
	#define world_list_result_locl 6
	struct _world_list_result_locl {
		BYTE	byRetCode;

		//월드수[1] {	오픈[1], 이름크기[1], 이름[..(NULL포함)]	}..

		WORD	wDataSize;
		char	sListData[0x0FFF];

		int size()	{	
			return sizeof(*this)-sizeof(char)*(0x0FFF-wDataSize);	}		
	};*/

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) AccountMsg_WorldListResult_locl", ++g_bWriteTestLogCount ); // 테스트 코드
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 월드 서버 선택
BOOL
CNetworkMgr::AccountMsg_SelectWorldRequest_cllo( DWORD pi_dwIndex )
{
	_select_world_request_cllo l_sSend;

	l_sSend.wWorldIndex = pi_dwIndex;

	// 메세지를 보냄
	BYTE l_nType[] = { account_msg, select_world_request_cllo };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 (char *)&l_sSend,
									 sizeof( _select_world_request_cllo ) );

	// 결과 처리
	if( l_nResult )
		SendRequestMsg();
	else
		NetLogWrite( "CNetworkMgr::AccountMsg_SelectWorldRequest_cllo(%d) Failure : %d", NST_GAME_SERVER, l_nResult );

	/*
	#define select_world_request_cllo 7
	struct _select_world_request_cllo {
		WORD	wWorldIndex;

		int size()	{	return sizeof(*this);	}
	};*/

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) AccountMsg_SelectWorldRequest_cllo", ++g_bWriteTestLogCount ); // 테스트 코드

	return l_nResult;
}

// 월드 서버 선택에 대한 응답
void
CNetworkMgr::AccountMsg_SelectWorldResult_locl( char * pi_pMsg )
{
	_select_world_result_locl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _select_world_result_locl ) );

	m_byResultOfSelectWorldServer = l_sRecv.byRetCode;

	m_dwServerIP[GST_WORLD]		= l_sRecv.dwWorldGateIP;
	m_dwServerPort[GST_WORLD]	= l_sRecv.wWorldGatePort;
	memcpy( m_dwWorldServerKey, l_sRecv.dwWorldMasterKey, sizeof( DWORD ) * KEY_NUM );

	ReceiveResponseMsg();

	/*
	#define select_world_result_locl 8
	struct _select_world_result_locl {
		BYTE	byRetCode;
		DWORD   dwWorldGateIP;
		WORD	wWorldGatePort;
		DWORD	dwWorldMasterKey[KEY_NUM];

		int size()	{	return sizeof(*this);	}
	};*/

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) AccountMsg_SelectWorldResult_locl", ++g_bWriteTestLogCount ); // 테스트 코드
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 유저가 입력한 계정이 이미 로그인 중일경우, 서버에 Disconnect 요청
BOOL
CNetworkMgr::AccountMsg_PushCloseRequest_cllo( void )
{
	_push_close_request_cllo l_sSend;
	ZeroMemory( &l_sSend, sizeof( _push_close_request_cllo ) );

	memcpy( l_sSend.szAccountID, m_pID, max_id_len + 1 );
	memcpy( l_sSend.szPassword, m_pPassword, max_pw_len + 1 );

	// 메세지를 보냄
	BYTE l_nType[] = { account_msg, push_close_request_cllo };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 (char *)&l_sSend,
									 sizeof( _push_close_request_cllo ) );

	// 결과 처리
	if( l_nResult )
	{
		SendRequestMsg();
		ZeroMemory( m_pPassword, 32 );
	}
	else
	{
		NetLogWrite( "CNetworkMgr::AccountMsg_PushCloseRequest_cllo(%d) Failure : %d", NST_GAME_SERVER, l_nResult );
	}

	/*
	#define push_close_request_cllo 9
	struct _push_close_request_cllo {
		char szAccountID[max_id_len+1];
		char szPassword[max_pw_len+1];

		int size()	{	return sizeof(*this);	}
	};*/

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) AccountMsg_PushCloseRequest_cllo", ++g_bWriteTestLogCount ); // 테스트 코드

	return l_nResult;
}

// Disconnect 요청에 대한 응답
void
CNetworkMgr::AccountMsg_PushCloseResult_locl( char * pi_pMsg )
{
	_push_close_result_locl l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _push_close_result_locl ) );

	m_byResultOfRequestForDisconnectAccount = l_sRecv.byRetCode;

	ReceiveResponseMsg();

	/*
	#define push_close_result_locl 10
	struct _push_close_result_locl {
		BYTE	byRetCode;

		int size()	{	return sizeof(*this);	}
	};*/

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) AccountMsg_PushCloseResult_locl", ++g_bWriteTestLogCount ); // 테스트 코드
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 월드 서버 접속 요청
BOOL
CNetworkMgr::SystemMsg_EnterWorldRequest_zone( void )
{
	_enter_world_request_zone l_sSend;
	ZeroMemory( &l_sSend, sizeof( _enter_world_request_zone ) );

	l_sSend.dwAccountSerial = m_dwAccountIndex;
	memcpy( l_sSend.dwMasterKey, ::CalcCodeKey( m_dwWorldServerKey ), sizeof( DWORD ) * KEY_NUM );

	l_sSend.bFullMode = !_IsWindowed();

	// 메세지를 보냄
	BYTE l_nType[] = { system_msg, enter_world_request_zone };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 (char *)&l_sSend,
									 sizeof( _enter_world_request_zone ) );

	// 결과 처리
	if( l_nResult )
		SendRequestMsg();
	else
		NetLogWrite( "CNetworkMgr::SystemMsg_EnterWorldRequest_zone(%d) Failure : %d", NST_GAME_SERVER, l_nResult );

	/*
	#define enter_world_request_zone 1	// 사용자 인증을 요청	
	struct _enter_world_request_zone {

		DWORD	dwAccountSerial;
		DWORD	dwMasterKey[KEY_NUM];		
		bool	bFullMode;	// add (12/8)

		int size()	{ return sizeof(*this);	}
	};*/

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) SystemMsg_EnterWorldRequest_zone", ++g_bWriteTestLogCount ); // 테스트 코드

	return l_nResult;
}

// 월드 서버 접속 요청에 대한 응답
void
CNetworkMgr::SystemMsg_EnterWorldResult_zone( char * pi_pMsg )
{
	_enter_world_result_zone l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _enter_world_result_zone ) );

	m_byResultOfEnterTheWorldServer = l_sRecv.byResult;

	m_byAvatarGrade = l_sRecv.byUserGrade;

	ReceiveResponseMsg();

	/*
	#define enter_world_result_zone 2// 사용자 인증의 결과	
	struct _enter_world_result_zone {
	//유저등급..( CharacterDB.h )
	//#define USER_DEGREE_STD			0		//유저등급-일반
	//#define USER_DEGREE_GID			1		//유저등급-가이드
	//#define USER_DEGREE_MGR			2		//유저등급-운영자
	//#define USER_DEGREE_DEV			3		//유저등급-개발자
	BYTE	byResult;
	BYTE	byUserGrade; //계정등급.. add(12/30).. 2, 3등급 치트키 및 운영자 메시지, /이동 사용가능

	int size()	{ return sizeof(*this);	}
};*/

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) SystemMsg_EnterWorldResult_zone", ++g_bWriteTestLogCount ); // 테스트 코드
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 서버에 접속
BOOL
CNetworkMgr::SystemMsg_MoveinUserRequest_zone( void )
{
	SystemMsg_MoveinUserResult_zone( NULL );
	return TRUE;

	/*
	_movein_user_request_zone l_sSend;
	ZeroMemory( &l_sSend, sizeof( _movein_user_request_zone ) );

	l_sSend.dwAccountSerial	= m_dwAccountIndex;
	l_sSend.dwZoneMasterKey	= m_dwZoneServerKey;
	l_sSend.bFullMode		= !_IsWindowed();

	// 메세지를 보냄
	BYTE l_nType[] = { system_msg, movein_user_request_zone };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 (char *)&l_sSend,
									 sizeof( _movein_user_request_zone ) );

	// 결과 처리
	if( l_nResult )
		SendRequestMsg();
	else
		NetLogWrite( "CNetworkMgr::SystemMsg_MoveinUserRequest_zone(%d) Failure : %d", NST_GAME_SERVER, l_nResult );

	//#define movein_user_request_zone 3	// 사용자 인증을 요청
	//struct _movein_user_request_zone {
	//	DWORD	dwAccountSerial;	
	//	DWORD	dwZoneMasterKey;
	//	bool	bFullMode;	//add(10/25).. full mode true, window mode.. false
	//
	//	int size()	{ return sizeof(*this);	}
	//};

	return l_nResult;*/
}

// 서버에 접속
void
CNetworkMgr::SystemMsg_MoveinUserResult_zone( char * pi_pMsg )
{
	m_byResultOfMoveIntoServer = MOVEINTO_SERVER_SUCCESS;

	if( m_nActiveServerID == GST_ZONE )
	{
		_GetAvatar()->CreateBoard();
	}

	/*
	_movein_user_result_zone l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _movein_user_result_zone ) );

	m_byResultOfMoveIntoServer = l_sRecv.byResult;

	if( l_sRecv.byResult == RET_CODE_SUCCESS )
	{
		if( m_nActiveServerID == GST_ZONE )
		{
			_GetAvatar()->CreateBoard();
			InitMsg_BaseDownloadRequest_clzo();
		}
	}

	ReceiveResponseMsg();

	//#define movein_user_result_zone 34// 사용자 인증의 결과
	//struct _movein_user_result_zone {
	//	BYTE byResult;			//LOGIN_RET.. 
	//
	//	int size()	{ return sizeof(*this);	}
	//};

	m_ClientLogFile.Write( "( %d ) SystemMsg_MoveinUserResult_zone : %d", ++g_bWriteTestLogCount, l_sRecv.byResult ); // 테스트 코드
	*/
}

// 서버에서 접속을 끊음
BOOL
CNetworkMgr::SystemMsg_MoveoutUserRequest_zone( BYTE pi_byMoveoutType )
{
	if( m_nActiveServerID != GST_ZONE )
	{
		SystemMsg_MoveoutUserResult_zone( NULL );
		return TRUE;
	}
	else
	{
		_moveout_user_request_zone l_sSend;
		ZeroMemory( &l_sSend, sizeof( _moveout_user_request_zone ) );

		l_sSend.byNextZoneType	= pi_byMoveoutType;

		// 메세지를 보냄
		BYTE l_nType[] = { system_msg, moveout_user_request_zone };
		BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
										 l_nType,
										 (char *)&l_sSend,
										 sizeof( _moveout_user_request_zone ) );
		// 결과 처리
		if( l_nResult )
			SendRequestMsg();
		else
			NetLogWrite( "CNetworkMgr::SystemMsg_MoveoutUserRequest_zone(%d) Failure : %d", NST_GAME_SERVER, l_nResult );

		if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) SystemMsg_MoveoutUserRequest_zone", ++g_bWriteTestLogCount ); // 테스트 코드

		return l_nResult;
	}

	/*
	#define moveout_user_request_zone 5
	struct _moveout_user_request_zone {
		//#define ZONE_TYPE_GATE	0	..캐릭터선택을 다시할라믄..
		//#define ZONE_TYPE_MAP		1	..맵이동을 햇다면..
		BYTE	byNextZoneType;	//add(7/8).. 

		int size() { return sizeof(*this); }
	};*/
}

// 서버에서 접속을 끊음
void
CNetworkMgr::SystemMsg_MoveoutUserResult_zone( char * pi_pMsg )
{
	if( m_nActiveServerID != GST_ZONE )
	{
		m_byResultOfMoveOutFromServer = MOVEOUT_FROM_SERVER_SUCCESS;
	}
	else
	{
		_moveout_user_result_zone l_sRecv;
		memcpy( &l_sRecv, pi_pMsg, sizeof( _moveout_user_result_zone ) );

		m_byResultOfMoveOutFromServer = l_sRecv.byRetCode;

		SetServerIP( ( GetActiveServerID() + 1 ) % MAX_GAME_SERVER, l_sRecv.dwIP );
		SetServerPort( ( GetActiveServerID() + 1 ) % MAX_GAME_SERVER, l_sRecv.wPort );

		m_dwZoneServerKey = l_sRecv.dwZoneMasterKey;

		ReceiveResponseMsg();
	}

	/*
	#define moveout_user_result_zone 6
	struct _moveout_user_result_zone {
		BYTE byRetCode;

		DWORD dwIP;	
		WORD wPort;
		DWORD dwZoneMasterKey;//add(4/2)

		int size()	{ return sizeof(*this);	}
	};*/

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) SystemMsg_MoveoutUserResult_zone", ++g_bWriteTestLogCount ); // 테스트 코드
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 마스터 서버에 등록되어 있는 캐릭터의 정보 요청
BOOL
CNetworkMgr::SystemMsg_RegedCharRequest_zone( void )
{
	// 메세지를 보냄
	BYTE l_nType[] = { system_msg, reged_char_request_zone };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 NULL,
									 0 );

	// 결과 처리
	if( l_nResult )
		SendRequestMsg();
	else
		NetLogWrite( "CNetworkMgr::SystemMsg_RegedCharRequest_zone(%d) Failure : %d", NST_GAME_SERVER, l_nResult );

	/*
	#define reged_char_request_zone 7 //등록됀 캐릭터를 요구
	struct _reged_char_request_zone {
		int size() { return 0; }
	};*/

	return l_nResult;
}

// 마스터 서버에 등록되어 있는 캐릭터의 정보 요청에 대한 결과
void
CNetworkMgr::SystemMsg_RegedCharResult_zone( char * pi_pMsg )
{
	_reged_char_result_zone l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _reged_char_result_zone ) );

	m_byResultOfUserInfo = l_sRecv.byRetCode;

	//--------------------------------------------------------------------------

	int				i, j;
	CAvatar *		l_pAvatarDummy;
	DWORD			l_dwPrevCharLastConnTime = 0;

	for( i = 0; i < l_sRecv.byCharNum; ++i )
	{
		if( ( l_sRecv.RegedList[i].m_bySlotIndex < 0 ) || ( l_sRecv.RegedList[i].m_bySlotIndex > 2 ) )
			continue;

		if( l_sRecv.RegedList[i].m_bySlotIndex >= MAX_AVATAR_DUMMY )
			continue;

		//----------------------------------------------------------------------

		l_pAvatarDummy = _GetAvatarDummy( l_sRecv.RegedList[i].m_bySlotIndex );

		l_pAvatarDummy->SetIndex( l_sRecv.RegedList[i].m_bySlotIndex );
		l_pAvatarDummy->SetObjectTypeID( OTI_CHAR );
		l_pAvatarDummy->SetCharTypeID( CTI_AVATAR );
		l_pAvatarDummy->SetName( l_sRecv.RegedList[i].m_szAvatorName );

		l_pAvatarDummy->SetClassDTIndexByStrDTCode( 0, l_sRecv.RegedList[i].m_szClassCode );

		l_pAvatarDummy->SetRace( l_sRecv.RegedList[i].m_byRaceSexCode );
		l_pAvatarDummy->SetBone( l_sRecv.RegedList[i].m_byRaceSexCode );
		l_pAvatarDummy->SetDTIndex( l_sRecv.RegedList[i].m_byRaceSexCode );
		
		l_pAvatarDummy->SetLevel( l_sRecv.RegedList[i].m_byLevel );

		l_pAvatarDummy->SetDalant( l_sRecv.RegedList[i].m_dwDalant );
		l_pAvatarDummy->SetGold( l_sRecv.RegedList[i].m_dwGold );

		//----------------------------------------------------------------------

		for( j = 0; j < base_fix_num; ++j )
		{
			l_pAvatarDummy->SetDefaultItem( IEPT_SERVER_TO_CLIENT[j],
											( ( l_sRecv.RegedList[i].m_dwBaseShape >> ( j * 4 ) ) & 0x0000000F ) + ( l_sRecv.RegedList[i].m_byRaceSexCode * MAX_DEFAULT_ITEM_NUM ) );
		}

		//----------------------------------------------------------------------

		CEquipItem *	pEquipItem = NULL;
		CNormalItem *	pNormalItem = NULL;
		for( j = 0; j < equip_fix_num; ++j )
		{
			if( l_sRecv.RegedList[i].m_EquipKey[j].zItemIndex == -1 )
			{
				if( j < base_fix_num )
				{
					pEquipItem = l_pAvatarDummy->GetDefaultItem( IEPT_SERVER_TO_CLIENT[j] );
					if( pEquipItem )
						l_pAvatarDummy->PutOnEquipment( IEPT_SERVER_TO_CLIENT[j], pEquipItem->GetDTIndex() );
				}
				else
				{
					l_pAvatarDummy->PutOffEquipment( IEPT_SERVER_TO_CLIENT[j] );
				}
			}
			else
			{
				l_pAvatarDummy->PutOnEquipment( IEPT_SERVER_TO_CLIENT[j], l_sRecv.RegedList[i].m_EquipKey[j].zItemIndex );
				pNormalItem = l_pAvatarDummy->GetNormalItem( IEPT_SERVER_TO_CLIENT[j] );
				if( pNormalItem )
					pNormalItem->SetLevel( l_sRecv.RegedList[i].m_byEquipLv[j] );
			}
		}
		l_pAvatarDummy->PutOnEquipment( CEPT_FACE, ( ( l_sRecv.RegedList[i].m_dwBaseShape >> 20 ) & 0x0000000F ) + ( l_sRecv.RegedList[i].m_byRaceSexCode * MAX_DEFAULT_ITEM_NUM ) );
		pEquipItem = l_pAvatarDummy->GetDefaultItem( IEPT_HELMET );
		if( pEquipItem )
			l_pAvatarDummy->PutOnEquipment( CEPT_HAIR, pEquipItem->GetDTIndex() );

		//----------------------------------------------------------------------

		l_pAvatarDummy->SetAction( CAI_MOVE_STOP );
		l_pAvatarDummy->SetLightColor( D3DCOLOR_XRGB(128,128,128) );

		//----------------------------------------------------------------------

		if( l_dwPrevCharLastConnTime < l_sRecv.RegedList[i].m_dwLastConnTime )
		{
			m_dwLastestAvatarIndex		= l_sRecv.RegedList[i].m_bySlotIndex;
			l_dwPrevCharLastConnTime	= l_sRecv.RegedList[i].m_dwLastConnTime;
		}
	}

	ReceiveResponseMsg();

	/*
	#define reged_char_result_zone 8 //등록됀 캐릭터를 요구
	struct _reged_char_result_zone {
		BYTE bRetCode;	//REGED_CHAR_RET..
	//	BYTE bySelectedCharIndex;	//del(2/21)

		BYTE byCharNum;

		_REGED_AVATOR_DB RegedList[max_char_per_user];	//upt(2/26)

		_reged_char_result_zone(){
			byCharNum = 0;
		}

		int size(){ 
			return sizeof(*this)-sizeof(_REGED_AVATOR_DB)*(max_char_per_user-byCharNum);
		}
	};

	struct _REGED_AVATOR_DB			// 등록아바타를 클라이언트로 보낼때담는하는 구조체
	{
		char	m_szAvatorName[max_name_len+1];
		DWORD	m_dwRecordNum;		// 아바타 등록 시리얼
		BYTE	m_byRaceCode;
		BYTE	m_bySlotIndex;	
		BYTE	m_byLevel;
		DWORD	m_dwDalant;
		DWORD	m_dwGold;
		DWORD	m_dwBaseShape;	//기본생김.. (4비트씩 0번비트부터..upper, lower, gauntlet, shoe, helmet, face)
		WORD	m_wFixEquip[equip_fix_num];
		BYTE	m_byEquipLv[equip_fix_num];	// add(9/17)
		DWORD	m_dwLastConnTime;	//마지막 로그오프한 시간..

		_REGED_AVATOR_DB(){		m_bySlotIndex = 0xFF;	}
	};*/

	//if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) SystemMsg_RegedCharResult_zone", ++g_bWriteTestLogCount ); // 테스트 코드
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

DWORD
SetEquipFlag( BYTE pi_byEquipType, BYTE pi_byCipher )
{
	CEquipItem * pEquipItem = _GetAvatar()->GetEquipItem( pi_byEquipType );
	if( pEquipItem )
		return ( ( pEquipItem->GetDTIndex() - _GetAvatar()->GetRace() * MAX_DEFAULT_ITEM_NUM ) << pi_byCipher );

	return 0;
}

// 새로운 캐릭터 등록 요청
BOOL
CNetworkMgr::SystemMsg_AddCharRequest_zone( BYTE pi_byAvatarIndex )
{
	_add_char_request_zone l_sSend;
	ZeroMemory( &l_sSend, sizeof( _add_char_request_zone ) );

	l_sSend.bySlotIndex		= pi_byAvatarIndex;
	l_sSend.byRaceSexCode	= _GetAvatar()->GetRace();

	for( int i = 0; i < base_fix_num - 1; ++i )
		l_sSend.dwBaseShape	|= SetEquipFlag( IEPT_SERVER_TO_CLIENT[i], i * 4 );
	l_sSend.dwBaseShape	|= SetEquipFlag( CEPT_HAIR, 16 );
	l_sSend.dwBaseShape	|= SetEquipFlag( CEPT_FACE, 20 );

	_strcpy( l_sSend.szCharName, _GetAvatar()->GetName(), max_name_len + 1 );
	memcpy( l_sSend.szClassCode, m_gClassTypeString, 4 );
	l_sSend.szClassCode[4] = 0;

	_GetAvatar()->SetClassDTIndexByStrDTCode( 0, m_gClassTypeString );

	// 메세지를 보냄
	BYTE l_nType[] = { system_msg, add_char_request_zone };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 (char *)&l_sSend,
									 sizeof( _add_char_request_zone ) );

	// 결과 처리
	if( l_nResult )
		SendRequestMsg();
	else
		NetLogWrite( "CNetworkMgr::SystemMsg_AddCharRequest_zone(%d) Failure : %d", NST_GAME_SERVER, l_nResult );

	/*
	#define add_char_request_zone 9	// 캐릭터 등록
	struct _add_char_request_zone {
		BYTE bySlotIndex;				
		char szCharName[max_name_len+1];
		BYTE byRaceCode;
		char szClassCode[class_code_len+1];	
		DWORD dwBaseShape;	
		
		int size()	{ return sizeof(*this);	}
	};*/

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) SystemMsg_AddCharRequest_zone", ++g_bWriteTestLogCount ); // 테스트 코드

	return l_nResult;
}

// 새로운 캐릭터 등록 요청에 대한 결과
void
CNetworkMgr::SystemMsg_AddCharResult_zone( char * pi_pMsg )
{
	_add_char_result_zone l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _add_char_result_zone ) );

	m_byResultOfCharacterCreate = l_sRecv.byRetCode;
	l_sRecv.byAddSlotIndex;

	_GetAvatar()->SetMaxHP( 70 );
	_GetAvatar()->SetHP( 70 );
	_GetAvatar()->SetMaxFP( 35 );
	_GetAvatar()->SetFP( 35 );
	_GetAvatar()->SetMaxSP( 120 );
	_GetAvatar()->SetSP( 120 );

	ReceiveResponseMsg();

	/*
	#define add_char_result_zone 10	// 캐릭터 등록 확인
	struct _add_char_result_zone {
		BYTE byRetCode;		//REG_RET..
		BYTE byAddSlotIndex;

		int size()	{ return sizeof(*this);	}
	};*/

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) SystemMsg_AddCharResult_zone", ++g_bWriteTestLogCount ); // 테스트 코드
}

// 기존 캐릭터 삭제 요청
BOOL
CNetworkMgr::SystemMsg_DelCharRequest_zone( BYTE pi_byAvatarIndex )
{
	_del_char_request_zone l_sSend;

	l_sSend.bySlotIndex = pi_byAvatarIndex;

	// 메세지를 보냄
	BYTE l_nType[] = { system_msg, del_char_request_zone };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 (char *)&l_sSend,
									 sizeof( _del_char_request_zone ) );

	// 결과 처리
	if( l_nResult )
		SendRequestMsg();
	else
		NetLogWrite( "CNetworkMgr::SystemMsg_DelCharRequest_zone(%d) Failure : %d", NST_GAME_SERVER, l_nResult );

	/*
	#define del_char_request_zone 11	//삭제할 케릭터 선택
	struct _del_char_request_zone {
		BYTE bySlotIndex;

		int size()	{ return sizeof(*this);	}
	};*/

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) SystemMsg_DelCharRequest_zone", ++g_bWriteTestLogCount ); // 테스트 코드

	return l_nResult;
}

// 기존 캐릭터 삭제 요청에 대한 결과
void
CNetworkMgr::SystemMsg_DelCharResult_zone( char * pi_pMsg )
{
	_del_char_result_zone l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _del_char_result_zone ) );

	m_byResultOfCharacterDelete = l_sRecv.byRetCode;
	l_sRecv.bySlotIndex;

	ReceiveResponseMsg();

	/*
	#define del_char_result_zone 12	//삭제할 케릭터 선택확인
	struct _del_char_result_zone {
		BYTE byRetCode;	//DEL_CHAR_RET..
		BYTE bySlotIndex;

		int size()	{ return sizeof(*this);	}
	};*/

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) SystemMsg_DelCharResult_zone", ++g_bWriteTestLogCount ); // 테스트 코드
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 플레이할 캐릭터 선택 요청
BOOL
CNetworkMgr::SystemMsg_SelCharRequest_zone( BYTE pi_byAvatarIndex )
{
	_sel_char_request_zone l_sSend;

	l_sSend.bySlotIndex = pi_byAvatarIndex;

	// 메세지를 보냄
	BYTE l_nType[] = { system_msg, sel_char_request_zone };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 (char *)&l_sSend,
									 sizeof( _sel_char_request_zone ) );

	// 결과 처리
	if( l_nResult )
		SendRequestMsg();
	else
		NetLogWrite( "CNetworkMgr::SystemMsg_SelCharRequest_zone(%d) Failure : %d", NST_GAME_SERVER, l_nResult );

	/*
	#define sel_char_request_zone 13	//플레이할 케릭터 선택
	struct _sel_char_request_zone {
		BYTE bySlotIndex;

		int size()	{ return sizeof(*this);	}
	};*/

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) SystemMsg_SelCharRequest_zone", ++g_bWriteTestLogCount ); // 테스트 코드

	return l_nResult;
}

// 플레이할 캐릭터 선택 요청에 대한 결과
void
CNetworkMgr::SystemMsg_SelCharResult_zone( char * pi_pMsg )
{
	_sel_char_result_zone l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _sel_char_result_zone ) );

	m_byResultOfCharacterSelect = l_sRecv.byRetCode;
	l_sRecv.bySlotIndex;

	m_dwAvatarIndex = l_sRecv.dwWorldSerial;

	ReceiveResponseMsg();

	/*
	#define sel_char_result_zone 14	//플레이할 케릭터 선택확인
	struct _sel_char_result_zone {
		BYTE byRetCode;	//SEL_CHAR_RET..
		BYTE bySlotIndex;

		int size()	{ return sizeof(*this);	}
	};*/

	if( g_bWriteTestLog ) m_ClientLogFile.Write( "( %d ) SystemMsg_SelCharResult_zone", ++g_bWriteTestLogCount ); // 테스트 코드
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CNetworkMgr::AccountMsg_ServerNotifyInform_locl( char * pi_pMsg )
{
	_server_notify_inform_locl * pRecvMsg = reinterpret_cast< _server_notify_inform_locl * >( pi_pMsg );

	// 유저 퇴장 메세지
	if( pRecvMsg->wMsgCode == FORCE_CLOSE_TYPE_KICK )
	{
		_SetSystemMsg( "잠시 후 게임 서비스가 중단됩니다. 자세한 사항은 운영자에게 문의하시기 바랍니다." );
	}
	// 서버 종료 메세지 => 다른곳에서 같은 계정의 사용자가 접속해서 접속을 곧 종료하겟음
	else if( pRecvMsg->wMsgCode == FORCE_CLOSE_TYPE_PUSH )
	{
		_SetSystemMsg( "같은 계정의 사용자가 다른곳에서 로그인을 하였습니다. 잠시 후 접속이 끊어집니다." );
	}
	// 서버 종료 메세지 => 내부 사정으로 인한 강제 종료
	else if( pRecvMsg->wMsgCode == FORCE_CLOSE_TYPE_SERVICE || pRecvMsg->wMsgCode == FORCE_CLOSE_TYPE_ERROR )
	{
		_SetSystemMsg( "잠시 후 게임 서비스가 중단됩니다. 자세한 사항은 RF Online 공식 홈페이지를 참조하시기 바랍니다." );
	}

	RequestQuitProgram();

	/*
	//SERVER NOTIFY CODE
	#define server_notify_inform_locl 11
	struct _server_notify_inform_locl {
		WORD wMsgCode;

		int size()	{	return sizeof(*this);	}
	};*/
}
