////////////////////////////////////////////////////////////////////////////////
//
// CGameEventBoard Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../../Manager/CCharacterMgr.h"
#include <deque>

#include "CGameEventBoard.h"

CGameEventBoard * g_pGameEventBoard = NULL;

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CGameEvent::CGameEvent()
{
	m_byType	= ID_INVALID;
	m_dwIndex	= ID_INVALID;
}

CGameEvent::~CGameEvent()
{
}

BOOL
CGameEvent::CheckWhetherShouldBeErase( CGameEvent & pi_clsOrgEvent )
{
	return FALSE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CPartyEvent::CPartyEvent()
{
	m_byPartyEventType	= ID_INVALID;

	ZeroMemory( m_pApplicantName, MAX_NAME_LENGTH );
	m_dwApplicantIndex	= ID_INVALID;
}

CPartyEvent::~CPartyEvent()
{
}

CGameEvent *
CPartyEvent::New( void )
{
	return ( dynamic_cast< CGameEvent * >( new CPartyEvent ) );
}

CGameEvent *
CPartyEvent::Clone( void )
{
	return ( dynamic_cast< CGameEvent * >( new CPartyEvent( *this ) ) );
}

BOOL
CPartyEvent::CheckWhetherShouldBeErase( CGameEvent & pi_clsOrgEvent )
{
	if( !pi_clsOrgEvent.IsEqualType( pi_clsOrgEvent.GetType() ) )
		return FALSE;

	CPartyEvent * l_pOrgEvent = static_cast< CPartyEvent * >( &pi_clsOrgEvent );
	if( IsEqualPartyEventType( l_pOrgEvent->GetPartyEventType() ) &&
		IsEqualApplicant( l_pOrgEvent->GetApplicantIndex() ) )
		return TRUE;

	return FALSE;
}

void
CPartyEvent::SetApplicant( CCharacter * pi_pChar )
{
	if( pi_pChar )
	{
		_strcpy( m_pApplicantName, pi_pChar->GetName(), MAX_NAME_LENGTH );
		m_dwApplicantIndex = pi_pChar->GetIndex();
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CTradeEvent::CTradeEvent()
{
}

CTradeEvent::~CTradeEvent()
{
}

CGameEvent *
CTradeEvent::New( void )
{
	return ( dynamic_cast< CGameEvent * >( new CTradeEvent ) );
}

CGameEvent *
CTradeEvent::Clone( void )
{
	return ( dynamic_cast< CGameEvent * >( new CTradeEvent( *this ) ) );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CClassEvent::CClassEvent()
{
	m_byClassEventType	= ID_INVALID;
}

CClassEvent::~CClassEvent()
{
}

CGameEvent *
CClassEvent::New( void )
{
	return ( dynamic_cast< CGameEvent * >( new CClassEvent ) );
}

CGameEvent *
CClassEvent::Clone( void )
{
	return ( dynamic_cast< CGameEvent * >( new CClassEvent( *this ) ) );
}


BOOL
CClassEvent::CheckWhetherShouldBeErase( CGameEvent & pi_clsOrgEvent )
{
	return pi_clsOrgEvent.IsEqualType( pi_clsOrgEvent.GetType() );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CGameEventBoard::CGameEventBoard()
{
	Clear();
}

CGameEventBoard::~CGameEventBoard()
{
	for( EVENT_DEQUE_ITER i = m_dequeEvent.begin(); i != m_dequeEvent.end(); )
		RemoveEvent( i );
}

void
CGameEventBoard::Clear( void )
{
	m_dwLastestEventPushTime	= ID_INVALID;
	m_dwCurrentEventIndex		= ID_INVALID;

	m_dwAllEventCount			= 0;

	ZeroMemory( m_pMessage, MAX_MESSAGE_LENGTH );

	for( EVENT_DEQUE_ITER i = m_dequeEvent.begin(); i != m_dequeEvent.end(); )
		RemoveEvent( i );
}

void
CGameEventBoard::InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY, CGUIScreen * pi_pUiScreen )
{
	m_uiEventWindow.Init();
	m_uiEventWindow.SetID( pi_dwID );
	m_uiEventWindow.SetPos( pi_nPosX, pi_nPosY );

	if( pi_pUiScreen ) pi_pUiScreen->Add( &m_uiEventWindow );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CGameEventBoard::PushEvent( CGameEvent & pi_clsOrgEvent )
{
	if( !CheckEventBeforePush( pi_clsOrgEvent ) )
		return FALSE;

	CGameEvent * pNewEvent = pi_clsOrgEvent.Clone();
	pNewEvent->SetIndex( ++m_dwAllEventCount );

	m_dequeEvent.push_front( pNewEvent );
	m_dwLastestEventPushTime = _GetTime();

	m_uiEventWindow.InsertEvent( 0, pNewEvent->GetIndex(),
								 ( CGUIEventWindow::EventType )pNewEvent->GetType() );
	m_uiEventWindow.SetBlink( pNewEvent->GetIndex() );

	if( !m_uiEventWindow.IsShow() )
		m_uiEventWindow.Open();

	return TRUE;
}

// 이벤트 푸쉬전에 각 이벤트타입별 체크.
BOOL
CGameEventBoard::CheckEventBeforePush( CGameEvent & pi_clsOrgEvent )
{
	if( pi_clsOrgEvent.IsEqualType( GET_PARTY ) )
	{
		return CheckEventBeforePush_Party( pi_clsOrgEvent );
	}
	else if( pi_clsOrgEvent.IsEqualType( GET_TRADE ) )
	{
		return CheckEventBeforePush_Trade( pi_clsOrgEvent );
	}
	else if( pi_clsOrgEvent.IsEqualType( GET_CLASS ) )
	{
		return CheckEventBeforePush_Class( pi_clsOrgEvent );
	}

	return TRUE;
}

// 이벤트 체크 - 동일한 이벤트가 있다면 삭제한다.
BOOL
CGameEventBoard::CheckEventBeforePush_Party( CGameEvent & pi_clsOrgEvent )
{
	// deque는 list와 erase 방식이 약간 틀리다.
	// list		=>	for( ITER i = m_List.begin(); i != m_List.end(); ++i )
	//				{
	//					if( l_bErasable )
	//					{
	//						i = m_List.erase( i );
	//						--i;
	//					}
	//				}
	// deque	=>	for( ITER i = m_Deque.begin(); i != m_Deque.end(); )
	//				{
	//					if( l_bErasable )
	//						i = m_Deque.erase( i );
	//					else
	//						++i;
	//				}
	for( EVENT_DEQUE_ITER i = m_dequeEvent.begin(); i != m_dequeEvent.end(); )
	{
		if( ( *i )->CheckWhetherShouldBeErase( pi_clsOrgEvent ) )
			RemoveEvent( i );
		else
			++i;
	}

	return TRUE;
}

BOOL
CGameEventBoard::CheckEventBeforePush_Trade( CGameEvent & pi_clsOrgEvent )
{
	return CheckEventBeforePush_Party( pi_clsOrgEvent );
}

BOOL
CGameEventBoard::CheckEventBeforePush_Class( CGameEvent & pi_clsOrgEvent )
{
	if( !CheckEventBeforePush_Party( pi_clsOrgEvent ) )
		return FALSE;

	PostMessage( _GetHWND(), WM_ENABLE_CLASS_CHANGE, UII_CLASS_CHANGE_WINDOW, 0 );

	return TRUE;
}

//==============================================================================

BOOL
CGameEventBoard::PopEvent( void )
{
	if( m_dequeEvent.empty() )
		return FALSE;

	CGameEvent * l_Event = m_dequeEvent.back();
	if( l_Event )
	{
		m_uiEventWindow.RemoveEvent( l_Event->GetIndex() );
		delete l_Event;
	}

	if( m_dequeEvent.empty() )
		m_uiEventWindow.Close();

	m_dequeEvent.pop_back();

	return TRUE;
}

//==============================================================================

BOOL
CGameEventBoard::RemoveEvent( EVENT_DEQUE_ITER & pi_iterEvent )
{
	CGameEvent * l_pEvent = *pi_iterEvent;

	m_uiEventWindow.RemoveEvent( l_pEvent->GetIndex() );
	delete l_pEvent;
	pi_iterEvent = m_dequeEvent.erase( pi_iterEvent );

	m_dwCurrentEventIndex = ID_INVALID;

	return TRUE;
}

void
CGameEventBoard::ClearAllEvent( BYTE pi_byEventType )
{
	for( EVENT_DEQUE_ITER i = m_dequeEvent.begin(); i != m_dequeEvent.end(); )
	{
		if( ( *i )->IsEqualType( pi_byEventType ) || ( pi_byEventType == 0xFF ) )
			RemoveEvent( i );
		else
			++i;
	}
}

void
CGameEventBoard::ClearEvent( BYTE pi_byEventType, BYTE pi_bySubType )
{
	for( EVENT_DEQUE_ITER i = m_dequeEvent.begin(); i != m_dequeEvent.end(); )
	{
		if( ( *i )->IsEqualType( pi_byEventType ) && ( *i )->IsEqualSubType( pi_bySubType ) )
			RemoveEvent( i );
		else
			++i;
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CGameEventBoard::FrameMove( void )
{
	if( m_dequeEvent.empty() )
		return;

	if( m_dwLastestEventPushTime != ID_INVALID )
	{
		static const DWORD LATEST_EVENT_NOTIFY_LIMIT_TIME = 5000;

		if( ( _GetTime() - m_dwLastestEventPushTime ) >= LATEST_EVENT_NOTIFY_LIMIT_TIME )
		{
			CGameEvent * l_Event = m_dequeEvent.front();
			if( l_Event )
				m_uiEventWindow.SetUnBlink( l_Event->GetIndex() );

			m_dwLastestEventPushTime = ID_INVALID;
		}
	}
}

//==============================================================================

LRESULT
CGameEventBoard::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case WM_EVENTWND_MOUSE_HOVER :
			MouseHoverOnEventIcon( wParam );
			break;

		case WM_EVENTWND_CLICK_OK :
			ClickOKButton( wParam );
			break;

		case WM_EVENTWND_CLICK_CANCEL :
			ClickCancelButton( wParam );
			break;
	}

	return 0;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CGameEventBoard::MouseHoverOnEventIcon( DWORD pi_dwEventIndex )
{
	if( m_dequeEvent.empty() )
		return FALSE;

	for( EVENT_DEQUE_ITER i = m_dequeEvent.begin(); i != m_dequeEvent.end(); ++i )
	{
		if( ( *i )->IsEqual( pi_dwEventIndex ) )
		{
			m_dwCurrentEventIndex = pi_dwEventIndex;

			switch( ( *i )->GetType() )
			{
				case GET_TUTORIAL :
					break;
				case GET_TRADE :
					return MouseHoverOnEventIcon_Trade( *i );
				case GET_PARTY :
					return MouseHoverOnEventIcon_Party( *i );
				case GET_GUILD :
					break;
				case GET_COMMAND :
					break;
				case GET_QUEST :
					break;
				case GET_CLASS :
					return MouseHoverOnEventIcon_Class( *i );
					break;
			}

			return TRUE;
		}
	}

	return FALSE;
}

BOOL
CGameEventBoard::MouseHoverOnEventIcon_Party( CGameEvent * pi_pHoverEvent )
{
	if( !pi_pHoverEvent )
		return FALSE;

	CPartyEvent *	pEvent = static_cast< CPartyEvent * >( pi_pHoverEvent );
	CGUITextInfo	guiTextInfo;

	SetPartyInviteOrJoinMessage( pEvent );
	guiTextInfo.SetText( m_pMessage );
	guiTextInfo.SetFont( &m_uiFontInfo[pEvent->GetType()] );

	m_uiEventWindow.OpenEventInfoBoard( pEvent->GetIndex(), &guiTextInfo, CGUIEventWindow::YesOrNo );

	return TRUE;
}

void
CGameEventBoard::SetPartyInviteOrJoinMessage( CPartyEvent * pi_pHoverEvent )
{
	if( !pi_pHoverEvent )
		return;

	if( pi_pHoverEvent->IsEqualPartyEventType( CPartyEvent::INVITE_AVATAR_TO_OTHER_PARTY ) )
	{
		sprintf( m_pMessage, "%s님이 파티에 초대 하였습니다. 합류 하시겠습니까?",
				 pi_pHoverEvent->GetApplicantName() );
	}
	else if( pi_pHoverEvent->IsEqualPartyEventType( CPartyEvent::PLAYER_JOIN_MY_PARTY ) )
	{
		sprintf( m_pMessage, "%s님이 현재 파티에 합류 요청을 하였습니다. 허락하시겠습니까?",
				 pi_pHoverEvent->GetApplicantName() );
	}
}

BOOL
CGameEventBoard::MouseHoverOnEventIcon_Trade( CGameEvent * pi_pHoverEvent )
{
	if( !pi_pHoverEvent )
		return FALSE;

	CTradeEvent *	pEvent = static_cast< CTradeEvent * >( pi_pHoverEvent );
	CGUITextInfo	guiTextInfo;

	sprintf( m_pMessage, "%s님으로부터 아이템 트레이드 요청이 왔습니다. 허락하시겠습니까?",
			 pEvent->GetApplicantName() );
	guiTextInfo.SetText( m_pMessage );
	guiTextInfo.SetFont( &m_uiFontInfo[pEvent->GetType()] );

	m_uiEventWindow.OpenEventInfoBoard( pEvent->GetIndex(), &guiTextInfo, CGUIEventWindow::YesOrNo );

	return TRUE;
}

BOOL
CGameEventBoard::MouseHoverOnEventIcon_Class( CGameEvent * pi_pHoverEvent )
{
	if( !pi_pHoverEvent )
		return FALSE;

	CClassEvent *	pEvent = static_cast< CClassEvent * >( pi_pHoverEvent );
	CGUITextInfo	guiTextInfo;

	sprintf( m_pMessage, "%s님께 전직할 수 있는 자격이 주어졌습니다. 베이스내의 종족관리인을 통해 전직할 수 있습니다.", _GetAvatar()->GetName() );
	guiTextInfo.SetText( m_pMessage );
	guiTextInfo.SetFont( &m_uiFontInfo[pEvent->GetType()] );

	m_uiEventWindow.OpenEventInfoBoard( pEvent->GetIndex(), &guiTextInfo, CGUIEventWindow::OK );

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CGameEventBoard::ClickOKButton( DWORD pi_dwEventIndex )
{
	if( m_dequeEvent.empty() )
		return FALSE;

	BOOL bResult = FALSE;

	for( EVENT_DEQUE_ITER i = m_dequeEvent.begin(); i != m_dequeEvent.end(); ++i )
	{
		if( ( *i )->IsEqual( pi_dwEventIndex ) )
		{
			switch( ( *i )->GetType() )
			{
				case GET_TUTORIAL	:
				case GET_TRADE		:
					bResult = ClickOKButton_Trade( *i ); break;
				case GET_PARTY		:
					bResult = ClickOKButton_Party( *i ); break;
				case GET_GUILD		:
				case GET_COMMAND	:
				case GET_QUEST		:
					break;
				case GET_CLASS		:
					bResult = ClickOKButton_Class( *i ); break;
			}

			RemoveEvent( i );
			return bResult;
		}
	}

	return bResult;
}

BOOL
CGameEventBoard::ClickOKButton_Party( CGameEvent * pi_pHoverEvent )
{
	if( !pi_pHoverEvent )
		return FALSE;

	CPartyEvent * pEvent = static_cast< CPartyEvent * >( pi_pHoverEvent );

	if( pEvent->IsEqualPartyEventType( CPartyEvent::PLAYER_JOIN_MY_PARTY ) )
		PostMessage( _GetHWND(), WM_PARTY_ACCEPT_JOIN, pEvent->GetApplicantIndex(), 0 );
	else if( pEvent->IsEqualPartyEventType( CPartyEvent::INVITE_AVATAR_TO_OTHER_PARTY ) )
		PostMessage( _GetHWND(), WM_PARTY_ACCEPT_INVITE, pEvent->GetApplicantIndex() , 0 );

	return TRUE;
}

BOOL
CGameEventBoard::ClickOKButton_Trade( CGameEvent * pi_pHoverEvent )
{
	if( !pi_pHoverEvent )
		return FALSE;

	CTradeEvent * pEvent = static_cast< CTradeEvent * >( pi_pHoverEvent );

	if( pEvent->IsEqualTradeEventType( CTradeEvent::RECEIVE_TRADE_REQUEST ) )
		PostMessage( _GetHWND(), WM_ACCEPT_TRADE_REQUEST, UII_TRADE_WINDOW, 0 );

	return TRUE;
}

BOOL
CGameEventBoard::ClickOKButton_Class( CGameEvent * pi_pHoverEvent )
{
	if( !pi_pHoverEvent )
		return FALSE;

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CGameEventBoard::ClickCancelButton( DWORD pi_dwEventIndex )
{
	if( m_dequeEvent.empty() )
		return FALSE;

	for( EVENT_DEQUE_ITER i = m_dequeEvent.begin(); i != m_dequeEvent.end(); ++i )
	{
		if( ( *i )->IsEqual( pi_dwEventIndex ) )
			return RemoveEvent( i );
	}

	return TRUE;
}
