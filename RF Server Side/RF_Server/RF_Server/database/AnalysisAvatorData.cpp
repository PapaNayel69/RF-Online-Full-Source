#include "StdAfx.h"
#include "NetUtil.h"
#include <math.h>
#include "AnalysisAvatorData.h"
#include "FaceItemFld.h"
#include "DfnEquipItemFld.h"
#include "MapFld.h"
#include "SkillFld.h"
#include "ForceFld.h"
#include "ResourceItemFld.h"
#include "ExpFld.h" 
#include "forceitemfld.h"
#include "PlayerCharacterFld.h"
#include "MonsterCharacterFld.h"
#include "NPCharacterFld.h"


int GetSFLv(int nLv, DWORD dwHitCount);


CAnalysisAvatorData::CAnalysisAvatorData()
{
	m_bOper = false;
}

CAnalysisAvatorData::~CAnalysisAvatorData()
{
}

bool CAnalysisAvatorData::Create(DWORD dwRegedWorldNum, _WORLD_LIST* pWorldList, char* OUT pszErrMsg)
{
	if(m_bOper)
		return true;

	if(!::WriteTableData(item_tbl_num, m_tblItemData, NULL))
	{
		sprintf(pszErrMsg, "아이템 테이블 읽기 실패");
		return false;
	}

	if(!m_tblEffectData[effect_code_skill].ReadRecord(".\\Script\\skill.dat", sizeof(_skill_fld)))	
	{
		sprintf(pszErrMsg, "스킬 테이블 읽기 실패");
		return false;
	}

	if(!m_tblEffectData[effect_code_force].ReadRecord(".\\Script\\force.dat", sizeof(_force_fld)))	
	{
		sprintf(pszErrMsg, "포스 테이블 읽기 실패");
		return false;
	}

	if(!m_tblMapData.ReadScript(".\\Map\\Map_Data.spt"))
	{
		sprintf(pszErrMsg, "맵 테이블 읽기 실패");
		return false;
	}

	if(!m_tblPlayerCharacter.ReadRecord(".\\Script\\PlayerCharacter.dat", sizeof(_player_fld)))
	{
		sprintf(pszErrMsg, "플레이어 테이블 읽기 실패");
		return false;
	}
	
	if(!m_tblMonsterCharacter.ReadRecord(".\\Script\\MonsterCharacter.dat", sizeof(_monster_fld)))	
	{
		sprintf(pszErrMsg, "몬스터 테이블 읽기 실패");
		return false;
	}

	if(!m_tblNPCharacter.ReadRecord(".\\Script\\NPCharacter.dat", sizeof(_npc_fld)))	
	{
		sprintf(pszErrMsg, "NPC 테이블 읽기 실패");
		return false;
	}

	if(!m_tblExp.ReadRecord(".\\Script\\Exp.dat", sizeof(_exp_fld)))
	{
		sprintf(pszErrMsg, "경험치 테이블 읽기 실패");
		return false;
	}

	if(!m_tblWeaponBulletLink.ReadRecord(".\\Script\\WeaponBulletLink.dat", &m_tblItemData[tbl_code_weapon]))
	{
		sprintf(pszErrMsg, "무기대 탄환링크 테이블 읽기 실패");
		return false;
	}

	if(!m_tblItemLoot.ReadRecord(".\\Script\\ItemLooting.dat", m_tblItemData))
	{
		sprintf(pszErrMsg, "아이템 루팅 테이블 읽기 실패");
		return false;
	}

	if(!m_tblOreCutting.ReadRecord(".\\Script\\OreCutting.dat", &m_tblItemData[tbl_code_ore], &m_tblItemData[tbl_code_res]))
	{
		sprintf(pszErrMsg, "가공 테이블 읽기 실패");
		return false;
	}

	if(!m_tblItemMakeData.ReadRecord(".\\Script\\ItemMakeData.dat", m_tblItemData))
	{
		sprintf(pszErrMsg, "아이템제작 테이블 읽기 실패");
		return false;
	}

	if(!m_tblItemUpgrade.ReadRecord(".\\Script\\ItemUpgrade.dat", &m_tblItemData[tbl_code_res]))
	{
		sprintf(pszErrMsg, "아이템업그레이드 테이블 읽기 실패");
		return false;
	}

	//월드세팅
	m_dwRegedWorldNum = dwRegedWorldNum;
	memcpy(m_WorldList, pWorldList, sizeof(_WORLD_LIST)*dwRegedWorldNum);

	int nForceNum = m_tblEffectData[effect_code_force].GetRecordNum();
	for(int i = 0; i < force_mastery_num; i++)
	{
		for(int f = 0; f < nForceNum; f++)
		{
			_force_fld* pFld = (_force_fld*)m_tblEffectData[effect_code_force].GetRecord(f);
			if(pFld->m_nMastIndex == i)
			{
				strcpy(m_szForceMasteryName[i], pFld->m_strMastKorName);
				break;
			}
		}
	}

	m_bOper = true;

	return true;
}

bool CAnalysisAvatorData::UnpackData(_AVATOR_VIEWER* OUT pViewer, _AVATOR_DATA* IN pData, DWORD dwWorldCode)
{
	if(!_UnpackAnalysisBase(&pViewer->m_Base, pData, dwWorldCode))
		return false;

	if(!_UnpackAnalysisGeneral(&pViewer->m_General, pData))
		return false;

	if(!_UnpackAnalysisSkill(pViewer->m_Skill, pData))
		return false;

	if(!_UnpackAnalysisForce(pViewer->m_Force, pData))
		return false;

	if(!_UnpackAnalysisEquip(pViewer->m_Equip, pData))
		return false;

	if(!_UnpackAnalysisInven(pViewer->m_Inven, pData))
		return false;

	if(!_UnpackAnalysisForceInven(pViewer->m_FItem, pData))
		return false;

	if(!_UnpackAnalysisBelt(pViewer->m_Belt, pData))
		return false;

	if(!_UnpackAnalysisLeftRes(pViewer->m_LeftRes, pData))
		return false;

	return true;
}

bool CAnalysisAvatorData::PackData(_AVATOR_DATA* OUT pData, _AVATOR_VIEWER* IN pViewer, _AVATOR_DATA* IN pOriginalData)
{
	memcpy(pData, pOriginalData, sizeof(_AVATOR_DATA));

	if(!_PackAnalysisBase(&pData->dbAvator, pViewer, pOriginalData))
		return false;

	if(!_PackAnalysisGeneral(&pData->dbStat, pViewer))
		return false;

	if(!_PackAnalysisEquip(&pData->dbEquip, pViewer, pOriginalData))
		return false;

	if(!_PackAnalysisInven(&pData->dbInven, pViewer))
		return false;

	if(!_PackAnalysisForceInven(&pData->dbForce, pViewer))
		return false;

	if(!_PackAnalysisBelt(&pData->dbBelt, pViewer))
		return false;

	return true;
}

