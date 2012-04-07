////////////////////////////////////////////////////////////////////////////
//
// CGUITradeWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////

#include "GUITradeWindow.h"
#include "../../SpriteMgr.h"
#include "../../UIGlobal.h"
#include "../../GUIController/GUIScreen.h"
#include "../GUIInventoryWindow.h"
#include "../GUIItemBeltWindow.h"
#include "../../GUIController/GUIMessageBox.h"

#define	SOCKET_V_NUM	3	// 소켓 세로 갯수
#define	SOCKET_H_NUM	5	// 소켓 가로 갯수

// ------------------------------------------------------------------------------

void	EP_OpenTradeWin( void );
void	EP_CloseTradeWin( void );


//------------------------------------------------------------------------------

class CGUIInventoryWindow;
extern CGUIInventoryWindow * g_pUiInventWindow;

CGUITradeWindow::CGUITradeWindow()
{
	m_bIsCreated	= FALSE;
}

CGUITradeWindow::~CGUITradeWindow()
{

}

void
CGUITradeWindow::Init( void )
{
	SetSize( 228, 571 );

	m_uiAniTitleBar.SetSize( 228, 28 );
	m_uiCloseButton.SetSize( 16, 13 );
	
	m_uiAvatarLockButton.SetSize( 77, 25 );
	m_uiAvatarAcceptButton.SetSize( 77, 25 );

	m_uiAvatarDalantInput.SetSize( 119, 12 );
	m_uiAvatarGoldInput.SetSize( 119, 12 );

	m_uiOtherLockButton.SetSize( 77, 25 );
	m_uiOtherAcceptButton.SetSize( 77, 25 );

	m_uiTitle.SetString( "교 환" );
	m_uiAvatarDalantStr.SetString( _GetAvatarMoneyName() );
	m_uiAvatarGoldStr.SetString  ( "골  드" );
	m_uiOtherDalantStr.SetString( _GetAvatarMoneyName() );
	m_uiOtherGoldStr.SetString  ( "골  드" );

	m_uiAvatarAcceptButton.SetLabel( "승 인" );
	m_uiOtherAcceptButton.SetLabel( "승 인" );

	int i, j;

	for( i=0; i<MAX_TRADE_ITEM_NUM; ++i )
	{
		m_uiAvatarUselessSocket[i].SetSize( 34, 34 );
		m_uiOtherUselessSocket[i].SetSize( 34, 34 );
	}

	// ------------------------------------------------------------	

	m_uiTitle.SetPos( m_ptPos.x + 6, m_ptPos.y + 7 );
	m_uiAniTitleBar.SetPos( m_ptPos.x, m_ptPos.y );
	m_uiCloseButton.SetPos(  m_ptPos.x + 211, m_ptPos.y + 2 );	

	
	for( i=0; i<SOCKET_V_NUM; ++i )
	{
		for( j=0; j<SOCKET_H_NUM; ++j )
		{
			m_uiAvatarItem[5 * i + j].SetPos( m_ptPos.x + 22 + ( 32 + 6 ) * j,
											  m_ptPos.y + 339 + ( 32 + 6 ) * i );

			m_uiAvatarUselessSocket[5 * i + j].SetPos( m_uiAvatarItem[5 * i + j].m_ptPos.x - 1,
													   m_uiAvatarItem[5 * i + j].m_ptPos.y - 1 );

			m_uiOtherItem[5 * i + j].SetPos( m_ptPos.x + 22 + ( 32 + 6 ) * j,
											 m_ptPos.y + 63 + ( 32 + 6 ) * i );

			m_uiOtherUselessSocket[5 * i + j].SetPos( m_uiOtherItem[5 * i + j].m_ptPos.x - 1,
													  m_uiOtherItem[5 * i + j].m_ptPos.y - 1 );													  
		}
	}

	m_uiAvatarName.SetPos( m_ptPos.x + 65, m_ptPos.y + 313 );
	m_uiAvatarDalantInput.SetPos( m_ptPos.x + 21, m_ptPos.y + 464 );	
	m_uiAvatarGoldInput.SetPos( m_ptPos.x + 21, m_ptPos.y + 493 );
	m_uiAvatarLockButton.SetPos( m_ptPos.x + 19, m_ptPos.y + 534 );
	m_uiAvatarAcceptButton.SetPos( m_ptPos.x + 132, m_ptPos.y + 534 );		


	m_uiOtherName.SetPos( m_ptPos.x + 65, m_ptPos.y + 37 );
	m_uiOtherDalant.SetPos( m_ptPos.x + 150, m_ptPos.y + 188 );
	m_uiOtherGold.SetPos( m_ptPos.x + 150, m_ptPos.y + 217 );
	m_uiOtherLockButton.SetPos( m_ptPos.x + 19, m_ptPos.y + 257 );
	m_uiOtherAcceptButton.SetPos( m_ptPos.x + 132, m_ptPos.y + 257 );

	m_uiOtherDalantStr.SetPos( m_ptPos.x + 162, m_ptPos.y + 187 );
	m_uiOtherGoldStr.SetPos( m_ptPos.x + 162, m_ptPos.y + 216 );
	m_uiAvatarDalantStr.SetPos( m_ptPos.x + 162, m_ptPos.y + 464 );
	m_uiAvatarGoldStr.SetPos( m_ptPos.x + 162, m_ptPos.y + 492 );

	// ------------------------------------------------------------	

	Add( &m_uiAniTitleBar );
	Add( &m_uiTitle );

	m_uiCloseButton.AddMouseEventHandler( this );
	Add( &m_uiCloseButton );
	
	AddMouseEventHandler( this );

	for( i=0; i<MAX_TRADE_ITEM_NUM; ++i )
	{
		m_uiAvatarItem[i].AddMouseEventHandler( this );		
		m_uiOtherItem[i].AddMouseEventHandler( this );

		Add( &m_uiAvatarUselessSocket[i]);
		Add( &m_uiAvatarItem[i] );

		Add( &m_uiOtherUselessSocket[i] );
		Add( &m_uiOtherItem[i] );
	}

	m_uiAvatarLockButton.AddMouseEventHandler( this );
	m_uiAvatarAcceptButton.AddMouseEventHandler( this );	

	m_uiAvatarDalantInput.AddMouseEventHandler( this );	
	m_uiAvatarGoldInput.AddMouseEventHandler( this );	
	m_uiAvatarDalantInput.AddKeyEventHandler( this );
	m_uiAvatarGoldInput.AddKeyEventHandler( this );

	Add( &m_uiAvatarName );
	Add( &m_uiAvatarDalantInput );
	Add( &m_uiAvatarGoldInput );
	Add( &m_uiAvatarLockButton );
	Add( &m_uiAvatarAcceptButton );
	
	Add( &m_uiOtherName );
	Add( &m_uiOtherDalant );
	Add( &m_uiOtherGold );
	Add( &m_uiOtherLockButton );
	Add( &m_uiOtherAcceptButton );

	Add( &m_uiAvatarDalantStr );
	Add( &m_uiAvatarGoldStr );
	Add( &m_uiOtherDalantStr );
	Add( &m_uiOtherGoldStr );

	// ------------------------------------------------------------
	SetSprite();	

	SetMoveObject( &m_uiAniTitleBar );	
	SetMoveObject( &m_uiTitle );		
	SetMovable( TRUE );

	m_uiAniTitleBar.SetTotalFrameNum( 18 );
	m_uiAniTitleBar.SetObjectNumInLine( 1 );
	m_uiAniTitleBar.SetFPS( 10 );
	m_uiAniTitleBar.SetStopInterval( 1000 );

	m_uiAvatarDalantInput.SetAlignment( CGUIInput::Right_Alignment );
	m_uiAvatarGoldInput.SetAlignment( CGUIInput::Right_Alignment );
	m_uiAvatarDalantInput.SetMargin( 2, 0 );
	m_uiAvatarGoldInput.SetMargin( 2, 0 );
	m_uiAvatarDalantInput.SetInputType( CGUIInput::Only_Number );
	m_uiAvatarGoldInput.SetInputType( CGUIInput::Only_Number );

	// input에만 포커스가 가도록 한다.
	DisableFocus();
	m_uiAvatarDalantInput.EnableFocus();
	m_uiAvatarGoldInput.EnableFocus();
	
	
	// --------------------------------------------------------
	SetARGB( 0xCCFFFFFF );	

	m_uiTitle.GetFont()->dwColor = D3DCOLOR_XRGB( 232, 232, 232 );

	m_uiAvatarDalantStr.GetFont()->dwColor = D3DCOLOR_XRGB( 232, 232, 232 );
	m_uiAvatarGoldStr.GetFont()->dwColor = D3DCOLOR_XRGB( 232, 232, 232 );
	m_uiOtherDalantStr.GetFont()->dwColor = D3DCOLOR_XRGB( 232, 232, 232 );
	m_uiOtherGoldStr.GetFont()->dwColor = D3DCOLOR_XRGB( 232, 232, 232 );

	m_uiAvatarName.GetFont()->dwColor = D3DCOLOR_XRGB( 232, 232, 232 );
	m_uiOtherName.GetFont()->dwColor = D3DCOLOR_XRGB( 232, 232, 232 );

	m_uiAvatarAcceptButton.SetLabelColor( D3DCOLOR_XRGB( 232, 232, 232 ) );
	m_uiOtherAcceptButton.SetLabelColor( D3DCOLOR_XRGB( 232, 232, 232 ) );

	m_uiAvatarAcceptButton.SetLabelStyle( R3_HAN_OUTLINE );
	m_uiOtherAcceptButton.SetLabelStyle( R3_HAN_OUTLINE );

	// --------------------------------------------------------		

	SetLock_Avatar( FALSE );
	SetAccept_Avatar( FALSE );
	SetLock_Other( FALSE );
	SetAccept_Other( FALSE );

	m_uiOtherLockButton.Disable();
	m_uiOtherAcceptButton.Disable();
	

	SetAvatarDalant( 0 );
	SetAvatarGold( 0 );

	SetOtherDalant( 0 );
	SetOtherGold( 0 );	

	m_bIsCreated = TRUE;	
}

