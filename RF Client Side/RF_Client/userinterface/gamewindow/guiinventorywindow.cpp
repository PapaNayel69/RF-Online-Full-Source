#include "GUIInventoryWindow.h"
#include "../GUIController/GUIMessageBox.h"
#include "../GUIController/GUIScreen.h"
#include "../SpriteMgr.h"
#include "../UIGlobal.h"

#define	EMPTY_EQUIP_SOCKET_ARGB		0x40FFFFFF
#define	TEXT_COLOR					0xFFC3C1BE

#define	NONE_SOCKET_INDEX			0xFF

#define	MSG_BOX_ID_INVEN_ETC		400


////////////////////////////////////////////////////////////////////////////
//
// CGUIInventoryBoard Class Implementation
//
////////////////////////////////////////////////////////////////////////////

CGUIInventoryBoard::CGUIInventoryBoard()
{	
	// init empty item
	for( int i=0; i<TOTAL_INVENTORY; ++i )
		SetEmpty( i );

	m_bySelectedSocketIndex = NONE_SOCKET_INDEX;		

	m_bIsClosedWidhEquipWin	= FALSE;

	m_bCreated				=	FALSE;

	SetSize( 228, 176 );
}

CGUIInventoryBoard::~CGUIInventoryBoard()
{	
}

void
CGUIInventoryBoard::Init( BYTE pi_byPackID )
{
	m_byPackID = pi_byPackID;
	
	char buf[16];
	sprintf( buf, "제 %d 배낭", m_byPackID + 1 );
	m_uiTitle.SetString( buf );

	// ------------------------------------------------------
	// size
	SetSize( 228, 176 );	
	m_uiTitle.SetSize( 70, 20 );

	int i, j;
	for( i = 0; i < TOTAL_INVENTORY; ++i )
	{
		m_uiUselessItemMark[i].SetSize( 34, 34 );
		m_uiUselessSocketMark[i].SetSize( 34, 34 );
		m_uiTradeItemMark[i].SetSize( 34, 34 );
	}

	m_uiSelectedSocket.SetSize( 34, 34 );	

	m_uiCloseButton.SetSize( 16, 13 );

	// ------------------------------------------------------
	// pos
	POINT l_ptPos;

	// item
	int index = 0;	
	for( i=0; i<4; ++i )
	{
		for ( j=0; j<5; ++j )
		{
			l_ptPos.x = 19 + 39 * j;
			l_ptPos.y = 26 + 36 * i;
			m_uiItem[index].SetPos( l_ptPos );			
			++index;
		}
	}	
	
	for( i = 0; i < TOTAL_INVENTORY; ++i )
	{
		m_uiUselessItemMark[i].SetPos( m_uiItem[i].m_ptPos.x - 1,
										m_uiItem[i].m_ptPos.y - 1 );
		m_uiUselessSocketMark[i].SetPos( m_uiUselessItemMark[i].m_ptPos );

		m_uiTradeItemMark[i].SetPos( m_uiUselessItemMark[i].m_ptPos );
	}

	m_uiTitle.SetPos( m_ptPos.x + 6, m_ptPos.y + 6 );

	m_uiCloseButton.SetPos( 207, 5 );

	// ------------------------------------------------------
	// add 	
	Add( &m_uiTitle );

	for( i = 0; i < TOTAL_INVENTORY; ++i)
	{
		Add( &m_uiItem[i] );

		Add( &m_uiUselessItemMark[i] );
		Add( &m_uiUselessSocketMark[i] );

		m_uiTradeItemMark[i].AddMouseEventHandler( this );
		Add( &m_uiTradeItemMark[i] );
	}	

	m_uiCloseButton.AddMouseEventHandler( this );
	Add( &m_uiCloseButton );

	SetSprite();

	m_uiTitle.GetFont()->dwColor = TEXT_COLOR;


	for( i = 0; i < TOTAL_INVENTORY; ++i )
	{
		SetUselessItem( i, FALSE );
		SetUselessSocket( i, FALSE );
	}

	m_uiSelectedSocket.SetARGB( 0x90ffffff );

	SetMoveObject( this );	
	SetMoveObject( &m_uiTitle );
	SetMovable( TRUE );

	DisableFocus();

	SetARGB( 0xCCFFFFFF );

	SetName( "invenBoard" );

	m_bCreated = TRUE;	
}

void
CGUIInventoryBoard::SetSprite( void )
{
	if( !m_bCreated )
	{		
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );		


		l_spriteInfo.wActionNo = 3;
		l_spriteInfo.wFrameNo = 0;
		SetSpriteInfo( &l_spriteInfo );		

		// useless item
		l_spriteInfo.wActionNo = 1;
		l_spriteInfo.wFrameNo = 4;
		for( int i=0; i<TOTAL_INVENTORY; ++i )
		{
			m_uiUselessItemMark[i].SetSpriteInfo( &l_spriteInfo );
			m_uiUselessSocketMark[i].SetSpriteInfo( &l_spriteInfo );
		}		

		// selected socket
		l_spriteInfo.wActionNo = 1;
		l_spriteInfo.wFrameNo = 2;
		m_uiSelectedSocket.SetSpriteInfo( &l_spriteInfo );		

		// trade item mark
		l_spriteInfo.pSprite = NULL;
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 3;
		for( i=0; i<TOTAL_INVENTORY; ++i )
		{
			m_uiTradeItemMark[i].SetSpriteInfo( &l_spriteInfo );
		}

		// close button	
		m_uiCloseButton.SetSpriteInfoEx( gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON1 ) );
		m_uiCloseButton.SetSpriteInfoEx( HOVERED, gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON2 ) );
		m_uiCloseButton.SetSpriteInfoEx( PRESSED, gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON3 ) );
	}
	else
	{
		CSprite * l_pSprite; 
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
		if( !l_pSprite )
			return;
		
		GetSpriteInfo()->pSprite = l_pSprite;		

		// useless item
		for( int i = 0; i < TOTAL_INVENTORY; ++i )
		{
			if( m_uiUselessItemMark[i].GetSpriteInfo()->pSprite )
				m_uiUselessItemMark[i].GetSpriteInfo()->pSprite = l_pSprite;

			if( m_uiUselessSocketMark[i].GetSpriteInfo()->pSprite )
				m_uiUselessSocketMark[i].GetSpriteInfo()->pSprite = l_pSprite;
		}

		// selected socket
		m_uiSelectedSocket.GetSpriteInfo()->pSprite = l_pSprite;

		// trade item mark
		for( i = 0; i < TOTAL_INVENTORY; ++i )
		{
			if( m_uiTradeItemMark[i].GetSpriteInfo()->pSprite )
				m_uiTradeItemMark[i].GetSpriteInfo()->pSprite = l_pSprite;
		}

		m_uiCloseButton.SetSprite( gSpriteMgr.GetSprite( SP_ID_COMMON ) );

		// item
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_ITEM );
		for( i = 0; i < TOTAL_INVENTORY; ++i )
		{
			if( !m_uiItem[i].IsEmpty() )
				m_uiItem[i].GetSpriteInfo()->pSprite = l_pSprite;
		}
	}
}

void
CGUIInventoryBoard::Open( void )
{
	m_pInventoryWin->SetPackState( m_byPackID, TRUE );
	
	// 최근에 열었던 배낭을 저장해둔다. 
	// 상점 열 때 최근 배낭을 열어야 하기 때문에
	m_pInventoryWin->m_byLastOpenedPack = m_byPackID;
	
	CGUIWindow::Open();	

	if( m_pInventoryWin->IsOpenOnlyOnePack() )
	{
		CGUIInventoryBoard * l_pPackWin;
		for( int i=0; i<TOTAL_PACK; ++i )
		{
			l_pPackWin = m_pInventoryWin->GetPackWindow( i );

			if( !l_pPackWin || m_byPackID == i )
				continue;

			if( l_pPackWin->IsShow() )
			{
				l_pPackWin->Close();
			}
		}
	}
}

void
CGUIInventoryBoard::Close( void )
{
	CGUIWindow::Close();

	m_pInventoryWin->SetPackState( m_byPackID, FALSE );	

	_CloseAllDialogBox( this );
	_CloseDialogBox( NULL, MSG_BOX_ID_INVEN_ETC );
}

