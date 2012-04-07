////////////////////////////////////////////////////////////////////////////
//
// CGUIObject Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "GUIObject.h"
#include "GUIWindow.h"
#include "CommonUtil.h"
#include "GUIScreen.h"

CGUIObject	* CGUIObject::s_pMouseDetectedObject		= NULL;
CGUIObject	* CGUIObject::s_pPrevMouseDetectedObject	= NULL;
CGUIObject	* CGUIObject::s_pPressedObject				= NULL;
CGUIObject	* CGUIObject::s_pFocusedObject				= NULL;
BOOL		  CGUIObject::s_bIsPressedOtherObject		= FALSE;

CNDQueue< _event_queue_data >	CGUIObject::s_qEventList;

CGUIObject::CGUIObject()
{	
	m_ptPos.x		= 0;
	m_ptPos.y		= 0;

	m_ptSize.x		= 0;
	m_ptSize.y		= 0;	
	
	m_wState		= 0;

	m_bShow			= FALSE;
	m_bActive		= FALSE;

	m_pName[0]		= '\0';

	m_dwARGB		= 0xffffffff;

	m_spriteInfo.pSprite	= NULL;		

	m_pMouseObserver		= NULL;
	m_pMouseMotionObserver	= NULL;
	m_pKeyObserver			= NULL;

	m_bEnableFocus			= TRUE;	
}

CGUIObject::~CGUIObject()
{
	if( s_pFocusedObject == this )
		s_pFocusedObject = NULL;
	
	if( s_pMouseDetectedObject == this )
		s_pMouseDetectedObject = NULL;

	if( s_pPrevMouseDetectedObject == this )
		s_pPrevMouseDetectedObject = NULL;

	if( s_pPressedObject == this )
	{
		s_pPressedObject = NULL;

		if( gMouseDev.IsPressed( L_BUTTON ) )
			SetPressedOtherObject();
	}

	RemoveEvent( this );
}

void
CGUIObject::SetName( char * pi_pName )
{
	strncpy( m_pName, pi_pName, 15 );
	m_pName[15] = '\0';
}

void
CGUIObject::SetPos( LONG pi_nPosX, LONG pi_nPosY )
{
	m_ptPos.x = pi_nPosX;
	m_ptPos.y = pi_nPosY;
}

void
CGUIObject::SetPos( POINT & pi_ptPos )
{
	m_ptPos.x = pi_ptPos.x;
	m_ptPos.y = pi_ptPos.y;
}

void
CGUIObject::SetSize( LONG pi_nWidth, LONG pi_nHeight )
{
	m_ptSize.x = pi_nWidth; 
	m_ptSize.y = pi_nHeight;	
}

void
CGUIObject::SetSize( POINT & pi_ptSize )
{
	m_ptSize.x = pi_ptSize.x; 
	m_ptSize.y = pi_ptSize.y;
}

void
CGUIObject::SetBound( RECT & pi_rtBound )
{	
	m_ptPos.x = pi_rtBound.left;
	m_ptPos.y = pi_rtBound.top;
	m_ptSize.x = pi_rtBound.right - pi_rtBound.left;
	m_ptSize.y = pi_rtBound.bottom - pi_rtBound.top;	
}

void
CGUIObject::GetBound( RECT & po_rtBound )
{
	po_rtBound.left = m_ptPos.x;
	po_rtBound.top = m_ptPos.y;
	po_rtBound.right = m_ptPos.x + m_ptSize.x;
	po_rtBound.bottom = m_ptPos.y + m_ptSize.y;
}

void
CGUIObject::SetSpriteInfo( SPRITE_INFO * pi_pSpriteInfo )
{
	if( !pi_pSpriteInfo )
	{
		m_spriteInfo.pSprite = NULL;
		m_spriteInfo.wActionNo = 0;
		m_spriteInfo.wFrameNo = 0;
	}
	else
	{
		memcpy( &m_spriteInfo, pi_pSpriteInfo, sizeof( SPRITE_INFO ) );
	}
}

void
CGUIObject::RemoveSpriteInfo()
{
	m_spriteInfo.pSprite = NULL;
}

void
CGUIObject::Update()
{	
}

