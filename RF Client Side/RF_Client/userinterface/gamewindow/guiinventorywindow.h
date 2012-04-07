////////////////////////////////////////////////////////////////////////////
//
// CGUIInventoryWindow Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUIINVENTORYWINDOW_H__
#define __GUIINVENTORYWINDOW_H__

#include "../GUIController/GUIWindow.h"
#include "../GUIController/GUIButton.h"
#include "../GUIController/GUIItem.h"
#include "../GUIController/GUIText.h"
#include "../GUIController/GUIScreen.h"
#include "Npc/GUIShopWindow.h"
#include "GUIItemBeltWindow.h"


#define	TOTAL_PACK			5
#define	TOTAL_INVENTORY		20
#define	TOTAL_EQUIP_ITEM	14


#define	BUY_ITEM_PACK_INDEX			100
#define	EQUIP_PACK_INDEX			101
#define	INPUT_ORE_PACK_INDEX		102
#define	OUTPUT_ORE_PACK_INDEX		103
#define	UPGRADE_PACK_INDEX			104
#define	BELT_PACK_INDEX				105

/*//////////////////////////////////////////////////////////////////////////

[ CGUIInventoryBoard ]
 
   - 배낭창

//////////////////////////////////////////////////////////////////////////*/
class CGUIInventoryBoard : public CGUIWindow						
{
friend class CGUIInventoryWindow;
// < Data Member > ---------------------------------------------------------
private:	
	BYTE					m_byPackID;

	CGUIText				m_uiTitle;	
	CGUIButton				m_uiCloseButton;	
	
	CGUIItem				m_uiItem[TOTAL_INVENTORY];				//  inventory item	

	CGUIObject				m_uiUselessItemMark[TOTAL_INVENTORY];	// 사용할수 없는 아이템표시
	CGUIObject				m_uiUselessSocketMark[TOTAL_INVENTORY];	// 사용할수없는 소켓표시
	CGUIObject				m_uiTradeItemMark[TOTAL_INVENTORY];		// 거래할 아이템 표시

	CGUIObject				m_uiSelectedSocket;						// 아이템 넣을 곳으로 선택된 소켓
	BYTE					m_bySelectedSocketIndex;				// 선택된 소켓
	
	CGUIInventoryWindow	*	m_pInventoryWin;

	BOOL					m_bIsClosedWidhEquipWin;				// 장착창이랑 같이 닫혔는가

	BOOL					m_bCreated;
	
// < Member Function > -----------------------------------------------------
public:

	CGUIInventoryBoard();
	virtual ~CGUIInventoryBoard();
	
			void		Init( BYTE pi_byPackID );
			void		SetSprite( void );			

			void		Open( void );
			void		Close( void );

	inline	void		SetInventoryWindow( CGUIInventoryWindow * pi_pWin )	{ m_pInventoryWin = pi_pWin; }	
	
			BOOL		TakeInItem( BYTE pi_bySocketIndex, DWORD pi_dwItemID, DWORD pi_dwSpriteID, BYTE pi_byNum, BYTE pi_byUsableLevel = NONE_LEVEL );
			BOOL		TakeInItem( BYTE pi_bySocketIndex, CGUIItem * pi_pItem );

			void		SetTradeItem( BYTE pi_bySocketIndex, BOOL pi_bIsTradeItem );

			BOOL		TakeOutItem( BYTE pi_bySocketIndex, BYTE pi_byNum = 0, CGUIItem * po_pItem = NULL );		
			
	inline	BOOL		IsEmpty( BYTE pi_bySocketIndex )					{ return ( m_uiItem[pi_bySocketIndex].GetItemID() == NONE_ITEM_ID ); }			
			void		SetEmpty( BYTE pi_bySocketIndex );

			CGUIItem *	GetItem( WORD pi_wSocketIndex );
			
			void		SetUselessSocket( BYTE pi_bySocketIndex, BOOL pi_bFlag );
			void		SetUselessItem( BYTE pi_bySocketIndex, BOOL pi_bFlag );
			
			void		MouseEvent( _mouse_event & event );	
			void		ClosingDialogBox( CGUIDialogBox * pi_pDlg );

			void		UpdateUselessItemByLevel( void );

			void		PickingObjectEvent( BOOL pi_bPick );

			BOOL		Draw( void );


	inline	void		SetClosedWithEquipWin( BOOL pi_bFlag )				{ m_bIsClosedWidhEquipWin = pi_bFlag; }
		

protected :	
			
			void		SetSelectedSocket( WORD pi_wSocketIndex );		// 아이템을 선택된 소켓에 넣을 수 있는지 표시해준다.
			void		UnsetSelectedSocket( void );		
};


