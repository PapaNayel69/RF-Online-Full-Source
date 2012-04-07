////////////////////////////////////////////////////////////////////////////
//
// CGUILoginWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "GUILoginWindow.h"
#include "../GUIController/GUIMessagebox.h"
#include "../GUIController/GUIScreen.h"
#include "../SpriteMgr.h"

///*MAYUN
//void PlayButtonSound();
void EventLoginFunc(char *id, char *password);
void EventCreateAccount(char *name, char *password);
#define INPUTLEN	16
//*/

CGUILoginWindow::CGUILoginWindow()
{
	m_pIDCreateDlg	= NULL;

	m_bCreated		= FALSE;
}

CGUILoginWindow::~CGUILoginWindow()
{
	if( m_pIDCreateDlg != NULL )
		delete m_pIDCreateDlg;
}

void
CGUILoginWindow::Init( void )
{
	POINT	l_ptPos;
	POINT	l_ptSize;

	//--------------------------------------------------------------
	// set size
	l_ptSize.x = gScreen.m_ptSize.x;
	l_ptSize.y = gScreen.m_ptSize.y;
	SetSize( l_ptSize );

	l_ptSize.x = 206;
	l_ptSize.y = 135;
	m_cBase.SetSize( l_ptSize );	
	m_cBoard.SetSize( l_ptSize );

	l_ptSize.x = 98;
	l_ptSize.y = 20;
	m_cIDInput.SetSize( l_ptSize );
	m_cPasswordInput.SetSize( l_ptSize );

	l_ptSize.x = 80;
	l_ptSize.y = 30;
	m_cCreateButton.SetSize( l_ptSize );
	m_cJoinButton.SetSize( l_ptSize );

	//--------------------------------------------------------------
	// set position
	l_ptPos.x = ( m_ptSize.x - m_cBoard.m_ptSize.x ) * 0.5;
	l_ptPos.y = m_ptSize.y - 200 ;
	m_cBase.SetPos( l_ptPos );
	m_cBoard.SetPos( l_ptPos );

	l_ptPos.x = m_cBase.m_ptPos.x + 89;
	l_ptPos.y = m_cBase.m_ptPos.y + 31;
	m_cIDInput.SetPos( l_ptPos );
	
	l_ptPos.y = m_cBase.m_ptPos.y + 65;
	m_cPasswordInput.SetPos( l_ptPos );

	l_ptPos.x = m_cBase.m_ptPos.x + 4;
	l_ptPos.y = m_cBase.m_ptPos.y + 100;
	m_cCreateButton.SetPos( l_ptPos );

	l_ptPos.x = m_cBoard.m_ptPos.x + 121;
	l_ptPos.y = m_cBoard.m_ptPos.y + 100;
	m_cJoinButton.SetPos( l_ptPos );

	
	// add event handler
	m_cCreateButton.AddMouseEventHandler( this );
	m_cJoinButton.AddMouseEventHandler( this );

	m_cIDInput.AddKeyEventHandler( this );
	m_cPasswordInput.AddKeyEventHandler( this );

	// add controller
	m_cBoard.Add( &m_cIDInput );
	m_cBoard.Add( &m_cPasswordInput );
	m_cBoard.Add( &m_cCreateButton );
	m_cBoard.Add( &m_cJoinButton );

	m_cBase.Add( &m_cBoard );

	Add( &m_cBase );

	m_cIDInput.SetMargin( 4, 4 );
	m_cPasswordInput.SetMargin( 4, 4 );

	// set sprite
	SetSprite();

	DisableFocus();	
	m_cIDInput.EnableFocus();
	m_cPasswordInput.EnableFocus();	

	SetFocusObject( &m_cIDInput );

	m_cPasswordInput.SetShowText( false );
	
	///*MAYUN
	m_cIDInput.SetInputLimit(INPUTLEN);
	m_cPasswordInput.SetInputLimit(INPUTLEN);
	//*/

	m_cBase.SetARGB( 0xbfffffff );

	m_bCreated = TRUE;
}

