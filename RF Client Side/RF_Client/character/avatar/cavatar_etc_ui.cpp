#include "../../GameObject/Manager/CCharacterMgr.h"
#include "../../GameObject/Board/Common/CBoardManager.h"

#include "../../UserInterface/GameWindow/GUIChatWindow.h"
#include "../../UserInterface/GameWindow/EtcWin/GUIMapLoadingWindow.h"

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

void DisplayItemInfo( CBoardItem * pi_pItem, BYTE pi_byShopMode );


extern char SKILL_EFFECT_TYPE[28][128];
extern char FORCE_EFFECT_TYPE[47][128];

char WEAPON_CATEGORY_TYPE[16][16] =
{
	"나이프", "소드", "엑스", "메이스", "스피어", "보우", "파이어 암", "런쳐",
	"던지기 형", "스태프", "채굴기", "EX1", "EX2", "EX3", "EX4", "EX5"
};

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

// By B.H.K
// DialogBox가 열려있는 동안은 단축키를 못쓰게 하기 위해
int	g_nOpenedDlgBoxCount = 0;		// 열려있는 dialog box 갯수
int	g_nOpenedNPCDlgBoxCount = 0;	// NPC상점에 열려 있는 dialog box 갯수
BOOL g_nOpenedTradeWin = FALSE;		// 거래창이 열렸을 때 채팅 포커스를 풀기 위해 사용한다.

void
EP_DisableAlwaysActive( void )
{
	if( _GetChatWindow() )
		_GetChatWindow()->SetAlwaysActive( FALSE );

	if( _GetChatInputWindow() )
		_GetChatInputWindow()->SetAlwaysActive( FALSE );

	if( _GetGaugeWindow() )
		_GetGaugeWindow()->SetAlwaysTop( FALSE );

	if( _GetMapLoadingWindow() )
		_GetMapLoadingWindow()->SetAlwaysTop( FALSE );
}

void
EP_EnableAlwaysActive( void )
{
	if( _GetChatWindow() )
		_GetChatWindow()->SetAlwaysActive( TRUE );

	if( _GetChatInputWindow() )
		_GetChatInputWindow()->SetAlwaysActive( TRUE );

	if( _GetGaugeWindow() )
		_GetGaugeWindow()->SetAlwaysTop( TRUE );

	if( _GetMapLoadingWindow() )
		_GetMapLoadingWindow()->SetAlwaysTop( TRUE );
}

void
EP_OpenDialogBox( CGUIWindow * pi_pOwnerWindow, CGUIDialogBox * pi_pDialogBox )
{
	++g_nOpenedDlgBoxCount;

	// 메시지창이 열려있는 동안은 마우스에 붙어있는 아이템을 안보이도록 한다.
	if( g_nOpenedDlgBoxCount > 0 &&
		pi_pDialogBox &&
		_GetUIScreen()->ExistObject() )
	{
		_GetUIScreen()->GetObject()->Show( FALSE );
		_GetUIScreen()->GetMouse()->Show( TRUE );		
	}

	// dialogBox가 떠 있는 동안은 ownerwindow를 제외한 다른 window에 입력이 안가도록 한다.
	if( pi_pDialogBox )
		pi_pDialogBox->SetID( UII_NUMBER_INPUT_DIALOG );	

	EP_DisableAlwaysActive();
}

void
EP_CloseDialogBox( CGUIWindow * pi_pOwnerWindow )
{
	--g_nOpenedDlgBoxCount;	
	if( g_nOpenedDlgBoxCount < 0 )
		g_nOpenedDlgBoxCount = 0;

	if( !g_nOpenedDlgBoxCount && !g_nOpenedTradeWin )
	{
		EP_EnableAlwaysActive();

		// 마우스에 아이템이 붙어있으면 다시 보이도록 한다.
		if( _GetUIScreen()->ExistObject() )
		{
			_GetUIScreen()->GetObject()->Show( TRUE );
			_GetUIScreen()->GetMouse()->Show( FALSE );
		}
	}	
}

void
EP_OpenNPCDialogBox( CGUIWindow * pi_pOwnerWindow, CGUIDialogBox * pi_pDialogBox )
{
	EP_OpenDialogBox( pi_pOwnerWindow, pi_pDialogBox );

	++g_nOpenedNPCDlgBoxCount;
}

void
EP_CloseNPCDialogBox( CGUIWindow * pi_pOwnerWindow )
{
	EP_CloseDialogBox( pi_pOwnerWindow );

	--g_nOpenedNPCDlgBoxCount;
}

void
EP_OpenTradeWin( void )
{
	g_nOpenedTradeWin = TRUE;

	EP_DisableAlwaysActive();
}

void
EP_CloseTradeWin( void )
{
	g_nOpenedTradeWin = FALSE;

	if( !g_nOpenedDlgBoxCount )
	{
		EP_EnableAlwaysActive();
	}
}

// By B.H.K
//------------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

char SF_EFFECT_TYPE_ATTACK[8][32] = 
{
	"One Hit",
	"Double Hit",
	"Combo Hit",
	"Penalty Attack",
	"Self Area",
	"Flash Damage",
	"Extent",
	"Sector Area"
};

