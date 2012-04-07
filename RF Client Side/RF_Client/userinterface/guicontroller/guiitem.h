////////////////////////////////////////////////////////////////////////////
//
// CGUIItem Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUIITEM_H__
#define __GUIITEM_H__

#include "GUIObject.h"
//#include "../GameWindow/GUIForceBeltWindow.h"

/////////////////////////////////////////////////////////////////////////////
////////////////////////                          ///////////////////////////
////////////////////////    아이템 식별 코드      ///////////////////////////
////////////////////////                          ///////////////////////////
/////////////////////////////////////////////////////////////////////////////

// Item ID

// 아이템 사용 주체 
#define	IID_BELLATOR		0x07	// 고대 공용
#define	IID_CORA			0x08	// 이계 공용
#define	IID_BELMALE			0x00	// 고대 남자
#define	IID_BELFEMALE		0x01	// 고대 여자
#define	IID_CORAMALE		0x02	// 이계 남자
#define	IID_CORAFEMALE		0x03	// 이계 여자
#define	IID_BELCOR			0x09	// 고대 이계 공용
#define	IID_MALE			0x05	// 남자 공용
#define	IID_FEMALE			0x06	// 여자 공용
#define	IID_ACCRETIA		0x04	// 과학
#define	IID_COM				0x0A	// 전체 공용

// 아이템 종류
#define	IID_ARMOR			0x00	// 방어구
#define	IID_WEAPON			0x01	// 무기
#define	IID_CONSUME			0x02	// 소모 아이템
#define	IID_ETC				0x03	// 기타
#define	IID_FORCE			0x04	// 포스
#define	IID_DEFAULT			0x05	// 디폴트

// 아이템 세부 종류 (방어구)
#define	IID_HELMET			0x00
#define	IID_FACE			0x01
#define	IID_UPPER			0x02
#define	IID_LOWER			0x03
#define	IID_GLOVES			0x04
#define	IID_SHOES			0x05
#define	IID_CLOAK			0x06
#define	IID_LSHIELD			0x07
#define	IID_AMULET			0x08
#define	IID_RING			0x09

// 아이템 세부 종류 (무기) 
#define	IID_RSWORD			0x00	// 오른손검
#define	IID_TSWORD			0x01	// 양손검
#define	IID_DSWORD			0x02	// 쌍손검
#define	IID_RKNIFE			0x03	// 오른손 나이프
#define	IID_TKNIFE			0x04	// 양손 나이프
#define	IID_RKNIFETHROW		0x05	// 오른손 나이프던
#define	IID_RAXE			0x06
#define	IID_TAXE			0x07
#define	IID_RAXETHROW		0x08
#define	IID_DAXE			0x09
#define	IID_RMACE			0x0A
#define	IID_TMACE			0x0B
#define	IID_RSTAFF			0x0C
#define	IID_TSTAFF			0x0D
#define	IID_TSPEAR			0x0E
#define	IID_TBOW			0x0F
#define	IID_TCROSSBOW		0x10
#define	IID_RGUN			0x11
#define	IID_DGUN			0x12
#define	IID_TRIFLE			0x13
#define	IID_TLAUNCHER		0x14
#define	IID_TFAUST			0x15
#define	IID_TMACHINEGUN		0x16
#define	IID_TFLAMETHROWER	0x17
#define	IID_RBEAMGUN		0x18
#define	IID_DBEAMGUN		0x19
#define	IID_TBEAMRIFLE		0x1A
#define	IID_TPLASMAGUN		0x1B
#define	IID_MININGTOOL		0x1C


// 아이템 세부 종류 (소모아이템)
#define	IID_HP				0x00	// HP
#define	IID_FP				0x01	// FORCE
#define	IID_SP				0x02	// STAMINA
#define	IID_BULLET			0x03	// 탄환
#define	IID_ARROW			0x04	// 화살
#define	IID_ADRENALINE		0x05	// 아드레날린		

