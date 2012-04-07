////////////////////////////////////////////////////////////////////////////
//
// CGUIItemUpgradeWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "GUIItemUpgradeWindow.h"
#include "../../SpriteMgr.h"
#include "../../../DefineMessage.h"

//------------------------------------------------------------------------------

CGUIItemUpgradeWindow::CGUIItemUpgradeWindow()
{	
	m_bIsCreated = FALSE;
}

CGUIItemUpgradeWindow::~CGUIItemUpgradeWindow()
{
	
}

void
CGUIItemUpgradeWindow::Init( void )
{
	
	SetSize( 228, 412 );

	m_uiAniTitleBar.SetSize( 228, 28 );	

	m_uiUpgradeItem[UpgradeItem].SetSize( 64, 64 );
	m_uiEmptySocketMark[UpgradeItem].SetSize( 64, 64 );
	for( int i = 1; i < TOTAL_UPGRADE_SOCKET_NO; ++i )
	{
		m_uiUpgradeItem[i].SetSize( 32, 32 );
		m_uiEmptySocketMark[i].SetSize( 32, 32 );
	}

	m_uiUpgradeButton.SetSize( 100, 25 );
	m_uiCloseButton.SetSize( 16, 13 );	
	m_uiHelpButton.SetSize( 16, 13 );	

	m_uiTitle.SetString( "업그레이드" );
	m_uiUpgradeButton.SetLabel( "업그레이드" );
	
	// --------------------------------------------------------
	m_uiTitle.SetPos( m_ptPos.x + 6, m_ptPos.y + 7 );

	m_uiUpgradeItem[UpgradeItem].SetPos( m_ptPos.x + 82, m_ptPos.y + 168 );
	m_uiUpgradeItem[Crystal].SetPos( m_ptPos.x + 98, m_ptPos.y + 87 );
	m_uiUpgradeItem[Water].SetPos( m_ptPos.x + 185, m_ptPos.y + 159 );
	m_uiUpgradeItem[Terra].SetPos( m_ptPos.x + 159, m_ptPos.y + 256 );
	m_uiUpgradeItem[Wind].SetPos( m_ptPos.x + 37, m_ptPos.y + 256 );
	m_uiUpgradeItem[Fire].SetPos( m_ptPos.x + 12, m_ptPos.y + 159 );
	
	for( i = 0; i < TOTAL_UPGRADE_SOCKET_NO; ++i )
	{		
		m_uiEmptySocketMark[i].SetPos( m_uiUpgradeItem[i].m_ptPos );
	}

	m_uiUpgradeButton.SetPos( m_ptPos.x + 64, m_ptPos.y + 372 );
	m_uiCloseButton.SetPos( m_ptPos.x + 211, m_ptPos.y + 4 );
	m_uiHelpButton.SetPos( m_ptPos.x + 194, m_ptPos.y + 4 );


	// --------------------------------------------------------

	Add( &m_uiAniTitleBar );
	Add( &m_uiTitle );

	m_uiUpgradeButton.AddMouseEventHandler( this );
	m_uiCloseButton.AddMouseEventHandler( this );
	m_uiHelpButton.AddMouseEventHandler( this );

	for( i=0; i<TOTAL_UPGRADE_SOCKET_NO; ++i )	
	{
		Add( &m_uiEmptySocketMark[i] );

		m_uiUpgradeItem[i].AddMouseEventHandler( this );
		Add( &m_uiUpgradeItem[i] );
	}
	
	Add( &m_uiUpgradeButton );
	Add( &m_uiCloseButton );
	Add( &m_uiHelpButton );


	SetSprite();

	m_uiTitle.GetFont()->dwColor = D3DCOLOR_XRGB( 195, 193, 190 );

	SetARGB( 0xCCFFFFFF );

	m_uiUpgradeButton.SetLabelStyle( R3_HAN_OUTLINE );

	// --------------------------------------------------

	// bar ani setting
	m_uiAniTitleBar.SetTotalFrameNum( 18 );
	m_uiAniTitleBar.SetObjectNumInLine( 1 );
	m_uiAniTitleBar.SetFPS( 10 );
	m_uiAniTitleBar.SetStopInterval( 1000 );	
	
	SetMoveObject( this );
	SetMoveObject( &m_uiAniTitleBar );
	SetMoveObject( &m_uiTitle );
	SetMovable( TRUE );
	

	EnableUpgrade( FALSE );

	// init empty socket
	for( i=0; i<TOTAL_UPGRADE_SOCKET_NO; ++i )
	{
		SetEmptySocket( (UpgradeSocket)i, TRUE );		
	}	

	SoundOn( TRUE );

	m_bIsCreated = TRUE;
}

