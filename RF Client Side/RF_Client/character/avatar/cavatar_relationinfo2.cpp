////////////////////////////////////////////////////////////////////////////////
//
// CAvatar Relation Infomation Implementation 2
//
////////////////////////////////////////////////////////////////////////////////

#include "../../GameObject/Manager/CCharacterMgr.h"
#include "../../GameObject/Manager/CTargetMgr.h"

#include "../../CCamera.h"

#include "../../GameObject/Board/General/CPartyBoard.h"
#include "../../UserInterface/GameWindow/GUIChatWindow.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Structure          //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CHAT_INFO::Init( void )
{
	ZeroMemory( this, sizeof( CHAT_INFO ) );
}

BOOL
CHAT_INFO::IsEmptyString( char * pi_pMessage )
{
	if( !pi_pMessage )
		return FALSE;

	for( int i = 0; i < strlen( pi_pMessage ); ++i )
	{
		if( pi_pMessage[i] != ' ' )
		{
			return FALSE;
		}
	}

	return TRUE;
}

BOOL
CHAT_INFO::SendMessage( char * pi_pMessage )
{
	if( !pi_pMessage )
		return FALSE;

	BYTE		l_byMessageType;
	char		l_pMsg[256], l_pWhisperCharName[32], l_pBuf[64];
	CCharacter*	l_pWhisperChar = NULL;

	//--------------------------------------------------------------------------
	// save message
	if( IsEmptyString( pi_pMessage ) )
		return FALSE;

	SaveSendMessage( pi_pMessage );

	//--------------------------------------------------------------------------
	// header
	ZeroMemory( m_pMessageType, 32 );

	if( pi_pMessage[0] == '/' )
	{
		ZeroMemory( l_pBuf, 64 );

		for( int i = 1; pi_pMessage[i] != 0; ++i )
		{
			if( ( pi_pMessage[i] == ' ' ) || ( i >= 63 ) )
				break;
			else
				l_pBuf[i-1] = pi_pMessage[i];
		}

		if( strcmp( l_pBuf, "파티" ) == 0 )
		{
			if( _GetAvatar()->GetPartyMemberType() == PMT_NONE )
				l_byMessageType = CMT_NORMAL;
			else
				l_byMessageType = CMT_PARTY;

			if( IsEmptyString( &pi_pMessage[i + 1] ) )
				return FALSE;
			sprintf( l_pMsg, "%s : %s", _GetAvatar()->GetName(), &pi_pMessage[i + 1] );
		}
		else if( strcmp( l_pBuf, "길드" ) == 0 )
		{
			l_byMessageType = CMT_GUILD;

			if( IsEmptyString( &pi_pMessage[i + 1] ) )
				return FALSE;
			sprintf( l_pMsg, "%s : %s", _GetAvatar()->GetName(), &pi_pMessage[i + 1] );
		}
		else if( strcmp( l_pBuf, "국가" ) == 0 )
		{
			l_byMessageType = CMT_NATION;

			if( IsEmptyString( &pi_pMessage[i + 1] ) )
				return FALSE;
			sprintf( l_pMsg, "%s : %s", _GetAvatar()->GetName(), &pi_pMessage[i + 1] );
		}
		else if( strcmp( l_pBuf, "종족" ) == 0 )
		{
			if( _GetAvatar()->IsEnable_SendMsgToRace() )
			{
				l_byMessageType = CMT_RACE;

				if( IsEmptyString( &pi_pMessage[i + 1] ) )
					return FALSE;
				sprintf( l_pMsg, "%s : %s", _GetAvatar()->GetName(), &pi_pMessage[i + 1] );
			}
			else
			{
				_SetSystemMsg( "같은 종족 모두에게 메세지를 보낼 수 없습니다." );
				return FALSE;
			}
		}
		else if( _GetNetworkMgr()->GetAvatarGrade() >= USER_DEGREE_GID )
		{
			if( strcmp( l_pBuf, "마을" ) == 0 )
			{
				_GetNetworkMgr()->PositionMsg_GotoBaseportalRequest_clzo( ID_INVALID );
				return TRUE;
			}
			else if( strcmp( l_pBuf, "공지" ) == 0 )
			{
				l_byMessageType = CMT_SYSTEM;
				sprintf( l_pMsg, "%s : %s", l_pBuf, &pi_pMessage[i + 1] );
			}
			else if( strcmp( l_pBuf, "팁" ) == 0 )
			{
				l_byMessageType = CMT_SYSTEM;
				sprintf( l_pMsg, "%s : %s", l_pBuf, &pi_pMessage[i + 1] );
			}
			else if( strcmp( l_pBuf, "이동" ) == 0 )
			{
				_GetNetworkMgr()->PositionMsg_GotoAvatorRequest_clzo( &pi_pMessage[i + 1] );
				return TRUE;
			}
			else if( strcmp( l_pBuf, "정보" ) == 0 )
			{
				CCharacter * l_pChar;
				if( ( pi_pMessage[i + 1] >= '0' ) && ( pi_pMessage[i + 1] <= '9' ) )
				{
					int l_nCharType = pi_pMessage[i + 1] - '0';
					int l_nCharIndex = atoi( &pi_pMessage[i + 3] );
					l_pChar = _GetCharMgr()->GetCharacter( l_nCharType, l_nCharIndex );
				}
				else
				{
					l_pChar = _GetCharMgr()->GetCharacterByName( CTI_PLAYER, &pi_pMessage[i + 1] );
				}
				_GetNetworkMgr()->SyncMsg_ObjectServerPosRequest_clzo( l_pChar );
				return TRUE;
			}
			else
			{
				_strcpy( l_pWhisperCharName, l_pBuf );

				l_byMessageType = CMT_WHISPER_BY_NAME;

				if( IsEmptyString( &pi_pMessage[i + 1] ) )
					return FALSE;

				sprintf( l_pMsg, "%s : %s", _GetAvatar()->GetName(), &pi_pMessage[i + 1] );

				// save whisper target
				sprintf( m_pLatestSendWhisperName, l_pWhisperCharName );
				SaveWhisperList( l_pWhisperCharName );
			}
		}
		else
		{
			_strcpy( l_pWhisperCharName, l_pBuf );

			l_byMessageType = CMT_WHISPER_BY_NAME;

			if( IsEmptyString( &pi_pMessage[i + 1] ) )
				return FALSE;

			sprintf( l_pMsg, "%s : %s", _GetAvatar()->GetName(), &pi_pMessage[i + 1] );

			// save whisper target
			sprintf( m_pLatestSendWhisperName, l_pWhisperCharName );
			SaveWhisperList( l_pWhisperCharName );
		}

		sprintf( m_pMessageType, "/%s ", l_pBuf );
	}
	else if( pi_pMessage[0] == '\'' )
	{
		if( m_pLatestReceiveWhisperName[0] != 0 )
		{
			if( IsEmptyString( &pi_pMessage[1] ) )
				return FALSE;

			l_byMessageType = CMT_WHISPER_BY_NAME;
			ZeroMemory( l_pWhisperCharName, 32 );
			memcpy( l_pWhisperCharName, m_pLatestReceiveWhisperName, strlen( m_pLatestReceiveWhisperName ) );
			sprintf( l_pMsg, "%s : %s", _GetAvatar()->GetName(), &pi_pMessage[1] );
		}

		_strcpy( m_pMessageType, "'", 32 );
	}
	else if( _GetNetworkMgr()->GetAvatarGrade() >= USER_DEGREE_GID )
	{
		if( pi_pMessage[0] == '%' )
		{
			l_byMessageType = CMT_CHEAT;
			_strcpy( l_pMsg, &pi_pMessage[1], 256 );
		}
		else if( pi_pMessage[0] == '!' )
		{
			l_byMessageType = CMT_SYSTEM;
			sprintf( l_pMsg, "%s : %s", "공지", &pi_pMessage[1] );
		}
		else if( pi_pMessage[0] == '@' )
		{
			l_byMessageType = CMT_SYSTEM;
			sprintf( l_pMsg, "%s : %s", "팁", &pi_pMessage[1] );
		}
		else if( pi_pMessage[0] == '#' )
		{
			l_byMessageType = CMT_WHISPER_BY_GM;
			sprintf( l_pMsg, "GM - %s : %s", _GetAvatar()->GetName(), &pi_pMessage[1] );
		}
		else
		{
			l_byMessageType = CMT_NORMAL;
			sprintf( l_pMsg, "%s : %s", _GetAvatar()->GetName(), pi_pMessage );
		}
	}
	else
	{
		if( pi_pMessage[0] == '%' )
			return FALSE;

		l_byMessageType = CMT_NORMAL;
		sprintf( l_pMsg, "%s : %s", _GetAvatar()->GetName(), pi_pMessage );
	}

	//--------------------------------------------------------------------------
	// send message
	switch( l_byMessageType )
	{
		case CMT_NORMAL :
			_GetNetworkMgr()->ChatMsg_ChatCircleRequest_clzo( l_pMsg );
			break;

		case CMT_PARTY :
			_GetNetworkMgr()->ChatMsg_ChatPartyRequest_clzo( l_pMsg );
			break;

		case CMT_GUILD :
			break;

		case CMT_NATION :
			break;

		case CMT_RACE :
			_GetNetworkMgr()->ChatMsg_ChatRaceRequest_clzo( l_pMsg );
			break;

		case CMT_WHISPER :
		case CMT_WHISPER_BY_NAME :
			_GetNetworkMgr()->ChatMsg_ChatFarRequest_clzo( l_pWhisperCharName, l_pMsg );
			break;

		case CMT_WHISPER_BY_GM :
			_GetNetworkMgr()->ChatMsg_ChatMgrWhisperRequest_clzo( l_pMsg );
			break;

		case CMT_CHEAT :
			_GetNetworkMgr()->ChatMsg_ChatCheatRequest_clzo( l_pMsg );
			break;

		case CMT_SYSTEM :
			_GetNetworkMgr()->ChatMsg_ChatOperatorRequest_clzo( l_pMsg );
			break;
	};

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CHAT_INFO::SaveSendMessage( char * pi_pMessage )
{
	if( !pi_pMessage )
		return;

	int i;

	// 똑같은 메세지가 이미 있는지를 검사
	for( i = 0; i < m_byMaxSendMessageNum; ++i )
	{
		if( strcmp( ( char * )m_listSendMessage[i], pi_pMessage ) == 0 )
			return;
	}

	// 메세지 저장
	if( ( m_byMaxSendMessageNum + 1 ) > MAX_SEND_MESSAGE )
	{
		m_byMaxSendMessageNum = MAX_SEND_MESSAGE - 1;

		for( i = 0; i < MAX_SEND_MESSAGE - 1; ++i )
			_strcpy( ( char * )m_listSendMessage[i], ( char * )m_listSendMessage[i+1], 256 );
	}

	++m_byMaxSendMessageNum;
	_strcpy( ( char * )m_listSendMessage[m_byMaxSendMessageNum - 1], pi_pMessage, 256 );

	m_byCurrentSendMessageNo = m_byMaxSendMessageNum - 1;
}

void
CHAT_INFO::DisplaySavedSendMessage( void )
{
	if( m_byMaxSendMessageNum <= 0 )
		return;

	CGUIInput * l_pInput = _GetChatInputWindow()->GetInputField();
	if( l_pInput )
	{
		l_pInput->Clear();
		l_pInput->SetText( ( char * )m_listSendMessage[m_byCurrentSendMessageNo] );
	}

	//--------------------------------------------------------------------------

	l_pInput = _GetChatWindow()->GetInputField();
	if( l_pInput )
	{
		l_pInput->Clear();
		l_pInput->SetText( ( char * )m_listSendMessage[m_byCurrentSendMessageNo] );
	}

	//--------------------------------------------------------------------------

	--m_byCurrentSendMessageNo;
	if( m_byCurrentSendMessageNo == 0xFF )
		m_byCurrentSendMessageNo = m_byMaxSendMessageNum - 1;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CHAT_INFO::SaveWhisperList( char * pi_pName )
{
	if( !pi_pName )
		return;

	int i;

	// 똑같은 메세지가 이미 있는지를 검사
	for( i = 0; i < m_byMaxSendWhisperNum; ++i )
	{
		if( strcmp( ( char * )m_listWhisper[i], pi_pName ) == 0 )
			return;
	}

	// 메세지 저장
	if( ( m_byMaxSendWhisperNum + 1 ) > MAX_SEND_MESSAGE )
	{
		m_byMaxSendWhisperNum = MAX_SEND_MESSAGE - 1;

		for( i = 0; i < MAX_SEND_MESSAGE - 1; ++i )
			_strcpy( ( char * )m_listWhisper[i], ( char * )m_listWhisper[i+1], 64 );
	}

	++m_byMaxSendWhisperNum;
	_strcpy( ( char * )m_listWhisper[m_byMaxSendWhisperNum - 1], pi_pName, 64 );

	m_byCurrentWhisperNo = m_byMaxSendWhisperNum - 1;
}

void
CHAT_INFO::DeleteWhisperList( char * pi_pName )
{
	if( !pi_pName )
		return;

	if( m_byMaxSendWhisperNum <= 0 )
		return;

	for( int i = 0; i < m_byMaxSendWhisperNum; ++i )
	{
		if( strcmp( ( char * )m_listWhisper[i], pi_pName ) == 0 )
		{
			for( int j = 0; j < MAX_SEND_MESSAGE - 1; ++j )
				_strcpy( ( char * )m_listWhisper[i], ( char * )m_listWhisper[i+1], 64 );

			--m_byMaxSendWhisperNum;
			if( m_byCurrentWhisperNo >= m_byMaxSendWhisperNum )
				m_byCurrentWhisperNo = m_byMaxSendWhisperNum - 1;
			if( m_byCurrentWhisperNo == 0xFF )
				m_byCurrentWhisperNo = 0;

			break;
		}
	}
}

void
CHAT_INFO::DisplaySavedWhisperList( void )
{
	if( m_byMaxSendWhisperNum <= 0 )
		return;

	CGUIInput * l_pInput;
	char		l_pBuf[64];

	sprintf( l_pBuf, "/%s ", ( char * )m_listWhisper[m_byCurrentWhisperNo] );

	l_pInput = _GetChatInputWindow()->GetInputField();
	if( l_pInput )
	{
		l_pInput->Clear();
		l_pInput->SetText( l_pBuf );
		l_pInput->SetCaretPos( strlen( l_pBuf ) );
	}

	//--------------------------------------------------------------------------

	l_pInput = _GetChatWindow()->GetInputField();
	if( l_pInput )
	{
		l_pInput->Clear();
		l_pInput->SetText( l_pBuf );
		l_pInput->SetCaretPos( strlen( l_pBuf ) );
	}

	//--------------------------------------------------------------------------

	--m_byCurrentWhisperNo;
	if( m_byCurrentWhisperNo == 0xFF )
		m_byCurrentWhisperNo = m_byMaxSendWhisperNum - 1;
}

void
CHAT_INFO::SetLatestReceiveWhisperPlayer( CPlayer * pi_pPlayer )
{
	m_pLatestReceiveWhisperIndex = pi_pPlayer->GetIndex();
	memcpy( m_pLatestReceiveWhisperName, pi_pPlayer->GetName(), strlen( pi_pPlayer->GetName() ) );
}

void
CHAT_INFO::SetLatestReceiveWhisperPlayer( DWORD pi_dwPlayerIndex )
{
	m_pLatestReceiveWhisperIndex = pi_dwPlayerIndex;
	ZeroMemory( m_pLatestReceiveWhisperName, 64 );
}

void
CHAT_INFO::SetLatestReceiveWhisperPlayer( char * pi_pPlayerName )
{
	m_pLatestReceiveWhisperIndex = ID_INVALID;
	memcpy( m_pLatestReceiveWhisperName, pi_pPlayerName, strlen( pi_pPlayerName ) );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Structure          //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

AFTER_IMAGE_INFO::AFTER_IMAGE_INFO()
{
	ZeroMemory( this, sizeof( AFTER_IMAGE_INFO ) );

	m_pImage = new CPlayer[MAX_AFTER_IMAGE_NUM];

	for( int i = 0; i < MAX_AFTER_IMAGE_NUM; ++i )
	{
		m_pImage[i].SetObjectTypeID( OTI_CHAR );
		m_pImage[i].SetCharTypeID( CTI_PLAYER );
	}
}

AFTER_IMAGE_INFO::~AFTER_IMAGE_INFO()
{
	delete [] m_pImage;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
AFTER_IMAGE_INFO::EnableRender( CPlayer * pi_pOrgPlayer )
{
	m_bIsRenderAfterImage = TRUE;

	for( int i = 0; i < MAX_AFTER_IMAGE_NUM; ++i )
		m_pImage[i].SetIndex( ID_INVALID );
}

void
AFTER_IMAGE_INFO::DisableRender( void )
{
	m_bIsRenderAfterImage = FALSE;
}

void
AFTER_IMAGE_INFO::Render( void )
{
	if( m_bIsRenderAfterImage )
	{
		for( int i = 0; i < MAX_AFTER_IMAGE_NUM; ++i )
		{
			if( m_pImage[i].GetIndex() != ID_INVALID )
			{
				m_pImage[i].Enable( ROSF_AFTER_IMAGE_OF_OBJECT | ROSF_RENDER_ALPHA );
				m_pImage[i].Disable( ROSF_RENDER_EFFECT | ROSF_ANIMATION_INTERPOLATION );
				m_pImage[i].Animation( m_pImage[i].GetBoneFrame() );
				m_pImage[i].SetAlphaDensity( i * ( 0.4f / MAX_AFTER_IMAGE_NUM ) );
				m_pImage[i].Render();
			}
		}
	}
}

void
AFTER_IMAGE_INFO::Animation( CPlayer * pi_pOrgPlayer )
{
	static DWORD SAVE_LIMIT_TIME = 100;

	if( m_bIsRenderAfterImage )
	{
		if( timeGetTime() - m_dwImageSavedTime >= SAVE_LIMIT_TIME )
		{
			for( int i = 0; i < MAX_AFTER_IMAGE_NUM - 1; i++ )
				CopyImage( &m_pImage[i], &m_pImage[i+1] );

			CopyImage( &m_pImage[MAX_AFTER_IMAGE_NUM-1], pi_pOrgPlayer );
			m_pImage[MAX_AFTER_IMAGE_NUM-1].SetIndex( 0 );

			m_dwImageSavedTime = timeGetTime();
		}
	}
}

void
AFTER_IMAGE_INFO::CopyImage( CPlayer * po_pTarImage, CPlayer * pi_pOrgImage )
{
	if( !po_pTarImage || !pi_pOrgImage )
		return;

	po_pTarImage->SetIndex( pi_pOrgImage->GetIndex() );
	po_pTarImage->SetDTIndex( pi_pOrgImage->GetDTIndex() );
	po_pTarImage->SetBone( pi_pOrgImage->GetDTIndex() );
	po_pTarImage->SetRace( pi_pOrgImage->GetDTIndex() );

	po_pTarImage->SetPosition( pi_pOrgImage->GetPosition() );
	po_pTarImage->SetRotY( pi_pOrgImage->GetRotY(), FALSE );

	po_pTarImage->Enable( ROSF_AFTER_IMAGE_OF_OBJECT | ROSF_RENDER_ALPHA );
	po_pTarImage->Disable( ROSF_RENDER_EFFECT | ROSF_ANIMATION_INTERPOLATION );

	po_pTarImage->PutOnEquipment( pi_pOrgImage->GetEquipItem( IEPT_WEAPON ) );
	po_pTarImage->SetCombatMode( pi_pOrgImage->GetCombatMode() );
	po_pTarImage->SetSFItem( pi_pOrgImage->GetSFItem() );
	po_pTarImage->SetAction( pi_pOrgImage->GetAction(), FALSE );

	po_pTarImage->SetBoneFrame( pi_pOrgImage->GetBoneFrame() );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Structure          //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

OBSERVER_INFO::OBSERVER_INFO()
{
	Init();
}

OBSERVER_INFO::~OBSERVER_INFO()
{
}

void
OBSERVER_INFO::Init( void )
{
	m_bIsObserver		= FALSE;
	m_byObserverMode	= OMI_NORMAL;
	m_byKeyDownCode		= 0;
}

void
OBSERVER_INFO::SetObserver( BOOL pi_bFlag )
{
	m_bIsObserver = pi_bFlag;

	if( m_bIsObserver )
	{
		SetObserverMode( m_byObserverMode );

		_GetAvatar()->Enable( ROSF_OBSERVER | ROSF_RENDER_ALPHA );
		_GetAvatar()->GetChatInfo()->SendMessage( "%투명인간" );
	}
	else
	{
		//_GetAvatar()->SetPosY( 1000000, FALSE );
		_GetAvatar()->Enable( ROSF_CHECK_Y_POS );

		_GetAvatar()->Disable( ROSF_OBSERVER | ROSF_RENDER_ALPHA );
		_GetAvatar()->SetAlphaDensity( 1.0f );

		_GetAvatar()->GetChatInfo()->SendMessage( "%안투명인간" );
	}
}

void
OBSERVER_INFO::SetObserverMode( BYTE pi_byMode )
{
	if( !m_bIsObserver )
		return;

	m_byObserverMode = pi_byMode;

	// 플라이 모드에서 노멀 모드로 바뀌었다면 캐릭터를 카메라가 있는 곳으로 이동시킨후
	// 카메라를 캐릭터에 부착시킨다.
	if( m_byObserverMode == OMI_NORMAL )
	{
		//_GetAvatar()->SetPosY( 1000000, FALSE );
		_GetAvatar()->Enable( ROSF_CHECK_Y_POS );
		_GetAvatar()->SetAlphaDensity( 0.5f );
	}
	// 플라이 모드
	else if( m_byObserverMode == OMI_FLY )
	{
		_GetAvatar()->Disable( ROSF_CHECK_Y_POS );
		_GetAvatar()->SetAlphaDensity( 0.0f );
	}
	// 애니메이션 모드
	else if( m_byObserverMode == OMI_ANIMATION )
	{
		_GetAvatar()->Disable( ROSF_CHECK_Y_POS );
		_GetAvatar()->SetAlphaDensity( 0.0f );

		if( !_GetCamera()->GetAniCamera()->IsLoadedAniCamera() )
			_GetCamera()->GetAniCamera()->LoadAniCamera( ".\\map\\lobby\\lobby.cam" );

		DWORD l_dwAniCameraIndex = _GetCamera()->GetAniCamera()->GetCameraIndex( "C01-01" );
		_GetCamera()->GetAniCamera()->SetPrePlayCamera( l_dwAniCameraIndex, 0, 64, _CAM_FLAG_LOOP );

		float l_matAniCamera[4][4];		// 애니메이션 카메라 매트릭스
		if( _GetCamera()->GetAniCamera()->PlayAniCamera( l_matAniCamera, 1.0f ) )
		{
			Vector3f l_vecAniCamera;
			R3SetCameraMatrix( l_vecAniCamera, l_matAniCamera );

			_GetAvatar()->SetPosition( l_vecAniCamera );
		}
	}
}