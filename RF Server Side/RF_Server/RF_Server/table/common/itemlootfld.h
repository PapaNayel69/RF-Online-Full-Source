#ifndef _ItemLooting_h_
#define _ItemLooting_h_

#include "BaseFld.h"

#define max_loot_kind	30

struct _ItemLooting_fld : public _base_fld
{ 
	_str_code   m_strLootMobIDX1;        // MobIDX1
	_str_code   m_strLootMobIDX2;        // MobIDX2
	_str_code   m_strLootMobIDX3;        // MobIDX3
	int         m_nLootMobLevel;         // 레벨
	int         m_nLootRate;             // loot확률
	int         m_nLootTime;             // 시간(초)
	int         m_nLootListCount;        // 종류갯수
	_str_code   m_strLootCodeKey_1;      // CodeKey_1
	_str_code   m_strLootCodeKey_2;      // CodeKey_2
	_str_code   m_strLootCodeKey_3;      // CodeKey_3
	_str_code   m_strLootCodeKey_4;      // CodeKey_4
	_str_code   m_strLootCodeKey_5;      // CodeKey_5
	_str_code   m_strLootCodeKey_6;      // CodeKey_6
	_str_code   m_strLootCodeKey_7;      // CodeKey_7
	_str_code   m_strLootCodeKey_8;      // CodeKey_8
	_str_code   m_strLootCodeKey_9;      // CodeKey_9
	_str_code   m_strLootCodeKey_10;     // CodeKey_10
	_str_code   m_strLootCodeKey_11;     // CodeKey_11
	_str_code   m_strLootCodeKey_12;     // CodeKey_12
	_str_code   m_strLootCodeKey_13;     // CodeKey_13
	_str_code   m_strLootCodeKey_14;     // CodeKey_14
	_str_code   m_strLootCodeKey_15;     // CodeKey_15
	_str_code   m_strLootCodeKey_16;     // CodeKey_16
	_str_code   m_strLootCodeKey_17;     // CodeKey_17
	_str_code   m_strLootCodeKey_18;     // CodeKey_18
	_str_code   m_strLootCodeKey_19;     // CodeKey_19
	_str_code   m_strLootCodeKey_20;     // CodeKey_20
	_str_code   m_strLootCodeKey_21;     // CodeKey_21
	_str_code   m_strLootCodeKey_22;     // CodeKey_22
	_str_code   m_strLootCodeKey_23;     // CodeKey_23
	_str_code   m_strLootCodeKey_24;     // CodeKey_24
	_str_code   m_strLootCodeKey_25;     // CodeKey_25
	_str_code   m_strLootCodeKey_26;     // CodeKey_26
	_str_code   m_strLootCodeKey_27;     // CodeKey_27
	_str_code   m_strLootCodeKey_28;     // CodeKey_28
	_str_code   m_strLootCodeKey_29;     // CodeKey_29
	_str_code   m_strLootCodeKey_30;     // CodeKey_30

	_ItemLooting_fld()
	{
		ZeroMemory( this, sizeof( *this ) );
	}
};

#endif
