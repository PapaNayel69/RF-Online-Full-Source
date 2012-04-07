////////////////////////////////////////////////////////////////////////////
//
// CGUIMapMoveWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "GUIMapMoveWindow.h"
#include "../../SpriteMgr.h"
#include "../../GUIController/GUIScreen.h"


void EP_MoveToPortal( BYTE pi_byPortalIndex );
void EP_SelectWorldServer( BYTE pi_byIndex );

////////////////////////////////////////////////////////////////////////////

CGUIMapMoveWindow::CGUIMapMoveWindow()
{
	m_wMapNum	= 0;	
}

CGUIMapMoveWindow::~CGUIMapMoveWindow()
{

}

void
CGUIMapMoveWindow::Init( void )
{
	SetSize( 232, 50 );
	m_uiTitleBoard.SetSize( 232, 26 );	

	m_uiTitle.SetString( "지역 이동" );
	m_uiMsgStr.SetString( "어디로 이동하시겠습니까?" );
	
	for( int i=0; i<MAX_MOVABLE_MAP_NUM; ++i )
		m_uiMapButton[i].SetSize( 164, 26 );

	m_uiTitleBoard.SetPos( m_ptPos.x, m_ptPos.y );
	m_uiTitle.SetPos( m_ptPos.x + (m_ptSize.x - m_uiTitle.m_ptSize.x)/2,
					  m_ptPos.y + (m_uiTitleBoard.m_ptSize.y - m_uiTitle.m_ptSize.y)/2 );

	m_uiMsgStr.SetPos( m_ptPos.x + ( m_ptSize.x - m_uiMsgStr.m_ptSize.x ) / 2,
					   m_uiTitleBoard.m_ptPos.y + m_uiTitleBoard.m_ptSize.y + 12 );

	Add( &m_uiTitleBoard );
	Add( &m_uiTitle );

	Add( &m_uiMsgStr );

	for( i=0; i<MAX_MOVABLE_MAP_NUM; ++i )
		m_uiMapButton[i].AddMouseEventHandler( this );		

	SetSprite();	

	m_uiTitle.GetFont()->dwColor = D3DCOLOR_XRGB(195,193,190);
	
	SetMoveObject( this );	
	SetMoveObject( &m_uiTitleBoard );
	SetMoveObject( &m_uiTitle );
	SetMovable( true );

	// init map num
	m_wMapNum = 0;	

	m_bUseForWorldSelect = FALSE;
}

void
CGUIMapMoveWindow::SetSprite( void )
{
	SPRITE_INFO		l_spriteInfo;
	l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_BELT );

	l_spriteInfo.wActionNo = 7;

	// title board
	l_spriteInfo.wFrameNo = 0;
	m_uiTitleBoard.SetSpriteInfo( &l_spriteInfo );	

	
	for( int i=0; i<MAX_MOVABLE_MAP_NUM; ++i )
	{
		l_spriteInfo.wFrameNo = 1;
		m_uiMapButton[i].SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 2;
		m_uiMapButton[i].SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 3;
		m_uiMapButton[i].SetSpriteInfoEx( HOVERED, &l_spriteInfo );
	}	
}

void
CGUIMapMoveWindow::AddMapName( char *(pi_pNameList[64]), BYTE * pi_byMapIndexList, WORD pi_wMapNum )
{	
	if( m_wMapNum > pi_wMapNum )
	{
		// 이전에 남은 mapbutton은 제거한다.
		for( int i=pi_wMapNum; i<m_wMapNum && i<MAX_MOVABLE_MAP_NUM; ++i )
			Remove( &m_uiMapButton[i] );	
	}
	
	// resize & repos
	if( m_wMapNum != pi_wMapNum )		
	{
		SetSize( m_ptSize.x, m_uiTitleBoard.m_ptSize.y + 12 + ( pi_wMapNum + 1 ) * (m_uiMapButton[0].m_ptSize.y+10) );		
		SetPos( gScreen.m_ptPos.x + m_ptPos.x,
				gScreen.m_ptPos.x + ( gScreen.m_ptSize.y - m_ptSize.y ) / 2 );
	}

	// set map num
	m_wMapNum = pi_wMapNum;
	if( m_wMapNum > MAX_MOVABLE_MAP_NUM )
		m_wMapNum = MAX_MOVABLE_MAP_NUM;		
	
	for( int i=0; i<m_wMapNum; ++i )
	{	
		// set map name
		m_uiMapButton[i].SetLabel( pi_pNameList[i] );		

		// set map index
		m_byMapIndex[i] = pi_byMapIndexList[i];

		m_uiMapButton[i].SetPos( m_ptPos.x + 40, 
								 m_ptPos.y + 60 + i * ( m_uiMapButton[i].m_ptSize.y + 10 ) );

		m_uiMapButton[i].Show( IsShow() );
		Add( &m_uiMapButton[i] );
	}
}
	
void
CGUIMapMoveWindow::RemoveAllMapName( void )
{
	for( int i=0; i<MAX_MOVABLE_MAP_NUM; ++i )
		Remove( &m_uiMapButton[i] );

	m_wMapNum = 0;
}

void	
CGUIMapMoveWindow::MouseEvent( _mouse_event & event )
{
	CGUIWindow::MouseEvent( event );
	
	if( event.id == MOUSE_CLICKED )
	{		
		for( int i=0; i<m_wMapNum; ++i )
		{
			if( event.source == &m_uiMapButton[i] )
			{
				//##################################
				// 코드 추가
				if( !m_bUseForWorldSelect )
					EP_MoveToPortal( m_byMapIndex[i] );
				else
					EP_SelectWorldServer( m_byMapIndex[i] );
				return;
			}
		}		
	}
}

#define	BASE_ARGB		0xCC232323
#define	BASE_LINE_ARGB	0xB0B4B4B4
#define TEXT_COLOR		0xFFE8E8E8

BOOL
CGUIMapMoveWindow::Draw( void )
{
	if( !IsShow() )
		return false;

	// base
	Draw2DRectangle( m_ptPos.x, m_ptPos.y + 10,
					 m_ptPos.x + m_ptSize.x, m_ptPos.y + m_ptSize.y,
					 BASE_ARGB );

	Draw2DRectangle( m_ptPos.x + 2, m_ptPos.y + m_uiTitleBoard.m_ptSize.y - 2,
					 m_ptPos.x + 3, m_ptPos.y + m_ptSize.y - 3,
					 BASE_LINE_ARGB );		
	Draw2DRectangle( m_ptPos.x + m_ptSize.x - 3, m_ptPos.y + m_uiTitleBoard.m_ptSize.y - 2,
					 m_ptPos.x + m_ptSize.x - 2, m_ptPos.y + m_ptSize.y - 3,
					 BASE_LINE_ARGB );		
	Draw2DRectangle( m_ptPos.x + 2, m_ptPos.y + m_ptSize.y - 3,
					 m_ptPos.x + m_ptSize.x - 2, m_ptPos.y + m_ptSize.y - 2,
					 BASE_LINE_ARGB );

	CGUIContainer::Draw();

	DrawDialogBox();

	return true;	
}