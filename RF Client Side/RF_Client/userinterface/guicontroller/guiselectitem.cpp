////////////////////////////////////////////////////////////////////////////
//
// CGUISelectItem Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "GUISelectItem.h"

CGUISelectItem::CGUISelectItem()
{
	m_pSelectItemObserver = NULL;

	m_itCurSelectedItem = m_listItemStr.begin();

	m_rgbItemColor = D3DCOLOR_XRGB( 255, 255, 255 );

	m_bHandleItemChange = false;
}

CGUISelectItem::~CGUISelectItem()
{

}

void
CGUISelectItem::Init( POINT pi_ptSelectButtonSize, int ItemBoardWidth )
{
	m_cUpButton.SetSize( pi_ptSelectButtonSize );
	m_cDownButton.SetSize( pi_ptSelectButtonSize );

	m_cItemBoard.SetSize( ItemBoardWidth, 12 );

	RECT l_rtBound;
	l_rtBound.left = m_ptPos.x;
	l_rtBound.right = m_ptPos.x + m_cUpButton.m_ptSize.x + 
					  m_cDownButton.m_ptSize.x + m_cItemBoard.m_ptSize.x;
	l_rtBound.top = m_ptPos.y;
	if( m_cUpButton.m_ptSize.y > m_cItemBoard.m_ptSize.y )
		l_rtBound.bottom = m_ptPos.y + m_cUpButton.m_ptSize.y;
	else
		l_rtBound.bottom = m_ptPos.y + m_cItemBoard.m_ptSize.y;

	SetBound( l_rtBound );


	m_cUpButton.AddMouseEventHandler( this );
	m_cDownButton.AddMouseEventHandler( this );

	Add( &m_cUpButton );
	Add( &m_cItemBoard );
	Add( &m_cDownButton );
}

void
CGUISelectItem::SetSprite( CSprite * pi_pSprite )
{
	if( m_cUpButton.GetSpriteInfo()->pSprite != NULL )
		m_cUpButton.SetSprite( pi_pSprite );

	if( m_cDownButton.GetSpriteInfo()->pSprite != NULL )
		m_cDownButton.SetSprite( pi_pSprite );

	if( m_cItemBoard.GetSpriteInfo()->pSprite != NULL )
		m_cItemBoard.GetSpriteInfo()->pSprite = pi_pSprite;
}

void
CGUISelectItem::SetUpButtonSpriteInfo( SPRITE_INFO * pi_pSpriteInfo )
{
	m_cUpButton.SetSpriteInfoEx( &pi_pSpriteInfo[0] );
	m_cUpButton.SetSpriteInfoEx( PRESSED, &pi_pSpriteInfo[1] );
}

void
CGUISelectItem::SetDownButtonSpriteInfo( SPRITE_INFO * pi_pSpriteInfo )
{
	m_cDownButton.SetSpriteInfoEx( &pi_pSpriteInfo[0] );
	m_cDownButton.SetSpriteInfoEx( PRESSED, &pi_pSpriteInfo[1] );
}

void
CGUISelectItem::SetItemBoardSpriteInfo( SPRITE_INFO * pi_pSpriteInfo )
{
	m_cItemBoard.SetSpriteInfo( pi_pSpriteInfo );
}

void
CGUISelectItem::SetItemBoardWidth( int pi_nWidth )
{
	RECT	rect;
	rect.left = m_ptPos.x;
	rect.top = m_ptPos.y;
	rect.right = m_ptPos.x + 
				m_cUpButton.m_ptSize.x + pi_nWidth + m_cDownButton.m_ptSize.x;
	rect.bottom = m_ptPos.y + m_ptSize.y;

	SetBound( rect );
}

void
CGUISelectItem::SetBound( RECT & pi_bound )
{
	// item board만 resize해준다.
	CGUIObject::SetBound( pi_bound );

	// 
	POINT	l_ptPos;
	l_ptPos.x = m_ptPos.x;
	l_ptPos.y = m_ptPos.y + ( m_ptSize.y - m_cUpButton.m_ptSize.y ) / 2;
	m_cUpButton.SetPos( l_ptPos );

	l_ptPos.x = m_ptPos.x + m_ptSize.x - m_cDownButton.m_ptSize.x;	
	m_cDownButton.SetPos( l_ptPos );


	l_ptPos.x = m_cUpButton.m_ptPos.x + m_cUpButton.m_ptSize.x;
	l_ptPos.y = m_ptPos.y + ( m_ptSize.y - m_cItemBoard.m_ptSize.y ) / 2;
	m_cItemBoard.SetPos( l_ptPos );

	l_ptPos.x = m_ptSize.x - m_cUpButton.m_ptSize.x * 2;
	l_ptPos.y = m_cItemBoard.m_ptSize.y;
	m_cItemBoard.SetSize( l_ptPos );
}

