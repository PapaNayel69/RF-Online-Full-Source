////////////////////////////////////////////////////////////////////////////
//
// CGUIScrollbar Class Implementation
//
////////////////////////////////////////////////////////////////////////////

#include "GUIScrollbar.h"

CGUIScrollbar::CGUIScrollbar()
{
	m_nTotalAmount			=	0;
	m_nCurrentAmount		=	0;

	m_wAdjustmentUnit		=	1;
	m_wArrowAdjustmentUnit	=	0; // 0이면 m_wAdjustmentUnit와 같다.

	m_dwScrolledTime		=	0;	

	m_nBarPressedPos		=	0;
	m_nTotalScrollLength	=	0;

	m_wScrollSpeed			=	150;

	m_pScrollObserver		=	NULL;

	m_uiBar.AddMouseEventHandler( this );
	m_uiBar.AddMouseMotionEventHandler( this );
	
	Add( &m_uiTopArrow );
	Add( &m_uiBottomArrow );
	//Add( &m_uiBar );	

	SetStyle( Vertical );

	m_uiBar.SoundOn( FALSE );
}

CGUIScrollbar::~CGUIScrollbar()
{

}

void
CGUIScrollbar::SetStyle( Style pi_eStyle )
{
	m_eStyle = pi_eStyle;

	RECT l_rtBound;

	l_rtBound.left = m_ptPos.x;
	l_rtBound.top = m_ptPos.y;
	l_rtBound.right = m_ptPos.x + m_ptSize.x;
	l_rtBound.bottom = m_ptPos.y + m_ptSize.y;

	SetBound( l_rtBound );
}

int
CGUIScrollbar::GetMinSize( void )
{
	if( m_eStyle == Vertical )
		return m_uiBar.m_ptSize.y + m_uiTopArrow.m_ptSize.y + m_uiBottomArrow.m_ptSize.y + 1;
	else
		return m_uiBar.m_ptSize.x + m_uiTopArrow.m_ptSize.x + m_uiBottomArrow.m_ptSize.x + 1;
}

void
CGUIScrollbar::SetSize( POINT & pi_ptSize )
{
	SetSize( pi_ptSize.x, pi_ptSize.y );
}

void
CGUIScrollbar::SetSize( LONG pi_nWidth, LONG pi_nHeight )
{
	RECT l_rtBound;

	l_rtBound.left = m_ptPos.x;
	l_rtBound.top = m_ptPos.y;
	l_rtBound.right = m_ptPos.x + pi_nWidth;
	l_rtBound.bottom = m_ptPos.y + pi_nHeight;

	SetBound( l_rtBound );
}

void
CGUIScrollbar::SetBound( RECT & pi_rtBound )
{		
	POINT	l_ptPos;

	if( m_eStyle == Vertical )
	{
		// set Top Arrow position
		l_ptPos.x = pi_rtBound.left;
		l_ptPos.y = pi_rtBound.top;
		m_uiTopArrow.SetPos( l_ptPos );

		// set Bottom Arrow position
		l_ptPos.y = pi_rtBound.bottom - m_uiBottomArrow.m_ptSize.y;
		m_uiBottomArrow.SetPos( l_ptPos );	

		// set bar position
		m_uiBar.SetPos( pi_rtBound.left, pi_rtBound.top + m_uiTopArrow.m_ptSize.y );

		// scroll length set
		m_nTotalScrollLength = pi_rtBound.bottom - pi_rtBound.top - m_uiTopArrow.m_ptSize.y -
							   m_uiBottomArrow.m_ptSize.y - m_uiBar.m_ptSize.y;
	}
	else
	{
		// set Top Arrow position
		l_ptPos.x = pi_rtBound.left;
		l_ptPos.y = pi_rtBound.top;
		m_uiTopArrow.SetPos( l_ptPos );

		// set Bottom Arrow position
		l_ptPos.x = pi_rtBound.right - m_uiBottomArrow.m_ptSize.x;
		m_uiBottomArrow.SetPos( l_ptPos );	

		// set bar position
		m_uiBar.SetPos( pi_rtBound.left + m_uiTopArrow.m_ptSize.x, pi_rtBound.top );		

		// scroll length set
		m_nTotalScrollLength = pi_rtBound.right - pi_rtBound.left - m_uiTopArrow.m_ptSize.x -
							   m_uiBottomArrow.m_ptSize.x - m_uiBar.m_ptSize.x;
	}

	// scroll base만 rebound해준다.
	CGUIObject::SetBound( pi_rtBound );
	
	if( m_nTotalAmount > 0 )
		UpdateBar();
}