void
CGUILoginWindow::SetSprite( void )
{
	if( !m_bCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_LOGIN );
		l_spriteInfo.wActionNo = 0;

		// base
		l_spriteInfo.wFrameNo = 0;
		m_cBase.SetSpriteInfo( &l_spriteInfo );	
		
		// board
		l_spriteInfo.wFrameNo = 1;
		m_cBoard.SetSpriteInfo( &l_spriteInfo );	

		// create button
		l_spriteInfo.wFrameNo = 2;
		m_cCreateButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 3;
		m_cCreateButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 4;
		m_cCreateButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );

		// join button
		l_spriteInfo.wFrameNo = 5;
		m_cJoinButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 6;
		m_cJoinButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 7;
		m_cJoinButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_LOGIN );
		if( !l_pSprite )
			return;		

		// base
		m_cBase.GetSpriteInfo()->pSprite = l_pSprite;

		// board		
		m_cBoard.GetSpriteInfo()->pSprite = l_pSprite;

		// create button
		m_cCreateButton.SetSprite( l_pSprite );

		// join button
		m_cJoinButton.SetSprite( l_pSprite );						
	}

	if( m_pIDCreateDlg )
		m_pIDCreateDlg->SetSprite();
}


void
CGUILoginWindow::MouseEvent( _mouse_event & event )
{
	if( event.id == MOUSE_CLICKED )
	{
		if( event.source == &m_cCreateButton )
		{
			//PlayButtonSound();
			// open id create window
			if( !m_pIDCreateDlg )
			{
				m_pIDCreateDlg = new CGUIIDCreateDlg( this );
				POINT l_pos;
				l_pos.x = m_cBoard.m_ptPos.x + ( m_cBoard.m_ptSize.x - m_pIDCreateDlg->m_ptSize.x ) * 0.5f;
				l_pos.y = m_cBoard.m_ptPos.y + ( m_cBoard.m_ptSize.y - m_pIDCreateDlg->m_ptSize.y ) * 0.5f;
				m_pIDCreateDlg->SetPos( l_pos );				
			}					
			
			m_pIDCreateDlg->SetFocusObject( &m_pIDCreateDlg->m_cIDInput );
			m_pIDCreateDlg->SetOwnerWindow( this );
			m_pIDCreateDlg->Open();

			//Show( false );						
			m_cBase.Show( false );
		}
		else if( event.source == &m_cJoinButton )
		{			
			//PlayButtonSound();
			char * id = m_cIDInput.GetText();
			char * password = m_cPasswordInput.GetText();

			if( !strlen( id ) )
			{
				_GUIMessageBox( this, "아이디를 입력하세요" );

				SetFocusObject( &m_cIDInput );			
			}			
			else if( !strlen( password ) ) 
			{
				_GUIMessageBox( this, "패스워드를 입력하세요" );

				SetFocusObject( &m_cPasswordInput );				
			}
			else
			{
				// ################## 코드 추가 시킬 곳 ###########################
				// Join Button을 눌렀다.
				// ################################################################				
				EventLoginFunc(id,password);
				m_cIDInput.Clear();
				m_cPasswordInput.Clear();
			}
		}
	}
}

void
CGUILoginWindow::KeyEvent( _key_event & event )
{
	if( event.source == &m_cIDInput || event.source == &m_cPasswordInput )
	{
		if( event.key == VK_RETURN )
		{
			char * id = m_cIDInput.GetText();
			char * password = m_cPasswordInput.GetText();
			// ################## 코드 추가 시킬 곳 ###########################
			// Join
			// ################################################################
			if( strlen( id ) == 0 )
			{
				_GUIMessageBox( this, "아이디를 입력하세요" );

				SetFocusObject( &m_cIDInput );
			}
			else if( strlen( password ) == 0 )
			{
				_GUIMessageBox( this, "패스워드를 입력하세요" );

				SetFocusObject( &m_cPasswordInput );
			}
			else 
			{
				EventLoginFunc( id, password );
				m_cIDInput.Clear();
				m_cPasswordInput.Clear();
			}
		}
		else if( event.key == VK_TAB )
		{
			if( event.source == &m_cIDInput )
				m_cPasswordInput.SetFocus();
			
			else if( event.source == &m_cPasswordInput )			
				m_cIDInput.SetFocus();
		}
	}
}

