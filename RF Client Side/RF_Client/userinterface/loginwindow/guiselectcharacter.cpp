#include "GUISelectCharacter.h"
#include "../GUIController/GUIMessageBox.h"
#include "../GUIController/GUIScreen.h"
#include "../SpriteMgr.h"
#include "../UIGlobal.h"


#define	DELETE_CHAR_BOX_ID	555


////////////////////////////////////////////////////////////////////////////
//
// CGUILoginWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////
void EventNextButton();
void EventPreviousButton();
void EventCreateCharacter();
void EventJoinGameFunc();
void EventDeleteCharacter();
void EventGameClose();


CGUISelectCharacter::CGUISelectCharacter()
{
	m_bIsCreated = FALSE;
}

CGUISelectCharacter::~CGUISelectCharacter()
{

}

void
CGUISelectCharacter::Init( void )
{		
	SetSize( gScreen.m_ptSize );


	// char info board
	m_cUpperBoard.SetSize( 588, 173 );
	m_cUpperBase.SetSize( 544, 94 );
	m_cUpperBoardAni.SetSize( 50, 50 );

	m_cLCharInfoBoard.SetSize( 161, 93 );
	m_cRCharInfoBoard.SetSize( 161, 93 );

	// menu board
	m_cLowerBoard.SetSize( 471, 159 );
	m_cLowerBase.SetSize( 167, 117 );
	m_cLowerBoardAni.SetSize( 92, 92 );

	m_cPrevButton.SetSize( 140, 36 );
	m_cNextButton.SetSize( 140, 36 );	

	m_cCreateButton.SetSize( 180, 44 );
	m_cDeleteButton.SetSize( 180, 44 );

	m_cConnectButton.SetSize( 64, 64 );
	m_cExitButton.SetSize( 98, 38 );


	m_cPrevButton.SetLabel( "이전 캐릭터" );
	m_cNextButton.SetLabel( "다음 캐릭터" );
	m_cCreateButton.SetLabel( "캐릭터 생성" );
	m_cDeleteButton.SetLabel( "캐릭터 삭제" );
	m_cExitButton.SetLabel( "종  료" );	

	// ----------------------------------------------------------------

	// char info board
	m_cUpperBoard.SetPos( ( m_ptSize.x - m_cUpperBoard.m_ptSize.x ) / 2, 10 );
	m_cUpperBase.SetPos( m_cUpperBoard.m_ptPos.x + (m_cUpperBoard.m_ptSize.x - m_cUpperBase.m_ptSize.x)/2,
						 m_cUpperBoard.m_ptPos.y );
	m_cUpperBoardAni.SetPos( m_cUpperBase.m_ptPos.x + 247, m_cUpperBase.m_ptPos.y + 20 );

	m_cLCharInfoBoard.SetPos( m_cUpperBase.m_ptPos.x + 139 - m_cLCharInfoBoard.m_ptSize.x,
							  m_cUpperBase.m_ptPos.y + 84 );
	m_cRCharInfoBoard.SetPos( m_cUpperBase.m_ptPos.x + 404, m_cUpperBase.m_ptPos.y + 84 );

//	m_cNameStr.SetPos( m_cLCharInfoBoard.m_ptPos.x + 15, m_cLCharInfoBoard.m_ptPos.y + 18 );
//	m_cLevelStr.SetPos( m_cLCharInfoBoard.m_ptPos.x + 15, m_cLCharInfoBoard.m_ptPos.y + 43 );
//	m_cAttributeStr.SetPos( m_cLCharInfoBoard.m_ptPos.x + 15, m_cLCharInfoBoard.m_ptPos.y + 68 );
	m_cNameStr.SetPos( m_cLCharInfoBoard.m_ptPos.x + 15, m_cLCharInfoBoard.m_ptPos.y + 25 );
	m_cLevelStr.SetPos( m_cLCharInfoBoard.m_ptPos.x + 15, m_cLCharInfoBoard.m_ptPos.y + 55 );
	m_cDalantStr.SetPos( m_cRCharInfoBoard.m_ptPos.x + 13, m_cRCharInfoBoard.m_ptPos.y + 25 );
	m_cGoldStr.SetPos( m_cRCharInfoBoard.m_ptPos.x + 13, m_cRCharInfoBoard.m_ptPos.y + 55 );

	// menu board
	m_cLowerBoard.SetPos( ( m_ptSize.x - m_cLowerBoard.m_ptSize.x ) / 2, 
						  m_ptSize.y - m_cLowerBoard.m_ptSize.y - 10 );
	m_cLowerBase.SetPos( m_cLowerBoard.m_ptPos.x + (m_cLowerBoard.m_ptSize.x - m_cLowerBase.m_ptSize.x)/2,
						 m_cLowerBoard.m_ptPos.y + 20 );
	m_cLowerBoardAni.SetPos( m_cLowerBase.m_ptPos.x + 37, m_cLowerBase.m_ptPos.y );

	m_cCreateButton.SetPos( m_cLowerBase.m_ptPos.x + 28 - m_cCreateButton.m_ptSize.x, 
						  m_cLowerBase.m_ptPos.y + 24 - m_cCreateButton.m_ptSize.y );
	m_cDeleteButton.SetPos( m_cLowerBase.m_ptPos.x + 137, m_cLowerBase.m_ptPos.y + 24 - m_cDeleteButton.m_ptSize.y );

	m_cPrevButton.SetPos( m_cLowerBase.m_ptPos.x + 16 - m_cPrevButton.m_ptSize.x,
						  m_cLowerBase.m_ptPos.y + 72 );
	m_cNextButton.SetPos( m_cLowerBase.m_ptPos.x + 149, m_cLowerBase.m_ptPos.y + 72 );

	m_cConnectButton.SetPos( m_cLowerBase.m_ptPos.x + 51, m_cLowerBase.m_ptPos.y + 13 );
	m_cExitButton.SetPos( m_cLowerBase.m_ptPos.x + 82 - m_cExitButton.m_ptSize.x / 2, m_cLowerBase.m_ptPos.y + 100 );

	m_cDeleteButton.SetLabelPos( 33, 18 );
	m_cCreateButton.SetLabelPos( 80, 18 );	
	m_cPrevButton.SetLabelPos( 36, 10 );
	m_cNextButton.SetLabelPos( 26, 10 );		

	// ----------------------------------------------------------------
	
	m_cCreateButton.AddMouseEventHandler( this );
	m_cDeleteButton.AddMouseEventHandler( this );	
	m_cNextButton.AddMouseEventHandler( this );
	m_cPrevButton.AddMouseEventHandler( this );	
	m_cConnectButton.AddMouseEventHandler( this );
	m_cExitButton.AddMouseEventHandler( this );

	m_cUpperBoard.Add( &m_cLCharInfoBoard );
	m_cUpperBoard.Add( &m_cRCharInfoBoard );
	m_cUpperBoard.Add( &m_cUpperBase );
	m_cUpperBoard.Add( &m_cUpperBoardAni );	
	m_cUpperBoard.Add( &m_cNameStr );
	m_cUpperBoard.Add( &m_cLevelStr );
//	m_cUpperBoard.Add( &m_cAttributeStr );
	m_cUpperBoard.Add( &m_cDalantStr );
	m_cUpperBoard.Add( &m_cGoldStr );
	Add( &m_cUpperBoard );
	
	m_cLowerBoard.Add( &m_cLowerBase );
	m_cLowerBoard.Add( &m_cLowerBoardAni );
	m_cLowerBoard.Add( &m_cCreateButton );
	m_cLowerBoard.Add( &m_cDeleteButton );
	m_cLowerBoard.Add( &m_cNextButton );
	m_cLowerBoard.Add( &m_cPrevButton );
	m_cLowerBoard.Add( &m_cConnectButton );
	m_cLowerBoard.Add( &m_cExitButton );
	Add( &m_cLowerBoard );

	// ----------------------------------------------------------------
	SetSprite();

	m_cLCharInfoBoard.SetARGB( 0xc0ffffff );
	m_cRCharInfoBoard.SetARGB( 0xc0ffffff );

	m_cCreateButton.SetARGB( 0xb0ffffff );
	m_cDeleteButton.SetARGB( 0xb0ffffff );
	m_cNextButton.SetARGB( 0xb0ffffff );
	m_cPrevButton.SetARGB( 0xb0ffffff );
	m_cExitButton.SetARGB( 0xb0ffffff );

#define BUTTON_COLOR		D3DCOLOR_XRGB(185,208,255)
#define BUTTON_HOVER_COLOR	D3DCOLOR_XRGB(232,232,232)
#define TEXT_COLOR			D3DCOLOR_XRGB(127,169,220 )

	m_cCreateButton.SetLabelColor( BUTTON_COLOR );
	m_cDeleteButton.SetLabelColor( BUTTON_COLOR );
	m_cNextButton.SetLabelColor( BUTTON_COLOR );
	m_cPrevButton.SetLabelColor( BUTTON_COLOR );
	m_cExitButton.SetLabelColor( BUTTON_COLOR );

	m_cDeleteButton.SetLabelHoveredColor( BUTTON_HOVER_COLOR );
	m_cCreateButton.SetLabelHoveredColor( BUTTON_HOVER_COLOR );
	m_cPrevButton.SetLabelHoveredColor( BUTTON_HOVER_COLOR );
	m_cNextButton.SetLabelHoveredColor( BUTTON_HOVER_COLOR );
	m_cExitButton.SetLabelHoveredColor( BUTTON_HOVER_COLOR );

	m_cNameStr.GetFont()->dwColor = TEXT_COLOR;
	m_cLevelStr.GetFont()->dwColor = TEXT_COLOR;
	m_cAttributeStr.GetFont()->dwColor = TEXT_COLOR;
	m_cDalantStr.GetFont()->dwColor = TEXT_COLOR;
	m_cGoldStr.GetFont()->dwColor = TEXT_COLOR;

	// -----------------------------------------------------------------


	m_cUpperBoardAni.SetTotalFrameNum( 5 );
	m_cUpperBoardAni.SetObjectNumInLine( 2 );
	m_cUpperBoardAni.SetStopInterval( 2000 );
	m_cUpperBoardAni.SetFPS( 10 );

	m_cLowerBoardAni.SetTotalFrameNum( 12 );
	m_cLowerBoardAni.SetObjectNumInLine( 2 );
	m_cLowerBoardAni.SetStopInterval( 2000 );
	m_cLowerBoardAni.SetFPS( 10 );


	m_cPrevButton.SetSoundID( SND_ID_LOGIN_SELECT );
	m_cNextButton.SetSoundID( SND_ID_LOGIN_SELECT );

	m_cCreateButton.SetSoundID( SND_ID_LOGIN_OK );
	m_cDeleteButton.SetSoundID( SND_ID_LOGIN_CANCEL );

	m_cConnectButton.SetSoundID( SND_ID_LOGIN_OK );
	m_cExitButton.SetSoundID( SND_ID_LOGIN_CANCEL );

	m_bIsCreated = TRUE;
	
}

