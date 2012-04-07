////////////////////////////////////////////////////////////////////////////
//
// CGUIShopWindow Class Header
//
////////////////////////////////////////////////////////////////////////////

#ifndef __GUISHOPWINDOW_H__
#define __GUISHOPWINDOW_H__

#include "../../GUIController/GUIWindow.h"
#include "../../GUIController/GUIButton.h"
#include "../../GUIController/GUIItem.h"
#include "../../GUIController/GUIText.h"
#include "../../GUIController/GUINumInputDlg.h"
#include "../../UIGlobal.h"

#define	TOTAL_SHOP_ITEM_SOCKET		20

// ----------------------------------------------------------------
// messagebox user defined ID
#define	MSGBOX_ID_REPAIR			300
#define	MSGBOX_ID_CHANGE_TRADE		301
#define	MSGBOX_ID_SHOP_ETC			303

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct UI_SHOP_ITEM
{
	DWORD	dwIndex;				// index for client ( = serial )

	DWORD	dwMeshID;
	DWORD	dwIconID;
	DWORD	dwQuantity;
	BYTE	byUsableLevel;			// 사용가능한 레벨
};


typedef	list< TRADE_ITEM_INFO * >	CGUITradeItemList;
typedef	list< CGUIItem	* >			CGUIItemList;

/*//////////////////////////////////////////////////////////////////////////

[ CGUIShopWindow ]

//////////////////////////////////////////////////////////////////////////*/
class CGUIShopWindow : public CGUIWindow					  
{
public:
	enum ShopType {		
		Weapon,				// 무기와 탄창, 탄환, 화살 거래 가능
		Potion,				// 모든 포션 거래 가능
		Armor,				// 방패를 포함한 모든 방어구 거래 가능
		Sundries,			// 잡화상
		Unit,				// 유닛
		SecondHand = 7,		// 중고품 상인 ( 무엇이든 거래 가능 )
		List = 255,			// 리스트에 있는 아이템만을 거래 가능
	};

	enum TradeMode {
		Buy_Mode,
		Sell_Mode,
		Repair_Mode,
		None_Mode
	};

// < Data Member > ---------------------------------------------------------
private:

	ShopType				m_eCurShopType;
	TradeMode				m_eCurTradeMode;
	
	CGUIObjectAni			m_uiAniTitleBar;
	CGUIText				m_uiTitle;

	//---------------------------------------------------------------------------------
	// Trade button
	CGUIButtonEx			m_uiTradeTab[3];

	// Page up, down	
	CGUIObject				m_uiPageBoard;
	CGUIButton				m_uiPageUp;
	CGUIButton				m_uiPageDown;		
	CGUIText				m_uiDalant;
	CGUIText				m_uiGold;

	// ok, cancel button
	CGUIButtonEx			m_uiOKButton;
	CGUIButton				m_uiCancelButton;
	CGUIButton				m_uiCloseButton;
	CGUIButton				m_uiHelpButton;

	//---------------------------------------------------------------------------------
	// Shop Board
	CGUISocket				m_uiShopItemBoard[TOTAL_SHOP_ITEM_SOCKET];
	BYTE					m_byCurPageNo;								// 현재 page
	BYTE					m_byTotalPageNo;
	CGUIText				m_uiCurPageStr;
	
	// Shop(Buy) item list
	CGUIItemList			m_listBuyItem;								// 판매 아이템 목록	
	
	
	CGUITradeItemList		m_listTradeItem;							// 선택된 item list( Sell, Repair Item )

	CGUIText				m_uiTradeMoneyTitle;
	CGUIText				m_uiTotalTradeDalantStr;					// 총 거래 가격 ( Dalant )
	CGUIText				m_uiTotalTradeGoldStr;						// 총 거래 가격 ( Gold )	
	
		
	TradeMode				m_eSelectedTradeMode;						// trade mode를 바꿀 때 msgbox뜨기전에 선택한 mode 백업
	BYTE					m_bySelectedSocketNo;						// 갯수입력받을 item의 index


	BOOL					m_bIsCreated;


// < Member Function > -----------------------------------------------------
public:
	CGUIShopWindow();
	~CGUIShopWindow();

			void		Init( void );
			void		SetSprite( void );	
			
			void		Close( void );


			void		SetShopType( ShopType pi_eShopType );
	inline	ShopType	GetShopType( void )						{ return m_eCurShopType; }

			void		SetTradeMode( TradeMode pi_eTrade );
	inline	TradeMode	GetCurrentTradeMode( void )				{ return m_eCurTradeMode; }
	

	// ---------------------------------------------------------------------------------
	// Buy Item						
			BOOL		TakeInBuyItem( UI_SHOP_ITEM * pi_pItemInfo, BOOL pi_bInsertAtBegin = FALSE );
			BOOL		TakeOutBuyItem( DWORD pi_dwIndex );	

			void		ClearBuyItemList( void );						// clear list

			WORD		GetBuyItemIndex( CGUIItem * pi_pItem );
			CGUIItem *	GetBuyableItem( WORD pi_wIndex );
			CGUIItem *	GetBuyableItemByID( DWORD pi_dwItemID );


	// ---------------------------------------------------------------------------------
	// Trade Item
			void		TakeInTradeItem( DWORD pi_dwIndex, DWORD pi_dwItemID, DWORD pi_dwSpriteID, BYTE pi_byQuantity );
			void		TakeOutTradeItem( DWORD pi_dwIndex );

			void		ClearTradeItemList( void );
	
			DWORD		GetTradeItemIndex( BYTE pi_bySocketNo );
			CGUIItem *	GetTradeItemByIndex( DWORD pi_dwIndex );


	// ---------------------------------------------------------------------------------
	// Shop Board
			void		UpdateShopBoard( void );						// shop board에 들어가는 item정보가 바뀌었을 때 update해줘야한다.
			void		ClearShopBoard( void );							// shop socket에 아이템을 모두 지운다.

			void		SetCurrentShopBoard( BYTE pi_byPage );			// page up, down 처리
	inline	WORD		GetCurPageNo( void )					{ return m_byCurPageNo; }
										


			void		SetDalant( DWORD pi_dwDalant );
			void		SetGold( DWORD pi_dwGold );

			void		SetEnableTrade( BOOL pi_bEnable );	


			void		MouseEvent( _mouse_event & event );	
			void		ClosingDialogBox( CGUIDialogBox * pi_pDlg );			

private:
	
			void		TabLabelPosSetting( TradeMode pi_eTradeMode, BOOL pi_bIsActive );

};

#endif // __GUISHOPWINDOW_H__