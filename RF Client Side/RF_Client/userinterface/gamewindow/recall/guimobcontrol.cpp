////////////////////////////////////////////////////////////////////////////
//
// CGUIMobControl Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "GUIMobControl.h"
#include "../../SpriteMgr.h"
#include "../../GUIController/GUITooltip.h"

void EP_OrderToMonster( BYTE pi_byOrderType );
void EP_ReleaseMonster( void );
void EP_ReleaseAllMonster( void );

////////////////////////////////////////////////////////////////////////////

CGUIMobControl::CGUIMobControl()
{
	m_bIsOpenedCmdBoard	= FALSE;

	m_bIsCreated		= FALSE;
}

CGUIMobControl::~CGUIMobControl()
{

}

void
CGUIMobControl::Init( void )
{
	SetSize( 185, 50 );
	m_uiBoard.SetSize( m_ptSize );

	m_uiHPGaugeSlash.SetSize( 7, 9 );
	m_uiFPGaugeSlash.SetSize( 7, 9 );

	m_uiMobCmdIcon.SetSize( 36, 34 );
	m_uiMobCmdBoard.SetSize( 43, 34 );

	for( int i=0; i<TOTAL_COMMAND_NUM; ++i )	
		m_uiCmdButton[i].SetSize( 36, 34 );	

	m_uiCmdBoard.SetSize( m_uiCmdButton[0].m_ptSize.x * TOTAL_COMMAND_NUM, m_uiCmdButton[0].m_ptSize.y );

	// --------------------------------------------------------------------------
	m_uiBoard.SetPos( m_ptPos );

	m_uiMobLevel.SetPos( m_ptPos.x + 6, m_ptPos.y + 4 );
	m_uiMobName.SetPos( m_ptPos.x + 35, m_ptPos.y + 4 );

	m_uiHPGauge.SetPos( m_ptPos.x + 9, m_ptPos.y + 16 );
	m_uiHPGaugeSlash.SetPos( m_uiHPGauge.m_ptPos.x + 62, m_uiHPGauge.m_ptPos.y + 1 );

	m_uiFPGauge.SetPos( m_ptPos.x + 9, m_ptPos.y + 32 );
	m_uiFPGaugeSlash.SetPos( m_uiFPGauge.m_ptPos.x + 62, m_uiFPGauge.m_ptPos.y + 1 );

	m_uiExpGauge.SetPos( m_ptPos.x + 74, m_ptPos.y + 8 );

	m_uiMobCmdBoard.SetPos( m_ptPos.x + 145, m_ptPos.y + 13 );
	m_uiMobCmdIcon.SetPos( m_ptPos.x + 145, m_ptPos.y + 13 );	
	
	m_uiCmdViewBaord.SetPos( m_ptPos.x + m_ptSize.x, m_uiMobCmdIcon.m_ptPos.y );

	m_uiCmdBoard.SetPos( m_uiCmdViewBaord.m_ptPos );
	for( i=0; i<TOTAL_COMMAND_NUM; ++i )
		m_uiCmdButton[i].SetPos( m_uiCmdBoard.m_ptPos.x + i * m_uiCmdButton[i].m_ptSize.x , m_uiCmdBoard.m_ptPos.y );


	// --------------------------------------------------------------------------
	Add( &m_uiBoard );

	Add( &m_uiMobLevel );
	Add( &m_uiMobName );

	Add( &m_uiHPGauge );
	Add( &m_uiCurHPGaugeNum );
	Add( &m_uiMaxHPGaugeNum );
	Add( &m_uiHPGaugeSlash );

	Add( &m_uiExpGauge );

	Add( &m_uiFPGauge );
	Add( &m_uiCurFPGaugeNum );
	Add( &m_uiMaxFPGaugeNum );
	Add( &m_uiFPGaugeSlash );

	Add( &m_uiMobCmdBoard );

	m_uiMobCmdIcon.AddMouseEventHandler( this );
	Add( &m_uiMobCmdIcon );


	for( i=0; i<TOTAL_COMMAND_NUM; ++i )
	{
		m_uiCmdButton[i].AddMouseEventHandler( this );
		m_uiCmdBoard.Add( &m_uiCmdButton[i] );
	}
	m_uiCmdViewBaord.Add( &m_uiCmdBoard );
	Add( &m_uiCmdViewBaord );


	SetSprite();

	SetARGB( 0xCCFFFFFF );

	m_uiMobLevel.GetFont()->dwColor = D3DCOLOR_XRGB( 224, 206, 169 );
	m_uiMobName.GetFont()->dwColor = D3DCOLOR_XRGB( 214, 212, 207 );

	SetMoveObject( &m_uiBoard );
	SetMoveObject( &m_uiMobName );
	SetMovable( true );	

	SetLevel( 1 );
	SetExp( 0.f );

	SetName( "MobControl" );

	m_bIsCreated = TRUE;
}

