////////////////////////////////////////////////////////////////////////////
//
// CGUIOreProcWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////
#include "GUIOreProcWindow.h"
#include "../../GUIController/GUIMessageBox.h"
#include "../../SpriteMgr.h"
#include "../../UIGlobal.h"
#include "../../../DefineMessage.h"

#define	RESULT_ORE_MAX_NUM			396

// ----------------------------------------------------------------------------

#define	RESULT_H_SOCKET_NUM			5
#define	RESULT_V_SOCKET_NUM			4

#define	RESULT_TOTAL_V_SOCKET_NUM	8

#define MSG_BOX_ARGB				0xf0ffffff

////////////////////////////////////////////////////////////////////////////////

CGUIOreProcWindow::CGUIOreProcWindow()
{
	m_wOutputViewedIndex	= 0;
	m_byOutputOreNum		= 0;
	
	m_bIsCreated			= FALSE;
}

CGUIOreProcWindow::~CGUIOreProcWindow()
{	
	
}

void
CGUIOreProcWindow::Init( void )
{
	POINT l_ptSize;
	POINT l_ptPos;
	int i, j;	

	// --------------------------------------------------------
	// size
	SetSize( 228, 395 );

	m_uiAniTitleBar.SetSize( 228, 28 );		

	m_uiInputOreItem.SetSize( 64, 64 );

	// result ore socket
	for( i=0; i<MAX_RESULT_ORE_SOCKET; ++i )
		m_uiOutputOreSocket[i].SetSize( 34, 34 );

	for( i=0; i<MAX_RESULT_ORE_ITEM; ++i )
		m_uiOutputOreItem[i].SetSize( 32, 32 );

	// result scrollbar	
	l_ptSize.x = 15; 
	l_ptSize.y = 10; 
	m_uiOutputScrollBar.SetTopArrowSize( l_ptSize );
	m_uiOutputScrollBar.SetBottomArrowSize( l_ptSize );
	l_ptSize.y = 18;
	m_uiOutputScrollBar.SetBarSize( l_ptSize );	
	l_ptSize.y = 188;
	m_uiOutputScrollBar.SetSize( l_ptSize );	

	// button
	m_uiProcButton.SetSize( 77, 25 );
	m_uiDisposalButton.SetSize( 77, 25 );
	m_uiCloseButton.SetSize( 16, 13 );
	m_uiHelpButton.SetSize( 16, 13 );	

	m_uiTitle.SetString( "가 공" );
	char buf[32];
	sprintf( buf, "가공비용               %s", _GetAvatarMoneyName() );
	m_uiDalant.SetString( buf );

	m_uiProcButton.SetLabel( "가  공" );
	m_uiDisposalButton.SetLabel( "처  분" );

	// --------------------------------------------------------
	// pos
	m_uiTitle.SetPos( m_ptPos.x + 6, m_ptPos.y + 7 );

	m_uiDalant.SetPos( m_ptPos.x + 26, m_ptPos.y + 112 );
	m_uiProcPrice.SetPos( m_ptPos.x + 136, m_ptPos.y + 112 );
		
	m_uiInputOreItem.SetPos( m_ptPos.x + 81, m_ptPos.y + 34 );

	// result ore socket
	int index = 0;
	for( i=0; i<RESULT_V_SOCKET_NUM; ++i )
	{
		for( j=0; j<RESULT_H_SOCKET_NUM; ++j )
		{
			l_ptPos.x = m_ptPos.x + 21 + j * ( 4 + m_uiOutputOreSocket[0].m_ptSize.x );
			l_ptPos.y = m_ptPos.y + 205 + i * ( 4 + m_uiOutputOreSocket[0].m_ptSize.y );
			m_uiOutputOreSocket[index].SetPos( l_ptPos );
			++index;
		}
	}
	

	// scrollbar
	m_uiOutputScrollBar.SetPos( m_ptPos.x + 207, m_ptPos.y + 197 );

	// button
	m_uiProcButton.SetPos( m_ptPos.x + 75, m_ptPos.y + 130 );	
	m_uiDisposalButton.SetPos( m_ptPos.x + 75, m_ptPos.y + 362 );
	m_uiCloseButton.SetPos( m_ptPos.x + 211, m_ptPos.y + 4 );
	m_uiHelpButton.SetPos( m_ptPos.x + 194, m_ptPos.y + 4 );

	// --------------------------------------------------------
	// add controller	
	Add( &m_uiAniTitleBar );	
	Add( &m_uiTitle );
	Add( &m_uiDalant );
	Add( &m_uiProcPrice );

	m_uiInputOreItem.AddMouseEventHandler( this );
	Add( &m_uiInputOreItem );

	for( i=0; i<MAX_RESULT_ORE_SOCKET; ++i )
	{
		m_uiOutputOreSocket[i].AddMouseEventHandler( this );
		Add( &m_uiOutputOreSocket[i] );
	}

	for( i=0; i<MAX_RESULT_ORE_ITEM; ++i )
		m_uiOutputOreItem[i].AddMouseEventHandler( this );

	AddMouseEventHandler( this );

	m_uiOutputScrollBar.AddScrollHandler( this );
//	Add( &m_uiOutputScrollBar );	
		
	m_uiCloseButton.AddMouseEventHandler( this );
	m_uiHelpButton.AddMouseEventHandler( this );

	m_uiProcButton.AddMouseEventHandler( this );
	m_uiDisposalButton.AddMouseEventHandler( this );
	
	Add( &m_uiProcButton );	
	Add( &m_uiDisposalButton );
	Add( &m_uiCloseButton );
	Add( &m_uiHelpButton );

	// --------------------------------------------------------
	SetSprite();

	SetARGB( 0xCCFFFFFF );

	m_uiTitle.GetFont()->dwColor = D3DCOLOR_XRGB( 195, 193, 190 );
	m_uiDalant.GetFont()->dwColor = D3DCOLOR_XRGB( 212, 186, 148 );
	m_uiProcPrice.GetFont()->dwColor = D3DCOLOR_XRGB( 195, 193, 190 );

	m_uiProcButton.SetLabelStyle( R3_HAN_OUTLINE );
	m_uiDisposalButton.SetLabelStyle( R3_HAN_OUTLINE );

	m_uiAniTitleBar.SetTotalFrameNum( 18 );
	m_uiAniTitleBar.SetObjectNumInLine( 1 );
	m_uiAniTitleBar.SetFPS( 10 );
	m_uiAniTitleBar.SetStopInterval( 1000 );	

	// scrollbar setting
	m_uiOutputScrollBar.SetAdjustmentUnit( 35 );	

	SetMoveObject( this );	
	SetMoveObject( &m_uiAniTitleBar );
	SetMoveObject( &m_uiTitle );
	SetMovable( TRUE );

	SetName( "OreProcWin" );
	

	EnableDisposal( FALSE );
	EnableProc( FALSE );

	m_bIsCreated = TRUE;
}

