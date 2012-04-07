#include "GUIItemMakeWindow.h"
#include "../../SpriteMgr.h"
#include "../../GUIController/GUIScreen.h"
#include "../../GUIController/GUIMessageBox.h"
#include "../../../DefineMessage.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define NONE_INDEX			0xffff
#define ID_CLOSE_WIN		0x0001

#define	SOCKET_V_NUM		4
#define	SOCKET_H_NUM		5

WORD g_wTabNum[3] = { WEAPON_CLASS_NUM, ARMOR_CLASS_NUM, CONSUME_CLASS_NUM };

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
// CGUIItemListWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////

CGUIItemListWindow::CGUIItemListWindow()
{
	m_byMakeItemType	= 0xFF;

	m_pTab				= NULL;
	m_byCurTabIndex		= 0xFF;
	m_byTotalTabNum		= 0;

	m_pItem				= NULL;
	m_byTotalItemNum	= 0;
	m_pEndItemIndex		= NULL;	

	m_bySelectedItemIndex = 0xFF;

	m_byCurPageIndex	= 0xFF;
	m_byTotalPageNo		= 0;

	m_bIsCreated		= FALSE;
}

CGUIItemListWindow::~CGUIItemListWindow()
{
	if( m_pTab )
	{
		delete[] m_pTab;
		m_pTab = NULL;
	}

	if( m_pItem )
	{
		for( int i = 0; i < m_byTotalTabNum; ++i )
		{
			delete[] m_pItem[i];
		}

		delete[] m_pItem;
		m_pItem = NULL;
	}
	
	if( m_pEndItemIndex )
	{
		delete[] m_pEndItemIndex;
		m_pEndItemIndex = NULL;
	}
}

void
CGUIItemListWindow::Create( BYTE pi_byMakeItemType, BYTE pi_byTotalPackNum, BYTE pi_byTotalSocketNum )
{
	if( m_bIsCreated )
		return;

	if( !pi_byTotalPackNum || !pi_byTotalSocketNum )
		return;
	
	m_byMakeItemType = pi_byMakeItemType;

	// create tab
	m_byTotalTabNum = pi_byTotalPackNum;
	m_pTab = new CGUIObject[m_byTotalTabNum];
	
	
	// create item
	m_pItem = new CGUIItem*[m_byTotalTabNum];
	m_pEndItemIndex = new BYTE[m_byTotalTabNum];

	m_byTotalItemNum = pi_byTotalSocketNum;
	for( int i = 0; i < m_byTotalTabNum; ++i )
	{
		m_pItem[i] = new CGUIItem[m_byTotalItemNum];
		m_pEndItemIndex[i] = 0;
	}

	
	Init();

	m_bIsCreated = TRUE;
}

void
CGUIItemListWindow::Init( void )
{
	SetSize( 228, 210 );

	m_uiTabBase.SetSize( 228, 28 );
		
	for( int i = 0; i < m_byTotalTabNum; ++i )
		m_pTab[i].SetSize( 20, 20 );	

	for( i = 0; i < TOTAL_ITEM_SOCKET_NUM; ++i )
		m_uiSocket[i].SetSize( 34, 34 );

	m_uiPageBoard.SetSize( 68, 17 );
	m_uiPageUp.SetSize( 45, 16 );
	m_uiPageDown.SetSize( 45, 16 );


	int l_nPosX = ( m_ptSize.x - m_pTab[0].m_ptSize.x * m_byTotalTabNum ) / 2;
	for( i = 0; i < m_byTotalTabNum; ++i )
		m_pTab[i].SetPos( m_ptPos.x + l_nPosX + m_pTab[i].m_ptSize.x * i, m_ptPos.y + 5 );
	
	
	for( i=0; i<SOCKET_V_NUM; ++i )
	{
		for( int j=0; j<SOCKET_H_NUM; ++j )
		{
			m_uiSocket[i * SOCKET_H_NUM + j].SetPos( 21 + 38 * j, 32 + 38 * i );			
		}
	}

	m_uiPageBoard.SetPos( 80, 196 );
	m_uiPageUp.SetPos( 39, 198 );
	m_uiPageDown.SetPos( 144, 198 );
	m_uiPageNoStr.SetPos( 98, 200 );

	Add( &m_uiTabBase );

	for( i = 0; i < m_byTotalTabNum; ++i )
	{		
		m_pTab[i].AddMouseEventHandler( this );
		Add( &m_pTab[i] );
	}

	for( i = 0; i < TOTAL_ITEM_SOCKET_NUM; ++i )
	{
		m_uiSocket[i].AddMouseEventHandler( this );
		Add( &m_uiSocket[i] );
	}

	Add( &m_uiPageBoard );
	Add( &m_uiPageNoStr );
	Add( &m_uiPageUp );
	Add( &m_uiPageDown );
	m_uiPageUp.AddMouseEventHandler( this );
	m_uiPageDown.AddMouseEventHandler( this );

	SetSprite();
}

