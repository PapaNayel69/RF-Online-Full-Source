////////////////////////////////////////////////////////////////////////////
//
// CGUIEventWindow Class Header
//
////////////////////////////////////////////////////////////////////////////
#include "GUIEventWindow.h"
#include "../../SpriteMgr.h"
#include "../../GUIController/GUIScreen.h"
#include "../../UIGlobal.h"

#include "../../../DefineMessage.h"

/*//////////////////////////////////////////////////////////////////////////

[ CGUIEventWindow ]    

//////////////////////////////////////////////////////////////////////////*/
CGUIEventWindow::CGUIEventWindow()
{
	m_bIsCreated		= FALSE;
	m_nFirstViewIndex	= 0;	

	m_byOpenedInfoIndex = 0xFF;

	m_dwSendEventID		= 0xFFFFFFFF;

	ZeroMemory( m_pEventInfoMsgStr, EVENT_MSG_BUFSIZE );
}

CGUIEventWindow::~CGUIEventWindow()
{
	CGUIEventList::iterator it;
	for( it = m_listEvent.begin(); it != m_listEvent.end(); ++it )
	{	
		delete *it;
	}	

	m_listEvent.clear();
}


void
CGUIEventWindow::Init( void )
{	
	for( int i=0; i<VIEW_EVENT_NUM; ++i )
		m_uiEventButton[i].SetSize( 30, 30 );

	m_uiUpButton.SetSize( 17, 10 );
	m_uiDownButton.SetSize( 17, 10 );	

	m_uiYESButton.SetSize( 30, 14 );
	m_uiNOButton.SetSize( 40, 14 );

	m_uiYESButton.SetLabel( "예" );
	m_uiNOButton.SetLabel( "아니오" );

	m_uiUpButton.AddMouseEventHandler( this );
	m_uiDownButton.AddMouseEventHandler( this );	

	for( i=0; i<VIEW_EVENT_NUM; ++i )
		m_uiEventButton[i].AddMouseEventHandler( this );

	m_uiEventInfoBoard.AddMouseEventHandler( this );
	m_uiYESButton.AddMouseEventHandler( this );
	m_uiNOButton.AddMouseEventHandler( this );

	m_uiEventInfoBoard.Add( &m_uiYESButton );

	m_uiEventBlock.AddMouseEventHandler( this );


	SetSprite();

	m_uiEventInfoBoard.SetARGB( 0xC0202020 );	

	m_uiYESButton.SetLabelColor( D3DCOLOR_XRGB( 212, 186, 148 ) );
	m_uiNOButton.SetLabelColor( D3DCOLOR_XRGB( 212, 186, 148 ) );

	m_uiYESButton.SetLabelHoveredColor( D3DCOLOR_XRGB( 232, 232, 232 ) );
	m_uiNOButton.SetLabelHoveredColor( D3DCOLOR_XRGB( 232, 232, 232 ) );


	m_bIsCreated = TRUE;
}

void
CGUIEventWindow::SetSprite( void )
{
	if( !m_bIsCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_GAUGE_EXP );

		l_spriteInfo.wActionNo = 2;
		l_spriteInfo.wFrameNo = 0;

		for( int i=0; i<VIEW_EVENT_NUM; ++i )
		{
			m_uiEventButton[i].SetSpriteInfo( &l_spriteInfo );
		}

		l_spriteInfo.wFrameNo = 14;
		m_uiUpButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 15;
		m_uiUpButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );

		l_spriteInfo.wFrameNo = 16;
		m_uiDownButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 17;
		m_uiDownButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );		
	}
	else
	{
		CSprite	* l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_GAUGE_EXP );
		if( !l_pSprite )
			return;

		for( int i=0; i<VIEW_EVENT_NUM; ++i )
		{
			m_uiEventButton[i].GetSpriteInfo()->pSprite = l_pSprite;
		}

		m_uiUpButton.SetSprite( l_pSprite );
		m_uiDownButton.SetSprite( l_pSprite );
	}
}

