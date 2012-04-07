////////////////////////////////////////////////////////////////////////////////
//
// CSFBoard Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _NEW_UI_

#include "../../Manager/CCharacterMgr.h"
#include "../../Manager/CTargetMgr.h"
#include "../Common/CBoardManager.h"

#include "../../../Network/NetMsgProcessor/Item/CNetMsgProcessor_ItemArrangement.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

void	DisplaySFInfo( CSFItem * pi_pSFItem );

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CSFBoard::CSFBoard()
{
	m_pNetMsgProcessor_Arrange	= static_cast< CNetMsgProcessor_ItemArrangement * >( _GetNetworkMgr()->GetNetMsgProcessor( NMP_ITEM_ARRANGE ) );

	m_dwSuccessCountOfMastery	= NULL;
	m_byPointOfMastery			= NULL;
	m_bIsUsableMastery			= NULL;

	m_dwCastTime				= NULL;
}

CSFBoard::~CSFBoard()
{
	_DELETE_ARRAY( m_dwSuccessCountOfMastery );
	_DELETE_ARRAY( m_byPointOfMastery );
	_DELETE_ARRAY( m_bIsUsableMastery );

	_DELETE_ARRAY( m_dwCastTime );
}

void	
CSFBoard::CreateBoardObject( BYTE pi_byTotalBoardNum, BYTE pi_byTotalMasteryNum, BYTE pi_byTotalSFNum )
{
	for( int i = 0; i < pi_byTotalBoardNum; ++i )	
		AddPack( i );

	m_byTotalMasteryNum			= pi_byTotalMasteryNum;
	m_dwSuccessCountOfMastery	= new DWORD[pi_byTotalMasteryNum];
	m_byPointOfMastery			= new BYTE[pi_byTotalMasteryNum];
	m_bIsUsableMastery			= new BYTE[pi_byTotalMasteryNum];

	m_byTotalSFNum				= pi_byTotalSFNum;
	m_dwCastTime				= new DWORD[pi_byTotalSFNum];

	ZeroMemory( m_dwSuccessCountOfMastery, sizeof( DWORD ) * pi_byTotalMasteryNum );
	ZeroMemory( m_byPointOfMastery, sizeof( BYTE ) * pi_byTotalMasteryNum );
	ZeroMemory( m_bIsUsableMastery, sizeof( BYTE ) * pi_byTotalMasteryNum );

	ZeroMemory( m_dwCastTime, sizeof( DWORD ) * pi_byTotalSFNum );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CSFBoard::Add_ProcessExistObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject )
{
	CSFItem * pBoardItem = static_cast< CSFItem * >( GetObject( pi_byTarPackNo, pi_byTarSocketNo ) );
	if( !pBoardItem )
		return FALSE;	

	//if( !pBoardItem->IsSameKind( pOrgBoardItem ) )
	//	return FALSE;

	return TRUE;
}

