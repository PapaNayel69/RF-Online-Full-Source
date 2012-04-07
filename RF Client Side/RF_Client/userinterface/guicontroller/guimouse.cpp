////////////////////////////////////////////////////////////////////////////
//
// CGUIGaugeBar Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "GUIMouse.h"
#include "D3DUtil.h"
#include "CommonUtil.h"
#include "../SpriteMgr.h"

CGUIMouse::CGUIMouse()
{	
	Show( true );

	m_bSystemMouse	= false;
	
	m_wFPS			= 8;

	m_fTempFrameNo	= 0.f;
	m_nCurFrameNo	= 0;

	m_pSprite		= NULL;


	m_eCursorState	= TOTAL_MOUSE_CURSOR_NUM;	
}

CGUIMouse::~CGUIMouse()
{
	
}

void
CGUIMouse::Init( void )
{
	POINT	l_size;

	l_size.x = 32;
	l_size.y = 32;

	SetSize( l_size );
	
	// sprite setting
	m_pSprite = gSpriteMgr.GetSprite( SP_ID_MOUSE );	

	// 일단 defualt로 기본 커서로 세팅해준다.
	for( int i=0; i<TOTAL_MOUSE_CURSOR_NUM; ++i )
	{
		m_nActionNo[i] = 0;		
		m_nTotalFrame[i] = 7;
	}

	// attack
	m_nActionNo[ATTACK] = 1;
	m_nTotalFrame[ATTACK] = 7;

	// pick up item
	m_nActionNo[PICKUP_ITEM] = 2;
	m_nTotalFrame[PICKUP_ITEM] = 10;

	// skill	
	m_nActionNo[WAITING_SKILL] = 3;
	m_nTotalFrame[WAITING_SKILL] = 5;

	// force
	m_nActionNo[WAITING_FORCE] = 4;
	m_nTotalFrame[WAITING_FORCE] = 8;
	
	// npc	
	m_nActionNo[NPC] = 5;
	m_nTotalFrame[NPC] = 6;	

	// same race
	m_nActionNo[SAME_RACE] = 6;
	m_nTotalFrame[SAME_RACE] = 7;
	

	SetCursorState( NORMAL );
}

void
CGUIMouse::SetSprite( void )
{
	m_pSprite = gSpriteMgr.GetSprite( SP_ID_MOUSE );

	GetSpriteInfo()->pSprite = m_pSprite;

	SetCursorState( m_eCursorState );
}

void
CGUIMouse::EnableSystemMouse( BOOL pi_bEnable )
{
	m_bSystemMouse = pi_bEnable;

	if( m_bSystemMouse )
	{
		GetD3dDevice()->ShowCursor( TRUE );		
	}
	else
	{
		GetD3dDevice()->ShowCursor( FALSE );
	}
}

void
CGUIMouse::SetCursorState( CursorState pi_eCursorState )
{	
	if( m_eCursorState == pi_eCursorState )
		return;

	m_eCursorState = pi_eCursorState;

	if( !m_pSprite )
		return;

	if( m_bSystemMouse )
	{
		LPDIRECT3DTEXTURE8 l_pTex;
		
		l_pTex = (LPDIRECT3DTEXTURE8)m_pSprite->GetD3D8TexPtr( m_nActionNo[m_eCursorState], 0 );
		WORD l_wWidth = m_pSprite->GetSpriteXL( m_nActionNo[m_eCursorState], 0 );
		WORD l_wHeight = m_pSprite->GetSpriteYL( m_nActionNo[m_eCursorState], 0 );

		D3DUtil_SetDeviceCursor( GetD3dDevice(), l_wWidth, l_wHeight, l_pTex );			
	}
	else
	{
		SPRITE_INFO	l_spriteInfo;
		l_spriteInfo.pSprite = m_pSprite;
		l_spriteInfo.wActionNo = m_nActionNo[m_eCursorState];
		l_spriteInfo.wFrameNo = 0;

		SetSpriteInfo( &l_spriteInfo );

		// set size
		POINT l_ptSize;
		l_ptSize.x = m_pSprite->GetSpriteOrgXL( m_nActionNo[m_eCursorState], 0 );
		l_ptSize.y = m_pSprite->GetSpriteOrgYL( m_nActionNo[m_eCursorState], 0 );

		SetSize( l_ptSize );	
	}
	
	m_fTempFrameNo = 0.f;
	m_nCurFrameNo = 0;
}

void
CGUIMouse::SetPos( POINT & pi_newPos )
{
	CGUIMouse::SetPos( pi_newPos.x, pi_newPos.y );	
}

void
CGUIMouse::SetPos( LONG pi_nPosX, LONG pi_nPosY )
{
	switch ( m_eCursorState )
	{
	case RANGE_ATTACK:
	case RESIZE:
		// 정중앙
		pi_nPosX -= m_ptSize.y / 2;
		pi_nPosY -= m_ptSize.y / 2;				
		break;

	default:
		// defualt는 왼쪽 상단	
		break;
	}

	CGUIObject::SetPos( pi_nPosX, pi_nPosY );
}

void
CGUIMouse::Update( void )
{
	if( !IsShow() )
		return;

	if( !m_pSprite )
		return;

	m_fTempFrameNo += R3GetLoopTime() * m_wFPS;

	if( m_fTempFrameNo >= m_nTotalFrame[m_eCursorState] )	
		m_fTempFrameNo = 0;

	if( (int)m_fTempFrameNo	!= m_nCurFrameNo )
	{		
		m_nCurFrameNo = m_fTempFrameNo;

		if( m_bSystemMouse )
		{
			LPDIRECT3DTEXTURE8 l_pTex;
				
			l_pTex = (LPDIRECT3DTEXTURE8)m_pSprite->GetD3D8TexPtr( m_nActionNo[m_eCursorState], m_nCurFrameNo );
			WORD l_wWidth = m_pSprite->GetSpriteXL( m_nActionNo[m_eCursorState], m_nCurFrameNo );
			WORD l_wHeight = m_pSprite->GetSpriteYL( m_nActionNo[m_eCursorState], m_nCurFrameNo );

			D3DUtil_SetDeviceCursor( GetD3dDevice(), l_wWidth, l_wHeight, l_pTex );			
		}
		else 
		{
			SPRITE_INFO	l_spriteInfo;
			l_spriteInfo.pSprite = m_pSprite;
			l_spriteInfo.wActionNo = m_nActionNo[m_eCursorState];
			l_spriteInfo.wFrameNo = m_nCurFrameNo;

			SetSpriteInfo( &l_spriteInfo );		
		}
	}
}

BOOL
CGUIMouse::Draw( void )
{
	if( m_bSystemMouse )
		return true;

	return CGUIObject::Draw();
}

void
CGUIMouse::Show( BOOL pi_bShow )
{
	if( m_bSystemMouse )
	{
		if( IsShow() != pi_bShow )
			ShowCursor( pi_bShow );
	}	

	CGUIObject::Show( pi_bShow );
}