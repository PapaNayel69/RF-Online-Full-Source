////////////////////////////////////////////////////////////////////////////
//
// CGUICharInfoWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////

#include "GUICharInfoWindow.h"
#include "../SpriteMgr.h"
#include "../GUIController/GUIScreen.h"

#include "../../DefineMessage.h"

void EP_SetSkillInfoString_CharInfoWindow( TOOLTIP_SF_INFO * po_pTooltipInfo, 
										   BYTE pi_byClassTypeID, BYTE pi_byStepTypeID );
void EP_SetForceInfoString_CharInfoWindow( TOOLTIP_SF_INFO * po_pTooltipInfo,
										   BYTE pi_byClassTypeID, BYTE pi_byStepTypeID );

////////////////////////////////////////////////////////////////////////////

#define	TAB_BASIC	0
#define	TAB_STATE	1
#define	TAB_SKILL	2
#define	TAB_RESIST	3

#define	TITLE_COLOR			D3DCOLOR_XRGB(195,193,190)
#define	INFO_TITLE_COLOR	D3DCOLOR_XRGB(188,186,180)
#define	INFO_COLOR			D3DCOLOR_XRGB(196,167,145)
#define	UPGRADE_COLOR	0xFF00FF00


CGUICharInfoWindow::CGUICharInfoWindow()
{
	m_pRefGaugeExpWindow	= NULL;	

	m_byRaceType		= 0xff;

	m_bCreated			= FALSE;
}

CGUICharInfoWindow::~CGUICharInfoWindow()
{

}

