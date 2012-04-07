////////////////////////////////////////////////////////////////////////////
//
// CSpriteMgr Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __SPRITEMGR_H__
#define __SPRITEMGR_H__

#include "Singleton.h"

#include "GUIController/GUIObject.h"
#include <list>

using namespace std ; 

/////////// Sprite ID //////////////////////
#define	SP_ID_MOUSE			0x00
#define	SP_ID_COMMON 		0x01
#define	SP_ID_GAUGE_EXP		0x02
#define SP_ID_BELT			0x03
#define	SP_ID_INVENTORY		0x04
#define	SP_ID_CHAT			0x05
#define	SP_ID_SHOP			0x06
#define SP_ID_LOGIN			0x07
#define SP_ID_LOADING		0x08
#define SP_ID_ITEM			0x09
#define	SP_ID_ORE_PROC		0x0A
#define	SP_ID_MINING		0x0B
#define	SP_ID_EXCHANGE		0x0C
#define	SP_ID_RECALL		0x0D
#define	SP_ID_ITEM_MAKE		0x0E
#define	SP_ID_ITEM_UPGRADE	0x0F
#define	SP_ID_PARTY			0x10
#define	SP_ID_TRADE			0x11
#define	SP_ID_MAPLOADING	0x12
#define	SP_ID_DUNGEON		0x13
#define	SP_ID_UNIT			0x14
#define	SP_ID_UNIT_PARTS	0x15
#define	SP_ID_CLASS			0x16
#define	SP_ID_MINIMAP		0x17



////////////////////////////////////////////

/////////// Sprite Extend ID //////////
#define	SPE_ID_OK_BUTTON1			0x01
#define	SPE_ID_OK_BUTTON2			0x02
#define	SPE_ID_OK_BUTTON3			0x03
#define	SPE_ID_CANCEL_BUTTON1		0x04
#define	SPE_ID_CANCEL_BUTTON2		0x05
#define	SPE_ID_CANCEL_BUTTON3		0x06
#define	SPE_ID_CLOSE_BUTTON1		0x07
#define	SPE_ID_CLOSE_BUTTON2		0x08
#define	SPE_ID_CLOSE_BUTTON3		0x09

#define	SPE_ID_UP_BUTTON1			0x10
#define	SPE_ID_UP_BUTTON2			0x11
#define	SPE_ID_DOWN_BUTTON1			0x12
#define	SPE_ID_DOWN_BUTTON2			0x13

// input dialog
#define	SPE_ID_INPUT_DLG_BASE		0x40
#define	SPE_ID_INPUT_DLG_BORDER		0x41

// message box
#define	SPE_ID_MSG_BOX_BASE			0x50
#define	SPE_ID_MSG_BOX_BORDER1		0x51
#define	SPE_ID_MSG_BOX_BORDER2		0x52
#define	SPE_ID_MSG_BOX_BORDER3		0x53

// tooltip
#define	SPE_ID_TOOLTIP_BORDER1		0x60
#define	SPE_ID_TOOLTIP_BORDER2		0x61
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
struct _sprite_info_DB {
	DWORD			dwID;
	_sprite_info	spriteInfo;	
};

typedef _sprite_info_DB		SPRITE_INFO_DB;

typedef	BOOL	(*SearchFunc)	( DWORD pi_dwID, DWORD pi_dwReserve, int pi_nWidth, SPRITE_INFO & po_spriteInfo );

/*//////////////////////////////////////////////////////////////////////////

[ CSpriteUnit ]

//////////////////////////////////////////////////////////////////////////*/
class CSpriteUnit
{
public:
	typedef list<SPRITE_INFO_DB *>	CSpriteInfoDBList;
public:
	WORD				m_wID;
	CSprite	*			m_pSprite;

	CSpriteInfoDBList	m_listSpriteInfoDB;	

	SearchFunc			m_pSearchFunc;	// _sprite_info를 찾는 함수
	
public:
	CSpriteUnit();
	~CSpriteUnit();

	void			Create( WORD pi_wSpriteID, char * pi_pFileName );
	void			Destroy( void );	
	void			AddSpriteInfoDB( DWORD dwID, WORD wActionNo, WORD wFrameNo );

	SPRITE_INFO	*	GetSpriteInfo( WORD	pi_wSpriteExID );
};

/*//////////////////////////////////////////////////////////////////////////

[ CSpriteMgr ]

//////////////////////////////////////////////////////////////////////////*/
class CSpriteMgr : public CSingleton<CSpriteMgr>
{
public:
	typedef list< CSpriteUnit * >		CSpriteUnitList;

// < Data Member > ---------------------------------------------------------
private:
	CSpriteUnitList		m_listSpriteUnit;

// < Member Function > -----------------------------------------------------
public:
	CSpriteMgr();
	~CSpriteMgr();

	void		Init( void );

	void		LoadSprite( WORD pi_wSpriteID );
	void		LoadSprite( WORD pi_wSpriteID, char * pi_pFilePathName );
	void		UnloadSprite( WORD pi_wSpriteID );
	void		UnloadSpriteAll( void );

	CSprite		*	GetSprite( WORD	pi_wSpriteID );
	SPRITE_INFO	*	GetSpriteInfo( WORD pi_wSpriteID, WORD pi_wSpriteExID );	

	void		GetSpriteInfo( WORD pi_wSpriteID, DWORD pi_dwSpriteExID, SPRITE_INFO & po_SpriteInfo );
	void		GetSpriteInfo( WORD pi_wSpriteID, DWORD pi_dwID, DWORD pi_dwClassifyID, int pi_nWidth, SPRITE_INFO & po_SpriteInfo );
};

BOOL	DefaultSearchFunc( DWORD pi_dwID, DWORD pi_wReserve, int pi_nWidth, SPRITE_INFO & po_spriteInfo );
BOOL	ItemSearchFunc( DWORD pi_dwID, DWORD pi_dwClassifyID, int pi_nWidth, SPRITE_INFO & po_spriteInfo );
BOOL	UnitPartsSearchFunc( DWORD pi_dwSpriteID, DWORD pi_dwPartsID, int pi_nWidth, SPRITE_INFO & po_spriteInfo );

#define	gSpriteMgr	CSpriteMgr::GetSingleton()

#endif // __SPRITEMGR_H__
