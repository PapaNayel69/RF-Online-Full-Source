////////////////////////////////////////////////////////////////////////////////
//
// CBeltBoard Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _NEW_UI_

#include "../../Manager/CCharacterMgr.h"
#include "../Common/CBoardManager.h"

#include "../../../Network/NetMsgProcessor/Item/CNetMsgProcessor_ItemArrangement.h"
#include "../../../Network/NetMsgProcessor/Item/CNetMsgProcessor_Item.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CBeltBoard::CBeltBoard()
{
	m_pNetMsgProcessor_Arrange	= static_cast< CNetMsgProcessor_ItemArrangement * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_ITEM_ARRANGE ) );
}

CBeltBoard::~CBeltBoard()
{

}

void
CBeltBoard::InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY )
{
	m_uiWindow = &m_uiBeltWindow;

	CBaseBoard::InitWindow( pi_dwID, pi_nPosX, pi_nPosY );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CBeltBoard::UI_CheckBeforeAdd_CheckHoverObject( void )
{
	if( !CObjectBoard::UI_CheckBeforeAdd_CheckHoverObject() )
		return FALSE;

	if( !_GetGBM()->GetHoverObject()->IsEqualItemType( BOT_BOARD_ITEM ) )
		return FALSE;

	CBoardItem * pHoverItem	= static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );
	if( !pHoverItem->IsEqualItemKind( IEPT_POTION ) )
		return FALSE;

	return CItemBoard::UI_CheckBeforeAdd_CheckHoverObject();
}

//------------------------------------------------------------------------------

void
CBeltBoard::UI_AddResult_ProcessObject( void )
{
	CItemBoard::UI_AddResult_ProcessObject();	

	PlayWave( SND_ID_BELT_EQUIP );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CBeltBoard::UI_UseRequest_ProcessObject( void )
{
	CBoardItem * pOrgItem = static_cast< CBoardItem * >( _GetGBM()->GetOrgObject() );
	if( !pOrgItem->IsEqualItemKind( IEPT_POTION ) )
		return FALSE;

	m_pNetMsgProcessor_Item->UsePotionRequest_clzo();

	_GetAvatar()->SetUsingItem( pOrgItem );

	return TRUE;
}

void
CBeltBoard::UI_UseResult_ProcessObject( void )
{
	CBoardItem * pOrgItem = static_cast< CBoardItem * >( _GetGBM()->GetObject( _GetGBM()->GetOrgBoardType(),
																			   _GetGBM()->GetOrgPackNo(),
																			   _GetGBM()->GetOrgSocketNo() ) );	
	if( !pOrgItem )
		return;

	// set effect
	_GetAvatar()->UseItem( pOrgItem );
	_GetAvatar()->RemoveUsingItem();

	// set quantity
	Remove( _GetGBM()->GetOrgPackNo(), _GetGBM()->GetOrgSocketNo(), 1 );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

LRESULT
CBeltBoard::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	UI_MOUSE_MSG	stRecvMsg( lParam );

	switch( uMsg )
	{
		case WM_BOARD_CHECK_REMOVE_OBJECT :
		{
			CBoardItem * pOrgItem = static_cast< CBoardItem * >( GetObject( stRecvMsg.m_byPackNo, stRecvMsg.m_bySocketNo ) );
			if( !pOrgItem ) // -> 아이템이 없으면 거래중인 아이템일 수 있다.
			{
				UI_RemoveRequest( stRecvMsg.m_byPackNo, stRecvMsg.m_bySocketNo, 1 );
			}
			else
			{
				if( ( pOrgItem->IsSplitable() ) && ( pOrgItem->GetQuantity() > 1 ) )
				{
					POINT pDlgPos = { gMouse.x, gMouse.y };
					// belt window가 _GetFuncKeyWindow()내에 포함되어 있어서 숫자입력창의 owner window를
					// _GetFuncKeyWindow()로 해야한다.
					_GUINumInputDlg( ( CGUIWindow * )_GetFuncKeyWindow(), pDlgPos, pOrgItem->GetQuantity(),
									 INPUT_DLG_ID, pOrgItem->GetQuantity() );
				}
				else
				{
					UI_RemoveRequest( stRecvMsg.m_byPackNo, stRecvMsg.m_bySocketNo, 1 );
				}
			}
		}
		return 1;
	}

	return CItemBoard::MsgProc( hWnd, uMsg, wParam, lParam );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BYTE		
CBeltBoard::FindEmptySocket( void )
{
	return CObjectBoard::FindEmptySocket( 0 );
}

CBoardItem *
CBeltBoard::GetItem( BYTE pi_bySocketNo )
{
	return static_cast< CBoardItem * >( GetObject( 0, pi_bySocketNo ) );
}

#endif // _NEW_UI_