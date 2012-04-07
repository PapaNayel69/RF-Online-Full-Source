////////////////////////////////////////////////////////////////////////////////
//
// CShopTradeBoard Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __SHOP_TRADE_BOARD_H__
#define __SHOP_TRADE_BOARD_H__

#include "../Common/CBoard.h"
#include "../../../UserInterface/GameWindow/Npc/GUIShopWindow.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define MAX_SHOP_TRADE_SOCKET ( MAX_INVENTORY_PACK * MAX_INVENTORY_SOCKET )

struct TRADE_INFO
{	
	BYTE			m_byBoardType;				// 거래할 Item이 속한 Board	
	BYTE			m_byItemPackNo;				// 거래할 Item의 pack number
	BYTE			m_byItemSocketNo;			// 거래할 Item의 socket number		

	CBoardItem		m_clsItem;

	TRADE_INFO()
	{
		m_byBoardType		= 0xFF;		
		m_byItemPackNo		= 0xFF;
		m_byItemSocketNo	= 0xFF;			
	}
};

typedef list< TRADE_INFO * >			CTradeInfoList;
typedef list< TRADE_INFO * >::iterator	CTradeInfoList_Iterator;

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CNetMsgProcessor_ItemTrade;
class CNetMsgProcessor_Unit;

class CShopTradeBoard : public CObjectBoard
{
	friend class CNpc;

public :

	enum { STM_BUY, STM_SELL, STM_REPAIR };	

protected :	

	BYTE						m_byTradeMode;				// 거래 모드

	DWORD						m_dwNpcIndex;				// 거래할 Npc의 Index
	CNpc *						m_pMerchant;		

	CTradeInfoList				m_listTradeInfo;			// 거래할 아이템 정보 리스트

	float						m_fGold;					// 거래할 아이템의 Gold 총 액
	float						m_fDalant;					// 거래할 아이템의 Dalant 총 액
															// 세율때문에 소수점 밑에 자리가 나올수가 있으므로 float 형태로 유지해야 한다.
															// 대신 Set, Get 함수들의 parameter나 return value는 DWORD로 유지한다.

	CGUIShopWindow				m_uiShopWindow;
	CNetMsgProcessor_ItemTrade*	m_pNMP_ItemTrade;
	CNetMsgProcessor_Unit *		m_pNMP_Unit;

public :

	CShopTradeBoard();
	virtual ~CShopTradeBoard();

			void			InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY );

			void			OpenShopWindow( DWORD pi_dwNpcIndex );		

protected :

	virtual	BOOL			Add_ProcessExistObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject );
	virtual	BOOL			Add_ProcessNewObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject );

	virtual	BOOL			Remove_ProcessRemoveQuantity( CBoardObject * pi_pOrgObject, BYTE pi_byQuantity );
	virtual	BOOL			Remove_ProcessRemoveObject( BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardObject * pi_pOrgObject );

	virtual	BOOL			UI_CheckBeforeAdd_CheckHoverObject( void );
	virtual	BOOL			UI_AddRequest_ProcessObject( void );
	virtual	void			UI_AddResult_ProcessObject( void );

	virtual	BOOL			UI_CheckBeforeRemove_CheckOriginalObject( BYTE pi_byQuantity );
	virtual	BOOL			UI_RemoveRequest_ProcessObject( BYTE pi_byQuantity );

	virtual	BOOL			UI_SwapRequest_ProcessObject( void )		{return FALSE; }
	virtual	void			UI_SwapResult_ProcessObject( void )			{;}

	virtual	BOOL			UI_UseRequest_ProcessObject( void )			{return FALSE; }
	virtual	void			UI_UseResult_ProcessObject( void )			{;}

