////////////////////////////////////////////////////////////////////////////
//
// CSpriteUnit Class Implementation
//
////////////////////////////////////////////////////////////////////////////

#include "SpriteMgr.h"
#include "GUIController/GUIItem.h"

CSpriteUnit::CSpriteUnit()
{
	m_pSprite = NULL;
}

CSpriteUnit::~CSpriteUnit()
{
	Destroy();
}

void
CSpriteUnit::Create( WORD pi_wSpriteID, char * pi_pFileName )
{
	m_wID = pi_wSpriteID;
	m_pSprite = new CSprite;
	m_pSprite->LoadSprite( pi_pFileName );
	
	m_pSearchFunc = DefaultSearchFunc;
}

void
CSpriteUnit::Destroy( void )
{
	SPRITE_INFO_DB * l_pInfoDB = NULL;
	for( CSpriteInfoDBList::iterator i = m_listSpriteInfoDB.begin(); i != m_listSpriteInfoDB.end(); ++i )
	{
		l_pInfoDB = (*i);		
		delete l_pInfoDB;
	}

	m_listSpriteInfoDB.clear();

	m_pSprite->ReleaseSprite();
	delete m_pSprite;
}

void
CSpriteUnit::AddSpriteInfoDB( DWORD dwID, WORD wActionNo, WORD wFrameNo )
{
	SPRITE_INFO_DB	* l_pSpriteInfoDB = new SPRITE_INFO_DB;
	l_pSpriteInfoDB->dwID = dwID;
	l_pSpriteInfoDB->spriteInfo.pSprite = m_pSprite;
	l_pSpriteInfoDB->spriteInfo.wActionNo = wActionNo;
	l_pSpriteInfoDB->spriteInfo.wFrameNo = wFrameNo;

	m_listSpriteInfoDB.insert( m_listSpriteInfoDB.end(), l_pSpriteInfoDB );
}

SPRITE_INFO	*
CSpriteUnit::GetSpriteInfo( WORD pi_wSpriteExID )
{
	for( CSpriteInfoDBList::iterator i = m_listSpriteInfoDB.begin(); i != m_listSpriteInfoDB.end(); ++i )
	{
		if( (*i)->dwID == pi_wSpriteExID )
		{
			return &((*i)->spriteInfo);
		}
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////////
//
// CSpriteMgr Class Implementation
//
////////////////////////////////////////////////////////////////////////////
CSpriteMgr::CSpriteMgr()
{

}

CSpriteMgr::~CSpriteMgr()
{
	UnloadSpriteAll();
}

void
CSpriteMgr::Init()
{
	
}

void
CSpriteMgr::LoadSprite( WORD pi_wSpriteID )
{
	if( pi_wSpriteID >= SP_ID_MINIMAP )
		return;

	WORD id;
	CSpriteUnit * l_pUnit;

	// 이미 loading되었는지 체크
	for (CSpriteUnitList::iterator i = m_listSpriteUnit.begin(); i != m_listSpriteUnit.end(); ++i)
	{
		id = (*i)->m_wID;
		l_pUnit = *i;

		if ((*i)->m_wID == pi_wSpriteID )
			return;
	}		

	char l_pSpriteFileName[][128] = 
	{ 
	  "./SpriteImage/mouse.spr",
	  "./SpriteImage/common.spr",
	  "./SpriteImage/gaugeexp.spr",
	  "./SpriteImage/belt.spr",
	  "./SpriteImage/inventory.spr",
	  "./SpriteImage/chat.spr",
	  "./SpriteImage/shop.spr",
	  "./SpriteImage/login.spr",
	  "./SpriteImage/loading.spr",
	  "./SpriteImage/item.spr",
	  "./SpriteImage/oreproc.spr",
	  "./SpriteImage/mining.spr",
	  "./SpriteImage/exchange.spr",
	  "./SpriteImage/recall.spr",
	  "./SpriteImage/itemmake.spr",
	  "./SpriteImage/itemupgrade.spr",
	  "./SpriteImage/party.spr",
	  "./SpriteImage/trade.spr",
	  "./SpriteImage/maploading.spr",
	  "./SpriteImage/dungeon.spr",
	  "./SpriteImage/unit.spr",
	  "./SpriteImage/unitparts.spr",
	  "./SpriteImage/class.spr"	  
	};

	CSpriteUnit * l_pSpriteUnit = new CSpriteUnit;
	// create sprite
	l_pSpriteUnit->Create( pi_wSpriteID, l_pSpriteFileName[pi_wSpriteID] );
	
	if( pi_wSpriteID == SP_ID_COMMON )
	{
		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_OK_BUTTON1, 0, 0 );
		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_OK_BUTTON2, 0, 1 );
		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_OK_BUTTON3, 0, 2 );

		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_CANCEL_BUTTON1, 0, 6 );
		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_CANCEL_BUTTON2, 0, 7 );
		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_CANCEL_BUTTON3, 0, 8 );

		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_CLOSE_BUTTON1, 0, 9 );
		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_CLOSE_BUTTON2, 0, 10 );
		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_CLOSE_BUTTON3, 0, 11 );

		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_MSG_BOX_BORDER1, 1, 0 );
		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_MSG_BOX_BORDER2, 1, 1 );
		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_MSG_BOX_BORDER3, 1, 2 );
		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_MSG_BOX_BASE, 1, 3 );		

		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_UP_BUTTON1, 3, 2);
		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_UP_BUTTON2, 3, 3 );
		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_DOWN_BUTTON1, 3, 4 );
		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_DOWN_BUTTON2, 3, 5 );
	}
	else if( pi_wSpriteID == SP_ID_ITEM )
	{		
		l_pSpriteUnit->m_pSearchFunc = ItemSearchFunc;
	}
	else if( pi_wSpriteID == SP_ID_UNIT_PARTS )
	{
		l_pSpriteUnit->m_pSearchFunc = UnitPartsSearchFunc;
	}

	// insert spirte to list
	m_listSpriteUnit.insert( m_listSpriteUnit.end(), l_pSpriteUnit );
}

