// GUIScreen.cpp: implementation of the CGUIScreen class.
//
//////////////////////////////////////////////////////////////////////

#ifdef _DEVELOPER_
// by J.S.H 임시 테스트 코드
#include "../../GameObject/Manager/CCharacterMgr.h"
#include "../../Item/CItemMgr.h"
// by J.S.H 임시 테스트 코드
#endif

#include "GUIScreen.h"
#include "GUIDialogBox.h"
#include "../SpriteMgr.h"

#include "Core.h"

#define	TOOLTIP_MARGIN		10	// 아래 위 여백
#define	TOOLTIP_V_GAP		10	// 글자사이의 세로 간격
#define	TOOLTIP_BORDER		1	// 테두리 굵기
#define	TOOLTIP_OUTLINE_GAP	1	// 테두리와 경계선사이 간격

#define	TOOLTIP_BASE				D3DCOLOR_RGBA(14,22,34,220)
#define	TOOLTIP_OUTLINE				D3DCOLOR_RGBA(122,117,173,220)
#define	TOOLTIP_STR_COLOR			D3DCOLOR_XRGB(209,209,209)

extern	int	g_nLockDlgBoxCount;

///////////////////////////////////////////////////////////////////////
//
void	EP_RebirthAvatar( void );

//void	EP_Accept_RequestForItemTrade( void );

void	EP_Disconnect_Account( BOOL pi_bIsDisconnect );
void	EP_DisconnectedFromServer( void );

///////////////////////////////////////////////////////////////////////
//

DWORD	g_dwGUIInputID = NONE_INPUT_ID;

CGUIScreen::CGUIScreen()
{
	m_hWnd = NULL;
	
	m_byTooltipType = TOOLTIP_TYPE_DEFAULT;
	m_pTooltipString[0][0]	= '\0';

	m_cTooltipItem.Show( TRUE );
	m_cTooltipItem.DrawItemNum( FALSE );	

	m_cTooltipItemBase.SetSize( 68, 68 );
	m_cUpgradeSocket.SetSize( 15, 24 );	
	m_cTooltipItemBase.Show( TRUE );
	m_cUpgradeSocket.Show( TRUE );

	for( int i=0; i<UPGRADE_SOCKET_NUM; ++i )
	{
		m_cUpgradeItem[i].SetSize( 28, 28 );
		m_cUpgradeItem[i].SetItemID( IID_COM, IID_ETC, IID_RESOURCE, 0 );
		m_cUpgradeItem[i].Show( TRUE );
	}
	
	m_byTotalUpgradeSocketNum = 0;

	m_dwTooltipDelayTime = 300;

	// event handler
	AddMouseEventHandler( this );
	AddMouseMotionEventHandler( this );	

	Show( TRUE );

	SetName( "Screen" );	

	m_bCreated	= FALSE;
}

CGUIScreen::~CGUIScreen()
{
	RemoveAll();

	// screen 객체가 없어질때만 critical error msg창을 없애준다.
	CGUIDialogBox * l_pDlgBox;
	while( !m_listDialogBox.empty() )
	{
		// error msg창 밖에 안남았다.
		l_pDlgBox = (*m_listDialogBox.begin());
		l_pDlgBox->RemoveOwnerWindow();		

		// destroy이면 메모리를 해제시킨다.
		if( l_pDlgBox->IsDestroy() )
			delete l_pDlgBox;
	}	
}

void
CGUIScreen::Init()
{
	// 해상도 setting
	_R3ENGINE_STATE * pState = GetR3State();
	SetSize( pState->mScreenXsize, pState->mScreenYsize );		

	// remove all window
	RemoveAll();

	RemoveObject();
	RemoveTooltip();

	g_nLockDlgBoxCount = 0;

	// create event queue
	//_CreateEventQueue( 10 );
	CGUIObject::CreateEventQueue( 10 );

	SetSprite();
	
	// set system mouse
	if( pState->mbFullScreen )
	{
		if( pState->mMouseAccelation )
			m_cMouse.EnableSystemMouse( TRUE );
		else
			m_cMouse.EnableSystemMouse( FALSE );
	}
	else
	{
		m_cMouse.EnableSystemMouse( FALSE );
	}

	m_bCreated = TRUE;
}

void
CGUIScreen::RestoreSprite( void )
{
	// 해상도 setting
	_R3ENGINE_STATE * pState = GetR3State();
	SetSize( pState->mScreenXsize, pState->mScreenYsize );

	CGUIWindow::RestoreSprite();

	CGUIWindow * l_pWinObj = NULL;
	for( CGUIObjectList::iterator it = m_listObject.begin(); it != m_listObject.end(); ++it )
	{
		l_pWinObj = (CGUIWindow *)(*it);

		l_pWinObj->RestoreSprite();
	}
}

void
CGUIScreen::SetSprite( void )
{
	if( !m_bCreated )
	{
		// mouse setting
		m_cMouse.Init();

		// tooltip
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );
		l_spriteInfo.wActionNo = 2;
		l_spriteInfo.wFrameNo = 0;
		m_cTooltipItemBase.SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 1;
		m_cUpgradeSocket.SetSpriteInfo( &l_spriteInfo );		
	}
	else
	{
		// mouse
		m_cMouse.SetSprite();
		
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );

		// tooltip
		m_cTooltipItemBase.GetSpriteInfo()->pSprite = l_pSprite;
		m_cUpgradeSocket.GetSpriteInfo()->pSprite = l_pSprite;

		l_pSprite = gSpriteMgr.GetSprite( SP_ID_ITEM );
		
		m_cTooltipItem.GetSpriteInfo()->pSprite = l_pSprite;

		for( int i=0; i<UPGRADE_SOCKET_NUM; ++i )
			m_cUpgradeItem[i].GetSpriteInfo()->pSprite = l_pSprite;

		// catched item
		if( m_cCatchedItem.GetItemID() != NONE_ITEM_ID )
			m_cCatchedItem.GetSpriteInfo()->pSprite = gSpriteMgr.GetSprite( SP_ID_ITEM );
	}
}