void
CGUIEventWindow::InsertEvent( WORD pi_wInsertIndex, DWORD pi_dwEventID, EventType pi_eEventType )
{
	if( pi_eEventType >= Event_Num )
		return;

	EVENT_INFO * l_pEventInfo;

	l_pEventInfo = new EVENT_INFO;
	l_pEventInfo->dwEventID		= pi_dwEventID;
	l_pEventInfo->byEventType	= pi_eEventType;
	l_pEventInfo->bIsBlinked	= FALSE;
	l_pEventInfo->dwBlinkedTime	= 0;


	WORD	l_wInsertIndex;
	if( m_listEvent.empty() )
		l_wInsertIndex = 0;
	else if( pi_wInsertIndex > m_listEvent.size() - 1 )
		l_wInsertIndex = m_listEvent.size() - 1;
	else
		l_wInsertIndex = pi_wInsertIndex;

	CGUIEventList::iterator it;
	it = m_listEvent.begin();

	for( int i=0; i<l_wInsertIndex && it!=m_listEvent.end(); ++i )
		++it;

	m_listEvent.insert( it, l_pEventInfo );	

	if( m_listEvent.size() <= VIEW_EVENT_NUM )
	{
		// resize
		SetSize( m_uiEventButton[0].m_ptSize.x, m_uiEventButton[0].m_ptSize.y * m_listEvent.size() + m_uiUpButton.m_ptSize.y * 2 );

		// event button이 추가될 때마다 전체를 위로 올려준다.
		SetPos( m_ptPos.x, m_ptPos.y - m_uiEventButton[0].m_ptSize.y );

		m_uiEventButton[m_listEvent.size()-1].Show( IsShow() );
		m_uiEventButton[m_listEvent.size()-1].SetPos( m_ptPos.x, 
													  m_ptPos.y + m_uiUpButton.m_ptSize.y +
													  ( m_listEvent.size() - 1 ) * m_uiEventButton[0].m_ptSize.y );		

		Add( &m_uiEventButton[m_listEvent.size()-1] );	
	}	

	BYTE l_byInsertButtonIndex;
	if( l_wInsertIndex < m_nFirstViewIndex )
	{
		// page를 insert하는곳이 보이는 곳으로 옮긴다.		
		SetPage( l_wInsertIndex );

		l_byInsertButtonIndex = 0;
	}
	else if( l_wInsertIndex > m_nFirstViewIndex + VIEW_EVENT_NUM )
	{
		SetPage( l_wInsertIndex - VIEW_EVENT_NUM );

		l_byInsertButtonIndex = VIEW_EVENT_NUM - 1;
	}
	else
	{
		// only update
		SetPage( m_nFirstViewIndex, TRUE );

		l_byInsertButtonIndex = l_wInsertIndex - m_nFirstViewIndex;	
	}

	// udpate
//	SetPage( m_nFirstViewIndex, TRUE );

	// insert되는 eventbutton이 맨 위에 오도록한다.
	Remove( &m_uiEventButton[l_byInsertButtonIndex] );
	Add( &m_uiEventButton[l_byInsertButtonIndex] );


	// eventbutton이 움직이는 동안 마우스 이벤트가 못일어나도록 하기위해 eventblock으로 막는다.
	m_uiEventBlock.SetSize( m_ptSize );
	m_uiEventBlock.SetPos( m_ptPos );
	Remove( &m_uiEventBlock );
	Add( &m_uiEventBlock );

	// move object
	g_cMoveObjectMgr.AddMoveObject( &m_uiEventButton[l_byInsertButtonIndex], MOVE_AXIS_X,
									-m_uiEventButton[l_byInsertButtonIndex].m_ptSize.x, m_ptPos.x, 500, 2 );
	
	g_cMoveObjectMgr.StartMove( &m_uiEventButton[l_byInsertButtonIndex] );

	PlayWave( SND_ID_EVENT_ALARM );
}

