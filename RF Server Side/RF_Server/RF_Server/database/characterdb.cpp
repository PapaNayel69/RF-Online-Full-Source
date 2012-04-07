#include "stdafx.h"
#include <math.h>
#include <stdio.h>
#include "CharacterDB.h"
#include "BaseFld.h"
#include "DfnEquipItemFld.h"
#include "WeaponItemFld.h"
#include "BulletItemFld.h"
#include "EtcItemFld.h"
#include "MakeToolItemFld.h"
#include "PotionItemFld.h"
#include "OreItemFld.h"
#include "ResourceItemFld.h"
#include "BatteryItemFld.h"
#include "UnitKeyItemFld.h"
#include "BootyItemFld.h"
#include "AmuletItemFld.h"
#include "RingItemFld.h"
#include "ForceItemFld.h"
#include "FaceItemFld.h"
#include "MapItemFld.h"
#include "TownItemFld.h"
#include "BattleDungeonItemFld.h"
#include "AnimusItemFld.h"
#include "GuardTowerItemFld.h"

static CRecordData* s_ptblItemData = NULL;

int GetItemTableCode(char* psItemCode)
{
	char szPreFix[3];
	strncpy(szPreFix, psItemCode, 2);
	szPreFix[2] = NULL;

	if(!strcmp(szPreFix, "iu"))				return tbl_code_upper;
	else if(!strcmp(szPreFix, "il"))		return tbl_code_lower;
	else if(!strcmp(szPreFix, "ig"))		return tbl_code_gauntlet;
	else if(!strcmp(szPreFix, "is"))		return tbl_code_shoe;
	else if(!strcmp(szPreFix, "ih"))		return tbl_code_helmet;
	else if(!strcmp(szPreFix, "id"))		return tbl_code_shield;
	else if(!strcmp(szPreFix, "iw"))		return tbl_code_weapon;
	else if(!strcmp(szPreFix, "im"))		return tbl_code_maketool;
	else if(!strcmp(szPreFix, "ie"))		return tbl_code_bag;
	else if(!strcmp(szPreFix, "ip"))		return tbl_code_potion;
	else if(!strcmp(szPreFix, "ib"))		return tbl_code_bullet;
	else if(!strcmp(szPreFix, "if"))		return tbl_code_face;
	else if(!strcmp(szPreFix, "ic"))		return tbl_code_fcitem;
	else if(!strcmp(szPreFix, "it"))		return tbl_code_battery;
	else if(!strcmp(szPreFix, "io"))		return tbl_code_ore;
	else if(!strcmp(szPreFix, "ir"))		return tbl_code_res;
	else if(!strcmp(szPreFix, "in"))		return tbl_code_key;
	else if(!strcmp(szPreFix, "iy"))		return tbl_code_booty;	
	else if(!strcmp(szPreFix, "ik"))		return tbl_code_cloak;
	else if(!strcmp(szPreFix, "ii"))		return tbl_code_ring;
	else if(!strcmp(szPreFix, "ia"))		return tbl_code_amulet;
	else if(!strcmp(szPreFix, "iz"))		return tbl_code_map;
	else if(!strcmp(szPreFix, "iq"))		return tbl_code_town;
	else if(!strcmp(szPreFix, "ix"))		return tbl_code_bdungeon;
	else if(!strcmp(szPreFix, "ij"))		return tbl_code_animus;
	else if(!strcmp(szPreFix, "gt"))		return tbl_code_tower;
	return -1;
}