void
CGUIScreen::Show( BOOL pi_bShow )
{
	// 포함된 window에는 show()함수를 적용하지 않는다
	CGUIObject::Show( pi_bShow );
	
	if( IsShow() )
	{
//		if( ExistObject() )
//		{
//			m_cCatchedItem.Show( TRUE );
//			m_cMouse.Show( FALSE );
//		}
//		else
//		{
//			m_cMouse.Show( TRUE );
//		}
	}
	else
	{	
//		m_cMouse.Show( FALSE );

//		if( ExistObject() )
//		{
//			m_cCatchedItem.Show( FALSE );
//			m_cMouse.Show( TRUE );
//		}		

		RemoveTooltip();

		// init mouse event buffer	
		CGUIObject::ClearMouseEventBuffer();
	}
}

void
CGUIScreen::SetMouseState( CGUIMouse::CursorState pi_eState )
{
	m_cMouse.SetCursorState( pi_eState );
}

void
CGUIScreen::Add( CGUIWindow * pi_pWindow )
{
	CGUIContainer::Add( pi_pWindow );		
	// parent window setting
	pi_pWindow->SetParent( this );
}

void
CGUIScreen::Remove( CGUIWindow * pi_pWindow )
{
	CGUIContainer::Remove( pi_pWindow );	
	pi_pWindow->SetParent( NULL );
}

void
CGUIScreen::RemoveAll( void )
{
	CGUIContainer::RemoveAll();
	
	CGUIDialogBox * l_pDlgBox;
	int l_nCrtErrNum = 0;
	while( !m_listDialogBox.empty() &&
			m_listDialogBox.size() != l_nCrtErrNum )
	{			
		CGUIDialogBoxList::iterator it;
		int l_nCnt=0;
		for( it=m_listDialogBox.begin(); it != m_listDialogBox.end(); ++it )
		{
			if( l_nCnt == l_nCrtErrNum )
			{
				l_pDlgBox = (*it);
				break;
			}

			if( (*it)->GetUserDefinedID() == MSGBOX_ID_CRITICAL_ERROR )
				++l_nCnt;			
		}
		if( it == m_listDialogBox.end() )
			break;

		// critical error msg창은 객체가 소멸될 때 없애기 위해 여기선 skip한다.
		if( l_pDlgBox->GetUserDefinedID() == MSGBOX_ID_CRITICAL_ERROR )
		{
			++l_nCrtErrNum;
			continue;
		}

		l_pDlgBox->RemoveOwnerWindow();		

		// destroy이면 메모리를 해제시킨다.
		if( l_pDlgBox->IsDestroy() )
			delete l_pDlgBox;		
	}

/*
	CGUIDialogBoxList::iterator i;
	for( i=m_listDialogBox.begin(); i != m_listDialogBox.end(); ++i )
	{	
		l_pDlgBox = (*i);
		(*i)->SetParent( NULL );
		(*i)->Close();

		// destroy이면 메모리를 해제시킨다.
		if( (*i)->IsDestroy() )
			delete *i;
	}

	// remove dialogbox
	m_listDialogBox.clear();
*/
}


BOOL
CGUIScreen::InputProc( void )
{
	if( !IsShow() )
	{
		SendMouseMotionEvent( MOUSE_MOVE );				
		CGUIObject::EventProcess();
	
		return TRUE;
	}

	// init mouse event buffer	
	CGUIObject::ClearMouseEventBuffer();	

	// backup previous guiinputid			
	DWORD l_dwPrevGUIInputID = g_dwGUIInputID;
	// init inputid
	g_dwGUIInputID = NONE_INPUT_ID;

	BOOL	l_bLockChildWin = FALSE;

	if( !m_listDialogBox.empty() )
	{		
		// 맨 마지막에 등록된 dialogbox만 input을 처리해준다.
		CGUIDialogBoxList::reverse_iterator rit;
		CGUIDialogBox *pBox;
		for( rit=m_listDialogBox.rbegin(); rit != m_listDialogBox.rend(); ++rit )
		{
			pBox = (*rit);//MAYUN	

			if( pBox->IsModal() || pBox->IsLockOtherWin() )
			{
				pBox->InputProc();

				l_bLockChildWin = TRUE;

				break;
			}
			else
			{
				if( pBox->InputProc() )
					break;
			}		
		}

		// set GUI Input ID
		if( rit != m_listDialogBox.rend() )	
		{
			//g_dwGUIInputID = pBox->GetID();
			g_dwGUIInputID = DEFAULT_ID_DIALOG_BOX;
		}
	}	

	// window에 lock을 건 상태이면 lock을 건 dialogbox만 입력을 받는다
	if( g_nLockDlgBoxCount > 0 &&
		g_dwGUIInputID == NONE_INPUT_ID )		
	{	
		CGUIWindow * l_pWinObj = NULL;
		CGUIObjectList::reverse_iterator rit;
		for( rit = m_listObject.rbegin(); rit != m_listObject.rend(); ++rit )	
		{
			l_pWinObj = (CGUIWindow *)(*rit);

			if( l_pWinObj->ExistDialogBox() )
			{
				l_pWinObj->InputProc();
				break;
			}
		}

		// set GUI Input ID
		g_dwGUIInputID = DEFAULT_ID_DIALOG_BOX;
	}
	else if( g_dwGUIInputID == NONE_INPUT_ID )
	{
		//---------------------------------------------------
		// window list Input process
		
		CGUIWindow * l_pActiveWin = NULL;

		CGUIWindow * l_pWinObj = NULL;
		CGUIObjectList::reverse_iterator i;		

		for( i = m_listObject.rbegin(); i != m_listObject.rend(); ++i )
		{
			l_pWinObj = (CGUIWindow *)(*i);			

			// window inputprocess
			if( l_pWinObj->InputProc() )
			{
				// event가 일어난 object가 포함된
				// window의 id를 guiInputID로 등록한다.
				if( g_dwGUIInputID != NONE_INPUT_ID )
					continue;
				
				if( gMouseDev.IsStroked( L_BUTTON ) &&
					l_pWinObj->IsCrossedWindow( gMouseDev.GetX(), gMouseDev.GetY() ) )
				{
					// new pressed object
					g_dwGUIInputID = l_pWinObj->GetID();

					l_pActiveWin = l_pWinObj;								
				}			
				else if( !CGUIObject::GetPressedObject() &&
						 CGUIObject::GetMouseDetectedObject() )
				{				
					// hovered object
					g_dwGUIInputID = l_pWinObj->GetID();				
				}
				
				break;
			}
		}
		
		// active Window
		if( l_pActiveWin )
		{
			ActiveWindow( l_pActiveWin );

			if( l_pActiveWin->IsActiveWindow() )
			{
				// set focus
				CGUIObject * l_pFocusedObj = CGUIObject::GetPressedObject();
				if( l_pFocusedObj )		
					l_pFocusedObj->SetFocus();
			}
		}
		
		// previous pressed object
		if( CGUIObject::GetPressedObject() &&
			g_dwGUIInputID == NONE_INPUT_ID )
			g_dwGUIInputID = l_dwPrevGUIInputID;		
	}

	// screen은 focusedobject 체크 안한다.
	CGUIObject::InputProc();

	// CGUIScreen은 맨 아래에 있는 layer이지만 
	// mouse cursor나 object의 경우 최상위에 있어야 하기 때문에
	// layer에 상관없이 mouse motion event를 보낸다.	
	SendMouseMotionEvent( MOUSE_MOVE );		
	
	// event process
	CGUIObject::EventProcess();


	// gui object를 가지고 있으면 screen ID로 세팅한다.
	if( ExistObject() )		
	{
		g_dwGUIInputID = m_dwID;
	}	

	return TRUE;
}

