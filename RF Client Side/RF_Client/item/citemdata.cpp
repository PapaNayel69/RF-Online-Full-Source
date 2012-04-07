////////////////////////////////////////////////////////////////////////////////
//
// CBaseItemData Class Implementation
//
////////////////////////////////////////////////////////////////////////////////

#ifdef _NEW_UI_

#include "../GameObject/Manager/CCharacterMgr.h"
#include "../GameObject/Manager/CGroundItemManager.h"
#include "../GameObject/Board/Common/CBoardManager.h"
#include "../GameObject/Board/General/CPartyBoard.h"

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CBoardObject::CBoardObject()
{
	m_byItemType	= BOT_BASE;
	m_bIsUsing		= FALSE;

	m_uiIcon		= NULL;
}

CBoardObject::~CBoardObject()
{
}

BOOL
CBoardObject::IsEqual( DWORD pi_dwIndex )
{
	return ( m_dwIndex == pi_dwIndex );
}

BOOL
CBoardObject::IsEqual( CBoardObject * pi_pTarObject )
{
	if( !pi_pTarObject )
		return FALSE;

	return ( m_byItemType	== pi_pTarObject->GetItemType() &&
			 m_dwIndex		== pi_pTarObject->GetIndex() );
}

void
CBoardObject::BeginDelayAnimation( DWORD pi_dwTime )
{
	if( m_uiIcon )
		m_uiIcon->BeginDelayAnimation( pi_dwTime );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CBaseItem::CBaseItem()
{
	Init();
}

CBaseItem::~CBaseItem()
{
}

void
CBaseItem::Init( void )
{
	m_pData	= NULL;
}

void
CBaseItem::Clear( void )
{
	Init();
}

BOOL
CBaseItem::Create( BYTE pi_byItemKind, DWORD pi_dwDTIndex )
{
	Clear();

	m_pData = static_cast< ITEM_DATA * >( _GetItemDataMgr()->GetData( pi_byItemKind, pi_dwDTIndex ) );

	return reinterpret_cast< BOOL >( m_pData );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

DWORD
CBaseItem::GetOrgMaxDurability( void )
{
	if( IsEquipmentItem() )
		return ( ( EQUIPMENT_DATA * )m_pData )->dwMaxDP;
	else if( IsEqualItemKind( IEPT_BATTERY ) )
		return ( ( BATTERY_DATA * )m_pData )->dwMaxDP;
	else if( IsEqualItemKind( IEPT_BULLET ) )
		return ( ( BULLET_DATA * )m_pData )->dwMaxDP;
	else if( IsEqualItemKind( IEPT_UNIT ) )
		return ( ( UNIT_FRAME_DATA * )m_pData )->dwMaxDP;
	else if( IsEqualItemKind( IEPT_UNIT_BACKPACK ) )
		return ( ( UNIT_BACKPACK_DATA * )m_pData )->byMaxBP;
	else if( IsEqualItemKind( IEPT_UNIT_BULLET ) )
		return ( ( UNIT_BULLET_DATA * )m_pData )->dwMaxDP;

	return 0;
}

BYTE
CBaseItem::GetUsableLevel( void )
{
	if( !m_pData )
		return 0;

	if( IsEquipmentItem() )
		return ( ( EQUIPMENT_DATA * )m_pData )->byUsableLevel;
	else if( IsEqualItemKind( IEPT_POTION ) )
		return ( ( POTION_DATA * )m_pData )->byUsableLevel;

	return 0;
}

BYTE
CBaseItem::GetUsableExpertnessType( BYTE pi_byOrderNo )
{
	if( IsEquipmentItem() )
		return static_cast< EQUIPMENT_DATA * >( m_pData )->GetUsableExpertnessType( pi_byOrderNo );
	
	return ID_INVALID;
}

BYTE
CBaseItem::GetUsableExpertness( BYTE pi_byOrderNo )
{
	if( IsEquipmentItem() )
		return static_cast< EQUIPMENT_DATA * >( m_pData )->GetUsableExpertness( pi_byOrderNo );
	
	return ID_INVALID;
}

BOOL
CBaseItem::IsSameKind( CBaseItem * pi_pItem )
{
	if( !m_pData || !pi_pItem )
		return FALSE;

	return ( ( GetItemKind() == pi_pItem->GetItemKind() ) &&
			 ( GetDTIndex() == pi_pItem->GetDTIndex() ) );
}

BOOL
CBaseItem::IsSameKind( BYTE pi_byItemKind, DWORD pi_dwDTIndex )
{
	return ( ( GetItemKind() == pi_byItemKind ) && ( GetDTIndex() == pi_dwDTIndex ) );
}

BOOL
CBaseItem::IsEquipmentItem( void )
{
	if( !m_pData )
		return FALSE;

	BYTE byItemType = m_pData->byEquipType;

	return ( ( byItemType >= IEPT_UPPER_PART ) && ( byItemType <= CEPT_CLOAK ) );
}

BOOL
CBaseItem::IsArmor( void )
{
	if( !m_pData )
		return FALSE;

	BYTE byItemType = m_pData->byEquipType;

	if( ( byItemType == IEPT_UPPER_PART ) || ( byItemType == IEPT_LOWER_PART ) ||
		( byItemType == IEPT_GLOVES ) || ( byItemType == IEPT_SHOES ) ||
		( byItemType == IEPT_HELMET ) || ( byItemType == IEPT_SHIELD ) ||
		( byItemType == IEPT_CLOAK ) )
			return TRUE;

	return FALSE;
}

BOOL
CBaseItem::IsSplitable( void )
{
	if( !m_pData )
		return FALSE;

	BYTE byItemKind = m_pData->byEquipType;

	if( ( byItemKind == IEPT_LOOT ) || ( byItemKind == IEPT_POTION ) ||
		( byItemKind == IEPT_ORE ) || ( byItemKind == IEPT_INGOT ) ||
		( byItemKind == IEPT_RETURN ) )
		return TRUE;

	return FALSE;
}

BOOL
CBaseItem::IsRepairable( void )
{
	if( !IsEquipmentItem() )
		return FALSE;
	
	EQUIPMENT_DATA * pEquipmentData = static_cast< EQUIPMENT_DATA * >( m_pData );

	return pEquipmentData->bIsRepairable;
}

BOOL
CBaseItem::IsUpgradable( void )
{
	if( !IsEquipmentItem() )
		return FALSE;

	if( static_cast< EQUIPMENT_DATA * >( m_pData )->GetUpgradeSocketNum() > 0 )
		return TRUE;

	return FALSE;
}

BOOL
CBaseItem::IsMineTool( void )
{
	if( !m_pData )
		return FALSE;

	if( m_pData->byEquipType == IEPT_WEAPON )
	{
		if( ( m_pData->dwMeshID & 0x000FFF00 ) == 0x00011800 )
			return TRUE;
	}

	return FALSE;
}

BYTE
CBaseItem::GetHoldType( void )
{
	if( IsEqualItemKind( CEPT_WEAPON ) )
	{
		DWORD dwWeaponKind = ( GetMeshID() & 0x0000FF00 ) >> 8;

		switch( dwWeaponKind )
		{
			case IWT_KNIFE_LT		:
			case IWT_AXE_LT			:
				return IHT_LEFT_HAND;

			case IWT_SWORD_R		:
			case IWT_KNIFE_R		:
			case IWT_AXE_R			:
			case IWT_MACE_R			:
			case IWT_STAFF_R		:
				return IHT_RIGHT_HAND;

			case IWT_SWORD_B		:
			case IWT_KNIFE_B		:
			case IWT_AXE_B			:
			case IWT_MACE_B			:
			case IWT_STAFF_B		:
			case IWT_SPEAR_B		:
			case IWT_BOW_B			:
			case IWT_CROSSBOW_B		:
			case IWT_GUN_B			:
			case IWT_BEAM_GUN_B		:
			case IWT_RIFLE_B		:
			case IWT_LAUNCHER_B		:
			case IWT_FAUST_B		:
			case IWT_MACHINEGUN_B	:
			case IWT_FLAMETHROWER_B	:
			case IWT_BEAM_RIFLE_B	:
			case IWT_BEAM_PLASMA_B	:
			case IWT_MINING_TOOL	:
				return IHT_BOTH_HAND;

			case IWT_SWORD_D		:
			case IWT_AXE_D			:
			case IWT_GUN_D			:
			case IWT_BEAM_GUN_D		:
				return IHT_DUAL_HAND;

			default					:
				return IHT_RIGHT_HAND;
		}
	}
	else if( IsEqualItemKind( CEPT_SHIELD ) )
	{
		return IHT_LEFT_HAND;
	}

	return IHT_NONE;
}

BYTE
CBaseItem::GetAttackDistanceType( void )
{
	if( IsEqualItemKind( CEPT_WEAPON ) )
	{
		switch( ( GetMeshID() & 0x0000FF00 ) >> 8 )
		{
			case IWT_SWORD_R		:
			case IWT_SWORD_B		:
			case IWT_SWORD_D		:
			case IWT_KNIFE_R		:
			case IWT_KNIFE_B		:
			case IWT_AXE_R			:
			case IWT_AXE_B			:
			case IWT_AXE_D			:
			case IWT_MACE_R			:
			case IWT_MACE_B			:
			case IWT_STAFF_R		:
			case IWT_STAFF_B		:
			case IWT_SPEAR_B		:
			case IWT_FIST			:
				return ADT_NEAR;

			case IWT_KNIFE_LT		:
			case IWT_AXE_LT			:
			case IWT_BOW_B			:
			case IWT_CROSSBOW_B		:
			case IWT_GUN_B			:
			case IWT_GUN_D			:
			case IWT_RIFLE_B		:
			case IWT_LAUNCHER_B		:
			case IWT_FAUST_B		:
			case IWT_MACHINEGUN_B	:
			case IWT_FLAMETHROWER_B	:
			case IWT_BEAM_GUN_B		:
			case IWT_BEAM_GUN_D		:
			case IWT_BEAM_RIFLE_B	:
			case IWT_BEAM_PLASMA_B	:
				return ADT_FAR;
		}
	}

	return ADT_NONE;
}

float
CBaseItem::GetAttackRange( void )
{
	if( IsEqualItemKind( IEPT_WEAPON ) )
		return static_cast< WEAPON_DATA * >( m_pData )->fDistance;
	if( IsEqualItemKind( IEPT_UNIT_ARMS ) || IsEqualItemKind( IEPT_UNIT_SHOULDER ) )
		return static_cast< UNIT_WEAPON_DATA * >( m_pData )->fAttackDistance;

	return 0.0f;
}

int
CBaseItem::GetAttackDelay( void )
{
	if( IsEqualItemKind( IEPT_WEAPON ) )
		return static_cast< WEAPON_DATA * >( m_pData )->nSpeed;

	return 0;
}

int
CBaseItem::GetWeaponCategoryID( void )
{
	if( IsEqualItemKind( IEPT_WEAPON ) )
		return static_cast< WEAPON_DATA * >( m_pData )->GetWeaponCategoryID();

	return ID_INVALID;
}

BOOL
CBaseItem::CheckWeaponWithBullet( DWORD pi_dwWeaponDTCode )
{
	if( IsEqualItemKind( IEPT_BULLET ) )
		return static_cast< BULLET_DATA * >( m_pData )->IsUsableWeapon( pi_dwWeaponDTCode );

	return FALSE;
}

BYTE
CBaseItem::GetUnitPartType( void )
{
	if( !m_pData )
		return 0xFF;

	if( ( m_pData->byEquipType >= IEPT_UNIT_HELMET ) && ( m_pData->byEquipType <= IEPT_UNIT_BULLET ) )
	{
		UNIT_PART_DATA * pPartData = static_cast< UNIT_PART_DATA * >( m_pData );
		return pPartData->byPartType;
	}

	return 0xFF;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 같은 종족의 아이템일 경우에는 * 0.5f, 다른 종족의 아이템일 경우에는 * 1.0f
float
CBaseItem::GetSellPrice( BYTE pi_byNum )
{
	CNpc * pMerchant = _GetShopTradeBoard()->GetMerchant();
	if( !pMerchant || !m_pData )
		return 0;

	// set price rate by race
	BYTE	byNpcRace = pMerchant->GetRace() * 2;
	float	fRaceRate = 1.0f;
	if( m_pData->byUsableRace[byNpcRace] || m_pData->byUsableRace[byNpcRace+1] )
		fRaceRate = 0.5f;

	// calculate sell price
	float fSellPrice;

	// ( DOWRD )로 캐스팅 해 줘서 +0.5f의 효과가 나타나도록 한다.
	fSellPrice = ( DWORD )( ( GetStandardPrice() * fRaceRate * GetDurabilityRate() ) + 0.5f );
	fSellPrice *= _GetAvatar()->GetSellTexRate();

	if( pi_byNum > 1 )
		fSellPrice *= pi_byNum;

	return fSellPrice;
}

float
CBaseItem::GetBuyPrice( BYTE pi_byNum )
{
	float fBuyPrice = GetStandardPrice() * _GetAvatar()->GetBuyTexRate();

	if( pi_byNum > 1 )
		fBuyPrice *= pi_byNum;

	return fBuyPrice;
}

// 원석 가공 비용
float
CBaseItem::GetOreProcPrice( BYTE pi_byNum )
{
	if( !IsEqualItemKind( IEPT_ORE ) )
		return 0;

	ORE_DATA *	pOreData			= static_cast< ORE_DATA * >( m_pData );
	DWORD		dwManageValue		= _GetAvatar()->GetExchangeManageValue();
	float		fAverageIngotNum	= ( float )( pOreData->byIngotMinNum + pOreData->byIngotMaxNum ) / 2.0f;
	float		fOreProcPrice		= 0;

	fOreProcPrice	= ( pOreData->dwOreProcPrice + ( dwManageValue * pOreData->dwOreProcPrice / 1000.0f ) );
	fOreProcPrice	*= _GetAvatar()->GetBuyTexRate();

	if( pi_byNum > 1 )
		fOreProcPrice *= pi_byNum;

	return fOreProcPrice;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

char *
CBaseItem::GetDescription( void )
{
	if( m_pData )
		return _GetItemDataMgr()->GetItemDescription( m_pData->byEquipType, m_pData->GetDescriptionDTIndex() );

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CItemUpgradeEffect::CItemUpgradeEffect()
{
	ZeroMemory( this, sizeof( CItemUpgradeEffect ) );

	m_fDefencePower		= 1.0f;
	m_byAttackAttribute	= 0xFF;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CNormalItem::CNormalItem()
{
	Init();
}

CNormalItem::~CNormalItem()
{
}

void
CNormalItem::Init( void )
{
	m_byQuantity				= 1;
	m_dwDurability				= 0;
	m_dwCurMaxDurability		= 0;	

	m_byMaxSocketNum			= 0;
	m_byLevel					= 0;
	ZeroMemory( m_listTalik, sizeof( INGOT_DATA * ) * MAX_ITEM_LEVEL );
}

void
CNormalItem::Clear( void )
{
	CBaseItem::Clear();

	Init();
}

BOOL
CNormalItem::Create( BYTE pi_byItemKind, DWORD pi_dwDTIndex )
{
	if( !CBaseItem::Create( pi_byItemKind, pi_dwDTIndex ) )
		return FALSE;

	if( !IsSplitable() )
		m_dwCurMaxDurability = m_dwDurability = GetOrgMaxDurability();

	if( IsEquipmentItem() )
		m_byMaxSocketNum = ( ( EQUIPMENT_DATA * )m_pData )->byUpgradeSocketNum;

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

float
CNormalItem::GetDurabilityRate( void )
{
	float	fDurabilityRate = 1.0f;
	DWORD	dwOrgMaxDurability = GetOrgMaxDurability();

	if( ( m_dwDurability != 0 ) && ( dwOrgMaxDurability != 0 ) )
		fDurabilityRate = (float)m_dwDurability / (float)dwOrgMaxDurability;

	return fDurabilityRate;
}

DWORD
CNormalItem::GetRepairableMaxDP( void )
{
	if( !IsRepairable() )
		return 0;

	if( IsEqualItemKind( IEPT_UNIT ) )
		return GetOrgMaxDurability();
	else
		return ( ( ( m_dwCurMaxDurability + m_dwDurability ) * 0.5f ) + 0.5f );
}

float
CNormalItem::GetRepairPrice( void )
{
	if( !IsRepairable() )
		return 0.0f;

	if( GetDurability() >= GetCurMaxDurability() )
		return 0.0f;

	EQUIPMENT_DATA * pEqumentData = static_cast< EQUIPMENT_DATA * >( m_pData );

	if( pEqumentData->GetRepairPrice() > 0.0f )
		return ( float )pEqumentData->GetRepairPrice();
	else
		return ( (float)( GetRepairableMaxDP() - GetDurability() ) * (float)GetStandardPrice() / (float)GetOrgMaxDurability() ) + 0.5f;

	return 0.0f;
}

DWORD
CNormalItem::GetDefencePoint( void )
{
	if( !IsArmor() )
		return 0;

	ARMOR_DATA * pArmorData = static_cast< ARMOR_DATA * >( m_pData );

	return ( pArmorData->dwDefencePoint * m_clUpgradeEffect.m_fDefencePower );
}

WORD
CNormalItem::GetResist( BYTE pi_byType )
{
	if( pi_byType >= MAX_ITEM_ATTRIBUTE )
		return 0;

	if( IsArmor() )
	{
		ARMOR_DATA * pArmorData = static_cast< ARMOR_DATA * >( m_pData );
		return ( pArmorData->wResist[pi_byType] + m_clUpgradeEffect.m_byResist[pi_byType] );
	}
	else if( IsEqualItemKind( IEPT_RING ) )
	{
		RING_DATA * pRingData = static_cast< RING_DATA * >( m_pData );
		return ( pRingData->wResist[pi_byType] + m_clUpgradeEffect.m_byResist[pi_byType] );
	}

	return 0;
}

BYTE
CNormalItem::GetWeaponAttributeType( void )
{
	if( !IsEqualItemKind( IEPT_WEAPON ) )
		return 0xFF;

	// 업그레이드를 통해 속성이 변했다면 변한 속성으로 리턴
	if( m_clUpgradeEffect.m_byAttackAttribute != 0xFF )
		return m_clUpgradeEffect.m_byAttackAttribute;
	else
		return static_cast< WEAPON_DATA * >( m_pData )->byAttributeType;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CNormalItem::InsertTalik( BYTE pi_byTalikDTIndex )
{
	if( m_byLevel < MAX_ITEM_LEVEL )
	{
		InsertTalik( static_cast< INGOT_DATA * >( _GetItemDataMgr()->GetData( IEPT_INGOT, pi_byTalikDTIndex + 20 ) ) );
	}
}

void
CNormalItem::InsertTalik( INGOT_DATA * pi_pTalikData )
{
	if( pi_pTalikData )
	{
		if( m_byLevel < MAX_ITEM_LEVEL )
		{
			++m_byLevel;
			m_listTalik[m_byLevel-1] = pi_pTalikData;
			ApplyUpgradeEffect( m_listTalik[m_byLevel-1], TRUE );
		}
	}
}

void
CNormalItem::InsertTalikFromServerFlag( DWORD pi_dwServerFlag )
{
	if( !IsUpgradable() )
		return;

	if( pi_dwServerFlag == ID_INVALID )
	{
		m_byMaxSocketNum = 0;
		return;
	}

	BYTE pi_byTalikDTIndex, i;

	m_byMaxSocketNum = pi_dwServerFlag >> 28;
	for( i = 0; i < m_byMaxSocketNum; ++i )
	{
		pi_byTalikDTIndex = ( pi_dwServerFlag >> ( i * 4 ) ) & 0x0000000F;
		if( pi_byTalikDTIndex != 0xF )
			InsertTalik( pi_byTalikDTIndex );
	}
}

void
CNormalItem::ExtractTalik( void )
{
	if( m_byLevel > 0 )
	{
		ApplyUpgradeEffect( m_listTalik[m_byLevel-1], FALSE );
		m_listTalik[m_byLevel-1] = NULL;
		--m_byLevel;
	}
}

void
CNormalItem::ExtractAllTalik( void )
{
	if( m_byLevel > 0 )
	{
		for( int i = m_byLevel - 1; i >= 0; --i )
		{
			ApplyUpgradeEffect( m_listTalik[i], FALSE );
			m_listTalik[i] = NULL;
		}

		m_byLevel = 0;
	}
}

INGOT_DATA *
CNormalItem::GetTalik( BYTE pi_bySocketNo )
{
	if( ( pi_bySocketNo >= m_byLevel ) || ( pi_bySocketNo >= MAX_ITEM_LEVEL ) )
		return NULL;

	return m_listTalik[pi_bySocketNo];
}

DWORD
CNormalItem::IsEnableInsertJewel( DWORD pi_dwJewelDTIndex )
{
	BYTE byItemKind = GetItemKind();
	BYTE byWeaponCategoryID = GetWeaponCategoryID();

	if( !( ( byItemKind > IEPT_FACE ) && ( byItemKind <= IEPT_SHIELD ) ) )
		return 0;

	INGOT_DATA * pJewelData = static_cast< INGOT_DATA * >( _GetItemDataMgr()->GetData( IEPT_INGOT, pi_dwJewelDTIndex ) );
	if( !pJewelData )
		return 0;

	// 아이템이 무기일 경우
	if( byItemKind == IEPT_WEAPON )
	{
		// 근접 무기
		if( ( byWeaponCategoryID == IWCT_KNIFE ) || ( byWeaponCategoryID == IWCT_SWORD ) ||
			( byWeaponCategoryID == IWCT_AXE ) || ( byWeaponCategoryID == IWCT_MACE ) ||
			( byWeaponCategoryID == IWCT_SPEAR ) || ( byWeaponCategoryID == IWCT_STAFF ) )
		{
			if( pJewelData->GetUpgradeInsertLimitNum( IEPT_WEAPON - 1 ) <= 0 )
				return 1;
		}
		// 원거리 무기
		else if( ( byWeaponCategoryID == IWCT_BOW ) || ( byWeaponCategoryID == IWCT_FIREARM ) ||
				 ( byWeaponCategoryID == IWCT_LUANCHER ) || ( byWeaponCategoryID == IWCT_THROW ) )
		{
			if( pJewelData->GetUpgradeInsertLimitNum( 7 ) <= 0 )
				return 1;
		}

		// 무기에는 광휘, 정화, 수호, 신념의 탈릭 중 오직 하나만을 사용할 수 있다. ( DT Index 27 ~ 30 )
		if( ( pi_dwJewelDTIndex >= 27 ) && ( pi_dwJewelDTIndex <= 30 ) )
		{
			for( int i = 0; i < m_byLevel; ++i )
			{
				if( !m_listTalik[i] )
					continue;
				if( ( m_listTalik[i]->GetDTIndex() >= 27 ) && ( m_listTalik[i]->GetDTIndex() <= 30 ) )
					return 2;
			}
		}
	}
	// 아이템이 아머일 경우
	else
	{
		if( pJewelData->GetUpgradeInsertLimitNum( byItemKind - 1 ) <= 0 )
			return 1;
	}

	return 3;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CNormalItem::ApplyUpgradeEffect( INGOT_DATA * pi_pTalik, BOOL pi_bIsAdd )
{
	BYTE byTalikNum = GetSameTalikNum( pi_pTalik );
	if( byTalikNum <= 0 )
		return;

	if( !pi_bIsAdd )
		--byTalikNum;

	BYTE	byEffectType	= pi_pTalik->byUpgradeEffectType;
	float	fEffectConstant	= pi_pTalik->fUpgradeEffectConstant[ byTalikNum - 1 ];

	// 탈릭의 이펙트 적용
	switch( byEffectType )
	{
		case 0 :
			break;

		case 1 :			// 모든공격력UP 1
			if( byTalikNum <= 0 )	m_clUpgradeEffect.m_fStrikingPower = 0;
			else					m_clUpgradeEffect.m_fStrikingPower = fEffectConstant;
			break;

		case 2 :			// 가격하는_데미지의_데이터필드_퍼센트값만큼을_HP로_흡수 2
		case 3 :			// 가격하는_데미지의_데이터필드_퍼센트값만큼을_FP로_흡수 3
			break;

		case 4 :			// 가격당하는_대상을_stun시킴(데이터필드_퍼센트값만큼의_확률로_1초동안) 4 => 아이템이 아닌 캐릭터에 적용되므로 +1은 하지 않는다.
			if( byTalikNum <= 0 )	m_clUpgradeEffect.m_fStunProb = 0;
			else					m_clUpgradeEffect.m_fStunProb = fEffectConstant;
			break;

		case 5 :			// 사정거리UP 5
			if( byTalikNum <= 0 )	m_clUpgradeEffect.m_byAttackRange = 0;
			else					m_clUpgradeEffect.m_byAttackRange = fEffectConstant;
			break;

		case 6 :			// 방어력UP 6
			if( byTalikNum <= 0 )	m_clUpgradeEffect.m_fDefencePower = 1;
			else					m_clUpgradeEffect.m_fDefencePower = 1 + fEffectConstant;
			break;

		case 7 :			// 데이터필드_퍼센트값만큼의_지속피해보조시간_단축 7
			break;

		case 8 :			// 캐릭터의_불내성값에_데이터필드_값만큼의_불내성증가 8
		case 9 :			// 캐릭터의_불내성값에_데이터필드_값만큼의_물내성증가 9
		case 10 :			// 캐릭터의_불내성값에_데이터필드_값만큼의_흙내성증가 9
		case 11 :			// 캐릭터의_불내성값에_데이터필드_값만큼의_바람내성증가 9
			// 무기일 경우는 속성 변화, 방어구일 경우는 내성 증가임.
			if( IsEqualItemKind( IEPT_WEAPON ) )
			{
				if( byTalikNum <= 0 )	m_clUpgradeEffect.m_byAttackAttribute = 0xFF;
				else					m_clUpgradeEffect.m_byAttackAttribute = byEffectType-8;
			}
			else
			{				
				if( byTalikNum <= 0 )	m_clUpgradeEffect.m_byResist[byEffectType-8] = 0;
				else					m_clUpgradeEffect.m_byResist[byEffectType-8] = fEffectConstant;
			}
			break;

		case 12 :			// 데이터필드_퍼센트값만큼의_HP회복량_증가 12
			break;

		case 13 :			// 데이터필드_퍼센트값만큼의_회피율_증가 13 => 아이템이 아닌 캐릭터에 적용되므로 +1은 하지 않는다.
			if( byTalikNum <= 0 )	m_clUpgradeEffect.m_fAvoidProb = 0;
			else					m_clUpgradeEffect.m_fAvoidProb = fEffectConstant;
			break;

		case 14 :			// 가장_최근에_삽입한_탈릭_날려버림 14
			break;
	}
}

// 동일한 탈릭이 있는지를 체크해서, 개수를 가져온다.
BYTE
CNormalItem::GetSameTalikNum( INGOT_DATA * pi_pTalik )
{
	if( !pi_pTalik )
		return 0;
	if( !IsUpgradable() )
		return 0;
	
	BYTE byTalikNum = 0;
	for( int i = 0; i < m_byLevel; ++i )
	{
		if( !m_listTalik[i] )
			continue;

		if( m_listTalik[i]->dwDTIndex == pi_pTalik->dwDTIndex )
			++byTalikNum;
	}

	return byTalikNum;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CBoardItem::CBoardItem()
{
	Init();
}

void
CBoardItem::Init( void )
{
	m_byItemType		= BOT_BOARD_ITEM;

	m_byObjectTypeID	= OTI_ITEM;
	m_dwExtra			= ID_INVALID;
}

void
CBoardItem::Clear( void )
{
	CNormalItem::Clear();

	Init();
}

BOOL
CBoardItem::Create( BYTE pi_byItemKind, DWORD pi_dwDTIndex )
{
	if( !CNormalItem::Create( pi_byItemKind, pi_dwDTIndex ) )
		return FALSE;

	SetIndex( pi_dwDTIndex | 0xF0000000 );
	SetServerIndex( pi_dwDTIndex | 0xF0000000 );
	SetName( m_pData->pName );

	return TRUE;
}

void
CBoardItem::SetData( ITEM_DATA * pi_pData )
{
	CBaseItem::SetData( pi_pData );

	if( m_pData )
		SetName( m_pData->pName );
}

void
CBoardItem::SetQuantity( BYTE pi_byQuantity )
{
	m_byQuantity = pi_byQuantity;

	if( m_uiIcon )
		m_uiIcon->m_wNum = m_byQuantity;
}

BYTE
CBoardItem::AddQuantity( BYTE pi_byQuantity )
{
	int nResult = m_byQuantity + pi_byQuantity;
	if( nResult > MAX_ITEM_QUANTITY )
		return 0;

	m_byQuantity += pi_byQuantity;
	if( m_uiIcon )
		m_uiIcon->m_wNum = m_byQuantity;

	return m_byQuantity;
}

BYTE
CBoardItem::RemoveQuantity( BYTE pi_byQuantity )
{
	int nResult = m_byQuantity - pi_byQuantity;
	if( nResult < 0 )
		return 0;

	m_byQuantity -= pi_byQuantity;
	if( m_uiIcon )
		m_uiIcon->m_wNum = m_byQuantity;

	return m_byQuantity;
}

CBoardObject *
CBoardItem::New( void )
{
	return ( dynamic_cast< CBoardObject * >( new CBoardItem ) );
}

CBoardObject *
CBoardItem::Clone( void )
{
	return ( dynamic_cast< CBoardObject * >( new CBoardItem( *this ) ) );
}

void
CBoardItem::Copy( CObject * pi_pOrgObject )
{
	if( pi_pOrgObject )
		memcpy( this, static_cast< CBoardItem * >( pi_pOrgObject ), sizeof( CBoardItem ) );
}

void
CBoardItem::CopyToNormalItem( CNormalItem * pi_pNormalItem )
{
	if( !pi_pNormalItem )
		return;

	pi_pNormalItem->m_pData					= m_pData;

	pi_pNormalItem->m_byQuantity			= m_byQuantity;
	pi_pNormalItem->m_dwDurability			= m_dwDurability;
	pi_pNormalItem->m_dwCurMaxDurability	= m_dwCurMaxDurability;

	pi_pNormalItem->m_byMaxSocketNum		= m_byMaxSocketNum;
	pi_pNormalItem->m_byLevel				= m_byLevel;
	for( int i = 0; i < MAX_ITEM_LEVEL; ++i )
		pi_pNormalItem->m_listTalik[i] = m_listTalik[i];
	memcpy( &pi_pNormalItem->m_clUpgradeEffect, &m_clUpgradeEffect, sizeof( CItemUpgradeEffect ) );

//	if( pi_pNormalItem )
//		memcpy( &pi_pNormalItem->m_pData, &m_pData, sizeof( CNormalItem ) );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "../AddCodebyJang/ForEffect.h"
#include "../AddCodebyJang/ItemSpecialEffect.h"
#include "../AddCodebyJang/EffectUtil.h"
#include "../AddCodebyJang/EffectCore.h"

///*MAYUN
void DrawCharacterctEffect(CHARACTEROBJECT *cobj,float pos[3],float rot,float alpha,float scale = 1.0f);
void SetCharacterctEffectArgv(DWORD id,Vector3f pos);	//이펙트더미의 인자들...

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CMeshItem::CMeshItem()
{
	Init();
}

CMeshItem::CMeshItem( CNormalItem * pi_pLinkItem )
{
	Init();

	m_pLinkItem	= pi_pLinkItem;
}

void
CMeshItem::Init( void )
{
	m_byObjectTypeID	= OTI_ITEM;

	m_pLinkItem			= NULL;
	m_pMesh				= NULL;

	Enable( ROSF_CHECK_Y_POS | ROSF_RENDER_MESH | ROSF_ANIMATION_LOOP | ROSF_MOVABLE );

// J.U.I < begin > -------------------------------------------------------------
	m_dwEFUniqueID		= 0;
	m_bCheckExistEF		= FALSE;
	m_dwRandRot			= rand()/100.0f;		//그라운드 아이템의 다양한 회전을 위한
	for( int i = 0; i < MAX_EFFECT_DUMMY; ++i )	//효과더미 인덱스들
		m_byDummy[i]	= 0;
// J.U.I < end > ---------------------------------------------------------------
}

void
CMeshItem::Clear( void )
{
	CBaseItem::Clear();

	Init();
}

// CMeshItem::Create() 의 pi_dwDTIndex는 최상위 바이트에 플레이어의 종족정보가 포함되어 있음.
// 기갑 장비의 경우에는 최상위 바이트가 무조건 0xE로 설정되어 있다.
BOOL
CMeshItem::Create( BYTE pi_byItemKind, DWORD pi_dwDTIndex )
{
	BYTE byRLIType	= RLI_ITEM;
	BYTE byRace		= ( pi_dwDTIndex & 0xF0000000 ) >> 28;
	pi_dwDTIndex &= 0x0FFFFFFF;

	if( !CBaseItem::Create( pi_byItemKind, pi_dwDTIndex ) )
		return FALSE;

	SetIndex( pi_dwDTIndex | 0xF0000000 );
	SetServerIndex( pi_dwDTIndex | 0xF0000000 );
	SetName( m_pData->pName );

	DWORD dwRealMeshID = m_pData->dwMeshID;

	// 그라운드 아이템이 아니라면 성별 구분이 필요
	if( !IsEqualItemType( BOT_GROUND_ITEM ) )
	{
		// 기갑 장비 메시일 경우
		if( byRace == 0xE )
		{
			byRLIType = RLI_UNIT;
		}
		// 플레이어 메시일 경우, 방어구라면 성별로 MeshID를 설정해 준다. ( 0xF면 무시 )
		else if( byRace != 0xF )
		{
			if(	( ( m_pData->dwMeshID & 0x000F0000 ) == 0x00000000 ) &&
				( ( m_pData->dwMeshID & 0x0000FF00 ) != 0x00000700 ) )
				dwRealMeshID = ( m_pData->dwMeshID & 0xFF0FFFFF ) | ( byRace << 20 );
		}
	}

	SetBone( dwRealMeshID, byRLIType );
	SetMesh( dwRealMeshID, byRLIType );

	// effect
	if( m_bCheckExistEF )	//이상하게 딜리트 안되고 올때가 있다.
		DeleteItemEffect();
	m_bCheckExistEF	= FALSE;

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CMeshItem::Init_EveryFrame( void )
{
	CResObject::Init_EveryFrame();
}

BOOL
CMeshItem::Render( void )
{
	if( !CResObject::Render() )
		return FALSE;

	if( !Render_CheckBeforeRender() )
		return FALSE;

	Render_PushMeshAlpha();

	_GetCharIF()->SetMaterial( m_pMesh, m_d3dMaterial );

	Render_RenderWithEffect();

	Render_PopMeshAlpha();

	return TRUE;
}

BOOL
CMeshItem::Render_CheckBeforeRender( void )
{
	if( !m_pMesh )
		return FALSE;

	//if( !( IsEnable( ROSF_RENDER_MESH ) && IsDisable( ROSF_CLIPPED ) ) )
	if( IsDisable( ROSF_RENDER_MESH ) || IsEnable( ROSF_CLIPPED ) )
		return FALSE;

	return TRUE;
}

void
CMeshItem::Render_PushMeshAlpha( void )
{
	if( IsEnable( ROSF_RENDER_ALPHA ) )
		_GetCharIF()->SetAlpha( m_pMesh, m_fAlphaDensity );
}

void
CMeshItem::Render_PopMeshAlpha( void )
{
	if( IsEnable( ROSF_RENDER_ALPHA ) )
		_GetCharIF()->SetAlpha( m_pMesh, 1.0f );
}

// MAYUN < begin > -------------------------------------------------------------

void
CMeshItem::Render_RenderWithEffect( void )
{
	static	float scroll = 0.0f;
	static	float enscroll = 0.0f;
	DWORD	ctime = timeGetTime();
	static	DWORD	btime = 0.0f;
	if (!btime)btime = ctime;

	float delta = ((float)(ctime - btime)) * 0.0005f;
	
	btime = ctime;
	scroll += delta ;
					
	_GetCharIF()->InitEffect(m_pMesh);
	//
	float alphad = _GetCharIF()->GetAlpha( m_pMesh );
	_GetCharIF()->SetAlpha( m_pMesh,alphad );
	//
	if (scroll>1.0)	scroll = 0.0;

	
	if ( GetUnitPartType()!=0xFF )
	{
		DWORD part =  GetItemKind() - IEPT_UNIT_HELMET;
		g_EFFUnitList.SetEffect( GetMesh(), GetDTIndex() + part * 100 );
		_GetCharIF()->DrawCharacter( m_pMesh, m_vecPos, m_vecRot[1], 1.0f, scroll );
		if (m_pMesh->IsHaveEffect())
				_GetItemEffectRenderer()->AddItem( this );
	}
	else
	{
		
		//MAYUN
		DWORD racecode = m_pMesh->m_ID >> 5*4;
		if (racecode ==1 || racecode==3)
			SetMeshEffect( m_pMesh, GetMeshID(), GetLevel(), 5 );
		else
			SetMeshEffect( m_pMesh, GetMeshID(), GetLevel() );

		_GetCharIF()->DrawCharacter( m_pMesh, m_vecPos, m_vecRot[1], 1.0f, scroll );
		if( IsHaveMeshEffect(m_pMesh, GetMeshID(), GetLevel()  ))
		{
			if (m_pMesh->IsHaveEffect())
				_GetItemEffectRenderer()->AddItem( this );
		}
	}

	
}

//이펙트에쓰일 더미위치를 구한다.
BOOL
CMeshItem::GetDummyPos( Vector3f * pi_pVecPos )
{
	if( !m_pMesh )
		return FALSE;

	ObjectMesh* pObjectMesh	= m_pMesh->m_pMesh;

	D3DXMATRIX yrot,ytemp;
	D3DXVECTOR3 ppp3;
	D3DXVECTOR4 pOut[2];

	DWORD m[10];
	m[0]=GetDummyIndex(_DUMMY_TRACE1);
	m[1]=GetDummyIndex(_DUMMY_TRACE2);

	ppp3.x=0;
	ppp3.y=0;
	ppp3.z=0;
	MatrixRotate(yrot.m,0,0,360-m_vecRot[1]);
	for( DWORD i=0; i<2; i++)
	{
		if( m[i] == 0xff )
			return FALSE;
		MatrixMultiply(ytemp.m, yrot.m, pObjectMesh[m[i]].m_MatResult.m);
		//ytemp = pObjectMesh[].m_MatResult*yrot;
		D3DXVec3Transform(&pOut[i],&ppp3,&ytemp);
		pi_pVecPos[i][0] = pOut[i].x;
		pi_pVecPos[i][1] = -pOut[i].z;
		pi_pVecPos[i][2] = pOut[i].y;
	}
	return TRUE;
}

BOOL
CMeshItem::RenderWithShadow( void )
{
	Vector3f l_vecDummy[10];

	if( !Render_CheckBeforeRender() )
		return FALSE;

	Render_PushMeshAlpha();

	_GetCharIF()->SetMaterial( m_pMesh, m_d3dMaterial );

	// 렌더링
	_GetCharIF()->UnSetState();
	SetMeshEffect( m_pMesh, GetMeshID(), GetLevel() );
	float alphad = _GetCharIF()->GetAlpha( m_pMesh );
	if( GetDummyPos(l_vecDummy) )	//이펙트 더미가 있으면 셋팅.
	{
		SetCharacterctEffectArgv(0,l_vecDummy[0]);
		SetCharacterctEffectArgv(1,l_vecDummy[1]);
	}
	DrawCharacterctEffect( m_pMesh, m_vecPos, m_vecRot[1],alphad );

	Render_PopMeshAlpha();

	return TRUE;
}

// MAYUN < end > ---------------------------------------------------------------

BOOL
CMeshItem::Animation( DWORD pi_dwAniFrame )
{
	if( !CResObject::Animation( pi_dwAniFrame ) )
		return FALSE;

	if( IsDisable( ROSF_ANIMATION ) )
		return FALSE;

	if( !m_pMesh )
		return FALSE;

	_GetCharIF()->FrameMove( m_pMesh );

	return TRUE;
}

void
CMeshItem::FrameMove( void )
{
	FrameMove_ItemEffect();

	if( IsDisable( ROSF_MOVABLE ) )
		return;

	CResObject::FrameMove();

	if( m_Animation.IsEqualID( IAI_EQUIP ) || m_Animation.IsEqualID( IAI_UNEQUIP ) )
	{
		if( !m_Animation.IsAnimating() )
			SetAnimation( IAI_DEFAULT );
	}
}

BOOL
CMeshItem::RenderName( void )
{
	return CResObject::RenderName();
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CMeshItem::SetAnimation( DWORD pi_dwAniID )
{
	m_Animation.SetAnimationID( ID_INVALID );
	m_Animation.SetAniData( NULL );

	CAniData * pAniData = GetAnimationData( pi_dwAniID );
	if( !pAniData && ( pi_dwAniID != IAI_DEFAULT ) )
		pAniData = GetAnimationData( IAI_DEFAULT );

	if( !pAniData )
		return FALSE;

	m_Animation.SetAniData( pAniData );
	m_Animation.SetAnimationID( IAI_DEFAULT );
	m_dwOldBoneFrame	= 0;
	m_dwBoneFrame		= 160;

	ResetAnimation( pAniData );

	return TRUE;
}

CAniData *
CMeshItem::GetAnimationData( DWORD pi_dwAniID )
{
	DWORD dwAnimationID = 0;
	
	dwAnimationID = ( GetMeshID() << 4 ) | pi_dwAniID;
	m_Animation.SetAnimationID( dwAnimationID );

	return _GetAniMgr()->GetAni( RLI_ITEM, dwAnimationID );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// pi_byRLIType 는 RLI_ITEM 이나 RLI_UNIT 중의 하나
void
CMeshItem::SetBone( DWORD pi_dwItemDTIndex, BYTE pi_byRLIType )
{
	m_pBone = _GetMeshMgr()->GetMesh( pi_byRLIType, pi_dwItemDTIndex );

	m_pBBox = _GetBBoxMgr()->GetBoundBox( pi_byRLIType, pi_dwItemDTIndex );
}

void
CMeshItem::SetMesh( DWORD pi_dwItemDTIndex, BYTE pi_byRLIType )
{
	m_pMesh = _GetMeshMgr()->GetMesh( pi_byRLIType, pi_dwItemDTIndex );
}

BYTE
CMeshItem::GetLevel( void )
{
	if( !m_pLinkItem )
		return 0;

	return m_pLinkItem->GetLevel();
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// J.U.I < begin > -------------------------------------------------------------

void
CMeshItem::FrameMove_ItemEffect( void )
{
	if( !m_pMesh )
		return;

	// 아이템에 들러붙는 효과들 처리
	if( !m_bCheckExistEF && m_pMesh->m_Load )
	{
		m_bCheckExistEF=TRUE;
		m_dwEFUniqueID = ::EFUtil_GetUniqueIDFromItem((void*)this); // 0을 리턴하면 효과가 없는경우다...
		::CItemSpecialEffectCreateDummy((void*)this, m_pMesh );		// 무기의경우 궤적이 있기때문에 미리 준비를 해줘야된다.
		if( IsEqualItemType( BOT_GROUND_ITEM ) )				//그라운드 아이템일경우 좀 표면에서 띄워놓는다.
		{
			Vector3f pos;
			GetPosition(pos);
			pos[1]+=2;
			SetPosition(pos,1);
		}
	}
	if( m_dwEFUniqueID )	// 유닉크 아이디가 있다.(아템에 들러붙는 효과가 있다.) 
		::CItemSpecialEffectFrameMove((void*)this,m_dwEFUniqueID);
}

void
CMeshItem::DeleteItemEffect( void )
{
	if( m_dwEFUniqueID )	//효과가 있다면 지운다.(장언일 )
	{
//		m_bCheckExistEF = FALSE;
		SetDisapearTimeMagicEffect(m_dwEFUniqueID,0);
		m_dwEFUniqueID=0;
	}
}

BYTE
CMeshItem::GetDummyIndex( DWORD pi_dwArrayId )
{
	if( pi_dwArrayId >= MAX_EFFECT_DUMMY )
		return 0xFF;

	return m_byDummy[pi_dwArrayId];
}

void
CMeshItem::SetDummyIndex( DWORD pi_dwArrayId, DWORD pi_dwId )
{
	if( pi_dwArrayId >= MAX_EFFECT_DUMMY )
		return;

	m_byDummy[pi_dwArrayId] = pi_dwId;
}

// J.U.I < end > ---------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CEquipItem::CEquipItem()
{
	m_pMesh			= NULL;

	m_byFadeMode	= FRM_NONE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CEquipItem::Render( void )
{
	ProcessFading();

	if( m_byFadeMode == FRM_IN_END )
		return TRUE;

	return CMeshItem::Render();
}

//==============================================================================

void
CEquipItem::SetFadeMode( BYTE pi_byMode, BYTE pi_byMaxFrame )
{
	m_byFadeMode		= pi_byMode;
	m_byMaxFadeFrame	= pi_byMaxFrame;

	if( m_byFadeMode == FRM_IN )
		m_byCurFadeFrame = m_byMaxFadeFrame;
	else
		m_byCurFadeFrame = 0;
}

// Fade In / Out 중인가?
BOOL
CEquipItem::IsFading( void )
{
	if( m_byFadeMode != FRM_NONE )
	{
		if( m_byFadeMode == FRM_IN && m_byCurFadeFrame > 0 )
		{
			return TRUE;
		}
		else if( m_byFadeMode == FRM_OUT && m_byCurFadeFrame < m_byMaxFadeFrame )
		{
			return TRUE;
		}
	}

	return FALSE;
}

// Fade In / Out 을 시작했는가?
BOOL
CEquipItem::IsStartFading( void )
{
	if( m_byFadeMode != FRM_NONE )
	{
		if( m_byFadeMode == FRM_IN && m_byCurFadeFrame == m_byMaxFadeFrame )
		{
			return TRUE;
		}
		else if( m_byFadeMode == FRM_OUT && m_byCurFadeFrame == 0 )
		{
			return TRUE;
		}
	}

	return FALSE;
}

// 아이템의 fade 처리
void
CEquipItem::ProcessFading( void )
{
	if( ( m_byFadeMode == FRM_IN ) || ( m_byFadeMode == FRM_OUT ) )
	{
		if( m_byFadeMode == FRM_IN && m_byCurFadeFrame > 0 )
		{
			--m_byCurFadeFrame;

			if( m_byCurFadeFrame <= 0 )
			{
				m_byCurFadeFrame	= 0;
				Disable( ROSF_RENDER_ALPHA );
				m_fAlphaDensity		= 0.0f;
				m_byFadeMode		= FRM_IN_END;
				return;
			}
		}
		else if( m_byFadeMode == FRM_OUT && m_byCurFadeFrame < m_byMaxFadeFrame )
		{
			++m_byCurFadeFrame;
			if( m_byCurFadeFrame >= m_byMaxFadeFrame )
			{
				m_byCurFadeFrame	= m_byMaxFadeFrame;
				Disable( ROSF_RENDER_ALPHA );
				m_byFadeMode		= FRM_OUT_END;
				m_fAlphaDensity		= 1.0f;
				return;
			}
		}

		m_fAlphaDensity = ( (float)m_byCurFadeFrame / (float)m_byMaxFadeFrame );
		Enable( ROSF_RENDER_ALPHA );
	}
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CGroundItem::CGroundItem()
{
	m_dwOwnerIndex			= ID_INVALID;
	m_dwDroperIndex[0]		= ID_INVALID;
	m_dwDroperIndex[1]		= ID_INVALID;
	m_dwCreateTime			= 0;

	m_byQuantity			= 0;

	m_bIsPortalGotoDungeon	= FALSE;
}

void
CGroundItem::SetBone( DWORD pi_dwItemDTIndex, BYTE pi_byRLIType )
{
	if( m_bIsPortalGotoDungeon )
		CMeshItem::SetBone( ( pi_dwItemDTIndex & 0xFFFFFF00 ) | 0x000000FE );
	else
		CMeshItem::SetBone( pi_dwItemDTIndex | 0x000000FF );
}

void
CGroundItem::SetMesh( DWORD pi_dwItemDTIndex, BYTE pi_byRLIType )
{
	if( m_bIsPortalGotoDungeon )
		CMeshItem::SetMesh( ( pi_dwItemDTIndex & 0xFFFFFF00 ) | 0x000000FE );
	else
		CMeshItem::SetMesh( pi_dwItemDTIndex | 0x000000FF );
}

void
CGroundItem::SetOwner( BYTE pi_byLootState, DWORD pi_dwOwnerIndex )
{
	//0: normal(지정자만이 먹을수잇는상태) 
	//1: open(누구든지 먹을수 있는 상태) 
	//2: hurry(소멸임박,깜빡임)

	m_dwOwnerIndex = pi_dwOwnerIndex;

	// 누구든지 루팅할 수 있음
	if( pi_byLootState == 1 )
	{
		m_dwOwnerIndex = ID_INVALID;
	}
	// 얼마 후 없어짐
	else if( pi_byLootState == 2 )
	{
		SetBlink( TRUE );
	}
	// 지정자만이 루팅할 수 있음
	else
	{
		// 지정자가 아바타가 아니더라도 파티원의 것이면 루팅이 가능하다.
		if( _GetAvatar()->GetIndex() != pi_dwOwnerIndex )
		{
			if( _GetPartyBoard()->IsPartyMember( pi_dwOwnerIndex ) )
				m_dwOwnerIndex = ID_INVALID;
		}
	}
}

void
CGroundItem::SetDroper( DWORD pi_dwCharTypeID, DWORD pi_dwIndex )
{
	m_dwDroperIndex[0] = pi_dwCharTypeID;
	m_dwDroperIndex[1] = pi_dwIndex;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "../UserInterface/GameWindow/SkillForce/GUISkillWindow.h"

CSFItem::CSFItem()
{
	Init();
}

CSFItem::~CSFItem()
{
}

void
CSFItem::Init( void )
{
	m_byItemType		= BOT_SF;
	m_bySFType			= CAT_SKILL;

	m_dwSuccessCount	= 0;
	m_byLevel			= 0;
	m_fExpByPercent		= 0.0f;
	m_pData				= NULL;
	m_uiIconBoard		= NULL;

	m_bIsHave			= FALSE;
}

void
CSFItem::Clear( void )
{
	Init();
}

BOOL
CSFItem::Create( BYTE pi_bySFType, DWORD pi_dwDTIndex )
{
	Clear();

	if( pi_bySFType == CAT_SKILL )
	{
		m_pData			= _GetCharDataMgr()->GetSkill( pi_dwDTIndex );
		m_dwIndex		= pi_dwDTIndex;					// 스킬은 서버에서 인덱스를 부여받지 않으므로 DT Index와 동일하게 설정한다.
		m_dwServerIndex	= pi_dwDTIndex;
	}
	else if( pi_bySFType == CAT_CLASS_SKILL )
	{
		m_pData			= _GetCharDataMgr()->GetClassSkill( pi_dwDTIndex );
		m_dwIndex		= pi_dwDTIndex | 0xF0000000;
		m_dwServerIndex	= pi_dwDTIndex;
		m_byLevel		= 1;
	}
	else
	{
		m_pData			= _GetCharDataMgr()->GetForce( pi_dwDTIndex );
		m_dwIndex		= pi_dwDTIndex | 0xF0000000;
		m_dwServerIndex	= pi_dwDTIndex | 0xF0000000;
	}

	if( !m_pData )
		return FALSE;

	m_bySFType			= pi_bySFType;

	SetName( m_pData->pName_Kor );

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CBoardObject *
CSFItem::New( void )
{
	return ( dynamic_cast< CBoardObject * >( new CSFItem ) );
}

CBoardObject *
CSFItem::Clone( void )
{
	return ( dynamic_cast< CBoardObject * >( new CSFItem( *this ) ) );
}

BOOL
CSFItem::IsEqual( CBoardObject * pi_pTarObject )
{
	if( !CBoardObject::IsEqual( pi_pTarObject ) )
		return FALSE;

	return IsEqualSFType( static_cast< CSFItem * >( pi_pTarObject )->GetSFType() );
}

BOOL
CSFItem::IsSameKind( CSFItem * pi_pSFItem )
{
	if( !pi_pSFItem )
		return FALSE;

	if( IsEqualSFType( pi_pSFItem->GetSFType() ) && ( GetDTIndex() == pi_pSFItem->GetDTIndex() ) )
		return TRUE;

	return FALSE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CSFItem::SetIndex( DWORD pi_dwIndex )
{
	CObject::SetIndex( pi_dwIndex );

	if( m_uiIconBoard )
		m_uiIconBoard->SetIndex( pi_dwIndex );
}

void
CSFItem::SetName( char * pi_pName )
{
	CObject::SetName( pi_pName );

	if( m_uiIconBoard )
		m_uiIconBoard->SetItemName( pi_pName );
}

void
CSFItem::SetData( SF_DATA * pi_pData )
{
	m_pData = pi_pData;	

	if( m_pData )
	{
		SetName( m_pData->pName_Kor );		
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CSFItem::CalculateSuccessCount( DWORD pi_dwCount )
{
	if( !m_pData )
		return;

	static const float MASTERY_CONSTANT[4] = { 200.0f, 200.0f, 200.0f, 200.0f };

	if( pi_dwCount != ID_INVALID )
		m_dwSuccessCount = pi_dwCount;

	//--------------------------------------------------------------------------
	BYTE byLevel;
	float fExpByPercent;

	byLevel = ( BYTE )( sqrt( (float)( m_dwSuccessCount + 1 ) / MASTERY_CONSTANT[m_pData->byStep] ) + 0.9999f );
	if( byLevel >= 7 )
	{
		SetLevel( 7 );
		SetExpByPercent( 99.99f );
		
		return;
	}	

	//----------------------------------------------------------------------

	DWORD	l_dwSC_AfterLevelUp;		// 레벨업 이후의 누적 횟수
	DWORD	l_dwNeedSC_ForLevelUp;		// 다음 레벨 까지 필요한 성공 횟수
	l_dwSC_AfterLevelUp		= m_dwSuccessCount - ( ( byLevel - 1 ) * ( byLevel - 1 ) * MASTERY_CONSTANT[m_pData->byStep] );
	l_dwNeedSC_ForLevelUp	= ( byLevel * byLevel * MASTERY_CONSTANT[m_pData->byStep] ) - ( ( byLevel - 1 ) * ( byLevel - 1 ) * MASTERY_CONSTANT[m_pData->byStep] ) + 1;

	fExpByPercent = ( (float)l_dwSC_AfterLevelUp / (float)l_dwNeedSC_ForLevelUp ) * 100.0f;
	if( fExpByPercent >= 99.99f )
		fExpByPercent = 99.99f;

	SetLevel( byLevel );
	SetExpByPercent( fExpByPercent );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CSFItem::SetLevel( BYTE pi_byLevel )
{
	if( pi_byLevel >= MAX_SF_LEVEL ) 
		m_byLevel = MAX_SF_LEVEL;
	else
		m_byLevel = pi_byLevel;

	if( m_uiIconBoard )
		m_uiIconBoard->SetLevel( m_byLevel );
}

void		
CSFItem::SetExpByPercent( float pi_fExp )
{
	m_fExpByPercent = pi_fExp; 

	if( m_uiIconBoard )
		m_uiIconBoard->SetExp( m_fExpByPercent );
}

BOOL
CSFItem::IsEqualTargetType( BYTE pi_byTargetType )
{
	if( !m_pData )
		return FALSE;
	if( pi_byTargetType >= MAX_SF_TARGET_TYPE )
		return FALSE;

	return m_pData->byTargetType[pi_byTargetType];
}

BYTE
CSFItem::GetUsingType( BYTE pi_byOrderNo )
{
	if( !m_pData || ( pi_byOrderNo >= MAX_SF_EFFECT ) )
		return 0xFF;

	return m_pData->stEffect[pi_byOrderNo].byUsingType;
}

BOOL
CSFItem::IsAttackable( void )
{
	return IsHaveUsingType( SFAT_ATTACK );
}

// 스킬/포스의 해당 타입을 가지고 있는가?
BOOL
CSFItem::IsHaveUsingType( BYTE pi_byUsingType )
{
	if( !m_pData )
		return FALSE;

	for( int i = 0; i < MAX_SF_EFFECT; ++i )
	{
		if( m_pData->stEffect[i].byUsingType == pi_byUsingType )
			return TRUE;
	}

	return FALSE;
}

BYTE
CSFItem::GetContinuanceUsingType( void )
{
	for( int i = 0; i < MAX_SF_EFFECT; ++i )
	{
		if( m_pData->stEffect[i].byUsingType == SFAT_SUPPORT_CONTINUANCE_LOSS ||
			m_pData->stEffect[i].byUsingType == SFAT_SUPPORT_CONTINUANCE_PROFIT )
			return m_pData->stEffect[i].byUsingType;
	}

	return 0xFF;
}

BYTE
CSFItem::GetRangeType( void )
{
	if( !IsAttackable() )
		return 0;

	// 공격은 무조건 0번에 있음
	return ( m_pData->stEffect[0].byEffectType[m_byLevel-1] );
}

// 스킬의 공격 거리는 일반 공격처럼 무기에 따라 다르므로 데이터를 가지고 있지 않음
float
CSFItem::GetAttackRange( void )
{
	if( m_bySFType == CAT_FORCE && m_pData )
		return ( static_cast< FORCE_DATA * >( m_pData )->fDistance );

	return 0.0f;
}

BYTE
CSFItem::GetEffectType( BYTE pi_byOrderNo )
{
	if( m_pData && pi_byOrderNo < MAX_SF_EFFECT )
	{
		if( m_pData->stEffect[pi_byOrderNo].byUsingType != 0xFF )
			return m_pData->stEffect[pi_byOrderNo].byEffectType[m_byLevel-1];
	}

	return 0xFF;
}

BYTE
CSFItem::GetEffectUnit( BYTE pi_byOrderNo )
{
	if( m_pData && pi_byOrderNo < MAX_SF_EFFECT )
	{
		if( m_pData->stEffect[pi_byOrderNo].byUsingType != 0xFF )
			return m_pData->stEffect[pi_byOrderNo].byEffectUnit;
	}

	return 0xFF;
}

float
CSFItem::GetEffectValue( BYTE pi_byOrderNo )
{
	if( m_pData && pi_byOrderNo < MAX_SF_EFFECT )
	{
		if( m_pData->stEffect[pi_byOrderNo].byUsingType != 0xFF )
			return m_pData->stEffect[pi_byOrderNo].fEffectValue[m_byLevel-1];
	}

	return 0.0f;
}

DWORD
CSFItem::GetEffectTime( void )
{
	if( !m_pData )
		return 0;

	if( !m_byLevel )
		return 0;

	return m_pData->dwEffectTime[m_byLevel-1];
}

BOOL		
CSFItem::IsUsableRace( BYTE pi_byRace )
{
	if( !m_pData )
		return FALSE;

	return m_pData->byUsableRace[pi_byRace];
}

BOOL
CSFItem::IsUsableClassType( BYTE pi_byClassType )
{
	if( !m_pData )
		return FALSE;

	// 0xFF면 무조건 사용 가능
	if( m_pData->byUsableClassType == 0xFF )
		return TRUE;

	return ( m_pData->byUsableClassType == pi_byClassType );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CSFItem::SetIconBoard( CGUISkFcUnit * pi_pIconBoard )
{
	m_uiIconBoard = pi_pIconBoard;

	if( pi_pIconBoard )
		m_uiIcon = pi_pIconBoard->GetItem();
	else
		m_uiIcon = NULL;
}

BYTE
CSFItem::GetUIClassType( void )
{
	if( !m_pData )
		return ID_INVALID;

	if( m_bySFType == CAT_SKILL )
	{
		switch( m_pData->byType )
		{
			case SCT_MELEE		: return SCT_UI_MELEE;
			case SCT_BOW		:
			case SCT_FIREARM	:
			case SCT_THROW		:
			case SCT_LAUNCHER	: return SCT_UI_MISSILE;
			case SCT_KNOWLEDGE	: return SCT_UI_KNOWLEDGE;
		}
	}
	else if( m_bySFType == CAT_CLASS_SKILL )
	{
		return SCT_UI_KNOWLEDGE;
	}
	else
	{
		switch( m_pData->byType )
		{
			case FCT_DARK	:
			case FCT_HOLY	: return FCT_UI_HOLYnDARK;
			case FCT_FIRE	: return FCT_UI_FIRE;
			case FCT_AQUA	: return FCT_UI_AQUA;
			case FCT_TERRA	: return FCT_UI_TERRA;
			case FCT_WIND	: return FCT_UI_WIND;
		}
	}
	
	return ID_INVALID;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CSFItem::SetState( BYTE pi_byState )
{
	if( m_uiIconBoard )
		m_uiIconBoard->SetUnitState( ( CGUISkFcUnit::UnitState )pi_byState );
}

BOOL		
CSFItem::IsEqualState( BYTE pi_byState )
{
	if( m_uiIconBoard )
		return ( ( BYTE )m_uiIconBoard->GetUnitState() == pi_byState );

	return FALSE;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "../UserInterface/GameWindow/Recall/GUIRecallWindow.h"

CAnimusItem::CAnimusItem()
{
	Init();
}

CAnimusItem::~CAnimusItem()
{
}

void
CAnimusItem::Init( void )
{
	m_byItemType	= BOT_ANIMUS;

	m_pCharData		= NULL;
	m_uiIconBoard	= NULL;

	m_dwExp			= 0;
	m_dwHP			= 0;
	m_dwFP			= 0;
}

void
CAnimusItem::Clear( void )
{
	CBaseItem::Clear();

	SetHP( 0 );
	SetFP( 0 );
	SetExp( 0 );
	SetState( CAnimusItem::AIS_NOT_EXIST );

	// max 값 세팅을 위해 ui의 함수를 직접 호출한다.
	if( m_uiIconBoard )
	{
		m_uiIconBoard->SetLevel( 1 );
		m_uiIconBoard->SetExp( 0 );
		m_uiIconBoard->SetHP( 0, 0 );
		m_uiIconBoard->SetFP( 0, 0 );
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CBoardObject *
CAnimusItem::New( void )
{
	return ( dynamic_cast< CBoardObject * >( new CAnimusItem ) );
}

CBoardObject *
CAnimusItem::Clone( void )
{
	return ( dynamic_cast< CBoardObject * >( new CAnimusItem( *this ) ) );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CAnimusItem::SetIndex( DWORD pi_dwIndex )
{
	CObject::SetIndex( pi_dwIndex );

	if( m_uiIconBoard )
		m_uiIconBoard->SetIndex( pi_dwIndex );
}

void
CAnimusItem::SetName( char * pi_pName )
{
	CObject::SetName( pi_pName );

	if( m_uiIconBoard )
		m_uiIconBoard->SetMobName( pi_pName );
}

void
CAnimusItem::SetData( ITEM_DATA * pi_pData )
{
	CBaseItem::SetData( pi_pData );

	if( m_pData )
	{
		SetName( m_pData->pName );
		if( m_uiIconBoard )
			m_uiIconBoard->SetMobName( m_pData->pName );
	}
}

void
CAnimusItem::SetCharData( CAnimusData * pi_pData )
{
	m_pCharData = pi_pData;

	if( m_pCharData )
	{
		if( m_uiIconBoard )
			m_uiIconBoard->SetLevel( m_pCharData->m_byLevel );
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CAnimusItem::SetIconBoard( CGUIRecallUnit * pi_pIconBoard )
{
	m_uiIconBoard = pi_pIconBoard;

	if( pi_pIconBoard )
		m_uiIcon = pi_pIconBoard->GetItem();
	else
		m_uiIcon = NULL;
}

void
CAnimusItem::SetState( BYTE pi_byState )
{
	if( m_uiIconBoard )
		m_uiIconBoard->SetUnitState( ( CGUIRecallUnit::UnitState )pi_byState );
}

BOOL
CAnimusItem::IsEqualState( BYTE pi_byState )
{
	if( m_uiIconBoard )
		return ( ( BYTE )m_uiIconBoard->GetUnitState() == pi_byState );

	return FALSE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BYTE
CAnimusItem::GetGrade( void )
{
	BYTE byLevel = GetLevel();

	if( byLevel <= 20 )
		return AGT_RECRUIT;
	else if( byLevel <= 40 )
		return AGT_VETERAN;
	else if( byLevel <= 50 )
		return AGT_MASTER;

	return AGT_AMI;
}

void
CAnimusItem::SetExp( DWORD pi_dwExp )
{
	m_dwExp		= pi_dwExp;
	m_pCharData	= _GetCharDataMgr()->GetAnimusByExp( GetDTIndex(), m_dwExp );

	if( m_uiIconBoard && m_pCharData )
	{
		m_uiIconBoard->SetExp( (float)GetExpByLevel() / (float)GetMaxExpByLevel() * 100 );
		m_uiIconBoard->SetLevel( m_pCharData->m_byLevel );
	}
}

// 현재 레벨의 상대적인 exp
DWORD
CAnimusItem::GetExpByLevel( void )
{
	if( m_pCharData )
	{
		DWORD dwMinExp = 0;
		CAnimusData * pPrevData	= _GetCharDataMgr()->GetAnimusByLevel( GetDTIndex(), m_pCharData->m_byLevel - 1 );
		if( pPrevData )
			dwMinExp = pPrevData->m_dwExp;
		
		return m_dwExp - dwMinExp;
	}

	return 0;
}

DWORD
CAnimusItem::GetMaxExpByLevel( void )
{
	if( m_pCharData )
	{
		DWORD dwMinExp = 0;
		CAnimusData * pPrevData	= _GetCharDataMgr()->GetAnimusByLevel( GetDTIndex(), m_pCharData->m_byLevel - 1 );
		if( pPrevData )
			dwMinExp = pPrevData->m_dwExp;
		
		return m_pCharData->m_dwExp - dwMinExp;
	}

	return 0;
}

void
CAnimusItem::SetHP( DWORD pi_dwHP )
{
	if( !m_pCharData )
		return;

	m_dwHP = pi_dwHP;
	if( m_dwHP > m_pCharData->m_dwMaxHP )
		m_dwHP = m_pCharData->m_dwMaxHP;

	if( m_uiIconBoard )
		m_uiIconBoard->SetHP( m_dwHP, m_pCharData->m_dwMaxHP );
}

void
CAnimusItem::RecoverHP( void )
{
	if( !m_pCharData )
		return;

	SetHP( m_dwHP + m_pCharData->m_dwHPRecoveryPoint );
}

void
CAnimusItem::SetFP( DWORD pi_dwFP )
{
	if( !m_pCharData )
		return;

	m_dwFP = pi_dwFP;
	if( m_dwFP > m_pCharData->m_dwMaxFP )
		m_dwFP = m_pCharData->m_dwMaxFP;

	if( m_uiIconBoard && m_pCharData )
		m_uiIconBoard->SetFP( m_dwFP, m_pCharData->m_dwMaxFP );
}

void
CAnimusItem::RecoverFP( void )
{
	if( !m_pCharData )
		return;

	SetFP( m_dwFP + m_pCharData->m_dwFPRecoveryPoint );
}

DWORD
CAnimusItem::GetMinAttPower( void )
{
	if( m_pCharData )
		return m_pCharData->m_dwAttackPower * 0.75;

	return 0;
}

DWORD
CAnimusItem::GetMaxAttPower( void )
{
	if( m_pCharData )
		return m_pCharData->m_dwAttackPower + ( m_pCharData->m_dwAttackPower - GetMinAttPower() );

	return 0;
}

BYTE
CAnimusItem::GetResist( BYTE pi_byAttributeType )
{
	if( pi_byAttributeType >= MAX_ITEM_ATTRIBUTE )
		return 0;

	if( !m_pCharData )
		return 0;

	return m_pCharData->m_byResist[pi_byAttributeType];
}

// 소환에 필요한 FP
DWORD
CAnimusItem::GetNeedfulFP( void )
{
	static const DWORD SUMMON_NEEDED_FP = 60;
	
	return SUMMON_NEEDED_FP;
}

// 소환에 필요한 숙련도
DWORD
CAnimusItem::GetNeedfulExpertness( void )
{
	// 소환가능숙련도=(애니머스레벨-12)^2/20
	return ( DWORD )( pow( ( GetLevel() - 12 ), 2 ) / 20.0f );
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#include "../GameObject/Board/Common/CBoardManager.h"

CUnitPartsItem::CUnitPartsItem()
{
	Init();
}

CUnitPartsItem::~CUnitPartsItem()
{
}

void
CUnitPartsItem::Init( void )
{
	m_byItemType	= BOT_UNIT_PARTS;

	m_dwDurability	= 0;
	m_bIsHave		= FALSE;
}

void
CUnitPartsItem::Clear( void )
{
	CBaseItem::Clear();

	Init();
}

BOOL
CUnitPartsItem::Create( BYTE pi_byItemKind, DWORD pi_dwDTIndex )
{
	if( !CBaseItem::Create( pi_byItemKind, pi_dwDTIndex ) )
		return FALSE;

	m_dwDurability = GetOrgMaxDurability();

	SetIndex( pi_dwDTIndex | 0xF0000000 );
	SetServerIndex( pi_dwDTIndex | 0xF0000000 );
	SetName( m_pData->pName );

	return TRUE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CBoardObject *
CUnitPartsItem::New( void )
{
	return ( dynamic_cast< CBoardObject * >( new CUnitPartsItem ) );
}

CBoardObject *
CUnitPartsItem::Clone( void )
{
	return ( dynamic_cast< CBoardObject * >( new CUnitPartsItem( *this ) ) );
}

void
CUnitPartsItem::Copy( CObject * pi_pOrgObject )
{
	if( pi_pOrgObject )
	{
		CGUIItem * 	uiIconBackup = m_uiIcon;
		memcpy( this, static_cast< CUnitPartsItem * >( pi_pOrgObject ), sizeof( CUnitPartsItem ) );
		m_uiIcon = uiIconBackup;
	}
}

// 디폴트 아이템인가? ( 빈 백팩, 빈 탄창 등 )
BOOL
CUnitPartsItem::IsDefaultParts( void )
{
	// 메쉬 아이디의 하위 2바이트가 0xFF면 빈 파츠임. ( 백팩이나 탄창에서 쓰인다. )
	DWORD dwMeshID = GetMeshID();
	if( ( dwMeshID != ID_INVALID ) && ( ( dwMeshID & 0xFF ) == 0xFF ) )
		return TRUE;

	return FALSE;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

DWORD
CUnitPartsItem::GetMaxStrikingPower( void )
{
	if( IsEqualItemKind( IEPT_UNIT_ARMS ) || IsEqualItemKind( IEPT_UNIT_SHOULDER ) )
	{
		UNIT_WEAPON_DATA * pWeaponData = static_cast< UNIT_WEAPON_DATA * >( m_pData );
		return pWeaponData->dwMaxStrikingPower;
	}

	return 0;
}

DWORD
CUnitPartsItem::GetMinStrikingPower( void )
{
	if( IsEqualItemKind( IEPT_UNIT_ARMS ) || IsEqualItemKind( IEPT_UNIT_SHOULDER ) )
	{
		UNIT_WEAPON_DATA * pWeaponData = static_cast< UNIT_WEAPON_DATA * >( m_pData );
		return pWeaponData->dwMinStrikingPower;
	}

	return 0;
}

DWORD
CUnitPartsItem::GetDefencePower( void )
{
	UNIT_PART_DATA * pPartData = static_cast< UNIT_PART_DATA * >( m_pData );
	if( pPartData )
		return pPartData->dwDefencePower;

	return 0;
}

WORD
CUnitPartsItem::GetResist( BYTE pi_byAttributeType )
{
	if( IsEqualItemKind( IEPT_UNIT ) )
		return 0;

	UNIT_PART_DATA * pPartData = static_cast< UNIT_PART_DATA * >( m_pData );
	if( pPartData && ( pi_byAttributeType < MAX_ITEM_ATTRIBUTE ) )
		return pPartData->wResist[ pi_byAttributeType ];

	return 0;
}

float
CUnitPartsItem::GetMoveSpeed( void )
{
	UNIT_PART_DATA * pPartData = static_cast< UNIT_PART_DATA * >( m_pData );
	if( pPartData )
		return pPartData->nMoveSpeed;

	return 0.0f;
}

BYTE
CUnitPartsItem::GetAttackExpertness( void )
{
	if( IsEqualItemKind( IEPT_UNIT_ARMS ) || IsEqualItemKind( IEPT_UNIT_SHOULDER ) )
	{
		UNIT_WEAPON_DATA * pWeaponData = static_cast< UNIT_WEAPON_DATA * >( m_pData );
		return pWeaponData->byAttackExpertness;
	}

	return 0;
}

// 유닛의 방어 숙련도는 유닛의 머리와 상체부분만이 사용된다.
BYTE
CUnitPartsItem::GetDefenceExpertness( void )
{
	if( IsEqualItemKind( IEPT_UNIT_HELMET ) || IsEqualItemKind( IEPT_UNIT_UPPER_PART ) )
	{
		UNIT_PART_DATA * pPartData = static_cast< UNIT_WEAPON_DATA * >( m_pData );
		return pPartData->byDefenceExpertness;
	}

	return 0;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

float
CUnitPartsItem::GetDurabilityRate( void )
{
	float	fDurabilityRate = 1.0f;
	DWORD	dwOrgMaxDurability = GetOrgMaxDurability();

	if( ( m_dwDurability != 0 ) && ( dwOrgMaxDurability != 0 ) )
		fDurabilityRate = (float)m_dwDurability / (float)dwOrgMaxDurability;

	return fDurabilityRate;
}

BYTE
CUnitPartsItem::GetBulletSocketNum( void )
{
	if( IsEqualItemKind( IEPT_UNIT_ARMS ) || IsEqualItemKind( IEPT_UNIT_SHOULDER ) )
	{
		return static_cast< UNIT_WEAPON_DATA * >( m_pData )->byMaxBulletSocket;
	}
	else if( IsEqualItemKind( IEPT_UNIT_BACKPACK ) )
	{
		return static_cast< UNIT_BACKPACK_DATA * >( m_pData )->byMaxBulletSocket;
	}

	return 0;
}

BYTE
CUnitPartsItem::GetBoosterSpeed( void )
{
	if( IsEqualItemKind( IEPT_UNIT_BACKPACK ) )
	{
		UNIT_BACKPACK_DATA * pBackpackData = static_cast< UNIT_BACKPACK_DATA * >( m_pData );
		return pBackpackData->byBoosterSpeed;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

CUnitItem::CUnitItem()
{
	m_byItemType			= BOT_UNIT_FRAME;

	m_byBackpackBulletNum	= 0;
	m_pWeaponParts			= &m_listPart[IEPT_UNIT_ARMS-IEPT_UNIT_HELMET];
	m_dwTractionDebt		= 0;

	m_uiTuningWindow		= NULL;
}

CUnitItem::~CUnitItem()
{
}

CBoardObject *
CUnitItem::New( void )
{
	return ( dynamic_cast< CBoardObject * >( new CUnitItem ) );
}

CBoardObject *
CUnitItem::Clone( void )
{
	return ( dynamic_cast< CBoardObject * >( new CUnitItem( *this ) ) );
}

void
CUnitItem::Copy( CObject * pi_pOrgObject )
{
	if( pi_pOrgObject )
	{
		CGUIItem * uiIconBackup = m_uiIcon;
		memcpy( this, static_cast< CUnitItem * >( pi_pOrgObject ), sizeof( CUnitItem ) );
		m_uiIcon = uiIconBackup;
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

BOOL
CUnitItem::CreateByFrameType( BYTE pi_byUnitFrameType )
{
	if( !AddFrame( pi_byUnitFrameType ) )
		return FALSE;

	UNIT_FRAME_DATA *	pFrameData = static_cast< UNIT_FRAME_DATA * >( m_pData );
	CUnitPartsItem *	pParts = NULL;

	for( int i = 0; i < MAX_UNIT_PART_TYPE; ++i )
	{
		pParts = AddParts( IEPT_UNIT_HELMET + i, pFrameData->dwBasePartID[i] );
		if( pParts )
			pParts->SetHave( TRUE );
	}

	SetHave( TRUE );

	return TRUE;
}

BOOL
CUnitItem::CreateByUnitItem( CUnitItem * pi_pUnitItem )
{
	if( !pi_pUnitItem || pi_pUnitItem->IsEmpty() )
		return FALSE;

	Clear();

	// unit frame
	if( !CreateFrameByUnitItem( pi_pUnitItem ) )
		return FALSE;

	// unit parts
	if( !CreateAllPartsByUnitItem( pi_pUnitItem ) )
		return FALSE;

	// bullets
	if( !CreateAllBulletsByUnitItem( pi_pUnitItem ) )
		return FALSE;

	// backpack bullets
	if( !CreateAllBackpackBulletsByUnitItem( pi_pUnitItem ) )
		return FALSE;

	return TRUE;
}

BOOL
CUnitItem::CreateFrameByUnitItem( CUnitItem * pi_pUnitItem )
{
	if( !pi_pUnitItem || pi_pUnitItem->IsEmpty() )
		return FALSE;

	if( !AddFrame( pi_pUnitItem ) )
		return FALSE;

	return TRUE;
}

BOOL
CUnitItem::CreateAllPartsByUnitItem( CUnitItem * pi_pUnitItem )
{
	if( !pi_pUnitItem || pi_pUnitItem->IsEmpty() )
		return FALSE;

	for( int i = 0; i < MAX_UNIT_PART_TYPE; ++i )
	{
		RemoveParts( IEPT_UNIT_HELMET + i );

		if( pi_pUnitItem->m_listPart[i].IsEmpty() )
			continue;

		AddParts( IEPT_UNIT_HELMET + i, &pi_pUnitItem->m_listPart[i] );
	}

	return TRUE;
}

BOOL
CUnitItem::CreateAllBulletsByUnitItem( CUnitItem * pi_pUnitItem )
{
	if( !pi_pUnitItem || pi_pUnitItem->IsEmpty() )
		return FALSE;

	for( int i = 0; i < MAX_UNIT_BULLET_TYPE; ++i )
	{
		RemoveBullet( IEPT_UNIT_ARMS + i );

		if( pi_pUnitItem->m_listBullet[i].IsEmpty() )
			continue;

		AddBullet( IEPT_UNIT_ARMS + i, &pi_pUnitItem->m_listBullet[i] );
	}

	return TRUE;
}

BOOL
CUnitItem::CreateAllBackpackBulletsByUnitItem( CUnitItem * pi_pUnitItem )
{
	if( !pi_pUnitItem || pi_pUnitItem->IsEmpty() )
		return FALSE;

	for( int i = 0; i < MAX_BACKPACK_BULLET_NUM; ++i )
	{
		RemoveBackpackBullet( i );

		if( pi_pUnitItem->m_listBackpackBullet[i].IsEmpty() )
			continue;

		AddBackpackBullet( i, &pi_pUnitItem->m_listBackpackBullet[i] );
	}

	return TRUE;
}

void
CUnitItem::Clear( void )
{
	for( int i = 0; i < MAX_BACKPACK_BULLET_NUM; ++i )
		RemoveBackpackBullet( i );

	for( i = 0; i < MAX_UNIT_BULLET_TYPE; ++i )
		RemoveBullet( IEPT_UNIT_ARMS + i );

	for( i = 0; i < MAX_UNIT_PART_TYPE; ++i )
		RemoveParts( IEPT_UNIT_HELMET + i );

	RemoveFrame();
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CUnitItem *
CUnitItem::AddFrame( DWORD pi_dwDTIndex )
{
	if( Create( IEPT_UNIT, pi_dwDTIndex ) )
	{
		SetFrameUI( TRUE );
		return this;
	}

	return NULL;
}

CUnitItem *
CUnitItem::AddFrame( CUnitItem * pi_pUnitItem )
{
	if( !pi_pUnitItem )
		return NULL;

	if( Create( IEPT_UNIT, pi_pUnitItem->GetDTIndex() ) )
	{
		CGUIUnitTuningWindow * pBackupTuningWindow = m_uiTuningWindow;

		Copy( pi_pUnitItem );

		m_uiTuningWindow = pBackupTuningWindow;

		SetFrameUI( TRUE );

		return this;
	}

	return NULL;
}

void
CUnitItem::RemoveFrame( void )
{
	SetEmpty();
	SetFrameUI( FALSE );
}

void
CUnitItem::SetFrameUI( BOOL pi_bIsAdd )
{
	if( !m_uiTuningWindow )
		return;

	if( pi_bIsAdd )
	{
		m_uiTuningWindow->TakeInMainFrameKey( GetMeshID(), GetIconID() );
		m_uiTuningWindow->SetMainFrameName( GetName() );
		m_uiTuningWindow->SetDefPower( GetDefencePower() );
		m_uiTuningWindow->SetDurability( GetDurabilityRate() * 100 );
		//m_uiTuningWindow->SetMovePower( GetUnitSpeed() );
		char pUnitSpeed[32];
		GetMoveSpeedString( pUnitSpeed );
		m_uiTuningWindow->SetMovePower( pUnitSpeed );

		SetIcon( m_uiTuningWindow->GetMainFrameKey() );
	}
	else
	{
		m_uiTuningWindow->TakeOutMainFrameKey();
		m_uiTuningWindow->SetMainFrameName( "" );
		m_uiTuningWindow->SetDefPower( 0 );
		m_uiTuningWindow->SetDurability( 0 );
		m_uiTuningWindow->SetMovePower( 0 );
	}
}

//==============================================================================

CUnitPartsItem *
CUnitItem::AddParts( BYTE pi_byPartType, DWORD pi_dwDTIndex )
{
	BYTE byOrderNo = pi_byPartType - IEPT_UNIT_HELMET;

	if( byOrderNo >= MAX_UNIT_PART_TYPE )
		return NULL;

	CUnitPartsItem * pParts = &m_listPart[byOrderNo];

	if( pParts->Create( pi_byPartType, pi_dwDTIndex ) )
	{
		ResetUnitDefenceAndSpeed();
		SetPartsUI( pParts, TRUE );
		return pParts;
	}

	// arms, shoulder, backpack 은 탄창 부분을 refresh 시켜야한다.
	RefreshBulletUI( pi_byPartType, TRUE );

	return NULL;
}

CUnitPartsItem *
CUnitItem::AddParts( BYTE pi_byPartType, CUnitPartsItem * pi_pOrgPartsItem )
{
	BYTE byOrderNo = pi_byPartType - IEPT_UNIT_HELMET;

	if( ( byOrderNo >= MAX_UNIT_PART_TYPE ) || !pi_pOrgPartsItem )
		return NULL;

	CUnitPartsItem * pParts = &m_listPart[byOrderNo];

	if( pParts->Create( pi_byPartType, pi_pOrgPartsItem->GetDTIndex() ) )
	{
		pParts->Copy( pi_pOrgPartsItem );
		ResetUnitDefenceAndSpeed();
		SetPartsUI( pParts, TRUE );
		return pParts;
	}

	// arms, shoulder, backpack 은 탄창 부분을 refresh 시켜야한다.
	RefreshBulletUI( pi_byPartType, TRUE );

	return NULL;
}

void
CUnitItem::RemoveParts( BYTE pi_byPartType )
{
	BYTE byOrderNo = pi_byPartType - IEPT_UNIT_HELMET;

	if( byOrderNo >= MAX_UNIT_PART_TYPE )
		return;

	m_listPart[byOrderNo].SetEmpty();
	ResetUnitDefenceAndSpeed();

	SetPartsUI( &m_listPart[byOrderNo], FALSE );

	// arms, shoulder, backpack 은 탄창 부분을 refresh 시켜야한다.
	RefreshBulletUI( pi_byPartType, FALSE );
}

void
CUnitItem::SetPartsUI( CUnitPartsItem * pi_pParts, BOOL pi_bIsAdd )
{
	if( !m_uiTuningWindow || !pi_pParts )
		return;

	if( pi_bIsAdd )
	{
		// common
		m_uiTuningWindow->Equip_Parts( pi_pParts->GetUnitPartType(), pi_pParts->GetIconID(), !pi_pParts->IsHave() );
		pi_pParts->SetIcon( m_uiTuningWindow->GetParts( pi_pParts->GetUnitPartType() ) );

		// arms
		if( pi_pParts->IsEqualItemKind( IEPT_UNIT_ARMS ) )
		{
			UNIT_WEAPON_DATA * pWeaponData = static_cast< UNIT_WEAPON_DATA * >( pi_pParts->GetData() );

			m_uiTuningWindow->SetWeaponName( pi_pParts->GetName() );
			m_uiTuningWindow->SetWeaponAttPower( pWeaponData->dwMinStrikingPower, pWeaponData->dwMaxStrikingPower );
			m_uiTuningWindow->SetBulletNum( m_listBullet[0].GetDurability(), m_listBullet[0].GetOrgMaxDurability() );
		}
		// shoulder
		else if( pi_pParts->IsEqualItemKind( IEPT_UNIT_SHOULDER ) )
		{
			UNIT_WEAPON_DATA * pWeaponData = static_cast< UNIT_WEAPON_DATA * >( pi_pParts->GetData() );

			m_uiTuningWindow->SetAddWeaponName( pi_pParts->GetName() );
			m_uiTuningWindow->SetAddWeaponAttPower( pWeaponData->dwMinStrikingPower, pWeaponData->dwMaxStrikingPower );
			m_uiTuningWindow->SetAddBulletNum( m_listBullet[1].GetDurability(), m_listBullet[1].GetOrgMaxDurability() );
		}
	}
	else
	{
		m_uiTuningWindow->UnEquip_Parts( pi_pParts->GetUnitPartType() );

		// arms
		if( pi_pParts->IsEqualItemKind( IEPT_UNIT_ARMS ) )
		{
			m_uiTuningWindow->SetWeaponName( "" );
			m_uiTuningWindow->SetWeaponAttPower( 0, 0 );
			m_uiTuningWindow->SetBulletNum( 0, 0 );
		}
		// shoulder
		else if( pi_pParts->IsEqualItemKind( IEPT_UNIT_SHOULDER ) )
		{
			m_uiTuningWindow->SetAddWeaponName( "" );
			m_uiTuningWindow->SetAddWeaponAttPower( 0, 0 );
			m_uiTuningWindow->SetAddBulletNum( 0, 0 );
		}
	}
}

//==============================================================================

CUnitPartsItem *
CUnitItem::AddBullet( BYTE pi_byPartType, DWORD pi_dwDTIndex )
{
	if( GetBulletSocketNum( pi_byPartType ) <= 0 )
		return NULL;

	BYTE byOrderNo = pi_byPartType - IEPT_UNIT_ARMS;

	if( byOrderNo >= MAX_UNIT_BULLET_TYPE )
		return NULL;

	CUnitPartsItem * pParts = &m_listBullet[byOrderNo];

	if( pParts->Create( IEPT_UNIT_BULLET, pi_dwDTIndex ) )
	{
		SetBulletUI( pi_byPartType, pParts, TRUE );
		return pParts;
	}

	return NULL;
}

CUnitPartsItem *
CUnitItem::AddBullet( BYTE pi_byPartType, CUnitPartsItem * pi_pOrgBulletItem )
{
	if( GetBulletSocketNum( pi_byPartType ) <= 0 )
		return NULL;

	BYTE byOrderNo = pi_byPartType - IEPT_UNIT_ARMS;

	if( ( byOrderNo >= MAX_UNIT_BULLET_TYPE ) || !pi_pOrgBulletItem )
		return NULL;

	CUnitPartsItem * pParts = &m_listBullet[byOrderNo];

	if( pParts->Create( IEPT_UNIT_BULLET, pi_pOrgBulletItem->GetDTIndex() ) )
	{
		pParts->Copy( pi_pOrgBulletItem );
		SetBulletUI( pi_byPartType, pParts, TRUE );
		return pParts;
	}

	return NULL;
}

void
CUnitItem::RemoveBullet( BYTE pi_byPartType )
{
	BYTE byOrderNo = pi_byPartType - IEPT_UNIT_ARMS;

	if( byOrderNo >= MAX_UNIT_BULLET_TYPE )
		return;

	m_listBullet[byOrderNo].SetEmpty();
	SetBulletUI( pi_byPartType, &m_listBullet[byOrderNo], FALSE );
}

void
CUnitItem::SetBulletUI( BYTE pi_byPartType, CUnitPartsItem * pi_pBullet, BOOL pi_bIsAdd )
{
	if( !m_uiTuningWindow || !pi_pBullet )
		return;

	if( pi_bIsAdd )
	{
		if( pi_byPartType == IEPT_UNIT_ARMS )
		{
			m_uiTuningWindow->Equip_Bullet( CGUIUnitTuningWindow::Bullet_Arms, pi_pBullet->GetIconID(), !pi_pBullet->IsHave() );
			pi_pBullet->SetIcon( m_uiTuningWindow->GetBullet( CGUIUnitTuningWindow::Bullet_Arms ) );
			m_uiTuningWindow->SetBulletNum( pi_pBullet->GetDurability(), pi_pBullet->GetOrgMaxDurability() );
		}
		else if( pi_byPartType == IEPT_UNIT_SHOULDER )
		{
			m_uiTuningWindow->Equip_Bullet( CGUIUnitTuningWindow::Bullet_Shoulder, pi_pBullet->GetIconID(), !pi_pBullet->IsHave() );
			pi_pBullet->SetIcon( m_uiTuningWindow->GetBullet( CGUIUnitTuningWindow::Bullet_Shoulder ) );
			m_uiTuningWindow->SetAddBulletNum( pi_pBullet->GetDurability(), pi_pBullet->GetOrgMaxDurability() );
		}
	}
	else
	{
		if( pi_byPartType == IEPT_UNIT_ARMS )
		{
			m_uiTuningWindow->UnEquip_Bullet( CGUIUnitTuningWindow::Bullet_Arms );
			m_uiTuningWindow->SetBulletNum( 0, 0 );
		}
		else if( pi_byPartType == IEPT_UNIT_SHOULDER )
		{
			m_uiTuningWindow->UnEquip_Bullet( CGUIUnitTuningWindow::Bullet_Shoulder );
			m_uiTuningWindow->SetAddBulletNum( 0, 0 );
		}
	}
}

//==============================================================================

CUnitPartsItem *
CUnitItem::AddBackpackBullet( BYTE pi_byOrderNo, DWORD pi_dwDTIndex )
{
	if( pi_byOrderNo >= MAX_BACKPACK_BULLET_NUM )
		return NULL;

	CUnitPartsItem * pParts = &m_listBackpackBullet[pi_byOrderNo];

	if( pParts->Create( IEPT_UNIT_BULLET, pi_dwDTIndex ) )
	{
		++m_byBackpackBulletNum;
		SetBackpackBulletUI( pi_byOrderNo, pParts, TRUE );
		return pParts;
	}

	return NULL;
}

CUnitPartsItem *
CUnitItem::AddBackpackBullet( BYTE pi_byOrderNo, CUnitPartsItem * pi_pOrgBackpackBulletItem )
{
	if( ( pi_byOrderNo >= MAX_BACKPACK_BULLET_NUM ) || !pi_pOrgBackpackBulletItem )
		return NULL;

	CUnitPartsItem * pParts = &m_listBackpackBullet[pi_byOrderNo];

	if( pParts->Create( IEPT_UNIT_BULLET, pi_pOrgBackpackBulletItem->GetDTIndex() ) )
	{
		pParts->Copy( pi_pOrgBackpackBulletItem );
		++m_byBackpackBulletNum;
		SetBackpackBulletUI( pi_byOrderNo, pParts, TRUE );
		return pParts;
	}

	return NULL;
}

void
CUnitItem::RemoveBackpackBullet( BYTE pi_byOrderNo )
{
	if( pi_byOrderNo >= MAX_BACKPACK_BULLET_NUM )
		return;

	m_listBackpackBullet[pi_byOrderNo].SetEmpty();
	++m_byBackpackBulletNum;

	SetBackpackBulletUI( pi_byOrderNo, &m_listBackpackBullet[pi_byOrderNo], FALSE );
}

void
CUnitItem::SetBackpackBulletUI( BYTE pi_byOrderNo, CUnitPartsItem * pi_pBackpackBullet, BOOL pi_bIsAdd )
{
	if( !m_uiTuningWindow || !pi_pBackpackBullet )
		return;

	if( pi_bIsAdd )
	{
		m_uiTuningWindow->Equip_BackPack_Bullet( pi_byOrderNo, pi_pBackpackBullet->GetIconID(), !pi_pBackpackBullet->IsHave() );
		pi_pBackpackBullet->SetIcon( m_uiTuningWindow->GetBackPackBullet( pi_byOrderNo ) );
	}
	else
	{
		m_uiTuningWindow->UnEquip_BackPack_Bullet( pi_byOrderNo );
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

CUnitPartsItem *
CUnitItem::GetParts( BYTE pi_byPartType )
{
	pi_byPartType -= IEPT_UNIT_HELMET;

	if( pi_byPartType >= MAX_UNIT_PART_TYPE )
		return NULL;

	if( m_listPart[pi_byPartType].IsEmpty() )
		return NULL;

	return &m_listPart[pi_byPartType];
}

CUnitPartsItem *
CUnitItem::GetBullet( BYTE pi_byPartType )
{
	pi_byPartType -= IEPT_UNIT_ARMS;

	if( pi_byPartType >= MAX_UNIT_BULLET_TYPE )
		return NULL;

	if( m_listBullet[pi_byPartType].IsEmpty() )
		return NULL;

	return &m_listBullet[pi_byPartType];
}

CUnitPartsItem *
CUnitItem::GetBackpackBullet( BYTE pi_byOrderNo )
{
	if( pi_byOrderNo >= MAX_BACKPACK_BULLET_NUM )
		return NULL;

	if( m_listBackpackBullet[pi_byOrderNo].IsEmpty() )
		return NULL;

	return &m_listBackpackBullet[pi_byOrderNo];
}

BYTE
CUnitItem::GetBulletSocketNum( BYTE pi_byPartType )
{
	CUnitPartsItem * pParts = GetParts( pi_byPartType );
	if( pParts )
		return pParts->GetBulletSocketNum();
	else
		return 0;
}

DWORD
CUnitItem::GetBulletNum( BYTE pi_byPartType )
{
	CUnitPartsItem * pBullet = GetBullet( pi_byPartType );
	if( pBullet )
		return pBullet->GetDurability();
	else
		return 0;
}

// arms, shoulder, backpack 은 탄창 부분을 refresh 시켜야한다.
void
CUnitItem::RefreshBulletUI( BYTE pi_byPartType, BOOL pi_bIsAdd )
{
	if( pi_byPartType == IEPT_UNIT_ARMS || pi_byPartType == IEPT_UNIT_SHOULDER )
	{
		if( GetBulletSocketNum( pi_byPartType ) > 0 )
			SetBulletUI( pi_byPartType, GetBullet( pi_byPartType ), pi_bIsAdd );
	}
	else if( pi_byPartType == IEPT_UNIT_BACKPACK )
	{
		for( int i = 0; i < GetBulletSocketNum( pi_byPartType ); ++i )
			SetBulletUI( pi_byPartType, GetBackpackBullet( i ), pi_bIsAdd );
	}
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CUnitItem::SetDurability( DWORD pi_dwCurDP )
{
	CUnitPartsItem::SetDurability( pi_dwCurDP );

	if( m_uiTuningWindow )
		m_uiTuningWindow->SetDurability( GetDurabilityRate() * 100 );
}

DWORD
CUnitItem::GetMaxStrikingPower( void )
{
	if( IsEmpty() || !m_pWeaponParts )
		return 0;

	return m_pWeaponParts->GetMaxStrikingPower();
}

DWORD
CUnitItem::GetMinStrikingPower( void )
{
	if( IsEmpty() || !m_pWeaponParts )
		return 0;

	return m_pWeaponParts->GetMinStrikingPower();
}

DWORD
CUnitItem::GetDefencePower( void )
{
	if( IsEmpty() )
		return 0;

	static const float const PARTS_RATE[MAX_UNIT_PART_TYPE] = { 0.2f, 0.4f, 0.3f, 0.1f, 0.0f, 0.0f };

	DWORD	dwUnitDefencePower = 0;
	for( int i = 0; i < MAX_UNIT_PART_TYPE; ++i )
	{
		if( !m_listPart[i].IsEmpty() )
			dwUnitDefencePower += (float)m_listPart[i].GetDefencePower() * PARTS_RATE[i];
	}

	return dwUnitDefencePower;
}

BYTE
CUnitItem::GetAttackExpertness( void )
{
	if( IsEmpty() || !m_pWeaponParts )
		return 0;

	return m_pWeaponParts->GetAttackExpertness();
}

BYTE
CUnitItem::GetDefenceExpertness( void )
{
	if( IsEmpty() )
		return 0;

	BYTE	byUnitDefenceExpertness = 0;

	for( int i = 0; i < MAX_UNIT_PART_TYPE; ++i )
	{
		if( !m_listPart[i].IsEmpty() &&
			( m_listPart[i].IsEqualItemKind( IEPT_UNIT_HELMET ) ||
			  m_listPart[i].IsEqualItemKind( IEPT_UNIT_UPPER_PART ) ) )
			byUnitDefenceExpertness += m_listPart[i].GetDefenceExpertness();
	}

	if( byUnitDefenceExpertness > 99 )
		byUnitDefenceExpertness = 99;

	return byUnitDefenceExpertness;
}

float
CUnitItem::GetMoveSpeed( void )
{
	if( IsEmpty() )
		return 0.0f;

	UNIT_FRAME_DATA * pFrameData = static_cast< UNIT_FRAME_DATA * >( m_pData );
	if( !pFrameData )
		return 0.0f;

	float	fUnitSpeed = pFrameData->fSpeed[0];

	for( int i = 0; i < MAX_UNIT_PART_TYPE; ++i )
	{
		if( !m_listPart[i].IsEmpty() )
			fUnitSpeed -= m_listPart[i].GetMoveSpeed();
	}

	return fUnitSpeed * CHAR_SPEED_DEFAULT;
}

void
CUnitItem::GetMoveSpeedString( char * po_pOutputString )
{
	static char const SPEED_STRING[5][32] = { "매우느림", "느림", "보통", "빠름", "매우빠름" };

	float fUnitSpeed = GetMoveSpeed() / CHAR_SPEED_DEFAULT;

	if( fUnitSpeed <= 1.0f )
		sprintf( po_pOutputString, "%s", SPEED_STRING[0] );
	else if( fUnitSpeed > 1.0f && fUnitSpeed <= 2.4f )
		sprintf( po_pOutputString, "%s", SPEED_STRING[1] );
	else if( fUnitSpeed > 2.4f && fUnitSpeed <= 3.0f )
		sprintf( po_pOutputString, "%s", SPEED_STRING[2] );
	else if( fUnitSpeed > 3.0f && fUnitSpeed <= 4.4f )
		sprintf( po_pOutputString, "%s", SPEED_STRING[3] );
	else
		sprintf( po_pOutputString, "%s", SPEED_STRING[4] );
}

WORD
CUnitItem::GetResist( BYTE pi_byAttributeType )
{
	if( IsEmpty() )
		return 0;

	DWORD	dwUnitResist = 0;
	for( int i = 0; i < MAX_UNIT_PART_TYPE; ++i )
	{
		if( !m_listPart[i].IsEmpty() )
			dwUnitResist += m_listPart[i].GetResist( pi_byAttributeType );
	}

	return dwUnitResist;
}

void
CUnitItem::SetCurrentBoosterPoint( BYTE pi_byCurBP )
{
	CUnitPartsItem * pBackpack = GetParts( IEPT_UNIT_BACKPACK );
	if( pBackpack )
		pBackpack->SetDurability( pi_byCurBP );
}

BYTE
CUnitItem::GetCurrentBoosterPoint( void )
{
	CUnitPartsItem * pBackpack = GetParts( IEPT_UNIT_BACKPACK );
	if( pBackpack )
		return pBackpack->GetDurability();

	return 0;
}

// 기갑 장비를 출고할 때의 비용 ( 달란트 단위임 )
// 단일 수량이므로 특별히 float으로 다룰 필요는 없음.
DWORD
CUnitItem::GetDeliveryPrice( DWORD pi_dwMerchantIndex )
{
	DWORD dwDeliveryPrice = m_dwTractionDebt + DEFAULT_KEEPING_PRICE;

	if( pi_dwMerchantIndex == ID_INVALID )
		dwDeliveryPrice += DEFAULT_DELIVERY_PRICE;

	return ( dwDeliveryPrice * _GetAvatar()->GetBuyTexRate() );
}

// 기갑 장비 수리비
DWORD
CUnitItem::GetRepairPrice( void )
{
	return 0;
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

void
CUnitItem::SetAttackParts( BYTE pi_byItemKind )
{
	if( ( pi_byItemKind == IEPT_UNIT_ARMS ) || ( pi_byItemKind == IEPT_UNIT_SHOULDER ) )
		m_pWeaponParts = &m_listPart[pi_byItemKind - IEPT_UNIT_HELMET];
}

void
CUnitItem::SetBulletNum( BYTE pi_byAttackParts, DWORD pi_dwBulletNum )
{
	if( pi_byAttackParts == IEPT_UNIT_ARMS )
		m_listBullet[0].SetDurability( pi_dwBulletNum );
	else
		m_listBullet[1].SetDurability( pi_dwBulletNum );
}

void
CUnitItem::SetBulletNumThatCurAttackParts( DWORD pi_dwBulletNum )
{
	if( !m_pWeaponParts )
		return;

	if( m_pWeaponParts->IsEqualItemKind( IEPT_UNIT_ARMS ) )
		m_listBullet[0].SetDurability( pi_dwBulletNum );
	else
		m_listBullet[1].SetDurability( pi_dwBulletNum );
}

DWORD
CUnitItem::GetBulletNumThatCurAttackParts( void )
{
	if( !m_pWeaponParts )
		return 0;

	if( m_pWeaponParts->IsEqualItemKind( IEPT_UNIT_ARMS ) )
		return m_listBullet[0].GetDurability();
	else
		return m_listBullet[1].GetDurability();
}

void
CUnitItem::ResetUnitDefenceAndSpeed( void )
{
	if( m_uiTuningWindow )
	{
		m_uiTuningWindow->SetDefPower( GetDefencePower() );
		char pUnitSpeed[32];
		GetMoveSpeedString( pUnitSpeed );
		m_uiTuningWindow->SetMovePower( pUnitSpeed );
	}
}

BOOL
CUnitItem::IsHavePartsOrBullets( void )
{
	int i;

	for( i = 0; i < MAX_UNIT_PART_TYPE; ++i )
	{
		if( !m_listPart[i].IsEmpty() )
			return TRUE;
	}

	for( i = 0; i < MAX_UNIT_BULLET_TYPE; ++i )
	{
		if( !m_listBullet[i].IsEmpty() )
			return TRUE;
	}

	for( i = 0; i < MAX_BACKPACK_BULLET_NUM; ++i )
	{
		if( !m_listBackpackBullet[i].IsEmpty() )
			return TRUE;
	}

	return FALSE;
}

void
CUnitItem::SetHaveAllPartAndBullets( void )
{
	int i;

	for( i = 0; i < MAX_UNIT_PART_TYPE; ++i )
	{
		if( !m_listPart[i].IsEmpty() )
			m_listPart[i].SetHave( TRUE );
	}

	for( i = 0; i < MAX_UNIT_BULLET_TYPE; ++i )
	{
		if( !m_listBullet[i].IsEmpty() )
			m_listBullet[i].SetHave( TRUE );
	}

	for( i = 0; i < MAX_BACKPACK_BULLET_NUM; ++i )
	{
		if( !m_listBackpackBullet[i].IsEmpty() )
			m_listBackpackBullet[i].SetHave( TRUE );
	}
}

void
CUnitItem::SetPartsHaveState( BYTE pi_byPartType, BOOL pi_bState )
{
	pi_byPartType -= IEPT_UNIT_HELMET;
	if( pi_byPartType >= MAX_UNIT_PART_TYPE )
		return;

	if( m_listPart[pi_byPartType].IsEmpty() )
		return;

	m_listPart[pi_byPartType].SetHave( pi_bState );
	if( m_uiTuningWindow )
		m_uiTuningWindow->SetPartsBuyingState( pi_byPartType, !pi_bState );
}

void
CUnitItem::SetBulletHaveState( BYTE pi_byPartType, BOOL pi_bState )
{
	pi_byPartType -= IEPT_UNIT_ARMS;
	if( pi_byPartType >= MAX_UNIT_BULLET_TYPE )
		return;

	if( m_listBullet[pi_byPartType].IsEmpty() )
		return;

	m_listBullet[pi_byPartType].SetHave( pi_bState );
	if( m_uiTuningWindow )
		m_uiTuningWindow->SetBulletBuyingState( pi_byPartType, !pi_bState );
}

void
CUnitItem::SetBackpackBulletHaveState( BYTE pi_byOrderNo, BOOL pi_bState )
{
	if( pi_byOrderNo >= MAX_BACKPACK_BULLET_NUM )
		return;

	if( m_listBackpackBullet[pi_byOrderNo].IsEmpty() )
		return;

	m_listBackpackBullet[pi_byOrderNo].SetHave( pi_bState );
	if( m_uiTuningWindow )
		m_uiTuningWindow->SetBackpackBulletBuyingState( pi_byOrderNo, !pi_bState );
}

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

// 튜닝 보드의 파츠 "구입" 리스트의 pack 번호임
// pi_bIsOrgItem => 아바타가 튜닝 전 원래 가지고 있던 파츠인가?
BOOL
CUnitItem::AddByTuningBoard( BYTE pi_byPackNo, CBoardItem * pi_pBoardItem, BOOL pi_bIsOrgItem )
{
	if( !pi_pBoardItem )
		return FALSE;

	CUnitPartsItem * pAddParts = NULL;

	// 파츠
	if( pi_byPackNo < MAX_UNIT_PART_TYPE )
	{
		pAddParts = AddParts( IEPT_UNIT_HELMET + pi_byPackNo, pi_pBoardItem->GetDTIndex() );
		SetPartsHaveState( IEPT_UNIT_HELMET +  pi_byPackNo, pi_bIsOrgItem );
	}
	// 탄환일 경우에는 튜닝하려 선택한 탄환의 소켓번호가 pi_byPackNo에 더해져서 온다.
	else if( pi_byPackNo < MAX_UNIT_PART_TYPE + MAX_UNIT_BULLET_TYPE )
	{
		BYTE bySocketNo = pi_byPackNo - MAX_UNIT_PART_TYPE;
		pAddParts = AddBullet( IEPT_UNIT_ARMS + bySocketNo, pi_pBoardItem->GetDTIndex() );
		SetBulletHaveState( IEPT_UNIT_ARMS + bySocketNo, pi_bIsOrgItem );
	}
	else
	{
		BYTE bySocketNo = pi_byPackNo - MAX_UNIT_PART_TYPE - MAX_UNIT_BULLET_TYPE;
		pAddParts = AddBackpackBullet( bySocketNo, pi_pBoardItem->GetDTIndex() );
		SetBackpackBulletHaveState( bySocketNo, pi_bIsOrgItem );
	}

	return reinterpret_cast< BOOL >( pAddParts );
}

// 튜닝 보드의 파츠 "장착" 리스트의 pack & socket 번호임
BOOL
CUnitItem::AddByTuningBoard( BYTE pi_byPackNo, BYTE pi_bySocketNo, CUnitPartsItem * pi_pPartItem )
{
	if( !pi_pPartItem )
		return FALSE;

	CUnitPartsItem * pAddParts = NULL;

	if( pi_byPackNo == 0 )
	{
		pAddParts = AddParts( IEPT_UNIT_HELMET + pi_bySocketNo, pi_pPartItem->GetDTIndex() );
		if( pAddParts )
		{
			pAddParts->Copy( pi_pPartItem );
			SetPartsHaveState( IEPT_UNIT_HELMET + pi_bySocketNo, pAddParts->IsHave() );
		}
	}
	else if( pi_byPackNo == 1 )
	{
		if( pi_bySocketNo < 2 )
		{
			pAddParts = AddBullet( IEPT_UNIT_ARMS + pi_bySocketNo, pi_pPartItem->GetDTIndex() );
			if( pAddParts )
			{
				pAddParts->Copy( pi_pPartItem );
				SetBulletHaveState( IEPT_UNIT_ARMS +  pi_bySocketNo, pAddParts->IsHave() );
			}
		}
		else if( pi_bySocketNo < 10 )
		{
			pAddParts = AddBackpackBullet( pi_bySocketNo - 2, pi_pPartItem->GetDTIndex() );
			if( pAddParts )
			{
				pAddParts->Copy( pi_pPartItem );
				SetBackpackBulletHaveState( pi_bySocketNo - 2, pAddParts->IsHave() );
			}
		}
	}

	return reinterpret_cast< BOOL >( pAddParts );
}

//==============================================================================

// 튜닝 보드의 파츠 "구입" 리스트의 pack 번호임
void
CUnitItem::RemoveByTuningBoard( BYTE pi_byPackNo, CBoardItem * pi_pBoardItem )
{
	if( !pi_pBoardItem )
		return;

	if( pi_byPackNo <= 5 )
	{
		RemoveParts( pi_byPackNo + IEPT_UNIT_HELMET );
	}
	else if( pi_byPackNo >= 6 )
	{
		if( !m_listBullet[0].IsEmpty() )
			RemoveBullet( IEPT_UNIT_ARMS );
		else
			RemoveBackpackBullet( m_byBackpackBulletNum - 1 );
	}
	else if( pi_byPackNo == 7 )
	{
		if( !m_listBullet[1].IsEmpty() )
			RemoveBullet( IEPT_UNIT_SHOULDER );
		else
			RemoveBackpackBullet( m_byBackpackBulletNum - 1 );
	}
}

// 튜닝 보드의 파츠 "장착" 리스트의 pack & socket 번호임
void
CUnitItem::RemoveByTuningBoard( BYTE pi_byPackNo, BYTE pi_bySocketNo )
{
	if( pi_byPackNo == 0 )
	{
		RemoveParts( pi_bySocketNo + IEPT_UNIT_HELMET );
	}
	else if( pi_byPackNo == 1 )
	{
		if( pi_bySocketNo < 2 )
			RemoveBullet( pi_bySocketNo + IEPT_UNIT_ARMS );
		else if( pi_bySocketNo < 10 )
			RemoveBackpackBullet( pi_bySocketNo - 2 );
	}
}

//==============================================================================

// 튜닝 보드의 파츠 "구입" 리스트의 pack 번호임
CUnitPartsItem *
CUnitItem::GetPartsByTuningBoard( BYTE pi_byPackNo, CBoardItem * pi_pBoardItem )
{
	if( !pi_pBoardItem )
		return NULL;

	if( pi_byPackNo < MAX_UNIT_PART_TYPE )
	{
		return GetParts( pi_byPackNo + IEPT_UNIT_HELMET );
	}
	// 탄환일 경우에는 튜닝하려 선택한 탄환의 소켓번호가 pi_byPackNo에 더해져서 온다.
	else if( pi_byPackNo < MAX_UNIT_PART_TYPE + MAX_UNIT_BULLET_TYPE )
	{
		return GetBullet( pi_byPackNo - MAX_UNIT_PART_TYPE + IEPT_UNIT_ARMS );
	}
	else
	{
		return GetBackpackBullet( pi_byPackNo - MAX_UNIT_PART_TYPE - MAX_UNIT_BULLET_TYPE + IEPT_UNIT_ARMS );
	}

	return NULL;
}

// 튜닝 보드의 파츠 "장착" 리스트의 pack & socket 번호임
CUnitPartsItem *
CUnitItem::GetPartsByTuningBoard( BYTE pi_byPackNo, BYTE pi_bySocketNo )
{
	if( pi_byPackNo == 0 )
	{
		return GetParts( pi_bySocketNo + IEPT_UNIT_HELMET );
	}
	else if( pi_byPackNo == 1 )
	{
		if( pi_byPackNo < 2 )
			return GetBullet( pi_bySocketNo + IEPT_UNIT_ARMS );
		else if( pi_byPackNo < 10 )
			return GetBackpackBullet( pi_bySocketNo - 2 );
	}

	return NULL;
}

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Boundary           //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

#endif // _NEW_UI_