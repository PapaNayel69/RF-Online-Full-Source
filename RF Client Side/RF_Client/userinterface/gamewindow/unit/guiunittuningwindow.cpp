#include "GUIUnitTuningWindow.h"
#include "../../SpriteMgr.h"
#include "../../GUIController/GUIScreen.h"

#define	TITLE_COLOR		0xFFC3C1BE
#define	SUB_TITLE_COLOR	0xFFEEEAE4
#define	HEAD_COLOR		0xFFDCD0B0
#define	TEXT_COLOR		0xFFB1B1B1

#define EMPTY_SOCKET_ARGB	0x70FFFFFF


////////////////////////////////////////////////////////////////////////////
//
// CGUIUnitTuningWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////
CGUIUnitTuningWindow::CGUIUnitTuningWindow()
{
	m_byCurTabIndex				= 0xFF;

	m_byTempBuyTabIndex[0] = 0xFF;
	m_byTempBuyTabIndex[1] = 0xFF;

	m_byCurBuyTabIndex		= 0xFF;	

	for( int i = 0; i < TOTAL_PARTS_TYPE_NUM; ++i )
		m_byBuyPartsPageStartIndex[i] = 0;	

	for( i = 0; i < BULLET_TYPE_NUM; ++i )
		m_byBuyBulletPageStartIndex[i] = 0;

	m_bIsCreated				= FALSE;
}

CGUIUnitTuningWindow::~CGUIUnitTuningWindow()
{
	for( int i = 0; i < TOTAL_PARTS_TYPE_NUM; ++i )
	{
		ClearBuyTuningPartsList( i );
	}

	for( i = 0; i < BULLET_TYPE_NUM; ++i )
	{
		ClearBuyBulletList( i );
	}
}

void
CGUIUnitTuningWindow::Init( void )
{
	SetSize( 228, 571 );

	m_uiUpperBase.SetSize( 228, 300 );
	m_uiLowerBase.SetSize( 228, 271 );

	m_uiAniTitleBar.SetSize( 228, 28 );
	m_uiCloseButton.SetSize( 16, 13 );
	m_uiHelpButton.SetSize( 16, 13 );

	m_uiMainFrameKey.SetSize( 64, 64 );
	m_uiRepairButton.SetSize( 57, 28 );		

	m_uiTab[PARTS_TAB].SetSize( 104, 32 );
	m_uiTab[BULLET_TAB].SetSize( 104, 32 );

	m_uiActiveTabBoard.SetSize( 75, 29 );

	for( int i = 0; i < BUY_ITEM_TAB_NUM; ++i )
		m_uiBuyItemTab[i].SetSize( 18, 18 );

	for( i = 0; i < ITEM_BOARD_NUM; ++i )
		m_uiBuyItemBoard[i].SetSize( 32, 32 );

	m_uiPageLeftButton.SetSize( 12, 20 );
	m_uiPageRightButton.SetSize( 12, 20 );	

	m_uiTuningButton.SetSize( 77, 25 );
	m_uiCancelButton.SetSize( 77, 25 );	

	m_uiTitle.SetString( "기갑장비 튜닝" );
	
	m_uiMainFrameInfoTitle[0].SetString( "메인프레임 : " );
	m_uiMainFrameInfoTitle[1].SetString( "방  어  력 : " );
	m_uiMainFrameInfoTitle[2].SetString( "내구도 : " );
	m_uiMainFrameInfoTitle[3].SetString( "이동력 : " );

	m_uiWeaponTitle.SetString( "기본 무장" );
	m_uiWeaponInfoTitle[0].SetString( "무기명 : " );	
	m_uiWeaponInfoTitle[1].SetString( "공격력 : " );
	m_uiWeaponInfoTitle[2].SetString( "잔  탄 : " );

	m_uiAddWeaponTitle.SetString( "추가 무장" );	
	for( i = 0; i < 3; ++i )
		m_uiAddWeaponInfoTitle[i].SetString( m_uiWeaponInfoTitle[i].GetString() );	

	m_uiTab[PARTS_TAB].SetLabel( "기본파츠" );
	m_uiTab[BULLET_TAB].SetLabel( "탄 약" );	

	char buf[32];
	sprintf( buf, "튜닝비용           %s", _GetAvatarMoneyName() );
	m_uiTuningCostTitle.SetString( buf );

	m_uiRepairButton.SetLabel( "수 리" );
	m_uiTuningButton.SetLabel( "승인요청" );
	m_uiCancelButton.SetLabel( "취 소" );	

	// --------------------------------------------------------------------

	m_uiUpperBase.SetPos( m_ptPos );
	m_uiTitle.SetPos( m_ptPos.x + 6, m_ptPos.y + 7 );
	m_uiCloseButton.SetPos( m_ptPos.x + 211, m_ptPos.y + 4 );	
	m_uiHelpButton.SetPos( m_ptPos.x + 194, m_ptPos.y + 4 );

	m_uiMainFrameKey.SetPos( m_ptPos.x + 11, m_ptPos.y + 66 );
	m_uiRepairButton.SetPos( m_ptPos.x + 15, m_ptPos.y + 139 );

	m_uiMainFrameInfoTitle[0].SetPos( m_ptPos.x + 9, m_ptPos.y + 33 );
	m_uiMainFrameInfoTitle[1].SetPos( m_ptPos.x + 9, m_ptPos.y + 47 );
	m_uiMainFrameInfoTitle[2].SetPos( m_ptPos.x + 130, m_ptPos.y + 33 );
	m_uiMainFrameInfoTitle[3].SetPos( m_ptPos.x + 130, m_ptPos.y + 47 );
	for( i = 0; i < 4; ++i )
		m_uiMainFrameInfo[i].SetPos( m_uiMainFrameInfoTitle[i].m_ptPos.x + m_uiMainFrameInfoTitle[i].m_ptSize.x - 2,
									 m_uiMainFrameInfoTitle[i].m_ptPos.y );

	m_uiWeaponTitle.SetPos( m_ptPos.x + 124, m_ptPos.y + 65 );
	for( i = 0; i < 3; ++i )
	{
		m_uiWeaponInfoTitle[i].SetPos( m_ptPos.x + 83, m_ptPos.y + 79 + i * 16 );
		m_uiWeaponInfo[i].SetPos( m_uiWeaponInfoTitle[i].m_ptPos.x + m_uiWeaponInfoTitle[i].m_ptSize.x - 3,
								  m_uiWeaponInfoTitle[i].m_ptPos.y );
	}

	m_uiAddWeaponTitle.SetPos( m_ptPos.x + 124, m_ptPos.y + 126 );
	for( i = 0; i < 3; ++i )
	{
		m_uiAddWeaponInfoTitle[i].SetPos( m_ptPos.x + 83, m_ptPos.y + 140 + i * 16 );
		m_uiAddWeaponInfo[i].SetPos( m_uiAddWeaponInfoTitle[i].m_ptPos.x + m_uiAddWeaponInfoTitle[i].m_ptSize.x - 3,
									 m_uiAddWeaponInfoTitle[i].m_ptPos.y );
	}	

	m_uiTab[PARTS_TAB].SetPos( m_ptPos.x + 7, m_ptPos.y + 188 );
	m_uiTab[BULLET_TAB].SetPos( m_ptPos.x + 117, m_ptPos.y + 188 );
	m_uiTab[PARTS_TAB].SetLabelPos( 28, 15 );
	m_uiTab[BULLET_TAB].SetLabelPos( 37, 15 );

	for( i = 0; i < ITEM_BOARD_NUM; ++i )
		m_uiBuyItemBoard[i].SetPos( m_ptPos.x + 22 + i * 37, m_ptPos.y + 262 );	

	m_uiPageLeftButton.SetPos( m_ptPos.x + 8, m_ptPos.y + 268 );
	m_uiPageRightButton.SetPos( m_ptPos.x + 207, m_ptPos.y + 268 );

	m_uiLowerBase.SetPos( m_ptPos.x, m_uiUpperBase.m_ptPos.y + m_uiUpperBase.m_ptSize.y );

	m_uiTuningCostTitle.SetPos( m_ptPos.x + ( m_ptSize.x - m_uiTuningCostTitle.m_ptSize.x ) / 2, m_ptPos.y + 522 );
	m_uiTuningCost.SetPos( m_ptPos.x + 109, m_ptPos.y + 522 );

	m_uiTuningButton.SetPos( m_ptPos.x + 18, m_ptPos.y + 543 );
	m_uiCancelButton.SetPos( m_ptPos.x + 137, m_ptPos.y + 543 );
	
	// --------------------------------------------------------------------

	Add( &m_uiUpperBase );

	Add( &m_uiAniTitleBar );
	Add( &m_uiTitle );	
	m_uiCloseButton.AddMouseEventHandler( this );
	m_uiHelpButton.AddMouseEventHandler( this );
	Add( &m_uiCloseButton );
	Add( &m_uiHelpButton );
	
	m_uiMainFrameKey.AddMouseEventHandler( this );
	m_uiRepairButton.AddMouseEventHandler( this );
	Add( &m_uiMainFrameKey );
	Add( &m_uiRepairButton );

	Add( &m_uiWeaponTitle );
	Add( &m_uiAddWeaponTitle );

	for( i = 0; i < 4; ++i )
	{
		Add( &m_uiMainFrameInfoTitle[i] );
		Add( &m_uiMainFrameInfo[i] );
	}

	for( i = 0; i < 3; ++i )
	{
		Add( &m_uiWeaponInfoTitle[i] );
		Add( &m_uiWeaponInfo[i] );

		Add( &m_uiAddWeaponInfoTitle[i] );
		Add( &m_uiAddWeaponInfo[i] );
	}		

	m_uiTab[PARTS_TAB].AddMouseEventHandler( this );
	m_uiTab[BULLET_TAB].AddMouseEventHandler( this );
	Add( &m_uiTab[PARTS_TAB] );
	Add( &m_uiTab[BULLET_TAB] );

	for( i = 0; i < BUY_ITEM_TAB_NUM; ++i )
	{
		m_uiBuyItemTab[i].AddMouseEventHandler( this );
	}

	for( i = 0; i < ITEM_BOARD_NUM; ++i )
	{
		m_uiBuyItemBoard[i].AddMouseEventHandler( this );
		Add( &m_uiBuyItemBoard[i] );
	}

	m_uiPageLeftButton.AddMouseEventHandler( this );
	m_uiPageRightButton.AddMouseEventHandler( this );
	Add( &m_uiPageLeftButton );
	Add( &m_uiPageRightButton );	

	Add( &m_uiLowerBase );

	m_uiTuningButton.AddMouseEventHandler( this );
	m_uiCancelButton.AddMouseEventHandler( this );
	Add( &m_uiTuningButton );
	Add( &m_uiCancelButton );

	Add( &m_uiTuningCostTitle );
	Add( &m_uiTuningCost );
	
	InitPartsBoard();
	InitBulletBoard();

	// --------------------------------------------------------------------
	
	SetSprite();

	m_uiUpperBase.SetARGB( 0xCCFFFFFF );
	m_uiPartsBoard.SetARGB( 0xCCFFFFFF );
	m_uiBulletBoard.SetARGB( 0xCCFFFFFF );

	m_uiTitle.GetFont()->dwColor = TITLE_COLOR;

	m_uiWeaponTitle.GetFont()->dwColor = SUB_TITLE_COLOR;
	m_uiAddWeaponTitle.GetFont()->dwColor = SUB_TITLE_COLOR;

	for( i = 0; i < 4; ++i )
	{
		m_uiMainFrameInfoTitle[i].GetFont()->dwColor = HEAD_COLOR;
		m_uiMainFrameInfo[i].GetFont()->dwColor = TEXT_COLOR;
	}

	for( i = 0; i < 3; ++i )
	{
		m_uiWeaponInfoTitle[i].GetFont()->dwColor = HEAD_COLOR;
		m_uiWeaponInfo[i].GetFont()->dwColor = TEXT_COLOR;

		m_uiAddWeaponInfoTitle[i].GetFont()->dwColor = HEAD_COLOR;
		m_uiAddWeaponInfo[i].GetFont()->dwColor = TEXT_COLOR;
	}			

	m_uiActiveTabName.GetFont()->dwColor = TEXT_COLOR;

	m_uiBackPackBulletTitle.GetFont()->dwColor = TEXT_COLOR;

	m_uiTuningCostTitle.GetFont()->dwColor = HEAD_COLOR;
	m_uiTuningCost.GetFont()->dwColor = 0xFFFFFFFF;

	m_uiTab[PARTS_TAB].SetLabelStyle( R3_HAN_OUTLINE );
	m_uiTab[BULLET_TAB].SetLabelStyle( R3_HAN_OUTLINE );

	m_uiRepairButton.SetLabelStyle( R3_HAN_OUTLINE );
	m_uiTuningButton.SetLabelStyle( R3_HAN_OUTLINE );
	m_uiCancelButton.SetLabelStyle( R3_HAN_OUTLINE );
	
	// --------------------------------------------------------------------

	m_uiAniTitleBar.SetTotalFrameNum( 18 );
	m_uiAniTitleBar.SetObjectNumInLine( 1 );
	m_uiAniTitleBar.SetFPS( 10 );
	m_uiAniTitleBar.SetStopInterval( 1000 );

	SetMoveObject( &m_uiTitle );
	SetMoveObject( &m_uiAniTitleBar );	
	SetMovable( TRUE );	

	// --------------------------------------------------------------------
	m_uiMainFrameKey.DrawItemNum( FALSE );

	SetTab( 0 );	

	for( i = 0; i < TOTAL_PARTS_TYPE_NUM; ++i )
		UnEquip_Parts( i );

	for( i = 0; i < BULLET_TYPE_NUM; ++i )
		UnEquip_Bullet( i );

	for( i = 0; i < BACK_PACK_BULLET_NUM; ++i )
		UnEquip_BackPack_Bullet( i );

	m_uiRepairButton.Disable();
	m_uiTuningButton.Disable();
	m_uiCancelButton.Disable();

	Select_Bullet( 0 );

	Enable_BackPack_Bullet( 0 );

	Enable_Shouler_Bullet( FALSE );

	m_bIsCreated = TRUE;
}

