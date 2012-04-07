#ifndef _AnimusFld_h_
#define _AnimusFld_h_

#include "BaseFld.h"

struct _animus_fld : public _base_fld
{ 
	int         m_nLevel;                // 레벨
	int         m_nForLvUpExp;           // 경험치
	float       m_fPenalty;              // 패널티
	int         m_nAttFcStd;             // 공격력기준
	int         m_nMinAFSelProb;         // 최소공격력선택확률
	int         m_nMaxAFSelProb;         // 최대공격력선택확률
	int         m_nAttSklUnit;           // 공격숙련도
	int         m_nDefSklUnit;           // 방어숙련도
	float       m_fWeakPart;             // 약한부위
	int         m_nStdDefFc;             // 기준방어력
	float       m_fDefGap;               // 상대방어력간격조정값
	float       m_fDefFacing;            // 상대방어력값
	int         m_nFireTol;              // 불내성
	int         m_nWaterTol;             // 물내성
	int         m_nSoilTol;              // 흙내성
	int         m_nWindTol;              // 바람내성
	int         m_nForceLevel;           // 포스Level
	int         m_nForceMastery;         // 포스숙련도
	int         m_nForceAttStd;          // 포스공격력기준
	_str_code   m_strAttTechID1;         // 포스공격기술1_ID
	int         m_nAttTech1UseProb;      // 포스공격기술1_선택확률
	int         m_nAttTechID1MotionTime; // 포스공격기술1모션타임
	_str_code   m_strPSecTechID;         // 포스플레이어보조기술ID
	int         m_nPSecTechIDMotionTime; // 포스PS기술모션타임
	_str_code   m_strMSecTechID;         // 포스Mob보조기술ID
	int         m_nMSecTechIDMotionTime; // 포스MS기술모션타임
	int         m_nMaxHP;                // 최대HP
	int         m_nHPRecDelay;           // HP회복Delay
	int         m_nHPRecUnit;            // HP회복량
	int         m_nMaxFP;                // 최대FP
	int         m_nFPRecDelay;           // FP회복Delay
	int         m_nFPRecUnit;            // FP회복량
	int         m_nAttSpd;               // 공격속도
	int         m_nAttMoTime1;           // 공격모션1시간
	int         m_nAttMoTime2;           // 공격모션2시간
	int         m_nCrtMoTime;            // 크리티컬모션시간
	int         m_nViewExt;              // 시야범위
	int         m_nRefExt;               // 검색범위
	int         m_nAttExt;               // 공격범위
	int         m_nMovSpd;               // 이동속도
	int         m_nScaleRate;            // 애니머스Scale
	int         m_nWidth;                // 애니머스크기(바운딩박스지름)
	int         m_nAttEffType;           // 공격이펙트재질
	int         m_nDefEffType;           // 방어이펙트재질
	_animus_fld()
	{
		ZeroMemory( this, sizeof( *this ) );
	}
};

#endif
