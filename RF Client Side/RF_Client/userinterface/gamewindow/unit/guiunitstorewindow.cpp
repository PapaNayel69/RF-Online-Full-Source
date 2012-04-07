#include "GUIUnitStoreWindow.h"
#include "../../SpriteMgr.h"

#define TITLE_COLOR		0xFFC3C1BE
#define	BASE_ARGB		0xCC161C1C
#define	BASE_LINE_ARGB	0xB0B4B4B4

#define	VIEW_UNIT_NUM	5
#define	UNIT_SIZE_Y		45

////////////////////////////////////////////////////////////////////////////
//
// STORE_UNIT_INFO Class Implementation
//
////////////////////////////////////////////////////////////////////////////
STORE_UNIT_INFO::STORE_UNIT_INFO()
{
	m_dwUnitID = -1L;

	SetSize( 220, UNIT_SIZE_Y );

	m_uiCover.SetSize( m_ptSize );
	m_uiOutLine.SetSize( m_ptSize.x, 1 );

	m_uiKeyBase.SetSize( 40, 40 );
	m_uiKeyIcon.SetSize( 32, 32 );

	m_uiUnitNameBase.SetSize( 169, 17 );

	m_uiKeyBase.SetPos( m_ptPos.x + 3, m_ptPos.y + 2 );
	m_uiKeyIcon.SetPos( m_uiKeyBase.m_ptPos.x + 4, m_uiKeyBase.m_ptPos.y + 4 );

	m_uiUnitNameBase.SetPos( m_ptPos.x + 46, m_ptPos.y + 3 );
	m_uiUnitCost.SetPos( m_ptPos.x + 52, m_ptPos.y + 27 );
	m_uiCover.SetPos( m_ptPos );
	m_uiOutLine.SetPos( m_ptPos );

	Add( &m_uiOutLine );
	Add( &m_uiKeyBase );
	Add( &m_uiKeyIcon );
	Add( &m_uiUnitNameBase );
	Add( &m_uiUnitName );
	Add( &m_uiUnitCost );
	Add( &m_uiCover );	
	
	m_uiOutLine.SetARGB( 0xFF5E5E5E );

	UnSelect();
}

STORE_UNIT_INFO::~STORE_UNIT_INFO()
{

}

void
STORE_UNIT_INFO::SetSprite( void )
{
	SPRITE_INFO		l_spriteInfo;
	l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_UNIT );
	if( !l_spriteInfo.pSprite )
		return;
	
	l_spriteInfo.wActionNo = 4;
	l_spriteInfo.wFrameNo = 0;
	m_uiKeyBase.SetSpriteInfo( &l_spriteInfo );

	if( !m_uiKeyIcon.IsEmpty() )
		m_uiKeyIcon.GetSpriteInfo()->pSprite = gSpriteMgr.GetSprite( SP_ID_ITEM );
}

void	
STORE_UNIT_INFO::SetKeyIcon( DWORD pi_dwItemID, DWORD pi_dwSpriteID )
{
	m_uiKeyIcon.SetItemID( pi_dwItemID );
	m_uiKeyIcon.SetSpriteID( pi_dwSpriteID );

	m_uiKeyIcon.SetSprite();
}

void
STORE_UNIT_INFO::SetUnitName( char * pi_pName )
{
	m_uiUnitName.SetString( pi_pName );

	m_uiUnitName.SetPos( m_uiUnitNameBase.m_ptPos.x + ( m_uiUnitNameBase.m_ptSize.x  - m_uiUnitName.m_ptSize.x ) / 2,
						 m_uiUnitNameBase.m_ptPos.y + 3 );
}

void	
STORE_UNIT_INFO::SetCost( DWORD pi_dwCost )
{
	char buf[32];
	sprintf( buf, "보관비용    %8d %s", pi_dwCost, _GetAvatarMoneyName() );
	m_uiUnitCost.SetString( buf );
}

void			
STORE_UNIT_INFO::Select( void )
{
	SetARGB( D3DCOLOR_XRGB( 75, 66, 60 ) );
	m_uiUnitNameBase.SetARGB( D3DCOLOR_XRGB( 76, 87, 87 ) );
	m_uiUnitName.GetFont()->dwColor = D3DCOLOR_XRGB( 150, 186, 255 );
	m_uiUnitCost.GetFont()->dwColor = D3DCOLOR_XRGB( 150, 186, 255 );
}