void
CGUIEventWindow::RemoveEvent( DWORD pi_dwEventID )
{
	CGUIEventList::iterator it;
	for( it = m_listEvent.begin(); it != m_listEvent.end(); ++it )
	{
		if( (*it)->dwEventID == pi_dwEventID )
		{
			delete *it;
			
			m_listEvent.erase( it );


			if( m_listEvent.size() < VIEW_EVENT_NUM )
			{
				Remove( &m_uiEventButton[m_listEvent.size()] );

				SetSize( m_uiEventButton[0].m_ptSize.x, m_uiEventButton[0].m_ptSize.y * m_listEvent.size() + m_uiUpButton.m_ptSize.y * 2 );

				SetPos( m_ptPos.x, m_ptPos.y + m_uiEventButton[0].m_ptSize.y );		
			}	

			// udpate
			SetPage( m_nFirstViewIndex, TRUE );

			break;
		}
	}			
	
}

void
CGUIEventWindow::SetBlink( DWORD pi_dwEventID )
{
	CGUIEventList::iterator it;
	for( it = m_listEvent.begin(); it != m_listEvent.end(); ++it )
	{
		if( (*it)->dwEventID == pi_dwEventID )
		{
			(*it)->bIsBlinked = TRUE;
			
			return;
		}
	}	
}

void
CGUIEventWindow::SetUnBlink( DWORD pi_dwEventID )
{
	CGUIEventList::iterator it;
	int l_nEventIndex = 0;
	for( it = m_listEvent.begin(); it != m_listEvent.end(); ++it, ++l_nEventIndex )
	{
		if( (*it)->dwEventID == pi_dwEventID )
		{
			(*it)->bIsBlinked = FALSE;	
			
			int l_nButtonIndex = l_nEventIndex - m_nFirstViewIndex;
			if( l_nButtonIndex >= 0 && l_nButtonIndex < VIEW_EVENT_NUM )
			{
				SetEventButtonState( (EventType)((*it)->byEventType), 
									  &m_uiEventButton[l_nButtonIndex],
									  FALSE,
									  l_nButtonIndex );
			}
			
			return;
		}
	}
}

void
CGUIEventWindow::SetPage( int pi_nPageIndex, BOOL pi_bIsUpdate/* = FALSE */ )
{
	int l_nPrevFirstViewIndex = m_nFirstViewIndex;

	if( pi_nPageIndex < 0 )
		m_nFirstViewIndex = 0;
	else if( m_listEvent.size() > VIEW_EVENT_NUM &&
			 pi_nPageIndex > m_listEvent.size() - VIEW_EVENT_NUM )
		m_nFirstViewIndex = m_listEvent.size() - VIEW_EVENT_NUM;
	else if( m_listEvent.size() <= VIEW_EVENT_NUM &&
			 pi_nPageIndex > 0 )
		m_nFirstViewIndex = 0;
	else
		m_nFirstViewIndex = pi_nPageIndex;

	
	if( l_nPrevFirstViewIndex == m_nFirstViewIndex && !pi_bIsUpdate )
		return;

	UpdateEventButton();	


	// page up, down button setting	
	if( m_nFirstViewIndex > 0 )
	{
		// add upbutton
		if( !IsAddedObject( &m_uiUpButton ) )
		{			
			m_uiUpButton.SetPos( m_ptPos.x + ( m_uiEventButton[0].m_ptSize.x - m_uiUpButton.m_ptSize.x ) / 2, m_ptPos.y );
			m_uiUpButton.Show( IsShow() );
			Add( &m_uiUpButton );
		}
	}
	else
	{
		// remove upbutton		
		if( IsAddedObject( &m_uiUpButton ) )
		{			
			Remove( &m_uiUpButton );
		}
	}

	if( m_listEvent.size() > VIEW_EVENT_NUM &&
		m_nFirstViewIndex < m_listEvent.size() - VIEW_EVENT_NUM )
	{
		// add downbutton
		if( !IsAddedObject( &m_uiDownButton ) )
		{			
			m_uiDownButton.SetPos( m_ptPos.x + ( m_uiEventButton[0].m_ptSize.x - m_uiDownButton.m_ptSize.x ) / 2,
								   m_ptPos.y + m_ptSize.y - m_uiDownButton.m_ptSize.y );
			m_uiDownButton.Show( IsShow() );
			Add( &m_uiDownButton );			
		}
	}
	else
	{
		// remove downbutton
		if( IsAddedObject( &m_uiDownButton ) )
		{			
			Remove( &m_uiDownButton );
		}
	}

}