// 아이템 세부 종류 (기타)
#define	IID_ORE				0x00
#define	IID_RESOURCE		0x01
#define	IID_BOOTY			0x02
#define	IID_GEM				0x03
#define	IID_TOOL			0x04
#define	IID_BAG				0x05
#define	IID_BATTERY			0x06
#define	IID_UNIT_KEY		0x07
#define	IID_BEL_TOWER_ARM	0x08
#define	IID_COR_TOWER_ARM	0x09
#define	IID_ACC_TOWER_ARM	0x0A
#define	IID_MAP				0x0B
#define	IID_TOWN			0x0C
#define	IID_DUNGEON			0x0D
#define	IID_GUARD_TOWER		0x0E

// 아이템 세부 종류 (포스)
#define	IID_DARK			0x00
#define	IID_HOLY			0x01
#define	IID_FIRE			0x02
#define IID_AQUA			0x03
#define	IID_TERRA			0x04
#define	IID_WIND			0x05
#define	IID_ANIMUSLIVER		0x06

// 아이템 세부 종류 (디폴트)
//


/////////////////////////////////////////////////////////////////////////////
////////////////////////                          ///////////////////////////
////////////////////////    기타 아이템 타입	  ///////////////////////////
////////////////////////                          ///////////////////////////
/////////////////////////////////////////////////////////////////////////////


#define	NONE_ITEM_ID			0xFFFFFFFF


// 무기 잡는 타입
#define	RIGHT_HAND_WEAPON		0	// 오른손 타입
#define	BOTH_HAND_WEAPON		1	// 양손 타입
#define	DUAL_HAND_WEAPON		2	// 쌍손타입


// 아이콘 타입
#define	ITEM_ICON				0
#define	SKILL_ICON				1
#define	CLASS_SKILL_ICON		2
#define	FORCE_ICON				3
#define	RECALL_ICON				4
#define	UNIT_ICON				5

// 사용 가능 타입
#define	UNCHECKED_USABLE		0xFF
#define	NONE_LEVEL				0xFF


// Sprite Size Type
#define	SST_32BY32				32
#define	SST_64BY64				64


/*//////////////////////////////////////////////////////////////////////////

[ CGUIItem ]

//////////////////////////////////////////////////////////////////////////*/
class CGUIItem : public CGUIObject
{
// < Data Member > ---------------------------------------------------------
public:
	DWORD		m_dwItemID;

	DWORD		m_dwSpriteID;

	BYTE		m_byIconType;		// 일반 item과 skill, force icon 구분	

	BYTE		m_bySpriteSize;		// item 하나의 실제 sprite 크기

	WORD		m_wNum;				// item갯수	

	CGUIItem *	m_pRefItem;			// 참조 item

	BOOL		m_bDrawItemNum;		// 아이템 갯수를 보여줄것인가

	BYTE		m_byUsableLevel;	// 사용 가능한 레벨

	BYTE		m_byUsable;			// 사용할수 있는가?

// < Member Function > -----------------------------------------------------
public:
	CGUIItem();
	~CGUIItem();

			void		SetItemID( DWORD pi_dwItemID );
			void		SetItemID( BYTE pi_btUser, BYTE pi_bType, BYTE pi_btExtType, BYTE pi_btID );
	inline	DWORD		GetItemID( void )							{ return m_dwItemID; }
			
	inline	void		SetSpriteID( DWORD pi_dwSpriteID )			{ m_dwSpriteID = pi_dwSpriteID; }
	inline	DWORD		GetSpriteID( void )							{ return m_dwSpriteID; }

			void		SetIconType( BYTE pi_byType );
	inline	BYTE		GetIconType( void )							{ return m_byIconType; }				

	inline	void		SetNum( WORD pi_wNum )						{ m_wNum = pi_wNum; }
	inline	BYTE		GetNum( void )								{ return m_wNum; }

	inline	void		SetRefItem( CGUIItem * pi_pRefItem )		{ m_pRefItem = pi_pRefItem; }
	inline	CGUIItem *	GetRefItem( void )							{ return m_pRefItem; }

	inline	void		SetUsableLevel( BYTE pi_byLevel )			{ m_byUsableLevel = pi_byLevel; }
	inline	BYTE		GetUsableLevel( void )						{ return m_byUsableLevel; }

	inline	void		SetUsable( BOOL pi_bUsable )				{ m_byUsable = pi_bUsable; }
	inline	BOOL		IsUsableChecked( void )						{ return (m_byUsable == UNCHECKED_USABLE ? false : true); }
	inline	BOOL		IsUsable( void )							{ return m_byUsable; }
			