bool CAnalysisAvatorData::CheckDataIntegrity(_AVATOR_DATA* IN pNewData, _AVATOR_DATA* IN pOldData)
{
	///////////////////////
	//	_AVATOR_DB_BASE
	///////////////////////

	_AVATOR_DB_BASE* pNBase = &pNewData->dbAvator;
	_AVATOR_DB_BASE* pOBase = &pOldData->dbAvator;

	if(strcmp(pNBase->m_szAvatorName, pOBase->m_szAvatorName))
	{
		sprintf(m_szLastErrMsg, "케릭터이름이 다름");
		return false;
	}
	if(pNBase->m_dwRecordNum != pOBase->m_dwRecordNum)
	{
		sprintf(m_szLastErrMsg, "케릭터 시리얼번호가 다름");
		return false;
	}
	if(pNBase->m_byRaceSexCode != pOBase->m_byRaceSexCode)
	{
		sprintf(m_szLastErrMsg, "케릭터 종족코드가 다름");
		return false;
	}
	if(pNBase->m_byLevel > max_level)
	{
		sprintf(m_szLastErrMsg, "레벨이 최대치를 넘었음");
		return false;
	}
	if(pNBase->m_byBagNum > max_bag_num)
	{
		sprintf(m_szLastErrMsg, "가방수가 최대치를 넘었음");
		return false;
	}
	if(pNBase->m_byMapCode != pOBase->m_byMapCode)
	{
		sprintf(m_szLastErrMsg, "맵코드가 다름");
		return false;
	}
	for(int i = 0; i < 3; i++)
	{
		if(pNBase->m_fStartPos[i] != pOBase->m_fStartPos[i])
		{
			sprintf(m_szLastErrMsg, "케릭터의 위치가 다름");
			return false;
		}
	}
	if(pNBase->m_dwBaseShape != pOBase->m_dwBaseShape)
	{
		sprintf(m_szLastErrMsg, "케릭터의 기본모양이 다름");
		return false;
	}

	///////////////////////
	//	_BELT_DB_BASE
	///////////////////////

	_BELT_DB_BASE::_POTION_LIST* pNBelt = pNewData->dbBelt.m_PotionList;

	for(i = 0; i < potion_belt_num; i++)
	{		
		if(!pNBelt[i].Key.IsFilled())
			continue;
		if(pNBelt[i].Key.wItemIndex >= m_tblItemData[tbl_code_potion].GetRecordNum())
		{
			sprintf(m_szLastErrMsg, "포션창 %d: rec idx 범위초과", i);
			return false;
		}
		if(pNBelt[i].Key.byAmount > max_overlap_num)
		{
			sprintf(m_szLastErrMsg, "포션창 %d: 갯수초과", i);
			return false;
		}
	}

	_BELT_DB_BASE::_LINK_LIST* pNLink = pNewData->dbBelt.m_LinkList;
	_BELT_DB_BASE::_LINK_LIST* pOLink = pOldData->dbBelt.m_LinkList;

	for(i = 0; i < sf_linker_num; i++)
	{
		if(pNLink[i].Key.CovDBKey() != pOLink[i].Key.CovDBKey())
		{
			sprintf(m_szLastErrMsg, "링크창 %d: 슬롯변경", i);
			return false;
		}
	}

	///////////////////////
	//	_EQUIP_DB_BASE
	///////////////////////
	
	for(i = 0; i < equip_fix_num; i++)
	{
		_EQUIPKEY* pEquipKey = &pNewData->dbAvator.m_EquipKey[i];
		DWORD* pdwEquipUpt = &pNewData->dbAvator.m_dwFixEquipLv[i];

		if(!pEquipKey->IsFilled())
			continue;

		if(pEquipKey->zItemIndex >= m_tblItemData[i].GetRecordNum())
		{
			sprintf(m_szLastErrMsg, "장착창 %d: rec idx 범위초과", i);
			return false;
		}
		BYTE byLimLv = ::GetItemUpgLimSocket(*pdwEquipUpt);
		BYTE byMaxLv = ::GetDefItemUpgSocketNum(i, pEquipKey->zItemIndex);
		if(byMaxLv == 0)
		{
			if(*pdwEquipUpt != __DF_UPT)
			{
				sprintf(m_szLastErrMsg, "장착창 %d: upgrade정보오류", i);
				return false;
			}
		}
		if(byLimLv > byMaxLv)
		{
			sprintf(m_szLastErrMsg, "장착창 %d: 한계레벨 > 최대레벨", i);
			return false;
		}
		if(byMaxLv > 0)
		{
			if(::GetItemUpgedLv(*pdwEquipUpt) > byLimLv)
			{
				sprintf(m_szLastErrMsg, "장착창 %d: 레벨 > 한계레벨", i);
				return false;
			}
		}
	}

	_EQUIP_DB_BASE::_EMBELLISH_LIST* pNEmbellish = pNewData->dbEquip.m_EmbellishList;

	for(i = 0; i < embellish_fix_num; i++)
	{
		if(!pNEmbellish[i].Key.IsFilled())
			continue;

//		BYTE byTableCode = ((BYTE*)&pNEmbellish[i].dwKey)[0];
//		WORD wRecordIndex = ((WORD*)&pNEmbellish[i].dwKey)[1];

		if(pNEmbellish[i].Key.byTableCode != tbl_code_ring && 
			pNEmbellish[i].Key.byTableCode != tbl_code_amulet && 
			pNEmbellish[i].Key.byTableCode != tbl_code_bullet)
		{
			sprintf(m_szLastErrMsg, "장신창 %d: 못넣는 아이템", i);
			return false;
		}
		if(pNEmbellish[i].Key.wItemIndex >= m_tblItemData[pNEmbellish[i].Key.byTableCode].GetRecordNum())
		{
			sprintf(m_szLastErrMsg, "장신창 %d: rec idx 범위초과", i);
			return false;
		}
	}

	///////////////////////
	//	_FORCE_DB_BASE
	///////////////////////

	_FORCE_DB_BASE::_LIST* pNForce = pNewData->dbForce.m_List;

	for(i = 0; i < force_storage_num; i++)
	{
		if(!pNForce[i].Key.IsFilled())
			continue;
		if(pNForce[i].Key.GetIndex() >= m_tblItemData[tbl_code_fcitem].GetRecordNum())
		{
			sprintf(m_szLastErrMsg, "포스창 %d: rec idx 범위초과", i);
			return false;
		}
		for(int t = i+1; t < force_storage_num; t++)
		{
			if(pNForce[i].Key.GetIndex() == pNForce[t].Key.GetIndex())
			{
				sprintf(m_szLastErrMsg, "포스창 %d: %d슬롯과 중복", i, t);
				return false;
			}
		}
	}

	///////////////////////
	//	_INVEN_DB_BASE
	///////////////////////

	_INVEN_DB_BASE::_LIST* pNInven = pNewData->dbInven.m_List;

	for(i = 0; i < pNewData->dbAvator.m_byBagNum * one_bag_store_num; i++)
	{
		if(!pNInven[i].Key.IsFilled())
			continue;

	//	BYTE byTableCode = ((BYTE*)&pNInven[i].dwKey)[0];
	//	WORD wRecordIndex = ((WORD*)&pNInven[i].dwKey)[1];

		if(pNInven[i].Key.byTableCode >= item_tbl_num)
		{
			sprintf(m_szLastErrMsg, "인벤창 %d: tbl code 오류", i);
			return false;
		}
		if(pNInven[i].Key.wItemIndex >= m_tblItemData[pNInven[i].Key.byTableCode].GetRecordNum())
		{
			sprintf(m_szLastErrMsg, "인벤창 %d: rec idx 범위초과", i);
			return false;
		}
		if(::IsOverLapItem(pNInven[i].Key.byTableCode))
		{
			if(pNInven[i].dwDur > max_overlap_num)
			{
				sprintf(m_szLastErrMsg, "인벤창 %d: 중첩량 초과", i);
				return false;
			}
		}
		BYTE byLimLv = ::GetItemUpgLimSocket(pNInven[i].dwUpt);
		BYTE byMaxLv = ::GetDefItemUpgSocketNum(pNInven[i].Key.byTableCode, pNInven[i].Key.wItemIndex);
		if(byMaxLv == 0)
		{
			if(pNInven[i].dwUpt != __DF_UPT)
			{
				sprintf(m_szLastErrMsg, "인벤창 %d: upgrade정보 오류", i);
				return false;
			}
		}
		if(byLimLv > byMaxLv)
		{
			sprintf(m_szLastErrMsg, "인벤창 %d: 한계레벨 > 최대레벨", i);
			return false;
		}
		if(byMaxLv > 0)
		{
			if(::GetItemUpgedLv(pNInven[i].dwUpt) > byLimLv)
			{
				sprintf(m_szLastErrMsg, "인벤창 %d: 레벨 > 한계레벨", i);
				return false;
			}
		}
	}
	
	///////////////////////
	//	_CUTTING_DB_BASE
	///////////////////////

	_CUTTING_DB_BASE::_LIST* pNRes = pNewData->dbCutting.m_List;
	_CUTTING_DB_BASE::_LIST* pORes = pOldData->dbCutting.m_List;

	if(pNewData->dbCutting.m_byLeftNum != pOldData->dbCutting.m_byLeftNum)
	{
		sprintf(m_szLastErrMsg, "보석창 갯수가 수정됨");
		return false;
	}

	for(i = 0; i < pNewData->dbCutting.m_byLeftNum; i++)
	{
		if(pNRes[i].byResIndex != pORes[i].byResIndex)
		{
			sprintf(m_szLastErrMsg, "보석창 %d: 종류가 수정됨", i);
			return false;
		}
		if(pNRes[i].byAmt != pORes[i].byAmt)
		{
			sprintf(m_szLastErrMsg, "보석창 %d: 갯수가 수정됨", i);
			return false;
		}
	}
	
	return true;
}

