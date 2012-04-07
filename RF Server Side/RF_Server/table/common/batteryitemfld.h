#ifndef _BatteryItem_h_
#define _BatteryItem_h_

#include "BaseFld.h"

struct _BatteryItem_fld : public _base_fld
{ 
	BOOL        m_bExist;                // 존재여부
	_str_code   m_strModle;              // 모델링코드(클)
	int         m_nIconIDX;              // 아이콘종류(클)
	_str_code   m_strName;               // 아이템이름
	int         m_nKindClt;              // 종류(클)
	int         m_nFixPart;              // 장착부위
	_str_code   m_strCivil;              // 문명
	int         m_nMoney;                // 돈구분
	int         m_nStdPrice;             // 기준가격
	int         m_nDurUnit;              // 내구도
	BOOL        m_bSell;                 // 팔기여부
	BOOL        m_bExchange;             // 주는(교환)여부
	BOOL        m_bGround;               // 그라운드여부
	_str_code   m_strTooltipIndex;       // 아이템설명

	_BatteryItem_fld()
	{
		ZeroMemory( this, sizeof( *this ) );
	}
};

#endif
