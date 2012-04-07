////////////////////////////////////////////////////////////////////////////////
//
// CBoard Class Implementation
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

CBaseBoard::CBaseBoard()
{
	m_byBoardType	= ID_INVALID;
	m_uiWindow		= NULL;
}

CBaseBoard::~CBaseBoard()
{
	m_uiWindow = NULL;
}

void
CBaseBoard::InitWindow( DWORD pi_dwWindowID, int pi_nPosX, int pi_nPosY )
{
	if( m_uiWindow )
	{
		m_uiWindow->Init();
		m_uiWindow->SetID( pi_dwWindowID );
		m_uiWindow->SetPos( pi_nPosX, pi_nPosY );

		if( _GetUIScreen() )
			_GetUIScreen()->Add( m_uiWindow );
	}
}

BOOL
CBaseBoard::IsEqualWindow( DWORD pi_dwWindowID )
{
	if( m_uiWindow )
		return ( m_uiWindow->GetID() == pi_dwWindowID );

	return FALSE;
}

BOOL
CBaseBoard::IsOpen( void )
{
	if( m_uiWindow )
		return m_uiWindow->IsShow();

	return FALSE;
}

void
CBaseBoard::OpenWindow( void )
{
	if( m_uiWindow )
	{
		if( !m_uiWindow->IsShow() )
			m_uiWindow->Open();
	}
}

void
CBaseBoard::CloseWindow( void )
{
	if( m_uiWindow )
		m_uiWindow->Close();
}

void
CBaseBoard::ToggleWindow( void )
{
	if( IsOpen() )
		CloseWindow();
	else
		OpenWindow();
}

void
CBaseBoard::SetWindowPos( int pi_nPosX, int pi_nPosY )
{
	if( m_uiWindow )
		m_uiWindow->SetPos( pi_nPosX, pi_nPosY );
}

void
CBaseBoard::GetWindowPos( POINT & po_ptWindowPos )
{
	if( m_uiWindow )
	{
		po_ptWindowPos.x	= m_uiWindow->m_ptPos.x;
		po_ptWindowPos.y	= m_uiWindow->m_ptPos.y;
	}
}

void
CBaseBoard::GetWindowRect( RECT & po_ptWindowRect )
{
	if( m_uiWindow )
	{
		po_ptWindowRect.left	= m_uiWindow->m_ptPos.x;
		po_ptWindowRect.top		= m_uiWindow->m_ptPos.y;
		po_ptWindowRect.right	= m_uiWindow->m_ptPos.x + m_uiWindow->m_ptSize.x;
		po_ptWindowRect.bottom	= m_uiWindow->m_ptPos.y + m_uiWindow->m_ptSize.y;
	}
}

void
CBaseBoard::FrameMove( void )
{
}

LRESULT
CBaseBoard::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case WM_CLOSING_WINDOW	:
			CloseWindow();
			break;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CObjectPack::CObjectPack()
{
	m_listSocket		= NULL;
	m_byMaxSocketNum	= 0;
	m_byUsingSocketNum	= 0;

	m_bIsExist			= FALSE;
}

CObjectPack::~CObjectPack()
{
	for( int i = 0; i < m_byMaxSocketNum; ++i )
		_DELETE( m_listSocket[i] );

	_DELETE_ARRAY( m_listSocket );
}

void
CObjectPack::Create( BYTE pi_byMaxSocketNum )
{
	m_byMaxSocketNum = pi_byMaxSocketNum;

	m_listSocket = new CBoardObject * [pi_byMaxSocketNum];

	ZeroMemory( m_listSocket, sizeof( CBoardObject * ) * pi_byMaxSocketNum );
}

void
CObjectPack::Clear( void )
{
	for( int i = 0; i < m_byMaxSocketNum; ++i )
		_DELETE( m_listSocket[i] );
}

BOOL
CObjectPack::Add( BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject )
{
	if( !m_bIsExist || ( pi_byTarSocketNo >= m_byMaxSocketNum ) || !pi_pOrgObject )
		return FALSE;

	if( m_listSocket[pi_byTarSocketNo] )
		return FALSE;

	m_listSocket[pi_byTarSocketNo] = pi_pOrgObject;

	return TRUE;
}