LRESULT
CGUIScreen::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !IsShow() )
		return 1; 

	if( !m_listDialogBox.empty() )
	{
		// 활성화된 dialogbox에만 msg를 보낸다.
		CGUIDialogBoxList::reverse_iterator i;
		i = m_listDialogBox.rbegin();
		(*i)->MsgProc( hWnd, uMsg, wParam, lParam );
	}
	else 
	{
		CGUIWindow	* pWindow = NULL;		
		for( CGUIObjectList::iterator i = m_listObject.begin(); i != m_listObject.end(); ++i )
		{				
			pWindow = ( CGUIWindow * )(*i);
			// 활성화된 window에만 msg를 보낸다.
			if( pWindow->MsgProc( hWnd, uMsg, wParam, lParam ) )
				break;
		}
	}

	// event process
	CGUIObject::EventProcess();

	return 1;
}

void
CGUIScreen::ActiveWindow( CGUIWindow * pi_pWindow )
{
	if( !m_listDialogBox.empty() )
	{
		CGUIDialogBoxList::reverse_iterator it;
		it = m_listDialogBox.rbegin();
		if( pi_pWindow == this )		
			(*it)->Active();

		CGUIWindow * l_pWin;
		CGUIObjectList::iterator wit;
		for( wit = m_listObject.begin(); wit != m_listObject.end(); ++wit )
		{
			l_pWin = (CGUIWindow *)(*wit);
			if( l_pWin->IsActive() )
				l_pWin->InActive();
		}

		return;
	}	

	if( find( m_listObject.begin(), m_listObject.end(), pi_pWindow ) == m_listObject.end() )
	{
		// pi_pWindow가 없으면 맨 위에 있는 window를 active한다.
		if( m_listObject.empty() )
			return;

		CGUIWindow	* l_pWin;
		CGUIObjectList::reverse_iterator it;
		BOOL	l_bActiveWin = FALSE;
			
		for( it = m_listObject.rbegin(); it != m_listObject.rend(); ++it )
		{		
			l_pWin = (CGUIWindow *)(*it);		

			if( l_pWin->IsAlwaysActive() &&
				l_pWin->IsShow() )
			{
				l_pWin->Active();
				l_bActiveWin = TRUE;
				break;
			}
		}
		
		if( !l_bActiveWin )
		{
			it = m_listObject.rbegin();
			l_pWin = (CGUIWindow *)(*it);
			l_pWin->Active();
		}

		return;
	}
	

	// 정렬하지 않고 제거했다가 다시 삽입한다.	
	// 먼저 pi_pWindow를 삭제한다.
	m_listObject.remove( pi_pWindow );	

	CGUIWindow * l_pOtherWin;
	CGUIObjectList::iterator it;

	BOOL l_bExistActiveWin = FALSE;

	// 나머지 window는 inactive시킨다.		
	for( it = m_listObject.begin(); it != m_listObject.end(); ++it )
	{
		l_pOtherWin = (CGUIWindow *)(*it);	

		if( !l_bExistActiveWin &&
			l_pOtherWin->IsAlwaysActive() &&
			l_pOtherWin->IsShow() )
		{	
			l_bExistActiveWin = TRUE;

			// 다른 window가 always active일 때 
			// 자신도 always active이면 다른 window를 inactive한다.
			if( pi_pWindow->IsShow() &&
				pi_pWindow->IsAlwaysActive() )				
			{				
				l_pOtherWin->InActive();				
			}
		}
		else
		{
			l_pOtherWin->InActive();
		}
	}

	// insert pi_pWindow
	if( pi_pWindow->IsAlwaysTop() )
	{
		m_listObject.push_back( pi_pWindow );
	}
	else
	{
		// always top window 아래에 추가시킨다.
		for( it = m_listObject.begin(); it != m_listObject.end(); ++it )
		{
			l_pOtherWin = (CGUIWindow *)(*it);
			if( l_pOtherWin->IsAlwaysTop() )
			{
				m_listObject.insert( it, pi_pWindow );					
				break;
			}
		}
		if( it == m_listObject.end() )
			m_listObject.push_back( pi_pWindow );
	}	

	
	// always active window가 없으면 active window
	if( pi_pWindow->IsShow() &&
		( pi_pWindow->IsAlwaysActive() || !l_bExistActiveWin ) )		
	{
		pi_pWindow->Active();				
	}	
	
	// ----------------------------------------------------------------------
	// docking customer이면 host를 customer 바로 밑 layer로 옮긴다.
	if( pi_pWindow->GetDockingHost() )		
	{
		if( pi_pWindow->GetDockingBorder() == NONE_BORDER )
			return;

		it = find( m_listObject.begin(), m_listObject.end(), pi_pWindow );

		m_listObject.remove( pi_pWindow->GetDockingHost() );
		m_listObject.insert( it, pi_pWindow->GetDockingHost() );
	}
	// docking host이면 customer를 host 바로 밑 layer로 옮긴다.
	else
	{
		CGUIWindowList *	l_pDockingList;
		l_pDockingList = pi_pWindow->GetDockingList();

		if( l_pDockingList->empty() )
			return;

		it = find( m_listObject.begin(), m_listObject.end(), pi_pWindow );

		CGUIWindowList::iterator dit;
		for( dit = l_pDockingList->begin(); dit != l_pDockingList->end(); ++dit )
		{	
			if( !(*dit)->IsShow() )
				continue;

			if( (*dit)->GetDockingBorder() == NONE_BORDER )
				continue;

			// pi_pWindow 바로 아래에 추가
			m_listObject.remove( *dit );
			m_listObject.insert( it, *dit );
		}
	}

}