BOOL
CGUIInventoryBoard::TakeInItem( BYTE pi_bySocketIndex, DWORD pi_dwItemID, DWORD pi_dwSpriteID, BYTE pi_byNum, BYTE pi_byUsableLevel )
{
	if( pi_bySocketIndex >= TOTAL_INVENTORY )
		return FALSE;

	// set item data
	m_uiItem[pi_bySocketIndex].SetItemID( pi_dwItemID );
	m_uiItem[pi_bySocketIndex].SetSpriteID( pi_dwSpriteID );
	m_uiItem[pi_bySocketIndex].SetNum( pi_byNum );
	
	if( pi_byUsableLevel != NONE_LEVEL )		
		m_uiItem[pi_bySocketIndex].SetUsableLevel( pi_byUsableLevel );

	m_uiItem[pi_bySocketIndex].SetSize( 32, 32 );
	m_uiItem[pi_bySocketIndex].SetSprite();
	

	// set usable item. 종족, 성별만 체크한다.
	if( _IsUsableItem( &m_uiItem[pi_bySocketIndex], _GetAvatarRace() ) )
		m_uiItem[pi_bySocketIndex].SetUsable( TRUE );
	else
		m_uiItem[pi_bySocketIndex].SetUsable( FALSE );	

	// 사용할수 없는 아이템인지 표시한다.
	SetUselessItem( pi_bySocketIndex, !m_uiItem[pi_bySocketIndex].IsUsable() );
	
	SetTradeItem( pi_bySocketIndex, FALSE );

	return TRUE;
}

BOOL
CGUIInventoryBoard::TakeInItem( BYTE pi_bySocketIndex, CGUIItem * pi_pItem )
{
	return TakeInItem( pi_bySocketIndex, pi_pItem->GetItemID(), pi_pItem->GetSpriteID(), 
					   pi_pItem->GetNum(), pi_pItem->GetUsableLevel() );	
}

void	
CGUIInventoryBoard::SetTradeItem( BYTE pi_bySocketIndex, BOOL pi_bIsTradeItem )
{
	if( pi_bySocketIndex >= TOTAL_INVENTORY )
		return;

	if( pi_bIsTradeItem )
		m_uiTradeItemMark[pi_bySocketIndex].GetSpriteInfo()->pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
	else 
		m_uiTradeItemMark[pi_bySocketIndex].GetSpriteInfo()->pSprite = NULL;
}

BOOL
CGUIInventoryBoard::TakeOutItem( BYTE pi_bySocketIndex, BYTE pi_byNum, CGUIItem * po_pItem )
{
	if( pi_bySocketIndex >= TOTAL_INVENTORY || IsEmpty( pi_bySocketIndex ) )		
		return FALSE;

	if( pi_byNum > m_uiItem[pi_bySocketIndex].GetNum() )
		return FALSE;

	BYTE l_byRestNum;
	if( pi_byNum == 0 )
		l_byRestNum = 0;
	else 
		l_byRestNum = m_uiItem[pi_bySocketIndex].GetNum() - pi_byNum;

	
	if( po_pItem != NULL )
	{
		po_pItem->SetData( &m_uiItem[pi_bySocketIndex] );
		po_pItem->SetNum( m_uiItem[pi_bySocketIndex].GetNum() - l_byRestNum );
	}
	
	if( l_byRestNum == 0 )
	{
		// empty item
		SetEmpty( pi_bySocketIndex );
	}
	else
	{
		m_uiItem[pi_bySocketIndex].SetNum( l_byRestNum );
	}	

	return TRUE;
}

void
CGUIInventoryBoard::SetEmpty( BYTE pi_bySocketIndex )
{
	m_uiItem[pi_bySocketIndex].SetEmpty();

	SetTradeItem( pi_bySocketIndex, FALSE );
	SetUselessItem( pi_bySocketIndex, FALSE );
}

CGUIItem	*
CGUIInventoryBoard::GetItem( WORD pi_wSocketIndex )
{
	if( pi_wSocketIndex >= TOTAL_INVENTORY )
		return NULL;

	if( IsEmpty( pi_wSocketIndex ) )
		return NULL;
	else
		return &m_uiItem[pi_wSocketIndex];
}