void			
STORE_UNIT_INFO::UnSelect( void )
{
	SetARGB( D3DCOLOR_XRGB( 50, 54, 55 ) );
	m_uiUnitNameBase.SetARGB( D3DCOLOR_XRGB( 56, 60, 79 ) );
	m_uiUnitName.GetFont()->dwColor = D3DCOLOR_XRGB( 220, 208, 185 );
	m_uiUnitCost.GetFont()->dwColor = D3DCOLOR_XRGB( 220, 208, 185 );
}

////////////////////////////////////////////////////////////////////////////
//
// CGUIUnitStoreWindow Class Implementation
//
////////////////////////////////////////////////////////////////////////////
CGUIUnitStoreWindow::CGUIUnitStoreWindow()
{
	m_bIsCreated = FALSE;
	m_pSelectedUnit = NULL;
}

CGUIUnitStoreWindow::~CGUIUnitStoreWindow()
{
	RemoveAllUnit();
}

void
CGUIUnitStoreWindow::Init( void )
{
	SetSize( 232, 288 );
	m_uiTitleBoard.SetSize( 232, 26 );
	m_uiTakeOutUnitButton.SetSize( 57, 28 );
	m_uiCloseButton.SetSize( 16, 13 );

	m_uiUnitListBoard.SetSize( 220, VIEW_UNIT_NUM * UNIT_SIZE_Y );
	
	
	m_uiTitle.SetString( "기갑장비 목록" );
	m_uiTakeOutUnitButton.SetLabel( "출 고" );
	
	m_uiTitleBoard.SetPos( m_ptPos.x, m_ptPos.y );
	m_uiTitle.SetPos( m_ptPos.x + ( m_ptSize.x - m_uiTitle.m_ptSize.x ) / 2, m_ptPos.y + 7 );
	m_uiCloseButton.SetPos( m_ptPos.x + 214, m_ptPos.y + 4 );

	m_uiUnitListBoard.SetPos( m_ptPos.x + 5, m_ptPos.y + 31 );
	
	m_uiTakeOutUnitButton.SetPos( m_ptPos.x + ( m_ptSize.x - m_uiTakeOutUnitButton.m_ptSize.x ) / 2, m_ptPos.y + 240 );

	m_uiCloseButton.AddMouseEventHandler( this );
	m_uiTakeOutUnitButton.AddMouseEventHandler( this );

	Add( &m_uiTitleBoard );
	Add( &m_uiTitle );
	Add( &m_uiCloseButton );
	Add( &m_uiUnitListBoard );
	Add( &m_uiTakeOutUnitButton );

	SetSprite();	

	m_uiTitle.GetFont()->dwColor = TITLE_COLOR;
	m_uiTakeOutUnitButton.SetLabelStyle( R3_HAN_OUTLINE );

	SetMoveObject( &m_uiTitle );
	SetMoveObject( &m_uiTitleBoard );	
	SetMovable( TRUE );	

	m_uiUnitListBoard.GetScrollbar()->SetAdjustmentUnit( UNIT_SIZE_Y );

	SetWheelScrollObject( m_uiUnitListBoard.GetScrollbar() );

	UnSelectUnit();

	m_bIsCreated = TRUE;
}

void
CGUIUnitStoreWindow::SetSprite( void )
{
	if( !m_bIsCreated )
	{
		SPRITE_INFO		l_spriteInfo;
		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_BELT );

		l_spriteInfo.wActionNo = 7;

		// title board
		l_spriteInfo.wFrameNo = 0;	
		m_uiTitleBoard.SetSpriteInfo( &l_spriteInfo );

		l_spriteInfo.pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );
		// takeoutunitbutton
		l_spriteInfo.wActionNo = 0;
		l_spriteInfo.wFrameNo = 21;
		m_uiTakeOutUnitButton.SetSpriteInfoEx( &l_spriteInfo );
		l_spriteInfo.wFrameNo = 22;
		m_uiTakeOutUnitButton.SetSpriteInfoEx( HOVERED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 23;
		m_uiTakeOutUnitButton.SetSpriteInfoEx( PRESSED, &l_spriteInfo );
		l_spriteInfo.wFrameNo = 24;
		m_uiTakeOutUnitButton.SetSpriteInfoEx( DISABLED, &l_spriteInfo );				

		// close button	
		m_uiCloseButton.SetSpriteInfoEx( gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON1 ) );
		m_uiCloseButton.SetSpriteInfoEx( HOVERED, gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON2 ) );
		m_uiCloseButton.SetSpriteInfoEx( PRESSED, gSpriteMgr.GetSpriteInfo( SP_ID_COMMON, SPE_ID_CLOSE_BUTTON3 ) );

		m_uiUnitListBoard.SetSprite();
	}
	else
	{
		CSprite * l_pSprite;
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_BELT );	
		if( !l_pSprite )
			return;		

		m_uiTitleBoard.GetSpriteInfo()->pSprite = l_pSprite;
		
		l_pSprite = gSpriteMgr.GetSprite( SP_ID_COMMON );				

		m_uiTakeOutUnitButton.SetSprite( l_pSprite );	

		// close button	
		m_uiCloseButton.SetSprite( l_pSprite );

		m_uiUnitListBoard.SetSprite();
	}
}


