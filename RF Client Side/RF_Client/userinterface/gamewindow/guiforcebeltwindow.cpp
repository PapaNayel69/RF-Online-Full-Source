////////////////////////////////////////////////////////////////////////////
//
// CGUIForceBeltWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////

#include "GUIForceBeltWindow.h"
#include "../GUIController/GUIScreen.h"
#include "../SpriteMgr.h"

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

CGUIForceBeltWindow::CGUIForceBeltWindow()
{
	m_bCreated = FALSE;
}

CGUIForceBeltWindow::~CGUIForceBeltWindow()
{
}

void
CGUIForceBeltWindow::Init( void )
{
	// Size
	SetSize( 295, 44 );
	m_uiBoard.SetSize( 295, 44 );

	for( int i = 0; i < MAX_FORCE_SOCKET_NO; ++i )
	{
		m_uiItem[i].SetSize( 32, 32 );
		m_uiSocket[i].SetSize( 32, 32 );
	}

	// Position
	for( i = 0; i < 8; ++i )
	{
		m_uiItem[i].SetPos( m_ptPos.x + i * ( m_uiItem[i].m_ptSize.x + 5 ) + 2, m_ptPos.y + 8 );
		m_uiSocket[i].SetPos( m_uiItem[i].m_ptPos );
	}

	// add controller		
	for( i = 0; i < MAX_FORCE_SOCKET_NO; ++i )
	{		
		Add( &m_uiItem[i] );	

		m_uiSocket[i].AddMouseEventHandler( this );
		m_uiBoard.Add( &m_uiSocket[i] );
	}

	Add( &m_uiBoard );


	SetSprite();

	SetName( "ForceBelt" );

	m_bCreated = TRUE;
}

void
CGUIForceBeltWindow::SetSprite( void )
{
	if( !m_bCreated )
	{
		SPRITE_INFO		l_spriteInfo;

		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_BELT );

		l_spriteInfo.wActionNo = 1;
		
		// background
		l_spriteInfo.wFrameNo = 0;
		SetSpriteInfo( &l_spriteInfo );

		// board
		l_spriteInfo.wFrameNo = 1;
		m_uiBoard.SetSpriteInfo( &l_spriteInfo );		
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_BELT );
		if( !l_pSprite )
			return;
		
		// background
		GetSpriteInfo()->pSprite = l_pSprite;

		// board		
		m_uiBoard.GetSpriteInfo()->pSprite = l_pSprite;	
		
		
		// item
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_ITEM );
		for( int i = 0; i < MAX_FORCE_SOCKET_NO; ++i )
		{
			if( m_uiItem[i].GetItemID() != NONE_ITEM_ID )
				m_uiItem[i].GetSpriteInfo()->pSprite = l_pSprite;
		}
	}
}

BOOL
CGUIForceBeltWindow::TakeInItem( BYTE pi_bySocketIndex,	CGUIItem * pi_pSourceItem )
{
	if( pi_bySocketIndex >= MAX_FORCE_SOCKET_NO )
		return FALSE;

	// item type check
	if( pi_pSourceItem->GetIconType() == ITEM_ICON )
		return FALSE;	

	m_uiItem[pi_bySocketIndex].SetData( pi_pSourceItem );
	m_uiItem[pi_bySocketIndex].SetSize( 32, 32 );
	m_uiItem[pi_bySocketIndex].SetRefItem( pi_pSourceItem );

	return TRUE;
}

BOOL
CGUIForceBeltWindow::TakeOutItem( BYTE pi_bySocketIndex )
{
	if( pi_bySocketIndex >= MAX_FORCE_SOCKET_NO )
		return FALSE;

	m_uiItem[pi_bySocketIndex].SetEmpty();

	return TRUE;
}

void
CGUIForceBeltWindow::UseItem( BYTE pi_bySocketIndex )
{
	PostUIMessage( WM_BOARD_USE_OBJECT, UI_MOUSE_MSG( 0, pi_bySocketIndex, 0, 0 ) );	
}

void
CGUIForceBeltWindow::MouseEvent( _mouse_event & event )
{
	CGUIWindow::MouseEvent( event );

	if( event.id == MOUSE_STROKED )
	{		
		for( int i=0; i<MAX_FORCE_SOCKET_NO; ++i )
		{
			if( event.source == &m_uiSocket[i] )
				break;
		}
		if( i >= MAX_FORCE_SOCKET_NO )
			return;	

		// add
		if( gScreen.ExistObject() )
		{
			PostUIMessage( WM_BOARD_ADD_OBJECT, UI_MOUSE_MSG( 0, i, 0, 0 ) );
		}
		// remove
		else
		{
			SetQuickEvent();

			if( g_bIsQuickEvent_Equip )
				PostUIMessage( WM_BOARD_EQUIP_HOT_KEY, UI_MOUSE_MSG( 0, i, 1, 0 ) );
			else
				PostUIMessage( WM_BOARD_REMOVE_OBJECT, UI_MOUSE_MSG( 0, i, 1, 0 ) );
		}
		
	}
	else if( event.id == LMOUSE_STROKED )
	{		
		if( gScreen.ExistObject() )
			return;

		// Use item
		for( int i=0; i<MAX_FORCE_SOCKET_NO; ++i )
		{
			if( event.source == &m_uiSocket[i] && !m_uiItem[i].IsEmpty() )
			{
				UseItem( i );
				break;
			}
		}
	}
	else if( event.id == MOUSE_HOVERED )
	{
		if( gScreen.ExistObject() )
			return;

		// Tooltip
		for( int i=0; i<MAX_FORCE_SOCKET_NO; ++i )
		{
			if( event.source == &m_uiSocket[i] && !m_uiItem[i].IsEmpty() )
			{
				PostUIMessage( WM_BOARD_HOVER_OBJECT, UI_MOUSE_MSG( 0, i, 0, 0 ) );
				break;
			}
		}
	}
}