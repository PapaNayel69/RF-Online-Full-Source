#include "GUIMessagebox.h"
#include "GUIScreen.h"
#include "../SpriteMgr.h"

#define	DEFAULT_SCROLL_BOARD_SIZE_X		300
#define	DEFAULT_SCROLL_BOARD_SIZE_Y		250

#define	MSG_MARGIN			15

#define	OUTLINE_MARGIN		3
#define	OUTLINE_SIZE		1


////////////////////////////////////////////////////////////////////////////
//
// msg box, msg window util
//
////////////////////////////////////////////////////////////////////////////

#define MSG_BOX_QSIZE		4
CGUIMessagebox	g_uiMsgBox[MSG_BOX_QSIZE];

#define	MSG_WIN_QSIZE		4
CGUIMessageWin	g_uiMsgWin[MSG_WIN_QSIZE];

void _GUIErrorMessageBox( char * pi_pMsg )
{
	CGUIMessagebox * l_pErroMsgBox;

	l_pErroMsgBox = new CGUIMessagebox;

	l_pErroMsgBox->Init( DLG_STYLE_OK );
	l_pErroMsgBox->SetDestroy( TRUE );
	
	l_pErroMsgBox->SetMsgBoardSize( 0, 0 );	
	
	l_pErroMsgBox->SetMsg( pi_pMsg );	
	
	// set error message user defined id
	l_pErroMsgBox->SetUserDefinedID( MSGBOX_ID_CRITICAL_ERROR );

	l_pErroMsgBox->SetOwnerWindow( NULL );	
	l_pErroMsgBox->SetAlpha( 0xD0 );	
	
	POINT l_ptPos;
	l_ptPos.x = ( gScreen.m_ptSize.x - l_pErroMsgBox->m_ptSize.x ) / 2;
	l_ptPos.y = ( gScreen.m_ptSize.y - l_pErroMsgBox->m_ptSize.y ) / 2;
	l_pErroMsgBox->SetPos( l_ptPos );

	// open msgbox
	l_pErroMsgBox->Open();
}

void _GUIMessageBox( _msgbox_create_info * pi_pCreateInfo )
{
	// error msg창이 떠있으면 msgbox를 띄울수없다.
	if( _IsOpenedDialogBox( NULL, MSGBOX_ID_CRITICAL_ERROR ) )
		return;

	int l_nAvailMsgBox = -1;

	for( int i=0; i<MSG_BOX_QSIZE; ++i )
	{
		if( !g_uiMsgBox[i].IsOpen() )
		{
			l_nAvailMsgBox = i;
			break;
		}
	}

	if( l_nAvailMsgBox == -1 )
		return;

	g_uiMsgBox[l_nAvailMsgBox].Init( pi_pCreateInfo->wDlgStyle );

	g_uiMsgBox[l_nAvailMsgBox].SetMsgBoardSize( pi_pCreateInfo->ptBoardSize.x,
												pi_pCreateInfo->ptBoardSize.y );
	g_uiMsgBox[l_nAvailMsgBox].SetScrollable( pi_pCreateInfo->bScrollable );
	g_uiMsgBox[l_nAvailMsgBox].SetMsg( pi_pCreateInfo->pMsg );
	
	g_uiMsgBox[l_nAvailMsgBox].SetUserDefinedID( pi_pCreateInfo->wUserDefinedID );	
	g_uiMsgBox[l_nAvailMsgBox].SetOwnerWindow( pi_pCreateInfo->pOwnerWindow );	

	g_uiMsgBox[l_nAvailMsgBox].SetAlpha( pi_pCreateInfo->dwRGBA >> 24 );
	g_uiMsgBox[l_nAvailMsgBox].SetDuration( pi_pCreateInfo->wDurationTime );
	
	g_uiMsgBox[l_nAvailMsgBox].SetModal( pi_pCreateInfo->bModal );
	g_uiMsgBox[l_nAvailMsgBox].LockOtherWin( pi_pCreateInfo->bLockOtherWin );
	if( !pi_pCreateInfo->bLockOtherWin )
		g_uiMsgBox[l_nAvailMsgBox].SetMovable( TRUE );

	// set pos
	if( pi_pCreateInfo->ptPos.x == -1 &&
		pi_pCreateInfo->ptPos.y == -1 )
	{
		POINT l_ptPos;
		l_ptPos.x = ( gScreen.m_ptSize.x - g_uiMsgBox[l_nAvailMsgBox].m_ptSize.x ) / 2;
		l_ptPos.y = ( gScreen.m_ptSize.y - g_uiMsgBox[l_nAvailMsgBox].m_ptSize.y ) / 2;
		g_uiMsgBox[l_nAvailMsgBox].SetPos( l_ptPos );
	}
	else
	{
		g_uiMsgBox[l_nAvailMsgBox].SetPos( pi_pCreateInfo->ptPos );
	}	

	g_uiMsgBox[l_nAvailMsgBox].Open();
}


