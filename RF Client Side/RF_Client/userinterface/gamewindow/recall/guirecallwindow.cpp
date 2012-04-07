#include "GUIRecallWindow.h"
#include "../../SpriteMgr.h"
#include "../../GUIController/GUIScreen.h"

////////////////////////////////////////////////////////////////////////////
//
// CGUIRecallUnit Class Implementation
//
////////////////////////////////////////////////////////////////////////////
CGUIRecallUnit::CGUIRecallUnit()
{
	SetSize( 208, 56 );

	m_cLink.SetSize( 11, 11 );

	m_cGagugeBase.SetSize( 162, 29 );
	m_cHPGauge.SetSize( 0, 8 );
	m_cFPGauge.SetSize( 0, 8 );
	m_cExpGauge.SetSize( 0, 3 );

	m_cHPSlash.SetSize( 7, 9 );
	m_cFPSlash.SetSize( 7, 9 );
	m_cExpPercent.SetSize( 7, 9 );

	m_cCover.SetSize( 208, 56 );
	m_cUnderline.SetSize( 208, 1 );



	m_cMobName.SetPos( m_ptPos.x + 42, m_ptPos.y + 5 );
	m_cLevel.SetPos( m_ptPos.x + 165, m_ptPos.y + 5 );
	m_cItem.SetPos( m_ptPos.x + 6, m_ptPos.y + 12 );

	m_cGagugeBase.SetPos( m_ptPos.x + 6, m_ptPos.y + 18 );

	m_cGagugeBase.SetPos( m_ptPos.x + 40, m_ptPos.y + 21 );
	m_cHPGauge.SetPos( m_cGagugeBase.m_ptPos.x + 22, m_cGagugeBase.m_ptPos.y );
	m_cFPGauge.SetPos( m_cGagugeBase.m_ptPos.x + 22, m_cGagugeBase.m_ptPos.y + 11 );
	m_cExpGauge.SetPos( m_cGagugeBase.m_ptPos.x + 22, m_cGagugeBase.m_ptPos.y + 24 );

	m_cCurHP.SetPos( m_cGagugeBase.m_ptPos.x + 89, m_cHPGauge.m_ptPos.y );
	m_cHPSlash.SetPos( m_cGagugeBase.m_ptPos.x + 89, m_cHPGauge.m_ptPos.y );
	m_cMaxHP.SetPos( m_cGagugeBase.m_ptPos.x + 89, m_cHPGauge.m_ptPos.y );

	m_cCurFP.SetPos( m_cGagugeBase.m_ptPos.x + 89, m_cFPGauge.m_ptPos.y );
	m_cFPSlash.SetPos( m_cGagugeBase.m_ptPos.x + 89, m_cFPGauge.m_ptPos.y );
	m_cMaxFP.SetPos( m_cGagugeBase.m_ptPos.x + 89, m_cFPGauge.m_ptPos.y );

	m_cExp.SetPos( m_ptPos.x + 82, m_cExpGauge.m_ptPos.y - 3 );
	m_cExpPercent.SetPos( m_ptPos.x + 82, m_cExp.m_ptPos.y );

	m_cUnderline.SetPos( m_ptPos.x, m_ptPos.y + m_ptSize.y - 1 );
	m_cCover.SetPos( m_ptPos );
	


	Add( &m_cMobName );
	Add( &m_cLevel );
	Add( &m_cItem );

	Add( &m_cGagugeBase );
	Add( &m_cHPGauge );
	Add( &m_cFPGauge );
	Add( &m_cExpGauge );

	Add( &m_cCurHP );	
	Add( &m_cHPSlash );
	Add( &m_cMaxHP );	

	Add( &m_cCurFP );	
	Add( &m_cFPSlash );
	Add( &m_cMaxFP );
	
	Add( &m_cExpBase );
	Add( &m_cExp );
	Add( &m_cExpPercent );

	Add( &m_cUnderline );

	Add( &m_cCover );

	m_cUnderline.SetARGB( 0xa0000000 );
	m_cMobName.GetFont()->dwColor = 0xFFFFFFFF;
	m_cLevel.GetFont()->dwColor = D3DCOLOR_XRGB( 255, 184, 71 );

	m_cExpBase.SetARGB( D3DCOLOR_ARGB( 204, 22, 28, 28 ) );

	m_cExp.SetNumberColor( 0xFFFFFFC8 );
	

	m_bLoadedSprite = FALSE;
}