void
CGUIUnitTuningWindow::InitPartsBoard()
{
	m_uiPartsBoard.SetSize( 228, 271 );
	for( int i = 0; i < TOTAL_PARTS_TYPE_NUM; ++i )
	{
		m_uiTuningParts[i].SetSize( 64, 64 );
		m_uiBuyingPartsFlag[i].SetSize( 66, 66 );
		m_uiEmptyPartsFlag[i].SetSize( 64, 64 );
	}

	m_uiSelectedParts.SetSize( 66, 66 );

	m_uiTuningParts[Parts_Head].SetPos( m_uiPartsBoard.m_ptPos.x + 82, m_uiPartsBoard.m_ptPos.y + 4 );
	m_uiTuningParts[Parts_Upper].SetPos( m_uiPartsBoard.m_ptPos.x + 82, m_uiPartsBoard.m_ptPos.y + 74 );
	m_uiTuningParts[Parts_Lower].SetPos( m_uiPartsBoard.m_ptPos.x + 82, m_uiPartsBoard.m_ptPos.y + 144 );
	m_uiTuningParts[Parts_Arms].SetPos( m_uiPartsBoard.m_ptPos.x + 12, m_uiPartsBoard.m_ptPos.y + 74 );
	m_uiTuningParts[Parts_Shoulder].SetPos( m_uiPartsBoard.m_ptPos.x + 152, m_uiPartsBoard.m_ptPos.y + 74 );	
	m_uiTuningParts[Parts_Back].SetPos( m_uiPartsBoard.m_ptPos.x + 152, m_uiPartsBoard.m_ptPos.y + 144 );	


	for( i = 0; i < TOTAL_PARTS_TYPE_NUM; ++i )
	{
		m_uiBuyingPartsFlag[i].SetPos( m_uiTuningParts[i].m_ptPos.x - 1, m_uiTuningParts[i].m_ptPos.y - 1 );
		m_uiEmptyPartsFlag[i].SetPos( m_uiTuningParts[i].m_ptPos );
	}

	m_uiPartsBoard.Add( &m_uiActiveTabBoard );
	m_uiPartsBoard.Add( &m_uiActiveTabName );

	m_uiPartsBoard.Add( &m_uiSelectedParts );

	// add flag
	for( i = 0; i < TOTAL_PARTS_TYPE_NUM; ++i )
	{
		m_uiPartsBoard.Add( &m_uiBuyingPartsFlag[i] );
		m_uiPartsBoard.Add( &m_uiEmptyPartsFlag[i] );
	}

	// add parts
	for( i = 0; i < TOTAL_PARTS_TYPE_NUM; ++i )
	{
		m_uiTuningParts[i].AddMouseEventHandler( this );
		m_uiPartsBoard.Add( &m_uiTuningParts[i] );

		m_uiTuningParts[i].DrawItemNum( FALSE );
	}
}

