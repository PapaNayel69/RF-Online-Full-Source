////////////////////////////////////////////////////////////////////////////////
//
// CItemDataMgr Class Header
//
////////////////////////////////////////////////////////////////////////////////

#ifndef __ITEM_DATA_MGR_H__
#define __ITEM_DATA_MGR_H__

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Item Data          //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

struct ITEM_DATA : public BASE_DATA
{
//protected :

	DWORD	dwDTIndex;				// DT Index
	DWORD	dwDTCode;				// Unique Code
	char	pName[32];
	
	DWORD	dwMeshID;
	DWORD	dwIconID;

	BYTE	byUsableRace[8];		// 아이템을 사용할 수 있는 종족

	BYTE	byEquipType;
	
	BYTE	byMoneyType;
	DWORD	dwStandardPrice;

	BOOL	bIsExchangable;			// 교환 가능한가?
	BOOL	bIsSellable;			// 판매 가능한가?
	BOOL	bIsDumpable;			// 버리기가 가능한가?

	DWORD	dwDescriptionDTIndex;

public :

	inline	DWORD	GetDTIndex( void )				{ return dwDTIndex; }
	inline	DWORD	GetDTCode( void )				{ return dwDTCode; }
	inline	char *	GetName( void )					{ return pName; }

	inline	DWORD	GetMeshID( void )				{ return dwMeshID; }
	inline	DWORD	GetIconID( void )				{ return dwIconID; }

	inline	BOOL	IsUsableRace( BYTE pi_byRace )	{ return byUsableRace[pi_byRace]; }

	inline	BYTE	GetEquipType( void )			{ return byEquipType; }

	inline	BYTE	GetMoneyType( void )			{ return byMoneyType; }
	inline	DWORD	GetStandardPrice( void )		{ return dwStandardPrice; }

	inline	BOOL	IsExchangable( void )			{ return bIsExchangable; }
	inline	BOOL	IsSellable( void )				{ return bIsSellable; }
	inline	BOOL	IsDumpable( void )				{ return bIsDumpable; }

	inline	DWORD	GetDescriptionDTIndex( void )	{ return dwDescriptionDTIndex; }

	ITEM_DATA()
	{
		dwDTIndex			= ID_INVALID;
		dwDTCode			= ID_INVALID;
		ZeroMemory( pName, 32 );
		
		dwMeshID			= ID_INVALID;
		dwIconID			= ID_INVALID;

		ZeroMemory( byUsableRace, 8 );

		byEquipType			= ID_INVALID;
		
		byMoneyType			= ID_INVALID;
		dwStandardPrice		= 0;

		bIsExchangable		= FALSE;
		bIsSellable			= FALSE;
		bIsDumpable			= FALSE;

		dwDescriptionDTIndex= ID_INVALID;
	};
};

//------------------------------------------------------------------------------

struct EQUIPMENT_DATA : public ITEM_DATA
{
public :

	enum { MAX_UET_NUM = 2 };						// Usable Expertness Type

//protected :

	BOOL	bIsRepairable;							// 수리 가능한가?
	BYTE	byUpgradeSocketNum;						// 업그레이드 소켓의 갯수
	DWORD	dwRepairPrice;							// 수리 가격
	DWORD	dwMaxDP;								// 최대 내구도 ( DP = Durability Point )

	BYTE	byUsableLevel;							// 사용 가능 레벨
	BYTE	byUsableExpertnessType[MAX_UET_NUM];	// 사용 가능 무기 숙련도 타입
	BYTE	byUsableExpertness[MAX_UET_NUM];		// 사용 가능 무기 숙련도

	int		nSpeed;									// 무기일 경우는 공격 속도이고, 아머일 경우는 공격 딜레이이다. ( 결국 공격시에 딜레이를 정해주는 동일한 용도로 쓰인다. )

	BYTE	byMaxEffectNum;							// 아이템에 걸려있는 이펙트의 갯수
	BYTE	byEffectType[MAX_ITEM_EFFECT];			// 아이템에 걸려있는 이펙트의 타입
	float	fEffectValue[MAX_ITEM_EFFECT];			// 아이템에 걸려있는 이펙트의 값

	DWORD	dwEffectMaterial;						// 이펙트에서만 사용됨

public :

	inline	BOOL	IsRepairable( void )					{ return bIsRepairable; }
	inline	BYTE	GetUpgradeSocketNum( void )				{ return byUpgradeSocketNum; }
	inline	DWORD	GetRepairPrice( void )					{ return dwRepairPrice; }
	inline	DWORD	GetMaxDP( void )						{ return dwMaxDP; }

