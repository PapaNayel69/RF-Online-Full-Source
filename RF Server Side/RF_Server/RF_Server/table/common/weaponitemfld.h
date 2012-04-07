#ifndef _WeaponItem_h_
#define _WeaponItem_h_

#include "BaseFld.h"

struct _WeaponItem_fld : public _base_fld
{ 
	BOOL        m_bExist;                // 존재여부
	_str_code   m_strModle;              // 모델링코드(클)
	int         m_nIconIDX;              // 아이콘종류(클)
	_str_code   m_strName;               // 아이템이름
	int         m_nKindClt;              // 종류(클)
	int         m_nFixPart;              // 장착부위
	int         m_nType;                 // 무기종류
	_str_code   m_strCivil;              // 문명
	int         m_nLevelLim;             // 레벨제한
	int         m_nExpertID1;            // 기술제한ID1
	int         m_nExpertLim1;           // 기술제한값
	int         m_nExpertID2;            // 기술제한ID2
	int         m_nExpertLim2;           // 기술제한2값
	int         m_nMoney;                // 돈구분
	int         m_nStdPrice;             // 기준가격
	BOOL        m_bAbr;                  // 마모여부
	int         m_nDurUnit;              // 내구도
	float       m_fEquipSpeed;           // 무게비중
	BOOL        m_bRepair;               // 수리가능여부
	int         m_nRepPrice;             // 수리비용
	int         m_nEffState;             // 착용상태
	int         m_nProperty;             // 속성
	float       m_fFireTol;              // 불내성
	float       m_fWaterTol;             // 물내성
	float       m_fSoilTol;              // 흙내성
	float       m_fWindTol;              // 바람내성
	float       m_fGADst;                // 일공공격거리
	int         m_nGASpd;                // 일공무기속도
	int         m_nGAMinSelProb;         // 일공최소선택확률
	float       m_fGAMinAF;              // 일공최소공격력
	int         m_nGAMaxSelProb;         // 일공최대선택확률
	float       m_fGAMaxAF;              // 일공최대공격력
	int         m_nMADst;                // 마공공격거리
	int         m_nMASpd;                // 마공무기속도
	int         m_nMAMinSelProb;         // 마공최소선택확률
	float       m_fMAMinAF;              // 마공최소공격력
	int         m_nMAMaxSelProb;         // 마공최대선택확률
	float       m_fMAMaxAF;              // 마공최대공격력
	int         m_nEff1Code;             // 효과1명ID
	float       m_fEff1Unit;             // 효과1
	int         m_nEff2Code;             // 효과2명ID
	float       m_fEff2Unit;             // 효과2
	int         m_nEff3Code;             // 효과3명ID
	float       m_fEff3Unit;             // 효과3
	int         m_nEff4Code;             // 효과4명ID
	float       m_fEff4Unit;             // 효과4
	int         m_nDuration;             // 지속시간
	BOOL        m_bSell;                 // 팔기여부
	BOOL        m_bExchange;             // 주는(교환)여부
	BOOL        m_bGround;               // 그라운드여부
	int         m_nUpgrade;              // 업글소켓수
	_str_code   m_strTooltipIndex;       // 아이템설명
	int         m_nAttEffType;           // 공격이펙트재질

	_WeaponItem_fld()
	{
		ZeroMemory( this, sizeof( *this ) );
	}
};

#endif
