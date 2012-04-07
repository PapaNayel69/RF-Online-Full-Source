#ifndef _skill_h_
#define _skill_h_

#include "BaseFld.h"

#define max_cont_param 5

#define cont_param_rate 0
#define cont_param_plus 1
#define cont_param_state 2

struct _cont_param_list
{
	int         m_nContParamCode;		 // 효과타입1
	int         m_nContParamIndex;       // 효과인덱스1
	float       m_fContValue[max_skill_level];               // 1_1Lv효과
};

struct _skill_fld : public _base_fld
{ 
	int         m_nClass;                // 계열
	int         m_nIconIDX;              // 아이콘종류(클)
	int         m_nMastIndex;            // Mastery_Index
	_str_code   m_strMastKorName;        // Mastery_한글이름
	_str_code   m_strMastEngName;        // Mastery_영문이름
	_str_code   m_strKorName;            // 스킬_한글이름
	_str_code   m_strEngName;            // 스킬_영문이름
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

	BOOL        m_bAttackable;             // 공격가능유무
	int         m_nAttType[max_skill_level];        // 공격타입분류(1~7LV)
	int         m_nAttConstant[max_skill_level];          // 1~7LV상수
	float       m_fAttFormulaConstant;      // 공식상수
	int         m_nAttNeedBt;               // 소모발사물수
	int         m_nBonusDistance;			// 보너스사정거리

	int			m_nTempEffectType;				// 일시보조타입-1은 없다..
	int			m_nTempParamCode;				//효과타입
	float       m_fTempValue[max_skill_level];     // 일시보조1~7Lv효과

	int         m_nContEffectType;			// 지속타입 (0:이득, 1:피해, -1: 없엉)
	int         m_nContAreaType;         // 일시보조효과범위분류
	_cont_param_list	m_ContParamList[max_cont_param];

	int         m_nContEffectSec[max_skill_level];          // 1~7Lv지속효과시간(초)

	int         m_nEtc;                  // 기타
	float       m_f1_2speed;             // 1_2타격_이동속도
	float       m_f1_2distance;          // 1_2타격_이동거리
	float       m_f2_3speed;             // 2_3타격_이동속도
	float       m_f2_3distance;          // 2_3타격_이동거리
	int         m_nEffectClass;          // 이펙트구조(발동분류)

	_skill_fld()
	{
		ZeroMemory( this, sizeof( *this ) );
	}
};

#endif
