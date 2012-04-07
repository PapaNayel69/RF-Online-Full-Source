#include "GUINumInputDlg.h"
#include "GUIScreen.h"
#include "../SpriteMgr.h"

////////////////////////////////////////////////////////////////////////////////////
CGUINumInputDlg		g_uiInputDlg;

void _GUINumInputDlg( CGUIWindow	*	pi_pOwnerWindow,
				   POINT			pi_ptEventPos,
				   int				pi_nMaxNumber,
				   WORD				pi_wUserDefinedID/*=NOT_DEFINE_ID*/,
				   int				pi_nInitNumber/*=0*/)
{
	if( g_uiInputDlg.IsOpen() )
		return;

	g_uiInputDlg.SetOwnerWindow( pi_pOwnerWindow );
	g_uiInputDlg.SetUserDefinedID( pi_wUserDefinedID );	

	g_uiInputDlg.SetMaxNumber( pi_nMaxNumber );
	
	if( pi_nInitNumber > 0 )
		g_uiInputDlg.SetNumber( pi_nInitNumber );
	else
		g_uiInputDlg.Clear();	

	g_uiInputDlg.InitInputState();


	g_uiInputDlg.SetSprite();

	POINT l_ptPos;
	l_ptPos.x = pi_ptEventPos.x - g_uiInputDlg.m_ptSize.x;
	l_ptPos.y = pi_ptEventPos.y - g_uiInputDlg.m_ptSize.y;

	if( l_ptPos.x < 0 )
		l_ptPos.x = 0;
	else if( l_ptPos.x + g_uiInputDlg.m_ptSize.x > gScreen.m_ptSize.x )
		l_ptPos.x = gScreen.m_ptSize.x - g_uiInputDlg.m_ptSize.x;

	if( l_ptPos.y < 0 )
		l_ptPos.y = 0;
	else if( l_ptPos.y + g_uiInputDlg.m_ptSize.y > gScreen.m_ptSize.y )
		l_ptPos.y = gScreen.m_ptSize.y - g_uiInputDlg.m_ptSize.y;

	g_uiInputDlg.SetPos( l_ptPos );

	g_uiInputDlg.Open();
}

////////////////////////////////////////////////////////////////////////////
//
// CGUIDialogBox Class Implementation
//
////////////////////////////////////////////////////////////////////////////

CGUINumInputDlg::CGUINumInputDlg()
:CGUIDialogBox( NULL, DLG_STYLE_OK_CANCEL, TRUE, NOT_DEFINE_ID )
{
	Init();
}

CGUINumInputDlg::CGUINumInputDlg( CGUIWindow * pi_pOwnerWindow, BOOL pi_bModal, WORD pi_wUserDefinedID/*=NOT_DEFINE_ID*/ )
:CGUIDialogBox( pi_pOwnerWindow, DLG_STYLE_OK_CANCEL, pi_bModal, pi_wUserDefinedID )
{
	Init();
}

CGUINumInputDlg::~CGUINumInputDlg()
{
	
}

