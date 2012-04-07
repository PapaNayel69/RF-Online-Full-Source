#ifndef _AnimusItem_h_
#define _AnimusItem_h_

#include "BaseFld.h"

struct _AnimusItem_fld : public _base_fld
{ 
	_str_code   m_strModel;              // 모델링코드
	_str_code   m_strCivil;              // 문명
	_str_code   m_strName;               // 이름
	int         m_nAnimusClass;          // 특화구분
	int         m_nKindClt;              // 종류(클)
	int         m_nFixPart;              // 장착부위
	int         m_nMoney;                // 돈구분
	int         m_nStdPrice;             // 기준가격
	int         m_nIconIDX;              // 아이콘종류(클)
	_str_code   m_strTooltipIndex;       // 아이템설명
	BOOL        m_bSell;                 // 팔기여부
	BOOL        m_bExchange;             // 주는(교환)여부
	BOOL        m_bGround;               // 그라운드여부

	_AnimusItem_fld()
	{
		ZeroMemory( this, sizeof( *this ) );
	}
};

#endif
