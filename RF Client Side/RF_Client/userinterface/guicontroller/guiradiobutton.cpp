////////////////////////////////////////////////////////////////////////////
//
// CGUIRadioButton Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "GUIRadioButton.h"

CGUIRadioButton::CGUIRadioButton()
{
	Add( &m_cButton );
	Add( &m_cText );	

	m_wIndex = 0;
	m_bSelected = false;
	m_sSelectedSprite.pSprite = NULL;
}

CGUIRadioButton::~CGUIRadioButton()
{

}

void
CGUIRadioButton::SetSize( LONG pi_nWidth, LONG pi_nHeight )
{
	CGUIObject::SetSize( pi_nWidth, pi_nHeight );

	Reset();
}

void
CGUIRadioButton::SetSize( POINT & pi_ptSize )
{
	CGUIObject::SetSize( pi_ptSize );

	Reset();
}

void
CGUIRadioButton::SetButtonSize( int pi_nSizeX, int pi_nSizeY )
{
	m_cButton.SetSize( pi_nSizeX, pi_nSizeY );

	Reset();
}

void
CGUIRadioButton::SetString( char * pi_pStr )
{
	m_cText.SetString( pi_pStr );

	Reset();
}

void
CGUIRadioButton::Reset( void )
{
	// resize
	POINT l_ptPoint;	
	l_ptPoint.x = m_cButton.m_ptSize.x + m_cText.m_ptSize.x;
	if( l_ptPoint.x > m_ptSize.x )
		m_ptSize.x = l_ptPoint.x;

	if( m_cButton.m_ptSize.y > m_cText.m_ptSize.y )
		l_ptPoint.y = m_cButton.m_ptSize.y;
	else
		l_ptPoint.y = m_cText.m_ptSize.y;

	if( l_ptPoint.y > m_ptSize.y )
		m_ptSize.y = l_ptPoint.y;	

	// text, button repos
	m_cText.SetPos( m_ptPos.x + m_cButton.m_ptSize.x, 
						m_ptPos.y + ( m_ptSize.y - m_cText.m_ptSize.y ) / 2 );
	m_cButton.SetPos( m_ptPos.x, m_ptPos.y + ( m_ptSize.y - m_cButton.m_ptSize.y ) / 2 );
}

void
CGUIRadioButton::SetButtonSprite( SPRITE_INFO	* pi_pSpriteInfo )
{
	m_cButton.SetSpriteInfo( pi_pSpriteInfo );
}

void
CGUIRadioButton::SetSelectedSprite( SPRITE_INFO	* pi_pSpriteInfo )
{
	memcpy( &m_sSelectedSprite, pi_pSpriteInfo, sizeof( SPRITE_INFO ) );
}

BOOL
CGUIRadioButton::Draw( void )
{
	if( !CGUIContainer::Draw() )
		return false;

	if( m_bSelected )
	{
		if( !m_sSelectedSprite.pSprite )
			return true;

		m_sSelectedSprite.pSprite->SetAction( m_sSelectedSprite.wActionNo );
		m_sSelectedSprite.pSprite->SetFrame( m_sSelectedSprite.wFrameNo );

		m_sSelectedSprite.pSprite->DrawSprite( m_cButton.m_ptPos.x, 
											   m_cButton.m_ptPos.y,								
											   m_cButton.m_ptPos.x + m_cButton.m_ptSize.x,
											   m_cButton.m_ptPos.y + m_cButton.m_ptSize.y );
	}

	return true;
}

////////////////////////////////////////////////////////////////////////////
//
// CGUIRadioButtonCtrl Class Implementation
//
////////////////////////////////////////////////////////////////////////////
CGUIRadioButtonCtrl::CGUIRadioButtonCtrl()
{
	m_bMultiSelect = false;
	m_bEnableUnSelect = false;

	m_pSelectItemObserver = NULL;
}

CGUIRadioButtonCtrl::~CGUIRadioButtonCtrl()
{

}


