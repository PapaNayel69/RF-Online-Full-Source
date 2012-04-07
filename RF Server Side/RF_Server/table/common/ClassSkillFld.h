#ifndef _ClassSkill_h_
#define _ClassSkill_h_

#include "BaseFld.h"
#include "skillFld.h" //_cont_param_list 땜시..

struct _claskill_fld : public _base_fld
{ 
	_str_code   m_strKorName;            // 스킬_한글이름
	_str_code   m_strEngName;            // 스킬_영문이름
	int         m_nGrade;                // 단계
	_str_code   m_strActableDst;         // 시전대상
	_str_code   m_strFixWeapon;          // 착용무기
	BOOL        m_bFixshield;            // 방패착용유무
	int         m_nSpecialType;          // 특수효과타입
	int         m_nNeedSpecialType;      // 필요특수효과
	int         m_nNeedFP;               // 소모FP
	float       m_fActDelay;             // 시전딜레이(초)

	BOOL        m_bAttackable;           // 공격가능유무
	int         m_nAttType;              // 공격효과분류
	int         m_nAttConstant;          // 공격상수
	float       m_fAttFormulaConstant;   // 공식상수
	int         m_nAttNeedBt;            // 소모발사물수
	int         m_nBonusDistance;        // 보너스사정거리

	int         m_nTempEffectType;       // 일시보조효과인덱스
	int         m_nTempAreaType;         // 일시보조효과범위분류
	int         m_nTempParamCode;        // 일시보조효과타입
	float       m_fTempValue;            // 일시보조효과

	int         m_nContEffectType;       // 지속타입
	int         m_nContAreaType;         // 지속보조효과범위분류
	_cont_param_list	m_ContParamList[max_cont_param];

	int         m_nContEffectTime;       // 지속효과시간
	int         m_nEtc;                  // 기타

	_claskill_fld()
	{
		ZeroMemory( this, sizeof( *this ) );
	}
};

#endif
