////////////////////////////////////////////////////////////////////////////////
//
// CSkillBoard Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _NEW_UI_

#include "../../Manager/CCharacterMgr.h"
#include "../../Manager/CTargetMgr.h"
#include "../Common/CBoardManager.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CSkillBoard::CSkillBoard()
{

}

CSkillBoard::~CSkillBoard()
{

}

void
CSkillBoard::InitWindow( DWORD pi_dwID, int pi_nPosX, int pi_nPosY )
{
	m_uiWindow = &m_uiSkillWindow;

	CBaseBoard::InitWindow( pi_dwID, pi_nPosX, pi_nPosY );
}

void	
CSkillBoard::CreateBoardObject( BYTE pi_byTotalBoardNum, BYTE pi_byTotalMasteryNum, BYTE pi_byTotalSFNum )
{
	CSFBoard::CreateBoardObject( pi_byTotalBoardNum, pi_byTotalMasteryNum, pi_byTotalSFNum );	

	CSFItem	clSkillItem;
	BYTE	byPackNo, bySocketNo;
	for( int i = 0; i < _GetCharDataMgr()->GetTotalSkillNum(); ++i )
	{
		if( !clSkillItem.Create( CAT_SKILL, i ) )
			continue;

		// ID로 사용 가능 여부를 체크한다. ( 0x000FFF00 이면 사용하지 않는다. )
		if( ( clSkillItem.GetDTCode() & 0x000FF000 ) == 0x000FF000 )
			continue;

		// 아바타 종족의 사용여부에 따라서 삽입한다.
		if( !clSkillItem.IsUsableRace( _GetAvatar()->GetRace() ) )			
			continue;

		// add skill
		byPackNo = clSkillItem.GetUIClassType();
		if( byPackNo >= pi_byTotalBoardNum )
			continue;
		bySocketNo = m_listItemPack[byPackNo].FindEmptySocket();

		Add( byPackNo, bySocketNo, &clSkillItem );
	}		
	
}