void _GUIMessageBox( CGUIWindow	*	pi_pOwnerWindow,
					 char		*	pi_pMsg,
					 DWORD			pi_dwRGBA/* = MSG_BOX_RGBA*/,
					 WORD			pi_wUserDefinedID/* = NOT_DEFINE_ID*/,
					 WORD			pi_wDlgStyle/* = DLG_STYLE_OK*/,
					 DWORD			pi_wDurationTime/* = 0xffffffff*/,
					 BOOL			pi_bScrollable/* = FALSE*/ )					 
{
	// error msg창이 떠있으면 msgbox를 띄울수없다.
	if( _IsOpenedDialogBox( NULL, MSGBOX_ID_CRITICAL_ERROR ) )
		return;

	int l_nAvailMsgBox = -1;

	for( int i=0; i<MSG_BOX_QSIZE; ++i )
	{
		if( !g_uiMsgBox[i].IsOpen() )
		{
			l_nAvailMsgBox = i;
			break;
		}
	}

	if( l_nAvailMsgBox == -1 )
		return;

	g_uiMsgBox[l_nAvailMsgBox].Init( pi_wDlgStyle );
	g_uiMsgBox[l_nAvailMsgBox].SetMsgBoardSize( 0, 0 );
	g_uiMsgBox[l_nAvailMsgBox].SetScrollable( pi_bScrollable );
	g_uiMsgBox[l_nAvailMsgBox].SetMsg( pi_pMsg );
	
	g_uiMsgBox[l_nAvailMsgBox].SetUserDefinedID( pi_wUserDefinedID );	
	g_uiMsgBox[l_nAvailMsgBox].SetOwnerWindow( pi_pOwnerWindow );	
	g_uiMsgBox[l_nAvailMsgBox].SetAlpha( pi_dwRGBA >> 24 );
	g_uiMsgBox[l_nAvailMsgBox].SetDuration( pi_wDurationTime );	
	//  position은 default로 화면 가운데로 한다.
	POINT l_ptPos;
	l_ptPos.x = ( gScreen.m_ptSize.x - g_uiMsgBox[l_nAvailMsgBox].m_ptSize.x ) / 2;
	l_ptPos.y = ( gScreen.m_ptSize.y - g_uiMsgBox[l_nAvailMsgBox].m_ptSize.y ) / 2;
	g_uiMsgBox[l_nAvailMsgBox].SetPos( l_ptPos );

	// open msgbox
	g_uiMsgBox[l_nAvailMsgBox].Open();
}

void
_GUIMessageWin( char		*	pi_pMsg,
				DWORD			pi_dwRGBA/* = MSG_BOX_RGBA*/,
				WORD			pi_wUserDefinedID/* = NOT_DEFINE_ID*/,				
				int				pi_nMsgBoardSizeX/* = 0*/,
				int				pi_nMsgBoardSizeY/* = 0*/ )
{
	int l_nAvailMsgWin = -1;

	for( int i=0; i<MSG_WIN_QSIZE; ++i )
	{
		if( !g_uiMsgWin[i].IsShow() )
		{
			l_nAvailMsgWin = i;
			break;
		}
	}

	if( l_nAvailMsgWin == -1 )
		return;

	g_uiMsgWin[i].SetID( MSG_WIN_ID );
	g_uiMsgWin[i].SetUserDefinedID( pi_wUserDefinedID );

	g_uiMsgWin[i].Init();
	g_uiMsgWin[i].SetAlpha( pi_dwRGBA );	

	g_uiMsgWin[i].SetMsgBoardSize( pi_nMsgBoardSizeX, pi_nMsgBoardSizeY );	
	
	g_uiMsgWin[i].SetMsg( pi_pMsg );

	POINT l_ptPos;
	l_ptPos.x = ( gScreen.m_ptSize.x - g_uiMsgWin[l_nAvailMsgWin].m_ptSize.x ) / 2;
	l_ptPos.y = ( gScreen.m_ptSize.y - g_uiMsgWin[l_nAvailMsgWin].m_ptSize.y ) / 2;
	g_uiMsgWin[l_nAvailMsgWin].SetPos( l_ptPos );


	gScreen.Add( &g_uiMsgWin[i] );
	
	g_uiMsgWin[i].Open();
}

void
_CloseMessageWin( WORD		pi_wUserDefinedID/* = NOT_DEFINE_ID*/ )
{
	CGUIWindow	* l_pWin;
	CGUIMessageWin * l_pMsgWin;

	int l_nTotalWinNum = gScreen.GetAddedObjectNum();
	for( int i=0; i<l_nTotalWinNum; ++i )
	{
		l_pWin = (CGUIWindow * )gScreen.GetAddedObject( i );

		if( l_pWin->GetID() == MSG_WIN_ID )
		{
			l_pMsgWin = (CGUIMessageWin *)l_pWin;

			if( l_pMsgWin->GetUserDefinedID() == pi_wUserDefinedID )
			{
				l_pWin->Close();
				gScreen.Remove( l_pWin );
			}

			break;
		}
	}
}

void
_CloseAllMessageWin( void )
{
	CGUIWindow	* l_pWin;	
	int l_nTotalWinNum;

	while( 1 )
	{
		l_nTotalWinNum = gScreen.GetAddedObjectNum();
		for( int i=0; i<l_nTotalWinNum; ++i )
		{
			l_pWin = (CGUIWindow * )gScreen.GetAddedObject( i );

			if( l_pWin->GetID() == MSG_WIN_ID )
				break;	
		}

		if( i >= l_nTotalWinNum )
			break;

		l_pWin = (CGUIWindow * )gScreen.GetAddedObject( i );
		l_pWin->Close();
		gScreen.Remove( l_pWin );
	}
}


////////////////////////////////////////////////////////////////////////////
//
// CGUIMessagebox Class Implementation
//
////////////////////////////////////////////////////////////////////////////

