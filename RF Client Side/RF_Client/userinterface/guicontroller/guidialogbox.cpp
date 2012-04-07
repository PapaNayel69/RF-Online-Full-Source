////////////////////////////////////////////////////////////////////////////
//
// CGUIDialogBox Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "GUIDialogBox.h"
#include "GUIScreen.h"

// ##########################################################################
// dialogbox가 열려있는 동안은 다른 윈도우에 입력을 못가게 하도록 한다.
void	EP_OpenDialogBox( CGUIWindow * pi_pOwnerWindow, CGUIDialogBox * pi_pDialogBox );
// ##########################################################################

int g_nLockDlgBoxCount = 0;	// 열려있는 동안 다른 윈도우에 lock을 거는 dialog box 갯수

void
_CloseDialogBox( CGUIWindow * pi_pOwnerWindow/*=NULL*/,
				 WORD pi_wUserDefinedID/*=NOT_DEFINE_ID*/ )
{
	CGUIWindow * l_pOwnerWindow;

	if( pi_pOwnerWindow == NULL )
		l_pOwnerWindow = &gScreen;
	else
		l_pOwnerWindow = pi_pOwnerWindow;
		
	
	// 맨 마지막 dialogbox만 close할수 있다.

	// 맨 마지막 dialogbox를 가져온다.
	CGUIDialogBox * l_pDialogBox = l_pOwnerWindow->GetLastDialogBox();
	if( l_pDialogBox == NULL )
		return;

	if( l_pDialogBox->GetUserDefinedID() == pi_wUserDefinedID )
	{		
		l_pDialogBox->SetResultMsg( MSG_NONE );
		l_pOwnerWindow->ClosingDialogBox( l_pDialogBox );
	}
}

void
_CloseAllDialogBox( CGUIWindow * pi_pOwnerWindow/*=NULL*/ )
{
	CGUIWindow * l_pOwnerWindow;

	if( pi_pOwnerWindow == NULL )
		l_pOwnerWindow = &gScreen;
	else
		l_pOwnerWindow = pi_pOwnerWindow;

	// 맨 마지막꺼부터 순서대로 닫는다.
	CGUIWindow::CGUIDialogBoxList * l_pDlgList;
	l_pDlgList = l_pOwnerWindow->GetDialogBoxList();
	
	CGUIWindow::CGUIDialogBoxList::reverse_iterator	it;
	for( it = l_pDlgList->rbegin(); it != l_pDlgList->rend(); ++it )
	{
		(*it)->SetResultMsg( MSG_NONE );
		//(*it)->SendDialogBoxEvent( DLG_BOX_CLOSED );
		l_pOwnerWindow->ClosingDialogBox( *it );
	}		
}

BOOL
_IsOpenedDialogBox( CGUIWindow * pi_pOwnerWindow/*=NULL*/,
					WORD pi_wUserDefinedID/*=NOT_DEFINE_ID*/ )
{
	CGUIWindow * l_pOwnerWindow;

	if( pi_pOwnerWindow == NULL )
		l_pOwnerWindow = &gScreen;
	else
		l_pOwnerWindow = pi_pOwnerWindow;

	CGUIWindow::CGUIDialogBoxList * l_pDlgList;
	l_pDlgList = l_pOwnerWindow->GetDialogBoxList();
	
	CGUIWindow::CGUIDialogBoxList::iterator	it;
	for( it = l_pDlgList->begin(); it != l_pDlgList->end(); ++it )
	{
		if( (*it)->GetUserDefinedID() == pi_wUserDefinedID )
		{
			if( (*it)->IsShow() )
				return TRUE;
			else
				return FALSE;
		}
	}	

	return FALSE;	
}

CGUIDialogBox::CGUIDialogBox()
{	
	m_pOwnerWindow		= NULL;

	m_wDlgStyleID		= DLG_STYLE_NONE;
	
	m_bOpened			= FALSE;
	m_bDestroy			= FALSE;

	m_bModal			= TRUE;
	m_bLockOtherWin		= TRUE;

	m_wResultMsg		= MSG_NONE;
}

CGUIDialogBox::CGUIDialogBox( CGUIWindow * pi_pOwnerWindow, WORD pi_wType, BOOL	pi_bModal, WORD pi_wUserDefinedID )
{	
	m_pOwnerWindow	= pi_pOwnerWindow;

	m_wUserDefinedID = pi_wUserDefinedID;

	// set dialog box type
	if( pi_wType >= DLG_STYLE_OK_CANCEL &&
		pi_wType <= DLG_STYLE_OK )
	{
		m_wDlgStyleID = pi_wType;
	}
	else
	{
		m_wDlgStyleID = DLG_STYLE_NONE;
	}

	// set modal
	m_bModal = pi_bModal;	

	m_uiOKButton.AddMouseEventHandler( this );
	m_uiCancelButton.AddMouseEventHandler( this );	
	m_uiCloseButton.AddMouseEventHandler( this );	
	
	switch( m_wDlgStyleID )
	{
	case DLG_STYLE_OK_CANCEL:
		{			
			Add( &m_uiOKButton );
			Add( &m_uiCancelButton );
		}
		break;
	case DLG_STYLE_OK:
		{
			Add( &m_uiOKButton );
		}
		break;	
	}

	Add( &m_uiCloseButton );	
	

	// init resultmsg
	m_wResultMsg	= MSG_NONE;		
	
	m_bOpened		= FALSE;
	m_bDestroy		= FALSE;	

	m_bModal		= TRUE;
	m_bLockOtherWin	= TRUE;	
}