void
CGUIScrollbar::AddScrollHandler( IScrollObserver * pi_pObj )
{
	m_pScrollObserver = pi_pObj;	
}

void
CGUIScrollbar::SetTopArrowSpriteInfo( SPRITE_INFO * pi_spriteInfos )
{
	m_uiTopArrow.SetSpriteInfoEx( &pi_spriteInfos[0] );
	m_uiTopArrow.SetSpriteInfoEx( PRESSED, &pi_spriteInfos[1] );	
}

void
CGUIScrollbar::SetBottomArrowSpriteInfo( SPRITE_INFO * pi_spriteInfos )
{
	m_uiBottomArrow.SetSpriteInfoEx( &pi_spriteInfos[0] );
	m_uiBottomArrow.SetSpriteInfoEx( PRESSED, &pi_spriteInfos[1] );	
}

void
CGUIScrollbar::SetBarSpriteInfo( SPRITE_INFO * pi_pSpriteInfo )
{ 
	m_uiBar.SetSpriteInfoEx( pi_pSpriteInfo );
}

void	
CGUIScrollbar::SetSprite( CSprite * pi_pSprite )
{
	m_uiTopArrow.SetSprite( pi_pSprite );
	m_uiBottomArrow.SetSprite( pi_pSprite );
	m_uiBar.SetSprite( pi_pSprite );

	if( GetSpriteInfo()->pSprite != NULL )
		GetSpriteInfo()->pSprite = pi_pSprite;
}

void
CGUIScrollbar::SetTotalAmount( LONG pi_nTotalAmount )
{	
	if( pi_nTotalAmount < 0 )
		m_nTotalAmount = 0;
	else
		m_nTotalAmount = pi_nTotalAmount;

	if( m_nTotalAmount == 0 )
	{
		m_uiTopArrow.RemoveMouseEventHandler();
		m_uiBottomArrow.RemoveMouseEventHandler();
		RemoveMouseEventHandler();

		// scroll할게 없으니깐 bar를 지운다.
		if( GetAddedObjectNum() == 3 )
			Remove( &m_uiBar );
	}
	else
	{
		m_uiTopArrow.AddMouseEventHandler( this );
		m_uiBottomArrow.AddMouseEventHandler( this );
		AddMouseEventHandler( this );

		// scroll data가 생겼으니깐 bar를 추가시킨다.
		if( GetAddedObjectNum() == 2 )		
		{
			if( m_eStyle == Vertical )
				m_uiBar.SetPos( m_ptPos.x + ( m_ptSize.x - m_uiBar.m_ptSize.x ) / 2, 0 );
			else
				m_uiBar.SetPos( 0, m_ptPos.y + ( m_ptSize.y - m_uiBar.m_ptSize.y ) / 2 );

			m_uiBar.Show( IsShow() );
			if( m_bEnableFocus )				
				m_uiBar.EnableFocus();
			else
				m_uiBar.DisableFocus();				

			Add( &m_uiBar );
		}

		// set bar position
		UpdateBar();
	}		
}

void
CGUIScrollbar::SetCurrentAmount( LONG pi_nCurrentAmount )
{
	if( pi_nCurrentAmount > m_nTotalAmount )
		m_nCurrentAmount = m_nTotalAmount;
	else if( pi_nCurrentAmount < 0 )
		m_nCurrentAmount = 0;
	else 
		m_nCurrentAmount = pi_nCurrentAmount;

	if( m_nTotalAmount > 0 )
		UpdateBar();
}