void
CGUIUnitTuningWindow::InitBulletBoard()
{
	m_uiBulletBoard.SetSize( 228, 271 );
	for( int i = 0; i < BULLET_TYPE_NUM; ++i )
	{
		m_uiTuningBullet[i].SetSize( 64, 64 );
		m_uiBuyingBulletFlag[i].SetSize( 66, 66 );
		m_uiEmptyBulletFlag[i].SetSize( 64, 64 );
	}

	for( i = 0; i < BACK_PACK_BULLET_NUM; ++i )
	{
		m_uiTuningBackPackBullet[i].SetSize( 32, 32 );
		m_uiBuyingBackPackBulletFlag[i].SetSize( 34, 34 );
		m_uiEmptyBackPackBulletFlag[i].SetSize( 32, 32 );
	}

	m_uiBackPackBulletTitle.SetString( "백팩" );

	m_uiTuningBullet[Bullet_Arms].SetPos( m_uiBulletBoard.m_ptPos.x + 23, m_uiBulletBoard.m_ptPos.y + 12 );
	m_uiTuningBullet[Bullet_Shoulder].SetPos( m_uiBulletBoard.m_ptPos.x + 139, m_uiBulletBoard.m_ptPos.y + 12 );

	for( i = 0; i < BULLET_TYPE_NUM; ++i )
	{
		m_uiBuyingBulletFlag[i].SetPos( m_uiTuningBullet[i].m_ptPos.x - 1, m_uiTuningBullet[i].m_ptPos.y - 1 );
		m_uiEmptyBulletFlag[i].SetPos( m_uiTuningBullet[i].m_ptPos );
	}

	for( i = 0; i < BACK_PACK_BULLET_NUM; ++i )
	{
		m_uiTuningBackPackBullet[i].SetPos( m_uiBulletBoard.m_ptPos.x + 35 + 42 * (i%4), m_uiBulletBoard.m_ptPos.y + 119 + 45 * (i/4) );
		m_uiBuyingBackPackBulletFlag[i].SetPos( m_uiTuningBackPackBullet[i].m_ptPos.x - 1, m_uiTuningBackPackBullet[i].m_ptPos.y - 1 );
		m_uiEmptyBackPackBulletFlag[i].SetPos( m_uiTuningBackPackBullet[i].m_ptPos );
	}

	m_uiBackPackBulletTitle.SetPos( m_uiBulletBoard.m_ptPos.x + ( m_uiBulletBoard.m_ptSize.x - m_uiBackPackBulletTitle.m_ptSize.x )/2,
									m_uiBulletBoard.m_ptPos.y + 100 );

//	m_uiBulletBoard.Add( &m_uiActiveTabBoard );
//	m_uiBulletBoard.Add( &m_uiActiveTabName );

	
	m_uiBulletBoard.Add( &m_uiSelectedBullet );

	// add bullet flag
	for( i = 0; i < BULLET_TYPE_NUM; ++i )
	{
		m_uiBulletBoard.Add( &m_uiBuyingBulletFlag[i] );
		m_uiBulletBoard.Add( &m_uiEmptyBulletFlag[i] );
	}
	
	// add backpack bullet flag
	for( i = 0; i < BACK_PACK_BULLET_NUM; ++i )
	{
		m_uiBulletBoard.Add( &m_uiEmptyBackPackBulletFlag[i] );
		m_uiBulletBoard.Add( &m_uiBuyingBackPackBulletFlag[i] );	
	}

	// add bullet
	for( i = 0; i < BULLET_TYPE_NUM; ++i )
	{
		m_uiTuningBullet[i].AddMouseEventHandler( this );
		m_uiBulletBoard.Add( &m_uiTuningBullet[i] );

		m_uiTuningBullet[i].DrawItemNum( FALSE );
	}

	// add backpack bullet
	for( i = 0; i < BACK_PACK_BULLET_NUM; ++i )
	{
		m_uiTuningBackPackBullet[i].AddMouseEventHandler( this );
		m_uiBulletBoard.Add( &m_uiTuningBackPackBullet[i] );

		m_uiTuningBackPackBullet[i].DrawItemNum( FALSE );
	}
	
	m_uiBulletBoard.Add( &m_uiBackPackBulletTitle );
}

void
CGUIUnitTuningWindow::SetSprite( void )
{
	if( !m_bIsCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_UNIT );		

		l_spriteInfo.wActionNo = 0;

		l_spriteInfo.wFrameNo = 0;
		m_uiUpperBase.SetSpriteInfo( &l_spriteInfo );		

		l_spriteInfo.wFrameNo = 1;
		m_uiPartsBoard.SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 2;
		m_uiBulletBoard.SetSpriteInfo( &l_spriteInfo );

		// parts tab, bullet tab
		l_spriteInfo.wFrameNo = 3;
		m_uiTab[PARTS_TAB].SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 4;
		m_uiTab[PARTS_TAB].SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 5;
		m_uiTab[PARTS_TAB].SetSpriteInfoEx( DISABLED, &l_spriteInfo );

		l_spriteInfo.wFrameNo = 6;
		m_uiTab[BULLET_TAB].SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 7;
		m_uiTab[BULLET_TAB].SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 8;
		m_uiTab[BULLET_TAB].SetSpriteInfoEx( DISABLED, &l_spriteInfo );

		// page up, down
		l_spriteInfo.wFrameNo = 9;
		m_uiPageLeftButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 10;
		m_uiPageLeftButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 11;
		m_uiPageLeftButton.SetSpriteInfoEx( DISABLED, &l_spriteInfo );

		l_spriteInfo.wFrameNo = 12;
		m_uiPageRightButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 13;
		m_uiPageRightButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 14;
		m_uiPageRightButton.SetSpriteInfoEx( DISABLED, &l_spriteInfo );
		
//		l_spriteInfo.wFrameNo = 15;
//		m_uiActiveTabBoard.SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wActionNo = 1;
		for( int i = 0; i < BUY_ITEM_TAB_NUM; ++i )			
		{
			l_spriteInfo.wFrameNo = i;
			m_uiBuyItemTab[i].SetSpriteInfo( &l_spriteInfo );
		}

		l_spriteInfo.wActionNo = 2;
		for( i = 0; i < TOTAL_PARTS_TYPE_NUM; ++i )
		{
			l_spriteInfo.wFrameNo = i;
			m_uiEmptyPartsFlag[i].SetSpriteInfo( &l_spriteInfo );
		}
		
		for( i = 0; i < BULLET_TYPE_NUM; ++i )
		{
			l_spriteInfo.wFrameNo = TOTAL_PARTS_TYPE_NUM + i;
			m_uiEmptyBulletFlag[i].SetSpriteInfo( &l_spriteInfo );
		}

		l_spriteInfo.wFrameNo = 7;
		for( i = 0; i < BACK_PACK_BULLET_NUM; ++i )
			m_uiEmptyBackPackBulletFlag[i].SetSpriteInfo( &l_spriteInfo );

		// title
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );		
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 5;
		m_uiAniTitleBar.SetSpriteInfo( &l_spriteInfo );

		
		l_spriteInfo.pSprite = NULL;
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 4;
		for( i = 0; i < TOTAL_PARTS_TYPE_NUM; ++i )
		{		
			m_uiBuyingPartsFlag[i].SetSpriteInfo( &l_spriteInfo );
		}

		for( i = 0; i < BULLET_TYPE_NUM; ++i )
		{		
			m_uiBuyingBulletFlag[i].SetSpriteInfo( &l_spriteInfo );
		}

		l_spriteInfo.wFrameNo = 3;
		for( i = 0; i < BACK_PACK_BULLET_NUM; ++i )
		{		
			m_uiBuyingBackPackBulletFlag[i].SetSpriteInfo( &l_spriteInfo );			
		}		


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

		// repair button
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 21;
		m_uiRepairButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 22;
		m_uiRepairButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 23;
		m_uiRepairButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 24;
		m_uiRepairButton.SetSpriteInfoEx( DISABLED, &l_spriteInfo );

		// tunining button
		l_spriteInfo.wFrameNo = 12;
		m_uiTuningButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 13;
		m_uiTuningButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 14;
		m_uiTuningButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 15;
		m_uiTuningButton.SetSpriteInfoEx( DISABLED, &l_spriteInfo );	
		
		// cancel button
		l_spriteInfo.wFrameNo = 12;
		m_uiCancelButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 13;
		m_uiCancelButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 14;
		m_uiCancelButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 15;
		m_uiCancelButton.SetSpriteInfoEx( DISABLED, &l_spriteInfo );
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_UNIT );		
		if( !l_pSprite )
			return;

		m_uiUpperBase.GetSpriteInfo()->pSprite = l_pSprite;		
		m_uiPartsBoard.GetSpriteInfo()->pSprite = l_pSprite;
		m_uiBulletBoard.GetSpriteInfo()->pSprite = l_pSprite;

		m_uiTab[PARTS_TAB].SetSprite( l_pSprite );
		m_uiTab[BULLET_TAB].SetSprite( l_pSprite );

		m_uiPageLeftButton.SetSprite( l_pSprite );
		m_uiPageRightButton.SetSprite( l_pSprite );

		for( int i = 0; i < BUY_ITEM_TAB_NUM; ++i )
			m_uiBuyItemTab[i].GetSpriteInfo()->pSprite = l_pSprite;

		
		for( i = 0; i < TOTAL_PARTS_TYPE_NUM; ++i )
		{		
			if( m_uiEmptyPartsFlag[i].GetSpriteInfo()->pSprite != NULL )
				m_uiEmptyPartsFlag[i].GetSpriteInfo()->pSprite = l_pSprite;
		}
		
		for( i = 0; i < BULLET_TYPE_NUM; ++i )
		{
			if( m_uiEmptyBulletFlag[i].GetSpriteInfo()->pSprite != NULL )
				m_uiEmptyBulletFlag[i].GetSpriteInfo()->pSprite = l_pSprite;
		}		

		for( i = 0; i < BACK_PACK_BULLET_NUM; ++i )
		{
			if( m_uiEmptyBackPackBulletFlag[i].GetSpriteInfo()->pSprite != NULL )
				m_uiEmptyBackPackBulletFlag[i].GetSpriteInfo()->pSprite = l_pSprite;
		}

		l_pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );

		m_uiAniTitleBar.GetSpriteInfo()->pSprite = l_pSprite;
		for( i = 0; i < TOTAL_PARTS_TYPE_NUM; ++i )
		{
			if( m_uiBuyingPartsFlag[i].GetSpriteInfo()->pSprite != NULL )
				m_uiBuyingPartsFlag[i].GetSpriteInfo()->pSprite = l_pSprite;
		}

		for( i = 0; i < BULLET_TYPE_NUM; ++i )
		{		
			if( m_uiBuyingBulletFlag[i].GetSpriteInfo()->pSprite != NULL )
				m_uiBuyingBulletFlag[i].GetSpriteInfo()->pSprite = l_pSprite;
		}

		for( i = 0; i < BACK_PACK_BULLET_NUM; ++i )
		{		
			if( m_uiBuyingBackPackBulletFlag[i].GetSpriteInfo()->pSprite != NULL )
				m_uiBuyingBackPackBulletFlag[i].GetSpriteInfo()->pSprite = l_pSprite;
		}

		if( m_uiSelectedParts.GetSpriteInfo()->pSprite != NULL )
			m_uiSelectedParts.GetSpriteInfo()->pSprite = l_pSprite;

		if( m_uiSelectedBullet.GetSpriteInfo()->pSprite != NULL )
			m_uiSelectedBullet.GetSpriteInfo()->pSprite = l_pSprite;

		l_pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );
		m_uiCloseButton.SetSprite( l_pSprite );
		m_uiHelpButton.SetSprite( l_pSprite );
		m_uiRepairButton.SetSprite( l_pSprite );
		m_uiTuningButton.SetSprite( l_pSprite );
		m_uiCancelButton.SetSprite( l_pSprite );

		l_pSprite = gSpriteMgr.GetSprite( SP_ID_ITEM );
		if( m_uiMainFrameKey.GetItemID() != NONE_ITEM_ID )
			m_uiMainFrameKey.GetSpriteInfo()->pSprite = l_pSprite;

		// unit parts sprite 
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_UNIT_PARTS );
		for( i = 0; i < TOTAL_PARTS_TYPE_NUM; ++i )
		{
			if( m_uiTuningParts[i].GetItemID() != NONE_ITEM_ID )
				m_uiTuningParts[i].GetSpriteInfo()->pSprite = l_pSprite;			
		}

		for( i = 0; i < BULLET_TYPE_NUM; ++i )
		{		
			if( m_uiTuningBullet[i].GetItemID() != NONE_ITEM_ID )
				m_uiTuningBullet[i].GetSpriteInfo()->pSprite = l_pSprite;
		}

		for( i = 0; i < BACK_PACK_BULLET_NUM; ++i )
		{		
			if( m_uiTuningBackPackBullet[i].GetItemID() != NONE_ITEM_ID )
				m_uiTuningBackPackBullet[i].GetSpriteInfo()->pSprite = l_pSprite;
		}

		// buy parts list
		CPartsList::iterator it;
		for( i = 0; i < TOTAL_PARTS_TYPE_NUM; ++i )
		{
			for( it = m_listBuyParts[i].begin(); it != m_listBuyParts[i].end(); ++it )
			{	
				if( (*it)->uiPartsIcon.GetSpriteInfo()->pSprite != NULL )
					(*it)->uiPartsIcon.GetSpriteInfo()->pSprite = l_pSprite;
			}
		}

		for( i = 0; i < BULLET_TYPE_NUM; ++i )
		{
			for( it = m_listBuyBullet[i].begin(); it != m_listBuyBullet[i].end(); ++it )
			{	
				if( (*it)->uiPartsIcon.GetSpriteInfo()->pSprite != NULL )
					(*it)->uiPartsIcon.GetSpriteInfo()->pSprite = l_pSprite;
			}
		}
	}
}