void
CGUIScreen::InActiveWindow( CGUIWindow * pi_pWindow )
{	
	if( !pi_pWindow->IsActive() )
		return;

	if( !m_listDialogBox.empty() )
	{
		CGUIDialogBoxList::reverse_iterator it;
		it = m_listDialogBox.rbegin();
		if( pi_pWindow == this )		
			(*it)->InActive();

		return;
	}	

	// inactive window
	pi_pWindow->InActive();

	if( m_listObject.empty() )
		return;
	
	CGUIWindow * l_pOtherWin;
	CGUIObjectList::iterator it;
	CGUIObjectList::iterator l_itActive = m_listObject.end();

	// 상위에서부터 always active window를 찾아서 active시킨다.	
	for( it = m_listObject.begin(); it != m_listObject.end(); ++it )
	{		
		l_pOtherWin = (CGUIWindow *)(*it);		

		if( l_pOtherWin->IsAlwaysActive() &&
			l_pOtherWin->IsShow() )
		{
			l_itActive = it;
		}
	}

	if( l_itActive != m_listObject.end() )
	{
		l_pOtherWin = (CGUIWindow *)(*l_itActive);
		l_pOtherWin->Active();

		// active window 아래에 추가시킨다.
		m_listObject.remove( pi_pWindow );
		m_listObject.insert( l_itActive, pi_pWindow );
	}
}

void
CGUIScreen::MouseEvent( _mouse_event & event )
{
	if( m_listDialogBox.empty() &&		
		!g_nLockDlgBoxCount &&
		ExistObject() )
	{
		if( event.id == MOUSE_STROKED )
		{
			g_dwGUIInputID = m_dwID;

			// 아이템을 바닥에 떨어뜨렸다.			
			PostUIMessage( WM_BOARD_DROP_OBJECT, 0 );
		}
	}
	
}

void
CGUIScreen::MouseMotionEvent( _mouse_motion_event & event )
{
	if( ExistObject() )
	{
		if( event.id == MOUSE_MOVE )
		{
			// item potision set
			m_cCatchedItem.SetPos( event.xPos - (float)m_cCatchedItem.m_ptSize.x / 2,
								   event.yPos - (float)m_cCatchedItem.m_ptSize.y / 2 );
		}		
	} 		

	// mouse cursor position set
	m_cMouse.SetPos( event.xPos, event.yPos );

	// tooltip repos
	if( m_byTooltipStringNum || m_byTooltipColorStrNum )
	{
		// screen을 벗어나지 않게 tooltip 위치를 잡아준다.

		// board X pos setting
		if( m_ptPos.x + m_ptSize.x - m_cMouse.m_ptPos.x - m_cMouse.m_ptSize.x < m_ptTooltipBoardSize.x )
		{
			m_ptTooltipBoardPos.x = m_cMouse.m_ptPos.x - m_ptTooltipBoardSize.x;
		}
		else
		{
			m_ptTooltipBoardPos.x = m_cMouse.m_ptPos.x + m_cMouse.m_ptSize.x;
		}

		// board Y pos setting
		if( m_ptPos.y + m_ptSize.y - m_cMouse.m_ptPos.y < m_ptTooltipBoardSize.y )
		{
			m_ptTooltipBoardPos.y = m_ptSize.y - m_ptTooltipBoardSize.y;
		}
		else
		{
			m_ptTooltipBoardPos.y = m_cMouse.m_ptPos.y;
		}

		// tooltip item
		if( m_byTooltipType == TOOLTIP_TYPE_ITEM )
		{			
			m_cTooltipItemBase.SetPos( m_ptTooltipBoardPos.x + m_ptTooltipBoardSize.x -
											m_cTooltipItemBase.m_ptSize.x - TOOLTIP_MARGIN ,
										m_ptTooltipBoardPos.y + 28 );
			
			m_cTooltipItem.SetPos( m_cTooltipItemBase.m_ptPos.x + 2, m_cTooltipItemBase.m_ptPos.y + 2 );
		}
	}
}