void
CGUISelectCharacter::SetSprite( void )
{
	if( !m_bIsCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_LOGIN );

		l_spriteInfo.wActionNo = 2;

		// char info board
		l_spriteInfo.wFrameNo = 0;
		m_cUpperBase.SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 1;
		m_cLCharInfoBoard.SetSpriteInfo( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 2;
		m_cRCharInfoBoard.SetSpriteInfo( &l_spriteInfo );


		// ------------------------------------------------------------
		// menu board
		l_spriteInfo.wFrameNo = 3;
		m_cLowerBase.SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 4;
		m_cCreateButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 5;
		m_cCreateButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 6;
		m_cCreateButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );

		l_spriteInfo.wFrameNo = 8;
		m_cDeleteButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 9;
		m_cDeleteButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 10;
		m_cDeleteButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );

		l_spriteInfo.wFrameNo = 12;
		m_cPrevButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 13;
		m_cPrevButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 14;
		m_cPrevButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		
		l_spriteInfo.wFrameNo = 15;
		m_cNextButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 16;
		m_cNextButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 17;
		m_cNextButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		
		l_spriteInfo.wFrameNo = 18;
		m_cConnectButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 19;
		m_cConnectButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );

		l_spriteInfo.wFrameNo = 20;
		m_cExitButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 21;
		m_cExitButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 22;
		m_cExitButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );


		// ani
		l_spriteInfo.wActionNo = 5;

		l_spriteInfo.wFrameNo = 0;
		m_cUpperBoardAni.SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.wFrameNo = 1;
		m_cLowerBoardAni.SetSpriteInfo( &l_spriteInfo );
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_LOGIN );
		if( !l_pSprite )
			return;	

		// char info board		
		m_cUpperBase.GetSpriteInfo()->pSprite = l_pSprite;
		
		m_cLCharInfoBoard.GetSpriteInfo()->pSprite = l_pSprite;		
		m_cRCharInfoBoard.GetSpriteInfo()->pSprite = l_pSprite;


		// ------------------------------------------------------------
		// menu board		
		m_cLowerBase.GetSpriteInfo()->pSprite = l_pSprite;

		m_cCreateButton.SetSprite( l_pSprite );		
		m_cDeleteButton.SetSprite( l_pSprite );		
		
		m_cPrevButton.SetSprite( l_pSprite );		
		m_cNextButton.SetSprite( l_pSprite );				
		
		m_cConnectButton.SetSprite( l_pSprite );		
		
		m_cExitButton.SetSprite( l_pSprite );

		// ani
		m_cUpperBoardAni.GetSpriteInfo()->pSprite = l_pSprite;		
		m_cLowerBoardAni.GetSpriteInfo()->pSprite = l_pSprite;
	}
}