void
CGUIOreProcWindow::SetSprite( void )
{
	if( !m_bIsCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_ORE_PROC );

		l_spriteInfo.wActionNo = 0;

		// background
		l_spriteInfo.wFrameNo = 0;
		SetSpriteInfo( &l_spriteInfo );		

		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );

		l_spriteInfo.wActionNo = 0;		
		l_spriteInfo.wFrameNo = 12;
		m_uiDisposalButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 13;
		m_uiDisposalButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 14;
		m_uiDisposalButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 15;
		m_uiDisposalButton.SetSpriteInfoEx( DISABLED, &l_spriteInfo );

		l_spriteInfo.wFrameNo = 12;
		m_uiProcButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 13;
		m_uiProcButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 14;
		m_uiProcButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 15;
		m_uiProcButton.SetSpriteInfoEx( DISABLED, &l_spriteInfo );


		// help button		
		l_spriteInfo.wActionNo = 5;
		l_spriteInfo.wFrameNo = 0;
		m_uiHelpButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 1;
		m_uiHelpButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 2;
		m_uiHelpButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		
		// scrollbar
		SPRITE_INFO l_sScrollbarSprite[2];
		l_sScrollbarSprite[0].pSprite = l_spriteInfo.pSprite;
		l_sScrollbarSprite[0].wActionNo = 4;
		memcpy( &l_sScrollbarSprite[1], &l_sScrollbarSprite[0], sizeof( SPRITE_INFO ) );
		
		l_sScrollbarSprite[0].wFrameNo = 0;
		m_uiOutputScrollBar.SetSpriteInfo( &l_sScrollbarSprite[0] );
		l_sScrollbarSprite[0].wFrameNo = 1;
		m_uiOutputScrollBar.SetBarSpriteInfo( &l_sScrollbarSprite[0] );

		l_sScrollbarSprite[0].wFrameNo = 2;
		l_sScrollbarSprite[1].wFrameNo = 3;
		m_uiOutputScrollBar.SetTopArrowSpriteInfo( l_sScrollbarSprite );

		l_sScrollbarSprite[0].wFrameNo = 4;
		l_sScrollbarSprite[1].wFrameNo = 5;
		m_uiOutputScrollBar.SetBottomArrowSpriteInfo( l_sScrollbarSprite );	

		// ani title bar	
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 5;
		m_uiAniTitleBar.SetSpriteInfo( &l_spriteInfo );	

		// close button	
		m_uiCloseButton.SetSpriteInfoEx( gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON1 ) );
		m_uiCloseButton.SetSpriteInfoEx( HOVERED, gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON2 ) );
		m_uiCloseButton.SetSpriteInfoEx( PRESSED, gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON3 ) );	
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_ORE_PROC );	
		if( !l_pSprite )
			return;	

		GetSpriteInfo()->pSprite = l_pSprite;		

		// ani title bar	
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_INVENTORY );
		m_uiAniTitleBar.GetSpriteInfo()->pSprite = l_pSprite;

		
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );

		m_uiProcButton.SetSprite( l_pSprite );
		m_uiDisposalButton.SetSprite( l_pSprite );

		// scrollbar				 
		m_uiOutputScrollBar.SetSprite( l_pSprite );

		// close button	
		m_uiCloseButton.SetSprite( l_pSprite );

		// help button
		m_uiHelpButton.SetSprite( l_pSprite );

		l_pSprite = gSpriteMgr.GetSprite( SP_ID_ITEM );

		// input ore
		if( m_uiInputOreItem.GetSpriteInfo()->pSprite != NULL )
			m_uiInputOreItem.GetSpriteInfo()->pSprite = l_pSprite;

		// output ore
		for( int i=0; i<MAX_RESULT_ORE_ITEM; ++i )
		{
			if( m_uiOutputOreItem[i].GetSpriteInfo()->pSprite != NULL )
				m_uiOutputOreItem[i].GetSpriteInfo()->pSprite = l_pSprite;
		}		
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CGUIOreProcWindow::TakeInInputOre( DWORD pi_dwItemID, DWORD pi_dwSpriteID, BYTE pi_byQuantity )
{
	m_uiInputOreItem.SetItemID( pi_dwItemID );
	m_uiInputOreItem.SetSpriteID( pi_dwSpriteID );
	m_uiInputOreItem.SetNum( pi_byQuantity );
	m_uiInputOreItem.SetSprite();

	EnableProc( TRUE );

	return TRUE;
}

