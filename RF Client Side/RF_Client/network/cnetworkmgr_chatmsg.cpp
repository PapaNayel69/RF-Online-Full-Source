////////////////////////////////////////////////////////////////////////////////
//
// CNetworkMgr Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../GameObject/Manager/CCharacterMgr.h"

#include "../UserInterface/GameWindow/GUIChatWindow.h"

extern BOOL  g_bWriteTestLog;		// 테스트 코드
extern DWORD g_bWriteTestLogCount;	// 테스트 코드

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 운영자가 보내는 채팅 메세지
BOOL
CNetworkMgr::ChatMsg_ChatOperatorRequest_clzo( char * pi_pChatMessage )
{
	if( !pi_pChatMessage )
		return FALSE;

	_chat_operator_request_clzo l_sSend;

	l_sSend.byRaceCode	= 0xFF;
	l_sSend.bySize		= strlen( pi_pChatMessage );
	_strcpy( l_sSend.sChatData, pi_pChatMessage, max_message_size );

	// 메세지를 보냄
	BYTE l_nType[] = { chat_msg, chat_operator_request_clzo };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 ( char * )&l_sSend,
									 l_sSend.size() );

	// 결과 처리
	if( !l_nResult )
		NetLogWrite( "CNetworkMgr::ChatMsg_ChatOperatorRequest_clzo(%d) Failure : %d", NST_GAME_SERVER, l_nResult );

	return l_nResult;

	/*
	#define chat_operator_request_clzo 1//CHAT_TYPE_OPERATOR
	struct _chat_operator_request_clzo {
		BYTE byRaceCode;	//0xFF면 전체.. 0, 1, 2면 각 종족	//add(4/29)
		BYTE bySize;
		char sChatData[max_message_size];

		int size()
		{
			return sizeof(*this)-(max_message_size-bySize);
		}
	};*/
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 아바타 주변의 플레이어에게 보내는 메세지
BOOL
CNetworkMgr::ChatMsg_ChatCircleRequest_clzo( char * pi_pChatMessage )
{
	if( !pi_pChatMessage )
		return FALSE;

	_chat_circle_request_clzo l_sSend;

	l_sSend.bySize = strlen( pi_pChatMessage );
	_strcpy( l_sSend.sChatData, pi_pChatMessage, max_message_size );

	// 메세지를 보냄
	BYTE l_nType[] = { chat_msg, chat_circle_request_clzo };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 ( char * )&l_sSend,
									 l_sSend.size() );

	// 결과 처리
	if( !l_nResult )
		NetLogWrite( "CNetworkMgr::ChatMsg_ChatCircleRequest_clzo(%d) Failure : %d", NST_GAME_SERVER, l_nResult );

	return l_nResult;

	/*
	// 아바타 주변의 플레이어에게 보내는 메세지
	#define chat_circle_request_clzo 2//CHAT_TYPE_CIRCLE
	struct _chat_circle_request_clzo {

		BYTE bySize;
		char sChatData[max_message_size];

		int size()
		{
			return sizeof(*this)-(max_message_size-bySize);
		}
	};*/
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 아바타가 다른 플레이에게 귓속말을 보냄 - 캐릭터 이름을 사용
BOOL
CNetworkMgr::ChatMsg_ChatFarRequest_clzo( char * pi_pPlayerName, char * pi_pChatMessage )
{
	if( !pi_pPlayerName || !pi_pChatMessage )
		return FALSE;

	_chat_far_request_clzo l_sSend;

	_strcpy( l_sSend.szName, pi_pPlayerName, max_name_len + 1 );

	l_sSend.bySize = strlen( pi_pChatMessage );
	_strcpy( l_sSend.sChatData, pi_pChatMessage, max_message_size );

	// 메세지를 보냄
	BYTE l_nType[] = { chat_msg, chat_far_request_clzo };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 ( char * )&l_sSend,
									 l_sSend.size() );

	// 결과 처리
	if( !l_nResult )
		NetLogWrite( "CNetworkMgr::ChatMsg_ChatFarRequest_clzo(%d) Failure : %d", NST_GAME_SERVER, l_nResult );

	return l_nResult;

	/*
	// 귓속말 - 캐릭터명으로 보냄
	#define chat_far_request_clzo 4//CHAT_TYPE_FAR_WHISPER
	struct _chat_far_request_clzo {

		char szName[max_name_len+1];
		
		BYTE bySize;
		char sChatData[max_message_size];

		int size()
		{
			return sizeof(*this)-(max_message_size-bySize);
		}
	};*/
}