void
CGUISelectCharacter::MouseEvent( _mouse_event & event )
{
	if( event.id == MOUSE_CLICKED )
	{
		if( event.source == &m_cCreateButton )
		{
			EventCreateCharacter();
			// ################## 코드 추가 시킬 곳 ###########################
			// 
			// ################################################################
		}
		else if( event.source == &m_cDeleteButton )
		{
			_GUIMessageBox( this, "캐릭터를 삭제하겠습니까?", 0xb0ffffff, 
							DELETE_CHAR_BOX_ID, DLG_STYLE_OK_CANCEL );
			//EventDeleteCharacter();
			// #pu################# 코드 추가 시킬 곳 ###########################
			// 
			// ################################################################
		}
		else if( event.source == &m_cConnectButton )
		{
			EventJoinGameFunc();
			// ################## 코드 추가 시킬 곳 ###########################
			// 
			// ################################################################
		}
		else if( event.source == &m_cExitButton )
		{			
			_GUIMessageBox( this, "게임을 종료하시겠습니까?", 0xb0ffffff, 0, DLG_STYLE_OK_CANCEL );
			// ################## 코드 추가 시킬 곳 ###########################
			// 
			// ################################################################
		}
		else if( event.source == &m_cNextButton )
		{
			EventNextButton();
			// ################## 코드 추가 시킬 곳 ###########################
			// 다음 캐릭터를 선택한다.
			// ################################################################
		}
		else if( event.source == &m_cPrevButton )
		{
			EventPreviousButton();
			// ################## 코드 추가 시킬 곳 ###########################
			// 이전 캐릭터를 선택한다.
			// ################################################################
		}
	}	
}

