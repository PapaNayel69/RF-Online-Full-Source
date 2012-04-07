#ifndef _ANALYSIS_AVATOR_DATA_H_
#define _ANALYSIS_AVATOR_DATA_H_

#include "CharacterDB.h"
#include "ItemLootTable.h"
#include "WeaponBulletLinkTable.h"
#include "OreCuttingTable.h"
#include "MapDataTable.h"
#include "ItemMakeDataTable.h"
#include "ItemUpgradeTable.h"

#define		V_	//Variable
#define		F_	//Fix
#define		H_	//Hide
#define		R_	//Reference

//아바타 불변 기본
struct __AVATOR_BASE	
{
	char		F_	szCharName[max_name_len+1];			//캐릭터이름
	char		F_	szWorldName[max_world_name_len+1];	//소속월드명
	char		F_	szRace[32];							//종족

	char		F_	szHair[32];							//머리
	char		F_	szFace[32];							//얼굴
	char		F_	szUpper[32];						//상의	
	char		F_	szPants[32];						//하의
	char		F_	szGauntlet[32];						//장갑
	char		F_	szShoes[32];						//신발

	__AVATOR_BASE() { Init();	}
	void Init() { memset(this, 0, sizeof(*this));	}
};

//아바타 일반 속성
struct __AVATOR_GENERAL
{
	BYTE		V_	byLevel;							//레벨
	double		V_	dExp;								//경험치
	DWORD		V_	dwGold;								//소지골드수
	DWORD		V_	dwDalant;							//소지달란트
	DWORD		V_	dwHP;								//HP
	DWORD		V_	dwFP;								//FP
	DWORD		V_	dwSP;								//SP
	BYTE		V_	byBagNum;							//가방수
	_COUNT_LV	V_	cntExpWp[WP_CLASS_NUM];				//숙련도
	_COUNT_LV	V_	cntDefence;							//Defence숙련도
	_COUNT_LV	V_	cntParrying;						//Parrying숙련도
	_COUNT_LV	V_  cntMakeCum[MI_CLASS_NUM];			//add.. 제작숙련도.. 0:갑옷, 1:방어구, 2:발사물
	_COUNT_LV	V_	cntSpecialCum;						//add.. 종족별특수숙련도	

	char		F_	szMapName[max_map_name_len+1];		//맵이름
	float		F_	fPos[3];							//위치

	__AVATOR_GENERAL() { Init();	}
	void Init() { memset(this, 0, sizeof(*this));	}
};

//스킬종류별능력
struct __AVATOR_SKILL
{
	char		F_	szSkillName[32];					//스킬이름
	WORD		H_	wRecIndex;							//스킬레코드의 인덱스
	BYTE		R_	byClass;							//클래스				
	_COUNT_LV	V_	cntExp;								//사용누적치	

	__AVATOR_SKILL() { Init();	}
	void Init() { memset(this, 0, sizeof(*this));	}
};

//포스마스터리별 능력
struct __AVATOR_FORCE
{
	char		F_	szMasteryName[32];					//마스터리이름
	BYTE		R_	byClass;							//클래스				
	DWORD		V_	dwExpCum;							//사용누적치	

	__AVATOR_FORCE() { Init();	}
	void Init() { memset(this, 0, sizeof(*this));	}
};

//장착 아이템
struct __ITEM_EQUIP
{
	bool		V_	bFix;								//사용슬롯여부
	char		V_	szItemCode[10];						//아이템코드
	char		F_	szItemName[64];						//아이템이름
	BYTE		H_	byTableCode;						//아이템종류코드
	WORD		H_	wRecIndex;							//레코드인덱스
	BYTE		R_	bySlotIndex;						//슬롯인덱스
	char		F_	szCivil[32];						//사용가능종족
	BYTE		F_	byEquipableLv;						//장착가능레벨
	DWORD		V_	dwAmount;							//수량
	BYTE		F_	byItemCurUpgSocketNum;				//현재 아이템 업그레이드 소켓수
	BYTE		V_	byItemLimUpgSocketNum;				//제한 아이템 업그레이드 소켓수
	BYTE		V_	byTalikIndex[MAX_ITEM_LV];			//붙여진 탈릭 인덱스	

	__ITEM_EQUIP() { Init();	}
	void Init() { 
		memset(this, 0, sizeof(*this));	
		memset(byTalikIndex, 0xFF, sizeof(byTalikIndex));			
	}
};

