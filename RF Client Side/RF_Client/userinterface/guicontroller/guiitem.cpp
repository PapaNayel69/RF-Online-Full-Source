#include "GUIItem.h"
#include "../SpriteMgr.h"

////////////////////////////////////////////////////////////////////////////
//
// CGUIItem Class Implementation
//
////////////////////////////////////////////////////////////////////////////
CGUIItem::CGUIItem()
{
	m_dwItemID		=	NONE_ITEM_ID;
	m_dwSpriteID	=	0;
	m_wNum			=	1;	
	
	m_pRefItem		=	NULL;
	
	m_bDrawItemNum	=	TRUE;

	m_byUsableLevel	=	NONE_LEVEL;
	m_byUsable		=	UNCHECKED_USABLE;

	SetIconType( ITEM_ICON );

	SetSize( 32, 32 );
}

CGUIItem::~CGUIItem()
{

}

void
CGUIItem::SetItemID( DWORD pi_dwItemID )
{
	m_dwItemID = pi_dwItemID;

	if( m_dwItemID == NONE_ITEM_ID )
		SetSpriteInfo( NULL );
}

void
CGUIItem::SetItemID( BYTE pi_btUser, BYTE pi_bType, BYTE pi_btExtType, BYTE pi_btID )
{
	m_dwItemID = 0;

	m_dwItemID |= pi_btUser << 20;

	m_dwItemID |= pi_bType << 16;

	m_dwItemID |= pi_btExtType << 8;
	
	m_dwItemID |= pi_btID;


	SetItemID( m_dwItemID );
}

void
CGUIItem::SetIconType( BYTE pi_byType )
{
	m_byIconType = pi_byType;


	// sprite size setting
	if( m_byIconType == ITEM_ICON ||
		m_byIconType == UNIT_ICON ) 
		m_bySpriteSize = SST_64BY64;

	else // skill, force, recall		
		m_bySpriteSize = SST_32BY32;
}

void
CGUIItem::SetData( CGUIItem * pi_pItem )
{
	if( !pi_pItem )
		return;

	// member data setting
	m_dwItemID			=	pi_pItem->m_dwItemID;
	m_dwSpriteID		=	pi_pItem->m_dwSpriteID;
	m_wNum				=	pi_pItem->m_wNum;

	m_pRefItem			=	pi_pItem->m_pRefItem;
	
	m_bDrawItemNum		=	pi_pItem->m_bDrawItemNum;

	m_byUsable			=	pi_pItem->m_byUsable;
	m_byUsableLevel		=	pi_pItem->m_byUsableLevel;

	m_byIconType		=	pi_pItem->m_byIconType;
	m_bySpriteSize	=	pi_pItem->m_bySpriteSize;	
	
	// size
	SetSize( pi_pItem->m_ptSize );	

	// sprite setting
	SetSpriteInfo( pi_pItem->GetSpriteInfo() );
}

void
CGUIItem::SetSprite( void )
{
	SPRITE_INFO l_spriteInfo;	
	l_spriteInfo.pSprite = NULL;

	if( m_byIconType == UNIT_ICON )
	{
		// m_dwItemID : parts index
		gSpriteMgr.GetSpriteInfo( SP_ID_UNIT_PARTS, m_dwSpriteID, m_dwItemID, m_bySpriteSize, l_spriteInfo );	
	}
	else
	{
		// 최상위 1 byte에 현재 아무값도 안들어가기 때문에 
		// 거기에 type을 넣어서 구분한다.
		DWORD l_dwIconClassifyID = (m_dwItemID & 0x0FFFFFFF) | ( m_byIconType << 28 );
		gSpriteMgr.GetSpriteInfo( SP_ID_ITEM, m_dwSpriteID, l_dwIconClassifyID, m_bySpriteSize, l_spriteInfo );	
	}

	SetSpriteInfo( &l_spriteInfo );
}

BOOL
CGUIItem::Draw( void )
{
	if( !IsShow() )
		return FALSE;

	if( !GetSpriteInfo()->pSprite )
		return TRUE;

	GetSpriteInfo()->pSprite->InitPageSprite( m_bySpriteSize, m_bySpriteSize );
	
	if( !CGUIObject::Draw() )
		return FALSE;

	if( m_wNum == 1 )
		return TRUE;

	if( m_byIconType != ITEM_ICON )
		return TRUE;

	if( m_bDrawItemNum )
	{
		char buf[8];
		_itoa( m_wNum, buf, 10 );
		DrawR3Hangul( m_ptPos.x,
					  m_ptPos.y + m_ptSize.y - 12,
					  buf,
					  D3DCOLOR_XRGB( 255, 255, 255),
					  R3_HAN_OUTLINE );
	}

	return TRUE;
}