void
CGUIScreen::AddObject( CGUIItem * pi_pItem )
{
	m_cCatchedItem.SetData( pi_pItem );
	m_cCatchedItem.DrawItemNum( TRUE );

	m_cCatchedItem.Show( TRUE );

	m_cCatchedItem.SetPos( m_cMouse.m_ptPos.x - (float)m_cCatchedItem.m_ptSize.x / 2,
						   m_cMouse.m_ptPos.y - (float)m_cCatchedItem.m_ptSize.y / 2 );
	
	// 아이템을 잡고 있는 동안은 마우스 포인터를 안보이게 한다.
	m_cMouse.Show( FALSE );

	RemoveTooltip();
	
	// object picking event를 모든 window에 알린다.
	CGUIObjectList::iterator it;
	for( it = m_listObject.begin(); it != m_listObject.end(); ++it )
	{
		((CGUIWindow *)(*it))->PickingObjectEvent( TRUE );
	}
}

void
CGUIScreen::ResizeObject( int pi_nWidth, int pi_nHeight )
{
	m_cCatchedItem.SetSize( pi_nWidth, pi_nHeight );

	m_cCatchedItem.SetPos( m_cMouse.m_ptPos.x - (float)m_cCatchedItem.m_ptSize.x / 2,
						   m_cMouse.m_ptPos.y - (float)m_cCatchedItem.m_ptSize.y / 2 );
}

void	
CGUIScreen::RemoveObject( void )
{
	m_cCatchedItem.SetItemID( NONE_ITEM_ID );	

	m_cCatchedItem.Show( FALSE );

	m_cMouse.Show( TRUE ); 

	// object unpicking event를 모든 window에 알린다.
	CGUIObjectList::iterator it;
	for( it = m_listObject.begin(); it != m_listObject.end(); ++it )
	{
		((CGUIWindow *)(*it))->PickingObjectEvent( FALSE );
	}
}

void
CGUIScreen::DeleteObject( void )
{
	RemoveObject();
}

BOOL
CGUIScreen::Draw( void )
{
	if( !IsShow() )
	{
		m_cMouse.Draw();
		
		if( ExistObject() )
			m_cCatchedItem.Draw();

		return FALSE;
	}

	CGUIWindow::Draw();

	// draw object
	if( ExistObject() )
		m_cCatchedItem.Draw();

	// draw tooltip
	if( ( m_byTooltipStringNum || m_byTooltipColorStrNum ) && ( timeGetTime() - m_dwTooltipStartTime > m_dwTooltipDelayTime ) )
		DrawTooltip();

	// draw mouse cursor
	m_cMouse.Draw();

	// --------------------------------------------------------
	// guiinputid	
/*
	char l_uiStr[13][64] = {
		"Inventory",
		"char info",
		"Chat",
		"Item belt",
		"skill force belt",
		"gauge",
		"skill",
		"force",
		"shop",
		"func key",
		"input dlg",		
		"none",
		"other"
	};

	WORD l_wIndex;
	if( g_dwGUIInputID == 0x0000ffff )
	{
		l_wIndex = 10;
	}
	else if( g_dwGUIInputID == 0xffffffff )
	{
		l_wIndex = 11;
	}	
	else if( g_dwGUIInputID < 0 || g_dwGUIInputID > 10 )
	{
		l_wIndex = 12;
	}	
	else
	{
		l_wIndex = g_dwGUIInputID;
	}

	DrawR3Hangul( 100,
				  100,
				  l_uiStr[l_wIndex],
				  D3DCOLOR_XRGB( 255, 255, 255 ) );
*/

	// --------------------------------------------------------
/*
	// active window	
	int l_nCount = 0;
	CGUIWindow *l_pWin;
	CGUIObjectList::iterator it;
	for( it = m_listObject.begin(); it != m_listObject.end(); ++it )
	{		
		l_pWin = (CGUIWindow *)(*it);

		DrawR3Hangul( 100,
					  50 + l_nCount * 16,
					  l_pWin->GetName(),
					  D3DCOLOR_XRGB( 255, 255, 255 ),
					  R3_HAN_OUTLINE );

		if( l_pWin->IsActiveWindow() )
		{
			DrawR3Hangul( 100 + strlen( l_pWin->GetName() )*6 + 30,
						  50 + l_nCount * 16,
						  "  <- Active",
						  D3DCOLOR_XRGB( 255, 255, 255 ),
						  R3_HAN_OUTLINE );			
		}
		
		++l_nCount;
	}
*/	
	
	return TRUE;
}

