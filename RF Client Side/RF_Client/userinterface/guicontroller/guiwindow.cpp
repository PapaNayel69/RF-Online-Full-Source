////////////////////////////////////////////////////////////////////////////
//
// CGUIWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////

#include "GUIWindow.h"
#include "GUIScreen.h"
#include "../ConsoleWin.h"
#include "../InputMgr.h"
#include "GUIDialogBox.h"
#include "GUIScrollbar.h"

#define	DOCKING_DETECT_DIST		20		// docking 감지 거리

// ######################################################

void	EP_CloseDialogBox( CGUIWindow * pi_pOwnerWindow );
// ######################################################

CGUIWindow::CGUIWindow()
{
	m_pParent			=	NULL;

	m_bReSizable		=	FALSE;
	m_bMovable			=	FALSE;
	m_byCurMotion		=	NONE_MOTION;

	for( int i=0; i<4; ++i )
		m_pResizeObject[i] = NULL;

	m_bExistResizeObj	=	FALSE;		

	m_bAlwaysTop		=	FALSE;
	m_bAlwaysActive		=	FALSE;	
	
	m_pFocusObject		=	NULL;

	m_dwID = 0;

	m_bEnableDocking	=	FALSE;
	m_pDockingHost		=	NULL;
	m_eDockingBorder	=	NONE_BORDER;

	m_bHostToCustomerDocking = FALSE;

	for( i=0; i<4; ++i )
		m_bEnableDockingBorder[i] = FALSE;

	m_pWheelScrollObject=	NULL;

	m_bSoundOn			=	FALSE;

	// default로 자신을 moveobject로 한다.
	SetMoveObject( this );
}

CGUIWindow::~CGUIWindow()
{
	CGUIDialogBox * l_pDlgBox;
	while( !m_listDialogBox.empty() )
	{
		l_pDlgBox = (*m_listDialogBox.begin());
		l_pDlgBox->RemoveOwnerWindow();		

		// destroy이면 메모리를 해제시킨다.
		if( l_pDlgBox->IsDestroy() )
			delete l_pDlgBox;
	}
/*
	CGUIDialogBox * l_pDlg;
	CGUIDialogBoxList::reverse_iterator rit;
	for( rit=m_listDialogBox.rbegin(); rit != m_listDialogBox.rend(); ++rit )
	{
		l_pDlg = (*rit);
		
		l_pDlg->SetParent( NULL );
		l_pDlg->Close();

		// destroy이면 메모리를 해제시킨다.
		if( l_pDlg->IsDestroy() )
			delete l_pDlg;
	}
	m_listDialogBox.clear();
*/
}

// sprite를 다시 설정한다.
void
CGUIWindow::RestoreSprite( void )
{
	CGUIDialogBoxList::iterator it;
	for( it=m_listDialogBox.begin(); it != m_listDialogBox.end(); ++it )			
		(*it)->SetSprite();

	SetSprite();
}

void
CGUIWindow::SetAlwaysTop( BOOL pi_bTop )
{	
	m_bAlwaysTop = pi_bTop;

	if( m_bAlwaysTop )
		ActiveWindow( this );
}

void
CGUIWindow::SetAlwaysActive( BOOL pi_bActive )
{
	m_bAlwaysActive = pi_bActive;

	if( m_bAlwaysActive )		
		ActiveWindow( this );
}

void
CGUIWindow::Open( void )
{
	// open하면 보여주고 활성화시킨다.
	if( !IsShow() )
	{
		Show( true );

		if( m_bSoundOn )
			PlayWave( SND_ID_WINDOW_OPEN );
	}

	ActiveWindow( this );

	// docking customer이면 host에 알려서
	// docking bound를 update한다.
	if( m_pDockingHost )
		m_pDockingHost->ResetDockingBound();

	PostUIMessage( WM_OPEN_WINDOW, 0 );
}

void
CGUIWindow::Close( void )
{	
	// 화면에 보여주지 않고 비활성화시킨다.
	if( IsShow() )
	{		
		Show( false );	

		if( m_bSoundOn )
			PlayWave( SND_ID_WINDOW_CLOSE );
	}

	InActiveWindow( this );

	// docking customer이면 host에 알려서
	// docking bound를 update한다.
	if( m_pDockingHost )
		m_pDockingHost->ResetDockingBound();

	PostUIMessage( WM_CLOSE_WINDOW, 0 );
}