BOOL
CObjectPack::Remove( BYTE pi_byOrgSocketNo )
{
	if( !m_bIsExist || ( pi_byOrgSocketNo >= m_byMaxSocketNum ) )
		return FALSE;

	_DELETE( m_listSocket[pi_byOrgSocketNo] );

	return TRUE;
}

BOOL
CObjectPack::Empty( BYTE pi_byOrgSocketNo )
{
	if( !m_bIsExist || ( pi_byOrgSocketNo >= m_byMaxSocketNum ) )
		return FALSE;

	m_listSocket[pi_byOrgSocketNo] = NULL;

	return TRUE;
}

void
CObjectPack::EmptyAll( void )
{
	ZeroMemory( m_listSocket, sizeof( CBoardObject * ) * m_byMaxSocketNum );
}

CBoardObject *
CObjectPack::GetObject( BYTE pi_byOrgSocketNo )
{
	if( !m_bIsExist || ( pi_byOrgSocketNo >= m_byMaxSocketNum ) )
		return NULL;

	return m_listSocket[pi_byOrgSocketNo];
}

CBoardObject *
CObjectPack::GetObjectByIndex( DWORD pi_dwIndex )
{
	if( !m_bIsExist )
		return NULL;

	for( int i = 0; i < m_byMaxSocketNum; ++i )
	{
		if( !m_listSocket[i] )
			continue;

		if( m_listSocket[i]->IsEqual( pi_dwIndex ) )
			return m_listSocket[i];
	}

	return NULL;
}

BYTE
CObjectPack::GetObjectPosition( DWORD pi_dwIndex )
{
	if( !m_bIsExist )
		return 0xFF;

	for( int i = 0; i < m_byMaxSocketNum; ++i )
	{
		if( !m_listSocket[i] )
			continue;

		if( m_listSocket[i]->IsEqual( pi_dwIndex ) )
			return i;
	}

	return 0xFF;
}

BYTE
CObjectPack::FindEmptySocket( void )
{
	if( !m_bIsExist )
		return 0xFF;

	for( int i = 0; i < m_byMaxSocketNum; ++i )
	{
		if( !m_listSocket[i] )
			return i;
	}

	return 0xFF;
}

