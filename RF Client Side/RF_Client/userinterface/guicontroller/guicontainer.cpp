////////////////////////////////////////////////////////////////////////////
//
// CGUIContainer Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "GUIContainer.h"
#include "../ConsoleWin.h"

////////////////////////////////////////////////////////////////////////////
//
CGUIContainer::CGUIContainer()
{
	
}

CGUIContainer::~CGUIContainer()
{
	RemoveAll();
}

void
CGUIContainer::Add( CGUIObject * pi_pObj )
{
	// 중복 add하지 않는다.
	if( find( m_listObject.begin(), m_listObject.end(), pi_pObj ) != m_listObject.end() )
		return;
	
	m_listObject.push_back( pi_pObj );
}

int
CGUIContainer::Add( CGUIObject * pi_pObj, WORD pi_wAddIndex )
{
	Remove( pi_pObj );


	WORD l_wAddIndex;

	if( m_listObject.size() == 0 )
		l_wAddIndex = 0;
	else if( pi_wAddIndex > m_listObject.size() )
		l_wAddIndex = m_listObject.size() - 1;
	else 
		l_wAddIndex = pi_wAddIndex;	

	WORD l_wIndexCnt = 0;
	for( CGUIObjectList::iterator it = m_listObject.begin(); it != m_listObject.end(); ++it, ++l_wIndexCnt )
	{
		if( l_wIndexCnt == l_wAddIndex )			
			break;		
	}

	m_listObject.insert( it, pi_pObj );

	return l_wAddIndex;
}

BOOL
CGUIContainer::Remove( CGUIObject * pi_pObj )
{
	if( find( m_listObject.begin(), m_listObject.end(), pi_pObj ) == m_listObject.end() )
		return false;

	// remove object
	m_listObject.remove( pi_pObj );


	if( pi_pObj == s_pMouseDetectedObject )
	{
		pi_pObj->SetState( pi_pObj->GetState() & ~HOVERED );
		pi_pObj->SendMouseEvent( MOUSE_LEAVED );
		s_pMouseDetectedObject = NULL;
	}

	if( s_pMouseDetectedObject != s_pPrevMouseDetectedObject &&
		pi_pObj == s_pPrevMouseDetectedObject )
	{
		pi_pObj->SetState( pi_pObj->GetState() & ~HOVERED );
		pi_pObj->SendMouseEvent( MOUSE_LEAVED );
		s_pPrevMouseDetectedObject = NULL;
	}

	if( pi_pObj == s_pPressedObject )
	{
		s_pPressedObject = NULL;

		if( gMouseDev.IsPressed( L_BUTTON ) )
			SetPressedOtherObject();
	}

	if( pi_pObj == s_pFocusedObject )
	{
		s_pFocusedObject = NULL;
	}
	
	return true;
}

void
CGUIContainer::RemoveAll( void )
{
	if( find( m_listObject.begin(), m_listObject.end(), s_pMouseDetectedObject ) != m_listObject.end() )
	{				
		if( s_pPrevMouseDetectedObject == s_pMouseDetectedObject )
		{
			s_pPrevMouseDetectedObject = NULL;
		}
		if( s_pPressedObject == s_pMouseDetectedObject )
		{
			s_pPressedObject->SetState( s_pPressedObject->GetState() & ~PRESSED );
			s_pPressedObject = NULL;
		}
		if( s_pFocusedObject == s_pMouseDetectedObject )
		{
			s_pFocusedObject = NULL;
		}

		s_pMouseDetectedObject->SendMouseEvent( MOUSE_LEAVED );
		s_pMouseDetectedObject = NULL;
	}

	if( s_pPrevMouseDetectedObject &&
		find( m_listObject.begin(), m_listObject.end(), s_pPrevMouseDetectedObject ) != m_listObject.end() )
	{
		s_pPrevMouseDetectedObject->SendMouseEvent( MOUSE_LEAVED );
		s_pPrevMouseDetectedObject = NULL;
	}	
	if( s_pPressedObject &&
		find( m_listObject.begin(), m_listObject.end(), s_pPressedObject ) != m_listObject.end() )
	{
		s_pPressedObject->SetState( s_pPressedObject->GetState() & ~PRESSED );
		s_pPressedObject = NULL;

		if( gMouseDev.IsPressed( L_BUTTON ) )
			SetPressedOtherObject();
	}	
	if( s_pFocusedObject &&
		find( m_listObject.begin(), m_listObject.end(), s_pPressedObject ) != m_listObject.end() )
	{
		s_pFocusedObject = NULL;
	}

	m_listObject.clear();
}

CGUIObject	*
CGUIContainer::GetAddedObject( WORD pi_wIndex )
{
	WORD l_wIndex;
	l_wIndex = 0;
	for( CGUIObjectList::iterator i = m_listObject.begin(); i != m_listObject.end(); ++i )
	{
		if( l_wIndex == pi_wIndex )
			return *i;
		++l_wIndex;
	}

	return NULL;
}