BOOL 
CGUIObject::Draw()
{
	if( !m_bShow )
		return FALSE;

	if( NULL == m_spriteInfo.pSprite )
	{
		if( m_dwARGB != 0xFFFFFFFF )
			Draw2DRectangle( m_ptPos.x, m_ptPos.y,
							 m_ptPos.x + m_ptSize.x, 
							 m_ptPos.y + m_ptSize.y,
							 m_dwARGB );
		return TRUE;
	}
	
	if( m_spriteInfo.pSprite->IsPageSprite() )
	{		
		m_spriteInfo.pSprite->SetPage( m_spriteInfo.wActionNo >> 8 );
		m_spriteInfo.pSprite->SetAction( m_spriteInfo.wActionNo & 0x00FF );
		m_spriteInfo.pSprite->SetFrame( m_spriteInfo.wFrameNo );

		m_spriteInfo.pSprite->DrawSprite( m_ptPos.x,
										  m_ptPos.y,
										  m_ptPos.x + m_ptSize.x,
										  m_ptPos.y + m_ptSize.y,
										  m_dwARGB);
	}
	else
	{
		m_spriteInfo.pSprite->SetAction( m_spriteInfo.wActionNo );
		m_spriteInfo.pSprite->SetFrame( m_spriteInfo.wFrameNo );

		m_spriteInfo.pSprite->DrawSprite( m_ptPos.x, 
										  m_ptPos.y,
										  m_ptPos.x + m_ptSize.x, 
										  m_ptPos.y + m_ptSize.y,
										  m_dwARGB );
	}

	return TRUE;
}

BOOL
CGUIObject::Draw( RECT & pi_rtClipping )
{
	if( !m_bShow )
		return FALSE;

	if( NULL == m_spriteInfo.pSprite )
	{
		if( m_dwARGB != 0xffffffff )		
			Draw2DRectangle( pi_rtClipping.left,
							 pi_rtClipping.top,
							 pi_rtClipping.right,
							 pi_rtClipping.bottom,							 
							 m_dwARGB );
		return TRUE;
	}

	float l_fUV[2][2];

	if( m_spriteInfo.pSprite->IsPageSprite() )
	{
		m_spriteInfo.pSprite->SetPage( m_spriteInfo.wActionNo >> 8 );
		m_spriteInfo.pSprite->SetAction( m_spriteInfo.wActionNo & 0x00FF );
		m_spriteInfo.pSprite->SetFrame( m_spriteInfo.wFrameNo );

		float l_fUFrag = (float) m_spriteInfo.pSprite->GetPageElementXL() / m_spriteInfo.pSprite->GetSpriteXL();
		float l_fVFrag = (float) m_spriteInfo.pSprite->GetPageElementYL() / m_spriteInfo.pSprite->GetSpriteYL();

		l_fUV[0][0] = ( (m_spriteInfo.wActionNo&0x00FF) + (float)(pi_rtClipping.left - m_ptPos.x) / m_ptSize.x ) * l_fUFrag;
		
		l_fUV[0][1] = l_fUV[0][0] + (float)(pi_rtClipping.right - pi_rtClipping.left) / m_ptSize.x * l_fUFrag;

		l_fUV[1][0] = ( m_spriteInfo.wFrameNo + (float)(pi_rtClipping.top - m_ptPos.y) / m_ptSize.y ) * l_fVFrag;
		
		l_fUV[1][1] = l_fUV[1][0] + (float)(pi_rtClipping.bottom - pi_rtClipping.top) / m_ptSize.y * l_fVFrag;
	}	
	else
	{
		m_spriteInfo.pSprite->SetAction( m_spriteInfo.wActionNo );
		m_spriteInfo.pSprite->SetFrame( m_spriteInfo.wFrameNo );

		float l_fUFrag = (float)m_spriteInfo.pSprite->GetSpriteOrgXL() / m_spriteInfo.pSprite->GetSpriteXL();
		float l_fVFrag = (float)m_spriteInfo.pSprite->GetSpriteOrgYL() / m_spriteInfo.pSprite->GetSpriteYL();

		l_fUV[0][0] = (float)( pi_rtClipping.left - m_ptPos.x ) / m_ptSize.x * l_fUFrag;
					  
		l_fUV[0][1] = (float)( pi_rtClipping.right - m_ptPos.x ) / m_ptSize.x * l_fUFrag;					  

		l_fUV[1][0] = (float)( pi_rtClipping.top - m_ptPos.y ) / m_ptSize.y * l_fVFrag;
					  
		l_fUV[1][1] = (float)( pi_rtClipping.bottom - m_ptPos.y ) / m_ptSize.y * l_fVFrag;					  
	}	

	m_spriteInfo.pSprite->DrawSpriteUV( pi_rtClipping.left,
										pi_rtClipping.top,
										l_fUV,
										pi_rtClipping.right,
										pi_rtClipping.bottom,
										m_dwARGB );

	return TRUE;
}

