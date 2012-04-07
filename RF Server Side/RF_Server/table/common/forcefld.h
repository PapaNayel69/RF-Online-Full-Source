#ifndef _Force_h_
#define _Force_h_

#include "BaseFld.h"
#include "skillFld.h" //_cont_param_list 땜시..

struct _force_fld : public _base_fld
{ 
	int         m_nClass;                // 계열
	int         m_nIconIDX;              // 아이콘종류(클)
	int         m_nMastIndex;            // Mastery_Index
	_str_code   m_strMastKorName;        // Mastery_한글이름
	_str_code   m_strMastEngName;        // Mastery_영문이름
	_str_code   m_strKorName;            // 포스_한글이름
	_str_code   m_strEngName;            // 포스_영문이름
	int         m_nLv;                   // 단계
	_str_code   m_strUsableRace;         // 사용가능종족
	_str_code   m_strActableDst;         // 시전대상
	int         m_nNeedMastIndex;        // 필요MasteryIndex
	_str_code   m_strFixWeapon;          // 착용무기
	BOOL        m_bFixshield;            // 방패착용유무
	int         m_nSpecialType;          // 특수효과타입
	int         m_nNeedSpecialType;      // 필요특수효과
	int         m_nNeedFP;               // 소모FP
	float       m_fActDelay;             // 시전딜레이(초)

	BOOL        m_bAttackable;           // 공격가능유무
	int         m_nProperty;             // 속성
	int         m_nEffectGroup;          // 효과분류(LV별_구분없음)
	float       m_fAttFormulaConstant;   // 공식상수
	int         m_nActDistance;          // 시전거리

	int			m_nTempEffectType;				// 일시보조타입-1은 없다..
	int			m_nTempParamCode;				//효과타입
	float       m_fTempValue[max_skill_level];     // 일시보조1~7Lv효과

	int         m_nContEffectType;			// 지속타입 (0:이득, 1:피해, -1: 없엉)
	int         m_nContAreaType;         // 지속효과범위분류
	_cont_param_list	m_ContParamList[max_cont_param];

	int         m_nContEffectSec[max_skill_level];          // 1~7Lv지속효과시간(초)

	int         m_nEffectClass;          // 이펙트구조(발동분류)

	_force_fld()
	{
		ZeroMemory( this, sizeof( *this ) );
	}
};

#endif