char SF_EFFECT_TYPE_SUPPORT_ONCE[15][128] =
{
	"자신의 HP를 대상의 FP로 만든다",
	"지속 피해보조 남은 시간 늘이기",
	"죽은 유저를 재생한다",
	"현재 HP증가",
	"현재 ST증가",
	"지속보조시간 증가",
	"MaxHP의 200%만큼만 가능하도록 한다",
	"가장 최근 지속이득보조 스킬 해제",
	"가장 최근 지속이득보조 포스 해제",
	"가장 최근 지속피해보조 포스 해제",
	"대상에 걸려있는 지속이득보조 스킬 제거",
	"대상에 걸려있는 지속이득보조 포스 제거",
	"대상에 걸려있는 지속 피해 보조 포스 해제",
	"시전자 주위 다른 종족들의 지속 이득 보조 스킬 & 포스 해제",
	"skill에 의한 지속적인 이득 남은 시간 늘이기"
};

char SF_EFFECT_TYPE_SUPPORT_CONTINUANCE_0[16][128] =
{
	"근접 공격력 증가",
	"원거리 공격력 증가",
	"런쳐 공격력 증가",
	"스킬 공격력 증가",
	"포스 공격력 증가",
	"스턴 확률",
	"방어력 증가",
	"FP소모율 감소",
	"지속피해보조 시간 증가",
	"최대 HP 증가",
	"최대 FP 증가",
	"최대 SP 증가",
	"데미지의 일부를 HP로 바꿈",
	"데미지의 일부를 FP로 바꿈",
	"데미지의 일부를 SP로 바꿈",
	"회복량 증가"
};

char SF_EFFECT_TYPE_SUPPORT_CONTINUANCE_1[27][128] =
{
	"근접 공격 성공 확률 증가",
	"원거리 공격 성공 확률 증가",
	"런쳐 공격 성공 확률 증가",
	"회피율 증가",
	"근접 공격 사정거리 증가",
	"원거리 공격 사정거리 증가",
	"런처 공격 사정거리 증가",
	"스킬 사정거리 증가",
	"포스 사정거리 증가",
	"근접 공격 딜레이 감소",
	"원거리 공격 딜레이 감소",
	"런처 공격 딜레이 감소",
	"스킬 사용 딜레이 감소",
	"포스 사용 딜레이 감소",
	"크리티컬 구간 증가",
	"불 내성 증가",
	"물 내성 증가",
	"흙 내성 증가",
	"바람 내성 증가",
	"스킬 레벨 상승",
	"뛰기 이동속도 상승",
	"스텔스",
	"디텍트",
	"지속피해보조 걸리지 않음",
	"대상의 약한 부위 찾기",
	"FP 회복량 증가",
	"시야범위 감소"
};

char SF_EFFECT_TYPE_SUPPORT_CONTINUANCE_2[15][128] =
{
	"대상의 공격 스킬 사용 제한",
	"대상의 공격 포스 사용 제한",
	"대상의 지속이득보조 스킬 사용 제한",
	"대상의 지속이득보조 포스 사용 제한",
	"대상의 지속피해보조 포스 사용 제한",
	"대상 스텔스",
	"대상의 이동 제한",
	"대상의 뛰기 제한",
	"절대회피",
	"크리티컬 성공 확률 100%",
	"대상의 HP/FP/SP회복 불가",
	"대상의 방패 방어력 무시",
	"대상의 방어력 무시",
	"스턴",
	"반격기"
};

char *
GetEffectStr_Attack( BYTE pi_byType )
{
	if( pi_byType >= 8 )
		return NULL;

	return SF_EFFECT_TYPE_ATTACK[pi_byType];
}

char *
GetEffectStr_Support_Once( BYTE pi_byType )
{
	if( pi_byType >= 15 )
		return NULL;

	return SF_EFFECT_TYPE_SUPPORT_ONCE[pi_byType];
}

char *
GetEffectStr_Support_Cont( BYTE pi_byType, BYTE pi_byTypeIndex )
{
	if( pi_byType > 2 )
		return NULL;
	
	if( pi_byType == 0 )
	{
		if( pi_byTypeIndex >= 16 )
			return NULL;

		return SF_EFFECT_TYPE_SUPPORT_CONTINUANCE_0[pi_byTypeIndex];
	}
	else if( pi_byType == 1 )
	{
		if( pi_byTypeIndex >= 27 )
			return NULL;

		return SF_EFFECT_TYPE_SUPPORT_CONTINUANCE_1[pi_byTypeIndex];
	}
	else
	{
		if( pi_byTypeIndex >= 15 )
			return NULL;

		return SF_EFFECT_TYPE_SUPPORT_CONTINUANCE_2[pi_byTypeIndex];
	}
}

///////////////////////////////////////////////////////////////////////////

