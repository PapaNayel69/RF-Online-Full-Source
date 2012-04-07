#ifndef _UnitPart_h_
#define _UnitPart_h_

#include "BaseFld.h"

struct _UnitPart_fld : public _base_fld
{ 
	_str_code   m_strModle;              // 모델링코드(클)
	int         m_nIconIDX;              // 아이콘종류(클)
	_str_code   m_strName;               // 파츠이름
	int         m_nFixPart;              // 장착부위
	_str_code   m_strDefFrame;           // 장착가능프레임
	int         m_nWPType;               // 무기타입
	int         m_nEffectGroup;          // 공격형태
	int         m_nNeedBt;               // 소모발사물
	BOOL        m_bAbr;                  // 마모여부
	int         m_nDurUnit;              // 내구도
	int         m_nLevelLim;             // 레벨제한
	int         m_nExpertID1;            // 기술제한ID1
	int         m_nExpertLim1;           // 기술제한1값
	int         m_nExpertID2;            // 기술제한ID2
	int         m_nExpertLim2;           // 기술제한2값
	float       m_fAttackRange;          // 공격거리
	int         m_nAttackDel;            // 공격딜레이
	float       m_fMoveSpdRev;           // 이동속도보정
	int         m_nGAMinAF;              // 일공최소공격력
	int         m_nGAMaxAF;              // 일공최대공격력
	int         m_nAttMastery;           // 공격기술숙련도
	int         m_nGAMinSelProb;         // 일공최소공격력선택확률
	int         m_nGAMaxSelProb;         // 일공최대공격력선택확률
	int         m_nDefFc;                // 방어력
	int         m_nDefMastery;           // 방어기술숙련도
	int         m_nProperty;             // 속성
	int         m_nFireTol;              // 불내성
	int         m_nWaterTol;             // 물내성
	int         m_nSoilTol;              // 흙내성
	int         m_nWindTol;              // 바람내성
	int         m_nMoney;                // 돈구분
	int         m_nStdPrice;             // 기준가격
	int         m_nRepPrice;             // 수리비
	int         m_nBstCha;               // 부스터차지량
	float       m_fBstSpd;               // 부스터이동속도보정
	int         m_nBackSlt;              // 백팩여유슬롯
	int         m_nEff1Code;             // 효과1명ID
	float       m_fEff1Unit;             // 효과1
	int         m_nEff2Code;             // 효과2명ID
	float       m_fEff2Unit;             // 효과2
	int         m_nEff3Code;             // 효과3명ID
	float       m_fEff3Unit;             // 효과3
	int         m_nEff4Code;             // 효과4명ID
	float       m_fEff4Unit;             // 효과4
	_str_code   m_strTooltipIndex;       // 아이템설명
	int         m_nAttEffType;           // 공격이펙트재질
	int         m_nDefEffType;           // 방어이펙트재질

	_UnitPart_fld()
	{
		ZeroMemory( this, sizeof( *this ) );
	}
};

#endif