BOOL
CGUIOreProcWindow::TakeOutInputOre( void )
{	
	if( m_uiInputOreItem.IsEmpty() )
		return FALSE;
	
	// set empty
	m_uiInputOreItem.SetEmpty();	

	EnableProc( FALSE );

	return TRUE;
}

void
CGUIOreProcWindow::SetProcPrice( DWORD pi_dwPrice )
{
	char buf[32];
	_GetPriceStr( pi_dwPrice, buf );	

	m_uiProcPrice.SetString( buf );

	// repos
	m_uiProcPrice.SetPos( m_uiDalant.m_ptPos.x + m_uiDalant.m_ptSize.x - m_uiProcPrice.m_ptSize.x - 57,
						  m_uiDalant.m_ptPos.y );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CGUIOreProcWindow::TakeInResultOre( BYTE pi_bySocketIndex, DWORD pi_dwItemID, DWORD pi_dwSpriteID, BYTE pi_byNum, BOOL pi_bResizeScrollbar )
{
	// item setting
	m_uiOutputOreItem[pi_bySocketIndex].SetItemID( pi_dwItemID );
	m_uiOutputOreItem[pi_bySocketIndex].SetSpriteID( pi_dwSpriteID );	
	m_uiOutputOreItem[pi_bySocketIndex].SetNum( pi_byNum );
	m_uiOutputOreItem[pi_bySocketIndex].SetSprite();

	++m_byOutputOreNum;

	if( m_byOutputOreNum == 1 )
		EnableDisposal( TRUE );
	

	// set scrollbar
	if( pi_bResizeScrollbar )
	{
		// 뒤에서부터 아이템이 들어간 마지막 socket을 찾는다.
		for( int i=MAX_RESULT_ORE_ITEM-1 ; i>=0 ; --i )
		{
			if( m_uiOutputOreItem[i].GetItemID() != NONE_ITEM_ID )			
				break;			
		}

		WORD l_wVerticalSocketLine = i / RESULT_H_SOCKET_NUM + 1;

		if( l_wVerticalSocketLine > RESULT_V_SOCKET_NUM )
		{
			if( m_uiOutputScrollBar.GetTotalAmount() == 0 )
			{
				m_uiOutputScrollBar.SetPos( m_ptPos.x + 207, m_ptPos.y + 197 );
				m_uiOutputScrollBar.Show( IsShow() );
				Add( &m_uiOutputScrollBar );
			}

			m_uiOutputScrollBar.SetTotalAmount( m_uiOutputScrollBar.GetAdjustmentUnit() *
												( l_wVerticalSocketLine - RESULT_V_SOCKET_NUM ) );		
			m_uiOutputScrollBar.SetCurrentAmount( m_uiOutputScrollBar.GetTotalAmount() );			
		}
		else
		{
			if( m_uiOutputScrollBar.GetTotalAmount() > 0 )
				Remove( &m_uiOutputScrollBar );

			m_uiOutputScrollBar.SetTotalAmount( 0 );
			m_uiOutputScrollBar.SetCurrentAmount( 0 );
		}		

		WORD l_wVeiwedIndex = m_uiOutputScrollBar.GetCurrentAmount() / m_uiOutputScrollBar.GetAdjustmentUnit();

		if( l_wVeiwedIndex > RESULT_TOTAL_V_SOCKET_NUM - RESULT_V_SOCKET_NUM )
			l_wVeiwedIndex = RESULT_TOTAL_V_SOCKET_NUM - RESULT_V_SOCKET_NUM;

		if( l_wVeiwedIndex != m_wOutputViewedIndex )
		{
			m_wOutputViewedIndex = l_wVeiwedIndex;
			UpdateResultSocket();

			return TRUE;
		}
	}

	if( pi_bySocketIndex >= m_wOutputViewedIndex * RESULT_H_SOCKET_NUM &&
		pi_bySocketIndex < m_wOutputViewedIndex * RESULT_H_SOCKET_NUM + RESULT_H_SOCKET_NUM * RESULT_V_SOCKET_NUM )
	{
		WORD l_wSocket;
		l_wSocket = pi_bySocketIndex - m_wOutputViewedIndex * RESULT_H_SOCKET_NUM;
		
		// add item
		m_uiOutputOreSocket[l_wSocket].SetItem( &m_uiOutputOreItem[pi_bySocketIndex] );
	}

	return TRUE;	
}

BOOL
CGUIOreProcWindow::TakeOutResultOre( BYTE pi_bySocketIndex, BYTE pi_byNum )
{
	if( m_uiOutputOreItem[pi_bySocketIndex].IsEmpty() )
		return FALSE;		

	// set empty
	if( !pi_byNum || m_uiOutputOreItem[pi_bySocketIndex].GetNum() <= pi_byNum )		
	{
		// set empty socket
		if( pi_bySocketIndex >= m_wOutputViewedIndex * RESULT_H_SOCKET_NUM &&		
			pi_bySocketIndex < m_wOutputViewedIndex * RESULT_H_SOCKET_NUM + RESULT_H_SOCKET_NUM * RESULT_V_SOCKET_NUM )
		{
			BYTE l_bySocket;
			l_bySocket = pi_bySocketIndex - m_wOutputViewedIndex * RESULT_H_SOCKET_NUM;			
			
			m_uiOutputOreSocket[l_bySocket].SetEmpty();
		}

		// set empty item
		m_uiOutputOreItem[pi_bySocketIndex].SetEmpty();

		--m_byOutputOreNum;

		if( m_byOutputOreNum == 0 )
			EnableDisposal( FALSE );		
	}
	else
	{
		m_uiOutputOreItem[pi_bySocketIndex].SetNum( m_uiOutputOreItem[pi_bySocketIndex].GetNum() - pi_byNum );
	}	

	return TRUE;
}

void
CGUIOreProcWindow::ClearResultOreSocket( void )
{
	// remove all ore item
	for( int i=0; i<MAX_RESULT_ORE_ITEM; ++i )
	{
		m_uiOutputOreItem[i].SetEmpty();
	}

	// init
	m_wOutputViewedIndex = 0;	
	m_byOutputOreNum = 0;

	m_uiOutputScrollBar.SetAmount( 0, 0 );
	Remove( &m_uiOutputScrollBar );
	
	// update socket
	UpdateResultSocket();

	EnableDisposal( FALSE );
}

BOOL	
CGUIOreProcWindow::TakeInItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex, DWORD pi_dwItemID, DWORD pi_dwSpriteID, BYTE pi_byNum, BYTE pi_byUsableLevel )
{
	return TakeInResultOre( pi_bySocketIndex, pi_dwItemID, pi_dwSpriteID, pi_byNum, TRUE );
}