void
CGUIScreen::ClosingDialogBox( CGUIDialogBox * pi_pDlg )
{
	if( !pi_pDlg )
		return;
/*
#ifdef _DEVELOPER_
	// by J.S.H 임시 테스트 코드
	char l_pBuf[32];
	sprintf( l_pBuf, "ClosingDialogBox => %x => %x", pi_pDlg->GetUserDefinedID(), pi_pDlg->GetResultMsg() );
	_GetNetworkMgr()->NetLogWrite( l_pBuf );
	// by J.S.H 임시 테스트 코드
#endif*/

	if( pi_pDlg->GetUserDefinedID() == MSGBOX_ID_REBIRTH )
	{
		if( pi_pDlg->GetResultMsg() == MSG_OK )
		{
/*#ifdef _DEVELOPER_
			_GetNetworkMgr()->NetLogWrite( "ClosingDialogBox => MSGBOX_ID_REBIRTH => MSG_OK " );
#endif*/
			EP_RebirthAvatar();
		}
		else if( pi_pDlg->GetResultMsg() == MSG_CANCEL )
		{
			return;
		}
	}/*
	else if( pi_pDlg->GetUserDefinedID() == MSGBOX_ID_REQUEST_TRADE )
	{
		if( pi_pDlg->GetResultMsg() == MSG_OK )
			EP_Accept_RequestForItemTrade();
	}*/
	else if( pi_pDlg->GetUserDefinedID() == MSGBOX_ID_DISCONNECT_ACCOUNT )
	{
		if( pi_pDlg->GetResultMsg() == MSG_OK )
			EP_Disconnect_Account( TRUE );
		else
			EP_Disconnect_Account( FALSE );
	}
	else if( pi_pDlg->GetUserDefinedID() == MSGBOX_ID_DISCONNECT_SERVER )
	{
		if( pi_pDlg->GetResultMsg() == MSG_OK )
			EP_DisconnectedFromServer();
	}
	else if( pi_pDlg->GetUserDefinedID() == MSGBOX_ID_CRITICAL_ERROR )
	{
		// ------------------------------------------------------------
		// 로그파일 보내기
		STARTUPINFO				l_sStartupInfo;
		PROCESS_INFORMATION		l_sProcessInfo;

		memset( &l_sStartupInfo, 0, sizeof( l_sStartupInfo ) );
		memset( &l_sProcessInfo, 0, sizeof( l_sProcessInfo ) ); 

		l_sStartupInfo.cb = sizeof( l_sStartupInfo ); 

		CreateProcess( ".\\SendLog.exe",
					   " -a",
					   0,
					   0,
					   FALSE,
					   CREATE_DEFAULT_ERROR_MODE,
					   0,
					   ".\\",
					   &l_sStartupInfo,
					   &l_sProcessInfo );
		// ------------------------------------------------------------

		PostMessage( gScreen.GetHWND(), WM_CLOSE, 0, 0 );
	}
	// 기갑 장비 출고
	else if( pi_pDlg->GetUserDefinedID() == MSGBOX_ID_UNIT_DELIVERY )
	{
		if( pi_pDlg->GetResultMsg() == MSG_OK )
			PostMessage( gScreen.GetHWND(), WM_AVATAR_UNIT_DELIVERY, 0, 0 );
	}

	// --------------------------------------------------------------------------

	CGUIWindow::ClosingDialogBox( pi_pDlg );

	if( !m_listDialogBox.empty() )
		return;


	// dialogbox가 없을 때 
	// 상위에서부터 always active window를 찾아서 active시킨다.		
	if( m_listObject.empty() )
		return;
	
	CGUIWindow	* l_pWin;
	CGUIObjectList::reverse_iterator it;
	BOOL	l_bActiveWin = FALSE;
		
	for( it = m_listObject.rbegin(); it != m_listObject.rend(); ++it )
	{		
		l_pWin = (CGUIWindow *)(*it);		

		if( l_pWin->IsAlwaysActive() &&
			l_pWin->IsShow() )
		{
			l_pWin->Active();
			l_bActiveWin = TRUE;
			break;
		}
	}
	
	if( !l_bActiveWin )
	{
		it = m_listObject.rbegin();
		l_pWin = (CGUIWindow *)(*it);
		l_pWin->Active();
	}			
}

void
CGUIScreen::Update( void )
{
	m_cMouse.Update();		
	
	CGUIWindow::Update();	

	g_cMoveObjectMgr.FrameMove();
}

// ===============================================================================================

void
CGUIScreen::SetTooltip( BYTE pi_byLineNum, char pi_pStrLine[][TOOLTIP_MAX_STR], BYTE pi_byColorStrNum, TOOLTIP_COLOR_STR_INFO * pi_pColorStr )
{
	// set tooltip type
	m_byTooltipType = TOOLTIP_TYPE_DEFAULT;

	// tooltip setting time
	m_dwTooltipStartTime = timeGetTime();

	// tooltip string
	m_byTooltipStringNum = pi_byLineNum;
	if( m_byTooltipStringNum > TOOLTIP_MAX_LINE )
		m_byTooltipStringNum = TOOLTIP_MAX_LINE;
	for( int i = 0; i < m_byTooltipStringNum; ++i )
	{
		strncpy( m_pTooltipString[i], pi_pStrLine[i], TOOLTIP_MAX_STR - 1 );
		m_pTooltipString[i][TOOLTIP_MAX_STR - 1] = '\0';
	}

	// color string
	m_byTooltipColorStrNum = pi_byColorStrNum;
	if( m_byTooltipColorStrNum > TOOLTIP_MAX_COLOR_STR_NUM )
		m_byTooltipColorStrNum = TOOLTIP_MAX_COLOR_STR_NUM;
	for( i = 0; i < m_byTooltipColorStrNum; ++i )
	{
		memcpy( &m_pTooltipColorStrInfo[i], &pi_pColorStr[i], sizeof(TOOLTIP_COLOR_STR_INFO) );
		m_pTooltipColorStrInfo[i].pString[TOOLTIP_MAX_STR - 1] = '\0';
	}

	// ------------------------------------------------------------------------------------

	WORD	wMaxCharNum = 0;
	WORD	wCharNum;
	WORD	wLineNum;

	// default string
	for( i = 0; i < m_byTooltipStringNum; ++i )
	{
		wCharNum = strlen( m_pTooltipString[i] );

		if( wCharNum > wMaxCharNum )
			wMaxCharNum = wCharNum;
	}
	// m_pTooltipString의 line수가 총 line수이다.
	wLineNum = i;

	// color string
	for( i = 0; i < m_byTooltipColorStrNum; ++i )
	{
		wCharNum = strlen( m_pTooltipColorStrInfo[i].pString );

		if( wCharNum > wMaxCharNum )
			wMaxCharNum = wCharNum;
	}

	// set tooltip board size
	m_ptTooltipBoardSize.x = wMaxCharNum * ONEBYTE_FONTSIZE + TOOLTIP_MARGIN * 2;
	m_ptTooltipBoardSize.y = wLineNum * ( TWOBYTE_FONTSIZE + TOOLTIP_V_GAP ) + TOOLTIP_MARGIN * 2;
}