void
CGUICharInfoWindow::Init( void )
{
	int i;

	SetSize( 228, 119 );
	
	m_uiAniTitleBar.SetSize( 228, 28 );
	m_uiCloseButton.SetSize( 16, 13 );
	m_uiHelpButton.SetSize( 16, 13 );

	m_uiLOutline.SetSize( 1, 0 );
	m_uiROutline.SetSize( 1, 0 );

	m_uiResizeUnderBar.SetSize( 228, 17 );		

	m_uiBoard.SetSize( m_ptSize.x, m_ptSize.y - m_uiAniTitleBar.m_ptSize.y - m_uiResizeUnderBar.m_ptSize.y );

	POINT l_ptSize;
	l_ptSize.x = 15;	
	l_ptSize.y = 10;
	m_uiScrollbar.SetTopArrowSize( l_ptSize );
	m_uiScrollbar.SetBottomArrowSize( l_ptSize );	
	l_ptSize.y = 18;
	m_uiScrollbar.SetBarSize( l_ptSize );	
	l_ptSize.y = m_uiBoard.m_ptSize.y;		
	m_uiScrollbar.SetSize( l_ptSize );

	for( i=0; i<4; ++i )	
	{
		m_uiTab[i].SetSize( 204, 18 );	
		m_uiTabBoard[i].SetSize( m_uiTab[i].m_ptSize );
	}
	
	m_uiTabBase.SetSize( m_uiTab[0].m_ptSize.x, m_uiBoard.m_ptSize.y );
	

	m_uiInfoBoard[TAB_BASIC].SetSize( m_uiTabBase.m_ptSize.x, 97 );
	m_uiInfoBoard[TAB_STATE].SetSize( m_uiTabBase.m_ptSize.x, 128 );
	m_uiInfoBoard[TAB_SKILL].SetSize( m_uiTabBase.m_ptSize.x, 214 );
	m_uiInfoBoard[TAB_RESIST].SetSize( m_uiTabBase.m_ptSize.x, 120 );

	m_uiCharInfoTitle[Name].SetString		( "캐릭터 이름:" );	
	m_uiCharInfoTitle[Race].SetString		( "       종족:" );
	m_uiCharInfoTitle[Attr].SetString		( "       성향:" );
	m_uiCharInfoTitle[Sex].SetString		( "       성별:" );
	m_uiCharInfoTitle[Level].SetString		( "       레벨:" );
	m_uiCharInfoTitle[Guild].SetString		( "       길드:" );
	m_uiCharInfoTitle[Hp].SetString			( "     생명력:" );
	m_uiCharInfoTitle[Fp].SetString			( "     포스력:" );
	m_uiCharInfoTitle[Sp].SetString			( "   스테미너:" );
	m_uiCharInfoTitle[Exp].SetString		( "     경험치:" );
	m_uiCharInfoTitle[PvP].SetString		( " PvP 포인트:" );
	m_uiCharInfoTitle[GenAttPower].SetString( "일반 공격력:" );
	m_uiCharInfoTitle[FcAttPower].SetString	( "포스 공격력:" );
	m_uiCharInfoTitle[Deffense].SetString	( "평균 방어력:" );	

	m_uiTabName[TAB_BASIC].SetString	( "일반 정보" );
	m_uiTabName[TAB_STATE].SetString	( "상태 정보" );
	m_uiTabName[TAB_SKILL].SetString	( "기술 정보" );
	m_uiTabName[TAB_RESIST].SetString	( "내성 정보" );

	m_uiResistIcon.SetSize( 15, 93 );
	
	m_uiBattleTitle.SetString( "전투 숙련도" );
	m_uiBattleName[MELEE_MASTERY].SetString		( "근접  기술" );
	m_uiBattleName[MISSILE_MASTERY].SetString	( "원거리기술" );
	m_uiBattleName[RACE_MASTERY].SetString		( "런처  기술" );
	m_uiBattleName[FORCE_MASTERY].SetString		( "포스  기술" );
	m_uiBattleName[SHIELD_MASTERY].SetString	( "방패  기술" );
	m_uiBattleName[DEFFENCE_MASTERY].SetString	( "방어  기술" );	

	m_uiMakeTitle.SetString( "제작 숙련도" );
	m_uiMakeName[WEAPON_MAKE].SetString	( "무기  방패" );
	m_uiMakeName[ARMOR_MAKE].SetString	( "방  어  구" );
	m_uiMakeName[BULLET_MAKE].SetString	( "발  사  물" );

	m_uiTitle.SetString( "캐릭터 정보" );


	// -----------------------------------------------------------
	m_uiTitle.SetPos( m_ptPos.x + 6, m_ptPos.y + 7 );
	m_uiCloseButton.SetPos( m_ptPos.x + 211, m_ptPos.y + 4 );	
	m_uiHelpButton.SetPos( m_ptPos.x + 194, m_ptPos.y + 4 );	

	m_uiBoard.SetPos( m_ptPos.x, m_ptPos.y + m_uiAniTitleBar.m_ptSize.y );

	m_uiLOutline.SetPos( m_uiBoard.m_ptPos.x + 2, m_uiAniTitleBar.m_ptPos.y + m_uiAniTitleBar.m_ptSize.y );
	m_uiROutline.SetPos( m_uiBoard.m_ptPos.x + m_uiBoard.m_ptSize.x - 2 - m_uiROutline.m_ptSize.x ,
						 m_uiAniTitleBar.m_ptPos.y + m_uiAniTitleBar.m_ptSize.y );

	m_uiTabBase.SetPos( m_uiLOutline.m_ptPos.x + m_uiLOutline.m_ptSize.x, m_uiBoard.m_ptPos.y );
	m_uiScrollbar.SetPos( m_uiROutline.m_ptPos.x - m_uiScrollbar.m_ptSize.x, m_uiBoard.m_ptPos.y );

	m_uiResizeUnderBar.SetPos( m_ptPos.x, m_uiBoard.m_ptPos.y + m_uiBoard.m_ptSize.y );	

	for( i=0; i<4; ++i )
	{
		m_uiTabBoard[i].SetPos( m_uiTabBase.m_ptPos.x, m_uiTabBase.m_ptPos.y + ( m_uiTab[i].m_ptSize.y + 3 ) * i );
		m_uiTab[i].SetPos( m_uiTabBoard[i].m_ptPos );
		m_uiTabName[i].SetPos( m_uiTab[i].m_ptPos.x + ( m_uiTab[i].m_ptSize.x - m_uiTabName[i].m_ptSize.x ) / 2,
							   m_uiTab[i].m_ptPos.y + 3 );
	}
	

	// 일반 정보
	for( i=0; i<Hp; ++i )
	{
		m_uiCharInfoTitle[i].SetPos( m_uiInfoBoard[TAB_BASIC].m_ptPos.x + 10,
									 m_uiInfoBoard[TAB_BASIC].m_ptPos.y + 5 + ( m_uiCharInfoTitle[i].m_ptSize.y + 3 ) * i );

		m_uiCharInfo[i].SetPos( m_uiCharInfoTitle[i].m_ptPos.x + m_uiCharInfoTitle[i].m_ptSize.x + 8,
								m_uiCharInfoTitle[i].m_ptPos.y );
	}

	// 상태 정보
	for( i=Hp; i<CharInfoNum; ++i )
	{
		m_uiCharInfoTitle[i].SetPos( m_uiInfoBoard[TAB_STATE].m_ptPos.x + 10,
									 m_uiInfoBoard[TAB_STATE].m_ptPos.y + 5 + ( m_uiCharInfoTitle[i].m_ptSize.y + 3 ) * ( i - Hp ) );

		m_uiCharInfo[i].SetPos( m_uiCharInfoTitle[i].m_ptPos.x + m_uiCharInfoTitle[i].m_ptSize.x + 8,
								m_uiCharInfoTitle[i].m_ptPos.y );
	}

	// 전투 기술
	m_uiBattleTitle.SetPos( m_uiInfoBoard[TAB_SKILL].m_ptPos.x + 70,
							m_uiInfoBoard[TAB_SKILL].m_ptPos.y + 7 );					
	for( i=0; i<DEFFENCE_MASTERY+1; ++i )
	{
		m_uiBattleName[i].SetPos( m_uiInfoBoard[TAB_SKILL].m_ptPos.x + 5, 
								  m_uiInfoBoard[TAB_SKILL].m_ptPos.y + 24 + i * 18 ); 

		m_uiBattlePoint[i].SetPos( m_uiInfoBoard[TAB_SKILL].m_ptPos.x,
								   m_uiBattleName[i].m_ptPos.y );

		m_uiBattleGauge[i].SetPos( m_uiInfoBoard[TAB_SKILL].m_ptPos.x + 66,
								   m_uiInfoBoard[TAB_SKILL].m_ptPos.y + 27 + i * 18 );
	}

	// 제작 기술
	m_uiMakeTitle.SetPos( m_uiInfoBoard[TAB_SKILL].m_ptPos.x + 70,
							m_uiInfoBoard[TAB_SKILL].m_ptPos.y + 139 );				
	for( i=0; i<BULLET_MAKE+1; ++i )
	{
		m_uiMakeName[i].SetPos( m_uiInfoBoard[TAB_SKILL].m_ptPos.x + 5, 
								m_uiInfoBoard[TAB_SKILL].m_ptPos.y + 156 + i * 18 ); 

		m_uiMakePoint[i].SetPos( m_uiInfoBoard[TAB_SKILL].m_ptPos.x,
								 m_uiMakeName[i].m_ptPos.y );

		m_uiMakeGauge[i].SetPos( m_uiInfoBoard[TAB_SKILL].m_ptPos.x + 66,
								 m_uiInfoBoard[TAB_SKILL].m_ptPos.y + 159 + i * 18 );	
	}

	// 내성 
	m_uiResistIcon.SetPos( m_uiInfoBoard[TAB_RESIST].m_ptPos.x + 8, m_uiInfoBoard[TAB_RESIST].m_ptPos.y + 14 );
	for( i=0; i<WIND_RESIST+1; ++i )
		m_uiResistPoint[i].SetPos( m_uiInfoBoard[TAB_RESIST].m_ptPos.x + 26,
								   m_uiInfoBoard[TAB_RESIST].m_ptPos.y + 16 + i * 26 );


	// -----------------------------------------------------------	
	for( i=0; i<Hp; ++i )
	{
		m_uiInfoBoard[TAB_BASIC].Add( &m_uiCharInfoTitle[i] );
		m_uiInfoBoard[TAB_BASIC].Add( &m_uiCharInfo[i] );
	}	

	for( i=Hp; i<CharInfoNum; ++i )
	{
		m_uiInfoBoard[TAB_STATE].Add( &m_uiCharInfoTitle[i] );
		m_uiInfoBoard[TAB_STATE].Add( &m_uiCharInfo[i] );
	}	

	m_uiInfoBoard[TAB_STATE].Add( &m_uiUpgradeGenAttFc[0] );
	m_uiInfoBoard[TAB_STATE].Add( &m_uiUpgradeGenAttFc[1] );
	m_uiInfoBoard[TAB_STATE].Add( &m_uiUpgradeForceAttFc[0] );
	m_uiInfoBoard[TAB_STATE].Add( &m_uiUpgradeForceAttFc[1] );

	m_uiInfoBoard[TAB_SKILL].Add( &m_uiBattleTitle );
	for( i=0; i<DEFFENCE_MASTERY+1; ++i )
	{
		m_uiInfoBoard[TAB_SKILL].Add( &m_uiBattleName[i] );
		m_uiInfoBoard[TAB_SKILL].Add( &m_uiBattlePoint[i] );
		m_uiInfoBoard[TAB_SKILL].Add( &m_uiBattleGauge[i] );
	}

	m_uiInfoBoard[TAB_SKILL].Add( &m_uiMakeTitle );
	for( i=0; i<BULLET_MAKE+1; ++i )
	{
		m_uiInfoBoard[TAB_SKILL].Add( &m_uiMakeName[i] );
		m_uiInfoBoard[TAB_SKILL].Add( &m_uiMakePoint[i] );
		m_uiInfoBoard[TAB_SKILL].Add( &m_uiMakeGauge[i] );		
	}
	
	m_uiInfoBoard[TAB_RESIST].Add( &m_uiResistIcon );
	for( i=0; i<WIND_RESIST+1; ++i )
		m_uiInfoBoard[TAB_RESIST].Add( &m_uiResistPoint[i] );

	// ------------------------------------------------------------

	for( i=0; i<4; ++i )
	{
		m_uiTab[i].AddMouseEventHandler( this );
		m_uiTabName[i].AddMouseEventHandler( this );		
		m_uiTabBoard[i].Add( &m_uiTab[i] );
		m_uiTabBoard[i].Add( &m_uiTabName[i] );		

		m_uiTabBase.Add( &m_uiTabBoard[i] );
	}			

	m_uiBoard.Add( &m_uiTabBase );
	Add( &m_uiBoard );	

	m_uiScrollbar.AddScrollHandler( this );
	Add( &m_uiScrollbar );


	Add( &m_uiAniTitleBar );
	Add( &m_uiTitle );	
	m_uiCloseButton.AddMouseEventHandler( this );
	m_uiHelpButton.AddMouseEventHandler( this );
	Add( &m_uiCloseButton );
	Add( &m_uiHelpButton );

	Add( &m_uiLOutline );
	Add( &m_uiROutline );

	m_uiResizeUnderBar.AddMouseEventHandler( this );	
	Add( &m_uiResizeUnderBar );
	

	// -----------------------------------------------------------
	SetSprite();	

	m_uiTitle.GetFont()->dwColor = TITLE_COLOR;
	m_uiBoard.SetARGB( D3DCOLOR_ARGB( 204, 22, 28, 28 ) );

	for( i=0; i<4; ++i )
		m_uiTabName[i].GetFont()->dwColor = D3DCOLOR_XRGB( 218, 216, 212 );

	for( i=0; i<CharInfoNum; ++i )
	{
		m_uiCharInfoTitle[i].GetFont()->dwColor = INFO_TITLE_COLOR;
		m_uiCharInfo[i].GetFont()->dwColor = INFO_COLOR;
	}

	m_uiLOutline.SetARGB( D3DCOLOR_XRGB( 175, 180, 186 ) );
	m_uiROutline.SetARGB( D3DCOLOR_XRGB( 175, 180, 186 ) );

	m_uiResistPoint[FIRE_RESIST].GetFont()->dwColor = INFO_COLOR;
	m_uiResistPoint[AQUA_RESIST].GetFont()->dwColor =  INFO_COLOR;
	m_uiResistPoint[TERRA_RESIST].GetFont()->dwColor = INFO_COLOR;
	m_uiResistPoint[WIND_RESIST].GetFont()->dwColor = INFO_COLOR;

	m_uiBattleTitle.GetFont()->dwColor = D3DCOLOR_XRGB( 207, 207, 207 );
	m_uiMakeTitle.GetFont()->dwColor = D3DCOLOR_XRGB( 207, 207, 207 );

	for( i=0; i<DEFFENCE_MASTERY+1; ++i )
	{
		m_uiBattleName[i].GetFont()->dwColor = INFO_TITLE_COLOR;
		m_uiBattlePoint[i].GetFont()->dwColor = INFO_COLOR;
	}

	for( i=0; i<BULLET_MAKE+1; ++i )
	{
		m_uiMakeName[i].GetFont()->dwColor = INFO_TITLE_COLOR;
		m_uiMakePoint[i].GetFont()->dwColor = INFO_COLOR;
	}

	// -----------------------------------------------------------
	m_uiAniTitleBar.SetTotalFrameNum( 18 );
	m_uiAniTitleBar.SetObjectNumInLine( 1 );
	m_uiAniTitleBar.SetFPS( 10 );
	m_uiAniTitleBar.SetStopInterval( 1000 );

	SetMoveObject( &m_uiTitle );
	SetMoveObject( &m_uiAniTitleBar );	
	SetMovable( TRUE );

	SetReSizeObject( CGUIWindow::BOTTOM, &m_uiResizeUnderBar );
	SetReSizable( TRUE );

	m_uiScrollbar.SetAdjustmentUnit( 12 );	

	for( i=0; i<DEFFENCE_MASTERY+1; ++i )					
		m_uiBattleGauge[i].SetGap( 3, 0 );	

	for( i=0; i<BULLET_MAKE+1; ++i )					
		m_uiMakeGauge[i].SetGap( 3, 0 );	

	// resize
	RECT l_rtBound;
	GetBound( l_rtBound );	
	l_rtBound.bottom = l_rtBound.top + 395;
	SetBound( l_rtBound );

	SetTab( 0, TRUE );	
	SetTab( 1, TRUE );

	SetName( "CharInfoWin" );
	
	m_bCreated = TRUE;		

	// init
	for( i=0; i<DEFFENCE_MASTERY+1; ++i )	
		SetBattleMastery( i, 0 );

	for( i=0; i<BULLET_MAKE+1; ++i )
		SetMakeMastery( i, 0 );

	SoundOn( TRUE );

	SetWheelScrollObject( &m_uiScrollbar );

/*
	// test
	SetMakeMasteryExp( 0, 0.5f );
	SetMakeMasteryExp( 1, 1.0f );
	SetMakeMasteryExp( 2, 0.3f );

	SetBattleMasteryExp( 1, 0.8f );
	SetBattleMasteryExp( 2, 0.3f );
*/	

}