BOOL
CSFBoard::Add_ProcessNewObject( BYTE pi_byTarPackNo, BYTE pi_byTarSocketNo, CBoardObject * pi_pOrgObject )
{
	CSFItem * pNewItem = static_cast< CSFItem * >( pi_pOrgObject->Clone() );
	if( !pNewItem )
		return FALSE;

	// set ui - take in item
	CGUISFWindow * pWindow = static_cast< CGUISFWindow * >( m_uiWindow );
	if( !pWindow )
		return FALSE;

	_skill_force_info	uiItem;
	ZeroMemory( &uiItem, sizeof( _skill_force_info ) );

	uiItem.bySFType			= pNewItem->GetSFType();
	uiItem.dwIndex			= pNewItem->GetIndex();
	uiItem.byStep			= pNewItem->GetStep();	
	uiItem.dwItemID			= pNewItem->GetDTCode();
	uiItem.dwSpriteID		= pNewItem->GetIconID();
	uiItem.byLevel			= pNewItem->GetLevel();
	uiItem.fExpByPercent	= pNewItem->GetExpByPercent();
	uiItem.pName			= pNewItem->GetData()->pName_Kor;
		
	pWindow->TakeInItem( pi_byTarPackNo, &uiItem );

	// set ui - get icon board
	pNewItem->SetIconBoard( pWindow->GetIconBoard( pi_byTarPackNo, pNewItem->GetIndex() ) );
	if( pNewItem->IsEqualSFType( CAT_SKILL ) )
		pNewItem->SetState( CSFItem::SFS_DEACTIVE );
	else if( pNewItem->IsEqualSFType( CAT_CLASS_SKILL ) )
		pNewItem->SetState( CSFItem::SFS_ACTIVE );
	else
		pNewItem->SetState( CSFItem::SFS_NOT_EXIST );

	// set data
	m_listItemPack[pi_byTarPackNo].Add( pi_byTarSocketNo, pNewItem );

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CSFBoard::Remove_ProcessRemoveQuantity( CBoardObject * pi_pOrgObject, BYTE pi_byQuantity )
{
	return FALSE;
}
	
BOOL
CSFBoard::Remove_ProcessRemoveObject( BYTE pi_byPackNo, BYTE pi_bySocketNo, CBoardObject * pi_pOrgObject )
{
	CGUISFWindow * pWindow = static_cast< CGUISFWindow * >( m_uiWindow );

	pWindow->TakeOutItem( pi_byPackNo, pi_pOrgObject->GetIndex() );
	m_listItemPack[pi_byPackNo].Remove( pi_bySocketNo );

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CSFBoard::UI_CheckBeforeRemove_CheckOriginalObject( BYTE pi_byQuantity )
{
	if( !CObjectBoard::UI_CheckBeforeRemove_CheckOriginalObject( pi_byQuantity ) )
		return FALSE;

	CSFItem * pOrgItem = static_cast< CSFItem * >( _GetGBM()->GetOrgObject() );	
	
	// delay가 적용되었으면 못꺼낸다
	CGUISFItem * pIcon = static_cast< CGUISFItem * >( pOrgItem->GetIcon() );
	if( pIcon && pIcon->IsDelayAnimation() )
		return FALSE;

	return TRUE;
}

BOOL	
CSFBoard::UI_RemoveRequest_ProcessObject( BYTE pi_byQuantity )
{
	CSFItem * pOrgItem = static_cast< CSFItem * >( _GetGBM()->GetOrgObject() );	

	pOrgItem->SetState( CSFItem::SFS_LINK );
	
	_GetGBM()->SetHoverObject( pOrgItem );

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CSFBoard::UI_SwapRequest_ProcessObject( void )
{
	return TRUE;
}

void
CSFBoard::UI_SwapResult_ProcessObject( void )
{
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CSFBoard::UI_CheckBeforeUse_CheckOriginalObject( void )
{
	if( !CObjectBoard::UI_CheckBeforeUse_CheckOriginalObject() )
		return FALSE;

	CSFItem * pOrgSFItem = static_cast< CSFItem * >( _GetGBM()->GetOrgObject() );

	// 공격형일 경우에는 전투모드에서만 선택 가능
	if( pOrgSFItem->IsAttackable() && ( _GetAvatar()->GetCombatMode() == CSI_PEACE ) )
	{
		_GetAvatar()->SetRequestChangeToCombatMode( TRUE );
		return FALSE;
	}

	return TRUE;	
}

// 스킬/포스 사용 요청
BOOL
CSFBoard::UI_UseRequest_ProcessObject( void )
{
	CSFItem * pOrgSFItem = static_cast< CSFItem * >( _GetGBM()->GetOrgObject() );

	// 동일한 스킬/포스 선택 횟수 설정
	if( !_GetGBM()->GetSelectSF() || pOrgSFItem->IsEqual( _GetGBM()->GetSelectSF() ) )
		_GetGBM()->SetSelectSFCountInUI( _GetGBM()->GetSelectSFCountInUI() + 1 );

	// 스킬/포스 저장
	_GetGBM()->SelectSF( pOrgSFItem );

	// 2번이상 동일한 스킬/포스를 선택했다면 무조건 아바타에게 시전되는 것이므로, 타겟을 아바타로 설정.
	CCharacter * pTarget = NULL;
	if( _GetGBM()->IsSFEventInLinkBoard() &&
		_GetGBM()->GetSelectSFCountInUI() >= 2 && pOrgSFItem->IsEqualTargetType( SFT_SAME_RACE ) )
		pTarget = _GetAvatar();
	else if( _GetGBM()->GetSFTarget() )
		pTarget = _GetGBM()->GetSFTarget();
	else
		pTarget = _GetTargetMgr()->GetTarget();

	if( pTarget && pTarget->IsEqualCharType( CTI_NPC ) )
		pTarget = NULL;

	// check target
	ChangeCursor( pOrgSFItem );
	if( IsEqualSFTargetTypeAndAvatarTarget( pOrgSFItem, pTarget ) )
	{
		return InvokeSkillForce( pOrgSFItem, pTarget );
	}

	return FALSE;
}

void
CSFBoard::UI_UseResult_ProcessObject( void )
{
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CSFBoard::IsEqualSFTargetTypeAndAvatarTarget( CSFItem * pi_pSFItem, CCharacter * pi_pTarget )
{	
	BOOL bIsEqualTargetType = FALSE;

	// 아바타의 타겟과 스킬/포스의 타겟 타입을 비교
	if( pi_pSFItem->IsEqualTargetType( SFT_ANYWHERE ) || pi_pSFItem->IsEqualTargetType( SFT_MAP ) )
	{
		bIsEqualTargetType = TRUE;
	}
	if( !bIsEqualTargetType && pi_pSFItem->IsEqualTargetType( SFT_ENEMY_NPC ) )
	{
		if( pi_pTarget && ( pi_pTarget->IsEqualCharType( CTI_MONSTER ) || pi_pTarget->IsEqualCharType( CTI_ANIMUS ) || pi_pTarget->IsEqualCharType( CTI_GUARD_TOWER ) ) )
			bIsEqualTargetType = _GetAvatar()->CheckTargetConditionForCast_EnemyNpc( pi_pTarget );
	}
	if( !bIsEqualTargetType && pi_pSFItem->IsEqualTargetType( SFT_SAME_RACE ) )
	{
		if( pi_pTarget && ( pi_pTarget->IsEqualCharType( CTI_PLAYER ) || pi_pTarget->IsEqualCharType( CTI_AVATAR ) ) )
		{			
			if( _GetGBM()->IsSFEventInLinkBoard() )
			{
				if( !( pi_pTarget->IsEqualCharType( CTI_AVATAR ) && ( _GetGBM()->GetSelectSFCountInUI() >= 2 ) )  )					
					bIsEqualTargetType = FALSE;
				else
					bIsEqualTargetType = TRUE;
			}
			else
			{
				bIsEqualTargetType = _GetAvatar()->CheckTargetConditionForCast_Player( pi_pTarget, TRUE );
			}
		}
	}
	if( !bIsEqualTargetType && pi_pSFItem->IsEqualTargetType( SFT_ENEMY_PLAYER ) )
	{
		if( pi_pTarget && pi_pTarget->IsEqualCharType( CTI_PLAYER ) )
			bIsEqualTargetType = _GetAvatar()->CheckTargetConditionForCast_Player( pi_pTarget, FALSE );
	}

	return bIsEqualTargetType;
}

BOOL
CSFBoard::InvokeSkillForce( CSFItem * pi_pSFItem, CCharacter * pi_pTarget )
{
	// 움직일 수 있는가? 움직일 수 없다면 다음 상태로 저장해 둔다. => 아바타가 자동 공격 상태라면 저장한다.
	if( !_GetAvatar()->IsEnableMove() && _GetAvatar()->IsEnableAutoAttack() )
	{
		_GetNextActionMgr()->SetNextAction( CAI_SKILL_INVOKE );
		_GetNextActionMgr()->SetNextSF( pi_pSFItem );
		_GetNextActionMgr()->SetNextTarget( pi_pTarget );
		return TRUE;
	}

	// 타겟 타입이 SFT_ANYWHERE 라면 무조건 아바타에게만 적용되므로 곧장 발동시킨다.
	if( pi_pSFItem->IsEqualTargetType( SFT_ANYWHERE ) )	
	{
		return _GetAvatar()->Attack( pi_pSFItem->GetSFType(), _GetAvatar() );			
	}

	// 타겟이 없더라도 TargetType이 Map이면 발동 => 타겟 좌표가 미리 세팅되어 있어야만 한다.
	if( pi_pSFItem->IsEqualTargetType( SFT_MAP ) )
	{
		Vector3f l_vecSFTargetPos;
		_GetAvatar()->GetSFTargetPos( l_vecSFTargetPos );
		if( ( l_vecSFTargetPos[0] == 0 ) && ( l_vecSFTargetPos[1] == 0 ) && ( l_vecSFTargetPos[2] == 0 ) )
			return FALSE;

		return _GetAvatar()->Attack( pi_pSFItem->GetSFType(), NULL );
	}

	// 스킬/포스는 타겟이 설정되어 있다면 곧장 발동된다.
	if( pi_pTarget )
	{
		return _GetAvatar()->Attack( pi_pSFItem->GetSFType(), pi_pTarget );
	}

	return FALSE;
}

void
CSFBoard::ChangeCursor( CSFItem * pi_pSFItem )
{
	if( pi_pSFItem->IsEqualSFType( CAT_FORCE ) )
		_GetUIScreen()->SetMouseState( CGUIMouse::WAITING_FORCE );
	else
		_GetUIScreen()->SetMouseState( CGUIMouse::WAITING_SKILL );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CSFBoard::FrameMove( void )
{
	CObjectBoard::FrameMove();
}

LRESULT
CSFBoard::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	BYTE byPackNo, bySocketNo;

	switch( uMsg )
	{
		case WM_BOARD_REMOVE_OBJECT	:
			if( GetObjectPosition( byPackNo, bySocketNo, lParam ) )
				UI_RemoveRequest( byPackNo, bySocketNo, lParam );
			break;

		case WM_BOARD_USE_OBJECT	:
			{
				_GetAvatar()->ClearSFTargetPos();

				CSFItem * pSFItem = GetSFByIndex( lParam );
				if( pSFItem )
					UseSkillForce_Request( pSFItem, NULL, TRUE );
			}
			break;

		case WM_BOARD_EQUIP_HOT_KEY	:
			UI_Link_By_Hotkey( lParam );
			break;

		case WM_BOARD_HOVER_OBJECT	:
			{
				CSFItem * pSFItem = GetSFByIndex( lParam );
				if( pSFItem )
					DisplaySFInfo( pSFItem );										
			}			
			break;
	}
	
	return CObjectBoard::MsgProc( hWnd, uMsg, wParam, lParam );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CSFItem *
CSFBoard::GetSF( BYTE pi_bySocketNo )
{
	return static_cast< CSFItem * >( GetObject( 0, pi_bySocketNo ) );
}

CSFItem *
CSFBoard::GetSFByIndex( DWORD pi_dwIndex )
{
	CSFItem * pSFItem = NULL;

	for( int i = 0; i < m_byMaxPackNum; ++i )
	{
		for( int j = 0; j < m_listItemPack[i].m_byMaxSocketNum; ++j )
		{
			if( !m_listItemPack[i].m_listSocket[j] )
				continue;

			pSFItem = static_cast< CSFItem * >( m_listItemPack[i].m_listSocket[j] );
			if( pSFItem->GetIndex() == pi_dwIndex )
				return pSFItem;
		}
	}

	return NULL;
}

CSFItem *
CSFBoard::GetSFByDTIndex( DWORD pi_dwDTIndex )
{
	CSFItem * pSFItem = NULL;

	for( int i = 0; i < m_byMaxPackNum; ++i )
	{
		for( int j = 0; j < m_listItemPack[i].m_byMaxSocketNum; ++j )
		{
			if( !m_listItemPack[i].m_listSocket[j] )
				continue;

			pSFItem = static_cast< CSFItem * >( m_listItemPack[i].m_listSocket[j] );
			if( pSFItem->GetDTIndex() == pi_dwDTIndex )
				return pSFItem;
		}
	}

	return NULL;
}

CSFItem *
CSFBoard::GetSFByDTCode( DWORD pi_dwDTCode )
{
	CSFItem * pSFItem = NULL;

	for( int i = 0; i < m_byMaxPackNum; ++i )
	{
		for( int j = 0; j < m_listItemPack[i].m_byMaxSocketNum; ++j )
		{
			if( !m_listItemPack[i].m_listSocket[j] )
				continue;

			pSFItem = static_cast< CSFItem * >( m_listItemPack[i].m_listSocket[j] );
			if( pSFItem->GetDTCode() == pi_dwDTCode )
				return pSFItem;
		}
	}

	return NULL;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CSFBoard::IsUsableSF( CSFItem * pi_pSFItem )
{
	if( !pi_pSFItem )
		return FALSE;

	// class skill 은 무조건 사용 가능하다.
	if( pi_pSFItem->IsEqualSFType( CAT_CLASS_SKILL ) )
		return TRUE;

	// skill / force
	BYTE byMasteryType	= pi_pSFItem->GetMasteryIndex();
	BYTE byStepType		= pi_pSFItem->GetMasteryIndex() % 4;

	return IsUsableMastery( byMasteryType, byStepType );
}

BOOL
CSFBoard::IsUsableMastery( BYTE pi_byMasteryType, BYTE pi_byStepType )
{
	//static const BYTE LIMIT_MASTERY_CONSTANT[2][MAX_SF_STEP][MAX_SF_STEP] =
	BYTE LIMIT_MASTERY_CONSTANT[2][MAX_SF_STEP][MAX_SF_STEP] =
	{
		{	// CAT_SKILL
			0,   0,  0,  0,
			30,  0,  0,  0,
			60, 60,  0,  0,
			90, 90, 90,  0
		},
		{	// CAT_FORCE
			0,   0,  0,  0,
			30,  0,  0,  0,
			60, 60,  0,  0,
			90, 90, 90,  0
		},
	};

	BYTE bySFType = CAT_SKILL;
	if( m_byBoardType == IBT_FORCE )
		bySFType = CAT_FORCE;

	for( int i = 0; i < MAX_SF_STEP-1; ++i )
	{
		if( m_byPointOfMastery[pi_byMasteryType-pi_byStepType+i] < LIMIT_MASTERY_CONSTANT[bySFType][pi_byStepType][i] )
			return FALSE;
	}

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BYTE
CSFBoard::GetPointOfMastery( BYTE pi_byMasteryType )
{
	if( pi_byMasteryType >= m_byMaxPackNum )
		return 0;

	return m_byPointOfMastery[pi_byMasteryType];
}

BYTE	
CSFBoard::GetPointOfMasteryByDTCode( DWORD pi_dwDTCode )
{
	CSFItem * pSFItem = GetSFByDTCode( pi_dwDTCode );
	if( !pSFItem )
		return 0;

	return GetPointOfMastery( pSFItem->GetMasteryIndex() );
}

BYTE
CSFBoard::GetSFExpertness( BYTE pi_byUIClassTypeNo )
{
	return GetSFExpertness( pi_byUIClassTypeNo, m_dwSuccessCountOfMastery );
}

// pi_pSuccessCountList에 있는 스킬/포스의 성공 횟수를 사용해 스킬/포스의 각 클래스 별 숙련도를 계산한다.
// 저-중-고-최고급 마스터리합의 평균값
BYTE
CSFBoard::GetSFExpertness( BYTE pi_byUIClassTypeNo, DWORD * pi_pSuccessCountList )
{
	if( !pi_pSuccessCountList )
		return 0;

	int	nStartMasteryIndex	= pi_byUIClassTypeNo * MAX_SF_STEP;
	int	nEndMasteryIndex	= nStartMasteryIndex + MAX_SF_STEP;
	DWORD dwSFExpertness	= 0;

	for( int i = nStartMasteryIndex; i < nEndMasteryIndex; ++i )
		dwSFExpertness += CalculateSuccessCountOfMastery( i , i - nStartMasteryIndex, pi_pSuccessCountList[i], FALSE );

	return ( dwSFExpertness / MAX_SF_STEP );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 스킬/포스의 마스터리 설정
void
CSFBoard::SetSuccessCountOfMastery( BYTE pi_byMasteryType, BYTE pi_byStepType, DWORD pi_dwCount, BOOL pi_bIsAdd )
{
	BYTE byPointOfMastery = CalculateSuccessCountOfMastery( pi_byMasteryType, pi_byStepType,
															pi_dwCount, pi_bIsAdd );	

	if( pi_byStepType >= ( MAX_SF_STEP - 1 ) )	// 0, 1, 2, 3 이므로 ( MAX_SF_STEP - 1 ) 보다 작아야한다.
		return;

	if( !m_bIsUsableMastery[pi_byMasteryType+1] &&		
		IsUsableMastery( pi_byMasteryType+1, pi_byStepType+1 ) )
	{
		m_bIsUsableMastery[pi_byMasteryType+1] = TRUE;

		BYTE		byClassType_UI = UI_GetClassType( pi_byMasteryType );
		if( byClassType_UI >= m_byMaxPackNum )
			return;

		CSFItem *	pSFItem;
		for( int i = 0; i < m_listItemPack[byClassType_UI].GetMaxSocketNum(); ++i )
		{
			pSFItem = static_cast< CSFItem * >( GetObject( byClassType_UI, i ) );
			if( !pSFItem )
				continue;
			if( !pSFItem->IsHave() )
				continue;

			if( pSFItem->GetMasteryIndex() == pi_byMasteryType + 1 )
			{
				if( _GetLinkBoard()->IsExistObject( pSFItem ) )
					pSFItem->SetState( CSFItem::SFS_LINK );
				else
					pSFItem->SetState( CSFItem::SFS_ACTIVE );
			}
		}
	}
}

BYTE
CSFBoard::CalculateSuccessCountOfMastery( BYTE pi_byMasteryType, BYTE pi_byStepType, DWORD pi_dwCount, BOOL pi_bIsAdd )
{
	if( ( pi_byMasteryType >= m_byTotalMasteryNum ) || ( pi_byStepType >= MAX_SF_STEP ) )
		return 1;

	if( pi_bIsAdd )
		m_dwSuccessCountOfMastery[pi_byMasteryType] += pi_dwCount;
	else
		m_dwSuccessCountOfMastery[pi_byMasteryType] = pi_dwCount;

	static const float MASTERY_CONSTANT[4] = { 40.0f, 40.0f, 40.0f, 40.0f };

	DWORD dwPointOfMastery = ( sqrt( ( ( m_dwSuccessCountOfMastery[pi_byMasteryType] + 1 ) / MASTERY_CONSTANT[pi_byStepType] ) * 1.0f ) + 0.99f );

	if( dwPointOfMastery > 99 )
		m_byPointOfMastery[pi_byMasteryType] = 99;
	else if( dwPointOfMastery < 1 )
		m_byPointOfMastery[pi_byMasteryType] = 1;
	else
		m_byPointOfMastery[pi_byMasteryType] = dwPointOfMastery;

	// update mastery ui	
	CGUISFWindow * pWindow = static_cast< CGUISFWindow *>( m_uiWindow );
	if( pWindow )
	{
		pWindow->SetLevelMastery( UI_GetClassType( pi_byMasteryType ), pi_byStepType,
								  m_byPointOfMastery[pi_byMasteryType] );
	}

	return m_byPointOfMastery[pi_byMasteryType];
}

void
CSFBoard::UseSkillForce_Request( CSFItem * pi_pSFItem, CCharacter * pi_pTarget, BOOL pi_bEventInLinkBoard )
{
	BYTE	byPackNo;
	BYTE	bySocketNo;

	if( !GetObjectPosition( byPackNo, bySocketNo, pi_pSFItem->GetIndex() ) )
		return;

	_GetGBM()->SetSFTarget( pi_pTarget );
	_GetGBM()->SetSFEventInLinkBoard( pi_bEventInLinkBoard );

	UI_UseRequest( byPackNo, bySocketNo );
}

void
CSFBoard::UI_Link_By_Hotkey( DWORD pi_dwIndex )
{
	BYTE byOrgPackNo, byOrgSocketNo;

	if( !GetObjectPosition( byOrgPackNo, byOrgSocketNo, pi_dwIndex ) )
		return;	
	
	BYTE byTarSocketNo;
	if( ( byTarSocketNo = _GetLinkBoard()->FindEmptySocket() ) == 0xFF )
	{
		_SetSystemMsg( "빈 소켓이 없습니다." );
		return;
	}

	if( !UI_RemoveRequest( byOrgPackNo, byOrgSocketNo, 1 ) )
		return;

	_GetLinkBoard()->UI_AddRequest( 0, byTarSocketNo );
}

void
CSFBoard::Unlink( DWORD pi_dwIndex )
{
	CSFItem * pOrgItem = GetSFByIndex( pi_dwIndex );
	if( !pOrgItem )
		return;
	if( pOrgItem->IsEqualState( CSFItem::SFS_NOT_EXIST ) )
		return;

	// remove link
	BYTE byPackNo, bySocketNo;
	if( !_GetLinkBoard()->GetObjectPosition( byPackNo, bySocketNo, pOrgItem->GetIndex() ) )
		return;
	_GetLinkBoard()->Remove( byPackNo, bySocketNo );

	// set state
	if( _GetAvatar()->IsUsableItem( pOrgItem ) )
		pOrgItem->SetState( CSFItem::SFS_ACTIVE );
	else
		pOrgItem->SetState( CSFItem::SFS_DEACTIVE );
}

#endif // _NEW_UI_