////////////////////////////////////////////////////////////////////////////
//
// CGUIChatWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////

#include "GUIChatWindow.h"
#include "../SpriteMgr.h"
#include "../GUIController/GUIScreen.h"

#include "../../DefineMessage.h"

////////////////////////////////////////////////////////////////////////////

BOOL	g_bChangedLangMode = false;

#define	CHATMSG_LIMIT_LINE_NUM		500


CGUIChatWindow::CGUIChatWindow()
{
	m_eCurChatMode = NONE;
	m_eWhisperPrevChatMode = NONE;


	m_pIDFont[FS_NORMAL].bySize				= 12;
	m_pIDFont[FS_NORMAL].dwColor			= 0xffffffff;
	m_pIDFont[FS_NORMAL].dwStyle			= R3_HAN_OUTLINE;
	m_pTextFont[FS_NORMAL].bySize			= 12;
	m_pTextFont[FS_NORMAL].dwColor			= 0xffffffff;
	m_pTextFont[FS_NORMAL].dwStyle			= R3_HAN_OUTLINE;

	m_pIDFont[FS_AVATAR].bySize				= 12;
	m_pIDFont[FS_AVATAR].dwColor			= 0xFFB0FFFF;
	m_pIDFont[FS_AVATAR].dwStyle			= R3_HAN_OUTLINE;
	m_pTextFont[FS_AVATAR].bySize			= 12;
	m_pTextFont[FS_AVATAR].dwColor			= 0xFFB0FFFF;
	m_pTextFont[FS_AVATAR].dwStyle			= R3_HAN_OUTLINE;

	m_pIDFont[FS_SYSTEM].bySize				= 12;
	m_pIDFont[FS_SYSTEM].dwColor			= 0xFFFFA670; //0xFFFF80A0;
	m_pIDFont[FS_SYSTEM].dwStyle			= R3_HAN_OUTLINE;
	m_pTextFont[FS_SYSTEM].bySize			= 12;
	m_pTextFont[FS_SYSTEM].dwColor			= 0xFFFFA670; //0xFFFF80A0;
	m_pTextFont[FS_SYSTEM].dwStyle			= R3_HAN_OUTLINE;

	m_pIDFont[FS_PARTY_NORMAL].bySize		= 12;
	m_pIDFont[FS_PARTY_NORMAL].dwColor		= 0xFFC0E0E0;
	m_pIDFont[FS_PARTY_NORMAL].dwStyle		= R3_HAN_OUTLINE;
	m_pTextFont[FS_PARTY_NORMAL].bySize		= 12;
	m_pTextFont[FS_PARTY_NORMAL].dwColor	= 0xFFC0E0E0;
	m_pTextFont[FS_PARTY_NORMAL].dwStyle	= R3_HAN_OUTLINE;

	m_pIDFont[FS_PARTY_AVATAR].bySize		= 12;
	m_pIDFont[FS_PARTY_AVATAR].dwColor		= 0xFFC0E0E0;
	m_pIDFont[FS_PARTY_AVATAR].dwStyle		= R3_HAN_OUTLINE;
	m_pTextFont[FS_PARTY_AVATAR].bySize		= 12;
	m_pTextFont[FS_PARTY_AVATAR].dwColor	= 0xFFC0E0E0;
	m_pTextFont[FS_PARTY_AVATAR].dwStyle	= R3_HAN_OUTLINE;

	m_pIDFont[FS_WHISPER_NORMAL].bySize		= 12;
	m_pIDFont[FS_WHISPER_NORMAL].dwColor	= 0xFF10DD10;
	m_pIDFont[FS_WHISPER_NORMAL].dwStyle	= R3_HAN_OUTLINE;
	m_pTextFont[FS_WHISPER_NORMAL].bySize	= 12;
	m_pTextFont[FS_WHISPER_NORMAL].dwColor	= 0xFF10DD10;
	m_pTextFont[FS_WHISPER_NORMAL].dwStyle	= R3_HAN_OUTLINE;

	m_pIDFont[FS_WHISPER_AVATAR].bySize		= 12;
	m_pIDFont[FS_WHISPER_AVATAR].dwColor	= 0xFFA0EEA0;
	m_pIDFont[FS_WHISPER_AVATAR].dwStyle	= R3_HAN_OUTLINE;
	m_pTextFont[FS_WHISPER_AVATAR].bySize	= 12;
	m_pTextFont[FS_WHISPER_AVATAR].dwColor	= 0xFFA0EEA0;
	m_pTextFont[FS_WHISPER_AVATAR].dwStyle	= R3_HAN_OUTLINE;

	m_pIDFont[FS_RACE].bySize				= 12;
	m_pIDFont[FS_RACE].dwColor				= 0xFFA0D0FF;
	m_pIDFont[FS_RACE].dwStyle				= R3_HAN_OUTLINE;
	m_pTextFont[FS_RACE].bySize				= 12;
	m_pTextFont[FS_RACE].dwColor			= 0xFFA0D0FF;
	m_pTextFont[FS_RACE].dwStyle			= R3_HAN_OUTLINE;
	
	m_pIDFont[FS_NOTIFY].bySize				= 12;
	m_pIDFont[FS_NOTIFY].dwColor			= 0xFFFFD0A0;
	m_pIDFont[FS_NOTIFY].dwStyle			= R3_HAN_OUTLINE;
	m_pTextFont[FS_NOTIFY].bySize			= 12;
	m_pTextFont[FS_NOTIFY].dwColor			= 0xFFFFD0A0;
	m_pTextFont[FS_NOTIFY].dwStyle			= R3_HAN_OUTLINE;

	m_pIDFont[FS_TIP].bySize				= 12;
	m_pIDFont[FS_TIP].dwColor				= 0xFFA0D0FF;
	m_pIDFont[FS_TIP].dwStyle				= R3_HAN_OUTLINE;
	m_pTextFont[FS_TIP].bySize				= 12;
	m_pTextFont[FS_TIP].dwColor				= 0xFFA0D0FF;
	m_pTextFont[FS_TIP].dwStyle				= R3_HAN_OUTLINE;

	m_pChatInputWin = NULL;

	m_eLangMode	= NoneMode;

	m_bCreated	= FALSE;
}

CGUIChatWindow::~CGUIChatWindow()
{
	
}

void	
CGUIChatWindow::SetChatInputWindow( CGUIChatInputWindow * pi_pWin )
{
	m_pChatInputWin = pi_pWin;
	
	m_pChatInputWin->SetChatWindow( this );
	
	
	m_pChatInputWin->Init( &m_uiInputField );	

	// 6줄까지만 보여준다.
	m_pChatInputWin->SetMaxLineNum( 6 ); 
	m_pChatInputWin->SetMaxCharNumInLine( ( m_uiChatBoard[0].m_ptSize.x -
										  m_uiChatBoard[0].GetMarginX() * 2 ) / ONEBYTE_FONTSIZE );

	m_pChatInputWin->SetColumnGap( m_uiChatBoard[0].GetColumnGap() );		
}

void
CGUIChatWindow::Open( void )
{
	if( m_pChatInputWin )	
	{
		m_pChatInputWin->Close();		
	}

	m_uiInputField.SetPos( m_uiInputBoard.m_ptPos.x + 27, m_uiInputBoard.m_ptPos.y + 25 );
	m_uiInputField.AddMouseEventHandler( this );
	m_uiInputField.AddKeyEventHandler( this );

	m_uiInputField.SetCaretColor( D3DCOLOR_XRGB( 95, 78, 252 ) );

	// 한영 이미지를 다시 세팅한다.
	if( gInputMgr.IsKoreanMode() )
		SetLangMode( Korean );
	else 
		SetLangMode( English );

	CGUIWindow::Open();
}

void
CGUIChatWindow::Close( void )
{
	CGUIWindow::Close();

	if( m_pChatInputWin )
	{	
		m_uiInputField.SetPos( m_pChatInputWin->m_ptPos.x + 6,
							   m_pChatInputWin->m_ptPos.y + 5 );

		m_uiInputField.AddMouseEventHandler( m_pChatInputWin );
		m_uiInputField.AddKeyEventHandler( m_pChatInputWin );

		m_uiInputField.SetCaretColor( D3DCOLOR_XRGB( 95, 78, 252 ) );

		m_pChatInputWin->Open();
	}	
}

