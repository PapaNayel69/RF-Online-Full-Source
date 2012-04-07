#ifndef _BattleDungeonItem_h_
#define _BattleDungeonItem_h_

#include "BaseFld.h"

struct _BattleDungeonItem_fld : public _base_fld
{ 
	BOOL        m_bExist;                // 존재여부
	_str_code   m_strModle;              // 모델링코드(클)
	int         m_nIconIDX;              // 아이콘종류(클)
	_str_code   m_strName;               // 아이템이름
	int         m_nKindClt;              // 종류(클)
	int         m_nFixPart;              // 장착부위
	_str_code   m_strCivil;              // 문명
	_str_code   m_strMapCode;            // 맵코드
	_str_code   m_strDummyName;          // 더미이름
	int         m_nGenMob;               // 몹생성구분
	int         m_nMoney;                // 돈구분
	int         m_nStdPrice;             // 기준가격
	BOOL        m_bQualification;        // 이동자격
	float       m_fMobAdd;               // 1PC당몹증가%
	int         m_nTerminateTime;        // 종료시간(초)
	int         m_nEndIF;                // 종료조건
	_str_code   m_strMobID;              // 종료조건MobID
	int         m_nMobCount;             // 종료조건MobKillCount
	BOOL        m_bItemLooting;          // 아이템루팅여부
	int         m_nExp;                  // 경험치보상
	int         m_nDalant;               // 달란트보상
	int         m_nGold;                 // 골드보상
	BOOL        m_bSell;                 // 팔기여부
	BOOL        m_bExchange;             // 주는(교환)여부
	BOOL        m_bGround;               // 그라운드여부
	_str_code   m_strTooltipIndex;       // 아이템설명
	_str_code   m_strMissionIndex;       // 미션설명

	_BattleDungeonItem_fld()
	{
		ZeroMemory( this, sizeof( *this ) );
	}
};

#endif