void
CSpriteMgr::LoadSprite( WORD pi_wSpriteID, char * pi_pFilePathName )
{
	WORD id;
	CSpriteUnit * l_pUnit;

	// 이미 loading되었는지 체크
	for (CSpriteUnitList::iterator i = m_listSpriteUnit.begin(); i != m_listSpriteUnit.end(); ++i)
	{
		id = (*i)->m_wID;
		l_pUnit = *i;

		if ((*i)->m_wID == pi_wSpriteID )
			return;
	}	

	// create sprite
	CSpriteUnit * l_pSpriteUnit = new CSpriteUnit;	
	l_pSpriteUnit->Create( pi_wSpriteID, pi_pFilePathName );

	if( pi_wSpriteID == SP_ID_COMMON )
	{
		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_OK_BUTTON1, 0, 0 );
		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_OK_BUTTON2, 0, 1 );
		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_OK_BUTTON3, 0, 2 );

		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_CANCEL_BUTTON1, 0, 6 );
		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_CANCEL_BUTTON2, 0, 7 );
		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_CANCEL_BUTTON3, 0, 8 );

		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_CLOSE_BUTTON1, 0, 9 );
		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_CLOSE_BUTTON2, 0, 10 );
		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_CLOSE_BUTTON3, 0, 11 );

		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_MSG_BOX_BORDER1, 1, 0 );
		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_MSG_BOX_BORDER2, 1, 1 );
		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_MSG_BOX_BORDER3, 1, 2 );
		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_MSG_BOX_BASE, 1, 3 );		

		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_UP_BUTTON1, 3, 2);
		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_UP_BUTTON2, 3, 3 );
		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_DOWN_BUTTON1, 3, 4 );
		l_pSpriteUnit->AddSpriteInfoDB( SPE_ID_DOWN_BUTTON2, 3, 5 );
	}
	else if( pi_wSpriteID == SP_ID_ITEM )
	{		
		l_pSpriteUnit->m_pSearchFunc = ItemSearchFunc;
	}

	// insert spirte to list
	m_listSpriteUnit.insert( m_listSpriteUnit.end(), l_pSpriteUnit );
}