void
CGUIChatWindow::Init( void )
{		
	int i;

	// -----------------------------------------------------------
	// Size setting	
	SetSize( 346, 170 );

	// title
	m_uiTitleBoard.SetSize( 346, 15 );

	m_uiResizeButton.SetSize( 5, 10 );
	m_uiResizeObj.SetSize( m_uiTitleBoard.m_ptSize );
	m_uiCloseButton.SetSize( 16, 13 );
	m_uiHelpButton.SetSize( 16, 13 );

	
	// output
	m_uiOutputBoard.SetSize( 346, 115 );
	for( i=0; i<TOTAL_CHAT_MODE; ++i )
		m_uiChatBoard[i].SetSize( 328, 115 );

	// scrollbar
	POINT l_ptSize;
	for( i=0; i<TOTAL_CHAT_MODE; ++i )
	{
		l_ptSize.x = 15;	l_ptSize.y = 10;
		m_uiScrollbar[i].SetTopArrowSize( l_ptSize );
		m_uiScrollbar[i].SetBottomArrowSize( l_ptSize );	
		l_ptSize.y = 18;
		m_uiScrollbar[i].SetBarSize( l_ptSize );	
		l_ptSize.y = m_uiChatBoard[0].m_ptSize.y;
		m_uiScrollbar[i].SetSize( l_ptSize );
	}
	
	// input
	m_uiInputBoard.SetSize( 346, 40 );	
	m_uiInputField.SetSize( 310, 12 );
	m_uiWhisperIDInput.SetSize( 100, 12 );
	m_uiWhisperButton.SetSize( 19, 13 );	
	m_uiLangButton.SetSize( 19, 13 );

	m_uiChatTabBase.SetSize( 211, 19 );
	for( i=0; i<TOTAL_CHAT_MODE; ++i )
		m_uiChatTab[i].SetSize( 31, 14 );	

	m_uiChatTab[ALL].SetLabel( "전체" );
	m_uiChatTab[PARTY].SetLabel( "파티" );
	m_uiChatTab[GUILD].SetLabel( "길드" );
	m_uiChatTab[NATION].SetLabel( "국가" );

	// -----------------------------------------------------------
	// Position

	// title
	m_uiResizeButton.SetPos( m_uiTitleBoard.m_ptPos.x + (m_uiTitleBoard.m_ptSize.x - m_uiResizeButton.m_ptSize.x)/2,
							 m_uiTitleBoard.m_ptPos.y + (m_uiTitleBoard.m_ptSize.y - m_uiResizeButton.m_ptSize.y)/2 );
	m_uiResizeObj.SetPos( m_uiTitleBoard.m_ptPos );
	m_uiCloseButton.SetPos( m_uiTitleBoard.m_ptPos.x + 328 , m_uiTitleBoard.m_ptPos.y + 1 );
	m_uiHelpButton.SetPos( m_uiTitleBoard.m_ptPos.x + 311 , m_uiTitleBoard.m_ptPos.y + 1 );
	
	// output
	m_uiOutputBoard.SetPos( m_ptPos.x, m_uiTitleBoard.m_ptSize.y );	
	
	// input
	m_uiInputBoard.SetPos( m_ptPos.x, m_uiOutputBoard.m_ptPos.y + m_uiOutputBoard.m_ptSize.y );	
	m_uiInputField.SetPos( m_uiInputBoard.m_ptPos.x + 27, m_uiInputBoard.m_ptPos.y + 25 );

	m_uiWhisperButton.SetPos( m_uiInputBoard.m_ptPos.x + 3, m_uiInputBoard.m_ptPos.y + 2 );
	m_uiWhisperIDInput.SetPos( m_uiInputBoard.m_ptPos.x + 27, m_uiInputBoard.m_ptPos.y + 3 );

	m_uiLangButton.SetPos( m_uiInputBoard.m_ptPos.x + 3, m_uiInputBoard.m_ptPos.y + 25 );

	
	m_uiChatTabBase.SetPos( m_uiInputBoard.m_ptPos.x + 135, m_uiInputBoard.m_ptPos.y );

	m_uiChatTab[ALL].SetPos( m_uiChatTabBase.m_ptPos.x + 13, m_uiChatTabBase.m_ptPos.y + 4 );
	m_uiChatTab[PARTY].SetPos( m_uiChatTabBase.m_ptPos.x + 64, m_uiChatTabBase.m_ptPos.y + 4 );
	m_uiChatTab[GUILD].SetPos( m_uiChatTabBase.m_ptPos.x + 116, m_uiChatTabBase.m_ptPos.y + 4 );
	m_uiChatTab[NATION].SetPos( m_uiChatTabBase.m_ptPos.x + 168, m_uiChatTabBase.m_ptPos.y + 4 );

	
	// -----------------------------------------------------------
	// Add controller

	// title	
	m_uiResizeObj.AddMouseEventHandler( this );	
	m_uiCloseButton.AddMouseEventHandler( this );
	m_uiHelpButton.AddMouseEventHandler( this );
	m_uiTitleBoard.Add( &m_uiResizeButton );	
	m_uiTitleBoard.Add( &m_uiResizeObj );
	m_uiTitleBoard.Add( &m_uiCloseButton );
	m_uiTitleBoard.Add( &m_uiHelpButton );

	Add( &m_uiTitleBoard );

	// output
	for( i=0; i<TOTAL_CHAT_MODE; ++i )	
		m_uiChatBoard[i].AddScrollbar( &m_uiScrollbar[i] );	

	Add( &m_uiOutputBoard );	

	// input	
	m_uiInputField.AddMouseEventHandler( this );
	m_uiInputField.AddKeyEventHandler( this );	
	m_uiWhisperButton.AddMouseEventHandler( this );
	m_uiWhisperIDInput.AddMouseEventHandler( this );
	m_uiWhisperIDInput.AddKeyEventHandler( this );
	m_uiLangButton.AddMouseEventHandler( this );

	m_uiInputBoard.Add( &m_uiWhisperButton );

	m_uiInputBoard.Add( &m_uiChatTabBase );
	
	for( i=0; i<TOTAL_CHAT_MODE; ++i )
	{
		m_uiChatTab[i].AddMouseEventHandler( this );
		m_uiInputBoard.Add( &m_uiChatTab[i] );
	}
	m_uiInputBoard.Add( &m_uiWhisperIDInput );
	m_uiInputBoard.Add( &m_uiLangButton );
	m_uiInputBoard.Add( &m_uiInputField );

	Add( &m_uiInputBoard );		


	// -----------------------------------------------------------
	// Set Resize Object;
	SetReSizeObject( CGUIWindow::TOP, &m_uiResizeObj );	
	SetReSizable( true );
	
	SetMoveObject( &m_uiInputBoard );
	for( i=0; i<TOTAL_CHAT_MODE; ++i )
		SetMoveObject( &m_uiChatBoard[i] );

	SetMovable( true );

	// sprite setting
	SetSprite();

	m_uiOutputBoard.SetARGB( 0xCC12131d );
	SetBaseARGB( 0xCCffffff );
	
	for( i=0; i<TOTAL_CHAT_MODE; ++i )
	{
		m_uiChatTab[i].SetLabelColor( D3DCOLOR_XRGB( 135, 135, 135 ) ); 
		m_uiChatTab[i].SetLabelHoveredColor( D3DCOLOR_XRGB( 255, 255, 255) );		
	}	


	// ----------------------------------------------------------------------

	// inputfield만 focus되게 한다.
	DisableFocus();	
	for( i=0; i<TOTAL_CHAT_MODE; ++i )
	{
		m_uiChatBoard[i].DisableFocus();
		m_uiScrollbar[i].DisableFocus();
	}

	m_uiInputField.EnableFocus();
	m_uiWhisperIDInput.EnableFocus();

	SetFocusObject( &m_uiInputField );
	
	// text의 최대 라인 수와 한줄에 최대 글자수를 지정해준다.	
	m_uiInputField.SetInputLimit( MAX_CHAT_INPUT_LEN );	
	m_uiInputField.SetMargin( 0, 0 );	
	m_uiWhisperIDInput.SetInputLimit( 16 );	
	m_uiWhisperIDInput.SetMargin( 0, 0 );

	for( i=0; i<TOTAL_CHAT_MODE; ++i )
	{
		m_uiChatBoard[i].SetColumnGap( 3 );	
		m_uiChatBoard[i].SetMargin( 6, 5 );
		m_uiChatBoard[i].SetAutoScroll( TRUE );
		m_uiChatBoard[i].SetLimitLineNum( CHATMSG_LIMIT_LINE_NUM );
	}

	SetChatMode( ALL );
	

	// set language mode
	// default로 영문으로 한다.
	gInputMgr.SetEnglish();
	SetLangMode( English );
	

	SetName( "ChatWindow" );

	m_bCreated = TRUE;
}

