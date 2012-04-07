////////////////////////////////////////////////////////////////////////////////
//
// CItemBoard Class Header
//
////////////////////////////////////////////////////////////////////////////////
#ifdef _NEW_UI_

#ifndef __ITEM_BOARD_H__
#define __ITEM_BOARD_H__

#include "../Common/CBoard.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BYTE	GetStoragePos( BYTE pi_byBoardType, BYTE pi_byItemKind );

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CNetMsgProcessor_ItemArrangement;
class CNetMsgProcessor_Item;

class CItemBoard : public CObjectBoard
{
protected :

	CNetMsgProcessor_ItemArrangement *	m_pNetMsgProcessor_Arrange;	
	CNetMsgProcessor_Item			 *	m_pNetMsgProcessor_Item;



public :
	CItemBoard();
	virtual ~CItemBoard();


protected :

	virtual	BOOL			Add_ProcessExistObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject );
	virtual	BOOL			Add_ProcessNewObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject );

	virtual	BOOL			Remove_ProcessRemoveQuantity( CBoardObject * pi_pOrgObject, BYTE pi_byQuantity );
	virtual	BOOL			Remove_ProcessRemoveObject( BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardObject * pi_pOrgObject );

	virtual	BOOL			UI_CheckBeforeAdd_CheckEmptySocket( void );
	virtual	BOOL			UI_CheckBeforeAdd_CheckHoverObject( void );
	virtual	BOOL			UI_CheckBeforeAdd_CheckTargetObject( void );
	virtual	BOOL			UI_AddRequest_ProcessObject( void );
			BOOL			UI_AddRequest_ProcessSplitableItem_Devide( void );
			BOOL			UI_AddRequest_ProcessSplitableItem_Combine( void );
			BOOL			UI_AddRequest_ProcessSplitableItem_Combine_CheckQuantity( void );
	virtual	BOOL			UI_AddRequest_ProcessNonSplitableItem( void );
	virtual	void			UI_AddResult_ProcessObject( void );

	virtual	BOOL			UI_CheckBeforeRemove_CheckHoverObject( void );
	virtual	BOOL			UI_CheckBeforeRemove_CheckOriginalObject( BYTE pi_byQuantity );
	virtual	BOOL			UI_RemoveRequest_ProcessObject( BYTE pi_byQuantity );

	virtual	BOOL			UI_SwapRequest_ProcessObject( void );
	virtual	void			UI_SwapResult_ProcessObject( void );
	
	virtual	BOOL			UI_CheckBeforeUse_CheckOriginalObject( void );
	virtual	BOOL			UI_UseRequest_ProcessObject( void );
	virtual	void			UI_UseResult_ProcessObject( void );


	virtual	BOOL			UI_CheckBeforeAdd_CheckEmptySocket_Trade( void );
			BOOL			UI_CheckBeforeAdd_CheckHoverObject_Trade( void );
	virtual	BOOL			UI_CheckBeforeAdd_CheckTargetObject_Trade( void );
	virtual	BOOL			UI_AddRequest_Trade( void );

			BOOL			UI_CheckBeforRemove_CheckOriginalObject_Trade( void );
	virtual	BOOL			UI_RemoveRequest_Trade( void );

	virtual	BOOL			UI_Trade_By_HotKey( BYTE pi_byPackNo, BYTE pi_bySocketNo );

			CBoardItem *	GetTradeItemByIndex( DWORD pi_dwIndex );
			CBoardItem *	GetTradeItemByIndex( BYTE pi_byPackNo, BYTE pi_bySocketNo );

public :

	virtual	void			FrameMove( void );
	virtual	LRESULT			MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );	

public :

			CBoardItem *	GetItem( BYTE pi_byPackNo, BYTE pi_bySocketNo );
			CBoardItem *	GetItemByIndex( DWORD pi_dwIndex );
			CBoardItem *	GetItemByIndex( BYTE & po_byPackNo, BYTE & po_bySocketNo, DWORD pi_dwIndex );

			CBoardItem *	GetItemByItemType( BYTE pi_byItemType );			
			BOOL			GetItemByItemType( CBoardItem & po_clsItem, BYTE & po_byPackNo, BYTE & po_bySocketNo, BYTE pi_byItemKind );
			CBoardItem *	GetLackOfQuantityItemByDTIndex( BYTE & pio_byPackNo, BYTE & po_bySocketNo, BYTE pi_byItemKind, DWORD pi_dwDTIndex, DWORD pi_dwItemNum, BOOL pi_bIncludeOtherBoard = FALSE );
			CBoardItem *	GetRemainOfQuantityItemByDTIndex( BYTE & pio_byPackNo, BYTE & po_bySocketNo, BYTE pi_byItemKind, DWORD pi_dwDTIndex, DWORD pi_dwItemNum );

			BOOL			FindEquipmentPosition( BYTE & po_byPackNo, BYTE & po_bySocketNo );	// 인벤토리에서 제일 먼저 검색되는 장착가능한 아이템의 위치를 반환	

			BOOL			FindTradeItemPosition( BYTE & po_byPackNo, BYTE & po_bySocketNo );


			char			FindEmptySocket_IncludeOtherBoard( BYTE pi_byPackNo );
			BOOL			FindEmptySocket_IncludeOtherBoard( BYTE & po_byPackNo, BYTE & po_bySocketNo );

private :
			char			FindEmptySocket_IncludeShopTradeList( BYTE pi_byPackNo );
			char			FindEmptySocket_IncludeOreProcList( BYTE pi_byPackNo );
			char			FindEmptySocket_IncludeUpgradeList( BYTE pi_byPackNo );

			BOOL			FindEmptySocket_IncludeShopTradeList( BYTE & po_byPackNo, BYTE & po_bySocketNo );
			BOOL			FindEmptySocket_IncludeOreProcList( BYTE & po_byPackNo, BYTE & po_bySocketNo );
			BOOL			FindEmptySocket_IncludeUpgradeList( BYTE & po_byPackNo, BYTE & po_bySocketNo );

public :
			void			UI_SetUselessSocket( BYTE pi_byPackNo, BYTE pi_bySocketNo, BOOL pi_bFlag );
};

#endif // __ITEM_BOARD_H__

#endif // _NEW_UI_
