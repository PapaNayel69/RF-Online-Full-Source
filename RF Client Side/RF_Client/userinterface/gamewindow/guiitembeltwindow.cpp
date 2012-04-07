////////////////////////////////////////////////////////////////////////////
//
// CGUIItemBeltWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "GUIItemBeltWindow.h"
#include "../GUIController/GUIMessageBox.h"
#include "../GUIController/GUINumInputDlg.h"
#include "../GUIController/GUIScreen.h"
#include "../SpriteMgr.h"
#include "GUIHotKeyBoard.h"

CGUIItemBeltWindow::CGUIItemBeltWindow()
{	
	m_bIsCreated		= FALSE;
}

CGUIItemBeltWindow::~CGUIItemBeltWindow()
{
	
}

void
CGUIItemBeltWindow::Init( void )
{
	// Size
	SetSize( 147, 44 );
	m_uiBoard.SetSize( 147, 44 );

	for( int i = 0; i < MAX_ITEM_SOCKET_NO; ++i )
	{
		m_uiItem[i].SetSize( 32, 32 );
		m_uiSocket[i].SetSize( 32, 32 );

		m_uiUselessSocketMark[i].SetSize( 32, 32 );

		m_uiTradeItemMark[i].SetSize( 32, 32 );
	}

	// Position
	for( i = 0; i < MAX_ITEM_SOCKET_NO; ++i )
	{
		m_uiItem[i].SetPos( m_ptPos.x + i * ( m_uiItem[i].m_ptSize.x + 5 ) + 2, m_ptPos.y + 8 );
		m_uiSocket[i].SetPos( m_uiItem[i].m_ptPos );

		m_uiUselessSocketMark[i].SetPos( m_uiItem[i].m_ptPos );

		m_uiTradeItemMark[i].SetPos( m_uiItem[i].m_ptPos );
	}

	// add controller	
	for( i = 0; i < MAX_ITEM_SOCKET_NO; ++i )
	{
		Add( &m_uiItem[i] );
		Add( &m_uiUselessSocketMark[i] );
		Add( &m_uiTradeItemMark[i] );

		m_uiSocket[i].AddMouseEventHandler( this );
		m_uiBoard.Add( &m_uiSocket[i] );
	}

	Add( &m_uiBoard );


	SetSprite();

	for( i = 0; i < MAX_ITEM_SOCKET_NO; ++i )
		SetUselessSocket( 0, i, FALSE );

	SetName( "ItemBelt" );

	m_bIsCreated = TRUE;
}

void
CGUIItemBeltWindow::SetSprite( void )
{
	if( !m_bIsCreated )
	{
		SPRITE_INFO		l_spriteInfo;

		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_BELT );

		l_spriteInfo.wActionNo = 0;
		
		l_spriteInfo.wFrameNo = 0;
		SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 1;
		m_uiBoard.SetSpriteInfo( &l_spriteInfo );		

		l_spriteInfo.pSprite = NULL;
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 3;		
		for( int i = 0; i < MAX_ITEM_SOCKET_NO; ++i )
			m_uiTradeItemMark[i].SetSpriteInfo( &l_spriteInfo );
		
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
		l_spriteInfo.wActionNo = 1;
		l_spriteInfo.wFrameNo = 4;
		for( i = 0; i < MAX_ITEM_SOCKET_NO; ++i )
			m_uiUselessSocketMark[i].SetSpriteInfo( &l_spriteInfo );
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_BELT );
		if( !l_pSprite )
			return;
		
		GetSpriteInfo()->pSprite = l_pSprite;

		m_uiBoard.GetSpriteInfo()->pSprite = l_pSprite;		

		// item
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_ITEM );
		for( int i=0; i<MAX_ITEM_SOCKET_NO; ++i )		
		{
			if( !m_uiItem[i].IsEmpty() )			
				m_uiItem[i].GetSpriteInfo()->pSprite = l_pSprite;					
		}	
		
		// trade item
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
		for( i=0; i<MAX_ITEM_SOCKET_NO; ++i )		
		{
			if( m_uiTradeItemMark[i].GetSpriteInfo()->pSprite != NULL )
				m_uiTradeItemMark[i].GetSpriteInfo()->pSprite = l_pSprite;

			if( m_uiUselessSocketMark[i].GetSpriteInfo()->pSprite != NULL )
				m_uiUselessSocketMark[i].GetSpriteInfo()->pSprite = l_pSprite;
		}
	}
}

BOOL
CGUIItemBeltWindow::TakeInItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex,
									DWORD pi_dwItemID, DWORD pi_dwSpriteID, BYTE pi_byNum, BYTE pi_byUsableLevel )
{
	if( pi_byNum == 0 || pi_bySocketIndex >= MAX_ITEM_SOCKET_NO )
		return FALSE;

	m_uiItem[pi_bySocketIndex].SetItemID( pi_dwItemID );
	m_uiItem[pi_bySocketIndex].SetSpriteID( pi_dwSpriteID );	
	m_uiItem[pi_bySocketIndex].SetNum( pi_byNum );
	m_uiItem[pi_bySocketIndex].SetSprite();

	// unmark trade item
	SetTradeItem( pi_byPackIndex, pi_bySocketIndex, FALSE );
	
	return TRUE;
}

void
CGUIItemBeltWindow::SetTradeItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex, BOOL pi_bIsTradeItem )
{
	if( pi_bIsTradeItem )
		m_uiTradeItemMark[pi_bySocketIndex].GetSpriteInfo()->pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
	else
		m_uiTradeItemMark[pi_bySocketIndex].GetSpriteInfo()->pSprite = NULL;
}