void
CGUIInventoryBoard::SetUselessSocket( BYTE pi_bySocketIndex, BOOL pi_bFlag )
{
	if( pi_bySocketIndex >= TOTAL_INVENTORY )
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
CGUIInventoryBoard::SetUselessItem( BYTE pi_bySocketIndex, BOOL pi_bFlag )
{
	if( pi_bySocketIndex >= TOTAL_INVENTORY )
		return;

	if( pi_bFlag )
	{
		m_uiUselessItemMark[pi_bySocketIndex].GetSpriteInfo()->pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
		m_uiUselessItemMark[pi_bySocketIndex].SetARGB( 0x90ffffff );
	}
	else
	{
		m_uiUselessItemMark[pi_bySocketIndex].GetSpriteInfo()->pSprite = NULL;
		m_uiUselessItemMark[pi_bySocketIndex].SetARGB( -1L );
	}
}

void
CGUIInventoryBoard::MouseEvent( _mouse_event & event )
{
	CGUIWindow::MouseEvent( event );

	if( event.id == MOUSE_STROKED )
	{
		if( m_bySelectedSocketIndex == NONE_SOCKET_INDEX )
			return;
		
		// add
		if( gScreen.ExistObject() )
		{
			if( event.source != &m_uiTradeItemMark[m_bySelectedSocketIndex] )
				return;

			PostUIMessage( WM_BOARD_ADD_OBJECT, UI_MOUSE_MSG( m_byPackID, m_bySelectedSocketIndex, 0, 0 ) );
		}
		// remove
		else
		{
			for( int i = 0; i < TOTAL_INVENTORY; ++i )
			{
				if( event.source == &m_uiTradeItemMark[i] )
				{
					m_bySelectedSocketIndex = i;
					break;
				}
			}

			if( IsEmpty( m_bySelectedSocketIndex ) || event.source != &m_uiTradeItemMark[m_bySelectedSocketIndex] )
				return;

			SetQuickEvent();

			if( g_bIsQuickEvent_Equip )
				PostUIMessage( WM_BOARD_EQUIP_HOT_KEY, UI_MOUSE_MSG( m_byPackID, m_bySelectedSocketIndex, 0, 0 ) );

			else if( g_bIsQuickEvent_Trade )
				PostUIMessage( WM_BOARD_TRADE_HOT_KEY, UI_MOUSE_MSG( m_byPackID, m_bySelectedSocketIndex, 0, 0 ) );

			else
				PostUIMessage( WM_BOARD_CHECK_REMOVE_OBJECT, UI_MOUSE_MSG( m_byPackID, m_bySelectedSocketIndex, 0, 0 ) );

			if( m_pInventoryWin )
			{
				m_pInventoryWin->m_byInputDlg_PackNo = m_byPackID;
				m_pInventoryWin->m_byInputDlg_SocketNo = m_bySelectedSocketIndex;
			}		
		}
		
	}
	else if( event.id == MOUSE_HOVERED )
	{
		for( int i=0; i<TOTAL_INVENTORY; ++i )
		{
			if( event.source == &m_uiTradeItemMark[i] )				
			{
				// socket에 갖다댔을 때 socket에 넣을수 있는지 표시해준다.
				if( gScreen.ExistObject() )
				{
					SetSelectedSocket(i);
				}
				// tooltip	
				else if( !IsEmpty( i ) ) 
				{
					PostUIMessage( WM_BOARD_HOVER_OBJECT, UI_MOUSE_MSG( m_byPackID, i, 0, 0 ) );
				}
				
				// set mouse hovered socket
				m_bySelectedSocketIndex = i;
			
				return;	
			}			
		}	
		
	}
	else if( event.id == MOUSE_LEAVED )
	{
		if( m_bySelectedSocketIndex != NONE_SOCKET_INDEX )
		{
			// remove selectedsocket
			if( event.source == &m_uiTradeItemMark[m_bySelectedSocketIndex] )				
				UnsetSelectedSocket();

			
		}			
	}
	else if( event.id == LMOUSE_STROKED )
	{
		if( m_bySelectedSocketIndex == NONE_SOCKET_INDEX )
			return;

		PostUIMessage( WM_BOARD_USE_OBJECT, UI_MOUSE_MSG( m_byPackID, m_bySelectedSocketIndex, 0, 0 ) );
		
	}	
	else if( event.id == MOUSE_CLICKED )
	{
		if( gScreen.ExistObject() )
			return;

		if( event.source == &m_uiCloseButton )
		{
			Close();
		}
	}
}

void	
CGUIInventoryBoard::ClosingDialogBox( CGUIDialogBox * pi_pDlg )
{
	CGUIWindow::ClosingDialogBox( pi_pDlg );	
}

void
CGUIInventoryBoard::UpdateUselessItemByLevel( void )
{	
	for( int i=0; i<TOTAL_INVENTORY; ++i )
	{
		if( IsEmpty( i ) )			
			continue;

		if( m_uiItem[i].IsUsable() ||
			m_uiItem[i].GetUsableLevel() == NONE_LEVEL )
			continue;

		if( _GetAvatarLevel() >= m_uiItem[i].GetUsableLevel() )
		{
			m_uiItem[i].SetUsable( TRUE );
			m_uiUselessItemMark[i].Show( FALSE );
		}
	}
}

void
CGUIInventoryBoard::PickingObjectEvent( BOOL pi_bPick )
{
	if( pi_bPick )
	{
		if( m_bySelectedSocketIndex != NONE_SOCKET_INDEX )
			SetSelectedSocket( m_bySelectedSocketIndex );
	}	
}

BOOL
CGUIInventoryBoard::Draw( void )
{
	if( !CGUIContainer::Draw() )
		return FALSE;

	if( m_bySelectedSocketIndex != NONE_SOCKET_INDEX &&
		gScreen.ExistObject() )
	{
		if( !m_uiUselessItemMark[m_bySelectedSocketIndex].GetSpriteInfo()->pSprite )
			m_uiSelectedSocket.Draw();
	}

	DrawDialogBox();

	return TRUE;
}

void
CGUIInventoryBoard::SetSelectedSocket( WORD pi_wSocketIndex )
{
	BOOL l_bAvailable;

	// item type check
	CGUIItem * l_pItem = (CGUIItem *)gScreen.GetObject();
	if( l_pItem->GetIconType() == SKILL_ICON )
		l_bAvailable = FALSE;			
	else
		l_bAvailable = TRUE;		

	// set selectedsocket image
	SPRITE_INFO * l_pSpriteInfo;
	l_pSpriteInfo = m_uiSelectedSocket.GetSpriteInfo();
	if( l_bAvailable )		
		l_pSpriteInfo->wFrameNo = 2;

	else		
		l_pSpriteInfo->wFrameNo = 4;				

	// 선택된 board에 selectedsocket 추가
	m_uiSelectedSocket.SetSize( m_uiItem[pi_wSocketIndex].m_ptSize.x + 2,
								m_uiItem[pi_wSocketIndex].m_ptSize.y + 2 );
	m_uiSelectedSocket.SetPos( m_uiItem[pi_wSocketIndex].m_ptPos.x - 1,
							   m_uiItem[pi_wSocketIndex].m_ptPos.y - 1 );

	m_uiSelectedSocket.Show( TRUE );
}

void
CGUIInventoryBoard::UnsetSelectedSocket( void )
{
	m_uiSelectedSocket.Show( FALSE );

	// reset	
	m_bySelectedSocketIndex = NONE_SOCKET_INDEX;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
//
// CGUIEquipWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////
CGUIEquipWindow::CGUIEquipWindow()
{
	m_bySelectedSocketIndex = NONE_SOCKET_INDEX;

	m_byEquipAbleIndex		= NONE_SOCKET_INDEX;
	m_byEquipAbleIndexEx	= NONE_SOCKET_INDEX;
	
	m_pInventoryWin			= NULL;

	m_bIsCreated			= FALSE;

	SetSize( 228, 287 );
}

CGUIEquipWindow::~CGUIEquipWindow()
{

}

void
CGUIEquipWindow::Init( void )
{	
	SetSize( 228, 287 );

	POINT	l_ptSize;
	l_ptSize.x = 64; l_ptSize.y = 64;
	for( int i=0; i<TOTAL_EQUIP_ITEM; ++i )
	{
		m_uiEquipItem[i].SetSize( l_ptSize );		
	}
	l_ptSize.x = 32; l_ptSize.y = 32;
	m_uiEquipItem[LRing].SetSize( l_ptSize );
	m_uiEquipItem[RRing].SetSize( l_ptSize );
	m_uiEquipItem[LAmulet].SetSize( l_ptSize );
	m_uiEquipItem[RAmulet].SetSize( l_ptSize );
	m_uiEquipItem[LBullet].SetSize( l_ptSize );
	m_uiEquipItem[RBullet].SetSize( l_ptSize );
	
	for( i=0; i<TOTAL_EQUIP_ITEM; ++i )
	{		
		m_uiEmptyEquipMark[i].SetSize( m_uiEquipItem[i].m_ptSize.x,
								       m_uiEquipItem[i].m_ptSize.y );
		m_uiTradeItemMark[i].SetSize( m_uiEquipItem[i].m_ptSize.x + 2,
								      m_uiEquipItem[i].m_ptSize.y + 2 );

		m_uiUselessSocketMark[i].SetSize( m_uiEquipItem[i].m_ptSize.x + 2,
										  m_uiEquipItem[i].m_ptSize.y + 2 );
	}

	m_uiDisableEquipShield.SetSize( 66, 66 );
	
	// -----------------------------------------------------------------
	m_uiEquipItem[Helmet].SetPos( m_ptPos.x + 82, m_ptPos.y + 7 );		
	m_uiEquipItem[Upper].SetPos( m_ptPos.x + 82, m_ptPos.y + 77 );	
	m_uiEquipItem[Lower].SetPos( m_ptPos.x + 82, m_ptPos.y + 147 );
	m_uiEquipItem[LRing].SetPos( m_ptPos.x + 44, m_ptPos.y + 217 );
	m_uiEquipItem[RRing].SetPos( m_ptPos.x + 152, m_ptPos.y + 217 );
	m_uiEquipItem[Weapon].SetPos( m_ptPos.x + 12, m_ptPos.y + 77 );	
	m_uiEquipItem[Shoes].SetPos( m_ptPos.x + 82, m_ptPos.y + 217 );
	m_uiEquipItem[Cloak].SetPos( m_ptPos.x + 152, m_ptPos.y + 147 );
	m_uiEquipItem[LAmulet].SetPos( m_ptPos.x + 150, m_ptPos.y + 39 );
	m_uiEquipItem[RAmulet].SetPos( m_ptPos.x + 185, m_ptPos.y + 39 );	
	m_uiEquipItem[Shield].SetPos( m_ptPos.x + 152, m_ptPos.y + 77 );
	m_uiEquipItem[Glove].SetPos( m_ptPos.x + 12, m_ptPos.y + 147 );
	m_uiEquipItem[LBullet].SetPos( m_ptPos.x + 11, m_ptPos.y + 39 );
	m_uiEquipItem[RBullet].SetPos( m_ptPos.x + 46, m_ptPos.y + 39 );

	for( i=0; i<TOTAL_EQUIP_ITEM; ++i )
	{
		m_uiEmptyEquipMark[i].SetPos( m_uiEquipItem[i].m_ptPos );
		m_uiTradeItemMark[i].SetPos( m_uiEquipItem[i].m_ptPos.x - 1, m_uiEquipItem[i].m_ptPos.y - 1 );
		m_uiUselessSocketMark[i].SetPos( m_uiEquipItem[i].m_ptPos.x - 1, m_uiEquipItem[i].m_ptPos.y - 1 );
	}

	// -----------------------------------------------------------------

	for( i=0; i<TOTAL_EQUIP_ITEM; ++i )
	{
		Add( &m_uiEmptyEquipMark[i] );
		Add( &m_uiEquipItem[i] );

		Add( &m_uiUselessSocketMark[i] );

		m_uiTradeItemMark[i].AddMouseEventHandler( this );
		Add( &m_uiTradeItemMark[i] );
	}

	m_uiEquipAbleSocket.AddMouseEventHandler( this );	
	m_uiEquipAbleSocketEx.AddMouseEventHandler( this );

	// -----------------------------------------------------------------
	
	SetSprite();

	m_uiSelectedSocket.SetARGB( 0x90ffffff );
	m_uiDisableEquipShield.SetARGB( 0x90ffffff );	

	for( i = 0; i < TOTAL_EQUIP_ITEM; ++i )
	{
		SetEmpty( (EquipItemBoard)i );
		SetUselessSocket( 0, i, FALSE );
	}

	m_uiEquipAbleSocket.SetTotalFrameNum( 4 );
	m_uiEquipAbleSocketEx.SetTotalFrameNum( 4 );
	m_uiEquipAbleSocket.SetObjectNumInLine( 4 );
	m_uiEquipAbleSocketEx.SetObjectNumInLine( 4 );
	m_uiEquipAbleSocket.SetFPS( 10 );
	m_uiEquipAbleSocketEx.SetFPS( 10 );

	m_bIsCreated = TRUE;
}

void
CGUIEquipWindow::SetSprite( void )
{
	if( !m_bIsCreated )
	{		
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );

		// select socket, useless socket
		l_spriteInfo.wActionNo = 1;
		l_spriteInfo.wFrameNo = 2;
		m_uiSelectedSocket.SetSpriteInfo( &l_spriteInfo );
		// equipable socket
		m_uiEquipAbleSocket.SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 5;
		m_uiDisableEquipShield.SetSpriteInfo( &l_spriteInfo );

		// equip board	
		l_spriteInfo.wActionNo = 2;	
		for( int i=0; i<TOTAL_EQUIP_ITEM-1; ++i )
		{
			l_spriteInfo.wFrameNo = i;
			m_uiEmptyEquipMark[i].SetSpriteInfo( &l_spriteInfo );
		}	
		// RBullet
		l_spriteInfo.wFrameNo = 12;
		m_uiEmptyEquipMark[RBullet].SetSpriteInfo( &l_spriteInfo );		

		// trade item mark
		l_spriteInfo.pSprite = NULL;
		l_spriteInfo.wActionNo = 0;		
		for( i=0; i<TOTAL_EQUIP_ITEM; ++i )
		{
			if( m_uiTradeItemMark[i].m_ptSize.x == 34 )
				l_spriteInfo.wFrameNo = 3;
			else			
				l_spriteInfo.wFrameNo = 4;

			m_uiTradeItemMark[i].SetSpriteInfo( &l_spriteInfo );
		}

		// useless socket
		l_spriteInfo.wActionNo = 1;
		for( i=0; i<TOTAL_EQUIP_ITEM; ++i )
		{
			if( m_uiUselessSocketMark[i].m_ptSize.x == 34 )
				l_spriteInfo.wFrameNo = 4;
			else
				l_spriteInfo.wFrameNo = 5;

			m_uiUselessSocketMark[i].SetSpriteInfo( &l_spriteInfo );
		}
	}
	else
	{
		CSprite * l_pSprite; 
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
		if( !l_pSprite )
			return;

		// select socket, useless socket		
		m_uiSelectedSocket.GetSpriteInfo()->pSprite = l_pSprite;
		// equipable socket
		m_uiEquipAbleSocket.GetSpriteInfo()->pSprite = l_pSprite;

		m_uiDisableEquipShield.GetSpriteInfo()->pSprite = l_pSprite;		

		for( int i = 0; i < TOTAL_EQUIP_ITEM; ++i )
		{
			// equip board
			if( m_uiEmptyEquipMark[i].GetSpriteInfo()->pSprite )
				m_uiEmptyEquipMark[i].GetSpriteInfo()->pSprite = l_pSprite;	

			// trade item mark		
			if( m_uiTradeItemMark[i].GetSpriteInfo()->pSprite )			
				m_uiTradeItemMark[i].GetSpriteInfo()->pSprite = l_pSprite;

			// useless socket
			if( m_uiUselessSocketMark[i].GetSpriteInfo()->pSprite )			
				m_uiUselessSocketMark[i].GetSpriteInfo()->pSprite = l_pSprite;
		}

		// Item
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_ITEM );
		for( i = 0; i < TOTAL_EQUIP_ITEM; ++i )		
		{
			if( !m_uiEquipItem[i].IsEmpty() )		
				m_uiEquipItem[i].GetSpriteInfo()->pSprite = l_pSprite;			
		}
	}
}