void
CGUITradeWindow::SetSprite( void )
{
	if( !m_bIsCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_TRADE );

		l_spriteInfo.wActionNo = 0;
		
		l_spriteInfo.wFrameNo = 0;	
		SetSpriteInfo( &l_spriteInfo );		

		// avatar lock button
		l_spriteInfo.wFrameNo = 1;
		m_uiAvatarLockButton.SetSpriteInfoEx( &l_spriteInfo );		
		l_spriteInfo.wFrameNo = 2;
		m_uiAvatarLockButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );				

		// other lock button
		l_spriteInfo.wFrameNo = 1;
		m_uiOtherLockButton.SetSpriteInfoEx( &l_spriteInfo );		
		l_spriteInfo.wFrameNo = 2;
		m_uiOtherLockButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		

		// avatar accept button
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 12;
		m_uiAvatarAcceptButton.SetSpriteInfoEx( &l_spriteInfo );		
		l_spriteInfo.wFrameNo = 14;
		m_uiAvatarAcceptButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );						

		// other accpet button
		l_spriteInfo.wFrameNo = 12;
		m_uiOtherAcceptButton.SetSpriteInfoEx( &l_spriteInfo );		
		l_spriteInfo.wFrameNo = 14;
		m_uiOtherAcceptButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );		
		

		// ani title bar	
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 5;
		m_uiAniTitleBar.SetSpriteInfo( &l_spriteInfo );

		m_uiCloseButton.SetSpriteInfoEx( gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON1 ) );
		m_uiCloseButton.SetSpriteInfoEx( HOVERED, gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON2 ) );
		m_uiCloseButton.SetSpriteInfoEx( PRESSED, gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON3 ) );

		l_spriteInfo.pSprite = NULL;
		l_spriteInfo.wActionNo = 1;
		l_spriteInfo.wFrameNo = 4;
		for( int i=0; i<MAX_TRADE_ITEM_NUM; ++i )
		{
			m_uiAvatarUselessSocket[i].SetSpriteInfo( &l_spriteInfo );
			m_uiOtherUselessSocket[i].SetSpriteInfo( &l_spriteInfo );
		}
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_TRADE );

		if( !l_pSprite )
			return;
		
		// background
		GetSpriteInfo()->pSprite = l_pSprite;		

		
		m_uiAvatarLockButton.SetSprite( l_pSprite );
		m_uiOtherLockButton.SetSprite( l_pSprite );		
		

		// ani title bar	
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );		
		m_uiAniTitleBar.GetSpriteInfo()->pSprite = l_pSprite;


		l_pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );		

		m_uiAvatarAcceptButton.SetSprite( l_pSprite );
		m_uiOtherAcceptButton.SetSprite( l_pSprite );

		m_uiCloseButton.SetSprite( l_pSprite );		

		// item 
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_ITEM );
		for( int i=0; i<MAX_TRADE_ITEM_NUM; ++i )
		{
			if( m_uiAvatarItem[i].GetSpriteInfo()->pSprite != NULL )
				m_uiAvatarItem[i].GetSpriteInfo()->pSprite = l_pSprite;

			if( m_uiOtherItem[i].GetSpriteInfo()->pSprite != NULL )
				m_uiOtherItem[i].GetSpriteInfo()->pSprite = l_pSprite;

			if( m_uiAvatarUselessSocket[i].GetSpriteInfo()->pSprite != NULL )
				m_uiAvatarUselessSocket[i].GetSpriteInfo()->pSprite = l_pSprite;

			if( m_uiOtherUselessSocket[i].GetSpriteInfo()->pSprite != NULL )
				m_uiOtherUselessSocket[i].GetSpriteInfo()->pSprite = l_pSprite;			
		}
	}

}


