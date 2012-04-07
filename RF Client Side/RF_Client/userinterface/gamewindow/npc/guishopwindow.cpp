////////////////////////////////////////////////////////////////////////////
//
// CGUIShopWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "GUIShopWindow.h"
#include "../../GUIController/GUIMessagebox.h"
#include "../../GUIController/GUIScreen.h"
#include "../../SpriteMgr.h"
#include "../../UIGlobal.h"
#include "../GUIInventoryWindow.h"
#include "../GUIItemBeltWindow.h"
#include "../../../DefineMessage.h"

#define	TEXT_COLOR		D3DCOLOR_XRGB(195,193,190)
#define	TEXT_COLOR2		D3DCOLOR_XRGB(212,186,148)  //D3DCOLOR_XRGB(150,207,204)


////////////////////////////////////////////////////////////////////////////

CGUIShopWindow::CGUIShopWindow()
{
	m_eCurTradeMode		= None_Mode;
	
	m_byCurPageNo		= 0;

	m_eCurShopType		= (ShopType)0;

	m_bIsCreated		= FALSE;
}

CGUIShopWindow::~CGUIShopWindow()
{	
	ClearBuyItemList();

	ClearTradeItemList();	
}

void
CGUIShopWindow::Init( void )
{	
	POINT l_ptPos;
	int i, j;	

	// ---------------------------------------------------------
	// Size
	SetSize( 228, 395 );	

	m_uiAniTitleBar.SetSize( 228, 28 );	

	// page up, down	
	m_uiPageBoard.SetSize( 68, 17 );
	m_uiPageUp.SetSize( 45, 16 );
	m_uiPageDown.SetSize( 45, 16 );		

	m_uiOKButton.SetSize( 77, 25 );	

	// close button	
	m_uiCloseButton.SetSize( 16, 13 );
	m_uiHelpButton.SetSize( 16, 13 );

	// trade button
	m_uiTradeTab[Buy_Mode].SetSize( 76, 30 );
	m_uiTradeTab[Repair_Mode].SetSize( 76, 30 );
	m_uiTradeTab[Sell_Mode].SetSize( 72, 30 );
	
	// shop item socket
	for( i=0; i<TOTAL_SHOP_ITEM_SOCKET; ++i )		
		m_uiShopItemBoard[i].SetSize( 34, 34 );


	m_uiTitle.SetString( "상점 거래" );

	m_uiDalant.SetString( _GetAvatarMoneyName() );
	m_uiGold.SetString( "골  드" );

	m_uiTradeTab[Buy_Mode].SetLabel( "구  입" );
	m_uiTradeTab[Repair_Mode].SetLabel( "수  리" );
	m_uiTradeTab[Sell_Mode].SetLabel( "판  매" );
	m_uiOKButton.SetLabel( "확 인" );
	
	
	// ---------------------------------------------------------
	// Position
	m_uiTitle.SetPos( m_ptPos.x + 6, m_ptPos.y + 7 );

	// page up, down
	m_uiPageBoard.SetPos( 80, 241 );
	m_uiPageUp.SetPos( 39, 243 );
	m_uiPageDown.SetPos( 144, 243 );
	m_uiCurPageStr.SetPos( m_uiPageBoard.m_ptPos.x + 18, m_uiPageBoard.m_ptPos.y + 4 );
	
	m_uiTradeMoneyTitle.SetPos( 82, 291 );
	
	m_uiDalant.SetPos( 162, 317 );
	m_uiGold.SetPos( 162, 346 );

	m_uiOKButton.SetPos( 75, 362 );	

	// close button	
	m_uiCloseButton.SetPos( 211, 4 );
	m_uiHelpButton.SetPos( 194, 4 );

	// trade button
	TabLabelPosSetting( Buy_Mode, FALSE );
	TabLabelPosSetting( Sell_Mode, FALSE );
	TabLabelPosSetting( Repair_Mode, FALSE );	

	// shop item board	
	int index = 0;
	for( i=0; i<4; ++i )
	{
		for( j=0; j<5; ++j )
		{
			l_ptPos.x = 22 + j * 38;
			l_ptPos.y = 78 + i * 38;
			m_uiShopItemBoard[index].SetPos( l_ptPos.x - 1, l_ptPos.y - 1 );
			++index;
		}
	}	

	// ---------------------------------------------------------
	// add controller
	Add( &m_uiAniTitleBar );
	Add( &m_uiTitle );	

	m_uiPageUp.AddMouseEventHandler( this );
	m_uiPageDown.AddMouseEventHandler( this );
	m_uiCloseButton.AddMouseEventHandler( this );
	m_uiHelpButton.AddMouseEventHandler( this );

	Add( &m_uiPageBoard );
	Add( &m_uiPageUp );
	Add( &m_uiPageDown );
	Add( &m_uiCurPageStr );

	Add( &m_uiTradeMoneyTitle );
	Add( &m_uiTotalTradeDalantStr );
	Add( &m_uiTotalTradeGoldStr );
	Add( &m_uiDalant );
	Add( &m_uiGold );

	Add( &m_uiCloseButton );
	Add( &m_uiHelpButton );

	m_uiTradeTab[Buy_Mode].AddMouseEventHandler( this );
	m_uiTradeTab[Sell_Mode].AddMouseEventHandler( this );
	m_uiTradeTab[Repair_Mode].AddMouseEventHandler( this );


	// shop item board	
	for( i=0; i<TOTAL_SHOP_ITEM_SOCKET; ++i )
	{		
		m_uiShopItemBoard[i].AddMouseEventHandler( this );
		Add( &m_uiShopItemBoard[i] );
	}	

	m_uiOKButton.AddMouseEventHandler( this );
	Add( &m_uiOKButton );	
	

	// ---------------------------------------------------------

	SetMoveObject( this );	
	SetMoveObject( &m_uiAniTitleBar );
	SetMoveObject( &m_uiTitle );
	SetMovable( true );

	
	m_uiAniTitleBar.SetTotalFrameNum( 18 );
	m_uiAniTitleBar.SetObjectNumInLine( 1 );
	m_uiAniTitleBar.SetFPS( 10 );
	m_uiAniTitleBar.SetStopInterval( 1000 );	

	SetSprite();

	SetARGB( 0xCCFFFFFF );		

	m_uiTitle.GetFont()->dwColor = TEXT_COLOR;

	m_uiTradeMoneyTitle.GetFont()->dwColor = TEXT_COLOR2;
	m_uiDalant.GetFont()->dwColor = TEXT_COLOR2;
	m_uiGold.GetFont()->dwColor = TEXT_COLOR2;
	
	m_uiTotalTradeDalantStr.GetFont()->dwColor = TEXT_COLOR;
	m_uiTotalTradeGoldStr.GetFont()->dwColor = TEXT_COLOR;

	m_uiCurPageStr.GetFont()->dwColor = D3DCOLOR_XRGB( 232, 232, 232 );

	m_uiTradeTab[Buy_Mode].SetLabelColor( D3DCOLOR_XRGB( 232, 232, 232 ) );
	m_uiTradeTab[Sell_Mode].SetLabelColor( D3DCOLOR_XRGB( 232, 232, 232 ) );
	m_uiTradeTab[Repair_Mode].SetLabelColor( D3DCOLOR_XRGB( 232, 232, 232 ) );

	m_uiOKButton.SetLabelColor( D3DCOLOR_XRGB( 232, 232, 232 ) );	

	m_uiOKButton.SetLabelStyle( R3_HAN_OUTLINE );

	SetName( "Shop" );

	SetDalant( 0 );
	SetGold( 0 );

	m_bIsCreated = TRUE;
}