void	
CGUIEquipWindow::Open( void )
{
	if( m_pInventoryWin )
		m_pInventoryWin->Open();
}

void	
CGUIEquipWindow::Close( void )
{
	if( m_pInventoryWin )
		m_pInventoryWin->Close();
}

BOOL
CGUIEquipWindow::TakeInItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex,
								  DWORD pi_dwItemID, DWORD pi_dwSpriteID, BYTE pi_byNum, BYTE pi_byUsableLevel )
{
	if( pi_bySocketIndex >= TOTAL_EQUIP_ITEM )
		return FALSE;

	// item data setting
	m_uiEquipItem[pi_bySocketIndex].SetItemID( pi_dwItemID );
	m_uiEquipItem[pi_bySocketIndex].SetSpriteID( pi_dwSpriteID );
	m_uiEquipItem[pi_bySocketIndex].SetNum( 1 );
	m_uiEquipItem[pi_bySocketIndex].SetSprite();
	
	// none empty equipboard setting
	m_uiEmptyEquipMark[pi_bySocketIndex].GetSpriteInfo()->pSprite = NULL;	
	m_uiEmptyEquipMark[pi_bySocketIndex].SetARGB( -1L );


	// 양손, 쌍손 무기이면 방패를 장착할수 없도록 표시한다.	
	if( pi_bySocketIndex == Weapon &&
		_GetWeapnHandType( pi_dwItemID ) != RIGHT_HAND_WEAPON )
	{
		m_uiDisableEquipShield.SetPos( m_uiEquipItem[Shield].m_ptPos.x - 1,
									   m_uiEquipItem[Shield].m_ptPos.y - 1 );
		m_uiDisableEquipShield.Show( IsShow() );
		
		Add( &m_uiDisableEquipShield );
	}	

	// unmark trade item
	SetTradeItem( pi_byPackIndex, pi_bySocketIndex, FALSE );
	
	return TRUE;
}

BOOL	
CGUIEquipWindow::TakeOutItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex, BYTE pi_byNum, CGUIItem * po_pOutItem )
{
	if( pi_bySocketIndex >= TOTAL_EQUIP_ITEM )
		return FALSE;

	if( IsEmpty( pi_bySocketIndex ) )
		return FALSE;


	// out item setting
	if( po_pOutItem )
	{
		po_pOutItem->SetData( &m_uiEquipItem[pi_bySocketIndex] );
	}

	// 양손, 쌍손 무기를 꺼냈으면 방패소켓 disable 표시를 뺀다	
	if( pi_bySocketIndex == Weapon && IsAddedObject( &m_uiDisableEquipShield ) )	
	{
		Remove( &m_uiDisableEquipShield );
	}

	SetEmpty( pi_bySocketIndex );	

	return TRUE;
}

void
CGUIEquipWindow::SetTradeItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex, BOOL pi_bIsTradeItem )
{
	if( pi_bySocketIndex >= TOTAL_EQUIP_ITEM )
		return;

	if( pi_bIsTradeItem )
		m_uiTradeItemMark[pi_bySocketIndex].GetSpriteInfo()->pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
	else
		m_uiTradeItemMark[pi_bySocketIndex].GetSpriteInfo()->pSprite = NULL;
}

CGUIItem *
CGUIEquipWindow::GetItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex )
{
	if( pi_bySocketIndex >= TOTAL_EQUIP_ITEM )
		return NULL;

	if( m_uiEquipItem[pi_bySocketIndex].IsEmpty() )
		return NULL;

	return &m_uiEquipItem[pi_bySocketIndex];
}