void
CGUITradeWindow::Open( void )
{	
	CGUIWindow::Open();

	// 거래창에 focus가 가게 하기 위해 
	// 채팅창 always active를 false로 만든다.
	//EP_OpenDialogBox( this, NULL );
	EP_OpenTradeWin();
}

void
CGUITradeWindow::Close( void )
{
	CGUIWindow::Close();

	// 채팅창을 원래대로 always active를 true로 한다.
	//EP_CloseDialogBox( NULL );
	EP_CloseTradeWin();
}

// ==============================================================================

void	
CGUITradeWindow::SetAvatarName( char * pi_pName )
{
	m_uiAvatarName.SetString( pi_pName );

	// repos
	m_uiAvatarName.SetPos( m_ptPos.x + ( m_ptSize.x - m_uiAvatarName.m_ptSize.x )/2,
						   m_uiAvatarName.m_ptPos.y );
}

void
CGUITradeWindow::SetEnableSocketNum_Avatar( BYTE pi_byNum )
{
	BYTE l_byUsableSocketNum;

	if( pi_byNum >= MAX_TRADE_ITEM_NUM )
		l_byUsableSocketNum = MAX_TRADE_ITEM_NUM;
	else
		l_byUsableSocketNum = pi_byNum;

	// set usable socket
	for( int i=0; i<l_byUsableSocketNum; ++i )
	{
		m_uiAvatarUselessSocket[i].GetSpriteInfo()->pSprite = NULL;
		m_uiAvatarUselessSocket[i].SetARGB( 0xFFFFFFFF );
	}

	// set useless socket
	CSprite * l_pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
	for( i=l_byUsableSocketNum; i<MAX_TRADE_ITEM_NUM; ++i )
	{
		m_uiAvatarUselessSocket[i].GetSpriteInfo()->pSprite = l_pSprite;
		m_uiAvatarUselessSocket[i].SetARGB( 0xA0FFFFFF );
	}
}