void
CGUIShopWindow::SetSprite( void )
{
	if( !m_bIsCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_SHOP );

		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 0;
		SetSpriteInfo( &l_spriteInfo );		
		
		// trade button		
		l_spriteInfo.wActionNo = 1;		
		l_spriteInfo.wFrameNo = 0;
		m_uiTradeTab[Buy_Mode].SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 2;
		m_uiTradeTab[Sell_Mode].SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 4;
		m_uiTradeTab[Repair_Mode].SetSpriteInfoEx( &l_spriteInfo );

		// page up, down button
		l_spriteInfo.wActionNo = 2;
		l_spriteInfo.wFrameNo = 0;
		m_uiPageBoard.SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 1;
		m_uiPageUp.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 2;
		m_uiPageUp.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 3;
		m_uiPageUp.SetSpriteInfoEx( DISABLED, &l_spriteInfo );

		l_spriteInfo.wFrameNo = 4;
		m_uiPageDown.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 5;
		m_uiPageDown.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 6;
		m_uiPageDown.SetSpriteInfoEx( DISABLED, &l_spriteInfo );
		

		// base anibar
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );	
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 5;
		m_uiAniTitleBar.SetSpriteInfo( &l_spriteInfo );			


		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );	
		// ok button		
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 12;
		m_uiOKButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 13;
		m_uiOKButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 14;
		m_uiOKButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 15;
		m_uiOKButton.SetSpriteInfoEx( DISABLED, &l_spriteInfo );

		
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
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_SHOP );	
		if( !l_pSprite )
			return;

		GetSpriteInfo()->pSprite = l_pSprite;		

		// page up, down button
		m_uiPageBoard.GetSpriteInfo()->pSprite = l_pSprite;
		
		m_uiPageUp.SetSprite( l_pSprite );
		m_uiPageDown.SetSprite( l_pSprite );		

		// trade button		
		for( int i=0; i<3; ++i )
		{			
			m_uiTradeTab[i].SetSprite( l_pSprite );
		}

		// base anibar
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );	
		m_uiAniTitleBar.GetSpriteInfo()->pSprite = l_pSprite;
		
		
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );	

		// ok button				
		m_uiOKButton.SetSprite( l_pSprite );

		// close button	
		m_uiCloseButton.SetSprite( l_pSprite );

		// help button
		m_uiHelpButton.SetSprite( l_pSprite );


		l_pSprite = gSpriteMgr.GetSprite( SP_ID_ITEM );
		
		// buy item list
		CGUIItemList::iterator it;
		for( it = m_listBuyItem.begin(); it != m_listBuyItem.end(); ++it )
		{
			(*it)->GetSpriteInfo()->pSprite = l_pSprite;
		}		

		// trade item list		
		CGUITradeItemList::iterator it2;
		for( it2 = m_listTradeItem.begin(); it2 != m_listTradeItem.end(); ++it2 )
		{
			(*it2)->pItem->GetSpriteInfo()->pSprite = l_pSprite;			
		}		

		// shop board
		for( i=0; i<TOTAL_SHOP_ITEM_SOCKET; ++i )
		{
//			if( !m_uiShopItemBoard[i].IsEmpty() )
//				m_uiShopItemBoard[i].GetSpriteInfo()->pSprite = l_pSprite;
		}
	}
}