void
CGUICharInfoWindow::SetSprite( void )
{
	if( !m_bCreated )
	{
		int i;
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );		

		l_spriteInfo.wActionNo = 6;

		// info board
		l_spriteInfo.wFrameNo = 0;
		m_uiInfoBoard[TAB_SKILL].SetSpriteInfo( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 2;
		m_uiInfoBoard[TAB_RESIST].SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 1;
		m_uiResistIcon.SetSpriteInfo( &l_spriteInfo );		


		l_spriteInfo.wActionNo = 7;

		// tab
		l_spriteInfo.wFrameNo = 1;
		for( i=0; i<4; ++i )				
			m_uiTab[i].SetSpriteInfo( &l_spriteInfo );		

		l_spriteInfo.wFrameNo = 3;
		for( i=0; i<DEFFENCE_MASTERY+1; ++i )				
			m_uiBattleGauge[i].SetSpriteInfo( &l_spriteInfo );

		for( i=0; i<BULLET_MAKE+1; ++i )	
			m_uiMakeGauge[i].SetSpriteInfo( &l_spriteInfo );
		
		
		// under bar
		l_spriteInfo.wFrameNo = 2;
		m_uiResizeUnderBar.SetSpriteInfo( &l_spriteInfo );

		// title
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 5;
		m_uiAniTitleBar.SetSpriteInfo( &l_spriteInfo );

		// close button
		m_uiCloseButton.SetSpriteInfoEx( gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON1 ) );
		m_uiCloseButton.SetSpriteInfoEx( HOVERED, gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON2 ) );
		m_uiCloseButton.SetSpriteInfoEx( PRESSED, gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON3 ) );

		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );
		l_spriteInfo.wActionNo = 5;
		l_spriteInfo.wFrameNo = 0;
		m_uiHelpButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 1;
		m_uiHelpButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 2;
		m_uiHelpButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );

		// scrollbar	
		SPRITE_INFO l_sScrollbarSprite[2];
		l_sScrollbarSprite[0].pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );
		l_sScrollbarSprite[0].wActionNo = 4;
		memcpy( &l_sScrollbarSprite[1], &l_sScrollbarSprite[0], sizeof( SPRITE_INFO ) );
		
		l_sScrollbarSprite[0].wFrameNo = 0;
		m_uiScrollbar.SetSpriteInfo( &l_sScrollbarSprite[0] );
		l_sScrollbarSprite[0].wFrameNo = 1;
		m_uiScrollbar.SetBarSpriteInfo( &l_sScrollbarSprite[0] );

		l_sScrollbarSprite[0].wFrameNo = 2;
		l_sScrollbarSprite[1].wFrameNo = 3;
		m_uiScrollbar.SetTopArrowSpriteInfo( l_sScrollbarSprite );

		l_sScrollbarSprite[0].wFrameNo = 4;
		l_sScrollbarSprite[1].wFrameNo = 5;
		m_uiScrollbar.SetBottomArrowSpriteInfo( l_sScrollbarSprite );
	}
	else
	{
		int i;

		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );		
		if( !l_pSprite )
			return;

		// tab		
		for( i=0; i<4; ++i )
			m_uiTab[i].GetSpriteInfo()->pSprite = l_pSprite;

		for( i=0; i<DEFFENCE_MASTERY+1; ++i )				
			m_uiBattleGauge[i].GetSpriteInfo()->pSprite = l_pSprite;

		for( i=0; i<BULLET_MAKE+1; ++i )	
			m_uiMakeGauge[i].GetSpriteInfo()->pSprite = l_pSprite;

		m_uiInfoBoard[TAB_SKILL].GetSpriteInfo()->pSprite = l_pSprite;
		m_uiInfoBoard[TAB_RESIST].GetSpriteInfo()->pSprite = l_pSprite;

		m_uiResizeUnderBar.GetSpriteInfo()->pSprite = l_pSprite;

		m_uiResistIcon.GetSpriteInfo()->pSprite = l_pSprite;

		m_uiAniTitleBar.GetSpriteInfo()->pSprite = l_pSprite;		

		l_pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );
		m_uiCloseButton.SetSprite( l_pSprite );
		m_uiHelpButton.SetSprite( l_pSprite );

		m_uiScrollbar.SetSprite( l_pSprite );
	}
}