// ========================================================================================

BOOL	
CGUITradeWindow::TakeInItem_Avatar( BYTE pi_bySocketIndex, DWORD pi_dwItemID, DWORD pi_dwSpriteID, BYTE pi_byQuantity )
{	
	m_uiAvatarItem[pi_bySocketIndex].SetItemID( pi_dwItemID );
	m_uiAvatarItem[pi_bySocketIndex].SetSpriteID( pi_dwSpriteID );
	m_uiAvatarItem[pi_bySocketIndex].SetNum( pi_byQuantity );
	m_uiAvatarItem[pi_bySocketIndex].SetSprite();

	return TRUE;
}

BOOL
CGUITradeWindow::TakeOutItem_Avatar( BYTE pi_bySocketIndex )
{
	if( m_uiAvatarItem[pi_bySocketIndex].IsEmpty() )
		return FALSE;	

	// set empty
	m_uiAvatarItem[pi_bySocketIndex].SetEmpty();	

	return TRUE;
}

// ========================================================================================

void
CGUITradeWindow::SetAvatarDalant( DWORD pi_dwDalant )
{
	m_dwAvatarDalant = pi_dwDalant;

	char buf[32];
	if( m_dwAvatarDalant == 0 )
		buf[0] = '\0';
	else	
		_GetPriceStr( m_dwAvatarDalant, buf );	

	// set dalant
	m_uiAvatarDalantInput.SetText( buf );
	
	// caret을 오른쪽 끝으로 보낸다.
	m_uiAvatarDalantInput.SetCaretPos( strlen(m_uiAvatarDalantInput.GetText()) );
}