void
CGUIShopWindow::Close( void )
{
	if( !IsShow() )
		return;

	CGUIWindow::Close();
	
	_CloseAllDialogBox( this );
	_CloseDialogBox( NULL, MSGBOX_ID_SHOP_ETC );
}

// ===========================================================================================

void
CGUIShopWindow::SetShopType( ShopType pi_eShopType )
{
	m_eCurShopType = pi_eShopType;

	/*
	if( m_eCurShopType == Unit )
	{
		// 구입, 판매, 수리
		m_uiTradeTab[Buy_Mode].SetPos( m_ptPos.x + 2, m_ptPos.y + 28 );
		m_uiTradeTab[Sell_Mode].SetPos( m_ptPos.x + 78, m_ptPos.y + 28 );
		m_uiTradeTab[Repair_Mode].SetPos( m_ptPos.x + 150, m_ptPos.y + 28 );		

		m_uiTradeTab[Buy_Mode].Show( IsShow() );
		m_uiTradeTab[Sell_Mode].Show( IsShow() );
		m_uiTradeTab[Repair_Mode].Show( IsShow() );

		Add( &m_uiTradeTab[Buy_Mode] );
		Add( &m_uiTradeTab[Sell_Mode] );
		Add( &m_uiTradeTab[Repair_Mode] );		


		// sell button reset
		m_uiTradeTab[Sell_Mode].SetSize( 72, 30 );
		TabLabelPosSetting( Sell_Mode, FALSE );

		SPRITE_INFO l_spriteInfo;
		l_spriteInfo.pSprite = GetSpriteInfo()->pSprite;
		l_spriteInfo.wActionNo = 1;
		l_spriteInfo.wFrameNo = 2;
		m_uiTradeTab[Sell_Mode].SetSpriteInfoEx( &l_spriteInfo );		
	}	
	else 
	{
		// 구입, 판매
		// remove repair button
		Remove( &m_uiTradeTab[Repair_Mode] );
		
		m_uiTradeTab[Buy_Mode].SetPos( m_ptPos.x + 2, m_ptPos.y + 28 );
		m_uiTradeTab[Sell_Mode].SetPos( m_ptPos.x + 150, m_ptPos.y + 28 );		

		m_uiTradeTab[Buy_Mode].Show( IsShow() );
		m_uiTradeTab[Sell_Mode].Show( IsShow() );

		Add( &m_uiTradeTab[Buy_Mode] );
		Add( &m_uiTradeTab[Sell_Mode] );
		
		// sell button reset
		m_uiTradeTab[Sell_Mode].SetSize( 76, 30 );
		TabLabelPosSetting( Sell_Mode, FALSE );

		SPRITE_INFO l_spriteInfo;
		l_spriteInfo.pSprite = GetSpriteInfo()->pSprite;
		l_spriteInfo.wActionNo = 1;
		l_spriteInfo.wFrameNo = 4;
		m_uiTradeTab[Sell_Mode].SetSpriteInfoEx( &l_spriteInfo );		
	}
	*/

	// 구입, 판매
	// remove repair button
	Remove( &m_uiTradeTab[Repair_Mode] );
	
	m_uiTradeTab[Buy_Mode].SetPos( m_ptPos.x + 2, m_ptPos.y + 28 );
	m_uiTradeTab[Sell_Mode].SetPos( m_ptPos.x + 150, m_ptPos.y + 28 );		

	m_uiTradeTab[Buy_Mode].Show( IsShow() );
	m_uiTradeTab[Sell_Mode].Show( IsShow() );

	Add( &m_uiTradeTab[Buy_Mode] );
	Add( &m_uiTradeTab[Sell_Mode] );
	
	// sell button reset
	m_uiTradeTab[Sell_Mode].SetSize( 76, 30 );
	TabLabelPosSetting( Sell_Mode, FALSE );

	SPRITE_INFO l_spriteInfo;
	l_spriteInfo.pSprite = GetSpriteInfo()->pSprite;
	l_spriteInfo.wActionNo = 1;
	l_spriteInfo.wFrameNo = 4;
	m_uiTradeTab[Sell_Mode].SetSpriteInfoEx( &l_spriteInfo );	
}