void
CGUIItemListWindow::SetSprite( void )
{
	if( !m_bIsCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_ITEM_MAKE );

		l_spriteInfo.wActionNo = 1 + m_byMakeItemType;

		// tabbase
		l_spriteInfo.wFrameNo = m_byTotalTabNum;
		m_uiTabBase.SetSpriteInfo( &l_spriteInfo );

		// tab
		l_spriteInfo.pSprite = NULL;
		for( int i = 0; i < m_byTotalTabNum; ++i )
		{
			l_spriteInfo.wFrameNo = i;
			m_pTab[i].SetSpriteInfo( &l_spriteInfo );
		}

		// socket
		l_spriteInfo.pSprite = NULL;
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 3;
		for( i = 0; i < TOTAL_ITEM_SOCKET_NUM; ++i )
		{			
			m_uiSocket[i].SetSpriteInfo( &l_spriteInfo );
		}
		
		// page up, down button
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_SHOP );
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
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_ITEM_MAKE );	
		if( !l_pSprite )
			return;

		m_uiTabBase.GetSpriteInfo()->pSprite = l_pSprite;

		for( int i = 0; i < m_byTotalTabNum; ++i )
		{			
			if( m_pTab[i].GetSpriteInfo()->pSprite != NULL )
				m_pTab[i].GetSpriteInfo()->pSprite = l_pSprite;
		}

		l_pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
		for( i = 0; i < TOTAL_ITEM_SOCKET_NUM; ++i )
		{			
			if( m_uiSocket[i].GetSpriteInfo()->pSprite != NULL )
				m_uiSocket[i].GetSpriteInfo()->pSprite = l_pSprite;
		}	

		l_pSprite = gSpriteMgr.GetSprite( SP_ID_ITEM );
		for( i = 0; i < m_byTotalTabNum; ++i )
		{
			for( int j = 0; j < m_byTotalItemNum; ++j )
			{
				if( m_pItem[i][j].GetSpriteInfo()->pSprite != NULL )
					m_pItem[i][j].GetSpriteInfo()->pSprite = l_pSprite;
			}
		}
		
		// page up, down button
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_SHOP );	
		m_uiPageBoard.GetSpriteInfo()->pSprite = l_pSprite;		
		m_uiPageUp.SetSprite( l_pSprite );
		m_uiPageDown.SetSprite( l_pSprite );	
	}
}

BOOL
CGUIItemListWindow::TakeInItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex,
								DWORD pi_dwItemID, DWORD pi_dwSpriteID, BYTE pi_byNum, BYTE pi_byUsableLevel )
{
	if( pi_byPackIndex > m_byTotalTabNum ||
		pi_bySocketIndex > m_byTotalItemNum )
		return FALSE;

	m_pItem[pi_byPackIndex][pi_bySocketIndex].SetItemID( pi_dwItemID );
	m_pItem[pi_byPackIndex][pi_bySocketIndex].SetSpriteID( pi_dwSpriteID );
	m_pItem[pi_byPackIndex][pi_bySocketIndex].SetNum( pi_byNum );
	m_pItem[pi_byPackIndex][pi_bySocketIndex].SetSprite();


	// 페이지 설정을 위해 마지막 아이템 인텍스를 저장한다.
	if( pi_bySocketIndex > m_pEndItemIndex[pi_byPackIndex] )
		m_pEndItemIndex[pi_byPackIndex] = pi_bySocketIndex;
	
	
	if( pi_byPackIndex == m_byCurTabIndex )
	{
		// update page
		m_byTotalPageNo = ( m_pEndItemIndex[pi_byPackIndex] + 1 ) / TOTAL_ITEM_SOCKET_NUM;
		if( ( m_pEndItemIndex[m_byCurTabIndex] + 1 ) % TOTAL_ITEM_SOCKET_NUM )
			++m_byTotalPageNo;
		
		SetPage( 0 );
	}
	
	return TRUE;
}
	