void
CGUIEventWindow::UpdateEventButton( void )
{
	for( int i=0; i<VIEW_EVENT_NUM; ++i )
	{
		if( m_nFirstViewIndex + i >= m_listEvent.size() )
			return;

		SetEventButtonState( (EventType)m_listEvent[m_nFirstViewIndex+i]->byEventType,
							  &m_uiEventButton[i],
							  FALSE,
							  i );		
	}	
}

#define	EVENT_BOARD_MARGIN	9

void
CGUIEventWindow::OpenEventInfoBoard( DWORD pi_dwEventID, CGUITextInfo * pi_pTextInfo, ButtonType pi_btButtonType )
{
	if( !pi_pTextInfo )
		return;

	int l_nOpenEventIndex = -1;
	for( int i=0; i<m_listEvent.size(); ++i )
	{
		if( m_listEvent[i]->dwEventID == pi_dwEventID )
		{
			l_nOpenEventIndex = i;
			break;
		}
	}

	if( l_nOpenEventIndex == -1 ||
		( l_nOpenEventIndex < m_nFirstViewIndex ||
		  l_nOpenEventIndex >= m_nFirstViewIndex + VIEW_EVENT_NUM ) )
		return;

	m_byOpenedInfoIndex = l_nOpenEventIndex - m_nFirstViewIndex;


	// -------------------------------------------------------------
	// set event info string

	strncpy( m_pEventInfoMsgStr, pi_pTextInfo->GetText(), EVENT_MSG_BUFSIZE - 1 );
	m_pEventInfoMsgStr[EVENT_MSG_BUFSIZE - 1] = '\0';

	memcpy( &m_sEventInfoFont, pi_pTextInfo->GetFont(), sizeof(FONT_INFO) );	
	

	if( pi_btButtonType == ButtonType::OK )
	{
		m_uiEventInfoBoard.SetSize( strlen( m_pEventInfoMsgStr ) * ONEBYTE_FONTSIZE + m_uiYESButton.m_ptSize.x + EVENT_BOARD_MARGIN * 3,
									m_uiYESButton.m_ptSize.y + EVENT_BOARD_MARGIN * 2 );

		m_uiEventInfoBoard.Remove( &m_uiNOButton );

		m_uiYESButton.SetPos( m_uiEventInfoBoard.m_ptPos.x + m_uiEventInfoBoard.m_ptSize.x - m_uiYESButton.m_ptSize.x - EVENT_BOARD_MARGIN,
							  m_uiEventInfoBoard.m_ptPos.y + m_uiEventInfoBoard.m_ptSize.y - m_uiYESButton.m_ptSize.y - EVENT_BOARD_MARGIN );

		m_uiYESButton.SetLabel( "확인" );
	}
	else if( pi_btButtonType == ButtonType::YesOrNo )
	{
		m_uiEventInfoBoard.SetSize( strlen( m_pEventInfoMsgStr ) * ONEBYTE_FONTSIZE + m_uiYESButton.m_ptSize.x + 
												m_uiNOButton.m_ptSize.x + EVENT_BOARD_MARGIN * 3,
									m_uiYESButton.m_ptSize.y + EVENT_BOARD_MARGIN * 2 );

		m_uiNOButton.Show( m_uiEventInfoBoard.IsShow() );
		m_uiEventInfoBoard.Add( &m_uiNOButton );

		m_uiNOButton.SetPos( m_uiEventInfoBoard.m_ptPos.x + m_uiEventInfoBoard.m_ptSize.x - m_uiNOButton.m_ptSize.x - EVENT_BOARD_MARGIN,
							 m_uiEventInfoBoard.m_ptPos.y + m_uiEventInfoBoard.m_ptSize.y - m_uiNOButton.m_ptSize.y - EVENT_BOARD_MARGIN );

		m_uiYESButton.SetPos( m_uiNOButton.m_ptPos.x - m_uiYESButton.m_ptSize.x - 5,
							  m_uiNOButton.m_ptPos.y );

		m_uiYESButton.SetLabel( "예" );
	}


	// -------------------------------------------------------------
	// m_uiEventInfoBoard pos setting
	POINT l_ptPos;

	l_ptPos.x = m_ptPos.x - m_uiEventInfoBoard.m_ptSize.x + 1;
	if( l_ptPos.x < gScreen.m_ptPos.x )
		l_ptPos.x = m_ptPos.x + m_ptSize.x;

	l_ptPos.y = m_uiEventButton[m_byOpenedInfoIndex].m_ptPos.y + 
				( m_uiEventButton[m_byOpenedInfoIndex].m_ptSize.y -	m_uiEventInfoBoard.m_ptSize.y ) / 2;	

	m_uiEventInfoBoard.SetPos( l_ptPos );

	m_uiEventInfoBoard.Show( TRUE );
	Add( &m_uiEventInfoBoard );
}