void
CGUIWindow::Active( void )
{
	// dialogbox가 있을 때는 dialogbox만 active시킨다.
	if( !m_listDialogBox.empty() )
	{
		CGUIDialogBoxList::iterator it;		
		it = m_listDialogBox.begin();

		(*it)->Active();
		return;		
	}

//	if( IsActive() )
//		return;

	// init input focus
	gInputMgr.SetInputObject( NULL );

	CGUIContainer::Active();

	if( m_pFocusObject )
		m_pFocusObject->SetFocus();
}

void
CGUIWindow::InActive( void )
{
	if( !m_listDialogBox.empty() )
	{
		CGUIDialogBoxList::iterator it;
		it = m_listDialogBox.begin();

		(*it)->InActive();
	}

	if( !IsActive() )
		return;

	// 현재 focus된 object를 백업해둔다.
//	if( find( m_listObject.begin(), m_listObject.end(), GetFocusedObject() ) != m_listObject.end() )
//		m_pFocusObject = GetFocusedObject();

	CGUIContainer::InActive();
}

BOOL
CGUIWindow::IsActiveWindow( void )
{
	if( !m_listDialogBox.empty() )
	{
		CGUIDialogBoxList::iterator it;
		it = m_listDialogBox.begin();

		return (*it)->IsActiveWindow();
	}

	return IsActive();	
}

BOOL
CGUIWindow::IsCrossedWindow( LONG pi_nXPos, LONG pi_nYPos )
{
	if( !m_listDialogBox.empty() )
	{
		CGUIDialogBoxList::iterator it;
		it = m_listDialogBox.begin();

		if( (*it)->IsCrossedWindow( pi_nXPos, pi_nYPos ) )		
			return true;
	}

	return IsCrossed( pi_nXPos, pi_nYPos );
}

void
CGUIWindow::ActiveWindow( CGUIWindow * pi_pWin )
{	
	if( m_pParent )		
		m_pParent->ActiveWindow( this );
}

void
CGUIWindow::InActiveWindow( CGUIWindow * pi_pWin )
{
	if( m_pParent )		
		m_pParent->InActiveWindow( this );
}

void
CGUIWindow::SetPos( LONG pi_nPosX, LONG pi_nPosY )
{
	m_rtDockingBound.left -= m_ptPos.x - pi_nPosX;
	m_rtDockingBound.right -= m_ptPos.x - pi_nPosX;
	m_rtDockingBound.top -= m_ptPos.y - pi_nPosY;
	m_rtDockingBound.bottom -= m_ptPos.y - pi_nPosY;		
	
	if( m_bEnableDocking )
	{
		// docking된 customer window를 재배치한다.
		CGUIWindowList::iterator it;
		for( it=m_listDockingCustomer.begin(); it != m_listDockingCustomer.end(); ++it )
		{
			switch( (*it)->GetDockingBorder() )
			{
			case LEFT:
				(*it)->SetPos( pi_nPosX - (*it)->m_ptSize.x,
							   (*it)->m_ptPos.y - m_ptPos.y + pi_nPosY );

				break;

			case TOP:
				(*it)->SetPos( (*it)->m_ptPos.x - m_ptPos.x + pi_nPosX,
							   pi_nPosY - (*it)->m_ptSize.y );

				break;

			case RIGHT:
				(*it)->SetPos( pi_nPosX + m_ptSize.x,
							   (*it)->m_ptPos.y - m_ptPos.y + pi_nPosY );

				break;

			case BOTTOM:
				(*it)->SetPos( (*it)->m_ptPos.x - m_ptPos.x + pi_nPosX,
							   pi_nPosY + m_ptSize.y );

				break;
			}
		}
	}	

	CGUIContainer::SetPos( pi_nPosX, pi_nPosY );
}

void
CGUIWindow::SetPos( POINT pi_ptPos )
{
	SetPos( pi_ptPos.x, pi_ptPos.y );
}

void
CGUIWindow::SetSize(  LONG pi_nWidth, LONG pi_nHeight )
{
	CGUIObject::SetSize( pi_nWidth, pi_nHeight );

	// update docking bound 
	ResetDockingBound();	
}

void
CGUIWindow::SetSize( POINT & pi_ptSize )
{
	SetSize( pi_ptSize.x, pi_ptSize.y );
}