void
CGUITradeWindow::SetAvatarGold( DWORD pi_dwGold )
{
	m_dwAvatarGold = pi_dwGold;

	char buf[32];
	if( m_dwAvatarGold == 0 )
		buf[0] = '\0';
	else
		_GetPriceStr( m_dwAvatarGold, buf );	
	
	// set gold
	m_uiAvatarGoldInput.SetText( buf );		

	// caret을 오른쪽 끝으로 보낸다.
	m_uiAvatarGoldInput.SetCaretPos( strlen(m_uiAvatarGoldInput.GetText()) );
}

// ========================================================================================

void
CGUITradeWindow::SetLock_Avatar( BOOL pi_bLock )
{
	SPRITE_INFO		l_spriteInfo;
	memcpy( &l_spriteInfo, m_uiAvatarLockButton.GetSpriteInfo(), sizeof( SPRITE_INFO ) );

	// 잠금
	if( pi_bLock )
	{
		l_spriteInfo.wFrameNo = 1;
		m_uiAvatarLockButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 2;
		m_uiAvatarLockButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		
		// 승인버튼 enable
		memcpy( &l_spriteInfo, m_uiAvatarAcceptButton.GetSpriteInfo(), sizeof( SPRITE_INFO ) );
		l_spriteInfo.wFrameNo = 12;
		m_uiAvatarAcceptButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 14;
		m_uiAvatarAcceptButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );

		m_uiAvatarAcceptButton.Enable();

		// 잠금이면 더이상 입력을 못하게 한다.
		m_uiAvatarDalantInput.DisableFocus();
		m_uiAvatarGoldInput.DisableFocus();
		
	}
	// 잠금 해제
	else
	{
		l_spriteInfo.wFrameNo = 3;
		m_uiAvatarLockButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 4;
		m_uiAvatarLockButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		
	
		// 승인버튼 disable
		memcpy( &l_spriteInfo, m_uiAvatarAcceptButton.GetSpriteInfo(), sizeof( SPRITE_INFO ) );
		l_spriteInfo.wFrameNo = 15;
		m_uiAvatarAcceptButton.SetSpriteInfoEx( &l_spriteInfo );

		m_uiAvatarAcceptButton.Disable();


		// 입력 잠금 해제		
		m_uiAvatarDalantInput.EnableFocus();
		m_uiAvatarGoldInput.EnableFocus();		
	}	
}