void
CGUIEquipWindow::SetUselessSocket( BYTE pi_byPackIndex, BYTE pi_bySocketIndex, BOOL pi_bFlag )
{
	if( pi_bySocketIndex >= TOTAL_EQUIP_ITEM )
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
CGUIEquipWindow::SetEmpty( BYTE pi_bySocketIndex )
{
	if( pi_bySocketIndex >= TOTAL_EQUIP_ITEM )
		return;

	m_uiEquipItem[pi_bySocketIndex].SetEmpty();

	// empty equipboard setting
	m_uiEmptyEquipMark[pi_bySocketIndex].GetSpriteInfo()->pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
	m_uiEmptyEquipMark[pi_bySocketIndex].SetARGB( EMPTY_EQUIP_SOCKET_ARGB );

	// unmark trade item
	SetTradeItem( 0, pi_bySocketIndex, FALSE );
}

void
CGUIEquipWindow::MouseEvent( _mouse_event & event )
{
	CGUIWindow::MouseEvent( event );

	if( event.id == MOUSE_STROKED )
	{
		// add
		if( gScreen.ExistObject() )
		{
			if( event.source == &m_uiEquipAbleSocket )
				PostUIMessage( WM_BOARD_ADD_OBJECT, UI_MOUSE_MSG( 0, m_byEquipAbleIndex, 0, 0 ) );

			else if( event.source == &m_uiEquipAbleSocketEx )
				PostUIMessage( WM_BOARD_ADD_OBJECT, UI_MOUSE_MSG( 0, m_byEquipAbleIndexEx, 0, 0 ) );
		}
		// remove
		else
		{
			for( int i = 0; i < TOTAL_EQUIP_ITEM; ++i )
			{
				if( event.source == &m_uiTradeItemMark[i] )
				{
					m_bySelectedSocketIndex = i;
					break;
				}
			}
			if( i >= TOTAL_EQUIP_ITEM )
				return;			

			SetQuickEvent();

			if( g_bIsQuickEvent_Equip )
				PostUIMessage( WM_BOARD_EQUIP_HOT_KEY, UI_MOUSE_MSG( 0, m_bySelectedSocketIndex, 1, 0 ) );

			else if( g_bIsQuickEvent_Trade )
				PostUIMessage( WM_BOARD_TRADE_HOT_KEY, UI_MOUSE_MSG( 0, m_bySelectedSocketIndex, 1, 0 ) );

			else
				PostUIMessage( WM_BOARD_REMOVE_OBJECT, UI_MOUSE_MSG( 0, m_bySelectedSocketIndex, 1, 0 ) );
		}
	}
	else if( event.id == MOUSE_HOVERED )
	{
		if( event.source == &m_uiEquipAbleSocket )
		{
			if( gScreen.ExistObject() )				
				SetSelectedSocket( m_byEquipAbleIndex );
			
			// set mouse hovered socket	
			m_bySelectedSocketIndex = m_byEquipAbleIndex;
			
			// 장착 표시에 on mouse됐을 때는 선택 표시는 안해준다.
			m_uiSelectedSocket.Show( FALSE );			
		}
		else if( event.source == &m_uiEquipAbleSocketEx )
		{
			if( gScreen.ExistObject() )				
				SetSelectedSocket( m_byEquipAbleIndexEx );
			
			// set mouse hovered socket	
			m_bySelectedSocketIndex = m_byEquipAbleIndexEx;
			
			// 장착 표시에 on mouse됐을 때는 선택 표시는 안해준다.
			m_uiSelectedSocket.Show( FALSE );			
		}
		else
		{

			for( int i = 0; i < TOTAL_EQUIP_ITEM; ++i )
			{
				if( event.source == &m_uiTradeItemMark[i] )
				{	
					// socket에 갖다댔을 때 socket에 넣을수 있는지 표시해준다.
					if( gScreen.ExistObject() )
					{
						SetSelectedSocket( i );
					}
					// tooltip
					else if( !IsEmpty( (EquipItemBoard) i ) ) 
					{
						PostUIMessage( WM_BOARD_HOVER_OBJECT, UI_MOUSE_MSG( 0, i, 0, 0 ) );
					}
					
					// set mouse hovered socket				
					m_bySelectedSocketIndex = i;

					return;
				}
			}		
		}
	}
	else if( event.id == MOUSE_LEAVED )
	{
		if( event.source == &m_uiEquipAbleSocket )
		{
			// m_uiEquipAbleSocket에서 마우스가 벗어났을 때
			// 아이템을 장착했을 때가 아님.
			if( m_byEquipAbleIndex != NONE_SOCKET_INDEX &&
				m_byEquipAbleIndex == m_bySelectedSocketIndex &&
				gScreen.ExistObject() )
				UnsetSelectedSocket();
		}
		else if( m_bySelectedSocketIndex != NONE_SOCKET_INDEX )
		{
			// remove selectedsocket
			if( event.source == &m_uiTradeItemMark[m_bySelectedSocketIndex] )
				UnsetSelectedSocket();
		}		
	}

}

void
CGUIEquipWindow::PickingObjectEvent( BOOL pi_bPick )
{
	if( pi_bPick )
	{
		CGUIItem * l_pItem;
		l_pItem = (CGUIItem *)gScreen.GetObject();

		if( l_pItem->m_byIconType != ITEM_ICON )
			return;

		// 장착 아이템인지 체크한다.
		BOOL l_bEquipItem;	
		BYTE l_byItemType = _GetItemType( l_pItem->m_dwItemID );	

		if( l_byItemType == IID_ARMOR )
		{
			if( _GetItemExtType( l_pItem->m_dwItemID ) == IID_LSHIELD && 				
				IsAddedObject( &m_uiDisableEquipShield ) )
				l_bEquipItem = FALSE;
			else
				l_bEquipItem = TRUE;				
		}
		else if( l_byItemType == IID_WEAPON )
		{
			l_bEquipItem = TRUE;
		}
		else if( l_byItemType == IID_CONSUME )
		{
			BYTE l_byItemExtType = _GetItemExtType( l_pItem->m_dwItemID );
			if( l_byItemExtType == IID_BULLET || l_byItemExtType == IID_ARROW )
				l_bEquipItem = TRUE;
			else
				l_bEquipItem = FALSE;
		}
		else
		{
			l_bEquipItem = FALSE;
		}

		if( l_bEquipItem )
		{
			for( int i=0; i<TOTAL_EQUIP_ITEM; ++i )
			{
				if( IsEquipableItem( (EquipItemBoard)i, l_pItem->m_dwItemID ) )
				{
					// 장작할수 있는 소켓 표시
					SetEquipAbleSocket( i );
					break;
				}
			}
		}		
		else
		{
			// 장작아이템이 아니면 장착표시를 없앤다.
			UnsetEquipAbleSocket();
		}
	}
	else
	{
		UnsetEquipAbleSocket();
	}
}

BOOL
CGUIEquipWindow::Draw( void )
{
	if( !CGUIContainer::Draw() )
		return FALSE;

	// draw selected socket
	// 마우스에 아이템이 붙어있을 때 선택된 소켓표시	
	if( gScreen.ExistObject() )
	{
		m_uiSelectedSocket.Draw();
	}
	
	DrawDialogBox();

	return TRUE;
}

BOOL
CGUIEquipWindow::IsEquipableItem( EquipItemBoard pi_eItemBoard, DWORD pi_dwItemID )
{
	if( pi_eItemBoard == Weapon )
	{
		if( !_IsItemType( pi_dwItemID, IID_WEAPON ) )
			return FALSE;
	}
	else if( pi_eItemBoard == LBullet ||
			 pi_eItemBoard == RBullet )
	{
		if( !_IsItemType( pi_dwItemID, IID_CONSUME ) ||
			!( _IsItemExtType( pi_dwItemID, IID_BULLET ) ||
			   _IsItemExtType( pi_dwItemID, IID_ARROW ) ) )
			return FALSE;
	}
	else
	{
		BYTE l_byType[14] = { IID_HELMET, IID_UPPER, IID_LOWER, IID_WEAPON, IID_SHOES,
							  IID_CLOAK, IID_LSHIELD, IID_GLOVES,
							  IID_RING, IID_RING, IID_AMULET, IID_AMULET, IID_BULLET, IID_BULLET };

		if( !_IsItemType( pi_dwItemID, IID_ARMOR ) ||
			!_IsItemExtType( pi_dwItemID, l_byType[pi_eItemBoard] ) )
			return FALSE;
	}
	
	return TRUE;	
}

// -------------------------------------------------------------------------------

// hover된 socket표시
void
CGUIEquipWindow::SetSelectedSocket( BYTE pi_bySocketIndex )
{
	BOOL l_bAvailable;

	// item type check
	CGUIItem * l_pItem = gScreen.GetObject();
	if( l_pItem->GetIconType() != ITEM_ICON ||
		!IsEquipableItem( (EquipItemBoard)pi_bySocketIndex, l_pItem->m_dwItemID ) )
		l_bAvailable = FALSE;			
	else
		l_bAvailable = TRUE;			
	
	// set selectedsocket image
	SPRITE_INFO * l_pSpriteInfo;
	l_pSpriteInfo = m_uiSelectedSocket.GetSpriteInfo();
	
	if( l_bAvailable )
	{
		if( m_uiEquipItem[pi_bySocketIndex].m_ptSize.x == 32 )
			l_pSpriteInfo->wFrameNo = 2;
		else // 64 x 64
			l_pSpriteInfo->wFrameNo = 3;
	}		
	else
	{
		if( m_uiEquipItem[pi_bySocketIndex].m_ptSize.x == 32 )
			l_pSpriteInfo->wFrameNo = 4;
		else // 64 x 64
			l_pSpriteInfo->wFrameNo = 5;
	}
	
	// 선택된 board에 selectedsocket 추가
	m_uiSelectedSocket.SetSize( m_uiEquipItem[pi_bySocketIndex].m_ptSize.x + 2,
								m_uiEquipItem[pi_bySocketIndex].m_ptSize.y + 2 );
	m_uiSelectedSocket.SetPos( m_uiEquipItem[pi_bySocketIndex].m_ptPos.x - 1,
		 					   m_uiEquipItem[pi_bySocketIndex].m_ptPos.y - 1 );

	m_uiSelectedSocket.Show( TRUE );
}

void
CGUIEquipWindow::UnsetSelectedSocket( void )
{	
	m_uiSelectedSocket.Show( FALSE );

	// reset	
	m_bySelectedSocketIndex = NONE_SOCKET_INDEX;
}

// -------------------------------------------------------------------------------

// 장착 가능한 소켓 표시
void
CGUIEquipWindow::SetEquipAbleSocket( int pi_nEquipAbleIndex )
{
	// clear
	UnsetEquipAbleSocket();
	

	// set equipable socket index
	m_byEquipAbleIndex = pi_nEquipAbleIndex;	
	
	// set m_uiEquipAbleSocket
	m_uiEquipAbleSocket.SetSize( m_uiEquipItem[m_byEquipAbleIndex].m_ptSize.x + 2,
								 m_uiEquipItem[m_byEquipAbleIndex].m_ptSize.y + 2 );

	m_uiEquipAbleSocket.SetPos( m_uiEquipItem[m_byEquipAbleIndex].m_ptPos.x - 1,
								m_uiEquipItem[m_byEquipAbleIndex].m_ptPos.y - 1 );

	if( m_uiEquipAbleSocket.m_ptSize.x == 34 )
		m_uiEquipAbleSocket.GetSpriteInfo()->wFrameNo = 0;	
	else
		m_uiEquipAbleSocket.GetSpriteInfo()->wFrameNo = 1;

	// size가 바뀔 때마다 setSpriteInfo를 호출해줘야한다.
	m_uiEquipAbleSocket.SetSpriteInfo( m_uiEquipAbleSocket.GetSpriteInfo() );

	m_uiEquipAbleSocket.Show( IsShow() );	
	
	// add socket
	Add( &m_uiEquipAbleSocket );


	// ----------------------------------------------------------------
	// Bullet, Ring, Amulet일 경우 두가지 다 표시해줘야한다. 
	if( m_byEquipAbleIndex == LBullet )			
		m_byEquipAbleIndexEx = RBullet;
	
	else if( m_byEquipAbleIndex == LAmulet )
		m_byEquipAbleIndexEx = RAmulet;
	
	else if( m_byEquipAbleIndex == LRing )
		m_byEquipAbleIndexEx = RRing;
	
	if( m_byEquipAbleIndexEx != NONE_SOCKET_INDEX )
	{
		m_uiEquipAbleSocketEx.SetSize( m_uiEquipItem[m_byEquipAbleIndexEx].m_ptSize.x + 2, 
									   m_uiEquipItem[m_byEquipAbleIndexEx].m_ptSize.y + 2 );
		
		m_uiEquipAbleSocketEx.SetPos( m_uiEquipItem[m_byEquipAbleIndexEx].m_ptPos.x - 1,
									  m_uiEquipItem[m_byEquipAbleIndexEx].m_ptPos.y - 1 );

		m_uiEquipAbleSocketEx.SetSpriteInfo( m_uiEquipAbleSocket.GetSpriteInfo() );
		m_uiEquipAbleSocketEx.Show( IsShow() );
		Add( &m_uiEquipAbleSocketEx );
	}
}

void
CGUIEquipWindow::UnsetEquipAbleSocket( void )
{
	if( m_byEquipAbleIndex != NONE_SOCKET_INDEX )
	{
		// remove socket
		Remove( &m_uiEquipAbleSocket );
		m_byEquipAbleIndex = NONE_SOCKET_INDEX;
	}
	if( m_byEquipAbleIndexEx != NONE_SOCKET_INDEX )
	{
		// remove socket
		Remove( &m_uiEquipAbleSocketEx );
		m_byEquipAbleIndexEx = NONE_SOCKET_INDEX;
	}
}


////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
//
// CGUIInventoryWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////
CGUIInventoryWindow::CGUIInventoryWindow()
{	
	m_pEquipWindow			=	NULL;	

	for( int i=0; i<TOTAL_PACK; ++i )
		m_pPackWindow[i] = NULL;	

	m_byLastOpenedPack		=	0xFF;

	m_bIsOpened				=	FALSE;

	m_bOpenOnlyOnePack		=	TRUE;	

	m_dwOwnDalant			=	0;
	m_dwOwnGold				=	0;

	m_bIsCreated			=	FALSE;

	SetSize( 228, 395 );
}

CGUIInventoryWindow::~CGUIInventoryWindow()
{
	// delete pack window
	for( int i=0; i<TOTAL_PACK; ++i )
	{
		if( m_pPackWindow[i] )
			delete m_pPackWindow[i];
	}	
}

void
CGUIInventoryWindow::Init( void )
{	
	m_uiTitle.SetString( "인벤토리" );
	m_uiDalantTitle.SetString( _GetAvatarMoneyName() );
	m_uiGoldTitle.SetString( "골  드" );
	
	// -------------------------------------------------------------	
	SetSize( 228, 395 );
	
	m_uiAniTitleBar.SetSize( 228, 28 );		
	m_uiCloseButton.SetSize( 16, 13 );
	m_uiHelpButton.SetSize( 16, 13 );	

	m_uiPackOpenOption.SetSize( 41, 20 );

	for( int i=0; i<TOTAL_PACK; ++i )
		m_uiPackButton[i].SetSize( 32, 32 );	
	
	// -------------------------------------------------------------
	
	m_uiTitle.SetPos( m_ptPos.x + 6, m_ptPos.y + 7 );			
	m_uiCloseButton.SetPos( m_ptPos.x + 211, m_ptPos.y + 4 );	
	m_uiHelpButton.SetPos( m_ptPos.x + 194, m_ptPos.y + 4 );

	m_uiDalantTitle.SetPos( m_ptPos.x + 178, m_ptPos.y + 318 );
	m_uiGoldTitle.SetPos( m_ptPos.x + 178, m_ptPos.y + 335 );

	m_uiPackOpenOption.SetPos( m_ptPos.x + 12, m_ptPos.y + 324 );	

	m_uiPackButton[0].SetPos( m_ptPos.x + 15, m_ptPos.y + 353 );
	m_uiPackButton[1].SetPos( m_ptPos.x + 56, m_ptPos.y + 353 );
	m_uiPackButton[2].SetPos( m_ptPos.x + 97, m_ptPos.y + 353 );
	m_uiPackButton[3].SetPos( m_ptPos.x + 138, m_ptPos.y + 353 );
	m_uiPackButton[4].SetPos( m_ptPos.x + 179, m_ptPos.y + 353 );	

	// -------------------------------------------------------------
	
	Add( &m_uiAniTitleBar );
	Add( &m_uiTitle );		
	m_uiCloseButton.AddMouseEventHandler( this );
	m_uiHelpButton.AddMouseEventHandler( this );
	Add( &m_uiCloseButton );	
	Add( &m_uiHelpButton );	

	Add( &m_uiDalantTitle );
	Add( &m_uiGoldTitle );
	Add( &m_uiDalant );
	Add( &m_uiGold );

	m_uiPackOpenOption.AddMouseEventHandler( this );
	Add( &m_uiPackOpenOption );

	for( i=0; i<TOTAL_PACK; ++i )		
		Add( &m_uiPackButton[i] );		

	// -------------------------------------------------------------
	
	SetSprite();

	SetARGB( 0xCCFFFFFF );

	m_uiTitle.GetFont()->dwColor = TEXT_COLOR;
	m_uiDalantTitle.GetFont()->dwColor = D3DCOLOR_XRGB( 212, 186, 148 );
	m_uiGoldTitle.GetFont()->dwColor = D3DCOLOR_XRGB( 212, 186, 148 );
	m_uiDalant.GetFont()->dwColor = TEXT_COLOR;
	m_uiGold.GetFont()->dwColor = TEXT_COLOR;

	// -------------------------------------------------------------

	SetMoveObject( &m_uiAniTitleBar );	
	SetMoveObject( &m_uiTitle );	
	SetMovable( TRUE );

	// docking
	EnableDocking( TRUE );	

	
	m_uiAniTitleBar.SetTotalFrameNum( 18 );
	m_uiAniTitleBar.SetObjectNumInLine( 1 );
	m_uiAniTitleBar.SetFPS( 10 );
	m_uiAniTitleBar.SetStopInterval( 1000 );	

	// --------------------------------------------------
	SetName( "Inventory" );	

	SetOpenOnlyOnePack( m_bOpenOnlyOnePack );

	SoundOn( TRUE );

	m_bIsCreated = TRUE;
	
}

void
CGUIInventoryWindow::SetSprite( void )
{
	if( !m_bIsCreated )
	{		
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );

		// base board	
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 0;
		SetSpriteInfo( &l_spriteInfo );				

		// ani title bar	
		l_spriteInfo.wFrameNo = 5;
		m_uiAniTitleBar.SetSpriteInfo( &l_spriteInfo );

		// pack option
		l_spriteInfo.wFrameNo = 1;
		m_uiPackOpenOption.SetSpriteInfoEx( &l_spriteInfo );		
		
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
		// sprite 포인터만 다시 설정한다.

		CSprite * l_pSprite; 
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
		if( !l_pSprite )
			return;
		
		GetSpriteInfo()->pSprite = l_pSprite;				

		// ani title bar			
		m_uiAniTitleBar.GetSpriteInfo()->pSprite = l_pSprite;

		// pack option		
		m_uiPackOpenOption.SetSprite( l_pSprite );	

		// pack button
		for( int i=0; i<TOTAL_PACK; ++i )
		{
			if( m_uiPackButton[i].GetSpriteInfo()->pSprite != NULL )			
				m_uiPackButton[i].GetSpriteInfo()->pSprite = l_pSprite;
		}				

		// close button	
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );
		m_uiCloseButton.SetSprite( l_pSprite );
		// help button
		m_uiHelpButton.SetSprite( l_pSprite );			


		// 배낭도 다시 로딩한다.
		for( i=0; i<TOTAL_PACK; ++i )
		{
			if( m_pPackWindow[i] != NULL )
				m_pPackWindow[i]->SetSprite();
		}
		
		// 장착창
		if( m_pEquipWindow )
			m_pEquipWindow->SetSprite();
	}
}