			void		SetData( CGUIItem * pi_pItem );	
	virtual	void		SetSprite( void );							// itemID와 spriteID가 다 세팅된 다음 호출해야한다.

	inline	void		DrawItemNum( BOOL pi_bDraw )				{ m_bDrawItemNum = pi_bDraw; }
	virtual	BOOL		Draw( void );
	virtual	BOOL		Draw( RECT & pi_rtClipping );
			
			void		SetFocus( void )							{;}	// 아이템에 포커스가 안오도록 한다.

	inline	BOOL		IsEmpty( void )								{ return ( m_dwItemID == NONE_ITEM_ID ); }
			void		SetEmpty( void );

	virtual	void		BeginDelayAnimation( DWORD pi_dwTime )		{;}
};

/*//////////////////////////////////////////////////////////////////////////

  [ CGUISFItem ]

 //////////////////////////////////////////////////////////////////////////*/
class CGUISFItem : public CGUIItem
{
// < Data Member > ---------------------------------------------------------
public:

	CGUITimer		m_uiDelay;

// < Member Function > -----------------------------------------------------
public :

	CGUISFItem();
	virtual ~CGUISFItem();

	virtual	void		BeginDelayAnimation( DWORD pi_dwTime );
	inline	BOOL		IsDelayAnimation( void )		{ return m_uiDelay.IsStarted(); }

	virtual	void		SetPos( POINT & pi_ptPos );
	virtual	void		SetPos( LONG pi_nPosX, LONG pi_nPosY );

	virtual	void		SetSize( LONG pi_nWidth, LONG pi_nHeight );
	virtual	void		SetSize( POINT & pi_ptSize );

	virtual	BOOL		Draw( void );
	virtual	BOOL		Draw( RECT & pi_rtClipping );
};

class CGUIAnimusItem : public CGUISFItem
{

// < Member Function > -----------------------------------------------------
public :

	CGUIAnimusItem();
	virtual ~CGUIAnimusItem();
};

/*//////////////////////////////////////////////////////////////////////////

[ CGUISocket ]

//////////////////////////////////////////////////////////////////////////*/
class CGUISocket : public CGUIObject
{
// < Data Member > ---------------------------------------------------------
public:

	CGUIItem *		m_pItem;

// < Member Function > -----------------------------------------------------
public :

	CGUISocket();
	virtual ~CGUISocket();	

			void		SetItem( CGUIItem * pi_pItem );
	inline	CGUIItem *	GetItem( void )						{ return m_pItem; }

	inline	BOOL		IsEmpty( void )						{ return ( m_pItem == NULL ); }
	inline	void		SetEmpty( void )					{ m_pItem = NULL; }


	virtual	void		SetPos( POINT & pi_ptPos );
	virtual	void		SetPos( LONG pi_nPosX, LONG pi_nPosY );

	virtual	BOOL		Draw( void );
	virtual	BOOL		Draw( RECT & pi_rtClipping );
};


/////////////////////////////////////////////////////////////////////////////
////////////////////////                          ///////////////////////////
////////////////////////     아이템  식별 함수	  ///////////////////////////
////////////////////////                          ///////////////////////////
/////////////////////////////////////////////////////////////////////////////

BOOL	_IsItemUser( DWORD & pi_dwItemID, BYTE pi_btType );
BOOL	_IsItemType( DWORD & pi_dwItemID, BYTE pi_btType );
BOOL	_IsItemExtType( DWORD & pi_dwItemID, BYTE pi_btType );
BOOL	_IsItemRealID( DWORD & pi_dwItemID, BYTE pi_btID );

BYTE	_GetItemUser( DWORD & pi_dwItemID );
BYTE	_GetItemType( DWORD & pi_dwItemID );
BYTE	_GetItemExtType( DWORD & pi_dwItemID );
BYTE	_GetItemRealID( DWORD & pi_dwItemID );


// --------------------------------------------------------------------------------------
BOOL	_IsUsableItem( CGUIItem * pi_pItem, BYTE pi_byRaceType, BYTE pi_byLevel = 0xFF );

BYTE	_GetWeapnHandType( DWORD pi_dwItemID );

BOOL	_IsAddableItem( DWORD pi_dwItemID );



#endif // __GUIITEM_H__