void
CGUIEventWindow::CloseEventInfoBoard( void )
{
	m_byOpenedInfoIndex = 0xFF;

	m_uiEventInfoBoard.Show( FALSE );
	Remove( &m_uiEventInfoBoard );
}

void
CGUIEventWindow::MouseEvent( _mouse_event & event )
{
	CGUIWindow::MouseEvent( event );

	// 버튼이 이동중이면 event를 받지 않는다.	
	if( event.source == &m_uiEventBlock )	
	{
		return;
	}	

	if( event.id == MOUSE_HOVERED )
	{	
		for( int i=0; i<VIEW_EVENT_NUM; ++i )
		{
			if( event.source == &m_uiEventButton[i] )
			{
				// 이미 열려 있으면 skip
				if( m_byOpenedInfoIndex == i )
					return;						

				DWORD m_dwEventID = m_listEvent[m_nFirstViewIndex+i]->dwEventID;
				if( m_dwSendEventID != m_dwEventID )
				{
					m_dwSendEventID = m_dwEventID;
					PostMessage( gScreen.GetHWND(), WM_EVENTWND_MOUSE_HOVER, m_dwEventID, 0 );

					// 이벤트정보창이 열리면 unblink한다.
					SetUnBlink( m_dwEventID );
				}
				return;
			}
		}		
	}
	else if( event.id == MOUSE_LEAVED )
	{
		if( event.source == &m_uiEventInfoBoard )
		{
			// 마우스가 event 설명창을 나갔어도 설명창에 있는
			// 버튼이나 해당 event button에 있으면 닫지않는다.
			if( !( IS_HOVERED(m_uiYESButton.GetState())		||
				   IS_HOVERED(m_uiNOButton.GetState())		||				   
				   IS_HOVERED(m_uiEventButton[m_byOpenedInfoIndex].GetState()) ) )
			{
				CloseEventInfoBoard();
				m_dwSendEventID = 0xFFFFFFFF;
			}
		}		
		else if( m_byOpenedInfoIndex != 0xFF &&
				 event.source == &m_uiEventButton[m_byOpenedInfoIndex] )
		{
			// event button을 나갔어도 event 설명창에 마우스가 있으면 
			// 설명창을 닫지 않는다.
			if( !( IS_HOVERED(m_uiYESButton.GetState())		||
				   IS_HOVERED(m_uiNOButton.GetState())		||			
				   IS_HOVERED(m_uiEventInfoBoard.GetState()) ) )
			{
				CloseEventInfoBoard();
				m_dwSendEventID = 0xFFFFFFFF;
			}
		}
	}
	else if( event.id == MOUSE_STROKED )
	{		
/*
		for( int i=0; i<VIEW_EVENT_NUM; ++i )
		{
			if( event.source == &m_uiEventButton[i] )
			{
				// 해당 event 설명창이 열린다.

				return;
			}
		}
*/
	}

	else  if( event.id == MOUSE_CLICKED )
	{
		if( event.source == &m_uiUpButton )
		{
			SetPage( m_nFirstViewIndex - 1 );			
		}
		else if( event.source == &m_uiDownButton )
		{
			SetPage( m_nFirstViewIndex + 1 );
		}
		else if( event.source == &m_uiYESButton )
		{
			PostMessage( gScreen.GetHWND(), WM_EVENTWND_CLICK_OK, m_dwSendEventID, 0 );
			CloseEventInfoBoard();
			m_dwSendEventID = 0xFFFFFFFF;
		}
		else if( event.source == &m_uiNOButton )
		{
			PostMessage( gScreen.GetHWND(), WM_EVENTWND_CLICK_CANCEL, m_dwSendEventID, 0 );
			CloseEventInfoBoard();
			m_dwSendEventID = 0xFFFFFFFF;
		}
	}
}

