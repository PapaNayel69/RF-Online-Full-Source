#ifndef _OreItem_h_
#define _OreItem_h_

#include "BaseFld.h"

struct _OreItem_fld : public _base_fld
{ 
	BOOL        m_bExist;                // 존재여부
	_str_code   m_strModle;              // 모델링코드(클)
	int         m_nIconIDX;              // 아이콘종류(클)
	_str_code   m_strName;               // 아이템이름
	int         m_nOre_List;             // 원석
	int         m_nOre_Level;            // 원석등급
	int         m_nKindClt;              // 종류(클)
	BOOL        m_bSell;                 // 팔기여부
	BOOL        m_bExchange;             // 주는(교환)여부
	BOOL        m_bGround;               // 그라운드여부
	int         m_nmin_C_random;         // min랜덤횟수
	int         m_nmax_C_random;         // max랜덤횟수
	int         m_nStdPrice;             // 가공비용
	_str_code   m_strTooltipIndex;       // 아이템설명

	_OreItem_fld()
	{
		ZeroMemory( this, sizeof( *this ) );
	}
};

#endif
