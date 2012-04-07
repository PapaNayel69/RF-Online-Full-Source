////////////////////////////////////////////////////////////////////////////////
//
// CForceBoard Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _NEW_UI_

#include "../../Manager/CCharacterMgr.h"
#include "../Common/CBoardManager.h"

#include "../../../Network/NetMsgProcessor/Item/CNetMsgProcessor_ItemArrangement.h"

#include "../../../UserInterface/GUIController/GUIMessagebox.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CForceBoard::CForceBoard()
{
}

CForceBoard::~CForceBoard()
{
}

void
CForceBoard::CreateBoardObject( BYTE pi_byTotalBoardNum, BYTE pi_byTotalMasteryNum, BYTE pi_byTotalSFNum )
{
	// Accretia 라도 보드자체는 생성시켜 놔야 한다.
	CSFBoard::CreateBoardObject( pi_byTotalBoardNum, pi_byTotalMasteryNum, pi_byTotalSFNum );

	if( _GetAvatar()->IsEqualRace( CRT_ACCRETIA ) )
		return;

	CSFItem	clSFItem;
	BYTE	byPackNo, bySocketNo;
	for( int i = 0; i < _GetCharDataMgr()->GetTotalForceNum(); ++i )
	{
		// create
		if( !clSFItem.Create( CAT_FORCE, i ) )
			continue;

		// 아바타 종족의 사용여부에 따라서 삽입한다.
		if( !clSFItem.IsUsableRace( _GetAvatar()->GetRace() ) )		
			continue;		

		// add force
		byPackNo = clSFItem.GetUIClassType();
		if( byPackNo >= pi_byTotalBoardNum )
			continue;
		bySocketNo = m_listItemPack[byPackNo].FindEmptySocket();

		Add( byPackNo, bySocketNo, &clSFItem );
	}		
}


void	
CForceBoard::InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY )
{
	// Accretia는 창을 못열게 한다
	if( _GetAvatar()->IsEqualRace( CRT_ACCRETIA ) )
		return;

	m_uiWindow = &m_uiForceWindow;

	CBaseBoard::InitWindow( pi_dwID, pi_nPosX, pi_nPosY );

	m_uiForceWindow.SetRaceType( _GetAvatar()->GetRace() );
}


//============================================================================//
//                                  Boundary                                  //
//============================================================================//


BOOL
CForceBoard::UI_CheckBeforeAdd_CheckEmptySocket( void )
{
	return TRUE;
}

BOOL
CForceBoard::UI_CheckBeforeAdd_CheckHoverObject( void )
{
	if( !CSFBoard::UI_CheckBeforeAdd_CheckHoverObject() )
		return FALSE;

	CBoardObject * pHoverObject	= _GetGBM()->GetHoverObject();

	// 포스
	if( pHoverObject->IsEqualItemType( BOT_SF ) )
	{
		CSFItem * pHoverSFItem	= static_cast< CSFItem * >( _GetGBM()->GetHoverObject() );
		if( pHoverSFItem->IsEqualSFType( CAT_FORCE ) )
			return TRUE;		
	}	
	// 포스 아이템 
	else if( pHoverObject->IsEqualItemType( BOT_BOARD_ITEM ) )
	{
		CBoardItem * pHoverItem	= static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );
		if( pHoverItem->IsEqualItemKind( IEPT_FORCE ) )
			return TRUE;
	}

	return FALSE;
}