void
CGUIUnitStoreWindow::InsertUnit( DWORD pi_dwInsertIndex, DWORD pi_dwUnitID, 
								 DWORD pi_dwItemID, DWORD pi_dwSpriteID, char * pi_pUnitName, DWORD pi_dwCost )
{	
	if( ExistUnit( pi_dwUnitID ) )
		return;

	STORE_UNIT_INFO * l_pNewUnitInfo;
	l_pNewUnitInfo = new STORE_UNIT_INFO;

	l_pNewUnitInfo->SetUnitID( pi_dwUnitID );
	l_pNewUnitInfo->SetUnitName( pi_pUnitName );
	l_pNewUnitInfo->SetKeyIcon( pi_dwItemID, pi_dwSpriteID );
	l_pNewUnitInfo->GetCorver()->AddMouseEventHandler( this );
	l_pNewUnitInfo->SetCost( pi_dwCost );
	l_pNewUnitInfo->SetSprite();

	m_uiUnitListBoard.InsertObject( l_pNewUnitInfo, pi_dwInsertIndex );
}
	
void	
CGUIUnitStoreWindow::RemoveUnit( DWORD pi_dwUnitID )
{
	STORE_UNIT_INFO * l_pUnitInfo;
	for( int i=0; i < m_uiUnitListBoard.GetListSize(); ++i )
	{
		l_pUnitInfo = (STORE_UNIT_INFO * )m_uiUnitListBoard.GetListObject( i );
		if( l_pUnitInfo &&
			l_pUnitInfo->GetUnitID() == pi_dwUnitID )
		{
			if( m_pSelectedUnit == l_pUnitInfo )
				UnSelectUnit();

			m_uiUnitListBoard.RemoveObject( l_pUnitInfo );

			delete l_pUnitInfo;

			return;
		}			
	}	
}

void	
CGUIUnitStoreWindow::RemoveAllUnit( void )
{
	STORE_UNIT_INFO * l_pUnitInfo;
	for( int i=0; i < m_uiUnitListBoard.GetListSize(); ++i )
	{
		l_pUnitInfo = (STORE_UNIT_INFO * )m_uiUnitListBoard.GetListObject( i );
		if( l_pUnitInfo )
			delete l_pUnitInfo;	
	}

	m_uiUnitListBoard.RemoveAllObject();

	UnSelectUnit();
}

void
CGUIUnitStoreWindow::UpdateCost( DWORD pi_dwUnitID, DWORD pi_dwCost )
{
	STORE_UNIT_INFO * l_pUnitInfo;
	l_pUnitInfo = GetUnit( pi_dwUnitID );
	if( l_pUnitInfo )
		l_pUnitInfo->SetCost( pi_dwCost );
}

BOOL	
CGUIUnitStoreWindow::ExistUnit( DWORD pi_dwUnitID )
{	
	if( GetUnit( pi_dwUnitID ) )
		return TRUE;
	else 
		return FALSE;
}

void	
CGUIUnitStoreWindow::SelectUnit( DWORD pi_dwUnitID )
{	
	STORE_UNIT_INFO * l_pUnitInfo;
	l_pUnitInfo = GetUnit( pi_dwUnitID );
	if( l_pUnitInfo )
		SelectUnit( l_pUnitInfo );
}