void
CGUIChatWindow::SetBaseARGB( DWORD pi_dwARGB )
{
//	m_uiTitleBoard.SetARGB( pi_dwARGB );
	m_uiTitleBoard.SetARGB( 0xE0FFFFFF );

	m_uiOutputBoard.SetARGB( (m_uiOutputBoard.GetARGB() & 0x00ffffff) | ( pi_dwARGB & 0xff000000 ) );
//	m_uiInputBoard.SetARGB( pi_dwARGB );
	m_uiInputBoard.SetARGB( 0xE0FFFFFF );
}

void
CGUIChatWindow::SetSprite( void )
{
	if( !m_bCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_CHAT );	
		
		l_spriteInfo.wActionNo = 0;

		l_spriteInfo.wFrameNo = 0;
		m_uiTitleBoard.SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 1;
		m_uiInputBoard.SetSpriteInfo( &l_spriteInfo );
		
		l_spriteInfo.wFrameNo = 2;
		m_uiChatTabBase.SetSpriteInfo( &l_spriteInfo );	
		

		// input langauge toggle
		l_spriteInfo.wActionNo = 1;
		l_spriteInfo.wFrameNo = 0;
		m_uiLangButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 1;
		m_uiLangButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		

		// whisper button
		l_spriteInfo.wFrameNo = 4;
		m_uiWhisperButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 5;
		m_uiWhisperButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		
		// resize button
		l_spriteInfo.wFrameNo = 6;
		m_uiResizeButton.SetSpriteInfo( &l_spriteInfo );		

		
		for( int i=0; i<TOTAL_CHAT_MODE; ++i )
		{
			l_spriteInfo.pSprite = NULL;
			m_uiChatTab[i].SetSpriteInfoEx( &l_spriteInfo );

			l_spriteInfo.wFrameNo = 7;
			l_spriteInfo.pSprite = m_uiTitleBoard.GetSpriteInfo()->pSprite;
			m_uiChatTab[i].SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		}	

		// close button
		m_uiCloseButton.SetSpriteInfoEx( gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON1 ) );
		m_uiCloseButton.SetSpriteInfoEx( HOVERED, gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON2 ) );
		m_uiCloseButton.SetSpriteInfoEx( PRESSED, gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON3 ) );	

		// help button
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );
		l_spriteInfo.wActionNo = 5;
		l_spriteInfo.wFrameNo = 0;
		m_uiHelpButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 1;
		m_uiHelpButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 2;
		m_uiHelpButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );

		
		// scrollbar	
		SPRITE_INFO l_sScrollbarSprite[2];
		l_sScrollbarSprite[0].pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );
		l_sScrollbarSprite[0].wActionNo = 4;
		memcpy( &l_sScrollbarSprite[1], &l_sScrollbarSprite[0], sizeof( SPRITE_INFO ) );
		
		for( i=0; i<TOTAL_CHAT_MODE; ++i )
		{
			l_sScrollbarSprite[0].wFrameNo = 0;
			m_uiScrollbar[i].SetSpriteInfo( &l_sScrollbarSprite[0] );
			l_sScrollbarSprite[0].wFrameNo = 1;
			m_uiScrollbar[i].SetBarSpriteInfo( &l_sScrollbarSprite[0] );

			l_sScrollbarSprite[0].wFrameNo = 2;
			l_sScrollbarSprite[1].wFrameNo = 3;
			m_uiScrollbar[i].SetTopArrowSpriteInfo( l_sScrollbarSprite );

			l_sScrollbarSprite[0].wFrameNo = 4;
			l_sScrollbarSprite[1].wFrameNo = 5;
			m_uiScrollbar[i].SetBottomArrowSpriteInfo( l_sScrollbarSprite );	
		}
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_CHAT );
		if( !l_pSprite )
			return;

		m_uiTitleBoard.GetSpriteInfo()->pSprite = l_pSprite;
		
		m_uiInputBoard.GetSpriteInfo()->pSprite = l_pSprite;
		
		m_uiChatTabBase.GetSpriteInfo()->pSprite = l_pSprite;
		
		
		// input langauge toggle
		m_uiLangButton.SetSprite( l_pSprite );

		// whisper button		
		m_uiWhisperButton.SetSprite( l_pSprite );	
		
		// resize button		
		m_uiResizeButton.GetSpriteInfo()->pSprite = l_pSprite;	

		// chat button		
		for( int i=0; i<TOTAL_CHAT_MODE; ++i )		
			m_uiChatTab[i].SetSprite( l_pSprite );	
		
		
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );		

		// close button			
		m_uiCloseButton.SetSprite( l_pSprite );	
		
		// help button			
		m_uiHelpButton.SetSprite( l_pSprite );	
		
		// scrollbar
		for( i=0; i<TOTAL_CHAT_MODE; ++i )		
			m_uiScrollbar[i].SetSprite( l_pSprite );		
	}

}

void
CGUIChatWindow::SetBound( RECT & pi_rtBound )
{
	WORD l_wResizeUnit = m_uiScrollbar[0].GetAdjustmentUnit();	
	
	if( abs(pi_rtBound.top - m_ptPos.y) < l_wResizeUnit )
		return;

	RECT	l_rtBound;
	memcpy( &l_rtBound, &pi_rtBound, sizeof( RECT ) );

	// scrollbar 최소 크기가 resizeunit 4개랑 같다.
	if( abs( l_rtBound.bottom - l_rtBound.top ) < l_wResizeUnit * 4 + 62 )
		l_rtBound.top = l_rtBound.bottom - ( l_wResizeUnit * 4 + 62 );	
	
	l_rtBound.top -= ( l_rtBound.top - m_ptPos.y )%l_wResizeUnit;
	
	
	//CGUIObject::SetBound( l_rtBound );
	m_ptPos.x = l_rtBound.left; // 포함된 object의 위치는 그대로 하기 위해 setpos를 안썼다.
	m_ptPos.y = l_rtBound.top;
	SetSize( l_rtBound.right - l_rtBound.left, l_rtBound.bottom - l_rtBound.top );

	// resize안할것들은 위치만 다시 잡아준다.
	m_uiTitleBoard.SetPos( l_rtBound.left, l_rtBound.top );

	// m_uiOutputBoard만 resize한다.
	m_uiOutputBoard.SetSize( m_uiTitleBoard.m_ptSize.x,
							 m_uiInputBoard.m_ptPos.y - m_uiTitleBoard.m_ptPos.y - m_uiTitleBoard.m_ptSize.y );
	
	m_uiOutputBoard.SetPos( m_uiTitleBoard.m_ptPos.x,
							m_uiTitleBoard.m_ptPos.y + m_uiTitleBoard.m_ptSize.y );	

	RECT	l_rtBoardBound;
	l_rtBoardBound.left = m_uiOutputBoard.m_ptPos.x;
	l_rtBoardBound.right = l_rtBoardBound.left + m_uiChatBoard[0].m_ptSize.x;
	l_rtBoardBound.top = m_uiOutputBoard.m_ptPos.y;
	l_rtBoardBound.bottom = m_uiOutputBoard.m_ptPos.y + m_uiOutputBoard.m_ptSize.y;
	for( int i=0; i<TOTAL_CHAT_MODE; ++i )
		m_uiChatBoard[i].SetBound( l_rtBoardBound );	

	l_rtBoardBound.left = m_uiOutputBoard.m_ptPos.x + m_uiOutputBoard.m_ptSize.x - m_uiScrollbar[0].m_ptSize.x - 2;
	l_rtBoardBound.right = l_rtBoardBound.left + m_uiScrollbar[0].m_ptSize.x;
	l_rtBoardBound.top = m_uiChatBoard[0].m_ptPos.y;
	l_rtBoardBound.bottom = m_uiChatBoard[0].m_ptPos.y + m_uiChatBoard[0].m_ptSize.y;
	for( i=0; i<TOTAL_CHAT_MODE; ++i )
		m_uiScrollbar[i].SetBound( l_rtBoardBound );

}

void
CGUIChatWindow::Clear( ChatMode pi_eOutputMode/*= NONE*/ )
{
	if( pi_eOutputMode == NONE )
	{
		for( int i=0; i<TOTAL_CHAT_MODE; ++i )
		{
			m_uiChatBoard[i].Clear();
		}
	}
	else
	{
		m_uiChatBoard[pi_eOutputMode].Clear();
	}
}