void
CGUILoginWindow::ClosingDialogBox( CGUIDialogBox * pi_pDlg )
{
	if( pi_pDlg == m_pIDCreateDlg )
	{
		if( m_pIDCreateDlg->GetResultMsg() == MSG_OK )
		{
			// ################## 코드 추가 시킬 곳 ###########################
			// ID와 password를 등록한다.
			// ################################################################

			//PlayButtonSound();			
			char * id = m_pIDCreateDlg->GetID();
			char * password = m_pIDCreateDlg->GetPassword();	
			if( strlen( id ) == 0 )
			{
				_GUIMessageBox( pi_pDlg, "아이디를 입력하세요");		
				
				m_pIDCreateDlg->SetFocusObject( &m_pIDCreateDlg->m_cIDInput );

				return;	// 아이디 생성창을 닫지 않는다.
			}
			else if( strlen( password ) == 0 )
			{
				_GUIMessageBox( pi_pDlg, "패스워드를 입력하세요");			

				m_pIDCreateDlg->SetFocusObject( &m_pIDCreateDlg->m_cPasswordInput );

				return;
			}
			else if( strcmp( password, m_pIDCreateDlg->GetRePassword() ) )
			{
				_GUIMessageBox( pi_pDlg, "패스워드가 일치하지 않습니다.");			
				
				m_pIDCreateDlg->SetFocusObject( &m_pIDCreateDlg->m_cRePasswordInput );

				return;
			}
			else
			{
				// id 등록
				EventCreateAccount(id,password);
				m_pIDCreateDlg->ClearID();
				m_pIDCreateDlg->ClearPassword();			
			}
		}
		else 
		{
			m_pIDCreateDlg->ClearID();
			m_pIDCreateDlg->ClearPassword();
		}		
	}

	CGUIWindow::ClosingDialogBox( pi_pDlg );
}

////////////////////////////////////////////////////////////////////////////
//
// CGUIIDCreateDlg Class Implementation
//
////////////////////////////////////////////////////////////////////////////
CGUIIDCreateDlg::CGUIIDCreateDlg( CGUILoginWindow * pi_pWin )
:CGUIDialogBox( (CGUIWindow *)pi_pWin, DLG_STYLE_OK_CANCEL, true, NOT_DEFINE_ID )
{	
	m_bCreated = FALSE;

	Init();	
	
	m_pLoginWindow = pi_pWin;	
}

CGUIIDCreateDlg::~CGUIIDCreateDlg()
{

}

void
CGUIIDCreateDlg::Init( void )
{
	POINT	l_ptPos;
	POINT	l_ptSize;

	//--------------------------------------------------------------
	// set size
	l_ptSize.x = 206;
	l_ptSize.y = 170;
	SetSize( l_ptSize );

	m_cBoard.SetSize( l_ptSize );

	l_ptSize.x = 99;
	l_ptSize.y = 20;
	m_cIDInput.SetSize( l_ptSize );
	m_cPasswordInput.SetSize( l_ptSize );
	m_cRePasswordInput.SetSize( l_ptSize );

	l_ptSize.x = 80;
	l_ptSize.y = 30;
	m_uiOKButton.SetSize( l_ptSize );
	m_uiCancelButton.SetSize( l_ptSize );

	//--------------------------------------------------------------
	// set position
	l_ptPos.x = 89;
	l_ptPos.y = 32;
	m_cIDInput.SetPos( l_ptPos );	
	l_ptPos.y = 66;
	m_cPasswordInput.SetPos( l_ptPos );
	l_ptPos.y = 100;
	m_cRePasswordInput.SetPos( l_ptPos );	

	l_ptPos.x = 4;
	l_ptPos.y = 135;
	m_uiOKButton.SetPos( l_ptPos ); 
	l_ptPos.x = 121;
	l_ptPos.y = 135;
	m_uiCancelButton.SetPos( l_ptPos );

	m_cIDInput.AddKeyEventHandler( this );
	m_cPasswordInput.AddKeyEventHandler( this );
	m_cRePasswordInput.AddKeyEventHandler( this );

	m_cIDInput.AddMouseEventHandler( this );
	m_cPasswordInput.AddMouseEventHandler( this );
	m_cRePasswordInput.AddMouseEventHandler( this );

	m_cBoard.Add( &m_cIDInput );
	m_cBoard.Add( &m_cPasswordInput );
	m_cBoard.Add( &m_cRePasswordInput );

	Remove( &m_uiOKButton );
	Remove( &m_uiCancelButton );

	m_cBoard.Add( &m_uiOKButton );
	m_cBoard.Add( &m_uiCancelButton );

	Add( &m_cBoard );

	m_cIDInput.SetMargin( 4, 4 );
	m_cPasswordInput.SetMargin( 4, 4 );
	m_cRePasswordInput.SetMargin( 4, 4 );
	
	SetSprite();

	m_cPasswordInput.SetShowText( false );
	m_cRePasswordInput.SetShowText( false );

	m_cIDInput.SetInputLimit(INPUTLEN);
	m_cPasswordInput.SetInputLimit(INPUTLEN);
	m_cRePasswordInput.SetInputLimit(INPUTLEN);

	// init focused object
	SetFocusObject( &m_cIDInput );

	DisableFocus();	
	m_cIDInput.EnableFocus();
	m_cPasswordInput.EnableFocus();
	m_cRePasswordInput.EnableFocus();

	// test code
	SetName( "CreateID" );
	m_cIDInput.SetName("id");
	m_cPasswordInput.SetName("pw");
	m_cPasswordInput.SetName("repw");

	SetARGB( 0xb0ffffff );

	m_bCreated = TRUE;
}