void
CGUINumInputDlg::Init( void )
{
	SetSize( 109, 70 );	

	m_cInput.SetSize( 84, 13 );	
	m_cInputStr.SetSize( 27, 8 );
	m_cUpButton.SetSize( 15, 10 );
	m_cDownButton.SetSize( 15, 10 );

	m_uiOKButton.SetSize( 49, 19 );
	m_uiCancelButton.SetSize( 49, 19 );

	// ---------------------------------------------------------------------
	// position
	m_cInputStr.SetPos( m_ptPos.x + 3, m_ptPos.y + 2 );
	m_uiOKButton.SetPos( m_ptPos.x + 4, m_ptPos.y + 47 );
	m_uiCancelButton.SetPos( m_ptPos.x + 56, m_uiOKButton.m_ptPos.y );	
	m_cInput.SetPos( m_ptPos.x + 7, m_ptPos.y + 19 );
	m_cUpButton.SetPos( m_ptPos.x + 91, m_ptPos.y + 11 );
	m_cDownButton.SetPos( m_ptPos.x + 91, m_ptPos.y + 29 );	

	// ---------------------------------------------------------------------
	// add controller
	Add( &m_cInputStr );
	Add( &m_uiOKButton );
	Add( &m_uiCancelButton );
	Add( &m_uiCloseButton );	
	Add( &m_cInput );

	m_cUpButton.AddMouseEventHandler( this );
	m_cDownButton.AddMouseEventHandler( this );
	Add( &m_cUpButton );
	Add( &m_cDownButton );
	
	// ---------------------------------------------------------------------

	m_uiOKButton.SetLabel( "확 인" );
	m_uiCancelButton.SetLabel( "취 소" );

	m_uiOKButton.SetLabelColor( D3DCOLOR_XRGB( 232, 232, 232 ) );
	m_uiCancelButton.SetLabelColor( D3DCOLOR_XRGB( 232, 232, 232 ) );


	// 숫자만 입력할수 있게한다.
	m_cInput.SetInputType( CGUIInput::Only_Number );
	m_cInput.SetAlignment( CGUIInput::Right_Alignment );
	m_cInput.SetMargin( 3, 0 );	

	m_cUpButton.DisableFocus();
	m_cDownButton.DisableFocus();
	m_uiOKButton.DisableFocus();
	m_uiCancelButton.DisableFocus();
	m_uiCloseButton.DisableFocus();

	SetMoveObject( this );	

	SetFocusObject( &m_cInput );	

	m_dwPressedTime = 0;
	
	SetARGB( 0xe0ffffff );

	m_bIsFirstInput = TRUE;
}

void
CGUINumInputDlg::SetSprite( void )
{
	SPRITE_INFO l_spriteInfo;
	l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );

	l_spriteInfo.wActionNo = 3;
	l_spriteInfo.wFrameNo = 0;
	SetSpriteInfo( &l_spriteInfo );

	l_spriteInfo.wFrameNo = 1;
	m_cInputStr.SetSpriteInfo( &l_spriteInfo );

	l_spriteInfo.wFrameNo = 2;
	m_uiOKButton.SetSpriteInfoEx( &l_spriteInfo );
	l_spriteInfo.wFrameNo = 3;
	m_uiOKButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );

	l_spriteInfo.wFrameNo = 2;
	m_uiCancelButton.SetSpriteInfoEx( &l_spriteInfo );
	l_spriteInfo.wFrameNo = 3;
	m_uiCancelButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );


	// up, down button
	l_spriteInfo.wActionNo = 4;
	l_spriteInfo.wFrameNo = 2;
	m_cUpButton.SetSpriteInfoEx( &l_spriteInfo );
	l_spriteInfo.wFrameNo = 3;
	m_cUpButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );

	l_spriteInfo.wFrameNo = 4;
	m_cDownButton.SetSpriteInfoEx( &l_spriteInfo );
	l_spriteInfo.wFrameNo = 5;
	m_cDownButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
}

void
CGUINumInputDlg::SetNumber( DWORD pi_dwNumber )
{
	char buf[32];

	if( pi_dwNumber > m_dwMaxNumber )
		itoa( m_dwMaxNumber, buf, 10 );
	else	
		itoa( pi_dwNumber, buf, 10 );

	m_cInput.SetText( buf );
}

void
CGUINumInputDlg::SetMaxNumber( DWORD pi_dwMaxNumber )
{
	m_dwMaxNumber = pi_dwMaxNumber;	
	
	if( atoi( m_cInput.GetText() ) > m_dwMaxNumber )
	{
		char buf[32];
		sprintf( buf, "%d", m_dwMaxNumber );
		m_cInput.SetText( buf );
	}
}


void
CGUINumInputDlg::InitInputState( void )
{
	m_bIsFirstInput	= TRUE;
	m_dwInitNumber	= atoi( m_cInput.GetText() );
}

void
CGUINumInputDlg::Clear( void )
{
	m_cInput.Clear();
}


void
CGUINumInputDlg::Active( void )
{
	SetFocusObject( &m_cInput );

	CGUIDialogBox::Active();
}