BOOL
CGUIItemListWindow::TakeOutItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex, BYTE pi_byNum, CGUIItem * po_pOutItem )
{
	if( pi_byPackIndex > m_byTotalTabNum ||
		pi_bySocketIndex > m_byTotalItemNum )
		return FALSE;

	if( pi_byNum == 0 || pi_byNum >= m_pItem[pi_byPackIndex][pi_bySocketIndex].GetNum() )
		m_pItem[pi_byPackIndex][pi_bySocketIndex].SetEmpty();
	else
		m_pItem[pi_byPackIndex][pi_bySocketIndex].SetNum( m_pItem[pi_byPackIndex][pi_bySocketIndex].GetNum() - pi_byNum );


	// set end item index
	for( int i = m_pEndItemIndex[pi_byPackIndex]; i >= 0; --i )
	{
		if( !m_pItem[pi_byPackIndex][i].IsEmpty() )
		{
			m_pEndItemIndex[pi_byPackIndex] = i;
			break;
		}
	}

	return TRUE;
}
	
CGUIItem *
CGUIItemListWindow::GetItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex )
{
	if( pi_byPackIndex > m_byTotalTabNum ||
		pi_bySocketIndex > m_byTotalItemNum )	
		return NULL;

	if( m_pItem[pi_byPackIndex][pi_bySocketIndex].IsEmpty() )
		return NULL;
	else
		return &m_pItem[pi_byPackIndex][pi_bySocketIndex];
}

void
CGUIItemListWindow::SetTab( BYTE pi_byTabIndex )
{
	if( pi_byTabIndex >= m_byTotalTabNum )
		return;

	BYTE l_byPrevTabIndex = m_byCurTabIndex;

	m_byCurTabIndex = pi_byTabIndex;

	// set previous tab
	if( l_byPrevTabIndex < m_byTotalTabNum )
	{
		m_pTab[l_byPrevTabIndex].GetSpriteInfo()->pSprite = NULL;
		m_pTab[l_byPrevTabIndex].Enable();
	}

	// set currnet tab
	m_pTab[m_byCurTabIndex].GetSpriteInfo()->pSprite = gSpriteMgr.GetSprite( SP_ID_ITEM_MAKE );
	m_pTab[m_byCurTabIndex].Disable();

	// ---------------------------------------------------------------------------------------

	// set total page
	m_byTotalPageNo = ( m_pEndItemIndex[m_byCurTabIndex] + 1 ) / TOTAL_ITEM_SOCKET_NUM;
	if( ( m_pEndItemIndex[m_byCurTabIndex] + 1 ) % TOTAL_ITEM_SOCKET_NUM )
		++m_byTotalPageNo;

	PostUIMessage( WM_ITEM_WORK_BOARD_CANCEL, 0 );

	SetPage( 0 );
}

void	
CGUIItemListWindow::SetPage( BYTE pi_byPageIndex )
{
	if( m_byTotalPageNo == 0 || pi_byPageIndex >= m_byTotalPageNo )		
		return;	
	if( m_byCurTabIndex >= m_byTotalTabNum )
		return;
	
	// set current page 
	m_byCurPageIndex = pi_byPageIndex;

	char buf[8];	
	itoa( pi_byPageIndex+1, buf, 10 );
	m_uiPageNoStr.SetString( buf );

	// page button setting
	if( m_byCurPageIndex == 0 )	
		m_uiPageUp.Disable();			
	else	
		m_uiPageUp.Enable();	

	if( m_byTotalPageNo == 0 || m_byCurPageIndex + 1 == m_byTotalPageNo )
		m_uiPageDown.Disable();
	else	
		m_uiPageDown.Enable();	


	// -----------------------------------------------------------------------


	// set item
	int l_nItemIndex = m_byCurPageIndex * TOTAL_ITEM_SOCKET_NUM;
	for( int i=0; i<TOTAL_ITEM_SOCKET_NUM; ++i )
	{
		if( m_pItem[m_byCurTabIndex][l_nItemIndex + i].IsEmpty() )
			m_uiSocket[i].SetEmpty();
		else
			m_uiSocket[i].SetItem( &m_pItem[m_byCurTabIndex][l_nItemIndex + i] );		
	}		

	// update selectitem
	SelectItem( m_bySelectedItemIndex );	
}