	inline	BYTE	GetUsableLevel( void )					{ return byUsableLevel; }
	inline	BYTE	GetUsableExpertnessType( BYTE pi_byOrderNo ){ return byUsableExpertnessType[pi_byOrderNo]; }
	inline	BYTE	GetUsableExpertness( BYTE pi_byOrderNo ){ return byUsableExpertness[pi_byOrderNo]; }

	inline	int		GetSpeed( void )						{ return nSpeed; }

	inline	BYTE	GetMaxEffectNum( void )					{ return byMaxEffectNum; }
	inline	BYTE	GetEffectType( BYTE pi_byOrderNo )		{ return byEffectType[pi_byOrderNo]; }
	inline	float	GetEffectValue( BYTE pi_byOrderNo )		{ return fEffectValue[pi_byOrderNo]; }

	EQUIPMENT_DATA()
	{
		bIsRepairable		= FALSE;
		byUpgradeSocketNum	= 0;
		dwRepairPrice		= 0;
		dwMaxDP				= 0;

		byUsableLevel		= 0;
		for( int i = 0; i < MAX_UET_NUM; ++i )
		{
			byUsableExpertnessType[i]	= ID_INVALID;
			byUsableExpertness[i]		= 0;
		}

		nSpeed				= 0;

		byMaxEffectNum		= 0;
		for( i = 0; i < MAX_ITEM_EFFECT; ++i )
		{
			byEffectType[i] = ID_INVALID;
			fEffectValue[i]	= 0.0f;
		}

		dwEffectMaterial	= ID_INVALID;
	}
};

//------------------------------------------------------------------------------

struct ARMOR_DATA : public EQUIPMENT_DATA
{
//protected :

	DWORD	dwDefencePoint;					// 방어력
	WORD	wResist[MAX_ITEM_ATTRIBUTE];	// 내성

public :

	inline	DWORD	GetDefencePoint( void )			{ return dwDefencePoint; }
	inline	WORD	GetResist( BYTE pi_byOrderNo )	{ return wResist[pi_byOrderNo]; }

	ARMOR_DATA()
	{
		dwDefencePoint	= 0;
		for( int i = 0; i < MAX_ITEM_ATTRIBUTE; ++i )
			wResist[i]	= 0;
	}
};

//------------------------------------------------------------------------------

struct WEAPON_DATA : public EQUIPMENT_DATA
{
//protected :

	int		nType;						// 무기의 종류
	float	fDistance;					// 공격 사정 거리

	DWORD	dwMinStrikingPower;			// 최소 일반 공격력
	DWORD	dwMaxStrikingPower;			// 최대 일반 공격력

	DWORD	dwMinForcePower;			// 최소 포스 공격력
	DWORD	dwMaxForcePower;			// 최대 포스 공격력

	BYTE	byAttributeType;			// 아이템의 속성타입 ( 0 = 불, 1 = 물, 2 = 흙, 3 = 바람 )

public :

	inline	int		GetWeaponCategoryID( void )	{ return nType; }
	inline	float	GetDistance( void )			{ return fDistance; }

	inline	DWORD	GetMinStrikingPower( void )	{ return dwMinStrikingPower; }
	inline	DWORD	GetMaxStrikingPower( void )	{ return dwMaxStrikingPower; }

	inline	DWORD	GetMinForcePower( void )	{ return dwMinForcePower; }
	inline	DWORD	GetMaxForcePower( void )	{ return dwMaxForcePower; }

	inline	BYTE	GetAttributeType( void )	{ return byAttributeType; }

	WEAPON_DATA()
	{
		nType					= ID_INVALID;
		fDistance				= 0.0f;

		dwMinStrikingPower		= 0;
		dwMaxStrikingPower		= 0;

		dwMinForcePower			= 0;
		dwMaxForcePower			= 0;

		byAttributeType			= ID_INVALID;
	}
};

//------------------------------------------------------------------------------

enum
{
	PET_NONE, PET_INC_HP, PET_INC_FP, PET_INC_SP,
	PET_INC_ATTACK_POINT, PET_INC_DEFENCE_POINT,
	MAX_POTION_EFFECT_TYPE
};

#define MAX_POTION_EFFECT_NUM	( 4 )

struct POTION_DATA : public ITEM_DATA
{
//protected :

	BYTE	byUsableLevel;			// 사용 가능 레벨

	BYTE	byEffectType[MAX_POTION_EFFECT_NUM];
	float	fEffectValue[MAX_POTION_EFFECT_NUM];
	BYTE	byContinuanceTime;		// 지속 시간 ( 단위 : 초 )

public :

	inline	BYTE	GetUsableLevel( void )			{ return byUsableLevel; }
	inline	BYTE	GetEffectType( BYTE pi_byNo )	{ return byEffectType[pi_byNo]; }
	inline	float	GetEffectValue( BYTE pi_byNo )	{ return fEffectValue[pi_byNo]; }
	inline	BYTE	GetContinuanceTime( void )		{ return byContinuanceTime; }