BOOL
CGUIContainer::IsAddedObject(CGUIObject * pi_pObj )
{
	if( find( m_listObject.begin(), m_listObject.end(), pi_pObj ) != m_listObject.end() )
		return true;

	return false;
}

int
CGUIContainer::GetAddedObjectIndex( CGUIObject * pi_pObj )
{	
	int l_nIndex;
	l_nIndex = 0;

	for( CGUIObjectList::iterator i = m_listObject.begin(); i != m_listObject.end(); ++i )
	{
		if( *i == pi_pObj )
			return l_nIndex;

		++l_nIndex;
	}

	// 없으면 -1 리턴
	return -1;
}

void
CGUIContainer::SetPos( POINT & pi_ptPos )
{
	CGUIObject	* l_pObj = NULL;
	POINT		l_ptPos;

	for( CGUIObjectList::iterator i = m_listObject.begin(); i != m_listObject.end(); ++i )
	{
		l_pObj = *i;	
		l_pObj->GetPos( l_ptPos );
		l_ptPos.x = pi_ptPos.x + l_ptPos.x - m_ptPos.x;
		l_ptPos.y = pi_ptPos.y + l_ptPos.y - m_ptPos.y;

		l_pObj->SetPos( l_ptPos );
	}
	
	m_ptPos.x = pi_ptPos.x;
	m_ptPos.y = pi_ptPos.y;
}

void
CGUIContainer::SetPos( LONG pi_nPosX, LONG pi_nPosY )
{	
	CGUIObject	* l_pObj = NULL;
	POINT		l_ptPos;

	for( CGUIObjectList::iterator i = m_listObject.begin(); i != m_listObject.end(); ++i )
	{
		l_pObj = *i;	
		l_pObj->GetPos( l_ptPos );		

		l_pObj->SetPos( pi_nPosX + l_ptPos.x - m_ptPos.x,
						pi_nPosY + l_ptPos.y - m_ptPos.y );
	}
	
	m_ptPos.x = pi_nPosX;
	m_ptPos.y = pi_nPosY;
}

void
CGUIContainer::SetBound( RECT & pi_bound )
{
	CGUIObject	* l_pObj = NULL;
	RECT		l_rtObjBound;
	
	for( CGUIObjectList::iterator i = m_listObject.begin(); i != m_listObject.end(); ++i )
	{
		l_pObj = *i;
		l_rtObjBound.left = pi_bound.left + l_pObj->m_ptPos.x - m_ptPos.x;
		l_rtObjBound.top = pi_bound.top + l_pObj->m_ptPos.y - m_ptPos.y;
		l_rtObjBound.right = pi_bound.right + 
							( l_pObj->m_ptPos.x + l_pObj->m_ptSize.x ) - ( m_ptPos.x + m_ptSize.x );
		l_rtObjBound.bottom = pi_bound.bottom + 
							( l_pObj->m_ptPos.y + l_pObj->m_ptSize.y ) - ( m_ptPos.y + m_ptSize.y );		
			
		l_pObj->SetBound( l_rtObjBound );
	}

	CGUIObject::SetBound( pi_bound );
}

BOOL
CGUIContainer::InputProc()
{	
	if( !IsCrossed( gMouseDev.GetX(), gMouseDev.GetY() ) )
		return FALSE;

	// 맨 마지막에 등록된 object가 최상위 layer이기 때문에
	// 역순으로 처리한다.	
	CGUIObjectList::reverse_iterator i;
	CGUIObject * l_pObj;
	for( i = m_listObject.rbegin(); i != m_listObject.rend(); ++i )
	{
		l_pObj = *i;
		if( (*i)->InputProc() )
			return TRUE;
	}
	
	return CGUIObject::InputProc();
}

LRESULT
CGUIContainer::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	CGUIObject * l_pObj;

	CGUIObjectList::reverse_iterator i;	
	for( i = m_listObject.rbegin(); i != m_listObject.rend(); ++i )
	{
		//(*i)->MsgProc( hWnd, uMsg, wParam, lParam );
		l_pObj = *i;
		if( l_pObj->MsgProc( hWnd, uMsg, wParam, lParam ) )
			return 1;
	}
	
	return CGUIObject::MsgProc( hWnd, uMsg, wParam, lParam );
}

void
CGUIContainer::Active( void )
{	

	for( CGUIObjectList::iterator i = m_listObject.begin(); i != m_listObject.end(); ++i )
	{
		(*i)->Active();
	}	

	CGUIObject::Active();
}

void
CGUIContainer::InActive( void )
{
	for( CGUIObjectList::iterator i = m_listObject.begin(); i != m_listObject.end(); ++i )
	{		
		(*i)->InActive();
	}

	CGUIObject::InActive();
}

BOOL
CGUIContainer::Draw()
{
	if( !CGUIObject::Draw() )
		return FALSE;

	CGUIObject * l_pObj;

	for( CGUIObjectList::iterator i = m_listObject.begin(); i != m_listObject.end(); ++i )
	{
		l_pObj = *i;
		l_pObj->Draw();
	}	
	
	return TRUE;
}

