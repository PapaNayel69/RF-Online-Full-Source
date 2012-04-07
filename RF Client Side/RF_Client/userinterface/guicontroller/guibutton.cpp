#include "GUIButton.h"

////////////////////////////////////////////////////////////////////////////
//
// CGUIButton Class Implementation
//
////////////////////////////////////////////////////////////////////////////

CGUIButton::CGUIButton()
{	
	for( int i=0; i<4; ++i )
		m_sSpriteInfoEx[i].pSprite = NULL;

	m_bSoundOn	= TRUE;

	m_dwSoundID	= SND_ID_BUTTON_CLICK;
}

CGUIButton::~CGUIButton()
{

}

void
CGUIButton::SetSpriteInfoEx( SPRITE_INFO * pi_spriteInfo )
{
	if( pi_spriteInfo == NULL )
	{
		for( int i=0; i<4; ++i )		
			m_sSpriteInfoEx[i].pSprite = NULL;					
		
		RemoveSpriteInfo();
		return;
	}

	for( int i=0; i<4; ++i )
	{		
		m_sSpriteInfoEx[i].pSprite = pi_spriteInfo->pSprite;
		m_sSpriteInfoEx[i].wActionNo = pi_spriteInfo->wActionNo;
		m_sSpriteInfoEx[i].wFrameNo = pi_spriteInfo->wFrameNo;
	}
	
	// update sprite
	SetSpriteInfo( &m_sSpriteInfoEx[GetSpriteIndexByState()] );
	//	SetSpriteInfo( &m_sSpriteInfoEx[0] );
}

void
CGUIButton::SetSpriteInfoEx( WORD pi_wState, SPRITE_INFO * pi_pSpriteInfo )
{
	int l_nIndex;
	switch( pi_wState )
	{	
	case HOVERED:
		l_nIndex = 1;
		break;
	case PRESSED:
		l_nIndex = 2;
		break;
	case DISABLED:
		l_nIndex = 3;
		break;
	default:
		l_nIndex = 0;
	}

	if( pi_pSpriteInfo == NULL )
	{
		m_sSpriteInfoEx[l_nIndex].pSprite = NULL;		
		return;
	}

	m_sSpriteInfoEx[l_nIndex].pSprite = pi_pSpriteInfo->pSprite;
	m_sSpriteInfoEx[l_nIndex].wActionNo = pi_pSpriteInfo->wActionNo;
	m_sSpriteInfoEx[l_nIndex].wFrameNo = pi_pSpriteInfo->wFrameNo;
	
	// update sprite
	SetSpriteInfo( &m_sSpriteInfoEx[GetSpriteIndexByState()] );
}

void
CGUIButton::SetState( WORD pi_wState )
{
	// stroke event
	if( m_bSoundOn &&
		( !(m_wState & PRESSED) && (pi_wState & PRESSED) ) )
	{
		if( !( m_wState & DISABLED ) )
			PlayWave( m_dwSoundID );
	}

	CGUIObject::SetState( pi_wState );	
	

	// set sprite image
	SetSpriteInfo( &m_sSpriteInfoEx[GetSpriteIndexByState()] );
}

WORD
CGUIButton::GetSpriteIndexByState( void )
{
	// state의 우선순위에 따라 sprite를 setting한다.
	WORD l_wIndex;
	if( m_wState & DISABLED )
		l_wIndex = 3;
	else if( !(m_wState & HOVERED) )
		l_wIndex = 0;
	else if( (m_wState & PRESSED) == PRESSED )
		l_wIndex = 2;	
	else if( m_wState & HOVERED )
		l_wIndex = 1;	
	else
		l_wIndex = 0;

	return l_wIndex;
}

void
CGUIButton::SetSprite( CSprite * pi_pSprite )
{
	// CSprite만 다시 로딩할 때

	for( int i=0; i<4; ++i )
	{	
		if( m_sSpriteInfoEx[i].pSprite != NULL )
			m_sSpriteInfoEx[i].pSprite = pi_pSprite;
	}

	if( GetSpriteInfo()->pSprite != NULL )
		GetSpriteInfo()->pSprite = pi_pSprite;
}