	POTION_DATA()
	{
		byUsableLevel		= 0;
		ZeroMemory( byEffectType, MAX_POTION_EFFECT_NUM );

		byContinuanceTime	= 0;
	}

//	DWORD GetSize( void ) { return sizeof( POTION_DATA ); }
};

//------------------------------------------------------------------------------

struct BULLET_DATA : public ITEM_DATA
{
//protected :

	enum { MAX_USABLE_WEAPON = 18 };

	BYTE	byAttribute;							// 아이템의 속성
	float	fIncreasePointOfStrikingPower;			// 일반 공격력 증가량
	DWORD	dwMaxDP;								// 최대 내구도 ( DP = Durability Point )

	BYTE	byUsableWeaponNum;						// 탄환을 사용할 수 있는 무기의 갯수
	DWORD	dwUsableWeaponList[MAX_USABLE_WEAPON];	// 탄환을 사용할 수 있는 무기의 리스트

public :

	inline	BYTE	GetAttribute( void )		{ return byAttribute; }
	inline	float	GetIncreasePointOfStrikingPower( void )	{ return fIncreasePointOfStrikingPower; }
	inline	DWORD	GetMaxDP( void )			{ return dwMaxDP; }

	inline	BYTE	GetUsableWeaponNum( void )	{ return byUsableWeaponNum; }
	inline	DWORD *	GetUsableWeaponList( void )	{ return dwUsableWeaponList; }
	inline	DWORD	GetUsableWeaponDTIndex( BYTE pi_byOrderNo )	{ return dwUsableWeaponList[pi_byOrderNo]; }

	BULLET_DATA()
	{
		byAttribute						= 0;
		fIncreasePointOfStrikingPower	= 0.0f;
		dwMaxDP							= 0;

		byUsableWeaponNum				= 0;
		for( int i = 0; i < MAX_USABLE_WEAPON; ++i )
			dwUsableWeaponList[i]		= ID_INVALID;
	}

	BOOL IsUsableWeapon( DWORD pi_dwWeaponDTCode )
	{
		for( int i = 0; i < byUsableWeaponNum; ++i )
		{
			if( dwUsableWeaponList[i] == pi_dwWeaponDTCode )
				return TRUE;
		}

		return FALSE;
	}

	DWORD GetSize( void ) { return sizeof( BULLET_DATA ); }
};

//------------------------------------------------------------------------------

struct FORCE_ITEM_DATA : public ITEM_DATA
{
//protected :

	DWORD	dwForceDTCode;				// 포스 아이템이 가지고 있는 포스의 DT Code

public :

//	inline	DWORD	GetForceDTCode( void )		{ return dwForceDTCode; }

	FORCE_ITEM_DATA()
	{
		dwForceDTCode	= ID_INVALID;
	}
};

//------------------------------------------------------------------------------

#define MAX_UNIT_FRAME			( UNIT_FRAME_CODE_NUM )

#define MAX_UNIT_PART_TYPE		( UNIT_PART_NUM )
#define UPI_HELMET				( unit_part_head )
#define UPI_UPPER_PART			( unit_part_upper )
#define UPI_LOWER_PART			( unit_part_lower )
#define UPI_ARMS				( unit_part_arms )
#define UPI_SHOULDER			( unit_part_shoulder )
#define UPI_BACKPACK			( unit_part_back )

#define MAX_UNIT_BULLET_TYPE	( UNIT_BULLET_NUM )
#define UBI_ARMS_TYPE			( unit_bullet_arms )
#define UBI_SHOULDER_TYPE		( unit_bullet_shoulder )

#define MAX_BACKPACK_BULLET_NUM	( max_unit_spare )

struct UNIT_FRAME_DATA : public ITEM_DATA
{
//protected :

	BYTE	byFrameType;							// goliath, nomade, etc..

	float	fSpeed[2];								// 프레임의 실제 움직임 속도 ( [0] = walk, [1] = run )
	BYTE	byHeight;								// 프레임의 키
	BYTE	byWidth;								// 프레임의 폭
	DWORD	dwMaxDP;								// 최대 내구도 ( DP = Durability Point ) ( == HP )

	DWORD	dwRepairPrice;							// 프레임의 수리비

	DWORD	dwBasePartID[MAX_UNIT_PART_TYPE];
	DWORD	dwBaseBulletID[MAX_UNIT_BULLET_TYPE];
};

struct UNIT_PART_DATA : public ITEM_DATA
{
	enum { MAX_UNIT_EFFECT = 4 };
	enum { MAX_UNIT_EQUABLE_EXPERTNESS = 2 };

	BYTE	byPartType;								// head, upper, etc...
	BYTE	byEquipableFrame[MAX_UNIT_FRAME];