bool CAnalysisAvatorData::GetItemNameFromCode(char* IN pszItemCode, char* OUT pszItemName)
{
	int nTableCode = ::GetItemTableCode(pszItemCode);
	if(nTableCode == -1)
		return false;
	
	_base_fld* pFld = m_tblItemData[nTableCode].GetRecord(pszItemCode);

	if(!pFld)
		return false;
	
	strcpy(pszItemName, pFld->m_strCode);

	return true;
}

bool CAnalysisAvatorData::_UnpackAnalysisBase(__AVATOR_BASE* OUT pBase, _AVATOR_DATA* IN pData, DWORD dwWorldCode)
{
	//초기화..
	pBase->Init();

	//케릭터 이름
	strcpy(pBase->szCharName, pData->dbAvator.m_szAvatorName);

	//월드이름
	strcpy(pBase->szWorldName, m_WorldList[dwWorldCode].m_szWorldName);

	//종족
	char*	pRaceName[5] = {"Bellato♂", "Bellato♀", "Cora♂", "Cora♀", "Accretia"};
	strcpy(pBase->szRace, pRaceName[pData->dbAvator.m_byRaceSexCode]);

	//얼굴
	int nFaceIndex = ((pData->dbAvator.m_dwBaseShape && 0x00F00000) >> (5*4)) + (default_per_part * pData->dbAvator.m_byRaceSexCode);
	_FaceItem_fld* pFaceFld = (_FaceItem_fld*)m_tblItemData[tbl_code_face].GetRecord(nFaceIndex);
	strcpy(pBase->szFace, pFaceFld->m_strName);

	//헤어
	int nHairIndex = ((pData->dbAvator.m_dwBaseShape && 0x000F0000) >> (4*4)) + (default_per_part * pData->dbAvator.m_byRaceSexCode);
	_DfnEquipItem_fld* pHairFld = (_DfnEquipItem_fld*)m_tblItemData[tbl_code_helmet].GetRecord(nHairIndex);
	strcpy(pBase->szHair, pHairFld->m_strName);

	//신발
	int nShoesIndex = ((pData->dbAvator.m_dwBaseShape && 0x0000F000) >> (3*4)) + (default_per_part * pData->dbAvator.m_byRaceSexCode);
	_DfnEquipItem_fld* pShoesFld = (_DfnEquipItem_fld*)m_tblItemData[tbl_code_shoe].GetRecord(nShoesIndex);
	strcpy(pBase->szShoes, pShoesFld->m_strName);

	//장갑
	int nGauntletIndex =((pData->dbAvator.m_dwBaseShape && 0x00000F00) >> (2*4)) + (default_per_part * pData->dbAvator.m_byRaceSexCode);
	_DfnEquipItem_fld* pGauntletFld = (_DfnEquipItem_fld*)m_tblItemData[tbl_code_gauntlet].GetRecord(nGauntletIndex);
	strcpy(pBase->szGauntlet, pGauntletFld->m_strName);

	//하의
	int nPantsIndex = ((pData->dbAvator.m_dwBaseShape && 0x000000F0) >> (1*4)) + (default_per_part * pData->dbAvator.m_byRaceSexCode);
	_DfnEquipItem_fld* pPantsFld = (_DfnEquipItem_fld*)m_tblItemData[tbl_code_lower].GetRecord(nPantsIndex);
	strcpy(pBase->szPants, pPantsFld->m_strName);

	//상의
	int nUpperIndex = ((pData->dbAvator.m_dwBaseShape && 0x0000000F) >> (0*4)) + (default_per_part * pData->dbAvator.m_byRaceSexCode);
	_DfnEquipItem_fld* pUpperFld = (_DfnEquipItem_fld*)m_tblItemData[tbl_code_upper].GetRecord(nUpperIndex);
	strcpy(pBase->szUpper, pUpperFld->m_strName);

	return true;
}