CGUIRecallUnit::~CGUIRecallUnit()
{

}

void
CGUIRecallUnit::SetSprite( void )
{
	if( !m_bLoadedSprite )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_RECALL );

		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 0;
		m_cGagugeBase.SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 1;
		m_cHPGauge.SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 2;
		m_cFPGauge.SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 3;
		m_cExpGauge.SetSpriteInfo( &l_spriteInfo );


		m_cHPGauge.SetSpriteV( 0.f, 1.f );
		m_cFPGauge.SetSpriteV( 0.f, 1.f );
		m_cExpGauge.SetSpriteV( 0.f, 1.f );


		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_GAUGE_EXP );
		l_spriteInfo.wActionNo = 0;

		POINT l_ptNumSize = { 7, 9 };
		l_spriteInfo.wFrameNo = 6;
		m_cCurHP.SetNumberSprite( &l_spriteInfo, l_ptNumSize );
		m_cMaxHP.SetNumberSprite( &l_spriteInfo, l_ptNumSize );
		m_cCurFP.SetNumberSprite( &l_spriteInfo, l_ptNumSize );
		m_cMaxFP.SetNumberSprite( &l_spriteInfo, l_ptNumSize );
		m_cExp.SetNumberSprite( &l_spriteInfo, l_ptNumSize );
		
		l_spriteInfo.wFrameNo = 8;
		m_cHPSlash.SetSpriteInfo( &l_spriteInfo );
		m_cFPSlash.SetSpriteInfo( &l_spriteInfo );
		
		l_spriteInfo.wFrameNo = 10;
		m_cExpPercent.SetSpriteInfo( &l_spriteInfo );


		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
		l_spriteInfo.wActionNo = 7;
		l_spriteInfo.wFrameNo = 5;
		m_cLink.SetSpriteInfo( &l_spriteInfo );


		m_bLoadedSprite = TRUE;
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_RECALL );
		if( !l_pSprite )
			return;					
		
		m_cGagugeBase.GetSpriteInfo()->pSprite = l_pSprite;

		m_cHPGauge.GetSpriteInfo()->pSprite = l_pSprite;
		m_cFPGauge.GetSpriteInfo()->pSprite = l_pSprite;
		m_cExpGauge.GetSpriteInfo()->pSprite = l_pSprite;

		m_cItem.GetSpriteInfo()->pSprite = gSpriteMgr.GetSprite( SP_ID_ITEM );


		l_pSprite = gSpriteMgr.GetSprite( SP_ID_GAUGE_EXP );

		m_cCurHP.GetSpriteInfo()->pSprite = l_pSprite;
		m_cMaxHP.GetSpriteInfo()->pSprite = l_pSprite;
		m_cCurFP.GetSpriteInfo()->pSprite = l_pSprite;
		m_cMaxFP.GetSpriteInfo()->pSprite = l_pSprite;
		m_cExp.GetSpriteInfo()->pSprite = l_pSprite;

		m_cHPSlash.GetSpriteInfo()->pSprite = l_pSprite;
		m_cFPSlash.GetSpriteInfo()->pSprite = l_pSprite;		
		
		m_cExpPercent.GetSpriteInfo()->pSprite = l_pSprite;

		m_cLink.GetSpriteInfo()->pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
	}
}