void
CGUIUnitStoreWindow::SelectUnit( STORE_UNIT_INFO * pi_pUnitInfo )
{
	if( !pi_pUnitInfo )
		return;

	// unselect previous unit
	if( m_pSelectedUnit )
	{
		m_pSelectedUnit->UnSelect();
	}
	
	// set selected unit
	m_pSelectedUnit = pi_pUnitInfo;
	m_pSelectedUnit->Select();

	m_uiTakeOutUnitButton.Enable();
}

void	
CGUIUnitStoreWindow::UnSelectUnit( void )
{
	if( m_pSelectedUnit )
	{
		m_pSelectedUnit->UnSelect();
		m_pSelectedUnit = NULL;
	}
	

	m_uiTakeOutUnitButton.Disable();
}

STORE_UNIT_INFO * 
CGUIUnitStoreWindow::GetUnit( DWORD pi_dwUnitID )
{
	STORE_UNIT_INFO * l_pUnitInfo;
	for( int i=0; i < m_uiUnitListBoard.GetListSize(); ++i )
	{
		l_pUnitInfo = (STORE_UNIT_INFO * )m_uiUnitListBoard.GetListObject( i );
		if( l_pUnitInfo &&
			l_pUnitInfo->GetUnitID() == pi_dwUnitID )
		{
			return l_pUnitInfo;
		}
	}

	return NULL;
}

void
CGUIUnitStoreWindow::MouseEvent( _mouse_event &event )
{
	CGUIWindow::MouseEvent( event );

	if( event.id == MOUSE_STROKED )
	{		
		STORE_UNIT_INFO * l_pUnitInfo;
		l_pUnitInfo = GetSelectedUnit( event.source );
		if( l_pUnitInfo )
		{
			if( m_pSelectedUnit != l_pUnitInfo )
			{
				// 기갑장비 선택
				SelectUnit( l_pUnitInfo );
			}
			else
			{
				// 선택 해제
				UnSelectUnit();		
			}
		}		
	}
	else if( event.id == MOUSE_CLICKED )
	{
		if( event.source == &m_uiTakeOutUnitButton )
		{
			// 출고
			if( m_pSelectedUnit )
				PostUIMessage( WM_BOARD_TRADE, m_pSelectedUnit->GetUnitID() );
		}
		else if( event.source == &m_uiCloseButton )
		{
			Close();
		}	
	}
}

STORE_UNIT_INFO *
CGUIUnitStoreWindow::GetSelectedUnit( CGUIObject * pi_pEventObj )
{
	STORE_UNIT_INFO * l_pUnitInfo;

	for( int i=0; i < m_uiUnitListBoard.GetListSize(); ++i )
	{
		l_pUnitInfo = (STORE_UNIT_INFO * )m_uiUnitListBoard.GetListObject( i );
		if( l_pUnitInfo && pi_pEventObj == l_pUnitInfo->GetCorver() )
			return l_pUnitInfo;
	}

	return NULL;
}

BOOL
CGUIUnitStoreWindow::Draw( void )
{
	if( !IsShow() )
		return FALSE;

	// base
	Draw2DRectangle( m_ptPos.x, m_ptPos.y + m_uiTitleBoard.m_ptSize.y - 10,
					 m_ptPos.x + m_ptSize.x, m_ptPos.y + m_ptSize.y,
					 //BASE_ARGB );
					 0xD0232323 );
	// left line
	Draw2DRectangle( m_ptPos.x + 2, m_ptPos.y + m_uiTitleBoard.m_ptSize.y - 10,
					 m_ptPos.x + 3, m_ptPos.y + m_ptSize.y - 3,
					 BASE_LINE_ARGB );					 
	// right line
	Draw2DRectangle( m_ptPos.x + m_ptSize.x - 3, m_ptPos.y + m_uiTitleBoard.m_ptSize.y - 10,
					 m_ptPos.x + m_ptSize.x - 2, m_ptPos.y + m_ptSize.y - 3,
					 BASE_LINE_ARGB );					
	// bottom line
	Draw2DRectangle( m_ptPos.x + 2, m_ptPos.y + m_ptSize.y - 3,
					 m_ptPos.x + m_ptSize.x - 2, m_ptPos.y + m_ptSize.y - 2,
					 BASE_LINE_ARGB );

	CGUIContainer::Draw();

	DrawDialogBox();

	return TRUE;
}