void
CGUIInventoryWindow::Open( void )
{
	CGUIWindow::Open();

	OpenDockingPack( TRUE );	
}

void
CGUIInventoryWindow::Close( void )
{
	CGUIWindow::Close();

	OpenDockingPack( FALSE );

	_CloseAllDialogBox( this );
	_CloseDialogBox( NULL, MSG_BOX_ID_INVEN_ETC );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void	
CGUIInventoryWindow::SetEquipWindow( CGUIEquipWindow * pi_pWindow )
{
	if( m_pEquipWindow )
		Remove( m_pEquipWindow );

	m_pEquipWindow = pi_pWindow;

	m_pEquipWindow->SetInventoryWin( this );

	m_pEquipWindow->SetPos( m_ptPos.x, m_uiAniTitleBar.m_ptPos.y + m_uiAniTitleBar.m_ptSize.y );
	m_pEquipWindow->Show( IsShow() );
	Add( m_pEquipWindow );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CGUIInventoryWindow::AddPack( BYTE pi_byPackIndex, BOOL pi_bNewPack )
{
	if( pi_byPackIndex >= TOTAL_PACK || m_pPackWindow[pi_byPackIndex] )
		return FALSE;

	// pack icon setting
	SPRITE_INFO l_spriteInfo;
	l_spriteInfo.pSprite = GetSpriteInfo()->pSprite;
	l_spriteInfo.wActionNo = 0;
	l_spriteInfo.wFrameNo = 6;
	m_uiPackButton[pi_byPackIndex].SetSpriteInfo( &l_spriteInfo );
	m_uiPackButton[pi_byPackIndex].AddMouseEventHandler( this );


	// set pack window
	m_pPackWindow[pi_byPackIndex] = new CGUIInventoryBoard;
	m_pPackWindow[pi_byPackIndex]->Init( pi_byPackIndex );
	m_pPackWindow[pi_byPackIndex]->SetInventoryWindow( this );	

	SetPackState( pi_byPackIndex, FALSE );

	// add window
	gScreen.Add( m_pPackWindow[pi_byPackIndex] );

	// set docking
	AddDockingCustomer( m_pPackWindow[pi_byPackIndex] );	

	// 디폴트로 인벤 아래쪽에 도킹시킨다.
	m_pPackWindow[pi_byPackIndex]->EnableDockingBorder( CGUIWindow::BOTTOM, TRUE ); // 인벤 아래쪽만 되게 한다.
	m_pPackWindow[pi_byPackIndex]->SetDockingBorder( CGUIWindow::BOTTOM );

	// 디폴트로 첫번째 배낭만 열어준다.
	if( pi_byPackIndex == 0 )
	{
		SetPackState( pi_byPackIndex, IsShow() );

		// 장착창을 열때 같이 열기 위해
		m_pPackWindow[pi_byPackIndex]->SetClosedWithEquipWin( TRUE );
		
		if( IsShow() )
		{
			m_pPackWindow[pi_byPackIndex]->Open();
		}				
	}	

	return TRUE;
}

void	
CGUIInventoryWindow::RemovePack( BYTE pi_byPackIndex )
{
	if( !m_pPackWindow[pi_byPackIndex] )
		return;

	m_uiPackButton[pi_byPackIndex].GetSpriteInfo()->pSprite = NULL;
	m_uiPackButton[pi_byPackIndex].RemoveMouseEventHandler();

	gScreen.Remove( m_pPackWindow[pi_byPackIndex] );

	// remove docking
	RemoveDockingCustomer( m_pPackWindow[pi_byPackIndex] );

	delete m_pPackWindow[pi_byPackIndex];
	m_pPackWindow[pi_byPackIndex] = NULL;
}

void	
CGUIInventoryWindow::OpenPack( BYTE pi_byPackIndex, BOOL pi_bOpen )
{
	if( pi_byPackIndex >= TOTAL_PACK )
		return;

	if( !m_pPackWindow[pi_byPackIndex] )
		return;

	if( pi_bOpen )
	{
		m_pPackWindow[pi_byPackIndex]->Open();

		PostUIMessage( WM_BOARD_OPEN_PACK, UI_MOUSE_MSG( pi_byPackIndex, 0, 0, 0 ) );
	}
	else
	{
		m_pPackWindow[pi_byPackIndex]->Close();
	}
}

void
CGUIInventoryWindow::SetPackState( BYTE pi_byPackIndex, BOOL pi_bOpen )
{
	if( !m_pPackWindow[pi_byPackIndex] )
		return;

	if( pi_bOpen )
	{
		m_uiPackButton[pi_byPackIndex].GetSpriteInfo()->wFrameNo = 6 + _GetAvatarRace()/2 * 2;		
	}
	else
	{
		m_uiPackButton[pi_byPackIndex].GetSpriteInfo()->wFrameNo = 7 + _GetAvatarRace()/2 * 2;
	}
}

BOOL
CGUIInventoryWindow::IsOpenedPack( BYTE pi_byPackIndex )
{
	if( pi_byPackIndex >= TOTAL_PACK )
		return FALSE;

	if( !m_pPackWindow[pi_byPackIndex] )
		return FALSE;

	return m_pPackWindow[pi_byPackIndex]->IsShow();
}

// docking된 pack window를 모두 닫는다.
void
CGUIInventoryWindow::OpenDockingPack( BOOL pi_bOpen )
{
	for( int i=0; i<TOTAL_PACK; ++i )
	{
		if( !m_pPackWindow[i] )
			continue;

		if( m_pPackWindow[i]->GetDockingBorder() != CGUIWindow::NONE_BORDER )
		{
			if( pi_bOpen )
			{
				if( m_pPackWindow[i]->m_bIsClosedWidhEquipWin )
					m_pPackWindow[i]->Open();
			}
			else
			{
				// 닫을 때 같이 닫으면 열때도 같이 연다.
				if( m_pPackWindow[i]->IsShow() )
					m_pPackWindow[i]->m_bIsClosedWidhEquipWin = TRUE;
				else 
					m_pPackWindow[i]->m_bIsClosedWidhEquipWin = FALSE;

				m_pPackWindow[i]->Close();				
			}
		}
	}	
}

void
CGUIInventoryWindow::SetOpenOnlyOnePack( BOOL pi_bOneOpen )
{
	m_bOpenOnlyOnePack = pi_bOneOpen;	

	SPRITE_INFO	l_sSpriteInfo;
	memcpy( &l_sSpriteInfo, m_uiPackOpenOption.GetSpriteInfo(), sizeof( SPRITE_INFO ) );

	if( m_bOpenOnlyOnePack )
	{
		l_sSpriteInfo.wFrameNo = 2;
		m_uiPackOpenOption.SetSpriteInfoEx( &l_sSpriteInfo );

		// 한개 이상 열려있으면 최근에 열것을 제외하고 모두 닫는다.
		int l_nOpenCount = 0;
		for( int i=0; i<TOTAL_PACK; ++i )
		{
			if( !m_pPackWindow[i] )
				continue;	

			if( m_pPackWindow[i]->IsShow() )
				++l_nOpenCount;			
		}

		if( l_nOpenCount > 1 )
		{
			for( i=0; i<TOTAL_PACK; ++i )
			{
				if( !m_pPackWindow[i] )
					continue;

				if( i != m_byLastOpenedPack )
					m_pPackWindow[i]->Close();
			}
		}		
	}
	else
	{
		l_sSpriteInfo.wFrameNo = 1;
		m_uiPackOpenOption.SetSpriteInfoEx( &l_sSpriteInfo );
	}
}


//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CGUIInventoryWindow::TakeInItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex,
								 DWORD pi_dwItemID, DWORD pi_dwSpriteID, BYTE pi_byNum, BYTE pi_byUsableLevel )								  
{
	if( pi_byNum == 0 || 
		pi_byPackIndex >= TOTAL_PACK ||
		!m_pPackWindow[pi_byPackIndex] )
		return FALSE;

	return m_pPackWindow[pi_byPackIndex]->TakeInItem( pi_bySocketIndex,
													  pi_dwItemID,
													  pi_dwSpriteID,
													  pi_byNum,
													  pi_byUsableLevel );														  	
}

BOOL
CGUIInventoryWindow::TakeOutItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex, BYTE pi_byNum, CGUIItem * po_pOutItem )
{
	if( pi_byPackIndex >= TOTAL_PACK ||
		!m_pPackWindow[pi_byPackIndex] )
		return FALSE;
	
	return m_pPackWindow[pi_byPackIndex]->TakeOutItem( pi_bySocketIndex, pi_byNum, po_pOutItem );
}

