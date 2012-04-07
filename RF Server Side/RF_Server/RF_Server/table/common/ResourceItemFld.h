#ifndef _ResourceItem_h_
#define _ResourceItem_h_

#include "BaseFld.h"

struct _ResourceItem_fld : public _base_fld
{ 
	BOOL        m_bExist;                // 존재여부
	_str_code   m_strModle;              // 모델링코드(클)
	int         m_nIconIDX;              // 아이콘종류(클)
	_str_code   m_strName;               // 아이템이름
	int         m_nKindClt;              // 종류(클)
	int         m_nFixPart;              // 장착부위
	_str_code   m_strCivil;              // 문명
	int         m_nMoney;                // 돈구분
	int         m_nAncStdPrice;          // 벨라토기준가격
	int         m_nExStdPrice;           // 코라기준가격
	int         m_nMecaStdPrice;         // 아크레시아기준가격
	int         m_nEffCode;              // 효과명ID
	float       m_fEffUnit;              // 효과값
	float       m_fEffUnitMax;           // 효과최대치
	BOOL        m_bSell;                 // 팔기여부
	BOOL        m_bExchange;             // 주는(교환)여부
	BOOL        m_bGround;               // 그라운드여부
	_str_code   m_strTooltipIndex;       // 아이템설명

	_ResourceItem_fld()
	{
		ZeroMemory( this, sizeof( *this ) );
	}
};

#endif