BOOL
CGUIObject::InputProc( void )
{	
	if( s_pMouseDetectedObject )
		return FALSE;

	if( !IsCrossed( gMouseDev.GetX(), gMouseDev.GetY() ) )
		return FALSE;

			
	// 처음으로 감지된 object를 s_pMouseDetectedObject으로 잡는다.	
	// set s_pMouseDetectedObject
	s_pMouseDetectedObject = this;

	// -------------------------------------------------------------
	// 먼저 이전에 mouse event가 일어났던 object부터 처리한다.
	if( !s_pPrevMouseDetectedObject )
	{
		s_pPrevMouseDetectedObject = s_pMouseDetectedObject;
	}
	else if( s_pPrevMouseDetectedObject != s_pMouseDetectedObject )
	{
		WORD l_wState = s_pPrevMouseDetectedObject->GetState();

		// pressed와 상관없이 이전에 마우스가 object안에 있다가
		// 나갔으면 MOUSE_LEAVED msg를 보낸다.
		if( IS_HOVERED( l_wState ) )
		{
			// leaved
			s_pPrevMouseDetectedObject->SetState( l_wState & ~HOVERED );
			s_pPrevMouseDetectedObject->SendMouseEvent( MOUSE_LEAVED );			
		}

		s_pPrevMouseDetectedObject = s_pMouseDetectedObject;
	}

	if( s_pPressedObject &&
		s_pPressedObject != s_pMouseDetectedObject )
	{
		WORD l_wState = s_pPressedObject->GetState();

		if( IS_PRESSED( l_wState ) )
		{
			if( !gMouseDev.IsPressed( L_BUTTON ) ||
				gMouseDev.IsReleased( L_BUTTON ) )
			{
				// not press
				s_pPressedObject->SetState( l_wState & ~PRESSED );			
				s_pPressedObject->SendMouseEvent( MOUSE_RELEASED );

				s_pPressedObject = NULL;				
			}
			else if( gMouseDev.IsPressed( L_BUTTON ) &&
					 gMouseDev.IsDragged() )
			{				
				// drag
				s_pPressedObject->SendMouseMotionEvent( MOUSE_DRAG );			
			}
		}		
	}
	else if( s_bIsPressedOtherObject )
	{
		if( gMouseDev.IsPressed( L_BUTTON ) )
			return TRUE;
		else
			s_bIsPressedOtherObject = FALSE;			
	}



	// ---------------------------------------------------------
	// 일단 오른쪽 마우스에 대한것은 stroke만 처리한다.
	if( !gMouseDev.IsPressed( L_BUTTON ) &&
		gMouseDev.IsStroked( R_BUTTON ) )
	{
		SendMouseEvent( LMOUSE_STROKED );			
	}
	
	if( !IS_PRESSED( m_wState ) &&
		gMouseDev.IsStroked( L_BUTTON ) )
	{
		if( !s_pPressedObject )
		{
			// stroked
			SetState( m_wState | (PRESSED | HOVERED) );
			SendMouseEvent( MOUSE_STROKED );

			// set pressed object
			s_pPressedObject = this;			
		}
	}
	else if( IS_PRESSED( m_wState ) &&
			 gMouseDev.IsPressed( L_BUTTON ) )
	{			
		if( !IS_HOVERED( m_wState ) )
		{
			SetState( m_wState | HOVERED );
		}

		// send press msg, drag
		SendMouseEvent( MOUSE_PRESSED );

		if( gMouseDev.IsDragged() )
		{
			SendMouseMotionEvent( MOUSE_DRAG );
		}
	}
	else if( !gMouseDev.IsPressed( L_BUTTON ) )
	{			
		if( !IS_HOVERED( m_wState ) && IS_PRESSED( m_wState ) )
		{
			// hovered and released
			SetState( (m_wState & ~PRESSED) | HOVERED );
			//SendMouseEvent( MOUSE_HOVERED | MOUSE_RELEASED );
			// object안에서 mouse를 띄면 mouse_clicked
			SendMouseEvent( MOUSE_CLICKED );

			s_pPressedObject = NULL;
		}			
		else if( !IS_HOVERED( m_wState ) )
		{					
			// hovered
			SetState( HOVERED );
			SendMouseEvent( MOUSE_HOVERED );
		}			
		else if( IS_PRESSED( m_wState ) )
		{
			// released
			SetState( m_wState & ~PRESSED );
			//SendMouseEvent( MOUSE_RELEASED );
			// object안에서 mouse를 띄면 mouse_clicked
			SendMouseEvent( MOUSE_CLICKED );

			s_pPressedObject = NULL;
		}

		// move
		if( gMouseDev.IsMoved() )
		{
			SendMouseMotionEvent( MOUSE_MOVE );
		}		
			
	}	

	return TRUE;
}

void
CGUIObject::SendMouseEvent( WORD pi_wID )
{
	// remove tooltip
	if( pi_wID == MOUSE_LEAVED ||
		pi_wID == MOUSE_STROKED ||
		pi_wID == LMOUSE_STROKED )
		gScreen.RemoveTooltip();

	// disable일 때는 MOUSE_HOVERED, MOUSE_LEAVED만 보낸다.
	if( ( m_wState & DISABLED ) &&
		!( pi_wID == MOUSE_HOVERED || pi_wID == MOUSE_LEAVED ) )
		return;

	if( m_pMouseObserver )
	{
		_event_queue_data	l_EventInfo;

		l_EventInfo.pEventHandler = m_pMouseObserver;
		l_EventInfo.pSource = this;
		l_EventInfo.wEventID = pi_wID;
		l_EventInfo.nPosX = gMouse.x;
		l_EventInfo.nPosY = gMouse.y;		
		
		//_SendEvent( l_EventInfo );
		SendEvent( l_EventInfo );
	}
}

