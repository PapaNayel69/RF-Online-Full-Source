#ifndef _BulletItem_h_
#define _BulletItem_h_

#include "BaseFld.h"

struct _BulletItem_fld : public _base_fld
{ 
	BOOL        m_bExist;                // 존재여부
	_str_code   m_strModle;              // 모델링코드(클)
	int         m_nIconIDX;              // 아이콘종류(클)
	_str_code   m_strName;               // 아이템이름
	int         m_nKindClt;              // 종류(클)
	int         m_nFixPart;              // 장착부위
	_str_code   m_strCivil;              // 문명
	int         m_nLevelLim;             // 레벨제한
	int         m_nMoney;                // 돈구분
	int         m_nStdPrice;             // 기준가격
	int         m_nDurUnit;              // 내구도
	int         m_nWeight;               // 무게
	int         m_nEffectGroup;          // 효과분류
	int         m_nProperty;             // 속성
	float       m_fFireTol;              // 불내성
	float       m_fWaterTol;             // 물내성
	float       m_fSoilTol;              // 흙내성
	float       m_fWindTol;              // 바람내성
	float       m_fGAAF;                 // *공격력
	int         m_nEff1Code;             // 효과1명ID
	float       m_fEff1Unit;             // 효과1
	int         m_nEff2Code;             // 효과2명ID
	float       m_fEff2Unit;             // 효과2
	int         m_nEff3Code;             // 효과3명ID
	float       m_fEff3Unit;             // 효과3
	int         m_nEff4Code;             // 효과4명ID
	float       m_fEff4Unit;             // 효과4
	BOOL        m_bSell;                 // 팔기여부
	BOOL        m_bExchange;             // 주는(교환)여부
	BOOL        m_bGround;               // 그라운드여부
	BOOL        m_bUpgrade;              // 업글가능여부
	_str_code   m_strTooltipIndex;       // 아이템설명

	_BulletItem_fld()
	{
		ZeroMemory( this, sizeof( *this ) );
	}
};

#endif