bool WriteTableData(int nItemNum, CRecordData* pItemData, char* szErrCode)
{
	if(s_ptblItemData)
	{
		if(szErrCode)
			sprintf(szErrCode, "두번째 호출");
		return false;
	}
	if(nItemNum != item_tbl_num)
	{
		if(szErrCode)
			sprintf(szErrCode, "nItemMax<%d> != item_tbl_num<%d>", nItemNum, item_tbl_num);
		return false;
	}

	static struct __list{
		char*	pfilename;
		int		nstructsize;
	}	ItemDataFile[item_tbl_num] = {
								".\\script\\UpperItem.dat",			sizeof(_DfnEquipItem_fld),		//tbl_code_upper
								".\\script\\LowerItem.dat",			sizeof(_DfnEquipItem_fld),		//tbl_code_lower
								".\\script\\GauntletItem.dat",		sizeof(_DfnEquipItem_fld),		//tbl_code_gauntlet
								".\\script\\ShoeItem.dat",			sizeof(_DfnEquipItem_fld),		//tbl_code_shoe
								".\\script\\HelmetItem.dat",		sizeof(_DfnEquipItem_fld),		//tbl_code_helmet
								".\\script\\ShieldItem.dat",		sizeof(_DfnEquipItem_fld),		//tbl_code_shield
								".\\script\\WeaponItem.dat",		sizeof(_WeaponItem_fld),		//tbl_code_weapon	
								".\\script\\CloakItem.dat",			sizeof(_DfnEquipItem_fld),		//tbl_code_cloak
								".\\script\\RingItem.dat",			sizeof(_RingItem_fld),			//tbl_code_ring
								".\\script\\AmuletItem.dat",		sizeof(_AmuletItem_fld),		//tbl_code_amulet
								".\\script\\BulletItem.dat",		sizeof(_BulletItem_fld),		//tbl_code_bullet
								".\\script\\MaketoolItem.dat",		sizeof(_MakeToolItem_fld),		//tbl_code_maketool
								".\\script\\BagItem.dat",			sizeof(_EtcItem_fld),			//tbl_code_etc
								".\\script\\PotionItem.dat",		sizeof(_PotionItem_fld),		//tbl_code_potion
								".\\script\\FaceItem.dat",			sizeof(_FaceItem_fld),			//tbl_code_face
								".\\script\\ForceItem.dat",			sizeof(_ForceItem_fld),			//tbl_code_fcitem
								".\\script\\BatteryItem.dat",		sizeof(_BatteryItem_fld),		//tbl_code_battery
								".\\script\\OreItem.dat", 			sizeof(_OreItem_fld), 			//tbl_code_ore
								".\\script\\ResourceItem.dat",	 	sizeof(_ResourceItem_fld), 		//tbl_code_resource
								".\\script\\UnitKeyItem.dat",		sizeof(_UnitKeyItem_fld),		//tbl_code_key
								".\\script\\BootyItem.dat",			sizeof(_BootyItem_fld),			//tbl_code_booty
								".\\script\\MapItem.dat", 			sizeof(_MapItem_fld),			//tbl_code_map
								".\\script\\TOWNItem.dat", 			sizeof(_TOWNItem_fld),			//tbl_code_town
								".\\script\\BattleDungeonItem.dat", sizeof(_BattleDungeonItem_fld),	//tbl_code_bdungeon
								".\\script\\AnimusItem.dat",		sizeof(_AnimusItem_fld),		//tbl_code_animus
								".\\script\\GuardTowerItem.dat",	sizeof(_GuardTowerItem_fld)	};	//tbl_code_tower
								

	for(int i = 0; i < item_tbl_num; i++)
	{
		if(!pItemData[i].ReadRecord(ItemDataFile[i].pfilename, ItemDataFile[i].nstructsize))
		{			
			if(szErrCode)
				sprintf(szErrCode, "m_tblItemData[%d].ReadRecord(%s) 로드 실패", i, ItemDataFile[i].pfilename);
			return false;
		}
	}

	s_ptblItemData = pItemData;

	return true;
}

int GetItemDurPoint(int nTableCode, int nIndex)
{
	CRecordData* pItemData = &s_ptblItemData[nTableCode];

	switch(nTableCode)
	{
	case tbl_code_upper:case tbl_code_lower:case tbl_code_gauntlet:case tbl_code_shoe:case tbl_code_shield:case tbl_code_helmet:		{
		_DfnEquipItem_fld* pRec = (_DfnEquipItem_fld*)pItemData->GetRecord(nIndex);
		if(pRec)			return pRec->m_nDurUnit;
		}break;
	case tbl_code_weapon:		{
		_WeaponItem_fld* pRec = (_WeaponItem_fld*)pItemData->GetRecord(nIndex);
		if(pRec)			return pRec->m_nDurUnit;
		}break;
	case tbl_code_maketool:		{
		_MakeToolItem_fld* pRec = (_MakeToolItem_fld*)pItemData->GetRecord(nIndex);
		if(pRec)			return pRec->m_nDurUnit;
		}break;
	case tbl_code_bullet:		{
		_BulletItem_fld* pRec = (_BulletItem_fld*)pItemData->GetRecord(nIndex);
		if(pRec)			return pRec->m_nDurUnit;
		}break;
	case tbl_code_battery:		{
		_BatteryItem_fld* pRec = (_BatteryItem_fld*)pItemData->GetRecord(nIndex);
		if(pRec)			return pRec->m_nDurUnit;
		}break;
	case tbl_code_tower:		{	
		_GuardTowerItem_fld* pRec = (_GuardTowerItem_fld*)pItemData->GetRecord(nIndex);
		if(pRec)			return pRec->m_nMaxHP;
		}break;
	}
	return 1;
}

