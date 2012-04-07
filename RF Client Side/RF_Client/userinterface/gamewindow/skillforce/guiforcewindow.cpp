////////////////////////////////////////////////////////////////////////////
//
// CGUIForceWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "GUIForceWindow.h"
#include "../../GUIController/GUIScreen.h"
#include "../../SpriteMgr.h"
#include "../../UIGlobal.h"
#include "../../../DefineMessage.h"

#define	TITLE_COLOR		0xFFC3C1BE

CGUIForceWindow::CGUIForceWindow()
{
	
}

CGUIForceWindow::~CGUIForceWindow()
{
	
}

void
CGUIForceWindow::Init( void )
{			
	BYTE byTabNumList[FORCE_MODE_NUM];
	memset( byTabNumList, 4, sizeof( BYTE ) * FORCE_MODE_NUM );
	CreateSFBoard( FORCE_MODE_NUM, byTabNumList );


	CGUISFWindow::Init();

	SetSprite();

	m_uiTitle.SetString( "포스" );	
	
	m_uiSFBoardTab[Fire].SetName( "파이어" );
	m_uiSFBoardTab[Aqua].SetName( "아쿠아" );
	m_uiSFBoardTab[Terra].SetName( "테라" );
	m_uiSFBoardTab[Wind].SetName( "윈드" );
	
	
	for( int i=0; i<m_bySFBoardNum; ++i ) 
		m_pSFBoardList[i].SetTab( 0, TRUE );	

	SetName( "ForceWindow" );			

	m_bIsCreated = TRUE;
}

void
CGUIForceWindow::SetSprite( void )
{
	CGUISFWindow::SetSprite();

	if( !m_bIsCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );

		// force tab
		l_spriteInfo.wActionNo = 4;
		for( int i=0; i<m_bySFBoardNum; ++i )
		{
			l_spriteInfo.wFrameNo = i * 2 + 1;
			m_uiSFBoardTab[i].SetSpriteInfoEx( &l_spriteInfo );
			l_spriteInfo.wFrameNo = i * 2;
			m_uiSFBoardTab[i].SetSpriteInfoEx( DISABLED, &l_spriteInfo );						
		}				
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
		if( !l_pSprite )
			return;

		for( int i=0; i<m_bySFBoardNum; ++i )
			m_uiSFBoardTab[i].SetSprite( l_pSprite );
	}
}

void
CGUIForceWindow::SetRaceType( BYTE pi_byRaceType )
{
	BYTE l_byRaceType = pi_byRaceType / 2;	

	// reset tab sprite
	if( l_byRaceType == RT_BELLATO )
	{
		SPRITE_INFO	l_sSpriteInfo;
		memcpy( &l_sSpriteInfo, m_uiSFBoardTab[0].GetSpriteInfo(), sizeof( SPRITE_INFO ) );

		l_sSpriteInfo.wFrameNo = 1;
		m_uiSFBoardTab[DarknHoly].SetSpriteInfoEx( &l_sSpriteInfo );
		l_sSpriteInfo.wFrameNo = 0;
		m_uiSFBoardTab[DarknHoly].SetSpriteInfoEx( DISABLED, &l_sSpriteInfo );	
		
		m_uiSFBoardTab[DarknHoly].SetName( "홀리" );
	}
	else if( l_byRaceType == RT_CORA )
	{
		SPRITE_INFO	l_sSpriteInfo;
		memcpy( &l_sSpriteInfo, m_uiSFBoardTab[0].GetSpriteInfo(), sizeof( SPRITE_INFO ) );		

		l_sSpriteInfo.wFrameNo = 11;
		m_uiSFBoardTab[DarknHoly].SetSpriteInfoEx( &l_sSpriteInfo );
		l_sSpriteInfo.wFrameNo = 10;
		m_uiSFBoardTab[DarknHoly].SetSpriteInfoEx( DISABLED, &l_sSpriteInfo );		

		m_uiSFBoardTab[DarknHoly].SetName( "다크" );
	}	
}