void
DisplaySFInfo( CSFItem * pi_pSFItem )
{	
	if( !pi_pSFItem )
		return;

	SF_DATA * pSFData = pi_pSFItem->GetData();
	if( !pSFData )
		return;

	TOOLTIP_SF stTooltipInfo;
	ZeroMemory( &stTooltipInfo, sizeof( TOOLTIP_SF ) );

	_strcpy( stTooltipInfo.pNameStr, pSFData->pName_Kor );
	stTooltipInfo.byClassID	= pSFData->byType;
	stTooltipInfo.byLvID	= pSFData->byStep;
	stTooltipInfo.byLevel	= pi_pSFItem->GetLevel();
	stTooltipInfo.wMastery	= pi_pSFItem->GetExpByPercent();

	BOOL bUsableAllWeapon = TRUE;
	int  i, cnt;
	ZeroMemory( stTooltipInfo.pUsableWeapon, 320 );
	for( i = 0; i < 10; ++i )
	{
		if( pSFData->byNeedfulWeapon[i] != 0 )
		{
			bUsableAllWeapon = FALSE;
			break;
		}
	}

	if( bUsableAllWeapon )
	{
		_strcpy( stTooltipInfo.pUsableWeapon[0], "모든 무기" );
	}
	else
	{
		for( i = 0, cnt = 0; i < 10; ++i )
		{
			if( pSFData->byNeedfulWeapon[i] )
				_strcpy( stTooltipInfo.pUsableWeapon[cnt++], WEAPON_CATEGORY_TYPE[i] );
		}
	}


	/*
	// 효과
	BYTE byEffectType;
	ZeroMemory( stTooltipInfo.pEffectStr, MAX_SF_EFFECT * 128 );
	for( i = 0; i < MAX_SF_EFFECT; ++i )
	{
		if( pSFData->stEffect[i].byUsingType == 0xFF )
			continue;

		byEffectType = pSFData->stEffect[i].byEffectType[ pi_pSFItem->GetLevel() - 1 ];

		switch ( pSFData->stEffect[i].byUsingType )
		{
		case SFAT_ATTACK		:			
			_strcpy( stTooltipInfo.pEffectStr[i], GetEffectStr_Attack( byEffectType ), 128 );
			break;

		case SFAT_SUPPORT_ONCE	:			
			_strcpy( stTooltipInfo.pEffectStr[i], GetEffectStr_Support_Once( byEffectType ), 128 );
			break;

		case SFAT_SUPPORT_CONTINUANCE_PROFIT	:
		case SFAT_SUPPORT_CONTINUANCE_LOSS		:
			{
				BYTE byEffectUnit = pSFData->stEffect[i].byEffectUnit;
				_strcpy( stTooltipInfo.pEffectStr[i], GetEffectStr_Support_Cont( byEffectUnit, byEffectType ), 128 );
			}
			break;
		}		
	}
	*/

	stTooltipInfo.byNeedFP		= pSFData->byFP;
	stTooltipInfo.fDelay		= pSFData->fDelay;
	_strcpy( stTooltipInfo.pDescription, pSFData->pDescription, 128 );

	if( pi_pSFItem->IsEqualSFType( CAT_FORCE ) )
		stTooltipInfo.byAttribute	= ( ( FORCE_DATA * )pSFData )->byAttribute;

	if( pi_pSFItem->IsEqualSFType( CAT_FORCE ) )
		SetTooltip_Force( pi_pSFItem->GetIcon(), &stTooltipInfo );
	else
		SetTooltip_Skill( pi_pSFItem->GetIcon(), &stTooltipInfo );
}

