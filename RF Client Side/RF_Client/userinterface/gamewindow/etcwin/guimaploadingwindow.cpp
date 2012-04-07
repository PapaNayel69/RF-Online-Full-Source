////////////////////////////////////////////////////////////////////////////
//
// CGUIMapLoadingWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "GUIMapLoadingWindow.h"
#include "../../SpriteMgr.h"
#include "../../GUIController/GUIScreen.h"

CGUIMapLoadingWindow::CGUIMapLoadingWindow()
{
	m_dwAniTime	= 0;

	m_bCreated	= FALSE;
}

CGUIMapLoadingWindow::~CGUIMapLoadingWindow()
{

}

void
CGUIMapLoadingWindow::Init( void )
{
	SetSize( gScreen.m_ptSize.x, gScreen.m_ptSize.y );	

//	Add( &m_uiSrcMapName );
//	Add( &m_uiDestMapName );

	SetSprite();

	SetAlwaysTop( TRUE );

	m_bCreated = TRUE;

	SetName( "maploading" );
}

void
CGUIMapLoadingWindow::SetSprite( void )
{
	if( !m_bCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_MAPLOADING );

		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 0;

		SetSpriteInfo( &l_spriteInfo );
	}
	else 
	{
		// resize
		SetSize( gScreen.m_ptSize.x, gScreen.m_ptSize.y );	

		CSprite	* l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_MAPLOADING );
		if( !l_pSprite )
			return;

		GetSpriteInfo()->pSprite = l_pSprite;		
		
//		m_uiSrcMapName.GetSpriteInfo()->pSprite = l_pSprite;
//		m_uiDestMapName.GetSpriteInfo()->pSprite = l_pSprite;		
	}
}

void
CGUIMapLoadingWindow::Open( void )
{
	CGUIWindow::Open();

	// 로딩중에는 마우스에 아이템이 붙어있으면 안보이게 한다.
	if( gScreen.ExistObject() )
		gScreen.GetObject()->Show( FALSE );

	// 마우스는 보여준다.
	if( !gScreen.GetMouse()->IsShow() )
		gScreen.GetMouse()->Show( TRUE );
}

void
CGUIMapLoadingWindow::Close( void )
{
	CGUIWindow::Close();

	// 아이템이 붙어있으면 다시 아이템을 보이게 한다.
	if( gScreen.ExistObject() )
	{
		gScreen.GetObject()->Show( TRUE );		
		gScreen.GetMouse()->Show( FALSE );
	}
}

void
CGUIMapLoadingWindow::SetMapMove( BYTE pi_bySrcMapID, BYTE pi_byDestMapID )
{
/*
	if( pi_bySrcMapID > 7 ||
		pi_byDestMapID > 7 ||
		!GetSpriteInfo()->pSprite )
		return;

	POINT l_pptMapPosition[] = {{171, 172}, // bellato base
								{765, 229}, // cora base
								{328, 100}, // accresia base
								{293, 326}, // bellato field
								{619,282},  // cor field
								{481, 202}, // accresia field
								{463, 429}, // neutral field
								{498, 609}};// resource field

	SPRITE_INFO *	l_pSpriteInfo;
	
	float			l_fMapOrgSizeRate[2];
	l_pSpriteInfo = GetSpriteInfo();
	l_fMapOrgSizeRate[0] = (float)gScreen.m_ptSize.x / 
							l_pSpriteInfo->pSprite->GetSpriteOrgXL( l_pSpriteInfo->wActionNo, l_pSpriteInfo->wFrameNo );
	l_fMapOrgSizeRate[1] = (float)gScreen.m_ptSize.y / 
							l_pSpriteInfo->pSprite->GetSpriteOrgYL( l_pSpriteInfo->wActionNo, l_pSpriteInfo->wFrameNo );


	// src map
	l_pSpriteInfo = m_uiSrcMapName.GetSpriteInfo();	
	l_pSpriteInfo->pSprite = GetSpriteInfo()->pSprite;
	l_pSpriteInfo->wActionNo = 1;
	l_pSpriteInfo->wFrameNo = pi_bySrcMapID;
	// set size m_uiSrcMapName
	m_uiSrcMapName.SetSize( l_pSpriteInfo->pSprite->GetSpriteOrgXL( 
								l_pSpriteInfo->wActionNo, l_pSpriteInfo->wFrameNo ) * 0.7,
							l_pSpriteInfo->pSprite->GetSpriteOrgYL( 
								l_pSpriteInfo->wActionNo, l_pSpriteInfo->wFrameNo ) * 0.7 );
	// set pos
	m_uiSrcMapName.SetPos( m_ptPos.x + l_pptMapPosition[pi_bySrcMapID].x * l_fMapOrgSizeRate[0] ,
						   m_ptPos.y + l_pptMapPosition[pi_bySrcMapID].y * l_fMapOrgSizeRate[1] );
	

	// ------------------------------------------------------------------------------
	// dest map
	l_pSpriteInfo = m_uiDestMapName.GetSpriteInfo();	
	l_pSpriteInfo->pSprite = GetSpriteInfo()->pSprite;	
	l_pSpriteInfo->wActionNo = 1;
	l_pSpriteInfo->wFrameNo = pi_byDestMapID;
	// set size m_uiDestMapName
	m_ptDestMapNameOrgSize.x = l_pSpriteInfo->pSprite->GetSpriteOrgXL( 
								l_pSpriteInfo->wActionNo, l_pSpriteInfo->wFrameNo );
	m_ptDestMapNameOrgSize.y = l_pSpriteInfo->pSprite->GetSpriteOrgYL( 
								l_pSpriteInfo->wActionNo, l_pSpriteInfo->wFrameNo );

	m_uiDestMapName.SetSize( m_ptDestMapNameOrgSize );

	// set pos
	m_ptDestMapNameOrgPos.x = l_pptMapPosition[pi_byDestMapID].x * l_fMapOrgSizeRate[0];
	m_ptDestMapNameOrgPos.y = l_pptMapPosition[pi_byDestMapID].y * l_fMapOrgSizeRate[1];

	m_uiDestMapName.SetPos( m_ptPos.x + m_ptDestMapNameOrgPos.x,
						    m_ptPos.y + m_ptDestMapNameOrgPos.y );	 
*/
}

void
CGUIMapLoadingWindow::Update( void )
{
	CGUIWindow::Update();
/*
	if( !IsShow() )
		return;	

	if( timeGetTime() - m_dwAniTime > 800 )
	{	
		if( m_uiDestMapName.m_ptSize.x == m_ptDestMapNameOrgSize.x )		
		{
			m_uiDestMapName.SetSize( m_uiDestMapName.m_ptSize.x * 0.7,
									 m_uiDestMapName.m_ptSize.y * 0.7 );
			m_uiDestMapName.SetPos( m_ptPos.x + m_ptDestMapNameOrgPos.x + m_uiDestMapName.m_ptSize.x * 0.15,
									m_ptPos.y + m_ptDestMapNameOrgPos.y + m_uiDestMapName.m_ptSize.y * 0.15 );
		}		
		else		
		{
			m_uiDestMapName.SetSize( m_ptDestMapNameOrgSize );
			m_uiDestMapName.SetPos( m_ptPos.x + m_ptDestMapNameOrgPos.x,
									m_ptPos.y + m_ptDestMapNameOrgPos.y );
		}

		m_dwAniTime = timeGetTime();
	}
*/
}