CGUIMessagebox::CGUIMessagebox()
:CGUIDialogBox( NULL, DLG_STYLE_OK, TRUE, 0 )
{
	m_bScrollable = FALSE;
}

CGUIMessagebox::CGUIMessagebox( CGUIWindow * pi_pOwnerWindow, char * msg, WORD pi_wDlgStyle,WORD pi_wUserDefinedID, BOOL pi_bScrollable/*=FALSE*/ )
:CGUIDialogBox( pi_pOwnerWindow, pi_wDlgStyle, TRUE, pi_wUserDefinedID )
{	
	//Init( pi_wDlgStyle, pi_bScrollable );	
	Init( pi_wDlgStyle );	

	SetScrollable( pi_bScrollable );

	// set message, window size
	SetMsg( msg );
}

CGUIMessagebox::~CGUIMessagebox()
{
	
}

void
CGUIMessagebox::Init( WORD pi_wStyle )
{
	// ----------------------------------------------------------
	// size	
	m_uiOKButton.SetSize( 42, 39 );
	m_uiCancelButton.SetSize( 42, 39 );
	m_uiCloseButton.SetSize( 14, 14 );

	// scrollbar
	POINT l_ptSize;
	l_ptSize.x = 15;	l_ptSize.y = 10;
	m_uiScrollbar.SetTopArrowSize( l_ptSize );
	m_uiScrollbar.SetBottomArrowSize( l_ptSize );	
	l_ptSize.y = 18;
	m_uiScrollbar.SetBarSize( l_ptSize );	
	l_ptSize.x = 15;
	m_uiScrollbar.SetSize( l_ptSize );

	// add controller by dialogbox style
	SetStyle( pi_wStyle );	

	SetSprite();
	
	m_cUpperOutLine.SetARGB( D3DCOLOR_XRGB( 149, 147, 193 ) );
	m_cLowerOutLine.SetARGB( m_cUpperOutLine.GetARGB() );
	m_cLeftOutLine.SetARGB( m_cUpperOutLine.GetARGB() );
	m_cRightOutLine.SetARGB( m_cUpperOutLine.GetARGB() );		
	
	SetMoveObject( NULL ); // clear movable object
	SetMoveObject( &m_cBase );
	SetMoveObject( &m_uiMsgBoard );

	// default로 열려있는 제한시간이 없다.
	m_dwDuration = 0xffffffff;

	m_uiMsgBoard.SetMargin( 5, 5 );
	m_uiMsgBoard.SetColumnGap( 4 );


	SetWheelScrollObject( &m_uiScrollbar );
}

void
CGUIMessagebox::SetSprite( void )
{
	SPRITE_INFO l_spriteInfo;
	l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );
	

	// button
	if( m_wDlgStyleID == DLG_STYLE_OK_CANCEL )
	{		
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 3;
		m_uiOKButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 4;
		m_uiOKButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 5;
		m_uiOKButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );

		l_spriteInfo.wFrameNo = 6;
		m_uiCancelButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 7;
		m_uiCancelButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 8;
		m_uiCancelButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
	}
	else if( m_wDlgStyleID == DLG_STYLE_OK )
	{	
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 0;
		m_uiOKButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 1;
		m_uiOKButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 2;
		m_uiOKButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
	}

	l_spriteInfo.wActionNo = 1;

	// close button
	l_spriteInfo.wFrameNo = 9;
	m_uiCloseButton.SetSpriteInfoEx( &l_spriteInfo );
	l_spriteInfo.wFrameNo = 10;
	m_uiCloseButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
	l_spriteInfo.wFrameNo = 11;
	m_uiCloseButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );	

	// base
	l_spriteInfo.wFrameNo = 0;
	m_cBase.SetSpriteInfo( &l_spriteInfo );

	// scrollbar	
	SPRITE_INFO l_sScrollbarSprite[2];
	l_sScrollbarSprite[0].pSprite = l_spriteInfo.pSprite;
	l_sScrollbarSprite[0].wActionNo = 4;
	memcpy( &l_sScrollbarSprite[1], &l_sScrollbarSprite[0], sizeof( SPRITE_INFO ) );
	
	l_sScrollbarSprite[0].wFrameNo = 0;
	m_uiScrollbar.SetSpriteInfo( &l_sScrollbarSprite[0] );
	l_sScrollbarSprite[0].wFrameNo = 1;
	m_uiScrollbar.SetBarSpriteInfo( &l_sScrollbarSprite[0] );

	l_sScrollbarSprite[0].wFrameNo = 2;
	l_sScrollbarSprite[1].wFrameNo = 3;
	m_uiScrollbar.SetTopArrowSpriteInfo( l_sScrollbarSprite );

	l_sScrollbarSprite[0].wFrameNo = 4;
	l_sScrollbarSprite[1].wFrameNo = 5;
	m_uiScrollbar.SetBottomArrowSpriteInfo( l_sScrollbarSprite );
}

void
CGUIMessagebox::SetStyle( WORD pi_wStyle )
{	
	m_wDlgStyleID = pi_wStyle;

	RemoveAll();

	Add( &m_cBase );

	Add( &m_cUpperOutLine );
	Add( &m_cLowerOutLine );
	Add( &m_cLeftOutLine );
	Add( &m_cRightOutLine );

//	Add( &m_uiCloseButton );
	
	if( m_wDlgStyleID == DLG_STYLE_OK_CANCEL )
	{
		// YES NO 		
		Add( &m_uiOKButton );
		Add( &m_uiCancelButton );		
	}
	else if( m_wDlgStyleID == DLG_STYLE_OK )
	{
		// only ok button		
		Add( &m_uiOKButton );		
	}

	Add( &m_uiMsgBoard );	

	Show( IsShow() );
}