void
CGUICharInfoWindow::SetBound( RECT & pi_rtBound )
{
	if( pi_rtBound.bottom - pi_rtBound.top < m_uiAniTitleBar.m_ptSize.y + m_uiResizeUnderBar.m_ptSize.y + 80 )
		return;
	
	//CGUIObject::SetBound( pi_rtBound );
	SetPos( pi_rtBound.left, pi_rtBound.top );
	SetSize( pi_rtBound.right - pi_rtBound.left, pi_rtBound.bottom - pi_rtBound.top );
	
	// resize board
	m_uiBoard.SetSize( m_ptSize.x, m_ptSize.y - m_uiAniTitleBar.m_ptSize.y - m_uiResizeUnderBar.m_ptSize.y );	

	// resize scrollbar	
	RECT l_rtBound;
	m_uiScrollbar.GetBound( l_rtBound );
	l_rtBound.bottom = l_rtBound.top + m_uiBoard.m_ptSize.y;
	m_uiScrollbar.SetBound( l_rtBound );

	// setamount scrollbar	
	int l_nCurAmount;
	int l_nTotalAmount;
	l_nTotalAmount = m_uiBoard.m_ptPos.y - m_uiTabBase.m_ptPos.y;
	if( m_uiTabBase.m_ptPos.y + m_uiTabBase.m_ptSize.y > m_uiBoard.m_ptPos.y + m_uiBoard.m_ptSize.y )
		l_nTotalAmount += m_uiTabBase.m_ptPos.y + m_uiTabBase.m_ptSize.y - ( m_uiBoard.m_ptPos.y + m_uiBoard.m_ptSize.y );	

	if( m_uiScrollbar.GetCurrentAmount() > l_nTotalAmount )
		l_nCurAmount = l_nTotalAmount;
	else
		l_nCurAmount = m_uiScrollbar.GetCurrentAmount();

	m_uiScrollbar.SetAmount( l_nTotalAmount, l_nCurAmount );

	m_uiLOutline.SetSize( m_uiLOutline.m_ptSize.x, m_uiBoard.m_ptSize.y );
	m_uiROutline.SetSize( m_uiROutline.m_ptSize.x, m_uiBoard.m_ptSize.y );

	// repos underbar
	m_uiResizeUnderBar.SetPos( m_ptPos.x, m_ptPos.y + m_ptSize.y - m_uiResizeUnderBar.m_ptSize.y );
}