void
CGUIItemUpgradeWindow::SetSprite( void )
{
	if( !m_bIsCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_ITEM_UPGRADE );

		l_spriteInfo.wActionNo = 0;

		l_spriteInfo.wFrameNo = 0;

		SetSpriteInfo( &l_spriteInfo );			

		// empty socket
		l_spriteInfo.wActionNo = 1;		
		for( int i=0; i<TOTAL_UPGRADE_SOCKET_NO; ++i )
		{
			l_spriteInfo.wFrameNo = i;
			m_uiEmptySocketMark[i].SetSpriteInfo( &l_spriteInfo );			
		}		

		// -------------------------------------------------------------------
		// base anibar
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 5;
		m_uiAniTitleBar.SetSpriteInfo( &l_spriteInfo );			

		// upgrade button
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );
		l_spriteInfo.wFrameNo = 16;
		m_uiUpgradeButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 17;
		m_uiUpgradeButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 18;
		m_uiUpgradeButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 19;
		m_uiUpgradeButton.SetSpriteInfoEx( DISABLED, &l_spriteInfo );

		// close button	
		m_uiCloseButton.SetSpriteInfoEx( gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON1 ) );
		m_uiCloseButton.SetSpriteInfoEx( HOVERED, gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON2 ) );
		m_uiCloseButton.SetSpriteInfoEx( PRESSED, gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON3 ) );

		// help button
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );
		l_spriteInfo.wActionNo = 5;
		l_spriteInfo.wFrameNo = 0;
		m_uiHelpButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 1;
		m_uiHelpButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 2;
		m_uiHelpButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_ITEM_UPGRADE );	
		if( !l_pSprite )
			return;
		
		GetSpriteInfo()->pSprite = l_pSprite;						
		
		// empty socket
		for( int i=0; i<TOTAL_UPGRADE_SOCKET_NO; ++i )
		{
			if(	m_uiEmptySocketMark[i].GetSpriteInfo()->pSprite != NULL )
				m_uiEmptySocketMark[i].GetSpriteInfo()->pSprite = l_pSprite;
		}		

		// upgrade item
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_ITEM );
		for( i=0; i<TOTAL_UPGRADE_SOCKET_NO; ++i )
		{
			if( !m_uiUpgradeItem[i].IsEmpty() )
				m_uiUpgradeItem[i].GetSpriteInfo()->pSprite = l_pSprite;
		}

		
		// base anibar
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );		
		m_uiAniTitleBar.GetSpriteInfo()->pSprite = l_pSprite;

		// upgrade button		
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );
		m_uiUpgradeButton.SetSprite( l_pSprite );		
		
		// close button			
		m_uiCloseButton.SetSprite( l_pSprite );

		m_uiHelpButton.SetSprite( l_pSprite );
	}
}

void
CGUIItemUpgradeWindow::EnableUpgrade( BOOL pi_bEnable )
{
	if( pi_bEnable )
	{
		m_uiUpgradeButton.Enable();
	}
	else // disable
	{		
		m_uiUpgradeButton.Disable();
	}
}