bool CAnalysisAvatorData::_UnpackAnalysisGeneral(__AVATOR_GENERAL* OUT pGeneral, _AVATOR_DATA* IN pData)
{
	pGeneral->Init();

	//레벨
	pGeneral->byLevel = pData->dbAvator.m_byLevel;

	//경험치
	pGeneral->dExp = pData->dbAvator.m_dExp;

	//소지골드수
	pGeneral->dwGold = pData->dbAvator.m_dwGold;

	//소지달란트
	pGeneral->dwDalant = pData->dbAvator.m_dwDalant;

	//HP
	pGeneral->dwHP = pData->dbAvator.m_dwHP;

	//FP
	pGeneral->dwFP = pData->dbAvator.m_dwFP;

	//SP
	pGeneral->dwSP = pData->dbAvator.m_dwSP;

	//가방수
	pGeneral->byBagNum = pData->dbAvator.m_byBagNum;

	//무기숙련도
	for(int i = 0; i < WP_CLASS_NUM; i++)
	{
		pGeneral->cntExpWp[i].dwCnt = pData->dbStat.m_dwDamWpCnt[i];
		pGeneral->cntExpWp[i].nLv = (BYTE)sqrt( ((pData->dbStat.m_dwDamWpCnt[i]+1)/1000) + sqrt(pData->dbStat.m_dwDamWpCnt[i]+1) );
	}

	//Defence숙련도
	pGeneral->cntDefence.dwCnt = pData->dbStat.m_dwDefenceCnt;
	pGeneral->cntDefence.nLv = (BYTE)sqrt( ((pData->dbStat.m_dwDefenceCnt+1)/1000) + sqrt(pData->dbStat.m_dwDefenceCnt+1) );

	//Parrying숙련도
	pGeneral->cntParrying.dwCnt = pData->dbStat.m_dwShieldCnt;
	pGeneral->cntParrying.nLv = (BYTE)sqrt( ((pData->dbStat.m_dwShieldCnt+1)/1000) + sqrt(pData->dbStat.m_dwShieldCnt+1) );

	//Make Item 숙련도
	for(i = 0; i < MI_CLASS_NUM; i++)
		pGeneral->cntMakeCum[i].dwCnt = pData->dbStat.m_dwMakeCum[i];
	pGeneral->cntMakeCum[mi_class_shield].nLv = (BYTE)sqrt( (float)(pData->dbStat.m_dwMakeCum[mi_class_shield]/1.1f) * 3.0f + 1);
	pGeneral->cntMakeCum[mi_class_armor].nLv = (BYTE)sqrt( (float)(pData->dbStat.m_dwMakeCum[mi_class_armor]/1.1f) * 3.0f + 1);
	pGeneral->cntMakeCum[mi_class_bullet].nLv = (BYTE)sqrt( (float)(pData->dbStat.m_dwMakeCum[mi_class_armor]/10.0f) * 3.0f + 1);

	//Special/Race 숙련도
	pGeneral->cntSpecialCum.dwCnt = pData->dbStat.m_dwSpecialCum;
	pGeneral->cntSpecialCum.nLv = (BYTE)sqrt( ((pData->dbStat.m_dwSpecialCum+1)/1000) + sqrt(pData->dbStat.m_dwSpecialCum+1) );		

	//맵이름
	_map_fld* pMapFld = m_tblMapData.GetRecord(pData->dbAvator.m_byMapCode);
	strcpy(pGeneral->szMapName, pMapFld->m_strCode);

	//위치
	memcpy(pGeneral->fPos, pData->dbAvator.m_fStartPos, sizeof(float)*3);

	return true;
}

bool CAnalysisAvatorData::_UnpackAnalysisSkill(__AVATOR_SKILL* OUT pSkill, _AVATOR_DATA* IN pData)
{
	for(int i = 0; i < max_skill_num; i++)
	{
		pSkill[i].Init();
	
		_skill_fld* pFld = (_skill_fld*)m_tblEffectData[effect_code_skill].GetRecord(i);

		if(!pFld)
			continue;

		strcpy(pSkill[i].szSkillName, pFld->m_strKorName);
		pSkill[i].wRecIndex = (WORD)i;
		pSkill[i].byClass = (BYTE)pFld->m_nClass;
		pSkill[i].cntExp.dwCnt = pData->dbStat.m_dwSkillCum[i];
		pSkill[i].cntExp.nLv = ::GetSFLv(pFld->m_nLv, pData->dbStat.m_dwSkillCum[i]);
	}

	return true;
}

bool CAnalysisAvatorData::_UnpackAnalysisForce(__AVATOR_FORCE* OUT pForce, _AVATOR_DATA* IN pData)
{
	for(int i = 0; i < force_mastery_num; i++)
	{
		pForce[i].Init();

		strcpy(pForce[i].szMasteryName, m_szForceMasteryName[i]);
		pForce[i].byClass = i/mastery_level_num;
		pForce[i].dwExpCum = pData->dbStat.m_dwForceCum[i];
	}

	return true;
}

bool CAnalysisAvatorData::_UnpackAnalysisEquip(__ITEM_EQUIP* OUT pEquip, _AVATOR_DATA* IN pData)
{
	for(int i = 0; i < equip_fix_num; i++)
	{
		_EQUIPKEY* pEquipKey = &pData->dbAvator.m_EquipKey[i];
		DWORD* pdwEquipUpt = &pData->dbAvator.m_dwFixEquipLv[i];

		pEquip[i].Init();

		if(!pEquipKey->IsFilled())
			continue;

		pEquip[i].byTableCode = i;
		pEquip[i].bySlotIndex = i;
		pEquip[i].wRecIndex = pEquipKey->zItemIndex;

		_base_fld* pFld = m_tblItemData[pEquip[i].byTableCode].GetRecord(pEquip[i].wRecIndex);

		if(!pFld)
			continue;

		strcpy(pEquip[i].szItemCode, pFld->m_strCode);

		char* pszItemName = ::GetItemKorName(pEquip[i].byTableCode, pEquip[i].wRecIndex);
		if(!pszItemName)
			continue;
		strcpy(pEquip[i].szItemName, pszItemName);

		char* pszCivilCode = ::GetItemEquipCivil(pEquip[i].byTableCode, pEquip[i].wRecIndex);
		if(!pszCivilCode)
			continue;
		strcpy(pEquip[i].szCivil, pszCivilCode);

		pEquip[i].byEquipableLv = ::GetItemEquipLevel(pEquip[i].byTableCode, pEquip[i].wRecIndex);
		pEquip[i].dwAmount = 0;//pData->dbEquip.m_EquipList[i].dwDur;
		pEquip[i].byItemCurUpgSocketNum = ::GetItemUpgedLv(*pdwEquipUpt);
		pEquip[i].byItemLimUpgSocketNum = ::GetItemUpgLimSocket(*pdwEquipUpt);

		memset(pEquip[i].byTalikIndex, 0xFF, sizeof(pEquip[i].byTalikIndex));			
		for(int u = 0; u < pEquip[i].byItemCurUpgSocketNum; u++)
			pEquip[i].byTalikIndex[u] = ::GetTalikFromSocket(*pdwEquipUpt, u);

		pEquip[i].bFix = true;
	}

	for(i = equip_fix_num; i < equip_fix_num+embellish_fix_num; i++)
	{
		pEquip[i].Init();

		if(!pData->dbEquip.m_EmbellishList[i-equip_fix_num].Key.IsFilled())
			continue;

		pEquip[i].byTableCode = pData->dbEquip.m_EmbellishList[i-equip_fix_num].Key.byTableCode;
		pEquip[i].bySlotIndex = pData->dbEquip.m_EmbellishList[i-equip_fix_num].Key.bySlotIndex;
		pEquip[i].wRecIndex = pData->dbEquip.m_EmbellishList[i-equip_fix_num].Key.wItemIndex;

		_base_fld* pFld = m_tblItemData[pEquip[i].byTableCode].GetRecord(pEquip[i].wRecIndex);

		if(!pFld)
			continue;

		strcpy(pEquip[i].szItemCode, pFld->m_strCode);

		char* pszItemName = ::GetItemKorName(pEquip[i].byTableCode, pEquip[i].wRecIndex);
		if(!pszItemName)
			continue;
		strcpy(pEquip[i].szItemName, pszItemName);

		char* pszCivilCode = ::GetItemEquipCivil(pEquip[i].byTableCode, pEquip[i].wRecIndex);
		if(!pszCivilCode)
			continue;
		strcpy(pEquip[i].szCivil, pszCivilCode);

		pEquip[i].byEquipableLv = ::GetItemEquipLevel(pEquip[i].byTableCode, pEquip[i].wRecIndex);
		pEquip[i].dwAmount = pData->dbEquip.m_EmbellishList[i-equip_fix_num].wAmount;
		pEquip[i].byItemCurUpgSocketNum = 0;
		pEquip[i].byItemLimUpgSocketNum = 0;
		memset(pEquip[i].byTalikIndex, 0xFF, sizeof(pEquip[i].byTalikIndex));			

		pEquip[i].bFix = true;		
	}

	//장식쪽 slotindex 재정리(중복돼는 인덱스 없애기)
	DWORD dwMaxArrange = equip_fix_num+embellish_fix_num;

	CNetIndexList listReArrange;
	bool bPut[equip_fix_num+embellish_fix_num];
	
	listReArrange.SetList(dwMaxArrange);
	memset(bPut, false, sizeof(bPut));

	//중복으로 위치지정됀거 찾기
	for(DWORD e = 0; e < dwMaxArrange; e++)
	{
		if(pEquip[e].bySlotIndex < dwMaxArrange) 
		{
			if(!bPut[e])
			{
				bPut[e] = true;
				continue;
			}
		}
		listReArrange.PushNode_Back(e);
	}

	DWORD dwList[equip_fix_num+embellish_fix_num];
	int nNum = listReArrange.CopyIndexList(dwList, dwMaxArrange);
	DWORD dwLastPut = 0;
	for(int n = 0; n < nNum; n++)
	{
		//bPut이 false인곳 찾기..
		for(DWORD p = dwLastPut; p < dwMaxArrange; p++)
		{
			if(!bPut[p])
			{
				pEquip[dwList[n]].bySlotIndex = (BYTE)p;
				bPut[p] = true;
				dwLastPut = p;
				break;
			}	
		}
	}

	return true;
}