void
CGUIMessagebox::SetAlpha( BYTE pi_byAlpha )
{
	m_cBase.SetARGB( (pi_byAlpha<<24) | 0x00ffffff );
}

void
CGUIMessagebox::SetBound( RECT & pi_rtBound )
{
	CGUIObject::SetBound( pi_rtBound );

	// base size	
	m_cBase.SetSize( m_ptSize.x, m_ptSize.y - m_uiOKButton.m_ptSize.y / 2 );

	// outline size
	m_cUpperOutLine.SetSize( m_cBase.m_ptSize.x - OUTLINE_MARGIN * 2, OUTLINE_SIZE );
	m_cLowerOutLine.SetSize( m_cBase.m_ptSize.x - OUTLINE_MARGIN * 2, OUTLINE_SIZE );
	m_cLeftOutLine.SetSize( OUTLINE_SIZE, m_cBase.m_ptSize.y - OUTLINE_MARGIN * 2 );
	m_cRightOutLine.SetSize( OUTLINE_SIZE, m_cBase.m_ptSize.y - OUTLINE_MARGIN * 2 );


	// set position
	if( IsAddedObject( &m_uiScrollbar ) )
		m_uiMsgBoard.SetPos( m_ptPos.x + ( m_ptSize.x - m_uiMsgBoard.m_ptSize.x - m_uiScrollbar.m_ptSize.x ) / 2,
							 m_ptPos.y + ( m_cBase.m_ptSize.y - m_uiMsgBoard.m_ptSize.y ) / 2 );
	else
		m_uiMsgBoard.SetPos( m_ptPos.x + ( m_ptSize.x - m_uiMsgBoard.m_ptSize.x ) / 2,
							 m_ptPos.y + ( m_cBase.m_ptSize.y - m_uiMsgBoard.m_ptSize.y ) / 2 );

	m_uiScrollbar.SetPos( m_ptPos.x + m_ptSize.x - m_uiScrollbar.m_ptSize.x - MSG_MARGIN, m_uiMsgBoard.m_ptPos.y );


	m_cUpperOutLine.SetPos( m_ptPos.x + OUTLINE_MARGIN, m_ptPos.y + OUTLINE_MARGIN );
	m_cLowerOutLine.SetPos( m_ptPos.x + OUTLINE_MARGIN, m_ptPos.y + m_cBase.m_ptSize.y - OUTLINE_MARGIN - OUTLINE_SIZE );
	m_cLeftOutLine.SetPos( m_ptPos.x + OUTLINE_MARGIN, m_ptPos.y + OUTLINE_MARGIN );
	m_cRightOutLine.SetPos( m_ptPos.x + m_ptSize.x - OUTLINE_SIZE - OUTLINE_MARGIN, m_ptPos.y + OUTLINE_MARGIN );

	if( m_wDlgStyleID == DLG_STYLE_OK_CANCEL )
	{
		m_uiOKButton.SetPos( m_ptPos.x + m_ptSize.x - m_uiOKButton.m_ptSize.x * 2,
							 m_ptPos.y + m_ptSize.y - m_uiOKButton.m_ptSize.y - 5 );
		m_uiCancelButton.SetPos( m_ptPos.x + m_ptSize.x - m_uiOKButton.m_ptSize.x,
								 m_ptPos.y + m_ptSize.y - m_uiOKButton.m_ptSize.y - 5 );
	}
	else if( m_wDlgStyleID == DLG_STYLE_OK )
	{
		m_uiOKButton.SetPos( m_ptPos.x + m_ptSize.x - m_uiOKButton.m_ptSize.x,
							 m_ptPos.y + m_ptSize.y - m_uiOKButton.m_ptSize.y - 5 );
	}
}