// ===========================================================================================

void
CGUIShopWindow::SetTradeMode( TradeMode pi_eTrade )
{	
	SPRITE_INFO l_spriteInfo;

	if( m_eCurTradeMode != None_Mode )
	{
		memcpy( &l_spriteInfo, m_uiTradeTab[m_eCurTradeMode].GetSpriteInfo(), sizeof( SPRITE_INFO ) );


//		if( m_eCurTradeMode == Sell_Mode &&
//			m_eCurShopType != Unit )
		if( m_eCurTradeMode == Sell_Mode )
		{
			// 구입, 판매만 있는 shop의 sell_mode인가
			// set Inactive button
			l_spriteInfo.wFrameNo = 4;
			m_uiTradeTab[m_eCurTradeMode].SetSpriteInfoEx( &l_spriteInfo );							
		}
		else 
		{
			// 구입, 판매, 수리
			// set Inactive button
			l_spriteInfo.wFrameNo = m_eCurTradeMode * 2;
			m_uiTradeTab[m_eCurTradeMode].SetSpriteInfoEx( &l_spriteInfo );			
		}


		TabLabelPosSetting( m_eCurTradeMode, FALSE );
	}		

	// set mode
	m_eCurTradeMode = pi_eTrade;

	if( m_eCurTradeMode == None_Mode )
		return;	


	// set active button
	memcpy( &l_spriteInfo, m_uiTradeTab[m_eCurTradeMode].GetSpriteInfo(), sizeof( SPRITE_INFO ) );
	
	// 구입, 판매만 있는 shop의 sell_mode인가
//	if( m_eCurTradeMode == Sell_Mode &&
//		m_eCurShopType != Unit )
	if( m_eCurTradeMode == Sell_Mode )
		l_spriteInfo.wFrameNo = 5;
	else
		l_spriteInfo.wFrameNo = m_eCurTradeMode * 2 + 1;	

	m_uiTradeTab[m_eCurTradeMode].SetSpriteInfoEx( &l_spriteInfo );

	TabLabelPosSetting( m_eCurTradeMode, TRUE );

	switch( m_eCurTradeMode )
	{
	case Buy_Mode:
		m_uiTradeMoneyTitle.SetString( "총 구입비용" );
		break;
	case Sell_Mode:
		m_uiTradeMoneyTitle.SetString( "총 판매비용" );
		break;
	case Repair_Mode:
		m_uiTradeMoneyTitle.SetString( "총 수리비용" );
		break;
	}	


	// shop board에 item을 보여준다.
	UpdateShopBoard();
}

// ===========================================================================================

BOOL
CGUIShopWindow::TakeInBuyItem( UI_SHOP_ITEM * pi_pItemInfo, BOOL pi_bInsertAtBegin )
{
	CGUIItem * l_pNewItem;

	l_pNewItem = new CGUIItem;

	l_pNewItem->SetItemID( pi_pItemInfo->dwMeshID );
	l_pNewItem->SetSpriteID( pi_pItemInfo->dwIconID );
	l_pNewItem->SetNum( pi_pItemInfo->dwQuantity );
	l_pNewItem->SetUsableLevel( pi_pItemInfo->byUsableLevel );
	l_pNewItem->SetIconType( ITEM_ICON );
	l_pNewItem->SetSize( 32, 32 );
	l_pNewItem->SetSprite();
	l_pNewItem->Show( IsShow() );
	l_pNewItem->DrawItemNum( FALSE );

	if( pi_bInsertAtBegin )
		m_listBuyItem.insert( m_listBuyItem.begin(), l_pNewItem );
	else
		m_listBuyItem.insert( m_listBuyItem.end(), l_pNewItem );

	// board update
	if( m_eCurTradeMode == Buy_Mode )
	{
		m_byTotalPageNo = m_listBuyItem.size() / TOTAL_SHOP_ITEM_SOCKET;

		if( ( m_listBuyItem.size() % TOTAL_SHOP_ITEM_SOCKET ) > 0 )
			++m_byTotalPageNo;

		if( m_byCurPageNo < m_byTotalPageNo )
			SetCurrentShopBoard( m_byCurPageNo );
		else
			SetCurrentShopBoard( 0 );
	}
	
	return TRUE;
}