bool CAnalysisAvatorData::_UnpackAnalysisInven(__ITEM_INVEN* OUT pInven, _AVATOR_DATA* IN pData)
{
	for(int i = 0; i < pData->dbAvator.m_byBagNum*one_bag_store_num; i++)
	{
		pInven[i].Init();

		if(!pData->dbInven.m_List[i].Key.IsFilled())
			continue;

		pInven[i].byTableCode = pData->dbInven.m_List[i].Key.byTableCode;
		pInven[i].bySlotIndex = pData->dbInven.m_List[i].Key.bySlotIndex;
		pInven[i].wRecIndex = pData->dbInven.m_List[i].Key.wItemIndex;

		_base_fld* pFld = m_tblItemData[pInven[i].byTableCode].GetRecord(pInven[i].wRecIndex);

		if(!pFld)
			continue;

		strcpy(pInven[i].szItemCode, pFld->m_strCode);
		char* pszItemName = ::GetItemKorName(pInven[i].byTableCode, pInven[i].wRecIndex);
		if(!pszItemName)
			continue;
		strcpy(pInven[i].szItemName, pszItemName);

		char* pszCivilCode = ::GetItemEquipCivil(pInven[i].byTableCode, pInven[i].wRecIndex);
		if(!pszCivilCode)
			continue;

		strcpy(pInven[i].szCivil, pszCivilCode);
		pInven[i].byEquipableLv = ::GetItemEquipLevel(pInven[i].byTableCode, pInven[i].wRecIndex);
		pInven[i].dwAmount = pData->dbInven.m_List[i].dwDur;
		pInven[i].byItemCurUpgSocketNum = ::GetItemUpgedLv(pData->dbInven.m_List[i].dwUpt);
		pInven[i].byItemLimUpgSocketNum = ::GetItemUpgLimSocket(pData->dbInven.m_List[i].dwUpt);

		memset(pInven[i].byTalikIndex, 0xFF, sizeof(pInven[i].byTalikIndex));			
		for(int u = 0; u < pInven[i].byItemCurUpgSocketNum; u++)
			pInven[i].byTalikIndex[u] = ::GetTalikFromSocket(pData->dbInven.m_List[i].dwUpt, u);

		pInven[i].bFix = true;
	}

	//slotindex 재정리(중복돼는 인덱스 없애기)
	DWORD dwMaxArrange = pData->dbAvator.m_byBagNum*one_bag_store_num;

	CNetIndexList listReArrange;
	bool bPut[bag_storage_num];
	
	listReArrange.SetList(dwMaxArrange);
	memset(bPut, false, sizeof(bPut));

		//중복으로 위치지정됀거 찾기
	for(DWORD e = 0; e < dwMaxArrange; e++)
	{
		if(pInven[e].bySlotIndex < dwMaxArrange) 
		{
			if(!bPut[e])
			{
				bPut[e] = true;
				continue;
			}
		}
		listReArrange.PushNode_Back(e);
	}

	DWORD dwList[bag_storage_num];
	int nNum = listReArrange.CopyIndexList(dwList, dwMaxArrange);
	DWORD dwLastPut = 0;
	for(int n = 0; n < nNum; n++)
	{
		//bPut이 false인곳 찾기..
		for(DWORD p = dwLastPut; p < dwMaxArrange; p++)
		{
			if(!bPut[p])
			{
				pInven[dwList[n]].bySlotIndex = (BYTE)p;
				bPut[p] = true;
				dwLastPut = p;
				break;
			}	
		}
	}

	return true;
}

bool CAnalysisAvatorData::_UnpackAnalysisForceInven(__ITEM_FORCE* OUT pFInven, _AVATOR_DATA* IN pData)
{
	for(int i = 0; i < force_storage_num; i++)
	{
		pFInven[i].Init();

		if(!pData->dbForce.m_List[i].Key.IsFilled())
			continue;	

		_ForceItem_fld* pFld = (_ForceItem_fld*)m_tblItemData[tbl_code_fcitem].GetRecord(pData->dbForce.m_List[i].Key.GetIndex());
		if(!pFld)
			continue;	

		_force_fld* pEfFld = (_force_fld*)m_tblEffectData[effect_code_force].GetRecord(pFld->m_strForce_Codekey);
		if(!pEfFld)
			continue;	

		strcpy(pFInven[i].szForceCode, pFld->m_strCode);
		strcpy(pFInven[i].szForceName, pFld->m_strName);
		pFInven[i].byClass = pEfFld->m_nClass;
		pFInven[i].cntExp.dwCnt = pData->dbForce.m_List[i].Key.GetStat();
		pFInven[i].cntExp.nLv = ::GetSFLv(pEfFld->m_nLv, pData->dbForce.m_List[i].Key.GetStat());

		pFInven[i].bFix = true;
	}

	return true;
}