int GetItemStdPrice(int nTableCode, int nItemIndex, int nRace, BYTE* pbyMoneyKind)
{	//tbl_code_face, tbl_code_ore는 가격없음
	CRecordData* pItemData = &s_ptblItemData[nTableCode];

	switch(nTableCode)
	{
	case tbl_code_upper:case tbl_code_lower:case tbl_code_gauntlet:case tbl_code_shoe:case tbl_code_shield:case tbl_code_helmet:case tbl_code_cloak:		{
		_DfnEquipItem_fld* pRec = (_DfnEquipItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		{	*pbyMoneyKind = pRec->m_nMoney;	return pRec->m_nStdPrice;	}	
		}break;
	case tbl_code_weapon:		{
		_WeaponItem_fld* pRec = (_WeaponItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		{	*pbyMoneyKind = pRec->m_nMoney;	return pRec->m_nStdPrice;	}
		}break;
	case tbl_code_maketool:		{
		_MakeToolItem_fld* pRec = (_MakeToolItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		{	*pbyMoneyKind = pRec->m_nMoney;	return pRec->m_nStdPrice;	}
		}break;
	case tbl_code_bag:	{
		_EtcItem_fld* pRec = (_EtcItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		{	*pbyMoneyKind = pRec->m_nMoney; return pRec->m_nStdPrice;	}
		}break;
	case tbl_code_potion:		{
		_PotionItem_fld* pRec = (_PotionItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		{	*pbyMoneyKind = pRec->m_nMoney;	return pRec->m_nStdPrice;	}
		}break;
	case tbl_code_bullet:		{
		_BulletItem_fld* pRec = (_BulletItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		{	*pbyMoneyKind = pRec->m_nMoney;	return pRec->m_nStdPrice;	}
		}break;
	case tbl_code_res:			{
		_ResourceItem_fld* pRec = (_ResourceItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		{	*pbyMoneyKind = pRec->m_nMoney;	if(nRace == 0) return pRec->m_nAncStdPrice; if(nRace == 1) return pRec->m_nExStdPrice; if(nRace == 2) return pRec->m_nMecaStdPrice;	}
		}break;
	case tbl_code_key:			{
		return 0;	//유닛을 사면 자동으로 딸려오는 아이템
		}break;
	case tbl_code_booty:		{
		_BootyItem_fld* pRec = (_BootyItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		{	*pbyMoneyKind = pRec->m_nMoney;	return pRec->m_nStdPrice;	}
		}break;
	case tbl_code_battery:		{
		_BatteryItem_fld* pRec = (_BatteryItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		{	*pbyMoneyKind = pRec->m_nMoney;	return pRec->m_nStdPrice;	}
		}break;
	case tbl_code_fcitem:		{
		_ForceItem_fld* pRec = (_ForceItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		{	*pbyMoneyKind = pRec->m_nMoney;	return pRec->m_nStdPrice;	}
		}break;
	case tbl_code_ring:		{
		_RingItem_fld* pRec = (_RingItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		{	*pbyMoneyKind = pRec->m_nMoney;	return pRec->m_nStdPrice;	}
		}break;
	case tbl_code_amulet:	{
		_AmuletItem_fld* pRec = (_AmuletItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		{	*pbyMoneyKind = pRec->m_nMoney;	return pRec->m_nStdPrice;	}
		}break;
	case tbl_code_map:	{
		_MapItem_fld* pRec = (_MapItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		{	*pbyMoneyKind = pRec->m_nMoney;	return pRec->m_nStdPrice;	}
		}break;
	case tbl_code_town:	{
		_TOWNItem_fld* pRec = (_TOWNItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		{	*pbyMoneyKind = pRec->m_nMoney;	return pRec->m_nStdPrice;	}
		}break;
	case tbl_code_bdungeon:	{
		_BattleDungeonItem_fld* pRec = (_BattleDungeonItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		{	*pbyMoneyKind = pRec->m_nMoney;	return pRec->m_nStdPrice;	}
		}break;
	case tbl_code_animus:	{
		_AnimusItem_fld* pRec = (_AnimusItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		{	*pbyMoneyKind = pRec->m_nMoney;	return pRec->m_nStdPrice;	}
		}break;
	case tbl_code_tower:	{
		_GuardTowerItem_fld* pRec = (_GuardTowerItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		{	*pbyMoneyKind = pRec->m_nMoney;	return pRec->m_nStdPrice;	}
		}break;
	}		
	return 0;
}

int GetItemEquipLevel(int nTableCode, int nItemIndex)
{
	CRecordData* pItemData = &s_ptblItemData[nTableCode];

	switch(nTableCode)
	{
	case tbl_code_upper:case tbl_code_lower:case tbl_code_gauntlet:case tbl_code_shoe:case tbl_code_shield:case tbl_code_helmet:case tbl_code_cloak:		{
		_DfnEquipItem_fld* pRec = (_DfnEquipItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)	return pRec->m_nLevelLim;			
		}break;
	case tbl_code_weapon:	{
		_WeaponItem_fld* pRec = (_WeaponItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)	return pRec->m_nLevelLim;
		}break;
	case tbl_code_tower:	{
		_GuardTowerItem_fld* pRec = (_GuardTowerItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)	return pRec->m_nLevelLim;
		}break;
	}
	return 1;
}

_EQUIP_MASTERY_LIM* GetItemEquipMastery(int nTableCode, int nItemIndex, int* OUT pnLimNum)
{
	CRecordData* pItemData = &s_ptblItemData[nTableCode];

	switch(nTableCode)
	{
	case tbl_code_upper:case tbl_code_lower:case tbl_code_gauntlet:case tbl_code_shoe:case tbl_code_shield:case tbl_code_helmet:case tbl_code_cloak:		{
		_DfnEquipItem_fld* pRec = (_DfnEquipItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec){ *pnLimNum = 2;	return (_EQUIP_MASTERY_LIM*)&pRec->m_nExpertID1;	}
		}break;		
	case tbl_code_weapon:		{
		_WeaponItem_fld* pRec = (_WeaponItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec){ *pnLimNum = 2;	return (_EQUIP_MASTERY_LIM*)&pRec->m_nExpertID1;	}
		}break;
	}
	return NULL;
}

char* GetItemEquipCivil(int nTableCode, int nItemIndex)
{
	CRecordData* pItemData = &s_ptblItemData[nTableCode];

	switch(nTableCode)
	{
	case tbl_code_upper:case tbl_code_lower:case tbl_code_gauntlet:case tbl_code_shoe:case tbl_code_shield:case tbl_code_helmet:case tbl_code_cloak:		{
		_DfnEquipItem_fld* pRec = (_DfnEquipItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strCivil;
		}break;
	case tbl_code_weapon:		{
		_WeaponItem_fld* pRec = (_WeaponItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strCivil;
		}break;
	case tbl_code_maketool:		{
		_MakeToolItem_fld* pRec = (_MakeToolItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strCivil;
		}break;
	case tbl_code_bag:	{
		_EtcItem_fld* pRec = (_EtcItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strCivil;
		}break;
	case tbl_code_potion:		{
		_PotionItem_fld* pRec = (_PotionItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strCivil;
		}break;
	case tbl_code_bullet:		{
		_BulletItem_fld* pRec = (_BulletItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strCivil;
		}break;
	case tbl_code_res:			{
		_ResourceItem_fld* pRec = (_ResourceItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strCivil;
		}break;
	case tbl_code_key:			{
		_UnitKeyItem_fld* pRec = (_UnitKeyItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strCivil;
		}break;
	case tbl_code_booty:		{
		_BootyItem_fld* pRec = (_BootyItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strCivil;
		}break;
	case tbl_code_battery:		{
		_BatteryItem_fld* pRec = (_BatteryItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strCivil;
		}break;
	case tbl_code_fcitem:		{
		_ForceItem_fld* pRec = (_ForceItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strCivil;
		}break;
	case tbl_code_ring:		{
		_RingItem_fld* pRec = (_RingItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strCivil;
		}break;
	case tbl_code_amulet:	{
		_AmuletItem_fld* pRec = (_AmuletItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strCivil;
		}break;
	case tbl_code_face:	{
		_FaceItem_fld* pRec = (_FaceItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strCivil;
		}break;
	case tbl_code_map:	{
		_MapItem_fld* pRec = (_MapItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strCivil;
		}break;
	case tbl_code_town:	{
		_TOWNItem_fld* pRec = (_TOWNItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strCivil;
		}break;
	case tbl_code_bdungeon:	{
		_BattleDungeonItem_fld* pRec = (_BattleDungeonItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strCivil;
		}break;
	case tbl_code_animus:	{
		_AnimusItem_fld* pRec = (_AnimusItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strCivil;
		}break;
	case tbl_code_tower:	{
		_GuardTowerItem_fld* pRec = (_GuardTowerItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strCivil;
		}break;
	}		
	return NULL;
}

char* GetItemKorName(int nTableCode, int nItemIndex)
{
	CRecordData* pItemData = &s_ptblItemData[nTableCode];

	switch(nTableCode)
	{
	case tbl_code_upper:case tbl_code_lower:case tbl_code_gauntlet:case tbl_code_shoe:case tbl_code_shield:case tbl_code_helmet:case tbl_code_cloak:		{
		_DfnEquipItem_fld* pRec = (_DfnEquipItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strName;
		}break;
	case tbl_code_weapon:		{
		_WeaponItem_fld* pRec = (_WeaponItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strName;
		}break;
	case tbl_code_maketool:		{
		_MakeToolItem_fld* pRec = (_MakeToolItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strName;
		}break;
	case tbl_code_bag:	{
		_EtcItem_fld* pRec = (_EtcItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strName;
		}break;
	case tbl_code_potion:		{
		_PotionItem_fld* pRec = (_PotionItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strName;
		}break;
	case tbl_code_bullet:		{
		_BulletItem_fld* pRec = (_BulletItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strName;
		}break;
	case tbl_code_ore:			{
		_OreItem_fld* pRec = (_OreItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strName;
		}break;
	case tbl_code_res:			{
		_ResourceItem_fld* pRec = (_ResourceItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strName;
		}break;
	case tbl_code_key:			{
		_UnitKeyItem_fld* pRec = (_UnitKeyItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strName;
		}break;
	case tbl_code_booty:		{
		_BootyItem_fld* pRec = (_BootyItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strName;
		}break;
	case tbl_code_battery:		{
		_BatteryItem_fld* pRec = (_BatteryItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strName;
		}break;
	case tbl_code_fcitem:		{
		_ForceItem_fld* pRec = (_ForceItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strName;
		}break;
	case tbl_code_ring:		{
		_RingItem_fld* pRec = (_RingItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strName;
		}break;
	case tbl_code_amulet:	{
		_AmuletItem_fld* pRec = (_AmuletItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strName;
		}break;
	case tbl_code_face:	{
		_FaceItem_fld* pRec = (_FaceItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strName;
		}break;
	case tbl_code_map:	{
		_MapItem_fld* pRec = (_MapItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strName;
		}break;
	case tbl_code_town:	{
		_TOWNItem_fld* pRec = (_TOWNItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strName;
		}break;
	case tbl_code_bdungeon:	{
		_BattleDungeonItem_fld* pRec = (_BattleDungeonItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strName;
		}break;
	case tbl_code_animus:	{
		_AnimusItem_fld* pRec = (_AnimusItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strName;
		}break;
	case tbl_code_tower:	{
		_GuardTowerItem_fld* pRec = (_GuardTowerItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_strName;
		}break;
	}		
	return NULL;
}

BYTE GetDefItemUpgSocketNum(int nTableCode, int nItemIndex)
{
	CRecordData* pItemData = &s_ptblItemData[nTableCode];

	BYTE bySlot = 0;
	switch(nTableCode)
	{
		case tbl_code_upper:case tbl_code_lower:case tbl_code_gauntlet:case tbl_code_shoe:case tbl_code_shield:case tbl_code_helmet:case tbl_code_cloak:		{
			_DfnEquipItem_fld* pRec = (_DfnEquipItem_fld*)pItemData->GetRecord(nItemIndex);
			if(pRec)	bySlot = pRec->m_nUpgrade;
			}break;
		case tbl_code_weapon:	{
			_WeaponItem_fld* pRec = (_WeaponItem_fld*)pItemData->GetRecord(nItemIndex);
			if(pRec)	bySlot = pRec->m_nUpgrade;
			}break;
	}
	if(bySlot > MAX_ITEM_LV)
	{
		char szbuf[128];
		sprintf(szbuf, "tbl:%d, idx:%d => slot: %d\n", nTableCode, nItemIndex, bySlot);
		::OutputDebugString( szbuf );
		bySlot = 0;
	}

	return bySlot;
}

BOOL IsTalikAboutTol(int nTalikIndex)
{
	if(nTalikIndex == talik_type_fire	||
		nTalikIndex == talik_type_water ||
		nTalikIndex == talik_type_soil	||	
		nTalikIndex == talik_type_wind)
		return true;
	return false;		
}

BOOL IsStorageCodeWithItemKind(int nTableCode, int nStorageCode)
{
	switch(nStorageCode)
	{ 
	case _STORAGE_POS::EQUIP:
		if(nTableCode >= equip_fix_num)
			return false;
		break;
	case _STORAGE_POS::EMBELLISH:
		if(nTableCode != tbl_code_ring && nTableCode != tbl_code_amulet && nTableCode != tbl_code_bullet)
			return false;
		break;
	case _STORAGE_POS::BELT:
		if(nTableCode != tbl_code_potion)
			return false;
		break;
	case _STORAGE_POS::FORCE:
		if(nTableCode != tbl_code_fcitem)
			return false;
		break;
	case _STORAGE_POS::ANIMUS:
		if(nTableCode != tbl_code_animus)
			return false;
		break;
	}
	return true;
}

BOOL IsItemEquipCivil(int nTableCode, int nItemIndex, BYTE byRaceSex)
{
	CRecordData* pItemData = &s_ptblItemData[nTableCode];

	if(byRaceSex > race_sex_kind_num)
		return FALSE;
	char* pszCivil = ::GetItemEquipCivil(nTableCode, nItemIndex);
	if(pszCivil)
	{
		if(pszCivil[byRaceSex] == '1')
			return TRUE;
	}
	return FALSE;
}

BOOL IsOverLapItem(int nTableCode)
{//tbl_code_potion, tbl_code_ore, tbl_code_res, tbl_code_booty, tbl_code_town
	if(nTableCode == tbl_code_potion || nTableCode == tbl_code_ore || 
		nTableCode == tbl_code_res || nTableCode == tbl_code_booty || nTableCode == tbl_code_town)
		return TRUE;
	return FALSE;
}

BOOL IsProtectItem(int nTableCode)//보호되는 아이템
{
	if(nTableCode == tbl_code_potion || nTableCode == tbl_code_ore || 
		nTableCode == tbl_code_booty || nTableCode == tbl_code_town ||
		nTableCode == tbl_code_key || nTableCode == tbl_code_battery )
		return FALSE;
	return TRUE;
}

BOOL IsRepairableItem(int nTableCode, int nItemIndex)
{
	CRecordData* pItemData = &s_ptblItemData[nTableCode];

	switch(nTableCode)
	{
		case tbl_code_upper:case tbl_code_lower:case tbl_code_gauntlet:case tbl_code_shoe:case tbl_code_shield:case tbl_code_helmet:case tbl_code_cloak:		{
			_DfnEquipItem_fld* pRec = (_DfnEquipItem_fld*)pItemData->GetRecord(nItemIndex);
			if(pRec)	return pRec->m_bRepair;		
			}break;
		case tbl_code_weapon:		{
			_WeaponItem_fld* pRec = (_WeaponItem_fld*)pItemData->GetRecord(nItemIndex);
			if(pRec)	return pRec->m_bRepair;			
			}break;
		case tbl_code_maketool:		{
			_MakeToolItem_fld* pRec = (_MakeToolItem_fld*)pItemData->GetRecord(nItemIndex);
			if(pRec)	return pRec->m_bRepair;
			}break;
	}
	return FALSE;
}

BOOL IsAbrItem(int nTableCode, int nItemIndex)
{
	CRecordData* pItemData = &s_ptblItemData[nTableCode];

	switch(nTableCode)
	{
		case tbl_code_weapon:	//	{	//무기의 경우 채굴기는 마모가 됀다
			//_WeaponItem_fld* pRec = (_WeaponItem_fld*)pItemData->GetRecord(nItemIndex);
			//if(pRec)	return pRec->m_bAbr;			
			//}break;
			return FALSE;
		case tbl_code_maketool:		
			return TRUE;
		case tbl_code_key:			
			return TRUE;
		case tbl_code_bullet:			
			return TRUE;
		case tbl_code_battery:		
			return TRUE;
		case tbl_code_tower:		
			return TRUE;
	}
	return FALSE;
}

BOOL IsGroundableItem(int nTableCode, int nItemIndex)
{
	CRecordData* pItemData = &s_ptblItemData[nTableCode];

	switch(nTableCode)
	{
	case tbl_code_upper:case tbl_code_lower:case tbl_code_gauntlet:case tbl_code_shoe:case tbl_code_shield:case tbl_code_helmet:case tbl_code_cloak:		{
		_DfnEquipItem_fld* pRec = (_DfnEquipItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bGround;
		}break;
	case tbl_code_weapon:		{
		_WeaponItem_fld* pRec = (_WeaponItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bGround;
		}break;
	case tbl_code_maketool:		{
		_MakeToolItem_fld* pRec = (_MakeToolItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bGround;
		}break;
	case tbl_code_bag:	{
		_EtcItem_fld* pRec = (_EtcItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bGround;
		}break;
	case tbl_code_potion:		{
		_PotionItem_fld* pRec = (_PotionItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bGround;
		}break;
	case tbl_code_bullet:		{
		_BulletItem_fld* pRec = (_BulletItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bGround;
		}break;
	case tbl_code_res:			{
		_ResourceItem_fld* pRec = (_ResourceItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bGround;
		}break;
	case tbl_code_ore:			{
		_OreItem_fld* pRec = (_OreItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bGround;
		}break;
	case tbl_code_key:			{
		_UnitKeyItem_fld* pRec = (_UnitKeyItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bGround;
		}break;
	case tbl_code_booty:		{
		_BootyItem_fld* pRec = (_BootyItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bGround;
		}break;
	case tbl_code_battery:		{
		_BatteryItem_fld* pRec = (_BatteryItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bGround;
		}break;
	case tbl_code_fcitem:		{
		_ForceItem_fld* pRec = (_ForceItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bGround;
		}break;
	case tbl_code_ring:		{
		_RingItem_fld* pRec = (_RingItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bGround;
		}break;
	case tbl_code_amulet:	{
		_AmuletItem_fld* pRec = (_AmuletItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bGround;
		}break;
	case tbl_code_map:	{
		_MapItem_fld* pRec = (_MapItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bGround;
		}break;
	case tbl_code_town:	{
		_TOWNItem_fld* pRec = (_TOWNItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bGround;
		}break;
	case tbl_code_bdungeon:	{
		_BattleDungeonItem_fld* pRec = (_BattleDungeonItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bGround;
		}break;
	case tbl_code_animus:	{
		_AnimusItem_fld* pRec = (_AnimusItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bGround;
		}break;
	case tbl_code_tower:	{
		_GuardTowerItem_fld* pRec = (_GuardTowerItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bGround;
		}break;
	}		
	return FALSE;
}

BOOL IsExistItem(int nTableCode, int nItemIndex)
{
	CRecordData* pItemData = &s_ptblItemData[nTableCode];

	switch(nTableCode)
	{
	case tbl_code_upper:case tbl_code_lower:case tbl_code_gauntlet:case tbl_code_shoe:case tbl_code_shield:case tbl_code_helmet:case tbl_code_cloak:		{
		_DfnEquipItem_fld* pRec = (_DfnEquipItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bExist;
		}break;
	case tbl_code_weapon:		{
		_WeaponItem_fld* pRec = (_WeaponItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bExist;
		}break;
	case tbl_code_maketool:		{
		_MakeToolItem_fld* pRec = (_MakeToolItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bExist;
		}break;
	case tbl_code_bag:	{
		_EtcItem_fld* pRec = (_EtcItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bExist;
		}break;
	case tbl_code_potion:		{
		_PotionItem_fld* pRec = (_PotionItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bExist;
		}break;
	case tbl_code_bullet:		{
		_BulletItem_fld* pRec = (_BulletItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bExist;
		}break;
	case tbl_code_res:			{
		_ResourceItem_fld* pRec = (_ResourceItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bExist;
		}break;
	case tbl_code_key:			{
		_UnitKeyItem_fld* pRec = (_UnitKeyItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bExist;
		}break;
	case tbl_code_booty:		{
		_BootyItem_fld* pRec = (_BootyItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bExist;
		}break;
	case tbl_code_battery:		{
		_BatteryItem_fld* pRec = (_BatteryItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bExist;
		}break;
	case tbl_code_fcitem:		{
		_ForceItem_fld* pRec = (_ForceItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bExist;
		}break;
	case tbl_code_ring:		{
		_RingItem_fld* pRec = (_RingItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bExist;
		}break;
	case tbl_code_amulet:	{
		_AmuletItem_fld* pRec = (_AmuletItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bExist;
		}break;
	case tbl_code_map:	{
		_MapItem_fld* pRec = (_MapItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bExist;
		}break;
	case tbl_code_town:	{
		_TOWNItem_fld* pRec = (_TOWNItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bExist;
		}break;
	case tbl_code_bdungeon:	{
		_BattleDungeonItem_fld* pRec = (_BattleDungeonItem_fld*)pItemData->GetRecord(nItemIndex);
		if(pRec)		return pRec->m_bExist;
		}break;
	case tbl_code_animus:	{
		return TRUE;
		}break;
	case tbl_code_tower:	{
		return TRUE;
		}break;
	}		
	return FALSE;
}

BYTE GetWeaponClass(int nItemIndex)
{
	CRecordData* pItemData = &s_ptblItemData[tbl_code_weapon];
	_WeaponItem_fld* pWPFld = (_WeaponItem_fld*)pItemData->GetRecord(nItemIndex);
	switch( pWPFld->m_nType )
	{
	case type_knife:case type_sword:case type_axe:case type_mace:case type_spear:case type_staff:		
		return wp_class_close;		
	case type_bow:case type_firearm:case type_throw:case type_luancher:
		return wp_class_long;	
	}
	return wp_class_close;	
}

BYTE GetItemUpgedLv(DWORD dwLvBit)
{
	BYTE byLv = 0;
	for(int g = 0; g < MAX_ITEM_LV; g++)
	{
		BYTE byTemp = (BYTE)((dwLvBit>>(g*4))&0x0000000F);
		if(byTemp == __NO_TALIK)
			break;
		byLv++;
	}
	return byLv;
}

BYTE GetItemUpgLimSocket(DWORD dwLvBit)
{
	DWORD dw = (dwLvBit>>(MAX_ITEM_LV*4))&0x0000000F;
	return (BYTE)dw;
}

BYTE GetTalikFromSocket(DWORD dwLvBit, BYTE bySocketIndex)
{
	DWORD dw = (dwLvBit>>(bySocketIndex*4))&0x0000000F;
	return (BYTE)dw;
}

DWORD GetBitAfterUpgrade(DWORD dwCurBit, DWORD dwTalikCode, BYTE byCurLv)
{
	DWORD dwWorkBit = dwCurBit;
	DWORD dwMask = ~(0xFFFFFFFF<<(byCurLv*4));
	
	dwWorkBit &= dwMask;
	dwWorkBit |= (dwTalikCode&0x0000000F)<<(byCurLv*4);

	DWORD dwHead = (dwCurBit>>((byCurLv+1)*4))<<((byCurLv+1)*4);
	dwHead |= dwWorkBit;

	return dwHead;
}

DWORD GetBitAfterDowngrade(DWORD dwCurBit, BYTE byCurLv)
{
	DWORD dwUpt = dwCurBit;
	BYTE byNextLv = byCurLv-1;
	dwUpt |= (0x0000000F<<(byNextLv*4));

	return dwUpt;
}

DWORD GetBitAfterSetLimSocket(BYTE byLimSocketNum)
{
	DWORD dwUpt = __DF_UPT;
	DWORD dwLimLv = byLimSocketNum;
	dwUpt |= (dwLimLv<<(MAX_ITEM_LV*4));
	
	return dwUpt;
}

static char g_szLv[32] = "00000000";
static char g_szUPT[32] = "f";
char* DisplayItemUpgInfo(int nTableCode, DWORD dwLvBit)
{
	static char szBufUpt[32];

	if(GetItemKindCode(nTableCode) != item_kind_std || dwLvBit == __DF_UPT)
		return g_szUPT;

	strcpy(g_szLv, "00000000");

	_itoa(dwLvBit, szBufUpt, 16);
	memcpy(g_szLv+(8-strlen(szBufUpt)), szBufUpt, strlen(szBufUpt));
	g_szLv[8] = NULL;
	if(!strcmp(&g_szLv[1], "fffffff"))//탈릭이 안박힌 거라면 f하나로 생략한다..
		g_szLv[2] = NULL;

	return g_szLv;
}

bool IsAddAbleTalikToItem(BYTE byItemTableCode, WORD wItemIndex, DWORD dwItemCurLv, WORD wTalikIndex, int* pnTalikLim)
{
	CRecordData* pItemData = &s_ptblItemData[byItemTableCode];

	if(wTalikIndex >= MAX_TALIK_NUM)
		return false;

	//무기중 내성관련탈릭은 중복됀것이 있나 검색한다.
	if(byItemTableCode == tbl_code_weapon && ::IsTalikAboutTol(wTalikIndex))
	{
		BYTE byLv = ::GetItemUpgedLv(dwItemCurLv);
		for(int i = 0; i < byLv; i++)
		{
			if(::IsTalikAboutTol(::GetTalikFromSocket(dwItemCurLv, i)))
				return false;
		}
	}

	int nItemCode = byItemTableCode;//필드순서와 아이템테이블코드순하고 근거리무기까지는 같다..
	if(byItemTableCode == tbl_code_weapon)
	{
		_WeaponItem_fld* pFld = (_WeaponItem_fld*)pItemData->GetRecord(wItemIndex);
		if(!pFld)
			return false;

		if(pFld->m_nType == type_firearm || pFld->m_nType == type_throw || pFld->m_nType == type_luancher || pFld->m_nType == type_bow)
			nItemCode  = 7;//원거리무기인경우는7번째 필드..
	}
	else if(byItemTableCode == tbl_code_key)
	{
		nItemCode  = 8;//유닛의경유는 8번째..
	}

	if(pnTalikLim[nItemCode] > 0)
	{
		int nCnt = 0;
		BYTE byLv = ::GetItemUpgedLv(dwItemCurLv);
		for(int i = 0; i < byLv; i++)
		{	
			BYTE idx = ::GetTalikFromSocket(dwItemCurLv, i);
			if(idx == wTalikIndex)
				nCnt++;
			if(nCnt > pnTalikLim[nItemCode])
				return false;
		}
	}
	else
	{
		return false;
	}

	return true;
}

BYTE GetItemKindCode(int nTableCode)
{
	switch(nTableCode)
	{
	case tbl_code_animus:
		return item_kind_ani;

	case tbl_code_key:
		return item_kind_key;
	}

	return item_kind_std;
}