void
CGUIWindow::AddDialogBox( CGUIDialogBox * pi_pDialogBox, BOOL pi_bModal )
{	
	if( find( m_listDialogBox.begin(), m_listDialogBox.end(), pi_pDialogBox ) 
		!= m_listDialogBox.end() )
		return;

	m_listDialogBox.push_back( pi_pDialogBox );

	pi_pDialogBox->SetParent( this );
}

void
CGUIWindow::RemoveDialogBox( CGUIDialogBox * pi_pDialogBox )
{
	m_listDialogBox.remove( pi_pDialogBox );

	pi_pDialogBox->SetParent( NULL );

	// 마우스이벤트가 일어났던 Object가 있으면 Object가 소멸될 수 있기 때문에
	// 이벤트를 지운다.
	if( pi_pDialogBox->IsAddedObject( CGUIObject::GetMouseDetectedObject() ) )
		CGUIObject::ClearMouseEventBuffer();

}

CGUIDialogBox *
CGUIWindow::GetLastDialogBox( void )
{
	CGUIDialogBoxList::reverse_iterator i;
	i = m_listDialogBox.rbegin();

	if( i == m_listDialogBox.rend() )
		return NULL;

	return *i;
}

void
CGUIWindow::SetReSizable( BOOL pi_bResizable )
{
	m_bReSizable = pi_bResizable;

	if( m_bReSizable )
	{
		for ( int i=0; i<4; ++i )
		{
			if( m_pResizeObject[i] )
			{
				m_pResizeObject[i]->AddMouseEventHandler( this );
				m_pResizeObject[i]->AddMouseMotionEventHandler( this );
			}
		}
	}
	else
	{
		for ( int i=0; i<4; ++i )
		{
			if( m_pResizeObject[i] )
			{
				m_pResizeObject[i]->RemoveMouseEventHandler();
				m_pResizeObject[i]->RemoveMouseMotionEventHandler();
			}
		}
	}
}

void
CGUIWindow::SetMovable( BOOL pi_bMovable )
{
	m_bMovable = pi_bMovable;

	if( m_listMoveObject.empty() )
		return;
	
	if( m_bMovable )
	{		
		CGUIObjectList::iterator i;
		for( i = m_listMoveObject.begin(); i != m_listMoveObject.end(); ++i )
		{
			(*i)->AddMouseEventHandler( this );
			(*i)->AddMouseMotionEventHandler( this );	
		}
	}
	else
	{
		CGUIObjectList::iterator i;
		for( i = m_listMoveObject.begin(); i != m_listMoveObject.end(); ++i )
		{
			(*i)->RemoveMouseEventHandler();
			(*i)->RemoveMouseMotionEventHandler();
		}
	}
}

void
CGUIWindow::SetReSizeObject( Border pi_eBorder, CGUIObject * pi_pResieObj )
{
	// resize object가 자기 자신일 수는 없다.
	if( pi_pResieObj == this ||
		pi_pResieObj == NULL )
		return;

	m_pResizeObject[pi_eBorder] = pi_pResieObj;	

	m_bExistResizeObj = true;
}

void
CGUIWindow::SetMoveObject( CGUIObject * pi_pMoveObj )
{
	if( pi_pMoveObj == NULL )
	{
		m_listMoveObject.clear();
		return;
	}

	// 중복 체크
	if( find( m_listMoveObject.begin(), m_listMoveObject.end(), pi_pMoveObj ) 
			!= m_listMoveObject.end() )
		return;
	
	m_listMoveObject.push_back( pi_pMoveObj );
}

// docking customer 등록
void
CGUIWindow::AddDockingCustomer( CGUIWindow * pi_pWindow )
{
	// 중복 체크
	if( find( m_listDockingCustomer.begin(), m_listDockingCustomer.end(), pi_pWindow )
			!= m_listDockingCustomer.end() )
		return;

	// insert 
	m_listDockingCustomer.push_back( pi_pWindow );

	pi_pWindow->SetDockingBorder( NONE_BORDER );
	pi_pWindow->SetDockingHost( this );
	pi_pWindow->EnableDocking( m_bEnableDocking );
}