/*//////////////////////////////////////////////////////////////////////////

[ CGUIEquipWindow ]
 
   - 장착창

//////////////////////////////////////////////////////////////////////////*/
class CGUIEquipWindow : public CGUIItemWindow
{
public :
	enum EquipItemBoard {
		Helmet,
		Upper,
		Lower,
		Weapon,
		Shoes,
		Cloak,
		Shield,
		Glove,
		LRing, // 장신구
		RRing,
		LAmulet,
		RAmulet,
		LBullet,
		RBullet
	};

// < Data Member > ---------------------------------------------------------
private :
	CGUIItem				m_uiEquipItem[TOTAL_EQUIP_ITEM];			// 장착 아이템

	CGUIObject				m_uiEmptyEquipMark[TOTAL_EQUIP_ITEM];		// 빈 소켓 표시	
	CGUIObject				m_uiUselessSocketMark[TOTAL_EQUIP_ITEM];	// 사용할수없는 소켓표시
	CGUIObject				m_uiTradeItemMark[TOTAL_EQUIP_ITEM];		// 거래할 아이템 표시

	CGUIObjectAni			m_uiEquipAbleSocket;						// 장착할수 있는 소켓 표시
	CGUIObjectAni			m_uiEquipAbleSocketEx;						// Bullet2
	BYTE					m_byEquipAbleIndex;
	BYTE					m_byEquipAbleIndexEx;

	CGUIObject				m_uiSelectedSocket;							// 아이템 넣을 곳으로 선택된 소켓	
	BYTE					m_bySelectedSocketIndex;					// 선택된 소켓

	CGUIObject				m_uiDisableEquipShield;						// 양손,쌍손무기일 때 장착할수없는 소켓 표시


	CGUIInventoryWindow *	m_pInventoryWin;
	

	BOOL					m_bIsCreated;
	

// < Member Function > -----------------------------------------------------
public :

	CGUIEquipWindow();
	virtual ~CGUIEquipWindow();

			void		Init( void );
			void		SetSprite( void );

	inline	void		SetInventoryWin( CGUIInventoryWindow * pi_pWin )	{ m_pInventoryWin = pi_pWin; }

	virtual	void		Open( void );
	virtual	void		Close( void );
		
	virtual	BOOL		TakeInItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex,
									DWORD pi_dwItemID, DWORD pi_dwSpriteID, BYTE pi_byNum, BYTE pi_byUsableLevel = NONE_LEVEL );
	virtual	BOOL		TakeOutItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex, BYTE pi_byNum = 0, CGUIItem * po_pOutItem = NULL );	

	virtual	void		SetTradeItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex, BOOL pi_bIsTradeItem );

	virtual	CGUIItem *	GetItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex );

	virtual	void		SetUselessSocket( BYTE pi_byPackIndex, BYTE pi_bySocketIndex, BOOL pi_bFlag );


	inline	BOOL		IsEmpty( BYTE pi_bySocketIndex )					{ return ( m_uiEquipItem[pi_bySocketIndex].GetItemID() == NONE_ITEM_ID ); }
			void		SetEmpty( BYTE pi_bySocketIndex );

			void		MouseEvent( _mouse_event & event );
			void		PickingObjectEvent( BOOL pi_bPick );

	virtual	BOOL		Draw( void );

protected :

			BOOL		IsEquipableItem( EquipItemBoard pi_eItemBoard, DWORD pi_dwItemID );	
			
			void		SetSelectedSocket( BYTE pi_bySocketIndex );		// 아이템을 선택된 소켓에 넣을 수 있는지 표시해준다.
			void		UnsetSelectedSocket( void );
			
			void		SetEquipAbleSocket( int pi_nEquipAbleIndex );	// 장착할수 있는 소켓을 표시해준다.
			void		UnsetEquipAbleSocket( void );			
};


/*//////////////////////////////////////////////////////////////////////////

[ CGUIInventoryWindow ]

//////////////////////////////////////////////////////////////////////////*/
class CGUIInventoryWindow : public CGUIItemWindow
{
// < Data Member > ---------------------------------------------------------
protected:		
	
	CGUIObjectAni			m_uiAniTitleBar;
	CGUIText				m_uiTitle;
	CGUIButton				m_uiCloseButton;
	CGUIButton				m_uiHelpButton;


	CGUIEquipWindow		*	m_pEquipWindow;

	
	CGUIText				m_uiDalantTitle;
	CGUIText				m_uiGoldTitle;
	CGUIText				m_uiDalant;
	CGUIText				m_uiGold;
	