void
CGUIChatWindow::InsertText( char * pi_pID, char * pi_pChat, FontStyle pi_eFontStyle, ChatMode pi_eOutputMode/*= ALL*/ )
{
	if( pi_eOutputMode == NONE )
		return;

	static const int MAX_NAME_LENGTH = 32;
	static const int MAX_MESSAGE_LENGTH = 256;

	static char l_pName[MAX_NAME_LENGTH];
	static char l_pMessage[MAX_MESSAGE_LENGTH];

	ZeroMemory( l_pName, MAX_NAME_LENGTH );
	ZeroMemory( l_pMessage, MAX_MESSAGE_LENGTH );

	if( pi_pID )
		sprintf( l_pName, "%s : ", pi_pID );
	if( pi_pChat )
		sprintf( l_pMessage, "%s%s\n", l_pName, pi_pChat );

	if( pi_eOutputMode == ALL || pi_eOutputMode == WHISPER )
	{
		for( int i = 0; i < TOTAL_CHAT_MODE; ++i )
			m_uiChatBoard[i].InsertText( l_pMessage, &m_pTextFont[pi_eFontStyle] );
	}		
	else
	{		
		m_uiChatBoard[pi_eOutputMode].InsertText( l_pMessage, &m_pTextFont[pi_eFontStyle] );
	}

	m_pChatInputWin->InsertChatMsg( l_pMessage, &m_pTextFont[pi_eFontStyle] );
}


void
CGUIChatWindow::SetWhisperCharName( char * pi_pName )
{
	m_uiWhisperIDInput.SetText( pi_pName );
	
	if( pi_pName )
	{
		SetChatMode( WHISPER );		

		// 채팅 입력창으로 포커스를 바꾼다.
		m_uiInputField.SetFocus();
		m_uiInputField.SetCaretPos( strlen(m_uiInputField.GetText()) );
		SetFocusObject( &m_uiInputField );
	}
	else
	{
		m_uiInputField.Clear();
	}
}

char *
CGUIChatWindow::GetWhisperCharName( void )
{
	return m_uiWhisperIDInput.GetText();
}

void
CGUIChatWindow::SetLangMode( LangMode pi_eMode )
{
	if( m_eLangMode == pi_eMode )
		return;

	m_eLangMode = pi_eMode;

	SPRITE_INFO	l_spriteInfo;

	memcpy( &l_spriteInfo, m_uiLangButton.GetSpriteInfo(), sizeof(SPRITE_INFO) );

	if( m_eLangMode == Korean )
	{
		l_spriteInfo.wFrameNo = 0;
		m_uiLangButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 1;
		m_uiLangButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
	}	
	else
	{
		l_spriteInfo.wFrameNo = 2;
		m_uiLangButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 3;
		m_uiLangButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
	}
}

void
CGUIChatWindow::KeyEvent( _key_event & event )
{	
	// send chat msg
	if( event.key == VK_RETURN )
	{
		if( event.source == &m_uiInputField )
		{		
			// ################## 코드 추가 시킬 곳 ###########################	
			// 유저가 입력한 채팅 메시지를 보낸다.
			if( m_pfEventProc_Ex )
				m_pfEventProc_Ex( ( void * )event.source );
			// ################################################################

			InitInputModeStr();
		}
		else if( event.source == &m_uiWhisperIDInput )
		{
			char * l_pWhisperCharName = m_uiWhisperIDInput.GetText();			
			
			if( l_pWhisperCharName[0] != '\0' )
			{
				// 귓속말 모드로 바꾼다.
				SetChatMode( WHISPER );	
				InitInputModeStr(); // 이미 귓속말모드일수 있기 때문에 다시 설정해준다.
			}
			
			// focus를 inputfield로 옮긴다.
			m_uiInputField.SetFocus();
			SetFocusObject( &m_uiInputField );
		}
	}	
	else if( event.key == VK_TAB )
	{
		if( event.source == &m_uiInputField )
		{
			m_uiWhisperIDInput.SetFocus();			
			SetFocusObject( &m_uiWhisperIDInput );
		}
		else if( event.source == &m_uiWhisperIDInput )
		{
			char * l_pWhisperCharName = m_uiWhisperIDInput.GetText();		
			
			if( l_pWhisperCharName[0] != '\0' )
			{
				// 귓속말 모드로 바꾼다.
				SetChatMode( WHISPER );	
				InitInputModeStr();
			}

			// focus를 inputfield로 옮긴다.
			m_uiInputField.SetFocus();
			SetFocusObject( &m_uiInputField );
		}
	}

}

/*
void
CGUIChatWindow::SetChatOutputMode( ChatMode pi_eOutputMode )
{
	if( pi_eOutputMode == m_eChatOutputMode )
		return;

	ChatMode l_ePrevOutputMode;
	l_ePrevOutputMode = m_eChatOutputMode;

	// set chat mode
	m_eChatOutputMode = pi_eOutputMode;

	// output tab setting
	if( l_ePrevOutputMode != NONE )
	{
		// inactive tab
		// button frame이 default, pressed, active순으로 저장되어있다.		
		SPRITE_INFO l_spriteInfo;
		memcpy( &l_spriteInfo, m_uiChatOutputTab[l_ePrevOutputMode].GetSpriteInfo(), sizeof(SPRITE_INFO) );
		
		l_spriteInfo.wFrameNo = l_ePrevOutputMode * 3;
		m_uiChatOutputTab[l_ePrevOutputMode].SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = l_ePrevOutputMode * 3 + 1;
		m_uiChatOutputTab[l_ePrevOutputMode].SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		
		// remove outputboard
		m_uiOutputBoard.Remove( &m_uiChatBoard[l_ePrevOutputMode] );		
		m_uiOutputBoard.Remove( &m_uiScrollbar[m_eChatOutputMode] );
	}

	// active tab
	SPRITE_INFO l_spriteInfo;
	memcpy( &l_spriteInfo, m_uiChatOutputTab[m_eChatOutputMode].GetSpriteInfo(), sizeof(SPRITE_INFO) );
	l_spriteInfo.wFrameNo = m_eChatOutputMode * 3 + 2;
	m_uiChatOutputTab[m_eChatOutputMode].SetSpriteInfoEx( &l_spriteInfo );
	
	// add outputboard
	m_uiChatBoard[m_eChatOutputMode].Show( IsShow() );
	m_uiChatBoard[m_eChatOutputMode].SetPos( m_uiOutputBoard.m_ptPos.x, m_uiOutputBoard.m_ptPos.y );
	m_uiScrollbar[m_eChatOutputMode].Show( IsShow() );
	m_uiScrollbar[m_eChatOutputMode].SetPos( m_uiOutputBoard.m_ptPos.x + m_uiOutputBoard.m_ptSize.x - 
																m_uiScrollbar[m_eChatOutputMode].m_ptSize.x,
											 m_uiOutputBoard.m_ptPos.y );

	m_uiOutputBoard.Add( &m_uiChatBoard[m_eChatOutputMode] );
	m_uiOutputBoard.Add( &m_uiScrollbar[m_eChatOutputMode] );	
}

void
CGUIChatWindow::SetChatInputMode( ChatMode pi_eInputMode )
{	
	if( pi_eInputMode == NONE )		
		return;

	ChatMode l_ePrevInputMode;
	l_ePrevInputMode = m_eChatInputMode;

	// set input mode
	m_eChatInputMode = pi_eInputMode;
	
	if( l_ePrevInputMode == WHISPER )
	{
		if( m_eChatInputMode != WHISPER )
			m_uiWhisperIDInput.Clear();
	}
	else if( l_ePrevInputMode != NONE )
	{
		// inactive button
		m_uiInputTab[l_ePrevInputMode].GetSpriteInfo()->pSprite = NULL;
	}


	if( m_eChatInputMode != WHISPER )
		m_uiInputTab[m_eChatInputMode].GetSpriteInfo()->pSprite = m_uiInputBoard.GetSpriteInfo()->pSprite;	

	InitInputModeStr( m_eChatInputMode );
}
*/