void
CGUIRecallUnit::SetUnitState( UnitState pi_eState )
{
	if( pi_eState == m_eUnitState )
		return;

	if( m_eUnitState == Linked )	
		Remove( &m_cLink );
	
	m_eUnitState = pi_eState;
	if( m_eUnitState == Enabled )
	{
		m_cCover.SetARGB( 0xFFFFFFFF );
	}
	else if( m_eUnitState == Linked )
	{
		m_cCover.SetARGB( 0xFFFFFFFF );

		m_cLink.Show( IsShow() );
		m_cLink.SetPos( m_cItem.m_ptPos.x,
						m_cItem.m_ptPos.y + m_cItem.m_ptSize.y - m_cLink.m_ptSize.y );		
		Add( &m_cLink );
	}
	else if( m_eUnitState == Disabled )
	{
		m_cCover.SetARGB( 0x80A05050 );
	}
	else if( m_eUnitState == NoneUnit )
	{
		m_cCover.SetARGB( 0xB0000000 );
	}
}

void
CGUIRecallUnit::SetMobName( char * pi_pName )
{
	m_cMobName.SetString( pi_pName );
}

void
CGUIRecallUnit::SetLevel( BYTE pi_byLevel )
{
	char buf[16];
	sprintf( buf, "Lv %d", pi_byLevel );

	m_cLevel.SetString( buf );
}

void
CGUIRecallUnit::SetItem( DWORD pi_dwItemID, DWORD pi_dwSpriteID )
{
	m_cItem.SetItemID( pi_dwItemID );
	m_cItem.SetSpriteID( pi_dwSpriteID );
	m_cItem.SetIconType( RECALL_ICON );

	m_cItem.SetSprite();
}

void
CGUIRecallUnit::SetHP( WORD pi_wCurHP, WORD pi_wMaxHP )
{
	m_cCurHP.SetNumber( pi_wCurHP );	
	m_cCurHP.SetPos( m_cHPSlash.m_ptPos.x - m_cCurHP.m_ptSize.x - 2, m_cCurHP.m_ptPos.y );

	m_cMaxHP.SetNumber( pi_wMaxHP );
	m_cMaxHP.SetPos( m_cHPSlash.m_ptPos.x + m_cHPSlash.m_ptSize.x + 2, m_cMaxHP.m_ptPos.y );

	float l_fRate = (float)pi_wCurHP / pi_wMaxHP;
	m_cHPGauge.SetSize( 140 * l_fRate, m_cHPGauge.m_ptSize.y );
	m_cHPGauge.SetSpriteU( 0.f, l_fRate );
}

void
CGUIRecallUnit::SetFP( WORD pi_wCurFP, WORD pi_wMaxFP )
{
	m_cCurFP.SetNumber( pi_wCurFP );	
	m_cCurFP.SetPos( m_cFPSlash.m_ptPos.x - m_cCurFP.m_ptSize.x - 2, m_cCurFP.m_ptPos.y );

	m_cMaxFP.SetNumber( pi_wMaxFP );
	m_cMaxFP.SetPos( m_cFPSlash.m_ptPos.x + m_cFPSlash.m_ptSize.x + 2, m_cMaxFP.m_ptPos.y );

	float l_fRate = (float)pi_wCurFP / pi_wMaxFP;
	m_cFPGauge.SetSize( 140 * l_fRate, m_cFPGauge.m_ptSize.y );
	m_cFPGauge.SetSpriteU( 0.f, l_fRate );
}

void
CGUIRecallUnit::SetExp( float pi_fExpPercent )
{
	if( pi_fExpPercent < 100.f )
		m_cExp.SetNumber( pi_fExpPercent, 2 );
	else
		m_cExp.SetNumber( 99.99, 2 );

	m_cExp.SetPos( m_cExpGauge.m_ptPos.x + ( 140 - m_cExp.m_ptSize.x - m_cExpPercent.m_ptSize.x ) / 2, m_cExp.m_ptPos.y );
	m_cExpPercent.SetPos( m_cExp.m_ptPos.x + m_cExp.m_ptSize.x, m_cExpPercent.m_ptPos.y );

	float l_fExpRate = pi_fExpPercent / 100.f;

	m_cExpGauge.SetSize( 140 * l_fExpRate, m_cExpGauge.m_ptSize.y );	
	m_cExpGauge.SetSpriteU( 0.f, l_fExpRate );

	m_cExpBase.SetSize( m_cExp.m_ptSize.x + m_cExpPercent.m_ptSize.x, m_cExp.m_ptSize.y );
	m_cExpBase.SetPos( m_cExp.m_ptPos );
}