void
CGUICharInfoWindow::MouseEvent( _mouse_event &event )
{
	CGUIWindow::MouseEvent( event );


	if( event.id == MOUSE_STROKED )
	{
		for( int i=0; i<4; ++i )
		{
			if( event.source == &m_uiTab[i] ||
				event.source == &m_uiTabName[i] )
			{
				SetTab( i, !IsOpenedTab( i ) );
				return;
			}	
		}
	}	
	else if( event.id == MOUSE_CLICKED )
	{	
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
CGUICharInfoWindow::ScrollEvent( _scroll_event & event )
{
	m_uiTabBase.SetPos( m_uiTabBase.m_ptPos.x, 
						m_uiBoard.m_ptPos.y - m_uiScrollbar.GetCurrentAmount() );						
}

void
CGUICharInfoWindow::SetGaugeExpWindow( CGUIGaugeExpWindow * l_pRefWindow )
{
	m_pRefGaugeExpWindow = l_pRefWindow;

	m_pRefGaugeExpWindow->SetCharInfoWin( this );
}

void
CGUICharInfoWindow::SetTab( BYTE pi_byTabIndex, BOOL pi_bOpen )
{	
	if( IsOpenedTab( pi_byTabIndex ) )
	{
		if( pi_bOpen ) return;
	}
	else
	{
		if( !pi_bOpen ) return;
	}

	if( pi_bOpen )
	{
		// add infoboard
		m_uiInfoBoard[pi_byTabIndex].SetPos( m_uiTab[pi_byTabIndex].m_ptPos.x,
											 m_uiTab[pi_byTabIndex].m_ptPos.y + m_uiTab[pi_byTabIndex].m_ptSize.y );
		m_uiInfoBoard[pi_byTabIndex].Show( IsShow() );
		m_uiTabBoard[pi_byTabIndex].Add( &m_uiInfoBoard[pi_byTabIndex] );

		// resize tabboard
		m_uiTabBoard[pi_byTabIndex].SetSize( m_uiTab[pi_byTabIndex].m_ptSize.x,
											 m_uiTab[pi_byTabIndex].m_ptSize.y + m_uiInfoBoard[pi_byTabIndex].m_ptSize.y );			

		// sprite
		m_uiTab[pi_byTabIndex].GetSpriteInfo()->wFrameNo = 0;
	}
	else
	{
		m_uiTabBoard[pi_byTabIndex].Remove( &m_uiInfoBoard[pi_byTabIndex] );		

		// resize tabboard
		m_uiTabBoard[pi_byTabIndex].SetSize( m_uiTab[pi_byTabIndex].m_ptSize.x, m_uiTab[pi_byTabIndex].m_ptSize.y );

		// sprite
		m_uiTab[pi_byTabIndex].GetSpriteInfo()->wFrameNo = 1;
	}

	// repos tabboard
	for( int i=pi_byTabIndex+1; i<4; ++i )
	{
		m_uiTabBoard[i].SetPos( m_uiTabBoard[i-1].m_ptPos.x,
								m_uiTabBoard[i-1].m_ptPos.y + m_uiTabBoard[i-1].m_ptSize.y );
	}

	// resize tabbase
	m_uiTabBase.SetSize( m_uiTabBase.m_ptSize.x, 
						 m_uiTabBoard[3].m_ptPos.y + m_uiTabBoard[3].m_ptSize.y - m_uiTabBoard[0].m_ptPos.y );

	// set scrollbar
	int l_nCurAmount;
	int l_nTotalAmount;
	l_nTotalAmount = m_uiBoard.m_ptPos.y - m_uiTabBase.m_ptPos.y;
	if( m_uiTabBase.m_ptPos.y + m_uiTabBase.m_ptSize.y > m_uiBoard.m_ptPos.y + m_uiBoard.m_ptSize.y )
		l_nTotalAmount += m_uiTabBase.m_ptPos.y + m_uiTabBase.m_ptSize.y - ( m_uiBoard.m_ptPos.y + m_uiBoard.m_ptSize.y );		

	if( m_uiScrollbar.GetCurrentAmount() > l_nTotalAmount )
		l_nCurAmount = l_nTotalAmount;
	else
		l_nCurAmount = m_uiScrollbar.GetCurrentAmount();

	m_uiScrollbar.SetAmount( l_nTotalAmount, l_nCurAmount );
}

BOOL
CGUICharInfoWindow::IsOpenedTab( BYTE pi_byTabIndex )
{
	return m_uiTabBoard[pi_byTabIndex].IsAddedObject( &m_uiInfoBoard[pi_byTabIndex] );		
}

void
CGUICharInfoWindow::SetLevel( WORD pi_wLevel )
{
	char buf[8];
	sprintf( buf, "%d", pi_wLevel );

	m_uiCharInfo[Level].SetString( buf );
}

void
CGUICharInfoWindow::SetHP( DWORD pi_dwCurHP, DWORD pi_dwMaxHP )
{
	char buf[32];
	sprintf( buf, "%d / %d", pi_dwCurHP, pi_dwMaxHP );

	m_uiCharInfo[Hp].SetString( buf );	
}

void
CGUICharInfoWindow::SetFP( DWORD pi_dwCurFP, DWORD pi_dwMaxFP )
{
	char buf[32];
	sprintf( buf, "%d / %d", pi_dwCurFP, pi_dwMaxFP );

	m_uiCharInfo[Fp].SetString( buf );	
}

void
CGUICharInfoWindow::SetSP( DWORD pi_dwCurSP, DWORD pi_dwMaxSP )
{
	char buf[32];
	sprintf( buf, "%d / %d", pi_dwCurSP, pi_dwMaxSP );

	m_uiCharInfo[Sp].SetString( buf );	
}

void
CGUICharInfoWindow::SetPvPPoint( DWORD pi_dwPvPPoint )
{
	char buf[16];
	sprintf( buf, "%d", pi_dwPvPPoint );

	m_uiCharInfo[PvP].SetString( buf );
}

void
CGUICharInfoWindow::SetExp( float pi_pRate )
{
	char buf[16];
	sprintf( buf, "%.2f %%", pi_pRate );

	m_uiCharInfo[Exp].SetString( buf );	
}

void
CGUICharInfoWindow::SetGenAttackFc( WORD pi_wMinGenAttackFc, WORD pi_wMaxGenAttackFc )
{
	WORD l_wMin[2];
	WORD l_wMax[2];
	l_wMin[0] = l_wMin[1] = pi_wMinGenAttackFc;
	l_wMax[0] = l_wMax[1] = pi_wMaxGenAttackFc;
	SetGenAttackFc2( l_wMin, l_wMax );
}

void
CGUICharInfoWindow::SetForceAttackFc( WORD pi_wMinForceAttackFc, WORD pi_wMaxForceAttackFc )
{
	WORD l_wMin[2];
	WORD l_wMax[2];
	l_wMin[0] = l_wMin[1] = pi_wMinForceAttackFc;
	l_wMax[0] = l_wMax[1] = pi_wMaxForceAttackFc;
	SetForceAttackFc2( l_wMin, l_wMax );
}

void
CGUICharInfoWindow::SetDeffenceFc( WORD pi_wDefFc )
{
	WORD l_wValue[2];
	l_wValue[0] = l_wValue[1] = pi_wDefFc;
	SetDeffenceFc2( l_wValue );
}	

void
CGUICharInfoWindow::SetGenAttackFc2( WORD pi_wMinGenAttackFc[2], WORD pi_wMaxGenAttackFc[2] )
{
	char buf[16];
	
	sprintf( buf, "%d - ", pi_wMinGenAttackFc[1] );
	m_uiCharInfo[GenAttPower].SetString( buf );

	if( pi_wMinGenAttackFc[0] < pi_wMinGenAttackFc[1] )
		m_uiUpgradeGenAttFc[0].GetFont()->dwColor = UPGRADE_COLOR;
	else
		m_uiUpgradeGenAttFc[0].GetFont()->dwColor = INFO_COLOR;

	if( pi_wMaxGenAttackFc[0] < pi_wMaxGenAttackFc[1] )
		m_uiUpgradeGenAttFc[1].GetFont()->dwColor = UPGRADE_COLOR;
	else
		m_uiUpgradeGenAttFc[1].GetFont()->dwColor = INFO_COLOR;
	
	itoa( pi_wMinGenAttackFc[1], buf, 10 );
	m_uiUpgradeGenAttFc[0].SetString( buf );

	itoa( pi_wMaxGenAttackFc[1], buf, 10 );
	m_uiUpgradeGenAttFc[1].SetString( buf );
	
	// repos text	
	m_uiUpgradeGenAttFc[0].SetPos( m_uiCharInfo[GenAttPower].m_ptPos );
	m_uiUpgradeGenAttFc[1].SetPos( m_uiCharInfo[GenAttPower].m_ptPos.x + m_uiCharInfo[GenAttPower].m_ptSize.x,
								   m_uiCharInfo[GenAttPower].m_ptPos.y );
								   
}

void
CGUICharInfoWindow::SetForceAttackFc2( WORD pi_wMinForceAttackFc[2], WORD pi_wMaxForceAttackFc[2] )
{
	char buf[16];
	
	sprintf( buf, "%d - ", pi_wMinForceAttackFc[1] );
	m_uiCharInfo[FcAttPower].SetString( buf );

	if( pi_wMinForceAttackFc[0] < pi_wMinForceAttackFc[1] )
		m_uiUpgradeForceAttFc[0].GetFont()->dwColor = UPGRADE_COLOR;
	else
		m_uiUpgradeForceAttFc[0].GetFont()->dwColor = INFO_COLOR;

	if( pi_wMaxForceAttackFc[0] < pi_wMaxForceAttackFc[1] )
		m_uiUpgradeForceAttFc[1].GetFont()->dwColor = UPGRADE_COLOR;
	else
		m_uiUpgradeForceAttFc[1].GetFont()->dwColor = INFO_COLOR;
	
	itoa( pi_wMinForceAttackFc[1], buf, 10 );
	m_uiUpgradeForceAttFc[0].SetString( buf );

	itoa( pi_wMaxForceAttackFc[1], buf, 10 );
	m_uiUpgradeForceAttFc[1].SetString( buf );
	
	// repos text	
	m_uiUpgradeForceAttFc[0].SetPos( m_uiCharInfo[FcAttPower].m_ptPos );
	m_uiUpgradeForceAttFc[1].SetPos( m_uiCharInfo[FcAttPower].m_ptPos.x + m_uiCharInfo[FcAttPower].m_ptSize.x,
								   m_uiCharInfo[FcAttPower].m_ptPos.y );
}

void
CGUICharInfoWindow::SetDeffenceFc2( WORD pi_wDefFc[2] )
{
	if( pi_wDefFc[0] < pi_wDefFc[1] )
		m_uiCharInfo[Deffense].GetFont()->dwColor = UPGRADE_COLOR;
	else 
		m_uiCharInfo[Deffense].GetFont()->dwColor = INFO_COLOR;

	char buf[16];
	sprintf( buf, "%d", pi_wDefFc[1] );

	m_uiCharInfo[Deffense].SetString( buf );
}

void
CGUICharInfoWindow::SetBattleMastery( BYTE pi_byMasteryIndex, WORD pi_wMastery )
{
	if( pi_byMasteryIndex > DEFFENCE_MASTERY )
		return;

	m_byBattlePoint[pi_byMasteryIndex] = pi_wMastery;	

	if( pi_wMastery >= 99 )
	{
		m_uiBattlePoint[pi_byMasteryIndex].SetString( "G M" );		

		m_uiBattlePoint[pi_byMasteryIndex].SetPos( 
				m_uiInfoBoard[TAB_SKILL].m_ptPos.x + m_uiInfoBoard[TAB_SKILL].m_ptSize.x - 
					m_uiBattlePoint[pi_byMasteryIndex].m_ptSize.x - 22,
				m_uiBattlePoint[pi_byMasteryIndex].m_ptPos.y );
	}
	else
	{
		char buf[16];
		sprintf( buf, "%d Point", pi_wMastery );

		m_uiBattlePoint[pi_byMasteryIndex].SetString( buf );

		m_uiBattlePoint[pi_byMasteryIndex].SetPos( 
				m_uiInfoBoard[TAB_SKILL].m_ptPos.x + m_uiInfoBoard[TAB_SKILL].m_ptSize.x - 
					m_uiBattlePoint[pi_byMasteryIndex].m_ptSize.x - 7,
				m_uiBattlePoint[pi_byMasteryIndex].m_ptPos.y );
	}	
}

void
CGUICharInfoWindow::SetBattleMasteryExp( BYTE pi_byMasteryIndex, float pi_fExp )
{
	if( pi_byMasteryIndex > DEFFENCE_MASTERY )
		return;

	if( pi_fExp < 0.f || pi_fExp > 1.f )
		return;	

	POINT l_ptTileSize;
	m_uiBattleGauge[pi_byMasteryIndex].GetTileSize( l_ptTileSize );	

	
	int MAX_GAUGE_BAR_NUM = 10;		// 막대 갯수

	int l_nSizeX = (int)( MAX_GAUGE_BAR_NUM * pi_fExp ) *
						( l_ptTileSize.x + m_uiBattleGauge[pi_byMasteryIndex].GetGapW() ) +
						m_uiBattleGauge[pi_byMasteryIndex].GetGapW();					

	m_uiBattleGauge[pi_byMasteryIndex].SetSize(	l_nSizeX, l_ptTileSize.y );
}

void
CGUICharInfoWindow::SetMakeMastery( BYTE pi_byMakeIndex, WORD pi_wMastery )
{
	if( pi_byMakeIndex > BULLET_MAKE )
		return;

	if( pi_wMastery >= 99 )
	{
		m_uiMakePoint[pi_byMakeIndex].SetString( "G M" );		

		m_uiMakePoint[pi_byMakeIndex].SetPos( 
				m_uiInfoBoard[TAB_SKILL].m_ptPos.x + m_uiInfoBoard[TAB_SKILL].m_ptSize.x - 
					m_uiMakePoint[pi_byMakeIndex].m_ptSize.x - 22,
				m_uiMakePoint[pi_byMakeIndex].m_ptPos.y );
	}
	else
	{
		char buf[16];
		sprintf( buf, "%d Point", pi_wMastery );

		m_uiMakePoint[pi_byMakeIndex].SetString( buf );

		m_uiMakePoint[pi_byMakeIndex].SetPos( 
				m_uiInfoBoard[TAB_SKILL].m_ptPos.x + m_uiInfoBoard[TAB_SKILL].m_ptSize.x - 
					m_uiMakePoint[pi_byMakeIndex].m_ptSize.x - 7,
				m_uiMakePoint[pi_byMakeIndex].m_ptPos.y );
	}
}

void
CGUICharInfoWindow::SetMakeMasteryExp( BYTE pi_byMakeIndex, float pi_fExp )
{
	if( pi_byMakeIndex > BULLET_MAKE )
		return;

	if( pi_fExp < 0.f || pi_fExp > 1.f )
		return;
	

	POINT l_ptTileSize;
	m_uiMakeGauge[pi_byMakeIndex].GetTileSize( l_ptTileSize );	

	int MAX_GAUGE_BAR_NUM = 10;

	int l_nSizeX = (int)( MAX_GAUGE_BAR_NUM * pi_fExp ) *
						( l_ptTileSize.x + m_uiMakeGauge[pi_byMakeIndex].GetGapW() ) +
						m_uiMakeGauge[pi_byMakeIndex].GetGapW();					

	m_uiMakeGauge[pi_byMakeIndex].SetSize(	l_nSizeX, l_ptTileSize.y );
}

void
CGUICharInfoWindow::SetResistPoint( BYTE pi_byResistIndex, WORD pi_wResistPoint )
{
	if( pi_byResistIndex > WIND_RESIST )
		return;

	char RESIST_STR[4][32] = { "파이어 내성", "아쿠아 내성", "테  라 내성", "윈  드 내성" };
	char buf[128];
	sprintf( buf, "%s        %3d Point", RESIST_STR[pi_byResistIndex], pi_wResistPoint );

	m_uiResistPoint[pi_byResistIndex].SetString( buf );
}


void
CGUICharInfoWindow::SetRaceType( BYTE pi_byRaceType )
{
	m_byRaceType = pi_byRaceType;

	char RACE_TYPE[3][20] = { "벨라토 연방", "신성동맹 코라", "아크레시아 제국" };
	char ZENDER_TYPE[2][12] = { "남", "여" };
	char BATTLE_NAME[3][16] = { "유닛  기술", "소환  기술", "런처  기술" };
	
	SetRace( RACE_TYPE[pi_byRaceType/2] );
	SetSex( ZENDER_TYPE[pi_byRaceType%2] );	

	m_uiBattleName[RACE_MASTERY].SetString( BATTLE_NAME[pi_byRaceType/2] );

	// 아크레시아이면 포스기술을 안보이게 한다.
	if( pi_byRaceType/2 == RT_ACCRETIA )
	{
		// resize board 
		m_uiInfoBoard[TAB_SKILL].SetSize( 204, 196 );

		// set sprite
		m_uiInfoBoard[TAB_SKILL].GetSpriteInfo()->wFrameNo = 3;

		// remove force skill
		m_uiInfoBoard[TAB_SKILL].Remove( &m_uiBattleName[FORCE_MASTERY] );
		m_uiInfoBoard[TAB_SKILL].Remove( &m_uiBattlePoint[FORCE_MASTERY] );
		m_uiInfoBoard[TAB_SKILL].Remove( &m_uiBattleGauge[FORCE_MASTERY] );		

		// 포스기술 아래쪽을 위로 다 올린다
		for( int i=FORCE_MASTERY+1; i<DEFFENCE_MASTERY+1; ++i )
		{
			m_uiBattleName[i].SetPos( m_uiInfoBoard[TAB_SKILL].m_ptPos.x + 5, 
									  m_uiInfoBoard[TAB_SKILL].m_ptPos.y + 24 + (i-1) * 18 ); 

			m_uiBattlePoint[i].SetPos( m_uiInfoBoard[TAB_SKILL].m_ptPos.x,
									   m_uiBattleName[i].m_ptPos.y );

			m_uiBattleGauge[i].SetPos( m_uiInfoBoard[TAB_SKILL].m_ptPos.x + 66,
									   m_uiInfoBoard[TAB_SKILL].m_ptPos.y + 27 + (i-1) * 18 );
		}
		
		m_uiMakeTitle.SetPos( m_uiInfoBoard[TAB_SKILL].m_ptPos.x + 70,
								m_uiInfoBoard[TAB_SKILL].m_ptPos.y + 121 );				
		for( i=0; i<BULLET_MAKE+1; ++i )
		{
			m_uiMakeName[i].SetPos( m_uiInfoBoard[TAB_SKILL].m_ptPos.x + 5, 
									m_uiInfoBoard[TAB_SKILL].m_ptPos.y + 138 + i * 18 ); 

			m_uiMakePoint[i].SetPos( m_uiInfoBoard[TAB_SKILL].m_ptPos.x,
									 m_uiMakeName[i].m_ptPos.y );

			m_uiMakeGauge[i].SetPos( m_uiInfoBoard[TAB_SKILL].m_ptPos.x + 66,
									 m_uiInfoBoard[TAB_SKILL].m_ptPos.y + 141 + i * 18 );	
		}
	}

	// init
	SetBattleMastery( RACE_MASTERY, 0 );
}