// docking customer 제거
void
CGUIWindow::RemoveDockingCustomer( CGUIWindow * pi_pWindow )
{	
	if( find( m_listDockingCustomer.begin(), m_listDockingCustomer.end(), pi_pWindow )
			!= m_listDockingCustomer.end() )
		return;

	// remove
	m_listDockingCustomer.remove( pi_pWindow );

	pi_pWindow->SetDockingBorder( NONE_BORDER );
	pi_pWindow->SetDockingHost( NULL );
	pi_pWindow->EnableDocking( FALSE );
}

// docking가능한 border를 따로 설정할 때
void
CGUIWindow::EnableDockingBorder( Border pi_eBorder, BOOL pi_bEnable )
{
	m_bEnableDockingBorder[pi_eBorder] = pi_bEnable;

	if( !pi_bEnable && m_eDockingBorder == pi_eBorder )
		SetDockingBorder( NONE_BORDER );
}

// docking enable/disable
void
CGUIWindow::EnableDocking( BOOL pi_bEnable )
{
	m_bEnableDocking = pi_bEnable;

	CGUIWindowList::iterator it;
	for( it = m_listDockingCustomer.begin(); it != m_listDockingCustomer.end(); ++it )		
		(*it)->EnableDocking( m_bEnableDocking );	
}

// customer를 host의 docking border에 붙여서 docking된 상태로 만든다.
void
CGUIWindow::SetDockingBorder( Border pi_eBorder )
{
	if( !m_bEnableDocking )
		return;

	m_eDockingBorder = pi_eBorder;			

	// docking customer일 때만	
	if( m_pDockingHost )
	{
		switch( m_eDockingBorder )
		{
		case TOP:
			SetPos( m_pDockingHost->m_ptPos.x, m_pDockingHost->m_ptPos.y - m_ptSize.y );
			break;

		case BOTTOM:
			SetPos( m_pDockingHost->m_ptPos.x, m_pDockingHost->m_ptPos.y + m_pDockingHost->m_ptSize.y );
			break;

		case LEFT:
			SetPos( m_pDockingHost->m_ptPos.x - m_ptSize.x,	m_pDockingHost->m_ptPos.y );
			break;

		case RIGHT:
			SetPos( m_pDockingHost->m_ptPos.x + m_pDockingHost->m_ptSize.x,	m_pDockingHost->m_ptPos.y );
			break;
		}

		// docking bound를 update한다.
		m_pDockingHost->ResetDockingBound();	
	}
}


// docking된 customer들을 둘러싼 bound를 알아내어
// customer와 함께 하나의 window처럼 사용하기 위해..
// screen을 벗어나는지 체크할 때도 사용한다.
void
CGUIWindow::ResetDockingBound( void )
{
	m_rtDockingBound.left = m_ptPos.x;
	m_rtDockingBound.top = m_ptPos.y;
	m_rtDockingBound.right = m_ptPos.x + m_ptSize.x;
	m_rtDockingBound.bottom = m_ptPos.y + m_ptSize.y;

	CGUIWindowList::iterator it;
	for( it=m_listDockingCustomer.begin(); it != m_listDockingCustomer.end(); ++it )
	{	
		if( !(*it)->IsShow() ||
			(*it)->GetDockingBorder() == NONE_BORDER )
			continue;

		switch( (*it)->GetDockingBorder() )
		{
		case LEFT:
			if( m_ptPos.x - (*it)->m_ptSize.x < m_rtDockingBound.left )
				m_rtDockingBound.left = m_ptPos.x - (*it)->m_ptSize.x;
			break;

		case RIGHT:
			if( m_ptPos.x + m_ptSize.x + (*it)->m_ptSize.x > m_rtDockingBound.right )
				m_rtDockingBound.right = m_ptPos.x + m_ptSize.x + (*it)->m_ptSize.x;
			break;

		case TOP:
			if( m_ptPos.y - (*it)->m_ptSize.y < m_rtDockingBound.top )
				m_rtDockingBound.top = m_ptPos.y - (*it)->m_ptSize.y;
			break;

		case BOTTOM:
			if( m_ptPos.y + m_ptSize.y + (*it)->m_ptSize.y > m_rtDockingBound.bottom )
				m_rtDockingBound.bottom = m_ptPos.y + m_ptSize.y + (*it)->m_ptSize.y;
			break;
		}
	}

}