BOOL
CGUIShopWindow::TakeOutBuyItem( DWORD pi_dwIndex )
{
	DWORD l_dwIndex;
	l_dwIndex = 0;
	for( CGUIItemList::iterator i = m_listBuyItem.begin(); i != m_listBuyItem.end(); ++i )
	{
		if( l_dwIndex == pi_dwIndex )
		{			
			CGUIItem * l_pItem;

			// remove item 
			l_pItem = *i;
			m_listBuyItem.remove( *i );

			delete l_pItem;

			return TRUE;
		}

		++l_dwIndex;
	}

	return FALSE;
}

void
CGUIShopWindow::ClearBuyItemList( void )
{
	for( CGUIItemList::iterator i = m_listBuyItem.begin(); i != m_listBuyItem.end(); ++i )
		delete *i;

	m_listBuyItem.clear();	
	if( m_eCurTradeMode == Buy_Mode )
	{
		ClearShopBoard();
		ClearTradeItemList();
	}
}

WORD
CGUIShopWindow::GetBuyItemIndex( CGUIItem * pi_pItem )
{
	WORD l_wIndex;
	l_wIndex = 0;
	for( CGUIItemList::iterator i = m_listBuyItem.begin(); i != m_listBuyItem.end(); ++i )
	{
		if( *i == pi_pItem )
		{			
			return l_wIndex;
		}

		++l_wIndex;
	}

	return m_listBuyItem.size();
}

CGUIItem *
CGUIShopWindow::GetBuyableItem( WORD pi_wIndex )
{
	WORD l_wIndex;
	l_wIndex = 0;
	for( CGUIItemList::iterator i = m_listBuyItem.begin(); i != m_listBuyItem.end(); ++i )
	{
		if( l_wIndex == pi_wIndex )
		{			
			return *i;
		}

		++l_wIndex;
	}

	return NULL;
}

CGUIItem *
CGUIShopWindow::GetBuyableItemByID( DWORD pi_dwItemID )
{
	for( CGUIItemList::iterator i = m_listBuyItem.begin(); i != m_listBuyItem.end(); ++i )
	{
		if( ( *i )->GetItemID() == pi_dwItemID )
		{			
			return *i;
		}
	}

	return NULL;
}

// ===========================================================================================

void
CGUIShopWindow::TakeInTradeItem( DWORD pi_dwIndex, DWORD pi_dwItemID, DWORD pi_dwSpriteID, BYTE pi_byQuantity )
{	
	// 새로운 아이템 삽입
	TRADE_ITEM_INFO	*	l_pTradeItemInfo;	

	l_pTradeItemInfo = new TRADE_ITEM_INFO;	

	l_pTradeItemInfo->pItem = new CGUIItem;
	l_pTradeItemInfo->pItem->SetItemID( pi_dwItemID );
	l_pTradeItemInfo->pItem->SetSpriteID( pi_dwSpriteID );
	l_pTradeItemInfo->pItem->SetSprite();
	l_pTradeItemInfo->pItem->SetNum( pi_byQuantity );
	l_pTradeItemInfo->pItem->DrawItemNum( TRUE );
	l_pTradeItemInfo->pItem->SetSize( 32, 32 );	
	
	l_pTradeItemInfo->dwListIndex = pi_dwIndex;


	// insert to tradelist
	m_listTradeItem.insert( m_listTradeItem.end(), l_pTradeItemInfo );

	if( m_eCurTradeMode == Sell_Mode ||
		m_eCurTradeMode == Repair_Mode )
	{		
		// set totalpage
		m_byTotalPageNo = m_listTradeItem.size() / TOTAL_SHOP_ITEM_SOCKET;
		if( m_listTradeItem.size() % TOTAL_SHOP_ITEM_SOCKET > 0 )
			++m_byTotalPageNo;
		
		if( m_byTotalPageNo > 0 )
			// Set page
			SetCurrentShopBoard( m_byTotalPageNo - 1 );
		else
			ClearShopBoard();
	}
}

void
CGUIShopWindow::TakeOutTradeItem( DWORD pi_dwIndex )
{
	TRADE_ITEM_INFO	*	l_pTradeItemInfo;	

	for( CGUITradeItemList::iterator it = m_listTradeItem.begin(); it != m_listTradeItem.end(); ++it )
	{
		if( (*it)->dwListIndex == pi_dwIndex )
		{
			l_pTradeItemInfo = *it;

			m_listTradeItem.erase( it );		
	
			delete l_pTradeItemInfo->pItem;
			delete l_pTradeItemInfo;	

			break;
		}
	}

	if( it != m_listTradeItem.end() )
	{
		if( m_eCurTradeMode == Sell_Mode ||
			m_eCurTradeMode == Repair_Mode )
		{
			// set totalpage
			m_byTotalPageNo = m_listTradeItem.size() / TOTAL_SHOP_ITEM_SOCKET;
			if( m_listTradeItem.size() % TOTAL_SHOP_ITEM_SOCKET > 0 )
				++m_byTotalPageNo;
			
			if( m_byTotalPageNo > 0 )
				// Set page
				SetCurrentShopBoard( m_byTotalPageNo - 1 );
			else
				ClearShopBoard();		
		}
	}
}