void
CGUIIDCreateDlg::SetSprite( void )
{
	if( !m_bCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_LOGIN );
		l_spriteInfo.wActionNo = 1;

		// background	
		l_spriteInfo.wFrameNo = 0;	
		SetSpriteInfo( &l_spriteInfo );

		// board
		l_spriteInfo.wFrameNo = 1;
		m_cBoard.SetSpriteInfo( &l_spriteInfo );

		// ok button
		l_spriteInfo.wFrameNo = 2;
		m_uiOKButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 3;
		m_uiOKButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 4;
		m_uiOKButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );

		// cancel button
		l_spriteInfo.wFrameNo = 5;
		m_uiCancelButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 6;
		m_uiCancelButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 7;
		m_uiCancelButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_LOGIN );
		if( !l_pSprite )
			return;		

		// background	
		GetSpriteInfo()->pSprite = l_pSprite;

		// board
		m_cBoard.GetSpriteInfo()->pSprite = l_pSprite;

		m_uiOKButton.SetSprite( l_pSprite );

		m_uiCancelButton.SetSprite( l_pSprite );
	}
}

void
CGUIIDCreateDlg::Close( void )
{
	CGUIDialogBox::Close();
	
	// background에 알파가 먹혀서 LoginWindow가 보인다.
	// 그래서 CGUIIDCreateDlg를 열었을 때 LoginWindow의 
	// 보이는 부분만 안보이게 해줬다가 닫을 때 다시 보여주게 한다.	
	m_pLoginWindow->m_cBase.Show( true );
}

void
CGUIIDCreateDlg::MouseEvent( _mouse_event & event )
{
	if( event.id == MOUSE_CLICKED )
	{
		if( event.source == &m_uiOKButton )
		{
			if( strlen( m_cIDInput.GetText() ) == 0 )
			{
				_GUIMessageBox( this, "아이디를 입력하세요");

				SetFocusObject( &m_cIDInput );
				return;
			}
			else if( strlen( m_cPasswordInput.GetText() ) == 0 )
			{
				_GUIMessageBox( this, "패스워드를 입력하세요");
				
				SetFocusObject( &m_cPasswordInput );
				return;
			}
			else if( strcmp( m_cPasswordInput.GetText(), m_cRePasswordInput.GetText() ) )
			{
				// messagebox
				_GUIMessageBox( this, "패스워드가 일치하지 않습니다.");
				
				SetFocusObject( &m_cRePasswordInput );
				return;
			}
		}
	}
	else if( event.id == MOUSE_STROKED )
	{
		if( event.source == &m_cIDInput )
		{
			m_cIDInput.SetFocus();
		}
		else if( event.source == &m_cPasswordInput )
		{
			m_cPasswordInput.SetFocus();
		}
		else if( event.source == &m_cRePasswordInput )
		{
			m_cRePasswordInput.SetFocus();
		}
	}

	CGUIDialogBox::MouseEvent( event );
}

void
CGUIIDCreateDlg::KeyEvent( _key_event & event )
{
	if( event.key == VK_TAB )
	{
		if( event.source == &m_cIDInput )
			m_cPasswordInput.SetFocus();
		
		else if( event.source == &m_cPasswordInput )
			m_cRePasswordInput.SetFocus();

		else if( event.source == &m_cRePasswordInput )
			m_cIDInput.SetFocus();
	}	

	CGUIDialogBox::KeyEvent( event );
}

LRESULT
CGUIIDCreateDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
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

			break;
		}
		

		CGUIContainer::MsgProc( hWnd, uMsg, wParam, lParam );
	}
	
	return 1;
}