void
CGUIMobControl::SetSprite( void )
{
	if( !m_bIsCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_RECALL );

		l_spriteInfo.wActionNo = 1;

		l_spriteInfo.wFrameNo = 0;
		SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 1;
		m_uiBoard.SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wActionNo = 2;
		l_spriteInfo.wFrameNo = 1;
		m_uiMobCmdIcon.SetSpriteInfo( &l_spriteInfo );

		for( int i=0; i<TOTAL_COMMAND_NUM; ++i )
		{
			l_spriteInfo.wFrameNo = i;
			m_uiCmdButton[i].SetSpriteInfoEx( &l_spriteInfo );
		}

		
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_GAUGE_EXP );
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 2;
		m_uiHPGauge.SetSpriteInfo( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 3;
		m_uiFPGauge.SetSpriteInfo( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 5;
		m_uiExpGauge.SetSpriteInfo( &l_spriteInfo );


		POINT l_ptNumSize;
		l_ptNumSize.x = 7;
		l_ptNumSize.y = 9;		
		l_spriteInfo.wFrameNo = 6;
		m_uiCurHPGaugeNum.SetNumberSprite( &l_spriteInfo, l_ptNumSize );
		m_uiMaxHPGaugeNum.SetNumberSprite( &l_spriteInfo, l_ptNumSize );
		m_uiCurFPGaugeNum.SetNumberSprite( &l_spriteInfo, l_ptNumSize );
		m_uiMaxFPGaugeNum.SetNumberSprite( &l_spriteInfo, l_ptNumSize );

		l_spriteInfo.wFrameNo = 8;
		m_uiHPGaugeSlash.SetSpriteInfo( &l_spriteInfo );
		m_uiFPGaugeSlash.SetSpriteInfo( &l_spriteInfo );		


		m_uiHPGauge.SetSpriteV( 0.f, 1.f );
		m_uiFPGauge.SetSpriteV( 0.f, 1.f );
		m_uiExpGauge.SetSpriteV( 0.f, 1.f );
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_RECALL );	
		if( !l_pSprite )
			return;

		GetSpriteInfo()->pSprite = l_pSprite;

		m_uiBoard.GetSpriteInfo()->pSprite = l_pSprite;

		m_uiMobCmdIcon.GetSpriteInfo()->pSprite = l_pSprite;
		for( int i=0; i<TOTAL_COMMAND_NUM; ++i )
		{			
			m_uiCmdButton[i].SetSprite( l_pSprite );
		}
		

		l_pSprite = gSpriteMgr.GetSprite( SP_ID_GAUGE_EXP );
		
		m_uiHPGauge.GetSpriteInfo()->pSprite = l_pSprite;		
		m_uiFPGauge.GetSpriteInfo()->pSprite = l_pSprite;
		m_uiExpGauge.GetSpriteInfo()->pSprite = l_pSprite;

		m_uiCurHPGaugeNum.GetSpriteInfo()->pSprite = l_pSprite;
		m_uiMaxHPGaugeNum.GetSpriteInfo()->pSprite = l_pSprite;
		m_uiCurFPGaugeNum.GetSpriteInfo()->pSprite = l_pSprite;
		m_uiMaxFPGaugeNum.GetSpriteInfo()->pSprite = l_pSprite;
		
		m_uiHPGaugeSlash.GetSpriteInfo()->pSprite = l_pSprite;
		m_uiFPGaugeSlash.GetSpriteInfo()->pSprite = l_pSprite;	
	}
}