void
CGUIUnitTuningWindow::TakeInMainFrameKey( DWORD pi_dwItemID, DWORD pi_dwSpriteID )
{	
	m_uiMainFrameKey.SetItemID( pi_dwItemID );	
	m_uiMainFrameKey.SetSpriteID( pi_dwSpriteID );
	m_uiMainFrameKey.SetNum( 1 );
	m_uiMainFrameKey.SetSprite();
}

void
CGUIUnitTuningWindow::TakeOutMainFrameKey( void )
{
	m_uiMainFrameKey.SetEmpty();	
}

void
CGUIUnitTuningWindow::SetMainFrameName( char * pi_pMainFrameName )
{
	m_uiMainFrameInfo[0].SetString( pi_pMainFrameName );	
}

void
CGUIUnitTuningWindow::SetDefPower( int pi_nDefPower )
{
	char buf[16];
	sprintf( buf, "%d", pi_nDefPower );	

	m_uiMainFrameInfo[1].SetString( buf );
}
	
void
CGUIUnitTuningWindow::SetDurability( int pi_nDurability )
{
	char buf[16];
	sprintf( buf, "%d%%", pi_nDurability );	

	m_uiMainFrameInfo[2].SetString( buf );	

	if( pi_nDurability < 100 )
	{
		m_uiMainFrameInfo[2].GetFont()->dwColor = 0xFFFF0000;
	}
	else
	{
		m_uiMainFrameInfo[2].GetFont()->dwColor = TEXT_COLOR;
	}
}
/*
void
CGUIUnitTuningWindow::SetMovePower( int pi_nMovePower )
{
	char buf[16];
	sprintf( buf, "%d", pi_nMovePower );	

	m_uiMainFrameInfo[3].SetString( buf );	
}*/

void
CGUIUnitTuningWindow::SetMovePower( char * pi_pSpeedStirng )
{
	m_uiMainFrameInfo[3].SetString( pi_pSpeedStirng );	
}

void	
CGUIUnitTuningWindow::SetWeaponName( char * pi_pWeaponName )
{
	m_uiWeaponInfo[0].SetString( pi_pWeaponName );
}

void
CGUIUnitTuningWindow::SetWeaponAttPower( int pi_nMinPower, int pi_nMaxPower )
{
	char buf[16];
	sprintf( buf, "%d ~ %d", pi_nMinPower, pi_nMaxPower );	
	
	m_uiWeaponInfo[1].SetString( buf );
}

void
CGUIUnitTuningWindow::SetBulletNum( int pi_nRestBulletNum, int pi_nTotalBulletNum )
{
	char buf[16];
	sprintf( buf, "%d / %d", pi_nRestBulletNum, pi_nTotalBulletNum );	
	
	m_uiWeaponInfo[2].SetString( buf );

	if( pi_nRestBulletNum > 0 )
		m_uiWeaponInfo[2].GetFont()->dwColor = TEXT_COLOR;
	else
		m_uiWeaponInfo[2].GetFont()->dwColor = 0xFFFF0000;
}

void	
CGUIUnitTuningWindow::SetAddWeaponName( char * pi_pWeaponName )
{
	m_uiAddWeaponInfo[0].SetString( pi_pWeaponName );
}

void
CGUIUnitTuningWindow::SetAddWeaponAttPower( int pi_nMinPower, int pi_nMaxPower )
{
	char buf[16];
	sprintf( buf, "%d ~ %d", pi_nMinPower, pi_nMaxPower );	
	
	m_uiAddWeaponInfo[1].SetString( buf );
}

void
CGUIUnitTuningWindow::SetAddBulletNum( int pi_nRestBulletNum, int pi_nTotalBulletNum )
{
	char buf[16];
	sprintf( buf, "%d / %d", pi_nRestBulletNum, pi_nTotalBulletNum );	
	
	m_uiAddWeaponInfo[2].SetString( buf );
}

// ============================================================================================

void
CGUIUnitTuningWindow::Select_Parts( BYTE pi_bySocketNo )
{
	if( pi_bySocketNo >= TOTAL_PARTS_TYPE_NUM )
		return;

	SPRITE_INFO stSpriteInfo;
	stSpriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
	stSpriteInfo.wActionNo = 1;
	stSpriteInfo.wFrameNo = 3;
	m_uiSelectedParts.SetSpriteInfo( &stSpriteInfo );
	m_uiSelectedParts.SetARGB( 0x90FFFFFF );

	m_uiSelectedParts.SetPos( m_uiTuningParts[pi_bySocketNo].m_ptPos.x - 1, m_uiTuningParts[pi_bySocketNo].m_ptPos.y - 1 );
}

