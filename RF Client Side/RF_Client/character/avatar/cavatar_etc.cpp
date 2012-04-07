#include "../../GameObject/Manager/CCharacterMgr.h"
#include "CAvatar_Quest.h"
#include "../../GameObject/Manager/CTargetMgr.h"
#include "../../Item/CItemData.h"

#include "../../UserInterface/GameWindow/GUIGaugeExpWindow.h"
#include "../../UserInterface/GameWindow/GUIHotKeyBoard.h"
#include "../../UserInterface/GameWindow/GUIForceBeltWindow.h"
#include "../../UserInterface/GameWindow/SkillForce/GUIForceWindow.h"
#include "../../UserInterface/GameWindow/SkillForce/GUISkillWindow.h"
#include "../../UserInterface/GameWindow/GUICharInfoWindow.h"
#include "../../UserInterface/GameWindow/GUISelectedCharWin.h"
#include "../../UserInterface/GameWindow/GUISelectedCharMenu.h"
#include "../../UserInterface/GameWindow/GUIAttackPartWindow.h"
#include "../../UserInterface/GameWindow/Mining/GUIMiningWindow.h"

#include "../../GameObject/Board/General/CPartyBoard.h"

#include "../../GameProgress/CGP_MainGame.h"

#include "../../GameObject/Board/Common/CBoardManager.h"

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
EP_SetCombatMode( CGUIHotKeyBoard::ActionMode pi_eActionMode )
{
	_GetAvatar()->ChangeCombatMode( pi_eActionMode );
	return TRUE;
}

BOOL
EP_SetWalkMode( CGUIHotKeyBoard::MoveMode pi_eWalkMode )
{
	_GetAvatar()->ChangeMoveMode( pi_eWalkMode );
	return TRUE;
}

void
EP_SetAttackMode( CGUIHotKeyBoard::AttackMode pi_eAttackMode )
{
	if( pi_eAttackMode == CGUIHotKeyBoard::Manual )
	{
		_GetAvatar()->EnableAutoAttack( FALSE );
		_GetFuncKeyWindow()->ChangeAttackMode( CGUIHotKeyBoard::Manual );
	}
	else
	{
		_GetAvatar()->EnableAutoAttack( TRUE );
		_GetFuncKeyWindow()->ChangeAttackMode( CGUIHotKeyBoard::Auto );
	}
}

BYTE GetBoardTypeOfItem( DWORD pi_dwIndex );

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Set & Get        ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CAvatar::PutOffEquipment( BYTE pi_byPartType )
{
	if( !CPlayer::PutOffEquipment( pi_byPartType ) )
		return FALSE;

	// put on default equipment
	if( pi_byPartType < IEPT_HELMET )
		return PutOnEquipment( &m_pDefaultItem[pi_byPartType] );

	return TRUE;
}

BOOL
CAvatar::SetDefaultItem( BYTE pi_byPartType, DWORD pi_dwDTIndex )
{
	if( pi_byPartType >= MAX_DEFAULT_PART )
		return FALSE;

	return m_pDefaultItem[pi_byPartType].Create( pi_byPartType, pi_dwDTIndex );
}

BOOL
CAvatar::SetDefaultItem( BYTE pi_byPartType, CEquipItem * pi_pItem )
{
	if( pi_byPartType >= MAX_DEFAULT_PART )
		return FALSE;

	if( !pi_pItem )
		return FALSE;

	memcpy( &m_pDefaultItem[pi_byPartType], pi_pItem, sizeof( CEquipItem ) );
	
	return TRUE;
}

BOOL
CAvatar::UnsetDefaultItem( BYTE pi_byPartType )
{
	if( pi_byPartType >= MAX_DEFAULT_PART )
		return FALSE;

	m_pDefaultItem[pi_byPartType].SetEmpty();

	return TRUE;
}