BOOL
CGUIItem::Draw( RECT & pi_rtClipping )
{
	if( !IsShow() )
		return false;

	if( !GetSpriteInfo()->pSprite )
		return true;

	GetSpriteInfo()->pSprite->InitPageSprite( m_bySpriteSize, m_bySpriteSize );
	
	if( !CGUIObject::Draw( pi_rtClipping ) )
		return false;

	if( m_wNum == 1 )
		return true;

	if( m_bDrawItemNum )
	{
		char buf[8];
		_itoa( m_wNum, buf, 10 );
		DrawR3Hangul( m_ptPos.x,
					  m_ptPos.y + m_ptSize.y - 12,
					  buf ,
					  D3DCOLOR_XRGB( 255, 255, 255),
					  R3_HAN_OUTLINE );
	}

	return true;
}

void
CGUIItem::SetEmpty( void )
{
	m_dwItemID = NONE_ITEM_ID;

	GetSpriteInfo()->pSprite = NULL;

	m_wNum = 0;
}

////////////////////////////////////////////////////////////////////////////
//
// CGUISFItem Class Implementation
//
////////////////////////////////////////////////////////////////////////////
CGUISFItem::CGUISFItem()
{
	m_uiDelay.Show( TRUE );
}

CGUISFItem::~CGUISFItem()
{

}

void
CGUISFItem::BeginDelayAnimation( DWORD pi_dwTime )
{
	m_uiDelay.SetDelay( pi_dwTime );
	m_uiDelay.Start();
}

void
CGUISFItem::SetPos( POINT & pi_ptPos )
{
	CGUISFItem::SetPos( pi_ptPos.x, pi_ptPos.y );
}

void
CGUISFItem::SetPos( LONG pi_nPosX, LONG pi_nPosY )
{
	CGUIObject::SetPos( pi_nPosX, pi_nPosY );

	m_uiDelay.SetPos( pi_nPosX, pi_nPosY );
}

void
CGUISFItem::SetSize( LONG pi_nWidth, LONG pi_nHeight )
{
	CGUIObject::SetSize( pi_nWidth, pi_nHeight );

	m_uiDelay.SetSize( pi_nWidth, pi_nHeight );
}

void
CGUISFItem::SetSize( POINT & pi_ptSize )
{
	CGUISFItem::SetSize( pi_ptSize.x, pi_ptSize.y );
}

BOOL
CGUISFItem::Draw( void )
{
	if( !CGUIItem::Draw() )
		return FALSE;

	m_uiDelay.Draw();

	return TRUE;
}