void
CGUIScreen::SetTooltip_Item( CGUIItem *	pi_pItem,
							 BYTE pi_byLineNum, char pi_pStrLine[][TOOLTIP_MAX_STR],
							 BYTE pi_byColorStrNum, TOOLTIP_COLOR_STR_INFO * pi_pColorStr,
							 UPGRADE_ITEM_INFO * pi_pUpgradeInfo )
{
	// set tooltip type
	m_byTooltipType = TOOLTIP_TYPE_ITEM;

	m_dwTooltipStartTime = timeGetTime();

	// tooltip item setting
	m_cTooltipItem.SetData( pi_pItem );
	m_cTooltipItem.SetSize( 64, 64 );
	m_cTooltipItem.DrawItemNum( FALSE );

	// tooltip string
	m_byTooltipStringNum = pi_byLineNum;
	if( m_byTooltipStringNum > TOOLTIP_MAX_LINE )
		m_byTooltipStringNum = TOOLTIP_MAX_LINE;
	for( int i = 0; i < m_byTooltipStringNum; ++i )
	{
		strncpy( m_pTooltipString[i], pi_pStrLine[i], TOOLTIP_MAX_STR - 1 );
		m_pTooltipString[i][TOOLTIP_MAX_STR - 1] = '\0';
	}

	// color string
	m_byTooltipColorStrNum = pi_byColorStrNum;
	if( m_byTooltipColorStrNum > TOOLTIP_MAX_COLOR_STR_NUM )
		m_byTooltipColorStrNum = TOOLTIP_MAX_COLOR_STR_NUM;
	for( i = 0; i < m_byTooltipColorStrNum; ++i )
	{
		memcpy( &m_pTooltipColorStrInfo[i], &pi_pColorStr[i], sizeof(TOOLTIP_COLOR_STR_INFO) );
		m_pTooltipColorStrInfo[i].pString[TOOLTIP_MAX_STR - 1] = '\0';
	}

	// ------------------------------------------------------------------------------------

	WORD	wMaxCharNum;
	WORD	wCharNum;
	WORD	wLineNum;

#define	ITEM_CHAR_NUM	14		// 최소한 아이템 이미지 크기만큼 문자가 있어야한다.

	wMaxCharNum = ITEM_CHAR_NUM;	

	// 업그레이드 아이템이면 업그레이드 소켓을 기준으로 최소한의 크기로 잡는다.	
	if( pi_pUpgradeInfo &&
		pi_pUpgradeInfo->byTotalSocketNum > 0 )
	{
		// set upgrade item info
		m_byTotalUpgradeSocketNum	= pi_pUpgradeInfo->byTotalSocketNum;
		m_byTotalUpgradeItemNum		= pi_pUpgradeInfo->byTotalItemNum;
		m_byUpgradeSocketLineIdx	= pi_pUpgradeInfo->byUpgradeSocketLineIdx;
		m_byUpgradeSocketCharIdx	= pi_pUpgradeInfo->byUpgradeSocketCharIdx;
		for( i = 0; i < m_byTotalUpgradeItemNum; ++i )
		{
			m_cUpgradeItem[i].SetSpriteID( pi_pUpgradeInfo->dwSpriteIDList[i] );
			m_cUpgradeItem[i].SetSprite();			
		}

		// set maxcharnum
		wMaxCharNum = ( m_byTotalUpgradeSocketNum * m_cUpgradeSocket.m_ptSize.x ) / ONEBYTE_FONTSIZE + 20;
		if( wMaxCharNum < ITEM_CHAR_NUM )
			wMaxCharNum = ITEM_CHAR_NUM;		
	}
	else
	{
		m_byTotalUpgradeSocketNum = 0;
	}

	// default string
	for( i = 0; i < m_byTooltipStringNum; ++i )
	{
		wCharNum = strlen( m_pTooltipString[i] );

		// 아이템 이미지 옆에 string인경우 아이템 이미지 크기만큼 문자수를 더한다.
		if( i < 4 )
			wCharNum += ITEM_CHAR_NUM;

		if( wCharNum > wMaxCharNum )
			wMaxCharNum = wCharNum;
	}
	// m_pTooltipString의 line수가 총 line수이다.
	wLineNum = i;

	// color string
	for( i = 0; i < m_byTooltipColorStrNum; ++i )
	{
		wCharNum = strlen( m_pTooltipColorStrInfo[i].pString );

		// 아이템 이미지 옆에 string인경우 아이템 이미지 크기만큼 문자수를 더한다.
		if( m_pTooltipColorStrInfo[i].byLineIndex < 4 )
			wCharNum += ITEM_CHAR_NUM;

		if( wCharNum > wMaxCharNum )
			wMaxCharNum = wCharNum;
	}
	
	m_ptTooltipBoardSize.x = wMaxCharNum * ONEBYTE_FONTSIZE + TOOLTIP_MARGIN * 2;
	m_ptTooltipBoardSize.y = wLineNum * ( TWOBYTE_FONTSIZE + TOOLTIP_V_GAP ) + TOOLTIP_MARGIN * 2;

	// 이미지 최소 크기 검사
	if( m_ptTooltipBoardSize.y < m_cTooltipItemBase.m_ptSize.y + ( TWOBYTE_FONTSIZE + TOOLTIP_V_GAP ) + TOOLTIP_MARGIN * 2 )
		m_ptTooltipBoardSize.y = m_cTooltipItemBase.m_ptSize.y + ( TWOBYTE_FONTSIZE + TOOLTIP_V_GAP ) + TOOLTIP_MARGIN * 2;
}

void
CGUIScreen::RemoveTooltip( void )
{
	m_pTooltipString[0][0]	= '\0';

	m_byTooltipStringNum	= 0;
	m_byTooltipColorStrNum	= 0;
}