void
CGUIInventoryWindow::SetTradeItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex, BOOL pi_bIsTradeItem )
{
	if( pi_byPackIndex >= TOTAL_PACK ||
		!m_pPackWindow[pi_byPackIndex] )
		return;

	m_pPackWindow[pi_byPackIndex]->SetTradeItem( pi_bySocketIndex, pi_bIsTradeItem );
}

CGUIItem *	
CGUIInventoryWindow::GetItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex )
{
	if( pi_byPackIndex >= TOTAL_PACK ||
		!m_pPackWindow[pi_byPackIndex] )
		return NULL;

	return m_pPackWindow[pi_byPackIndex]->GetItem( pi_bySocketIndex );
}

void
CGUIInventoryWindow::SetUselessSocket( BYTE pi_byPackIndex, BYTE pi_bySocketIndex, BOOL pi_bFlag )
{
	if( pi_byPackIndex >= TOTAL_PACK ||
		!m_pPackWindow[pi_byPackIndex] )
		return;

	m_pPackWindow[pi_byPackIndex]->SetUselessSocket( pi_bySocketIndex, pi_bFlag );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CGUIInventoryWindow::Equip( BYTE pi_bySocketIndex, DWORD pi_dwItemID, DWORD pi_dwSpriteID )
{
	if( m_pEquipWindow )
		return m_pEquipWindow->TakeInItem( 0, pi_bySocketIndex, pi_dwItemID, pi_dwSpriteID, 1 );	

	return FALSE;
}

BOOL
CGUIInventoryWindow::UnEquip( BYTE pi_bySocketIndex )
{
	if( m_pEquipWindow )
		return m_pEquipWindow->TakeOutItem( 0, pi_bySocketIndex, 1, NULL );

	return FALSE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CGUIInventoryWindow::SetOwnDalant( DWORD pi_dwDalant )
{
	m_dwOwnDalant = pi_dwDalant;

	char buf[32];
	_GetPriceStr( m_dwOwnDalant, buf );
	m_uiDalant.SetString( buf );

	m_uiDalant.SetPos( m_ptPos.x + m_ptSize.x - m_uiDalant.m_ptSize.x - 60, m_ptPos.y + 318 );
}

void
CGUIInventoryWindow::SetOwnGold( DWORD pi_dwGold )
{
	m_dwOwnGold = pi_dwGold;

	char buf[32];
	_GetPriceStr( m_dwOwnGold, buf );
	m_uiGold.SetString( buf );

	m_uiGold.SetPos( m_ptPos.x + m_ptSize.x - m_uiGold.m_ptSize.x - 60, m_ptPos.y + 335 );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CGUIInventoryWindow::MouseEvent( _mouse_event & event )
{
	CGUIWindow::MouseEvent( event );

	if( m_pEquipWindow )
		m_pEquipWindow->MouseEvent( event );


	if( event.id == MOUSE_STROKED )
	{	
		// pack window button
		for( int i=0; i<TOTAL_PACK; ++i )
		{
			if( event.source == &m_uiPackButton[i] )
			{
				OpenPack( i, !IsOpenedPack( i ) );

				return;
			}
		}

		// pack option
		if( event.source == &m_uiPackOpenOption )
		{
			SetOpenOnlyOnePack( !m_bOpenOnlyOnePack );
			return;
		}
		
	}	
	else if( event.id == MOUSE_CLICKED )
	{
		// window close
		if( event.source == &m_uiCloseButton )
		{
			Close();			
		}
		else if( event.source == &m_uiHelpButton )
		{
			// 도움말 창 열기
			PostMessage( gScreen.GetHWND(), WM_OPEN_WINDOW_HELP, m_dwID, 0 );
		}
	}
	
}


void
CGUIInventoryWindow::ClosingDialogBox( CGUIDialogBox * pi_pDlg )
{
	if( pi_pDlg->GetResultMsg() == MSG_OK )
	{
		CGUINumInputDlg * pInputDlg = static_cast< CGUINumInputDlg * >( pi_pDlg );
		WORD wQuantity = atoi( pInputDlg->GetInput() );

		PostUIMessage( WM_BOARD_REMOVE_OBJECT,
					   UI_MOUSE_MSG( m_byInputDlg_PackNo, m_byInputDlg_SocketNo, wQuantity, 0 ) );
	}

	CGUIWindow::ClosingDialogBox( pi_pDlg );	
}


// level up 됐을 때 inven에 모든 item에 대해 uesless item을 update해준다.
void
CGUIInventoryWindow::UpdateUselessItemByLevel( void )
{
	for( int i=0; i<TOTAL_PACK; ++i )
	{
		if( !m_pPackWindow[i] )
			continue;

		m_pPackWindow[i]->UpdateUselessItemByLevel();	
	}
}

void
CGUIInventoryWindow::PickingObjectEvent( BOOL pi_bPick )
{
	if( m_pEquipWindow )
		m_pEquipWindow->PickingObjectEvent( pi_bPick );	
}