// docking이 일어나는지 체크한다.
BOOL
CGUIWindow::DetectedDocking( POINT & pi_ptPos )
{
	if( !m_bEnableDocking )
		return FALSE;

	// customer인 경우
	if( m_pDockingHost )		
	{
		// docking안된 상태이면 host와 docking되는지 체크한다.
		if( m_eDockingBorder == NONE_BORDER )
		{
			if( !m_pDockingHost->IsShow() )
				return FALSE;

			Border l_eBorder = DetectedDockingBorder( m_pDockingHost, this, pi_ptPos );

			if( l_eBorder == NONE_BORDER ||
				!m_bEnableDockingBorder[l_eBorder] )
			{
				return FALSE;
			}
			else
			{
				SetDockingBorder( l_eBorder );
				return TRUE;
			}			
		}		

		// 이미 도킹된 상태이면 undocking되는지 체크한다.
		if( !m_pDockingHost->IsShow() )
		{
			// host가 close된 상태이면 undocking된다.
			SetDockingBorder( NONE_BORDER );
			return FALSE;
		}

		Border l_eBorder = DetectedDockingBorder( m_pDockingHost, this, pi_ptPos );

		if( l_eBorder == NONE_BORDER ||
			!m_bEnableDockingBorder[l_eBorder] )
		{
			SetDockingBorder( NONE_BORDER );
			return FALSE;
		}
		
		return TRUE;		
	}
	// host인 경우
	else
	{
		// detect customer docking

		CGUIWindowList::iterator it;
		Border l_eBorder;

		for( it=m_listDockingCustomer.begin(); it != m_listDockingCustomer.end(); ++it )
		{	
			if( !(*it)->IsShow() )
				continue;

			// host를 이동해서 customer에 docking된 상태이면 
			// docking된 상태에서 undocking이 일어나는지 검사한다.

			// docking -> undocking
			if( m_bHostToCustomerDocking )
			{
				// skip undocking
				if( (*it)->GetDockingBorder() == NONE_BORDER )
					continue;

				// customer를 host로 가정하고 border를 가져온다.
				l_eBorder = DetectedDockingBorder( *it, this, pi_ptPos );

				// undocking
				if( l_eBorder == NONE_BORDER ||
					!(*it)->IsEnabledDockingBorder( (Border)((l_eBorder+2)%4) ) ) // host쪽에서보면 customer의 반대 border이다.
				{
					(*it)->SetDockingBorder( NONE_BORDER );
					
					m_bHostToCustomerDocking = FALSE;									
				}			
			}
			// undocking -> docking
			else
			{	
				// skip docking 
				if( (*it)->GetDockingBorder() != NONE_BORDER )
					continue;

				// customer를 host로 가정하고 border를 가져온다.
				l_eBorder = DetectedDockingBorder( *it, this, pi_ptPos );
				
				if( l_eBorder == NONE_BORDER ||
					!(*it)->IsEnabledDockingBorder( (Border)((l_eBorder+2)%4) ) )
					return FALSE; 
				
				// customer를 기준으로 한 border니깐 
				// host를 기준으로 한다면 반대로 생각하면 된다.
				switch( l_eBorder )
				{
				case TOP:
					SetPos( (*it)->m_ptPos.x, (*it)->m_ptPos.y - m_ptSize.y );
					(*it)->SetDockingBorder( BOTTOM );					
					break;

				case BOTTOM:
					SetPos( (*it)->m_ptPos.x, (*it)->m_ptPos.y + (*it)->m_ptSize.y );
					(*it)->SetDockingBorder( TOP );					
					break;

				case LEFT:
					SetPos( (*it)->m_ptPos.x - m_ptSize.x, (*it)->m_ptPos.y );
					(*it)->SetDockingBorder( RIGHT );					
					break;

				case RIGHT:
					SetPos( (*it)->m_ptPos.x + (*it)->m_ptSize.x, (*it)->m_ptPos.y );
					(*it)->SetDockingBorder( LEFT );
					break;
				}

				// host를 이동해서 customer에 docking된 상태이고
				// 마우스를 띄기 전까지는 undocking만 할 수 있다.
				m_bHostToCustomerDocking = TRUE;

				return TRUE;				
			}
		}
	}

	return FALSE;
}