void	
CGUIShopWindow::ClearTradeItemList( void )
{
	for( CGUITradeItemList::iterator it = m_listTradeItem.begin(); it != m_listTradeItem.end(); ++it )
	{
		delete (*it)->pItem;
		delete (*it);
	}

	m_listTradeItem.clear();

	if( m_eCurTradeMode == Sell_Mode ||
		m_eCurTradeMode == Repair_Mode )
		ClearShopBoard();	
}

DWORD				
CGUIShopWindow::GetTradeItemIndex( BYTE pi_bySocketNo )
{
	BYTE l_byCount = 0;
	for( CGUITradeItemList::iterator i = m_listTradeItem.begin(); i != m_listTradeItem.end(); ++i )
	{
		if( l_byCount == pi_bySocketNo )
			return (*i)->dwListIndex;

		++l_byCount;
	}

	return -1L;
}

CGUIItem *			
CGUIShopWindow::GetTradeItemByIndex( DWORD pi_dwIndex )
{	
	for( CGUITradeItemList::iterator i = m_listTradeItem.begin(); i != m_listTradeItem.end(); ++i )
	{
		if( (*i)->dwListIndex == pi_dwIndex )
			return (*i)->pItem;	
	}

	return NULL;
}

// ===========================================================================================

void
CGUIShopWindow::UpdateShopBoard( void )
{	
	switch( m_eCurTradeMode )
	{
	case Buy_Mode:
		{
			m_byTotalPageNo = m_listBuyItem.size() / TOTAL_SHOP_ITEM_SOCKET;

			if( ( m_listBuyItem.size() % TOTAL_SHOP_ITEM_SOCKET ) > 0 )
				++m_byTotalPageNo;

			if( m_byCurPageNo < m_byTotalPageNo )
				SetCurrentShopBoard( m_byCurPageNo );
			else
				SetCurrentShopBoard( 0 );
		}
		break;

	case Sell_Mode:
	case Repair_Mode:
		{
			m_byTotalPageNo = 0;			
			SetCurrentShopBoard( 0 );
		}
		break;
	}

}

void
CGUIShopWindow::ClearShopBoard( void )
{
	// shop board에 소켓을 비운다.
	for( int i=0; i<TOTAL_SHOP_ITEM_SOCKET; ++i )
		m_uiShopItemBoard[i].SetEmpty();
}

void
CGUIShopWindow::SetCurrentShopBoard( BYTE pi_byPage )
{
	if( m_byTotalPageNo != 0 && pi_byPage >= m_byTotalPageNo )
		return;

	// set current page no
	m_byCurPageNo = pi_byPage;	

	char buf[8];
	if( m_byTotalPageNo > 0 ) 
		_itoa( m_byCurPageNo+1, buf, 10 );
	else
		_itoa( m_byCurPageNo, buf, 10 );
	m_uiCurPageStr.SetString( buf );

	// ------------------------------------------------------------
	// page button setting
	if( m_byCurPageNo == 0 )	
		m_uiPageUp.Disable();			
	else	
		m_uiPageUp.Enable();	

	if( m_byTotalPageNo == 0 || m_byCurPageNo + 1 == m_byTotalPageNo )
		m_uiPageDown.Disable();
	else	
		m_uiPageDown.Enable();		
	
	
	// ------------------------------------------------------------
	// shopboard item setting

	// 우선 socket을 비운다.
	ClearShopBoard();

	if( m_byTotalPageNo == 0 )
		return;


	if( m_eCurTradeMode == Buy_Mode )
	{
		BYTE	l_byPageFirstItemIndex;
		BYTE	l_byItemIndex;
		CGUIItem	* l_pItem;

		// 현재 페이지의 첫번째 item를 찾은 다음 socket에 item을 넣는다.
		l_byPageFirstItemIndex = m_byCurPageNo * TOTAL_SHOP_ITEM_SOCKET;
		l_byItemIndex = 0;		

		for( CGUIItemList::iterator it = m_listBuyItem.begin(); it != m_listBuyItem.end(); ++it )
		{
			if( l_byItemIndex == l_byPageFirstItemIndex )
			{				
				for( int j=0; j<TOTAL_SHOP_ITEM_SOCKET; )
				{
					if( it == m_listBuyItem.end() )
						break;				
					
					// set item
					l_pItem = *it;
					
					//m_uiShopItemBoard[j].SetData( l_pItem );
					m_uiShopItemBoard[j].SetItem( l_pItem );

					
					++j;
					++it;
				}

				break;
			}

			++l_byItemIndex;
		}
	}
	else
	{			
		BYTE	l_byPageFirstItemIndex;
		BYTE	l_byItemIndex;
		CGUIItem	* l_pItem;
		
		// 현재 페이지의 첫번째 item를 찾은 다음 socket에 item을 넣는다.
		l_byPageFirstItemIndex = m_byCurPageNo * TOTAL_SHOP_ITEM_SOCKET;
		l_byItemIndex = 0;
		
		for( CGUITradeItemList::iterator it = m_listTradeItem.begin(); it != m_listTradeItem.end(); ++it )
		{
			if( l_byItemIndex == l_byPageFirstItemIndex )
			{
				for( int j=0; j<TOTAL_SHOP_ITEM_SOCKET; )
				{
					if( it == m_listTradeItem.end() )
						break;					
					
					// set item
					l_pItem = (*it)->pItem;			
					
					//m_uiShopItemBoard[j].SetData( l_pItem );					
					m_uiShopItemBoard[j].SetItem( l_pItem );
					
					++j;
					++it;
				}

				break;
			}

			++l_byItemIndex;
		}
	}
	
}

