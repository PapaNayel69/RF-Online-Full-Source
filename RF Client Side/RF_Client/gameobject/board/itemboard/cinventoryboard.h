////////////////////////////////////////////////////////////////////////////////
//
// CInventoryBoard Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _NEW_UI_

#ifndef __INVENTORY_BOARD_H__
#define __INVENTORY_BOARD_H__

#include "CItemBoard.h"
#include "../../../UserInterface/GameWindow/GUIInventoryWindow.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct BUILD_GUARD_TOWER
{
	DWORD	m_dwSkillDTIndex;							// 가드 타워를 짓는데 사용할 스킬의 DT Index
	DWORD	m_dwGuardTowerIndex;						// 가드 타워 아이템의 인덱스

	DWORD	m_dwMaterialIndex[MAX_ITEM_MATERIAL_NUM];	// 필요한 재료의 인덱스
	DWORD	m_dwMaterialNum[MAX_ITEM_MATERIAL_NUM];		// 필요한 재료이 수량

	BUILD_GUARD_TOWER();
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

class CInventoryBoard : public CItemBoard
{
protected :

	CGUIInventoryWindow		m_uiInventoryWindow;	

	BYTE					m_byLootingPackNo;			// 아이템 루팅시 수량을 합칠 아이템의 Pack 번호
	BYTE					m_byLootingSocketNo;		// 아이템 루팅시 수량을 합칠 아이템의 Socket 번호

	BUILD_GUARD_TOWER		m_stGuardTowerInfo;			// 가드 타워를 짓는데 필요한 정보

public :

	CInventoryBoard();
	virtual ~CInventoryBoard();

			void		InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY );

	virtual	BOOL		AddPack( BYTE pi_byAddNum );			

			BOOL		UI_AddPackRequest( void );
			void		UI_AddPackResult( void );

			void		UI_OpenPack( BYTE pi_byPackNo, BOOL pi_bOpen );
			BOOL		UI_IsOpenedPack( BYTE pi_byPackNo );

			BOOL		UI_PutOnEquipment_By_HotKey( BYTE pi_byPackNo, BYTE pi_bySocketNo );

	virtual	BOOL		UI_Trade_By_HotKey( BYTE pi_byPackNo, BYTE pi_bySocketNo );

protected :	
	
	virtual	BOOL		UI_CheckBeforeAdd_CheckHoverObject( void );	
			BOOL		UI_CheckBeforeAdd_CheckMining( void );
	virtual	BOOL		UI_AddRequest_ProcessNonSplitableItem( void );			
	virtual	void		UI_AddResult_ProcessObject( void );

	virtual	BOOL		UI_CheckBeforeAdd_CheckEmptySocket_Trade( void );
	virtual	BOOL		UI_CheckBeforeRemove_CheckHoverObject( void );
	virtual	BOOL		UI_CheckBeforeRemove_CheckOriginalObject( BYTE pi_byQuantity );

	virtual	BOOL		UI_UseRequest_ProcessObject( void );
			BOOL		UI_UseRequest_ProcessObject_Unit( void );
			BOOL		UI_UseRequest_ProcessObject_GuardTower( void );
	virtual	void		UI_UseResult_ProcessObject( void );
			void		UI_UseResult_ProcessObject_GuardTower( void );

public :

			BOOL		UI_AutoPickUpItemRequest( void );
			BOOL		UI_PickUpItemRequest( CGroundItem * pi_pGroundItem );
			BOOL		UI_PickUpItemResult_CreateItem( CBoardItem * pi_pLootingItem );
			BOOL		UI_PickUpItemResult_AddQuantity( DWORD pi_dwItemIndex, DWORD pi_dwQuantity );

			BOOL		FindMapItemPosition( char * pi_pMapName, BYTE & po_byPackNo, BYTE & po_bySocketNo );

protected :

			BOOL		UI_PickUpItemRequest_CheckBoard( void );

public :
			void		SetEquipWindow( CGUIEquipWindow * pi_pEquipWindow );	
			
	virtual	LRESULT		MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

public :

	inline	BUILD_GUARD_TOWER *	GetGuardTowerInfo( void )	{ return &m_stGuardTowerInfo; }
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __INVENTORY_BOARD_H__

#endif // _NEW_UI_