void
CGUIChatWindow::SetChatMode( ChatMode pi_eChatMode )
{	
	if( pi_eChatMode == m_eCurChatMode ||
		pi_eChatMode == NONE )
		return;

	// 귓속말모드이면 나중에 현재 tab을 비활성화 하기위해 
	// 귓속말이전 모드를 백업해둔다.
	if( pi_eChatMode == WHISPER )
	{
		m_eWhisperPrevChatMode = m_eCurChatMode;
	}


	int l_nPrevTabIndex;
	if( m_eCurChatMode == WHISPER )
		l_nPrevTabIndex = m_eWhisperPrevChatMode;
	else 
		l_nPrevTabIndex = m_eCurChatMode;

	if( pi_eChatMode != WHISPER &&
		l_nPrevTabIndex < TOTAL_CHAT_MODE )
	{		
		// set inactive tab
		m_uiChatTab[l_nPrevTabIndex].SetLabelColor( D3DCOLOR_XRGB( 135, 135, 135 ) );
		m_uiChatTab[l_nPrevTabIndex].SetLabelHoveredColor( D3DCOLOR_XRGB( 255, 255, 255) );


		// remove outputboard
		m_uiOutputBoard.Remove( &m_uiChatBoard[l_nPrevTabIndex] );		
		m_uiOutputBoard.Remove( &m_uiScrollbar[l_nPrevTabIndex] );
	}


	// set current chatmode
	m_eCurChatMode = pi_eChatMode;	

	InitInputModeStr();

	// set active tab
	if( m_eCurChatMode < TOTAL_CHAT_MODE )
	{
		m_uiChatTab[m_eCurChatMode].SetLabelColor( D3DCOLOR_XRGB( 213, 188, 160 ) );	

		// add outputboard
		m_uiChatBoard[m_eCurChatMode].Show( IsShow() );
		m_uiChatBoard[m_eCurChatMode].SetPos( m_uiOutputBoard.m_ptPos );
		m_uiScrollbar[m_eCurChatMode].Show( IsShow() );
		m_uiScrollbar[m_eCurChatMode].SetPos( m_uiOutputBoard.m_ptPos.x + m_uiOutputBoard.m_ptSize.x - 
																m_uiScrollbar[m_eCurChatMode].m_ptSize.x - 2,
												 m_uiOutputBoard.m_ptPos.y );

		m_uiOutputBoard.Add( &m_uiChatBoard[m_eCurChatMode] );
		m_uiOutputBoard.Add( &m_uiScrollbar[m_eCurChatMode] );

		SetWheelScrollObject( &m_uiScrollbar[m_eCurChatMode] );
	}	
	
}

void
CGUIChatWindow::InitInputModeStr( void )
{
	switch( m_eCurChatMode )
	{
	case ALL:
		m_uiInputField.Clear();
		break;

	case WHISPER:
		{
			char * l_pID = m_uiWhisperIDInput.GetText();
			if( l_pID[0] == '\0' )
				return;

			char buf[20];
			sprintf( buf, "/%s ", l_pID );
			m_uiInputField.SetText( buf );			
		}						
		break;

	case PARTY:
		m_uiInputField.SetText( "/파티 " );
		break;

	case GUILD:
		m_uiInputField.SetText( "/길드 " );
		break;
	
	case NATION:
		m_uiInputField.SetText( "/국가 " );
		break;		
	}

	m_uiInputField.SetCaretPos( strlen(m_uiInputField.GetText()) );
}

void
CGUIChatWindow::MouseEvent( _mouse_event &event )
{
	CGUIWindow::MouseEvent( event );

	if( event.id == MOUSE_CLICKED )
	{	
		if( event.source == &m_uiWhisperButton )
		{			
			// ################## 코드 추가 시킬 곳 ###########################
			// 친구 리스트창를 연다

		}		
		else if( event.source == &m_uiLangButton )
		{
			if( m_eLangMode == Korean )
			{
				SetLangMode( English );
//				gInputMgr.SetEnglish();				
			}
			else
			{
				SetLangMode( Korean );
//				gInputMgr.SetKorean();				
			}

			g_bChangedLangMode = true;
		}
		else if( event.source == &m_uiCloseButton )
		{
			Close();
		}
		else if( event.source == &m_uiHelpButton )
		{
			// 도움말 창 열기
			PostMessage( gScreen.GetHWND(), WM_OPEN_WINDOW_HELP, m_dwID, 0 );
		}
		
	}
	else if( event.id == MOUSE_STROKED )
	{
		
		for( int i=0; i<TOTAL_CHAT_MODE; ++i )
		{
			// change chatmode
			if( event.source == &m_uiChatTab[i] )
			{
				SetChatMode( (ChatMode) i );
				return;
			}			
		}		


		if( event.source == &m_uiWhisperIDInput )
		{
			m_uiWhisperIDInput.SetFocus();			
			SetFocusObject( &m_uiWhisperIDInput );
		}
		else if( event.source == &m_uiInputField )
		{	
			// 이전에 m_uiWhisperIDInput에 focus가 가있을 때 
			if( GetFocusObject() != &m_uiInputField )
			{
				char * l_pWhisperCharName = m_uiWhisperIDInput.GetText();			
			
				if( l_pWhisperCharName[0] != '\0' )
				{
					// 귓속말 모드로 바꾼다.
					SetChatMode( WHISPER );
					InitInputModeStr();
				}				

				// focus를 inputfield로 옮긴다.
				m_uiInputField.SetFocus();
			}

			SetFocusObject( &m_uiInputField );
		}		
	}
	
}

BOOL
CGUIChatWindow::Draw( void )
{
	if( !CGUIContainer::Draw() )
		return false;

	// top
	Draw2DRectangle( m_ptPos.x + 1, m_ptPos.y + 1,
					 m_ptPos.x + m_ptSize.x - 1, m_ptPos.y + 2,
					 0xcc232323 );

	// bottom
	Draw2DRectangle( m_ptPos.x + 1, m_ptPos.y + m_ptSize.y - 2,
					 m_ptPos.x + m_ptSize.x - 1, m_ptPos.y + m_ptSize.y - 1,
					 0xcc232323 );

	// left
	Draw2DRectangle( m_ptPos.x + 1, m_ptPos.y + 2,
					 m_ptPos.x + 2, m_ptPos.y + m_ptSize.y - 2,
					 0xcc232323 );

	// right
	Draw2DRectangle( m_ptPos.x + m_ptSize.x - 2, m_ptPos.y + 2,
					 m_ptPos.x + m_ptSize.x - 1, m_ptPos.y + m_ptSize.y - 2,
					 0xcc232323 );	

	/*
	Draw2DRectangle( m_uiOutputBoard.m_ptPos.x, m_uiOutputBoard.m_ptPos.y,
					 m_uiOutputBoard.m_ptPos.x + m_uiOutputBoard.m_ptSize.x, 
					 m_uiOutputBoard.m_ptPos.y + m_uiOutputBoard.m_ptSize.y,
					 0x60500000 );
	

	Draw2DRectangle( m_uiInputField.m_ptPos.x, m_uiInputField.m_ptPos.y,
				 m_uiInputField.m_ptPos.x + m_uiInputField.m_ptSize.x, 
				 m_uiInputField.m_ptPos.y + m_uiInputField.m_ptSize.y,
				 0x50000050 );
	*/

	DrawDialogBox();

	return true;
}


////////////////////////////////////////////////////////////////////////////
//
// CGUIChatInputWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////

CGUIChatInputWindow::CGUIChatInputWindow()
{	
	SetSize( 275, 22 );

	// -------------------------------------
	// 채팅 출력 부분
	m_wMaxLineNum = 0;
	m_wMaxCharNumInLine = 0;
	
	m_wTotalNewLineNum = 0;
	m_wLastCharNum = 0;
	m_wViewLineNum = 0;
	
	m_wColumnGap = 0;
	m_wFirstCharIndex = 0;

	SetName( "SimpleChat" );

	m_pInputField = NULL;
}

CGUIChatInputWindow::~CGUIChatInputWindow()
{
	
}

void
CGUIChatInputWindow::Init( CGUIInput * pi_pInput )
{
	m_pInputField = pi_pInput;

	m_uiOpenChatWin.SetSize( 20, 20 );
	SetSize( m_pInputField->m_ptSize.x + m_uiOpenChatWin.m_ptSize.x + 6, m_uiOpenChatWin.m_ptSize.y + 2 );	

	m_uiOpenChatWin.SetPos( m_ptPos.x + m_ptSize.x - m_uiOpenChatWin.m_ptSize.x - 1, m_ptPos.y + 1 );

	m_uiOpenChatWin.AddMouseEventHandler( this );					
	
	Add( m_pInputField );
	Add( &m_uiOpenChatWin );

	DisableFocus();
	SetFocusObject( m_pInputField ); 	

	SetFocusObject( m_pInputField );

	SetSprite();
}