// ===========================================================================================

void
CGUIShopWindow::SetDalant( DWORD pi_dwDalant )
{
	char buf[32];
	_GetPriceStr( pi_dwDalant, buf );
	
	m_uiTotalTradeDalantStr.SetString( buf );

	m_uiTotalTradeDalantStr.SetPos( m_ptPos.x + m_ptSize.x - 88 - m_uiTotalTradeDalantStr.m_ptSize.x,
									m_ptPos.y + 316 );
}

void
CGUIShopWindow::SetGold( DWORD pi_dwGold )
{
	char buf[32];
	_GetPriceStr( pi_dwGold, buf );
	
	m_uiTotalTradeGoldStr.SetString( buf );

	m_uiTotalTradeGoldStr.SetPos( m_ptPos.x + m_ptSize.x - 88 - m_uiTotalTradeGoldStr.m_ptSize.x,
								  m_ptPos.y + 345 );
}

// ===========================================================================================

void
CGUIShopWindow::SetEnableTrade( BOOL pi_bEnable )
{
	if( pi_bEnable )
	{
		// trade button enable	
		m_uiOKButton.Enable();
	}
	else
	{
		// trade button disable
		m_uiOKButton.Disable();
	}
}


void
CGUIShopWindow::MouseEvent( _mouse_event & event )
{
	CGUIWindow::MouseEvent( event );

	if( event.id == MOUSE_STROKED )
	{
		// event가 일어난 socket을 찾는다.		
		int	 l_nEventSocketIndex = -1;
		for( int i=0; i<TOTAL_SHOP_ITEM_SOCKET; ++i )
		{
			if( event.source == &m_uiShopItemBoard[i] )
			{
				l_nEventSocketIndex = i;				
				break;
			}			
		}
		

		
		// add
		if( gScreen.ExistObject() )
		{
			if( !( l_nEventSocketIndex != -1 || event.source == this ) )
				return;

			PostUIMessage( WM_BOARD_ADD_OBJECT, UI_MOUSE_MSG( 0, 0, 0, 0 ) );
		}
		// remove
		else
		{
			if( l_nEventSocketIndex == -1 || m_uiShopItemBoard[l_nEventSocketIndex].IsEmpty() )
				return;		

			
			BYTE bySocketNo = m_byCurPageNo * TOTAL_SHOP_ITEM_SOCKET + l_nEventSocketIndex;

			SetQuickEvent();

			if( g_bIsQuickEvent_Trade )
				PostUIMessage( WM_BOARD_TRADE_HOT_KEY, UI_MOUSE_MSG( 0, bySocketNo, 0, 0 ) );
			else
				PostUIMessage( WM_BOARD_CHECK_REMOVE_OBJECT, UI_MOUSE_MSG( 0, bySocketNo, 0, 0 ) );

			m_bySelectedSocketNo = bySocketNo;
		}

	}
	else if( event.id == MOUSE_CLICKED )
	{
		if( gScreen.ExistObject() )
			return;

		// page up, down
		if( event.source == &m_uiPageUp )
		{
			SetCurrentShopBoard( m_byCurPageNo - 1 );			
		}		
		else if( event.source == &m_uiPageDown )
		{			
			SetCurrentShopBoard( m_byCurPageNo + 1 );			
		}
		
		// trade button
		else if( event.source == &m_uiOKButton )
		{					
			PostUIMessage( WM_BOARD_TRADE, 0 );
		}
		// cancel trade
		else if( event.source == &m_uiCancelButton )
		{				
			PostUIMessage( WM_BOARD_CANCEL_TRADE, 0 );			
		}
		// 도움말 창 열기
		else if( event.source == &m_uiHelpButton )
		{	
			PostUIMessage( WM_OPEN_WINDOW_HELP, 0 );			
		}
		// close
		else if( event.source == &m_uiCloseButton )
		{
			PostUIMessage( WM_CLOSING_WINDOW, 0 );			
		}	
		// change trade mode
		else
		{		
			for( int i=0; i<3; ++i )
			{
				if( event.source == &m_uiTradeTab[i] )
				{
					PostUIMessage( WM_BOARD_CHANGE_TRADE_MODE, UI_MOUSE_MSG( i, 0, 0, 0 ) );	
					m_eSelectedTradeMode = (TradeMode)i;
					break;
				}
			}
		}
	}
	else if( event.id == MOUSE_HOVERED )
	{
		for( int i = 0; i < TOTAL_SHOP_ITEM_SOCKET; ++i )
		{
			if( event.source == &m_uiShopItemBoard[i] )
			{
				BYTE bySocketNo = m_byCurPageNo * TOTAL_SHOP_ITEM_SOCKET + i;

				PostUIMessage( WM_BOARD_HOVER_OBJECT, UI_MOUSE_MSG( 0, bySocketNo, 0, 0 ) );
				break;
			}			
		}		
	}	
}