void
CGUINumInputDlg::MouseEvent( _mouse_event & event )
{
	CGUIDialogBox::MouseEvent( event );	

	if( event.id == MOUSE_CLICKED )
	{
		if( event.source == &m_cUpButton )
		{			
			int l_nNum = atoi( m_cInput.GetText() ) + 1;
			if( l_nNum > m_dwMaxNumber )
				l_nNum = m_dwMaxNumber;

			char l_pBuf[32];
			sprintf( l_pBuf, "%d", l_nNum );			
			m_cInput.SetText( l_pBuf );
		}
		else if( event.source == &m_cDownButton )
		{			
			int l_nNum = atoi( m_cInput.GetText() ) - 1;

			if( l_nNum <= 0 )
			{
				m_cInput.Clear();
			}
			else
			{
				char l_pBuf[32];
				sprintf( l_pBuf, "%d", l_nNum );
				m_cInput.SetText( l_pBuf );
			}
		}		
	}
	else if( event.id == MOUSE_STROKED )
	{
		m_dwPressedTime = timeGetTime();		
	}
	else if( event.id == MOUSE_PRESSED )
	{
		if( event.source == &m_cUpButton )
		{
			if( timeGetTime() - m_dwPressedTime > 100 )
			{
				char l_pBuf[3];
				int l_nNum = atoi( m_cInput.GetText() ) + 1;
				if( l_nNum > m_dwMaxNumber )
					l_nNum = m_dwMaxNumber;

				itoa( l_nNum, l_pBuf, 10 );			
				m_cInput.SetText( l_pBuf );

				m_dwPressedTime = timeGetTime();
			}
		}
		else if( event.source == &m_cDownButton )
		{
			if( timeGetTime() - m_dwPressedTime > 100 )
			{
				char l_pBuf[3];
				int l_nNum = atoi( m_cInput.GetText() ) - 1;

				if( l_nNum <= 0 )
				{
					m_cInput.Clear();
				}
				else
				{
					itoa( l_nNum, l_pBuf, 10 );
					m_cInput.SetText( l_pBuf );
				}

				m_dwPressedTime = timeGetTime();
			}
		}
	}
}


LRESULT
CGUINumInputDlg::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if( !CGUIDialogBox::MsgProc( hWnd, uMsg, wParam, lParam ) )
		return 0;


	if( m_listDialogBox.empty() )
	{
		switch( uMsg )
		{	
		case WM_KEYDOWN:
		case WM_CHAR:
			{	
				if( m_bIsFirstInput )
				{					
					// 처음 입력하는거면 맨뒤에 입력된 숫자빼고 전에 숫자는 지운다
					int l_nInputNumber = atoi( m_cInput.GetText() );

					// 백스페이스에 의해 숫자가 지워진 경우
					// 이때는 있는 숫자를 다 지워준다.
					if( m_dwInitNumber > l_nInputNumber )
					{
						m_cInput.Clear();

						m_bIsFirstInput = FALSE;
					}
					// 숫자가 입력되었을 경우
					// 마지막에 입력된 숫자만 빼고 다 지운다.
					else if( m_dwInitNumber < l_nInputNumber )
					{
						char buf[16];
						char * l_pInputStr = m_cInput.GetText();
						buf[0] = l_pInputStr[strlen( l_pInputStr ) - 1];
						buf[1] = '\0';

						m_cInput.SetText( buf );

						m_bIsFirstInput = FALSE;
					}				
				}

				// 0으로 시작하지 않게
				
				char * l_pNumStr = m_cInput.GetText();
				if( l_pNumStr[0] == '0' )
				{
					for( int i=1; l_pNumStr[i] != '\0'; ++i )
					{
						if( l_pNumStr[i] != '0' )
							break;				
					}
					
					if( i > 0 && l_pNumStr[i] != '\0' )
					{
						char buf[32];
						strcpy( buf, l_pNumStr + i );
						m_cInput.SetText( buf );
					}
				}
				
				// max number를 넘으면 max number로 
				if( atoi( m_cInput.GetText() ) > m_dwMaxNumber )
				{
					char buf[32];
					sprintf( buf, "%d", m_dwMaxNumber );
					m_cInput.SetText( buf );
				}
			}
			break;
		}
	}
	
	return 1;
}