// 실제 docking border를 찾는 함수
CGUIWindow::Border
CGUIWindow::DetectedDockingBorder( CGUIWindow * pi_pHostWin, CGUIWindow * pi_pCustomerWin, POINT & pi_ptCustMovePos )
{
	// host를 기준으로 border를 구한다.

	// 이동하는 것이 customer이고 가만히 있는 것이 host라고 생각한다.
	// pi_ptCustMovePos는 customer가 이동하려는 위치이다.

	if( abs( pi_pHostWin->m_ptPos.x - pi_ptCustMovePos.x ) < DOCKING_DETECT_DIST )				
	{
		// top
		if( //m_bEnableDockingBorder[TOP] &&
			abs( pi_ptCustMovePos.y + pi_pCustomerWin->m_ptSize.y - pi_pHostWin->m_ptPos.y ) < DOCKING_DETECT_DIST )		
			return TOP;			
		
		// bottom
		else if( //m_bEnableDockingBorder[BOTTOM] &&
				 abs( pi_pHostWin->m_ptPos.y + pi_pHostWin->m_ptSize.y - pi_ptCustMovePos.y ) < DOCKING_DETECT_DIST )
			return BOTTOM;

	}				
	else if( abs( pi_pHostWin->m_ptPos.y - pi_ptCustMovePos.y ) < DOCKING_DETECT_DIST )				
	{
		// left
		if( //m_bEnableDockingBorder[LEFT] &&
			abs( pi_ptCustMovePos.x + pi_pCustomerWin->m_ptSize.x - pi_pHostWin->m_ptPos.x ) < DOCKING_DETECT_DIST )
			return LEFT;		

		// right
		else if( //m_bEnableDockingBorder[RIGHT] &&
				 abs( pi_pHostWin->m_ptPos.x + pi_pHostWin->m_ptSize.x - pi_ptCustMovePos.x ) < DOCKING_DETECT_DIST )
			return RIGHT;		
	}

	return NONE_BORDER;
}

void
CGUIWindow::MouseEvent( _mouse_event & event )
{
	if( gScreen.ExistObject() )
		return;

	if( !( event.id == MOUSE_STROKED ||
		   event.id == MOUSE_RELEASED ||
		   event.id == MOUSE_CLICKED ) )
		return;

	if( m_bMovable &&
		find( m_listMoveObject.begin(),	m_listMoveObject.end(), event.source )
			!= m_listMoveObject.end() )
	{		
		if( event.id == MOUSE_STROKED )			
		{
			m_byCurMotion = MOVE_MOTION;			
			
			m_ptPressPos.x = event.xPos - m_ptPos.x;
			m_ptPressPos.y = event.yPos - m_ptPos.y;
			
			// init
			m_bHostToCustomerDocking = FALSE;
		}
		else // MOUSE_RELEASED, MOUSE_CLICKED
		{
			m_byCurMotion = NONE_MOTION;
		}
	}
	else if( m_bReSizable &&
			 m_bExistResizeObj )

	{
		// search resize_object		
		for( int i=0; i<4; ++i )
		{
			if( event.source == m_pResizeObject[i] )							
				break;
		}

		if( i == 4 )
			return;

		if( event.id == MOUSE_STROKED )
		{
			m_byCurMotion = RESIZE_MOTION;		

			if( event.source == m_pResizeObject[LEFT] ||
				event.source == m_pResizeObject[TOP] ) 
			{
				m_ptPressPos.x = event.xPos - m_ptPos.x;
				m_ptPressPos.y = event.yPos - m_ptPos.y;
			}
			else // right, bottom
			{
				m_ptPressPos.x = m_ptPos.x + m_ptSize.x - event.xPos;
				m_ptPressPos.y = m_ptPos.y + m_ptSize.y - event.yPos;
			}
		}
		else // MOUSE_RELEASED, MOUSE_CLICKED
		{
			m_byCurMotion = NONE_MOTION;
		}		
	}
	
}