	int		nMoveSpeed;
	DWORD	dwDefencePower;							// 방어력 ( 모든 파츠 )
	WORD	wResist[MAX_ITEM_ATTRIBUTE];			// 내성 ( 모든 파츠 )
	BYTE	byDefenceExpertness;					// 방어기술 숙련도 ( 방어기술 숙련도는 헬멧과 상체만을 더한다. )

	BYTE	byMaxEffectNum;							// 아이템에 걸려있는 이펙트의 갯수
	BYTE	byEffectType[MAX_UNIT_EFFECT];			// 아이템에 걸려있는 이펙트의 타입
	float	fEffectValue[MAX_UNIT_EFFECT];			// 아이템에 걸려있는 이펙트의 값

	BYTE	byEquableLevel;							// 장착 가능 레벨
	BYTE	byEquableExpertnessType[MAX_UNIT_EQUABLE_EXPERTNESS];	// 장착 가능 숙련도
	BYTE	byEquableExpertnessValue[MAX_UNIT_EQUABLE_EXPERTNESS];

	DWORD	dwRepairPrice;							// 각 파트의 수리비
};

struct UNIT_WEAPON_DATA : public UNIT_PART_DATA
{
//protected :

	float	fAttackDistance;
	DWORD	dwAttackDelay;
	BYTE	byRangeType;

	DWORD	dwMinStrikingPower;						// 최소 일반 공격력
	DWORD	dwMaxStrikingPower;						// 최대 일반 공격력

	BYTE	byAttackExpertness;						// 공격기술 숙련도

	DWORD	byBulletType;							// 사용가능한 발사물의 타입 ( arms, shoulder )
	DWORD	dwBulletUsingQuantity;					// 1회 사용시 소모되는 발사물의 양
	BYTE	byMaxBulletSocket;						// 발사물 소켓의 수
};

struct UNIT_BACKPACK_DATA : public UNIT_PART_DATA
{
//protected :

	BYTE	byMaxBP;								// Maximum Boost Point
	BYTE	byBoosterSpeed;
	BYTE	byMaxBulletSocket;						// 발사물 소켓의 수
};

struct UNIT_BULLET_DATA : public UNIT_PART_DATA
{
	DWORD	dwMaxDP;

	BYTE	byBulletType;							// 발사물 타입
	DWORD	dwStrikingPower;						// 공격력
};

//------------------------------------------------------------------------------

struct BATTERY_DATA : public ITEM_DATA
{
//protected :

	DWORD	dwMaxDP;								// 최대 내구도 ( DP = Durability Point )

public :

	inline	DWORD	GetMaxDP( void )	{ return dwMaxDP; }

	BATTERY_DATA()
	{
		dwMaxDP	= 0;
	}
};

//------------------------------------------------------------------------------

struct ORE_DATA : public ITEM_DATA
{
	BYTE	byIngotMinNum;							// 가공시 나올 수 있는 잉갓의 최소 개수
	BYTE	byIngotMaxNum;							// 가공시 나올 수 있는 잉갓의 최대 개수
	DWORD	dwOreProcPrice;							// 가공 비용

	ORE_DATA()
	{
		byIngotMinNum	= 1;
		byIngotMaxNum	= 1;
		dwOreProcPrice	= 0;
	}
};

//------------------------------------------------------------------------------

struct INGOT_DATA : public ITEM_DATA
{
//protected :

	BYTE	byEffectType;							// 잉갓의 이펙트 타입
	float	fEffectValue;							// 잉갓의 이펙트 값
	float	fEffectMaxValue;						// 잉갓의 이펙트 최대값

	BOOL	bIsUpgradeItem;							// 업그레이드 아이템인가?

	DWORD	dwUpgradeAttributeType;					// 업그레이드 아이템의 속성 타입
	BYTE	byUpgradeInsertLimitNum[8];				// 업그레이드 부위마다 제한된 삽입 갯수 ( 총 8 부분에 넣을 수 있음 )
	BYTE	byUpgradeEffectType;					// 업그레이드 효과
	float	fUpgradeEffectConstant[MAX_ITEM_LEVEL];	// 업그레이드 상수값 ( 수치 )

	float	fUpgradeJewelConstant;					// 보석 상수값

public :

	inline BYTE		GetEffectType( void )							{ return byEffectType; }
	inline float	GetEffectValue( void )							{ return fEffectValue; }
	inline float	GetEffectMaxValue( void )						{ return fEffectMaxValue; }