BOOL
CGUIItemUpgradeWindow::TakeInItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex,
								    DWORD pi_dwItemID, DWORD pi_dwSpriteID, BYTE pi_byNum, BYTE pi_byUsableLevel )
{
	// item data setting
	m_uiUpgradeItem[pi_bySocketIndex].SetItemID( pi_dwItemID );
	m_uiUpgradeItem[pi_bySocketIndex].SetSpriteID( pi_dwSpriteID );
	m_uiUpgradeItem[pi_bySocketIndex].SetNum( pi_byNum );
	m_uiUpgradeItem[pi_bySocketIndex].SetSprite();	
	
	// set none empty socket
	SetEmptySocket( (UpgradeSocket)pi_bySocketIndex, FALSE );		

	return true;
}
	
BOOL
CGUIItemUpgradeWindow::TakeOutItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex, BYTE pi_byNum , CGUIItem * po_pOutItem )
{
	if( m_uiUpgradeItem[pi_bySocketIndex].IsEmpty() )
		return FALSE;	

	m_uiUpgradeItem[pi_bySocketIndex].SetEmpty();

	// set empty socket
	SetEmptySocket( (UpgradeSocket)pi_bySocketIndex, TRUE );	

	return TRUE;
}

CGUIItem *	
CGUIItemUpgradeWindow::GetItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex )
{
	if( m_uiUpgradeItem[pi_bySocketIndex].IsEmpty() )
		return NULL;

	return &m_uiUpgradeItem[pi_bySocketIndex];	
}

void
CGUIItemUpgradeWindow::SetEmptySocket( UpgradeSocket pi_eSocket, BOOL pi_bIsEmpty )
{
	if( pi_bIsEmpty )
	{	
		m_uiEmptySocketMark[pi_eSocket].GetSpriteInfo()->pSprite = GetSpriteInfo()->pSprite;
		m_uiEmptySocketMark[pi_eSocket].SetARGB( 0x80FFFFFF );	
	}
	else
	{
		m_uiEmptySocketMark[pi_eSocket].GetSpriteInfo()->pSprite = NULL;
		m_uiEmptySocketMark[pi_eSocket].SetARGB( -1L );		
	}
}

void
CGUIItemUpgradeWindow::MouseEvent( _mouse_event & event )
{
	CGUIWindow::MouseEvent( event );	

	if( event.id == MOUSE_STROKED )
	{	
		for( int i=0; i<TOTAL_UPGRADE_SOCKET_NO; ++i )
		{
			if( event.source == &m_uiUpgradeItem[i] )			
				break;			
		}
		if( i >= TOTAL_UPGRADE_SOCKET_NO )
			return;
		

		// add item
		if( gScreen.ExistObject() )
		{			
			PostUIMessage( WM_BOARD_ADD_OBJECT, UI_MOUSE_MSG( 0, i, 0, 0 ) );					
		}
		// remove item
		else
		{			
			SetQuickEvent();
			
			if( g_bIsQuickEvent_Trade )
				PostUIMessage( WM_BOARD_TRADE_HOT_KEY, UI_MOUSE_MSG( 0, i, 0, 0 ) );
			else
				PostUIMessage( WM_BOARD_REMOVE_OBJECT, UI_MOUSE_MSG( 0, i, 0, 0 ) );
		}		
	}
	else if( event.id == MOUSE_CLICKED )
	{
		if( event.source == &m_uiUpgradeButton )
		{			
			// upgrade
			PostUIMessage( WM_ITEM_UPGRADE_BOARD_PROCESS, 0 );
		}		
		else if( event.source == &m_uiHelpButton )
		{
			// 도움말 창 열기
			PostUIMessage( WM_OPEN_WINDOW_HELP, 0 );
		}
		else if( event.source == &m_uiCloseButton )
		{			
			Close();
		}		
	}
	else if( event.id == MOUSE_HOVERED )
	{
		for( int i=0; i<TOTAL_UPGRADE_SOCKET_NO; ++i )
		{
			if( event.source == &m_uiUpgradeItem[i] && !m_uiUpgradeItem[i].IsEmpty() )
			{
				// tooltip
				PostUIMessage( WM_BOARD_HOVER_OBJECT, UI_MOUSE_MSG( 0, i, 0, 0 ) );

				return;
			}
		}		
	}
}