float
CGUIRecallUnit::GetExpRate( void )
{
	float * pExpRate = m_cExpGauge.GetU();

	return pExpRate[1];
}

////////////////////////////////////////////////////////////////////////////
//
// CGUIRecallWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////

#define	TITLE_COLOR			0xFFC3C1BE

CGUIRecallWindow::CGUIRecallWindow()
{	
	m_bIsCreated			= FALSE;
}

CGUIRecallWindow::~CGUIRecallWindow()
{
	m_uiUnitBoard.RemoveAll();

	CGUIRecallList::iterator it;		
	for( it = m_listRecallUnit.begin(); it != m_listRecallUnit.end(); ++it )
	{
		delete *it;
	}
	m_listRecallUnit.clear();	
}

void
CGUIRecallWindow::Init( void )
{
	SetSize( 228, 395 );

	m_uiAniTitleBar.SetSize( 228, 28 );	
	m_uiCloseButton.SetSize( 16, 13 );	
	m_uiHelpButton.SetSize( 16, 13 );
	
	m_uiResizeUnderBar.SetSize( 228, 17 );	

	m_uiLOutline.SetSize( 1, m_ptSize.y - m_uiAniTitleBar.m_ptSize.y - m_uiResizeUnderBar.m_ptSize.y );
	m_uiROutline.SetSize( m_uiLOutline.m_ptSize );

	m_uiBoard.SetSize( m_ptSize.x, m_ptSize.y - m_uiAniTitleBar.m_ptSize.y - m_uiResizeUnderBar.m_ptSize.y );	
	m_uiUnitBoard.SetSize( m_ptSize.x - 6, m_ptSize.y - m_uiAniTitleBar.m_ptSize.y - m_uiResizeUnderBar.m_ptSize.y );	
	
	
	POINT	l_ptSize;
	// scrollbar	
	// scrollbar size
	l_ptSize.x = 15;	l_ptSize.y = 10;
	m_uiScrollbar.SetTopArrowSize( l_ptSize );
	m_uiScrollbar.SetBottomArrowSize( l_ptSize );	
	l_ptSize.x = 15;	l_ptSize.y = 18;
	m_uiScrollbar.SetBarSize( l_ptSize );	
	l_ptSize.x = 15;
	l_ptSize.y = m_uiUnitBoard.m_ptSize.y;
	m_uiScrollbar.SetSize( l_ptSize );	

	m_uiTitle.SetString( "소환" );
	
	// ------------------------------------------------------------------
	m_uiTitle.SetPos( m_ptPos.x + 6, m_ptPos.y + 7 );
	m_uiAniTitleBar.SetPos( m_ptPos.x, m_ptPos.y );
	m_uiCloseButton.SetPos( m_ptPos.x + 211, m_ptPos.y + 4 );
	m_uiHelpButton.SetPos( m_ptPos.x + 194, m_ptPos.y + 4 );

	m_uiBoard.SetPos( m_ptPos.x, m_uiAniTitleBar.m_ptPos.y + m_uiAniTitleBar.m_ptSize.y );

	m_uiUnitBoard.SetPos( m_ptPos.x + 3, m_uiAniTitleBar.m_ptPos.y + m_uiAniTitleBar.m_ptSize.y );
	m_uiScrollbar.SetPos( m_ptPos.x + m_ptSize.x - m_uiScrollbar.m_ptSize.x - 2, m_uiUnitBoard.m_ptPos.y );	

	m_uiLOutline.SetPos( m_ptPos.x + 2, m_uiAniTitleBar.m_ptPos.y + m_uiAniTitleBar.m_ptSize.y );
	m_uiROutline.SetPos( m_ptPos.x + m_ptSize.x - 2 - m_uiROutline.m_ptSize.x, m_uiLOutline.m_ptPos.y );

	m_uiResizeUnderBar.SetPos( m_ptPos.x, m_ptPos.y + m_ptSize.y - m_uiResizeUnderBar.m_ptSize.y );

	// ------------------------------------------------------------------
	Add( &m_uiAniTitleBar );
	Add( &m_uiTitle );

	m_uiCloseButton.AddMouseEventHandler( this );
	m_uiHelpButton.AddMouseEventHandler( this );
	Add( &m_uiCloseButton );
	Add( &m_uiHelpButton );

	m_uiBoard.AddMouseEventHandler( this );
	Add( &m_uiBoard );

	m_uiBoard.Add( &m_uiUnitBoard );
	m_uiScrollbar.AddScrollHandler( this );
	Add( &m_uiScrollbar );

	Add( &m_uiLOutline );
	Add( &m_uiROutline );	

	m_uiResizeUnderBar.AddMouseEventHandler( this );	
	Add( &m_uiResizeUnderBar );	
	

	// ------------------------------------------------------------------
	SetSprite();

	m_uiTitle.GetFont()->dwColor = TITLE_COLOR;
	m_uiBoard.SetARGB( D3DCOLOR_ARGB( 204, 22, 28, 28 ) );

	m_uiLOutline.SetARGB( D3DCOLOR_XRGB( 175, 180, 186 ) );
	m_uiROutline.SetARGB( D3DCOLOR_XRGB( 175, 180, 186 ) );
	
	// ------------------------------------------------------------------
	m_uiAniTitleBar.SetTotalFrameNum( 18 );
	m_uiAniTitleBar.SetObjectNumInLine( 1 );
	m_uiAniTitleBar.SetFPS( 10 );
	m_uiAniTitleBar.SetStopInterval( 1000 );

	m_uiScrollbar.SetAdjustmentUnit( 56 );
	
	// Set Resize Object;
	SetReSizeObject( CGUIWindow::BOTTOM, &m_uiResizeUnderBar );
	SetReSizable( true );

	SetMoveObject( &m_uiAniTitleBar );	
	SetMoveObject( &m_uiTitle );			
	SetMovable( true );

	SetWheelScrollObject( &m_uiScrollbar );

	SoundOn( TRUE );

	SetName( "recallWindow2" );

	m_bIsCreated = TRUE;
}

