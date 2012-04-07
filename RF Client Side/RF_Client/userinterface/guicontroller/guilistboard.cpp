////////////////////////////////////////////////////////////////////////////
//
// CGUIListBoard Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "GUIListBoard.h"
#include "../SpriteMgr.h"

CGUIListBoard::CGUIListBoard()
{
	SetSize( 100, 100 );

	m_uiBoard.SetSize( 0, 0 );

	POINT l_ptSize;
	l_ptSize.x = 15; 
	l_ptSize.y = 10; 
	m_uiScrollbar.SetTopArrowSize( l_ptSize );
	m_uiScrollbar.SetBottomArrowSize( l_ptSize );
	l_ptSize.y = 18;
	m_uiScrollbar.SetBarSize( l_ptSize );	
	l_ptSize.y = m_ptSize.y;
	m_uiScrollbar.SetSize( l_ptSize );	


	m_uiScrollbar.AddScrollHandler( this );

	Add( &m_uiBoard );
}

CGUIListBoard::~CGUIListBoard()
{

}

void	
CGUIListBoard::SetSize( POINT pi_ptSize )
{
	SetSize( pi_ptSize.x, pi_ptSize.y );
}

void	
CGUIListBoard::SetSize( LONG pi_nWidth, LONG pi_nHeight )
{
	if( pi_nWidth < m_uiScrollbar.m_ptSize.x || pi_nHeight < 40 )
		return;

	CGUIObject::SetSize( pi_nWidth, pi_nHeight );	

	RECT l_rtRect;	
	l_rtRect.left = m_uiScrollbar.m_ptPos.x;
	l_rtRect.right = m_uiScrollbar.m_ptPos.x + m_uiScrollbar.m_ptSize.x;
	l_rtRect.top = m_uiScrollbar.m_ptPos.y;
	l_rtRect.bottom = m_uiScrollbar.m_ptPos.y + pi_nHeight;

	m_uiScrollbar.SetBound( l_rtRect );

	if( m_uiScrollbar.GetTotalAmount() > 0 )
		m_uiBoard.SetSize( m_ptSize.x - m_uiScrollbar.m_ptSize.x, m_uiBoard.m_ptSize.y );
	else
		m_uiBoard.SetSize( m_ptSize.x, m_uiBoard.m_ptSize.y );
}

int
CGUIListBoard::InsertObject( CGUIObject * pi_pObj, WORD pi_wInsertIndex )
{	
	int l_nInsertIndex;

	l_nInsertIndex = m_uiBoard.Add( pi_pObj, pi_wInsertIndex );

	if( l_nInsertIndex != -1 )
	{	
		pi_pObj->Show( IsShow() );

		if( m_uiScrollbar.GetTotalAmount() > 0 )
			m_uiBoard.SetSize( m_ptSize.x - m_uiScrollbar.m_ptSize.x, m_uiBoard.m_ptSize.y + pi_pObj->m_ptSize.y );
		else
			m_uiBoard.SetSize( m_ptSize.x, m_uiBoard.m_ptSize.y + pi_pObj->m_ptSize.y );
				

		CGUIObject * l_pObj;
		CGUIObject * l_pPrevObj;
		
		// set inserted object pos
		if( l_nInsertIndex == 0 )
		{
			l_pPrevObj = m_uiBoard.GetAddedObject( l_nInsertIndex );
			pi_pObj->SetPos( m_uiBoard.m_ptPos.x, m_uiBoard.m_ptPos.y );			
		}
		else
		{
			l_pPrevObj = m_uiBoard.GetAddedObject( l_nInsertIndex - 1 );
			pi_pObj->SetPos( m_uiBoard.m_ptPos.x, l_pPrevObj->m_ptPos.y + l_pPrevObj->m_ptSize.y );		

			l_pPrevObj = pi_pObj;
		}
		
		// reset other object pos
		for( int i=l_nInsertIndex+1; i<m_uiBoard.GetAddedObjectNum(); ++i )
		{
			l_pObj = m_uiBoard.GetAddedObject( i );
			l_pObj->SetPos( m_uiBoard.m_ptPos.x, l_pPrevObj->m_ptPos.y + l_pPrevObj->m_ptSize.y );

			l_pPrevObj = l_pObj;
		}				

		UpdateScrollbar();
	}

	return l_nInsertIndex;
}