//인벤토리창의 아이템
struct __ITEM_INVEN	
{
	bool		V_	bFix;								//사용슬롯여부
	char		V_	szItemCode[10];						//아이템코드
	char		F_	szItemName[64];						//아이템이름
	BYTE		H_	byTableCode;						//아이템종류코드
	WORD		H_	wRecIndex;							//레코드인덱스
	BYTE		R_	bySlotIndex;						//슬롯인덱스
	char		F_	szCivil[32];						//사용가능종족
	BYTE		F_	byEquipableLv;						//장착가능레벨
	DWORD		V_	dwAmount;							//수량
	BYTE		F_	byItemCurUpgSocketNum;				//현재 아이템 업그레이드 소켓수
	BYTE		V_	byItemLimUpgSocketNum;				//제한 아이템 업그레이드 소켓수
	BYTE		V_	byTalikIndex[MAX_ITEM_LV];			//붙여진 탈릭 인덱스	

	__ITEM_INVEN() { Init();	}
	void Init() { memset(this, 0, sizeof(*this));	}
};

//포스창의 아이템
struct __ITEM_FORCE	
{
	bool		V_	bFix;								//사용슬롯여부
	char		V_	szForceCode[10];					//포스코드
	char		F_	szForceName[64];					//포스이름
	BYTE		R_	byClass;							//클래스
	_COUNT_LV	V_	cntExp;								//사용누적치	

	__ITEM_FORCE() { Init();	}
	void Init() { memset(this, 0, sizeof(*this));	}
};

//벨트창의 아이템
struct __ITEM_BELT
{
	bool		V_	bFix;								//사용슬롯여부
	char		V_	szItemCode[10];						//아이템코드
	char		F_	szItemName[64];						//아이템이름
	BYTE		H_	byTableCode;						//아이템종류코드
	WORD		H_	wRecIndex;							//레코드인덱스
	BYTE		R_	bySlotIndex;						//슬롯인덱스
	DWORD		V_	dwAmount;							//수량

	__ITEM_BELT() { Init();	}
	void Init() { memset(this, 0, sizeof(*this));	}
};

//남은 가공창의 아이템
struct __ITEM_RBUFFER	
{
	bool		F_	bFix;								//사용슬롯여부
	char		F_	szResCode[10];						//보석코드
	char		F_	szResName[64];						//보석이름
	WORD		F_	wRecIndex;							//레코드인덱스
	DWORD		F_	dwPrice;							//종족별가격
	DWORD		F_	dwAmount;							//수량

	__ITEM_RBUFFER() { Init();	}
	void Init() { memset(this, 0, sizeof(*this));	}
};

struct _AVATOR_VIEWER
{
	__AVATOR_BASE		m_Base;
	__AVATOR_GENERAL	m_General;
	__AVATOR_SKILL		m_Skill[max_skill_num];
	__AVATOR_FORCE		m_Force[force_mastery_num];
	__ITEM_EQUIP		m_Equip[equip_fix_num+embellish_fix_num];
	__ITEM_INVEN		m_Inven[bag_storage_num];
	__ITEM_FORCE		m_FItem[force_storage_num];
	__ITEM_BELT			m_Belt[potion_belt_num];
	__ITEM_RBUFFER		m_LeftRes[left_cutting_num];

	_AVATOR_VIEWER(){	InitData();	}
	
	void InitData()
	{
		m_Base.Init();
		m_General.Init();
		for(int i = 0; i < max_skill_num; i++)
			m_Skill[i].Init();
		for(i = 0; i < force_mastery_num; i++)
			m_Force[i].Init();
		for(i = 0; i < equip_fix_num+embellish_fix_num; i++)
			m_Equip[i].Init();
		for(i = 0; i < bag_storage_num; i++)
			m_Inven[i].Init();
		for(i = 0; i < force_storage_num; i++)
			m_FItem[i].Init();
		for(i = 0; i < potion_belt_num; i++)
			m_Belt[i].Init();
		for(i = 0; i < left_cutting_num; i++)
			m_LeftRes[i].Init();
	}
};

struct _WORLD_LIST
{
	bool m_bOpen;
	char m_szWorldName[max_world_name_len+1];
};

class CAnalysisAvatorData
{
private:

	bool					m_bOper;

	CMapDataTable			m_tblMapData;

	CRecordData				m_tblPlayerCharacter;
	CRecordData				m_tblMonsterCharacter;
	CRecordData				m_tblNPCharacter;
	CRecordData				m_tblExp;