void
CGUIObject::SendMouseMotionEvent( WORD pi_wID )
{
	if( m_wState & DISABLED )
		return;

	if( m_pMouseMotionObserver )				
	{
		_event_queue_data	l_EventInfo;

		l_EventInfo.pEventHandler = m_pMouseMotionObserver;
		l_EventInfo.pSource = this;
		l_EventInfo.wEventID = pi_wID;
		l_EventInfo.nPosX = gMouse.x;
		l_EventInfo.nPosY = gMouse.y;
		//l_EventInfo.nMoveX = gMouse.x - gMouse.oldx;
		//l_EventInfo.nMoveY = gMouse.y - gMouse.oldy;		
		l_EventInfo.nMoveX = gMouseDev.GetMoveX();
		l_EventInfo.nMoveY = gMouseDev.GetMoveY();
		
		SendEvent( l_EventInfo );
	}
}

LRESULT
CGUIObject::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !m_bShow )
		return 0;

	if( s_pFocusedObject != this )
		return 0;	

	if( !m_pKeyObserver )
		return 1;	

	
	switch( uMsg )
	{
	case WM_KEYDOWN:
		{
			if( wParam == VK_RETURN ||
				wParam == VK_TAB )
			{
				// send key event
				_event_queue_data	l_EventInfo;

				l_EventInfo.pEventHandler = m_pKeyObserver;
				l_EventInfo.pSource = this;
				l_EventInfo.wEventID = KEY_STROKED;
				l_EventInfo.byKeyCode = wParam;

				SendEvent( l_EventInfo );
			}
		}
		break;
	}

	return 1;
}

BOOL
CGUIObject::IsCrossed( LONG pi_nXPos, LONG pi_nYPos )
{
	if( !m_bShow )
		return FALSE;

	if( pi_nXPos >= m_ptPos.x && pi_nXPos <= m_ptPos.x + m_ptSize.x &&
		pi_nYPos >= m_ptPos.y && pi_nYPos <= m_ptPos.y + m_ptSize.y )
		return TRUE;

	return FALSE;
}

void
CGUIObject::Enable( void )
{
	m_wState &= ~DISABLED;
	SetState( m_wState );
}

void
CGUIObject::Disable( void )
{ 
	SetState( m_wState | DISABLED );
}

void
CGUIObject::Active( void )
{
	m_bActive = TRUE;
}

void
CGUIObject::InActive( void )
{
	m_bActive = FALSE;
}

void
CGUIObject::EnableFocus( void )
{
	m_bEnableFocus = TRUE;
}

void
CGUIObject::DisableFocus( void )
{
	m_bEnableFocus = FALSE;
	
	if( s_pFocusedObject == this )
		s_pFocusedObject = NULL;
}

////////////////////////////////////////////////////////////////////////////
//
// CGUIObjectEx Class Implementation
//
////////////////////////////////////////////////////////////////////////////
CGUIObjectEx::CGUIObjectEx()
{
	m_fSpriteUV[0][0] = 0.f;
	m_fSpriteUV[0][1] = 1.f;
	m_fSpriteUV[1][0] = 0.f;
	m_fSpriteUV[1][1] = 1.f;

	m_fRotate	= 0.f;
}
CGUIObjectEx::~CGUIObjectEx()
{

}

void
CGUIObjectEx::SetSpriteUV( float pi_fUV[2][2] )
{
	m_fSpriteUV[0][0] = pi_fUV[0][0];
	m_fSpriteUV[0][1] = pi_fUV[0][1];
	m_fSpriteUV[1][0] = pi_fUV[1][0];
	m_fSpriteUV[1][1] = pi_fUV[1][1];
}

void
CGUIObjectEx::SetSpriteU( float pi_fU0, float pi_fU1 )
{
	if( !m_spriteInfo.pSprite )
		return;

	m_spriteInfo.pSprite->SetAction( m_spriteInfo.wActionNo );
	m_spriteInfo.pSprite->SetFrame( m_spriteInfo.wFrameNo );

	float l_fMaxSize = (float)m_spriteInfo.pSprite->GetSpriteOrgXL() / m_spriteInfo.pSprite->GetSpriteXL();

	m_fSpriteUV[0][0] = l_fMaxSize * pi_fU0;
	m_fSpriteUV[0][1] = l_fMaxSize * pi_fU1;
}