	inline	BYTE	IsUpgradeItem( void )							{ return bIsUpgradeItem; }
	inline	DWORD	GetUpgradeAttributeType( void )					{ return dwUpgradeAttributeType; }
	inline	BYTE	GetUpgradeInsertLimitNum( BYTE pi_byItemType )	{ return byUpgradeInsertLimitNum[pi_byItemType]; }
	inline	BYTE	GetUpgradeEffectType( void )					{ return byUpgradeEffectType; }
	inline	float	GetUpgradeEffectConstant( BYTE pi_byOrderNo )	{ if( pi_byOrderNo >= MAX_ITEM_LEVEL ) return 0.0f; else return fUpgradeEffectConstant[pi_byOrderNo]; }
	inline	float	GetUpgradeJewelConstant( void )					{ return fUpgradeJewelConstant; }

	INGOT_DATA()
	{
		byEffectType			= ID_INVALID;
		fEffectValue			= 0.0f;
		fEffectMaxValue			= 0.0f;

		bIsUpgradeItem			= FALSE;

		dwUpgradeAttributeType	= ID_INVALID;
		ZeroMemory( byUpgradeInsertLimitNum, 8 );
		byUpgradeEffectType		= ID_INVALID;

		for( int i = 0; i < MAX_ITEM_LEVEL; ++i )
			fUpgradeEffectConstant[i] = 0;

		fUpgradeJewelConstant	= 0.0f;
	}
};

//------------------------------------------------------------------------------

struct RING_DATA : public EQUIPMENT_DATA
{
//protected :

	WORD	wResist[MAX_ITEM_ATTRIBUTE];	// 내성

public :

	inline	WORD	GetResist( BYTE pi_byOrderNo )	{ return wResist[pi_byOrderNo]; }

	RING_DATA()
	{
		for( int i = 0; i < MAX_ITEM_ATTRIBUTE; ++i )
			wResist[i]	= 0;
	}
};

//------------------------------------------------------------------------------

struct MAP_DATA : public ITEM_DATA
{
//protected :

	char	pMapName[MAX_NAME_LENGTH];

public :

	inline	char *	GetMapName( void )				{ return pMapName; }

	MAP_DATA()
	{
		ZeroMemory( pMapName, MAX_NAME_LENGTH );
	}
};

//------------------------------------------------------------------------------

struct RETURN_ITEM_DATA : public ITEM_DATA
{
//protected :

	char	pMapName[MAX_NAME_LENGTH];
	char	pPortalName[MAX_NAME_LENGTH];

public :

	char *	GetMapName( void )		{ return pMapName; }
	char *	GetPortalName( void )	{ return pPortalName; }

	RETURN_ITEM_DATA()
	{
		ZeroMemory( pMapName, MAX_NAME_LENGTH );
		ZeroMemory( pPortalName, MAX_NAME_LENGTH );
	}
};

//------------------------------------------------------------------------------

struct DUNGEON_ITEM_DATA : public ITEM_DATA
{
	enum { DEL_NONE, DEL_PARTY, MAX_DEL_TYPE };
	enum { DCC_KILL_ALL_MONSTER, DCC_KILL_SPECIFY_MONSTER, MAX_DCC_TYPE };

//protected :

	BYTE	m_byEnterLimitation;		// 던젼에 들어갈 수 있는 조건
	DWORD	m_dwClearTime;				// 던젼 클리어 시간
	BYTE	m_byClearCondition;			// 던젼 클리어 조건
	DWORD	m_byClearValue;				// 던젼 클리어 조건에 따른 값
	DWORD	m_dwTargetMonsterTotalNum;	// 던젼내 미션의 목표가되는 몬스터의 총 수
	DWORD	m_dwDungeonDescDTIndex;
	DWORD	m_dwPrizeDalant;
	DWORD	m_dwPrizeGold;
	DWORD	m_dwPrizeExp;

public :

	inline	BYTE	GetEnterLimitation( void )		{ return m_byEnterLimitation; }
	inline	DWORD	GetClearTime( void )			{ return m_dwClearTime; }
	inline	BYTE	GetClearCondition( void )		{ return m_byClearCondition; }
	inline	DWORD	GetClearValue( void )			{ return m_byClearValue; }
	inline	DWORD	GetTargetMonsterTotalNum( void ){ return m_dwTargetMonsterTotalNum; }
	inline	DWORD	GetDungeonDescDTIndex( void )	{ return m_dwDungeonDescDTIndex; }
	inline	DWORD	GetPrizeDalant( void )			{ return m_dwPrizeDalant; }
	inline	DWORD	GetPrizeGold( void )			{ return m_dwPrizeGold; }
	inline	DWORD	GetPrizeExp( void )				{ return m_dwPrizeExp; }

public :

	DUNGEON_ITEM_DATA()
	{
		m_byEnterLimitation			= DEL_NONE;
		m_dwClearTime				= 0;
		m_byClearCondition			= DCC_KILL_ALL_MONSTER;
		m_byClearValue				= 0;
		m_dwTargetMonsterTotalNum	= 0;
		m_dwDungeonDescDTIndex		= ID_INVALID;
		m_dwPrizeDalant				= 0;
		m_dwPrizeGold				= 0;
		m_dwPrizeExp				= 0;
	}
};