void
CGUIMobControl::SetMobName( char * pi_pName )
{
	m_uiMobName.SetString( pi_pName );
}

void
CGUIMobControl::SetLevel( BYTE pi_byLevel )
{
	char buf[32];
	sprintf( buf, "Lv%d", pi_byLevel );
	m_uiMobLevel.SetString( buf );
}

void
CGUIMobControl::SetHP( int pi_nHP, int pi_nMaxHP )
{
	m_uiCurHPGaugeNum.SetNumber( pi_nHP );
	m_uiMaxHPGaugeNum.SetNumber( pi_nMaxHP );

	m_uiCurHPGaugeNum.SetPos( m_uiHPGaugeSlash.m_ptPos.x - m_uiCurHPGaugeNum.m_ptSize.x - 2, m_uiHPGaugeSlash.m_ptPos.y );
	m_uiMaxHPGaugeNum.SetPos( m_uiHPGaugeSlash.m_ptPos.x + m_uiHPGaugeSlash.m_ptSize.x + 2, m_uiHPGaugeSlash.m_ptPos.y );
	
	float l_fRate = (float)pi_nHP/pi_nMaxHP;
	if( l_fRate > 1.f )
		l_fRate = 1.f;
	m_uiHPGauge.SetSpriteU( 0.f, l_fRate );
	m_uiHPGauge.SetSize( 130 * l_fRate , 11 );
}

void	
CGUIMobControl::SetFP( int pi_nFP, int pi_nMaxFP )
{
	m_uiCurFPGaugeNum.SetNumber( pi_nFP );
	m_uiMaxFPGaugeNum.SetNumber( pi_nMaxFP );

	m_uiCurFPGaugeNum.SetPos( m_uiFPGaugeSlash.m_ptPos.x - m_uiCurFPGaugeNum.m_ptSize.x - 2, m_uiFPGaugeSlash.m_ptPos.y );
	m_uiMaxFPGaugeNum.SetPos( m_uiFPGaugeSlash.m_ptPos.x + m_uiFPGaugeSlash.m_ptSize.x + 2, m_uiFPGaugeSlash.m_ptPos.y );
	
	float l_fRate = (float)pi_nFP/pi_nMaxFP;
	if( l_fRate > 1.f )
		l_fRate = 1.f;
	m_uiFPGauge.SetSpriteU( 0.f, l_fRate );
	m_uiFPGauge.SetSize( 130 * l_fRate, 11 );
}

void
CGUIMobControl::SetExp( float pi_fExpRate )
{
	if( pi_fExpRate < 0.f )
		pi_fExpRate = 0.f;
	else if( pi_fExpRate > 1.f )
		pi_fExpRate = 1.f;

	m_uiExpGauge.SetSpriteU( 0.f, pi_fExpRate );
	m_uiExpGauge.SetSize( 64 * pi_fExpRate, 2 );
}