void	
CGUIRadioButtonCtrl::AddButton( CGUIRadioButton * pi_pButton )
{
	// 중복 체크
	if( find( m_listButton.begin(), m_listButton.end(), pi_pButton ) != m_listButton.end() )
		return;

	// set index
	pi_pButton->m_wIndex = m_listButton.size();

	// add event
	pi_pButton->m_cButton.AddMouseEventHandler( this );
	pi_pButton->m_cText.AddMouseEventHandler( this );
	pi_pButton->AddMouseEventHandler( this );
	// add button
	m_listButton.push_back( pi_pButton );
}

BOOL
CGUIRadioButtonCtrl::IsSelected( WORD pi_wIndex )
{
	WORD l_wIndex = 0;
	for( CGUButtonList::iterator i = m_listButton.begin(); i != m_listButton.end(); ++i )
	{
		if( l_wIndex == pi_wIndex )
		{
			return (*i)->IsSelected();
		}
		++l_wIndex;
	}

	return false;
}

void
CGUIRadioButtonCtrl::Select( WORD pi_wIndex )
{
	WORD l_wIndex = 0;
	for( CGUButtonList::iterator i = m_listButton.begin(); i != m_listButton.end(); ++i )
	{
		if( l_wIndex == pi_wIndex )
		{
			(*i)->Select();
			return;
		}
		++l_wIndex;
	}
}

void
CGUIRadioButtonCtrl::UnSelect( WORD pi_wIndex )
{
	WORD l_wIndex = 0;
	for( CGUButtonList::iterator i = m_listButton.begin(); i != m_listButton.end(); ++i )
	{
		if( l_wIndex == pi_wIndex )
		{
			(*i)->UnSelect();
			return;
		}
		++l_wIndex;
	}
}

void
CGUIRadioButtonCtrl::UnSelectAll( void )
{	
	for( CGUButtonList::iterator i = m_listButton.begin(); i != m_listButton.end(); ++i )
	{
		(*i)->UnSelect();	
	}
}

void
CGUIRadioButtonCtrl::MouseEvent( _mouse_event & event )
{
	if( event.id == MOUSE_STROKED )
	{
		CGUIRadioButton * l_pSrcButton = GetSource( event.source );

		if( l_pSrcButton )
		{
			// 이미 선택되어있다면 even를 보낼 것인지 체크한다.
			if( l_pSrcButton->IsSelected() ) 
			{
				if( m_bEnableUnSelect )
				{
					// unselect 
					l_pSrcButton->UnSelect();

					// send event
					if( !m_pSelectItemObserver )
						return;
					
					_event_queue_data	l_EventInfo;

					l_EventInfo.pEventHandler = m_pSelectItemObserver;
					l_EventInfo.pSource = l_pSrcButton;
					l_EventInfo.wEventID = UNSELECTED;
					l_EventInfo.wSelectIndex = l_pSrcButton->m_wIndex;
					
					CGUIObject::SendEvent( l_EventInfo );
				}
			}
			else
			{
				if( !m_bMultiSelect )
					UnSelectAll();				

				// Select
				l_pSrcButton->Select();

				// send event
				if( !m_pSelectItemObserver )
					return;

				_event_queue_data	l_EventInfo;

				l_EventInfo.pEventHandler = m_pSelectItemObserver;
				l_EventInfo.pSource = l_pSrcButton;
				l_EventInfo.wEventID = SELECTED;
				l_EventInfo.wSelectIndex = l_pSrcButton->m_wIndex;
				
				CGUIObject::SendEvent( l_EventInfo );
			}
			
		}	
	}
}

CGUIRadioButton *
CGUIRadioButtonCtrl::GetSource( CGUIObject * pi_pSrc )
{
	for( CGUButtonList::iterator i = m_listButton.begin(); i != m_listButton.end(); ++i )
	{
		if( pi_pSrc == &(*i)->m_cButton ||
			pi_pSrc == &(*i)->m_cText ||
			pi_pSrc == (*i) )
			return *i;
	}

	return NULL;
}