void
CGUIRecallWindow::SetSprite( void )
{
	if( !m_bIsCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );

		// ani title bar	
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 5;
		m_uiAniTitleBar.SetSpriteInfo( &l_spriteInfo );		

		// resize bar
		l_spriteInfo.wActionNo = 7;
		l_spriteInfo.wFrameNo = 2;
		m_uiResizeUnderBar.SetSpriteInfo( &l_spriteInfo );
		
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

		// scrollbar
		SPRITE_INFO l_sScrollbarSprite[2];
		l_sScrollbarSprite[0].pSprite = l_spriteInfo.pSprite;
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
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
		if( !l_pSprite )
			return;	
		
		// ani title bar	
		m_uiAniTitleBar.GetSpriteInfo()->pSprite = l_pSprite;		

		m_uiResizeUnderBar.GetSpriteInfo()->pSprite = l_pSprite;

		// close button	
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );		 
		m_uiCloseButton.SetSprite( l_pSprite );		
		// help button
		m_uiHelpButton.SetSprite( l_pSprite );

		m_uiScrollbar.SetSprite( l_pSprite );

		CGUIRecallList::iterator it;
		for( it = m_listRecallUnit.begin(); it != m_listRecallUnit.end(); ++it )
		{
			(*it)->SetSprite();
		}
	}
}