	CWeaponBulletLinkTable  m_tblWeaponBulletLink;
	CItemLootTable			m_tblItemLoot;
	COreCuttingTable		m_tblOreCutting;
	CItemMakeDataTable		m_tblItemMakeData;
	CItemUpgradeTable		m_tblItemUpgrade;

	CRecordData				m_tblItemData[item_tbl_num];
	CRecordData				m_tblEffectData[EFFECT_CODE_NUM];

	DWORD					m_dwRegedWorldNum;
	_WORLD_LIST				m_WorldList[MAX_WORLD_PER_GLOBAL];

	char					m_szForceMasteryName[force_mastery_num][32];

	char					m_szLastErrMsg[128];

public:

	CAnalysisAvatorData();
	~CAnalysisAvatorData();

	bool Create(DWORD dwRegedWorldNum, _WORLD_LIST* pWorldList, char* OUT pszErrMsg);
	bool UnpackData(_AVATOR_VIEWER* OUT pViewer, _AVATOR_DATA* IN pData, DWORD dwWorldCode);
	bool PackData(_AVATOR_DATA* OUT pData, _AVATOR_VIEWER* IN pViewer, _AVATOR_DATA* IN pOriginalData);
	bool CheckDataIntegrity(_AVATOR_DATA* IN pNewData, _AVATOR_DATA* IN pOldData);
	
	bool GetItemNameFromCode(char* IN pszItemCode, char* OUT pszItemName);
	char* GetLastErrMsg() {	return m_szLastErrMsg;	}

	bool GetItemName( char* IN szItemCode, char* OUT szItemName );
	bool GetItemName( DWORD dwItemCode, char* OUT szItemName );
	
	DWORD GetItemCode_K( char* szItemCode );
	void SplitItemCode( DWORD dwItemCode, int* pnTableCode, WORD* pwItemIndex );

private:

	bool _UnpackAnalysisBase(__AVATOR_BASE* OUT pBase, _AVATOR_DATA* IN pData, DWORD dwWorldCode);
	bool _UnpackAnalysisGeneral(__AVATOR_GENERAL* OUT pGeneral, _AVATOR_DATA* IN pData);
	bool _UnpackAnalysisSkill(__AVATOR_SKILL* OUT pSkill, _AVATOR_DATA* IN pData);
	bool _UnpackAnalysisForce(__AVATOR_FORCE* OUT pForce, _AVATOR_DATA* IN pData);
	bool _UnpackAnalysisEquip(__ITEM_EQUIP* OUT pEquip, _AVATOR_DATA* IN pData);	
	bool _UnpackAnalysisInven(__ITEM_INVEN* OUT pInven, _AVATOR_DATA* IN pData);	
	bool _UnpackAnalysisForceInven(__ITEM_FORCE* OUT pFInven, _AVATOR_DATA* IN pData);
//	bool _UnpackAnalysisUnitInven(_UNIT_DB_BASE* OUT pFInven, _AVATOR_DATA* IN pData);
//	bool _UnpackAnalysisAnimusInven(_ANIMUS_DB_BASE* OUT pFInven, _AVATOR_DATA* IN pData);
	


	bool _UnpackAnalysisBelt(__ITEM_BELT* OUT pBelt, _AVATOR_DATA* IN pData);	
	bool _UnpackAnalysisLeftRes(__ITEM_RBUFFER* OUT pRes, _AVATOR_DATA* IN pData);	

	bool _PackAnalysisBase(_AVATOR_DB_BASE* OUT pBase, _AVATOR_VIEWER* IN pViewer, _AVATOR_DATA* IN pOriginalData);
	bool _PackAnalysisGeneral(_STAT_DB_BASE* OUT pGeneral, _AVATOR_VIEWER* IN pViewer);
	bool _PackAnalysisEquip(_EQUIP_DB_BASE* OUT pEquip, _AVATOR_VIEWER* IN pViewer,  _AVATOR_DATA* IN pOriginalData);	
	bool _PackAnalysisInven(_INVEN_DB_BASE* OUT pInven, _AVATOR_VIEWER* IN pViewer);	
	bool _PackAnalysisForceInven(_FORCE_DB_BASE* OUT pFInven, _AVATOR_VIEWER* IN pViewer);	
	bool _PackAnalysisBelt(_BELT_DB_BASE* OUT pBelt, _AVATOR_VIEWER* IN pViewer);	
};

#endif