BOOL
CGUISFItem::Draw( RECT & pi_rtClipping )
{
	if( !CGUIItem::Draw( pi_rtClipping ) )
		return FALSE;

	m_uiDelay.Draw( pi_rtClipping );

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////
//
// CGUIAnimusItem Class Implementation
//
////////////////////////////////////////////////////////////////////////////

CGUIAnimusItem::CGUIAnimusItem()
{

}

CGUIAnimusItem::~CGUIAnimusItem()
{

}

////////////////////////////////////////////////////////////////////////////
//
// CGUISocket Class Implementation
//
////////////////////////////////////////////////////////////////////////////
CGUISocket::CGUISocket()
{
	m_pItem		= NULL;
}

CGUISocket::~CGUISocket()
{

}

void
CGUISocket::SetItem( CGUIItem * pi_pItem )
{
	if( !pi_pItem )
		return;

	m_pItem = pi_pItem;

	m_pItem->Show( TRUE );

	// reset pos
	SetPos( m_ptPos );
}

void		
CGUISocket::SetPos( POINT & pi_ptPos )
{
	SetPos( pi_ptPos.x, pi_ptPos.y );
}

void		
CGUISocket::SetPos( LONG pi_nPosX, LONG pi_nPosY )
{
	CGUIObject::SetPos( pi_nPosX, pi_nPosY );

	if( m_pItem )
		m_pItem->SetPos( m_ptPos.x + 1, m_ptPos.y + 1 );
}

BOOL
CGUISocket::Draw( void )
{
	if( !CGUIObject::Draw() )
		return FALSE;

	if( m_pItem )
		m_pItem->Draw();

	return TRUE;
}

BOOL	
CGUISocket::Draw( RECT & pi_rtClipping )
{
	if( !CGUIObject::Draw( pi_rtClipping ) )
		return FALSE;
	
	if( m_pItem )
		m_pItem->Draw( pi_rtClipping );
	
	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////    Global Function       ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
_IsItemUser( DWORD & pi_dwItemID, BYTE pi_btType )
{	
	if( ( pi_btType << 20 ) == ( pi_dwItemID & 0x0ff00000 ) )
		return true;
	
	return false;
}

BOOL
_IsItemType( DWORD & pi_dwItemID, BYTE pi_btType )
{	
	if( ( pi_btType << 16 ) == ( pi_dwItemID & 0x000f0000 ) )
		return true;
	
	return false;
}

BOOL
_IsItemExtType( DWORD & pi_dwItemID, BYTE pi_btType )
{
	if( ( pi_btType << 8 ) == ( pi_dwItemID & 0x0000ff00 ) )
		return true;
	
	return false;
}

BOOL
_IsItemRealID( DWORD & pi_dwItemID, BYTE pi_btID )
{
	if( pi_btID == ( pi_dwItemID & 0x000000ff ) )
		return true;
	
	return false;
}

BYTE	_GetItemUser( DWORD & pi_dwItemID )
{
	return ( pi_dwItemID & 0x0ff00000 ) >> 20;
}

BYTE	_GetItemType( DWORD & pi_dwItemID )
{
	return ( pi_dwItemID & 0x000f0000 ) >> 16;
}

BYTE	_GetItemExtType( DWORD & pi_dwItemID )
{	
	return ( pi_dwItemID & 0x0000ff00 ) >> 8;
}

BYTE	_GetItemRealID( DWORD & pi_dwItemID )
{
	return ( pi_dwItemID & 0x000000ff );
}


////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 종족이나 레벨에 따라 사용 가능한 아이템인지 체크한다.
BOOL	_IsUsableItem( CGUIItem * pi_pItem, BYTE pi_byRaceType, BYTE pi_byLevel/*=0xFF*/ )
{
	BYTE	l_byUser = _GetItemUser( pi_pItem->m_dwItemID );

	if( l_byUser < 0x05 )
	{
		if( pi_byRaceType != l_byUser )
			return false;
	}
	else
	{
		if( l_byUser == IID_COM ) {;}
		else if( l_byUser == IID_BELCOR )
		{
			if( pi_byRaceType == IID_ACCRETIA )
				return false;			
		}
		else if( l_byUser == IID_CORA )
		{
			if( !( pi_byRaceType == IID_CORAMALE ||
				   pi_byRaceType == IID_CORAFEMALE ) )
				return false;
		}
		else if( l_byUser == IID_BELLATOR )
		{
			if( !( pi_byRaceType == IID_BELMALE ||
				   pi_byRaceType == IID_BELFEMALE ) )
				return false;
		}
		else if( l_byUser == IID_FEMALE )
		{
			if( !(pi_byRaceType % 2) )
				return false;
		}
		else // male
		{
			if( pi_byRaceType % 2 )
				return false;
		}
	}

	// 레벨값이 들어왔으면 레벨도 체크한다.
	if( pi_byLevel != 0xFF )
	{
		if( pi_pItem->GetUsableLevel() != NONE_LEVEL &&
			pi_byLevel < pi_pItem->GetUsableLevel() )
			return false;	
	}

	return true;
}


// 무기 잡는 타입을 리턴한다.
BYTE
_GetWeapnHandType( DWORD pi_dwItemID )
{
	BYTE l_byType = _GetItemExtType( pi_dwItemID );

	switch( l_byType )
	{
	case IID_RSWORD:
	case IID_RKNIFE:
	case IID_RKNIFETHROW:
	case IID_RAXE:
	case IID_RAXETHROW:
	case IID_RMACE:
	case IID_RSTAFF:
//	case IID_RGUN:	// firearm류는 다 양손으로 바꼈다.
//	case IID_RBEAMGUN:
		return RIGHT_HAND_WEAPON;

	case IID_DSWORD:
	case IID_DAXE:
	case IID_DGUN:
	case IID_DBEAMGUN:
		return DUAL_HAND_WEAPON;

		/*
		IID_TSWORD			
		IID_TKNIFE		
		IID_TAXE
		IID_TMACE
		IID_TSTAFF	
		IID_TSPEAR	
		IID_TBOW	
		IID_TCROSSBOW
		IID_TRIFLE	
		IID_TLAUNCHER
		IID_TFAUST
		IID_TMACHINEGUN
		IID_TFLAMETHROWER
		IID_TBEAMRIFLE
		IID_MININGTOOL
		IID_TPLASMAGUN
		*/
	default:
		return BOTH_HAND_WEAPON;		
	}
}


// 합칠수 있는 아이템인지 체크하는 함수
BOOL
_IsAddableItem( DWORD pi_dwItemID )
{
	BYTE l_byType = _GetItemType( pi_dwItemID );

	if( l_byType == IID_CONSUME )
	{
		l_byType = _GetItemExtType( pi_dwItemID );
		if( l_byType < IID_BULLET ||
			l_byType == IID_ADRENALINE )
			return TRUE;
	}		
	else if( l_byType == IID_ETC )
	{
		l_byType = _GetItemExtType( pi_dwItemID );
		if( l_byType < IID_TOOL ||
			l_byType == IID_TOWN )
			return TRUE;
	}
	
	return FALSE;	
}