BOOL
CGUIOreProcWindow::TakeOutItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex, BYTE pi_byNum, CGUIItem * po_pOutItem )
{
	return TakeOutResultOre( pi_bySocketIndex, pi_byNum );
}
	
CGUIItem *
CGUIOreProcWindow::GetItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex )
{
	if( pi_bySocketIndex >= MAX_RESULT_ORE_ITEM )
		return NULL;

	if( m_uiOutputOreItem[pi_bySocketIndex].IsEmpty() )
		return NULL;

	return &m_uiOutputOreItem[pi_bySocketIndex];
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CGUIOreProcWindow::EnableDisposal( BOOL pi_bEnable )
{
	if( pi_bEnable )
	{		
		m_uiDisposalButton.Enable();
	}
	else
	{	
		m_uiDisposalButton.Disable();
	}	
}

void
CGUIOreProcWindow::EnableProc( BOOL pi_bEnable )
{
	if( pi_bEnable )
	{	
		m_uiProcButton.Enable();
	}
	else
	{	
		m_uiProcButton.Disable();
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CGUIOreProcWindow::MouseEvent( _mouse_event & event )
{
	CGUIWindow::MouseEvent( event );	

	if( event.id == MOUSE_STROKED )
	{
		if( gScreen.ExistObject() )
		{
			if( event.source == &m_uiTitle ||
				event.source == &m_uiAniTitleBar ||				
				event.source == &m_uiProcButton ||
				event.source == &m_uiDisposalButton ||
				event.source == &m_uiCloseButton )
				return;

			
			// add item
			PostUIMessage( WM_BOARD_ADD_OBJECT, 0 );			
		}
		else
		{
			BYTE byEventSocketIndex;

			// 가공할 원석
			if( event.source == &m_uiInputOreItem )			
			{
				if( m_uiInputOreItem.IsEmpty() )
					return;

				byEventSocketIndex = 0xFF;
			}
			// 가공된 잉갓
			else
			{				
				for( int s = 0; s < MAX_RESULT_ORE_SOCKET; ++s )
				{
					if( event.source == &m_uiOutputOreSocket[s] )
						break;									
				}

				if( s >= MAX_RESULT_ORE_SOCKET )
					return;

				if( m_uiOutputOreSocket[s].IsEmpty() )
					return;

				byEventSocketIndex = m_wOutputViewedIndex * RESULT_H_SOCKET_NUM + s;
			}


			SetQuickEvent();

			if( g_bIsQuickEvent_Trade )
			{
				PostUIMessage( WM_BOARD_TRADE_HOT_KEY, UI_MOUSE_MSG( 0, byEventSocketIndex, 0, 0 ) );
			}
			else
			{
				PostUIMessage( WM_BOARD_REMOVE_OBJECT, UI_MOUSE_MSG( 0, byEventSocketIndex, 0, 0 ) );
			}			
			
		}			

	}
	else if ( event.id == MOUSE_HOVERED )
	{
		if( event.source == &m_uiInputOreItem )		
		{
			if( m_uiInputOreItem.IsEmpty() )
				return;

			// tooltip
			PostUIMessage( WM_BOARD_HOVER_OBJECT, UI_MOUSE_MSG( 0, 0xFF, 0, 0 ) );
			
			return;
		}

		int i = m_wOutputViewedIndex * RESULT_H_SOCKET_NUM;
		for( ; i<MAX_RESULT_ORE_SOCKET; ++i )
		{
			if( event.source == &m_uiOutputOreSocket[i] )
			{							
				// tool tip			
				PostUIMessage( WM_BOARD_HOVER_OBJECT, UI_MOUSE_MSG( 0, i, 0, 0 ) );
				
				return;
			}
		}
	}
	else if( event.id == MOUSE_CLICKED )
	{
		if( gScreen.ExistObject() )
			return;
		
		if( event.source == &m_uiProcButton )
		{
			// 가공
			PostUIMessage( WM_ORE_PROC_BOARD_PROCESS, 0 );
		}		
		else if( event.source == &m_uiDisposalButton )
		{
			// 처분
			PostUIMessage( WM_ORE_PROC_BOARD_SELL, 0 );			
		}
		else if( event.source == &m_uiCloseButton )
		{
			Close();
		}
		else if( event.source == &m_uiHelpButton )
		{			
			PostUIMessage( WM_OPEN_WINDOW_HELP, 0 );			
		}
	}
}

void
CGUIOreProcWindow::ScrollEvent( _scroll_event & event )
{
	WORD l_wVeiwedIndex = m_uiOutputScrollBar.GetCurrentAmount() / m_uiOutputScrollBar.GetAdjustmentUnit();

	if( l_wVeiwedIndex > RESULT_TOTAL_V_SOCKET_NUM - RESULT_V_SOCKET_NUM )
		l_wVeiwedIndex = RESULT_TOTAL_V_SOCKET_NUM - RESULT_V_SOCKET_NUM;

	if( l_wVeiwedIndex != m_wOutputViewedIndex )
	{
		m_wOutputViewedIndex = l_wVeiwedIndex;
		UpdateResultSocket();
	}
}

void
CGUIOreProcWindow::ClosingDialogBox( CGUIDialogBox * pi_pDlg )
{
	if( pi_pDlg->GetUserDefinedID() == MSGBOX_ID_CLOSE )	
	{
		if( pi_pDlg->GetResultMsg() == MSG_OK )
		{
			// 가공
			PostUIMessage( WM_ORE_PROC_BOARD_PROCESS, 0 );
		}
		else if( pi_pDlg->GetResultMsg() == MSG_CANCEL )
		{			
			Close();
		}
	}	

	CGUIWindow::ClosingDialogBox( pi_pDlg );	
}

BOOL
CGUIOreProcWindow::Draw( void )
{
	if( !CGUIContainer::Draw() )
		return FALSE;

	if( m_byOutputOreNum > 0 )
	{		
		DrawR3Hangul( m_ptPos.x + 22, m_ptPos.y + 167,
					  "가공결과입니다.", D3DCOLOR_XRGB( 232, 232, 232 ) );
		DrawR3Hangul( m_ptPos.x + 22, m_ptPos.y + 182,
					  "인벤토리에 넣어주세요.", D3DCOLOR_XRGB( 232, 232, 232 ) );
	}

	DrawDialogBox();

	return TRUE;
}

void
CGUIOreProcWindow::UpdateResultSocket( void )
{
	WORD l_wItemIndex;
	l_wItemIndex = m_wOutputViewedIndex * RESULT_H_SOCKET_NUM;

	for( int s=0; s<MAX_RESULT_ORE_SOCKET; ++s )
	{
		m_uiOutputOreSocket[s].SetEmpty();

		if( !m_uiOutputOreItem[l_wItemIndex+s].IsEmpty() )
		{
			// add item
			m_uiOutputOreSocket[s].SetItem( &m_uiOutputOreItem[l_wItemIndex+s] );
		}
	}	
}