CGUIDialogBox::~CGUIDialogBox()
{
	RemoveOwnerWindow();
}

void
CGUIDialogBox::SetOwnerWindow( CGUIWindow * pi_pWin )
{	
	if( pi_pWin == NULL )
		m_pOwnerWindow = &gScreen;
	else	
		m_pOwnerWindow = pi_pWin;	
}

void
CGUIDialogBox::RemoveOwnerWindow( void )
{
	if( !m_pOwnerWindow )
		return;

	m_pOwnerWindow->RemoveDialogBox( this );
	m_pOwnerWindow = NULL;
}

void
CGUIDialogBox::LockOtherWin( BOOL pi_bLock )
{
	if( m_bLockOtherWin == pi_bLock )
		return;

	if( pi_bLock )
	{
		if( m_bOpened )
			++g_nLockDlgBoxCount;
	}
	else
	{
		if( m_bOpened )
			--g_nLockDlgBoxCount;		
	}

	m_bLockOtherWin = pi_bLock;
}

void
CGUIDialogBox::Open( void )
{
	if( !m_pOwnerWindow )
		return;

	if( m_bOpened )
	{
		CGUIWindow::Open();
		return;
	}

	// ################## 코드 추가 시킬 곳 #################
	EP_OpenDialogBox( m_pOwnerWindow, this );
	// ######################################################

	m_bOpened = true;
	
	// 우선 ownerwindow에 input이 전달되는 것을 막기위해 inactive시킨다.
	m_pOwnerWindow->InActive();
	// Ownerwindow에 dialogbox를 둥록시킨다.
	m_pOwnerWindow->AddDialogBox( this, true );

	// init result msg
	m_wResultMsg = MSG_NONE;
	
	CGUIWindow::Open();

	if( m_bLockOtherWin )
		++g_nLockDlgBoxCount;
}

void
CGUIDialogBox::Close( void )
{
	if( !m_bOpened )
		return;

	// m_bOpened flag를 false로 해주면 ownerwindow에서 알아서 close해준다.
	m_bOpened = false;	
	
	CGUIWindow::Close();

	if( m_bLockOtherWin )
		--g_nLockDlgBoxCount;
}

void
CGUIDialogBox::ActiveWindow( CGUIWindow * pi_pWin )
{
	CGUIWindow * l_pParent;
	l_pParent = pi_pWin->GetParent();
	
	if( l_pParent )		
		l_pParent->ActiveWindow( l_pParent );
}

void
CGUIDialogBox::InActiveWindow( CGUIWindow * pi_pWin )
{
	if( IsShow() )
		Show( false );

	if( IsActive() )
		InActive();
}

void
CGUIDialogBox::MouseEvent( _mouse_event & event )
{	
	CGUIWindow::MouseEvent( event );

	if( event.id == MOUSE_CLICKED )
	{
		if( event.source == &m_uiOKButton )
		{
			m_wResultMsg = MSG_OK;
			
			SendDialogBoxEvent( DLG_BOX_CLOSED );
		}
		else if( event.source == &m_uiCancelButton )
		{
			m_wResultMsg = MSG_CANCEL;
			
			SendDialogBoxEvent( DLG_BOX_CLOSED );
		}
		else if( event.source == &m_uiCloseButton )
		{
			m_wResultMsg = MSG_CLOSE;			
			
			SendDialogBoxEvent( DLG_BOX_CLOSED );
		}		
	}

	return;
}

LRESULT
CGUIDialogBox::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
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

		
		switch( uMsg )
		{
		case WM_KEYDOWN:		
			if( wParam == VK_RETURN )
			{
				m_wResultMsg = MSG_OK;				
				
				SendDialogBoxEvent( DLG_BOX_CLOSED );

				return 1;
			}
			break;

		case WM_CHAR:
			if( wParam == VK_ESCAPE )
			{
				m_wResultMsg = MSG_CANCEL;				
				
				SendDialogBoxEvent( DLG_BOX_CLOSED );

				return 1;
			}
			
			if( m_wDlgStyleID == DLG_STYLE_OK_CANCEL )
			{
				if( wParam == 'y' ||
					wParam == 'Y' ) 
				{
					m_wResultMsg = MSG_OK;

					SendDialogBoxEvent( DLG_BOX_CLOSED );

					return 1;
				}
				else if( wParam == 'n' ||
						 wParam == 'N' )
				{
					m_wResultMsg = MSG_CANCEL;

					SendDialogBoxEvent( DLG_BOX_CLOSED );

					return 1;
				}
			}

			break;
		}
		

		CGUIContainer::MsgProc( hWnd, uMsg, wParam, lParam );
	}
	
	return 1;
}

void
CGUIDialogBox::SendDialogBoxEvent( WORD pi_wID )
{
	_event_queue_data	l_EventInfo;

	l_EventInfo.pEventHandler = m_pOwnerWindow;
	l_EventInfo.pSource = this;
	l_EventInfo.wEventID = pi_wID;	
	
	CGUIObject::SendEvent( l_EventInfo );
}