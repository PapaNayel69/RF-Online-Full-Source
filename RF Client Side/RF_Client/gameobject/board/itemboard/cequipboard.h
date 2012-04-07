////////////////////////////////////////////////////////////////////////////////
//
// CEquipBoard Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _NEW_UI_

#ifndef __EQUIP_BOARD_H__
#define __EQUIP_BOARD_H__

#include "CItemBoard.h"
#include "../../../UserInterface/GameWindow/GUIInventoryWindow.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#define MAX_EQUIP_SOCKET	( 14 )

class CEquipBoard : public CItemBoard
{
protected :

	CGUIEquipWindow		m_uiEquipWindow;

	BYTE				m_byUsingBulletSocketNo;

	BYTE				m_byReservedItemPackNo;
	BYTE				m_byReservedItemSocketNo;

public :
	CEquipBoard();
	virtual ~CEquipBoard();

			void			InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY );

protected :	

	virtual	BOOL			Add_ProcessNewObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject );
	virtual	BOOL			Remove_ProcessRemoveObject( BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardObject * pi_pOrgObject );

	virtual	BOOL			UI_CheckBeforeAdd_CheckHoverObject( void );	
	virtual	BOOL			UI_AddRequest_ProcessObject( void );
	virtual	void			UI_AddResult_ProcessObject( void );		

	virtual	BOOL			UI_RemoveRequest_ProcessObject( BYTE pi_byQuantity );

public :

	virtual	LRESULT			MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );	

			BOOL			UI_PutOffEquipment_By_HotKey( BYTE pi_bySocketNo );

	inline	void			SetReservedEquipItem( BYTE pi_byPackNo, BYTE pi_bySocketNo )	{ m_byReservedItemPackNo = pi_byPackNo; m_byReservedItemSocketNo = pi_bySocketNo; }
	inline	void			ClearReservedEquipItem( void )									{ m_byReservedItemPackNo = 0xFF; m_byReservedItemSocketNo = 0xFF; }
	inline	BOOL			ExistReservedEquipItem( void )									{ return ( m_byReservedItemPackNo != 0xFF && m_byReservedItemSocketNo != 0xFF ); }
	inline	BYTE			GetReservedEquipItemPackNo( void )								{ return m_byReservedItemPackNo; }
	inline	BYTE			GetReservedEquipItemSocketNo( void )							{ return m_byReservedItemSocketNo; }

			BYTE			GetEmptySocketByEquipPart( BYTE pi_byEquipPart );

	inline	void			SetUsingBulletSocketNo( BYTE pi_bySocketNo )	{ m_byUsingBulletSocketNo = pi_bySocketNo; }
	inline	BYTE			GetUsingBulletSocketNo( void )					{ return m_byUsingBulletSocketNo; }
			DWORD			GetUsingBulletIndex( void );	

public :			
			BOOL			IsEquipableItem( CBoardItem * pi_pItem, BYTE pi_byTarSocketNo );
			void			BulletAutoReload( BYTE pi_bySocketNo, DWORD pi_dwDTIndex );

			CBoardItem *	GetItem( BYTE pi_bySocketNo );
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // __EQUIP_BOARD_H__

#endif // _NEW_UI_