void
CGUIChatInputWindow::SetSprite( void )
{
	SPRITE_INFO		l_spriteInfo;
	l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_CHAT );	
	
	l_spriteInfo.wActionNo = 1;
	l_spriteInfo.wFrameNo = 8;
	
	m_uiOpenChatWin.SetSpriteInfo( &l_spriteInfo );
}

void
CGUIChatInputWindow::SetMaxLineNum( WORD pi_wMaxLineNum )
{
	m_wMaxLineNum = pi_wMaxLineNum;

	m_qChatMsg.Destroy();

//	m_qChatMsg.Create( m_wMaxLineNum * 3 );
	m_qChatMsg.Create( m_wMaxLineNum );
}

void
CGUIChatInputWindow::InsertChatMsg( char * pi_pMsg, FONT_INFO * pi_pFontInfo )
{
	if( m_wMaxLineNum == 0 )
		return;

	CGUITextInfo	l_MsgInfo;	
	int l_nCharIndex;
	char buf[128];	
	
	l_nCharIndex = 0;
	buf[0] = '\0';
	
	for( int i=0; pi_pMsg[i] != '\0'; ++i )
	{
		if( pi_pMsg[i] == '\n' )
		{
			buf[l_nCharIndex] = '\0';

			// insert msg
			l_MsgInfo.SetText( buf );
			l_MsgInfo.SetFont( pi_pFontInfo );

			m_qChatMsg.Push( l_MsgInfo );
			
			// init
			l_nCharIndex = 0;
			buf[0] = '\0';
		}
		else
		{
			buf[l_nCharIndex] = pi_pMsg[i];

			if( l_nCharIndex+1 < m_wMaxCharNumInLine )
			{
				++l_nCharIndex;
				continue;
			}

			buf[l_nCharIndex+1] = '\0';

			if( buf[l_nCharIndex] & 0x80 )
			{
				for( int ch=l_nCharIndex-1; ch>=0; --ch )
				{
					if( !(buf[ch] & 0x80) )					
						break;					
				}

				if( ch < 0 )
				{
					if( (l_nCharIndex+1) % 2 != 0 )
					{						
						buf[l_nCharIndex] = '\0';
						--i;
					}					
				}
				else
				{
					if( (l_nCharIndex - ch) % 2 != 0 )
					{						
						buf[l_nCharIndex] = '\0';
						--i;
					}					
				}
			}			

			// insert msg
			l_MsgInfo.SetText( buf );
			l_MsgInfo.SetFont( pi_pFontInfo );

			m_qChatMsg.Push( l_MsgInfo );
			
			// init
			l_nCharIndex = 0;
			buf[0] = '\0';
		}
	}

	// buf에 남아있는 것도 insert한다
	if( l_nCharIndex > 0 )
	{
		buf[l_nCharIndex] = '\0';

		// insert msg
		l_MsgInfo.SetText( buf );
		l_MsgInfo.SetFont( pi_pFontInfo );

		m_qChatMsg.Push( l_MsgInfo );
	}
	
}

/*
// 한줄에 두개이상 다른 font로 출력할 때 
void
CGUIChatInputWindow::InsertChatMsg( char * pi_pMsg, FONT_INFO * pi_pFontInfo )
{
	if( m_wMaxLineNum == 0 )
		return;

	int i, ch;	
	int l_nPrevLastCharNum = m_wLastCharNum;

	// set total new line, last line char num
	for( i=0; pi_pMsg[i] != '\0'; ++i )
	{
		if( pi_pMsg[i] == '\n' )
		{
			// increase line
			++m_wTotalNewLineNum;

			// first line
			m_wLastCharNum = 0;
		}
		else
		{
			++m_wLastCharNum;
			
			if( m_wLastCharNum == m_wMaxCharNumInLine )
			{
				// increase line
				++m_wTotalNewLineNum;

				// first line
				m_wLastCharNum = 0;
				
				if( pi_pMsg[i] & 0x80 )
				{				
					for( ch=i; ch>=0; --ch )
					{
						if( !( pi_pMsg[ch] & 0x80 ) )					
						{							
							if( ( i - ch ) % 2 != 0 )
								++m_wLastCharNum;
							break;
						}
					}

					if( ch < 0 )
					{
						if( ( i - ch + 1 ) % 2 != 0 )
							++m_wLastCharNum;
					}
				}
				else
				{
					// line over됐을 때 줄에 첫 문자가 new line일 때 건너뛴다.
					if( pi_pMsg[i+1] != '\0' &&
						pi_pMsg[i+1] == '\n' )
					{					
						++i; // skip new line
					}
				}
			}
		}
	}


	// add chat message
	CGUITextInfo	l_MsgInfo;
	l_MsgInfo.SetText( pi_pMsg );
	l_MsgInfo.SetFont( pi_pFontInfo );

	m_qChatMsg.Push( l_MsgInfo );


	// max line을 넘으면 앞줄을 삭제한다.
	if( m_wTotalNewLineNum > m_wMaxLineNum ||
		( m_wTotalNewLineNum == m_wMaxLineNum &&
		  m_wLastCharNum > 0 ) )
	{
		// 제거할 줄수
		WORD l_wRemoveLine;
		l_wRemoveLine = m_wTotalNewLineNum - m_wMaxLineNum;
		if( m_wLastCharNum > 0 )
			++l_wRemoveLine;
		
		char * l_pMsg;
		WORD l_wLineCount = 0;	
		WORD l_wCharCount = 0;
		WORD l_wStartCharIndex = m_wFirstCharIndex;
		int j;

		for( i=0; i<m_qChatMsg.GetCurNum(); ++i )
		{
			l_pMsg = m_qChatMsg[i].GetText();

			for( j=l_wStartCharIndex; l_pMsg[j] != '\0'; ++j )
			{
				++l_wCharCount;

				if( l_pMsg[j] == '\n' )
				{		
					++l_wLineCount;					
					l_wCharCount = 0;
					
					if( l_wLineCount == l_wRemoveLine )
					{
						if( l_pMsg[j+1] == '\0' )
						{
							++i;
							m_wFirstCharIndex = 0;
						}
						else
						{ 
							m_wFirstCharIndex = j+1;
						}
						
						break;					
					}
				}
				else if( l_wCharCount >= m_wMaxCharNumInLine )
				{
					++l_wLineCount;					
					l_wCharCount = 0;					

					if( l_pMsg[j] & 0x80 )
					{				
						for( ch=j; ch>=0; --ch )
						{
							if( !( l_pMsg[ch] & 0x80 ) )					
							{							
								if( ( j - ch ) % 2 != 0 )
									l_wCharCount = 1;
								break;
							}
						}

						if( ch < 0 )
						{
							if( ( j - ch + 1 ) % 2 != 0 )
								l_wCharCount = 1;
						}

						if( l_wLineCount == l_wRemoveLine )
						{
							m_wFirstCharIndex = j + 1 - l_wCharCount;
							break;
						}
					}
					else
					{
						if( l_wLineCount == l_wRemoveLine )
						{
							if( l_pMsg[j+1] != '\0' &&
								l_pMsg[j+1] == '\n' )
								m_wFirstCharIndex = j + 2;
							else 
								m_wFirstCharIndex = j + 1;

							if( l_pMsg[m_wFirstCharIndex] == '\0' )
							{
								++i;
								m_wFirstCharIndex = 0;
							}
							
							break;
						}
						else if( l_pMsg[j+1] != '\0' &&
								 l_pMsg[j+1] == '\n' )
						{
							// line over됐을 때 줄에 첫 문자가 new line일 때 건너뛴다.
							
							++j; // skip new line							
						}
					}					
					
				}				
			}				
			
			if( l_wLineCount == l_wRemoveLine )
				break;

			l_wStartCharIndex = 0;
		}	
		

		// remove line		
		for( j=0; j<i; ++j )
			m_qChatMsg.Pop();
		
		m_wViewLineNum = m_wMaxLineNum;
		m_wTotalNewLineNum = m_wViewLineNum;
		if( m_wLastCharNum > 0 )		
			--m_wTotalNewLineNum;
	}
	else
	{
		m_wViewLineNum = m_wTotalNewLineNum;
		if( m_wLastCharNum > 0 )
			++m_wViewLineNum;
	}
}
*/