// _chat_far_request_clzo => 에서 요청한 플레이어가 없을 경우
void
CNetworkMgr::ChatMsg_ChatFarFailure_zocl( char * pi_pMsg )
{
	_SetSystemMsg( "귓속말을 보낼 플레이어가 잘못되었거나 접속해 있지 않습니다." );

	if( strcmp( _GetChatWindow()->GetWhisperCharName(), "" ) == 0 )
	{
		CGUIInput * l_pInput;
		l_pInput = _GetChatInputWindow()->GetInputField();
		if( l_pInput )
			l_pInput->Clear();

		l_pInput = _GetChatWindow()->GetInputField();
		if( l_pInput )
			l_pInput->Clear();
	}
	else
	{
		if( strcmp( _GetChatWindow()->GetWhisperCharName(), _GetAvatar()->GetChatInfo()->m_pLatestSendWhisperName ) == 0 )
			_GetChatWindow()->SetWhisperCharName( NULL );
	}

	_GetAvatar()->GetChatInfo()->DeleteWhisperList( _GetAvatar()->GetChatInfo()->m_pLatestSendWhisperName );

	/*
	// _chat_far_request_clzo => 에서 요청한 플레이어가 없을 경우
	#define chat_far_failure_zocl 14//플레이어를 못찾앗다..
	struct _chat_far_failure_zocl {
	};*/
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 아바타가 파티 메세지를 보냄
BOOL
CNetworkMgr::ChatMsg_ChatPartyRequest_clzo( char * pi_pChatMessage )
{
	if( !pi_pChatMessage )
		return FALSE;

	_chat_party_request_clzo l_sSend;

	l_sSend.bySize = strlen( pi_pChatMessage );
	_strcpy( l_sSend.sChatData, pi_pChatMessage, max_message_size );

	// 메세지를 보냄
	BYTE l_nType[] = { chat_msg, chat_party_request_clzo };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 ( char * )&l_sSend,
									 l_sSend.size() );

	// 결과 처리
	if( !l_nResult )
		NetLogWrite( "CNetworkMgr::ChatMsg_ChatPartyRequest_clzo(%d) Failure : %d", NST_GAME_SERVER, l_nResult );

	return l_nResult;

	/*
	// 파티 메세지
	#define chat_party_request_clzo 4//CHAT_TYPE_PARTY
	struct _chat_party_request_clzo {

		BYTE bySize;
		char sChatData[max_message_size];

		int size()
		{
			return sizeof(*this)-(max_message_size-bySize);
		}
	};*/
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 아바타가 종족 메세지를 보냄
BOOL
CNetworkMgr::ChatMsg_ChatRaceRequest_clzo( char * pi_pChatMessage )
{
	if( !pi_pChatMessage )
		return FALSE;

	_chat_race_request_clzo l_sSend;

	l_sSend.bySize = strlen( pi_pChatMessage );
	_strcpy( l_sSend.sChatData, pi_pChatMessage, max_message_size );

	// 메세지를 보냄
	BYTE l_nType[] = { chat_msg, chat_race_request_clzo };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 ( char * )&l_sSend,
									 l_sSend.size() );

	// 결과 처리
	if( !l_nResult )
		NetLogWrite( "CNetworkMgr::ChatMsg_ChatPartyRequest_clzo(%d) Failure : %d", NST_GAME_SERVER, l_nResult );

	return l_nResult;

	/*
	#define chat_race_request_clzo 16
	struct _chat_race_request_clzo {
		BYTE bySize;
		char sChatData[max_message_size];

		int size()
		{
			return sizeof(*this)-(max_message_size-bySize);
		}
	};*/
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 운영자가 사용하는 치트 메세지
BOOL
CNetworkMgr::ChatMsg_ChatCheatRequest_clzo( char * pi_pChatMessage )
{
	if( !pi_pChatMessage )
		return FALSE;

	_chat_cheat_request_clzo l_sSend;

	l_sSend.bySize = strlen( pi_pChatMessage );
	_strcpy( l_sSend.sChatData, pi_pChatMessage, max_message_size );

	// 메세지를 보냄
	BYTE l_nType[] = { chat_msg, chat_cheat_request_clzo };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 ( char * )&l_sSend,
									 l_sSend.size() );

	// 결과 처리
	if( !l_nResult )
		NetLogWrite( "CNetworkMgr::ChatMsg_ChatCheatRequest_clzo(%d) Failure : %d", NST_GAME_SERVER, l_nResult );

	return l_nResult;

	/*
	// 치트 메세지
	#define chat_cheat_request_clzo 4
	struct _chat_cheat_request_clzo {

		BYTE bySize;
		char sChatData[max_message_size];

		int size()
		{
			return sizeof(*this)-(max_message_size-bySize);
		}
	};*/
}