void
DisplayBeltSFInfo( CSFItem * pi_pSFItem )
{
	if( !pi_pSFItem )
		return;
	if( !pi_pSFItem->GetData() )
		return;

	TOOLTIP_SF stTooltipInfo;

	_strcpy( stTooltipInfo.pNameStr, pi_pSFItem->GetData()->pName_Kor );			
	stTooltipInfo.byClassID		=	pi_pSFItem->GetData()->byType;	
	stTooltipInfo.byLevel		=	pi_pSFItem->GetLevel();	
	stTooltipInfo.byNeedFP		=	pi_pSFItem->GetData()->byFP;	

	if( pi_pSFItem->IsEqualSFType( CAT_FORCE ) )
		stTooltipInfo.byAttribute	= ( ( FORCE_DATA * )pi_pSFItem->GetData() )->byAttribute;

	
	BOOL bUsableAllWeapon = TRUE;
	int  i, cnt;
	ZeroMemory( stTooltipInfo.pUsableWeapon, 320 );
	for( i = 0; i < 10; ++i )
	{
		if( pi_pSFItem->GetData()->byNeedfulWeapon[i] != 0 )
		{
			bUsableAllWeapon = FALSE;
			break;
		}
	}

	if( bUsableAllWeapon )
	{
		_strcpy( stTooltipInfo.pUsableWeapon[0], "모든 무기" );
	}
	else
	{
		for( i = 0, cnt = 0; i < 10; ++i )
		{
			if( pi_pSFItem->GetData()->byNeedfulWeapon[i] )
				_strcpy( stTooltipInfo.pUsableWeapon[cnt++], WEAPON_CATEGORY_TYPE[i] );
		}
	}

	SetTooltip_SF_Belt( pi_pSFItem->GetIcon(), &stTooltipInfo, pi_pSFItem->GetSFType() );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
DisplayAnimusInfo( CAnimusItem * pi_AnimusItem )
{
	if( !pi_AnimusItem )
		return;

	TOOLTIP_ANIMUS	stItemInfo;
	ZeroMemory( &stItemInfo, sizeof( TOOLTIP_ANIMUS ) );

	_strcpy( stItemInfo.pNameStr, pi_AnimusItem->GetName() );

	stItemInfo.byLevel			= pi_AnimusItem->GetLevel();

	stItemInfo.byGrade			= pi_AnimusItem->GetGrade();

	stItemInfo.wUsableMastery	= pi_AnimusItem->GetNeedfulExpertness();

	stItemInfo.dwMinAttPower	= pi_AnimusItem->GetMinAttPower();
	stItemInfo.dwMaxAttPower	= pi_AnimusItem->GetMaxAttPower();

	stItemInfo.dwDeffensePower	= pi_AnimusItem->GetDeffencePower();

	for( int i = 0, cnt = 0; i < MAX_ITEM_ATTRIBUTE; ++i )
	{
		stItemInfo.dwTolerance[cnt]	= pi_AnimusItem->GetResist( i );
		if( stItemInfo.dwTolerance[cnt] > 0 )
		{
			stItemInfo.byToleranceType[cnt] = i + 1;
			++cnt;
		}
	}

	stItemInfo.byNeedFP			= pi_AnimusItem->GetNeedfulFP();

	_strcpy( stItemInfo.pDescription, pi_AnimusItem->GetDescription(), 128 );

	SetTooltip_Animus( pi_AnimusItem->GetIcon(), &stItemInfo );
}

void
DisplayBeltAnimusInfo( CAnimusItem * pi_AnimusItem )
{
	if( !pi_AnimusItem )
		return;

	TOOLTIP_ANIMUS	stItemInfo;
	ZeroMemory( &stItemInfo, sizeof( TOOLTIP_ANIMUS ) );

	_strcpy( stItemInfo.pNameStr, pi_AnimusItem->GetName() );

	stItemInfo.byLevel			= pi_AnimusItem->GetLevel();

	stItemInfo.byGrade			= pi_AnimusItem->GetGrade();

	stItemInfo.byNeedFP			= pi_AnimusItem->GetNeedfulFP();
	
	SetTooltip_Animus_Belt( pi_AnimusItem->GetIcon(), &stItemInfo );
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

void
DisplayWeaponInfo( CBoardItem * pi_pItem, BYTE pi_byShopMode )
{
	WEAPON_DATA * pItemData = ( WEAPON_DATA * )pi_pItem->GetData();
	if( !pItemData )
		return;

	TOOLTIP_WEAPON	stItemInfo;
	ZeroMemory( &stItemInfo, sizeof( TOOLTIP_WEAPON ) );

	_strcpy( stItemInfo.pNameStr, pi_pItem->GetName() );

	stItemInfo.wLevel				= pi_pItem->GetUsableLevel();

	BYTE byExpType;
	for( int i = 0; i < EQUIPMENT_DATA::MAX_UET_NUM; ++i )
	{
		byExpType = pi_pItem->GetUsableExpertnessType( i );
		if( byExpType < EVT_MAKE_WEAPON )
			stItemInfo.m_byNeedMastery[byExpType] = pi_pItem->GetUsableExpertness( i );
	}
	
	stItemInfo.dwCurDurability		= pi_pItem->GetDurability();
	stItemInfo.dwRepairDurability	= pi_pItem->GetRepairableMaxDP();
	stItemInfo.dwMaxDurability		= pi_pItem->GetCurMaxDurability();

	stItemInfo.byAttribute			= pi_pItem->GetWeaponAttributeType();

	stItemInfo.bRepairable			= pi_pItem->IsRepairable();

	// 업그레이드 정보
	stItemInfo.wUpgradeLevel		= pi_pItem->GetLevel();
	stItemInfo.byUpgradeSocketNo	= pi_pItem->GetMaxSocketNum();
	if( pi_byShopMode == SHOP_MODE_BUY && stItemInfo.byUpgradeSocketNo > 3 )
		stItemInfo.byUpgradeSocketNo = 3;


	_strcpy( stItemInfo.pDescription, pi_pItem->GetDescription(), 128 );

	// 상점 아이템
	if( pi_byShopMode == SHOP_MODE_BUY )
	{
		stItemInfo.dwMinGenAttPower[0]	= pItemData->GetMinStrikingPower();
		stItemInfo.dwMinGenAttPower[1]	= pItemData->GetMinStrikingPower();
		stItemInfo.dwMaxGenAttPower[0]	= pItemData->GetMaxStrikingPower();
		stItemInfo.dwMaxGenAttPower[1]	= pItemData->GetMaxStrikingPower();

		stItemInfo.dwMinForceAttPower[0]= pItemData->GetMinForcePower();
		stItemInfo.dwMinForceAttPower[1]= pItemData->GetMinForcePower();
		stItemInfo.dwMaxForceAttPower[0]= pItemData->GetMaxForcePower();
		stItemInfo.dwMaxForceAttPower[1]= pItemData->GetMaxForcePower();
	}
	// 인벤 아이템
	else
	{
		stItemInfo.dwMinGenAttPower[0]	= pItemData->GetMinStrikingPower();
		stItemInfo.dwMaxGenAttPower[0]	= pItemData->GetMaxStrikingPower();
		stItemInfo.dwMinForceAttPower[0]= pItemData->GetMinForcePower();
		stItemInfo.dwMaxForceAttPower[0]= pItemData->GetMaxForcePower();

		stItemInfo.dwMinGenAttPower[1]	= pItemData->GetMinStrikingPower() * ( 1.0f + pi_pItem->GetUpgradeEffectInfo()->m_fStrikingPower );
		stItemInfo.dwMaxGenAttPower[1]	= pItemData->GetMaxStrikingPower() * ( 1.0f + pi_pItem->GetUpgradeEffectInfo()->m_fStrikingPower );
		stItemInfo.dwMinForceAttPower[1]= pItemData->GetMinForcePower() * ( 1.0f + pi_pItem->GetUpgradeEffectInfo()->m_fStrikingPower );
		stItemInfo.dwMaxForceAttPower[1]= pItemData->GetMaxForcePower() * ( 1.0f + pi_pItem->GetUpgradeEffectInfo()->m_fStrikingPower );		

		INGOT_DATA * l_pTalik;
		for( int i = 0; i < stItemInfo.wUpgradeLevel; ++i )
		{
			l_pTalik = pi_pItem->GetTalik( i );
			if( l_pTalik )
				stItemInfo.dwUpgradeSpriteID[i] = l_pTalik->GetIconID();
		}
	}

	// 거래중 가격 표시
	if( pi_byShopMode != SHOP_MODE_NONE )
	{
		stItemInfo.byPriceType = pi_pItem->GetMoneyType();

		if( pi_byShopMode == SHOP_MODE_BUY )
			stItemInfo.dwPrice	= pi_pItem->GetBuyPrice();
		else if( pi_byShopMode == SHOP_MODE_SELL )
			stItemInfo.dwPrice	= pi_pItem->GetSellPrice( pi_pItem->GetQuantity() );
		else if( pi_byShopMode == SHOP_MODE_REPAIR )
			stItemInfo.dwPrice	= pi_pItem->GetRepairPrice();		
	}	

	// set tooltip
	SetTooltip_Weapon( pi_pItem->GetIcon(), &stItemInfo, pi_byShopMode );
};

void
DisplayArmorInfo( CBoardItem * pi_pItem, BYTE pi_byShopMode )
{
	ARMOR_DATA * pItemData = ( ARMOR_DATA * )pi_pItem->GetData();
	if( !pItemData )
		return;

	int i, cnt;
	TOOLTIP_ARMOR	stItemInfo;
	ZeroMemory( &stItemInfo, sizeof( TOOLTIP_ARMOR ) );
	
	_strcpy( stItemInfo.pNameStr, pi_pItem->GetName() );

	stItemInfo.wLevel				= pi_pItem->GetUsableLevel();

	BYTE byExpType;
	for( i = 0; i < EQUIPMENT_DATA::MAX_UET_NUM; ++i )
	{
		byExpType = pi_pItem->GetUsableExpertnessType( i );
		if( byExpType < EVT_MAKE_WEAPON )
			stItemInfo.m_byNeedMastery[byExpType] = pi_pItem->GetUsableExpertness( i );
	}

	stItemInfo.dwCurDurability		= pi_pItem->GetDurability();
	stItemInfo.dwRepairDurability	= pi_pItem->GetRepairableMaxDP();
	stItemInfo.dwMaxDurability		= pi_pItem->GetCurMaxDurability();

	stItemInfo.bRepairable			= pi_pItem->IsRepairable();

	// 내성
	for( i = 0, cnt = 0; i < MAX_ITEM_ATTRIBUTE; ++i )
	{
		stItemInfo.dwTolerance[cnt]	= pItemData->GetResist( i );
		if( stItemInfo.dwTolerance[cnt] > 0 )
		{
			stItemInfo.byToleranceType[cnt] = i + 1;
			++cnt;
		}
	}

	// 업그레이드 정보
	stItemInfo.wUpgradeLevel		= pi_pItem->GetLevel();
	stItemInfo.byUpgradeSocketNo	= pi_pItem->GetMaxSocketNum();
	if( pi_byShopMode == SHOP_MODE_BUY && stItemInfo.byUpgradeSocketNo > 3 )
		stItemInfo.byUpgradeSocketNo = 3;

	_strcpy( stItemInfo.pDescription, pi_pItem->GetDescription(), 128 );
		
	// 상점 아이템
	if( pi_byShopMode == SHOP_MODE_BUY )
	{
		stItemInfo.dwDeffensePower[0]	= pItemData->GetDefencePoint();
		stItemInfo.dwDeffensePower[1]	= pItemData->GetDefencePoint();
	}
	// 인벤 아이템
	else
	{
		stItemInfo.dwDeffensePower[0]	= pItemData->GetDefencePoint();
		stItemInfo.dwDeffensePower[1]	= pItemData->GetDefencePoint() * pi_pItem->GetUpgradeEffectInfo()->m_fDefencePower;

		INGOT_DATA * pTalik;
		for( int i = 0; i < stItemInfo.wUpgradeLevel; ++i )
		{
			pTalik = pi_pItem->GetTalik( i );
			if( pTalik )
				stItemInfo.dwUpgradeSpriteID[i] = pTalik->GetIconID();
		}
	}

	// 거래중 가격 표시
	if( pi_byShopMode != SHOP_MODE_NONE )
	{
		stItemInfo.byPriceType = pi_pItem->GetMoneyType();

		if( pi_byShopMode == SHOP_MODE_BUY )
			stItemInfo.dwPrice	= pi_pItem->GetBuyPrice();
		else if( pi_byShopMode == SHOP_MODE_SELL )
			stItemInfo.dwPrice	= pi_pItem->GetSellPrice( pi_pItem->GetQuantity() );
		else if( pi_byShopMode == SHOP_MODE_REPAIR )
			stItemInfo.dwPrice	= pi_pItem->GetRepairPrice();		
	}

	// set tooltip
	SetTooltip_Armor( pi_pItem->GetIcon(), &stItemInfo, pi_byShopMode );	
};

void
DisplayAccessoryInfo( CBoardItem * pi_pItem, BYTE pi_byShopMode )
{
	RING_DATA *	pItemData = ( RING_DATA * )pi_pItem->GetData();
	if( !pItemData )
		return;

	TOOLTIP_ACCESSORY	stItemInfo;
	ZeroMemory( &stItemInfo, sizeof( TOOLTIP_ACCESSORY ) );
	
	_strcpy( stItemInfo.pNameStr, pi_pItem->GetName() );
	stItemInfo.wLevel = pi_pItem->GetUsableLevel();	

	// 효과
	//	stItemInfo.pEffectStr

	// 내성
	for( int i = 0, cnt = 0; i < MAX_ITEM_ATTRIBUTE; ++i )
	{
		stItemInfo.dwTolerance[cnt]	= pItemData->GetResist( i );
		if( stItemInfo.dwTolerance[cnt] > 0 )
		{
			stItemInfo.byToleranceType[cnt] = i + 1;
			++cnt;
		}
	}

	_strcpy( stItemInfo.pDescription, pi_pItem->GetDescription(), 128 );

	// 상점 아이템
	if( pi_byShopMode != SHOP_MODE_NONE ) 	
	{
		stItemInfo.byPriceType = pi_pItem->GetMoneyType();

		if( pi_byShopMode == SHOP_MODE_BUY )
			stItemInfo.dwPrice	= pi_pItem->GetBuyPrice();
		else if( pi_byShopMode == SHOP_MODE_SELL )
			stItemInfo.dwPrice	= pi_pItem->GetSellPrice( pi_pItem->GetQuantity() );
		else if( pi_byShopMode == SHOP_MODE_REPAIR )
			stItemInfo.dwPrice	= pi_pItem->GetRepairPrice();			
	}

	// set tooltip
	SetTooltip_Accessory( pi_pItem->GetIcon(), &stItemInfo, pi_byShopMode );
};

void
DisplayBulletInfo( CBoardItem * pi_pItem, BYTE pi_byShopMode )
{
	TOOLTIP_BULLET	stItemInfo;
	ZeroMemory( &stItemInfo, sizeof( TOOLTIP_BULLET ) );

	_strcpy( stItemInfo.pNameStr, pi_pItem->GetName() );	

	stItemInfo.dwRestQuantity	= pi_pItem->GetDurability();

	BULLET_DATA * pBulletData = ( BULLET_DATA * )pi_pItem->GetData();

	// 사용가능한 무기
	ITEM_DATA * pItemData;
	DWORD *		l_pWeaponDTIndexList = pBulletData->GetUsableWeaponList();
	for( int i = 0; i < pBulletData->GetUsableWeaponNum(); ++i )
	{
		pItemData = ( ITEM_DATA * )_GetItemDataMgr()->GetDataByDTCode( IEPT_WEAPON, l_pWeaponDTIndexList[i] );
		if( pItemData )
			_strcpy( stItemInfo.pUsableWeapon[i], pItemData->GetName() );
	}
	
	stItemInfo.dwMinAttackPower = pBulletData->GetIncreasePointOfStrikingPower();
	stItemInfo.dwMaxAttackPower	= pBulletData->GetIncreasePointOfStrikingPower();

	// 속성
	stItemInfo.byAttribute		= pBulletData->byAttribute;

	// 부가 속성
	//stItemInfo.byAddAttribute	= pBulletData->byAttribute;

	// 설명
	_strcpy( stItemInfo.pDescription, pi_pItem->GetDescription(), 128 );

	// 상점 아이템
	if( pi_byShopMode != SHOP_MODE_NONE ) 	
	{
		stItemInfo.byPriceType = pi_pItem->GetMoneyType();

		if( pi_byShopMode == SHOP_MODE_BUY )
			stItemInfo.dwPrice	= pi_pItem->GetBuyPrice();
		else if( pi_byShopMode == SHOP_MODE_SELL )
			stItemInfo.dwPrice	= pi_pItem->GetSellPrice( pi_pItem->GetQuantity() );
		else if( pi_byShopMode == SHOP_MODE_REPAIR )
			stItemInfo.dwPrice	= pi_pItem->GetRepairPrice();			
	}

	// set tooltip
	SetTooltip_Bullet( pi_pItem->GetIcon(), &stItemInfo, pi_byShopMode );
};

void
DisplayPotionInfo( CBoardItem * pi_pItem, BYTE pi_byShopMode )
{
	TOOLTIP_POTION	stItemInfo;
	ZeroMemory( &stItemInfo, sizeof( TOOLTIP_POTION ) );

	_strcpy( stItemInfo.pNameStr, pi_pItem->GetName() );		

	POTION_DATA * pData = ( POTION_DATA * )pi_pItem->GetData();

	stItemInfo.dwReoveryPoint	= pData->GetEffectValue( 0 );
	stItemInfo.wQuantity		= pi_pItem->GetQuantity();

	_strcpy( stItemInfo.pDescription, pi_pItem->GetDescription(),128 );

	// 상점 아이템
	if( pi_byShopMode != SHOP_MODE_NONE ) 	
	{
		stItemInfo.byPriceType = pi_pItem->GetMoneyType();

		if( pi_byShopMode == SHOP_MODE_BUY )
			stItemInfo.dwPrice	= pi_pItem->GetBuyPrice();
		else if( pi_byShopMode == SHOP_MODE_SELL )
			stItemInfo.dwPrice	= pi_pItem->GetSellPrice( pi_pItem->GetQuantity() );
		else if( pi_byShopMode == SHOP_MODE_REPAIR )
			stItemInfo.dwPrice	= pi_pItem->GetRepairPrice();			
	}

	// set tooltip
	SetTooltip_Potion( pi_pItem->GetIcon(), &stItemInfo, pi_byShopMode );
};

void
DisplayEtcInfo( CBoardItem * pi_pItem, BYTE pi_byShopMode, BOOL pi_bExistQuantity )
{	
	TOOLTIP_ETC_ITEM	stItemInfo;
	ZeroMemory( &stItemInfo, sizeof( TOOLTIP_ETC_ITEM ) );

	_strcpy( stItemInfo.pNameStr, pi_pItem->GetName() );

	_strcpy( stItemInfo.pDescription, pi_pItem->GetDescription(), 128 );

	if( pi_bExistQuantity )
		stItemInfo.dwQuantity = pi_pItem->GetQuantity();

	// 상점 아이템
	if( pi_byShopMode != SHOP_MODE_NONE ) 	
	{
		stItemInfo.byPriceType = pi_pItem->GetMoneyType();

		if( pi_byShopMode == SHOP_MODE_BUY )
			stItemInfo.dwPrice	= pi_pItem->GetBuyPrice();
		else if( pi_byShopMode == SHOP_MODE_SELL )
			stItemInfo.dwPrice	= pi_pItem->GetSellPrice( pi_pItem->GetQuantity() );
		else if( pi_byShopMode == SHOP_MODE_REPAIR )
			stItemInfo.dwPrice	= pi_pItem->GetRepairPrice();			
	}

	// set tooltip
	SetTooltip_Etc( pi_pItem->GetIcon(), &stItemInfo, pi_byShopMode, pi_bExistQuantity );
};

void
DisplayBatteryInfo( CBoardItem * pi_pItem, BYTE pi_byShopMode )
{
	TOOLTIP_BATTERY	stItemInfo;
	ZeroMemory( &stItemInfo, sizeof( TOOLTIP_BATTERY ) );

	_strcpy( stItemInfo.pNameStr, pi_pItem->GetName() );
	
	stItemInfo.dwQuantity = pi_pItem->GetDurability();

	_strcpy( stItemInfo.pDescription, pi_pItem->GetDescription(), 128 );

	// 상점 아이템
	if( pi_byShopMode != SHOP_MODE_NONE ) 	
	{
		stItemInfo.byPriceType = pi_pItem->GetMoneyType();

		if( pi_byShopMode == SHOP_MODE_BUY )
			stItemInfo.dwPrice	= pi_pItem->GetBuyPrice();
		else if( pi_byShopMode == SHOP_MODE_SELL )
			stItemInfo.dwPrice	= pi_pItem->GetSellPrice( pi_pItem->GetQuantity() );
		else if( pi_byShopMode == SHOP_MODE_REPAIR )
			stItemInfo.dwPrice	= pi_pItem->GetRepairPrice();			
	}

	// set tooltip
	SetTooltip_Battery( pi_pItem->GetIcon(), &stItemInfo, pi_byShopMode );
};

// 유닛
void
DisplayUnitInfo( CBoardItem * pi_pItem, BYTE pi_byShopMode )
{
	static CUnitItem	s_clsUnitItem;
	if( !s_clsUnitItem.CreateByFrameType( pi_pItem->GetDTIndex() ) )
		return;

	UNIT_FRAME_DATA * pItemData = ( UNIT_FRAME_DATA * )s_clsUnitItem.GetData();
	if( !pItemData )
		return;

	TOOLTIP_UNIT_ITEM stItemInfo;
	ZeroMemory( &stItemInfo, sizeof( TOOLTIP_UNIT_ITEM ) );

	_strcpy( stItemInfo.pNameStr, s_clsUnitItem.GetName() );

	CUnitPartsItem * pAttPartsItem = s_clsUnitItem.GetAttackParts();
	if( pAttPartsItem )
	{
		UNIT_WEAPON_DATA * pWeaponData = ( UNIT_WEAPON_DATA * )pAttPartsItem->GetData();
		stItemInfo.byAttackType		= pWeaponData->byRangeType;
	}

	s_clsUnitItem.GetMoveSpeedString( stItemInfo.pMoveSpeed );

	stItemInfo.dwMinAttackPower		= s_clsUnitItem.GetMinStrikingPower();
	stItemInfo.dwMaxAttackPower		= s_clsUnitItem.GetMaxStrikingPower();

	stItemInfo.dwDeffensePower		= s_clsUnitItem.GetDefencePower();

	stItemInfo.dwCurDurability		= s_clsUnitItem.GetDurability();
	stItemInfo.dwMaxDurability		= pItemData->dwMaxDP;

	// 내성
	for( int i = 0, cnt = 0; i < MAX_ITEM_ATTRIBUTE; ++i )
	{
		stItemInfo.dwTolerance[cnt]	= s_clsUnitItem.GetResist( i );
		if( stItemInfo.dwTolerance[cnt] > 0 )
		{
			stItemInfo.byToleranceType[cnt] = i + 1;
			++cnt;
		}
	}

	_strcpy( stItemInfo.pDescription, s_clsUnitItem.GetDescription(), 128 );

	// 상점 아이템
	if( pi_byShopMode != SHOP_MODE_NONE )
	{
		stItemInfo.byPriceType = s_clsUnitItem.GetMoneyType();

		if( pi_byShopMode == SHOP_MODE_BUY )
			stItemInfo.dwPrice	= s_clsUnitItem.GetBuyPrice();
		else if( pi_byShopMode == SHOP_MODE_SELL )
			stItemInfo.dwPrice	= s_clsUnitItem.GetSellPrice( pi_pItem->GetQuantity() );
		else if( pi_byShopMode == SHOP_MODE_REPAIR )
			stItemInfo.dwPrice	= pItemData->dwRepairPrice;
	}

	// set tooltip
	SetTooltip_UnitItem( pi_pItem->GetIcon(), &stItemInfo, pi_byShopMode );
}

void
DisplayAnimusLiverInfo( CBoardItem * pi_pItem, BYTE pi_byShopMode )
{
	TOOLTIP_ANIMUS_LIVER_ITEM stItemInfo;
	ZeroMemory( &stItemInfo, sizeof( TOOLTIP_ANIMUS_LIVER_ITEM ) );

	_strcpy( stItemInfo.pNameStr, pi_pItem->GetName() );

	_strcpy( stItemInfo.pDescription, pi_pItem->GetDescription(), 128 );

	// 상점 아이템
	if( pi_byShopMode != SHOP_MODE_NONE )
	{
		stItemInfo.byPriceType = pi_pItem->GetMoneyType();

		if( pi_byShopMode == SHOP_MODE_BUY )
			stItemInfo.dwPrice	= pi_pItem->GetBuyPrice();
		else if( pi_byShopMode == SHOP_MODE_SELL )
			stItemInfo.dwPrice	= pi_pItem->GetSellPrice( pi_pItem->GetQuantity() );
		else if( pi_byShopMode == SHOP_MODE_REPAIR )
			stItemInfo.dwPrice	= pi_pItem->GetRepairPrice();
	}

	// set tooltip
	SetTooltip_AnimusLiver( pi_pItem->GetIcon(), &stItemInfo, pi_byShopMode );
}

void DisplayItemInfo( CBoardItem * pi_pItem, BYTE pi_byShopMode )
{
	if( !pi_pItem )
		return;

	switch( pi_pItem->GetItemKind() )
	{
		case IEPT_FACE			:
		case IEPT_UPPER_PART	:
		case IEPT_LOWER_PART	:
		case IEPT_GLOVES		:
		case IEPT_SHOES			:
		case IEPT_HELMET		:
		case IEPT_SHIELD		:
		case IEPT_CLOAK			:
			DisplayArmorInfo( pi_pItem, pi_byShopMode );
			break;

		case IEPT_WEAPON		:
			DisplayWeaponInfo( pi_pItem, pi_byShopMode );
			break;

		case IEPT_POTION		:
			DisplayPotionInfo( pi_pItem, pi_byShopMode );
			break;

		case IEPT_BULLET		:
			DisplayBulletInfo( pi_pItem, pi_byShopMode );
			break;

		case IEPT_BATTERY		:
			DisplayBatteryInfo( pi_pItem, pi_byShopMode );
			break;

		case IEPT_ORE			:
		case IEPT_INGOT			:
		case IEPT_LOOT			:
		case IEPT_RETURN		:
		case IEPT_DUNGEON		:
		case IEPT_GUARD_TOWER	:
			DisplayEtcInfo( pi_pItem, pi_byShopMode, TRUE );
			break;

		case IEPT_UNIT			:
			DisplayUnitInfo( pi_pItem, pi_byShopMode );
			break;

		case IEPT_WORKTOOL		:
		case IEPT_ETC			:
		case IEPT_FORCE			:
			DisplayEtcInfo( pi_pItem, pi_byShopMode, FALSE );
			break;

		case IEPT_RING			:
		case IEPT_AMULET		:
			DisplayAccessoryInfo( pi_pItem, pi_byShopMode );
			break;

		case IEPT_MAP			:
			DisplayEtcInfo( pi_pItem, pi_byShopMode, FALSE );
			break;

		case IEPT_ANIMUS		:
			DisplayAnimusLiverInfo( pi_pItem, pi_byShopMode );

		default					:
			return;
	}
}

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 유닛 파츠
void
DisplayUnitParts( CUnitPartsItem * pi_pUnitParts, BYTE pi_byShopMode )
{
	if( !pi_pUnitParts )
		return;

	UNIT_PART_DATA	* pPartsData = ( UNIT_PART_DATA * )pi_pUnitParts->GetData();
	if( !pPartsData )
		return;

	TOOLTIP_UNIT_PARTS	stPartsInfo;
	ZeroMemory( &stPartsInfo, sizeof( TOOLTIP_UNIT_PARTS ) );

	stPartsInfo.byPartsType			= ( pi_pUnitParts->GetItemKind() - IEPT_UNIT_HELMET );

	stPartsInfo.byEquableLevel		= pPartsData->byEquableLevel;

	_strcpy( stPartsInfo.pNameStr, pi_pUnitParts->GetName() );

	stPartsInfo.dwMinAttackPower	= pi_pUnitParts->GetMinStrikingPower();
	stPartsInfo.dwMaxAttackPower	= pi_pUnitParts->GetMaxStrikingPower();

	stPartsInfo.dwDeffensePower		= pi_pUnitParts->GetDefencePower();

	stPartsInfo.dwMoveSpeed			= pi_pUnitParts->GetMoveSpeed();

	if( pi_pUnitParts->IsEqualItemKind( IEPT_UNIT_ARMS ) ||
		pi_pUnitParts->IsEqualItemKind( IEPT_UNIT_SHOULDER ) )
	{
		UNIT_WEAPON_DATA * pWeaponData = ( UNIT_WEAPON_DATA * )( pPartsData );

		stPartsInfo.dwAttackDelay	= pWeaponData->dwAttackDelay;

		stPartsInfo.byAttackType	= pWeaponData->byRangeType;
	}

	stPartsInfo.byAttribute			= 0;

	// 내성
	for( int i = 0, cnt = 0; i < MAX_ITEM_ATTRIBUTE; ++i )
	{
		stPartsInfo.dwTolerance[cnt]	= pi_pUnitParts->GetResist( i );
		if( stPartsInfo.dwTolerance[cnt] > 0 )
		{
			stPartsInfo.byToleranceType[cnt] = i + 1;
			++cnt;
		}
	}

	stPartsInfo.dwDurability		= pi_pUnitParts->GetDurability();
	stPartsInfo.byMaxBulletNum		= pi_pUnitParts->GetBulletSocketNum();

	// set tooltip
	SetTooltip_UnitParts( pi_pUnitParts->GetIcon(), &stPartsInfo, pi_byShopMode );
}