////////////////////////////////////////////////////////////////////////////
//
// CGUIButtonEx Class Implementation
//
////////////////////////////////////////////////////////////////////////////
CGUIButtonEx::CGUIButtonEx()
{
	m_ptLabelPos[0].x = 0;
	m_ptLabelPos[0].y = 0;

	m_ptLabelPos[1].x = 1;
	m_ptLabelPos[1].y = 1;

	m_dwLabelColor[0] = 0xFFFFFFFF;
	m_dwLabelColor[1] = 0xFFFFFFFF;
}

CGUIButtonEx::~CGUIButtonEx()
{

}

void
CGUIButtonEx::SetState( WORD pi_wState )
{
	CGUIButton::SetState( pi_wState );

	ResetLabelPos();

	ResetLabelColor();
}

BOOL
CGUIButtonEx::InputProc( void )
{
	if( !IsCrossed( gMouseDev.GetX(), gMouseDev.GetY() ) )
		return FALSE;

	// skip m_cLabel

	return CGUIObject::InputProc();	
}

void	
CGUIButtonEx::SetLabel( char * pi_pLabel )
{	
	m_cLabel.SetString( pi_pLabel );

	// default label position
	m_ptLabelPos[0].x = ( m_ptSize.x - m_cLabel.m_ptSize.x ) / 2;
	m_ptLabelPos[0].y = ( m_ptSize.y - m_cLabel.m_ptSize.y ) / 2;

	m_ptLabelPos[1].x = m_ptLabelPos[0].x + 1;
	m_ptLabelPos[1].y = m_ptLabelPos[0].y + 1;


	m_cLabel.SetPos( m_ptPos.x + m_ptLabelPos[0].x, 
					 m_ptPos.y + m_ptLabelPos[0].y );
	m_cLabel.Show( IsShow() );
	Add( &m_cLabel );
}

void
CGUIButtonEx::SetLabelColor( DWORD pi_dwColor )
{
	m_dwLabelColor[0] = pi_dwColor;
	m_dwLabelColor[1] = pi_dwColor;

	ResetLabelColor();
}

void
CGUIButtonEx::SetLabelHoveredColor( DWORD pi_dwColor )
{	
	m_dwLabelColor[1] = pi_dwColor;

	ResetLabelColor();
}

void
CGUIButtonEx::SetLabelStyle( DWORD pi_dwStyle )
{
	m_cLabel.GetFont()->dwStyle = pi_dwStyle;
}

void
CGUIButtonEx::SetLabelPos( int pi_nPosX, int pi_nPosY )
{
	m_ptLabelPos[0].x = pi_nPosX;
	m_ptLabelPos[0].y = pi_nPosY;

	m_ptLabelPos[1].x = pi_nPosX + 1;
	m_ptLabelPos[1].y = pi_nPosY + 1;

	ResetLabelPos();
}

void
CGUIButtonEx::SetLabelPressedPos( int pi_nPosX, int pi_nPosY )
{
	m_ptLabelPos[1].x = pi_nPosX;
	m_ptLabelPos[1].y = pi_nPosY;

	ResetLabelPos();
}

void
CGUIButtonEx::ResetLabelColor( void )
{
	if( GetState() & HOVERED )
	{
		m_cLabel.GetFont()->dwColor = m_dwLabelColor[1];
	}		
	else
	{
		m_cLabel.GetFont()->dwColor = m_dwLabelColor[0];
	}
}

static WORD	s_wPressedState = HOVERED | PRESSED;

void
CGUIButtonEx::ResetLabelPos( void )
{
	if( GetState() == s_wPressedState )
	{		
		m_cLabel.SetPos( m_ptPos.x + m_ptLabelPos[1].x,
						 m_ptPos.y + m_ptLabelPos[1].y );
	}
	else
	{
		m_cLabel.SetPos( m_ptPos.x + m_ptLabelPos[0].x, 
						 m_ptPos.y + m_ptLabelPos[0].y );
	}
}