	CGUIInventoryBoard	*	m_pPackWindow[TOTAL_PACK];			// Pack Window
	CGUIObject				m_uiPackButton[TOTAL_PACK];			// Pack Button	
	CGUIButton				m_uiPackOpenOption;					// 배낭 옵션


	//---------------------------------------------------------------------------------

	// money
	DWORD					m_dwOwnDalant;
	DWORD					m_dwOwnGold;	

	// pack	
	BOOL					m_bOpenOnlyOnePack;					// 배낭을 하나만 열게 할것인가


	BOOL					m_bIsCreated;	

public:
	BYTE					m_byLastOpenedPack;					// 최근에 열었던 pack id

	BYTE					m_byInputDlg_PackNo;				// 갯수입력창이 뜬 pack no, socket no
	BYTE					m_byInputDlg_SocketNo;

	BOOL					m_bIsOpened;

// < Member Function > -----------------------------------------------------
public:
	CGUIInventoryWindow();
	~CGUIInventoryWindow();

	
			void		Init( void );
			void		SetSprite( void );			

			void		Open( void );
			void		Close( void );


			void		SetEquipWindow( CGUIEquipWindow * pi_pWindow );
	

	//---------------------------------------------------------------------------------
	// Pack control
			BOOL		AddPack( BYTE pi_byPackIndex, BOOL pi_bNewPack = FALSE );
			void		RemovePack( BYTE pi_byPackIndex );
			void		OpenPack( BYTE pi_byPackIndex, BOOL pi_bOpen );			
			void		SetPackState( BYTE pi_byPackIndex, BOOL pi_bOpen );
			BOOL		IsOpenedPack( BYTE pi_byPackIndex );
			

			void		OpenDockingPack( BOOL pi_bOpen );		

			void		SetOpenOnlyOnePack( BOOL pi_bOneOpen );
	inline	BOOL		IsOpenOnlyOnePack( void )		{ return m_bOpenOnlyOnePack; }

	inline	BYTE		GetLastOpenedPack( void )		{ return m_byLastOpenedPack; }


	// --------------------------------------------------------------------------------
	// 배낭창
	virtual	BOOL		TakeInItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex,
									DWORD pi_dwItemID, DWORD pi_dwSpriteID, BYTE pi_byNum, BYTE pi_byUsableLevel = NONE_LEVEL );
	virtual	BOOL		TakeOutItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex, BYTE pi_byNum = 0, CGUIItem * po_pOutItem = NULL );	

	virtual	void		SetTradeItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex, BOOL pi_bIsTradeItem );

	virtual	CGUIItem *	GetItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex );			

	virtual	void		SetUselessSocket( BYTE pi_byPackIndex, BYTE pi_bySocketIndex, BOOL pi_bFlag );
	

	//---------------------------------------------------------------------------------
	// 장착창
			BOOL		Equip( BYTE pi_bySocketIndex, DWORD pi_dwItemID, DWORD pi_dwSpriteID );			
			BOOL		UnEquip( BYTE pi_bySocketIndex );		


	//---------------------------------------------------------------------------------
	// gold, dalant
			void		SetOwnDalant( DWORD pi_dwDalant );
			void		SetOwnGold( DWORD pi_dwGold );
			
	inline	DWORD		GetOwnDalant( void )			{ return m_dwOwnDalant; }
	inline	DWORD		GetOwnGold( void )				{ return m_dwOwnGold; }
	inline	char *		GetDalantStr( void )			{ return m_uiDalant.GetString(); }
	inline	char *		GetGoldStr( void )				{ return m_uiGold.GetString(); }
	

	//---------------------------------------------------------------------------------			
	// event proc
			void		MouseEvent( _mouse_event & event );			
			void		ClosingDialogBox( CGUIDialogBox * pi_pDlg );

			void		UpdateUselessItemByLevel( void );

			void		PickingObjectEvent( BOOL pi_bPick );

	//---------------------------------------------------------------------------------		

	inline	CGUIObject* GetPack( BYTE pi_byPackIndex )									{ return &m_uiPackButton[pi_byPackIndex]; }
	inline	CGUIItem *	GetInventoryItem( BYTE pi_byPackIndex, BYTE pi_bySocketIndex )	{ return m_pPackWindow[pi_byPackIndex]->GetItem( pi_bySocketIndex ); }
	inline	CGUIItem *	GetEquipItem( BYTE pi_byEquipIndex )							{ return m_pEquipWindow->GetItem( 0, pi_byEquipIndex ); }
	inline	CGUIInventoryBoard * GetPackWindow( WORD pi_wPackIndex )					{ return m_pPackWindow[pi_wPackIndex]; }	
};

#endif // __GUIINVENTORYWINDOW_H__