CEquipItem *
CAvatar::GetDefaultItem( BYTE pi_byPartType )
{
	if( pi_byPartType >= MAX_DEFAULT_PART )
		return FALSE;

	if( m_pDefaultItem[pi_byPartType].IsEmpty() )
		return NULL;

	return &m_pDefaultItem[pi_byPartType];
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CAvatar::RefreshConditionInfo( void )
{
	if( IsEqualUnitMountState( UMS_DISMOUNT ) )
	{
		CCharacter::RefreshConditionInfo();

		if( _GetEquipBoard() )
		{
			const BYTE	SOCKET_NO[4] = { UIPT_RING_LEFT, UIPT_RING_RIGHT, UIPT_AMULET_LEFT, UIPT_AMULET_RIGHT };

			for( int i = 0; i < 4; ++i )
				m_stConditionInfo.ApplyItemEffect( _GetEquipBoard()->GetItem( SOCKET_NO[i] ), TRUE );
		}
	}

	UI_RefreshCharInfoWindow();
}

// pi_bCallForMountUnit - 유닛의 탑승 / 해제시에 호출한 것인가?
void
CAvatar::UI_RefreshCharInfoWindow( BOOL pi_bCallForMountUnit )
{
	if( IsEqualUnitMountState( UMS_MOUNT ) )
	{
		UI_RefreshCharInfoWindow_WhenMountUnit( pi_bCallForMountUnit );
	}
	else
	{
		UI_RefreshCharInfoWindow_WhenDismountUnit( pi_bCallForMountUnit );
	}
}

// pi_bCallForMountUnit - 유닛의 탑승 / 해제시에 호출한 것인가?
void
CAvatar::UI_RefreshCharInfoWindow_WhenMountUnit( BOOL pi_bCallForMountUnit )
{
	if( !_GetCharInfoWindow() )
		return;

	CUnitItem * pUnitItem = m_clsUnitProc.GetUnitItem();
	if( !pUnitItem )
		return;

	_GetCharInfoWindow()->SetGenAttackFc( pUnitItem->GetMinStrikingPower(), pUnitItem->GetMaxStrikingPower() );
	_GetCharInfoWindow()->SetForceAttackFc( 0, 0 );

	_GetCharInfoWindow()->SetDeffenceFc( pUnitItem->GetDefencePower() );
	_GetCharInfoWindow()->SetHitRate( 1.0f );
	_GetCharInfoWindow()->SetAvoidRate( 1.0f );

	for( int i = 0; i < MAX_ITEM_ATTRIBUTE; ++i )
		_GetCharInfoWindow()->SetResistPoint( i, pUnitItem->GetResist( i ) );

	if( pi_bCallForMountUnit )
	{
		m_dwPrevMaxHP	= m_dwMaxHP;
		m_dwPrevHP		= m_dwHP;
		m_dwPrevMaxFP	= m_dwMaxFP;
		m_dwPrevFP		= m_dwFP;
		m_dwPrevMaxSP	= m_dwMaxSP;
		m_dwPrevSP		= m_dwSP;

		SetMaxHP( m_clsUnitProc.GetMaxHP() );
		SetHP( m_clsUnitProc.GetHP() );
		SetMaxFP( 0 );
		SetFP( 0 );
		SetMaxSP( m_clsUnitProc.GetMaxSP() );
		SetSP( m_clsUnitProc.GetSP() );

		// EVT_RACE_SKILL 은 변경하지 않는다.
		_GetCharInfoWindow()->SetBattleMastery( EVT_NEAR, pUnitItem->GetAttackExpertness() );
		_GetCharInfoWindow()->SetBattleMastery( EVT_DEFENCE, pUnitItem->GetDefenceExpertness() );
		_GetCharInfoWindow()->SetBattleMastery( EVT_FAR, 0 );
		_GetCharInfoWindow()->SetBattleMastery( EVT_FORCE, 0 );
		_GetCharInfoWindow()->SetBattleMastery( EVT_SHIELD, 0 );

		for( i = EVT_NEAR; i < EVT_DEFENCE; ++i )
			_GetCharInfoWindow()->SetBattleMasteryExp( i, 0.0f );
	}
}

// pi_bCallForMountUnit - 유닛의 탑승 / 해제시에 호출한 것인가?
void
CAvatar::UI_RefreshCharInfoWindow_WhenDismountUnit( BOOL pi_bCallForMountUnit )
{
	if( !_GetCharInfoWindow() )
		return;

	_GetCharInfoWindow()->SetGenAttackFc( m_stConditionInfo.m_dwTotalStrikingPower[0], m_stConditionInfo.m_dwTotalStrikingPower[1] );
	_GetCharInfoWindow()->SetForceAttackFc( m_stConditionInfo.m_dwTotalForcePower[0], m_stConditionInfo.m_dwTotalForcePower[1] );

	_GetCharInfoWindow()->SetDeffenceFc( m_stConditionInfo.m_dwTotalDefencePower );
	_GetCharInfoWindow()->SetHitRate( m_stConditionInfo.m_fTotalHitProb );
	_GetCharInfoWindow()->SetAvoidRate( m_stConditionInfo.m_fAvoidProb[0] );	// [0]번으로 고정

	for( int i = 0; i < MAX_ITEM_ATTRIBUTE; ++i )
		_GetCharInfoWindow()->SetResistPoint( i, m_stConditionInfo.m_nTotalResist[i] );

	if( pi_bCallForMountUnit )
	{
		SetMaxHP( m_dwPrevMaxHP );
		SetHP( m_dwPrevHP );
		SetMaxFP( m_dwPrevMaxFP );
		SetFP( m_dwPrevFP );
		SetMaxSP( m_dwPrevMaxSP );
		SetSP( m_dwPrevSP );

		for( int i = 0; i < MAX_EXPERTNESS_VALUE_TYPE; ++i )
			SetExpertness( i, m_dwExpertness[i] );
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

QBI_BASE_DATA *
CAvatar::GetCurrentProcessingQBI( int & pio_nStartIndex )
{
	if( !m_pTutorialInfo )
		return NULL;

	QUEST * l_pQuest = m_pTutorialInfo->GetQuest();
	if( !l_pQuest )
		return NULL;

	QBI_BASE_DATA * l_pQBI;
	for( ; pio_nStartIndex <= l_pQuest->m_wCurQBINum; ++pio_nStartIndex )
	{
		l_pQBI = l_pQuest->GetQBI( pio_nStartIndex );
		if( !l_pQBI )
			continue;
		if( !( l_pQBI->m_byFlag & QBIF_BEGIN ) )
			continue;
		if( l_pQBI->m_byFlag & QBIF_FINISH )
			continue;

		++pio_nStartIndex;
		return l_pQBI;
	}

	return NULL;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CAvatar::SetWalkMode( BYTE pi_byWalkMode )
{
	CCharacter::SetWalkMode( pi_byWalkMode );

	// 현재 캐릭터가 움직이는 상태라면 액션 변경
	// 정지 상태에서는 애니메이션의 변경이 없으므로 바꾸지 않는다.
	if( IsEqualAction( CAI_MOVE_MOVING ) )
	{
		// 위치 재요청 - 나중에 프로토콜이 새로 생기면 바꾼다.
		Vector3f vecLastPos;
		GetLastPath( vecLastPos );
		_GetNetworkMgr()->PositionMsg_MoveRequest_clzo( vecLastPos, TRUE );
	}

	// UI 설정
	if( _GetFuncKeyWindow() )
	{
		if( pi_byWalkMode == CMM_MOVE_WALK )
			_GetFuncKeyWindow()->ChangeMoveMode( CGUIHotKeyBoard::Walk );
		else
			_GetFuncKeyWindow()->ChangeMoveMode( CGUIHotKeyBoard::Run );
	}
}

void
CAvatar::SetCombatMode( BYTE pi_byCombatMode )
{
	BYTE byPrevCombatMode = m_byCombatMode;
	CCharacter::SetCombatMode( pi_byCombatMode );

	// UI 설정
	if( _GetFuncKeyWindow() )
	{
		// 전투/평화 아이콘 변경
		if( pi_byCombatMode == CSI_PEACE )
			_GetFuncKeyWindow()->ChangeActionMode( CGUIHotKeyBoard::Peace );
		else
			_GetFuncKeyWindow()->ChangeActionMode( CGUIHotKeyBoard::Combat );

		// Window 변경
		if( _GetMiningWindow() )
		{
			if( m_byCombatMode == CSI_COMBAT )
			{
				CNormalItem * pWeapon = GetNormalItem( IEPT_WEAPON );
				if( pWeapon )
				{
					if( pWeapon->IsEqualWeaponCategoryID( IWCT_MINE ) )
					{
						if( byPrevCombatMode == CSI_PEACE )
						{
							_GetMiningWindow()->Mining_Off();
							_GetMiningWindow()->Open();
						}
					}
				}
			}
			else
			{
				_GetMiningWindow()->Close();
			}
		}
	}

	// 채굴 취소
	if( ( pi_byCombatMode == CSI_PEACE ) && ( m_dwActionID == CAI_ITEM_USE_MINING ) )
	{
		_GetMiningBoard()->Mining_Cancel();		
	}
}

void
CAvatar::SetExp( float pi_fExp )
{
	m_fExp = pi_fExp;

	if( _GetGaugeWindow() )
	{
		_GetGaugeWindow()->SetTotalExp( 100 );
		_GetGaugeWindow()->SetCurrentExp( pi_fExp );
	}

	if( _GetCharInfoWindow() )
	{
		if( pi_fExp == 0 )
			_GetCharInfoWindow()->SetExp( 0.0f );
		else
			_GetCharInfoWindow()->SetExp( pi_fExp );
	}
}

void
CAvatar::SetExpByCount( double pi_dbExp )
{
	double l_dbLevelUpExp = _GetCharDataMgr()->GetNeedExpForLevelup( m_byRace, m_dwLevel );
	if( pi_dbExp == 0 || l_dbLevelUpExp == 0 )
		m_fExp = 0.0f;
	else
		m_fExp = ( pi_dbExp / l_dbLevelUpExp ) * 100.0f;

	if( _GetGaugeWindow() )
	{
		_GetGaugeWindow()->SetTotalExp( l_dbLevelUpExp );
		_GetGaugeWindow()->SetCurrentExp( pi_dbExp );
	}

	if( _GetCharInfoWindow() )
	{
		_GetCharInfoWindow()->SetExp( m_fExp );
	}
}

void
CAvatar::SetLevel( DWORD pi_dwLevel )
{
	CPlayer::SetLevel( pi_dwLevel );

	if( _GetGaugeWindow() )
	{
		_GetGaugeWindow()->SetLevel( pi_dwLevel );		
	}

	//SetMaxHP();
	//SetMaxFP();
	//SetMaxSP();
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CAvatar::SetMaxHP( DWORD pi_dwMaxHP )
{
	CPlayer::SetMaxHP( pi_dwMaxHP );

	if( _GetGaugeWindow() )
		_GetGaugeWindow()->SetGauge( CGUIGaugeExpWindow::LIFE_BAR, GetMaxHP(), GetHP() );
}

void
CAvatar::SetHP( DWORD pi_dwHP )
{
	CPlayer::SetHP( pi_dwHP );

	if( _GetGaugeWindow() )
		_GetGaugeWindow()->SetGauge( CGUIGaugeExpWindow::LIFE_BAR, GetMaxHP(), GetHP() );
}

void
CAvatar::SetMaxFP( DWORD pi_dwMaxFP )
{
	CPlayer::SetMaxFP( pi_dwMaxFP );

	if( _GetGaugeWindow() )
		_GetGaugeWindow()->SetGauge( CGUIGaugeExpWindow::FORCE_BAR, GetMaxFP(), GetFP() );
}

void
CAvatar::SetFP( DWORD pi_dwFP )
{
	CPlayer::SetFP( pi_dwFP );

	if( _GetGaugeWindow() )
		_GetGaugeWindow()->SetGauge( CGUIGaugeExpWindow::FORCE_BAR, GetMaxFP(), GetFP() );
}

void
CAvatar::SetMaxSP( DWORD pi_dwMaxSP )
{
	CPlayer::SetMaxSP( pi_dwMaxSP );

	if( _GetGaugeWindow() )
		_GetGaugeWindow()->SetGauge( CGUIGaugeExpWindow::STAMINA_BAR, GetMaxSP(), GetSP() );
}

void
CAvatar::SetSP( DWORD pi_dwSP )
{
	CPlayer::SetSP( pi_dwSP );

	if( _GetGaugeWindow() )
		_GetGaugeWindow()->SetGauge( CGUIGaugeExpWindow::STAMINA_BAR, GetMaxSP(), GetSP() );
}

//==============================================================================

void
CAvatar::SetPartyMemberType( BYTE pi_byType )
{
	CPlayer::SetPartyMemberType( pi_byType );
}

//==============================================================================

void
CAvatar::SetGold( DWORD pi_dwGold )
{
	m_dwGold = pi_dwGold;
	if( _GetInventoryWindow() )
		_GetInventoryWindow()->SetOwnGold( m_dwGold );
}

void
CAvatar::SetDalant( DWORD pi_dwDalant )
{
	m_dwDalant = pi_dwDalant;
	if( _GetInventoryWindow() )
		_GetInventoryWindow()->SetOwnDalant( m_dwDalant );
}

void
CAvatar::SetOrgTexRate( float pi_fTexRate )
{
	m_fTexRate = pi_fTexRate;

	_GetExchangeWindow()->SetTaxRate( m_fTexRate * 100.0f );
}

void
CAvatar::SetExchangeRate( DWORD pi_dwExchangeRate )
{
	m_dwExchangeRate = pi_dwExchangeRate;

	_GetExchangeWindow()->SetExchangeRate( m_dwExchangeRate );
}

//==============================================================================

void
CAvatar::SetClassDTIndex( BYTE pi_byOrderNo, DWORD pi_dwDTIndex )
{
	if( pi_byOrderNo >= MAX_AVATAR_CLASS_NUM )
		return;

	m_dwClassDTIndex[pi_byOrderNo] = pi_dwDTIndex;

	// 클래스 스킬 설정
	if( _GetSkillBoard() )
		_GetSkillBoard()->CreateClassSkill( m_dwClassDTIndex[pi_byOrderNo] );

	// 0 번은 현재 아바타의 클래스이므로 클래스명을 세팅한다.
	if( pi_byOrderNo == 0 )
	{
		CPlayerClass * pClassData = _GetCharDataMgr()->GetClassDataByDTIndex( pi_dwDTIndex );
		if( pClassData )
			SetClassName( pClassData->GetClassName() );
	}
}

void
CAvatar::SetClassDTIndexByStrDTCode( BYTE pi_byOrderNo, char * pi_strDTCode )
{
	if( ( pi_byOrderNo >= MAX_AVATAR_CLASS_NUM ) || !pi_strDTCode )
		return;

	CPlayerClass * pClassData = _GetCharDataMgr()->GetClassData( pi_strDTCode );
	if( !pClassData )
		return;

	m_dwClassDTIndex[pi_byOrderNo] = pClassData->GetDTIndex();

	// 클래스 스킬 설정
	if( _GetSkillBoard() )
		_GetSkillBoard()->CreateClassSkill( m_dwClassDTIndex[pi_byOrderNo] );

	// 0 번은 현재 아바타의 클래스이므로 클래스명을 세팅한다.
	if( pi_byOrderNo == 0 )
		SetClassName( pClassData->GetClassName() );
}

DWORD
CAvatar::GetClassDTIndex( BYTE pi_byOrderNo )
{
	if( pi_byOrderNo >= MAX_AVATAR_CLASS_NUM )
		return ID_INVALID;

	return m_dwClassDTIndex[pi_byOrderNo];
}

BYTE
CAvatar::GetClassLevel( void )
{
	for( int i = 1, nLevel = 0; i < MAX_AVATAR_CLASS_NUM; ++i )
	{
		if( m_dwClassDTIndex[i] != ID_INVALID )
			++nLevel;
	}

	return nLevel;
}

void
CAvatar::SetClassName( char * pi_pClassName )
{
	if( !pi_pClassName )
		return;

	_strcpy( m_pClassName, pi_pClassName, MAX_NAME_LENGTH );

	if( _GetCharInfoWindow() )
		_GetCharInfoWindow()->SetAttribute( m_pClassName );
}

// 전직이 가능한 상태인가?
BOOL
CAvatar::IsChangableClass( void )
{
	for( int i = 1; i < MAX_AVATAR_CLASS_NUM; ++i )
	{
		if( m_dwClassDTIndex[i] == ID_INVALID )
			return TRUE;
	}

	return FALSE;
}

// 전직
void
CAvatar::ChangeClass( DWORD pi_dwNewClassDTIndex )
{
	for( int i = 1; i < MAX_AVATAR_CLASS_NUM; ++i )
	{
		if( m_dwClassDTIndex[i] == ID_INVALID )
		{
			m_dwClassDTIndex[i] = m_dwClassDTIndex[0];
			break;
		}
	}

	SetClassDTIndex( 0, pi_dwNewClassDTIndex );

	SetEffectInfo( CAI_GESTURE_CLASS_CHANGE );
}

// 전직과 관련된 정보를 재설정한다.
void
CAvatar::RefreshClassInfo( void )
{
	// 이전 클래스들의 정보는 CNetworkMgr::InitMsg_BaseDownloadResult_zocl() 에서
	// 세팅되므로 여기서는 현재 클래스의 정보만 설정해 준다.
	SetClassDTIndex( 0, m_dwClassDTIndex[0] );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CAvatar::SetExpertness( BYTE pi_byType, DWORD pi_dwCount )
{
	if( pi_byType >= MAX_EXPERTNESS_VALUE_TYPE )
		return;

	DWORD l_dwExp = 0;

	if( pi_byType == EVT_FORCE )
	{
		l_dwExp = _GetForceBoard()->CalculateForceExpertness();
		return;		// 아무런 연산없이 그냥 넘어간다.
	}

	m_dwExpertness[pi_byType] = pi_dwCount;

	l_dwExp = CalculateExpertness( pi_byType, pi_dwCount );

	//--------------------------------------------------------------------------

	if( _GetCharInfoWindow() )
	{
		double	l_dbSC_AfterLevelUp	= 0;	// 레벨업 이후의 누적 횟수
		double	l_dbNeedSC_ForLevelUp;		// 다음 레벨 까지 필요한 성공 횟수

		if( pi_byType < EVT_MAKE_WEAPON )
		{
			_GetCharInfoWindow()->SetBattleMastery( pi_byType, l_dwExp );

			if( l_dwExp < 2 )
			{
				l_dbSC_AfterLevelUp = pi_dwCount;

				double l_dbTempD = pow( 1000, 2 ) + ( 4.0f * pow( ( l_dwExp + 1 ), 2 ) * 1000.0f );
				double l_dbTempE = ( -1000.0f + sqrt( l_dbTempD ) ) / 2.0f;
				double l_dbTempF = pow( l_dbTempE, 2 );
				l_dbNeedSC_ForLevelUp = ( DWORD )l_dbTempF;
			}
			else
			{
				double l_dbTempA = pow( 1000, 2 ) + ( 4.0f * pow( l_dwExp, 2 ) * 1000.0f );
				double l_dbTempB = ( -1000.0f + sqrt( l_dbTempA ) ) / 2.0f;
				double l_dbTempC = pow( l_dbTempB, 2 );
				l_dbSC_AfterLevelUp = pi_dwCount - l_dbTempC;

				double l_dbTempD = pow( 1000, 2 ) + ( 4.0f * pow( ( l_dwExp + 1 ), 2 ) * 1000.0f );
				double l_dbTempE = ( -1000.0f + sqrt( l_dbTempD ) ) / 2.0f;
				double l_dbTempF = pow( l_dbTempE, 2 );
				l_dbNeedSC_ForLevelUp = ( DWORD )( l_dbTempF - l_dbTempC );
			}

			if( l_dwExp >= 99 )
				_GetCharInfoWindow()->SetBattleMasteryExp( pi_byType, 1.0f );
			else
				_GetCharInfoWindow()->SetBattleMasteryExp( pi_byType, ( (float)l_dbSC_AfterLevelUp / (float)l_dbNeedSC_ForLevelUp ) );
		}
		else
		{
			_GetCharInfoWindow()->SetMakeMastery( pi_byType - EVT_MAKE_WEAPON, l_dwExp );

			if( pi_byType != EVT_MAKE_BULLET )
			{
				if( l_dwExp < 2 )
				{
					l_dbSC_AfterLevelUp = pi_dwCount;
				}
				else
				{
					double l_dbTempA = ( ( pow( l_dwExp, 2 ) - 1.0f ) / 3.0f * 1.1f ) + 0.9f;
					l_dbSC_AfterLevelUp = pi_dwCount - ( DWORD )l_dbTempA;
				}

				double l_dbTempA = pow( ( l_dwExp + 1 ), 2 ) - 1;
				double l_dbTempB = ( l_dbTempA / 3.0f * 1.1f ) + 0.9f;
				double l_dbTempC = ( ( pow( l_dwExp, 2 ) - 1 ) / 3.0f * 1.1f ) + 0.9f;
				l_dbNeedSC_ForLevelUp = ( DWORD )( l_dbTempB - l_dbTempC );
			}
			else
			{
				if( l_dwExp < 2 )
				{
					l_dbSC_AfterLevelUp = pi_dwCount;
				}
				else
				{
					double l_dbTempA = ( ( pow( l_dwExp, 2.0f ) - 1.0f ) / 3.0f ) * 10.0f + 0.9f;
					l_dbSC_AfterLevelUp = pi_dwCount - ( DWORD )l_dbTempA;
				}

				double l_dbTempA = ( ( pow( ( l_dwExp + 1 ), 2 ) - 1 ) / 3.0f * 10.0f ) + 0.9f;
				double l_dbTempB = ( ( pow( l_dwExp, 2 ) - 1 ) / 3.0f * 10.0f ) + 0.9f;
				l_dbNeedSC_ForLevelUp = ( DWORD )( l_dbTempA - l_dbTempB );
			}

			if( l_dwExp >= 99 )
				_GetCharInfoWindow()->SetMakeMasteryExp( pi_byType - EVT_MAKE_WEAPON, 1.0f );
			else
				_GetCharInfoWindow()->SetMakeMasteryExp( pi_byType - EVT_MAKE_WEAPON, ( (float)l_dbSC_AfterLevelUp / (float)l_dbNeedSC_ForLevelUp ) );
		}
	}
}

BYTE
CAvatar::GetExpertness( BYTE pi_byType )
{
	return CalculateExpertness( pi_byType, m_dwExpertness[pi_byType] );
}

BYTE
CAvatar::CalculateExpertness( BYTE pi_byType, DWORD pi_dwCount )
{
	if( pi_byType >= MAX_EXPERTNESS_VALUE_TYPE )
		return 0;

	DWORD l_dwExpertness = 0;

	if( pi_byType == EVT_FORCE )
	{
		l_dwExpertness = _GetForceBoard()->CalculateForceExpertness();
	}
	else if( pi_byType < EVT_MAKE_WEAPON )
	{
		l_dwExpertness = ( DWORD )( sqrt( ( ( pi_dwCount + 1 ) / 1000.0f ) + sqrt( pi_dwCount + 1 ) ) );
	}
	else
	{
		if( pi_byType != EVT_MAKE_BULLET )
			l_dwExpertness = ( DWORD )( sqrt( ( pi_dwCount / 1.1f ) * 3 + 1 ) );
		else
			l_dwExpertness = ( DWORD )( sqrt( ( pi_dwCount / 10.f ) * 3 + 1 ) );
	}

	if( l_dwExpertness < 1 )	l_dwExpertness = 1;
	if( l_dwExpertness > 99 )	l_dwExpertness = 99;

	return l_dwExpertness;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 종족체크 & 레벨제한체크 & etc 체크
static const BYTE	RACE[5][5] =
{
	{ CRT_ALL, CRT_BELLATOR_CORA, CRT_MALE,     CRT_BELLATOR, CRT_BELLATOR_MALE },		// 고대 남자
	{ CRT_ALL, CRT_BELLATOR_CORA, CRT_FEMALE,   CRT_BELLATOR, CRT_BELLATOR_FEMALE },	// 고대 여자
	{ CRT_ALL, CRT_BELLATOR_CORA, CRT_MALE,     CRT_CORA,     CRT_CORA_MALE },			// 이계 남자
	{ CRT_ALL, CRT_BELLATOR_CORA, CRT_FEMALE,   CRT_CORA,     CRT_CORA_FEMALE },		// 이계 여자
	{ CRT_ALL, CRT_ACCRETIA,	  CRT_ACCRETIA, CRT_ACCRETIA, CRT_ACCRETIA }			// 과학
};

BOOL
CAvatar::IsUsableItem( CBoardObject * pi_pBoardObject, BOOL pi_bIsRenderErrMsg )
{
	if( !pi_pBoardObject )
		return FALSE;

	if( pi_pBoardObject->IsEqualItemType( BOT_ANIMUS ) )
	{
		return IsUsableAnimus( static_cast< CAnimusItem * >( pi_pBoardObject ) );
	}
	else if( pi_pBoardObject->IsEqualItemType( BOT_SF ) )
	{
		return IsUsableSF( static_cast< CSFItem * >( pi_pBoardObject ), pi_bIsRenderErrMsg );
	}
	else if( pi_pBoardObject->IsEqualItemType( BOT_BOARD_ITEM ) )
	{
		if( !IsUsableItem_CheckRace( static_cast< CBoardItem * >( pi_pBoardObject ) ) )
		{
			if( pi_bIsRenderErrMsg )
				_SetSystemMsg( "아바타가 사용할 수 없는 아이템입니다. 다른 종족의 아이템입니다." );
			return FALSE;
		}

		if( !IsUsableItem_CheckLevel( static_cast< CBoardItem * >( pi_pBoardObject ) ) )
		{
			if( pi_bIsRenderErrMsg )
				_SetSystemMsg( "아바타가 사용할 수 없는 아이템입니다. 사용 가능 레벨이 아바타의 레벨보다 높습니다." );
			return FALSE;
		}

		if( !IsUsableItem_CheckExpertness( static_cast< CBoardItem * >( pi_pBoardObject ) ) )
		{
			if( pi_bIsRenderErrMsg )
				_SetSystemMsg( "아바타가 사용할 수 없는 아이템입니다. 사용 가능 숙련도가 아바타의 숙련도보다 높습니다." );
			return FALSE;
		}

		if( !IsUsableItem_CheckPotion( static_cast< CBoardItem * >( pi_pBoardObject ) ) )
		{
			if( pi_bIsRenderErrMsg )
				_SetSystemMsg( "아바타가 현재 회복 불가능 상태입니다. 포션을 사용할 수 없습니다." );
			return FALSE;
		}
	}

	return TRUE;
}

BOOL
CAvatar::IsUsableItem_CheckRace( CBoardItem * pi_pItem )
{
	BYTE byRace			= ( pi_pItem->GetMeshID() & 0x0FF00000 ) >> 20;
	BOOL bIsUsableRace	= FALSE;

	for( int i = 0; i < 5; ++i )
	{
		if( RACE[m_byRace][i] == byRace )
			return TRUE;
	}

	return FALSE;
}

BOOL
CAvatar::IsUsableItem_CheckLevel( CBoardItem * pi_pItem )
{
	if( ( m_dwLevel + m_nItemEquipLevel ) < pi_pItem->GetUsableLevel() )
		return FALSE;

	return TRUE;
}

BOOL
CAvatar::IsUsableItem_CheckExpertness( CBoardItem * pi_pItem )
{
	if ( ( pi_pItem->GetItemKind() < CEPT_UPPER_PART ) || ( pi_pItem->GetItemKind() > CEPT_CLOAK ) )
		return TRUE;

	BYTE byItemExpertness	= 0;
	BYTE byAvatarExpertness	= 0;

	for( int i = 0; i < EQUIPMENT_DATA::MAX_UET_NUM; ++i )
	{
		if( pi_pItem->GetUsableExpertnessType( i ) >= MAX_EXPERTNESS_VALUE_TYPE )
			continue;

		byItemExpertness	= pi_pItem->GetUsableExpertness( i );
		if( byItemExpertness == 0xFF )	// 0xFF면 체크하지 않음
			continue;
		byAvatarExpertness	= GetExpertness( pi_pItem->GetUsableExpertnessType( i ) );

		if( byAvatarExpertness < byItemExpertness )
			return FALSE;
	}

	return TRUE;
}


BOOL
CAvatar::IsUsableItem_CheckPotion( CBoardItem * pi_pItem )
{
	if( !pi_pItem->IsEqualItemKind( IEPT_POTION ) )
		return TRUE;

	return ( !m_stConditionInfo.m_bDisable_Recover );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CAvatar::IsUsableSF( CSFItem * pi_pSFItem, BOOL pi_bIsRenderErrMsg )
{
	BOOL bIsUsable = FALSE;

	// 클래스에 따른 사용 가능 체크 - 저급은 모든 클래스가 사용 가능.
	if( pi_pSFItem->GetStep() != 0 )
	{
		// 클래스의 종류에 따른 사용 가능 체크
		CPlayerClass * pClass = NULL;
		for( int i = 0; i < MAX_AVATAR_CLASS_NUM; ++i )
		{
			pClass = _GetCharDataMgr()->GetClassDataByDTIndex( m_dwClassDTIndex[i] );
			if( !pClass )
				continue;

			if( pi_pSFItem->IsUsableClassType( pClass->GetType() ) )
			{
				bIsUsable = TRUE;
				break;
			}
		}

		// 클래스의 레벨에 따른 사용 가능 체크
		if( bIsUsable && ( pi_pSFItem->GetStep() > GetClassLevel() ) )
			bIsUsable = FALSE;

		// message
		if( !bIsUsable )
		{
			if( pi_bIsRenderErrMsg )
				_SetSystemMsg( "현재 선택한 스킬/포스를 사용하기 위해서는 사용 가능 클래스로 전직해야 합니다." );
			return FALSE;
		}
	}

	// 마스터리에 따른 사용 가능 체크
	if( pi_pSFItem->IsEqualSFType( CAT_FORCE ) )
		bIsUsable = _GetForceBoard()->IsUsableSF( pi_pSFItem );
	else
		bIsUsable = _GetSkillBoard()->IsUsableSF( pi_pSFItem );

	// message
	if( !bIsUsable )
	{
		if( pi_bIsRenderErrMsg )
			_SetSystemMsg( "현재 선택한 스킬/포스를 사용하기 위한 마스터리가 부족합니다." );
		return FALSE;
	}

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CAvatar::IsUsableAnimus( CAnimusItem * pi_pAnimusItem )
{
	// check expertness
	BOOL	bIsSummonable		= FALSE;
	BYTE	byAnimusItemLevel	= pi_pAnimusItem->GetLevel();
	BYTE	bySummonExpertness	= GetExpertness( EVT_RACE_SKILL );

	// AMI => 소환 스킬이 99(GM)인 상태에서만 소환할 수 있다.
	if( byAnimusItemLevel > MAX_ANIMUS_LEVEL )
	{
		if( bySummonExpertness >= 99 )
			bIsSummonable = TRUE;
	}
	// AMI 미만
	else
	{
		int nSummonableAnimusLevel = sqrt( bySummonExpertness * 20 ) + 12;
		if( nSummonableAnimusLevel >= byAnimusItemLevel )
			bIsSummonable = TRUE;
	}

	if( !bIsSummonable )
		_SetSystemMsg( "아바타의 소환 숙련도가 낮기때문에, 애니머스 리버를 사용할 수 없습니다." );

	return bIsSummonable;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 주어진 아이템을 아바타가 장착할 수 있는가?
BOOL
CAvatar::IsEquipable( CBaseItem * pi_pBaseItem )
{
	if( !pi_pBaseItem )
		return FALSE;

	CNormalItem * pEquipWeapon = GetNormalItem( IEPT_WEAPON );
	CNormalItem * pEquipShield = GetNormalItem( IEPT_SHIELD );

	// 만약 장착 하려는 아이템이 양손/쌍손 무기일 경우에 방패를 장착중이라면 장착 실패
	if( pi_pBaseItem->IsEqualItemKind( IEPT_WEAPON ) )
	{
		if( pi_pBaseItem->IsEqualHoldType( IHT_BOTH_HAND ) || pi_pBaseItem->IsEqualHoldType( IHT_DUAL_HAND ) )
		{
			if( pEquipShield )
			{
				_SetSystemMsg( "현재 방패를 장착중이므로 양손/쌍손 무기는 장착할 수 없습니다." );
				return FALSE;
			}
		}
	}
	// 만약 장착하려는 아이템이 방패일 경우에 아바타가 양손/쌍손 무기를 장착중이라면 장착 실패
	else if( pi_pBaseItem->IsEqualItemKind( IEPT_SHIELD ) )
	{
		if( pEquipWeapon )
		{
			if( pEquipWeapon->IsEqualHoldType( IHT_BOTH_HAND ) || pEquipWeapon->IsEqualHoldType( IHT_DUAL_HAND ) )
			{
				_SetSystemMsg( "현재 양손/쌍손 무기를 장착중이므로 방패는 장착할 수 없습니다." );
				return FALSE;
			}
		}
	}

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 주어진 캐릭터가 디텍트가 가능한가?
BOOL
CAvatar::IsEnableDetect( CCharacter * pi_pCharacter )
{
	if( !pi_pCharacter )
		return FALSE;

	if( pi_pCharacter->IsEqualCharType( CTI_AVATAR ) )
		return TRUE;

	// 아바타가 옵져버 혹은 운영자라면 모든 캐릭터 디텍트 가능
	if( IsEnable( ROSF_OBSERVER ) || ( _GetNetworkMgr()->GetAvatarGrade() >= USER_DEGREE_MGR ) )
		return TRUE;

	// 캐릭터가 옵져버라면 디텍트 불가능
	if( pi_pCharacter->IsEnable( ROSF_OBSERVER ) )
		return FALSE;

	// 캐릭터가 아바타의 파티원일 경우
	if( _GetPartyBoard()->IsPartyMember( pi_pCharacter->GetIndex() )	)
	{
		return TRUE;
	}
	// 아바타가 디텍트 가능 상태일 경우 ( 유닛 탑승시에는 디텍트 불가능 )
	else if( m_stConditionInfo.m_bEnable_Detect && !IsEqualUnitMountState( UMS_MOUNT ) )
	{
		static const float MAX_DETECT_DISTANCE = 230.0f;
		if( GetDist( m_vecPos, pi_pCharacter->GetPosition() ) <= MAX_DETECT_DISTANCE )
			return TRUE;
	}

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////
/*
void
CAvatar::IncreaseSFSuccessCount( void )
{
	m_stSFInfo.IncreaseSuccessCount();
	m_stSFInfo.CalculateSuccessCount();

	// 스킬/포스창 업데이트
	if( m_stSFInfo.GetType() == CAT_SKILL )
	{
		_GetSkillBoard()->SetSuccessCount( &m_stSFInfo );
		_GetSkillBoard()->SetSuccessCountOfMastery( m_stSFInfo.GetData()->byMasteryIndex,
													m_stSFInfo.GetData()->byStep,
													1, TRUE, TRUE );
	}
	else
	{
		_GetForceBoard()->SetSuccessCount( &m_stSFInfo );
		_GetForceBoard()->SetSuccessCountOfMastery( m_stSFInfo.GetData()->byMasteryIndex,
													m_stSFInfo.GetData()->byStep,
													1, TRUE, TRUE );
	}

	// 스킬/포스 벨트창 업데이트
	SF_INFO * l_pBeltSFInfo = _GetSFBeltBoard()->GetSFByID( m_stSFInfo.GetID() );
	if( l_pBeltSFInfo )
	{
		l_pBeltSFInfo->SetSuccessCount( m_stSFInfo.GetSuccessCount() );
		l_pBeltSFInfo->SetLevel( m_stSFInfo.GetLevel() );
		l_pBeltSFInfo->SetExpByPercent( m_stSFInfo.GetExpByPercent() );
	}
}*/

// 스킬/포스의 성공 횟수 저장 및 계산
// pi_dwIndex => 스킬일 경우에는 DT Index, 포스일 경우에는 아이템의 Index( == serial )
void
CAvatar::SetSFSuccessCount( BYTE pi_bySFType, DWORD pi_dwIndex, DWORD pi_dwCount )
{
	CSFItem * pSFItem = NULL;

	if( pi_bySFType == CAT_SKILL )
		pSFItem = _GetSkillBoard()->GetSFByIndex( pi_dwIndex );
	else
		pSFItem = _GetForceBoard()->GetSFByIndex( pi_dwIndex );

	if( !pSFItem || !pSFItem->GetData() )
		return;

	DWORD dwIncreaseCount = pi_dwCount - pSFItem->GetSuccessCount();

	// update success count
	pSFItem->CalculateSuccessCount( pi_dwCount );

	if( pi_bySFType == CAT_SKILL )
	{
		_GetSkillBoard()->SetSuccessCountOfMastery( pSFItem->GetData()->byMasteryIndex,
													pSFItem->GetData()->byStep,
													dwIncreaseCount, TRUE );
	}
	else
	{
		_GetForceBoard()->CalculateForceExpertness();
	}

	//_WriteLog( "1) sf sc => %s : %d", l_stNewSFInfo.GetData()->pName_Kor, pi_dwCount );
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 제이드 이펙트 타입
enum JADE_EFFECT_TYPE
{
	INC_GAMBLE_SUCC_PROB, INC_PROFIT_TRADE, INC_GAIN_EXP, ENABLE_SEND_MSG_TO_RACE, INC_ITEM_EQUIP_LEVEL
};




void
CAvatar::ApplyItemEffect_Jade( CBoardItem * pi_pBoardItem, BOOL pi_bIsAdd )
{
	if( !pi_pBoardItem )
		return;
	if( !pi_pBoardItem->IsEqualItemKind( IEPT_INGOT ) )
		return;

	INGOT_DATA * pIngotData = ( INGOT_DATA * )pi_pBoardItem->GetData();
	if( !pIngotData )
		return;

	int i;
	switch( pIngotData->GetEffectType() )
	{
		case 0 : //	#define 0 도박장에서 도박할 때 플레이어에게 주어지는 성공확률에 값필드의 배수만큼의 확률을 높여주도록 함)                            퍼센트
			break;

		case 1 : // #define 1 상인에게 물품 구입과 판매에만 값필드의 배수만큼의 해당하는 양의 이득을 보도록 함(구입할 때에는 저렴,팔때는 더 비싸게)      퍼센트
			for( i = 0; i < pi_pBoardItem->GetQuantity(); ++i )
			{
				if( pi_bIsAdd )
					m_fProfitOfTrade += pIngotData->fEffectValue;
				else
					m_fProfitOfTrade -= pIngotData->fEffectValue;
			}

			if( m_fProfitOfTrade > pIngotData->fEffectMaxValue )
				m_fProfitOfTrade = pIngotData->fEffectMaxValue;
			if( m_fProfitOfTrade < 0 )
				m_fProfitOfTrade = 0;
			break;

		case 2 : // #define 2 때릴 때나 죽였을 때 획득하는 경험치 모두에 값필드의 배수만큼에 해당하는 양을 추가 획득하도록 함                            퍼센트
			break;

		case 3 : // #define 3 자신과 같은 종족 전체 플레이어들에게 채팅내용 전달할 수 있음 [/종족 내용]                                                  없음
			for( i = 0; i < pi_pBoardItem->GetQuantity(); ++i )
			{
				if( pi_bIsAdd )
					++m_bEnable_SendMsgToRace;
				else
					--m_bEnable_SendMsgToRace;
			}
			break;

		case 4 : // #define 4 아이템을 착용할 때 걸리는 레벨제한을 값필드의 값만큼에 해당되는 상위아이템을 착용할 수 있음.                               값
				 // 아이템 착용에만 영향을 미칠 뿐 다른 곳에는 영향을 못미친다.
			for( i = 0; i < pi_pBoardItem->GetQuantity(); ++i )
			{
				if( pi_bIsAdd )
					m_nItemEquipLevel += pIngotData->fEffectValue;
				else
					m_nItemEquipLevel -= pIngotData->fEffectValue;
			}

			if( m_nItemEquipLevel > pIngotData->fEffectMaxValue )
				m_nItemEquipLevel = pIngotData->fEffectMaxValue;
			if( m_nItemEquipLevel < 0 )
				m_nItemEquipLevel = 0;
			break;
	}
}

void
CAvatar::UseItem( CBoardItem * pi_pBoardItem )
{
	if( !pi_pBoardItem )
		return;

	if( pi_pBoardItem->IsEqualItemKind( IEPT_POTION ) )
	{
		static const DWORD ADRENALIN_DT_INDEX = 60;

		if( ( pi_pBoardItem->GetDTIndex() >= ADRENALIN_DT_INDEX ) &&
			( pi_pBoardItem->GetDTIndex() < ADRENALIN_DT_INDEX + 3 ) )
		{
			POTION_DATA * pData = static_cast< POTION_DATA * >( pi_pBoardItem->GetData() );
			if( pData )
			{
				if( m_dwPotionEffectEndTime == 0 )
					m_stConditionInfo.ApplyPotionEffect( pData, TRUE );

				m_dwPotionEffectEndTime = timeGetTime() + ( pData->byContinuanceTime * 1000 );
				m_pUsedPotionData		= pData;

				_SetSystemMsg( "%s의 효과가 적용 되었습니다.", pData->pName );
			}
		}
	}

	SetEffectInfo( CAI_ITEM_USE_ITEM );
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

BOOL
CAvatar::IsPassedLimitTimeFromReceiveNetworkMsg( void )
{
	return FALSE;
}

BOOL
CAvatar::IsPassedLimitTimeForDelete( void )
{
	return FALSE;
}

BOOL
CAvatar::IsMovingMap( void )
{
	return ( m_pMoveMapInfo->m_byMoveStep != MOVEMENT_BETWEEN_MAP_INFO::MMS_NONE );
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

char SKILL_EFFECT_TYPE[28][128] =
{
	"HP 감소",
	"공격력 증가 (Melee)",
	"명중률 증가 (Melee)",
	"사정 거리 증가 (Melee)",
	"스킬의 지속 시간을 늘려줌",
	"반격기 사용",
	"공격시 방패 무시",
	"크리티컬 공격 확률 증가",
	"상대방을 밀어냄",
	"상대방을 기절시킴",
	"크리티컬 공격 & 기절",
	"크리티컬 공격 & 밀어내기",
	"공격시 모든 방어력 무시",
	"공격 딜레이 감소 (Bow)",
	"사정 거리 증가 (Bow)",
	"명중률 증가 (Bow)",
	"공격 딜레이 감소 (Fire Arm)",
	"사정 거리 증가 (Fire Arm)",
	"명중률 증가 (Fire Arm)",
	"공격 딜레이 감소 (Throw)",
	"사정 거리 증가 (Throw)",
	"명중률 증가 (Throw)",
	"공격 딜레이 감소 (Launcher)",
	"사정 거리 증가 (Launcher)",
	"무기 방패 제작",
	"방어구 (투구,상의,하의,장갑,신발) 제작",
	"발사물 제작",
	"소환"
};

char FORCE_EFFECT_TYPE[47][128] =
{
	"HP 감소",
	"포스 사정거리 증가",
	"자신의 현재 HP를 대상의 FP로 만듬",
	"공격 포스의 시전 딜레이 감소",
	"포스 공격력_증가",
	"공격 속도 증가 (Melee)",
	"이동 속도 증가",
	"포스 성공 확률 증가",
	"지속 피해 보조 포스의 남은 시간 늘려주기",
	"모든 스킬/포스의 사용시 FP소모량 감소",
	"가격하는 데미지의 일정량을 FP로 흡수",
	"스텔스",
	"부활",
	"현재 HP 증가",
	"현재 SP 증가",
	"HP & FP 회복 속도 증가",
	"저항력 증가",
	"방어력 증가",
	"일반 공격 회피율 증가",
	"지속 이득 보조 포스의 남은 시간 늘려주기",
	"가격하는 데미지의 일정량을 HP로 흡수",
	"절대 방어",
	"오버 힐링",
	"시야 감소",
	"저항력 감소",
	"방어력 감소",
	"일반 & 포스의 공격력 감소",
	"일반 공격의 명중률 감소",
	"일반 공격의 속도 감소",
	"HP & FP 회복 속도 감소",
	"최대 공격력 선택확률 감소",
	"HP / FP / SP 회복 제한",
	"뛰기 제한 (걷기만 가능함)",
	"지속 이득 보조 스킬의 사용 제한",
	"이동 금지 (걷기/뛰기 포함)",
	"지속 피해 보조 포스의 사용 제한",
	"지속 이득 보조 포스의 사용 제한",
	"모든 공격 스킬의 사용 제한",
	"모든 공격 포스의 사용 제한",
	"가장 최근에 적용된 지속 이득 보조 스킬의 해제",
	"가장 최근에 적용된 지속 이득 보조 포스의 해제",
	"가장 최근에 적용된 지속 피해 보조 포스의 해제",
	"투명화 해제",
	"걸려있는 모든 지속 이득 보조 스킬의 해제",
	"걸려있는 모든 지속 이득 보조 포스의 해제",
	"걸려있는 모든 지속 피해 보조 포스의 해제",
	"시전자 주위 일정범위내에 있는 모든 적들의 지속 이득 보조 포스 & 스킬 해제"
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

#ifdef _DEVELOPER_

void
CAvatar::RenderSFUsingMsg( void )
{
	if( !m_pSFUsingMsg[0] )
		return;

	DrawR3Hangul( 10, 100, m_pSFUsingMsg, 0xFFA0FFA0, R3_HAN_OUTLINE );

	if( timeGetTime() - m_dwReciveSFUsingMsgTime >= 5000 )
		ZeroMemory( m_pSFUsingMsg, 256 );
}

void
CAvatar::RenderAppliedSFMsg( void )
{
	static char pProfitTypeName[2][8] = { "득", "실" };
	static char pBuf[256];

	int		cnt = 0;
	DWORD	dwCurTime = timeGetTime();

	CSFItem * pSFItem = NULL;
	SF_DATA * pSFData = NULL;

	for( int i = 0; i < 2; ++i )
	{
		for( int j = 0; j < m_stAppliedSF.m_bySFNum[i]; ++j )
		{
			if( m_stAppliedSF.m_listSF[i][j].IsEmpty() )
				continue;

			pSFItem = &m_stAppliedSF.m_listSF[i][j];

			sprintf( pBuf, "( %s ) %d. %s %d레벨 : %d/%d초",
					 pProfitTypeName[i], j,
					 pSFItem->GetName(), pSFItem->GetLevel(),
					 ( dwCurTime - m_stAppliedSF.m_dwApplyTime[i][j] ) / 1000,
					 pSFItem->GetEffectTime() / 1000 );

			DrawR3Hangul( 10, 120 + ( cnt * 15 ), pBuf, 0xFFA0A0FF, R3_HAN_OUTLINE );

			++cnt;
		}
	}
}

extern char g_pTargetClientPosMsg[128];	// 임시코드
extern char g_pTargetServerPosMsg[128];

void
CAvatar::RenderModeInfo( void )
{
	if( IsEnableAutoAttack() )
		DrawR3Hangul( 10, 290, "Auto Attack : On", 0xFFC0FFC0, R3_HAN_OUTLINE );
	else
		DrawR3Hangul( 10, 290, "Auto Attack : Off", 0xFFC0FFC0, R3_HAN_OUTLINE );

	//--------------------------------------------------------------------------

	if( _GetMainGame() )
	{
		if( _GetMainGame()->IsAutoMouse() )
			DrawR3Hangul( 10, 305, "Auto Mouse  : On", 0xFFC0FFC0, R3_HAN_OUTLINE );
		else
			DrawR3Hangul( 10, 305, "Auto Mouse  : Off", 0xFFC0FFC0, R3_HAN_OUTLINE );

		if( _GetMainGame()->IsFreeTargetCombatMode() )
			DrawR3Hangul( 10, 320, "Combat Mode : Free Target Mode", 0xFFC0FFC0, R3_HAN_OUTLINE );
		else
			DrawR3Hangul( 10, 320, "Combat Mode : Target Mode", 0xFFC0FFC0, R3_HAN_OUTLINE );
	}

	//--------------------------------------------------------------------------

	char l_pBuf[256];

	CCharacter * l_pTarget = _GetTargetMgr()->GetTarget();
	if( l_pTarget )
	{
		sprintf( l_pBuf, "Target : %s ( Index : %d )", l_pTarget->GetName(), l_pTarget->GetIndex() );
		DrawR3Hangul( 10, 335, l_pBuf, 0xFFC0FFC0, R3_HAN_OUTLINE );

		if( l_pTarget->GetCharTypeID() == CTI_PLAYER )
		{
			static char PART_NAME[5][8] = { "Upper", "Lower", "Gloves", "Shoes", "Helmet" };
			sprintf( l_pBuf, ", Weak Point : %s", PART_NAME[l_pTarget->GetConditionInfo()->m_byWeakPartID - IEPT_UPPER_PART] );
			DrawR3Hangul( 200, 335, l_pBuf, 0xFFC0FFC0, R3_HAN_OUTLINE );
		}
	}
	else
	{
		DrawR3Hangul( 10, 335, "Target : None", 0xFFC0FFC0, R3_HAN_OUTLINE );
	}

	//--------------------------------------------------------------------------

	DrawR3Hangul( 10, 350, g_pTargetClientPosMsg, 0xFFC0FFC0, R3_HAN_OUTLINE );
	DrawR3Hangul( 10, 365, g_pTargetServerPosMsg, 0xFFC0FFC0, R3_HAN_OUTLINE );

	/*
	// Internal Target
	//l_pTarget = IS_GetTarget();
	l_pTarget = _GetTargetMgr()->GetTarget();
	if( l_pTarget )
	{
		sprintf( l_pBuf, "Internal Target : %s ( Index : %d )", l_pTarget->GetName(), l_pTarget->GetIndex() );
		DrawR3Hangul( 10, 350, l_pBuf, 0xFFC0FFC0, R3_HAN_OUTLINE );
	}
	else
	{
		DrawR3Hangul( 10, 350, "Internal Target : None", 0xFFC0FFC0, R3_HAN_OUTLINE );
	}*/

	//--------------------------------------------------------------------------

	// FPS
	sprintf( l_pBuf, "FPS : %.3f", GetFPS() );
	DrawR3Hangul( 10, 380, l_pBuf, 0xFFC0FFC0, R3_HAN_OUTLINE );

	//--------------------------------------------------------------------------

	sprintf( l_pBuf, "Effect Frame : %d / %d", m_Animation.GetCurrentEffectFrameNo(), m_Animation.GetMaxEffectFrameNum() );
	DrawR3Hangul( 10, 395, l_pBuf, 0xFFC0FFC0, R3_HAN_OUTLINE );

	CAniData * l_pAni = m_Animation.GetAniData();
	if( l_pAni )
	{
		sprintf( l_pBuf, "%s", l_pAni->m_Name );
		DrawR3Hangul( 10, 410, l_pBuf, 0xFFC0FFC0, R3_HAN_OUTLINE );
	}

	if( !_GetTargetMgr()->IsEmptyTarget() )
	{
		CCharacter * l_pTarget = _GetTargetMgr()->GetTarget();
		if( l_pTarget )
		{
			CAnimation * l_pTargetAni = l_pTarget->GetAnimation();
			sprintf( l_pBuf, "Target Effect Frame : %d / %d, Bone Frame : %d, %d",
					 l_pTargetAni->GetCurrentEffectFrameNo(), l_pTargetAni->GetMaxEffectFrameNum(),
					 l_pTarget->GetBoneFrame(), l_pTargetAni->GetAniData()->m_MaxFrame );
			DrawR3Hangul( 10, 425, l_pBuf, 0xFFC0FFC0, R3_HAN_OUTLINE );
		}
	}

	//--------------------------------------------------------------------------

	if( GetSFItem() )
	{
		sprintf( l_pBuf, "Attack Range : %f", GetAttackRange( CAT_FORCE ) );
		DrawR3Hangul( 10, 440, l_pBuf, 0xFFC0FFC0, R3_HAN_OUTLINE );
	}
}

#endif

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CAvatar::SetSFUsingMsg( BOOL pi_bIsSuccessed )
{
#ifdef _DEVELOPER_

	static const char RESULT_TYPE[2][8] = { "실패", "성공" };
/*
	if( m_pSFItem->IsEqualType( CAT_SKILL ) )
	{
		sprintf( m_pSFUsingMsg, "[스킬] %s < Lv : %d > - %s ( 효과 : %s )",
				 m_pSFItem->GetName(), m_pSFItem->GetLevel(),
				 RESULT_TYPE[pi_bIsSuccessed],
				 SKILL_EFFECT_TYPE[m_pSFItem->GetEffectType()] );
	}
	else
	{
		sprintf( m_pSFUsingMsg, "[포스] %s < Lv : %d > - %s ( 효과 : %s )",
				 m_pSFItem->GetName(), m_pSFItem->GetLevel(),
				 RESULT_TYPE[pi_bIsSuccessed],
				 FORCE_EFFECT_TYPE[m_pSFItem->GetEffectType()] );
	}

	m_dwReciveSFUsingMsgTime = timeGetTime();
*/
#endif
}
