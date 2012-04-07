#ifndef _UnitFrame_h_
#define _UnitFrame_h_

#include "BaseFld.h"

struct _UnitFrame_fld : public _base_fld
{ 
	int         m_nFRAType;              // 프레임타입
	int         m_nUnit_HP;              // 내구도(HP)
	float       m_fDefGap;               // 간격조정값
	float       m_fDefFacing;            // 상대방어력값
	float       m_fMoveRate_Seed;        // 이동속도기준
	float       m_fHeight;               // 몸크기(높이)
	float       m_fWidth;                // 몸크기(폭)
	int         m_nMoney;                // 돈구분
	int         m_nStdPrice;             // 기준가격
	int         m_nRepPrice;             // 수리비
	_str_code   m_strDFHead;             // 기본머리
	_str_code   m_strDFUpper;            // 기본상체
	_str_code   m_strDFLower;            // 기본하체
	_str_code   m_strDFArms;             // 기본팔
	_str_code   m_strDFShoulder;         // 기본어깨
	_str_code   m_strDFBack;             // 기본백팩
	_str_code   m_strDFBtA;              // 기본손탄창
	_str_code   m_strDFBtS;              // 기본어깨탄창

	_UnitFrame_fld()
	{
		ZeroMemory( this, sizeof( *this ) );
	}
};

#endif