void
CGUIListBoard::RemoveObject( CGUIObject * pi_pObj )
{
	int l_nObjectIndex = m_uiBoard.GetAddedObjectIndex( pi_pObj );
	if( l_nObjectIndex == -1 )
		return;
	
	// remove object
	m_uiBoard.Remove( pi_pObj );
	

	// reset other object pos
	CGUIObject * l_pObj;
	CGUIObject * l_pPrevObj;
	
	int i = l_nObjectIndex;
	if( i == 0 )
	{		
		l_pPrevObj = m_uiBoard.GetAddedObject( 0 );
		if( l_pPrevObj )
			l_pPrevObj->SetPos( m_uiBoard.m_ptPos.x, m_uiBoard.m_ptPos.y );

		++i;
	}
	else
	{		
		l_pPrevObj = m_uiBoard.GetAddedObject( l_nObjectIndex - 1 );		
	}

	for( ; i<m_uiBoard.GetAddedObjectNum(); ++i )
	{
		l_pObj = m_uiBoard.GetAddedObject( i );

		l_pObj->SetPos( l_pPrevObj->m_ptPos.x, l_pPrevObj->m_ptPos.y + l_pPrevObj->m_ptSize.y );

		l_pPrevObj = l_pObj;
	}

	UpdateScrollbar();

	// resize board
	if( m_uiScrollbar.GetTotalAmount() > 0 )
		m_uiBoard.SetSize( m_uiBoard.m_ptSize.x - m_uiScrollbar.m_ptSize.x, m_uiBoard.m_ptSize.y - pi_pObj->m_ptSize.y );
	else
		m_uiBoard.SetSize( m_uiBoard.m_ptSize.x, m_uiBoard.m_ptSize.y - pi_pObj->m_ptSize.y );	
}

void
CGUIListBoard::RemoveAllObject( void )
{
	m_uiBoard.RemoveAll();
	
	m_uiBoard.SetSize( 0, 0 );

	UpdateScrollbar();
}

int
CGUIListBoard::GetListSize( void )
{
	return m_uiBoard.GetAddedObjectNum();
}

CGUIObject * 
CGUIListBoard::GetListObject( WORD pi_wListIndex )
{
	return m_uiBoard.GetAddedObject( pi_wListIndex );
}

void	
CGUIListBoard::UpdateScrollbar( void )
{
	if( m_uiBoard.m_ptSize.y > m_ptSize.y )
	{
		m_uiScrollbar.SetTotalAmount( m_uiBoard.m_ptSize.y - m_ptSize.y );

		if( m_uiScrollbar.GetCurrentAmount() > m_uiScrollbar.GetTotalAmount() )
			m_uiScrollbar.SetCurrentAmount( m_uiScrollbar.GetTotalAmount() );

		m_uiBoard.SetPos( m_ptPos.x, m_ptPos.y - m_uiScrollbar.GetCurrentAmount() );
		
		m_uiScrollbar.SetPos( m_ptPos.x + m_ptSize.x - m_uiScrollbar.m_ptSize.x, m_ptPos.y );
		m_uiScrollbar.Show( IsShow() );
		Add( &m_uiScrollbar );
	}
	else
	{
		m_uiScrollbar.SetAmount( 0, 0 );

		m_uiBoard.SetPos( m_ptPos.x, m_ptPos.y );

		Remove( &m_uiScrollbar );
	}
}

void
CGUIListBoard::ScrollEvent( _scroll_event & event )
{
	m_uiBoard.SetPos( m_ptPos.x, m_ptPos.y - m_uiScrollbar.GetCurrentAmount() );
}

void	
CGUIListBoard::SetSprite( void )
{
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

	CGUIContainer::SetSprite();
}
