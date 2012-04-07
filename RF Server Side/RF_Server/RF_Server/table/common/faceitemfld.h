#ifndef _FaceItem_h_
#define _FaceItem_h_

#include "BaseFld.h"

struct _FaceItem_fld : public _base_fld
{ 
	BOOL        m_bExist;                // 존재여부
	_str_code   m_strModle;              // 모델링코드(클)
	_str_code   m_strName;               // 아이템이름
	int         m_nKindClt;              // 종류(클)
	int         m_nFixPart;              // 장착부위
	_str_code   m_strCivil;              // 문명
	int         m_nDefEffType;           // 방어이펙트재질

	_FaceItem_fld()
	{
		ZeroMemory( this, sizeof( *this ) );
	}
};

#endif
