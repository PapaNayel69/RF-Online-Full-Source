////////////////////////////////////////////////////////////////////////////
//
// Tooltip parameter structuctor
//
////////////////////////////////////////////////////////////////////////////

#include "../GameWindow/GUICharInfoWindow.h"

#define	SHOP_MODE_BUY		0x00
#define	SHOP_MODE_SELL		0x01
#define	SHOP_MODE_REPAIR	0x02
#define	SHOP_MODE_NONE		0xFF

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

// 무기 
struct TOOLTIP_WEAPON {
	char		pNameStr[32];			// 이름	
	WORD		wLevel;					// 사용 가능 레벨		

	BYTE		m_byNeedMastery[DEFFENCE_MASTERY+1];	// 요구 숙련도	

	DWORD		dwCurDurability;		// 현재 내구도
	DWORD		dwRepairDurability;		// 수리 가능 내구도
	DWORD		dwMaxDurability;		// 최대 내구도	

	BOOL		bRepairable;			// 수리 가능 여부

	DWORD		dwMinGenAttPower[2];	// 최소 일반 공격력
	DWORD		dwMaxGenAttPower[2];	// 최대 일반 공격력

	DWORD		dwMinForceAttPower[2];	// 최소 포스 공격력
	DWORD		dwMaxForceAttPower[2];	// 최대 포스 공격력	

	BYTE		byAttribute;			// 속성

	WORD		wUpgradeLevel;			// 업그레이드 단계
	BYTE		byUpgradeSocketNo;		// 최대 업그레이드 소켓 갯수
	DWORD		dwUpgradeSpriteID[8];	// 업그레이드 item sprite id	

	// 부가속성

	BYTE		byPriceType;		
	DWORD		dwPrice;				// 가격


	char		pDescription[128];		// 설명
};


// 방어구
struct TOOLTIP_ARMOR {
	char		pNameStr[32];			// 이름	
	WORD		wLevel;					// 사용 가능 레벨		

	BYTE		m_byNeedMastery[DEFFENCE_MASTERY+1];	// 요구 숙련도


	DWORD		dwCurDurability;		// 현재 내구도
	DWORD		dwRepairDurability;		// 수리 가능 내구도
	DWORD		dwMaxDurability;		// 최대 내구도	

	BOOL		bRepairable;			// 수리 가능 여부

	DWORD		dwDeffensePower[2];		// 방어력	

	BYTE		byToleranceType[4];		// 내성 타입
	DWORD		dwTolerance[4];			// 내성 수치


	WORD		wUpgradeLevel;			// 업그레이드 단계
	BYTE		byUpgradeSocketNo;		// 최대 업그레이드 소켓 갯수
	DWORD		dwUpgradeSpriteID[8];	// 업그레이드 item sprite id	

	// 부가속성

	BYTE		byPriceType;		
	DWORD		dwPrice;				// 가격

	char		pDescription[128];		// 설명
};

// 탄환
struct TOOLTIP_BULLET
{
	char		pNameStr[32];			// 이름		
	DWORD		dwRestQuantity;			// 잔량

	DWORD		dwMinAttackPower;		// 최소 공격력
	DWORD		dwMaxAttackPower;		// 최대 공격력	

	char		pUsableWeapon[18][32];	// 사용 가능한 무기	
		

	BYTE		byAttribute;			// 속성

	BYTE		byAddAttribute;			// 부가 속성	

	char		pDescription[128];		// 설명

	BYTE		byPriceType;		
	DWORD		dwPrice;				// 가격
};

// 포션
struct TOOLTIP_POTION
{
	char		pNameStr[32];		// 이름
	char		pKindStr[32];		// 포션 종류
	char		pRaceStr[16];		// 사용 가능 종족

	DWORD		dwReoveryPoint;		// 회복량

	WORD		wQuantity;			// 수량

	char		pDescription[128];	// 설명	

	BYTE		byPriceType;		
	DWORD		dwPrice;			// 가격	
};

// 배터리
struct TOOLTIP_BATTERY
{
	char		pNameStr[32];		// 이름	

	DWORD		dwQuantity;			// 내구도	
	
	char		pDescription[128];	// 설명

	BYTE		byPriceType;		
	DWORD		dwPrice;			// 가격
};