//------------------------------------------------------------------------------

#define MAX_ITEM_MATERIAL_NUM	( 5 )

struct GUARD_TOWER_DATA : public ITEM_DATA
{
	DWORD	dwCharMeshID;					// 가드타워를 설치했을 때의 Mesh ID

	BYTE	byLevel;						// 가드 타워의 레벨
	BYTE	byHeight;						// 가드 타워의 키
	BYTE	byWidth;						// 가드 타워의 폭

	BYTE	byUsableLevel;					// 사용 가능 레벨
	BYTE	byUsableExpertness;				// 사용 가능 숙련도

	DWORD	dwBuildTime;					// 가드 타워 완성 시간

	DWORD	dwMinStrikingPower;				// 최소 일반 공격력
	DWORD	dwMaxStrikingPower;				// 최대 일반 공격력
	BYTE	byEffectType;					// 가드 타워의 공격 효과 타입
	BYTE	byAttributeType;				// 아이템의 속성타입 ( 0 = 불, 1 = 물, 2 = 흙, 3 = 바람 )

	DWORD	dwDefencePower;					// 방어력
	BYTE	wResist[MAX_ITEM_ATTRIBUTE];	// 내성
	float	fAttackDistance;				// 공격 거리
	DWORD	dwAttackDelay;					// 공격 딜레이

	DWORD	dwMaxDP;						// 최대 내구도 ( DP = Durability Point )

	BYTE	byMaterialNum;								// 재료의 총 갯수
	BYTE	byMaterialType[MAX_ITEM_MATERIAL_NUM];		// 아이템의 재료가 되는 광물의 Type
	DWORD	dwMaterialDTIndex[MAX_ITEM_MATERIAL_NUM];	// 아이템의 재료가 되는 광물의 DT Index
	DWORD	dwMaterialDTCode[MAX_ITEM_MATERIAL_NUM];	// 아이템의 재료가 되는 광물의 DT Code
	DWORD	dwMaterialNum[MAX_ITEM_MATERIAL_NUM];		// 아이템의 재료가 되는 광물의 갯수

	DWORD	dwAttackEffectMaterial;						// 이펙트에서만 사용됨
	DWORD	dwDefenceEffectMaterial;					// 이펙트에서만 사용됨

	GUARD_TOWER_DATA()
	{
		int nStart	= ( int )this + sizeof( ITEM_DATA );
		int nSize	= sizeof( GUARD_TOWER_DATA ) - sizeof( ITEM_DATA );
		ZeroMemory( ( void * )nStart, nSize );

		for( int i = 0; i < MAX_ITEM_MATERIAL_NUM; ++i )
			byMaterialType[i]	= ID_INVALID;

		dwAttackEffectMaterial	= ID_INVALID;
		dwDefenceEffectMaterial	= ID_INVALID;
	}
};

//------------------------------------------------------------------------------

typedef ITEM_DATA			FACE_DATA;
typedef ARMOR_DATA			HELMET_DATA;
typedef ARMOR_DATA			UPPER_PART_DATA;
typedef ARMOR_DATA			LOWER_PART_DATA;
typedef ARMOR_DATA			GLOVES_DATA;
typedef ARMOR_DATA			SHOES_DATA;
typedef ARMOR_DATA			SHIELD_DATA;
typedef ITEM_DATA			ETC_DATA;
typedef ITEM_DATA			WORKTOOL_DATA;
typedef ITEM_DATA			LOOT_DATA;
typedef RING_DATA			AMULET_DATA;
typedef ARMOR_DATA			CLOAK_DATA;
typedef ITEM_DATA			ANIMUS_DATA;
typedef UNIT_PART_DATA		UNIT_HELMET_DATA;
typedef UNIT_PART_DATA		UNIT_UPPER_PART_DATA;
typedef UNIT_PART_DATA		UNIT_LOWER_PART_DATA;
typedef UNIT_WEAPON_DATA	UNIT_ARMS_DATA;
typedef UNIT_WEAPON_DATA	UNIT_SHOULDER_DATA;

//------------------------------------------------------------------------------

struct ITEM_WORK_DATA : public BASE_DATA
{
//protected :

	DWORD	dwDTIndex;									// ITEM_WORKING_DATA 자체의 DT Index
	BYTE	byItemType;									// 만들어지는 아이템의 Type
	DWORD	dwItemDTIndex;								// 만들어지는 아이템의 DT Index
	DWORD	dwItemDTCode;								// 만들어지는 아이템의 DT Code
	DWORD	dwItemMeshID;								// 만들어지는 아이템의 Mesh ID
	DWORD	dwItemIconID;								// 만들어지는 아이템의 Icon ID
	BYTE	byWorkableExp;								// 아이템을 만들 수 있는 숙련도
	BYTE	byUsableRace[8];							// 아이템을 사용할 수 있는 ( 만들 수 있는 종족 )