void
CGUIChatInputWindow::KeyEvent( _key_event & event )
{
	if( event.key == VK_RETURN &&
		event.source == m_pInputField )
	{
		if( m_pfEventProc_Ex )
			m_pfEventProc_Ex( ( void * )event.source );
	}
}

void
CGUIChatInputWindow::MouseEvent( _mouse_event & event )
{
	if( event.id == MOUSE_CLICKED )
	{
		if( event.source == &m_uiOpenChatWin )
		{	
			m_pChatWindow->Open();						
		}
	}
}

BOOL
CGUIChatInputWindow::Draw( void )
{
	if( !IsShow() )
		return false;


	if( m_pInputField->IsFocused() )
	{	
		Draw2DRectangle( m_ptPos.x, m_ptPos.y,
						 m_ptPos.x + m_ptSize.x, 
						 m_ptPos.y + m_ptSize.y,
						 0x90604023 );	

		// top
		Draw2DRectangle( m_ptPos.x, m_ptPos.y,
						 m_ptPos.x + m_ptSize.x, 
						 m_ptPos.y + 1,
						 0xA0D5B383 );

		// bottom
		Draw2DRectangle( m_ptPos.x, m_ptPos.y + m_ptSize.y - 1,
						 m_ptPos.x + m_ptSize.x, 
						 m_ptPos.y + m_ptSize.y,
						 0xA0D5B383 );

		// left
		Draw2DRectangle( m_ptPos.x, m_ptPos.y + 1,
						 m_ptPos.x + 1, 
						 m_ptPos.y + m_ptSize.y - 1,
						 0xA0D5B383 );

		// right
		Draw2DRectangle( m_ptPos.x + m_ptSize.x - 1, m_ptPos.y + 1,
						 m_ptPos.x + m_ptSize.x, 
						 m_ptPos.y + m_ptSize.y - 1,
						 0xA0D5B383 );		
	}
	else
	{		
		Draw2DRectangle( m_ptPos.x, m_ptPos.y,
						 m_ptPos.x + m_ptSize.x,
						 m_ptPos.y + m_ptSize.y,
						 0x52604023 );	
	}


	CGUIContainer::Draw();

	DWORD l_dwAlpha;

	// ---------------------------------------------------------------
	// chat msg
	for( int i=0; i<m_qChatMsg.GetCurNum(); ++i )
	{
		if( i + m_wMaxLineNum - m_qChatMsg.GetCurNum() == 0 )
			l_dwAlpha = 0x7FFFFFFF;
		else if( i + m_wMaxLineNum - m_qChatMsg.GetCurNum() == 1 )
			l_dwAlpha = 0xBFFFFFFF;
		else
			l_dwAlpha = 0xFFFFFFFF;

		DrawR3Hangul( m_ptPos.x,
					  m_ptPos.y + ( i - m_qChatMsg.GetCurNum() ) * ( TWOBYTE_FONTSIZE+m_wColumnGap ),
					  m_qChatMsg[i].GetText(),
					  m_qChatMsg[i].GetFont()->dwColor & l_dwAlpha,
					  m_qChatMsg[i].GetFont()->dwStyle );
	}	
	// ---------------------------------------------------------------
	
	DrawDialogBox();

	return true;
}

/*
// 한줄에 두개이상 다른 font로 출력할 때 
BOOL
CGUIChatInputWindow::Draw( void )
{
	if( !IsShow() )
		return false;

	Draw2DRectangle( m_ptPos.x, m_ptPos.y,
					 m_ptPos.x + m_ptSize.x, 
					 m_ptPos.y + m_ptSize.y,
					 0x70604023 );

	CGUIContainer::Draw();

	// ---------------------------------------------------------------
	// chat msg
	char *	l_pText;
	WORD	l_wStrLength;
	char *	l_pNewLine;
	
	WORD	l_wLineCount = 0;	
	WORD	l_wRestCharCountInLine = m_wMaxCharNumInLine;
	WORD	l_wCurrentCharIndex = m_wFirstCharIndex;	

	WORD	l_wNewLineIndex;
	WORD	l_wDrawStartIndex;
	BOOL	l_bCutChar;

	DWORD	l_dwAlpha;

	for( int i=0; i<m_qChatMsg.GetCurNum(); ++i )
	{
		l_pText = m_qChatMsg[i].GetText();
		l_wStrLength = strlen( l_pText );

		while( 1 )
		{
			l_pNewLine = strstr( l_pText + l_wCurrentCharIndex, "\n" );			
			if( !l_pNewLine )			
			{
				l_wNewLineIndex = l_wStrLength;
			}			
			else
			{				
				l_wNewLineIndex = l_pNewLine - l_pText;				
			}
			
			l_wDrawStartIndex = l_wCurrentCharIndex;

			// 한 라인만 끊어서 처리한다.
			while( l_wDrawStartIndex < l_wNewLineIndex )
			{
				// x축 여백이 모자랄 때 여백 크기만큼만 출력해주고 다시 체크한다.				
				if( l_wNewLineIndex - l_wDrawStartIndex > l_wRestCharCountInLine )
				{
					memcpy( m_pOneLineBuf, l_pText + l_wDrawStartIndex, l_wRestCharCountInLine );
					m_pOneLineBuf[l_wRestCharCountInLine] = '\0';

					l_bCutChar = false;

					if( m_pOneLineBuf[l_wRestCharCountInLine-1] & 0x80 )
					{
						for( int j=l_wRestCharCountInLine-2; j>=0; --j )
						{
							if( !( m_pOneLineBuf[j] & 0x80 ) )
							{
								if( (l_wRestCharCountInLine-1-j)%2 != 0 )
									l_bCutChar = true;
								break;
							}								
						}
						if( j < 0 )
						{							
							if( l_wRestCharCountInLine % 2 != 0 )
								l_bCutChar = true;
						}
					}

					if( l_bCutChar )
					{
						m_pOneLineBuf[l_wRestCharCountInLine - 1] = '\0';

						// l_wDrawStartIndex incrase
						l_wDrawStartIndex += l_wRestCharCountInLine - 1;
					}
					else
					{
						// l_wDrawStartIndex incrase
						l_wDrawStartIndex += l_wRestCharCountInLine;
					}

					if( l_wLineCount + m_wMaxLineNum - m_wViewLineNum == 0 )					
						l_dwAlpha = 0x7FFFFFFF;											
					else if( l_wLineCount + m_wMaxLineNum - m_wViewLineNum == 1 )
						l_dwAlpha = 0xBFFFFFFF;					
					else
						l_dwAlpha = 0xFFFFFFFF;

					
					DrawR3Hangul( m_ptPos.x + (m_wMaxCharNumInLine - l_wRestCharCountInLine) * ONEBYTE_FONTSIZE,
								  m_ptPos.y - m_wMaxLineNum * (TWOBYTE_FONTSIZE + m_wColumnGap) +
										( l_wLineCount + m_wMaxLineNum - m_wViewLineNum ) *
										(TWOBYTE_FONTSIZE+m_wColumnGap),
								  m_pOneLineBuf,
								  m_qChatMsg[i].GetFont()->dwColor & l_dwAlpha,								  								  
								  m_qChatMsg[i].GetFont()->dwStyle );

					// to next line					
					l_wRestCharCountInLine = m_wMaxCharNumInLine;
					++l_wLineCount;					
					
					if( l_wLineCount == m_wMaxLineNum )
						return true;
				}
				// x축으로 여백이 모자라지 않을 때 남은 text를 출력하고 나온다.
				else
				{					
					memcpy( m_pOneLineBuf, l_pText + l_wDrawStartIndex, l_wNewLineIndex - l_wDrawStartIndex );
					m_pOneLineBuf[l_wNewLineIndex - l_wDrawStartIndex] = '\0';	
					
					if( l_wLineCount + m_wMaxLineNum - m_wViewLineNum == 0 )					
						l_dwAlpha = 0x7FFFFFFF;
					else if( l_wLineCount + m_wMaxLineNum - m_wViewLineNum == 1 )
						l_dwAlpha = 0xBFFFFFFF;					
					else
						l_dwAlpha = 0xFFFFFFFF;
					
					DrawR3Hangul( m_ptPos.x + (m_wMaxCharNumInLine - l_wRestCharCountInLine) * ONEBYTE_FONTSIZE,
								  m_ptPos.y - m_wMaxLineNum * (TWOBYTE_FONTSIZE + m_wColumnGap) +
										( l_wLineCount + m_wMaxLineNum - m_wViewLineNum ) *
										(TWOBYTE_FONTSIZE+m_wColumnGap),
								  m_pOneLineBuf,
								  m_qChatMsg[i].GetFont()->dwColor & l_dwAlpha,								  
								  m_qChatMsg[i].GetFont()->dwStyle );													  

					// l_nTextXPos set					
					l_wRestCharCountInLine -= l_wNewLineIndex - l_wDrawStartIndex;

					break;
				}
			}

			if( l_pNewLine )
			{
				// to next line				
				l_wRestCharCountInLine = m_wMaxCharNumInLine;
				++l_wLineCount;
			}							
			// next char
			l_wCurrentCharIndex = l_wNewLineIndex + 1;
			
			if( l_wLineCount == m_wMaxLineNum )
				return true;			
			
			if( l_wCurrentCharIndex + 1 >= l_wStrLength ) // text를 다 출력했을 때
				break;				
		}

		if( l_wLineCount == m_wMaxLineNum )
			return true;

		// init			
		l_wCurrentCharIndex = 0;		
	}
	
	// ---------------------------------------------------------------
	DrawDialogBox();

	return true;
}
*/