void
CGUITradeWindow::SetAccept_Avatar( BOOL pi_bAccept )
{
	SPRITE_INFO		l_spriteInfo;
	memcpy( &l_spriteInfo, m_uiAvatarAcceptButton.GetSpriteInfo(), sizeof( SPRITE_INFO ) );

	if( pi_bAccept )
	{
		l_spriteInfo.wFrameNo = 20;
		m_uiAvatarAcceptButton.SetSpriteInfoEx( &l_spriteInfo );		

		m_uiAvatarAcceptButton.Disable();
	}
	else
	{
		l_spriteInfo.wFrameNo = 15;
		m_uiAvatarAcceptButton.SetSpriteInfoEx( &l_spriteInfo );		

		m_uiAvatarAcceptButton.Disable();
	}
}

// ========================================================================================

CGUIItem *	
CGUITradeWindow::GetItem_Avatar( BYTE pi_bySocketIndex )
{	
	if( m_uiAvatarItem[pi_bySocketIndex].IsEmpty() )
		return NULL;

	return &m_uiAvatarItem[pi_bySocketIndex];
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void	
CGUITradeWindow::SetOtherName( char * pi_pName )
{
	m_uiOtherName.SetString( pi_pName );

	// repos
	m_uiOtherName.SetPos( m_ptPos.x + ( m_ptSize.x - m_uiOtherName.m_ptSize.x )/2,
						   m_uiOtherName.m_ptPos.y );
}

void	
CGUITradeWindow::SetEnableSocketNum_Other( BYTE pi_byNum )
{
	BYTE l_byUsableSocketNum;

	if( pi_byNum >= MAX_TRADE_ITEM_NUM )
		l_byUsableSocketNum = MAX_TRADE_ITEM_NUM;
	else
		l_byUsableSocketNum = pi_byNum;

	// set usable socket
	for( int i=0; i<l_byUsableSocketNum; ++i )
	{
		m_uiOtherUselessSocket[i].GetSpriteInfo()->pSprite = NULL;
		m_uiOtherUselessSocket[i].SetARGB( 0xFFFFFFFF );
	}

	// set useless socket
	CSprite * l_pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
	for( i=l_byUsableSocketNum; i<MAX_TRADE_ITEM_NUM; ++i )
	{
		m_uiOtherUselessSocket[i].GetSpriteInfo()->pSprite = l_pSprite;
		m_uiOtherUselessSocket[i].SetARGB( 0xA0FFFFFF );
	}
}

// ========================================================================================

BOOL
CGUITradeWindow::TakeInItem_Other( BYTE pi_bySocketIndex, DWORD pi_dwItemID, DWORD pi_dwSpriteID, BYTE pi_byQuantity )
{
	if( !m_uiOtherItem[pi_bySocketIndex].IsEmpty() )
		return FALSE;

	m_uiOtherItem[pi_bySocketIndex].SetItemID( pi_dwItemID );
	m_uiOtherItem[pi_bySocketIndex].SetSpriteID( pi_dwSpriteID );
	m_uiOtherItem[pi_bySocketIndex].SetNum( pi_byQuantity );

	m_uiOtherItem[pi_bySocketIndex].SetSprite();

	return TRUE;
}
	
BOOL
CGUITradeWindow::TakeOutItem_Other( BYTE pi_bySocketIndex )
{
	if( m_uiOtherItem[pi_bySocketIndex].IsEmpty() )
		return FALSE;

	// set empty
	m_uiOtherItem[pi_bySocketIndex].SetEmpty();

	return TRUE;
}

CGUIItem *
CGUITradeWindow::GetItem_Other( BYTE pi_bySocketIndex )
{
	if( pi_bySocketIndex >= MAX_TRADE_ITEM_NUM )
		return NULL;

	return &m_uiOtherItem[pi_bySocketIndex];
}

// ========================================================================================

void
CGUITradeWindow::SetOtherDalant( DWORD pi_dwDalant )
{
	char buf[32];
	_GetPriceStr( pi_dwDalant, buf );	

	// set dalant
	m_uiOtherDalant.SetString( buf );

	// repos
	m_uiOtherDalant.SetPos( m_ptPos.x + 136 - m_uiOtherDalant.m_ptSize.x , m_uiOtherDalant.m_ptPos.y );
}

void
CGUITradeWindow::SetOtherGold( DWORD pi_dwGold )
{
	char buf[32];
	_GetPriceStr( pi_dwGold, buf );	
	
	// set gold
	m_uiOtherGold.SetString( buf );

	// repos
	m_uiOtherGold.SetPos( m_ptPos.x + 136 - m_uiOtherGold.m_ptSize.x , m_uiOtherGold.m_ptPos.y );	
}

// ========================================================================================

void
CGUITradeWindow::SetLock_Other( BOOL pi_bLock )
{
	m_bIsLock_Other = pi_bLock;

	SPRITE_INFO		l_spriteInfo;
	memcpy( &l_spriteInfo, m_uiOtherLockButton.GetSpriteInfo(), sizeof( SPRITE_INFO ) );

	if( m_bIsLock_Other )
		l_spriteInfo.wFrameNo = 1;
	else
		l_spriteInfo.wFrameNo = 3;

	m_uiOtherLockButton.SetSpriteInfoEx( &l_spriteInfo );
}

void
CGUITradeWindow::SetAccept_Other( BOOL pi_bAccept )
{
	m_bIsAccept_Other = pi_bAccept;

	SPRITE_INFO		l_spriteInfo;
	memcpy( &l_spriteInfo, m_uiOtherAcceptButton.GetSpriteInfo(), sizeof( SPRITE_INFO ) );

	if( m_bIsAccept_Other )
	{
		l_spriteInfo.wFrameNo = 20;
	}
	else
	{
		if( m_bIsLock_Other )
			l_spriteInfo.wFrameNo = 12;
		else
			l_spriteInfo.wFrameNo = 15;
	}

	m_uiOtherAcceptButton.SetSpriteInfoEx( &l_spriteInfo );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CGUITradeWindow::MouseEvent( _mouse_event & event )
{
	CGUIWindow::MouseEvent( event );

	if( event.id == MOUSE_STROKED )
	{
		if( gScreen.ExistObject() )
		{
			// add item
			PostUIMessage( WM_BOARD_ADD_OBJECT, 0 );
		}
		else
		{		
			// input focus setting
			if( event.source == &m_uiAvatarDalantInput )
			{			
				m_uiAvatarDalantInput.SetFocus();
				SetFocusObject( &m_uiAvatarDalantInput );				
				return;
			}
			else if( event.source == &m_uiAvatarGoldInput )
			{
				m_uiAvatarGoldInput.SetFocus();
				SetFocusObject( &m_uiAvatarGoldInput );				
				return;
			}
			

			// remove item
			for( int i=0; i<MAX_TRADE_ITEM_NUM; ++i )
			{
				if( event.source == &m_uiAvatarItem[i] )
				{		
					if( m_uiAvatarItem[i].IsEmpty() )
						return;

					SetQuickEvent();						

					if( g_bIsQuickEvent_Trade )
						PostUIMessage( WM_BOARD_TRADE_HOT_KEY, UI_MOUSE_MSG( 0, i, 0, 0 ) );
					else
						PostUIMessage( WM_BOARD_REMOVE_OBJECT, UI_MOUSE_MSG( 0, i, 0, 0 ) );

					return;
				}
			}			

		}
		
	}
	else if( event.id == MOUSE_CLICKED )
	{
		if( gScreen.ExistObject() )
			return;

		if( event.source == &m_uiAvatarLockButton )
		{
			// 잠금 설정			
			PostUIMessage( WM_PLAYER_TRADE_BOARD_LOCK, 0 );		
		}
		else if( event.source == &m_uiAvatarAcceptButton )
		{			
			// 승인
			PostUIMessage( WM_PLAYER_TRADE_BOARD_ACCEPT, 0 );
		}		
		else if( event.source == &m_uiCloseButton )
		{			
			PostUIMessage( WM_CLOSING_WINDOW, 0 );
		}		
	}
	else if( event.id == MOUSE_HOVERED )
	{
		// avatar item
		for( int i=0; i<MAX_TRADE_ITEM_NUM; ++i )
		{
			if( event.source == &m_uiAvatarItem[i] )
			{
				if( m_uiAvatarItem[i].IsEmpty() )
					return;

				PostUIMessage( WM_BOARD_HOVER_OBJECT, UI_MOUSE_MSG( 0, i, 0, 0 ) );
			}
		}

		// other player item
		for( i=0; i<MAX_TRADE_ITEM_NUM; ++i )
		{
			if( event.source == &m_uiOtherItem[i] )
			{
				if( m_uiOtherItem[i].IsEmpty() )
					return;

				PostUIMessage( WM_BOARD_HOVER_OBJECT, UI_MOUSE_MSG( 1, i, 0, 0 ) );
			}
		}
	}
}

void
CGUITradeWindow::KeyEvent( _key_event & event )
{
	if( event.key == VK_TAB )
	{
		if( event.source == &m_uiAvatarDalantInput )
		{
			m_uiAvatarGoldInput.SetFocus();
			SetFocusObject( &m_uiAvatarGoldInput );
		}
		else if( event.source == &m_uiAvatarGoldInput )
		{
			m_uiAvatarDalantInput.SetFocus();
			SetFocusObject( &m_uiAvatarDalantInput );
		}
	}	
}

LRESULT
CGUITradeWindow::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
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

		switch( uMsg )
		{	
		case WM_KEYDOWN:
		case WM_CHAR:
			{
				if( m_uiAvatarDalantInput.IsFocused() )
				{
					// input에서 dalant값을 걸러낸다
					m_dwAvatarDalant = _GetPriceNum( m_uiAvatarDalantInput.GetText() );

					SetAvatarDalant( m_dwAvatarDalant );
				}
				else if( m_uiAvatarGoldInput.IsFocused() )
				{
					m_dwAvatarGold = _GetPriceNum( m_uiAvatarGoldInput.GetText() );

					SetAvatarGold( m_dwAvatarGold );
				}			


				// 거래금액이 보유금액을 넘는지 체크한다.
				if( m_dwAvatarDalant > g_pUiInventWindow->GetOwnDalant() )
				{
					SetAvatarDalant( g_pUiInventWindow->GetOwnDalant() );					

					if( uMsg == WM_CHAR &&
						g_pUiInventWindow->GetOwnDalant()	== 0 )
					{
						char buf[32];
						sprintf( buf, "보유한 %s가 없습니다.", _GetAvatarMoneyName() );
						_GUIMessageBox( this, buf, 0xf0ffffff );
					}
				}
				else if( m_dwAvatarGold > g_pUiInventWindow->GetOwnGold() )
				{					
					SetAvatarGold( g_pUiInventWindow->GetOwnGold() );

					if( uMsg == WM_CHAR &&
						g_pUiInventWindow->GetOwnGold()	== 0 )
					{
						_GUIMessageBox( this, "보유한 골드가 없습니다", 0xf0ffffff );
					}
				}	
			}
			break;
		}
	}
	
	return 1;
}