void
CSpriteMgr::UnloadSprite( WORD pi_wSpriteID )
{
	for( CSpriteUnitList::iterator i = m_listSpriteUnit.begin(); i != m_listSpriteUnit.end(); ++i)
	{
		// list에서 삭제한다.
		if( (*i)->m_wID == pi_wSpriteID ) {
			// backup remove unit
			CSpriteUnit * pUnit;			
			pUnit = (*i);

			m_listSpriteUnit.remove( (*i) );			
			
			delete pUnit;

			return;
		}
	}
}

void
CSpriteMgr::UnloadSpriteAll( void )
{
	CSpriteUnit * pUnit = NULL;

	for( CSpriteUnitList::iterator i = m_listSpriteUnit.begin(); i != m_listSpriteUnit.end(); ++i )
	{
		// backup remove unit
		pUnit = (*i);		

		delete pUnit;
	}

	m_listSpriteUnit.clear();
}

CSprite *
CSpriteMgr::GetSprite( WORD pi_wSpriteID )
{
	for( CSpriteUnitList::iterator i = m_listSpriteUnit.begin(); i != m_listSpriteUnit.end(); ++i )
	{
		if( (*i)->m_wID == pi_wSpriteID )
			return (*i)->m_pSprite;
	}

	return NULL;
}

SPRITE_INFO	*	
CSpriteMgr::GetSpriteInfo( WORD pi_wSpriteID, WORD pi_wSpriteExID )
{
	for( CSpriteUnitList::iterator i = m_listSpriteUnit.begin(); i != m_listSpriteUnit.end(); ++i )
	{
		if( (*i)->m_wID == pi_wSpriteID )
		{
			return (*i)->GetSpriteInfo( pi_wSpriteExID );
		}
	}
	
	return NULL;
}

void
CSpriteMgr::GetSpriteInfo( WORD pi_wSpriteID, DWORD pi_dwSpriteExID, SPRITE_INFO & po_SpriteInfo )
{
	for( CSpriteUnitList::iterator i = m_listSpriteUnit.begin(); i != m_listSpriteUnit.end(); ++i )
	{
		if( (*i)->m_wID == pi_wSpriteID )
		{
			po_SpriteInfo.pSprite = (*i)->m_pSprite;
			if( !(*i)->m_pSearchFunc( pi_dwSpriteExID, 0, 0, po_SpriteInfo ) )
			{
				po_SpriteInfo.wActionNo = 0;
				po_SpriteInfo.wFrameNo = 0;
			}
			return;
		}
	}	
}

void
CSpriteMgr::GetSpriteInfo( WORD pi_wSpriteID, DWORD pi_wID, DWORD pi_dwClassifyID, int pi_nWidth, SPRITE_INFO & po_SpriteInfo )
{
	for( CSpriteUnitList::iterator i = m_listSpriteUnit.begin(); i != m_listSpriteUnit.end(); ++i )
	{
		if( (*i)->m_wID == pi_wSpriteID )
		{
			po_SpriteInfo.pSprite = (*i)->m_pSprite;
			if( !(*i)->m_pSearchFunc( pi_wID, pi_dwClassifyID, pi_nWidth, po_SpriteInfo ) )
			{
				po_SpriteInfo.wActionNo = 0;
				po_SpriteInfo.wFrameNo = 0;
			}
			return;
		}
	}
}

BOOL DefaultSearchFunc( DWORD pi_dwID, DWORD pi_wReserve, int pi_nWidth, SPRITE_INFO & po_spriteInfo )
{
	po_spriteInfo.wActionNo = ( pi_dwID & 0xFF00 ) >> 8;
	po_spriteInfo.wFrameNo = pi_dwID & 0x00FF;

	return TRUE;
}


