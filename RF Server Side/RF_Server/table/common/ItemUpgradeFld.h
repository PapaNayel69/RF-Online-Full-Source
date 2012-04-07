#ifndef _ItemUpgrade_h_
#define _ItemUpgrade_h_

#include "BaseFld.h"

struct _ItemUpgrade_fld : public _base_fld
{ 
	_str_code   m_strName;               // 아이템이름
	int         m_nJewelType;            // 종류구분
	float       m_fJewelFieldValue;      // 보석상수값
	int         m_nUpperUp;              // 상의
	int         m_nLowerUp;              // 하의
	int         m_nGauntletUp;           // 장갑
	int         m_nShoeUp;               // 신발
	int         m_nHelmetUp;             // 투구
	int         m_nShieldUp;             // 방패
	int         m_nMeleeUp;              // 근접무기
	int         m_nMissileUp;            // 원거리무기
	int         m_nUnitUp;               // 유닛
	int         m_nDataEffect;           // 데이터효과
	int         m_nEffectUnit;           // 효과단위
	float       m_fUp1;                  // 1up효과
	float       m_fUp2;                  // 2up효과
	float       m_fUp3;                  // 3up효과
	float       m_fUp4;                  // 4up효과
	float       m_fUp5;                  // 5up효과
	float       m_fUp6;                  // 6up효과
	float       m_fUp7;                  // 7up효과

	_ItemUpgrade_fld()
	{
		ZeroMemory( this, sizeof( *this ) );
	}
};


#endif