void
CGUIUnitTuningWindow::Select_Bullet( BYTE pi_bySocketNo )
{
	if( pi_bySocketNo >= BULLET_TYPE_NUM + BACK_PACK_BULLET_NUM )
		return;

	SPRITE_INFO stSpriteInfo;
	stSpriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
	stSpriteInfo.wActionNo = 1;

	if( pi_bySocketNo < BULLET_TYPE_NUM )
	{
		m_uiSelectedBullet.SetSize( 66, 66 );

		stSpriteInfo.wFrameNo = 3;
		m_uiSelectedBullet.SetSpriteInfo( &stSpriteInfo );
		m_uiSelectedBullet.SetARGB( 0x90FFFFFF );
		
		m_uiSelectedBullet.SetPos( m_uiTuningBullet[pi_bySocketNo].m_ptPos.x - 1, m_uiTuningBullet[pi_bySocketNo].m_ptPos.y - 1 );
	}
	else
	{
		m_uiSelectedBullet.SetSize( 34, 34 );

		stSpriteInfo.wFrameNo = 2;
		m_uiSelectedBullet.SetSpriteInfo( &stSpriteInfo );
		m_uiSelectedBullet.SetARGB( 0x90FFFFFF );
		
		m_uiSelectedBullet.SetPos( m_uiTuningBackPackBullet[pi_bySocketNo - BULLET_TYPE_NUM].m_ptPos.x - 1,
								   m_uiTuningBackPackBullet[pi_bySocketNo - BULLET_TYPE_NUM].m_ptPos.y - 1 );
	}
}

// ============================================================================================

void
CGUIUnitTuningWindow::Equip_Parts( BYTE pi_bySocketNo, DWORD pi_dwSpriteID, BOOL pi_bIsForBuying )
{
	if( pi_bySocketNo >= TOTAL_PARTS_TYPE_NUM )
		return;

	m_uiTuningParts[pi_bySocketNo].SetItemID( pi_bySocketNo );
	m_uiTuningParts[pi_bySocketNo].SetSpriteID( pi_dwSpriteID );
	m_uiTuningParts[pi_bySocketNo].SetIconType( UNIT_ICON );
	m_uiTuningParts[pi_bySocketNo].SetNum( 1 );
	m_uiTuningParts[pi_bySocketNo].SetSprite();

	SetPartsBuyingState( pi_bySocketNo, pi_bIsForBuying );

	m_uiEmptyPartsFlag[pi_bySocketNo].GetSpriteInfo()->pSprite = NULL;
	m_uiEmptyPartsFlag[pi_bySocketNo].SetARGB( -1L );
}

void
CGUIUnitTuningWindow::UnEquip_Parts( BYTE pi_bySocketNo )
{
	if( pi_bySocketNo >= TOTAL_PARTS_TYPE_NUM )
		return;

	m_uiTuningParts[pi_bySocketNo].SetEmpty();

	SetPartsBuyingState( pi_bySocketNo, FALSE );

	m_uiEmptyPartsFlag[pi_bySocketNo].GetSpriteInfo()->pSprite = m_uiPartsBoard.GetSpriteInfo()->pSprite;
	m_uiEmptyPartsFlag[pi_bySocketNo].SetARGB( EMPTY_SOCKET_ARGB );
}

void
CGUIUnitTuningWindow::SetPartsBuyingState( BYTE pi_bySocketNo, BOOL pi_bState )
{
	if( pi_bySocketNo >= TOTAL_PARTS_TYPE_NUM )
		return;

	if( pi_bState )
		m_uiBuyingPartsFlag[pi_bySocketNo].GetSpriteInfo()->pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
	else
		m_uiBuyingPartsFlag[pi_bySocketNo].GetSpriteInfo()->pSprite = NULL;
}

// ============================================================================================

void
CGUIUnitTuningWindow::Equip_Bullet( BYTE pi_byBulletType, DWORD pi_dwSpriteID, BOOL pi_bIsForBuying )
{
	if( pi_byBulletType >= BULLET_TYPE_NUM )
		return;

	m_uiTuningBullet[pi_byBulletType].SetItemID( BULLET_ICON_TYPE );
	m_uiTuningBullet[pi_byBulletType].SetSpriteID( pi_dwSpriteID );
	m_uiTuningBullet[pi_byBulletType].SetIconType( UNIT_ICON );
	m_uiTuningBullet[pi_byBulletType].SetNum( 1 );
	m_uiTuningBullet[pi_byBulletType].SetSprite();
	
	SetBulletBuyingState( pi_byBulletType, pi_bIsForBuying );

	m_uiEmptyBulletFlag[pi_byBulletType].GetSpriteInfo()->pSprite = NULL;
	m_uiEmptyBulletFlag[pi_byBulletType].SetARGB( -1L );
}

void
CGUIUnitTuningWindow::UnEquip_Bullet( BYTE pi_byBulletType )
{
	if( pi_byBulletType >= BULLET_TYPE_NUM )
		return;

	m_uiTuningBullet[pi_byBulletType].SetEmpty();

	SetBulletBuyingState( pi_byBulletType, FALSE );

	m_uiEmptyBulletFlag[pi_byBulletType].GetSpriteInfo()->pSprite = m_uiBulletBoard.GetSpriteInfo()->pSprite;
	m_uiEmptyBulletFlag[pi_byBulletType].SetARGB( EMPTY_SOCKET_ARGB );
}

void
CGUIUnitTuningWindow::Enable_Shouler_Bullet( BOOL pi_bIsEnabled )
{
	SPRITE_INFO l_spriteInfo;

	if( pi_bIsEnabled )
	{
		l_spriteInfo.pSprite = NULL;
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 4;

		m_uiBuyingBulletFlag[Bullet_Shoulder].SetSpriteInfo( &l_spriteInfo );
		m_uiBuyingBulletFlag[Bullet_Shoulder].SetARGB( -1L );

		m_uiTuningBullet[Bullet_Shoulder].Enable();
	}
	else
	{
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
		l_spriteInfo.wActionNo = 1;
		l_spriteInfo.wFrameNo = 5;

		m_uiBuyingBulletFlag[Bullet_Shoulder].SetSpriteInfo( &l_spriteInfo );
		m_uiBuyingBulletFlag[Bullet_Shoulder].SetARGB( 0xA0FFFFFF );

		m_uiTuningBullet[Bullet_Shoulder].Disable();
	}
}

void
CGUIUnitTuningWindow::SetBulletBuyingState( BYTE pi_bySocketNo, BOOL pi_bState )
{
	if( pi_bySocketNo >= BULLET_TYPE_NUM )
		return;

	if( pi_bySocketNo == Bullet_Shoulder &&
		IS_DISABLED( m_uiTuningBullet[Bullet_Shoulder].GetState() ) )
		return;

	if( pi_bState )
	{
		m_uiBuyingBulletFlag[pi_bySocketNo].GetSpriteInfo()->pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
		m_uiBuyingBulletFlag[pi_bySocketNo].SetARGB( 0xA0FFFFFF );
	}
	else
	{
		m_uiBuyingBulletFlag[pi_bySocketNo].GetSpriteInfo()->pSprite = NULL;
		m_uiBuyingBulletFlag[pi_bySocketNo].SetARGB( -1L );
	}
}

// ============================================================================================

void
CGUIUnitTuningWindow::Equip_BackPack_Bullet( BYTE pi_bySocketNo, DWORD pi_dwSpriteID, BOOL pi_bIsForBuying )
{
	if( pi_bySocketNo >= BACK_PACK_BULLET_NUM )
		return;

	m_uiTuningBackPackBullet[pi_bySocketNo].SetItemID( BULLET_ICON_TYPE );
	m_uiTuningBackPackBullet[pi_bySocketNo].SetSpriteID( pi_dwSpriteID );
	m_uiTuningBackPackBullet[pi_bySocketNo].SetIconType( UNIT_ICON );
	m_uiTuningBackPackBullet[pi_bySocketNo].SetNum( 1 );
	m_uiTuningBackPackBullet[pi_bySocketNo].SetSprite();

	m_uiEmptyBackPackBulletFlag[pi_bySocketNo].GetSpriteInfo()->pSprite = NULL;
	m_uiEmptyBackPackBulletFlag[pi_bySocketNo].SetARGB( -1L );

	SetBackpackBulletBuyingState( pi_bySocketNo, pi_bIsForBuying );
}

void
CGUIUnitTuningWindow::UnEquip_BackPack_Bullet( BYTE pi_bySocketNo )
{
	if( pi_bySocketNo >= BACK_PACK_BULLET_NUM )
		return;	

	m_uiTuningBackPackBullet[pi_bySocketNo].SetEmpty();

	m_uiEmptyBackPackBulletFlag[pi_bySocketNo].GetSpriteInfo()->pSprite = m_uiBulletBoard.GetSpriteInfo()->pSprite;
	m_uiEmptyBackPackBulletFlag[pi_bySocketNo].SetARGB( 0xA0FFFFFF );

	SetBackpackBulletBuyingState( pi_bySocketNo, FALSE );
}