void
CGUIItemListWindow::SelectItem( BYTE pi_byItemIndex )
{
	DeselectItem();

	if( pi_byItemIndex >= m_byTotalItemNum )
		return;

	m_bySelectedItemIndex = pi_byItemIndex;

	if( m_bySelectedItemIndex >= m_byCurPageIndex * TOTAL_ITEM_SOCKET_NUM  &&
		m_bySelectedItemIndex < (m_byCurPageIndex + 1) * TOTAL_ITEM_SOCKET_NUM )		
		m_uiSocket[m_bySelectedItemIndex%TOTAL_ITEM_SOCKET_NUM].GetSpriteInfo()->pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
}

void
CGUIItemListWindow::DeselectItem( void )
{
	m_bySelectedItemIndex = 0xFF;

	for( int i=0; i<TOTAL_ITEM_SOCKET_NUM; ++i )
		m_uiSocket[i].GetSpriteInfo()->pSprite = NULL;
}

void
CGUIItemListWindow::MouseEvent( _mouse_event & event )
{
	CGUIWindow::MouseEvent( event );

	if( event.id == MOUSE_STROKED )
	{
		// set tab
		for( int i=0; i<m_byTotalTabNum; ++i )
		{
			if( event.source == &m_pTab[i] )
			{				
				SetTab( i );				

				return;
			}			
		}

		// select item
		for( i = 0; i < TOTAL_ITEM_SOCKET_NUM; ++i )
		{
			if( event.source == &m_uiSocket[i] && !m_uiSocket[i].IsEmpty() )				
			{
				BYTE bySocketIndex = m_byCurPageIndex * TOTAL_ITEM_SOCKET_NUM + i;

				PostUIMessage( WM_BOARD_CLICK_OBJECT, 
							   UI_MOUSE_MSG( m_byCurTabIndex, bySocketIndex, 0, 0 ) );		
				return;
			}
		}
		
	}
	else if( event.id == MOUSE_HOVERED )
	{
		if( gScreen.ExistObject() )
			return;
		
		// set tooltip
		for( int i = 0; i < TOTAL_ITEM_SOCKET_NUM; ++i )
		{
			if( event.source == &m_uiSocket[i] && !m_uiSocket[i].IsEmpty() )
			{					
				BYTE bySocketIndex = m_byCurPageIndex * TOTAL_ITEM_SOCKET_NUM + i;

				PostUIMessage( WM_BOARD_HOVER_OBJECT, 
							   UI_MOUSE_MSG( m_byCurTabIndex, bySocketIndex, 0, 0 ) );				

				return;
			}
		}	
	}
	else if( event.id == MOUSE_CLICKED )
	{
		if( event.source == &m_uiPageUp )
		{
			if( m_byCurPageIndex > 0 )			
				SetPage( m_byCurPageIndex - 1 );			
		}
		else if( event.source == &m_uiPageDown )
		{			
			if( m_byCurPageIndex < m_byTotalPageNo )			
				SetPage( m_byCurPageIndex + 1 );			
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
// CGUIItemMakeWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////

CGUIItemMakeWindow::CGUIItemMakeWindow()
{
	m_eCurMakeGroup	= None;	

	m_bIsCreated	= FALSE;
}

CGUIItemMakeWindow::~CGUIItemMakeWindow()
{
	
}

void
CGUIItemMakeWindow::Init( void )
{	
	SetSize( 228, 395 );
	
	m_uiAniTitleBar.SetSize( 228, 28 );		

	m_uiMakeButton.SetSize( 77, 25 );
	m_uiCloseButton.SetSize( 16, 13 );	
	m_uiHelpButton.SetSize( 16, 13 );		

	m_uiMakeButton.SetLabel( "제  작" );
	m_uiMaterialTitle[0].SetString( "주재료" );
	m_uiMaterialTitle[1].SetString( "부재료" );

	// --------------------------------------------------------
	m_uiTitle.SetPos( 6, 7 );	

	m_uiMakeButton.SetPos( 75, 358 );
	m_uiCloseButton.SetPos( 211, 4 );
	m_uiHelpButton.SetPos( 194, 4 );
	
	m_uiMaterialTitle[0].SetPos( 23, 265 );
	m_uiMaterialTitle[1].SetPos( 23, 289 );

	// --------------------------------------------------------
	Add( &m_uiAniTitleBar );
	Add( &m_uiTitle );	
	Add( &m_uiCloseButton );
	Add( &m_uiHelpButton );	
	Add( &m_uiMaterialTitle[0] );
	Add( &m_uiMaterialTitle[1] );	

	for( int i = 0; i < 5; ++i )
		Add( &m_uiMaterialStr[i] );

	Add( &m_uiMakeButton );	

	m_uiMakeButton.AddMouseEventHandler( this );	
	m_uiCloseButton.AddMouseEventHandler( this );
	m_uiHelpButton.AddMouseEventHandler( this );	
		

	// --------------------------------------------------------
	SetSprite();

	SetARGB( 0xCCFFFFFF );

	m_uiTitle.GetFont()->dwColor = D3DCOLOR_XRGB( 195, 193, 190 );

	m_uiMaterialTitle[0].GetFont()->dwColor = D3DCOLOR_XRGB( 195, 193, 190 );
	m_uiMaterialTitle[1].GetFont()->dwColor = D3DCOLOR_XRGB( 195, 193, 190 );

	m_uiMakeButton.SetLabelColor( D3DCOLOR_XRGB( 232, 232, 232 ) );

	m_uiMakeButton.SetLabelStyle( R3_HAN_OUTLINE );
	
	m_uiAniTitleBar.SetTotalFrameNum( 18 );
	m_uiAniTitleBar.SetObjectNumInLine( 1 );
	m_uiAniTitleBar.SetFPS( 10 );
	m_uiAniTitleBar.SetStopInterval( 1000 );

	SetMoveObject( this );
	SetMoveObject( &m_uiAniTitleBar );
	SetMoveObject( &m_uiTitle );
	SetMovable( TRUE );
	
	EnableMake( FALSE );


	SoundOn( TRUE );

	m_bIsCreated = TRUE;
}

void
CGUIItemMakeWindow::SetSprite( void )
{
	if( !m_bIsCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_ITEM_MAKE );

		l_spriteInfo.wActionNo = 0;

		l_spriteInfo.wFrameNo = 0;
		SetSpriteInfo( &l_spriteInfo );
		
		
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 5;
		m_uiAniTitleBar.SetSpriteInfo( &l_spriteInfo );	

		// make button
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );
		l_spriteInfo.wFrameNo = 12;
		m_uiMakeButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 13;
		m_uiMakeButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 14;
		m_uiMakeButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 15;
		m_uiMakeButton.SetSpriteInfoEx( DISABLED, &l_spriteInfo );		

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

		m_uiMakeItemList[0].SetSprite();
		m_uiMakeItemList[1].SetSprite();
		m_uiMakeItemList[2].SetSprite();
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_ITEM_MAKE );	
		if( !l_pSprite )
			return;

		GetSpriteInfo()->pSprite = l_pSprite;				

		// base anibar
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
		m_uiAniTitleBar.GetSpriteInfo()->pSprite = l_pSprite;
		
		// make button
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );
		m_uiMakeButton.SetSprite( l_pSprite );
		// close button			
		m_uiCloseButton.SetSprite( l_pSprite );
		// help button
		m_uiHelpButton.SetSprite( l_pSprite );		

		m_uiMakeItemList[0].SetSprite();
		m_uiMakeItemList[1].SetSprite();
		m_uiMakeItemList[2].SetSprite();
	}
}