void
CGUIShopWindow::ClosingDialogBox( CGUIDialogBox * pi_pDlg )
{
	if( pi_pDlg->GetUserDefinedID() == INPUT_DLG_ID )		
	{
		if( pi_pDlg->GetResultMsg() == MSG_OK )
		{
			CGUINumInputDlg * pInputDlg = static_cast< CGUINumInputDlg * >( pi_pDlg );
			WORD wQuantity = atoi( pInputDlg->GetInput() );
			

			if( g_bIsQuickEvent_Trade )
				PostUIMessage( WM_BOARD_TRADE_HOT_KEY,
							   UI_MOUSE_MSG( 0, m_bySelectedSocketNo, wQuantity, 0 ) );
			else
				PostUIMessage( WM_BOARD_REMOVE_OBJECT,
							   UI_MOUSE_MSG( 0, m_bySelectedSocketNo, wQuantity, 0 ) );
		}
	}
	else if( pi_pDlg->GetUserDefinedID() == MSGBOX_ID_CLOSE )
	{
		if( pi_pDlg->GetResultMsg() == MSG_OK )
		{
			// trade
			PostUIMessage( WM_BOARD_TRADE,
						   UI_MOUSE_MSG( 0, 0, 0, 0 ) );
		}
		else
		{
			// cancel trade
			PostUIMessage( WM_BOARD_CANCEL_TRADE,
						   UI_MOUSE_MSG( 0, 0, 0, 0 ) );

			Close();
		}
	}
	else if( pi_pDlg->GetUserDefinedID() == MSGBOX_ID_CHANGE_TRADE )
	{
		if( pi_pDlg->GetResultMsg() == MSG_OK )
		{
			// trade
			PostUIMessage( WM_BOARD_TRADE,
						   UI_MOUSE_MSG( 0, 0, 0, 0 ) );
		}
		else
		{
			// cancel trade
			PostUIMessage( WM_BOARD_CANCEL_TRADE,
						   UI_MOUSE_MSG( 0, 0, 0, 0 ) );

			// change trade mode
			PostUIMessage( WM_BOARD_CHANGE_TRADE_MODE, UI_MOUSE_MSG( m_eSelectedTradeMode, 0, 0, 0 ) );				
		}
	}

	CGUIWindow::ClosingDialogBox( pi_pDlg );
}

void
CGUIShopWindow::TabLabelPosSetting( TradeMode pi_eTradeMode, BOOL pi_bIsActive )
{
	if( pi_eTradeMode == Buy_Mode )
	{
		if( pi_bIsActive )
		{
			m_uiTradeTab[pi_eTradeMode].SetLabelPos( 29, 12 );
			m_uiTradeTab[pi_eTradeMode].SetLabelPressedPos( 29, 12 );
		}
		else
		{
			m_uiTradeTab[pi_eTradeMode].SetLabelPos( 29, 4 );
			m_uiTradeTab[pi_eTradeMode].SetLabelPressedPos( 29, 4 );
		}
	}
/*
	else if( pi_eTradeMode == Sell_Mode &&
			 m_eCurShopType == Unit )
	{
		if( pi_bIsActive )
		{
			m_uiTradeTab[pi_eTradeMode].SetLabelPos( 19, 13 );
			m_uiTradeTab[pi_eTradeMode].SetLabelPressedPos( 19, 13 );
		}
		else
		{
			m_uiTradeTab[pi_eTradeMode].SetLabelPos( 19, 5 );
			m_uiTradeTab[pi_eTradeMode].SetLabelPressedPos( 19, 5 );
		}	
	}
*/
	else
	{
		if( pi_bIsActive )
		{
			m_uiTradeTab[pi_eTradeMode].SetLabelPos( 12, 12 );
			m_uiTradeTab[pi_eTradeMode].SetLabelPressedPos( 12, 12 );
		}
		else
		{
			m_uiTradeTab[pi_eTradeMode].SetLabelPos( 12, 4 );
			m_uiTradeTab[pi_eTradeMode].SetLabelPressedPos( 12, 4 );
		}	
	}

}	