// 아바타 클래스의 DT Index를 받아서 클래스 스킬을 세팅한다.
void
CSkillBoard::CreateClassSkill( DWORD pi_dwClassDTIndex )
{
#ifdef _DEVELOPER_
	CPlayerClass * pClass = _GetCharDataMgr()->GetClassDataByDTIndex( pi_dwClassDTIndex );
	if( !pClass )
		return;

	SF_DATA *	pSkillData;
	CSFItem		clNewClassSkill;
	CSFItem *	pAddedClassSkill = NULL;
	BYTE		byPackNo, bySocketNo;

	for( int i = 0; i < CPlayerClass::MAX_SKILL_NUM; ++i )
	{
		pSkillData = pClass->GetClassSkill( i );
		if( !pSkillData )
			continue;

		// create class skill
		if( GetSkillByDTCode( pSkillData->dwDTCode ) )
			continue;
		if( !clNewClassSkill.Create( CAT_CLASS_SKILL, pSkillData->dwDTIndex ) )
			continue;

		// add skill
		byPackNo	= clNewClassSkill.GetUIClassType();
		bySocketNo	= _GetSkillBoard()->FindEmptySocket( byPackNo );

		if( !Add( byPackNo, bySocketNo, &clNewClassSkill ) )
			continue;

		// set property
		pAddedClassSkill = GetSkill( byPackNo, bySocketNo );
		if( !pAddedClassSkill )
			continue;

		pAddedClassSkill->SetHave( TRUE );
		pAddedClassSkill->SetState( CSFItem::SFS_ACTIVE );
	}
#endif
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CSkillBoard::UI_CheckBeforeAdd_CheckEmptySocket( void )
{
	return TRUE;
}

BOOL
CSkillBoard::UI_CheckBeforeAdd_CheckHoverObject( void )
{
	if( !CSFBoard::UI_CheckBeforeAdd_CheckHoverObject() )
		return FALSE;

	CBoardObject * pHoverObject	= _GetGBM()->GetHoverObject();
	if( !pHoverObject->IsEqualItemType( BOT_SF ) )
		return FALSE;	

	CSFItem * pHoverItem = static_cast< CSFItem * >( pHoverObject );	
	if( pHoverItem->IsEqualSFType( CAT_FORCE ) )
		return FALSE;

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CSkillBoard::UI_AddRequest_ProcessObject( void )
{
	CSFItem * pTarItem = static_cast< CSFItem * >( _GetGBM()->GetTarObject() );	

	if( _GetGBM()->GetOrgBoardType() == IBT_SKILL )
	{
		UI_AddResult( TRUE );
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
CSkillBoard::UI_AddResult_ProcessObject( void )
{
	CSFItem * pTarItem = static_cast< CSFItem * >( _GetGBM()->GetTarObject() );		

	BOOL bIsSetState = FALSE;

	if( _GetGBM()->GetOrgBoardType() == IBT_SKILL )
	{
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

	if( _GetGBM()->GetTarPackNo() != m_uiSkillWindow.GetCurBoardTab() )
		m_uiSkillWindow.SetCurBoardTab( _GetGBM()->GetTarPackNo() );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL	
CSkillBoard::UI_CheckBeforeRemove_CheckOriginalObject( BYTE pi_byQuantity )
{
	if( !CSFBoard::UI_CheckBeforeRemove_CheckOriginalObject( pi_byQuantity ) )
		return FALSE;

	CSFItem * pOrgItem = static_cast< CSFItem * >( _GetGBM()->GetOrgObject() );
	if( !pOrgItem->IsEqualState( CSFItem::SFS_ACTIVE ) )		
		return FALSE;

	return TRUE;
}


//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CSkillBoard::IsUsableMastery( BYTE pi_byMasteryType, BYTE pi_byStepType )
{
	return CSFBoard::IsUsableMastery( pi_byMasteryType, pi_byStepType );
}

BYTE
CSkillBoard::UI_GetClassType( BYTE pi_byMasteryType )
{
	BYTE byUIClassType;

	if( pi_byMasteryType < 4 )
		byUIClassType = SCT_UI_MELEE;
	else if( pi_byMasteryType < 8 )
		byUIClassType = SCT_UI_MISSILE;
	else
		byUIClassType = SCT_UI_KNOWLEDGE;

	return byUIClassType;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CSFItem *
CSkillBoard::GetSkillByDTCode( DWORD pi_dwDTCode )
{
	const DWORD const CLASS_SKILL_TAB = 2;

	CSFItem * pSkillItem = NULL;

	for( int i = 0; i < m_listItemPack[CLASS_SKILL_TAB].GetMaxSocketNum(); ++i )
	{
		pSkillItem = GetSkill( CLASS_SKILL_TAB, i );
		if( !pSkillItem )
			continue;

		if(	pSkillItem->GetDTCode() == pi_dwDTCode )
			return pSkillItem;
	}

	return NULL;
}

// 가드 타워 설치 스킬을 가져온다.
CSFItem *
CSkillBoard::GetGuardTowerSkill( void )
{
	const DWORD const CLASS_SKILL_TAB = 2;
	const DWORD const GUARD_TOWER_SKILL_EFFECT_TYPE = 19;

	CSFItem *	pSkillItem = NULL;
	int			i, j;
	for( i = 0; i < m_listItemPack[CLASS_SKILL_TAB].GetMaxSocketNum(); ++i )
	{
		pSkillItem = GetSkill( CLASS_SKILL_TAB, i );
		if( !pSkillItem )
			continue;

		for( j = 0; j < MAX_SF_EFFECT; ++j )
		{
			if( ( pSkillItem->GetUsingType( j ) == SFAT_SUPPORT_ONCE ) &&
				( pSkillItem->GetEffectType( j ) == GUARD_TOWER_SKILL_EFFECT_TYPE ) )
				return pSkillItem;
		}
	}

	return NULL;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

LRESULT
CSkillBoard::MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch( uMsg )
	{
		case WM_BOARD_ADD_OBJECT	:
			{
				CBoardObject * pHoverObject = _GetGBM()->GetHoverObject();
				if( !pHoverObject )
					break;
				if( !pHoverObject->IsEqualItemType( BOT_SF ) )
					break;

				BYTE byPackNo, bySocketNo;

				if( GetObjectPosition( byPackNo, bySocketNo, pHoverObject->GetIndex() ) )
					UI_AddRequest( byPackNo, bySocketNo );
			}
			break;
	}

	return CSFBoard::MsgProc( hWnd, uMsg, wParam, lParam );
}

#endif // _NEW_UI_