bool CAnalysisAvatorData::_UnpackAnalysisBelt(__ITEM_BELT* OUT pBelt, _AVATOR_DATA* IN pData)
{
	for(int i = 0; i < potion_belt_num; i++)
	{
		pBelt[i].Init();

		if(pData->dbBelt.m_PotionList[i].Key.IsFilled())
			continue;

		pBelt[i].byTableCode = tbl_code_potion;
		pBelt[i].bySlotIndex = pData->dbBelt.m_PotionList[i].Key.bySlotIndex;
		pBelt[i].wRecIndex = pData->dbBelt.m_PotionList[i].Key.wItemIndex;

		_base_fld* pFld = m_tblItemData[tbl_code_potion].GetRecord(pBelt[i].wRecIndex);

		if(!pFld)
			continue;

		strcpy(pBelt[i].szItemCode, pFld->m_strCode);

		char* pszItemName = ::GetItemKorName(pBelt[i].byTableCode, pBelt[i].wRecIndex);
		if(!pszItemName)
			continue;
		strcpy(pBelt[i].szItemName, pszItemName);
		pBelt[i].dwAmount = pData->dbBelt.m_PotionList[i].Key.byAmount;

		pBelt[i].bFix = true;
	}

	//slotindex 재정리(중복돼는 인덱스 없애기)
	DWORD dwMaxArrange = potion_belt_num;

	CNetIndexList listReArrange;
	bool bPut[potion_belt_num];
	
	listReArrange.SetList(dwMaxArrange);
	memset(bPut, false, sizeof(bPut));

	//중복으로 위치지정됀거 찾기
	for(DWORD e = 0; e < dwMaxArrange; e++)
	{
		if(pBelt[e].bySlotIndex < dwMaxArrange) 
		{
			if(!bPut[e])
			{
				bPut[e] = true;
				continue;
			}
		}
		listReArrange.PushNode_Back(e);
	}

	DWORD dwList[potion_belt_num];
	int nNum = listReArrange.CopyIndexList(dwList, dwMaxArrange);
	DWORD dwLastPut = 0;
	for(int n = 0; n < nNum; n++)
	{
		//bPut이 false인곳 찾기..
		for(DWORD p = dwLastPut; p < dwMaxArrange; p++)
		{
			if(!bPut[p])
			{
				pBelt[dwList[n]].bySlotIndex = (BYTE)p;
				bPut[p] = true;
				dwLastPut = p;
				break;
			}	
		}
	}

	return true;
}

bool CAnalysisAvatorData::_UnpackAnalysisLeftRes(__ITEM_RBUFFER* OUT pRes, _AVATOR_DATA* IN pData)
{
	for(int i = 0; i < left_cutting_num; i++)
	{
		pRes[i].Init();

		if(pData->dbCutting.m_List[i].byResIndex == 0xFF)
			continue;

		_ResourceItem_fld* pFld = (_ResourceItem_fld*)m_tblItemData[tbl_code_res].GetRecord(pData->dbCutting.m_List[i].byResIndex);

		if(!pFld)
			continue;

		pRes[i].wRecIndex = pData->dbCutting.m_List[i].byResIndex;
		strcpy(pRes[i].szResCode, pFld->m_strCode);
		strcpy(pRes[i].szResName, pFld->m_strName);
	
		int* pnPrice = &pFld->m_nAncStdPrice;

		pRes[i].dwPrice = pnPrice[pData->dbAvator.m_byRaceSexCode/2];
		pRes[i].dwAmount = pData->dbCutting.m_List[i].byAmt;

		pRes[i].bFix = true;
	}

	return true;
}

bool CAnalysisAvatorData::_PackAnalysisBase(_AVATOR_DB_BASE* OUT pBase, _AVATOR_VIEWER* IN pViewer, _AVATOR_DATA* IN pOriginalData)
{
	_AVATOR_DB_BASE base;
	memcpy(&base, pBase, sizeof(_AVATOR_DB_BASE));

	//가방갯수
	if(pViewer->m_General.byBagNum > max_bag_num)
	{
		sprintf(m_szLastErrMsg, "가방의 수(%d)가 최대 %d보다 큼", pViewer->m_General.byBagNum, max_bag_num);
		return false;
	}
	base.m_byBagNum = pViewer->m_General.byBagNum;

	//레벨
	if(pViewer->m_General.byLevel > max_level)
	{
		sprintf(m_szLastErrMsg, "레벨이(%d)가 최대 %d보다 큼", pViewer->m_General.byLevel, max_level);
		return false;
	}
	base.m_byLevel = pViewer->m_General.byLevel;

	//경험치
	if(pViewer->m_General.byLevel == 0)
	{
		pViewer->m_General.dExp = 0;
	}
	else
	{
		_exp_fld* pExpRcd = (_exp_fld*)m_tblExp.GetRecord(pViewer->m_General.byLevel);
		double dExp = atof(pExpRcd->m_strAccretia);
		if(pViewer->m_General.dExp >= dExp)
			pViewer->m_General.dExp = dExp-1;
	}
	base.m_dExp = pViewer->m_General.dExp;

	//달란트
	base.m_dwDalant = pViewer->m_General.dwDalant;

	//골드
	base.m_dwGold = pViewer->m_General.dwGold;

	//FP
	base.m_dwFP = pViewer->m_General.dwFP;

	//HP
	base.m_dwHP = pViewer->m_General.dwHP;

	//SP
	base.m_dwSP = pViewer->m_General.dwSP;

	//장착..
	for(int i = 0; i < equip_fix_num; i++)
	{
		__ITEM_EQUIP* pe = &pViewer->m_Equip[i];

		if(!pe->bFix)
		{
			base.m_EquipKey[i].SetRelease();
			continue;
		}

		//테이블과 레코드인덱스 찾기
		int nTableCode = ::GetItemTableCode(pe->szItemCode);
		if(nTableCode == -1)
		{
			sprintf(m_szLastErrMsg, "장착아이템중 %s는 없는 아이템임", pe->szItemCode);
			return false;
		}

		_base_fld* pFld = m_tblItemData[nTableCode].GetRecord(pe->szItemCode);
		if(!pFld)
		{
			sprintf(m_szLastErrMsg, "장착아이템중 %s는 없는 아이템임", pe->szItemCode);
			return false;
		}
		WORD wItemRecIndex = (WORD)pFld->m_dwIndex;

		//테이블과 장착순서가 맞는지..
		if(i != nTableCode)
		{
			sprintf(m_szLastErrMsg, "%s의 장착순서가 틀림", pe->szItemCode);
			return false;
		}

		//장착가능종족인지
		if(!IsItemEquipCivil(nTableCode, wItemRecIndex, pOriginalData->dbAvator.m_byRaceSexCode))
		{
			sprintf(m_szLastErrMsg, "%s는 종족상 장착할수없는 아이템임", pe->szItemCode);
			return false;
		}

		DWORD dwLvBit = __DF_UPT;
		if(::GetDefItemUpgSocketNum(nTableCode, wItemRecIndex) > 0)
		{
			//아이템 업그레이드 제한레벌 맞는지..
			if(pe->byItemLimUpgSocketNum > GetDefItemUpgSocketNum(nTableCode, wItemRecIndex))
			{
				sprintf(m_szLastErrMsg, "%s는 종족상 장착할수없는 아이템임", pe->szItemCode);
				return false;
			}		

			//현재레별구하기..
			int nCurLv = 0;
			dwLvBit = ::GetBitAfterSetLimSocket(pe->byItemLimUpgSocketNum);
			for(int v = 0; v < MAX_ITEM_LV; v++)
			{
				if(pe->byTalikIndex[v] == 0xFF)
					break;
				else if(MAX_TALIK_NUM > pe->byTalikIndex[v])
				{
					dwLvBit = ::GetBitAfterUpgrade(dwLvBit, pe->byTalikIndex[v], nCurLv);
					nCurLv++;
				}
				else
				{
					sprintf(m_szLastErrMsg, "%s에 존재하지않는 탈릭코드(%d)가 있음", pe->szItemCode, pe->byTalikIndex[v]);
					return false;
				}
			}
			if(nCurLv > pe->byItemLimUpgSocketNum)
			{
				sprintf(m_szLastErrMsg, "%s이 현재레벨(%d)이 제한레벨(%d)보다 큼 있음", pe->szItemCode, nCurLv, pe->byItemLimUpgSocketNum);
				return false;
			}
		}

		//packing..

		base.m_EquipKey[i].zItemIndex = wItemRecIndex;
		base.m_dwFixEquipLv[i] = dwLvBit;
	}

	memcpy(pBase, &base, sizeof(_AVATOR_DB_BASE));

	return true;
}

