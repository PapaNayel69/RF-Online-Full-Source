////////////////////////////////////////////////////////////////////////////////
//
// COreProcBoard Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __ORE_PROC_BOARD_H__
#define __ORE_PROC_BOARD_H__

#include "CItemBoard.h"
#include "../../../UserInterface/GameWindow/Npc/GUIOreProcWindow.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define MAX_ORE_PROC_SOCKET		( 40 )

class CNetMsgProcessor_Resource;

class COreProcBoard : public CItemBoard
{
protected :

	CGUIOreProcWindow			m_uiOreProcWindow;
	CNetMsgProcessor_Resource *	m_m_pNetMsgProcessor_Resource;

	CBoardItem	m_clsOre;					// 가공할 원석
	BYTE		m_byOrePackNo;				// 가공할 원석이 들어있는 Pack No
	BYTE		m_byOreSocketNo;			// 가공할 원석이 들어있는 Socket No
	
	BYTE		m_byInvenPackNo;			// 가공된 잉갓을 인벤토리로 옮길 경우, 인벤토리에 동일한 아이템이 있다면 합치기 위해서 사용되는 Pack No
	BYTE		m_byInvenSocketNo;			// 가공된 잉갓을 인벤토리로 옮길 경우, 인벤토리에 동일한 아이템이 있다면 합치기 위해서 사용되는 Socket No	

public :

	COreProcBoard();
	virtual ~COreProcBoard();

			void			InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY );	

			void			ClearIngot( void );

protected :		

	virtual	BOOL			UI_CheckBeforeAdd_CheckHoverObject( void );
	virtual	BOOL			UI_CheckBeforeAdd_CheckTargetObject( void );
	virtual	BOOL			UI_AddRequest_ProcessObject( void );
	virtual	void			UI_AddResult_ProcessObject( void );

	virtual	BOOL			UI_CheckBeforeRemove_CheckOriginalObject( BYTE pi_byQuantity );
	virtual	BOOL			UI_RemoveRequest_ProcessObject( BYTE pi_byQuantity );

	virtual	BOOL			UI_SwapRequest_ProcessObject( void );
	virtual	void			UI_SwapResult_ProcessObject( void );

	virtual	BOOL			UI_UseRequest_ProcessObject( void )	{ return FALSE; }
	virtual	void			UI_UseResult_ProcessObject( void ) {;}			

public :	

			BOOL			UI_OreProcessing_Request( void );
			BOOL			UI_OreProcessing_Result( void );
			BOOL			UI_OreProcessing_Cancel( void );

			BOOL			UI_SellIngot_Request( void );
			BOOL			UI_SellIngot_Result( void );			

			BOOL			UI_Ingot_To_Inventory_Request( BYTE pi_bySocketNo );
			BOOL			UI_Ingot_To_Inventory_Result( DWORD pi_dwItemIndex );

			void			ReturnItemToItemBoard( void );
	
			BOOL			UI_SetOre( BYTE pi_byPackNo, BYTE pi_bySocketNo, BYTE pi_byOreNum, BOOL pi_bTradeByHotKey );
			BOOL			UI_UnsetOre( BOOL pi_bTradeByHotKey );

			void			RemoveOre( void );
			

	inline	CBoardItem *	GetOre( void )						{ if( m_clsOre.IsEmpty() ) return NULL; else return &m_clsOre; }
	inline	BYTE			GetOrePackNo( void )				{ return m_byOrePackNo; }
	inline	BYTE			GetOreSocketNo( void )				{ return m_byOreSocketNo; }
			CBoardItem *	GetOreInInventory( void );
			

	inline	BOOL			IsTrading( void )					{ return m_uiOreProcWindow.IsShow(); }

public :

	inline	BYTE			FindEmptySocket( void )				{ return CObjectBoard::FindEmptySocket( 0 ); }
			CBoardItem *	GetLackOfQuantityItemByDTIndex( BYTE & po_bySocketNo, DWORD pi_dwDTIndex );

			CBoardItem *	GetItem( BYTE pi_bySocketNo );

public :

	virtual	LRESULT			MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );	
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __ORE_PROC_BOARD_H__