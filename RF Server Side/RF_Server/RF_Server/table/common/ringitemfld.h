#ifndef _RingItem_h_
#define _RingItem_h_

#include "BaseFld.h"

struct _RingItem_fld : public _base_fld
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
	BOOL        m_bAbr;                  // 마모여부
	int         m_nEffState;             // 착용상태
	float       m_fFireTol;              // 불내성
	float       m_fWaterTol;             // 물내성
	float       m_fSoilTol;              // 흙내성
	float       m_fWindTol;              // 바람내성
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
	_str_code   m_strTooltipIndex;       // 아이템설명

	_RingItem_fld()
	{
		ZeroMemory( this, sizeof( *this ) );
	}
};

#endif