void
CGUIUnitTuningWindow::Enable_BackPack_Bullet( BYTE pi_byEnableNum )
{
	SPRITE_INFO l_spriteInfo;

	// enable
	l_spriteInfo.pSprite = NULL;
	l_spriteInfo.wActionNo = 0;
	l_spriteInfo.wFrameNo = 3;
	for( int i=0; i<pi_byEnableNum; ++i )
	{
		m_uiBuyingBackPackBulletFlag[i].SetSpriteInfo( &l_spriteInfo );		
		m_uiBuyingBackPackBulletFlag[i].SetARGB( -1L );

		m_uiTuningBackPackBullet[i].Enable();
	}

	// disable	
	l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
	l_spriteInfo.wActionNo = 1;
	l_spriteInfo.wFrameNo = 4;
	for( ; i<BACK_PACK_BULLET_NUM; ++i )
	{
		m_uiBuyingBackPackBulletFlag[i].SetSpriteInfo( &l_spriteInfo );		
		m_uiBuyingBackPackBulletFlag[i].SetARGB( 0xA0FFFFFF );

		m_uiTuningBackPackBullet[i].Disable();
	}
}

void
CGUIUnitTuningWindow::SetBackpackBulletBuyingState( BYTE pi_bySocketNo, BOOL pi_bState )
{
	if( pi_bySocketNo >= BACK_PACK_BULLET_NUM )
		return;

	if( pi_bState )
	{
		m_uiBuyingBackPackBulletFlag[pi_bySocketNo].GetSpriteInfo()->pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
	}
	else
	{
		m_uiBuyingBackPackBulletFlag[pi_bySocketNo].GetSpriteInfo()->pSprite = NULL;
		m_uiBuyingBackPackBulletFlag[pi_bySocketNo].SetARGB( -1L );
	}
}

// ============================================================================================

void	
CGUIUnitTuningWindow::SetTuningCost( DWORD pi_dwCost )
{
	char buf[32];
	_GetPriceStr( pi_dwCost, buf );	
	
	m_uiTuningCost.SetString( buf );

	m_uiTuningCost.SetPos( m_uiTuningCostTitle.m_ptPos.x + m_uiTuningCostTitle.m_ptSize.x - m_uiTuningCost.m_ptSize.x - 40,
						   m_uiTuningCost.m_ptPos.y );
}

void
CGUIUnitTuningWindow::SetTab( BYTE pi_byTabIndex )
{
	if( m_byCurTabIndex == pi_byTabIndex )
		return;

	if( pi_byTabIndex >= TOTAL_TAB_NUM )
		return;

	m_byCurTabIndex = pi_byTabIndex;	
	
	m_uiTab[m_byCurTabIndex].Disable();
	m_uiTab[m_byCurTabIndex].SetLabelColor( 0xFFEEEAE3 );
	m_uiTab[(m_byCurTabIndex+1)%2].Enable();
	m_uiTab[(m_byCurTabIndex+1)%2].SetLabelColor( 0xFFA7A49F );
	
	if( m_byCurTabIndex == PARTS_TAB )
	{
		// set item tab
		for( int i = 0; i < TOTAL_PARTS_TYPE_NUM; ++i )
		{			
			m_uiBuyItemTab[i].GetSpriteInfo()->wFrameNo = i;

			m_uiBuyItemTab[i].Show( IsShow() );
			Add( &m_uiBuyItemTab[i] );
		}
		for( ; i < BUY_ITEM_TAB_NUM; ++i )
			Remove( &m_uiBuyItemTab[i] );
		
		// set board
		m_uiLowerBase.Remove( &m_uiBulletBoard );

		m_uiPartsBoard.SetPos( m_uiLowerBase.m_ptPos );
		m_uiPartsBoard.Show( IsShow() );
		m_uiLowerBase.Add( &m_uiPartsBoard );
	}
	else
	{
		int i = 0;
		/*
		// set item tab
		for( int i = 0; i < BULLET_TYPE_NUM; ++i )
		{			
			m_uiBuyItemTab[i].GetSpriteInfo()->wFrameNo = TOTAL_PARTS_TYPE_NUM + i;

			m_uiBuyItemTab[i].Show( IsShow() );
			Add( &m_uiBuyItemTab[i] );
		}
		*/
		for( ; i < BUY_ITEM_TAB_NUM; ++i )
			Remove( &m_uiBuyItemTab[i] );
		
		// set board
		m_uiLowerBase.Remove( &m_uiPartsBoard );

		m_uiBulletBoard.SetPos( m_uiLowerBase.m_ptPos );
		m_uiBulletBoard.Show( IsShow() );
		m_uiLowerBase.Add( &m_uiBulletBoard );
	}

	// update parts(or bullet) tab
	SetBuyItemTab( m_byTempBuyTabIndex[m_byCurTabIndex], TRUE );
}

void
CGUIUnitTuningWindow::InsertBuyTuningParts( BYTE pi_byPartsTabIndex, DWORD pi_dwPartsID, DWORD pi_dwSpriteID )
{
	if( pi_byPartsTabIndex >= TOTAL_PARTS_TYPE_NUM + BULLET_TYPE_NUM )
		return;

	PARTS_INFO * l_pPartsInfo;
	l_pPartsInfo = new PARTS_INFO;
	l_pPartsInfo->dwPartsID		= pi_dwPartsID;
	l_pPartsInfo->uiPartsIcon.SetIconType( UNIT_ICON );
	l_pPartsInfo->uiPartsIcon.SetItemID( pi_byPartsTabIndex );
	// bullet이면
	if( pi_byPartsTabIndex > TOTAL_PARTS_TYPE_NUM )
		l_pPartsInfo->uiPartsIcon.SetItemID( BULLET_ICON_TYPE );

	l_pPartsInfo->uiPartsIcon.SetSpriteID( pi_dwSpriteID );
	l_pPartsInfo->uiPartsIcon.SetSprite();

	l_pPartsInfo->uiPartsIcon.DrawItemNum( FALSE );

	// parts
	if( pi_byPartsTabIndex < TOTAL_PARTS_TYPE_NUM )
	{
		m_listBuyParts[pi_byPartsTabIndex].push_back( l_pPartsInfo );

		// update page
		if( ( m_byCurTabIndex == PARTS_TAB ) && ( pi_byPartsTabIndex == m_byCurBuyTabIndex ) )
			SetBuyPage( m_byBuyPartsPageStartIndex[pi_byPartsTabIndex] );
	}
	// bullet
	else 
	{
		BYTE l_byTabIndex = pi_byPartsTabIndex - TOTAL_PARTS_TYPE_NUM;

		m_listBuyBullet[l_byTabIndex].push_back( l_pPartsInfo );

		// update page
		if( ( m_byCurTabIndex == BULLET_TAB ) && ( l_byTabIndex == m_byCurBuyTabIndex ) )
			SetBuyPage( m_byBuyBulletPageStartIndex[l_byTabIndex] );
	}
}

void
CGUIUnitTuningWindow::RemoveBuyTuningParts( BYTE pi_byPartsTabIndex, DWORD pi_dwPartsID )
{
	if( pi_byPartsTabIndex >= TOTAL_PARTS_TYPE_NUM + BULLET_TYPE_NUM )
		return;

	// parts
	if( pi_byPartsTabIndex < TOTAL_PARTS_TYPE_NUM )
	{
		CPartsList::iterator it;
		for( it = m_listBuyParts[pi_byPartsTabIndex].begin(); it != m_listBuyParts[pi_byPartsTabIndex].end(); ++it )
		{
			if( ( *it )->dwPartsID == pi_dwPartsID )
			{
				delete ( *it );
				( *it ) = NULL;
				m_listBuyParts[pi_byPartsTabIndex].erase( it );

				// update page
				if( ( m_byCurTabIndex == PARTS_TAB ) && ( pi_byPartsTabIndex == m_byCurBuyTabIndex ) )
					SetBuyPage( m_byBuyBulletPageStartIndex[pi_byPartsTabIndex] );

				return;
			}
		}
	}
	// bullet
	else
	{
		BYTE l_byTabIndex = pi_byPartsTabIndex - TOTAL_PARTS_TYPE_NUM;

		CPartsList::iterator it;
		for( it = m_listBuyBullet[l_byTabIndex].begin(); it != m_listBuyBullet[l_byTabIndex].end(); ++it )
		{
			if( ( *it )->dwPartsID == pi_dwPartsID )
			{
				delete ( *it );
				( *it ) = NULL;
				m_listBuyBullet[l_byTabIndex].erase( it );

				// update page
				if( ( m_byCurTabIndex == BULLET_TAB ) && ( l_byTabIndex == m_byCurBuyTabIndex ) )
					SetBuyPage( m_byBuyBulletPageStartIndex[l_byTabIndex] );

				return;
			}
		}
	}
}

