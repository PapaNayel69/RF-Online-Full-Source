#ifndef _Class_h_
#define _Class_h_

#include "BaseFld.h"
#include "CharacterDB.h"

#define max_branch_class	8	
#define max_link_claskill	3
#define max_class_bns_item	7

struct _class_fld : public _base_fld
{
	int         m_nRaceCode;             // 종족
	int         m_nIconIDX;              // 아이콘종류(클)
	int         m_nGrade;                // 단계
	int         m_nUpGradeLv;            // 전직레벨
	_str_code   m_strCh_Class[max_branch_class];          // 자식클래스
	_str_code   m_strTemp;               // 구분
	_str_code   m_strKorName;            // 한글이름
	_str_code   m_strEngName;            // 영문이름
	int         m_nConLim;               // 필요기여도
	_str_code   m_strLinkClassSkill[max_link_claskill];    // 클래스스킬

	int         m_nBnsMMastery[WP_CLASS_NUM];    // 보너스_무기숙련도
	int         m_nBnsSMastery;          // 보너스_스페셜숙련도
	int         m_nBnsDefMastery;        // 보너스_방어숙련도
	int         m_nBnsPryMastery;        // 보너스_방패숙련도
	int         m_nBnsMakeMastery[MI_CLASS_NUM];       // 보너스_제작숙련도
	int         m_nBnsForceMastery[force_mastery_num]; // 보너스_포스숙련도

	_str_code   m_strDefaultItem[max_class_bns_item];       // DefaultItem
	_str_code   m_strPtSocket[potion_belt_num];           // 포션벨트
	_str_code   m_strSFSocket[sf_linker_num];           // 단축소켓

	_class_fld()
	{
		ZeroMemory( this, sizeof( *this ) );
	}
};

#endif
