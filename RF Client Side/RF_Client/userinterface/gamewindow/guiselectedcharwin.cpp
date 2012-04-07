////////////////////////////////////////////////////////////////////////////
//
// CGUISelectedCharWin Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "GUISelectedCharWin.h"
#include "../SpriteMgr.h"

CGUISelectedCharWin::CGUISelectedCharWin()
{
	m_bCreated = FALSE;
}

CGUISelectedCharWin::~CGUISelectedCharWin()
{

}

void
CGUISelectedCharWin::Init( void )
{
	SetSize( 113, 8 );
	m_uiStateBoard.SetSize( 113, 8 );
	m_uiHPGauge.SetSize( 113, 8 );
	m_wGaugeXLen = m_uiHPGauge.m_ptSize.x;

//	m_uiStateLightL.SetPos( m_ptPos.x, m_ptPos.y );
//	m_uiStateLightR.SetPos( m_ptPos.x + m_ptSize.x - m_uiStateLightR.m_ptSize.x, m_ptPos.y );
	m_uiHPGauge.SetPos( m_ptPos.x, m_ptPos.y );
	m_uiCharName.SetPos( m_ptPos.x, m_ptPos.y + 15 );

	Add( &m_uiStateBoard );
//	Add( &m_uiStateLightL );
//	Add( &m_uiStateLightR );
	Add( &m_uiHPGauge );
	Add( &m_uiCharName );
	// ---------------------------------------

	SetSprite();
	
	// ---------------------------------------
	SetSelectState();
	SetHPRate( 0.f );

	m_uiCharName.GetFont()->dwColor = D3DCOLOR_XRGB( 253, 63, 130 );
	m_uiCharName.GetFont()->dwStyle = R3_HAN_OUTLINE;	

	m_bCreated = TRUE;
}

void
CGUISelectedCharWin::SetSprite( void )
{
	if( !m_bCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_BELT );

		l_spriteInfo.wActionNo = 5;
		
		l_spriteInfo.wFrameNo = 0;
		m_uiStateBoard.SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 2;
		m_uiHPGauge.SetSpriteInfo( &l_spriteInfo );

		m_uiHPGauge.SetSpriteV( 0.f, 1.f );

//		l_spriteInfo.wFrameNo = 2;
//		m_uiStateLightR.SetSpriteInfo( &l_spriteInfo );
//		l_spriteInfo.wFrameNo = 3;
//		m_uiStateLightL.SetSpriteInfo( &l_spriteInfo );	
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_BELT );
		if( !l_pSprite )
			return;

		m_uiStateBoard.GetSpriteInfo()->pSprite = l_pSprite;

		m_uiHPGauge.GetSpriteInfo()->pSprite = l_pSprite;
	}

}
	
void
CGUISelectedCharWin::SetCharName( char * pi_pName )
{
	m_uiCharName.SetString( pi_pName );

	// set repos
	m_uiCharName.SetPos( m_ptPos.x + ( m_ptSize.x - m_uiCharName.m_ptSize.x ) / 2, m_uiCharName.m_ptPos.y );
}

void
CGUISelectedCharWin::SetCharNameColor( DWORD pi_dwColor )
{
	m_uiCharName.GetFont()->dwColor = pi_dwColor;
}

void
CGUISelectedCharWin::SetAttackState( void )
{
	m_bIsAttackState = true;

	if( !m_uiHPGauge.GetSpriteInfo() )
		return;

	m_uiHPGauge.GetSpriteInfo()->wFrameNo = 1;
}

void
CGUISelectedCharWin::SetSelectState( void )
{
	m_bIsAttackState = false;

	if( !m_uiHPGauge.GetSpriteInfo() )
		return;

	m_uiHPGauge.GetSpriteInfo()->wFrameNo = 2;
}

void
CGUISelectedCharWin::SetHPRate( float pi_fHPRate )
{	
	m_uiHPGauge.SetSize( pi_fHPRate * m_wGaugeXLen, m_uiHPGauge.m_ptSize.y );

	m_uiHPGauge.SetSpriteU( 0.f, pi_fHPRate );
}

