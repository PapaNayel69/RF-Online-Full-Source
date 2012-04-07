////////////////////////////////////////////////////////////////////////////////
//
// CLinkBoard Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#include "../../../Character/Avatar/CAvatar.h"
#include "../../../DataTable/CItemDataMgr.h"

#include "CBoardManager.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void	DisplayBeltSFInfo( CSFItem * pi_pSFItem );
void	DisplayBeltAnimusInfo( CAnimusItem * pi_AnimusItem );

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CLinkBoard::CLinkBoard()
{

}

CLinkBoard::~CLinkBoard()
{
	for( int i = 0; i < m_byMaxPackNum; ++i )
		m_listItemPack[i].EmptyAll();
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CLinkBoard::InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY )
{
	m_uiWindow = &m_uiLinkWindow;

	CBaseBoard::InitWindow( pi_dwID, pi_nPosX, pi_nPosY );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CLinkBoard::Add_ProcessExistObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject )
{
	if( GetObject( pi_byTarPackNo, pi_byTarSocketNo ) )
		return TRUE;

	return FALSE;
}

BOOL
CLinkBoard::Add_ProcessNewObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject )
{
	m_uiLinkWindow.TakeInItem( pi_byTarSocketNo, pi_pOrgObject->GetIcon() );

	if( pi_pOrgObject->IsEqualItemType( BOT_ANIMUS ) )
	{
		CAnimusItem * pAnimusItem = static_cast< CAnimusItem * >( pi_pOrgObject );
		pAnimusItem->SetState( CAnimusItem::AIS_LINK );
	}
	else if( pi_pOrgObject->IsEqualItemType( BOT_SF ) )
	{
		CSFItem * pSFItem = static_cast< CSFItem * >( pi_pOrgObject );
		pSFItem->SetState( CSFItem::SFS_LINK );
	}

	m_listItemPack[pi_byTarPackNo].Add( pi_byTarSocketNo, pi_pOrgObject );

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CLinkBoard::Remove_ProcessRemoveQuantity( CBoardObject * pi_pOrgObject, BYTE pi_byQuantity )
{
	return FALSE;
}

BOOL
CLinkBoard::Remove_ProcessRemoveObject( BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardObject * pi_pOrgObject )
{
	m_uiLinkWindow.TakeOutItem( pi_bySocketNo );
	m_listItemPack[pi_byPackNo].Empty( pi_bySocketNo );

	return TRUE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CLinkBoard::UI_CheckBeforeAdd_CheckEmptySocket( void )
{
	return TRUE;
}

// -----------------------------------------------------------------------------

BOOL	
CLinkBoard::UI_CheckBeforeAdd_CheckTargetObject( void )
{
	if( !CObjectBoard::UI_CheckBeforeAdd_CheckTargetObject() )
		return FALSE;

	// target에 delay가 적용되었으면 swap할수없다.
	CGUISFItem * pIcon = m_uiLinkWindow.GetItem( _GetGBM()->GetTarSocketNo() );
	if( pIcon && pIcon->IsDelayAnimation() )
		return FALSE;
	
	// check whether swap
	CBoardItem * pHoverItem	= static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );
	CBoardItem * pTarItem	= static_cast< CBoardItem * >( _GetGBM()->GetTarObject() );
	if( pHoverItem && pTarItem )
	{		
		UI_SwapRequest( _GetGBM()->GetTarPackNo(), _GetGBM()->GetTarSocketNo() );
		return FALSE;
	}

	return TRUE;
}

//==============================================================================

BOOL
CLinkBoard::UI_AddRequest_ProcessObject( void )
{
	CBoardItem * pHoverItem = _GetGBM()->GetHoverItem();

	if( _GetGBM()->GetOrgBoardType() == IBT_LINK )
	{
		UI_AddResult( TRUE );
	}
	else if( _GetGBM()->GetOrgBoardType() == IBT_ANIMUS ||
			 _GetGBM()->GetOrgBoardType() == IBT_FORCE  ||
			 _GetGBM()->GetOrgBoardType() == IBT_SKILL )
	{
		if( !_GetAvatar()->IsUsableItem( pHoverItem, TRUE ) )
			return FALSE;

		UI_AddResult( TRUE );
	}
	else if( _GetGBM()->GetOrgBoardType() == IBT_INVENTORY )
	{
		// hover를 org로 돌려놓고 링크를 걸어야 함.
		//UI_AddResult( TRUE );
		return FALSE;
	}	
	else
	{
		return FALSE;
	}

	return TRUE;
}

void
CLinkBoard::UI_AddResult_ProcessObject( void )
{	

	if( _GetGBM()->GetOrgBoardType() == IBT_ANIMUS	||		
		_GetGBM()->GetOrgBoardType() == IBT_FORCE	||
		_GetGBM()->GetOrgBoardType() == IBT_SKILL )
	{		
		Add( _GetGBM()->GetTarPackNo(), _GetGBM()->GetTarSocketNo(), _GetGBM()->GetOrgObject() );
		PlayWave( SND_ID_BELT_EQUIP );
	}
	else if( _GetGBM()->GetOrgBoardType() == IBT_LINK )
	{	
		// 원래 아이템이 있던 보드에서 pOrgObject를 찾는다.
		CBoardObject *	pOrgObject = GetOrgBoardObject( _GetGBM()->GetHoverObject() );
		if( !pOrgObject )
			return;				
		
		Add( _GetGBM()->GetTarPackNo(), _GetGBM()->GetTarSocketNo(), pOrgObject );
		PlayWave( SND_ID_BELT_EQUIP );
	}
	else if( _GetGBM()->GetOrgBoardType() == IBT_INVENTORY )
	{

	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CLinkBoard::UI_CheckBeforeRemove_CheckOriginalObject( BYTE pi_byQuantity )
{
	if( !CObjectBoard::UI_CheckBeforeRemove_CheckOriginalObject( pi_byQuantity ) )
		return FALSE;
	
	// delay가 적용되었으면 못꺼낸다
	CGUISFItem * pIcon = m_uiLinkWindow.GetItem( _GetGBM()->GetOrgSocketNo() );
	if( pIcon && pIcon->IsDelayAnimation() )
		return FALSE;

	return TRUE;
}

BOOL
CLinkBoard::UI_RemoveRequest_ProcessObject( BYTE pi_byQuantity )
{	
	CBoardObject *	pOrgItem = _GetGBM()->GetOrgObject();

/*
	if( pOrgItem->IsEqualItemType( BOT_ANIMUS ) )
	{
		CBoardItem		clsHoverItem;
		if( !_GetGBM()->ConvertBoardObject( &clsHoverItem, pOrgItem ) )
			return FALSE;

		_GetGBM()->SetHoverObject( &clsHoverItem );
	}
	else
	{
		_GetGBM()->SetHoverObject( pOrgItem );
	}	
*/
	_GetGBM()->SetHoverObject( pOrgItem );

	if( !Remove( _GetGBM()->GetOrgPackNo(), _GetGBM()->GetOrgSocketNo(), pi_byQuantity ) )
		return FALSE;

	// reset org object
	_GetGBM()->SetOrgObject( GetObject( _GetGBM()->GetOrgPackNo(), _GetGBM()->GetOrgSocketNo() ) );

	_GetUIScreen()->GetObject()->SetSize( 40, 40 );

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CLinkBoard::UI_SwapRequest_ProcessObject( void )
{	
	CBoardItem * pTarItem	= static_cast< CBoardItem * >( _GetGBM()->GetTarObject() );
	CBoardItem * pHoverItem	= static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );

	if( _GetGBM()->GetOrgBoardType() == IBT_ANIMUS )
	{
	}
	else if( _GetGBM()->GetOrgBoardType() == IBT_INVENTORY )
	{
	}	
	else if( _GetGBM()->GetOrgBoardType() == IBT_SKILL	||
			 _GetGBM()->GetOrgBoardType() == IBT_FORCE	||
			 _GetGBM()->GetOrgBoardType() == IBT_LINK )
	{
		UI_SwapResult( TRUE );
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

void
CLinkBoard::UI_SwapResult_ProcessObject( void )
{
	CBoardObject *	pTarObject	= static_cast< CBoardObject * >( _GetGBM()->GetTarObject() );
	CBoardObject *	pOrgItem	= GetOrgBoardObject( _GetGBM()->GetHoverObject() ); 	

	// remove target object
	if( !Remove( _GetGBM()->GetTarPackNo(), _GetGBM()->GetTarSocketNo() ) )
		return;

	// add target object
	if( !Add( _GetGBM()->GetTarPackNo(), _GetGBM()->GetTarSocketNo(), pOrgItem ) )
		return;

	// link board -> add original ojbect
	if( _GetGBM()->GetOrgBoardType() == IBT_LINK )
	{
		_GetGBM()->AddObject( _GetGBM()->GetOrgBoardType(),
							  _GetGBM()->GetOrgPackNo(), _GetGBM()->GetOrgSocketNo(),
							  pTarObject );
	}
	else 
	{
		// skill / force board => set item state
		if( pTarObject->IsEqualItemType( BOT_SF ) )
		{
			CSFItem * pSFItem = static_cast< CSFItem * >( pTarObject );
			pSFItem->SetState( CSFItem::SFS_ACTIVE );
		}
		// animus board => set item state
		else if( pTarObject->IsEqualItemType( BOT_ANIMUS ) )
		{
			CAnimusItem * pAnimusItem = static_cast< CAnimusItem * >( pTarObject );
			pAnimusItem->SetState( CAnimusItem::AIS_ACTIVE );
		}
	}

	PlayWave( SND_ID_BELT_EQUIP );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//


BOOL
CLinkBoard::UI_UseRequest_ProcessObject( void )
{
	return FALSE;
}

void
CLinkBoard::UI_UseResult_ProcessObject( void )
{
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CLinkBoard::UI_UseSkillForceRequest( CSFItem * pi_pSFItem, CCharacter * pi_pTarget )
{
	if( !pi_pSFItem )
		return;

	if( pi_pSFItem->IsEqualSFType( CAT_FORCE ) )					
		_GetForceBoard()->UseSkillForce_Request( pi_pSFItem, pi_pTarget, FALSE );
	else
		_GetSkillBoard()->UseSkillForce_Request( pi_pSFItem, pi_pTarget, FALSE );
}

void
CLinkBoard::UI_UseSkillForceRequest( BYTE pi_bySocketNo, CCharacter * pi_pTarget, BOOL pi_bEventInSFBeltBoard )
{
	CBoardObject * pOrgObject = GetObject( 0, pi_bySocketNo );

	if( !pOrgObject )
		return;
	if( !pOrgObject->IsEqualItemType( BOT_SF ) )
		return;

	CSFItem * pSFItem = static_cast< CSFItem * >( pOrgObject );

	if( pSFItem->IsEqualSFType( CAT_FORCE ) )
		_GetForceBoard()->UseSkillForce_Request( pSFItem, pi_pTarget, pi_bEventInSFBeltBoard );
	else
		_GetSkillBoard()->UseSkillForce_Request( pSFItem, pi_pTarget, pi_bEventInSFBeltBoard );
}

// -------------------------------------------------------------------------------

void		
CLinkBoard::UI_Unlink_By_Hotkey( BYTE pi_bySocketNo )
{
	if( !UI_RemoveRequest( 0, pi_bySocketNo, 1 ) )
		return;

	CBoardObject * pHoverObject = _GetGBM()->GetHoverObject();
	if( !pHoverObject )
		return;

	BYTE byPackNo, bySocketNo;
	CObjectBoard * pOrgBoard = NULL;

	if( pHoverObject->IsEqualItemType( BOT_SF ) )
	{
		CSFItem * pSFItem = static_cast< CSFItem * >( pHoverObject );
		if( pSFItem->IsEqualSFType( CAT_FORCE ) )
			pOrgBoard = _GetForceBoard();
		else
			pOrgBoard = _GetSkillBoard();
	}
	else if( pHoverObject->IsEqualItemType( BOT_ANIMUS ) )
	{
		pOrgBoard = _GetAnimusBoard();
	}
	

	if( pOrgBoard )
	{
		if( pOrgBoard->GetObjectPosition( byPackNo, bySocketNo, pHoverObject->GetIndex() ) )
			pOrgBoard->UI_AddRequest( byPackNo, bySocketNo );
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CLinkBoard::FrameMove( void )
{
	CObjectBoard::FrameMove();
}

LRESULT
CLinkBoard::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	CAnimusItem *	pTarItem = NULL;
	UI_MOUSE_MSG	stRecvMsg( lParam );

	switch( uMsg )
	{
		case WM_BOARD_ADD_OBJECT	:
			UI_AddRequest( stRecvMsg.m_byPackNo, stRecvMsg.m_bySocketNo );
			break;

		case WM_BOARD_REMOVE_OBJECT :
			UI_RemoveRequest( stRecvMsg.m_byPackNo, stRecvMsg.m_bySocketNo, stRecvMsg.m_byQuantity );
			break;

		case WM_BOARD_USE_OBJECT	:
			{
				_GetAvatar()->ClearSFTargetPos();

				CBoardObject *	pOrgObject = GetObject( stRecvMsg.m_byPackNo, stRecvMsg.m_bySocketNo );
				if( !pOrgObject )
					break;
				
				if( pOrgObject->IsEqualItemType( BOT_SF ) )
				{			
					UI_UseSkillForceRequest( stRecvMsg.m_bySocketNo, NULL, TRUE );					
				}
				else if( pOrgObject->IsEqualItemType( BOT_ANIMUS ) )
				{
					BYTE byPackNo;
					BYTE bySocketNo;

					if( _GetAnimusBoard()->GetObjectPosition( byPackNo, bySocketNo, pOrgObject->GetIndex() ) )
						_GetAnimusBoard()->UI_UseRequest( byPackNo, bySocketNo );
				}				
			}
			break;			

		case WM_BOARD_EQUIP_HOT_KEY	:
			{
				UI_Unlink_By_Hotkey( stRecvMsg.m_bySocketNo );
			}
			break;

		case WM_BOARD_HOVER_OBJECT	:
			{
				CBoardObject *	pOrgObject = GetObject( stRecvMsg.m_byPackNo, stRecvMsg.m_bySocketNo );
				if( !pOrgObject )
					break;

				if( pOrgObject->IsEqualItemType( BOT_SF ) )
					DisplayBeltSFInfo( ( CSFItem * )pOrgObject );

				else if( pOrgObject->IsEqualItemType( BOT_ANIMUS ) )
					DisplayBeltAnimusInfo( ( CAnimusItem * )pOrgObject );
			}
			break;
			
	}

	return CObjectBoard::MsgProc( hWnd, uMsg, wParam, lParam );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CBoardObject *	
CLinkBoard::GetOrgBoardObject( CBoardObject * pi_pObject )
{
	if( !pi_pObject )
		return NULL;
	
	CBoardObject * pOrgObject = NULL;	
	
	if( pi_pObject->IsEqualItemType( BOT_SF ) )
	{
		CSFItem * pSFItem = static_cast< CSFItem * >( pi_pObject );
		if( pSFItem->IsEqualSFType( CAT_FORCE ) )
			pOrgObject = _GetForceBoard()->GetObjectByIndex( pi_pObject->GetIndex() );
		else
			pOrgObject = _GetSkillBoard()->GetObjectByIndex( pi_pObject->GetIndex() );
	}
	else if( pi_pObject->IsEqualItemType( BOT_ANIMUS ) )
	{
		pOrgObject = _GetAnimusBoard()->GetObjectByIndex( pi_pObject->GetIndex() );
	}

	return pOrgObject;
}

CBoardObject *
CLinkBoard::GetEqualObject( CBoardObject * pi_pOrgObject )
{
	if( !pi_pOrgObject )
		return NULL;

	CBoardObject * pLinkObject;
	for( int i = 0; i < m_listItemPack[0].GetMaxSocketNum(); ++i )
	{
		pLinkObject = GetObject( 0, i );
		if( !pLinkObject )
			continue;

		if( pi_pOrgObject->IsEqual( pLinkObject ) )
			return pLinkObject;
	}

	return NULL;
}

BYTE
CLinkBoard::FindEmptySocket( void )
{
	return CObjectBoard::FindEmptySocket( 0 );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
CLinkBoard::BeginDelayAnimation( CBoardObject * pi_pOrgObject, DWORD pi_dwTime )
{
	if( !pi_pOrgObject )
		return;

	pi_pOrgObject->BeginDelayAnimation( pi_dwTime );

	CBoardObject * pLinkObject = GetEqualObject( pi_pOrgObject );
	if( !pLinkObject )
		return;

	BYTE byPackNo, bySocketNo;
	if( GetObjectPosition( byPackNo, bySocketNo, pLinkObject->GetIndex() ) )
	{
		CGUISFItem * pIcon = m_uiLinkWindow.GetItem( bySocketNo );
		pIcon->BeginDelayAnimation( pi_dwTime );
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////


/*
BOOL
CLinkBoard::BoardItemToAnimusItem( CAnimusItem * pi_pTarItem, CBoardItem * pi_pOrgItem )
{
	if( !pi_pOrgItem || !pi_pOrgItem->IsEqualItemType( BOT_ITEM ) ||
		!pi_pTarItem || !pi_pTarItem->IsEqualItemType( BOT_ANIMUS ) )
		return FALSE;

	pi_pTarItem->SetIndex( pi_pOrgItem->GetIndex() );
	pi_pTarItem->SetData( pi_pOrgItem->GetData() );

	return TRUE;
}

BOOL
CLinkBoard::AnimusItemToBoardItem( CBoardItem * pi_pTarItem, CAnimusItem * pi_pOrgItem )
{
	if( !pi_pOrgItem || !pi_pOrgItem->IsEqualItemType( BOT_ANIMUS ) ||
		!pi_pTarItem || !pi_pTarItem->IsEqualItemType( BOT_ITEM ) )
		return FALSE;

	pi_pTarItem->SetIndex( pi_pOrgItem->GetIndex() );
	pi_pTarItem->SetData( pi_pOrgItem->GetData() );
	pi_pTarItem->SetIcon( pi_pOrgItem->GetIcon() );

	return TRUE;
}*/

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////