BYTE
CObjectPack::GetEmptySocketNum( void )
{
	if( !m_bIsExist )
		return 0;

	for( int i = 0, cnt = 0; i < m_byMaxSocketNum; ++i )
	{
		if( !m_listSocket[i] )
			++cnt;
	}

	return cnt;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CObjectBoard::CObjectBoard()
{
	m_listItemPack		= NULL;

	m_byMaxPackNum		= 0;
	m_byActivePackNo	= 0;
}

CObjectBoard::~CObjectBoard()
{
	_DELETE_ARRAY( m_listItemPack );
}

void
CObjectBoard::Create( BYTE pi_byBoardType, BYTE pi_byMaxPackNum, BYTE pi_byMaxSocketNum )
{
	CBaseBoard::Create( pi_byBoardType );

	m_byMaxPackNum	= pi_byMaxPackNum;

	if( m_byMaxPackNum )
	{
		m_listItemPack	= new CObjectPack[pi_byMaxPackNum];
		for( int i = 0; i < pi_byMaxPackNum; ++i )
			m_listItemPack[i].Create( pi_byMaxSocketNum );
	}
}

void
CObjectBoard::Clear( void )
{
	for( int i = 0; i < m_byMaxPackNum; ++i )
	{
		for( int j = 0; j < m_listItemPack[i].GetMaxSocketNum(); ++j )
			Remove( i, j );
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CObjectBoard::AddPack( BYTE pi_byPackNo )
{
	if( !m_byMaxPackNum )
		return FALSE;

	if( m_listItemPack[pi_byPackNo].m_bIsExist )
		return FALSE;

	m_listItemPack[pi_byPackNo].m_bIsExist = TRUE;

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CObjectBoard::Add( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject )
{
	if( !pi_pOrgObject )
		return FALSE;
	if( pi_byTarPackNo >= m_byMaxPackNum )
		return FALSE;

	if( Add_ProcessExistObject( pi_byTarPackNo, pi_byTarSocketNo, pi_pOrgObject ) )
		return TRUE;

	return Add_ProcessNewObject( pi_byTarPackNo, pi_byTarSocketNo, pi_pOrgObject );
}

//==============================================================================

BOOL
CObjectBoard::Remove( BYTE pi_byPackNo, BYTE pi_bySocketNo, BYTE pi_byQuantity )
{
	return Remove_Common( pi_byPackNo, pi_bySocketNo, pi_byQuantity );
}

BOOL
CObjectBoard::RemoveByIndex( DWORD pi_dwOrgIndex, BYTE pi_byQuantity )
{
	// remove hover object
	CBoardItem *	pHoverItem = _GetGBM()->GetHoverItem();
	BYTE			byRemainQuantity = pi_byQuantity;
	if( pHoverItem && m_byBoardType == _GetGBM()->GetOrgBoardType() )
	{
		if( pi_byQuantity == 0xFF )
			byRemainQuantity = pHoverItem->GetQuantity();

		if( pHoverItem->IsEqual( pi_dwOrgIndex ) )
			byRemainQuantity -= _GetGBM()->RemoveHoverObjectQuantity( pi_byQuantity );
	}

	// remove object from board
	BYTE byPackNo, bySocketNo;
	if( GetObjectPosition( byPackNo, bySocketNo, pi_dwOrgIndex ) )
		return Remove_Common( byPackNo, bySocketNo, byRemainQuantity );

	return FALSE;
}

// ( pi_byQuantity == 0xFF ) 이면 수량에 관계없이 삭제한다.
BOOL
CObjectBoard::Remove_Common( BYTE pi_byPackNo, BYTE pi_bySocketNo, BYTE pi_byQuantity )
{
	if( pi_byQuantity != 0xFF && pi_byQuantity > MAX_ITEM_QUANTITY )
		return FALSE;
	if( pi_byQuantity <= 0 )
		return FALSE;

	CBoardObject * pOrgObject = GetObject( pi_byPackNo, pi_bySocketNo );
	if( !pOrgObject )
		return FALSE;

	if( pi_byQuantity != 0xFF )
	{
		if( Remove_ProcessRemoveQuantity( pOrgObject, pi_byQuantity ) )
			return TRUE;
	}

	return Remove_ProcessRemoveObject( pi_byPackNo, pi_bySocketNo, pOrgObject );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CObjectBoard::UI_AddRequest( BYTE pi_byPackNo, BYTE pi_bySocketNo )
{
	// IsEnableProcess()는 반드시 SetOperationType() 등의 함수가 호출되기 전에 체크해야 함.
	// 안 그러면 OrgObject가 현재 처리중인 아이템이 아닌, 현재 체크한 아이템이 세팅되므로 
	// 버그를 유발시킨다.
	if( !_GetGBM()->IsEnableProcess() )
		return FALSE;

	_GetGBM()->SetOperationType( IOT_SET );
	_GetGBM()->SetTarObject( m_byBoardType, pi_byPackNo, pi_bySocketNo,
							 GetObject( pi_byPackNo, pi_bySocketNo ) );

	if( !UI_CheckBeforeAdd() )
		return FALSE;

	return UI_AddRequest_ProcessObject();
}

//------------------------------------------------------------------------------

BOOL
CObjectBoard::UI_CheckBeforeAdd( void )
{
	if( !UI_CheckBeforeAdd_CheckAvatar() )
		return FALSE;

	if( !UI_CheckBeforeAdd_CheckEmptySocket() )
		return FALSE;

	if( !UI_CheckBeforeAdd_CheckHoverObject() )
		return FALSE;

	if( !UI_CheckBeforeAdd_CheckTargetObject() )
		return FALSE;

	return TRUE;
}

BOOL
CObjectBoard::UI_CheckBeforeAdd_CheckAvatar( void )
{
	if( _GetAvatar()->IsDeath() )
	{
		_SetSystemMsg( "아바타가 죽은 상태에서는 아이템을 장착/해제/사용할 수 없습니다." );
		return FALSE;
	}

	return TRUE;
}

BOOL
CObjectBoard::UI_CheckBeforeAdd_CheckEmptySocket( void )
{
	return ( GetEmptySocketNum() > 0 );
}

BOOL
CObjectBoard::UI_CheckBeforeAdd_CheckHoverObject( void )
{
	if( !_GetGBM()->HaveHoverObject() )
		return FALSE;

	if( _GetGBM()->GetHoverObject()->IsUsing() )
		return FALSE;

	return TRUE;
}

BOOL
CObjectBoard::UI_CheckBeforeAdd_CheckTargetObject( void )
{
	if( _GetGBM()->GetTarObject() )
	{
		if( _GetGBM()->GetTarObject()->IsUsing() )
			return FALSE;
	}

	return TRUE;
}

//------------------------------------------------------------------------------

void
CObjectBoard::UI_AddResult( BOOL pi_bIsSuccessed )
{
	if( pi_bIsSuccessed )
	{
		UI_AddResult_ProcessObject();
	}
	else
	{
		_GetGBM()->ReturnHoverObject();
	}

	_GetGBM()->ClearOperation();
}

//==============================================================================

BOOL
CObjectBoard::UI_RemoveRequest( BYTE pi_byPackNo, BYTE pi_bySocketNo, BYTE pi_byQuantity )
{
	// IsEnableProcess()는 반드시 SetOperationType() 등의 함수가 호출되기 전에 체크해야 함.
	// 안 그러면 OrgObject가 현재 처리중인 아이템이 아닌, 현재 체크한 아이템이 세팅되므로 
	// 버그를 유발시킨다.
	if( !_GetGBM()->IsEnableProcess() )
		return FALSE;

	_GetGBM()->SetOperationType( IOT_UNSET );
	_GetGBM()->SetOrgObject( m_byBoardType, pi_byPackNo, pi_bySocketNo,
							 GetObject( pi_byPackNo, pi_bySocketNo ) );

	if( !UI_CheckBeforeRemove( pi_byQuantity ) )
		return FALSE;

	return UI_RemoveRequest_ProcessObject( pi_byQuantity );
}

//------------------------------------------------------------------------------

BOOL
CObjectBoard::UI_CheckBeforeRemove( BYTE pi_byQuantity )
{
	if( !UI_CheckBeforeAdd_CheckAvatar() )
		return FALSE;

	if( !UI_CheckBeforeRemove_CheckHoverObject() )
		return FALSE;

	if( !UI_CheckBeforeRemove_CheckOriginalObject( pi_byQuantity ) )
		return FALSE;

	return TRUE;
}

BOOL
CObjectBoard::UI_CheckBeforeRemove_CheckHoverObject( void )
{
	if( _GetGBM()->HaveHoverObject() )
		return FALSE;

	return TRUE;
}

BOOL
CObjectBoard::UI_CheckBeforeRemove_CheckOriginalObject( BYTE pi_byQuantity )
{
	if( !_GetGBM()->GetOrgObject() )
		return FALSE;

	if( _GetGBM()->GetOrgObject()->IsUsing() )
		return FALSE;

	return TRUE;
}

//------------------------------------------------------------------------------

void
CObjectBoard::UI_RemoveResult( void )
{
	/*
	UI_AddRequest_ProcessObject();

	_GetGBM()->SetHoverObject( NULL );
	*/
}

//==============================================================================

// UI_SwapRequest()은 UI_AddRequest()에서 호출되므로 따로 검사과정이 필요없음.
BOOL
CObjectBoard::UI_SwapRequest( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo )
{
	_GetGBM()->SetOperationType( IOT_SWAP );

	return UI_SwapRequest_ProcessObject();
}

//------------------------------------------------------------------------------

void
CObjectBoard::UI_SwapResult( BOOL pi_bIsSuccessed )
{
	if( pi_bIsSuccessed )
		UI_SwapResult_ProcessObject();

	_GetGBM()->ClearOperation();
}

//==============================================================================

BOOL
CObjectBoard::UI_UseRequest( BYTE pi_byPackNo, BYTE pi_bySocketNo )
{
	// IsEnableProcess()는 반드시 SetOperationType() 등의 함수가 호출되기 전에 체크해야 함.
	// 안 그러면 OrgObject가 현재 처리중인 아이템이 아닌, 현재 체크한 아이템이 세팅되므로 
	// 버그를 유발시킨다.
	if( !_GetGBM()->IsEnableProcess() )
		return FALSE;

	// hover object가 있을 경우 _GetGBM()->SetOrgObject()을 호출하면
	// hover object의 packNo, socketNo가 없어져서 
	// swap할 때 문제가 생기므로 hover object를 먼저 체크한다.
	if( _GetGBM()->HaveHoverObject() )
		return FALSE;

	_GetGBM()->SetOperationType( IOT_USE );
	_GetGBM()->SetOrgObject( m_byBoardType, pi_byPackNo, pi_bySocketNo,
							 GetObject( pi_byPackNo, pi_bySocketNo ) );

	if( !UI_CheckBeforeUse() )
		return FALSE;

	return UI_UseRequest_ProcessObject();
}

//------------------------------------------------------------------------------

BOOL
CObjectBoard::UI_CheckBeforeUse( void )
{
	if( !UI_CheckBeforeAdd_CheckAvatar() )
		return FALSE;

	if( !UI_CheckBeforeUse_CheckHoverObject() )
		return FALSE;

	if( !UI_CheckBeforeUse_CheckOriginalObject() )
		return FALSE;

	return TRUE;
}

BOOL
CObjectBoard::UI_CheckBeforeUse_CheckHoverObject( void )
{
	if( _GetGBM()->HaveHoverObject() )
		return FALSE;

	return TRUE;
}

BOOL
CObjectBoard::UI_CheckBeforeUse_CheckOriginalObject( void )
{
	CBoardObject * pOrgObject = _GetGBM()->GetOrgObject();

	if( !pOrgObject )
		return FALSE;

	if( pOrgObject->IsUsing() )
		return FALSE;

	if( !_GetAvatar()->IsUsableItem( pOrgObject, TRUE ) )
		return FALSE;

	return TRUE;
}

//------------------------------------------------------------------------------

void
CObjectBoard::UI_UseResult( BOOL pi_bIsSuccessed )
{
	if( pi_bIsSuccessed )
		UI_UseResult_ProcessObject();

	_GetGBM()->ClearOperation();
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CObjectBoard::FrameMove( void )
{
	CBaseBoard::FrameMove();
}

LRESULT
CObjectBoard::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	return CBaseBoard::MsgProc( hWnd, uMsg, wParam, lParam );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CBoardObject *
CObjectBoard::GetObject( BYTE pi_byPackNo, BYTE pi_bySocketNo )
{
	if( pi_byPackNo >= m_byMaxPackNum )
		return NULL;

	return m_listItemPack[pi_byPackNo].GetObject( pi_bySocketNo );
}

CBoardObject *
CObjectBoard::GetObjectByIndex( DWORD pi_dwIndex )
{
	CBoardObject * pObject;
	for( int i = 0; i < m_byMaxPackNum; ++i )
	{
		pObject = m_listItemPack[i].GetObjectByIndex( pi_dwIndex );
		if( pObject )
			return pObject;
	}

	return NULL;
}

BOOL
CObjectBoard::GetObjectPosition( BYTE & po_byPackNo, BYTE & po_bySocketNo, DWORD pi_dwIndex )
{
	for( po_byPackNo = 0; po_byPackNo < m_byMaxPackNum; ++po_byPackNo )
	{
		po_bySocketNo = m_listItemPack[po_byPackNo].GetObjectPosition( pi_dwIndex );
		if( po_bySocketNo != 0xFF )
			return TRUE;
	}

	return FALSE;
}

BOOL	
CObjectBoard::IsExistObject( CBoardObject * pi_pObject )
{
	CBoardObject * pObject;
	for( int i = 0; i < m_byMaxPackNum; ++i )
	{
		for( int j = 0; j < m_listItemPack[i].GetMaxSocketNum(); ++j )
		{
			pObject = m_listItemPack[i].GetObject( j );		
			if( pObject && pObject == pi_pObject )
				return TRUE;
		}
	}

	return FALSE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CObjectBoard::FindEmptySocket( BYTE & po_byPackNo, BYTE & po_bySocketNo )
{
	for( po_byPackNo = 0; po_byPackNo < m_byMaxPackNum; ++po_byPackNo )
	{
		po_bySocketNo = m_listItemPack[po_byPackNo].FindEmptySocket();
		if( po_bySocketNo != 0xFF )
			return TRUE;
	}

	return FALSE;
}

BYTE
CObjectBoard::FindEmptySocket( BYTE pi_byPackNo )
{
	if( pi_byPackNo >= m_byMaxPackNum )
		return 0;

	return m_listItemPack[pi_byPackNo].FindEmptySocket();
}

BYTE
CObjectBoard::GetEmptySocketNum( void )
{
	BYTE byEmptySocketNum = 0;

	for( int i = 0; i < m_byMaxPackNum; ++i )
		byEmptySocketNum += m_listItemPack[i].GetEmptySocketNum();

	return byEmptySocketNum;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////