// 장신구
struct TOOLTIP_ACCESSORY
{
	char		pNameStr[32];		// 이름	
	WORD		wLevel;				// 사용 가능 레벨	

	BYTE		byToleranceType[4];	// 내성 타입
	DWORD		dwTolerance[4];		// 내성 수치

	char		pEffectStr[128];		// 효과


	char		pDescription[128];	// 설명	

	// 부가 속성

	BYTE		byPriceType;		
	DWORD		dwPrice;			// 가격	
};

// 포스 아이템
struct TOOLTIP_FORCE_ITEM
{
	char		pNameStr[32];		// 이름
	BYTE		byClass;			// 계열
	WORD		wLevel;				// 레벨
	char		pDescription[128];	// 설명

	BYTE		byPriceType;		
	DWORD		dwPrice;			// 가격
};

// 유닛 아이템
struct TOOLTIP_UNIT_ITEM
{
	char		pNameStr[32];		// 이름	
	
	BYTE		byAttackType;		// 공격타입

	char		pMoveSpeed[32];		// 이동속도

	DWORD		dwMinAttackPower;	// 최소 공격력
	DWORD		dwMaxAttackPower;	// 최대 공격력	

	DWORD		dwDeffensePower;	// 방어력	

	DWORD		dwAttackDelay;		// 공격 딜레이	

	BYTE		byToleranceType[4];	// 내성 타입
	DWORD		dwTolerance[4];		// 내성 수치

	DWORD		dwCurDurability;	// 현재 내구도
	DWORD		dwMaxDurability;	// 최대 내구도

	char		pDescription[128];	// 설명

	BYTE		byPriceType;		
	DWORD		dwPrice;			// 가격
};

struct TOOLTIP_ETC_ITEM
{
	char		pNameStr[32];		// 이름	

	DWORD		dwQuantity;			// 수량	
	
	char		pDescription[128];	// 설명

	BYTE		byPriceType;		
	DWORD		dwPrice;			// 가격
};

struct TOOLTIP_ANIMUS_LIVER_ITEM
{
	char		pNameStr[32];		// 이름

	char		pDescription[128];	// 설명

	BYTE		byPriceType;		
	DWORD		dwPrice;			// 가격
};

// 유닛 파츠
struct TOOLTIP_UNIT_PARTS
{
	BYTE		byPartsType;

	char		pNameStr[32];

	BYTE		byEquableLevel;		// 장작 가능 레벨

	DWORD		dwMinAttackPower;	// 최소 공격력
	DWORD		dwMaxAttackPower;	// 최대 공격력	

	DWORD		dwDeffensePower;	// 방어력
	
	WORD		dwMoveSpeed;		// 이동속도

	BYTE		byAttackType;		// 공격타입
	DWORD		dwAttackDelay;		// 공격딜레이
	BYTE		byAttribute;		// 속성

	BYTE		byToleranceType[4];	// 내성 타입
	DWORD		dwTolerance[4];		// 내성 수치

	DWORD		dwDurability;		// pack인경우 부스터 차지량, bullet인경우 남은 탄환
	BYTE		byMaxBulletNum;		// bullet인경우 탄창 갯수

	BYTE		byBoosterSpeed;		// 부스터 속도

	BYTE		byPriceType;		
	DWORD		dwPrice;			// 가격
};

// 스킬포스
struct TOOLTIP_SF
{
	char		pNameStr[32];			// 이름	
	
	BYTE		byClassID;				// 분류
	BYTE		byLvID;					// 단계
	
	BYTE		byLevel;				// 레벨
	WORD		wMastery;				// 숙련도

	char		pUsableWeapon[10][32];	// 사용 가능한 무기	
	BYTE		byNeedFP;				// 필요 포스량
	float		fDelay;					// 딜레이	
	BYTE		byAttribute;			// 속성 (포스만)
	char		pDescription[128];		// 설명
};

struct TOOLTIP_SF_INFO
{
	BYTE		byClassID;				// 분류
	BYTE		byLvID;					// 단계

	char		pSFNameStr[10][32];		// 단계에 포함되는 스킬, 포스의 이름들
	BYTE		byLevel[10];			// 각 스킬, 포스별 레벨
};

struct TOOLTIP_ANIMUS
{
	char		pNameStr[32];

	BYTE		byType;

	BYTE		byLevel;

	BYTE		byGrade;			// 애니머스 상태 등급