void
CGUISelectedCharWin::Update( void )
{
/*
	static DWORD s_dwLightOnTime;

	if( m_bIsAttackState )
	{
		if( timeGetTime() - s_dwLightOnTime > 500 )
		{
			if( m_uiStateLightR.GetSpriteInfo()->pSprite )
			{
				m_uiStateLightR.GetSpriteInfo()->pSprite = NULL;
				m_uiStateLightL.GetSpriteInfo()->pSprite = NULL;
			}
			else
			{
				m_uiStateLightR.GetSpriteInfo()->pSprite = m_uiStateBoard.GetSpriteInfo()->pSprite;
				m_uiStateLightL.GetSpriteInfo()->pSprite = m_uiStateBoard.GetSpriteInfo()->pSprite;
			}

			s_dwLightOnTime = timeGetTime();		
		}
	}
*/
}


////////////////////////////////////////////////////////////////////////////
//
// CGUISelectedMobInfoWin Class Implementation
//
////////////////////////////////////////////////////////////////////////////
CGUISelectedMobInfoWin::CGUISelectedMobInfoWin()
{
	m_bIsCreated = FALSE;
}

CGUISelectedMobInfoWin::~CGUISelectedMobInfoWin()
{

}

void
CGUISelectedMobInfoWin::Init( void )
{
	SetSize( 126, 52 );
	
	m_byMaxGaugeLen = 113;
	m_uiHPGauge.SetSize( 0, 9 );
	
	m_uiPowerBase.SetSize( 50, 8 );
	m_uiPower.SetSize( 0, 8 );

	m_uiMobName.SetPos( 0, 6 );
	m_uiHPGauge.SetPos( 6, 22 );

	m_uiPowerBase.SetPos( 38, 35 );
	m_uiPower.SetPos( m_uiPowerBase.m_ptPos );

	Add( &m_uiMobName );
	Add( &m_uiHPGauge );
	Add( &m_uiPowerBase );
	Add( &m_uiPower );

	SetSprite();

	SetARGB( 0xCCFFFFFF );

	SetMoveObject( &m_uiMobName );
	SetMovable( TRUE );

	m_bIsCreated = TRUE;
}
	
void
CGUISelectedMobInfoWin::SetSprite( void )
{
	if( !m_bIsCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_GAUGE_EXP );
		if( !l_spriteInfo.pSprite )
			return;

		l_spriteInfo.wActionNo = 3;
		
		l_spriteInfo.wFrameNo = 0;
		SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 1;
		m_uiPowerBase.SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 2;
		m_uiPower.SetSpriteInfo( &l_spriteInfo );

		m_uiPower.SetSpriteV( 0.f, 1.f );

		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 2;
		m_uiHPGauge.SetSpriteInfo( &l_spriteInfo );
		m_uiHPGauge.SetSpriteV( 0.f, 1.f );
	}
	else
	{		
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_GAUGE_EXP );
		if( !l_pSprite )
			return;		

		GetSpriteInfo()->pSprite = l_pSprite;
		
		m_uiPowerBase.GetSpriteInfo()->pSprite = l_pSprite;
		
		m_uiPower.GetSpriteInfo()->pSprite = l_pSprite;		
		
		m_uiHPGauge.GetSpriteInfo()->pSprite = l_pSprite;		
	}
}

void
CGUISelectedMobInfoWin::SetMobName( char * pi_pString )
{
	m_uiMobName.SetString( pi_pString );

	m_uiMobName.SetPos( m_ptPos.x + (m_ptSize.x - m_uiMobName.m_ptSize.x)/2, m_uiMobName.m_ptPos.y );
}

void
CGUISelectedMobInfoWin::SetHP( float pi_fHP )
{
	if( pi_fHP < 0.f || pi_fHP > 1.f )
		return;

	m_uiHPGauge.SetSize( pi_fHP * m_byMaxGaugeLen, m_uiHPGauge.m_ptSize.y );

	m_uiHPGauge.SetSpriteU( 0.f, pi_fHP );
}

void
CGUISelectedMobInfoWin::SetPower( BYTE pi_byPower )
{
	BYTE l_byPower;
	if( pi_byPower > 7 )
		l_byPower = 0;
	else
		l_byPower = pi_byPower;
	
	m_uiPower.SetSize( 7 * l_byPower, m_uiPower.m_ptSize.y );

	m_uiPower.SetSpriteU( 0.f, (float)m_uiPower.m_ptSize.x / 50.f );
}