void
CGUIEventWindow::Update( void )
{
	if( !IsShow() )
		return;	

	int i;

	if( m_uiEventBlock.m_ptSize.x > 0 )
	{
		// eventbutton 도착했으면 eventblock을 걷는다
		for( i=0; i<VIEW_EVENT_NUM; ++i )
		{
			if( !g_cMoveObjectMgr.IsFinishedMove( &m_uiEventButton[i] ) )
				break;	
		}

		if( i == VIEW_EVENT_NUM )
		{
			m_uiEventBlock.SetSize( 0, 0 );
		}
	}


	for( i=0; i<VIEW_EVENT_NUM; ++i )
	{
		if( m_nFirstViewIndex + i >= m_listEvent.size() )
			return;

		if( m_listEvent[m_nFirstViewIndex+i]->bIsBlinked )
		{
			if( timeGetTime() - m_listEvent[m_nFirstViewIndex+i]->dwBlinkedTime > 600 )
			{
				SetEventButtonState( (EventType)m_listEvent[m_nFirstViewIndex+i]->byEventType, 
									  &m_uiEventButton[i],
									  !IsBlinked( (EventType)m_listEvent[m_nFirstViewIndex+i]->byEventType, &m_uiEventButton[i] ),
									  i );

				m_listEvent[m_nFirstViewIndex+i]->dwBlinkedTime = timeGetTime();
			}
		}

		
	}

}

BOOL
CGUIEventWindow::IsCrossed( LONG pi_nXPos, LONG pi_nYPos )
{
	if( CGUIObject::IsCrossed( pi_nXPos, pi_nYPos ) )
		return TRUE;

	if( m_uiEventInfoBoard.IsShow() )
		return m_uiEventInfoBoard.IsCrossed( pi_nXPos, pi_nYPos );

	return FALSE;
}

#define OUTLINE_COLOR	0xCCAAA183