void
CGUIMessagebox::SetMsg( char * pi_pMsg )
{
	WORD	l_wMaxCharNum = 0;
	WORD	l_wCharNum = 0;		
	WORD	l_wLineNum = 0;

	for( int i=0; pi_pMsg[i] != '\0'; ++i )
	{		
		if( pi_pMsg[i] == '\n' )
		{
			++l_wLineNum;			

			if( l_wCharNum > l_wMaxCharNum )
			{
				l_wMaxCharNum = l_wCharNum;
			}
			
			l_wCharNum = 0;
		}
		else 
		{
			++l_wCharNum;
		}
	}

	if( l_wCharNum > 0 )
	{
		++l_wLineNum;		
		
		if( l_wCharNum > l_wMaxCharNum )
		{
			l_wMaxCharNum = l_wCharNum;
		}
	}

	RECT	l_rtBound;	

	// set m_uiMsgBoard size
	if( m_bScrollable )
	{
		l_rtBound.left = m_uiMsgBoard.m_ptPos.x;
		l_rtBound.top = m_uiMsgBoard.m_ptPos.y;

		if( m_uiMsgBoard.m_ptSize.x > 0 )		
		{
			l_rtBound.right = m_uiMsgBoard.m_ptPos.x + m_uiMsgBoard.m_ptSize.x;
		}
		else
		{
			int l_nSizeX = l_wMaxCharNum * ONEBYTE_FONTSIZE + m_uiMsgBoard.GetMarginX() * 2;
			if( l_nSizeX > DEFAULT_SCROLL_BOARD_SIZE_X )
				l_rtBound.right = m_uiMsgBoard.m_ptPos.x + DEFAULT_SCROLL_BOARD_SIZE_X;
			else
				l_rtBound.right = m_uiMsgBoard.m_ptPos.x + l_nSizeX;
		}


		int l_nSizeY = l_wLineNum * (TWOBYTE_FONTSIZE+m_uiMsgBoard.GetColumnGap()) + m_uiMsgBoard.GetMarginY() * 2;

		if( m_uiMsgBoard.m_ptSize.y > 0 )
		{
			l_rtBound.bottom = m_uiMsgBoard.m_ptPos.y + m_uiMsgBoard.m_ptSize.y;
		}
		else
		{
			
			if( l_nSizeY > DEFAULT_SCROLL_BOARD_SIZE_Y )
				l_rtBound.bottom = m_uiMsgBoard.m_ptPos.y + DEFAULT_SCROLL_BOARD_SIZE_Y;
			else
				l_rtBound.bottom = m_uiMsgBoard.m_ptPos.y + l_nSizeY;
		}

		m_uiMsgBoard.SetBound( l_rtBound );

		// scroll이 가능할 때만 scorllbar를 보여준다.
		if( l_rtBound.bottom - l_rtBound.top < l_nSizeY )
		{
			m_uiScrollbar.Show( IsShow() );
			Add( &m_uiScrollbar );			

			// set scrollbar size
			l_rtBound.left = m_uiScrollbar.m_ptPos.x;
			l_rtBound.top = m_uiScrollbar.m_ptPos.y;
			l_rtBound.right = m_uiScrollbar.m_ptPos.x + m_uiScrollbar.m_ptSize.x;
			l_rtBound.bottom = m_uiScrollbar.m_ptPos.y + m_uiMsgBoard.m_ptSize.y;

			m_uiScrollbar.SetBound( l_rtBound );
			
			m_uiMsgBoard.AddScrollbar( &m_uiScrollbar );
		}
	}
	else
	{
		l_rtBound.left = m_uiMsgBoard.m_ptPos.x;
		l_rtBound.top = m_uiMsgBoard.m_ptPos.y;
		l_rtBound.right = m_uiMsgBoard.m_ptPos.x + l_wMaxCharNum * ONEBYTE_FONTSIZE + m_uiMsgBoard.GetMarginX() * 2;
		l_rtBound.bottom = m_uiMsgBoard.m_ptPos.y + 
						l_wLineNum * ( TWOBYTE_FONTSIZE + m_uiMsgBoard.GetColumnGap() ) + m_uiMsgBoard.GetMarginY() * 2;

		m_uiMsgBoard.SetBound( l_rtBound );
	}	

	l_rtBound.left = m_ptPos.x;
	l_rtBound.top = m_ptPos.y;
	if( IsAddedObject( &m_uiScrollbar ) )
	{
		l_rtBound.right = m_ptPos.x + m_uiMsgBoard.m_ptSize.x + m_uiScrollbar.m_ptSize.x + MSG_MARGIN * 4;
		l_rtBound.bottom = m_ptPos.y + m_uiMsgBoard.m_ptSize.y + MSG_MARGIN * 3 + m_uiOKButton.m_ptSize.y;
	}
	else
	{
		l_rtBound.right = m_ptPos.x + m_uiMsgBoard.m_ptSize.x + MSG_MARGIN * 2;
		l_rtBound.bottom = m_ptPos.y + m_uiMsgBoard.m_ptSize.y + MSG_MARGIN * 2 + m_uiOKButton.m_ptSize.y;
	}	

	// msgbox 최소 크기 검사
	if( l_rtBound.right - l_rtBound.left < 200 )
		l_rtBound.right = l_rtBound.left + 200;
	if( l_rtBound.bottom - l_rtBound.top < 100 )
		l_rtBound.bottom = l_rtBound.top + 100;

	SetBound( l_rtBound );

	m_uiMsgBoard.Clear();
	m_uiMsgBoard.InsertText( pi_pMsg );
}