// 운영자끼리의 귓속말
BOOL
CNetworkMgr::ChatMsg_ChatMgrWhisperRequest_clzo( char * pi_pChatMessage )
{
	if( !pi_pChatMessage )
		return FALSE;

	_chat_cheat_request_clzo stSend;

	stSend.bySize = strlen( pi_pChatMessage );
	_strcpy( stSend.sChatData, pi_pChatMessage, max_message_size );

	// 메세지를 보냄
	BYTE l_nType[] = { chat_msg, chat_mgr_whisper_request_clzo };
	BOOL l_nResult = SendNetMessage( NST_GAME_SERVER,
									 l_nType,
									 ( char * )&stSend,
									 stSend.size() );

	// 결과 처리
	if( !l_nResult )
		NetLogWrite( "CNetworkMgr::ChatMsg_ChatCheatRequest_clzo(%d) Failure : %d", NST_GAME_SERVER, l_nResult );

	return l_nResult;

	/*
	// 운영자끼리의 메세지
	#define chat_mgr_whisper_request_clzo 66
	struct _chat_mgr_whisper_request_clzo {	//add(01/02)
		BYTE bySize;
		char sChatData[max_message_size];

		int size()
		{
			return sizeof(*this)-(max_message_size-bySize);
		}
	};*/
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
GetNameFromChatMessage( char * po_pName, char * pi_pChatMessage )
{
	if( !po_pName || !pi_pChatMessage )
		return;

	for( int i = 0; pi_pChatMessage[i] != ' '; ++i )
		po_pName[i] = pi_pChatMessage[i];
}

// 채팅 메세지 수신 ( 아바타의 시야에 있는 사람 )
void
CNetworkMgr::ChatMsg_ChatMessageReceipt_udp( char * pi_pMsg )
{
	if( !_GetChatWindow() )
		return;

	_chat_message_receipt_udp stRecvMsg;
	ZeroMemory( &stRecvMsg, sizeof( _chat_message_receipt_udp ) );
	memcpy( &stRecvMsg, pi_pMsg, reinterpret_cast< _chat_message_receipt_udp * >( pi_pMsg )->size() );

	if( stRecvMsg.bFiltering )
	{
		static const char WORD[] = { "!@#$%^&*,." };
		int nLength = rand() % 40;
		for( int i = 0; i < nLength; ++i )
			stRecvMsg.sChatData[i] = WORD[ rand() % 10 ];
	}

	/*
	_chat_message_receipt_udp * pRecvMsg = reinterpret_cast< _chat_message_receipt_udp * >( pi_pMsg );

	if( pRecvMsg->bFiltering )
		return;
		*/

	// 서버의 시스템 메세지임
	if( stRecvMsg.byMessageType == CHAT_TYPE_OPERATOR )
	{
		_SetNotifyMsg( stRecvMsg.sChatData );
	}
	// 운영자간의 메세지
	else if( stRecvMsg.byMessageType == CHAT_TYPE_MGRWSP )
	{
		_GetChatWindow()->InsertText( NULL, stRecvMsg.sChatData, CGUIChatWindow::FS_RACE );
	}
	// 종족 메세지
	else if( stRecvMsg.byMessageType == CHAT_TYPE_RACE )
	{
		_GetChatWindow()->InsertText( NULL, stRecvMsg.sChatData, CGUIChatWindow::FS_RACE );
	}
	// 파티 메세지
	else if( stRecvMsg.byMessageType == CHAT_TYPE_PARTY )
	{
		CPlayer * pPlayer = _GetCharMgr()->GetPlayer( stRecvMsg.dwSenderSerial );
		if( pPlayer )
		{
			if( pPlayer->IsEqualCharType( CTI_AVATAR ) )
			{
				_GetChatWindow()->InsertText( NULL, stRecvMsg.sChatData, CGUIChatWindow::FS_PARTY_AVATAR, CGUIChatWindow::PARTY );
				pPlayer->GetEtcInfo()->SetChatMsg( stRecvMsg.sChatData, CGUIChatWindow::FS_PARTY_AVATAR );
			}
			else
			{
				if( stRecvMsg.bFiltering )
					_GetChatWindow()->InsertText( pPlayer->GetName(), stRecvMsg.sChatData, CGUIChatWindow::FS_PARTY_NORMAL, CGUIChatWindow::PARTY );
				else
					_GetChatWindow()->InsertText( NULL, stRecvMsg.sChatData, CGUIChatWindow::FS_PARTY_NORMAL, CGUIChatWindow::PARTY );
				pPlayer->GetEtcInfo()->SetChatMsg( stRecvMsg.sChatData, CGUIChatWindow::FS_PARTY_NORMAL );
			}
			pPlayer->SetReceiveNetMsg( "Chat : Party" );
		}
		else
		{
			if( stRecvMsg.bFiltering )
				_GetChatWindow()->InsertText( "??????", stRecvMsg.sChatData, CGUIChatWindow::FS_PARTY_NORMAL, CGUIChatWindow::PARTY );
			else
				_GetChatWindow()->InsertText( NULL, stRecvMsg.sChatData, CGUIChatWindow::FS_PARTY_NORMAL, CGUIChatWindow::PARTY );
		}
	}
	// 귓속말
	else if( stRecvMsg.byMessageType == CHAT_TYPE_WHISPER )
	{
		CPlayer * pPlayer = _GetCharMgr()->GetPlayer( stRecvMsg.dwSenderSerial );
		if( pPlayer )
		{
			if( pPlayer->IsEqualCharType( CTI_AVATAR ) )
			{
				_GetChatWindow()->InsertText( NULL, stRecvMsg.sChatData, CGUIChatWindow::FS_WHISPER_AVATAR, CGUIChatWindow::WHISPER );
				pPlayer->GetEtcInfo()->SetChatMsg( stRecvMsg.sChatData, CGUIChatWindow::FS_WHISPER_AVATAR );
			}
			else
			{
				if( stRecvMsg.bFiltering )
					_GetChatWindow()->InsertText( pPlayer->GetName(), stRecvMsg.sChatData, CGUIChatWindow::FS_WHISPER_NORMAL, CGUIChatWindow::WHISPER );
				else
					_GetChatWindow()->InsertText( NULL, stRecvMsg.sChatData, CGUIChatWindow::FS_WHISPER_NORMAL, CGUIChatWindow::WHISPER );
				pPlayer->GetEtcInfo()->SetChatMsg( stRecvMsg.sChatData, CGUIChatWindow::FS_WHISPER_NORMAL );
				_GetAvatar()->GetChatInfo()->SetLatestReceiveWhisperPlayer( pPlayer );
			}
			pPlayer->SetReceiveNetMsg( "Chat : Whisper" );
		}
		else
		{
			if( stRecvMsg.bFiltering )
				_GetChatWindow()->InsertText( "??????", stRecvMsg.sChatData, CGUIChatWindow::FS_WHISPER_NORMAL, CGUIChatWindow::WHISPER );
			else
				_GetChatWindow()->InsertText( NULL, stRecvMsg.sChatData, CGUIChatWindow::FS_WHISPER_NORMAL, CGUIChatWindow::WHISPER );

			if( !stRecvMsg.bFiltering )
			{
				char pName[64];
				ZeroMemory( pName, 64 );
				GetNameFromChatMessage( pName, stRecvMsg.sChatData );
				_GetAvatar()->GetChatInfo()->SetLatestReceiveWhisperPlayer( pName );
			}
		}
	}
	// 일반
	else
	{
		CPlayer * pPlayer = _GetCharMgr()->GetPlayer( stRecvMsg.dwSenderSerial );
		if( pPlayer )
		{
			if( pPlayer->IsEqualCharType( CTI_AVATAR ) )
			{
				_GetChatWindow()->InsertText( NULL, stRecvMsg.sChatData, CGUIChatWindow::FS_AVATAR );
				pPlayer->GetEtcInfo()->SetChatMsg( stRecvMsg.sChatData, CGUIChatWindow::FS_AVATAR );
			}
			else
			{
				if( stRecvMsg.bFiltering )
					_GetChatWindow()->InsertText( pPlayer->GetName(), stRecvMsg.sChatData, CGUIChatWindow::FS_NORMAL );
				else
					_GetChatWindow()->InsertText( NULL, stRecvMsg.sChatData, CGUIChatWindow::FS_NORMAL );
				pPlayer->GetEtcInfo()->SetChatMsg( stRecvMsg.sChatData, CGUIChatWindow::FS_NORMAL );
			}
			pPlayer->SetReceiveNetMsg( "Chat : Normal" );
		}
		else
		{
			if( stRecvMsg.bFiltering )
				_GetChatWindow()->InsertText( "??????", stRecvMsg.sChatData, CGUIChatWindow::FS_NORMAL );
			else
				_GetChatWindow()->InsertText( NULL, stRecvMsg.sChatData, CGUIChatWindow::FS_NORMAL );
		}
	}

	/*
	#define chat_message_receipt_udp	8
	struct _chat_message_receipt_udp {	

	//#define CHAT_TYPE_OPERATOR		0 //운영자..
	//#define CHAT_TYPE_CIRCLE		1 //주변전체에..
	//#define CHAT_TYPE_WHISPER		2 //시리얼번호를 아는상대와 일대일..
	//#define CHAT_TYPE_FAR_WHISPER	3 //시리얼번호를 모르는상대와 일대일..
	//#define CHAT_TYPE_PARTY			4 //파티원들에게..
	//#define CHAT_TYPE_RACE		5    //동족에게

		BYTE	byMessageType;	
		DWORD	dwSenderSerial;
		BYTE	byRaceCode;			//0xFF면 전체 종족 메세지 add(4/29)
		bool	bFiltering;			//true면 이하 생략..	//add(4/29)

		BYTE bySize;
		char sChatData[max_message_size];

		int size()
		{
			if(bySize >= max_message_size) bySize = 0;
			return sizeof(*this)-(max_message_size-bySize);
		}
	};*/
}

// 채팅 메세지 수신 ( 아바타의 시야에 없는 사람 )
void
CNetworkMgr::ChatMsg_AnnounMessageReceipt_udp( char * pi_pMsg )
{
	/*
	if( !_GetChatWindow() )
		return;

	_announ_message_receipt_udp l_sRecv;
	memcpy( &l_sRecv, pi_pMsg, sizeof( _announ_message_receipt_udp ) );

	// 서버의 시스템 메세지임
	if( l_sRecv.byMessageType == CHAT_TYPE_OPERATOR )
	{
		_SetNotifyMsg( l_sRecv.sChatData );
	}
	// 종족 메세지
	else if( l_sRecv.byMessageType == CHAT_TYPE_RACE )
	{
		_GetChatWindow()->InsertText( NULL, l_sRecv.sChatData, CGUIChatWindow::FS_RACE );
	}
	// 파티 메세지
	else if( l_sRecv.byMessageType == CHAT_TYPE_PARTY )
	{
		CPlayer * l_pPlayer = ( CPlayer * )_GetCharMgr()->GetCharacterByName( CTI_PLAYER, l_sRecv.szSenderName );
		if( l_pPlayer )
		{
			if( l_pPlayer->GetCharTypeID() == CTI_AVATAR )
			{
				_GetChatWindow()->InsertText( NULL, l_sRecv.sChatData, CGUIChatWindow::FS_PARTY_AVATAR, CGUIChatWindow::PARTY );
				l_pPlayer->GetEtcInfo()->SetChatMsg( l_sRecv.sChatData, CGUIChatWindow::FS_PARTY_AVATAR );
			}
			else
			{
				_GetChatWindow()->InsertText( NULL, l_sRecv.sChatData, CGUIChatWindow::FS_PARTY_NORMAL, CGUIChatWindow::PARTY );
				l_pPlayer->GetEtcInfo()->SetChatMsg( l_sRecv.sChatData, CGUIChatWindow::FS_PARTY_NORMAL );
			}
		}
		else
		{
			_GetChatWindow()->InsertText( NULL, l_sRecv.sChatData, CGUIChatWindow::FS_PARTY_NORMAL, CGUIChatWindow::PARTY );
		}
	}
	// 귓속말
	else if( l_sRecv.byMessageType == CHAT_TYPE_WHISPER )
	{
		CPlayer * l_pPlayer = ( CPlayer * )_GetCharMgr()->GetCharacterByName( CTI_PLAYER, l_sRecv.szSenderName );
		if( l_pPlayer )
		{
			if( l_pPlayer->GetCharTypeID() == CTI_AVATAR )
			{
				_GetChatWindow()->InsertText( NULL, l_sRecv.sChatData, CGUIChatWindow::FS_WHISPER_AVATAR, CGUIChatWindow::WHISPER );
				l_pPlayer->GetEtcInfo()->SetChatMsg( l_sRecv.sChatData, CGUIChatWindow::FS_WHISPER_AVATAR );
			}
			else
			{
				_GetChatWindow()->InsertText( NULL, l_sRecv.sChatData, CGUIChatWindow::FS_WHISPER_NORMAL, CGUIChatWindow::WHISPER );
				l_pPlayer->GetEtcInfo()->SetChatMsg( l_sRecv.sChatData, CGUIChatWindow::FS_WHISPER_NORMAL );
				_GetAvatar()->GetChatInfo()->SetLatestReceiveWhisperPlayer( l_pPlayer );
			}
		}
		else
		{
			_GetChatWindow()->InsertText( NULL, l_sRecv.sChatData, CGUIChatWindow::FS_WHISPER_NORMAL, CGUIChatWindow::WHISPER );
			_GetAvatar()->GetChatInfo()->SetLatestReceiveWhisperPlayer( l_sRecv.szSenderName );
		}
	}
	// 귓속말 ( 모르는 사람 )
	else if( l_sRecv.byMessageType == CHAT_TYPE_FAR_WHISPER )
	{
		_GetChatWindow()->InsertText( NULL, l_sRecv.sChatData, CGUIChatWindow::FS_WHISPER_NORMAL, CGUIChatWindow::WHISPER );
		_GetAvatar()->GetChatInfo()->SetLatestReceiveWhisperPlayer( l_sRecv.szSenderName );
	}
	// 일반
	else
	{
		CPlayer * l_pPlayer = ( CPlayer * )_GetCharMgr()->GetCharacterByName( CTI_PLAYER, l_sRecv.szSenderName );
		if( l_pPlayer )
		{
			if( l_pPlayer->GetCharTypeID() == CTI_AVATAR )
			{
				_GetChatWindow()->InsertText( NULL, l_sRecv.sChatData, CGUIChatWindow::FS_AVATAR );
				l_pPlayer->GetEtcInfo()->SetChatMsg( l_sRecv.sChatData, CGUIChatWindow::FS_AVATAR );
			}
			else
			{
				_GetChatWindow()->InsertText( NULL, l_sRecv.sChatData, CGUIChatWindow::FS_NORMAL );
				l_pPlayer->GetEtcInfo()->SetChatMsg( l_sRecv.sChatData, CGUIChatWindow::FS_NORMAL );
			}
		}
		else
		{
			_GetChatWindow()->InsertText( NULL, l_sRecv.sChatData, CGUIChatWindow::FS_NORMAL );
		}
	}*/

	/*
	#define announ_message_receipt_udp	18
	struct _announ_message_receipt_udp {

	//#define CHAT_TYPE_OPERATOR		0 //운영자..
	//#define CHAT_TYPE_CIRCLE		1 //주변전체에..
	//#define CHAT_TYPE_WHISPER		2 //시리얼번호를 아는상대와 일대일..
	//#define CHAT_TYPE_FAR_WHISPER	3 //시리얼번호를 모르는상대와 일대일..
	//#define CHAT_TYPE_PARTY			4 //파티원들에게..
	//#define CHAT_TYPE_RACE		5    //동족에게
	//#define CHAT_TYPE_MGRWSP		6 //운영자귓말

		BYTE byMessageType;	
		char szSenderName[max_name_len+1];	//NULL 포함..

		BYTE bySize;
		char sChatData[max_message_size];

		int size()
		{
			return sizeof(*this)-(max_message_size-bySize);
		}
	};*/
}