void
CGUIMobControl::MouseEvent( _mouse_event & event )
{
	CGUIWindow::MouseEvent( event );

	if( event.id == MOUSE_CLICKED )
	{
		for( int i=0; i<TOTAL_COMMAND_NUM; ++i )
		{
			if( event.source == &m_uiCmdButton[i] )
			{
				// ##################################################
				// set command ( animus board로 보내야 한다. )
				PostUIMessage( WM_ANIMUSCONTROL_BOARD_COMMAND, i );

				// close cmd buttons
				OpenCmdBoard( FALSE );
				return;
			}
		}		
	}
	else if( event.id == MOUSE_HOVERED )
	{
		if( event.source == &m_uiMobCmdIcon )
		{			
			// open cmd buttons
			OpenCmdBoard( TRUE );
		}
		else
		{
			static char s_pCmdTooltipStr[TOTAL_COMMAND_NUM][32] = { "공격할 타겟 선택", "공격 모드", "이동 모드" };
			for( int i = 0; i < TOTAL_COMMAND_NUM; ++i )
			{
				if( event.source == &m_uiCmdButton[i] )
				{
					SetTooltip( &m_uiCmdButton[i], s_pCmdTooltipStr[i] );
					return;
				}
			}
		}
	}
	else if( event.id == MOUSE_LEAVED )
	{
		if( event.source == &m_uiMobCmdIcon )
		{
			if( IS_HOVERED( m_uiMobCmdBoard.GetState() ) ||
				IS_HOVERED( m_uiCmdViewBaord.GetState() ) )
				return;			

			for( int i=0; i<TOTAL_COMMAND_NUM; ++i )
			{
				// 상태정보에서 명령어버튼으로 마우스를 이동했으면 
				// 명령어버튼을 닫지않는다.
				if( IS_HOVERED( m_uiCmdButton[i].GetState() ) )
					return;
			}

			// close cmd buttons
			OpenCmdBoard( FALSE );
		}
		else 
		{
			if( IS_HOVERED( m_uiMobCmdBoard.GetState() ) ||
				IS_HOVERED( m_uiMobCmdIcon.GetState() ) )
				return;			

			for( int i=0; i<TOTAL_COMMAND_NUM; ++i )
			{
				if( IS_HOVERED( m_uiCmdButton[i].GetState() ) )
					return;
			}			
			
			// close cmd buttons
			OpenCmdBoard( FALSE );			
		}
	}
}

BOOL	
CGUIMobControl::IsCrossed( LONG pi_nXPos, LONG pi_nYPos )
{
	if( CGUIObject::IsCrossed( pi_nXPos, pi_nYPos ) )
		return TRUE;

	return m_uiCmdViewBaord.IsCrossed( pi_nXPos, pi_nYPos );

//	return FALSE;
}

void
CGUIMobControl::OpenCmdBoard( BOOL pi_bOpen )
{
	if( pi_bOpen )
	{
		if( m_bIsOpenedCmdBoard )
			return;
		
		m_uiCmdViewBaord.SetSize( m_uiCmdButton[0].m_ptSize.x * TOTAL_COMMAND_NUM,
								  m_uiCmdButton[0].m_ptSize.y );				

		g_cMoveObjectMgr.AddMoveObject( &m_uiCmdBoard, MOVE_AXIS_X,
										m_uiCmdViewBaord.m_ptPos.x - m_uiCmdBoard.m_ptSize.x, m_uiCmdViewBaord.m_ptPos.x, 200 );
	
		g_cMoveObjectMgr.StartMove( &m_uiCmdBoard );


		m_bIsOpenedCmdBoard = TRUE;
	}
	else
	{
		if( !m_bIsOpenedCmdBoard )
			return;				

		g_cMoveObjectMgr.AddMoveObject( &m_uiCmdBoard, MOVE_AXIS_X,
										m_uiCmdViewBaord.m_ptPos.x, m_uiCmdViewBaord.m_ptPos.x - m_uiCmdBoard.m_ptSize.x, 150 );
	
		g_cMoveObjectMgr.StartMove( &m_uiCmdBoard );


		m_bIsOpenedCmdBoard = FALSE;
	}
}

void
CGUIMobControl::Update( void )
{
	if( !IsShow() )
		return; 

	CGUIWindow::Update();

	if( !m_bIsOpenedCmdBoard &&
		m_uiCmdViewBaord.m_ptSize.x > 0 )
	{
		if( g_cMoveObjectMgr.IsFinishedMove( &m_uiCmdBoard ) )
			m_uiCmdViewBaord.SetSize( 0, 0 );
	}	
}