void
CGUIObjectEx::SetSpriteV( float pi_fV0, float pi_fV1 )
{
	if( !m_spriteInfo.pSprite )
		return;

	m_spriteInfo.pSprite->SetAction( m_spriteInfo.wActionNo );
	m_spriteInfo.pSprite->SetFrame( m_spriteInfo.wFrameNo );

	float l_fMaxSize = (float)m_spriteInfo.pSprite->GetSpriteOrgYL() / m_spriteInfo.pSprite->GetSpriteYL();

	m_fSpriteUV[1][0] = l_fMaxSize * pi_fV0;
	m_fSpriteUV[1][1] = l_fMaxSize * pi_fV1;
}

// object크기의 이미지가 타일식으로 된 스프라이트일 경우 index에 의해 sprite를 설정한다.
void
CGUIObjectEx::SetSpriteTileByIndex( DWORD pi_dwIndex )
{
	if( !m_spriteInfo.pSprite )
		return;
	if( !m_ptSize.x )
		return;

	m_spriteInfo.pSprite->SetAction( m_spriteInfo.wActionNo );
	m_spriteInfo.pSprite->SetFrame( m_spriteInfo.wFrameNo );
	
	float l_fUnitSizeX =  (float)m_ptSize.x / m_spriteInfo.pSprite->GetSpriteXL();
	float l_fUnitSizeY =  (float)m_ptSize.y / m_spriteInfo.pSprite->GetSpriteYL();

	int l_nUnitNumInLine = m_spriteInfo.pSprite->GetSpriteOrgXL() / m_ptSize.x;
	if( !l_nUnitNumInLine )
		return;

	m_fSpriteUV[0][0] = l_fUnitSizeX * ( pi_dwIndex % l_nUnitNumInLine );
	m_fSpriteUV[0][1] = m_fSpriteUV[0][0] + l_fUnitSizeX;

	m_fSpriteUV[1][0] = l_fUnitSizeY * ( pi_dwIndex / l_nUnitNumInLine );
	m_fSpriteUV[1][1] = m_fSpriteUV[1][0] + l_fUnitSizeY;
}

BOOL
CGUIObjectEx::Draw( void )
{
	if( !m_bShow )
		return FALSE;

	if( NULL == m_spriteInfo.pSprite )
		return TRUE;

	m_spriteInfo.pSprite->SetAction( m_spriteInfo.wActionNo );
	m_spriteInfo.pSprite->SetFrame( m_spriteInfo.wFrameNo );

	if( m_fRotate == 0.f )	
		m_spriteInfo.pSprite->DrawSpriteUV( m_ptPos.x, 
											m_ptPos.y,
											m_fSpriteUV,
											m_ptPos.x + m_ptSize.x,
											m_ptPos.y + m_ptSize.y,
											m_dwARGB );
	else // 중심을 회전축으로 한다.
		/*
		m_spriteInfo.pSprite->DrawSpriteRot( m_ptPos.x + m_ptSize.x * 0.5, 
											 m_ptPos.y + m_ptSize.y * 0.5,
											 m_fRotate,
											 m_ptPos.x + m_ptSize.x * 1.5,
											 m_ptPos.y + m_ptSize.y * 1.5,
											 m_dwARGB );
		*/

///*
		m_spriteInfo.pSprite->DrawSpriteRotUV( m_ptPos.x + m_ptSize.x * 0.5, 
											   m_ptPos.y + m_ptSize.y * 0.5,
											   m_fRotate,
											   m_fSpriteUV,
											   m_ptPos.x + m_ptSize.x * 1.5,
											   m_ptPos.y + m_ptSize.y * 1.5,
											   m_dwARGB );
//*/
	return TRUE;
}