CGUIItem *
CGUIUnitTuningWindow::GetBuyParts( BYTE pi_byPartsTabIndex, DWORD pi_dwPartsID )
{
	if( pi_byPartsTabIndex >= TOTAL_PARTS_TYPE_NUM + BULLET_TYPE_NUM )
		return NULL;

	// parts
	if( pi_byPartsTabIndex < TOTAL_PARTS_TYPE_NUM )
	{
		CPartsList::iterator it;
		for( it = m_listBuyParts[pi_byPartsTabIndex].begin(); it != m_listBuyParts[pi_byPartsTabIndex].end(); ++it )
		{
			if( ( *it )->dwPartsID == pi_dwPartsID )
			{
				return &( *it )->uiPartsIcon;
			}
		}
	}
	// bullet
	else
	{
		BYTE l_byTabIndex = pi_byPartsTabIndex - TOTAL_PARTS_TYPE_NUM;

		CPartsList::iterator it;
		for( it = m_listBuyBullet[l_byTabIndex].begin(); it != m_listBuyBullet[l_byTabIndex].end(); ++it )
		{
			if( ( *it )->dwPartsID == pi_dwPartsID )
			{
				return &( *it )->uiPartsIcon;
			}
		}
	}

	return NULL;
}

void
CGUIUnitTuningWindow::ClearBuyTuningPartsList( BYTE pi_byPartsTabIndex )
{
	if( pi_byPartsTabIndex >= TOTAL_PARTS_TYPE_NUM )
		return;

	CPartsList::iterator it;
	for( it = m_listBuyParts[pi_byPartsTabIndex].begin(); it != m_listBuyParts[pi_byPartsTabIndex].end(); ++it )
	{
		delete *it;
	}

	m_listBuyParts[pi_byPartsTabIndex].clear();

	if( m_byCurTabIndex == PARTS_TAB )
	{
		for( int i = 0; i < ITEM_BOARD_NUM; ++i )
			m_uiBuyItemBoard[i].SetEmpty();
	}
}

void
CGUIUnitTuningWindow::ClearBuyBulletList( BYTE pi_byBulletTabIndex )
{
	if( pi_byBulletTabIndex >= BULLET_TYPE_NUM )
		return;

	CPartsList::iterator it;
	for( it = m_listBuyBullet[pi_byBulletTabIndex].begin(); it != m_listBuyBullet[pi_byBulletTabIndex].end(); ++it )
	{
		delete *it;
	}

	m_listBuyBullet[pi_byBulletTabIndex].clear();

	if( m_byCurTabIndex == BULLET_TAB )
	{
		for( int i = 0; i < ITEM_BOARD_NUM; ++i )
			m_uiBuyItemBoard[i].SetEmpty();
	}
}

void	
CGUIUnitTuningWindow::SetBuyItemTab( BYTE pi_byTabIndex, BOOL pi_bUpdate )
{
	if( m_byCurTabIndex == 0xFF )
		return;	

	if( m_byCurTabIndex == PARTS_TAB && pi_byTabIndex >= TOTAL_PARTS_TYPE_NUM )
		pi_byTabIndex = 0;
	else if( m_byCurTabIndex == BULLET_TAB && pi_byTabIndex >= BULLET_TYPE_NUM )
		pi_byTabIndex = 0;

	if( !pi_bUpdate && m_byCurBuyTabIndex == pi_byTabIndex )
		return;		

	m_byTempBuyTabIndex[m_byCurTabIndex] = pi_byTabIndex;
	m_byCurBuyTabIndex = m_byTempBuyTabIndex[m_byCurTabIndex];	
	
	// set active tab
	if( m_byCurTabIndex == PARTS_TAB )
	{
		m_uiActiveTabBoard.SetPos( m_ptPos.x + 30 + m_byCurBuyTabIndex * 21, m_ptPos.y + 224 );	

		static char PARTS_TAB_STR[TOTAL_PARTS_TYPE_NUM][16] = { "머리", "상체", "하체", "기본무기", "추가무기", "백팩" };
		m_uiActiveTabName.SetString( PARTS_TAB_STR[m_byCurBuyTabIndex] );
	}
	else
	{
/*
		m_uiActiveTabBoard.SetPos( m_ptPos.x + 60 + m_byCurBuyTabIndex * 21, m_ptPos.y + 224 );

		static char BULLET_TAB_STR[BULLET_TYPE_NUM][16] = { "탄창1", "탄창2" };
		m_uiActiveTabName.SetString( BULLET_TAB_STR[m_byCurBuyTabIndex] );
*/
	}
	m_uiActiveTabName.SetPos( m_uiActiveTabBoard.m_ptPos.x + 24 + ( 50 - m_uiActiveTabName.m_ptSize.x ) / 2, m_uiActiveTabBoard.m_ptPos.y + 6 );	
	m_uiBuyItemTab[m_byCurBuyTabIndex].SetPos( m_uiActiveTabBoard.m_ptPos.x + 3, m_uiActiveTabBoard.m_ptPos.y + 3 );

	// repos tab
	for( int i = m_byCurBuyTabIndex-1; i >= 0; --i )
	{		
		m_uiBuyItemTab[i].SetPos( m_uiActiveTabBoard.m_ptPos.x - (m_byCurBuyTabIndex - i) * ( m_uiBuyItemTab[i].m_ptSize.x + 3 ),
								   m_uiActiveTabBoard.m_ptPos.y + 7 );	
	}

	int l_nTotalTabNum = ( m_byCurTabIndex == PARTS_TAB ) ? TOTAL_PARTS_TYPE_NUM : BULLET_TYPE_NUM;
	for( i = m_byCurBuyTabIndex+1; i < l_nTotalTabNum; ++i )
	{		
		m_uiBuyItemTab[i].SetPos( m_uiActiveTabBoard.m_ptPos.x + m_uiActiveTabBoard.m_ptSize.x + 1 + (i-m_byCurBuyTabIndex-1)*21,
								   m_uiActiveTabBoard.m_ptPos.y + 7 );
	}

	// set page
	if( m_byCurTabIndex == PARTS_TAB )
		SetBuyPage( m_byBuyPartsPageStartIndex[m_byCurBuyTabIndex] );
	else
		SetBuyPage( m_byBuyBulletPageStartIndex[m_byCurBuyTabIndex] );

	// select parts
	if( m_byCurTabIndex == PARTS_TAB )
		Select_Parts( m_byCurBuyTabIndex );
}

void
CGUIUnitTuningWindow::SetBuyPage( BYTE pi_byPageIndex )
{	
	if( m_byCurTabIndex >= TOTAL_TAB_NUM )
		return;

	CPartsList * l_pBuyList;
	if( m_byCurTabIndex == PARTS_TAB )
	{
		if( m_byCurBuyTabIndex >= TOTAL_PARTS_TYPE_NUM )
			return;

		l_pBuyList = m_listBuyParts;
	}
	else if( m_byCurTabIndex == BULLET_TAB )
	{
		if( m_byCurBuyTabIndex >= BULLET_TYPE_NUM )
			return;

		l_pBuyList = m_listBuyBullet;
	}
	else
	{
		return;
	}

	BYTE l_byPageStartIndex = pi_byPageIndex;
	if( l_pBuyList[m_byCurBuyTabIndex].size() < ITEM_BOARD_NUM )		
		l_byPageStartIndex = 0;
	else if( l_byPageStartIndex > l_pBuyList[m_byCurBuyTabIndex].size() - ITEM_BOARD_NUM )	
		l_byPageStartIndex = l_pBuyList[m_byCurBuyTabIndex].size() - ITEM_BOARD_NUM;

	// save buy page index
	if( m_byCurTabIndex == PARTS_TAB )
		m_byBuyPartsPageStartIndex[m_byCurBuyTabIndex] = l_byPageStartIndex;
	else
		m_byBuyBulletPageStartIndex[m_byCurBuyTabIndex] = l_byPageStartIndex;


	for( int i = 0; i < ITEM_BOARD_NUM; ++i )
	{	
		if( l_byPageStartIndex + i < l_pBuyList[m_byCurBuyTabIndex].size() )
		{
			m_uiBuyItemBoard[i].SetItem( &l_pBuyList[m_byCurBuyTabIndex][l_byPageStartIndex + i]->uiPartsIcon );
		}
		else
		{
			m_uiBuyItemBoard[i].SetEmpty();
		}
	}

	if( l_byPageStartIndex > 0 )
		m_uiPageLeftButton.Enable();
	else
		m_uiPageLeftButton.Disable();

	if( l_byPageStartIndex + ITEM_BOARD_NUM < l_pBuyList[m_byCurBuyTabIndex].size() )
	{
		m_uiPageRightButton.Enable();		
	}
	else
	{
		m_uiPageRightButton.Disable();		
	}
}

