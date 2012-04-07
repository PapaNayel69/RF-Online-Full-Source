////////////////////////////////////////////////////////////////////////////
//
// CGUILoginWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "GUILoadingWindow.h"
#include "../GUIController/GUIScreen.h"
#include "../SpriteMgr.h"

CGUILoadingWindow::CGUILoadingWindow()
{

}

CGUILoadingWindow::~CGUILoadingWindow()
{

}

void
CGUILoadingWindow::Init( void )
{
	POINT	l_ptSize;
	POINT	l_ptPos;
	
	l_ptSize.x = gScreen.m_ptSize.x;
	l_ptSize.y = gScreen.m_ptSize.y;
	m_cLoading.SetSize( l_ptSize );

	l_ptPos.x = 0;
	l_ptPos.y = 0;
	m_cLoading.SetPos( l_ptPos );


	Add( &m_cLoading );

	SetSprite();	
}

void
CGUILoadingWindow::SetSprite( void )
{
	SPRITE_INFO		l_spriteInfo;
	l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_LOADING );
	l_spriteInfo.wActionNo = 0;
	l_spriteInfo.wFrameNo = 0;
	m_cLoading.SetSpriteInfo( &l_spriteInfo );
}