void
CGUIItemMakeWindow::SetMakeGroup( MakeGroup pi_eMakeGroup )
{
	MakeGroup l_ePrevMakeGroup;

	l_ePrevMakeGroup = m_eCurMakeGroup;

	m_eCurMakeGroup = pi_eMakeGroup;

	if( l_ePrevMakeGroup != None )
	{
		Remove( &m_uiMakeItemList[l_ePrevMakeGroup] );
	}	

	if( !m_uiMakeItemList[m_eCurMakeGroup].IsCreated() )
	{
		m_uiMakeItemList[m_eCurMakeGroup].Create( m_eCurMakeGroup, g_wTabNum[m_eCurMakeGroup], 100 );
		m_uiMakeItemList[m_eCurMakeGroup].SetID( GetID() );
	}
	
	m_uiMakeItemList[m_eCurMakeGroup].SetPos( m_ptPos.x, m_ptPos.y + m_uiAniTitleBar.m_ptSize.y );
	m_uiMakeItemList[m_eCurMakeGroup].Show( IsShow() );
	Add( &m_uiMakeItemList[m_eCurMakeGroup] );


	if( m_eCurMakeGroup == Weapon )
		m_uiTitle.SetString( "무기 제작  " );
	else if( m_eCurMakeGroup == Armor )
		m_uiTitle.SetString( "방어구 제작" );
	else
		m_uiTitle.SetString( "발사물 제작" );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CGUIItemMakeWindow::SetActiveTab( BYTE pi_byTabIndex )
{
	if( m_eCurMakeGroup == None )
		return;

	m_uiMakeItemList[m_eCurMakeGroup].SetTab( pi_byTabIndex );	
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CGUIItemMakeWindow::SetPage( BYTE pi_byPageIndex )
{
	if( m_eCurMakeGroup == None )
		return;

	m_uiMakeItemList[m_eCurMakeGroup].SetPage( pi_byPageIndex );	
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CGUIItemMakeWindow::SelectItem( BYTE pi_byItemIndex )
{
	if( m_eCurMakeGroup == None )
		return;

	m_uiMakeItemList[m_eCurMakeGroup].SelectItem( pi_byItemIndex );
}

void			
CGUIItemMakeWindow::DeselectItem( void )
{
	if( m_eCurMakeGroup == None )
		return;

	m_uiMakeItemList[m_eCurMakeGroup].DeselectItem();
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CGUIItemMakeWindow::EnableMake( BOOL pi_bEnable )
{
	if( pi_bEnable )
		m_uiMakeButton.Enable();
	else
		m_uiMakeButton.Disable();
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CGUIItemMakeWindow::SetMaterialStr( BYTE pi_byIndex, char * pi_pStr, BYTE pi_byNum, BOOL pi_bIsHaving )
{	
	if( !pi_pStr )	
	{
		m_uiMaterialStr[pi_byIndex].SetString( NULL );
		return;
	}

	char buf[32];
	sprintf( buf, "%s %3d개", pi_pStr, pi_byNum );
	m_uiMaterialStr[pi_byIndex].SetString( buf );

	if( pi_byIndex == 0 )
		m_uiMaterialStr[pi_byIndex].SetPos( m_ptPos.x + 204 - m_uiMaterialStr[pi_byIndex].m_ptSize.x, m_ptPos.y + 268 );

	else
		m_uiMaterialStr[pi_byIndex].SetPos( m_ptPos.x + 204 - m_uiMaterialStr[pi_byIndex].m_ptSize.x,
											  m_ptPos.y + 297 + (m_uiMaterialStr[pi_byIndex].m_ptSize.y + 2) * ( pi_byIndex - 1 ) );

	if( pi_bIsHaving )
		m_uiMaterialStr[pi_byIndex].GetFont()->dwColor = D3DCOLOR_XRGB( 255, 255, 255 );
	else
		m_uiMaterialStr[pi_byIndex].GetFont()->dwColor = D3DCOLOR_XRGB( 255, 139, 130 );
}

void
CGUIItemMakeWindow::ClearMaterial( void )
{	
	for( int i = 0; i < 5; ++i )
		m_uiMaterialStr[i].SetString( NULL );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CGUIItemMakeWindow::MouseEvent( _mouse_event & event )
{
	CGUIWindow::MouseEvent( event );

	if( event.id == MOUSE_CLICKED )
	{
		if( gScreen.ExistObject() )
			return;
		
		if( event.source == &m_uiMakeButton )
		{			
			// item work
			PostUIMessage( WM_ITEM_WORK_BOARD_PROCESS, 0 );
		}				
		else if( event.source == &m_uiHelpButton )
		{		
			PostUIMessage( WM_OPEN_WINDOW_HELP, 0 );			
		}		
		else if( event.source == &m_uiCloseButton )
		{
			Close();
		}			
	}
}

void
CGUIItemMakeWindow::ClosingDialogBox( CGUIDialogBox * pi_pDlg )
{
	if( pi_pDlg->GetUserDefinedID() == ID_CLOSE_WIN )
	{
		if( pi_pDlg->GetResultMsg() == MSG_OK )
		{
			PostUIMessage( WM_ITEM_WORK_BOARD_PROCESS, 0 );
		}
		else if( pi_pDlg->GetResultMsg() == MSG_CANCEL )
		{
			PostUIMessage( WM_ITEM_WORK_BOARD_CANCEL, 0 );

			Close();
		}
	}

	CGUIWindow::ClosingDialogBox( pi_pDlg );	
}