public :

	inline	BYTE			GetTradeMode( void )						{ return m_byTradeMode; }
			void			SetTradeMode( BYTE pi_byTradeMode );

			void			SetMerchant( CNpc * pi_pMerchant );
	inline	DWORD			GetTradeCharIndex( void )					{ return m_dwNpcIndex; }
	inline	CNpc *			GetMerchant( void )							{ return m_pMerchant; }

	inline	BOOL			IsTrading( void )							{ return m_uiShopWindow.IsShow(); }
			
			void			ClearTradeList( void );
			void			CancelTrade( void );			
			void			ClearShopItemList( void );						

			BOOL			BuyItem_Request( void );
			void			BuyItem_Result( BOOL pi_bIsSuccessed );
			BOOL			SelectBuyItem( BYTE pi_byListIndex, BYTE pi_byNum, BYTE pi_byTarBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo );			
			BOOL			SelectBuyItem_CheckItem( CBoardItem * pi_pShopItem );
			BOOL			SelectBuyItem_CheckUnitItem( CBoardItem * pi_pShopItem );
			BOOL			DeselectBuyItem( BYTE pi_byOrgBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo );

			BOOL			SellItem_Request( void );
			void			SellItem_Result( BOOL pi_bIsSuccessed );
			BOOL			SelectSellItem( BYTE pi_byOrgBoardType, BYTE pi_byOrgPackNo, BYTE pi_byOrgSocketNo, CBoardItem * pi_pItem );
			BOOL			SelectSellItem_CheckUnitItem( CBoardItem * pi_pItem );
			BOOL			DeselectSellItem( DWORD pi_dwIndex, BYTE pi_byQuantity );
			
			BOOL			RepairItem_Request( void );
			BOOL			RepairItem_Result( BOOL pi_bIsSuccess );
			BOOL			SelectRepairItem( BYTE pi_byOrgBoardType, BYTE pi_byOrgPackNo, BYTE pi_byOrgSocketNo, CBoardItem * pi_pItem );
			BOOL			DeselectRepairItem( DWORD pi_dwIndex );
			
			void			UI_SelectItem_By_HotKey( BYTE pi_bySocketNo, BYTE pi_byQuantity );

protected :
			
			TRADE_INFO	*	AddTradeInfo( BYTE pi_byBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardItem * pi_pItem, BYTE pi_byQuantity );
			
			BOOL			RemoveTradeInfo( TRADE_INFO	* pi_pTradeInfo );
			BOOL			RemoveTradeInfoByIndex( DWORD pi_dwItemIndex );
			BOOL			RemoveTradeInfo( BYTE pi_byBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo );
			void			RemoveAllTradeInfo( void );

			TRADE_INFO	*	GetTradeInfo( DWORD pi_dwItemIndex );

			DWORD			GetMaxNumToBuy( CBoardItem * pi_pItem );

public :

	inline	CTradeInfoList*	GetTradeInfoList( void )					{ return &m_listTradeInfo; }

			CBoardItem *	GetTradeItem( BYTE pi_bySocketNo );
			CBoardItem *	GetTradeItemByIndex( DWORD pi_dwIndex );
			CBoardItem *	GetTradeItem( BYTE pi_byBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo );
			BOOL			IsTradingItem( BYTE pi_byBoardType, BYTE pi_byPackNo, BYTE pi_bySocketNo );
			BOOL			IsTradableItem( BYTE pi_byItemTypeID );
	inline	int				GetTotalTradeItemNum( void )				{ return m_listTradeInfo.size(); }

			CBoardItem *	GetShopItem( BYTE pi_bySocketNo );
			CBoardItem *	GetShopItemByIndex( DWORD pi_dwIndex );
			CBoardItem *	GetShopItemByMeshID( DWORD pi_dwMeshID );		

public :

			void			StoreListRequest( void );

			void			SetDalant( float pi_fDalant );
			void			SetGold( float pi_fGold );
	inline	DWORD			GetDalant( void )							{ return ( DWORD )m_fDalant; }
	inline	DWORD			GetGold( void )								{ return ( DWORD )m_fGold; }

public :		

	virtual	LRESULT			MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __SHOP_TRADE_BOARD_H__