	BYTE	byTotalMaterialNum;							// 재료의 총 갯수
	BYTE	byMaterialType[MAX_ITEM_MATERIAL_NUM];		// 아이템의 재료가 되는 광물의 Type
	DWORD	dwMaterialDTIndex[MAX_ITEM_MATERIAL_NUM];	// 아이템의 재료가 되는 광물의 DT Index
	DWORD	dwMaterialDTCode[MAX_ITEM_MATERIAL_NUM];	// 아이템의 재료가 되는 광물의 DT Code
	DWORD	dwMaterialNum[MAX_ITEM_MATERIAL_NUM];		// 아이템의 재료가 되는 광물의 갯수

public :

	inline	DWORD	GetDTIndex( void )					{ return dwDTIndex; }
	inline	BYTE	GetItemType( void )					{ return byItemType; }
	inline	DWORD	GetItemDTIndex( void )				{ return dwItemDTIndex; }
	inline	DWORD	GetItemDTCode( void )				{ return dwItemDTCode; }
	inline	DWORD	GetItemMeshID( void )				{ return dwItemMeshID; }
	inline	DWORD	GetItemIconID( void )				{ return dwItemIconID; }
	inline	BYTE	GetWorkableExp( void )				{ return byWorkableExp; }
	inline	BOOL	IsUsableRace( BYTE pi_byRace )		{ return byUsableRace[pi_byRace]; }
	inline	BYTE	GetTotalMaterialNum( void )			{ return byTotalMaterialNum; }
	inline	BYTE	GetMaterialType( BYTE pi_byNo )		{ return byMaterialType[pi_byNo]; }
	inline	DWORD	GetMaterialDTIndex( BYTE pi_byNo )	{ return dwMaterialDTIndex[pi_byNo]; }
	inline	DWORD	GetMaterialDTCode( BYTE pi_byNo )	{ return dwMaterialDTCode[pi_byNo]; }
	inline	DWORD	GetMaterialNum( BYTE pi_byNo )		{ return dwMaterialNum[pi_byNo]; }

	ITEM_WORK_DATA()
	{
		dwDTIndex		= ID_INVALID;
		byItemType		= ID_INVALID;
		dwItemDTIndex	= ID_INVALID;
		dwItemDTCode	= ID_INVALID;
		dwItemMeshID	= ID_INVALID;
		dwItemIconID	= ID_INVALID;
		byWorkableExp	= 0;
		ZeroMemory( &byUsableRace, 8 );

		byTotalMaterialNum	= 0;
		for( int i = 0; i < MAX_ITEM_MATERIAL_NUM; ++i )
		{
			byMaterialType[i]		= ID_INVALID;
			dwMaterialDTIndex[i]	= ID_INVALID;
			dwMaterialDTCode[i]		= ID_INVALID;
			dwMaterialNum[i]		= 0;
		}
	}
};

//============================================================================//
//                                  Boundary                                  //
//============================================================================//

class CItemDescription
{
	friend CItemDataMgr;

	DWORD	m_dwDTIndex;
	DWORD	m_dwDTCode;

	char *	m_pDescription[MAX_LANGUAGE_TYPE];
	int		m_nDescriptionLength[MAX_LANGUAGE_TYPE];

public :

	CItemDescription()
	{
		ZeroMemory( this, sizeof( CItemDescription ) );

		m_dwDTIndex	= ID_INVALID;
		m_dwDTCode	= ID_INVALID;
	}

	~CItemDescription()
	{
		for( int i = 0; i < MAX_LANGUAGE_TYPE; ++i )
			delete [] m_pDescription[i];
	}
};

class CDungeonDescription
{
	friend CItemDataMgr;

public :
	enum { DDT_BRIEFING, MAX_DDT_TYPE };

private :

	DWORD	m_dwDTIndex;
	DWORD	m_dwDTCode;

	char *	m_pDescription[MAX_DDT_TYPE][MAX_LANGUAGE_TYPE];
	int		m_nDescriptionLength[MAX_DDT_TYPE][MAX_LANGUAGE_TYPE];

public :

	CDungeonDescription()
	{
		ZeroMemory( this, sizeof( CDungeonDescription ) );

		m_dwDTIndex	= ID_INVALID;
		m_dwDTCode	= ID_INVALID;
	}

	~CDungeonDescription()
	{
		for( int i = 0; i < MAX_DDT_TYPE; ++i )
			for( int j = 0; j < MAX_LANGUAGE_TYPE; ++j )
				delete [] m_pDescription[i][j];
	}
};