char *
CGUISelectItem::GetItem( WORD pi_wIndex )
{
	if( m_listItemStr.size() == 0 )
		return NULL;

	WORD l_wIndex = 0;
	for( CGUIItemStrList::iterator i=m_listItemStr.begin(); i != m_listItemStr.end(); ++i )
	{
		if( l_wIndex == pi_wIndex )
			return *i;
						
		++l_wIndex;
	}

	return NULL;
}

char *	
CGUISelectItem::GetCurrentItem( void )
{
	if( m_listItemStr.size() == 0 )
		return NULL;

	return *m_itCurSelectedItem;
}

void
CGUISelectItem::SetCurrentItem( WORD pi_wIndex )
{
	if( m_listItemStr.size() == 0 )
		return;

	WORD l_wIndex = 0;
	for( CGUIItemStrList::iterator i=m_listItemStr.begin(); i != m_listItemStr.end(); ++i )
	{
		if( l_wIndex == pi_wIndex )
		{
			m_itCurSelectedItem = i;
			return;
		}
						
		++l_wIndex;
	}
}

void
CGUISelectItem::InsertItem( char * pi_pItemStr )
{	
	// 중복 체크
	if( find( m_listItemStr.begin(), m_listItemStr.end(), pi_pItemStr ) != m_listItemStr.end() )
		return;

	m_listItemStr.insert( m_listItemStr.end(), pi_pItemStr );

	if( m_listItemStr.size() == 1 )
		m_itCurSelectedItem = m_listItemStr.begin();
}
	
void
CGUISelectItem::DeleteItem( char * pi_pItemStr )
{
	m_listItemStr.remove( pi_pItemStr );

	if( m_listItemStr.size() == 0 )
		m_itCurSelectedItem = m_listItemStr.begin();		
}

void
CGUISelectItem::Clear( void )
{
	m_listItemStr.clear();
	m_itCurSelectedItem = m_listItemStr.begin();
}

BOOL
CGUISelectItem::Draw( void )
{
	if( !CGUIContainer::Draw() )
		return false;

	if( m_listItemStr.size() > 0 )
	{
		// draw item str
		char * l_pItemStr = *m_itCurSelectedItem;
		DrawR3Hangul( m_cItemBoard.m_ptPos.x + ( m_cItemBoard.m_ptSize.x - strlen(l_pItemStr)*6 ) / 2,
					  m_cItemBoard.m_ptPos.y,
					  l_pItemStr,
					  m_rgbItemColor );					  
	}

	return true;
}

void
CGUISelectItem::MouseEvent( _mouse_event & event )
{
	if( event.id == MOUSE_CLICKED )
	{
		if( event.source == &m_cUpButton )
		{
			BOOL l_bItemChanged;
			
			l_bItemChanged = SelectUp();		
				
			if( m_pSelectItemObserver )
			{
				if( ( m_bHandleItemChange && l_bItemChanged ) ||
					!m_bHandleItemChange )
				{
					SendSelectedEvent( UP_BUTTON );
				}
			}
		}
		else if( event.source == &m_cDownButton )
		{
			BOOL l_bItemChanged;
			
			l_bItemChanged = SelectDown();

			if( m_pSelectItemObserver )
			{
				if( ( m_bHandleItemChange && l_bItemChanged ) ||
					!m_bHandleItemChange )
				{
					SendSelectedEvent( DOWN_BUTTON );					
				}
			}
		}
	}
}

BOOL
CGUISelectItem::SelectUp( void )
{
	if( m_listItemStr.size() == 0 )
		return false;
	
	if( m_itCurSelectedItem == m_listItemStr.begin() )
		return false;
	
	--m_itCurSelectedItem;
	return true;
}

BOOL
CGUISelectItem::SelectDown( void )
{
	if( m_listItemStr.size() == 0 )
		return false;

	CGUIItemStrList::iterator l_itCurPos;

	l_itCurPos = m_itCurSelectedItem;
	++l_itCurPos;

	if( l_itCurPos != m_listItemStr.end() )
	{
		m_itCurSelectedItem = l_itCurPos;
		return true;
	}

	return false;
}

void
CGUISelectItem::SendSelectedEvent( WORD pi_wSelecedIndex )
{
	_event_queue_data	l_EventInfo;

	l_EventInfo.pEventHandler = m_pSelectItemObserver;
	l_EventInfo.pSource = this;
	l_EventInfo.wEventID = SELECTED;
	l_EventInfo.wSelectIndex = pi_wSelecedIndex;
	
	CGUIObject::SendEvent( l_EventInfo );
}