void
CGUIRecallWindow::SetBound( RECT & pi_rtBound )
{
	if( pi_rtBound.bottom - pi_rtBound.top < m_uiAniTitleBar.m_ptSize.y + m_uiResizeUnderBar.m_ptSize.y + 137 )
		return;	
	
	SetPos( pi_rtBound.left, pi_rtBound.top );
	SetSize( pi_rtBound.right - pi_rtBound.left, pi_rtBound.bottom - pi_rtBound.top );

	m_uiBoard.SetSize( m_uiBoard.m_ptSize.x, m_ptSize.y - m_uiAniTitleBar.m_ptSize.y - m_uiResizeUnderBar.m_ptSize.y );	

	RECT l_rtBound;
	l_rtBound.left = m_ptPos.x + m_ptSize.x - m_uiScrollbar.m_ptSize.x - 2;
	l_rtBound.right = l_rtBound.left + m_uiScrollbar.m_ptSize.x;
	l_rtBound.top = m_uiBoard.m_ptPos.y;
	l_rtBound.bottom = m_uiBoard.m_ptPos.y + m_uiBoard.m_ptSize.y;
	m_uiScrollbar.SetBound( l_rtBound );	
	UpdateScrollbar();	

	m_uiLOutline.SetSize( m_uiLOutline.m_ptSize.x, m_uiBoard.m_ptSize.y );
	m_uiROutline.SetSize( m_uiLOutline.m_ptSize );	

	// repos underbar
	m_uiResizeUnderBar.SetPos( m_ptPos.x, m_ptPos.y + m_ptSize.y - m_uiResizeUnderBar.m_ptSize.y );	
}

BOOL
CGUIRecallWindow::TakeInItem( _recall_info * pi_pRecallInfo )
{	
	if( GetRecallUnitByIndex( pi_pRecallInfo->dwIndex ) )
		return FALSE;

	CGUIRecallUnit * l_pNewUnit;

	l_pNewUnit = new CGUIRecallUnit;

	l_pNewUnit->SetSprite();

	l_pNewUnit->SetIndex( pi_pRecallInfo->dwIndex );
	l_pNewUnit->SetMobName( pi_pRecallInfo->pName );

	l_pNewUnit->SetItem( pi_pRecallInfo->dwItemID, pi_pRecallInfo->dwSpriteID );
	
	l_pNewUnit->SetLevel( pi_pRecallInfo->byLevel );
	
	l_pNewUnit->SetHP( pi_pRecallInfo->nCurHP, pi_pRecallInfo->nMaxHP );	
	l_pNewUnit->SetFP( pi_pRecallInfo->nCurFP, pi_pRecallInfo->nMaxFP );
	l_pNewUnit->SetExp( pi_pRecallInfo->fExp );	

	// add mouse event
	l_pNewUnit->GetCover()->AddMouseEventHandler( this );

	l_pNewUnit->SetPos( m_uiUnitBoard.m_ptPos.x, m_uiUnitBoard.m_ptPos.y + m_listRecallUnit.size() * l_pNewUnit->m_ptSize.y );	
	l_pNewUnit->Show( IsShow() );
	m_uiUnitBoard.Add( l_pNewUnit );	

	m_listRecallUnit.push_back( l_pNewUnit );

	m_uiUnitBoard.SetSize( m_uiUnitBoard.m_ptSize.x, m_listRecallUnit.size() * l_pNewUnit->m_ptSize.y );

	UpdateScrollbar();	
	
	return TRUE;
}

BOOL	
CGUIRecallWindow::TakeOutItem( DWORD pi_dwIndex )
{
	CGUIRecallUnit * l_pRecallUnit;
	
	l_pRecallUnit = GetRecallUnitByIndex( pi_dwIndex );
	if( l_pRecallUnit )
	{
		m_uiUnitBoard.Remove( l_pRecallUnit );
		m_listRecallUnit.remove( l_pRecallUnit );		

		delete l_pRecallUnit;

		// repos recall unit
		CGUIRecallList::iterator it;
		int index = 0;
		for( it = m_listRecallUnit.begin(); it != m_listRecallUnit.end(); ++it, ++index )
		{
			(*it)->SetPos( m_uiUnitBoard.m_ptPos.x, m_uiUnitBoard.m_ptPos.y + (*it)->m_ptSize.y * index );				
		}

		UpdateScrollbar();

		return TRUE;
	}

	return FALSE;
}