BOOL
CForceBoard::UI_CheckBeforeAdd_CheckTargetObject( void )
{
	if( !CSFBoard::UI_CheckBeforeAdd_CheckTargetObject() )
		return FALSE;

	// 포스 아이템인 경우에만
	if( _GetGBM()->GetOrgBoardType() == IBT_INVENTORY )
	{
		CSFItem * pTarItem	= static_cast< CSFItem * >( _GetGBM()->GetTarObject() );	

		// 사용 가능 포스인지를 체크
		if( !pTarItem->IsUsableRace( _GetAvatar()->GetRace() ) )
		{
			_GUIMessageBox( NULL, "아바타가 사용할 수 없는 포스입니다.", 0xAAFFFFFF );
			return FALSE;
		}

		// 이미 포스가 있는지를 체크
		if( !pTarItem->IsEqualState( CSFItem::SFS_NOT_EXIST ) )
		{
			_GUIMessageBox( NULL, "포스창에 이미 등록되어 있는 포스입니다.", 0xAAFFFFFF );
			return FALSE;
		}
	}

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CForceBoard::UI_AddRequest_ProcessObject( void )
{
	CSFItem * pTarItem = static_cast< CSFItem * >( _GetGBM()->GetTarObject() );	

	if( _GetGBM()->GetOrgBoardType() == IBT_FORCE )
	{
		UI_AddResult( TRUE );
	}
	else if( _GetGBM()->GetOrgBoardType() == IBT_INVENTORY )
	{
		// 포스 아이템을 포스창으로 이동시킨다.
		if( pTarItem->IsEqualState( CSFItem::SFS_NOT_EXIST ) )
			m_pNetMsgProcessor_Arrange->ForceInvenChangeRequest_clzo();
	}
	else if( _GetGBM()->GetOrgBoardType() == IBT_LINK )
	{
		if( pTarItem->IsEqualState( CSFItem::SFS_LINK ) )
			UI_AddResult( TRUE );
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

void	
CForceBoard::UI_AddResult_ProcessObject( void )
{
	CSFItem * pTarItem = static_cast< CSFItem * >( _GetGBM()->GetTarObject() );		

	BOOL bIsSetState = FALSE;

	if( _GetGBM()->GetOrgBoardType() == IBT_FORCE )
	{
		bIsSetState = TRUE;
	}
	else if( _GetGBM()->GetOrgBoardType() == IBT_INVENTORY )
	{
		CBoardItem * pHoverItem = static_cast< CBoardItem * >( _GetGBM()->GetHoverObject() );

		// force item -> force
		_GetGBM()->ConvertBoardObject( pTarItem, pHoverItem );

		PlayWave( SND_ID_ITEM_ADD_IN_PACK );
		bIsSetState = TRUE;
	}
	else if( _GetGBM()->GetOrgBoardType() == IBT_LINK )
	{
		bIsSetState = TRUE;
	}

	if( bIsSetState )
	{
		if( _GetAvatar()->IsUsableItem( pTarItem ) )
			pTarItem->SetState( CSFItem::SFS_ACTIVE );
		else
			pTarItem->SetState( CSFItem::SFS_DEACTIVE );
	}

	if( _GetGBM()->GetTarPackNo() != m_uiForceWindow.GetCurBoardTab() )
		m_uiForceWindow.SetCurBoardTab( _GetGBM()->GetTarPackNo() );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL	
CForceBoard::UI_CheckBeforeRemove_CheckOriginalObject( BYTE pi_byQuantity )
{
	if( !CSFBoard::UI_CheckBeforeRemove_CheckOriginalObject( pi_byQuantity ) )
		return FALSE;

	CSFItem * pOrgItem = static_cast< CSFItem * >( _GetGBM()->GetOrgObject() );
	if( pOrgItem->IsEqualState( CSFItem::SFS_LINK ) ||
		pOrgItem->IsEqualState( CSFItem::SFS_NOT_EXIST ) )
		return FALSE;

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CForceBoard::IsUsableMastery( BYTE pi_byMasteryType, BYTE pi_byStepType )
{
	BYTE l_byClassType = pi_byMasteryType / 4;

	if( _GetAvatar()->IsBellato() && ( l_byClassType == FCT_DARK ) )
		return FALSE;
	else if( _GetAvatar()->IsCora() && ( l_byClassType == FCT_HOLY ) )
		return FALSE;
	else if( _GetAvatar()->IsAccretia() )
		return FALSE;

	return CSFBoard::IsUsableMastery( pi_byMasteryType, pi_byStepType );
}


BYTE
CForceBoard::UI_GetClassType( BYTE pi_byMasteryType )
{
	BYTE byUIClassType = pi_byMasteryType / 4;

	// 벨라토 종족의 경우는 홀리부터 시작하므로 byClassType 에서 1을 뺀다.
	if( _GetAvatar()->IsBellato() )
	{
		// 다크가 없으므로 0xFF
		if( byUIClassType == 0 )
			return 0xFF;

		--byUIClassType;
	}
	// 코라 종족의 경우는 홀리가 없으므로 다크를 제외한 byClassType 에서 1을 뺀다.
	else
	{
		// 홀리가 없으므로 0xFF
		if( byUIClassType == 1 )
			return 0xFF;

		if( byUIClassType > 0 )
			--byUIClassType;
	}

	return byUIClassType;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BYTE
CForceBoard::CalculateForceExpertness( DWORD * pi_pSuccessCountOfMastery )
{
	static const float	EXPERTNESS_CONSTANT[MAX_SF_STEP] = { 1.125f, 2.25f, 3.375f, 4.5f };

	if( !pi_pSuccessCountOfMastery )
		pi_pSuccessCountOfMastery = m_dwSuccessCountOfMastery;

	DWORD	l_dwForceExpertness = 0;
	double	l_dbTempForceExpertness1 = 0;
	double	l_dbTempForceExpertness2 = 0;
	double	l_dbTotalCountEachStep;
	for( int i = 0; i < MAX_SF_STEP; ++i )
	{
		l_dbTotalCountEachStep = 0;
		for( int j = 0; j < MAX_FORCE_CLASS_TYPE; ++j )
			l_dbTotalCountEachStep += pi_pSuccessCountOfMastery[ i + ( j * MAX_SF_STEP ) ];

		l_dbTempForceExpertness1 += l_dbTotalCountEachStep * EXPERTNESS_CONSTANT[i];
	}
	++l_dbTempForceExpertness1;

	l_dbTempForceExpertness2 = l_dbTempForceExpertness1 / 1000.0f;
	l_dbTempForceExpertness2 += sqrt( l_dbTempForceExpertness1 );

	l_dwForceExpertness = sqrt( l_dbTempForceExpertness2 );
	if( l_dwForceExpertness <= 1 )
		l_dwForceExpertness;
	if( l_dwForceExpertness >= 99 )
		l_dwForceExpertness = 99;

	//--------------------------------------------------------------------------

	float	l_fRate = 0.0f;
	double	l_dbSC_AfterLevelUp	= 0;		// 레벨업 이후의 누적 횟수
	double	l_dbNeedSC_ForLevelUp = 0;		// 다음 레벨 까지 필요한 성공 횟수

	if( l_dwForceExpertness >= 99 )
	{
	   l_fRate = 1.0f;
	}
	else
	{
		if( l_dwForceExpertness < 2 )
		{
			l_dbSC_AfterLevelUp = l_dbTempForceExpertness1;
			l_dbNeedSC_ForLevelUp = GetSuccessCountForEachExpertnessLevel( l_dwForceExpertness + 1 );
		}
		else
		{
			l_dbSC_AfterLevelUp = l_dbTempForceExpertness1 - GetSuccessCountForEachExpertnessLevel( l_dwForceExpertness );
			l_dbNeedSC_ForLevelUp = GetSuccessCountForEachExpertnessLevel( l_dwForceExpertness + 1 ) -
									GetSuccessCountForEachExpertnessLevel( l_dwForceExpertness );
		}

		l_fRate = (float)l_dbSC_AfterLevelUp / (float)l_dbNeedSC_ForLevelUp;
	}

	if( _GetCharInfoWindow() )
	{
		_GetCharInfoWindow()->SetBattleMastery( EVT_FORCE, l_dwForceExpertness );
		_GetCharInfoWindow()->SetBattleMasteryExp( EVT_FORCE, l_fRate );
	}

	return l_dwForceExpertness;
}

DWORD	
CForceBoard::GetSuccessCountForEachExpertnessLevel( BYTE pi_byExpertness )
{
	DWORD l_dwTempA = ( -1000 + sqrt( pow( 1000, 2 ) + 4 * pow( pi_byExpertness, 2 ) * 1000 ) );
	DWORD l_dwTempB = ( DWORD )pow( ( l_dwTempA / 2.0f ), 2 );

	return l_dwTempB;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

LRESULT	
CForceBoard::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case WM_BOARD_ADD_OBJECT	:
			{
				CBoardObject * pHoverObject = _GetGBM()->GetHoverObject();
				if( !pHoverObject )
					break;

				if( pHoverObject->IsEqualItemType( BOT_SF ) )
				{
					BYTE byPackNo, bySocketNo;

					if( GetObjectPosition( byPackNo, bySocketNo, pHoverObject->GetIndex() ) )
						UI_AddRequest( byPackNo, bySocketNo );
				}
				else if( pHoverObject->IsEqualItemType( BOT_BOARD_ITEM ) )
				{
					CBoardItem * pHoverItem = static_cast< CBoardItem * >( pHoverObject );
					if( pHoverItem->GetItemKind() == IEPT_FORCE )
					{
						FORCE_ITEM_DATA *	pForceData;
						pForceData = ( FORCE_ITEM_DATA * )_GetItemDataMgr()->GetData( IEPT_FORCE, pHoverItem->GetDTIndex() );
						if( !pForceData )
							break;

						CSFItem * pSFItem = GetSFByDTCode( pForceData->dwForceDTCode );
						if( !pSFItem )
							break;

						BYTE byPackNo, bySocketNo;
						if( GetObjectPosition( byPackNo, bySocketNo, pSFItem->GetIndex() ) )
							UI_AddRequest( byPackNo, bySocketNo );						
					}
				}
			}
			break;
	}

	return CSFBoard::MsgProc( hWnd, uMsg, wParam, lParam );
}

#endif // _NEW_UI_