////////////////////////////////////////////////////////////////////////////
//
// CGUIBuddyWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////

CGUIBuddyWindow::CGUIBuddyWindow()
{
	m_bCreated = FALSE;
}

CGUIBuddyWindow::~CGUIBuddyWindow()
{

}

void
CGUIBuddyWindow::Init( void )
{
	SetSize( 136, 196 );
	m_uiBoard.SetSize( 136, 196 );

	m_uiCloseButton.SetSize( 14, 14 );

	char buf[17];
	memset( buf, ' ', sizeof(char)*16 );
	buf[16] = '\0';

	for( int i=0; i<8; ++i )
	{
		m_uiBuddy[i].SetString( buf );
		m_uiRemove[i].SetSize( 16, 16 );

		m_uiBuddy[i].SetPos( m_ptPos.x + 30, m_ptPos.y + 26 + i * 22 ); 
		m_uiRemove[i].SetPos( m_ptPos.x + 8, m_ptPos.y + 24 + i * 22 );		
	}	
	
	m_uiBoard.SetPos( m_ptPos.x, m_ptPos.y );
	m_uiCloseButton.SetPos( m_ptPos.x + 121, m_ptPos.y + 3 );


	Add( &m_uiBoard );
	m_uiCloseButton.AddMouseEventHandler( this );
	Add( &m_uiCloseButton );
	for( i=0; i<8; ++i )
	{
		m_uiBuddy[i].AddMouseEventHandler( this );		
		Add( &m_uiBuddy[i] );

		m_uiRemove[i].AddMouseEventHandler( this );		
		Add( &m_uiRemove[i] );		
	}

	// ----------------------------------------------------------------
	SetSprite();

	// ----------------------------------------------------------------
	SetMoveObject( &m_uiBoard );
	SetMovable( TRUE );

	// init
	for( i=0; i<8; ++i )
	{
		m_dwBuddyID[i] = -1L;	
		m_uiBuddy[i].Disable();
		EnableRemove( i, FALSE );
	}

	m_bCreated = TRUE;
}

void
CGUIBuddyWindow::SetSprite( void )
{
	if( !m_bCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_CHAT );	
		
		l_spriteInfo.wActionNo = 2;
		l_spriteInfo.wFrameNo = 0;
		SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 1;
		m_uiBoard.SetSpriteInfo( &l_spriteInfo );
		
		m_uiCloseButton.SetSpriteInfoEx( gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON1 ) );
		m_uiCloseButton.SetSpriteInfoEx( HOVERED, gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON2 ) );
		m_uiCloseButton.SetSpriteInfoEx( PRESSED, gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON3 ) );

		m_spriteRemove.pSprite = gSpriteMgr.GetSprite( SP_ID_PARTY );
		m_spriteRemove.wActionNo = 1;
		m_spriteRemove.wFrameNo = 3;	
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_CHAT );
		if( !l_pSprite )
			return;
		
		// background
		GetSpriteInfo()->pSprite = l_pSprite;

		m_uiBoard.GetSpriteInfo()->pSprite = l_pSprite;

		l_pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );
		m_uiCloseButton.SetSprite( l_pSprite );

		m_spriteRemove.pSprite = gSpriteMgr.GetSprite( SP_ID_PARTY );

		for( int i=0; i<8; ++i )
			m_uiRemove[i].SetSprite( m_spriteRemove.pSprite );
	}
}

void
CGUIBuddyWindow::AddBuddy( DWORD pi_dwBuddyID, char * pi_pBuddyName )
{
	for( int i=0; i<8; ++i )
	{
		if( m_dwBuddyID[i] == -1L )		
			break;		
	}

	// 빈칸이 없다.
	if( i == 8 )
		return;	

	m_dwBuddyID[i] = pi_dwBuddyID;
	m_uiBuddy[i].SetString( pi_pBuddyName );
	// resize
	m_uiBuddy[i].SetSize( 96, 12 );
	m_uiBuddy[i].Enable();
	// enable remove
	EnableRemove( i, TRUE );
}
	
void
CGUIBuddyWindow::RemoveBuddy( DWORD pi_dwBuddyID )
{
	WORD l_wRemoveIndex;

	for( int i=0; i<8; ++i )
	{
		if( m_dwBuddyID[i] == pi_dwBuddyID )		
		{
			l_wRemoveIndex = i;
			break;
		}
	}

	// buddy가 없다.
	if( i == 8 )
		return;

	// remove buddy
	m_dwBuddyID[l_wRemoveIndex] = -1L;
	m_uiBuddy[l_wRemoveIndex].SetString( NULL );	
	m_uiBuddy[l_wRemoveIndex].SetSize( 96, 12 );
	m_uiBuddy[l_wRemoveIndex].Disable();
	// disable remove	
	EnableRemove( l_wRemoveIndex, FALSE );

	// shift buddy list
	for( i=l_wRemoveIndex; i<7; ++i )
	{
		if( m_dwBuddyID[i+1] == -1L )
			break;

		m_dwBuddyID[i] = m_dwBuddyID[i + 1];
		m_uiBuddy[i].SetString( m_uiBuddy[i+1].GetString() );				
		m_uiBuddy[i].SetSize( 96, 12 );
		EnableRemove( i, TRUE );
		m_uiBuddy[i].Enable();
	}
	
	// 마지막 건 비운다.
	if( i > l_wRemoveIndex )
	{
		m_dwBuddyID[i] = -1L;		
		m_uiBuddy[i].SetString( " " );
		m_uiBuddy[i].SetSize( 96, 12 );
		m_uiBuddy[i].Disable();
		EnableRemove( i, FALSE );	
	}
}

void
CGUIBuddyWindow::EnableRemove( WORD pi_wIndex, BOOL pi_bEnable )
{	
	if( pi_bEnable )
	{
		m_uiRemove[pi_wIndex].SetSpriteInfoEx( &m_spriteRemove );

		++m_spriteRemove.wFrameNo;
		
		m_uiRemove[pi_wIndex].SetSpriteInfoEx( PRESSED, &m_spriteRemove );
		
		--m_spriteRemove.wFrameNo;

		// enable
		m_uiRemove[pi_wIndex].Enable();
	}
	else
	{
		m_spriteRemove.wFrameNo += 2;

		m_uiRemove[pi_wIndex].SetSpriteInfoEx( &m_spriteRemove );

		m_spriteRemove.wFrameNo -= 2;

		// disable 
		m_uiRemove[pi_wIndex].Disable();
	}
}

void
CGUIBuddyWindow::MouseEvent( _mouse_event &event )
{
	CGUIWindow::MouseEvent( event );

	if( event.id == MOUSE_CLICKED )
	{
		if( event.source == &m_uiCloseButton )
		{
			Close();
			return;
		}
		
		for( int i=0; i<8; ++i )
		{
			if( event.source == &m_uiRemove[i] )
			{	
				// ################## 코드 추가 시킬 곳 ###########################
				// 친구 리스트에서 지운다.			

				// test
				RemoveBuddy( m_dwBuddyID[i] );

				return;
			}		
		}
	}
	else if( event.id == MOUSE_STROKED )
	{
		for( int i=0; i<8; ++i )
		{
			if( event.source == &m_uiBuddy[i] )
			{
				// ################## 코드 추가 시킬 곳 ###########################
				// 채팅창에 귓속말 대상으로 설정한다.

				return;
			}
		}
	}
}