void
CGUIScrollbar::SetAmount( LONG pi_nTotalAmount, LONG pi_nCurrentAmount )
{
	if( pi_nTotalAmount < 0 )
		m_nTotalAmount = 0;
	else
		m_nTotalAmount = pi_nTotalAmount;

	if( pi_nCurrentAmount > m_nTotalAmount )
		m_nCurrentAmount = m_nTotalAmount;
	else if( pi_nCurrentAmount < 0 )
		m_nCurrentAmount = 0;
	else 
		m_nCurrentAmount = pi_nCurrentAmount;

	if( m_nTotalAmount == 0 )
	{
		m_uiTopArrow.RemoveMouseEventHandler();
		m_uiBottomArrow.RemoveMouseEventHandler();
		RemoveMouseEventHandler();

		// scroll할게 없으니깐 bar를 지운다.
		if( GetAddedObjectNum() == 3 )
			Remove( &m_uiBar );
	}
	else
	{
		m_uiTopArrow.AddMouseEventHandler( this );
		m_uiBottomArrow.AddMouseEventHandler( this );
		AddMouseEventHandler( this );

		// scroll data가 생겼으니깐 bar를 추가시킨다.
		if( GetAddedObjectNum() == 2 )		
		{
			if( m_eStyle == Vertical )
				m_uiBar.SetPos( m_ptPos.x + ( m_ptSize.x - m_uiBar.m_ptSize.x ) / 2, 0 );
			else
				m_uiBar.SetPos( 0, m_ptPos.y + ( m_ptSize.y - m_uiBar.m_ptSize.y ) / 2 );

			m_uiBar.Show( IsShow() );
			if( m_bEnableFocus )				
				m_uiBar.EnableFocus();
			else
				m_uiBar.DisableFocus();	

			Add( &m_uiBar );
		}

		// set bar position
		UpdateBar();
	}
	
}

void
CGUIScrollbar::UpdateBar( void )
{	
	LONG l_nCurScrollLength;	
	
	// m_nCurrentAmount에 해당하는 Y축 길이 
	l_nCurScrollLength = m_nTotalScrollLength * m_nCurrentAmount / m_nTotalAmount;

	// set bar position
	if( m_eStyle == Vertical )
		m_uiBar.m_ptPos.y = m_uiTopArrow.m_ptPos.y + m_uiTopArrow.m_ptSize.y + l_nCurScrollLength;
	else
		m_uiBar.m_ptPos.x = m_uiTopArrow.m_ptPos.x + m_uiTopArrow.m_ptSize.x + l_nCurScrollLength;
}