BOOL
CGUIObjectEx::Draw( RECT & pi_rtClipping )
{
	if( !m_bShow )
		return FALSE;

	if( NULL == m_spriteInfo.pSprite )
	{
		if( m_dwARGB != 0xffffffff )		
			Draw2DRectangle( pi_rtClipping.left,
							 pi_rtClipping.top,
							 pi_rtClipping.right,
							 pi_rtClipping.bottom,							 
							 m_dwARGB );
		return TRUE;
	}

	float l_fUV[2][2];

	m_spriteInfo.pSprite->SetAction( m_spriteInfo.wActionNo );
	m_spriteInfo.pSprite->SetFrame( m_spriteInfo.wFrameNo );
		
	l_fUV[0][0] = (float)( pi_rtClipping.left - m_ptPos.x ) / m_ptSize.x * m_fSpriteUV[0][0];				  
	l_fUV[0][1] = (float)( pi_rtClipping.right - m_ptPos.x ) / m_ptSize.x * m_fSpriteUV[0][1];			  
	l_fUV[1][0] = (float)( pi_rtClipping.top - m_ptPos.y ) / m_ptSize.y * m_fSpriteUV[1][0];				  
	l_fUV[1][1] = (float)( pi_rtClipping.bottom - m_ptPos.y ) / m_ptSize.y * m_fSpriteUV[1][1];	

	m_spriteInfo.pSprite->DrawSpriteUV( pi_rtClipping.left,
										pi_rtClipping.top,
										l_fUV,
										pi_rtClipping.right,
										pi_rtClipping.bottom,
										m_dwARGB );

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//
// CGUIObjectAni Class Implementation
//
////////////////////////////////////////////////////////////////////////////
CGUIObjectAni::CGUIObjectAni()
{
	m_fCurFrameNo		= 0.f;
	m_wTotalFrameNo		= 0;

	m_wFPS				= 10;

	m_wObjNumInLine		= 1;

	m_bStop				= FALSE;
	m_dwStopInterval	= 0;
	m_dwStopTime		= 0;

	m_fUVSize[0]		= 0.f;
	m_fUVSize[1]		= 0.f;	
}

CGUIObjectAni::~CGUIObjectAni()
{

}

void
CGUIObjectAni::SetSpriteInfo( SPRITE_INFO * pi_pSpriteInfo )
{
	CGUIObject::SetSpriteInfo( pi_pSpriteInfo );

	if( !pi_pSpriteInfo->pSprite )
		return;

	// set unit size
	pi_pSpriteInfo->pSprite->SetAction( pi_pSpriteInfo->wActionNo );
	pi_pSpriteInfo->pSprite->SetFrame( pi_pSpriteInfo->wFrameNo );

	m_fUVSize[0] = (float)m_ptSize.x / pi_pSpriteInfo->pSprite->GetSpriteXL();
	m_fUVSize[1] = (float)m_ptSize.y / pi_pSpriteInfo->pSprite->GetSpriteYL();
}

void
CGUIObjectAni::Update( void )
{
	if( !IsShow() )
		return;

	if( m_bStop )
	{
		if( timeGetTime() - m_dwStopTime > m_dwStopInterval )
			m_bStop = FALSE;
	}
	else
	{
		m_fCurFrameNo += R3GetLoopTime() * m_wFPS;

		if( m_fCurFrameNo >= m_wTotalFrameNo )
		{
			m_bStop = TRUE;
			m_dwStopTime = timeGetTime();
			m_fCurFrameNo = 0;
		}
		else
		{
			m_fSpriteUV[0][0] = (int)m_fCurFrameNo % m_wObjNumInLine * m_fUVSize[0];
			m_fSpriteUV[0][1] = m_fSpriteUV[0][0] + m_fUVSize[0];
			m_fSpriteUV[1][0] = (int)m_fCurFrameNo / m_wObjNumInLine * m_fUVSize[1];
			m_fSpriteUV[1][1] = m_fSpriteUV[1][0] + m_fUVSize[1];
		}
	}
}

////////////////////////////////////////////////////////////////////////////
//
// CGUITileObject Class Implementation
//
////////////////////////////////////////////////////////////////////////////

CGUITileObject::CGUITileObject()
{
	m_wTileNum[0] = 0;
	m_wTileNum[1] = 0;

	m_wGap[0] = 0;
	m_wGap[1] = 0;

	m_ptTileSize.x = 0;
	m_ptTileSize.y = 0;
}

CGUITileObject::~CGUITileObject()
{

}

void
CGUITileObject::SetSpriteInfo( SPRITE_INFO * pi_pSpriteInfo )
{
	CGUIObject::SetSpriteInfo( pi_pSpriteInfo );

	if( !pi_pSpriteInfo->pSprite )
		return;
	
	ResetTileNum();
}

void
CGUITileObject::SetGap( int pi_nWGap, int pi_nHGap )
{
	m_wGap[0] = pi_nWGap;
	m_wGap[1] = pi_nHGap;

	ResetTileNum();	
}

void
CGUITileObject::SetTileSize( LONG pi_nWidth, LONG pi_nHeight )
{
	m_ptTileSize.x = pi_nWidth;
	m_ptTileSize.y = pi_nHeight;

	ResetTileNum();
}

void
CGUITileObject::SetSize( POINT & pi_ptSize )
{
	CGUIObject::SetSize( pi_ptSize );

	ResetTileNum();
}

void
CGUITileObject::SetSize( LONG pi_nWidth, LONG pi_nHeight )
{
	CGUIObject::SetSize( pi_nWidth, pi_nHeight );

	ResetTileNum();
}

void
CGUITileObject::ResetTileNum()
{
	if( m_ptTileSize.x == 0 && 
		m_ptTileSize.y == 0 )
	{
		if( !GetSpriteInfo()->pSprite )
			return;

		m_spriteInfo.pSprite->SetAction( m_spriteInfo.wActionNo );
		m_spriteInfo.pSprite->SetFrame( m_spriteInfo.wFrameNo );

		// set tile number
		m_ptTileSize.x = m_spriteInfo.pSprite->GetSpriteOrgXL();
		m_ptTileSize.y = m_spriteInfo.pSprite->GetSpriteOrgYL();
	}

	m_wTileNum[0] = (float)( m_ptSize.x - m_wGap[0] ) / ( m_ptTileSize.x + m_wGap[0] );
	m_wTileNum[1] = (float)( m_ptSize.y - m_wGap[1] ) / ( m_ptTileSize.y + m_wGap[1] );
}

BOOL
CGUITileObject::Draw( void )
{
	if( !IsShow() )
		return FALSE;

	if( !m_spriteInfo.pSprite )
		return TRUE;

	m_spriteInfo.pSprite->SetAction( m_spriteInfo.wActionNo );
	m_spriteInfo.pSprite->SetFrame( m_spriteInfo.wFrameNo );

	int x, y;

	for( int i=0; i<m_wTileNum[0]; ++i )
	{
		for( int j=0; j<m_wTileNum[1]; ++j )
		{
			x = m_ptPos.x + m_wGap[0] + i * ( m_ptTileSize.x + m_wGap[0] );
			y = m_ptPos.y + m_wGap[1] + j * ( m_ptTileSize.y + m_wGap[1] );

			m_spriteInfo.pSprite->DrawSprite( x, y, x + m_ptTileSize.x, y + m_ptTileSize.y, m_dwARGB );			
		}
	}

	return TRUE;
}

BOOL
CGUITileObject::Draw( RECT & pi_rtClipping )
{
	if( !IsShow() )
		return FALSE;

	if( !m_spriteInfo.pSprite )
		return TRUE;	

	m_spriteInfo.pSprite->SetAction( m_spriteInfo.wActionNo );
	m_spriteInfo.pSprite->SetFrame( m_spriteInfo.wFrameNo );

	float	uv[2][2];	
	RECT	clip;

	for( int i=0; i<m_wTileNum[0]; ++i )
	{
		for( int j=0; j<m_wTileNum[1]; ++j )
		{
			clip.left = m_ptPos.x + m_wGap[0] + i * ( m_ptTileSize.x + m_wGap[0] );
			if( clip.left > pi_rtClipping.right )
				continue;
			else if( clip.left < pi_rtClipping.left )
				clip.left = pi_rtClipping.left;

			clip.right = clip.left + m_ptTileSize.x;
			if( clip.right < pi_rtClipping.left )
				continue;
			else if( clip.right > pi_rtClipping.right )
				clip.right = pi_rtClipping.right;

			clip.top = m_ptPos.y + m_wGap[1] + j * ( m_ptTileSize.y + m_wGap[1] );
			if( clip.top > pi_rtClipping.bottom )
				continue;
			else if( clip.top < pi_rtClipping.top )
				clip.top = pi_rtClipping.top;

			clip.bottom = clip.top + m_ptTileSize.y;
			if( clip.bottom < pi_rtClipping.top )
				continue;
			else if( clip.bottom > pi_rtClipping.bottom )
				clip.bottom = pi_rtClipping.bottom;

			uv[0][0] = (float)( pi_rtClipping.left - clip.left ) / m_spriteInfo.pSprite->GetSpriteXL();
			uv[0][1] = (float)( pi_rtClipping.right - clip.left ) / m_spriteInfo.pSprite->GetSpriteXL();	

			uv[1][0] = (float)( pi_rtClipping.top - clip.top ) / m_spriteInfo.pSprite->GetSpriteYL();
			uv[1][1] = (float)( pi_rtClipping.bottom - clip.top ) / m_spriteInfo.pSprite->GetSpriteYL();

			m_spriteInfo.pSprite->DrawSpriteUV( clip.left, 
												clip.top,
												uv,
												clip.right, 
												clip.bottom,
												m_dwARGB );
		}
	}

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//
// CGUITimer Class Implementation
//
////////////////////////////////////////////////////////////////////////////
CGUITimer::CGUITimer()
{
	m_dwDelay		= 0;
	m_dwStartTime	= 0;
	
	m_nDelayAngle	= 0;

	m_bIsStarted	= FALSE;

	for( int i = 0; i < 3; ++i )
	{
		m_vVertex[i].color = 0xa0040204;
		m_vVertex[i].z = 1.0f;
		m_vVertex[i].w = 1.0f;
		m_vVertex[i].u = 0.f;
		m_vVertex[i].v = 0.f;
	}
}

CGUITimer::~CGUITimer()
{

}

void	
CGUITimer::SetColor( DWORD pi_dwColor )
{
	for( int i = 0; i < 3; ++i )
		m_vVertex[i].color = pi_dwColor;
}

void
CGUITimer::Start( void )
{
	m_dwStartTime = timeGetTime();

	m_bIsStarted = TRUE;
}

BOOL
CGUITimer::Draw( void )
{
	if( !IsShow() )
		return FALSE;

	if( !m_bIsStarted )
		return FALSE;

	DWORD l_dwCurTime;
	l_dwCurTime = timeGetTime() - m_dwStartTime;

	if( l_dwCurTime >= m_dwDelay )
	{
		m_bIsStarted = FALSE;
		return TRUE;
	}	
	else
	{
		m_nDelayAngle = (float)l_dwCurTime / (float)m_dwDelay * 360;		
	}
	
	m_vVertex[2].x = m_ptPos.x + m_ptSize.x * 0.5;
	m_vVertex[2].y = m_ptPos.y + m_ptSize.y * 0.5;

	if( m_nDelayAngle < 45 )
	{
		m_vVertex[0].x = m_vVertex[2].x + m_ptSize.y * 0.5 * tan( m_nDelayAngle * D3DX_PI / 180 );
		m_vVertex[0].y = m_ptPos.y;
		
		m_vVertex[1].x = m_ptPos.x + m_ptSize.x;
		m_vVertex[1].y = m_ptPos.y;
		
		Draw2DSprite( m_vVertex, 1, NULL );
	}	

	if( m_nDelayAngle < 135 )
	{
		m_vVertex[0].x = m_ptPos.x + m_ptSize.x;

		if( m_nDelayAngle < 45 )		
			m_vVertex[0].y = m_ptPos.y;
		
		else if( m_nDelayAngle < 90 )
			m_vVertex[0].y = m_ptPos.y + m_ptSize.x * 0.5 * ( 1.f - tan( (90 - m_nDelayAngle) * D3DX_PI / 180 ) );
				
		else		
			m_vVertex[0].y = m_vVertex[2].y + m_ptSize.x * 0.5 * tan( (m_nDelayAngle-90) * D3DX_PI / 180 );
		
		
		m_vVertex[1].x = m_ptPos.x + m_ptSize.x;
		m_vVertex[1].y = m_ptPos.y + m_ptSize.y;		
		
		Draw2DSprite( m_vVertex, 1, NULL );
	}
	
	if( m_nDelayAngle < 225 )
	{
		if( m_nDelayAngle < 135 )
			m_vVertex[0].x = m_ptPos.x + m_ptSize.x;

		else if( m_nDelayAngle < 180 )
			m_vVertex[0].x = m_vVertex[2].x + m_ptSize.y * 0.5 * tan( (180 - m_nDelayAngle) * D3DX_PI / 180 );

		else
			m_vVertex[0].x = m_ptPos.x + m_ptSize.y * 0.5 * ( 1.f - tan( (m_nDelayAngle-180) * D3DX_PI / 180 ) );

		m_vVertex[0].y = m_ptPos.y + m_ptSize.y;
		
		m_vVertex[1].x = m_ptPos.x;
		m_vVertex[1].y = m_ptPos.y + m_ptSize.y;
		
		Draw2DSprite( m_vVertex, 1, NULL );
	}
	
	if( m_nDelayAngle < 315 )
	{
		m_vVertex[0].x = m_ptPos.x;		

		if( m_nDelayAngle < 225 )
			m_vVertex[0].y = m_ptPos.y + m_ptSize.y;			

		else if( m_nDelayAngle < 270 )
			m_vVertex[0].y = m_vVertex[2].y + m_ptSize.x * 0.5 * tan( (270 - m_nDelayAngle) * D3DX_PI / 180 );

		else
			m_vVertex[0].y = m_ptPos.y + m_ptSize.x * 0.5 * ( 1.f - tan( (m_nDelayAngle-270) * D3DX_PI / 180 ) );
			
		
		m_vVertex[1].x = m_ptPos.x;
		m_vVertex[1].y = m_ptPos.y;
		
		Draw2DSprite( m_vVertex, 1, NULL );
	}

	if( m_nDelayAngle < 360 )
	{
		if( m_nDelayAngle < 315 )
			m_vVertex[0].x = m_ptPos.x;

		else
			m_vVertex[0].x = m_ptPos.x + m_ptSize.y * 0.5 * ( 1.f - tan( (360 - m_nDelayAngle) * D3DX_PI / 180 ) );

		m_vVertex[0].y = m_ptPos.y;
		
		m_vVertex[1].x = m_vVertex[2].x;
		m_vVertex[1].y = m_ptPos.y;
		
		Draw2DSprite( m_vVertex, 1, NULL );
	}

	return TRUE;
}

BOOL
CGUITimer::Draw( RECT & pi_rtClipping )
{
	if( m_ptPos.x < pi_rtClipping.left ||
		m_ptPos.x + m_ptSize.x > pi_rtClipping.right ||
		m_ptPos.y < pi_rtClipping.top ||
		m_ptPos.y + m_ptSize.y > pi_rtClipping.bottom )
		return FALSE;

	return CGUITimer::Draw();
}