void	
CGUISelectCharacter::ClosingDialogBox( CGUIDialogBox * pi_pDlg )
{
	if( pi_pDlg->GetUserDefinedID() == DELETE_CHAR_BOX_ID )
	{
		if( pi_pDlg->GetResultMsg() == MSG_OK )
			EventDeleteCharacter();			
	}
	else 
	{
		if( pi_pDlg->GetResultMsg() == MSG_OK )
			EventGameClose();
			// ################## 코드 추가 시킬 곳 ###########################
			// 게임을 종료한다.
			// ################################################################
	}	

	CGUIWindow::ClosingDialogBox( pi_pDlg );
}

void
CGUISelectCharacter::EnableCreate( BOOL pi_bEnable )
{
	SPRITE_INFO		l_spriteInfo;
	memcpy( &l_spriteInfo, m_cCreateButton.GetSpriteInfo(), sizeof(SPRITE_INFO) );

	if( pi_bEnable )
	{
		l_spriteInfo.wFrameNo = 4;
		m_cCreateButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 5;
		m_cCreateButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 6;
		m_cCreateButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		m_cCreateButton.Enable();
	}
	else
	{
		l_spriteInfo.wFrameNo = 7;
		m_cCreateButton.SetSpriteInfoEx( &l_spriteInfo );
		m_cCreateButton.Disable();
	}
}