void
CGUIScrollbar::MouseEvent( _mouse_event & event )
{
	if( event.id == MOUSE_STROKED )
	{
		if( event.source == &m_uiTopArrow )
		{
			m_dwStrokedTime = timeGetTime();

			if( m_wArrowAdjustmentUnit )
				SetCurrentAmount( m_nCurrentAmount - m_wArrowAdjustmentUnit );
			else
				SetCurrentAmount( m_nCurrentAmount - m_wAdjustmentUnit );
			
			SendScrollEvent( SCROLLED_UP );
		}
		else if( event.source == &m_uiBottomArrow )
		{
			m_dwStrokedTime = timeGetTime();

			if( m_wArrowAdjustmentUnit )
				SetCurrentAmount( m_nCurrentAmount + m_wArrowAdjustmentUnit );
			else
				SetCurrentAmount( m_nCurrentAmount + m_wAdjustmentUnit );

			SendScrollEvent( SCROLLED_DOWN );
		}
		else if( event.source == &m_uiBar )
		{
			if( m_eStyle == Vertical )
				m_nBarPressedPos = event.yPos - m_uiBar.m_ptPos.y;
			else
				m_nBarPressedPos = event.xPos - m_uiBar.m_ptPos.x;
		}
		else if( event.source == this )
		{
			// 바닥을 클릭하면 클릭한 위치에 가까운 위치에 amount로..

			// get current amount
			int l_nCurScrollLength;
			
			if( m_eStyle == Vertical )
				l_nCurScrollLength = event.yPos - ( m_uiBar.m_ptSize.y / 2 ) -
									m_uiTopArrow.m_ptPos.y - m_uiTopArrow.m_ptSize.y;
			else
				l_nCurScrollLength = event.xPos - ( m_uiBar.m_ptSize.x / 2 ) -
									m_uiTopArrow.m_ptPos.x - m_uiTopArrow.m_ptSize.x;
			
			if( l_nCurScrollLength < 0 )
				l_nCurScrollLength = 0;

			int l_nCurrentAmount = CalcCurrentAmount( l_nCurScrollLength );

			
			// Update current amount
			if( l_nCurrentAmount != m_nCurrentAmount )
			{
				SetCurrentAmount( l_nCurrentAmount );

				if( l_nCurrentAmount < m_nCurrentAmount )		
					SendScrollEvent( SCROLLED_UP );				

				else
					SendScrollEvent( SCROLLED_DOWN );
			}
		}
	}
	else if( event.id == MOUSE_PRESSED ) 
	{
		if( event.source == &m_uiTopArrow )
		{
			if( timeGetTime() - m_dwScrolledTime < m_wScrollSpeed )
				return;			

			if( m_wArrowAdjustmentUnit )
				SetCurrentAmount( m_nCurrentAmount - 
								m_wArrowAdjustmentUnit * ( timeGetTime() - m_dwScrolledTime ) / m_wScrollSpeed );
			else
				SetCurrentAmount( m_nCurrentAmount - 
								m_wAdjustmentUnit * ( timeGetTime() - m_dwScrolledTime ) / m_wScrollSpeed );

			
			SendScrollEvent( SCROLLED_UP );			
		}
		else if( event.source == &m_uiBottomArrow )
		{
			if( timeGetTime() - m_dwScrolledTime < m_wScrollSpeed )
				return;
			
			if( m_wArrowAdjustmentUnit )
				SetCurrentAmount( m_nCurrentAmount + 
								m_wArrowAdjustmentUnit * ( timeGetTime() - m_dwScrolledTime ) / m_wScrollSpeed );
			else
				SetCurrentAmount( m_nCurrentAmount + 
								m_wAdjustmentUnit * ( timeGetTime() - m_dwScrolledTime ) / m_wScrollSpeed );


			SendScrollEvent( SCROLLED_DOWN );			
		}		
	}
	else if( event.id == MOUSE_RELEASED ||
			 event.id == MOUSE_CLICKED )
	{
		if( event.source == &m_uiBar )
		{
			// get current amount
			int l_nCurScrollLength;
			
			if( m_eStyle == Vertical )
				l_nCurScrollLength = event.yPos - m_nBarPressedPos -
									 m_uiTopArrow.m_ptPos.y - m_uiTopArrow.m_ptSize.y;
			else
				l_nCurScrollLength = event.xPos - m_nBarPressedPos -
									 m_uiTopArrow.m_ptPos.x - m_uiTopArrow.m_ptSize.x;

			if( l_nCurScrollLength < 0 )
				l_nCurScrollLength = 0;

			int l_nCurrentAmount = CalcCurrentAmount( l_nCurScrollLength );
			
			// Update current amount
			if( l_nCurrentAmount != m_nCurrentAmount )
			{	
				SetCurrentAmount( l_nCurrentAmount );

				if( l_nCurrentAmount < m_nCurrentAmount )				
					SendScrollEvent( SCROLLED_UP );
				
				else				
					SendScrollEvent( SCROLLED_DOWN );				
			}
			else
			{
				// bar를 원래 위치로..
				UpdateBar();
			}			
		}	
	}

}