/*
void
CGUIMessagebox::SetMsg( char * pi_pMsg )
{
	ClearMsg();	

	if( pi_pMsg == NULL )
	{
		// set messagebox size
		RECT l_rtBound;

		l_rtBound.left = m_ptPos.x;		
		l_rtBound.right = m_cUpperBorder.m_ptSize.x;
		l_rtBound.top = m_ptPos.y;
		l_rtBound.bottom = m_ptPos.y + m_cUpperBorder.m_ptSize.y + m_cLowerBorder.m_ptSize.y + 20;			

		SetBound( l_rtBound );
		return;
	}

	// 한줄에 최대 문자 갯수
	WORD l_wLimitLineCharNum = 60;

	WORD	l_wCharNum = 0;
	WORD	l_wLineFirstChar = 0;
	WORD	l_wMaxCharNum = 0;

	for( int i=0; pi_pMsg[i] != '\0'; ++i )
	{
		if( l_wCharNum > l_wLimitLineCharNum )
		{
			DWORD l_wLineCharNum = l_wCharNum - 1;

			if( pi_pMsg[i - 2] < 0x00 || pi_pMsg[i - 2] > 0xff )
			{				
				for( int j=i-3; j>=0; --j )
				{
					if( !( pi_pMsg[j]  < 0x00 || pi_pMsg[j]  > 0xff ) )
					{
						// 한글이 짤리므로 char를 하나 내려준다.
						if( ( i - 2 - j ) % 2 != 0 )
							--l_wLineCharNum;
						break;
					}
				}

				if( pi_pMsg[j] < 0 )
				{
					if(  (i-1) % 2 != 0 )
						--l_wLineCharNum;
				}
			}

			// copy line
			char * l_pStr = new char[l_wLineCharNum + 1];
			strncpy( l_pStr, pi_pMsg + l_wLineFirstChar, l_wLineCharNum );
			l_pStr[l_wLineCharNum] = NULL;

			m_ListStr.insert( m_ListStr.end(), l_pStr );

			// next line
			l_wLineFirstChar += l_wLineCharNum;
			
			l_wCharNum = l_wCharNum - l_wLineCharNum;		
		}		
		
		if( pi_pMsg[i] == '\n' )
		{
			// copy line
			char * l_pStr = new char[l_wCharNum + 1];
			strncpy( l_pStr, pi_pMsg + l_wLineFirstChar, l_wCharNum );
			l_pStr[l_wCharNum] = NULL;

			m_ListStr.insert( m_ListStr.end(), l_pStr );

			// next line
			l_wLineFirstChar += l_wCharNum + 1;

			if( l_wCharNum > l_wMaxCharNum )
			{
				l_wMaxCharNum = l_wCharNum;
			}
			
			l_wCharNum = 0;
		}
		else 
		{
			++l_wCharNum;
		}
	}

	if( l_wCharNum > 0 )
	{
		// copy line
		char * l_pStr = new char[l_wCharNum + 1];
		strncpy( l_pStr, pi_pMsg + l_wLineFirstChar, l_wCharNum );
		l_pStr[l_wCharNum] = NULL;

		m_ListStr.insert( m_ListStr.end(), l_pStr );
		
		if( l_wCharNum > l_wMaxCharNum )
		{
			l_wMaxCharNum = l_wCharNum;
		}
	}	

	// set messagebox size		
	RECT l_rtBound;

	l_rtBound.left = m_ptPos.x;		

	l_rtBound.right = m_ptPos.x + l_wMaxCharNum * ONEBYTE_FONTSIZE + 30;
	if( l_rtBound.right < m_cUpperBorder.m_ptSize.x )
		l_rtBound.right = m_cUpperBorder.m_ptSize.x;

	l_rtBound.top = m_ptPos.y;

	l_rtBound.bottom = m_ptPos.y + m_ListStr.size() * ( TWOBYTE_FONTSIZE + MSG_BOX_STR_V_GAP ) + 60;		

	SetBound( l_rtBound );
}
*/

void
CGUIMessagebox::Open( void )
{
	CGUIDialogBox::Open();

	m_dwOpenTime = timeGetTime();

	if( GetStyleID() != DLG_STYLE_NONE )
		PlayWave( SND_ID_WINDOW_OPEN );
}

void
CGUIMessagebox::Update( void )
{
	if( m_bOpened )
	{
		if( m_dwDuration == 0xffffffff )
			return;

		if( timeGetTime() - m_dwOpenTime > m_dwDuration )
			SendDialogBoxEvent( DLG_BOX_CLOSED );
	}	
}



////////////////////////////////////////////////////////////////////////////
//
// CGUIMessageWin Class Implementation
//
////////////////////////////////////////////////////////////////////////////

CGUIMessageWin::CGUIMessageWin()
{
	SetName( "msgwin" );
}

CGUIMessageWin::~CGUIMessageWin()
{

}

void
CGUIMessageWin::Init( void )
{
	m_uiOKButton.SetSize( 42, 39 );
	
	POINT l_ptSize;
	l_ptSize.x = 15;	l_ptSize.y = 10;
	m_uiScrollbar.SetTopArrowSize( l_ptSize );
	m_uiScrollbar.SetBottomArrowSize( l_ptSize );	
	l_ptSize.y = 18;
	m_uiScrollbar.SetBarSize( l_ptSize );	
	l_ptSize.x = 15;
	m_uiScrollbar.SetSize( l_ptSize );

	Add( &m_cBase );

	Add( &m_cUpperOutLine );
	Add( &m_cLowerOutLine );
	Add( &m_cLeftOutLine );
	Add( &m_cRightOutLine );	

	Add( &m_uiMsgBoard );	

	Add( &m_uiOKButton );	

	m_uiOKButton.AddMouseEventHandler( this );

	SetSprite();

	m_cUpperOutLine.SetARGB( D3DCOLOR_XRGB( 149, 147, 193 ) );
	m_cLowerOutLine.SetARGB( m_cUpperOutLine.GetARGB() );
	m_cLeftOutLine.SetARGB( m_cUpperOutLine.GetARGB() );
	m_cRightOutLine.SetARGB( m_cUpperOutLine.GetARGB() );		
	
	SetMoveObject( NULL ); // clear movable object
	SetMoveObject( &m_cBase );
	SetMoveObject( &m_uiMsgBoard );
	SetMovable( TRUE );
	
	m_uiMsgBoard.SetMargin( 0, 0 );
	m_uiMsgBoard.SetColumnGap( 4 );
}