void
CGUIUnitTuningWindow::MouseEvent( _mouse_event &event )
{
	CGUIWindow::MouseEvent( event );

	if( event.id == MOUSE_STROKED )
	{
		if( event.source == &m_uiMainFrameKey )
		{
			PostUIMessage( WM_TUNING_BOARD_MAINFRAME, 0 );
			return;
		}

		// select tab
		for( int i=0; i<TOTAL_TAB_NUM; ++i )
		{
			if( event.source == &m_uiTab[i] )
			{
				SetTab( i );
				return;
			}
		}
		
		// select item tab
		for( i = 0; i < BUY_ITEM_TAB_NUM; ++i )
		{
			if( event.source == &m_uiBuyItemTab[i] )
			{
				SetBuyItemTab( i );
				return;
			}
		}	


		// select parts
		for( i = 0; i < ITEM_BOARD_NUM; ++i )
		{
			if( event.source == &m_uiBuyItemBoard[i] )
			{
				if( m_uiBuyItemBoard[i].IsEmpty() )
					return;

				// select parts
				UI_MOUSE_MSG stSendMsg;				
				if( m_byCurTabIndex == PARTS_TAB )
				{
					stSendMsg.m_byPackNo	= m_byCurBuyTabIndex;
					stSendMsg.m_bySocketNo	= m_byBuyPartsPageStartIndex[m_byCurBuyTabIndex] + i;
				}
				else
				{
					stSendMsg.m_byPackNo	= m_byCurBuyTabIndex + TOTAL_PARTS_TYPE_NUM;
					stSendMsg.m_bySocketNo	= m_byBuyBulletPageStartIndex[m_byCurBuyTabIndex] + i;
				}

				stSendMsg.m_byExtra = 0;

				PostUIMessage( WM_BOARD_ADD_OBJECT, stSendMsg );
				return;
			}
		}

		// unselect parts
		for( i = 0; i < TOTAL_PARTS_TYPE_NUM; ++i )
		{
			if( event.source == &m_uiTuningParts[i] )
			{
				if( m_uiTuningParts[i].IsEmpty() )
					return;
				
				PostUIMessage( WM_BOARD_REMOVE_OBJECT, UI_MOUSE_MSG( PARTS_TAB, i, 1, 1 ) );

				return;
			}
		}

		// unselect bullet
		for( i = 0; i < BULLET_TYPE_NUM; ++i )
		{
			if( event.source == &m_uiTuningBullet[i] )
			{
				PostUIMessage( WM_BOARD_REMOVE_OBJECT, UI_MOUSE_MSG( BULLET_TAB, i, 1, 1 ) );

				return;
			}
		}

		// unselect backpack bullet
		for( i = 0; i < BACK_PACK_BULLET_NUM; ++i )
		{
			if( event.source == &m_uiTuningBackPackBullet[i] )
			{
				PostUIMessage( WM_BOARD_REMOVE_OBJECT, UI_MOUSE_MSG( BULLET_TAB, ( i + BULLET_TYPE_NUM ), 1, 1 ) );

				return;
			}
		}
		
	}
	else if( event.id == MOUSE_HOVERED )
	{
		if( event.source == &m_uiMainFrameKey )
		{		
			//PostUIMessage( WM_BOARD_HOVER_OBJECT, 0 );
			return;
		}

		// buy parts list
		for( int i = 0; i < ITEM_BOARD_NUM; ++i )
		{
			if( event.source == &m_uiBuyItemBoard[i] )
			{
				if( m_uiBuyItemBoard[i].IsEmpty() )
					return;

				UI_MOUSE_MSG stSendMsg;				
				if( m_byCurTabIndex == PARTS_TAB )
				{
					stSendMsg.m_byPackNo	= m_byCurBuyTabIndex;
					stSendMsg.m_bySocketNo	= m_byBuyPartsPageStartIndex[m_byCurBuyTabIndex] + i;
				}
				else
				{
					stSendMsg.m_byPackNo	= m_byCurBuyTabIndex + TOTAL_PARTS_TYPE_NUM;
					stSendMsg.m_bySocketNo	= m_byBuyBulletPageStartIndex[m_byCurBuyTabIndex] + i;
				}

				stSendMsg.m_byExtra = 0;

				PostUIMessage( WM_BOARD_HOVER_OBJECT, stSendMsg );
				return;
			}
		}

		// parts
		for( i = 0; i < TOTAL_PARTS_TYPE_NUM; ++i )
		{
			if( event.source == &m_uiTuningParts[i] )
			{
				if( m_uiTuningParts[i].IsEmpty() )
					return;
				
				PostUIMessage( WM_BOARD_HOVER_OBJECT, UI_MOUSE_MSG( PARTS_TAB, i, 1, 1 ) );

				return;
			}
		}

		// bullet
		for( i = 0; i < BULLET_TYPE_NUM; ++i )
		{
			if( event.source == &m_uiTuningBullet[i] )
			{
				if( m_uiTuningBullet[i].IsEmpty() )
					return;

				PostUIMessage( WM_BOARD_HOVER_OBJECT, UI_MOUSE_MSG( BULLET_TAB, i, 1, 1 ) );

				return;
			}
		}

		// backpack bullet
		for( i = 0; i < BACK_PACK_BULLET_NUM; ++i )
		{
			if( event.source == &m_uiTuningBackPackBullet[i] )
			{
				if( m_uiTuningBackPackBullet[i].IsEmpty() )
					return;

				PostUIMessage( WM_BOARD_HOVER_OBJECT, UI_MOUSE_MSG( BULLET_TAB, ( i + BULLET_TYPE_NUM ), 1, 1 ) );

				return;
			}
		}
	}
	else if( event.id == MOUSE_CLICKED )
	{
		if( event.source == &m_uiPageLeftButton )
		{
			SetBuyPage( m_byBuyPartsPageStartIndex[m_byCurBuyTabIndex] - 1 );
		}
		else if( event.source == &m_uiPageRightButton )
		{
			SetBuyPage( m_byBuyPartsPageStartIndex[m_byCurBuyTabIndex] + 1 );
		}		
		else if( event.source == &m_uiRepairButton )
		{
			PostUIMessage( WM_TUNING_BOARD_REPAIR, 0 );
		}
		else if( event.source == &m_uiTuningButton )
		{
			PostUIMessage( WM_TUNING_BOARD_TUNING, 0 );
		}
		else if( event.source == &m_uiCancelButton )
		{
			PostUIMessage( WM_TUNING_BOARD_CANCEL, 0 );
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
CGUIUnitTuningWindow::SetRepairButtonState( BOOL pi_bFlag )
{
	if( pi_bFlag )
		m_uiRepairButton.Enable();
	else
		m_uiRepairButton.Disable();
}

void
CGUIUnitTuningWindow::SetTuningButtonState( BOOL pi_bFlag )
{
	if( pi_bFlag )
		m_uiTuningButton.Enable();
	else
		m_uiTuningButton.Disable();
}

void
CGUIUnitTuningWindow::SetCancelButtonState( BOOL pi_bFlag )
{
	if( pi_bFlag )
		m_uiCancelButton.Enable();
	else
		m_uiCancelButton.Disable();
}

//#define TAB_BORDER_COLOR	0xFF6C727D
#define TAB_BORDER_COLOR	0xFF7C828D

BOOL	
CGUIUnitTuningWindow::Draw( void )
{
	if( !IsShow() )
		return FALSE;

	CGUIContainer::Draw();

	if( m_byCurTabIndex == PARTS_TAB )
	{
		// tab left line
		Draw2DRectangle( m_ptPos.x + 3, m_ptPos.y + 252,
						 m_uiActiveTabBoard.m_ptPos.x, m_ptPos.y + 253, 
						 TAB_BORDER_COLOR );

		// active tab board 
		Draw2DRectangle( m_uiActiveTabBoard.m_ptPos.x, m_uiActiveTabBoard.m_ptPos.y,
						 m_uiActiveTabBoard.m_ptPos.x + 1, m_uiActiveTabBoard.m_ptPos.y + m_uiActiveTabBoard.m_ptSize.y,
						 TAB_BORDER_COLOR );

		Draw2DRectangle( m_uiActiveTabBoard.m_ptPos.x + 1, m_uiActiveTabBoard.m_ptPos.y,
						 m_uiActiveTabBoard.m_ptPos.x + m_uiActiveTabBoard.m_ptSize.x - 1, m_uiActiveTabBoard.m_ptPos.y + 1, 
						 TAB_BORDER_COLOR );

		Draw2DRectangle( m_uiActiveTabBoard.m_ptPos.x + m_uiActiveTabBoard.m_ptSize.x - 1, m_uiActiveTabBoard.m_ptPos.y,
						 m_uiActiveTabBoard.m_ptPos.x + m_uiActiveTabBoard.m_ptSize.x, m_uiActiveTabBoard.m_ptPos.y + m_uiActiveTabBoard.m_ptSize.y,
						 TAB_BORDER_COLOR );

		// tab right line
		Draw2DRectangle( m_uiActiveTabBoard.m_ptPos.x + m_uiActiveTabBoard.m_ptSize.x, m_ptPos.y + 252,
						 m_ptPos.x + m_ptSize.x - 3, m_ptPos.y + 253, 
						 TAB_BORDER_COLOR );
	}

	DrawDialogBox();

	return TRUE;
}