BOOL
CGUIRecallWindow::TakeOutLink( DWORD pi_dwIndex, BOOL pi_bByIndex )
{
	// get item
	CGUIRecallUnit * l_pUnit;
	if( pi_bByIndex )
		l_pUnit = GetRecallUnitByIndex( pi_dwIndex );
	else
		l_pUnit = GetRecallUnitByItemID( pi_dwIndex );

	if( !l_pUnit )
		return FALSE;

	// add item to mouse pointer
	gScreen.AddObject( l_pUnit->GetItem() );
	gScreen.GetObject()->SetRefItem( l_pUnit->GetItem() );
	gScreen.ResizeObject( 40, 40 );	

	// set unit link
	l_pUnit->SetUnitState( CGUIRecallUnit::Linked );	

	return TRUE;	
}

void
CGUIRecallWindow::SetLevel( DWORD pi_dwIndex, BYTE pi_byLevel, BOOL pi_bByIndex )
{
	CGUIRecallUnit * l_pUnit;
	if( pi_bByIndex )
		l_pUnit = GetRecallUnitByIndex( pi_dwIndex );
	else
		l_pUnit = GetRecallUnitByItemID( pi_dwIndex );

	if( !l_pUnit )
		return;
		
	l_pUnit->SetLevel( pi_byLevel );
}

void
CGUIRecallWindow::SetHP( DWORD pi_dwIndex, int pi_nCurHP, int pi_nMaxHP, BOOL pi_bByIndex )
{	
	CGUIRecallUnit * l_pUnit;
	if( pi_bByIndex )
		l_pUnit = GetRecallUnitByIndex( pi_dwIndex );
	else
		l_pUnit = GetRecallUnitByItemID( pi_dwIndex );

	if( !l_pUnit )
		return;
		
	l_pUnit->SetHP( pi_nCurHP, pi_nMaxHP );
}

void
CGUIRecallWindow::SetFP( DWORD pi_dwIndex, int pi_nCurFP, int pi_nMaxFP, BOOL pi_bByIndex )
{	
	CGUIRecallUnit * l_pUnit;
	if( pi_bByIndex )
		l_pUnit = GetRecallUnitByIndex( pi_dwIndex );
	else
		l_pUnit = GetRecallUnitByItemID( pi_dwIndex );

	if( !l_pUnit )
		return;
		
	l_pUnit->SetFP( pi_nCurFP, pi_nMaxFP );
}

void	
CGUIRecallWindow::SetExp( DWORD pi_dwIndex, float pi_fExp, BOOL pi_bByIndex )
{
	CGUIRecallUnit * l_pUnit;
	if( pi_bByIndex )
		l_pUnit = GetRecallUnitByIndex( pi_dwIndex );
	else
		l_pUnit = GetRecallUnitByItemID( pi_dwIndex );

	if( !l_pUnit )
		return;
		
	l_pUnit->SetExp( pi_fExp );
}

void
CGUIRecallWindow::SetItemState( DWORD pi_dwIndex, CGUIRecallUnit::UnitState pi_eState, BOOL pi_bByIndex )
{
	CGUIRecallUnit * l_pUnit;
	if( pi_bByIndex )
		l_pUnit = GetRecallUnitByIndex( pi_dwIndex );
	else
		l_pUnit = GetRecallUnitByItemID( pi_dwIndex );

	if( !l_pUnit )
		return;
		
	l_pUnit->SetUnitState( pi_eState );
}