////////////////////////////////////////////////////////////////////////////////
//////////////////////////                            //////////////////////////
//////////////////////////         Item Data          //////////////////////////
//////////////////////////                            //////////////////////////
////////////////////////////////////////////////////////////////////////////////

/*//////////////////////////////////////////////////////////////////////////////

[ CItemDataMgr ]

  - 아이템의 데이터 테이블에서 읽은 데이터를 관리한다.

//////////////////////////////////////////////////////////////////////////////*/

class CBoardItem;

class CItemDataMgr
{
private :
	CDataMgr				m_listData[MAX_ITEM_TYPE];
	DWORD					m_pItemWorkData_ItemTypeStartDTIndex[MAX_ITEM_TYPE];	// 아이템 제작 데이터의 아이템 별 경계가 되는 DTIndex
	DWORD					m_pItemWorkDataNumByItemType[MAX_ITEM_TYPE];			// 아이템 제작 데이터의 아이템 별 갯수

	CItemDescription *		m_listItemDescription[MAX_ITEM_TYPE];
	DWORD					m_dwTotalItemDescriptionNum[MAX_ITEM_TYPE];

	CDungeonDescription *	m_listDungeonDescription;
	DWORD					m_dwTotalDungeonDescriptionNum;

public :
	CItemDataMgr();
	~CItemDataMgr();

	void		Init( void );
	BOOL		Create( void );
	BOOL		Destroy( void );

	//--------------------------------------------------------------------------

	BOOL		LoadData( void );
	BOOL		UnloadData( BYTE pi_dwItemTypeID );

	//--------------------------------------------------------------------------

	BASE_DATA *	GetData( DWORD pi_dwItemTypeID, DWORD pi_dwIndex );
	//BOOL		GetData( DWORD pi_dwItemTypeID, DWORD pi_dwIndex, CItem & pio_cItem, BOOL pi_bIsLoadResource, BYTE pi_byRaceCode );
	//BOOL		GetData( DWORD pi_dwItemTypeID, DWORD pi_dwIndex, CItem * pio_pItem, BOOL pi_bIsLoadResource, BYTE pi_byRaceCode );
	BOOL		GetData( DWORD pi_dwItemTypeID, DWORD pi_dwIndex, CBoardItem & pio_pItem, BOOL pi_bIsLoadResource, BYTE pi_byRaceCode );
	BOOL		GetData( DWORD pi_dwItemTypeID, DWORD pi_dwIndex, CBoardItem * pio_pItem, BOOL pi_bIsLoadResource, BYTE pi_byRaceCode );

	BASE_DATA *	GetDataByDTCode( DWORD pi_dwItemTypeID, DWORD pi_dwDTCode );
//	BOOL		GetDataByDTCode( DWORD pi_dwItemTypeID, DWORD pi_dwDTCode, CItem & pio_cItem, BOOL pi_bIsLoadResource );
//	BOOL		GetDataByDTCode( DWORD pi_dwItemTypeID, DWORD pi_dwDTCode, CItem * pio_pItem, BOOL pi_bIsLoadResource );
	BOOL		GetDataByDTCode( DWORD pi_dwItemTypeID, DWORD pi_dwDTCode, CBoardItem & pio_cItem, BOOL pi_bIsLoadResource );
	BOOL		GetDataByDTCode( DWORD pi_dwItemTypeID, DWORD pi_dwDTCode, CBoardItem * pio_pItem, BOOL pi_bIsLoadResource );

	BASE_DATA *	GetDataByMeshID( DWORD pi_dwItemTypeID, DWORD pi_dwMeshID );

	char *		GetItemDescription( DWORD pi_dwItemTypeID, DWORD pi_dwDTIndex );
	char *		GetDungeonDescription( DWORD pi_dwDTIndex, BYTE pi_byDescType );

	//--------------------------------------------------------------------------

	int			GetTotalDataNum( DWORD pi_dwItemTypeID );

	//--------------------------------------------------------------------------

			ITEM_WORK_DATA *	GetItemWorkData( DWORD pi_dwItemTypeID, DWORD pi_dwIndex );
			ITEM_WORK_DATA *	GetItemWorkDataByMeshID( DWORD pi_dwMeshID );
	inline	DWORD				GetItemWorkDataStartDTIndex( DWORD pi_dwItemTypeID )	{ return m_pItemWorkData_ItemTypeStartDTIndex[pi_dwItemTypeID]; }
	inline	DWORD				GetItemWorkDataNum( DWORD pi_dwItemTypeID )				{ return m_pItemWorkDataNumByItemType[pi_dwItemTypeID]; }

private :

	BOOL		LoadData_ItemDescription( CDataString * l_pSourceData );
	BOOL		LoadData_DungeonDescription( CDataString * l_pSourceData );
};

#endif // __ITEM_DATA_MGR_H__