void
CGUIWindow::MouseMotionEvent( _mouse_motion_event & event )
{
	if( event.id != MOUSE_DRAG )
		return;
	
	if( m_byCurMotion == MOVE_MOTION )
	{

		POINT l_ptPos;		
		l_ptPos.x = event.xPos - m_ptPressPos.x;
		l_ptPos.y = event.yPos - m_ptPressPos.y;		

		// docking이 일어나면 window를 이동하지 않는다.		
		if( DetectedDocking( l_ptPos ) )
			return;

		// host가 customer에 docking중이면 이동을 못하게 한다.
		if( m_bHostToCustomerDocking )
			return;

		// screen을 벗어나는지 체크한다.
		if( m_rtDockingBound.left - m_ptPos.x + l_ptPos.x < gScreen.m_ptPos.x )
			l_ptPos.x = gScreen.m_ptPos.x + m_ptPos.x - m_rtDockingBound.left;
		else if( m_rtDockingBound.right - m_ptPos.x + l_ptPos.x > gScreen.m_ptPos.x + gScreen.m_ptSize.x )
			l_ptPos.x = gScreen.m_ptPos.x + gScreen.m_ptSize.x + m_ptPos.x - m_rtDockingBound.right;

		if( m_rtDockingBound.top - m_ptPos.y + l_ptPos.y < gScreen.m_ptPos.y )
			l_ptPos.y = gScreen.m_ptPos.y + m_ptPos.y - m_rtDockingBound.top;
		else if( m_rtDockingBound.bottom - m_ptPos.y + l_ptPos.y > gScreen.m_ptPos.y + gScreen.m_ptSize.y )
			l_ptPos.y = gScreen.m_ptPos.y + gScreen.m_ptSize.y + m_ptPos.y - m_rtDockingBound.bottom;


		// window position setting
		SetPos( l_ptPos );
	}
	else if( m_byCurMotion == RESIZE_MOTION )
	{
		if( event.source == m_pResizeObject[LEFT] )
		{
			int l_nLeft;
			l_nLeft = event.xPos - m_ptPressPos.x;

			if( l_nLeft < gScreen.m_ptPos.x )
				l_nLeft = gScreen.m_ptPos.x;
			else if( l_nLeft < m_ptPos.x + m_ptSize.x - 1 )
				l_nLeft = m_ptPos.x + m_ptSize.x - 1;			

			RECT	l_rtBound;
			l_rtBound.left = l_nLeft;
			l_rtBound.top = m_ptPos.y;
			l_rtBound.right = m_ptPos.x + m_ptSize.x;
			l_rtBound.bottom = m_ptPos.y + m_ptSize.y;

			SetBound( l_rtBound );
		}
		else if( event.source == m_pResizeObject[TOP] )
		{	
			int l_nTop;
			l_nTop = event.yPos - m_ptPressPos.y;

			if( l_nTop < gScreen.m_ptPos.y )
				l_nTop = gScreen.m_ptPos.y;
			else if( l_nTop > m_ptPos.y + m_ptSize.y -1 )
				l_nTop = m_ptPos.y + m_ptSize.y - 1;

			RECT	l_rtBound;
			l_rtBound.left = m_ptPos.x;
			l_rtBound.top = l_nTop;
			l_rtBound.right = m_ptPos.x + m_ptSize.x;
			l_rtBound.bottom = m_ptPos.y + m_ptSize.y;				

			SetBound( l_rtBound );				
		}
		else if( event.source == m_pResizeObject[RIGHT] )
		{
			int l_nRight;			
			l_nRight = event.xPos + m_ptPressPos.x;

			if( l_nRight > gScreen.m_ptPos.x + gScreen.m_ptSize.x )
				l_nRight = gScreen.m_ptPos.x + gScreen.m_ptSize.x;
			else if( l_nRight < m_ptPos.x + 1 )
				l_nRight = m_ptPos.x + 1;

			RECT	l_rtBound;
			l_rtBound.left = m_ptPos.x;
			l_rtBound.top = m_ptPos.y;
			l_rtBound.right = l_nRight;
			l_rtBound.bottom = m_ptPos.y + m_ptSize.y;

			SetBound( l_rtBound );
		}
		else if( event.source == m_pResizeObject[BOTTOM] )
		{
			int l_nBottom;			
			l_nBottom = event.yPos + m_ptPressPos.y;

			if( l_nBottom > gScreen.m_ptPos.y + gScreen.m_ptSize.y )
				l_nBottom = gScreen.m_ptPos.y + gScreen.m_ptSize.y;
			else if( l_nBottom < m_ptPos.y + 1 )
				l_nBottom = m_ptPos.y + 1;

			RECT	l_rtBound;
			l_rtBound.left = m_ptPos.x;
			l_rtBound.top = m_ptPos.y;
			l_rtBound.right = m_ptPos.x + m_ptSize.x;
			l_rtBound.bottom = l_nBottom;

			SetBound( l_rtBound );
		}	
	}
}