BOOL
CGUIEventWindow::Draw( void )
{
	if( !CGUIContainer::Draw() )
		return FALSE;

	// -------------------------------------------------------------------
	// eventinfoboard outline
	if( m_uiEventInfoBoard.IsShow() )
	{
		// top
		Draw2DRectangle( m_uiEventInfoBoard.m_ptPos.x + 1,
						 m_uiEventInfoBoard.m_ptPos.y + 1,
						 m_uiEventInfoBoard.m_ptPos.x + m_uiEventInfoBoard.m_ptSize.x - 1,
						 m_uiEventInfoBoard.m_ptPos.y + 2,
						 OUTLINE_COLOR );
		// bottom
		Draw2DRectangle( m_uiEventInfoBoard.m_ptPos.x + 1,
						 m_uiEventInfoBoard.m_ptPos.y + m_uiEventInfoBoard.m_ptSize.y - 2,
						 m_uiEventInfoBoard.m_ptPos.x + m_uiEventInfoBoard.m_ptSize.x - 1,
						 m_uiEventInfoBoard.m_ptPos.y + m_uiEventInfoBoard.m_ptSize.y - 1,
						 OUTLINE_COLOR );
		// left
		Draw2DRectangle( m_uiEventInfoBoard.m_ptPos.x + 1,
						 m_uiEventInfoBoard.m_ptPos.y + 2,
						 m_uiEventInfoBoard.m_ptPos.x + 2,
						 m_uiEventInfoBoard.m_ptPos.y + m_uiEventInfoBoard.m_ptSize.y - 2,
						 OUTLINE_COLOR );
		// right
		Draw2DRectangle( m_uiEventInfoBoard.m_ptPos.x + m_uiEventInfoBoard.m_ptSize.x - 2,
						 m_uiEventInfoBoard.m_ptPos.y + 2,
						 m_uiEventInfoBoard.m_ptPos.x + m_uiEventInfoBoard.m_ptSize.x - 1,
						 m_uiEventInfoBoard.m_ptPos.y + m_uiEventInfoBoard.m_ptSize.y - 2,
						 OUTLINE_COLOR );

		DrawR3Hangul( m_uiEventInfoBoard.m_ptPos.x + EVENT_BOARD_MARGIN,
					  m_uiEventInfoBoard.m_ptPos.y + EVENT_BOARD_MARGIN,
					  m_pEventInfoMsgStr,
					  m_sEventInfoFont.dwColor,
					  m_sEventInfoFont.dwStyle );
	}
	// -------------------------------------------------------------------

	DrawDialogBox();

	return TRUE;	
}

#define EVENT_BUTTON_ORG_SIZE	30

void
CGUIEventWindow::SetEventButtonState( EventType pi_eEventType, CGUIObject * po_pEventObj, BOOL pi_bIsBlinked, BYTE pi_byIndex )
{
	BYTE pi_byIconType = pi_eEventType;
	if( pi_byIconType >= Event_Num )
		pi_byIconType = 0;
	// bulletin과 class icon은 같은걸쓴다.
	else if( pi_byIconType == Bulletin )
		pi_byIconType = Class;

	if( pi_bIsBlinked )
	{
		// set sprite
		po_pEventObj->GetSpriteInfo()->wFrameNo = pi_byIconType * 2 + 1;

		// resize, repos
		po_pEventObj->SetSize( 40, 40 );
		po_pEventObj->SetPos( m_ptPos.x + ( m_ptSize.x - po_pEventObj->m_ptSize.x ) / 2,
							  m_ptPos.y + m_uiUpButton.m_ptSize.y +	
									pi_byIndex * EVENT_BUTTON_ORG_SIZE + ( EVENT_BUTTON_ORG_SIZE - po_pEventObj->m_ptSize.y ) / 2 );
	}
	else
	{
		// set sprite
		po_pEventObj->GetSpriteInfo()->wFrameNo = pi_byIconType * 2;

		// resize, repos
		po_pEventObj->SetSize( EVENT_BUTTON_ORG_SIZE, EVENT_BUTTON_ORG_SIZE );
		po_pEventObj->SetPos( m_ptPos.x,
							  m_ptPos.y + m_uiUpButton.m_ptSize.y +	pi_byIndex * po_pEventObj->m_ptSize.y );
	}
}

BOOL
CGUIEventWindow::IsBlinked( EventType pi_eEventType, CGUIObject * pi_pEventObj )
{
	if( pi_pEventObj->GetSpriteInfo()->wFrameNo == pi_eEventType * 2 + 1 )
		return TRUE;
	else
		return FALSE;
}