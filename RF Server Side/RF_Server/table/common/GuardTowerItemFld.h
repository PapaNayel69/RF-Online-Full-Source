#ifndef _GuardTowerItem_h_
#define _GuardTowerItem_h_

#include "BaseFld.h"

#define max_tower_material 3

struct _GuardTowerItem_fld : public _base_fld
{ 
	_str_code   m_strModle;              // 모델링코드
	_str_code   m_strCharMeshID;         // 매쉬ID
	_str_code   m_strName;               // 이름
	_str_code   m_strCivil;              // 문명
	int         m_nLevelLim;             // 레벨제한
	int         m_nExpertLim;            // 스킬제한
	int         m_nExpTime;              // 완성시간
	int         m_nEffectGroup;          // 공격형태
	int         m_nIconIDX;              // 아이콘종류(클)
	int         m_nLevel;                // 레벨
	int         m_nHeight;               // 높이
	int         m_nWidth;                // 폭
	int         m_nGADst;                // 공격거리
	int         m_nGASpd;                // 일공공격속도
	int         m_nAttSklUnit;           // 공격기술숙련도
	int         m_nGAMinAF;              // 일공최소공격력
	int         m_nGAMaxAF;              // 일공최대공격력
	int         m_nGAMinSelProb;         // 일공최소공격력선택확률
	int         m_nGAMaxSelProb;         // 일공최대공격력선택확률
	int         m_nDefSklUnit;           // 방어기술숙련도
	int         m_nDefFc;                // 방어력
	float       m_fDefGap;               // 상대방어력간격조정값
	float       m_fDefFacing;            // 상대방어력값
	int         m_nProperty;             // 속성
	int         m_nFireTol;              // 불내성
	int         m_nWaterTol;             // 물내성
	int         m_nSoilTol;              // 흙내성
	int         m_nWindTol;              // 바람내성
	int         m_nMaxHP;                // 최대내구도(HP)
	int         m_nMoney;                // 돈구분
	int         m_nStdPrice;             // 기준가격

	struct __material {
		_str_code   strMaterialCode;      // 재료코드
		int         nMaterialNum;         // 재료개수
	};

	__material	m_Material[max_tower_material];

	BOOL        m_bSell;                 // 팔기여부
	BOOL        m_bExchange;             // 주는(교환)여부
	BOOL        m_bGround;               // 그라운드여부
	BOOL        m_bUpgrade;              // 업글가능여부
	_str_code   m_strTooltipIndex;       // 아이템설명
	int         m_nAttEffType;           // 공격이펙트재질
	int         m_nDefEffType;           // 방어이펙트재질

	_GuardTowerItem_fld()
	{
		ZeroMemory( this, sizeof( *this ) );
	}
};

#endif