void
CGUIRecallWindow::UpdateScrollbar( void )
{
	int l_nUnitSize;
	if( !m_listRecallUnit.empty() )
		l_nUnitSize = (*m_listRecallUnit.begin())->m_ptSize.y;
	else
		l_nUnitSize = 0;

	int l_nTotalAmount;
	int l_nCurAmount;
	l_nTotalAmount = m_uiBoard.m_ptPos.y - m_uiUnitBoard.m_ptPos.y;
	if( m_uiUnitBoard.m_ptPos.y + m_uiUnitBoard.m_ptSize.y > m_uiBoard.m_ptPos.y + m_uiBoard.m_ptSize.y )
		l_nTotalAmount += m_uiUnitBoard.m_ptPos.y + m_uiUnitBoard.m_ptSize.y - (m_uiBoard.m_ptPos.y + m_uiBoard.m_ptSize.y);

	// set amount
	if( m_uiScrollbar.GetCurrentAmount() > l_nTotalAmount )
		l_nCurAmount = l_nTotalAmount;
	else
		l_nCurAmount = m_uiScrollbar.GetCurrentAmount();

	m_uiScrollbar.SetAmount( l_nTotalAmount, l_nCurAmount );
}

CGUIRecallUnit *
CGUIRecallWindow::GetRecallUnitByIndex( DWORD pi_dwIndex )
{
	CGUIRecallList::iterator it;
	for( it = m_listRecallUnit.begin(); it != m_listRecallUnit.end(); ++it )
	{
		if( (*it)->GetIndex() == pi_dwIndex )
			return *it;
	}

	return NULL;
}

CGUIRecallUnit *
CGUIRecallWindow::GetRecallUnitByItemID( DWORD pi_dwItemID )
{
	CGUIRecallList::iterator it;
	for( it = m_listRecallUnit.begin(); it != m_listRecallUnit.end(); ++it )
	{
		if( (*it)->GetItemID() == pi_dwItemID )
			return *it;
	}

	return NULL;
}

CGUIRecallUnit *
CGUIRecallWindow::GetRecallUnitByEventObj( CGUIObject * pi_pObj )
{
	CGUIRecallList::iterator it;
	for( it = m_listRecallUnit.begin(); it != m_listRecallUnit.end(); ++it )
	{
		if( (*it)->GetCover() == pi_pObj )
			return *it;
	}

	return NULL;
}

void
CGUIRecallWindow::MouseEvent( _mouse_event & event )
{
	CGUIWindow::MouseEvent( event );

	if( event.id == MOUSE_STROKED )
	{
		if( gScreen.ExistObject() )
		{
			CGUIItem *		pHoverItem		= static_cast< CGUIItem * >( gScreen.GetObject() );
			CGUIRecallUnit* pOrgIconBoard	= GetRecallUnitByItemID( pHoverItem->GetItemID() );
			if( !pOrgIconBoard )
				return;

			PostUIMessage( WM_BOARD_ADD_OBJECT, pOrgIconBoard->GetIndex() );
		}
		else
		{
			CGUIRecallUnit* pOrgIconBoard = GetRecallUnitByEventObj( event.source );
			if( !pOrgIconBoard )
				return;

			SetQuickEvent();

			if( g_bIsQuickEvent_Equip )
				PostUIMessage( WM_BOARD_EQUIP_HOT_KEY, pOrgIconBoard->GetIndex() );
			else
				PostUIMessage( WM_BOARD_REMOVE_OBJECT, pOrgIconBoard->GetIndex() );
		}
	}
	else if( event.id == LMOUSE_STROKED )
	{
		CGUIRecallUnit* pOrgIconBoard = GetRecallUnitByEventObj( event.source );
		if( !pOrgIconBoard )
			return;

		PostUIMessage( WM_BOARD_USE_OBJECT, pOrgIconBoard->GetIndex() );
	}
	else if( event.id == MOUSE_HOVERED )
	{
		if( gScreen.ExistObject() )
			return;

		CGUIRecallUnit* pOrgIconBoard = GetRecallUnitByEventObj( event.source );
		if( !pOrgIconBoard )
			return;

		PostUIMessage( WM_BOARD_HOVER_OBJECT, pOrgIconBoard->GetIndex() );
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
CGUIRecallWindow::ScrollEvent( _scroll_event & event )
{
	m_uiUnitBoard.SetPos( m_uiUnitBoard.m_ptPos.x,
					  m_uiBoard.m_ptPos.y - m_uiScrollbar.GetCurrentAmount() );
}