BOOL
CGUIItemBeltWindow::TakeOutItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex, BYTE pi_byNum, CGUIItem * po_pOutItem )
{
	if( pi_bySocketIndex >= MAX_ITEM_SOCKET_NO )
		return FALSE;

	if( IsEmpty( pi_bySocketIndex ) )
		return FALSE;

	
	BYTE l_byOutNum;
	if( pi_byNum == 0 )
		l_byOutNum = m_uiItem[pi_bySocketIndex].GetNum();
	else
		l_byOutNum = pi_byNum;

	if( l_byOutNum > m_uiItem[pi_bySocketIndex].GetNum() )
		return FALSE;


	// set outitem setting
	if( po_pOutItem )
	{
		po_pOutItem->SetData( &m_uiItem[pi_bySocketIndex] );
		po_pOutItem->SetNum( l_byOutNum );
	}

	m_uiItem[pi_bySocketIndex].SetNum( m_uiItem[pi_bySocketIndex].GetNum() - l_byOutNum );

	if( m_uiItem[pi_bySocketIndex].GetNum() == 0 )
		SetEmpty( pi_bySocketIndex );

	return TRUE;
}

CGUIItem *
CGUIItemBeltWindow::GetItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex )
{
	if( pi_bySocketIndex >= MAX_ITEM_SOCKET_NO )
		return NULL;

	if( IsEmpty( pi_bySocketIndex ) )
		return NULL;
	else
		return &m_uiItem[pi_bySocketIndex];
}

void
CGUIItemBeltWindow::SetUselessSocket( BYTE pi_byPackIndex, BYTE pi_bySocketIndex, BOOL pi_bFlag )
{
	if( pi_bySocketIndex >= MAX_ITEM_SOCKET_NO )
		return;

	if( pi_bFlag )
	{
		m_uiUselessSocketMark[pi_bySocketIndex].GetSpriteInfo()->pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
		m_uiUselessSocketMark[pi_bySocketIndex].SetARGB( 0x90ffffff );
	}
	else
	{
		m_uiUselessSocketMark[pi_bySocketIndex].GetSpriteInfo()->pSprite = NULL;
		m_uiUselessSocketMark[pi_bySocketIndex].SetARGB( -1L );
	}
}

void
CGUIItemBeltWindow::SetEmpty( BYTE pi_bySocketIndex )
{	
	m_uiItem[pi_bySocketIndex].SetEmpty();	

	SetTradeItem( 0, pi_bySocketIndex, FALSE );
}

void
CGUIItemBeltWindow::UseItem( BYTE pi_bySocketIndex )
{
	PostUIMessage( WM_BOARD_USE_OBJECT, UI_MOUSE_MSG( 0, pi_bySocketIndex, 0, 0 ) );	
}

// ------------------------------------------------------------------------------

void
CGUIItemBeltWindow::MouseEvent( _mouse_event & event )
{
	CGUIWindow::MouseEvent( event );	

	if( event.id == MOUSE_STROKED )
	{
		for( int i=0; i<MAX_ITEM_SOCKET_NO; ++i )
		{			
			if( event.source == &m_uiSocket[i] )
				break;				
		}
		if( i >= MAX_ITEM_SOCKET_NO )
			return;

		// ----------------------------------------------------------------------
		// add
		if( gScreen.ExistObject() )
		{
			PostUIMessage( WM_BOARD_ADD_OBJECT, UI_MOUSE_MSG( 0, i, 0, 0 ) );
		}
		// remove
		else
		{
			SetQuickEvent();

			if( g_bIsQuickEvent_Trade )
				PostUIMessage( WM_BOARD_TRADE_HOT_KEY, UI_MOUSE_MSG( 0, i, 0, 0 ) );
			else
				PostUIMessage( WM_BOARD_CHECK_REMOVE_OBJECT, UI_MOUSE_MSG( 0, i, 0, 0 ) );
		}

		m_byInputSocketIndex = i;

	}
	else if( event.id == LMOUSE_STROKED )
	{
		// Use item
		if( gScreen.ExistObject() )
			return;		

		for( int i=0; i<MAX_ITEM_SOCKET_NO; ++i )
		{
			if( event.source == &m_uiSocket[i] )				
			{
				// 아이템 사용
				UseItem( i );
				break;			
			}
		}

	}
	else if( event.id == MOUSE_HOVERED )
	{
		if( gScreen.ExistObject() )
			return;
		
		for( int i=0; i<MAX_ITEM_SOCKET_NO; ++i )
		{
			if( event.source == &m_uiSocket[i] &&
				!IsEmpty( i ) )
			{
				// 아이템의 정보를 보여준다.
				PostUIMessage( WM_BOARD_HOVER_OBJECT, UI_MOUSE_MSG( 0, i, 0, 0 ) );
				break;
			}
		}		
	}
}

void
CGUIItemBeltWindow::ClosingDialogBox( CGUIDialogBox * pi_pDlg )
{	
	if( pi_pDlg->GetUserDefinedID() == INPUT_DLG_ID &&
		pi_pDlg->GetResultMsg() == MSG_OK )
	{
		if( !ProcNumInputDlg( (CGUINumInputDlg *)pi_pDlg ) )
			return;
	}	

	CGUIWindow::ClosingDialogBox( pi_pDlg );
}

BOOL
CGUIItemBeltWindow::ProcNumInputDlg( CGUINumInputDlg * pi_pInputDlg )
{	
	WORD wQuantity = atoi( pi_pInputDlg->GetInput() );

	PostUIMessage( WM_BOARD_REMOVE_OBJECT,
				   UI_MOUSE_MSG( 0, m_byInputSocketIndex, wQuantity, 0 ) );
	return TRUE;	
}