bool CAnalysisAvatorData::_PackAnalysisGeneral(_STAT_DB_BASE* OUT pGeneral, _AVATOR_VIEWER* IN pViewer)
{
	_STAT_DB_BASE base;
	memcpy(&base, pGeneral, sizeof(_STAT_DB_BASE));

	//무기숙련도
	for(int i = 0; i < WP_CLASS_NUM; i++)
	{
		base.m_dwDamWpCnt[i] = pViewer->m_General.cntExpWp[i].dwCnt;
	}

	//방어숙련도
	base.m_dwDefenceCnt = pViewer->m_General.cntDefence.dwCnt;

	//방패숙련도
	base.m_dwShieldCnt = pViewer->m_General.cntParrying.dwCnt;

	//제작숙련도
	for(i = 0; i < MI_CLASS_NUM; i++)
	{
		base.m_dwMakeCum[i] = pViewer->m_General.cntMakeCum[i].dwCnt;
	}

	//특수숙련도
	base.m_dwSpecialCum = pViewer->m_General.cntSpecialCum.dwCnt;

	//포스마스터리별 숙련도
	for(i = 0; i < force_mastery_num; i++)
	{
		base.m_dwForceCum[i] = pViewer->m_Force[i].dwExpCum;
	}

	//스킬별 숙련도
	for(i = 0; i < max_skill_num; i++)
	{
		base.m_dwSkillCum[i] = pViewer->m_Skill[i].cntExp.dwCnt;
	}

	memcpy(pGeneral, &base, sizeof(_STAT_DB_BASE));

	return true;
}

bool CAnalysisAvatorData::_PackAnalysisEquip(_EQUIP_DB_BASE* OUT pEquip, _AVATOR_VIEWER* IN pViewer, _AVATOR_DATA* IN pOriginalData)
{
	_EQUIP_DB_BASE base;
	memcpy(&base, pEquip, sizeof(_EQUIP_DB_BASE));

	//장식..
	for(int i = 0; i < embellish_fix_num; i++)
	{
		__ITEM_EQUIP* pe = &pViewer->m_Equip[i+equip_fix_num];

		if(!pe->bFix)
		{
			base.m_EmbellishList[i].Key.SetRelease();
			continue;
		}

		//테이블과 레코드인덱스 찾기
		int nTableCode = ::GetItemTableCode(pe->szItemCode);
		if(nTableCode == -1)
		{
			sprintf(m_szLastErrMsg, "장착아이템중 %s는 없는 아이템임", pe->szItemCode);
			return false;
		}

		_base_fld* pFld = m_tblItemData[nTableCode].GetRecord(pe->szItemCode);
		if(!pFld)
		{
			sprintf(m_szLastErrMsg, "장착아이템중 %s는 없는 아이템임", pe->szItemCode);
			return false;
		}
		WORD wItemRecIndex = (WORD)pFld->m_dwIndex;

		//장착가능종족인지
		if(!IsItemEquipCivil(nTableCode, wItemRecIndex, pOriginalData->dbAvator.m_byRaceSexCode))
		{
			sprintf(m_szLastErrMsg, "%s는 종족상 장착할수없는 아이템임", pe->szItemCode);
			return false;
		}

		//packing..

		base.m_EmbellishList[i].Key.byTableCode = (BYTE)nTableCode;
		base.m_EmbellishList[i].Key.bySlotIndex = (BYTE)pe->bySlotIndex-equip_fix_num;	
		base.m_EmbellishList[i].Key.wItemIndex = wItemRecIndex;	
		base.m_EmbellishList[i].wAmount = (WORD)pe->dwAmount;		
	}

	memcpy(pEquip, &base, sizeof(_EQUIP_DB_BASE));

	return true;
}

bool CAnalysisAvatorData::_PackAnalysisInven(_INVEN_DB_BASE* OUT pInven, _AVATOR_VIEWER* IN pViewer)
{
	_INVEN_DB_BASE base;
	memcpy(&base, pInven, sizeof(_INVEN_DB_BASE));

	for(int i = 0; i < pViewer->m_General.byBagNum*one_bag_store_num; i++)
	{
		__ITEM_INVEN* pe = &pViewer->m_Inven[i];

		if(!pe->bFix)
		{
			base.m_List[i].Key.SetRelease();
			continue;
		}

		//테이블과 레코드인덱스 찾기
		int nTableCode = ::GetItemTableCode(pe->szItemCode);
		if(nTableCode == -1)
		{
			sprintf(m_szLastErrMsg, "인벤아이템중 %s는 없는 아이템임", pe->szItemCode);
			return false;
		}

		_base_fld* pFld = m_tblItemData[nTableCode].GetRecord(pe->szItemCode);
		if(!pFld)
		{
			sprintf(m_szLastErrMsg, "인벤아이템중 %s는 없는 아이템임", pe->szItemCode);
			return false;
		}
		WORD wItemRecIndex = (WORD)pFld->m_dwIndex;

		DWORD dwLvBit = __DF_UPT;
		if(::GetDefItemUpgSocketNum(nTableCode, wItemRecIndex) > 0)
		{
			//아이템 업그레이드 제한레벌 맞는지..
			if(pe->byItemLimUpgSocketNum > ::GetDefItemUpgSocketNum(nTableCode, wItemRecIndex))
			{
				sprintf(m_szLastErrMsg, "%s는 종족상 장착할수없는 아이템임", pe->szItemCode);
				return false;
			}		
			
			//현재레별구하기..
			int nCurLv = 0;
			dwLvBit = ::GetBitAfterSetLimSocket(pe->byItemLimUpgSocketNum);
			for(int v = 0; v < MAX_ITEM_LV; v++)
			{
				if(pe->byTalikIndex[v] == 0xFF)
					break;
				else if(MAX_TALIK_NUM > pe->byTalikIndex[v])
				{
					dwLvBit = ::GetBitAfterUpgrade(dwLvBit, pe->byTalikIndex[v], nCurLv);
					nCurLv++;
				}
				else
				{
					sprintf(m_szLastErrMsg, "%s에 존재하지않는 탈릭코드(%d)가 있음", pe->szItemCode, pe->byTalikIndex[v]);
					return false;
				}
			}
			if(nCurLv > pe->byItemLimUpgSocketNum)
			{
				sprintf(m_szLastErrMsg, "%s이 현재레벨(%d)이 제한레벨(%d)보다 큼 있음", pe->szItemCode, nCurLv, pe->byItemLimUpgSocketNum);
				return false;
			}
		}

		//packing..

		base.m_List[i].Key.byTableCode = (BYTE)nTableCode;
		base.m_List[i].Key.bySlotIndex = (BYTE)pe->bySlotIndex;
		base.m_List[i].Key.wItemIndex = wItemRecIndex;	
		base.m_List[i].dwDur = pe->dwAmount;
		base.m_List[i].dwUpt = dwLvBit;
	}
	
	memcpy(pInven, &base, sizeof(_INVEN_DB_BASE));

	return true;
}