void
CGUIScrollbar::MouseMotionEvent( _mouse_motion_event & event )
{
	if( event.id == MOUSE_DRAG &&
		event.source == &m_uiBar )
	{
		if( m_eStyle == Vertical )
		{
			// get bar position
			int l_nBarPosY = event.yPos - m_nBarPressedPos;

			// boundary check
			if( l_nBarPosY < m_uiTopArrow.m_ptPos.y + m_uiTopArrow.m_ptSize.y )
				l_nBarPosY = m_uiTopArrow.m_ptPos.y + m_uiTopArrow.m_ptSize.y;		

			else if( l_nBarPosY > m_uiBottomArrow.m_ptPos.y - m_uiBar.m_ptSize.y )
				l_nBarPosY = m_uiBottomArrow.m_ptPos.y - m_uiBar.m_ptSize.y;

			// get current amount
			int l_nCurScrollLength;
			l_nCurScrollLength = l_nBarPosY - m_uiTopArrow.m_ptPos.y - m_uiTopArrow.m_ptSize.y;

			int l_nCurrentAmount = CalcCurrentAmount( l_nCurScrollLength );		
			

			// update current amount
			if( l_nCurrentAmount != m_nCurrentAmount )
			{
				SetCurrentAmount( l_nCurrentAmount );

				if( l_nCurrentAmount < m_nCurrentAmount )			
					SendScrollEvent( SCROLLED_UP );
				
				else
					SendScrollEvent( SCROLLED_DOWN );				
			}

			// update bar position
			m_uiBar.m_ptPos.y = l_nBarPosY;

		}
		else // Horizontal
		{
			// get bar position
			int l_nBarPosX = event.xPos - m_nBarPressedPos;

			// boundary check
			if( l_nBarPosX < m_uiTopArrow.m_ptPos.x + m_uiTopArrow.m_ptSize.x )
				l_nBarPosX = m_uiTopArrow.m_ptPos.x + m_uiTopArrow.m_ptSize.x;		

			else if( l_nBarPosX > m_uiBottomArrow.m_ptPos.x - m_uiBar.m_ptSize.x )
				l_nBarPosX = m_uiBottomArrow.m_ptPos.x - m_uiBar.m_ptSize.x;

			// get current amount
			int l_nCurScrollLength;
			l_nCurScrollLength = l_nBarPosX - m_uiTopArrow.m_ptPos.x - m_uiTopArrow.m_ptSize.x;	

			int l_nCurrentAmount = CalcCurrentAmount( l_nCurScrollLength );
			

			// update current amount
			if( l_nCurrentAmount != m_nCurrentAmount )
			{			
				SetCurrentAmount( l_nCurrentAmount );

				if( l_nCurrentAmount < m_nCurrentAmount )
					SendScrollEvent( SCROLLED_UP );
				
				else		
					SendScrollEvent( SCROLLED_DOWN );				
			}

			// update bar position
			m_uiBar.m_ptPos.x = l_nBarPosX;
		}		
	}
}

void
CGUIScrollbar::SendScrollEvent( WORD pi_wID )
{
	if(	m_pScrollObserver )
	{
		_event_queue_data	l_EventInfo;

		l_EventInfo.pEventHandler = m_pScrollObserver;
		l_EventInfo.pSource = this;
		l_EventInfo.wEventID = pi_wID;		
		
		CGUIObject::SendEvent( l_EventInfo );
	}	

	m_dwScrolledTime = timeGetTime();
}


// 현재 스크롤 된 길이로 amount를 계산한다.
LONG
CGUIScrollbar::CalcCurrentAmount( int pi_nCurrentScrollLength )
{
	int l_nResultCurAmount;

	float l_fCurAmount = m_nTotalAmount * ((float)pi_nCurrentScrollLength / m_nTotalScrollLength);		
	

	l_nResultCurAmount = l_fCurAmount;

	if( l_nResultCurAmount >= m_nTotalAmount )
	{
		l_nResultCurAmount = m_nTotalAmount;
	}
	else if( m_nTotalAmount < m_wAdjustmentUnit )
	{
		if( l_fCurAmount < (float)m_nTotalAmount / 2 )
			l_nResultCurAmount = 0;
		else 
			l_nResultCurAmount = m_nTotalAmount;
	}
	else
	{
		// m_wAdjustmentUnit단위로 자른다.
		int l_nRemoveAmount = l_nResultCurAmount % m_wAdjustmentUnit;

		l_nResultCurAmount -= l_nRemoveAmount;

		// 반올림이 되면 하나 더 증가한다.
		if( l_fCurAmount - l_nResultCurAmount >= (float)m_wAdjustmentUnit / 2 )
		{
			l_nResultCurAmount += m_wAdjustmentUnit;
			if( l_nResultCurAmount > m_nTotalAmount )
				l_nResultCurAmount = m_nTotalAmount;
		}				
	}

	return l_nResultCurAmount;
}