void
CGUIWindow::KeyEvent( _key_event & event )
{

}

void
CGUIWindow::PickingObjectEvent( BOOL pi_bPick )
{

}

void
CGUIWindow::Update( void )
{
	for( CGUIDialogBoxList::iterator i = m_listDialogBox.begin(); i != m_listDialogBox.end(); ++i )
	{
		(*i)->Update();
	}

	CGUIContainer::Update();	
}

BOOL
CGUIWindow::Draw( void )
{
	CGUIContainer::Draw();	

	DrawDialogBox();	

	return true;
}

void
CGUIWindow::DrawDialogBox( void )
{
	if( !m_listDialogBox.empty() )
	{
		for( CGUIDialogBoxList::iterator i = m_listDialogBox.begin(); i != m_listDialogBox.end(); ++i )
		{
			(*i)->Draw();
		}
	}
}

BOOL
CGUIWindow::InputProc( void )
{
	if( !m_listDialogBox.empty() )
	{
		CGUIDialogBoxList::reverse_iterator rit;	

		for( rit=m_listDialogBox.rbegin(); rit!=m_listDialogBox.rend(); ++rit )
		{
			if( (*rit)->IsModal() || (*rit)->IsLockOtherWin() )
				return (*rit)->InputProc();
			else 	
				(*rit)->InputProc();
		}		
	}

	if( m_pWheelScrollObject &&
		!CGUIObject::GetMouseDetectedObject() )
	{
		if( gMouseDev.IsWheelScrolled() &&
			IsCrossed( gMouseDev.GetX(), gMouseDev.GetY() ) )
		{
			if( gMouseDev.GetZ() > 0 )
			{
				m_pWheelScrollObject->SetCurrentAmount( m_pWheelScrollObject->GetCurrentAmount() - 
											m_pWheelScrollObject->GetAdjustmentUnit() * gMouseDev.GetZ()/120 );
				m_pWheelScrollObject->SendScrollEvent( SCROLLED_UP );
			}
			else 
			{
				m_pWheelScrollObject->SetCurrentAmount( m_pWheelScrollObject->GetCurrentAmount() - 
											m_pWheelScrollObject->GetAdjustmentUnit() * gMouseDev.GetZ()/120 );
				m_pWheelScrollObject->SendScrollEvent( SCROLLED_DOWN );
			}
		}
	}
		
	return CGUIContainer::InputProc();
}

LRESULT
CGUIWindow::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !m_listDialogBox.empty() )
	{		
		CGUIDialogBoxList::reverse_iterator i;
		i = m_listDialogBox.rbegin();

		CGUIDialogBox * l_pDlg;
		l_pDlg = *i;
		return l_pDlg->MsgProc( hWnd, uMsg, wParam, lParam );
	}
	else 
	{
		if( !IsActive() )
			return 0;

		CGUIContainer::MsgProc( hWnd, uMsg, wParam, lParam );
	}

	return 1;
}

void
CGUIWindow::ClosingDialogBox( CGUIDialogBox * pi_pDlg )
{
	pi_pDlg->RemoveOwnerWindow();

	pi_pDlg->Close();

//	RemoveDialogBox( pi_pDlg );	


	// ################## 코드 추가 시킬 곳 #################
	EP_CloseDialogBox( pi_pDlg->GetOwnerWindow() );
	// ######################################################

	if( pi_pDlg->IsDestroy() )
		delete pi_pDlg;

	// owner window를 active한다.
	ActiveWindow( this );
}

void
CGUIWindow::SendWindowEvent( WORD pi_wID )
{
	_event_queue_data	l_EventInfo;

	l_EventInfo.pEventHandler = this;
	l_EventInfo.pSource = this;
	l_EventInfo.wEventID = pi_wID;
	
	CGUIObject::SendEvent( l_EventInfo );
}

BOOL
CGUIWindow::PostUIMessage( UINT pi_uMsg, UI_MOUSE_MSG & pi_stMouseMsg )
{
	return PostMessage( gScreen.GetHWND(), pi_uMsg, m_dwID, pi_stMouseMsg.m_dwData );
}

BOOL
CGUIWindow::PostUIMessage( UINT pi_uMsg, LPARAM pi_lParam )
{
	return PostMessage( gScreen.GetHWND(), pi_uMsg, m_dwID, pi_lParam );
}