void
CGUIMessageWin::SetSprite( void )
{
	SPRITE_INFO l_spriteInfo;
	l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );	

	// button
	l_spriteInfo.wActionNo = 0;
	l_spriteInfo.wFrameNo = 0;
	m_uiOKButton.SetSpriteInfoEx( &l_spriteInfo );
	l_spriteInfo.wFrameNo = 1;
	m_uiOKButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
	l_spriteInfo.wFrameNo = 2;
	m_uiOKButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );

	l_spriteInfo.wActionNo = 1;	

	// base
	l_spriteInfo.wFrameNo = 0;
	m_cBase.SetSpriteInfo( &l_spriteInfo );

	// scrollbar
	SPRITE_INFO l_sScrollbarSprite[2];
	l_sScrollbarSprite[0].pSprite = l_spriteInfo.pSprite;
	l_sScrollbarSprite[0].wActionNo = 4;
	memcpy( &l_sScrollbarSprite[1], &l_sScrollbarSprite[0], sizeof( SPRITE_INFO ) );
	
	l_sScrollbarSprite[0].wFrameNo = 0;
	m_uiScrollbar.SetSpriteInfo( &l_sScrollbarSprite[0] );
	l_sScrollbarSprite[0].wFrameNo = 1;
	m_uiScrollbar.SetBarSpriteInfo( &l_sScrollbarSprite[0] );

	l_sScrollbarSprite[0].wFrameNo = 2;
	l_sScrollbarSprite[1].wFrameNo = 3;
	m_uiScrollbar.SetTopArrowSpriteInfo( l_sScrollbarSprite );

	l_sScrollbarSprite[0].wFrameNo = 4;
	l_sScrollbarSprite[1].wFrameNo = 5;
	m_uiScrollbar.SetBottomArrowSpriteInfo( l_sScrollbarSprite );
}

void
CGUIMessageWin::SetAlpha( BYTE pi_byAlpha )
{
	m_cBase.SetARGB( (pi_byAlpha<<24) | 0x00ffffff );
}
	
void
CGUIMessageWin::SetMsg( char * pi_pMsg )
{
	WORD	l_wMaxCharNum = 0;
	WORD	l_wCharNum = 0;		
	WORD	l_wLineNum = 0;

	for( int i=0; pi_pMsg[i] != '\0'; ++i )
	{		
		if( pi_pMsg[i] == '\n' )
		{
			++l_wLineNum;			

			if( l_wCharNum > l_wMaxCharNum )
			{
				l_wMaxCharNum = l_wCharNum;
			}
			
			l_wCharNum = 0;
		}
		else 
		{
			++l_wCharNum;
		}
	}

	if( l_wCharNum > 0 )
	{
		++l_wLineNum;		
		
		if( l_wCharNum > l_wMaxCharNum )
		{
			l_wMaxCharNum = l_wCharNum;
		}
	}

	RECT	l_rtBound;		

	// ------------------------------------------------------------
	// set m_uiMsgBoard size
	l_rtBound.left = m_uiMsgBoard.m_ptPos.x;
	l_rtBound.top = m_uiMsgBoard.m_ptPos.y;

	// bottom
	int l_nSizeY = l_wLineNum * ( TWOBYTE_FONTSIZE + m_uiMsgBoard.GetColumnGap() );

	if( m_uiMsgBoard.m_ptSize.y == 0 ||
		l_nSizeY < m_uiMsgBoard.m_ptSize.y )
	{
		int l_nMaxSizeY = gScreen.m_ptSize.y * 0.7;
		l_nMaxSizeY -= l_nMaxSizeY % (TWOBYTE_FONTSIZE + m_uiMsgBoard.GetColumnGap());

		if( l_nSizeY > l_nMaxSizeY )
			l_rtBound.bottom = m_uiMsgBoard.m_ptPos.y + l_nMaxSizeY;
		else
			l_rtBound.bottom = m_uiMsgBoard.m_ptPos.y + l_nSizeY;
	}
	else if( l_nSizeY > m_uiMsgBoard.m_ptSize.y &&
			 m_uiMsgBoard.m_ptSize.y < m_uiScrollbar.GetMinSize() )
	{
		l_rtBound.bottom = m_uiMsgBoard.m_ptPos.y + m_uiScrollbar.GetMinSize();
	}
	else
	{
		l_rtBound.bottom = m_uiMsgBoard.m_ptPos.y + m_uiMsgBoard.m_ptSize.y;
	}

	// right
	int l_nSizeX = l_wMaxCharNum * ONEBYTE_FONTSIZE + m_uiMsgBoard.GetMarginX() * 2;
	if( m_uiMsgBoard.m_ptSize.x == 0 ||
		l_rtBound.bottom - l_rtBound.top < m_uiScrollbar.GetMinSize() )
	{
		if( l_nSizeX > gScreen.m_ptSize.x * 0.5 )
			l_rtBound.right = m_uiMsgBoard.m_ptPos.x + gScreen.m_ptSize.x * 0.5;
		else
			l_rtBound.right = m_uiMsgBoard.m_ptPos.x + l_nSizeX;
	}
	else
	{
		l_rtBound.right = m_uiMsgBoard.m_ptPos.x + m_uiMsgBoard.m_ptSize.x;
	}	

	m_uiMsgBoard.SetBound( l_rtBound );	

	// ------------------------------------------------------------
	// scroll이 가능할 때만 scorllbar를 보여준다.
	if( ( m_uiMsgBoard.m_ptSize.y < l_nSizeY ||
		  m_uiMsgBoard.m_ptSize.x < l_nSizeX ) &&		  
		m_uiMsgBoard.m_ptSize.y >= m_uiScrollbar.GetMinSize() )		
	{
		m_uiScrollbar.Show( IsShow() );
		Add( &m_uiScrollbar );			

		// set scrollbar size
		l_rtBound.left = m_uiScrollbar.m_ptPos.x;
		l_rtBound.top = m_uiScrollbar.m_ptPos.y;
		l_rtBound.right = m_uiScrollbar.m_ptPos.x + m_uiScrollbar.m_ptSize.x;
		l_rtBound.bottom = m_uiScrollbar.m_ptPos.y + m_uiMsgBoard.m_ptSize.y;

		m_uiScrollbar.SetBound( l_rtBound );
		
		m_uiMsgBoard.AddScrollbar( &m_uiScrollbar );
	}	
	else
	{
		Remove( &m_uiScrollbar );
	}


	// ------------------------------------------------------------
	// window size
	l_rtBound.left = m_ptPos.x;
	l_rtBound.top = m_ptPos.y;
	if( IsAddedObject( &m_uiScrollbar ) )
	{
		l_rtBound.right = m_ptPos.x + m_uiMsgBoard.m_ptSize.x + m_uiScrollbar.m_ptSize.x + MSG_MARGIN * 2;
		l_rtBound.bottom = m_ptPos.y + m_uiMsgBoard.m_ptSize.y + MSG_MARGIN * 3 + m_uiOKButton.m_ptSize.y;
	}
	else
	{
		l_rtBound.right = m_ptPos.x + m_uiMsgBoard.m_ptSize.x + MSG_MARGIN * 2;
		l_rtBound.bottom = m_ptPos.y + m_uiMsgBoard.m_ptSize.y + MSG_MARGIN * 2 + m_uiOKButton.m_ptSize.y;
	}		

	// msgbox 최소 크기 검사
	if( l_rtBound.right - l_rtBound.left < 200 )
		l_rtBound.right = l_rtBound.left + 200;
	if( l_rtBound.bottom - l_rtBound.top < 100 )
		l_rtBound.bottom = l_rtBound.top + 100;

	SetBound( l_rtBound );

	// ------------------------------------------------------------
	m_uiMsgBoard.Clear();
	m_uiMsgBoard.InsertText( pi_pMsg );	
}

