#ifndef _PlayerCharacter_h_
#define _PlayerCharacter_h_

#include "BaseFld.h"

struct _player_fld : public _base_fld
{ 
	_str_code   m_strName;               // 이름
	int         m_nRaceCode;             // 종족
	int         m_nSexCode;              // 성별
	float       m_fMoveWalkRate;         // 이동속도(걷기)
	float       m_fMoveRunRate;          // 이동속도(뛰기)
	float       m_fHeight;               // 몸크기(높이)
	float       m_fWidth;                // 몸크기(폭)
	float       m_fAttackRange;          // 공격거리
	float       m_fDefGap;               // 간격조정값
	float       m_fDefFacing;            // 상대방어력값

	_player_fld()
	{
		ZeroMemory( this, sizeof( *this ) );
	}
};

#endif