void
CGUIScreen::DrawTooltip( void )
{
	// draw background
	Draw2DRectangle( m_ptTooltipBoardPos.x,
					 m_ptTooltipBoardPos.y,
					 m_ptTooltipBoardPos.x + m_ptTooltipBoardSize.x,
					 m_ptTooltipBoardPos.y + m_ptTooltipBoardSize.y,					 
					 TOOLTIP_BASE );

	// top
	Draw2DRectangle( m_ptTooltipBoardPos.x + TOOLTIP_OUTLINE_GAP,
					 m_ptTooltipBoardPos.y + TOOLTIP_OUTLINE_GAP,
					 m_ptTooltipBoardPos.x + m_ptTooltipBoardSize.x - TOOLTIP_OUTLINE_GAP,
					 m_ptTooltipBoardPos.y + TOOLTIP_BORDER + TOOLTIP_OUTLINE_GAP,					 
					 TOOLTIP_OUTLINE );
	// bottom
	Draw2DRectangle( m_ptTooltipBoardPos.x + TOOLTIP_OUTLINE_GAP,
					 m_ptTooltipBoardPos.y + m_ptTooltipBoardSize.y - TOOLTIP_BORDER - TOOLTIP_OUTLINE_GAP,
					 m_ptTooltipBoardPos.x + m_ptTooltipBoardSize.x - TOOLTIP_OUTLINE_GAP,
					 m_ptTooltipBoardPos.y + m_ptTooltipBoardSize.y - TOOLTIP_OUTLINE_GAP,					 
					 TOOLTIP_OUTLINE );

	// left
	Draw2DRectangle( m_ptTooltipBoardPos.x + TOOLTIP_OUTLINE_GAP,
					 m_ptTooltipBoardPos.y + TOOLTIP_OUTLINE_GAP,
					 m_ptTooltipBoardPos.x + TOOLTIP_BORDER + TOOLTIP_OUTLINE_GAP,
					 m_ptTooltipBoardPos.y + m_ptTooltipBoardSize.y - TOOLTIP_OUTLINE_GAP,					 
					 TOOLTIP_OUTLINE );
	// right
	Draw2DRectangle( m_ptTooltipBoardPos.x + m_ptTooltipBoardSize.x - TOOLTIP_BORDER - TOOLTIP_OUTLINE_GAP,
					 m_ptTooltipBoardPos.y + TOOLTIP_OUTLINE_GAP,
					 m_ptTooltipBoardPos.x + m_ptTooltipBoardSize.x - TOOLTIP_OUTLINE_GAP,
					 m_ptTooltipBoardPos.y + m_ptTooltipBoardSize.y - TOOLTIP_OUTLINE_GAP,
					 TOOLTIP_OUTLINE );

	if( m_byTooltipType == TOOLTIP_TYPE_ITEM )
	{
		// draw item base
		m_cTooltipItemBase.Draw();
		// draw item
		m_cTooltipItem.Draw();
	}
	

	// draw text
	for( int i = 0; i < m_byTooltipStringNum; ++i )
	{
		if( m_pTooltipString[i][0] == '\0' || m_pTooltipString[i][0] == '\n' )
			continue;

		// title
		if( i == 0 )
			DrawR3Hangul( m_ptTooltipBoardPos.x + 
								( m_ptTooltipBoardSize.x - strlen(m_pTooltipString[i])*ONEBYTE_FONTSIZE )/2,
						  m_ptTooltipBoardPos.y + TOOLTIP_MARGIN,
						  m_pTooltipString[i],
						  TOOLTIP_STR_COLOR );
		else
			DrawR3Hangul( m_ptTooltipBoardPos.x + TOOLTIP_MARGIN,
						  m_ptTooltipBoardPos.y + TOOLTIP_MARGIN + (TWOBYTE_FONTSIZE + TOOLTIP_V_GAP) * i,
						  m_pTooltipString[i],
						  TOOLTIP_STR_COLOR );
	}

	// color text
	for( i = 0; i < m_byTooltipColorStrNum; ++i )
	{
		if( m_pTooltipColorStrInfo[i].pString[0] == '\0' || m_pTooltipColorStrInfo[i].pString[0] == '\n' )
			continue;

		// title - 첫번째 라인만 charIndex에 상관없이 가운데 정렬한다.
		if( m_pTooltipColorStrInfo[i].byLineIndex == 0 )			
			DrawR3Hangul( m_ptTooltipBoardPos.x + 
								( m_ptTooltipBoardSize.x - strlen(m_pTooltipColorStrInfo[i].pString)*ONEBYTE_FONTSIZE )/2,
						  m_ptTooltipBoardPos.y + TOOLTIP_MARGIN,
						  m_pTooltipColorStrInfo[i].pString,
						  m_pTooltipColorStrInfo[i].dwColor );
		else
			DrawR3Hangul( m_ptTooltipBoardPos.x + TOOLTIP_MARGIN + ONEBYTE_FONTSIZE * m_pTooltipColorStrInfo[i].byCharIndex,
						  m_ptTooltipBoardPos.y + TOOLTIP_MARGIN + 
								(TWOBYTE_FONTSIZE + TOOLTIP_V_GAP) * m_pTooltipColorStrInfo[i].byLineIndex,
						  m_pTooltipColorStrInfo[i].pString,
						  m_pTooltipColorStrInfo[i].dwColor );
	}

	// upgrade socket
	if( m_byTooltipType == TOOLTIP_TYPE_ITEM &&
		m_byTotalUpgradeSocketNum > 0 )
	{
		POINT	ptPos;
		ptPos.x = m_ptTooltipBoardPos.x + TOOLTIP_MARGIN + ONEBYTE_FONTSIZE * m_byUpgradeSocketCharIdx - 4;
		ptPos.y = m_ptTooltipBoardPos.y + TOOLTIP_MARGIN + (TWOBYTE_FONTSIZE + TOOLTIP_V_GAP) * m_byUpgradeSocketLineIdx - 3;

		// draw socket
		for( i = 0; i < m_byTotalUpgradeSocketNum; ++i )
		{
			m_cUpgradeSocket.SetPos( ptPos );
									  
			m_cUpgradeSocket.Draw();

			ptPos.x += m_cUpgradeSocket.m_ptSize.x;
		}

		// draw item
		ptPos.x = m_ptTooltipBoardPos.x + TOOLTIP_MARGIN + ONEBYTE_FONTSIZE * m_byUpgradeSocketCharIdx - 11;
		ptPos.y -= 2;

		for( i = 0; i < m_byTotalUpgradeItemNum; ++i )
		{
			m_cUpgradeItem[i].SetPos( ptPos );

			m_cUpgradeItem[i].Draw();
			
			ptPos.x += m_cUpgradeSocket.m_ptSize.x;
		}
	}
}