void
CGUISelectCharacter::EnableDelete( BOOL pi_bEnable )
{
	SPRITE_INFO		l_spriteInfo;
	memcpy( &l_spriteInfo, m_cDeleteButton.GetSpriteInfo(), sizeof(SPRITE_INFO) );

	if( pi_bEnable )
	{
		l_spriteInfo.wFrameNo = 8;
		m_cDeleteButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 9;
		m_cDeleteButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 10;
		m_cDeleteButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		m_cDeleteButton.Enable();
	}
	else
	{
		l_spriteInfo.wFrameNo = 11;
		m_cDeleteButton.SetSpriteInfoEx( &l_spriteInfo );
		m_cDeleteButton.Disable();
	}
}

void
CGUISelectCharacter::SetEmptyInfoBoard( void )
{
	m_cNameStr.SetString( NULL );
	m_cLevelStr.SetString( NULL );
	m_cAttributeStr.SetString( NULL );
	m_cDalantStr.SetString( NULL );
	m_cGoldStr.SetString( NULL );
}

LRESULT
CGUISelectCharacter::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
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
				EventJoinGameFunc();
				return 1;
			}
			else if( wParam == VK_RIGHT )
			{
				EventNextButton();
			}
			else if( wParam == VK_LEFT )
			{
				EventPreviousButton();
			}
			else if( wParam == VK_ESCAPE )
			{
				_GUIMessageBox( this, "게임을 종료하시겠습니까?", 0xb0ffffff, 0, DLG_STYLE_OK_CANCEL );
			}
			/*
			else if( wParam == VK_DELETE )
			{
				_GUIMessageBox( this, "캐릭터를 삭제하겠습니까?", 0xb0ffffff, 
								DELETE_CHAR_BOX_ID, DLG_STYLE_OK_CANCEL );
			}
			*/
			break;

		case WM_CHAR:
			if( wParam == 'C' ||
				wParam == 'c' )
			{
				EventCreateCharacter();
			}
			break;
		}

		CGUIContainer::MsgProc( hWnd, uMsg, wParam, lParam );
	}

	return 1;
}

void
CGUISelectCharacter::SetCharInfo_Name( char * pi_pName )
{
	char buf[32];
	sprintf( buf, "이름 : %s", pi_pName );
	m_cNameStr.SetString( buf );
}

void
CGUISelectCharacter::SetCharInfo_Level( char * pi_pLevel )
{
	char buf[32];
	sprintf( buf, "레벨 : %s", pi_pLevel );
	m_cLevelStr.SetString( buf );	
}

void
CGUISelectCharacter::SetCharInfo_Attribute( BYTE pi_byAttrType )
{
	if( pi_byAttrType > 3 )
		return;

	char l_pAttrName[4][16] = { "전사계", "레인저계", "정신계", "특수계" };

	char buf[32];
	sprintf( buf, "성향 : %s", l_pAttrName[pi_byAttrType] );
	m_cAttributeStr.SetString( buf );
}
	
void
CGUISelectCharacter::SetCharInfo_Money( DWORD pi_dwDalant, DWORD pi_dwGold, BYTE pi_byRaceType )
{		
	char buf[64];
	char pMoneyStr[32];

	if( pi_byRaceType / 2 >= 2 ) 
	{
		// 아크레시아 글자 정렬할려고 하드코딩 -_-
		_GetPriceStr( pi_dwDalant, pMoneyStr );
		sprintf( buf, "C  P  : %s", pMoneyStr );
		m_cDalantStr.SetString( buf );

		_GetPriceStr( pi_dwGold, pMoneyStr );
		sprintf( buf, "골 드 : %s", pMoneyStr );
		m_cGoldStr.SetString( buf );
	}
	else
	{
		_GetPriceStr( pi_dwDalant, pMoneyStr );
		sprintf( buf, "%s : %s", _GetMoneyName( pi_byRaceType ), pMoneyStr );
		m_cDalantStr.SetString( buf );

		_GetPriceStr( pi_dwGold, pMoneyStr );
		sprintf( buf, "골  드 : %s", pMoneyStr );
		m_cGoldStr.SetString( buf );
	}	
}
