#ifndef _UnitBullet_h_
#define _UnitBullet_h_

#include "BaseFld.h"

struct _UnitBullet_fld : public _base_fld
{ 
	_str_code   m_strModle;              // 모델링코드(클)
	int         m_nIconIDX;              // 아이콘종류(클)
	_str_code   m_strName;               // 이름
	int         m_nWPType;               // 무기타입
	BOOL        m_bAbr;                  // 마모여부
	int         m_nDurUnit;              // 내구도
	int         m_nMoney;                // 돈구분
	int         m_nStdPrice;             // 기준가격
	float       m_fGAAF;                 // *공격력
	int         m_nEff1Code;             // 효과1명ID
	float       m_fEff1Unit;             // 효과1
	int         m_nEff2Code;             // 효과2명ID
	float       m_fEff2Unit;             // 효과2
	int         m_nEff3Code;             // 효과3명ID
	float       m_fEff3Unit;             // 효과3
	int         m_nEff4Code;             // 효과4명ID
	float       m_fEff4Unit;             // 효과4
	_str_code   m_strTooltipIndex;       // 아이템설명

	_UnitBullet_fld()
	{
		ZeroMemory( this, sizeof( *this ) );
	}
};

#endif