void
CGUIMessageWin::SetBound( RECT & pi_rtBound )
{
	CGUIObject::SetBound( pi_rtBound );

	// base size	
	m_cBase.SetSize( m_ptSize.x, m_ptSize.y - m_uiOKButton.m_ptSize.y / 2 );

	// outline size
	m_cUpperOutLine.SetSize( m_cBase.m_ptSize.x - OUTLINE_MARGIN * 2, OUTLINE_SIZE );
	m_cLowerOutLine.SetSize( m_cBase.m_ptSize.x - OUTLINE_MARGIN * 2, OUTLINE_SIZE );
	m_cLeftOutLine.SetSize( OUTLINE_SIZE, m_cBase.m_ptSize.y - OUTLINE_MARGIN * 2 );
	m_cRightOutLine.SetSize( OUTLINE_SIZE, m_cBase.m_ptSize.y - OUTLINE_MARGIN * 2 );


	// set position
	if( IsAddedObject( &m_uiScrollbar ) )
		m_uiMsgBoard.SetPos( m_ptPos.x + ( m_ptSize.x - m_uiMsgBoard.m_ptSize.x - m_uiScrollbar.m_ptSize.x ) / 2,
							 m_ptPos.y + ( m_cBase.m_ptSize.y - m_uiMsgBoard.m_ptSize.y ) / 2 );
	else
		m_uiMsgBoard.SetPos( m_ptPos.x + ( m_ptSize.x - m_uiMsgBoard.m_ptSize.x ) / 2,
							 m_ptPos.y + ( m_cBase.m_ptSize.y - m_uiMsgBoard.m_ptSize.y ) / 2 );

	m_uiScrollbar.SetPos( m_ptPos.x + m_ptSize.x - m_uiScrollbar.m_ptSize.x - MSG_MARGIN, m_uiMsgBoard.m_ptPos.y );


	m_cUpperOutLine.SetPos( m_ptPos.x + OUTLINE_MARGIN, m_ptPos.y + OUTLINE_MARGIN );
	m_cLowerOutLine.SetPos( m_ptPos.x + OUTLINE_MARGIN, m_ptPos.y + m_cBase.m_ptSize.y - OUTLINE_MARGIN - OUTLINE_SIZE );
	m_cLeftOutLine.SetPos( m_ptPos.x + OUTLINE_MARGIN, m_ptPos.y + OUTLINE_MARGIN );
	m_cRightOutLine.SetPos( m_ptPos.x + m_ptSize.x - OUTLINE_SIZE - OUTLINE_MARGIN, m_ptPos.y + OUTLINE_MARGIN );

	m_uiOKButton.SetPos( m_ptPos.x + m_ptSize.x - m_uiOKButton.m_ptSize.x,
						 m_ptPos.y + m_ptSize.y - m_uiOKButton.m_ptSize.y - 5 );
}

void
CGUIMessageWin::MouseEvent( _mouse_event & event )
{
	CGUIWindow::MouseEvent( event );

	if( event.id == MOUSE_CLICKED )
	{
		if( event.source == &m_uiOKButton )		
		{
			Close();
			gScreen.Remove( this );
		}
	}
}