BOOL
CGUIContainer::Draw( RECT & pi_rtClipping )
{
	if( !CGUIObject::Draw( pi_rtClipping ) )
		return TRUE;

	CGUIObject * l_pObj;
	RECT	l_rtClipping;

	for( CGUIObjectList::iterator i = m_listObject.begin(); i != m_listObject.end(); ++i )
	{
		l_pObj = *i;
		
		// left
		if( l_pObj->m_ptPos.x < pi_rtClipping.left )
		{
			// container 영역 밖
			if( l_pObj->m_ptPos.x + l_pObj->m_ptSize.x < pi_rtClipping.left )
				continue;

			l_rtClipping.left = pi_rtClipping.left;
		}
		else
		{
			// container 영역 밖
			if( l_pObj->m_ptPos.x >= pi_rtClipping.right )
				continue;

			l_rtClipping.left = l_pObj->m_ptPos.x;
		}

		// right
		if( l_pObj->m_ptPos.x + l_pObj->m_ptSize.x > pi_rtClipping.right )
			l_rtClipping.right = pi_rtClipping.right;
		else 
			l_rtClipping.right = l_pObj->m_ptPos.x + l_pObj->m_ptSize.x;


		// top
		if( l_pObj->m_ptPos.y < pi_rtClipping.top )
		{
			// container 영역 밖
			if( l_pObj->m_ptPos.y + l_pObj->m_ptSize.y < pi_rtClipping.top )
				continue;

			l_rtClipping.top = pi_rtClipping.top;
		}
		else
		{
			// container 영역 밖
			if( l_pObj->m_ptPos.y >= pi_rtClipping.bottom )
				continue;

			l_rtClipping.top = l_pObj->m_ptPos.y;
		}

		// bottom
		if( l_pObj->m_ptPos.y + l_pObj->m_ptSize.y > pi_rtClipping.bottom )
			l_rtClipping.bottom = pi_rtClipping.bottom;
		else 
			l_rtClipping.bottom = l_pObj->m_ptPos.y + l_pObj->m_ptSize.y;

		// draw
		l_pObj->Draw( l_rtClipping );
	}

	return TRUE;	
}

void
CGUIContainer::Update( void )
{
	CGUIObject::Update();

	CGUIObject * l_pObj;
	for( CGUIObjectList::iterator i = m_listObject.begin(); i != m_listObject.end(); ++i )
	{
		l_pObj = *i;
		(*i)->Update();		
	}
}

void
CGUIContainer::Show( BOOL pi_bShow )
{
	CGUIObject::Show( pi_bShow );

	CGUIObject * l_pObj;
	for( CGUIObjectList::iterator i = m_listObject.begin(); i != m_listObject.end(); ++i )
	{
		l_pObj = *i;		
		(*i)->Show( pi_bShow );		
	}	
}

void
CGUIContainer::Enable( void )
{
	CGUIObject::Enable();

	for( CGUIObjectList::iterator i = m_listObject.begin(); i != m_listObject.end(); ++i )
	{
		(*i)->Enable();		
	}	
}

void
CGUIContainer::Disable( void )
{
	CGUIObject::Disable();

	for( CGUIObjectList::iterator i = m_listObject.begin(); i != m_listObject.end(); ++i )
	{
		(*i)->Disable();		
	}	
}

void
CGUIContainer::EnableFocus( void )
{
	CGUIObject::EnableFocus();

	for( CGUIObjectList::iterator i = m_listObject.begin(); i != m_listObject.end(); ++i )
	{
		(*i)->EnableFocus();
	}
}

void
CGUIContainer::DisableFocus( void )
{
	CGUIObject::DisableFocus();

	for( CGUIObjectList::iterator i = m_listObject.begin(); i != m_listObject.end(); ++i )
	{
		(*i)->DisableFocus();
	}
}

void
CGUIContainer::SetSprite( void )
{
	for( CGUIObjectList::iterator i = m_listObject.begin(); i != m_listObject.end(); ++i )
	{				
		(*i)->SetSprite();
	}
}

////////////////////////////////////////////////////////////////////////////
//
// CGUIContainerEx Class Implementation
//
////////////////////////////////////////////////////////////////////////////

CGUIContainerEx::CGUIContainerEx()
{
	
}

CGUIContainerEx::~CGUIContainerEx()
{
	
}

BOOL
CGUIContainerEx::Draw( void )
{
	if( !IsShow() )
		return FALSE;

	RECT l_rtClippingArea;

	l_rtClippingArea.left = m_ptPos.x;
	l_rtClippingArea.right = m_ptPos.x + m_ptSize.x;

	l_rtClippingArea.top = m_ptPos.y;
	l_rtClippingArea.bottom = m_ptPos.y + m_ptSize.y;

	CGUIContainer::Draw( l_rtClippingArea );

	return TRUE;
}