BOOL ItemSearchFunc( DWORD pi_dwID, DWORD pi_dwClassifyID, int pi_nWidth, SPRITE_INFO & po_spriteInfo )
{
	if( !pi_nWidth )
		return FALSE;

	if( !po_spriteInfo.pSprite )	
		return FALSE;

	BYTE	l_byPageNo = 0;
	int		l_nIconNumInLine;

	BYTE	l_byIconType = pi_dwClassifyID >> 28;

	if( l_byIconType == SKILL_ICON )
	{
		l_byPageNo = 1;
	}
	else if( l_byIconType == FORCE_ICON )
	{
		l_byPageNo = 2;
	}
	else if( l_byIconType == RECALL_ICON )
	{
		l_byPageNo = 24;
	}
	else if( l_byIconType == CLASS_SKILL_ICON )
	{
		l_byPageNo = 26;
	}
	else // ITEM_ICON
	{
		BYTE l_byItemType = _GetItemType( pi_dwClassifyID );
		switch( l_byItemType )
		{
		case IID_ARMOR:
			{
				BYTE l_byPageNoList[10] = { 13, 13, 14, 15, 16, 17, 19, 18, 19, 19 }; // armor세부항목들의 page no
				BYTE l_byItemExtType = _GetItemExtType( pi_dwClassifyID );

				if( l_byItemExtType > 9 )
					return FALSE;

				l_byPageNo = l_byPageNoList[l_byItemExtType];
			}
			break;

		case IID_WEAPON:
			{
				l_byPageNo = 0;
			}
			break;

		case IID_CONSUME:
			{	
				BYTE l_byItemExtType = _GetItemExtType( pi_dwClassifyID );				

				if( l_byItemExtType == IID_BULLET ||
					l_byItemExtType == IID_ARROW )
				{
					l_byPageNo = 4;
				}				
				else // potion
				{
					l_byPageNo = 5;
				}				
			}
			break;

		case IID_ETC:
			{
				// 64X64
				BYTE l_byPageNoList[15]    = { 7, 8, 12, 8, 9, 6, 10, 11, 11, 11, 11, 20, 21, 22, 25 };

				BYTE l_byItemExtType = _GetItemExtType( pi_dwClassifyID );

				if( l_byItemExtType > 14 )
					return FALSE;

				l_byPageNo = l_byPageNoList[l_byItemExtType];
			}
			break;

		case IID_FORCE:
			{
				BYTE l_byItemExtType = _GetItemExtType( pi_dwClassifyID );
				if( l_byItemExtType > IID_WIND )
				{
					l_byPageNo = 23;
				}
				else
				{
					l_byPageNo = 3;
				}
			}
			break;

		default :
			return FALSE;
		}
	}

	// page정보는 wActionNo의 상위 2byte에 넣는다.
	if( l_byPageNo >= po_spriteInfo.pSprite->GetPageNum() )
		return FALSE;
	po_spriteInfo.wActionNo = 0;
	po_spriteInfo.wActionNo = l_byPageNo << 8;
	po_spriteInfo.pSprite->InitPageSprite( pi_nWidth, pi_nWidth );
	po_spriteInfo.pSprite->SetPage( l_byPageNo );
	l_nIconNumInLine = po_spriteInfo.pSprite->GetSpriteOrgXL() / pi_nWidth;
	if( !l_nIconNumInLine )
		return FALSE;

	// actionNo( u )
	po_spriteInfo.wActionNo |= pi_dwID % l_nIconNumInLine;
	// frameNo( v )
	po_spriteInfo.wFrameNo = pi_dwID / l_nIconNumInLine;

	return TRUE;
}

BOOL	UnitPartsSearchFunc( DWORD pi_dwSpriteID, DWORD pi_dwPartsID, int pi_nWidth, SPRITE_INFO & po_spriteInfo )
{	
	if( !pi_nWidth )
		return FALSE;
	if( !po_spriteInfo.pSprite )
		return FALSE;

	if( pi_dwPartsID >= po_spriteInfo.pSprite->GetPageNum() )
		return FALSE;

	// page정보는 wActionNo의 상위 2byte에 넣는다.
	po_spriteInfo.wActionNo = 0;
	po_spriteInfo.wActionNo = pi_dwPartsID << 8;	

	// 64X64 in 256X256
	po_spriteInfo.pSprite->InitPageSprite( pi_nWidth, pi_nWidth );
	po_spriteInfo.pSprite->SetPage( pi_dwPartsID );
	int l_nIconNumInLine = po_spriteInfo.pSprite->GetSpriteOrgXL() / pi_nWidth;
	if( !l_nIconNumInLine )
		return FALSE;

	po_spriteInfo.wActionNo |= pi_dwSpriteID % l_nIconNumInLine;
	po_spriteInfo.wFrameNo = pi_dwSpriteID / l_nIconNumInLine;

	return TRUE;
}