bool CAnalysisAvatorData::_PackAnalysisForceInven(_FORCE_DB_BASE* OUT pFInven, _AVATOR_VIEWER* IN pViewer)
{
	_FORCE_DB_BASE base;
	memcpy(&base, pFInven, sizeof(_FORCE_DB_BASE));

	for(int i = 0; i < force_storage_num; i++)
	{
		__ITEM_FORCE* pe = &pViewer->m_FItem[i];

		if(!pe->bFix)
		{
			base.m_List[i].Key.SetRelease();
			continue;
		}

		//테이블과 레코드인덱스 찾기
		int nTableCode = ::GetItemTableCode(pe->szForceCode);
		if(nTableCode == -1)
		{
			sprintf(m_szLastErrMsg, "포스아이템중 %s는 없는 아이템임", pe->szForceCode);
			return false;
		}

		_base_fld* pFld = m_tblItemData[nTableCode].GetRecord(pe->szForceCode);
		if(!pFld)
		{
			sprintf(m_szLastErrMsg, "포스아이템중 %s는 없는 아이템임", pe->szForceCode);
			return false;
		}
		WORD wItemRecIndex = (WORD)pFld->m_dwIndex;

		if(nTableCode != tbl_code_fcitem)
		{
			sprintf(m_szLastErrMsg, "포스창에 포스아이템이 아닌것(%s)을 넣었음", pe->szForceCode);
			return false;
		}

		//packing..

		base.m_List[i].Key.SetKey((BYTE)wItemRecIndex, pe->cntExp.dwCnt);
	}
	
	memcpy(pFInven, &base, sizeof(_FORCE_DB_BASE));

	return true;
}

bool CAnalysisAvatorData::_PackAnalysisBelt(_BELT_DB_BASE* OUT pBelt, _AVATOR_VIEWER* IN pViewer)
{
	_BELT_DB_BASE base;
	memcpy(&base, pBelt, sizeof(_BELT_DB_BASE));

	for(int i = 0; i < potion_belt_num; i++)
	{
		__ITEM_BELT* pe = &pViewer->m_Belt[i];

		if(!pe->bFix)
		{
			base.m_PotionList[i].Key.SetRelease();
			continue;
		}

		//테이블과 레코드인덱스 찾기
		int nTableCode = ::GetItemTableCode(pe->szItemCode);
		if(nTableCode == -1)
		{
			sprintf(m_szLastErrMsg, "벨트아이템중 %s는 없는 아이템임", pe->szItemCode);
			return false;
		}

		_base_fld* pFld = m_tblItemData[nTableCode].GetRecord(pe->szItemCode);
		if(!pFld)
		{
			sprintf(m_szLastErrMsg, "벨트아이템중 %s는 없는 아이템임", pe->szItemCode);
			return false;
		}
		WORD wItemRecIndex = (WORD)pFld->m_dwIndex;

		//포션인지
		if(nTableCode != tbl_code_potion)
		{
			sprintf(m_szLastErrMsg, "벨트아이템중 %s는 포션아이템이 아님", pe->szItemCode);
			return false;
		}

		//수량검사
		if(::IsOverLapItem(nTableCode))
		{
			if(pe->dwAmount > max_overlap_num)
				pe->dwAmount = max_overlap_num;
		}

		//packing..

		base.m_PotionList[i].Key.bySlotIndex = (BYTE)pe->bySlotIndex;
		base.m_PotionList[i].Key.wItemIndex = wItemRecIndex;	
		base.m_PotionList[i].Key.byAmount = (BYTE)pe->dwAmount;		
	}
	
	memcpy(pBelt, &base, sizeof(_BELT_DB_BASE));

	return true;
}

int GetSFLv(int nLv, DWORD dwHitCount)
{
	static float sR[mastery_level_num] = { 200, 200, 200, 200 };

	int nLevel = (int)(sqrt( (float)dwHitCount/sR[nLv] ) + 0.9999);

	if(nLevel > 7)			
		nLevel = 7;
	else if(nLevel < 1)		
		nLevel = 1;

	return nLevel;
}


bool CAnalysisAvatorData::GetItemName( char* IN szItemCode, char* OUT szItemName )
{

	int nTableCode = ::GetItemTableCode(szItemCode);
	if(nTableCode == -1)
	{
		sprintf(m_szLastErrMsg, "인벤아이템중 %s는 없는 아이템입니다", szItemCode);
		return false;
	}

	_base_fld* pFld = m_tblItemData[nTableCode].GetRecord(szItemCode);
	if(!pFld)
	{
		sprintf(m_szLastErrMsg, "인벤아이템중 %s는 없는 아이템입니다", szItemCode);
		return false;
	}
	WORD wItemRecIndex = (WORD)pFld->m_dwIndex;

	char* pSzItemName = ::GetItemKorName(nTableCode, wItemRecIndex);
	if ( pSzItemName != NULL )
	{
		strcpy( szItemName, pSzItemName );
	}
	else
		return false;

	return true;
}

bool CAnalysisAvatorData::GetItemName( DWORD dwItemCode, char* OUT szItemName )
{
	int nTableCode;
	WORD wItemIndex;


	SplitItemCode( dwItemCode, &nTableCode, &wItemIndex );

	char* pszItemName = ::GetItemKorName(nTableCode, wItemIndex);
	if(!pszItemName)
		return false;

	strcpy(szItemName, pszItemName);

	return true;
}

DWORD CAnalysisAvatorData::GetItemCode_K( char* szItemCode )
{
	int nTableCode = ::GetItemTableCode(szItemCode);
	if(nTableCode == -1)
	{
		sprintf(m_szLastErrMsg, "인벤아이템중 %s는 없는 아이템입니다", szItemCode);
		return 0xFFFFFFFF;
	}

	_base_fld* pFld = m_tblItemData[nTableCode].GetRecord(szItemCode);
	if(!pFld)
	{
		sprintf(m_szLastErrMsg, "인벤아이템중 %s는 없는 아이템입니다", szItemCode);
		return 0xFFFFFFFF;
	}

	_INVENKEY ItemCode_K;
	ItemCode_K.SetRelease();
	ItemCode_K.bySlotIndex	=	0;
	ItemCode_K.byTableCode	=	nTableCode;
	ItemCode_K.wItemIndex	=	(WORD)pFld->m_dwIndex;


	return ItemCode_K.CovDBKey();
}

void CAnalysisAvatorData::SplitItemCode( DWORD dwItemCode, int* pnTableCode, WORD* pwItemIndex )
{

	_INVENKEY ItemCode_K;
	ItemCode_K.SetRelease();
	ItemCode_K.LoadDBKey( dwItemCode );

	*pnTableCode = ItemCode_K.byTableCode;
	*pwItemIndex = ItemCode_K.wItemIndex;
}