	WORD		wUsableMastery;		// 사용 가능 소환 숙련도

	DWORD		dwMinAttPower;		// 공격력
	DWORD		dwMaxAttPower;

	DWORD		dwDeffensePower;	// 방어력

	BYTE		byCombatAttribute;	// 속성

	BYTE		byToleranceType[4];	// 내성 타입
	DWORD		dwTolerance[4];		// 내성 수치

	BYTE		byNeedFP;			// 필요 포스량

	char		pDescription[128];
};

////////////////////////////////////////////////////////////////////////////////
///////////////////////////                          ///////////////////////////
///////////////////////////         Boundary         ///////////////////////////
///////////////////////////                          ///////////////////////////
////////////////////////////////////////////////////////////////////////////////

// Tooltip Fucntion

// 아이템별 툴팁
void	SetTooltip_Weapon( CGUIItem * pi_pItem, TOOLTIP_WEAPON * pi_pItemInfo, BYTE pi_byShopMode );
void	SetTooltip_Armor( CGUIItem * pi_pItem, TOOLTIP_ARMOR * pi_pItemInfo, BYTE pi_byShopMode );
void	SetTooltip_Shield( CGUIItem * pi_pItem, TOOLTIP_ARMOR * pi_pItemInfo, BYTE pi_byShopMode );
void	SetTooltip_Bullet( CGUIItem * pi_pItem, TOOLTIP_BULLET *	pi_pItemInfo, BYTE pi_byShopMode );
void	SetTooltip_Potion( CGUIItem * pi_pItem, TOOLTIP_POTION * pi_pItemInfo, BYTE pi_byShopMode );
void	SetTooltip_Battery( CGUIItem * pi_pItem, TOOLTIP_BATTERY * pi_pItemInfo, BYTE pi_byShopMode );
void	SetTooltip_Accessory( CGUIItem * pi_pItem, TOOLTIP_ACCESSORY * pi_pItemInfo, BYTE pi_byShopMode );
void	SetTooltip_ForceItem( CGUIItem * pi_pItem, TOOLTIP_FORCE_ITEM * pi_pItemInfo, BYTE pi_byShopMode );
void	SetTooltip_Mining( CGUIItem * pi_pItem, TOOLTIP_WEAPON * pi_pItemInfo, BYTE pi_byShopMode );
void	SetTooltip_UnitItem( CGUIItem * pi_pItem, TOOLTIP_UNIT_ITEM * pi_pItemInfo, BYTE pi_byShopMode );
void	SetTooltip_Etc( CGUIItem * pi_pItem, TOOLTIP_ETC_ITEM * pi_pItemInfo, BYTE pi_byShopMode, BOOL pi_bExistQuantity );
void	SetTooltip_AnimusLiver( CGUIItem * pi_pItem, TOOLTIP_ANIMUS_LIVER_ITEM * pi_pItemInfo, BYTE pi_byShopMode );


// 유닛 파츠
void	SetTooltip_UnitParts( CGUIItem * pi_pItem, TOOLTIP_UNIT_PARTS * pi_pItemInfo, BYTE pi_byShopMode );

// 스킬 포스 창
void	SetTooltip_Skill( CGUIItem * pi_pItem, TOOLTIP_SF * pi_pSFInfo );
void	SetTooltip_Force( CGUIItem * pi_pItem, TOOLTIP_SF * pi_pSFInfo );

// 스킬 포스 벨트창
void	SetTooltip_SF_Belt( CGUIItem * pi_pItem, TOOLTIP_SF * pi_pSFInfo, BYTE pi_bySFType );

// 캐릭터 정보창에 스킬 포스
void	SetTooltip_SkillInfo( CGUIObject * pi_pObject, TOOLTIP_SF_INFO * pi_pSkillInfo );
void	SetTooltip_ForceInfo( CGUIObject * pi_pObject, TOOLTIP_SF_INFO * pi_pForceInfo );

// 소환 툴팁
void	SetTooltip_Animus( CGUIObject * pi_pObject, TOOLTIP_ANIMUS * pi_pAnimusInfo );
void	SetTooltip_Animus_Belt( CGUIObject * pi_pObject, TOOLTIP_ANIMUS * pi_pAnimusInfo );


// 기타 일반 툴팁
void	SetTooltip( CGUIObject * pi_pObject, char * pi_pStr );

